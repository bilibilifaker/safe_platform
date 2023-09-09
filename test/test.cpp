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
    // 1. 创建person对象，并初始化
    itcast::Person person;
    person.set_id(1001);
    // 申请一块内存,存储名字
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

    // 2. 将person对象序列化 -> 字符串
    string output;
    person.SerializeToString(&output);
    cout << "序列化后的数据:" << output << endl;
    
    // 3. 数据传输
    // transfer()...

    // 4. 接收数据，解析 -> 解码 -> 原始数据
    itcast::Person result;
    result.ParseFromString(output);

    // 5. 处理原始数据 -> 打印数据信息
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