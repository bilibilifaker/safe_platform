#include <openssl/rsa.h>
#include <iostream>
#include <openssl/sha.h>
#include <openssl/pem.h>
using namespace std;

int main(){
    RSA *my_rsa = RSA_new();
    BIGNUM *my_bignum = BN_new();
    BN_set_word(my_bignum,1145);
    RSA_generate_key_ex(my_rsa,1024,my_bignum,nullptr);
    // 第二个参数决定秘钥长度  第三个数随便给个数就行 几千之类的

    FILE* fp = fopen("public.pem","w");
    PEM_write_RSAPublicKey(fp,my_rsa);
    fclose(fp);

    fp = fopen("private.pem","w");
    PEM_write_RSAPrivateKey(fp,my_rsa,nullptr,nullptr,0,nullptr,nullptr);
    fclose(fp);

    BIO* bio = BIO_new_file("BIOpublic.pem","w");
    PEM_write_bio_RSAPublicKey(bio,my_rsa);
    BIO_free(bio);

    bio = BIO_new_file("BIOprivate.pem","w");
    PEM_write_bio_RSAPrivateKey(bio,my_rsa,nullptr,nullptr,0,nullptr,nullptr);
    BIO_free(bio);

    // 从内存获取公钥私钥
    RSA* publicKey = RSAPublicKey_dup(my_rsa);
    RSA* privateKey = RSAPrivateKey_dup(my_rsa);

    string text = "let me change the world.";
    cout << "row data : " << text << endl;
    // 公钥加密
    BIO* readbio = BIO_new_file("public.pem","r");
    RSA* pubkey = RSA_new();
    RSA* ret = PEM_read_bio_RSAPublicKey(readbio,&pubkey,nullptr,nullptr);
    if(ret == nullptr){
        cout << "read public key failed!" << endl;
        return 0;
    }
    BIO_free(readbio);
    //char *result = new char[128];
    // 128是因为，数据被加密之后，长度和密钥-11的长度相同
    // 如果不知道密钥长度，这个RSA_size函数可以得到秘钥的长度
    // 或者RSA_public_encrypt的返回值也就是密文长度
    int keyLen = RSA_size(pubkey);
    char *result = new char[keyLen];
    int len = RSA_public_encrypt(text.size(),(const unsigned char*)text.data(),
            (unsigned char*)result,pubkey,RSA_PKCS1_PADDING);
    string text2(result, len);
    cout << "after encrypt : " << text2 << endl;

    // 私钥解密
    readbio = BIO_new_file("private.pem","r");
    RSA* prikey = RSA_new();
    ret = PEM_read_bio_RSAPrivateKey(readbio,&prikey,nullptr,nullptr);
    if(ret == nullptr){
        cout << "read private key failed!" << endl;
        return 0;
    }
    BIO_free(readbio);
    keyLen = RSA_size(prikey);
    char *result2 = new char[keyLen];
    len = RSA_private_decrypt(text2.size(),(const unsigned char*)text2.data(),
            (unsigned char*)result2,prikey,RSA_PKCS1_PADDING);
    string text3(result2, len);
    cout << "after decrypt : " << text3 << endl;



    publicKey = RSAPublicKey_dup(my_rsa);
    privateKey = RSAPrivateKey_dup(my_rsa);
    string sign_text = "sign check";
    // 签名
    // 数据签名的长度不能超过密钥-11的长度
    unsigned int outLen;
    unsigned char* out = new unsigned char[RSA_size(privateKey)];
    RSA_sign(NID_sha1,(unsigned char *)sign_text.data(),text.size(),out,&outLen,privateKey);
    
    // 验证签名
    string sign_text2((char *)out,outLen);
    int vertify = RSA_verify(NID_sha1,(unsigned char *)sign_text.data(),text.size(),(unsigned char *)sign_text2.data(),sign_text2.size(),publicKey);
    cout << "vertify : " << vertify << endl;

    
    return 0;
}