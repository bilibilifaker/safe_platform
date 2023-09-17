#include <openssl/rsa.h>
#include <iostream>
#include <openssl/aes.h>
#include <string.h>
using namespace std;

class MyAES{
    private:
        AES_KEY enc_key;
        AES_KEY dec_key;
    public:
        MyAES(string key = "1234567887654321",string ivec = "99999999"){
            AES_set_encrypt_key((const unsigned char*)key.data(),128,&enc_key);
            AES_set_decrypt_key((const unsigned char*)key.data(),128,&dec_key);
        }

        string encrypt_data(string data){
            int length = 0,len = strlen(data.c_str()) + 1;
            if(len % 16 != 0){ 
                length = ((len/16)+1)*16;
            }else{
                length = len;
            }
            unsigned char* out = new unsigned char[length];
            unsigned char ivec[AES_BLOCK_SIZE];
            memset(ivec,9,sizeof(ivec));
            AES_cbc_encrypt((const unsigned char*)data.data(),out,length,&enc_key,ivec,AES_ENCRYPT);
            return string((char *)out,length);
        }

        string decrypt_data(string data){
            int length = 0,len = strlen(data.c_str()) + 1;
            if(len % 16 != 0){ 
                length = ((len/16)+1)*16;
            }else{
                length = len;
            }
            unsigned char* out = new unsigned char[length];
            unsigned char ivec[AES_BLOCK_SIZE];
            memset(ivec,9,sizeof(ivec));
            AES_cbc_encrypt((const unsigned char*)data.data(),out,length,&dec_key,ivec,AES_DECRYPT);
            return string((char *)out,len);
        }

        ~MyAES(){}
};