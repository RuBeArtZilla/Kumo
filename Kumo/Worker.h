#include "Protection.h"
#include "kumo_db.h"

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

typedef std::vector<std::wstring> WSVECTOR;
typedef std::vector<std::wstring>::iterator WSVECTOR_ITERATOR;

#define DEFAULT_BUFFER_SIZE 4096

#define MESSAGE_PARSE_START L"?"
#define MESSAGE_PARSE_SEPARATOR L"&"
#define PARAMETR_PARSE_SEPARATOR L"="
#define PARAMETR_PARSE_BORDER L"\""

#define MESSAGE_PARAMETER_NOT_FOUND L""

#define MESSAGE_THEME L"msg"
#define MESSAGE_SESSION L"session"
#define MESSAGE_TIME L"time"
#define MESSAGE_ERROR L"error"

#define MESSAGE_ERROR_CODE L"code"
#define MESSAGE_ERROR_CODE_BAD_AUTHORISATION L"303"
#define MESSAGE_ERROR_CODE_BAD_MESSAGE L"304"

#define MESSAGE_AUTHORISATION L"auth"
#define MESSAGE_AUTHORISATION_LOGIN L"login"
#define MESSAGE_AUTHORISATION_PASSWORD L"pass"

#define MESSAGE_DEVICE_INFO L"info"


#define MESSAGE_DIRECTORY_REQUEST L"dir"
#define MESSAGE_DIRECTORY_REQUEST_PATH L"path"
#define MESSAGE_DIRECTORY_REQUEST_LIST L"data"
#define MESSAGE_DIRECTORY_REQUEST_LIST_SEPARATOR L", "

#define MESSAGE_FILE_REQUEST L"file"
#define MESSAGE_FILE_REQUEST_PATH L"path"
#define MESSAGE_FILE_REQUEST_NAME L"name"

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
	unsigned int session;
	wstring user;
public:
	int init(WorkerData *wd);
	SOCKET getClientSocket(void);
	int Authorisation(LISTWSMSGPRM *msg);
	int DeviceInfo(LISTWSMSGPRM *msg);
	int DirectoryRequest(LISTWSMSGPRM *msg);
	int FileRequest(LISTWSMSGPRM *msg);
	int SendErrorMessage(wstring msg);
	int SendDirectory(std::wstring path, WSVECTOR data);
	int SendMessage(LISTWSMSGPRM * msg);
	wsMessageParameter createParam(wstring name, wstring data);
};

