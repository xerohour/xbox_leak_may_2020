#include "std.h"
#include "xapp.h"
#include "FileUtil.h"
#include "node.h"
#include "runner.h"
#include "SettingsFile.h"
#include "TitleCollection.h"
#include "Locale.h"

struct CTraceTime
{
	CTraceTime(TCHAR* szMsg)
	{
		m_time = GetTickCount();
		m_szMsg = szMsg;
	}

	~CTraceTime()
	{
		TRACE(_T("TraceTime: %s: %dmS\n"), m_szMsg, GetTickCount() - m_time);
	}

	DWORD m_time;
	TCHAR* m_szMsg;
};


#ifdef _DEBUG0
#define TRACETIME(szMsg) CTraceTime tracetime(szMsg);
#else
#define TRACETIME(szMsg)
#endif


////////////////////////////////////////////////////////////////////////////

CTitleArray g_titles [9];

void TitleArray_Init()
{
	g_titles[8].SetRoot('c', false);
	// NOTE: Memory units will be set as they are mounted...
}

CTitleArray::CTitleArray()
{
    InitializeCriticalSection(&m_RootLock);

	m_szRoot[0] = 0;
	m_szRoot[1] = ':';
	m_szRoot[2] = '\\';
	m_szRoot[3] = 0;

	m_bDirty = true;
	m_nTitleCount = 0;
	m_nTitleAlloc = 0;
	m_rgtitle = NULL;
}

CTitleArray::~CTitleArray()
{
	DeleteAll(false);
}

void CTitleArray::DeleteAll(bool bUpdate /* = true */)
{
	for (int i = 0; i < m_nTitleCount; i += 1)
	{
		delete [] m_rgtitle[i].m_szID;
		delete [] m_rgtitle[i].m_szName;
		delete [] m_rgtitle[i].m_rgsaves;
	}

	delete [] m_rgtitle;
	m_rgtitle = NULL;

	m_nTitleCount = 0;
	m_nTitleAlloc = 0;
	m_bDirty = true;

    if (bUpdate)
    {
        Update();
    }
}

void CTitleArray::SetRoot(TCHAR chNewRoot, bool bUpdate /* = true */)
{
    EnterCriticalSection(&m_RootLock);
	m_szRoot[0] = chNewRoot;
    LeaveCriticalSection(&m_RootLock);
	DeleteAll(bUpdate);
}

static int __cdecl SortTitleCompare(const void *elem1, const void *elem2)
{
	const CTitle* pTitle1 = (const CTitle*)elem1;
	const CTitle* pTitle2 = (const CTitle*)elem2;
	return _tcsicmp(pTitle1->m_szName, pTitle2->m_szName);
}

void CTitleArray::AddTitle(const TCHAR* szTitleID)
{
	TCHAR szXbxFile[MAX_PATH];
    TCHAR szTranslate[MAX_TRANSLATE_LEN];
	MakePath(szXbxFile, GetUData(), szTitleID);
	MakePath(szXbxFile, szXbxFile, szTitleDataXBX);

	TCHAR szTitleName[64];

    TCHAR szLanguageCode[MAX_LANGUAGE_CODE_LEN];
    GetLanguageCode(szLanguageCode);

	CSettingsFile settings;
	bool broken = false;
	if (!settings.Open(szXbxFile) || !settings.GetValue(szLanguageCode, _T("TitleName"), szTitleName, countof(szTitleName)))
	{
		TRACE(_T("\002Broken Game: title id: %s\n"), szTitleID);
		_tcscpy(szTitleName, Translate(_T("Broken Game"), szTranslate));
		broken = true;
	}

	if (m_nTitleCount == m_nTitleAlloc)
	{
		m_nTitleAlloc += 50;

		CTitle* rgtitle = new CTitle [m_nTitleAlloc];
		CopyMemory(rgtitle, m_rgtitle, m_nTitleCount * sizeof (CTitle));
		delete [] m_rgtitle;
		m_rgtitle = rgtitle;
	}

	ASSERT(m_nTitleCount < m_nTitleAlloc);

	int cch = _tcslen(szTitleID) + 1;
	m_rgtitle[m_nTitleCount].m_szID = new TCHAR [cch];
	CopyChars(m_rgtitle[m_nTitleCount].m_szID, szTitleID, cch);

	cch = _tcslen(szTitleName) + 1;
	m_rgtitle[m_nTitleCount].m_szName = new TCHAR [cch];
	CopyChars(m_rgtitle[m_nTitleCount].m_szName, szTitleName, cch);

	m_rgtitle[m_nTitleCount].m_nSavedGameCount = -1;
	m_rgtitle[m_nTitleCount].m_nSavedGameBlocks = -1; // 'unknown'
	m_rgtitle[m_nTitleCount].m_nTotalBlocks = -1; // 'unknown'
	m_rgtitle[m_nTitleCount].m_rgsaves = NULL;
	m_rgtitle[m_nTitleCount].m_bBroken = broken;

	m_nTitleCount += 1;
}

void CTitleArray::Update()
{
	ASSERT(m_bDirty);

    EnterCriticalSection(&m_RootLock);

	if (m_szRoot[0] == 0)
	{
        LeaveCriticalSection(&m_RootLock);
		m_bDirty = false;
		return;
	}

	TCHAR szFileName [MAX_PATH];
	WIN32_FIND_DATA fd;

    MakePath(szFileName, GetUData(), _T("*.*"));
    FSCHAR szBuf [MAX_PATH];
    Ansi(szBuf, szFileName, countof (szBuf));
    HANDLE hFind = FindFirstFile(szBuf, &fd);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        if (ERROR_DEVICE_NOT_CONNECTED != GetLastError())
        {
            m_bDirty = false;
        }
        LeaveCriticalSection(&m_RootLock);
        return;
    }

    do
    {
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			continue;

		Unicode(szFileName, fd.cFileName, countof(szFileName));

		// Ignore the Xbox default title...
		if (_tcsicmp(szFileName, _T("fffe0000")) == 0)
			continue;

		AddTitle(szFileName);
    }
    while (FindNextFile(hFind, &fd));

    if (ERROR_DEVICE_NOT_CONNECTED == GetLastError())
    {
        FindClose(hFind);
        DeleteAll(false);
        LeaveCriticalSection(&m_RootLock);
        return;
    }

	FindClose(hFind);

	qsort(m_rgtitle, m_nTitleCount, sizeof (CTitle), SortTitleCompare);

    // Enumerate saved game count
    for (int i=0; i<m_nTitleCount; i++)
    {
        ASSERT(m_rgtitle[i].m_nSavedGameCount == -1);

        int nSavedGameCount = 0;
        CSave* rgsaves = new CSave [MAX_SAVED_GAMES];
        nSavedGameCount =

        m_rgtitle[i].m_nSavedGameCount = ScanSavedGames(GetUData(), m_rgtitle[i].m_szID, rgsaves, NULL);
        m_rgtitle[i].m_rgsaves = new CSave [nSavedGameCount];
        CopyMemory(m_rgtitle[i].m_rgsaves, rgsaves, sizeof (CSave) * nSavedGameCount);
        delete [] rgsaves;
    }

    LeaveCriticalSection(&m_RootLock);
	m_bDirty = false;
}

int CTitleArray::GetTitleCount()
{
	if (m_bDirty)
        return 0;

	return m_nTitleCount;
}

bool CTitleArray::IsBroken(int nTitle)
{
    ASSERT(nTitle < m_nTitleCount);
	return m_rgtitle[nTitle].m_bBroken;
}

const TCHAR* CTitleArray::GetTitleID(int nTitle)
{
	const TCHAR* sz = _T("");
	
    if (!m_bDirty && nTitle >= 0 && nTitle < m_nTitleCount)
		sz = m_rgtitle[nTitle].m_szID;

	return sz;
}

const TCHAR* CTitleArray::GetTitleName(int nTitle)
{
	const TCHAR* sz = _T("");
	
    if (!m_bDirty && nTitle >= 0 && nTitle < m_nTitleCount)
		sz = m_rgtitle[nTitle].m_szName;

	return sz;
}

int CTitleArray::GetSavedGameCount(int nTitle, HANDLE hCancelEvent /*= NULL*/)
{
	int nSavedGameCount = 0;

	if (!m_bDirty && nTitle >= 0 && nTitle < m_nTitleCount)
	{
		nSavedGameCount = m_rgtitle[nTitle].m_nSavedGameCount;

		if (nSavedGameCount == -1)
		{
			CSave* rgsaves = new CSave [MAX_SAVED_GAMES];

			nSavedGameCount = ScanSavedGames(GetUData(), m_rgtitle[nTitle].m_szID, rgsaves, hCancelEvent);

			m_rgtitle[nTitle].m_nSavedGameCount = nSavedGameCount;
			m_rgtitle[nTitle].m_rgsaves = new CSave [nSavedGameCount];
			CopyMemory(m_rgtitle[nTitle].m_rgsaves, rgsaves, sizeof (CSave) * nSavedGameCount);

            delete [] rgsaves;
		}
	}

	return nSavedGameCount;
}

bool InitSave(CSave* pSave, const TCHAR* szRoot, const TCHAR* szTitleID, const TCHAR* szSaveDirName, FILETIME saveTime)
{
	ZeroMemory(pSave, sizeof (CSave));

	if (!pSave->SetDirName(szSaveDirName))
	{
		// Invalid ID; must be a private directory...
		TRACE(_T("\002Ignoring %s\\%s: invalid ID\n"), szTitleID, szSaveDirName);
		return false;
	}

	pSave->m_filetime = saveTime;
	pSave->m_dwFlags |= SAVEFLAG_UNKIMAGE;

	return true;
}

int CTitleArray::FindTitle(const TCHAR* szTitleID)
{
	for (int nTitle = 0; nTitle < m_nTitleCount; nTitle += 1)
	{
		if (_tcsicmp(szTitleID, m_rgtitle[nTitle].m_szID) == 0)
			return nTitle;
	}

	return -1;
}

static int __cdecl SortSaveCompare(const void *elem1, const void *elem2)
{
	// Reversed these to go from newest -> oldest
	const CSave* pSave2 = (const CSave*)elem1;
	const CSave* pSave1 = (const CSave*)elem2;
	return CompareFileTime(&pSave1->m_filetime, &pSave2->m_filetime);
}

void CTitleArray::AddSavedGame(const TCHAR* szTitleID, const TCHAR* szDirName, FILETIME saveTime)
{
    if (m_bDirty)
        return;

	int nTitle = FindTitle(szTitleID);
	if (nTitle == -1)
	{
		AddTitle(szTitleID);

		m_rgtitle[m_nTitleCount - 1].m_nSavedGameCount = 0;

		qsort(m_rgtitle, m_nTitleCount, sizeof (CTitle), SortTitleCompare);

		// start search over because of sort...
		nTitle = FindTitle(szTitleID);
		ASSERT(nTitle >= 0);
	}

	ASSERT(nTitle >= 0 && nTitle < m_nTitleCount);
	CTitle* pTitle = &m_rgtitle[nTitle];

	if (pTitle->m_nSavedGameCount == -1)
		GetSavedGameCount(nTitle);

	ASSERT(pTitle->m_nSavedGameCount >= 0);
	ASSERT(pTitle->m_nSavedGameCount < MAX_SAVED_GAMES);

	CSave* rgsaves = new CSave [pTitle->m_nSavedGameCount + 1];
	CopyMemory(rgsaves, pTitle->m_rgsaves, sizeof (CSave) * pTitle->m_nSavedGameCount);
	delete [] pTitle->m_rgsaves;
	pTitle->m_rgsaves = rgsaves;
	
	if (InitSave(&pTitle->m_rgsaves[pTitle->m_nSavedGameCount], GetUData(), pTitle->m_szID, szDirName, saveTime))
	{
		pTitle->m_nSavedGameCount += 1;
		qsort(pTitle->m_rgsaves, pTitle->m_nSavedGameCount, sizeof (CSave), SortSaveCompare);
	}

	pTitle->m_nTotalBlocks = -1; // recalculate this
	pTitle->m_nSavedGameBlocks = -1; // recalculate this
}

void CTitleArray::RemoveTitle(int nTitle)
{
	CTitle* pTitle = &m_rgtitle[nTitle];
	delete [] pTitle->m_szID;
	delete [] pTitle->m_szName;
	delete [] pTitle->m_rgsaves;

	MoveMemory(&m_rgtitle[nTitle], &m_rgtitle[nTitle + 1], sizeof (CTitle) * (m_nTitleCount - nTitle - 1));
	m_nTitleCount -= 1;
}

void CTitleArray::RemoveSavedGame(int nTitle, int nSavedGame)
{
	CTitle* pTitle = &m_rgtitle[nTitle];
	MoveMemory(&pTitle->m_rgsaves[nSavedGame], &pTitle->m_rgsaves[nSavedGame + 1], sizeof (CSave) * (pTitle->m_nSavedGameCount - nSavedGame - 1));
	pTitle->m_nSavedGameCount -= 1;

	pTitle->m_nTotalBlocks = -1; // recalculate this
	pTitle->m_nSavedGameBlocks = -1; // recalculate this
}

void CTitleArray::RemoveSavedGame(const TCHAR* szTitleID, const TCHAR* szDirName)
{
    int nTitle = FindTitle(szTitleID);
    if (nTitle == -1)
    {
        return;
    }

    int i, nSavedGame = -1;
    CTitle* pTitle = &m_rgtitle[nTitle];

    // Search for the matching saved game
    for (int i=0; i<pTitle->m_nSavedGameCount; i++)
    {
        if (_tcsicmp(pTitle->m_rgsaves[i].m_szDirName, szDirName) == 0)
        {
            nSavedGame = i;
            break;
        }
    }

    if (nSavedGame != -1)
    {
        RemoveSavedGame(nTitle, nSavedGame);
    }
}

const TCHAR* CTitleArray::GetSavedGameID(int nTitle, int nSavedGame)
{
	int nSavedGameCount = GetSavedGameCount(nTitle);
	if (nSavedGame < 0 || nSavedGame >= nSavedGameCount)
	{
		TRACE(_T("\001CTitleArray::GetSavedGameID(%d, %d): invalid save index!\n"), nTitle, nSavedGame);
		return _T("");
	}

	return m_rgtitle[nTitle].m_rgsaves[nSavedGame].GetDirName(); // NOTE: Temporary access!
}

FILETIME CTitleArray::GetSavedGameTime(int nTitle, int nSavedGame)
{
	int nSavedGameCount = GetSavedGameCount(nTitle);
	if (nSavedGame < 0 || nSavedGame >= nSavedGameCount)
	{
		TRACE(_T("\001CTitleArray::GetSavedGameTime(%d, %d): invalid save index!\n"), nTitle, nSavedGame);
		static FILETIME filetime;
		return filetime;
	}

	return m_rgtitle[nTitle].m_rgsaves[nSavedGame].m_filetime;
}

/*
int CTitleArray::GetTitleSavedGameBlocks(int nTitle)
{
	if (nTitle < 0 || nTitle >= m_nTitleCount)
		return 0;

	if (m_rgtitle[nTitle].m_nSavedGameBlocks == -1)
		m_rgtitle[nTitle].m_nSavedGameBlocks = ComputeTitleSavedGameBlocks(GetUData(), m_rgtitle[nTitle].m_szID);

	return m_rgtitle[nTitle].m_nSavedGameBlocks;
}
*/

int CTitleArray::GetTitleTotalBlocks(int nTitle, HANDLE hCancelEvent)
{
	if (nTitle < 0 || nTitle >= m_nTitleCount)
		return 0;

	if (m_rgtitle[nTitle].m_nTotalBlocks == -1)
	{
		m_rgtitle[nTitle].m_nTotalBlocks = ComputeTitleTotalBlocks(GetUData(), m_rgtitle[nTitle].m_szID, hCancelEvent);
		if (m_szRoot[0] == 'c' && m_rgtitle[nTitle].m_nTotalBlocks >= 0)
			m_rgtitle[nTitle].m_nTotalBlocks += ComputeTitleTotalBlocks(GetTData(), m_rgtitle[nTitle].m_szID, hCancelEvent);
	}

	return m_rgtitle[nTitle].m_nTotalBlocks;
}

////////////////////////////////////////////////////////////////////////////

/*
int ComputeTitleSavedGameBlocks(const TCHAR* szRoot, const TCHAR* szTitleID)
{
	TCHAR szDir [MAX_PATH];
	MakePath(szDir, szRoot, szTitleID);
	return GetDirectoryBlocks(szDir, BLOCK_SIZE, true);
}
*/

int ComputeTitleTotalBlocks(const TCHAR* szRoot, const TCHAR* szTitleID, HANDLE hCancelEvent)
{
	TCHAR szDir [MAX_PATH];
	MakePath(szDir, szRoot, szTitleID);
	return GetDirectoryBlocks(szDir, BLOCK_SIZE, true, hCancelEvent);
}

/*
static DWORD ParseTitleID(const TCHAR* szID)
{
	if (_tcslen(szID) != 8)
		return 0xffffffff; // Invalid ID

	DWORD dw = 0;
	const TCHAR* pch = szID;
	while (*pch != 0)
	{
		DWORD dwDigit;

		if (*pch >= '0' && *pch <= '9')
			dwDigit = *pch - '0';
		else if (*pch >= 'a' && *pch <= 'f')
			dwDigit = 10 + *pch - 'a';
		else if (*pch >= 'A' && *pch <= 'F')
			dwDigit = 10 + *pch - 'A';
		else
			return 0xffffffff; // Invalid ID

		dw = (dw << 4) + dwDigit;

		pch += 1;
	}

	return dw;
}
*/

bool InitSave(CSave* pSave, const TCHAR* szRoot, const TCHAR* szTitleID, const TCHAR* szSaveDirName, FILETIME saveTime);

int ScanSavedGames(const TCHAR* szRoot, const TCHAR* szTitleID, CSave* rgsaves /*[MAX_SAVED_GAMES]*/, HANDLE hCancelEvent)
{
    if (hCancelEvent && WaitForSingleObject(hCancelEvent, 0) == WAIT_OBJECT_0)
    {
        return -1;
    }

	int nSavedGameCount = 0;
    bool bCancel = false;

	// scan for saves...
	HANDLE hFind;
	WIN32_FIND_DATA fd;

	// BLOCK:
	{
		TCHAR szTitleDirWild [MAX_PATH];
		MakePath(szTitleDirWild, szRoot, szTitleID);
		MakePath(szTitleDirWild, szTitleDirWild, _T("*.*"));

		char szBuf [MAX_PATH];
		Ansi(szBuf, szTitleDirWild, MAX_PATH);
		hFind = FindFirstFile(szBuf, &fd);
	}

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
            if (hCancelEvent && WaitForSingleObject(hCancelEvent, 0) == WAIT_OBJECT_0)
            {
                bCancel = true;
                break;
            }

			if (nSavedGameCount == MAX_SAVED_GAMES)
			{
				TRACE(_T("\001Too many saved games in title %s!\n"), szTitleID);
				break;
			}

			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
				continue;

			TCHAR szFileName [MAX_PATH];
			Unicode(szFileName, fd.cFileName, countof(szFileName));

			if (!InitSave(&rgsaves[nSavedGameCount], szRoot, szTitleID, szFileName, fd.ftLastWriteTime))
				continue;

			nSavedGameCount += 1;
		}
		while (FindNextFile(hFind, &fd));
		FindClose(hFind);
	}

    if (!bCancel)
    {
    	qsort(rgsaves, nSavedGameCount, sizeof (CSave), SortSaveCompare);

#ifdef _DEBUG
    	TRACE(_T("Sorted saved games:\n"));
    	for (int i = 0; i < nSavedGameCount; i += 1)
    	{
    		SYSTEMTIME st;
    		FILETIME local;
    		FileTimeToLocalFileTime(&rgsaves[i].m_filetime, &local);
    		FileTimeToSystemTime(&local, &st);
    		TRACE(_T("%02d/%02d/%04d %02d:%02d:%02d %s\n"), st.wMonth, st.wDay, st.wYear, st.wHour, st.wMinute, st.wSecond, rgsaves[i].m_szDirName);
    	}
#endif
    }

	return bCancel ? -1 : nSavedGameCount;
}

////////////////////////////////////////////////////////////////////////////

const TCHAR* CTitleArray::GetTData() const
{
	ASSERT(m_szRoot[0] != 0);

	if (m_szRoot[0] != 'c')
		return m_szRoot;

	return _T("c:\\tdata");
}

const TCHAR* CTitleArray::GetUData() const
{
	ASSERT(m_szRoot[0] != 0);

	if (m_szRoot[0] != 'c')
		return m_szRoot;

	return _T("c:\\udata");
}

bool CTitleArray::IsValid() const
{
	return m_szRoot[0] != 0;
}

bool CTitleArray::IsDirty() const
{
	return (m_szRoot[0] && m_bDirty);
}

void CTitleArray::GetSavedGameImageName(int nTitle, int nSavedGame, TCHAR* szPath/*[MAX_PATH]*/)
{
	ASSERT(nTitle >= 0 && nTitle < m_nTitleCount);
	ASSERT(nSavedGame >= 0 && nSavedGame < GetSavedGameCount(nTitle));

	if ((m_rgtitle[nTitle].m_rgsaves[nSavedGame].m_dwFlags & SAVEFLAG_UNKIMAGE) != 0)
	{
		m_rgtitle[nTitle].m_rgsaves[nSavedGame].m_dwFlags &= ~SAVEFLAG_UNKIMAGE;

		MakePath(szPath, GetUData(), GetTitleID(nTitle));
		MakePath(szPath, szPath, GetSavedGameID(nTitle, nSavedGame));
		MakePath(szPath, szPath, szSaveImageXBX);

		if (DoesFileExist(szPath))
		{
			m_rgtitle[nTitle].m_rgsaves[nSavedGame].m_dwFlags |= SAVEFLAG_HASIMAGE;
			return;
		}
	}

	MakePath(szPath, GetUData(), GetTitleID(nTitle));

	if ((m_rgtitle[nTitle].m_rgsaves[nSavedGame].m_dwFlags & SAVEFLAG_HASIMAGE) != 0)
	{
		// This save has its own image; use it...
		MakePath(szPath, szPath, GetSavedGameID(nTitle, nSavedGame));
	}

	MakePath(szPath, szPath, szSaveImageXBX);

	if (!DoesFileExist(szPath)) //saved game doesn't have any associated images
	{
#ifdef COOL_XDASH
		_tcscpy(szPath, _T("xboxlogo64.xbx.cool"));
#else
		_tcscpy(szPath, _T("xboxlogo64.xbx"));
#endif
	}
}

bool CTitleArray::IsPublisherExists(const TCHAR* szPublisherID) const
{
	for (int i=0; i<m_nTitleCount; i++)
	{
        if (_tcsnicmp(szPublisherID, m_rgtitle[i].m_szID, 4) == 0)
        {
            return true;
        }
	}

    return false;
}

bool CSave::SetDirName(const TCHAR* szDirName)
{
/*
	m_dwID = ParseTitleID(szDirName);
	return m_dwID != 0xffffffff;
*/
	if (_tcslen(szDirName) + 1 > countof (m_szDirName))
		return false;

	_tcscpy(m_szDirName, szDirName);
	return true;
}

const TCHAR* CSave::GetDirName()
{
/*
	static TCHAR szBuf [10];
	_stprintf(szBuf, _T("%08x"), m_dwID);
	return szBuf; // NOTE: Temporary access!
*/
	return m_szDirName;
}

