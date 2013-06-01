#include "StdAfx.h"
#include "Listener.h"
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
	int intLastError = 0;
	if (intLastError = WSAStartup(WINSOCK_VERSION_REQUEST, &wsaData))
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

	while (true)
	{
		sockaddr_in client_addr;
		SOCKET client_sock;
		int size_client_addr = sizeof(client_addr);
		client_sock = accept(pListener->inSocket, (sockaddr*)&client_addr, &size_client_addr);
		//TODO: Create new worker class in new thread;
		send(client_sock, "HELLO★ПРИВЕТ",sizeof("HELLO★ПРИВЕТ"),0);//TEMP:just for test
	}
}