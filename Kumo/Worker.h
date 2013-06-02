#include "Protection.h"

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
typedef list<wsMessageParameter>::iterator LISTWSMSGPRM_ITERATOR;

#define DEFAULT_BUFFER_SIZE 4096

#define MESSAGE_PARSE_START L"?"
#define MESSAGE_PARSE_SEPARATOR L"&"
#define PARAMETR_PARSE_SEPARATOR L"="
#define PARAMETR_PARSE_BORDER L"\""

#define MESSAGE_THEME L"msg"
#define MESSAGE_AUTHORISATION L"auth"
#define MESSAGE_AUTHORISATION_LOGIN L"login"
#define MESSAGE_AUTHORISATION_PASSWORD L"pass"

#define MESSAGE_DEVICE_INFO L"info"
#define MESSAGE_DIRECTORY_REQUEST L"dir"
#define MESSAGE_FILE_REQUEST L"file"

#define MESSAGE_AUTHORISATION_ID 1
#define MESSAGE_DEVICE_INFO_ID 2
#define MESSAGE_DIRECTORY_REQUEST_ID 3
#define MESSAGE_FILE_REQUEST_ID 4

LISTWSMSGPRM ParseInputMessage(wstring msg);
int CheckMessage(LISTWSMSGPRM * input);
wstring wsFindParameter(LISTWSMSGPRM * input);

void CreateNewWorker(void * pParams);

class Worker
{
public:
	Worker(void);
	~Worker(void);
protected:
	Protection P;
	SOCKET client;
	sockaddr_in addr;
public:
	int init(WorkerData *wd);
	SOCKET getClientSocket(void);
	int Authorisation(LISTWSMSGPRM *msg);
	int DeviceInfo(LISTWSMSGPRM *msg);
	int DirectoryRequest(LISTWSMSGPRM *msg);
	int FileRequest(LISTWSMSGPRM *msg);
	void SendErrorMessage(wstring msg);
};

