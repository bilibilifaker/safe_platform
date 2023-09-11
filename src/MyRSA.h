#include <openssl/rsa.h>
#include <iostream>
#include <openssl/sha.h>
#include <openssl/pem.h>
using namespace std;

class MyRSA{
    private:
        RSA* public_key;
        RSA* private_key;
    public:
        MyRSA(){
            RSA *my_rsa = RSA_new();
            BIGNUM *my_bignum = BN_new();
            BN_set_word(my_bignum,1145);
            RSA_generate_key_ex(my_rsa, 1024, my_bignum, nullptr);
            public_key = RSAPublicKey_dup(my_rsa);
            private_key = RSAPrivateKey_dup(my_rsa);
        }

        void Init_key(){

        }

        string pubkey_encrypt(string text){
            int keyLen = RSA_size(public_key);
            char *result = new char[keyLen];
            int len = RSA_public_encrypt(text.size(),(const unsigned char*)text.data(),
            reinterpret_cast<unsigned char*>(result),public_key,RSA_PKCS1_PADDING);
            return string(result, len);
        }

        string prikey_decrypt(string text){
            int keyLen = RSA_size(private_key);
            char *result2 = new char[keyLen];
            int len = RSA_private_decrypt(text.size(),(const unsigned char*)text.data(),
            (unsigned char*)result2,private_key,RSA_PKCS1_PADDING);
            return string(result2, len);
        }
        
        string data_sign(string text){
            unsigned int outLen;
            unsigned char* out = new unsigned char[RSA_size(private_key)];
            RSA_sign(NID_sha1,(unsigned char *)text.data(),text.size(),out,&outLen,private_key);
            return string(reinterpret_cast<char*>(out), static_cast<int>(outLen));
        }

        int data_vertify(string text, string sign_text){
            int vertify = RSA_verify(NID_sha1,reinterpret_cast<const unsigned char *>(text.data()),text.size(),reinterpret_cast<const unsigned char*>(sign_text.data()),sign_text.size(),public_key);
            return vertify;
        }

        ~MyRSA(){};
};