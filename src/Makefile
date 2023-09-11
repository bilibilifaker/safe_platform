OBJS = Myhash MyRSA MyAES testTcpServer testTcpClient testseckeyshm1 testCodecFactory ConnectPoll
TARGET = all

$(TARGET):$(OBJS)
	echo "compile finnal"

Myhash:Myhash.h
	g++ Myhash.cpp -o Myhash -std=c++11 -lcrypto

MyRSA:MyRSA.h
	g++ MyRSA.cpp -o MyRSA -std=c++11 -lcrypto
	
MyAES:MyAES.h
	g++ MyAES.cpp -o MyAES -std=c++11 -lcrypto

testTcpServer:TcpServer.h
	g++ -o testTcpServer testTcpServer.cpp -std=c++11

ConnectPoll:TcpServer.h
	g++ -o ConnectPoll ConnectPoll.cpp -std=c++11

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