// look : ipcs -m 
// delete : ipcrm -M/m shmkey/shmid

//g++ -o testseckeyshm1 testseckeyshm1.cpp -std=c++11
#include <cstdio>
#include <iostream>
#include <string.h>
#include "SecKeyShm.h"
using namespace std;

void readShm();
void writeShm();
int main()
{
	cout << "please enter your need: " << endl;
	cout << "1. write memory, 2. read memory" << endl;
	int sel = 0;
	cin >> sel;
	writeShm();
	readShm();
	cout << "bye bye!!!" << endl;

	return 0;
}

void writeShm()
{
	NodeSHMInfo info;
    strcpy(info.clientID, "kevin");
    strcpy(info.serverID, "Alice");
    strcpy(info.seckey, "i am god...");
	info.seckeyID = 100;
	info.status = 1;
	SecKeyShm shm("/home", 4096);
	shm.shmInit();
	shm.shmWrite(&info);


    strcpy(info.clientID, "Bob");
    strcpy(info.serverID, "Jack");
    strcpy(info.seckey, "god is amend - donald!!!");
	info.seckeyID = 666;
	info.status = 1;
	shm.shmWrite(&info);
}

void readShm()
{
	char tmp[128];
	strcpy(tmp, "Bob");
	SecKeyShm shm("/home", 4096);
	NodeSHMInfo* info = shm.shmRead(tmp, "Jack");
	cout << "ClientID: " << info->clientID << endl;
	cout << "ServerID: " << info->serverID << endl;
	cout << "SecKey: " << info->seckey << endl;
	cout << "seckeyID: " << info->seckeyID << endl;
	cout << "status: " << info->status << endl<<endl;

    strcpy(tmp, "kevin");
	info = shm.shmRead(tmp, "Alice");
	cout << "ClientID: " << info->clientID << endl;
	cout << "ServerID: " << info->serverID << endl;
	cout << "SecKey: " << info->seckey << endl;
	cout << "seckeyID: " << info->seckeyID << endl;
	cout << "status: " << info->status << endl;
	// shm.delShm();
}