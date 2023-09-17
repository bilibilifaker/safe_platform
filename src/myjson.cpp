#include <iostream>
#include "MyJson.h"
using namespace std;

int main(){
    MyJson temp;
    temp.append(123);
    temp.append("qwe");
    temp.write_json();

    MyJson read;
    read.read_json();
}