#include <openssl/rsa.h>
#include <iostream>
#include <openssl/sha.h>
#include <openssl/pem.h>
#include <string.h>
using namespace std;

class MyRSA{
    private:
        RSA* public_key;
        RSA* private_key;
        string pubkey_filename;
        string prikey_filename;
    public:
        MyRSA(string filename = ""){
            if(filename == "client"){
                RSA *my_rsa = RSA_new();
                BIGNUM *my_bignum = BN_new();
                BN_set_word(my_bignum,1145);
                RSA_generate_key_ex(my_rsa, 1024, my_bignum, nullptr);
                public_key = RSAPublicKey_dup(my_rsa);
                private_key = RSAPrivateKey_dup(my_rsa);
                // ¹«Ô¿Ë½Ô¿Ð´Èë´ÅÅÌ
                pubkey_filename = filename + "_public.pem";
                prikey_filename = filename + "_private.pem";
                FILE* fp = fopen(pubkey_filename.c_str(),"w");
                PEM_write_RSAPublicKey(fp,my_rsa);
                fclose(fp);

                fp = fopen(prikey_filename.c_str(),"w");
                PEM_write_RSAPrivateKey(fp,my_rsa,nullptr,nullptr,0,nullptr,nullptr);
                fclose(fp);
                return ;
            }
            pubkey_filename = filename + "_public.pem";
            BIO* readbio = BIO_new_file(pubkey_filename.c_str(),"r");
            public_key = RSA_new();
            RSA* ret = PEM_read_bio_RSAPublicKey(readbio,&public_key,nullptr,nullptr);
            BIO_free(readbio);
        }

        string pubkey_encrypt(string text){
            int keyLen = RSA_size(public_key);
            char *result = new char[keyLen];
            int len = RSA_public_encrypt(text.size(),(const unsigned char*)text.data(),
            reinterpret_cast<unsigned char*>(result),public_key,RSA_PKCS1_PADDING);
            string res = string();
            res = toBase64(result, len);
            return res;
        }

        string prikey_decrypt(string text){
            char* data = fromBase64(text);
            int keyLen = RSA_size(private_key);
            char *result2 = new char[keyLen+1];
            int len = RSA_private_decrypt(keyLen,(unsigned char*)data,
            (unsigned char*)result2,private_key,RSA_PKCS1_PADDING);
            return string(result2, len);
        }
        
        string data_sign(string text){
            unsigned int outLen = 0;
            unsigned char* out = new unsigned char[RSA_size(private_key)];
            RSA_sign(NID_sha1,(unsigned char *)text.data(),text.size(),out,&outLen,private_key);
            // string res(reinterpret_cast<char*>(out), static_cast<int>(outLen));
            string res = toBase64((char*)out, outLen);
            //cout << (int)outLen << endl;
            //cout << outLen << endl;
            //string res((char*)(out), 20);
            //cout << res <<endl;
            //cout <<"11111111111111111111111111111" << endl;
            return res;
        }

        int data_vertify(string text, string sign_text){
            int keyLen = RSA_size(public_key);
	        char* sign = fromBase64(sign_text);
            int vertify = RSA_verify(NID_sha1,reinterpret_cast<const unsigned char *>(text.data()),text.size(),
                                    (unsigned char*)sign,keyLen,public_key);
            return vertify;
        }

        string toBase64(const char* str, int len) {
	        BIO* mem = BIO_new(BIO_s_mem());
	        BIO* bs64 = BIO_new(BIO_f_base64());
	        // mem添加到bs64中
	        bs64 = BIO_push(bs64, mem);
	        // 写数据
	        BIO_write(bs64, str, len);
	        BIO_flush(bs64);
	        // 得到内存对象指针
	        BUF_MEM *memPtr;
	        BIO_get_mem_ptr(bs64, &memPtr);
	        string retStr = string(memPtr->data, memPtr->length - 1);
	        BIO_free_all(bs64);
	        return retStr;
        }

        char* fromBase64(string str) {
	        int length = str.size();
	        BIO* bs64 = BIO_new(BIO_f_base64());
	        BIO* mem = BIO_new_mem_buf(str.data(), length);
	        BIO_push(bs64, mem);
	        char* buffer = new char[length];
	        memset(buffer, 0, length);
	        BIO_read(bs64, buffer, length);
	        BIO_free_all(bs64);

	        return buffer;
        }

        ~MyRSA(){};
};