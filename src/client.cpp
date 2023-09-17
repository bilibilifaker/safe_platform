#include <iostream>
#include "client.h"
using namespace std;

int main(){
    Client client("client.json");
    cout << "press 1 : Secret key negotiate" << endl;
    cout << "press 2 : Secret key vertify" << endl;
    cout << "press 3 : Secret key sign out" << endl;
    cout << "press 0 : Quit" << endl;
    int choose = 1;
    while( choose ){
        cin >> choose;
        switch (choose)
        {
        case 1:
            client.secret_key_negotiate();            
            break;
        case 2:
            client.secret_key_vertify();            
            break;
        case 3:
            client.secret_key_sign_out();            
            break;
        default:
            break;
        }
    }
    cout << "welcome use next time,bye!" << endl;
}