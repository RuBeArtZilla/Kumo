#include "StdAfx.h"
#include "Worker.h"

Worker::Worker(void)
{
	session = rand();
	user = MESSAGE_PARAMETER_NOT_FOUND;
}


Worker::~Worker(void)
{
}

void CreateNewWorker(void * pParams)
{
	Worker W;
	W.init((WorkerData*)pParams);
	LISTMSGPRM MsgPrmList;
	string input;

	for(;;)
	{
		char buf[DEFAULT_BUFFER_SIZE] = {0};//TODO: check buffer size
		int msize = recv(W.getClientSocket(), buf, sizeof(buf)-1, 0);//buf[msize]='\0';
		
		send(W.getClientSocket(), (char *)L"ТЫ ДНО", sizeof WCHAR * 6, 0);

		buf[0] = buf[2];
		buf[1] = buf[3];

		input += buf;

		//TODO: Reapair this ↓
		//WARNING: if "original" message lenght from client larger DEFAULT_BUFFER_SIZE then message's will be broken

		if (msize < 0) return;

		//TODO: after recv need use p.encode();
		
		if (input.length() < 1) continue;
		wstring wsInput;
		wsInput.resize(input.length(), 0);
		MultiByteToWideChar(CP_UTF8, 0, &input[0], (int)input.length(), &wsInput[0], (int)wsInput.length());
		wsInput.erase(0, 1);

		LISTWSMSGPRM lwmp = ParseInputMessage(wsInput);

		//TODO: answer to message

		int iMsg = CheckMessage(&lwmp);

		switch (iMsg)
		{
		case MESSAGE_AUTHORISATION_ID:
			W.Authorisation(&lwmp);
		case MESSAGE_DEVICE_INFO_ID:
			W.DeviceInfo(&lwmp);
		case MESSAGE_DIRECTORY_REQUEST_ID:
			W.DirectoryRequest(&lwmp);
		case MESSAGE_FILE_REQUEST_ID:
			W.FileRequest(&lwmp);
		default: continue;
		}
	}
}

LISTWSMSGPRM ParseInputMessage(wstring msg)
{
	LISTWSMSGPRM result;
	wstring tmsg = msg;

	int index = tmsg.find(MESSAGE_PARSE_START, 0);
	if (index == std::wstring::npos)	return result;
	tmsg.erase(0, index + 1);

	for(;;)
	{
		wsMessageParameter wsMsgPrm;

		index = tmsg.find(PARAMETR_PARSE_SEPARATOR, 0);
		if (index == std::wstring::npos)	return result;
		wsMsgPrm.name = tmsg.substr(0, index);
		tmsg.erase(0, index + 1);

		index = tmsg.find(PARAMETR_PARSE_BORDER, 0);
		if (index == std::wstring::npos)	return result;
		tmsg.erase(0, index + 1);

		index = tmsg.find(PARAMETR_PARSE_BORDER, 0);
		if (index == std::wstring::npos)	return result;
		wsMsgPrm.data = tmsg.substr(0, index);
		tmsg.erase(0, index + 1);

		index = tmsg.find(MESSAGE_PARSE_SEPARATOR, 0);
		if (index != std::wstring::npos)	tmsg.erase(0, index + 1);

		result.push_back(wsMsgPrm);
	}
}

wstring wsFindParameter(LISTWSMSGPRM * source, wstring find)
{
	LISTWSMSGPRM_ITERATOR iterator = source->begin();
	while(iterator != source->end())
	{
		if (!iterator->name.compare(find)) 
			return iterator->data;
		iterator++;
	}
	return MESSAGE_PARAMETER_NOT_FOUND;
}

int CheckMessage(LISTWSMSGPRM * input)
{
	wstring result = wsFindParameter(input, MESSAGE_THEME);
	if (result == L"") return -1;

	//TODO: check message time!

	if (!result.compare(MESSAGE_AUTHORISATION)) return MESSAGE_AUTHORISATION_ID;
	if (!result.compare(MESSAGE_DEVICE_INFO)) return MESSAGE_DEVICE_INFO_ID;
	if (!result.compare(MESSAGE_DIRECTORY_REQUEST)) return MESSAGE_DIRECTORY_REQUEST_ID;
	if (!result.compare(MESSAGE_FILE_REQUEST)) return MESSAGE_FILE_REQUEST_ID;

	return 0;
}

int Worker::init(WorkerData *wd)
{
	client = wd->sock;
	addr = wd->addr;
	return 0;
}


SOCKET Worker::getClientSocket(void)
{
	return client;
}

int Worker::Authorisation(LISTWSMSGPRM *msg)
{
	wstring login = wsFindParameter(msg, MESSAGE_AUTHORISATION_LOGIN);
	wstring password = wsFindParameter(msg, MESSAGE_AUTHORISATION_PASSWORD);
	if ((login != MESSAGE_PARAMETER_NOT_FOUND)&&(password != MESSAGE_PARAMETER_NOT_FOUND)) 
	{
		wstring inputPasswordHash = Protection::getHash((void *)password.c_str(), sizeof WCHAR * password.length());
		wstring dbPasswordHash = kumo_db::getUserPassHash(login);

		if (!dbPasswordHash.compare(inputPasswordHash))
		{
			user = login;
		}
		else SendErrorMessage(MESSAGE_ERROR_CODE_BAD_AUTHORISATION);
	}
	return 0;
}

int Worker::DeviceInfo(LISTWSMSGPRM *msg)
{
	//TODO: DeviceInfo
	return 0;
}

int Worker::DirectoryRequest(LISTWSMSGPRM *msg)
{
	wstring dir = wsFindParameter(msg, MESSAGE_DIRECTORY_REQUEST_PATH);

	if (dir != MESSAGE_PARAMETER_NOT_FOUND)
	{
		WSVECTOR filenames = kumo_db::getDir(dir);

		std::wstring wsMessage;


		wsMessage += MESSAGE_PARSE_START + 
			std::wstring(MESSAGE_THEME) + 
			std::wstring(PARAMETR_PARSE_SEPARATOR) +  
			std::wstring(PARAMETR_PARSE_BORDER) +
			std::wstring(MESSAGE_DIRECTORY_REQUEST) + 
			std::wstring(PARAMETR_PARSE_BORDER) +

			std::wstring(MESSAGE_PARSE_SEPARATOR) +
			std::wstring(MESSAGE_TIME) +
			std::wstring(PARAMETR_PARSE_SEPARATOR) +  
			std::wstring(PARAMETR_PARSE_BORDER) +
			std::to_wstring((_Longlong)session) + //TODO: change to current time
			std::wstring(PARAMETR_PARSE_BORDER) +

			std::wstring(MESSAGE_PARSE_SEPARATOR) +
			std::wstring(MESSAGE_SESSION) +
			std::wstring(PARAMETR_PARSE_SEPARATOR) +  
			std::wstring(PARAMETR_PARSE_BORDER) +
			std::to_wstring((_Longlong)session) +
			std::wstring(PARAMETR_PARSE_BORDER) +

			std::wstring(MESSAGE_PARSE_SEPARATOR) +
			std::wstring(MESSAGE_DIRECTORY_REQUEST_PATH) +
			std::wstring(PARAMETR_PARSE_SEPARATOR) +  
			std::wstring(PARAMETR_PARSE_BORDER) +
			dir +
			std::wstring(PARAMETR_PARSE_BORDER) +
			
			std::wstring(MESSAGE_PARSE_SEPARATOR) +
			std::wstring(MESSAGE_DIRECTORY_REQUEST_LIST) +
			std::wstring(PARAMETR_PARSE_SEPARATOR) +  
			std::wstring(PARAMETR_PARSE_BORDER);

		WSVECTOR_ITERATOR iterator = filenames.begin();
		while(iterator != filenames.end())
		{
			wsMessage += iterator->c_str();
			iterator++;
			if ((iterator != filenames.end())) 
				wsMessage += MESSAGE_DIRECTORY_REQUEST_LIST_SEPARATOR;
		}
		wsMessage += PARAMETR_PARSE_BORDER;

		send(client, (char *)&wsMessage, sizeof WCHAR * wsMessage.length(), 0);

	}

	return 0;
}

int Worker::FileRequest(LISTWSMSGPRM *msg)
{
	//TODO: FileRequest
	return 0;
}

void Worker::SendErrorMessage(wstring msg)
{
	send(client, (char *) msg.c_str(), msg.length() * sizeof (wchar_t), 0);
}


int Worker::SendDirectory(std::wstring path, WSVECTOR data)
{
	return 0;
}
