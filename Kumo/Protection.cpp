#include "StdAfx.h"
#include "Protection.h"


Protection::Protection(void)
{
}


Protection::~Protection(void)
{
}


std::wstring Protection::getHash(void * pData, int size)
{
	HCRYPTPROV hCryptProv;
    HCRYPTHASH hHash;
	DWORD count = 0;
	std::wstring result = L"";
    if(!CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))	result;
	if(!CryptCreateHash(hCryptProv, CALG_SHA, 0, 0, &hHash))	result;
	if(!CryptHashData(hHash, (BYTE*)pData, size, 0))	result;
	if(!CryptGetHashParam(hHash, HP_HASHVAL, NULL, &count, 0))	result;

	char* hash_value = static_cast<char*>(malloc(count + 1));
	ZeroMemory(hash_value, count + 1);

	if(!CryptGetHashParam(hHash, HP_HASHVAL, (BYTE*)hash_value, &count, 0))	result;

	for(int i = count - 1; i >= 0; i--){if (hash_value[i]==0) hash_value[i] = 255;}

	std::string val(hash_value);
	std::wstring wstr( val.begin(), val.end() );

	int index = 0;
	for(;;)
	{
		index = wstr.find(L"\n");
		if (index == std::wstring::npos) break;
		wstr.erase(index, 1);
	}
	for(;;)
	{
		index = wstr.find(L"\r");
		if (index == std::wstring::npos) break;
		wstr.erase(index, 1);
	}

	return wstr;
}