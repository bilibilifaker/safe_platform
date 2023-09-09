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
	int status;		// ��Կ״̬: 1����, 0:������
	int seckeyID;	// ��Կ�ı��
	char clientID[12];	// �ͻ���ID, �ͻ��˵ı�ʶ
	char serverID[12];	// ������ID, ��������ʶ
	char seckey[128];	// �ԳƼ��ܵ���Կ
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
            // û��ָ��size��Ĭ��4096
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
            // û��ָ��size��Ĭ��4096
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
	        // ���������ڴ�
	        NodeSHMInfo *pAddr = static_cast<NodeSHMInfo*>(m_shmAddr);
	        // pAddr = static_cast<NodeSHMInfo*>(mapShm());

	        //����������Ϣ
	        int i = 0;
	        NodeSHMInfo	*pNode = NULL;
	        // ͨ��clientID��serverID���ҽڵ�
	        for (i = 0; i < m_maxNode; i++){
		        pNode = pAddr + i;
		        if (strcmp(pNode->clientID, pNodeInfo->clientID) == 0 &&
		        	strcmp(pNode->serverID, pNodeInfo->serverID) == 0){
			        // �ҵ��Ľڵ���Ϣ, ��������������
                    memcpy(pNode->seckey, pNodeInfo->seckey, sizeof(pNode->seckey));
                    pNode->seckeyID = pNodeInfo->seckeyID;
                    pNode->status = pNodeInfo->status;
			        return 1;
		        }
	        }
            // ��û���ҵ���Ӧ����Ϣ, ��һ���սڵ㽫��Կ��Ϣд��
	        NodeSHMInfo tmpNodeInfo; //�ս��
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
	        // ���������ڴ�
	        NodeSHMInfo *pAddr = static_cast<NodeSHMInfo*>(m_shmAddr);
	        pAddr = static_cast<NodeSHMInfo*>(mapShm());

	        //����������Ϣ
	        int i = 0;
	        NodeSHMInfo	*pNode;;
	        // ͨ��clientID��serverID���ҽڵ�
	        for (i = 0; i < m_maxNode; i++){
		        pNode = pAddr + i;
		        if (strcmp(pNode->clientID, clientID.data()) == 0 &&
		        	strcmp(pNode->serverID, serverID.data()) == 0){
			        // �ҵ��Ľڵ���Ϣ, ��������������
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