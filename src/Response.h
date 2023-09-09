#pragma once
#include "../proto/respondmsg.pb.h"
#include "./Codec.h"

using namespace std;

struct ResponseInfo{
    bool status;           // ����ֵ
    int seckeyid;      // �Գ���Կ���
    string clientId;   // �ͻ��˱��
    string serverId;   // ����˱��
    string data;       // �������������
};

class Response : public Codec{
    private:
        string m_encStr;
        RespondMsg m_msg;

    public:
        Response(){};

        Response(string encStr):m_encStr(encStr){};

        Response(int status,int seckeyID,string clientID,string serverID,string data){
            initMessage(status,seckeyID,clientID,serverID,data);
        };

        Response(ResponseInfo &info){
            initMessage(info);
        };

        void initMessage(string encstr){
            m_encStr = encstr;
        }

        void initMessage(int status,int seckeyID,string clientID,string serverID,string data){
            this->m_msg.set_status(status);
            this->m_msg.set_seckeyid(seckeyID);
            this->m_msg.set_clientid(clientID);
            this->m_msg.set_serverid(serverID);
            this->m_msg.set_data(data);
        }

        void initMessage(ResponseInfo &info){
            this->m_msg.set_status(info.status);
            this->m_msg.set_seckeyid(info.seckeyid);
            this->m_msg.set_clientid(info.clientId);
            this->m_msg.set_serverid(info.serverId);
            this->m_msg.set_data(info.data);
        }

        string encodeMsg(){
            string output;
            m_msg.SerializeToString(&output);
            return output;
        }

        void* decodeMsg(){
            m_msg.ParseFromString(m_encStr);
            return &m_msg;
        }
        
        ~Response(){};
};