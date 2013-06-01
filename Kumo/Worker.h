#pragma once
using namespace std;
struct WorkerData
{
	sockaddr_in addr;
	SOCKET sock;
};

struct MessageParameter 
{
	char * name;
	char * data; 
};

typedef list<wstring> LISTSTR;
typedef list<MessageParameter> LISTMSGPRM;

void CreateNewWorker(void * pParams);

class Worker
{
public:
	Worker(void);
	~Worker(void);
protected:
	SOCKET client;
	sockaddr_in addr;
public:
	int init(WorkerData *wd);
	SOCKET getClientSocket(void);
};

