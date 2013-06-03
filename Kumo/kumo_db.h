#pragma once
#define MESSAGE_DIRECTORY_REQUEST_PATH_SEPARATOR L"/"
#define USER_LIST_FILE L"db/user_list.db"
#define DB_SEPARATOR L"\""
#define PATH_TO_DB L"db/"

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
};

