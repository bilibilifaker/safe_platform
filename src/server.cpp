#include <iostream>
#include "server.h"
using namespace std;

int main(){
    Server server("server.json");
    server.start_server();
    return 0;
}