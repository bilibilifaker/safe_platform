#include <iostream>
#include "MyAES.h"
using namespace std;

int main(){
    MyAES temp;
    string res = temp.encrypt_data("successfully transform.");
    string res2 = temp.decrypt_data(res);
    cout << res2 << endl;
}