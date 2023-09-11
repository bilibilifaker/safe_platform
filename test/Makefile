OBJS = sha1test rsa aes myjson testproto stringtest test
TARGET = all

$(TARGET):$(OBJS)
	echo "compile finnal"

sha1test:sha1test.cpp
	g++ sha1test.cpp -o sha1test -lcrypto

rsa:rsa.cpp
	g++ rsa.cpp -o rsa -lcrypto -std=c++11

stringtest:stringtest.cpp
	g++ stringtest.cpp -o stringtest -lcrypto -std=c++11

aes:aes.cpp
	g++ aes.cpp -o aes -lcrypto -std=c++11 -lssl

myjson:myjson.cpp
	g++ myjson.cpp -o myjson -lcrypto -std=c++11 -lpthread -ljson

testproto:testproto.cpp
	g++ -o testproto testproto.cpp ../proto/test.pb.cc -I/path/to/protobuf/include -L/path/to/protobuf/lib -pthread -lprotobuf -std=c++11

test:test.cpp
	g++ -o test test.cpp ../proto/person.pb.cc ../proto/info.pb.cc -I/path/to/protobuf/include -L/path/to/protobuf/lib -pthread -lprotobuf -std=c++11

clean:
	rm -rf $(OBJS) BIOprivate.pem BIOpublic.pem private.pem public.pem test.json

# compile :  make
# erase all compile :  make clean