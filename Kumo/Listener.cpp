#include "StdAfx.h"
#include "Listener.h"
#include "Worker.h"


Listener::Listener(void)
{
	inSocket = INVALID_SOCKET;
	port = DEFAULT_PORT;
}

Listener::~Listener(void)
{
	WSACleanup();// here ? TODO: TEST IT
}

int Listener::start(void)
{
	int intLastError = WSAStartup(WINSOCK_VERSION_REQUEST, &wsaData);
	if (intLastError)
	{
		return intLastError;
	}

	inSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (inSocket == INVALID_SOCKET)
	{
		return WSAGetLastError();
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.S_un.S_addr = 0;

	intLastError = bind(inSocket, (sockaddr*) & server_addr, sizeof (server_addr));

	if (intLastError == SOCKET_ERROR)
	{
		intLastError = WSAGetLastError();
		closesocket(inSocket);
		WSACleanup(); // use here? TODO: TEST IT
		return intLastError;
	}

	if (listen(inSocket, SOMAXCONN) == SOCKET_ERROR)
		return WSAGetLastError();

	hThread = (HANDLE)_beginthread( start_listen, 0, (void*) this );

	return 0;
}


int Listener::stop(void)
{
	TerminateThread(hThread, 0);//not dangerous, because thread just accepting incoming connections
	closesocket(inSocket);
	WSACleanup();// ???
	return 0;
}


int Listener::restart(void)
{
	return 0;
}


int Listener::setPort(WORD new_port)
{
	return port = new_port;
}


WORD Listener::getPort(void)
{
	return port;
}


void start_listen(void*  pParams)
{
	Listener *pListener = (Listener*)pParams;

	for(;;)
	{
		WorkerData WD;
		int size_client_addr = sizeof(WD.addr);
		WD.sock = accept(pListener->inSocket, (sockaddr*)&WD.addr, &size_client_addr);
		/*HANDLE hNewThread = (HANDLE)*/_beginthread( CreateNewWorker, 0, (void*) &WD ); //TODO: use this hadle later
	}
}