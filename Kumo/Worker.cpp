#include "StdAfx.h"
#include "Worker.h"
#include "boost\filesystem.hpp"

Worker::Worker(void)
{
	srand((unsigned int)time(NULL));
	session = rand();
}

Worker::~Worker(void)
{
	closesocket(client);
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

		if ((buf[0] == 'G') && (buf[1] == 'E') && (buf[2] == 'T'))
		{
			W.recv_HTTP_GET(buf, msize);
			return;
		}

		char size[2] = {buf[0], buf[1]}; // TODO: use this size of message!
		size[0] = size[0]; //for ignore warning
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

		std::wstring cur_session = wsFindParameter(&lwmp, std::wstring(MESSAGE_SESSION));///////////////////////////////////

		if (cur_session != MESSAGE_PARAMETER_NOT_FOUND)
		{
			long cs = _wtol(cur_session.c_str());
			if (!kumo_db::checkSession(cs))
			{
				W.SendErrorMessage(MESSAGE_ERROR_CODE_BAD_SESSION);
				return;
			}
			W.session = cs;
		}

		switch (iMsg)
		{
		case MESSAGE_AUTHORISATION_ID:
			W.Authorisation(&lwmp);
			break;
		case MESSAGE_DEVICE_INFO_ID:
			W.DeviceInfo(&lwmp);
			break;
		case MESSAGE_DIRECTORY_REQUEST_ID:
			W.DirectoryRequest(&lwmp);
			break;
		case MESSAGE_FILE_REQUEST_ID:
			W.FileRequest(&lwmp);
			break;
		default:
			{
				W.SendErrorMessage(MESSAGE_ERROR_CODE_BAD_MESSAGE);
				break;
			}	
		}
		return;
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

std::wstring wsFindParameter(LISTWSMSGPRM * source, std::wstring find)
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
			LISTWSMSGPRM newMessage;

			newMessage.push_back(createParam(MESSAGE_THEME, MESSAGE_AUTHORISATION));
			newMessage.push_back(createParam(MESSAGE_TIME, std::to_wstring((_Longlong)kumo_db::getCurrentTime())));
			newMessage.push_back(createParam(MESSAGE_SESSION, std::to_wstring((_Longlong)session)));

			SESSION_INFO newSession;
			newSession.session = session;
			newSession.user = login;
			kumo_db::startSession(newSession);
			return SendMessage(&newMessage);
		}
	}
	return SendErrorMessage(MESSAGE_ERROR_CODE_BAD_AUTHORISATION);
}

int Worker::DeviceInfo(LISTWSMSGPRM *msg)
{
	//TODO: DeviceInfo
	return 0;
}

int Worker::DirectoryRequest(LISTWSMSGPRM *msg)
{
	wstring dir = wsFindParameter(msg, MESSAGE_DIRECTORY_REQUEST_PATH);
	WSVECTOR filenames;

	if ((dir == MESSAGE_PARAMETER_NOT_FOUND) || (dir.find(MESSAGE_DIRECTORY_REQUEST_ROOT) == 0))
	{
		dir = MESSAGE_DIRECTORY_REQUEST_ROOT;
		filenames = kumo_db::getUserPath(kumo_db::getSession(session).user);
	}
	else
	{
		filenames = kumo_db::getDir(dir);
	}
	return SendDirectory(dir, filenames);
}

int Worker::FileRequest(LISTWSMSGPRM *msg)
{
	wstring path = wsFindParameter(msg, MESSAGE_FILE_REQUEST_PATH);
	wstring name = wsFindParameter(msg, MESSAGE_FILE_REQUEST_NAME);

	if (!CheckUserPath(path))
		return SendErrorMessage(MESSAGE_ERROR_CODE_BAD_PATH);

	namespace bfs = boost::filesystem;
	if (!bfs::exists(path + name))
		return SendErrorMessage(MESSAGE_ERROR_CODE_BAD_FILE_NOT_EXIST);

	LISTWSMSGPRM newMessage;

	bfs::path(path + name).extension();
	
	newMessage.push_back(createParam(MESSAGE_THEME, MESSAGE_FILE_REQUEST));
	newMessage.push_back(createParam(MESSAGE_TIME, std::to_wstring((_Longlong)kumo_db::getCurrentTime())));
	newMessage.push_back(createParam(MESSAGE_SESSION, std::to_wstring((_Longlong)session)));
	newMessage.push_back(createParam(MESSAGE_FILE_REQUEST_PATH, path));
	newMessage.push_back(createParam(MESSAGE_FILE_REQUEST_NAME, name));
	
	string surl =  std::to_string((_Longlong)session) + std::to_string((_Longlong)kumo_db::getCurrentTime()) + bfs::path(path + name).extension().string();
	wstring url =  std::to_wstring((_Longlong)session) + std::to_wstring((_Longlong)kumo_db::getCurrentTime()) + bfs::path(path + name).extension().wstring();
	
	wstring newPath = path + name;

	newMessage.push_back(createParam(MESSAGE_FILE_REQUEST_URL, url));
	//wstring param = L"-i " + path + name + L" -ab 128 -f " + bfs::path(path + name).extension().wstring() +  L" F:/" + url;
	/*wstring param = L"-i " + path + name + L" -ar 44100 -ac 2 -ab 128 F:/" + url;

	STARTUPINFO cif;
	ZeroMemory(&cif, sizeof(STARTUPINFO));
	PROCESS_INFORMATION pi;
	if (CreateProcess(L"C:/ffmpeg/bin/ffmpeg.exe", (LPWSTR)param.c_str(), NULL, NULL, FALSE, NULL, NULL, NULL, &cif, &pi)==TRUE)
	{
		DWORD dwResult = WaitForSingleObject(pi.hProcess, 10000);
		if (dwResult == 0)
		{
			newPath = url;
		}
	}
	*/
	kumo_db::addFileBind( newPath, surl);

	return SendMessage(&newMessage);
}

int Worker::SendErrorMessage(wstring msg)
{
	LISTWSMSGPRM newMessage;
	newMessage.push_back(createParam(MESSAGE_THEME, MESSAGE_ERROR));
	newMessage.push_back(createParam(MESSAGE_TIME, std::to_wstring((_Longlong)kumo_db::getCurrentTime())));
	if (kumo_db::checkSession(session))
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
	newMessage.push_back(createParam(MESSAGE_TIME, std::to_wstring((_Longlong)kumo_db::getCurrentTime())));
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

	short wstringLen = (short)wsMessage.length();
	short size = wstringLen * (sizeof WCHAR) + 4;

	char* bMessage = static_cast<char*>(malloc(size));

	if (bMessage != NULL)
	{
		ZeroMemory(bMessage, size);
		char * pData = &bMessage[2];
		memcpy(pData, wsMessage.c_str(), size - 4);
		bMessage[0] = ((char*)&wstringLen)[1];
		bMessage[1] = ((char*)&wstringLen)[0];
		bMessage[size - 2] = 0;
		bMessage[size - 1] = 0;
		send(client, bMessage, size, 0);
		shutdown(client, 0);
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

bool Worker::CheckUserPath(std::wstring path)
{
	if (kumo_db::checkSession(session))
	{
		SESSION_INFO si = kumo_db::getSession(session);
		WSVECTOR filenames = kumo_db::getUserPath(si.user);		
		WSVECTOR_ITERATOR iterator = filenames.begin();
		while(iterator != filenames.end())
		{
			if (path.find(*iterator) == 0)
				return true;
			iterator++;
		}
	}
	return false;
}

int Worker::recv_HTTP_GET(char * buf, int size)
{
	//TODO: refactor this long function (it's easy)
	std::string input = buf;
	int pos0 = input.find(" ", 0);
	int pos1 = input.find(" ", pos0 + 1);

	int pos2 = input.find("Range: bytes=", 0);
	int pos3 = input.find("-", pos2);
	int pos4 = input.find("\r\n", pos3);

	bool partial_content = (pos2 > 1); 

	std::string range_begin = "";
	std::string range_end = "";
	LONGLONG ulBegin = 0;
	LONGLONG ulEnd = 0;

	std::string url = input.substr(pos0 + 2, pos1 - pos0 - 2);

	wstring wsFile = L"E:/AUDIO/OST/Shingeki no Kyojin/ED Single - Utsukushiki Zankoku na Sekai/01 - Utsukushiki Zankoku na Sekai.mp3";
	wstring wsDBFile = kumo_db::getFileByUrl(url);
	if (wsDBFile != L"")
	{
		wsFile = wsDBFile;
	}
	else
	{
		char error404[] = "HTTP/1.1 404 Not Found\r\nServer: AZ-Mini\r\nKeep-Alive: timeout=5, max=100\r\nConnection: Keep-Alive\r\n\r\nFile not found on Server";
		send(client, error404, strlen(error404), 0);
		return 1;
	}

	HANDLE hFile;
	hFile = CreateFile(wsFile.c_str(),
		GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	LONGLONG nFileLen = 0;
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwSizeHigh=0, dwSizeLow=0;
		dwSizeLow = GetFileSize(hFile, &dwSizeHigh);
		nFileLen = (dwSizeHigh * (MAXDWORD+1)) + dwSizeLow;
		CloseHandle(hFile);
	}
	else
	{
		char error404[] = "HTTP/1.1 404 Not Found\r\nServer: AZ-Mini\r\nKeep-Alive: timeout=5, max=100\r\nConnection: Keep-Alive\r\n\r\nFile not found on Server";
		send(client, error404, strlen(error404), 0);
		return 1;
	}

	if (partial_content)
	{
		range_begin = input.substr(pos2 + 13, pos3 - pos2 - 13);
		range_end = input.substr(pos3 + 2, pos4 - pos3 - 2);
		ulEnd = (pos4 - pos3 == 1) ? nFileLen : stoul(range_end, NULL);
		if (ulEnd > nFileLen) 
			ulEnd = nFileLen;
		ulBegin = (pos3 - pos2 == 13) ? nFileLen - ulEnd : stoul(range_begin, NULL);
		if (ulEnd - ulBegin == nFileLen)
			partial_content = false;
	}

	char buffer[1024];

	string sheader = (partial_content)? HTTP_ANSWER_206 : HTTP_ANSWER_200;
	sheader += "Server: AZ-Mini\r\n";
	if (partial_content)
		sheader += "Content-Range: bytes " + std::to_string(ulBegin) + "-" + std::to_string(ulEnd) + "/" + std::to_string(nFileLen) + "\r\n";
	if (partial_content) 
		sheader += "Content-Length: " + std::to_string(ulBegin - ulEnd) + "\r\n";
	else
		sheader += "Content-Length: " + std::to_string(nFileLen) + "\r\n";
	
	sheader += "Accept-Ranges: bytes\r\nConnection: close\r\n\r\n";

	send(client, sheader.c_str(), sheader.length(), 0);
	
	std::ifstream file(wsFile,  std::ios::in |std::ios::binary);
	if (partial_content)
	{
		file.seekg(ulBegin);
	}

	char * alloc_buffer = new char[65536];

	while (file){
		file.read(alloc_buffer, 65536);
		send(client, alloc_buffer, file.gcount(), 0);
	}
	delete alloc_buffer;
	closesocket(client);
	//CloseHandle(hFile);//added, need test
	return 0;
}

int Worker::recv_HTTP_GET2(char * buf, int size)
{
	return 0;
}

int Worker::send_file(std::wstring filename, long l, long r)
{
	std::ifstream file(filename,  std::ios::in |std::ios::binary);
	char buffer[1024];
	while (file){
		file.read(buffer, 1024);
		send(client, buffer, file.gcount(), 0);
	}
	return 0;
}
