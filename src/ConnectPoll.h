#include "TcpServer.h"
#include <queue>
#include <iostream>

using namespace std;

class ConnectPoll {
    private:
        string m_ip;
        unsigned short m_port;
        queue<TcpSocket> m_socket;

    public:
        ConnectPoll(){};

        ConnectPoll (string ip, unsigned short port, int number) {
            m_ip = ip;
            m_port = port;
            for(int i = 0; i < number ; i++) {
                TcpSocket socket;
                int fd = socket.connectToHost(ip, port);
                TcpSocket temp(fd);
                m_socket.push(temp);
            }
        };

        TcpSocket getConnect() {
            TcpSocket result(-1);
            if (m_socket.size() > 0) {
                result = m_socket.front();
                m_socket.pop();
            }
            return result;
        }
        
        void putConnect(TcpSocket sock, bool isvaild = true){
            if (isvaild) {
                m_socket.push(sock);
            }
            else {
                TcpSocket socket;
                int fd = socket.connectToHost(m_ip, m_port);
                TcpSocket temp(fd);
                m_socket.push(temp);
            }
        }

        ~ConnectPoll(){};
};