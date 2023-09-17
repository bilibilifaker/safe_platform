#pragma once
#include <iostream>
#include <string.h>
#include <thread>
#include <map>
#include <unordered_map>
#include "MyJson.h"
#include "Myhash.h"
#include <iostream>
#include <json/json.h>
#include "threadpool.h"
#include <fstream>
#include "Request.h"
#include "Response.h"
#include "Codec.h"
#include "SecKeyShm.h"
#include "my_sql.h"
#include "RequestFactory.h"
#include "RespondFactory.h"
#include "../proto/respondmsg.pb.h"
#include "../proto/requestmsg.pb.h"
#include "TcpServer.h"
#include "MyAES.h"
//#include "MyRSA.h"
using namespace std;
using namespace Json;


class Server{
    private:
        MyAES* my_aes;
        ThreadPool* my_pool;
        MySQL my_sql;
        MyJson my_json;
        int target_port;
        string server_id;
        TcpServer* my_server;
        int my_memkey;
        SecKeyShm* my_share_mem;
        static unordered_map<pthread_t,TcpSocket*> all_sock;
        static Server* my_this;

        static string get_randkey(int len){
            srand(time(nullptr));
            // len must equal 16  24  32
            int flag = 0;
            string res = "";
            char all_char[14] = "!@#$%^&*()|}{";
            for (int i = 0; i<len ;i++) {
                flag = rand()%74;
                if(flag < 26){
                    res.append(1, 'a'+flag);
                }
                else if(flag < 52){
                    res.append(1, 'A'+flag-26);
                }
                else if(flag < 62){
                    res.append(1, '0'+flag-52);
                }
                else{
                    res.append(1, all_char[flag-62]);
                }
            }
            return res;
        }

    public:
        Server(string json_filename = "server.json"):
        my_json(json_filename){
            Server::my_this = this;
            my_pool = new ThreadPool(2,8);
            my_this->my_share_mem = new SecKeyShm(my_memkey);
            my_this->my_share_mem->shmInit();
            Value my_value = my_json.read_json();
            my_memkey = my_value["ShareMemKey"].asInt();
            target_port = my_value["Port"].asInt();
            server_id = my_value["ServerID"].asString();
        }

        void start_server(){
            my_server = new TcpServer(target_port);
            cout << "start server now..." << endl;
            while(1){
                TcpSocket* sock = my_server->acceptConn();
                cout << "get acceptconn" << endl;
                if(sock == nullptr){
                    cout << "error occured" << endl;
                    continue;
                }

                Server* server = (Server*)this;

                string msg = sock->recvMsg();

                CodecFactory* factory = new RequestFactory(msg);
                Codec* codec = factory->createCodec();
                RequestMsg* request = (RequestMsg*)codec->decodeMsg();

                Task task;
                pair<RequestMsg*,TcpSocket*> arg = {request, sock};
                task.arg = (void*)&arg;
                
                switch(request->cmdtype()){
                    case 1:{
                        task.function = Server::secret_key_negotiate;
                        server->my_pool->addPoolTask(task);
                        break;
                    }
                    case 2:{
                        task.function = Server::secret_key_vertify;
                        server->my_pool->addPoolTask(task);
                        break;
                    }
                    case 3:{
                        task.function = Server::secret_key_sign_out;
                        server->my_pool->addPoolTask(task);
                        break;
                    }
                }
                delete factory;
                delete codec;
            }
        }

        static void* secret_key_negotiate(void* arg){
            pair<RequestMsg*,TcpSocket*>* msg = (pair<RequestMsg*,TcpSocket*>*)arg;
            ofstream ofs("server_public.pem");
            ofs << msg->first->data() << endl;
            ofs.close();
            ResponseInfo info;
            info.status = true;

            MyRSA my_rsa("server");
            Myhash my_hash(M_SHA1);
            my_hash.add_data(msg->first->data());

            string hash_final = my_hash.get_final();
            if (!my_rsa.data_vertify(hash_final, msg->first->sign())) {
                cout << "vertify sign false." << endl;
                info.status = false;
            }
            else {
                cout << "vertify sign success!" << endl;
            }
            string key = get_randkey(16);
            string secret_key = my_rsa.pubkey_encrypt(key);
            
            info.clientId = msg->first->clientid();
            info.data = secret_key;
            int infoseckeyid = 12;
            // 还没想好这个怎么处理
            info.seckeyid = infoseckeyid;
            info.serverId = my_this->server_id;

            //客户端发的非对称加密的公钥没用了  保存自己生成的随机字符串就行 插入mysql和共享内存
            my_this->my_sql.insert(msg->first->clientid(), msg->first->serverid(), key);
            cout << "key = " << key <<endl;
            NodeSHMInfo temp;
            memcpy(temp.clientID, msg->first->clientid().data(), msg->first->clientid().size());
            memcpy(temp.serverID, msg->first->serverid().data(), msg->first->serverid().size());
            memcpy(temp.seckey, key.data(), key.size());
            temp.status = info.status;
            temp.seckeyID = infoseckeyid;
            my_this->my_share_mem->shmWrite(&temp);

            CodecFactory* factory = new RespondFactory(&info);
            Codec* codec = factory->createCodec();
            string encode_msg(codec->encodeMsg());
            msg->second->sendMsg(encode_msg);
            delete msg->second;
            return nullptr;
        }

        static void* secret_key_vertify(void* arg){
            pair<RequestMsg*,TcpSocket*>* msg = (pair<RequestMsg*,TcpSocket*>*)arg;

            ResponseInfo info;
            info.status = true;

            string aes_key = my_this->my_sql.get(msg->first->clientid(), msg->first->serverid());
            cout << "aes key = " << aes_key <<endl;
            MyRSA my_rsa("server");
            Myhash my_hash(M_SHA1);
            my_hash.add_data(msg->first->data());

            string hash_final = my_hash.get_final();
            if (!my_rsa.data_vertify(hash_final, msg->first->sign())) {
                cout << "vertify sign false." << endl;
                info.status = false;
            }

            my_this->my_aes = new MyAES(aes_key);
            cout << "decrypt data: " <<my_this->my_aes->decrypt_data(msg->first->data()) <<endl;
            if(my_this->my_aes->decrypt_data(msg->first->data()).substr(0,18) != "secret key vertify"){
                //  这里0,18是解码最后有点乱码问题  有空了解决
                cout << "vertify sign false." << endl;
                info.status = false;
            }
            else {
                cout << "vertify sign success!" << endl;
            }

            info.clientId = msg->first->clientid();
            info.data = "1233121234567";
            info.seckeyid = 12;
            info.serverId = my_this->server_id;

            CodecFactory* factory = new RespondFactory(&info);
            Codec* codec = factory->createCodec();
            string encode_msg(codec->encodeMsg());
            msg->second->sendMsg(encode_msg);
            delete msg->second;
            delete my_this->my_aes;
            return nullptr;
        }

        static void* secret_key_sign_out(void* arg){
            pair<RequestMsg*,TcpSocket*>* msg = (pair<RequestMsg*,TcpSocket*>*)arg;

            ResponseInfo info;
            info.status = true;

            MyRSA my_rsa("server");
            Myhash my_hash(M_SHA1);
            my_hash.add_data(msg->first->data());

            string hash_final = my_hash.get_final();
            if (!my_rsa.data_vertify(hash_final, msg->first->sign())) {
                cout << "vertify sign false." << endl;
                info.status = false;
            }
            string aes_key = my_this->my_sql.get(msg->first->clientid(), msg->first->serverid());
            my_this->my_aes = new MyAES(aes_key);
            my_this->my_aes = new MyAES(aes_key);
            cout << "decrypt data: " <<my_this->my_aes->decrypt_data(msg->first->data()) <<endl;
            if(my_this->my_aes->decrypt_data(msg->first->data()).substr(0,16) != "secret key delet"){
                //  这里0,16是最后有点乱码问题  有空了解决
                cout << "vertify sign false." << endl;
                info.status = false;
            }
            else {
                cout << "vertify sign success!" << endl;
                int erase_res = my_this->my_sql.erase(msg->first->clientid(), msg->first->serverid());
            }

            info.clientId = msg->first->clientid();
            info.data = "1233121234567";
            info.seckeyid = 12;
            info.serverId = my_this->server_id;

            CodecFactory* factory = new RespondFactory(&info);
            Codec* codec = factory->createCodec();
            string encode_msg(codec->encodeMsg());
            msg->second->sendMsg(encode_msg);
            delete msg->second;
            delete my_this->my_aes;
            return nullptr;
        }

        ~Server(){
            if(my_server){
                delete my_server;
            }
        };
};

unordered_map<pthread_t,TcpSocket*> Server::all_sock;
Server* Server::my_this;