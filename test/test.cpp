//g++ -o test test.cpp ../proto/person.pb.cc ../proto/info.pb.cc -I/path/to/protobuf/include -L/path/to/protobuf/lib -pthread -lprotobuf -std=c++11

#include <iostream>
#include "../proto/person.pb.h"

using namespace std;

// message Person
// {
//     int32 id = 1;
//     string name = 2;
//     string sex = 3;
//     int32 age = 4;
// }

int main(){
    // 1. ����person���󣬲���ʼ��
    itcast::Person person;
    person.set_id(1001);
    // ����һ���ڴ�,�洢����
    person.add_name();
    person.set_name(0,"ineri");
    person.add_name();
    person.set_name(1,"and");
    person.add_name();
    person.set_name(2,"harmless");
    person.set_sex("woman");
    person.set_age(17);
    person.set_color(itcast::Blue);
    itheima::Info* info = person.mutable_info();
    info->set_address("peiking changping qimingxing building");
    info->set_number(911);

    // 2. ��person�������л� -> �ַ���
    string output;
    person.SerializeToString(&output);
    cout << "���л��������:" << output << endl;
    
    // 3. ���ݴ���
    // transfer()...

    // 4. �������ݣ����� -> ���� -> ԭʼ����
    itcast::Person result;
    result.ParseFromString(output);

    // 5. ����ԭʼ���� -> ��ӡ������Ϣ
    cout << "id: " << result.id() << endl;
    cout << "sex: " << result.sex() << endl;
    cout << "name: " << result.name(0) << " " <<  result.name(1) << " " << result.name(2) <<endl;
    cout << "age: " << result.age() << endl;
    cout << "color: " << result.color() << endl;
    itheima::Info result_info = result.info();
    cout << "address:" << result_info.address() << endl;
    cout << "number:" << result_info.number() << endl;
    return 0;
}