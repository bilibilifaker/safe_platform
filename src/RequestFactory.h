#pragma once
#include "Request.h"
#include "CodecFactory.h"

using namespace std;

class RequestFactory : public CodecFactory{
    private:
        bool m_flag;
        string m_encStr;
        RequestInfo* m_info;

    public:
        RequestFactory(){}

        RequestFactory(string enc){
            m_encStr = enc;
            m_flag = true;
        }

        RequestFactory(RequestInfo* info){
            m_info = new RequestInfo(*info);
            m_flag = false;
        }

        virtual Codec* createCodec(){
            Codec* result;
            if(m_flag) {
                result = new Request(m_encStr);
            }
            else{
                result = new Request(*m_info);
            }
            return result;
        };
        
        virtual ~RequestFactory(){};
};