#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>

using namespace std;

class NodeSHMInfo
{
public:
	NodeSHMInfo() : status(0), seckeyID(0)
	{
		bzero(clientID, sizeof(clientID));
		bzero(serverID, sizeof(serverID));
		bzero(seckey, sizeof(seckey));
	}
	int status;		// 秘钥状态: 1可用, 0:不可用
	int seckeyID;	// 秘钥的编号
	char clientID[12];	// 客户端ID, 客户端的标识
	char serverID[12];	// 服务器ID, 服务器标识
	char seckey[128];	// 对称加密的秘钥
};

class BaseShm{
    protected:
        int m_shmID;
        void* m_shmAddr;

    public:
        BaseShm(){};

        BaseShm(int key){};

        BaseShm(int key, int size){};

        BaseShm(string name){};

        BaseShm(string name, int size){};

        virtual void* mapShm(){};

        virtual int unmapShm(){};

        virtual int delShm(){};

        ~BaseShm(){};

    private:
        virtual int getShmID(key_t key, int shmSize, int flag){};
};

class SecKeyShm : public BaseShm{
    private:
        int m_maxNode;

    public:
        SecKeyShm(){
            int key = 114514;
            int size = 4096;
            m_maxNode = size/sizeof(NodeSHMInfo);
            m_shmID = shmget(key, size, IPC_CREAT|0664);
            mapShm();
        };
    
        SecKeyShm(int key){
            int size = 4096;
            m_maxNode = size/sizeof(NodeSHMInfo);
            // 没有指定size则默认4096
            m_shmID = shmget(key, size, IPC_CREAT|0664);
            mapShm();
        };

        SecKeyShm(int key, int size){
            m_maxNode = size/sizeof(NodeSHMInfo);
            m_shmID = shmget(key, size, IPC_CREAT|0664);
            mapShm();
        };

        SecKeyShm(string name){
            int size = 4096;
            m_maxNode = size/sizeof(NodeSHMInfo);
            key_t key = ftok(name.c_str(), 100);
            // 没有指定size则默认4096
            m_shmID = shmget(key, size, IPC_CREAT|0664);
            mapShm();
        };

        SecKeyShm(string name, int size){
            m_maxNode = size/sizeof(NodeSHMInfo);
            key_t key = ftok(name.c_str(), 100);
            m_shmID = shmget(key, size, IPC_CREAT|0664);
            mapShm();
        };
        
        void* mapShm(){
            m_shmAddr = shmat(m_shmID, nullptr, 0);
            return m_shmAddr;
        };

        int unmapShm(){
            int result = shmdt(m_shmAddr);
            return result;
        };

        int delShm(){
            shmctl(m_shmID, IPC_RMID, nullptr);
        };

        int shmWrite(NodeSHMInfo* pNodeInfo){
	        // 关联共享内存
	        NodeSHMInfo *pAddr = static_cast<NodeSHMInfo*>(m_shmAddr);
	        // pAddr = static_cast<NodeSHMInfo*>(mapShm());

	        //遍历网点信息
	        int i = 0;
	        NodeSHMInfo	*pNode = NULL;
	        // 通过clientID和serverID查找节点
	        for (i = 0; i < m_maxNode; i++){
		        pNode = pAddr + i;
		        if (strcmp(pNode->clientID, pNodeInfo->clientID) == 0 &&
		        	strcmp(pNode->serverID, pNodeInfo->serverID) == 0){
			        // 找到的节点信息, 拷贝到传出参数
                    memcpy(pNode->seckey, pNodeInfo->seckey, sizeof(pNode->seckey));
                    pNode->seckeyID = pNodeInfo->seckeyID;
                    pNode->status = pNodeInfo->status;
			        return 1;
		        }
	        }
            // 若没有找到对应的信息, 找一个空节点将秘钥信息写入
	        NodeSHMInfo tmpNodeInfo; //空结点
	        for (i = 0; i < m_maxNode; i++){
		        pNode = pAddr + i;
		        if (memcmp(&tmpNodeInfo, pNode, sizeof(NodeSHMInfo)) == 0){
			        memcpy(pNode, pNodeInfo, sizeof(NodeSHMInfo));
			        break;
		        }
	        }
	        return 1;
        };

        NodeSHMInfo* shmRead(string clientID, string serverID){
            int ret = 0;
	        // 关联共享内存
	        NodeSHMInfo *pAddr = static_cast<NodeSHMInfo*>(m_shmAddr);
	        pAddr = static_cast<NodeSHMInfo*>(mapShm());

	        //遍历网点信息
	        int i = 0;
	        NodeSHMInfo	*pNode;;
	        // 通过clientID和serverID查找节点
	        for (i = 0; i < m_maxNode; i++){
		        pNode = pAddr + i;
		        if (strcmp(pNode->clientID, clientID.data()) == 0 &&
		        	strcmp(pNode->serverID, serverID.data()) == 0){
			        // 找到的节点信息, 拷贝到传出参数
			        NodeSHMInfo* res = new NodeSHMInfo;
                    memcpy(res->seckey, pNode->seckey, sizeof(pNode->seckey));
                    res->seckeyID = pNode->seckeyID;
                    res->status = pNode->status;
                    memcpy(res->clientID, pNode->clientID, sizeof(pNode->clientID));
                    memcpy(res->serverID, pNode->serverID, sizeof(pNode->serverID));
			        return res;
		        }
	        }
	        unmapShm();
	        return nullptr;
        };
        
        void shmInit(){
	        if (m_shmAddr != NULL){
        		memset(m_shmAddr, 0, m_maxNode * sizeof(NodeSHMInfo));
	        }
        }

        ~SecKeyShm(){};
};