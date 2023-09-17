OBJS = Myhash MyRSA MyAES testTcpServer client testTcpClient testseckeyshm1 testCodecFactory ConnectPoll myjson server testthreadpool
TARGET = all

$(TARGET):$(OBJS)
	echo "compile finnal"

Myhash:Myhash.h
	g++ Myhash.cpp -o Myhash -std=c++11 -lcrypto

MyRSA:MyRSA.h
	g++ MyRSA.cpp -o MyRSA -std=c++11 -lcrypto
	
MyAES:MyAES.h
	g++ MyAES.cpp -o MyAES -std=c++11 -lcrypto

myjson:MyJson.h
	g++ myjson.cpp -o myjson -std=c++11 -lcrypto -ljson

client:client.h
	g++ -o client client.cpp -std=c++11 ../proto/requestmsg.pb.cc ../proto/respondmsg.pb.cc -I/path/to/protobuf/include -L/path/to/protobuf/lib -pthread -lprotobuf -std=c++11 -ljson -lcrypto

server:server.h
	g++ -o server server.cpp -std=c++11 ../proto/requestmsg.pb.cc ../proto/respondmsg.pb.cc -I/path/to/protobuf/include -L/path/to/protobuf/lib -pthread -lprotobuf -std=c++11 -ljson -lcrypto -I/usr/include/mysql -L/usr/lib64/mysql -lmysqlclient

testTcpServer:TcpServer.h
	g++ -o testTcpServer testTcpServer.cpp -std=c++11

ConnectPoll:TcpServer.h
	g++ -o ConnectPoll ConnectPoll.h -std=c++11

testthreadpool:
	g++ -o testthreadpool testthreadpool.cpp -std=c++11 -lpthread

testTcpClient:TcpServer.h
	g++ -o testTcpClient testTcpClient.cpp -std=c++11

testseckeyshm1:SecKeyShm.h
	g++ -o testseckeyshm1 testseckeyshm1.cpp -std=c++11

testCodecFactory:Request.h Response.h Codec.h RequestFactory.h RespondFactory.h
	g++ -o testCodecFactory testCodecFactory.cpp ../proto/requestmsg.pb.cc ../proto/respondmsg.pb.cc -I/path/to/protobuf/include -L/path/to/protobuf/lib -pthread -lprotobuf -std=c++11

clean:
	rm -rf $(OBJS)

# compile :  make
# erase all compile :  make clean