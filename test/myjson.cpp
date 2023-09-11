#include <iostream>
#include <json/json.h>
#include <fstream>
using namespace std;
using namespace Json;

void writejson(){
    Value root;
    root.append(12);
    root.append(13.45);
    root.append("hello,world");
    root.append(true);
    root.append(false);
    Value sub;
    sub.append(12);
    sub.append(13.45);
    sub.append("hello,world");
    root.append(sub);
    Value obj;
    obj["a"] = "b";
    root.append(obj);

    // 将value对象格式化
    string json = root.toStyledString();

    FastWriter w;
    json = w.write(root);
    // 加上这一行就是一行字 没有空格换行符啥的了

    // 写磁盘文件
    ofstream ofs("test.json");
    ofs << json;
    ofs.close();

}

void readjson(){
    ifstream ifs("test.json");
    Reader r;
    Value root;
    r.parse(ifs, root);
    if(root.isArray()){
        for(int i = 0;i<root.size();i++){
            Value sub = root[i];
            if(sub.isInt()){
                cout << sub.asInt() << endl;
            }
            if(sub.isString()){
                cout << sub.asString() << endl;
            }
            if(sub.isDouble()){
                cout << sub.asDouble() << endl;
            }
            if(sub.isBool()){
                cout << sub.asBool() << endl;
            }
            if(sub.isObject()){
                cout << sub["a"].asString() << endl;
            }
        }
    }
}

int main(){
    writejson();
    readjson();
}