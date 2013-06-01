#pragma once

struct WorkerData
{
	sockaddr_in addr;
	SOCKET sock;
};

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

