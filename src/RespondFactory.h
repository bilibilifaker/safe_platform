#pragma once
#include "Response.h"
#include "CodecFactory.h"

using namespace std;

class RespondFactory : public CodecFactory{
    private:
        bool m_flag;
        string m_encStr;
        ResponseInfo* m_info;

    public:
        RespondFactory(){}

        RespondFactory(string enc){
            m_encStr = enc;
            m_flag = true;
        }

        RespondFactory(ResponseInfo* info){
            m_info = new ResponseInfo(*info);
            m_flag = false;
        }

        virtual Codec* createCodec(){
            Codec* result;
            if(m_flag) {
                result = new Response(m_encStr);
            }
            else{
                result = new Response(*m_info);
            }
            return result;
        };
        
        virtual ~RespondFactory(){};
};