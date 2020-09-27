#include "mapistuff.h"

ULONG (FAR PASCAL *lpfnMAPILogon)(ULONG_PTR, LPSTR, LPSTR, FLAGS, ULONG, LPLHANDLE);
ULONG (PASCAL *lpfnMAPISendMail)(ULONG, ULONG_PTR, MapiMessage*, FLAGS, ULONG);
ULONG (PASCAL *lpfnMAPIResolveName)(LHANDLE, ULONG_PTR, LPTSTR, FLAGS, ULONG, MapiRecipDesc **);
ULONG (FAR PASCAL *lpfnMAPILogoff)(LHANDLE, ULONG_PTR, FLAGS, ULONG);
ULONG (FAR PASCAL *lpfnMAPIFreeBuffer)(LPVOID);
HINSTANCE hInstMapi = NULL;

BOOL SendOffFailure(HWND hWnd, TCHAR *pszToList, TCHAR *pszTitle, TCHAR *pszMessage)
{
	LHANDLE lhSession;
	ULONG lResult = 0;
	MapiMessage mmmessage;
	lpMapiRecipDesc rdList = NULL;
	DWORD i = 0;

	memset(&mmmessage, 0, sizeof(mmmessage));

	// Set up procedure pointers
	if (!GetLibraryPointers(hWnd))
		return FALSE;

	// Log on to existing session
	lResult = lpfnMAPILogon(0, NULL, NULL, 0, 0, &lhSession);
	if (lResult != SUCCESS_SUCCESS)
	{
		// We need outlook up and running to send mail
		// Maybe I write the code to do the connect when I get some time
		MessageBox(hWnd, 
					TEXT("Unable to Logon to an existing MAPI session.  Make sure you have Outlook started!!!"), 
					TEXT("Send Mail Error!"), 
					MB_OK|MB_ICONERROR);
		FreeLibrary(hInstMapi);
		return FALSE;
	}

	mmmessage.ulReserved = 0;
	mmmessage.lpszMessageType = NULL;
	mmmessage.lpszSubject = pszTitle;
	mmmessage.lpszNoteText = pszMessage;
	mmmessage.flFlags = MAPI_SENT;
	mmmessage.lpOriginator = NULL;
	mmmessage.nFileCount = 0;

	mmmessage.nRecipCount = CountRecips(pszToList);
	if (mmmessage.nRecipCount == 0)
	{
		MessageBox(hWnd, 
					TEXT("No Valid Names where specifed!!!"), 
					TEXT("Send Mail Failure!"), 
					MB_OK|MB_ICONERROR);
		lpfnMAPILogoff(0, lhSession, 0, 0);
		FreeLibrary(hInstMapi);
		return FALSE;
	}
	else if (mmmessage.nRecipCount == 1)
	{
		lResult = lpfnMAPIResolveName(lhSession,
										0,
										pszToList,
										0,
										0,
										&mmmessage.lpRecips);
		if (lResult != SUCCESS_SUCCESS)
		{
			TCHAR ErrorString[256];
			sprintf(ErrorString, "Unable to Resolve Names properly!!! Got 0x%08x", lResult);
			MessageBox(hWnd, 
						ErrorString, 
						TEXT("Fatal Error!"), 
						MB_OK|MB_ICONERROR);
			lpfnMAPILogoff(0, lhSession, 0, 0);
			FreeLibrary(hInstMapi);
			return FALSE;
		}
	}
	else
	{
		TCHAR *token = NULL;
		DWORD index = 0;

		rdList = (lpMapiRecipDesc) calloc(mmmessage.nRecipCount, sizeof (MapiRecipDesc));
		if (!rdList)
		{
			MessageBox(hWnd, 
						TEXT("Unable to allocate memory for names!!!"), 
						TEXT("Fatal Error!"), 
						MB_OK|MB_ICONERROR);
			lpfnMAPILogoff(0, lhSession, 0, 0);
			FreeLibrary(hInstMapi);
			return FALSE;
		}

		
		token = _tcstok(pszToList, _T(";"));
		while( token != NULL )
		{
			lResult = lpfnMAPIResolveName(lhSession,
											0,
											token,
											0,
											0,
											&mmmessage.lpRecips);
			if (lResult != SUCCESS_SUCCESS)
			{
				TCHAR BadNames[MAX_PATH];

				_stprintf(BadNames, _T("Unable to resolve %s properly! Failing Send"), token);

				MessageBox(hWnd, 
							BadNames, 
							TEXT("Name Resolution Error"), 
							MB_OK|MB_ICONERROR);
				
				lpfnMAPILogoff(0, lhSession, 0, 0);
				FreeLibrary(hInstMapi);
				for (i = 0; i < mmmessage.nRecipCount; i++)
				{
					if (rdList[i].lpEntryID)
						free(rdList[i].lpEntryID);
					if (rdList[i].lpszAddress)
						free(rdList[i].lpszAddress);
					if (rdList[i].lpszName)
						free(rdList[i].lpszName);
				}
				free(rdList);
				return FALSE;
			}
			else
			{
				if (mmmessage.lpRecips->lpEntryID)
				{
					rdList[index].lpEntryID = malloc(mmmessage.lpRecips->ulEIDSize);
					if (!rdList[index].lpEntryID)
					{
						MessageBox(hWnd, 
									TEXT("Unable to allocate memory for lpEntryID"), 
									TEXT("Memory Allocation Error"), 
									MB_OK|MB_ICONERROR);

						lpfnMAPILogoff(0, lhSession, 0, 0);
						FreeLibrary(hInstMapi);
						free(rdList);
						return FALSE;
					}
					memcpy(rdList[index].lpEntryID, mmmessage.lpRecips->lpEntryID, mmmessage.lpRecips->ulEIDSize);
				}

				if (mmmessage.lpRecips->lpszAddress)
				{
					rdList[index].lpszAddress = _tcsdup(mmmessage.lpRecips->lpszAddress);
					if (!rdList[index].lpszAddress)
					{
						MessageBox(hWnd, 
									TEXT("Unable to allocate memory for lpszAddress"), 
									TEXT("Memory Allocation Error"), 
									MB_OK|MB_ICONERROR);

						lpfnMAPILogoff(0, lhSession, 0, 0);
						FreeLibrary(hInstMapi);
						for (i = 0; i < mmmessage.nRecipCount; i++)
						{
							if (rdList[i].lpEntryID)
								free(rdList[i].lpEntryID);
							if (rdList[i].lpszAddress)
								free(rdList[i].lpszAddress);
							if (rdList[i].lpszName)
								free(rdList[i].lpszName);
						}
						free(rdList);
						return FALSE;
					}
				}

				if (mmmessage.lpRecips->lpszName)
				{
					rdList[index].lpszName = _tcsdup(mmmessage.lpRecips->lpszName);
					if (!rdList[index].lpszName)
					{
						MessageBox(hWnd, 
									TEXT("Unable to allocate memory for lpszName"), 
									TEXT("Memory Allocation Error"), 
									MB_OK|MB_ICONERROR);

						lpfnMAPILogoff(0, lhSession, 0, 0);
						FreeLibrary(hInstMapi);
						for (i = 0; i < mmmessage.nRecipCount; i++)
						{
							if (rdList[i].lpEntryID)
								free(rdList[i].lpEntryID);
							if (rdList[i].lpszAddress)
								free(rdList[i].lpszAddress);
							if (rdList[i].lpszName)
								free(rdList[i].lpszName);
						}
						free(rdList);
						return FALSE;
					}
				}

				rdList[index].ulEIDSize = mmmessage.lpRecips->ulEIDSize;
				rdList[index].ulRecipClass = MAPI_TO;
				rdList[index].ulReserved = mmmessage.lpRecips->ulReserved;
				lpfnMAPIFreeBuffer(mmmessage.lpRecips);
			}

			index++;			
			token = _tcstok(NULL, _T(";"));
		}

		mmmessage.lpRecips = rdList;		
	}

	// Send the message
	lResult = lpfnMAPISendMail(0, 0, &mmmessage, 0, 0);

	// Cleanup
	lpfnMAPILogoff(0, lhSession, 0, 0);

	if (mmmessage.nRecipCount == 1)
		lpfnMAPIFreeBuffer(mmmessage.lpRecips);
	else
	{
		for (i = 0; i < mmmessage.nRecipCount; i++)
		{
			if (rdList[i].lpEntryID)
				free(rdList[i].lpEntryID);
			if (rdList[i].lpszAddress)
				free(rdList[i].lpszAddress);
			if (rdList[i].lpszName)
				free(rdList[i].lpszName);
		}
		free(rdList);
	}

	FreeLibrary(hInstMapi);

	if (lResult != SUCCESS_SUCCESS)
	{
		MessageBox(hWnd, 
					TEXT("SendMail Failed!!!"), 
					TEXT("Fatal Error!"), 
					MB_OK|MB_ICONERROR);
		return FALSE;
	}
	else
    {  
		MessageBox(hWnd, 
					TEXT("Your Mail was sent successfully."), 
					TEXT("Mail Send Successful"), 
					MB_OK);
		return TRUE;
    }

}

BOOL GetLibraryPointers(HWND hWnd)
{

	hInstMapi = LoadLibrary("Mapi32.dll");
	if(hInstMapi == NULL)
	{
		MessageBox(hWnd, 
					TEXT("Unable to Load MAPI32.dll!!!"), 
					TEXT("Fatal Error!"), 
					MB_OK|MB_ICONERROR);
		return FALSE;
	}

	// Find the addresses of functions 
	(FARPROC)lpfnMAPILogon = GetProcAddress(hInstMapi, "MAPILogon");
	if (lpfnMAPILogon == NULL)
	{
		MessageBox(hWnd, 
					TEXT("Unable to find function MAPILogon!!!"), 
					TEXT("Fatal Error!"), 
					MB_OK|MB_ICONERROR);
		FreeLibrary(hInstMapi);
		return FALSE;
	}
	
		(FARPROC)lpfnMAPILogoff = GetProcAddress(hInstMapi, "MAPILogoff");
	if (lpfnMAPILogoff == NULL)
	{
		MessageBox(hWnd, 
					TEXT("Unable to find function MAPILogoff!!!"), 
					TEXT("Fatal Error!"), 
					MB_OK|MB_ICONERROR);
		FreeLibrary(hInstMapi);
		return FALSE;
	}

	(FARPROC)lpfnMAPIFreeBuffer = GetProcAddress(hInstMapi, "MAPIFreeBuffer");
	if (lpfnMAPIFreeBuffer == NULL)
	{
		MessageBox(hWnd, 
					TEXT("Unable to find function MAPIFreeBuffer!!!"), 
					TEXT("Fatal Error!"), 
					MB_OK|MB_ICONERROR);
		FreeLibrary(hInstMapi);
		return FALSE;
	}

	(FARPROC)lpfnMAPIResolveName = GetProcAddress(hInstMapi, "MAPIResolveName");
	if (lpfnMAPIResolveName == NULL)
	{
		MessageBox(hWnd, 
					TEXT("Unable to find function MAPIResolveName!!!"), 
					TEXT("Fatal Error!"), 
					MB_OK|MB_ICONERROR);
		FreeLibrary(hInstMapi);
		return FALSE;
	} 

	(FARPROC)lpfnMAPISendMail = GetProcAddress(hInstMapi, "MAPISendMail");
	if (lpfnMAPISendMail == NULL)
	{
		MessageBox(hWnd, 
					TEXT("Unable to find function MAPIResolveName!!!"), 
					TEXT("Fatal Error!"), 
					MB_OK|MB_ICONERROR);
		FreeLibrary(hInstMapi);
		return FALSE;
	}

	return TRUE;	
}

DWORD CountRecips(PTCHAR pszToList)
{
	DWORD i = 0;
	PTCHAR ptr = pszToList;

	if ((!ptr)||(ptr[0] == TEXT('\0')))
		return 0;

	ptr = pszToList + _tcslen(pszToList) - 1;
	// rear trim
	while ((ptr >= pszToList)&&((ptr[0] == TEXT(';'))||(ptr[0] == TEXT(' '))||
			(ptr[0] == TEXT('/r'))||(ptr[0] == TEXT('/n'))||
			(ptr[0] == TEXT('/t'))||(ptr[0] == TEXT(':'))))
	{
		ptr[0] = TEXT('\0');
		ptr--;
	}

	ptr = pszToList;
	// front trim
	while ((ptr[0] == TEXT(';'))||(ptr[0] == TEXT(' '))||
			(ptr[0] == TEXT('/r'))||(ptr[0] == TEXT('/n'))||
			(ptr[0] == TEXT('/t'))||(ptr[0] == TEXT(':')))
		_tcscpy(ptr, ptr + 1);

	// remove spaces
	while (ptr = _tcschr(pszToList, TEXT(' ')))
		_tcscpy(ptr, ptr + 1);

	ptr = pszToList;
	while (ptr = _tcschr(ptr, TEXT(';')))
	{
		i++;
		ptr++;
	}

	if (!_tcslen(pszToList))
		return 0;

	return i + 1;
}