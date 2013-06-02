#include "StdAfx.h"
#include "Worker.h"
#include "Protection.h"


Worker::Worker(void)
{
}


Worker::~Worker(void)
{
}

void CreateNewWorker(void * pParams)
{
	Worker W;
	Protection P;
	W.init((WorkerData*)pParams);
	LISTMSGPRM MsgPrmList;
	string input;
	

	//TODO: WORK HERE;

	while (true)
	{
		
		char buf[1024] = {0};//TODO: check buffer size
		int msize = recv(W.getClientSocket(), buf, sizeof(buf)-1, 0);//buf[msize]='\0';
	
		buf[0] = buf[2];
		buf[1] = buf[3];

		input += buf;

		while (msize < 0) 
		{
			if (WSAGetLastError() == WSAEMSGSIZE)
			{
				msize = recv(W.getClientSocket(), buf, sizeof(buf)-1, 0);
				input += buf;
			}
			else
				return;
		}

		if (msize == 0) return;

		//TODO: after recv need use p.encode();

		wstring wsInput;
		wsInput.resize(input.length(), 0);
		MultiByteToWideChar(CP_UTF8, 0, &input[0], (int)input.length(), &wsInput[0], (int)wsInput.length());

		wsInput.erase(0, 1);

		LISTWSMSGPRM lwmp = ParseInputMessage(wsInput);

		Sleep(100);
		//TODO: answer to message
		//if (input.length() > 8){
		//send(W.getClientSocket(), input.c_str(), input.length(), 0);
		//}
	}
	//send(W.getClientSocket(), "HELLO★ПРИВЕТ",sizeof("HELLO★ПРИВЕТ"),0);//TEMP:just for test

}

LISTWSMSGPRM ParseInputMessage(wstring msg)
{
	LISTWSMSGPRM result;
	wstring tmsg = msg;

	int index = tmsg.find(MESSAGE_PARSE_START, 0);
	if (index == std::wstring::npos)	return result;
	tmsg.erase(0, index + 1);

	while(true)
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
		if (index == std::wstring::npos)	return result;
		tmsg.erase(0, index + 1);

		result.push_back(wsMsgPrm);
	}
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

