#include <iostream>
#include "Myhash.h"

int main(){
    Myhash temp(hash_type::M_MD5);
    temp.add_data("hello world");
    string res = temp.get_final();
    cout << res << endl;
    return 0;
}