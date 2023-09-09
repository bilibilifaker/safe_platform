//g++ -o proto_test testproto.cpp ../proto/test.pb.cc -I/path/to/protobuf/include -L/path/to/protobuf/lib -pthread -lprotobuf -std=c++11

#include "../proto/test.pb.h"

int main() {
  // 创建一个 Person 消息对象
  Person person;
  person.set_name("John");
  person.set_age(30);
  person.add_email("john@example.com");

  // 将消息序列化为字节流
  std::string serialized_data;
  person.SerializeToString(&serialized_data);

  // 从字节流中解析消息
  Person parsed_person;
  parsed_person.ParseFromString(serialized_data);

  // 访问消息字段
  std::cout << "Name: " << parsed_person.name() << std::endl;
  std::cout << "Age: " << parsed_person.age() << std::endl;
  for (int i = 0; i < parsed_person.email_size(); ++i) {
    std::cout << "Email: " << parsed_person.email(i) << std::endl;
  }

  return 0;
}
