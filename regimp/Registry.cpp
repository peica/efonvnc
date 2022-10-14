/*************************************************
Copyright (c) 2000-2009 by Peica Chen.
MIT License

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*************************************************/

// Registry.cpp: implementation of the CRegistry class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Registry.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRegistry::CRegistry()
{

}

CRegistry::~CRegistry()
{

}

void StrToBytes(char * str, BYTE * bytes, int * len)
{
	int max_len;
	int slen;
	char * p = str;
	int val;
	char tmp[16];

	*len=0;
	max_len = strlen(str);
	while(p && (p - str) <= max_len) {		
		slen=0;
		while(p[slen] && (p[slen]!=','|| p[slen]==0))slen++;

		memcpy(tmp, p, slen + 1);
		tmp[slen+1] = 0;

		sscanf(tmp, "%x,", &val);

		bytes[(*len)]=(char)val;
		(*len)++;
		p=p+slen+1;
	}	
}

BOOL CRegistry::FlushRegistry(UINT params)
{
	if (ERROR_SUCCESS != RegFlushKey(HKEY_LOCAL_MACHINE))
	{
		return FALSE;
	}
	Sleep(500);
	
	if (ERROR_SUCCESS != RegFlushKey(HKEY_CURRENT_USER))
	{
		return FALSE;
	}
	Sleep(500);
	
	if (ERROR_SUCCESS != RegFlushKey(HKEY_CLASSES_ROOT))
	{
		return FALSE;
	}
	Sleep(500);
	
	if (ERROR_SUCCESS != RegFlushKey(HKEY_CLASSES_ROOT))
	{
		return FALSE;
	}
	Sleep(500);

	return TRUE;
}

BOOL CRegistry::ImportRegistry(char *fileName)
{
	dictionary * dict;
	int nSectCount, nKeyCount;
	char * pSect;char * pKeyName;char * pKeyValue;
	HKEY mainKey, subKey;
	TCHAR szSubKey[MAX_PATH], szSubKeyValName[MAX_PATH];	
	
	dict = INI::iniparser_load(fileName);
	if (!dict) {		
		return FALSE;
	}	
	
	nSectCount = INI::iniparser_getnsec(dict);

	for (int iSect=0; iSect<nSectCount; iSect++)
	{
		DWORD dwDisp;
		int mainKeyLen = 0;
		int subKeyLen = 0;

		pSect = INI::iniparser_getsecname(dict, iSect);		
		while(pSect[mainKeyLen]!='\\')mainKeyLen++;
		subKeyLen = strlen(pSect) - mainKeyLen - 1;
		
		if (_strnicmp(pSect, "HKEY_CLASSES_ROOT", mainKeyLen) == 0)
		{
			mainKey = HKEY_CLASSES_ROOT;
		}

		if (_strnicmp(pSect, "HKEY_LOCAL_MACHINE", mainKeyLen) == 0)
		{
			mainKey = HKEY_LOCAL_MACHINE ;
		}

		if (_strnicmp(pSect, "HKEY_CURRENT_USER", mainKeyLen) == 0)
		{
			mainKey = HKEY_CURRENT_USER ;
		}

		if (_strnicmp(pSect, "HKEY_USERS", mainKeyLen) == 0)
		{
			mainKey = HKEY_USERS ;
		}		

		mbstowcs((LPTSTR)szSubKey, (LPSTR) &pSect[mainKeyLen + 1], subKeyLen);
		szSubKey[subKeyLen] = 0;

		if (RegCreateKeyEx(mainKey, (LPTSTR)szSubKey, 0, 0, 0, 0, 0, &subKey, &dwDisp)
			!= ERROR_SUCCESS) continue;

		nKeyCount = INI::iniparser_getnkey(dict, pSect);
		for (int iKey=0; iKey<nKeyCount; iKey++) {
			char longkey[MAX_PATH] = {0};
			pKeyName = INI::iniparser_getkeyname(dict, pSect, iKey);
			sprintf(longkey, "%s:%s", pSect, pKeyName);

			pKeyValue = INI::iniparser_getstring(dict, longkey, "");
			if (strlen(pKeyValue) <= 0) continue;

			mbstowcs((LPTSTR)szSubKeyValName, pKeyName, sizeof(szSubKeyValName)/sizeof(szSubKeyValName[0]) - 1);

			// Multi-line string
			if (_strnicmp(pKeyValue, "hex(7):", strlen("hex(7):")) == 0)
			{
				BYTE bData[MAX_PATH]={0}; INT bDataLen = MAX_PATH;
				StrToBytes((LPSTR)pKeyValue+7, bData, &bDataLen);
				if (bDataLen > 0) {
					RegSetValueEx(subKey, szSubKeyValName, 0, REG_MULTI_SZ, (BYTE *)bData, sizeof(bData[0]) * bDataLen);
				}
			} else
			// Dword
			if (_strnicmp(pKeyValue, "dword:", strlen("dword:")) == 0)
			{
				ULONG lVal = 0;
				sscanf(pKeyValue, "dword:%lx", &lVal);
				RegSetValueEx(subKey, szSubKeyValName, 0, REG_DWORD, (BYTE *)&lVal, sizeof(lVal));
			} else
			// Binary
			if (_strnicmp(pKeyValue, "hex:", strlen("hex:")) == 0)
			{
				BYTE bData[MAX_PATH]={0}; INT bDataLen = MAX_PATH;
				StrToBytes((LPSTR)pKeyValue+4, bData, &bDataLen);
				if (bDataLen > 0) {
					RegSetValueEx(subKey, szSubKeyValName, 0, REG_BINARY, (BYTE *)bData, sizeof(bData[0]) * bDataLen);
				}
			} else	
			// String
			{					
				TCHAR szSubKeyVal[MAX_PATH];
				if (szSubKeyValName[0] == TEXT('@') && szSubKeyValName[1] == 0)
				{
					_stprintf(szSubKeyValName, TEXT("Default"));
				}
				mbstowcs((LPTSTR)szSubKeyVal, pKeyValue, sizeof(szSubKeyVal)/sizeof(szSubKeyVal[0]) - 1);
				RegSetValueEx(subKey, szSubKeyValName, 0, REG_SZ, (BYTE *)szSubKeyVal, (sizeof(szSubKeyVal[0]) * (_tcslen(szSubKeyVal) + 1)));
			}
		}	

		RegCloseKey(subKey);
	}	


	INI::iniparser_freedict(dict);
	return TRUE;
}
