#include <openssl/rsa.h>
#include <iostream>
#include <openssl/aes.h>
#include <string.h>
using namespace std;

int main(){
    // ׼�����ݺ���Կ
    const char* pt = "0123456789012345678901234567890123456789";
    cout << (int)pt[0] <<endl;
    const char* key = "1234567887654321";
    // ��ʼ����Կ
    AES_KEY encKey;
    AES_set_encrypt_key((const unsigned char*)key,128,&encKey);
    AES_KEY deckey;
    AES_set_decrypt_key((const unsigned char*)key,128,&deckey);
    // ����
    int length = 0,len = strlen((char*)pt) + 1;
    if(len % 16 != 0){ 
        length = ((len/16)+1)*16;
    }else{
        length = len;
    }
    unsigned char* out = new unsigned char[length];
    unsigned char ivec[AES_BLOCK_SIZE];
    memset(ivec,9,sizeof(ivec));
    AES_cbc_encrypt((const unsigned char*)pt,out,length,&encKey,ivec,AES_ENCRYPT);
    // ����
    memset(ivec,9,sizeof(ivec));
    // ע�� : ���ivec��AES_cbc_encrypt�����������ҵ������Ա����ٸ��������У���Ȼ��ʼ���ܺ�ʼ��16�ֽڻ�����
    unsigned char* data = new unsigned char[length];
    AES_cbc_encrypt(out,data,length,&deckey,ivec,AES_DECRYPT);
    cout <<(char*)data<<endl;
    cout << (int)data[0] <<endl;

    return 0;
}