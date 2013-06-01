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
	
	//TODO: WORK HERE;

	while (true)
	{
		char buf[1024];
		int msize = recv(W.getClientSocket(), buf, sizeof(buf)-1, 0);
		buf[msize]='\0';

		//TODO: after recv need use p.deshifr();

		//TODO: parse message

		//TODO: answer to message

		send(W.getClientSocket(), buf, msize, 0);
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
