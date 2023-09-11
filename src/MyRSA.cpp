#include <iostream>
#include "MyRSA.h"

using namespace std;

int main(){
    MyRSA myrsa;
    string jiami = myrsa.pubkey_encrypt("good job!");
    string jiemi = myrsa.prikey_decrypt(jiami);
    cout << jiemi << endl;
    
    string sign = myrsa.data_sign("qwerty");
    int res = myrsa.data_vertify("qwerty", sign);
    cout << "res = " << res << endl;
    return 0;
}