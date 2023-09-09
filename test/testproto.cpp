//g++ -o proto_test testproto.cpp ../proto/test.pb.cc -I/path/to/protobuf/include -L/path/to/protobuf/lib -pthread -lprotobuf -std=c++11

#include "../proto/test.pb.h"

int main() {
  // ����һ�� Person ��Ϣ����
  Person person;
  person.set_name("John");
  person.set_age(30);
  person.add_email("john@example.com");

  // ����Ϣ���л�Ϊ�ֽ���
  std::string serialized_data;
  person.SerializeToString(&serialized_data);

  // ���ֽ����н�����Ϣ
  Person parsed_person;
  parsed_person.ParseFromString(serialized_data);

  // ������Ϣ�ֶ�
  std::cout << "Name: " << parsed_person.name() << std::endl;
  std::cout << "Age: " << parsed_person.age() << std::endl;
  for (int i = 0; i < parsed_person.email_size(); ++i) {
    std::cout << "Email: " << parsed_person.email(i) << std::endl;
  }

  return 0;
}
