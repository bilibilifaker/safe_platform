#include <iostream>
using namespace std;

// 定义回调函数类型别名（函数原型）
typedef void* (*callback)(void*);
typedef void* (callback2)(void*);
// 示例函数1：加倍整数
void* doubleInteger(void* arg) {
    int* numPtr = static_cast<int*>(arg);
    *numPtr *= 2;
    return nullptr;
}

// 示例函数2：打印字符串
void* printString(void* arg) {
    const char* str = static_cast<const char*>(arg);
    std::cout << str << std::endl;
    return nullptr;
}

int main() {
    int num = 5;
    const char* message = "Hello, Callback!";
    cout << "callback1:"<<endl;
    // 使用函数原型声明函数
    callback function1 = doubleInteger;
    callback function2 = printString;

    // 调用函数
    function1(&num);
    function2((void*)message);

    std::cout << "Doubled Number: " << num << std::endl;

    callback2* function3 = doubleInteger;
    cout << "callback2:"<<endl;
    function3(&num);


    return 0;
}
