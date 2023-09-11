#include <iostream>
#include <string.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <stdio.h>

using namespace std;

enum hash_type{
    M_MD5,
    M_SHA1,
    M_SHA224,
    M_SHA256,
    M_SHA384,
    M_SHA512
};

class Myhash{
    private:
		hash_type m_type;
        MD5_CTX m_md5;
	    SHA_CTX m_sha1;
	    SHA256_CTX m_sha224;
	    SHA256_CTX m_sha256;
	    SHA512_CTX m_sha384;
	    SHA512_CTX m_sha512;
	public:
        Myhash(){
            m_type = hash_type::M_MD5;
            MD5_Init(&m_md5);
        };

        Myhash(hash_type type){
            m_type = type;
            switch (m_type)
            {
            case hash_type::M_MD5:
                MD5_Init(&m_md5);
                break;
            case hash_type::M_SHA1:
                SHA1_Init(&m_sha1); 
                break;
            case hash_type::M_SHA224:
                SHA224_Init(&m_sha224);
                break;
            case hash_type::M_SHA256:
                SHA256_Init(&m_sha256);
                break;
            case hash_type::M_SHA384:
                SHA384_Init(&m_sha384);
                break;
            case hash_type::M_SHA512:
                SHA512_Init(&m_sha512); 
                break;
            default:
                break;
            }
        }

        int add_data(string data){
            switch (m_type)
            {
            case hash_type::M_MD5:
                MD5_Update(&m_md5, data.c_str(),strlen(data.data()));
                //MD5_Update(&m_md5, data, strlen(data));
                break;
            case hash_type::M_SHA1:
                SHA1_Update(&m_sha1, data.data(), strlen(data.data()));
                break;
            case hash_type::M_SHA224:
                SHA224_Update(&m_sha224, data.data(), strlen(data.data()));
                break;
            case hash_type::M_SHA256:
                SHA256_Update(&m_sha256, data.data(), strlen(data.data()));
                break;
            case hash_type::M_SHA384:
                SHA384_Update(&m_sha384, data.data(), strlen(data.data()));
                break;
            case hash_type::M_SHA512:
                SHA512_Update(&m_sha512, data.data(), strlen(data.data()));
                break;
            default:
                break;
            }
        }

        string get_final(){
	        string result = string();
	        switch (m_type)
	        {
	        case hash_type::M_MD5:
	        	result = md5Result();
	        	break;
	        case hash_type::M_SHA1:
	        	result = sha1Result();
	        	break;
	        case hash_type::M_SHA224:
	        	result = sha224Result();
	        	break;
	        case hash_type::M_SHA256:
	        	result = sha256Result();
	        	break;
	        case hash_type::M_SHA384:
	        	result = sha384Result();
	        	break;
	        case hash_type::M_SHA512:
	        	result = sha512Result();
	        	break;
	        default:
	        	result = md5Result();
	        	break;
	        }
	        return result;
        }

        string md5Result(){
	        unsigned char md[MD5_DIGEST_LENGTH];
	        char res[MD5_DIGEST_LENGTH * 2 + 1];
	        MD5_Final(md, &m_md5);
	        for (int i = 0; i < MD5_DIGEST_LENGTH; ++i){
	        	sprintf(&res[i * 2], "%02x", md[i]);
	        }
	        return string(res, MD5_DIGEST_LENGTH * 2 + 1);
        }

        string sha1Result(){
	        unsigned char md[SHA_DIGEST_LENGTH];
	        char res[SHA_DIGEST_LENGTH * 2 + 1];
	        SHA1_Final(md, &m_sha1);
	        for (int i = 0; i < SHA_DIGEST_LENGTH; ++i){
	        	sprintf(&res[i * 2], "%02x", md[i]);
	        }
	        return string(res, SHA_DIGEST_LENGTH * 2 + 1);
        }

        string sha224Result(){
	        unsigned char md[SHA224_DIGEST_LENGTH];
	        char res[SHA224_DIGEST_LENGTH * 2 + 1];
	        SHA224_Final(md, &m_sha224);
	        for (int i = 0; i < SHA224_DIGEST_LENGTH; ++i){
	        	sprintf(&res[i * 2], "%02x", md[i]);
	        }
	        return string(res, SHA224_DIGEST_LENGTH * 2 + 1);
        }

        string sha256Result(){
	        unsigned char md[SHA256_DIGEST_LENGTH];
	        char res[SHA256_DIGEST_LENGTH * 2 + 1];
	        SHA256_Final(md, &m_sha256);
	        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i){
	        	sprintf(&res[i * 2], "%02x", md[i]);
	        }
	        return string(res, SHA256_DIGEST_LENGTH * 2 + 1);
        }

        string sha384Result(){
	        unsigned char md[SHA384_DIGEST_LENGTH];
	        char res[SHA384_DIGEST_LENGTH * 2 + 1];
	        SHA384_Final(md, &m_sha384);
	        for (int i = 0; i < SHA384_DIGEST_LENGTH; ++i){
	        	sprintf(&res[i * 2], "%02x", md[i]);
	        }
	        return string(res, SHA384_DIGEST_LENGTH * 2 + 1);
        }

        string sha512Result(){
	        unsigned char md[SHA512_DIGEST_LENGTH];
	        char res[SHA512_DIGEST_LENGTH * 2 + 1];
	        SHA512_Final(md, &m_sha512);
	        for (int i = 0; i < SHA512_DIGEST_LENGTH; ++i){
	        	sprintf(&res[i * 2], "%02x", md[i]);
	        }
	        return string(res, SHA512_DIGEST_LENGTH * 2 + 1);
        }

        ~Myhash(){};
};