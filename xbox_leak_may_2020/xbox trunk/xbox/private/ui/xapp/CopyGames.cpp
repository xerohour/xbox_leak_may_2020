#include "std.h"
#include "XApp.h"
#include "Node.h"
#include "FileUtil.h"
#include "TitleCollection.h"
#include "CopyGames.h"

const TCHAR szTitleDataXBX [] = _T("TitleMeta.xbx");
const TCHAR szTitleImageXBX [] = _T("TitleImage.xbx");
const TCHAR szSaveDataXBX [] = _T("SaveMeta.xbx");
const TCHAR szSaveImageXBX [] = _T("SaveImage.xbx");


struct CCopyGame
{
	TCHAR* m_szTitleID;
	TCHAR* m_szGameDir;
	FILETIME m_time;
	bool m_bNeedToAdd;
};

CGameCopier::CGameCopier()
{
	m_hThread = NULL;
	m_progress = 0.0f;
	m_bInternalError = false;
    m_bAlreadyExists = false;
	m_error = false;
	m_done = false;
	m_nTotalBlocks = 0;
	m_nCopiedBlocks = 0;
	m_szDestRoot = NULL;
	m_szSrcRoot = NULL;
	m_rgCopyGame = NULL;
	m_nCopyGameAlloc = 0;
	m_nCopyGameCount = 0;
	m_buffer = NULL;
	m_srcDevUnit = -1;
	m_destDevUnit = -1;
}

CGameCopier::~CGameCopier()
{
#ifndef _XBOX
	if (m_hThread != NULL)
		TerminateThread(m_hThread, 0);
#endif

	RemoveAll();
}

void CGameCopier::RemoveAll()
{
	delete [] m_szDestRoot;
	m_szDestRoot = NULL;

	delete [] m_szSrcRoot;
	m_szSrcRoot = NULL;

	for (int i = 0; i < m_nCopyGameCount; i += 1)
	{
		delete [] m_rgCopyGame[i].m_szTitleID;
		delete [] m_rgCopyGame[i].m_szGameDir;
	}

	delete [] m_rgCopyGame;
	m_rgCopyGame = NULL;
	m_nCopyGameAlloc = 0;
	m_nCopyGameCount = 0;
	m_nTotalBlocks = 0;

	delete [] m_buffer;
	m_buffer = NULL;
}

void CGameCopier::Finish()
{
	for (int i = 0; i < m_nCopyGameCount; i += 1)
	{
		if (m_rgCopyGame[i].m_bNeedToAdd)
		{
            // Remove it if it already exists
            if (m_bAlreadyExists)
            {
                g_titles[m_destDevUnit].RemoveSavedGame(m_rgCopyGame[i].m_szTitleID, m_rgCopyGame[i].m_szGameDir);
            }

			g_titles[m_destDevUnit].AddSavedGame(m_rgCopyGame[i].m_szTitleID, m_rgCopyGame[i].m_szGameDir, m_rgCopyGame[i].m_time);
			m_rgCopyGame[i].m_bNeedToAdd = false;
		}
	}

	RemoveAll();
}

DWORD CALLBACK CGameCopier::StartThread(LPVOID pvContext)
{
	CGameCopier *pThis = (CGameCopier*)pvContext;
	return pThis->ThreadProc();
}

void CGameCopier::CopyGame(int nCopyGame/*const TCHAR* szTitleID, const TCHAR* szGameDir*/)
{
	const TCHAR* szTitleID = m_rgCopyGame[nCopyGame].m_szTitleID;
	const TCHAR* szGameDir = m_rgCopyGame[nCopyGame].m_szGameDir;

	TRACE(_T("CopyGame(%s, %s)\n"), szTitleID, szGameDir);
	ASSERT(!m_bInternalError);

	TCHAR szSrcPath [MAX_PATH];
	TCHAR szDestPath [MAX_PATH];
	bool bNewTitle = false;

	// BLOCK: Make sure the destination has the title directory
	{
		MakePath(szDestPath, m_szDestRoot, szTitleID);
		bNewTitle = CreateDirectory(szDestPath);
	}

	// BLOCK: Copy the title's titleimage.xbx file
	{
		MakePath(szDestPath, m_szDestRoot, szTitleID);
		MakePath(szDestPath, szDestPath, szTitleImageXBX);

		if (bNewTitle || GetFileSize(szDestPath) == 0)
		{
			MakePath(szSrcPath, m_szSrcRoot, szTitleID);
			MakePath(szSrcPath, szSrcPath, szTitleImageXBX);
		
			CopyFile(szSrcPath, szDestPath);
		}
	}

	// BLOCK: Copy the title's default save game image file
	{
		MakePath(szDestPath, m_szDestRoot, szTitleID);
		MakePath(szDestPath, szDestPath, szSaveImageXBX);

		MakePath(szSrcPath, m_szSrcRoot, szTitleID);
		MakePath(szSrcPath, szSrcPath, szSaveImageXBX);
		
		if (DoesFileExist(szSrcPath) && GetFileSize(szDestPath) == 0)
		{
			CopyFile(szSrcPath, szDestPath);
		}
	}

	// Reset this in case one of those less important files failed to copy...
	m_bInternalError = false;
    m_bAlreadyExists = false;

	// BLOCK: Create a saved game directory
	{
		MakePath(szDestPath, m_szDestRoot, szTitleID);
		MakePath(szDestPath, szDestPath, szGameDir);

		if (!CreateDirectory(szDestPath))
		{
            if (GetLastError() == ERROR_ALREADY_EXISTS)
            {
                m_bAlreadyExists = true;

                // Delete all files in this saved game
                DeleteDirectory(szDestPath, false);
            }
            else
            {
    			m_bInternalError = true;
    			return;
            }
		}

		TRACE(_T("Creating game in %s\n"), szDestPath);

		HANDLE hFile = XAppCreateFile(szDestPath, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
		if (hFile != INVALID_HANDLE_VALUE)
		{
			VERIFY(SetFileTime(hFile, &m_rgCopyGame[nCopyGame].m_time, &m_rgCopyGame[nCopyGame].m_time, &m_rgCopyGame[nCopyGame].m_time));
			VERIFY(CloseHandle(hFile));
		}
#ifdef _DEBUG
		else
		{
			TRACE(_T("Cannot open directory (%s) to set times (%d)\n"), szDestPath, GetLastError());
		}
#endif
	}

	MakePath(szSrcPath, m_szSrcRoot, szTitleID);
	MakePath(szSrcPath, szSrcPath, szGameDir);

	if (!CopyDirectory(szSrcPath, szDestPath))
	{
		TRACE(_T("\001The copy didn't complete; removing what we did copy...\n"));

		// The copy didn't complete; remove what we did copy...
		if (bNewTitle)
		{
			TRACE(_T("it was a new title; deleting: %s %s\n"), m_szDestRoot, szTitleID);
			MakePath(szDestPath, m_szDestRoot, szTitleID);
		}

		DeleteDirectory(szDestPath);
		return;
	}

	// BLOCK: Update the directory name
	{
		TCHAR* pch = _tcsrchr(szDestPath, '\\');
		ASSERT(pch != NULL);
		pch += 1;

		delete [] m_rgCopyGame[nCopyGame].m_szGameDir;
		m_rgCopyGame[nCopyGame].m_szGameDir = new TCHAR [_tcslen(pch) + 1];
		_tcscpy(m_rgCopyGame[nCopyGame].m_szGameDir, pch);
	}

	// BLOCK: Copy the title's meta.xbx file (last in case MU is pulled out causing an error we can't clean up)
	{
		MakePath(szDestPath, m_szDestRoot, szTitleID);
		MakePath(szDestPath, szDestPath, szTitleDataXBX);

		if (bNewTitle || GetFileSize(szDestPath) == 0)
		{
			MakePath(szSrcPath, m_szSrcRoot, szTitleID);
			MakePath(szSrcPath, szSrcPath, szTitleDataXBX);

			CopyFile(szSrcPath, szDestPath);
		}
	}
}

bool CGameCopier::CreateDirectory(const TCHAR* szDir)
{
	TRACE(_T("CreateDirectory(%s)\n"), szDir);

#ifdef _XBOX
	char szDir2 [MAX_PATH];
	Ansi(szDir2, szDir, MAX_PATH);
#else
#define szDir2 szDir
#endif

	return ::CreateDirectory(szDir2, NULL) != FALSE;
}

bool CGameCopier::CopyDirectory(const TCHAR* szSrcPath, const TCHAR* szDestPath)
{
	TRACE(_T("CopyDirectory(%s, %s)\n"), szSrcPath, szDestPath);

	CreateDirectory(szDestPath);

	TCHAR szBuf [MAX_PATH];
	MakePath(szBuf, szSrcPath, _T("*.*"));

	WIN32_FIND_DATA fd;
#ifdef _XBOX
	char szBufX [MAX_PATH];
	Ansi(szBufX, szBuf, MAX_PATH);
	HANDLE h = FindFirstFile(szBufX, &fd);
#else
	HANDLE h = FindFirstFile(szBuf, &fd);
#endif
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
#ifdef _XBOX
			TCHAR szFileName [MAX_PATH];
			Unicode(szFileName, fd.cFileName, MAX_PATH);
#else
			const TCHAR* szFileName = fd.cFileName;
#endif
			if (_tcscmp(szFileName, _T(".")) == 0 || _tcscmp(szFileName, _T("..")) == 0)
				continue;

			MakePath(szBuf, szSrcPath, szFileName);

			TCHAR szBuf2 [MAX_PATH];
			MakePath(szBuf2, szDestPath, szFileName);

			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				if (!CopyDirectory(szBuf, szBuf2))
				{
					TRACE(_T("CopyDirectory(%s, %s) failed\n"), szBuf, szBuf2);
					FindClose(h);
					m_bInternalError = true;
					return false;
				}
			}
			else
			{
				if (!CopyFile(szBuf, szBuf2))
				{
					TRACE(_T("CopyFile(%s, %s) failed\n"), szBuf, szBuf2);
					FindClose(h);
					m_bInternalError = true;
					return false;
				}
			}
		}
		while (FindNextFile(h, &fd));
		FindClose(h);
	}

	return true;
}

bool CGameCopier::CopyFile(const TCHAR* szSrcFile, const TCHAR* szDestFile)
{
	TRACE(_T("CopyFile(%s, %s)\n"), szSrcFile, szDestFile);

	HANDLE hSrcFile = XAppCreateFile(szSrcFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hSrcFile == INVALID_HANDLE_VALUE)
	{
		TRACE(_T("OpenFile(%s) failed\n"), szSrcFile);
		m_bInternalError = true;
		return false;
	}

	HANDLE hDestFile = XAppCreateFile(szDestFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL);
	if (hDestFile == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hSrcFile);
		TRACE(_T("CreateFile(%s) failed\n"), szDestFile);
		m_bInternalError = true;
		return false;
	}

	for (;;)
	{
		DWORD dwRead, dwWrite;

		if (!ReadFile(hSrcFile, m_buffer, BLOCK_SIZE, &dwRead, NULL))
		{
			TRACE(_T("ReadFile(%s) failed (%d)\n"), szSrcFile, GetLastError());
			m_bInternalError = true;
			break;
		}

		if (dwRead == 0)
			break;

		if (!WriteFile(hDestFile, m_buffer, dwRead, &dwWrite, NULL))
		{
			TRACE(_T("WriteFile(%s) failed (%d)\n"), szDestFile, GetLastError());
			m_bInternalError = true;
			break;
		}

		m_nCopiedBlocks += 1;
		m_progress = (float)m_nCopiedBlocks / (float)m_nTotalBlocks;
		TRACE(_T("copy progress: %f block %d of %d\n"), m_progress, m_nCopiedBlocks, m_nTotalBlocks);
	}

	if (!m_bInternalError)
	{
		FILETIME create, access, write;
		GetFileTime(hSrcFile, &create, &access, &write);
		SetFileTime(hDestFile, &create, &access, &write);
	}

	CloseHandle(hSrcFile);

	if (!CloseHandle(hDestFile))
	{
		TRACE(_T("CloseHandle(%s) failed\n"), szDestFile);
		m_bInternalError = true;
	}

	return !m_bInternalError;
}

bool CGameCopier::DeleteFile(const TCHAR* szFile)
{
//	TRACE(_T("DeleteFile(%s)\n"), szFile);

#ifdef _XBOX
	char szFileA [MAX_PATH];
	Ansi(szFileA, szFile, MAX_PATH);
	SetFileAttributes(szFileA, FILE_ATTRIBUTE_NORMAL);
	return ::DeleteFile(szFileA) != FALSE;
#else
	SetFileAttributes(szFile, FILE_ATTRIBUTE_NORMAL);
	return ::DeleteFile(szFile) != FALSE;
#endif
}

bool CGameCopier::DeleteDirectory(const TCHAR* szDir, bool RemoveSelf /*= true*/)
{
//	TRACE(_T("DeleteDirectory(%s)\n"), szDir);

	TCHAR szBuf [MAX_PATH];
	MakePath(szBuf, szDir, _T("*.*"));

	WIN32_FIND_DATA fd;
#ifdef _XBOX
	char szBufX [MAX_PATH];
	Ansi(szBufX, szBuf, MAX_PATH);
	HANDLE h = FindFirstFile(szBufX, &fd);
#else
	HANDLE h = FindFirstFile(szBuf, &fd);
#endif
	if (h != INVALID_HANDLE_VALUE)
	{
		do
		{
#ifdef _XBOX
			TCHAR szFileName [MAX_PATH];
			Unicode(szFileName, fd.cFileName, MAX_PATH);
#else
			const TCHAR* szFileName = fd.cFileName;
#endif

			if (_tcscmp(szFileName, _T(".")) == 0 || _tcscmp(szFileName, _T("..")) == 0)
				continue;

			MakePath(szBuf, szDir, szFileName);

			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				if (!DeleteDirectory(szBuf))
				{
					FindClose(h);
					m_bInternalError = true;
					return false;
				}
			}
			else
			{
				if (!DeleteFile(szBuf))
				{
					FindClose(h);
					m_bInternalError = true;
					return false;
				}
			}
		}
		while (FindNextFile(h, &fd));
		FindClose(h);
	}

    if (!RemoveSelf)
    {
        return true;
    }

#ifdef _XBOX
	char szDirA [MAX_PATH];
	Ansi(szDirA, szDir, MAX_PATH);
	return ::RemoveDirectory(szDirA) != FALSE;
#else
	return ::RemoveDirectory(szDir) != FALSE;
#endif
}

DWORD CGameCopier::ThreadProc()
{
	ASSERT(m_buffer != NULL);

	while (m_nCopyGameCur < m_nCopyGameCount)
	{
		CopyGame(m_nCopyGameCur/*m_rgCopyGame[m_nCopyGameCur].m_szTitleID, m_rgCopyGame[m_nCopyGameCur].m_szGameDir*/);
		if (m_bInternalError)
			break;

		m_rgCopyGame[m_nCopyGameCur].m_bNeedToAdd = true;
		m_nCopyGameCur += 1;
	}

	m_done = true;

	delete [] m_buffer;
	m_buffer = NULL;

	m_hThread = NULL;
	m_error = m_bInternalError;

	TRACE(_T("CGameCopier::ThreadProc terminating; error=%d\n"), m_error);

	return 1;
}

void CGameCopier::SetSource(int nDevUnit)
{
	m_srcDevUnit = nDevUnit;

	const TCHAR* szRoot = g_titles[nDevUnit].GetUData();

	TRACE(_T("SetSource(%s)\n"), szRoot);
	delete [] m_szSrcRoot;
	m_szSrcRoot = new TCHAR [_tcslen(szRoot) + 1];
	_tcscpy(m_szSrcRoot, szRoot);

	ASSERT(m_nTotalBlocks == 0);
	ASSERT(m_nCopyGameCount == 0);
}

void CGameCopier::SetDestination(int nDevUnit)
{
	m_destDevUnit = nDevUnit;

	const TCHAR* szRoot = g_titles[nDevUnit].GetUData();

	TRACE(_T("SetDestination(%s)\n"), szRoot);
	delete [] m_szDestRoot;
	m_szDestRoot = new TCHAR [_tcslen(szRoot) + 1];
	_tcscpy(m_szDestRoot, szRoot);

	ASSERT(m_nTotalBlocks == 0);
	ASSERT(m_nCopyGameCount == 0);
}

void CGameCopier::AddGame(const TCHAR* szTitleID, const TCHAR* szGameDir, FILETIME saveTime, int nBlocks)
{
	TRACE(_T("CGameCopier::AddGame: %s %s %d blocks\n"), szTitleID, szGameDir, nBlocks);

	if (m_nCopyGameCount == m_nCopyGameAlloc)
	{
		m_nCopyGameAlloc += 10;
		CCopyGame* rgCopyGame = new CCopyGame [m_nCopyGameAlloc];
		CopyMemory(rgCopyGame, m_rgCopyGame, m_nCopyGameCount * sizeof (CCopyGame));
		delete [] m_rgCopyGame;
		m_rgCopyGame = rgCopyGame;
	}

	m_rgCopyGame[m_nCopyGameCount].m_szTitleID = new TCHAR [_tcslen(szTitleID) + 1];
	_tcscpy(m_rgCopyGame[m_nCopyGameCount].m_szTitleID, szTitleID);

	m_rgCopyGame[m_nCopyGameCount].m_szGameDir = new TCHAR [_tcslen(szGameDir) + 1];
	_tcscpy(m_rgCopyGame[m_nCopyGameCount].m_szGameDir, szGameDir);

	m_rgCopyGame[m_nCopyGameCount].m_time = saveTime;

	m_rgCopyGame[m_nCopyGameCount].m_bNeedToAdd = false;

	m_nCopyGameCount += 1;

	m_nTotalBlocks += nBlocks;
}

void CGameCopier::Start()
{
	ASSERT(m_srcDevUnit >= 0 && m_srcDevUnit <= 8);
	ASSERT(m_destDevUnit >= 0 && m_destDevUnit <= 8);
	ASSERT(m_srcDevUnit != m_destDevUnit);

	TRACE(_T("Start game copy: %d blocks\n"), m_nTotalBlocks);

	m_progress = 0.0f;
	m_bInternalError = false;
	m_error = false;
	m_done = false;
	m_nCopiedBlocks = 0;
	m_nCopyGameCur = 0;

	if (m_buffer == NULL)
		m_buffer = new BYTE [BLOCK_SIZE];

	m_hThread = CreateThread(NULL, 0, StartThread, this, 0, 0);

    if (m_hThread)
    {
        CloseHandle(m_hThread);
    }
    else
    {
        StartThread(this);
    }
}
