#include "std.h"
#include "xapp.h"

bool DoesFileExist(const TCHAR* szFileName)
{
    CHAR szFile[MAX_PATH];
    WIN32_FILE_ATTRIBUTE_DATA fad;

    Ansi(szFile, szFileName, MAX_PATH);
    return GetFileAttributesEx(szFile, GetFileExInfoStandard, &fad) != 0;
}

void MakePath(TCHAR* szBuf, const TCHAR* szDir, const TCHAR* szFile)
{
	int cch = _tcslen(szDir);

	if (szBuf != szDir)
		CopyChars(szBuf, szDir, cch);

	ASSERT(cch > 0);
	if (szBuf[cch - 1] != '\\')
	{
		szBuf[cch] = '\\';
		cch += 1;
	}

	_tcscpy(szBuf + cch, szFile);
}

void CleanFilePath(char* szPath, const TCHAR* szSrcPath)
{
	char* pchPath = szPath;
	const TCHAR* pch = szSrcPath;
	if (_tcsnicmp(pch, _T("file:"), 5) == 0)
		pch += 5;
	for (; *pch != '\0'; pch += 1)
	{
		if (*pch == '/')
			*pchPath++ = '\\';
		else if (*pch == '?')
			break;
		else
			*pchPath++ = (char)*pch;
	}
	*pchPath = '\0';
}

#if defined(_UNICODE)
void CleanFilePath(TCHAR* szPath, const TCHAR* szSrcPath)
{
	TCHAR* pchPath = szPath;
	const TCHAR* pch = szSrcPath;
	if (_tcsnicmp(pch, _T("file:"), 5) == 0)
		pch += 5;
	for (; *pch != '\0'; pch += 1)
	{
		if (*pch == '/')
			*pchPath++ = '\\';
		else if (*pch == '?')
			break;
		else
			*pchPath++ = (TCHAR)*pch;
	}
	*pchPath = '\0';
}
#endif

void MakeAbsoluteURL2(TCHAR* szBuf, const TCHAR* szBase, const TCHAR* szURL)
{
    ASSERT(theApp.m_dwMainThreadId == GetCurrentThreadId());

	if (_tcschr(szURL, ':') != NULL || (szURL[0] == '\\' && szURL[1] == '\\') || szBase == NULL || (_tcschr(szBase, ':') == NULL && _tcschr(szBase, '/') == NULL))
	{
		_tcscpy(szBuf, szURL);
		return;
	}

	const TCHAR* pchURL = szURL;

	// make relative url absolute...
	_tcscpy(szBuf, szBase);
	TCHAR* pch = _tcsrchr(szBuf, '/');
	if (pch == NULL)
		pch = _tcsrchr(szBuf, '\\');
	if (*pch == NULL)
	{
		pch = szBuf + _tcslen(szBuf) - 1;
	}
	else
	{
		while (pchURL[0] == '.' && pchURL[1] == '.' && (pchURL[2] == '/' || pchURL[2] == '\\'))
		{
			pchURL += 3;

			*pch = '\0';
			pch = _tcsrchr(szBuf, '/');
			if (pch == NULL)
				pch = _tcsrchr(szBuf, '\\');
			if (pch == NULL)
			{
				pch = szBuf + _tcslen(szBuf) - 1;
				break;
			}
		}
	}

	*pch = '/';
	_tcscpy(pch + 1, pchURL);
}

void MakeAbsoluteURL(TCHAR* szBuf, const TCHAR* szBase, const TCHAR* szURL)
{
	if (szBase == NULL)
	{
		MakeAbsoluteURL2(szBuf, theApp.m_szAppDir, szURL);
	}
	else
	{
		TCHAR szBuf2 [1024];
		MakeAbsoluteURL2(szBuf2, theApp.m_szAppDir, szBase);
		MakeAbsoluteURL2(szBuf, szBuf2, szURL);
	}
}

TCHAR g_szCurDir [1024];

void UpdateCurDirFromFile(const TCHAR* szURL)
{
	TCHAR szBuf [1024];
	MakeAbsoluteURL(szBuf, szURL);

	if (_tcschr(szBuf, ':') != NULL || (szBuf[0] == '\\' && szBuf[1] == '\\'))
	{
//		if (_tcsicmp(g_szCurDir, szBuf) != 0)
//		{
			_tcscpy(g_szCurDir, szBuf);
//			TRACE(_T("Setting base directory to %s\n"), g_szCurDir);
//		}

/* REVIEW: File is removed in MakeAbsoluteURL...
		TCHAR* pch = _tcsrchr(g_szCurDir, '/');
		if (pch == NULL)
			pch = _tcsrchr(g_szCurDir, '\\');
		if (pch != NULL)
			*pch = '\0';
		TRACE(_T("Set base URL to: %s\n"), g_szCurDir);
*/
	}
}

void MakeAbsoluteURL(TCHAR* szBuf, const TCHAR* szURL)
{
	TCHAR szCurDir [1024];
	MakeAbsoluteURL2(szCurDir, theApp.m_szAppDir, g_szCurDir);
	MakeAbsoluteURL2(szBuf, szCurDir, szURL);
}

void FindFilePath(TCHAR* szFullPath/*[MAX_PATH]*/, const TCHAR* szFile)
{
	// Make it absolute...
	MakeAbsoluteURL(szFullPath, szFile);

	if (!DoesFileExist(szFullPath))
	{
		TRACE(_T("File not found: %s\n"), szFullPath);
		_tcscpy(szFullPath, theApp.m_szAppDir);
		_tcscat(szFullPath, szFile);
		TRACE(_T("\ttrying %s instead...\n"), szFullPath);
	}
}


DWORD GetUniqueFile(const TCHAR* szBase, TCHAR* szPath/*=NULL*/)
{
	TCHAR szDirBuf [MAX_PATH];
	if (szPath == NULL)
		szPath = szDirBuf;

	for (DWORD dw = 0; ; dw += 1)
	{
		_stprintf(szPath, _T("%s\\%08x"), szBase, dw);
		if (!DoesFileExist(szPath))
			return dw;
	}
}

DWORD CreateUniqueDir(const TCHAR* szBase)
{
	TCHAR szDirBuf [MAX_PATH];
	DWORD dw = GetUniqueFile(szBase, szDirBuf);
	TRACE(_T("CreateDirectory: %s\n"), szDirBuf);
	VERIFY(XAppCreateDirectory(szDirBuf));

	return dw;
}

int GetFileBlocks(const TCHAR* szFilePath, int nBlockSize)
{
	HANDLE hFile = XAppCreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return 0;

	int nBytes = GetFileSize(hFile, NULL);

	CloseHandle(hFile);

	return (nBytes + nBlockSize - 1) / nBlockSize;
}

int GetDirectoryBlocks(const TCHAR* szDirPath, int nBlockSize, bool bRecursive, HANDLE hCancelEvent)
{
    if (hCancelEvent && WaitForSingleObject(hCancelEvent, 0) == WAIT_OBJECT_0)
    {
        return -1;
    }

	TCHAR szBuf [MAX_PATH];
	MakePath(szBuf, szDirPath, _T("*.*"));
	
    WIN32_FIND_DATA fd;

	//
	// nTotalBlocks starts out with 1 to take into account the size taken up from the directory entry itself.
	//
	int nTotalBlocks = 1;
    bool bCancel = false;

	char sszBuf [MAX_PATH];
	Ansi(sszBuf, szBuf, MAX_PATH);
	HANDLE hFind = FindFirstFile(sszBuf, &fd);

	if (hFind == INVALID_HANDLE_VALUE)
		return 0;

	do
	{
        if (hCancelEvent && WaitForSingleObject(hCancelEvent, 0) == WAIT_OBJECT_0)
        {
            bCancel = true;
            break;
        }

		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
		{
			nTotalBlocks += (fd.nFileSizeLow + nBlockSize - 1) / nBlockSize;
		}
		else if (bRecursive && fd.cFileName[0] != '.')
		{
			TCHAR szFileName [MAX_PATH];
			Unicode(szFileName, fd.cFileName, countof(szFileName));
			MakePath(szBuf, szDirPath, szFileName);
			nTotalBlocks += GetDirectoryBlocks(szBuf, nBlockSize, true, hCancelEvent);
		}
	}
	while (FindNextFile(hFind, &fd));
	FindClose(hFind);

	return bCancel ? -1 : nTotalBlocks;
}

int GetFileSize(const TCHAR* szFilePath)
{
	HANDLE hFile = XAppCreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return 0;

	int nFileSize = GetFileSize(hFile, NULL);

	CloseHandle(hFile);

	return nFileSize;
}

#ifdef _WINDOWS // TODO: Need Xbox versions too!

bool DeleteDirectory(const TCHAR* szPath)
{
	TCHAR szBuf [MAX_PATH];
	_stprintf(szBuf, _T("%s\\*.*"), szPath);

	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile(szBuf, &fd);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (_tcscmp(fd.cFileName, _T(".")) == 0 || _tcscmp(fd.cFileName, _T("..")) == 0)
				continue;

			_stprintf(szBuf, _T("%s\\%s"), szPath, fd.cFileName);

			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				if (!DeleteDirectory(szBuf))
					return false;
			}
			else
			{
				if (!DeleteFile(szBuf))
					return false;
			}
		}
		while (FindNextFile(h, &fd));
		FindClose(h);
	}

	return RemoveDirectory(szPath) != FALSE;
}

bool CopyDirectory(const TCHAR* szSrcPath, const TCHAR* szDestPath)
{
	CreateDirectory(szDestPath, NULL);

	TCHAR szBuf [MAX_PATH];
	_stprintf(szBuf, _T("%s\\*.*"), szSrcPath);

	WIN32_FIND_DATA fd;
	HANDLE h = FindFirstFile(szBuf, &fd);
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (_tcscmp(fd.cFileName, _T(".")) == 0 || _tcscmp(fd.cFileName, _T("..")) == 0)
				continue;

			_stprintf(szBuf, _T("%s\\%s"), szSrcPath, fd.cFileName);

			TCHAR szBuf2 [MAX_PATH];
			_stprintf(szBuf2, _T("%s\\%s"), szDestPath, fd.cFileName);

			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				if (!CopyDirectory(szBuf, szBuf2))
					return false;
			}
			else
			{
				if (!CopyFile(szBuf, szBuf2, FALSE))
					return false;
			}
		}
		while (FindNextFile(h, &fd));
		FindClose(h);
	}

	return true;
}

#endif
