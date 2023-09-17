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
#include <fstream>
#include "Request.h"
#include "Response.h"
#include <fstream>
#include "Codec.h"
#include "my_sql.h"
#include "RequestFactory.h"
#include "RespondFactory.h"
#include "../proto/respondmsg.pb.h"
#include "../proto/requestmsg.pb.h"
#include "TcpServer.h"
#include <sstream>
#include "MyAES.h"

using namespace std;
using namespace Json;

class Client{
    private:
        MyAES* my_aes;
        MyJson my_json;
        MyRSA* my_rsa;
        string client_id;
        string server_id;
        string target_ip;
        string pubkey_filename;
        string prikey_filename;
        int target_port;
    public:
        Client(string json_filename = "client.json",string key_filename = "client"):
        my_json(json_filename){
            Value my_value = my_json.read_json();
            client_id = my_value["ClientID"].asString();
            server_id = my_value["ServerID"].asString();
            target_ip = my_value["targetIP"].asString();
            target_port = my_value["Port"].asInt();
            pubkey_filename = key_filename + "_public.pem";
            prikey_filename = key_filename + "_private.pem";
            my_rsa = new MyRSA(key_filename);
        };

        bool secret_key_negotiate(){
            // 从磁盘读取公钥
            ifstream ifs(pubkey_filename.c_str());
            stringstream str;
            str << ifs.rdbuf();

            RequestInfo info;
            info.clientId = client_id;
            info.serverId = server_id;
            info.cmdType = 1;
            // 1代表秘钥协商
            info.data = str.str();

            Myhash my_hash(M_SHA1);
            my_hash.add_data(str.str());

            // data就是非对称加密的公钥
            string hash_final = my_hash.get_final();
            info.sign = my_rsa->data_sign(hash_final);
            // sign就是公钥的哈希值的签名

            CodecFactory* factory = new RequestFactory(&info);
            Codec* codec = factory->createCodec();
            string encode_str = codec->encodeMsg();
            delete factory;
            delete codec;

            // 发送信息
            TcpSocket sock(target_ip,target_port);
            int ret = sock.sendMsg(encode_str);
            if(ret == -1){
                cout << "send message failed!" << endl;
            }
            string msg = sock.recvMsg();

            // 解析服务器数据
            factory = new RespondFactory(msg);

            codec = factory->createCodec();
            RespondMsg* res_data = (RespondMsg*)codec->decodeMsg();

            if(!res_data->status()){
                cout << "negotitate failed!" << endl;
                return false;
            }

            string key = my_rsa->prikey_decrypt(res_data->data());
            cout << "symmetry Encrypted key : " << key << endl;
            my_aes = new MyAES(key);
            delete factory;
            delete codec;
            return true;
        }

        bool secret_key_vertify(){
            RequestInfo info;
            info.clientId = client_id;
            info.serverId = server_id;
            info.cmdType = 2;
            // 2代表秘钥校验
            string send_data = my_aes->encrypt_data("secret key vertify");
            info.data = send_data;

            Myhash my_hash(M_SHA1);
            my_hash.add_data(send_data);

            // data就是非对称加密的公钥
            string hash_final = my_hash.get_final();
            info.sign = my_rsa->data_sign(hash_final);
            // sign就是公钥的哈希值的签名

            CodecFactory* factory = new RequestFactory(&info);
            Codec* codec = factory->createCodec();
            string encode_str = codec->encodeMsg();
            delete factory;
            delete codec;

            // 发送信息
            TcpSocket sock(target_ip,target_port);
            int ret = sock.sendMsg(encode_str);
            if(ret == -1){
                cout << "send message failed!" << endl;
            }
            string msg = sock.recvMsg();

            // 解析服务器数据
            factory = new RespondFactory(msg);

            codec = factory->createCodec();
            RespondMsg* res_data = (RespondMsg*)codec->decodeMsg();

            if(!res_data->status()){
                cout << "vertify failed!" << endl;
                return false;
            }
            string key = my_aes->decrypt_data(res_data->data());
            cout << "vertify success!" << endl;
            delete factory;
            delete codec;
            return true;
        }

        bool secret_key_sign_out(){
            RequestInfo info;
            info.clientId = client_id;
            info.serverId = server_id;
            info.cmdType = 3;
            // 3代表秘钥注销
            string send_data = my_aes->encrypt_data("secret key delete");
            info.data = send_data;

            Myhash my_hash(M_SHA1);
            my_hash.add_data(send_data);

            // data就是非对称加密的公钥
            string hash_final = my_hash.get_final();
            info.sign = my_rsa->data_sign(hash_final);
            // sign就是公钥的哈希值的签名

            CodecFactory* factory = new RequestFactory(&info);
            Codec* codec = factory->createCodec();
            string encode_str = codec->encodeMsg();
            delete factory;
            delete codec;

            // 发送信息
            TcpSocket sock(target_ip,target_port);
            int ret = sock.sendMsg(encode_str);
            if(ret == -1){
                cout << "send message failed!" << endl;
            }
            string msg = sock.recvMsg();

            // 解析服务器数据
            factory = new RespondFactory(msg);

            codec = factory->createCodec();
            RespondMsg* res_data = (RespondMsg*)codec->decodeMsg();

            if(!res_data->status()){
                cout << "secret key delete failed!" << endl;
                return false;
            }
            string key = my_aes->decrypt_data(res_data->data());
            cout << "secret key delete success!" << endl;
            delete factory;
            delete codec;
            return true;
        }

        ~Client(){};
};