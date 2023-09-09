#pragma once
#include <iostream>

using namespace std;

class Codec{
    private:
        string m_encStr;

    public:
        Codec(){};

        virtual string encodeMsg(){
            return "";
        }

        virtual void* decodeMsg(){
            return nullptr;
        }
        
        virtual ~Codec(){};
};