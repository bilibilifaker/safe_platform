#include <iostream>
#include "TcpServer.h"

int main(){
    TcpSocket client("127.0.0.1",5000);
    while(1){
        string temp;
        cin >> temp;
        client.sendMsg(temp);
    }
    return 0;
}

// g++ -o testTcpClient testTcpClient.cpp -std=c++11