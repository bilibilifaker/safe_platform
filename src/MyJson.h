#pragma once
#include <iostream>
#include <json/json.h>
#include <fstream>
using namespace std;
using namespace Json;
class MyJson{

    private:
        string filename;
        Value my_json;
    public:
        MyJson(string name = "test.json"){
            filename = name;
        }

        template<typename T>
        void append(T t){
            my_json.append(t);
        }

        int write_json(){
            // 将value对象格式化
            string json = my_json.toStyledString();

            //FastWriter w;
            //json = w.write(root);
            // 加上这一行就是一行字 没有空格换行符啥的了

            // 写磁盘文件
            ofstream ofs(filename);
            ofs << json;
            ofs.close();
            return 0;
        }

        Value read_json(){
            ifstream ifs(filename);
            Reader r;
            r.parse(ifs, my_json);
            return my_json;
        }

        ~MyJson(){}
};