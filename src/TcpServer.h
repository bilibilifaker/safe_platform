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

using namespace std;

class TcpSocket{
    private:
        int m_socket;

    public:
        TcpSocket(){}

        TcpSocket(int connfd){
            m_socket = connfd;
        }

        TcpSocket(string ip,unsigned short port,int timeout = 10000){
            connectToHost(ip,port,timeout);
        }

        int connectToHost(string ip,unsigned short port,int timeout = 10000) {
            m_socket = socket(AF_INET,SOCK_STREAM,0);
            struct sockaddr_in target_addr;
            target_addr.sin_family = AF_INET;
            target_addr.sin_port = htons(port);
            target_addr.sin_addr.s_addr = inet_addr(ip.c_str());
            int ret = 0;
            ret = connectTimeout(&target_addr, (unsigned int)timeout);
            if (ret < 0){
		        // ��ʱ
		        if (ret == -1 && errno == ETIMEDOUT){
		        	ret = -2;
		        	return ret;
		        }
		        else{
		        	printf("connectTimeout error number: %d\n", errno);
		        	return errno;
		        }
	        }
	        return ret;
        }

        int sendMsg(string data,int timeout = 10000){
            fd_set set;
            FD_ZERO(&set);
            FD_SET(m_socket, &set);
            struct timeval time;
            time.tv_sec = timeout;
            time.tv_usec = 0;
            int ret = 0;
            do{
                ret = select(m_socket+1, nullptr, &set, nullptr, &time);
            }while(ret < 0 && errno == EINTR);
            if(ret == -1){
                // ʧ�ܷ���-1����ʱ����-1����errno = ETIMEDOUT
                if(errno == ETIMEDOUT){
                    return -2;
                }
                return -1;
            }
            int data_len = data.size() + 4;
            unsigned char* netdata = (unsigned char*)malloc(data_len);
            int netlen = htonl(data.size());
            memcpy(netdata, &netlen, 4);
    		memcpy(netdata + 4, data.data(), data.size());
            
            int writed = write(m_socket,netdata,data_len);
            return writed;
        }

        string recvMsg(int timeout = 10000){
            if(timeout <= 0){
                return "";
            }
            fd_set set;
            FD_ZERO(&set);
            FD_SET(m_socket, &set);
            struct timeval time;
            time.tv_sec = timeout;
            time.tv_usec = 0;
            int ret = 0;
            do{
                ret = select(m_socket+1, &set, nullptr, nullptr, &time);
            }while(ret < 0 && errno == EINTR);
            if(ret == 0){
                return "";
            }
            int net_data_len = 0;
            ret = readn(4,&net_data_len);
            if (ret == -1){
		        printf("func readn() err:%d \n", ret);
		        return string();
	        } else if (ret < 4) {
		        printf("func readn() err peer closed:%d \n", ret);
		        return string();
	        }
            net_data_len = ntohl(net_data_len);
            char* buf = (char *)malloc(net_data_len + 1);
            ret = readn(net_data_len,buf);
            buf[net_data_len] = '\0';
            string result = string(buf);
            free(buf);
            return result;
        }

        int connectTimeout(struct sockaddr_in *addr, unsigned int wait_seconds){
        	int ret;
        	socklen_t addrlen = sizeof(struct sockaddr_in);

	        if (wait_seconds > 0){
	        	setNonBlock(m_socket);	// ���÷�����IO
	        }

	        ret = connect(m_socket, (struct sockaddr*)addr, addrlen);
	        // ������ģʽ����, ����-1, ����errnoΪEINPROGRESS, ��ʾ�������ڽ�����
	        if (ret < 0 && errno == EINPROGRESS){
		        fd_set connect_fdset;
		        struct timeval timeout;
		        FD_ZERO(&connect_fdset);
		        FD_SET(m_socket, &connect_fdset);
		        timeout.tv_sec = wait_seconds;
		        timeout.tv_usec = 0;
		        do{
		        	// һ�����ӽ��������׽��־Ϳ�д ����connect_fdset������д������
		        	ret = select(m_socket + 1, NULL, &connect_fdset, NULL, &timeout);
		        }while(ret < 0 && errno == EINTR);
		        if (ret == 0){
			        // ��ʱ
			        ret = -1;
			        errno = ETIMEDOUT;
		        }
		        else if (ret < 0){
		        	return -1;
		        }
		        else if (ret == 1){
			        /* ret����Ϊ1����ʾ�׽��ֿ�д�������������������һ�������ӽ����ɹ���һ�����׽��ֲ�������*/
			        /* ��ʱ������Ϣ���ᱣ����errno�����У���ˣ���Ҫ����getsockopt����ȡ�� */
			        int err;
			        socklen_t sockLen = sizeof(err);
			        int sockoptret = getsockopt(m_socket, SOL_SOCKET, SO_ERROR, &err, &sockLen);
			        if (sockoptret == -1){
			        	return -1;
			        }
			        if (err == 0){
			        	ret = 0;	// �ɹ���������
			        }
			        else{
			        	// ����ʧ��
			        	errno = err;
			        	ret = -1;
			        }
		        }
	        }
	        if (wait_seconds > 0){
	        	setBlock(m_socket);	// �׽������û�����ģʽ
	        }
	        return 0;
        }

        int readn(int number,void *buf){
            size_t nleft = number;
	        ssize_t nread;
	        char *bufp = (char*)buf;
	        while (nleft > 0){
		        if ((nread = read(m_socket, bufp, nleft)) < 0){
			        if (errno == EINTR){
			        	continue;
			        }
			        return -1;
		        }
		        else if (nread == 0){
		        	return number - nleft;
		        }
		        bufp += nread;
		        nleft -= nread;
	        }
	        return number;
        }

        int setNonBlock(int fd){
        	int flags = fcntl(fd, F_GETFL);
        	if (flags == -1){
	        	return flags;
	        }

	        flags |= O_NONBLOCK;
	        int ret = fcntl(fd, F_SETFL, flags);
	        return ret;
        }

        int setBlock(int fd){
	        int ret = 0;
	        int flags = fcntl(fd, F_GETFL);
	        if (flags == -1){
	        	return flags;
	        }

	        flags &= ~O_NONBLOCK;
	        ret = fcntl(fd, F_SETFL, flags);
	        return ret;
        }

        void disconnect(){
            int res = close(m_socket);
        }

        ~TcpSocket(){
            int res = close(m_socket);
        }
};

class TcpServer{
    private:
        int m_lfd;

    public:
        TcpServer(){
            int lis = setLinsten();
        };

        TcpServer(int port){
            int lis = setLinsten(port);
        };

        TcpSocket* acceptConn(unsigned int timeout = 10000) {
            int res = 0;
            if(timeout <= 0){
                return nullptr;
            }
            fd_set set;
            FD_ZERO(&set);
            FD_SET(m_lfd, &set);
            struct timeval time_wait;
            time_wait.tv_sec = timeout;
            time_wait.tv_usec = 0;
            int ret = -1;
            do{
                ret = select(m_lfd+1, &set, nullptr, nullptr, &time_wait);
            }while(ret < 0 && errno == EINTR);
            if(ret <= 0){
                return nullptr;
            }
            struct sockaddr addr;
            socklen_t client_addr_len = sizeof(addr);
            int fd = accept(m_lfd,&addr,&client_addr_len);
            if(fd == -1){
                return nullptr;
            }
            TcpSocket *result = new TcpSocket(fd);
            return result;
        }

        int setLinsten(int port = 5000){
            int res = 0;
            m_lfd = socket(AF_INET,SOCK_STREAM,0);
            if (m_lfd == -1){
		        res = errno;
		        return res;
        	}
            struct sockaddr_in target_addr;
            bzero(&target_addr,sizeof(target_addr));
            target_addr.sin_family = AF_INET;
            target_addr.sin_port = htons(5000);
            // Ĭ�ϼ����˿�5000
            target_addr.sin_addr.s_addr = htonl(INADDR_ANY);
            int on = 1;
	        // ���ö˿ڸ���
            res = setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
            if (res == -1){
		        res = errno;
		        return res;
	        }
            res = bind(m_lfd,(struct sockaddr *)&target_addr,sizeof(target_addr));
            if (res == -1){
		        res = errno;
		        return res;
	        }
            res = listen(m_lfd,10);
            if (res == -1){
		        res = errno;
		        return res;
	        }
            return res;
        }
        
        ~TcpServer(){
            int res = close(m_lfd);
        };
};