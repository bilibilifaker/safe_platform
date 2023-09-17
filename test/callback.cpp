#include <iostream>
using namespace std;

// ����ص��������ͱ���������ԭ�ͣ�
typedef void* (*callback)(void*);
typedef void* (callback2)(void*);
// ʾ������1���ӱ�����
void* doubleInteger(void* arg) {
    int* numPtr = static_cast<int*>(arg);
    *numPtr *= 2;
    return nullptr;
}

// ʾ������2����ӡ�ַ���
void* printString(void* arg) {
    const char* str = static_cast<const char*>(arg);
    std::cout << str << std::endl;
    return nullptr;
}

int main() {
    int num = 5;
    const char* message = "Hello, Callback!";
    cout << "callback1:"<<endl;
    // ʹ�ú���ԭ����������
    callback function1 = doubleInteger;
    callback function2 = printString;

    // ���ú���
    function1(&num);
    function2((void*)message);

    std::cout << "Doubled Number: " << num << std::endl;

    callback2* function3 = doubleInteger;
    cout << "callback2:"<<endl;
    function3(&num);


    return 0;
}
