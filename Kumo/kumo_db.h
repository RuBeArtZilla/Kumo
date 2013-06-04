#pragma once
#define MESSAGE_DIRECTORY_REQUEST_PATH_SEPARATOR L"/"
#define USER_LIST_FILE L"db/user_list.db"
#define PATH_LIST_FILE L"db/path_list.db"
#define DB_SEPARATOR L"\""
#define PATH_TO_DB L"db/"

struct SESSION_INFO
{
	std::wstring user;
	long session;
};

typedef std::list<SESSION_INFO> SESSION_LIST;
typedef std::list<SESSION_INFO>::iterator SESSION_LIST_ITERATOR;

class kumo_db
{
public:
	kumo_db(void);
	~kumo_db(void);
	static std::wstring getUserPassHash(std::wstring user);
	static std::list<std::wstring> getUserPath(std::wstring user);
	static int addUser(std::wstring user, std::wstring password);
	static std::vector<std::wstring> getDir(std::wstring path);
	static int addUserPath(std::wstring user, std::wstring path);
	static int addDeviceInfo(std::wstring user, std::wstring device, std::wstring info);
protected:
	
public:
	static int startSession(SESSION_INFO info);
	static SESSION_INFO checkSession(long session_id);
};

