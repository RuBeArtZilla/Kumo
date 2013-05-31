#include "StdAfx.h"
#include "Listener.h"

Listener::Listener(void)
{
	inSocket = INVALID_SOCKET;
	port = DEFAULT_PORT;
}

Listener::~Listener(void)
{
	WSACleanup();// here ?
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

	intLastError = bind(inSocket, (sockaddr*)&server_addr, sizeof(server_addr));
	
	return 0;
}


int Listener::stop(void)
{
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