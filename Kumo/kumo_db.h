#pragma once

static class kumo_db
{
public:
	kumo_db(void);
	~kumo_db(void);
	wstring getUserPassHash(wstring user);
	list<wstring> getUserPath(wstring user);
};

