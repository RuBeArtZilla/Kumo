#include "StdAfx.h"
#include "Listener.h"
#include "Worker.h"
#include "kumo_db.h"

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
	//kumo_db::addUser(L"user",L"0000");
	//kumo_db::addUser(L"admin",L"0000");
	//kumo_db::addUser(L"root",L"0000");
	//kumo_db::addUser(L"roots",L"0000");
	//kumo_db::addUserPath(L"user", L"F:/");
	//kumo_db::addUserPath(L"admin", L"F:/");
	//kumo_db::addUserPath(L"admin", L"E:/AUDIO");
	//kumo_db::addUserPath(L"root", L"F:/");
	//kumo_db::delUserPath(L"root", L"E:/AUDIO");
	//kumo_db::delUser(L"roots");
	kumo_db::addFileBind(L"E:/AUDIO/Lossless J/Denpa Onna to Seishun Otoko/OP Single — Os-Uchuujin (Touwa Erio (CV. Oogame Asuka))/01 Os-Uchuujin.flac", "my.flac");
	kumo_db::addFileBind(L"E:/AUDIO/OST/Haiyore! Nyaruko-san/Haiyore! Nyaruko-san/Character Song Mini Album & Original Soundtrack/CD1/08 - Taiyou Iwaku Moeyo Chaos (30min Non-limit ver.).mp3", "long.mp3");
	kumo_db::addFileBind(L"W:/music.mp3", "my.mp3");
	kumo_db::addFileBind(L"F:/anime/Bonus/Black Rock Shooter (OP).mp4", "my.mp4");
	kumo_db::addFileBind(L"D:/VIDEO/CLIPS/Aya Hirano & Nana Mizuki Discotheque MonStAR Sub Espanol.flv", "my.flv");
	kumo_db::addFileBind(L"D:/VIDEO/CLIPS/Aya Hirano & Nana Mizuki Discotheque MonStAR Sub Espanol.flv", "my.swf");
	kumo_db::addFileBind(L"D:/Photo/Artem/Sony Ericsson K750i/v1/MOV00021.3gp", "my.3gp");
	kumo_db::addFileBind(L"F:/test.jpg", "my.jpg");
	
	for(;;)
	{
		WorkerData WD;
		int size_client_addr = sizeof(WD.addr);
		WD.sock = accept(pListener->inSocket, (sockaddr*)&WD.addr, &size_client_addr);
		/*HANDLE hNewThread = (HANDLE)*/_beginthread( CreateNewWorker, 0, (void*) &WD ); //TODO: use this hadle later
	}
}