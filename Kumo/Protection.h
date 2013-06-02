#pragma once
class Protection
{
public:
	Protection(void);
	~Protection(void);
	static std::wstring getHash(void * pData, int size);
};

