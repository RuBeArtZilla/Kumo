#pragma once
#define MESSAGE_DIRECTORY_REQUEST_PATH_SEPARATOR L"/"
#define USER_LIST_FILE L"user_list.db"
#define PATH_LIST_FILE L"path_list.db"
#define USER_LIST_FILE_NAME L"user_list"
#define PATH_LIST_FILE_NAME L"path_list"
#define CANT_OPEN_FILE -1
#define CANT_CREATE_TMP_FILE -2

#define NULL_PASSWORD_HASH L""

#define DB_SEPARATOR L"\""
#define PATH_TO_DB L"db/"

struct SESSION_INFO
{
	std::wstring user;
	long session;
};

typedef std::list<SESSION_INFO> SESSION_LIST;
typedef std::list<SESSION_INFO>::iterator SESSION_LIST_ITERATOR;

struct FILES_BIND
{
	std::wstring file;
	std::string url;
};

typedef std::list<FILES_BIND> FILES_BIND_LIST;
typedef std::list<FILES_BIND>::iterator FILES_BIND_LIST_ITERATOR;

class kumo_db
{
public:
	kumo_db(void);
	~kumo_db(void);

	static std::vector<std::wstring> getDir(std::wstring path);

	static int addUser(std::wstring user, std::wstring password);
	static int delUser(std::wstring user);
	static std::wstring getUserPassHash(std::wstring user);
	
	static int addUserPath(std::wstring user, std::wstring path);
	static int delUserPath(std::wstring user, std::wstring path);
	static std::vector<std::wstring> getUserPath(std::wstring user);

	static int addDeviceInfo(std::wstring user, std::wstring device, std::wstring info);

	static bool startSession(SESSION_INFO info);
	static SESSION_INFO getSession(long session_id);
	static bool checkSession(long session_id);
	static long getCurrentTime(void);
	static std::wstring getFileByUrl(std::string url);
	static void addFileBind(std::wstring file, std::string url);
	static void delFileBind(std::string url);
};
