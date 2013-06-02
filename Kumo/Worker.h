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

struct wsMessageParameter 
{
	wstring name;
	wstring data; 
};

typedef list<wstring> LISTSTR;
typedef list<MessageParameter> LISTMSGPRM;
typedef list<wsMessageParameter> LISTWSMSGPRM;

#define MESSAGE_PARSE_START L"?"
#define MESSAGE_PARSE_SEPARATOR L"&"
#define PARAMETR_PARSE_SEPARATOR L"="
#define PARAMETR_PARSE_BORDER L"\""

LISTWSMSGPRM ParseInputMessage(wstring msg);

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

