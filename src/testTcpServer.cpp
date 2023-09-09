#include <iostream>
#include "TcpServer.h"

int main(){
    TcpServer server(5000);
    TcpSocket *tcp = server.acceptConn();
    while(1){
        string res = tcp->recvMsg();
        cout << "receive message: " << res << endl;
    }
    return 0;
}

// g++ -o testTcpServer testTcpServer.cpp -std=c++11