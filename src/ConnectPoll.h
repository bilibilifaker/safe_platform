#include "TcpServer.h"
#include <queue>
#include <iostream>

using namespace std;

class ConnectPoll{
    private:
        queue<TcpSocket> m_socket;

    public:
        ConnectPoll(){};

        ConnectPoll(string ip, unsigned short port, int number){
            for(int i = 0; i < number ; i++) {
                TcpSocket socket;
                int fd = socket.connectToHost(ip, port);
                TcpSocket temp(fd);
                m_socket.push(temp);
            }
        };

        TcpSocket getConnect(){
            TcpSocket result(-1);
            if(m_socket.size() > 0){
                result = m_socket.front();
                m_socket.pop();
            }
            return result;
        }
        
        void putConnect(TcpSocket sock,bool isvaild = true){
            if(isvaild) {
                m_socket.push(sock);
            }
        }

        ~ConnectPoll(){};
};