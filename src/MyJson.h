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
            // ��value�����ʽ��
            string json = my_json.toStyledString();

            //FastWriter w;
            //json = w.write(root);
            // ������һ�о���һ���� û�пո��з�ɶ����

            // д�����ļ�
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