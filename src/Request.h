#pragma once
#include "../proto/requestmsg.pb.h"
#include "./Codec.h"

using namespace std;

struct RequestInfo{
    // 1 秘钥协商   2 秘钥校验    3 秘钥注销
    int cmdType ;      // 报文类型
    string clientId;    // 客户端编号
    string serverId;    // 服务器端编号
    string sign;
    string data;
};

class Request : public Codec{
    private:
        string m_encStr;
        RequestMsg m_msg;

    public:
        Request(){};

        Request(string encStr):m_encStr(encStr){};

        Request(RequestInfo info){
            initMessage(info);
        };

        Request(int cmd,string clientID,string serverID,string sign,string data){
            initMessage(cmd,clientID,serverID,sign,data);
        }

        void initMessage(string encstr){
            this->m_encStr = encstr;
        }

        void initMessage(RequestInfo info){
            this->m_msg.set_cmdtype(info.cmdType);
            this->m_msg.set_clientid(info.clientId);
            this->m_msg.set_serverid(info.serverId);
            this->m_msg.set_sign(info.sign);
            this->m_msg.set_data(info.data);
        }

        void initMessage(int cmd,string clientID,string serverID,string sign,string data){
            this->m_msg.set_cmdtype(cmd);
            this->m_msg.set_clientid(clientID);
            this->m_msg.set_serverid(serverID);
            this->m_msg.set_sign(sign);
            this->m_msg.set_data(data);
        }

        string encodeMsg(){
            string output;
            m_msg.SerializeToString(&output);
            return output;
        }

        void* decodeMsg(){
            m_msg.ParseFromString(m_encStr);
            RequestMsg* ret = new RequestMsg(m_msg);
            return (void*)ret;
        }
        
        virtual ~Request(){};
};