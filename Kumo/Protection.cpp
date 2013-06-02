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
	HCRYPTPROV hProv;
	HCRYPTHASH hHash;

	// ������������� ��������� ����������������
	if(!CryptAcquireContext(&hProv, 
		"{EB57ED8A-CCCC-4bf5-8659-9DF2F05F24AD}", NULL, PROV_RSA_FULL, 0))
	{
		Error("CryptAcquireContext");
		return;
	}

	std::cout << "Cryptographic provider initialized" << std::endl;

	// C������� ���-�������
	if(!CryptCreateHash(hProv, CALG_MD5, 0, 0, &hHash))
	{
		Error("CryptCreateHash");
		return;
	}

	std::cout << "Hash created" << std::endl;

	// �������� ������ ��� �����������
	char string[] = "Test";
	DWORD count = strlen(string);

	// �������� ���������� ������ ���-�������.
	if(!CryptHashData(hHash, (BYTE*)string, count, 0))
	{
		Error("CryptHashData");
		return;
	}

	std::cout << "Hash data loaded" << std::endl;

	// ��������� ���-��������
	count = 0;

	if(!CryptGetHashParam(hHash, HP_HASHVAL, NULL, &count, 0))
	{
		Error("CryptGetHashParam");
		return;
	}

	char* hash_value = static_cast<char*>(malloc(count + 1));
	ZeroMemory(hash_value, count + 1);

	if(!CryptGetHashParam(hHash, HP_HASHVAL, (BYTE*)hash_value, &count, 0))
	{
		Error("CryptGetHashParam");
		return;
	}

	std::cout << "Hash value is received" << std::endl;

	// ����� �� ����� ����������� ���-��������
	std::cout << "Hash value: " << hash_value << std::endl;






	return std::wstring();
}
