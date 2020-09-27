/*************************************************************
Module name: PrfTxtDB.CPP
*************************************************************/

#include "stdafx.h"
#pragma hdrstop

#include "prftxtdb.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////


long	CPRFTXTDB::sm_nInstancesOfClass = -1;
HANDLE	CPRFTXTDB::sm_hEventDataLoaded;
LPWSTR	CPRFTXTDB::sm_mszCounterText;
LPWSTR	CPRFTXTDB::sm_mszHelpText;
int		CPRFTXTDB::sm_nLastCounter;
int		CPRFTXTDB::sm_nLastHelp;


//////////////////////////////////////////////////////////////


CPRFTXTDB::CPRFTXTDB(int nPreferredLanguageID)
{
	HKEY hkey;
	DWORD dwType, cbData;


	InterlockedIncrement(&sm_nInstancesOfClass);
	sm_hEventDataLoaded = CreateEvent(NULL, TRUE, FALSE, __TEXT("CPRFCNTTXTDB"));

	if (GetLastError() != ERROR_ALREADY_EXISTS)
	{
		// The title and help text has never been read 
		// from the registry; let's do it now.
		LPCTSTR szPerflib = __TEXT("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Perflib");
		LONG lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szPerflib, 0, KEY_QUERY_VALUE, &hkey);
		// JMR: Do error checking here.

		// The data was found OK; initialize the static 
		// data members.
		cbData = sizeof(sm_nLastCounter);
		RegQueryValueEx(hkey, __TEXT("Last Counter"), NULL, &dwType, (LPBYTE) &sm_nLastCounter, &cbData);

		cbData = sizeof(sm_nLastHelp);
		RegQueryValueEx(hkey, __TEXT("Last Help"), NULL, &dwType, (LPBYTE) &sm_nLastHelp, &cbData);

		RegCloseKey(hkey);


		TCHAR szBuf[100];
		_stprintf(szBuf, __TEXT("%s\\%03d"), szPerflib, nPreferredLanguageID);
		lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szBuf, 0, KEY_QUERY_VALUE, &hkey);
		if ((lRes != ERROR_SUCCESS) && (nPreferredLanguageID != LOCALE_IDEFAULTLANGUAGE))
		{
			// Couldn't find the help data for the preferred 
			// language; let's try getting the data for the 
			// default language.
			_stprintf(szBuf, __TEXT("%s\\%03d"), szPerflib, LOCALE_IDEFAULTLANGUAGE);
			lRes = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szBuf, 0, KEY_QUERY_VALUE, &hkey);
		}

		if (lRes != ERROR_SUCCESS)
		{
			// The data couldn't be found regardless of the 
			// language used to retrieve.
		}
		else
		{
			// Get the size of the buffer needed for the Counter title text.
			RegQueryValueEx(hkey, __TEXT("Counters"), NULL, &dwType, NULL, &cbData);
			HANDLE hHeap = GetProcessHeap();
			sm_mszCounterText = (LPWSTR) HeapAlloc(hHeap, 0, cbData);
			// JMR: Check for NULL
			RegQueryValueEx(hkey, __TEXT("Counters"), NULL, &dwType, (LPBYTE) sm_mszCounterText, &cbData);

			// Get the size of the buffer needed for the Help title text.
			RegQueryValueEx(hkey, __TEXT("Help"), NULL, &dwType, NULL, &cbData);
			sm_mszHelpText = (LPWSTR) HeapAlloc(hHeap, 0, cbData);
			// JMR: Check for NULL
			RegQueryValueEx(hkey, __TEXT("Help"), NULL, &dwType, (LPBYTE) sm_mszCounterText, &cbData);


			RegCloseKey(hkey);

			// Signal to this thread and other threads that 
			// the data is ready.
			SetEvent(sm_hEventDataLoaded);
		}
	}
	WaitForSingleObject(sm_hEventDataLoaded, INFINITE);
}


//////////////////////////////////////////////////////////////


CPRFTXTDB::~CPRFTXTDB(void)
{
	HANDLE hHeap = GetProcessHeap();
	HeapFree(hHeap, 0, sm_mszHelpText);
	HeapFree(hHeap, 0, sm_mszCounterText);
	CloseHandle(sm_hEventDataLoaded);
	if (InterlockedDecrement(&sm_nInstancesOfClass) < 0)
		sm_hEventDataLoaded = NULL;
}


//////////////////////////////////////////////////////////////


int CPRFTXTDB::NumStrings(LPCTSTR mszStrings) const 
{
	for (int n = 0; mszStrings[0] != 0; n++)
		mszStrings = _tcschr(mszStrings, 0) + 1;
	return(n);
}


//////////////////////////////////////////////////////////////


LPCWSTR CPRFTXTDB::GetStringNum(LPCTSTR mszStrings, int n) const
{
	// JMR: Check for end of strings here
	for (; n > 0; n--) 
		mszStrings = _tcschr(mszStrings, 0) + 1;
	return((LPCWSTR)mszStrings);
}


//////////////////////////////////////////////////////////////


int CPRFTXTDB::GetNumOfCounters(void) const
{
	return(NumStrings((LPCTSTR)sm_mszCounterText) / 2);
}


//////////////////////////////////////////////////////////////


int CPRFTXTDB::CounterNumFromIndex(int nIndex) const
{
	int n;
	_stscanf((LPCTSTR)GetStringNum((LPCTSTR)sm_mszCounterText, nIndex * 2), 
		(LPCTSTR)__TEXT("%d"), &n);
	return(n);
}


//////////////////////////////////////////////////////////////


LPCWSTR CPRFTXTDB::CounterTextFromIndex(int nIndex) const
{
	return(GetStringNum((LPCTSTR)sm_mszCounterText, nIndex * 2 + 1));
}


//////////////////////////////////////////////////////////////


LPCWSTR CPRFTXTDB::CounterTextFromNum(int nNum) const
{
	int nIndex = CounterIndexFromNum(nNum);
	// JMR: Error check for invalid nIndex;
	return(CounterTextFromIndex(nIndex));
}


//////////////////////////////////////////////////////////////


int CPRFTXTDB::CounterIndexFromNum(int nNum) const
{
	for (int nIndex = 0; nIndex < GetNumOfCounters(); nIndex++)
		if (nNum == CounterNumFromIndex(nIndex))
			return(nIndex);
	return(-1);
}


//////////////////////////////////////////////////////////////


int CPRFTXTDB::GetNumOfHelpTexts (void) const {
	return(NumStrings((LPCTSTR)sm_mszHelpText) / 2);
}


//////////////////////////////////////////////////////////////


int CPRFTXTDB::HelpNumFromIndex(int nIndex) const
{
	int n;
	_stscanf((LPCTSTR)__TEXT("%d"), (LPCTSTR)GetStringNum((LPCTSTR)sm_mszHelpText, nIndex * 2), &n);
	return(n);
}


//////////////////////////////////////////////////////////////


LPCWSTR CPRFTXTDB::HelpTextFromIndex(int nIndex) const
{
	return(GetStringNum((LPCTSTR)sm_mszHelpText, nIndex * 2 + 1));
}


//////////////////////////////////////////////////////////////


int CPRFTXTDB::HelpIndexFromNum(int nNum) const
{
	for (int nIndex = 0; nIndex < GetNumOfHelpTexts(); nIndex++)
		if (nNum == HelpNumFromIndex(nIndex))
			return(nIndex);
	return(-1);
}


//////////////////////////////////////////////////////////////


LPCWSTR CPRFTXTDB::HelpTextFromNum(int nNum) const
{
	int nIndex = HelpIndexFromNum(nNum);
	// JMR: Error check for invalid nIndex;
	return(HelpTextFromIndex(nIndex));
}


//////////////////////// End Of File /////////////////////////
