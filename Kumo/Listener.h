#pragma once
#define WINSOCK_VERSION_REQUEST MAKEWORD(2, 2)
#define DEFAULT_PORT 33777
class Listener
{
public:
	Listener(void);
	~Listener(void);
	int start(void);
	int stop(void);
	int restart(void);
	int setPort(WORD new_port);
	WORD getPort(void);
protected:
	WORD port;
	WSADATA wsaData;
	SOCKET inSocket;
	sockaddr_in server_addr;
};