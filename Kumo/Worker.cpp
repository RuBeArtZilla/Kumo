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
		char buf[1024] = {0};
		int msize = recv(W.getClientSocket(), buf, sizeof(buf)-1, 0);
		if (msize , 0) return;
		buf[msize]='\0';

		buf[0] = buf[2];
		buf[1] = buf[3];

		input += buf;

		if (msize < 0) return;
		if (msize == 0) continue;

		wstring wsWide;
		wsWide.resize(input.length(), 0);
		MultiByteToWideChar(
			CP_UTF8, 
			0, 
			&input[0], 
			(int)input.length(), 
			&wsWide[0], 
			(int)wsWide.length());

		wsWide[0] = L'?';

		//TODO: after recv need use p.deshifr();

		//TODO: parse message

		
		//input.find('&');
		//TODO: answer to message
		//if (input.length() > 8){
		send(W.getClientSocket(), input.c_str(), input.length(), 0);
		//}
	}
	//send(W.getClientSocket(), "HELLO★ПРИВЕТ",sizeof("HELLO★ПРИВЕТ"),0);//TEMP:just for test

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
