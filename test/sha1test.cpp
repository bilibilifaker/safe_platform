#include <iostream>
#include <string.h>
#include <openssl/sha.h>
#include <stdio.h>
using namespace std;

void sha1Test(){
    SHA_CTX ctx;
    SHA1_Init(&ctx);
    SHA1_Update(&ctx,"hello",strlen("hello"));
    SHA1_Update(&ctx,", world",strlen(", world"));
    unsigned char* md = new unsigned char[SHA_DIGEST_LENGTH+1];
    char* res = new char[SHA_DIGEST_LENGTH*2+1];
    SHA1_Final(md,&ctx);
    for(int i = 0;i<SHA_DIGEST_LENGTH;i++){
        sprintf(&res[i*2],"%02x",md[i]);
    }
    cout << "sha1 : " << res <<endl;
}

int main(){
    sha1Test();
    return 0;
}