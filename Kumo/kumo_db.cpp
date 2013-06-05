#include "StdAfx.h"
#include "kumo_db.h"
#include "Protection.h"
#include "boost\filesystem.hpp"

SESSION_LIST sessions;
FILES_BIND_LIST files_bind;

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
	if (!stream.is_open()) return NULL_PASSWORD_HASH;

    stream.imbue(utf8_locale);
    std::wstring line;

	while(!stream.eof())
	{
		std::getline(stream, line);
		int pos0 = line.find(DB_SEPARATOR, 0);
		int pos1 = line.find(DB_SEPARATOR, pos0 + 1);
		if ((pos0 == std::wstring::npos) ||
			(pos1 == std::wstring::npos) ||
			(pos1 - pos0 - 1 <= 0))
			break;
		if (!line.substr(pos0 + 1, pos1 - pos0 - 1).compare(user))
		{
			if (line.length() - pos1 <= 0) break;
			return line.substr(pos1 + 1, line.length() - pos1);
		}
	}

	return NULL_PASSWORD_HASH;
}

int kumo_db::addUser(std::wstring user, std::wstring password)
{
	const std::locale empty_locale = std::locale::empty();
    typedef std::codecvt_utf8<wchar_t> converter_type;
    const converter_type* converter = new converter_type;
    const std::locale utf8_locale = std::locale(empty_locale, converter);

	const BYTE* p = (const BYTE*)password.c_str();

	if (getUserPassHash(user) != NULL_PASSWORD_HASH)
		delUser(user);
	//It's not bug, if you wan't to change password, just call addUser ^_^

	std::wstring wsPasswordHash =
		Protection::getHash((void *)password.c_str(), sizeof WCHAR * password.length());
	wchar_t buffer[128];

	std::wfstream file(USER_LIST_FILE,  std::ios::out | std::ios::in |std::ios::binary);
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

	file << ((num > 0)? std::to_wstring((_Longlong)num + 1): L"1")
		<< DB_SEPARATOR  << user << DB_SEPARATOR << wsPasswordHash << std::endl;
	file.close();

	return 0;
}


int kumo_db::delUser(std::wstring user)
{
	const std::locale empty_locale = std::locale::empty();
    typedef std::codecvt_utf8<wchar_t> converter_type;
    const converter_type* converter = new converter_type;
    const std::locale utf8_locale = std::locale(empty_locale, converter);

	std::wfstream file(USER_LIST_FILE,  std::ios::out | std::ios::in |std::ios::binary);
	for(int i = 0; i < 10; i--)
	{
		if (file.is_open()) break;
		Sleep(100);
		file.open (USER_LIST_FILE,  std::ios::out | std::ios::in | std::ios::binary);
	}
	if (!file.is_open()) return CANT_OPEN_FILE;

	std::wstring tmpPath = USER_LIST_FILE_NAME + std::to_wstring((_Longlong)rand()) + L".tmp";
	std::wfstream tmpfile(tmpPath,  std::ios::out | std::ios::trunc |std::ios::binary);
	if (!tmpfile)
	{
		file.close();
		return CANT_CREATE_TMP_FILE;
	}

	std::wstring line;

	do{
		std::getline(file, line);

		int pos0 = line.find(DB_SEPARATOR, 0);
		int pos1 = line.find(DB_SEPARATOR, pos0 + 1);
		if ((pos0 == std::wstring::npos) ||
			(pos1 == std::wstring::npos) ||
			(pos1 - pos0 - 1 <= 0))
			continue;
		if (line.substr(pos0 + 1, pos1 - pos0 - 1).compare(user))
		{
			tmpfile << line << std::endl;
		}
	}
	while(!file.eof());

	tmpfile.close();
	file.close();
	//WARNING: BUG: if somebody use this files, before swap
	namespace bfs = boost::filesystem;
	bfs::remove(USER_LIST_FILE);
	bfs::rename(tmpPath, USER_LIST_FILE);

	return 0;
}


std::vector<std::wstring> kumo_db::getDir(std::wstring path)
{
	namespace bfs = boost::filesystem;
    std::vector<std::wstring> filenames;
	std::wstring tmp;
	const bfs::directory_iterator end;

	if (!bfs::exists(path))	return filenames;

	for (bfs::directory_iterator it = (bfs::directory_iterator)bfs::path(path); it != end; ++it)
	{
		tmp = it->path().filename().wstring();
		if (bfs::is_directory(it->path()))	tmp += MESSAGE_DIRECTORY_REQUEST_PATH_SEPARATOR;
		filenames.push_back(tmp);
	}

	return filenames;
}

int kumo_db::delUserPath(std::wstring user, std::wstring path)
{
	const std::locale empty_locale = std::locale::empty();
    typedef std::codecvt_utf8<wchar_t> converter_type;
    const converter_type* converter = new converter_type;
    const std::locale utf8_locale = std::locale(empty_locale, converter);

	std::wfstream file(PATH_LIST_FILE,  std::ios::out | std::ios::in |std::ios::binary);
	for(int i = 0; i < 10; i--)
	{
		if (file.is_open()) break;
		Sleep(100);
		file.open (PATH_LIST_FILE,  std::ios::out | std::ios::in | std::ios::binary);
	}
	if (!file.is_open()) return CANT_OPEN_FILE;

	std::wstring tmpPath = PATH_LIST_FILE_NAME + std::to_wstring((_Longlong)rand()) + L".tmp";
	std::wfstream tmpfile(tmpPath,  std::ios::out | std::ios::trunc |std::ios::binary);
	if (!tmpfile)
	{
		file.close();
		return CANT_CREATE_TMP_FILE;
	}

	std::wstring line;
	std::wstring delLine = user + DB_SEPARATOR + path + DB_SEPARATOR;

	do{
		std::getline(file, line);
		
		if (line.compare(delLine) && (line.length() > 3))
		{
			tmpfile << line << std::endl;
		}
	}
	while(!file.eof());

	tmpfile.close();
	file.close();
	//WARNING: BUG: if somebody use this files, before swap
	namespace bfs = boost::filesystem;
	bfs::remove(PATH_LIST_FILE);
	bfs::rename(tmpPath, PATH_LIST_FILE);

	return 0;
}

int kumo_db::addUserPath(std::wstring user, std::wstring path)
{
	const std::locale empty_locale = std::locale::empty();
    typedef std::codecvt_utf8<wchar_t> converter_type;
    const converter_type* converter = new converter_type;
    const std::locale utf8_locale = std::locale(empty_locale, converter);

	std::wfstream file(PATH_LIST_FILE,  std::ios::out | std::ios::app |std::ios::binary);

	for(int i = 0; i < 10; i--)
	{
		if (file.is_open()) break;
		Sleep(100);
		file.open (PATH_LIST_FILE,  std::ios::out | std::ios::app | std::ios::binary);
	}
	if (!file.is_open()) return CANT_OPEN_FILE;

	file.imbue(utf8_locale);
	file << user << DB_SEPARATOR << path << DB_SEPARATOR << std::endl;
	file.close();

	return 0;
}

std::vector<std::wstring> kumo_db::getUserPath(std::wstring user)
{
	std::vector<std::wstring> result;

	const std::locale empty_locale = std::locale::empty();
    typedef std::codecvt_utf8<wchar_t> converter_type;
    const converter_type* converter = new converter_type;
    const std::locale utf8_locale = std::locale(empty_locale, converter);

	std::wifstream stream(PATH_LIST_FILE);
	for(int i = 0; i < 20; i--)
	{
		if (stream.is_open()) break;
		Sleep(100);
	}
	if (!stream.is_open()) return result;

    stream.imbue(utf8_locale);
    std::wstring line;

	while(!stream.eof()){
		std::getline(stream, line);
		int pos0 = line.find(DB_SEPARATOR, 0);
		int pos1 = line.find(DB_SEPARATOR, pos0 + 1);
		if ((pos0 == std::wstring::npos) ||
			(pos1 == std::wstring::npos) ||
			(pos1 - pos0 - 1 <= 0))
			break;
		if (!line.substr(0, pos0).compare(user))
		{
			result.push_back( line.substr(pos0 + 1, pos1 - pos0 - 1) );
		}
	}
	return result;
}

int kumo_db::addDeviceInfo(std::wstring user, std::wstring device, std::wstring info)
{
	return 0;
}

bool kumo_db::startSession(SESSION_INFO info)
{
	if (!checkSession(info.session))
	{
		sessions.push_back(info);
		return true;
	}
	return 0;
}

bool kumo_db::checkSession(long session_id)
{
	SESSION_LIST_ITERATOR iterator = sessions.begin();
	while(iterator != sessions.end())
	{
		if (iterator->session == session_id)
			return true;
		iterator++;
	}
	return false;
}

SESSION_INFO kumo_db::getSession(long session_id)
{
	SESSION_LIST_ITERATOR iterator = sessions.begin();
	while(iterator != sessions.end())
	{
		if (iterator->session == session_id)
			return *iterator;
		iterator++;
	}
	return SESSION_INFO();
}

long kumo_db::getCurrentTime(void)
{
	return time(NULL);
}

std::wstring kumo_db::getFileByUrl(std::string url)
{
	FILES_BIND_LIST_ITERATOR iterator = files_bind.begin();
	while(iterator != files_bind.end())
	{
		if (iterator->url == url)
			return iterator->file;
		iterator++;
	}
	return L"";
}

void kumo_db::addFileBind(std::wstring file, std::string url)
{
	FILES_BIND tmp = {file, url};
	files_bind.push_back(tmp);
}

void kumo_db::delFileBind(std::string url)
{
	FILES_BIND_LIST_ITERATOR iterator = files_bind.begin();
	while(iterator != files_bind.end())
	{
		/*if (iterator->url == url)
			files_bind.remove(*iterator);*/
		iterator++;
	}
}