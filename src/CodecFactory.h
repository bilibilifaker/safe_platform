#pragma once
#include <iostream>

using namespace std;

class CodecFactory{
    public:
        CodecFactory(){};

        virtual Codec* createCodec(){
            return nullptr;
        };
        
        virtual ~CodecFactory(){};
};