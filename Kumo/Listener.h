#pragma once
#define WINSOCK_VERSION_REQUEST MAKEWORD(2, 2)
#define DEFAULT_PORT 33777
#define MAXIMUM_QUEUE_LENGHT 32
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
	HANDLE hThread;
protected:
	WORD port;
	WSADATA wsaData;
	SOCKET inSocket;
	sockaddr_in server_addr;
	friend void start_listen(void*  pParams);
};