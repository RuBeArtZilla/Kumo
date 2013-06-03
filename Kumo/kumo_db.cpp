#include "StdAfx.h"
#include "kumo_db.h"
#include "Protection.h"
#include "boost\filesystem.hpp"

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


	std::wfstream file(USER_LIST_FILE,  std::ios::out | std::ios::in |std::ios::binary /*| std::ios::trunc*/);
	if (!file.is_open())
		file.open (USER_LIST_FILE,  std::ios::out | std::ios::binary);

	for(int i = 0; i < 10; i--)
	{
		if (file.is_open()) break;
		Sleep(100);
		file.open (USER_LIST_FILE,  std::ios::out | std::ios::in | std::ios::binary);
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

std::vector<std::wstring> kumo_db::getDir(std::wstring path)
{
	namespace bfs = boost::filesystem;
    std::vector<std::wstring> filenames;
	const bfs::directory_iterator end;
	std::wstring tmp;
	for (bfs::directory_iterator it = (bfs::directory_iterator)bfs::path(path); it != end; ++it)
	{
		tmp = it->path().filename().wstring();
		if (bfs::is_directory(it->path()))	tmp += MESSAGE_DIRECTORY_REQUEST_PATH_SEPARATOR;
		filenames.push_back(tmp); 
	}
	return filenames;
}


int kumo_db::addUserPath(std::wstring user, std::wstring path)
{
	return 0;
}


int kumo_db::addDeviceInfo(std::wstring user, std::wstring device, std::wstring info)
{
	return 0;
}
