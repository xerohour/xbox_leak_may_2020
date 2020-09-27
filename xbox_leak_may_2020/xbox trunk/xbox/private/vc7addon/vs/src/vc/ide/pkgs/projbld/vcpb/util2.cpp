#include "stdafx.h"
#pragma hdrstop

#include "util2.h"

static bool bPlatformInited = false;
static DWORD dwPlatform;

/*** GETEXETYPE
 *
 * PURPOSE: Given an open handle to file determine it's executable type if any.
 *
 * INPUT:
 *      hFile - Handle to an open file
 *
 * OUTPUT:
 *      EXT   - EXecutable Type in return value
 *
 * EXCEPTIONS:
 *
 * IMPLEMENTATION:
 *
 *      This function will save the file pointer and restore it on exit.
 *      A return type of EXE_NONE indicates either an error occured while
 *      seeking/reading or the file wasn't of any recognized EXE type.
 *
 *		If the EXE type is EXE_NT, then *pwSubsystem will indicate
 *		the subsystem type (IMAGE_SUBSYSTEM_...).  If the EXE type is
 *		NOT EXE_NT, the value *pwSubsystem on return is meaningless.
 *
 *		You can pass NULL for pwSubsystem, in which case no value will
 *		be returned through that pointer.
 *
 ****************************************************************************/
#define NE_UNKNOWN	0x0
#define NE_OS2		0x1
#define NE_WINDOWS	0x2
#define NE_DOS		0x3

EXT GetExeType		
(
	LPCOLESTR	lpstrEXEFilename,
	WORD *	pwSubsystem
)
{
	IMAGE_DOS_HEADER        doshdr;
	IMAGE_OS2_HEADER        os2hdr;
	IMAGE_NT_HEADERS        nthdr;
	HANDLE					hFile;
	BOOL					fGotNE = FALSE;
	DWORD					dwBytesRead;

	/*
	** open up our EXE
	*/
	hFile = CreateFileW(
				lpstrEXEFilename,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL, 
				NULL
			);
	if (hFile == INVALID_HANDLE_VALUE) {
		return EXE_FAIL;
	}

    /*
    ** Try to read a an MZ header
    */
    ReadFile(hFile, &doshdr, sizeof (IMAGE_DOS_HEADER),	&dwBytesRead, NULL);

	if (dwBytesRead != sizeof (IMAGE_DOS_HEADER))
    {
		CloseHandle(hFile);
        return EXE_NONE;
    }

    /*
    ** If it was an MZ header and if the address of the relocation table
    ** isn't 0x0040 then it is a DOS non segmented executable.
    */
    if (doshdr.e_magic == IMAGE_DOS_SIGNATURE)
    {
    	if (doshdr.e_lfarlc != 0x0040)
    	{
			CloseHandle(hFile);
			return EXE_DOS;
		}
		else
			fGotNE = TRUE;
    }

    /*
	** Seek now to either the beginning of the file if there was no MZ stub or
	** to the new header specified in the MZ header.
    */
    if (SetFilePointer(hFile, fGotNE ? doshdr.e_lfanew : 0L, NULL, FILE_BEGIN) == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
        return EXE_NONE;
	}

    ReadFile(hFile, &os2hdr, sizeof (IMAGE_OS2_HEADER), &dwBytesRead, NULL);
	if (dwBytesRead != sizeof (IMAGE_OS2_HEADER))
	{
		CloseHandle(hFile);
        return EXE_NONE;
    }

    if (os2hdr.ne_magic == IMAGE_OS2_SIGNATURE)
    {
		EXT	ext;

		switch(os2hdr.ne_exetyp)
		{
			case NE_OS2:
				ext = EXE_OS2_NE;
				break;

			case NE_WINDOWS:
				ext = EXE_WIN;
				break;

			case NE_DOS:
				ext = EXE_DOS;
				break;

			default:
				ext = EXE_NONE;
				break;
		}

		CloseHandle(hFile);
        return ext;
    }
    else if (os2hdr.ne_magic == IMAGE_OS2_SIGNATURE_LE)
    {
		EXT ext;

		ext = (os2hdr.ne_exetyp == NE_UNKNOWN) ? EXE_NONE : EXE_OS2_LE;

		CloseHandle(hFile);
        return EXE_OS2_LE;
    }


    /*
    ** Re-seek to the header, read it as an NT header and check for PE exe.
    */

    if (SetFilePointer(hFile, doshdr.e_lfanew, NULL, FILE_BEGIN) == 0xFFFFFFFF)
	{
		CloseHandle(hFile);
        return EXE_NONE;
    }

    ReadFile(hFile, &nthdr, sizeof (IMAGE_NT_HEADERS), &dwBytesRead, NULL);
	if (dwBytesRead != sizeof (IMAGE_NT_HEADERS))
    {
		CloseHandle(hFile);
        return EXE_NONE;
    }

    if (nthdr.Signature == IMAGE_NT_SIGNATURE)
    {
		// Return the subsystem as well.
		//
		// [CUDA 3557: 4/21/93 mattg]
		if (pwSubsystem != NULL)
		{
			if (nthdr.FileHeader.SizeOfOptionalHeader < IMAGE_SIZEOF_NT_OPTIONAL_HEADER)
				*pwSubsystem = IMAGE_SUBSYSTEM_UNKNOWN;
			else
				*pwSubsystem = nthdr.OptionalHeader.Subsystem;
		}
		CloseHandle(hFile);
        return EXE_NT;
    }


    /*
    ** Well, It isn't an Executable type that we know about....
    */

	CloseHandle(hFile);

    return EXE_NONE;

} /* GetExeType */

void InitPlatform()
{
	if( bPlatformInited )
	    return;

	bPlatformInited = true;
	// what OS are we running on?
	OSVERSIONINFO osver;
	memset( &osver, 0, sizeof(osver) );
	osver.dwOSVersionInfoSize = sizeof(osver);
	GetVersionEx( &osver );
	dwPlatform = osver.dwPlatformId;
	return;
}

//-----------------------------------------------------------------------------
// Handle SendMessage as SendMessageW on NT/Win2k
//-----------------------------------------------------------------------------
LRESULT UtilSendMessageW( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    	InitPlatform();
	// if we're running on NT, use Unicode 
	if( dwPlatform == VER_PLATFORM_WIN32_NT )
		return SendMessageW( hWnd, Msg, wParam, lParam );
	// otherwise, Win9x, we can't use Unicode
	else
		return SendMessage( hWnd, Msg, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Handle SendMessage as SendMessageW on NT/Win2k where the LPARAM is a string
// (hence ANSI on Win9x and Unicode on NT/Win2k...)
//-----------------------------------------------------------------------------
LRESULT UtilSendMessageWString( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    	InitPlatform();
	// if we're running on NT, use Unicode 
	if( dwPlatform == VER_PLATFORM_WIN32_NT )
	{
		return SendMessageW( hWnd, Msg, wParam, lParam );
	}
	// otherwise, Win9x, we can't use Unicode
	else
	{
		// convert the string to ANSI
		USES_CONVERSION;
		char *szString = W2A( (wchar_t*)lParam );
		return SendMessageA( hWnd, Msg, wParam, (LPARAM)szString );
	}
}

FILE *fopenW( const wchar_t *szFileName, const wchar_t* szAttributes)
{
    	FILE *hFile = NULL;
    	InitPlatform();
	if( dwPlatform == VER_PLATFORM_WIN32_NT )
	{
		hFile = _wfopen( szFileName, szAttributes );
	}
	// otherwise, Win9x, we can't use Unicode
	else
	{
		// convert the string to ANSI
		USES_CONVERSION;
		char * szFN = W2A( (wchar_t*)szFileName );
		char * szAT = W2A( (wchar_t*)szAttributes );
		hFile = fopen( szFN, szAT );
	}
	return hFile;
}


bool CanCreateProcess( LPCOLESTR szExe )
{
	// check \\HKCU\Software\Microsoft\Windows\CurrentVersion\Policies\Explorer\RestrictRun
	// to see if we can execute this process or not

	LONG lRet;
	HKEY hSectionKey = NULL;
	DWORD nType = REG_SZ;
	DWORD nSize = 2047;
	wchar_t szRet[2048] = {0};

	// open the reg key
	lRet = RegOpenKeyExW( HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer", 0, KEY_READ, &hSectionKey );

	// if we found it, read the value
	if( hSectionKey && lRet == ERROR_SUCCESS )
		lRet = RegQueryValueExW( hSectionKey, L"RestrictRun", NULL, &nType, (LPBYTE)szRet, &nSize );
	// else return true, the key doesn't exist, so we can run whatever we
	// please
	else
	{
		if (hSectionKey)
			RegCloseKey(hSectionKey);
		return true;
	}

	// close the key
	RegCloseKey( hSectionKey );

	// the value doesn't exist, so we can run whatever we please
	if( lRet != ERROR_SUCCESS )
		return true;

	// is szExe in the key?
	if( wcsistr( szRet, (LPOLESTR)szExe ) )
		return true;
	else
		return false;
}

LPCOLESTR wcsistr(LPCOLESTR szSearch, LPCOLESTR szReqd)
{
	// case insensitive string inclusion (like wcsstr but insensitive)
	LPCOLESTR pszRet = NULL;

	wchar_t *pszSrchTmp;
	wchar_t *pszReqdTmp;

	pszSrchTmp = (wchar_t *)malloc(sizeof(wchar_t)*(wcslen(szSearch)+1));
	pszReqdTmp = (wchar_t *)malloc(sizeof(wchar_t)*(wcslen(szReqd)+1));
	if(pszSrchTmp  && pszReqdTmp )
	{
	    wchar_t *pstr;
    
	    wcslwr(wcscpy(pszSrchTmp, szSearch));
	    wcslwr(wcscpy(pszReqdTmp, szReqd));
    
	    pstr = wcsstr(pszSrchTmp, pszReqdTmp);
	    if (pstr)
		    pszRet = (pstr - pszSrchTmp) + szSearch;

	}
	free(pszSrchTmp);
	free(pszReqdTmp);

	return pszRet;
}

LPSTR stristr(char *szSearch, char *szReqd)
{
	// case insensitive string inclusion (like _ftcsstr but insensitive)
	unsigned char *pszRet = NULL;

	unsigned char *pszSrchTmp;
	unsigned char *pszReqdTmp;

	pszSrchTmp = (unsigned char *)malloc(sizeof(char)*(_mbslen((unsigned char *)szSearch)+1));
	pszReqdTmp = (unsigned char *)malloc(sizeof(char)*(_mbslen((unsigned char *)szReqd)+1));

	if(pszSrchTmp  && pszReqdTmp )
	{
	    unsigned char *pstr;
    
	    _mbslwr(_mbscpy(pszSrchTmp, (unsigned char *)szSearch));
	    _mbslwr(_mbscpy(pszReqdTmp, (unsigned char *)szReqd));
    
	    pstr = _mbsstr(pszSrchTmp, pszReqdTmp);
	    if (pstr)
		    pszRet = (unsigned char *)szSearch + (pstr - pszSrchTmp);
	}
    
	free(pszSrchTmp);
	free(pszReqdTmp);

	return (LPSTR)pszRet;
}

