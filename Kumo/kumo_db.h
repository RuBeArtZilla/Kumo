#pragma once
#define USER_LIST_FILE L"user_list.db"
#define DB_SEPARATOR L"\""
class kumo_db
{
public:
	kumo_db(void);
	~kumo_db(void);
	static std::wstring getUserPassHash(std::wstring user);
	static std::list<std::wstring> getUserPath(std::wstring user);
	static int addUser(std::wstring user, std::wstring password);
};

