#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <malloc.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <fcntl.h>
#include <iostream>
#include <string>

using namespace std;

int main(){
    int a = 3;
    a = htonl(3);
    char* temp = reinterpret_cast<char*>(&a);
    cout << temp[0]<<temp[1]<<temp[3]<<temp[3]<<endl;
    cout <<a<<endl;
    cout <<htonl(3)<<endl;
    cout <<to_string(htonl(3))<<endl;
    cout <<to_string(htonl(3)).c_str()<<endl;
    cout <<sizeof(htonl(3))<<endl;
    cout <<sizeof(to_string(htonl(3)))<<endl;
    cout <<sizeof(4)<<endl;
    cout <<sizeof(to_string(4).c_str())<<endl;
}