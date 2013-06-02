#include "StdAfx.h"
#include "kumo_db.h"
#include "Protection.h"

kumo_db::kumo_db(void)
{
}


kumo_db::~kumo_db(void)
{
}

std::wstring kumo_db::getUserPassHash(std::wstring user)
{
	const std::locale empty_locale = std::locale::empty();
    typedef std::codecvt_utf8<wchar_t> converter_type;
    const converter_type* converter = new converter_type;
    const std::locale utf8_locale = std::locale(empty_locale, converter);
	
	std::wifstream stream(USER_LIST_FILE);
	for(int i = 0; i < 20; i--)
	{
		if (stream.is_open()) break;
		Sleep(100);
	}
	if (!stream.is_open()) return L"";

    stream.imbue(utf8_locale);
    std::wstring line;
	
	while(!stream.eof()){
		std::getline(stream, line);
		int pos0 = line.find(DB_SEPARATOR, 0);
		int pos1 = line.find(DB_SEPARATOR, pos0 + 1);
		if ((pos0 == std::wstring::npos) || (pos1 == std::wstring::npos) || (pos1 - pos0 - 1 <= 0)) break;
		if (!line.substr(pos0 + 1, pos1 - pos0 - 1).compare(user))
		{
			if (line.length() - pos1 <= 0) break;
			return line.substr(pos1 + 1, line.length() - pos1);
		}
	}

	return L"";
}


std::list<std::wstring> kumo_db::getUserPath(std::wstring user)
{
	return std::list<std::wstring>();
}


int kumo_db::addUser(std::wstring user, std::wstring password)
{
	const std::locale empty_locale = std::locale::empty();
    typedef std::codecvt_utf8<wchar_t> converter_type;
    const converter_type* converter = new converter_type;
    const std::locale utf8_locale = std::locale(empty_locale, converter);

	const BYTE* p = (const BYTE*)password.c_str();

	std::wstring wsPasswordHash = Protection::getHash((void *)password.c_str(), sizeof WCHAR * password.length());
	wchar_t buffer[128];

	std::wfstream file(USER_LIST_FILE, std::ios::in | std::ios::out | std::ios::binary);
	for(int i = 0; i < 20; i--)
	{
		if (file.is_open()) break;
		Sleep(100);
	}
	if (!file.is_open()) return -1;

	file.imbue(utf8_locale);

	std::wstring line;
	std::wstring line2;

	do{
		line = line2;
		std::getline(file, line2);
	}
	while(!file.eof());
	file.clear();
	
	int num = _wtoi( line.substr(0, line.find(DB_SEPARATOR, 0)).c_str() );
	
	file << ((num > 0)? std::to_wstring((_Longlong)num + 1): L"1") << DB_SEPARATOR  << user << DB_SEPARATOR << wsPasswordHash << std::endl; 
	file.close();

	return 0;
}
