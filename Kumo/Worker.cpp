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
	setlocale(LC_CTYPE,"C-UTF-8");
	W.init((WorkerData*)pParams);
	LISTMSGPRM MsgPrmList;
	string input;

	for(;;)
	{
		char buf[DEFAULT_BUFFER_SIZE] = {0};// TODO: check buffer size
		int msize = recv(W.getClientSocket(), buf, sizeof(buf)-1, 0);//buf[msize]='\0';
		if (msize <= 0) return;

		char size[2] = {buf[0], buf[1]}; // TODO: use this size of message!

		buf[0] = buf[2];
		buf[1] = buf[3];

		input += buf;

		//TODO: Reapair this ↓
		//WARNING: if "original" message lenght from client larger 
		//DEFAULT_BUFFER_SIZE then message's will be broken

		//TODO: after recv need use p.encode();
		
		if (input.length() < 1) continue;
		wstring wsInput;
		wsInput.resize(input.length(), 0);
		MultiByteToWideChar(CP_UTF8, 0, &input[0], (int)input.length(), &wsInput[0], (int)wsInput.length());
		wsInput.erase(0, 1);

		LISTWSMSGPRM lwmp = ParseInputMessage(wsInput);

		int iMsg = CheckMessage(&lwmp);

		switch (iMsg)
		{
		case MESSAGE_AUTHORISATION_ID:
			W.Authorisation(&lwmp);return;
			break;
		case MESSAGE_DEVICE_INFO_ID:
			W.DeviceInfo(&lwmp);return;
			break;
		case MESSAGE_DIRECTORY_REQUEST_ID:
			W.DirectoryRequest(&lwmp);return;
			break;
		case MESSAGE_FILE_REQUEST_ID:
			W.FileRequest(&lwmp);return;
			break;
		default: 
			{
				W.SendErrorMessage(MESSAGE_ERROR_CODE_BAD_MESSAGE);
				break;;
			}
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
		wstring inputPasswordHash = 
			Protection::getHash((void *)password.c_str(), sizeof WCHAR * password.length());
		wstring dbPasswordHash = kumo_db::getUserPassHash(login);

		if (!dbPasswordHash.compare(inputPasswordHash))
		{
			user = login;
			LISTWSMSGPRM newMessage;

			newMessage.push_back(createParam(MESSAGE_THEME, MESSAGE_AUTHORISATION));
			newMessage.push_back(createParam(MESSAGE_TIME, std::to_wstring((_Longlong)session)));
			newMessage.push_back(createParam(MESSAGE_SESSION, std::to_wstring((_Longlong)session)));

			return SendMessage(&newMessage);
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
		return SendDirectory(dir, filenames);
	}

	return -1;
}

int Worker::FileRequest(LISTWSMSGPRM *msg)
{
	//TODO: FileRequest
	return 0;
}

int Worker::SendErrorMessage(wstring msg)
{
	LISTWSMSGPRM newMessage;
	newMessage.push_back(createParam(MESSAGE_THEME, MESSAGE_ERROR));
	newMessage.push_back(createParam(MESSAGE_TIME, std::to_wstring((_Longlong)session)));
	if (user != MESSAGE_PARAMETER_NOT_FOUND) 
		newMessage.push_back(createParam(MESSAGE_SESSION, std::to_wstring((_Longlong)session)));
	newMessage.push_back(createParam(MESSAGE_ERROR_CODE, msg));
	return SendMessage(&newMessage);
}


int Worker::SendDirectory(std::wstring path, WSVECTOR data)
{
	std::wstring wsData;
	WSVECTOR_ITERATOR iterator = data.begin();

	while(iterator != data.end())
	{
		wsData += iterator->c_str();
		iterator++;
		if ((iterator != data.end())) 
			wsData += MESSAGE_DIRECTORY_REQUEST_LIST_SEPARATOR;
	}

	LISTWSMSGPRM newMessage;

	newMessage.push_back(createParam(MESSAGE_THEME, MESSAGE_DIRECTORY_REQUEST));
	newMessage.push_back(createParam(MESSAGE_TIME, std::to_wstring((_Longlong)session)));
	newMessage.push_back(createParam(MESSAGE_SESSION, std::to_wstring((_Longlong)session)));
	newMessage.push_back(createParam(MESSAGE_DIRECTORY_REQUEST_PATH, path));
	newMessage.push_back(createParam(MESSAGE_DIRECTORY_REQUEST_LIST, wsData));

	return SendMessage(&newMessage);
}


int Worker::SendMessage(LISTWSMSGPRM * msg)
{
	LISTWSMSGPRM_ITERATOR iterator = msg->begin();
	std::wstring wsMessage = MESSAGE_PARSE_START;

	while(iterator != msg->end())
	{
		wsMessage += std::wstring(iterator->name) + 
			std::wstring(PARAMETR_PARSE_SEPARATOR) +  
			std::wstring(PARAMETR_PARSE_BORDER) +
			std::wstring(iterator->data) + 
			std::wstring(PARAMETR_PARSE_BORDER);
		iterator++;
		if (iterator!= msg->end())
			wsMessage += MESSAGE_PARSE_SEPARATOR;
	}
	
	short size = wsMessage.length() * (sizeof WCHAR);
	short size2 = wsMessage.length();
	char* bMessage = static_cast<char*>(malloc(size + 2));

	if (bMessage != NULL)
	{
		ZeroMemory(bMessage, size + 2);
		char * pData = &bMessage[2];
		memcpy(pData, wsMessage.c_str(), size);
		bMessage[0] = ((char*)&size2)[1]; 
		bMessage[1] = ((char*)&size2)[0];
		send(client, bMessage, size + 2, 0);
		free(bMessage);
	}
	closesocket(client);
	_endthread();
	return 0;
}


wsMessageParameter Worker::createParam(wstring name, wstring data)
{
	wsMessageParameter tmp = {name, data};
	return tmp;
}
