/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    savedgamegrid.cpp

Abstract:

    This module implements routine to display and manage saved game grid
    in memory menu.  It also contains routine to copy and delete saved
    games and soundtracks.

--*/

#include "std.h"
#include "xapp.h"
#include "FileUtil.h"
#include "node.h"
#include "runner.h"
#include "Texture.h"
#include "Locale.h"
#include "SettingsFile.h"
#include "TitleCollection.h"
#include "CopyGames.h"

#define VISIBLE_ICON_ROWS 3

extern void MakePath(TCHAR* szBuf, const TCHAR* szDir, const TCHAR* szFile);

extern int GetSoundtrackCount();
extern const TCHAR* GetSoundtrackName(int nSoundtrack);
extern int GetSoundtrackSize(int nSoundtrack, HANDLE hCancelEvent = NULL);
extern void DeleteSoundtrack(int nSoundtrack);
extern void DeleteAllSoundtracks();

extern void FormatDeviceName(int devUnit, TCHAR* szBuf);

extern CNode* GetTextNode(const TCHAR* szText, float nWidth);
extern float g_nEffectAlpha;
extern XTIME g_pulseStartTime;
extern const TCHAR* g_szCurTitleImage;
extern const TCHAR* g_szSelTitleImage;
TCHAR szSelectionBuf [MAX_PATH];

static D3DXMATRIX matrixHack;
static bool bMatrixHackValid;

CGameCopier theGameCopier;

class CSavedGameGrid : public CNode
{
	DECLARE_NODE(CSavedGameGrid, CNode)
public:
	CSavedGameGrid();
	~CSavedGameGrid();

	void Render();
	void Advance(float nSeconds);
	bool OnSetProperty(const PRD* pprd, const void* pvValue);

	CNode* m_pod;
	CNode* m_podRing;
	CNode* m_podSavePanel;
	CNode* m_podSoundtrackPanel;
	CNode* m_podHilite;
	CNode* m_MUheader;
	CNode* m_MUhiliteHeader;
	CNode* m_firstMURow;
	CNode* m_header;
	CNode* m_hiliteHeader;
	CNode* m_firstRow;
	CNode* m_secondRow;
	CNode* m_otherRow;
	CNode* m_smallIcon;
	CNode* m_SavedIconPanel;
	CNode* m_SoundtrackIconPanel;
	CNode* m_iconRing;
	CNode* m_smallIconHilite;
	CNode* m_moreUp;
	CNode* m_moreDown;
	bool m_renderIcons;
	int m_iconsPerRow;
	float m_scroll;
	int m_curTitle;
    int m_curTitleCache;
	int m_curSavedGame;
	int m_curSavedGameCache;
	int m_iconRowScroll;
	float m_smallIconSpacing;
	bool m_detachIcon;
	int m_curDevUnit;
	float m_copyProgress;
	int m_freeBlocks;
	int m_gameBlocks;
    bool m_isActive;
    bool m_busy;
	int m_nPrefColumn;

	void selectUp();
	void selectDown();
	void selectLeft();
	void selectRight();

	void setSelImage();

	//CStrObject* ReturnSavedGameImage();
	CStrObject* FormatSavedGameName();
	CStrObject* FormatSavedGameTime();
	CStrObject* FormatSavedGameSize();
	CStrObject* FormatTitleSize();
	CStrObject* FormatTotalBlocks();
	CStrObject* FormatFreeBlocks();

    int GetTotalBlocks();

	CStrObject* GetSavedGamePath(int nTitle, int nSavedGame);
	CStrObject* GetUpdateString(void);

	void StartCopy(int destDevUnit);
	void StartDelete();
    int DoesSavedGameExists(int destDevUnit);

	int CanDetachIcon();

	int GetTitleCount();
	int CanCopy();
	int IsSoundtrackSelected();
	int IsDevUnitReady(int nUnit);

protected:
    int m_nLanguage;
	int m_nTotalBlocks;
	float m_nScrollTo;
	XTIME m_timeScroll;
	int m_curTitleLast;
	bool m_bCopying;
    XTIME m_timeCopyingStarted;
	bool m_bCopyError;
	int m_nDeletedTitle;
	XTIME m_timeOfDelete;
	int m_nSoundtrackTitle;
	int m_nSoundtrackCount;

    int m_nCacheTitleSize;
    int m_nCacheSavedGameSize;

	void RenderIconRow(D3DXMATRIX* pMatrix, float y, int nTitle, int nFirstSavedGame, int nSavedGameCount);
	float RenderLoop(bool bRender);
	void SelectTitle(int nTitle, bool bInstantScroll = false);
	bool GetSavedGamePath(TCHAR* szBuf, int nTitle, int nSavedGame);

	int GetSavedGameCount(int nTitle);
	int GetTitleTotalBlocks(int nTitle, HANDLE hCancelEvent);
	const TCHAR* GetTitleID2(int nTitle);
	void GetTitleName2(int nTitle, TCHAR* szBuf);
	const TCHAR* GetSavedGameID2(int nTitle, int nSavedGame);
	FILETIME GetSavedGameTime(int nTitle, int nSavedGame);

	CStrObject* GetTitleID(int nTitle);
	CStrObject* GetTitleName(int nTitle);

	DWORD m_strProgress;
	int m_periodStatus;
	TCHAR m_szStatus[MAX_TRANSLATE_LEN];

	DECLARE_NODE_PROPS();
	DECLARE_NODE_FUNCTIONS();

private:
    HANDLE m_hTitlesEnumThread;
    HANDLE m_hSavedGameEnumThread;
    HANDLE m_hCancelEvent;
    HANDLE m_hStartEvent, m_hStopEvent;
    bool m_bSavedGameQueryPending;
    volatile bool m_bDone;
    XTIME m_timeToSendEnd;
    static void WINAPI TitlesEnumThread(CSavedGameGrid* p);
    static void WINAPI SavedGameEnumThread(CSavedGameGrid* p);

    // Background delete thread and flag
    HANDLE m_hDeleteThread;
    bool m_bDeleting;
    static void WINAPI DeleteThread(CSavedGameGrid* p);
};

IMPLEMENT_NODE("SavedGameGrid", CSavedGameGrid, CNode)

START_NODE_PROPS(CSavedGameGrid, CNode)
	NODE_PROP(pt_node, CSavedGameGrid, pod)
	NODE_PROP(pt_node, CSavedGameGrid, podRing)
	NODE_PROP(pt_node, CSavedGameGrid, podSavePanel)
	NODE_PROP(pt_node, CSavedGameGrid, podSoundtrackPanel)
	NODE_PROP(pt_node, CSavedGameGrid, podHilite)
	NODE_PROP(pt_node, CSavedGameGrid, MUheader)
	NODE_PROP(pt_node, CSavedGameGrid, MUhiliteHeader)
	NODE_PROP(pt_node, CSavedGameGrid, firstMURow)
	NODE_PROP(pt_node, CSavedGameGrid, header)
	NODE_PROP(pt_node, CSavedGameGrid, hiliteHeader)
	NODE_PROP(pt_node, CSavedGameGrid, firstRow)
	NODE_PROP(pt_node, CSavedGameGrid, secondRow)
	NODE_PROP(pt_node, CSavedGameGrid, otherRow)
	NODE_PROP(pt_integer, CSavedGameGrid, renderIcons)
	NODE_PROP(pt_integer, CSavedGameGrid, iconsPerRow)
	NODE_PROP(pt_number, CSavedGameGrid, scroll)
	NODE_PROP(pt_integer, CSavedGameGrid, curTitle)
	NODE_PROP(pt_node, CSavedGameGrid, smallIcon)
	NODE_PROP(pt_node, CSavedGameGrid, SavedIconPanel)
	NODE_PROP(pt_node, CSavedGameGrid, SoundtrackIconPanel)
	NODE_PROP(pt_node, CSavedGameGrid, iconRing)
	NODE_PROP(pt_node, CSavedGameGrid, smallIconHilite)
	NODE_PROP(pt_number, CSavedGameGrid, smallIconSpacing)
	NODE_PROP(pt_integer, CSavedGameGrid, curSavedGame)
	NODE_PROP(pt_integer, CSavedGameGrid, iconRowScroll)
	NODE_PROP(pt_node, CSavedGameGrid, moreUp)
	NODE_PROP(pt_node, CSavedGameGrid, moreDown)
	NODE_PROP(pt_boolean, CSavedGameGrid, detachIcon)
	NODE_PROP(pt_boolean, CSavedGameGrid, isActive)
	NODE_PROP(pt_integer, CSavedGameGrid, curDevUnit)
	NODE_PROP(pt_number, CSavedGameGrid, copyProgress)
	NODE_PROP(pt_integer, CSavedGameGrid, freeBlocks)
	NODE_PROP(pt_integer, CSavedGameGrid, gameBlocks)
	NODE_PROP(pt_integer, CSavedGameGrid, nPrefColumn)
END_NODE_PROPS()

START_NODE_FUN(CSavedGameGrid, CNode)
	NODE_FUN_VV(selectUp)
	NODE_FUN_VV(selectDown)
	NODE_FUN_VV(selectLeft)
	NODE_FUN_VV(selectRight)
	NODE_FUN_VV(setSelImage)
	NODE_FUN_SV(FormatSavedGameName)
	NODE_FUN_SV(FormatSavedGameTime)
	NODE_FUN_SV(FormatSavedGameSize)
	NODE_FUN_SV(FormatTitleSize)
	NODE_FUN_SV(FormatTotalBlocks)
	NODE_FUN_IV(GetTotalBlocks)
	NODE_FUN_SV(FormatFreeBlocks)
	NODE_FUN_IV(CanDetachIcon)
	NODE_FUN_II(GetSavedGameCount)
	NODE_FUN_SI(GetTitleName)
	NODE_FUN_SI(GetTitleID)
	NODE_FUN_VI(StartCopy)
	NODE_FUN_VV(StartDelete)
    NODE_FUN_II(DoesSavedGameExists)
	NODE_FUN_SII(GetSavedGamePath)
	NODE_FUN_IV(CanCopy)
	NODE_FUN_IV(IsSoundtrackSelected)
	NODE_FUN_II(IsDevUnitReady)
	NODE_FUN_IV(GetTitleCount)
	NODE_FUN_SV(GetUpdateString)
END_NODE_FUN()

CSavedGameGrid::CSavedGameGrid() :
    m_nLanguage(0),
	m_pod(NULL),
	m_podRing(NULL),
	m_podSavePanel(NULL),
	m_podSoundtrackPanel(NULL),
	m_podHilite(NULL),
	m_MUheader(NULL),
	m_MUhiliteHeader(NULL),
	m_firstMURow(NULL),
	m_header(NULL),
	m_hiliteHeader(NULL),
	m_firstRow(NULL),
	m_secondRow(NULL),
	m_otherRow(NULL),
	m_moreUp(NULL),
	m_moreDown(NULL),
	m_renderIcons(true),
	m_iconsPerRow(4),
	m_scroll(0.0f),
	m_curTitle(-1),
	m_curTitleCache(-1),
	m_curSavedGame(-1),
	m_curSavedGameCache(-1),
	m_smallIcon(NULL),
	m_SavedIconPanel(NULL),
	m_SoundtrackIconPanel(NULL),
	m_iconRing(NULL),
	m_smallIconHilite(NULL),
	m_smallIconSpacing(1.0f),
	m_detachIcon(false),
    m_isActive(false),
    m_busy(false),
    m_hDeleteThread(NULL),
    m_bDeleting(false),
    m_hTitlesEnumThread(NULL),
    m_hSavedGameEnumThread(NULL),
    m_nCacheSavedGameSize(-1),
    m_nCacheTitleSize(-1),
    m_bSavedGameQueryPending(false),
    m_bDone(false),
    m_bCopying(false),
    m_timeCopyingStarted(0.0f),
	m_curDevUnit(-1),
	m_iconRowScroll(0),
	m_nPrefColumn(-1)
{
	m_nScrollTo = 0.0f;
	m_timeScroll = 0.0f;
	m_curTitleLast = -1;
	m_nTotalBlocks = -1;
	m_freeBlocks = -1;
	m_gameBlocks = -1;
	m_nDeletedTitle = -1;
	m_timeOfDelete = 0.0f;
	m_nSoundtrackTitle = -1;
	m_nSoundtrackCount = 0;

    VERIFY(m_hStartEvent = CreateEvent(0, FALSE, FALSE, 0));
    VERIFY(m_hStopEvent = CreateEvent(0, TRUE, FALSE, 0));
    VERIFY(m_hCancelEvent = CreateEvent(0, TRUE, FALSE, 0));
    VERIFY(m_hSavedGameEnumThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SavedGameEnumThread, (LPVOID)this, 0, 0));
}

CSavedGameGrid::~CSavedGameGrid()
{
	if (m_pod != NULL)
		m_pod->Release();

	if (m_podRing != NULL)
		m_podRing->Release();

	if (m_podSavePanel != NULL)
		m_podSavePanel->Release();

	if (m_podSoundtrackPanel != NULL)
		m_podSoundtrackPanel->Release();
	
	if (m_podHilite != NULL)
		m_podHilite->Release();

	if (m_MUheader != NULL)
		m_MUheader->Release();

	if (m_MUhiliteHeader != NULL)
		m_MUhiliteHeader->Release();
	
	if (m_firstMURow != NULL)
		m_firstMURow->Release();
	
	if (m_header != NULL)
		m_header->Release();

	if (m_hiliteHeader != NULL)
		m_hiliteHeader->Release();
	
	if (m_firstRow != NULL)
		m_firstRow->Release();
	
	if (m_secondRow != NULL)
		m_secondRow->Release();

	if (m_otherRow != NULL)
		m_otherRow->Release();

	if (m_smallIcon != NULL)
		m_smallIcon->Release();

	if (m_SavedIconPanel != NULL)
		m_SavedIconPanel->Release();

	if (m_SoundtrackIconPanel != NULL)
		m_SoundtrackIconPanel->Release();
	
	if (m_iconRing != NULL)
		m_iconRing->Release();

	if (m_smallIconHilite != NULL)
		m_smallIconHilite->Release();

	if (m_moreUp != NULL)
		m_moreUp->Release();

	if (m_moreDown != NULL)
		m_moreDown->Release();

    // We never should never hit this, the object never gets destroyed
    ASSERT(FALSE);

    if (m_bSavedGameQueryPending)
    {
        m_bDone = true;
        SetEvent(m_hCancelEvent);
    }

    WaitForSingleObject(m_hSavedGameEnumThread, INFINITE);

    VERIFY(CloseHandle(m_hStartEvent));
    VERIFY(CloseHandle(m_hStopEvent));
    VERIFY(CloseHandle(m_hCancelEvent));
    VERIFY(CloseHandle(m_hSavedGameEnumThread));
}

int CSavedGameGrid::GetTitleCount()
{
	ASSERT(m_curDevUnit >= 0 && m_curDevUnit <= 8);

	int nTitleCount = g_titles[m_curDevUnit].GetTitleCount();
	if(m_curDevUnit == Dev0)
	{
		m_nSoundtrackCount = GetSoundtrackCount();
		m_nSoundtrackTitle = nTitleCount;
	}
	else
	{
		m_nSoundtrackCount = 0;
		m_nSoundtrackTitle = -1;
	}

	if (m_curDevUnit == Dev0 && m_nSoundtrackCount > 0)
	{
		ASSERT(nTitleCount == m_nSoundtrackTitle);
		nTitleCount += 1;
	}


	return nTitleCount;
}

inline int CSavedGameGrid::GetTitleTotalBlocks(int nTitle, HANDLE hCancelEvent)
{
	ASSERT(m_curDevUnit >= 0 && m_curDevUnit <= 8);

	if (nTitle < 0)
		return 0;

	if (m_nSoundtrackTitle < 0 || nTitle < m_nSoundtrackTitle)
		return g_titles[m_curDevUnit].GetTitleTotalBlocks(nTitle, hCancelEvent);

	return GetSoundtrackSize(-1);
}

inline const TCHAR* CSavedGameGrid::GetTitleID2(int nTitle)
{
	ASSERT(m_curDevUnit >= 0 && m_curDevUnit <= 8);

	ASSERT(nTitle >= 0 && (m_nSoundtrackTitle < 0 || nTitle < m_nSoundtrackTitle));
	return g_titles[m_curDevUnit].GetTitleID(nTitle);
}

inline CStrObject* CSavedGameGrid::GetTitleID(int nTitle)
{
	ASSERT(nTitle >= 0 && (m_nSoundtrackTitle < 0 || nTitle < m_nSoundtrackTitle));
	return new CStrObject(GetTitleID2(nTitle));
}

void CSavedGameGrid::GetTitleName2(int nTitle, TCHAR* szBuf)
{
    TCHAR sz[MAX_TRANSLATE_LEN];
	ASSERT(m_curDevUnit >= 0 && m_curDevUnit <= 8);

	if (nTitle < 0)
	{
		if (m_curDevUnit == Dev0)
			_tcscpy(szBuf, Translate(_T("Xbox Hard Disk"), sz));
		else
		{
			FormatDeviceName(m_curDevUnit, szBuf);
			if(szBuf[0] == 0)
			{
				TCHAR ch = _T('B');

				// Generate a name based on location of slot...
				int nGamePad = (m_curDevUnit + 1) % 2;
				if (nGamePad)
					ch = _T('A');
				
				Translate(_T("memory unit"), sz);
				swprintf(szBuf, _T("%s %d%c"), sz, (m_curDevUnit / 2) + 1, ch);
			}

		}
		return;
	}

	if (m_nSoundtrackTitle < 0 || nTitle < m_nSoundtrackTitle)
	{
		_tcscpy(szBuf, g_titles[m_curDevUnit].GetTitleName(nTitle));
		return;
	}

	_tcscpy(szBuf, Translate(_T("Soundtracks"), sz));
}

inline CStrObject* CSavedGameGrid::GetTitleName(int nTitle)
{
	TCHAR szBuf [256];
	GetTitleName2(nTitle, szBuf);
	return new CStrObject(szBuf);
}

inline const TCHAR* CSavedGameGrid::GetSavedGameID2(int nTitle, int nSavedGame)
{
	ASSERT(m_curDevUnit >= 0 && m_curDevUnit <= 8);

	ASSERT(nTitle >= 0 && (m_nSoundtrackTitle < 0 || nTitle < m_nSoundtrackTitle));
	return g_titles[m_curDevUnit].GetSavedGameID(nTitle, nSavedGame);
}

inline FILETIME CSavedGameGrid::GetSavedGameTime(int nTitle, int nSavedGame)
{
	ASSERT(m_curDevUnit >= 0 && m_curDevUnit <= 8);

	ASSERT(nTitle >= 0 && (m_nSoundtrackTitle < 0 || nTitle < m_nSoundtrackTitle));
	return g_titles[m_curDevUnit].GetSavedGameTime(nTitle, nSavedGame);
}

void WINAPI CSavedGameGrid::TitlesEnumThread(CSavedGameGrid* p)
{
    p->m_busy = true;

    // Enumerate all titles on current device unit
    g_titles[p->m_curDevUnit].Update();

    // Also enumerate soundtrack if it's a hard disk
	if (p->m_curDevUnit == Dev0 && p->m_nSoundtrackTitle == -1)
	{
		int nSoundtrackCount = GetSoundtrackCount();
		if (nSoundtrackCount > 0)
		{
			p->m_nSoundtrackTitle = p->GetTitleCount();
			p->m_nSoundtrackCount = nSoundtrackCount;
		}
	}

    p->m_busy = false;
}

void WINAPI CSavedGameGrid::SavedGameEnumThread(CSavedGameGrid* p)
{
    while (!p->m_bDone)
    {
        ASSERT(p->m_hStartEvent);
        ASSERT(p->m_hStopEvent);

        SignalObjectAndWait(p->m_hStopEvent, p->m_hStartEvent, INFINITE, FALSE);
        ResetEvent(p->m_hStopEvent);

        ASSERT(!g_titles[p->m_curDevUnit].IsDirty());

        if (p->m_nCacheSavedGameSize < 0 && p->m_curTitle >=0 && p->m_curSavedGame >= 0)
        {
            if (p->m_nSoundtrackTitle < 0 || p->m_curTitle < p->m_nSoundtrackTitle)
            {
                TCHAR szSavedGameDir[MAX_PATH];
                if (p->GetSavedGamePath(szSavedGameDir, p->m_curTitle, p->m_curSavedGame))
                {
                    p->m_nCacheSavedGameSize = GetDirectoryBlocks(szSavedGameDir, BLOCK_SIZE, true, p->m_hCancelEvent);
                }
                else
                {
                    // REVIEW: What should be returned in this case?
                    p->m_nCacheSavedGameSize = 0;
                }
            }
            else
            {
                p->m_nCacheSavedGameSize = GetSoundtrackSize(p->m_curSavedGame);
            }
        }
        else if (p->m_nCacheTitleSize < 0)
        {
            p->m_nCacheTitleSize = p->GetTitleTotalBlocks(p->m_curTitle, p->m_hCancelEvent);
        }
    }
}

void CSavedGameGrid::Advance(float nSeconds)
{
    bool bUpdateSavedGames;

	CNode::Advance(nSeconds);

	if (m_curDevUnit == -1 || !m_isActive)
		return;

	ASSERT(m_curDevUnit >= 0 && m_curDevUnit <= 8);

    // Enumerating all titles in UDATA
    if (m_hTitlesEnumThread)
    {
        if (XAppGetNow() > m_timeToSendEnd && WaitForSingleObject(m_hTitlesEnumThread, 0) == WAIT_OBJECT_0)
        {
            VERIFY(CallFunction(this, _T("OnUpdatingTitlesEnd")));
            VERIFY(CloseHandle(m_hTitlesEnumThread));
            m_hTitlesEnumThread = NULL;
        }
        else
        {
            return;
        }
    }
    else if (g_titles[m_curDevUnit].IsDirty())
    {
        if (!m_hTitlesEnumThread)
        {
            m_nSoundtrackTitle = -1;
            m_nSoundtrackCount = 0;

            m_timeToSendEnd = XAppGetNow() + 1.0f;
            VERIFY(CallFunction(this, _T("OnUpdatingTitlesBegin")));

            m_hTitlesEnumThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)TitlesEnumThread, (LPVOID)this, 0, 0);

            // If the CreateThread failed, call it directly
            if (!m_hTitlesEnumThread)
            {
                TitlesEnumThread(this);
                VERIFY(CallFunction(this, _T("OnUpdatingTitlesEnd")));
                goto next;
            }
        }
        return;
    }

next:

    //
    //  While copying or deleting is in progress, there is no need to do anything else.
    //  Even (especially?) look for device removal.  In the device removal case, the
    //  delete or file copy will error out, clearing the in progress bits, and then
    //  we will fall through and handle the removal.  With any luck this fixes bug 5212
    //  and a host of related stuff.
    //

    // BLOCK: Copy progress...
	if (m_bCopying)
	{
		if (m_copyProgress != theGameCopier.m_progress)
		{
			m_copyProgress = theGameCopier.m_progress;
			CallFunction(this, _T("OnCopyProgressChanged"));
		}

		if (theGameCopier.m_error)
		{
			m_bCopying = false;
			theGameCopier.Finish();
			TRACE(_T("OnCopyError\n"));
			CallFunction(this, _T("OnCopyError"));
		}
		else if (theGameCopier.m_done && XAppGetNow() >= m_timeCopyingStarted + 0.5f)
		{
			m_bCopying = false;
			theGameCopier.Finish();
			TRACE(_T("OnCopyComplete\n"));
			CallFunction(this, _T("OnCopyComplete"));
		}
        return;
	}

    // BLOCK: Delete notification
    if (m_bDeleting)
    {
        ASSERT(m_hDeleteThread);
        if (WaitForSingleObject(m_hDeleteThread, 0) == WAIT_OBJECT_0)
        {
            CloseHandle(m_hDeleteThread);
            m_hDeleteThread = NULL;
            m_bDeleting = false;
            VERIFY(CallFunction(this, _T("OnDeleteEnd")));
            VERIFY(CallFunction(this, _T("OnSelChange")));
            g_pulseStartTime = XAppGetNow();
        }
        return;
    }

	if (!g_titles[m_curDevUnit].IsValid())
	{
		CallFunction(this, _T("OnDeviceRemoved"));
		m_curDevUnit = -1;
		return;
	}

    if (g_titles[m_curDevUnit].IsDirty())
    {
        return;
    }

    // BLOCK: send notification if saved game query is done
    if (m_bSavedGameQueryPending && WaitForSingleObject(m_hStopEvent, 0) == WAIT_OBJECT_0)
    {
        VERIFY(CallFunction(this, _T("OnSelChange")));
        m_bSavedGameQueryPending = false;
    }

    // BLOCK: check if saved game info needs to be updated
    if (m_curSavedGame != m_curSavedGameCache)
    {
        m_curSavedGameCache = m_curSavedGame;
        m_nCacheSavedGameSize = -1;
        bUpdateSavedGames = true;
    }
    else if (m_curTitle != m_curTitleCache)
    {
        m_curTitleCache = m_curTitle;
        m_nCacheTitleSize = -1;
        m_curSavedGameCache = m_curSavedGame;
        m_nCacheSavedGameSize = -1;
        bUpdateSavedGames = true;
    }
    else if (!m_bSavedGameQueryPending && (m_nCacheTitleSize < 0 || m_nCacheSavedGameSize < 0))
    {
        bUpdateSavedGames = true;
    }
    else
    {
        bUpdateSavedGames = false;
    }

    // BLOCK: start/stop background thread and query saved game as needed
    if (bUpdateSavedGames && m_curTitle >= 0)
    {
        if (m_nCacheTitleSize < 0 || (m_nCacheSavedGameSize < 0 && m_curSavedGame >= 0))
        {
            // Send OnSelChange so that meta panel will be updated to unknown size
            VERIFY(CallFunction(this, _T("OnSelChange")));

            // Cancel previous pending query
            if (m_bSavedGameQueryPending)
            {
                SignalObjectAndWait(m_hCancelEvent, m_hStopEvent, INFINITE, FALSE);
            }

            ResetEvent(m_hCancelEvent);
            SetEvent(m_hStartEvent);
            m_bSavedGameQueryPending = true;
        }
    }

	// BLOCK: Get total/free space info...
	{
#ifdef _XBOX
		FSCHAR szPath [MAX_PATH];
		Ansi(szPath, g_titles[m_curDevUnit].GetUData(), MAX_PATH);
#else
		const FSCHAR* szPath = g_titles[m_curDevUnit].GetUData();
#endif
		ULARGE_INTEGER qwAvailBytes, qwTotalBytes, qwFreeBytes;
		
        if (!GetDiskFreeSpaceEx(szPath, &qwAvailBytes, &qwTotalBytes, &qwFreeBytes))
		{
            qwFreeBytes.QuadPart  = 0;
            qwTotalBytes.QuadPart = BLOCK_SIZE;
            qwFreeBytes.QuadPart  = 0;
			TRACE(_T("GetDiskFreeSpaceEx %s failed: %d\n"), g_titles[m_curDevUnit].GetUData(), GetLastError());
		}

        int totalBlocks = (int)((qwTotalBytes.QuadPart + BLOCK_SIZE - 1) / BLOCK_SIZE);
        int freeBlocks = (int)((qwFreeBytes.QuadPart + BLOCK_SIZE - 1) / BLOCK_SIZE);

        totalBlocks -= 1; // we always have a root dir that uses at least one block...

        if (totalBlocks != m_nTotalBlocks || freeBlocks != m_freeBlocks || g_nCurLanguage != m_nLanguage)
        {
            m_nLanguage = g_nCurLanguage;
            m_nTotalBlocks = totalBlocks;
            m_freeBlocks = freeBlocks;
            CallFunction(this, _T("OnTotalFreeChanged"));
            CallFunction(this, _T("OnSelChange"));
        }
	}

	if (m_timeScroll != 0.0f)
	{
		float t = (float) (XAppGetNow() - m_timeScroll) / 0.25f;
		if (t >= 1.0f)
		{
			m_timeScroll = 0.0f;
			t = 1.0f;
		}

		float t1 = 1.0f - t;
		m_scroll = t1 * m_scroll + t * m_nScrollTo;

		bMatrixHackValid = false;
	}
}

CStrObject* CSavedGameGrid::GetUpdateString(void)
{
	if((GetTickCount() - m_strProgress) > 500)
	{
		m_strProgress = GetTickCount();
		m_periodStatus++;
		if(m_periodStatus > 3)
			m_periodStatus = 0;

		Translate(_T("Copying"), m_szStatus);

		for(int i = 0; i < m_periodStatus; i++)
			_tcscat(m_szStatus, _T("."));
	}

	return new CStrObject(m_szStatus);
}

bool CSavedGameGrid::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	if ((int)pprd->pbOffset == offsetof(m_curDevUnit))
	{
		m_curDevUnit = *(int*)pvValue;

		// Set initial focus position
		if(m_curDevUnit < 8)
			SelectTitle(-1, true);
		else if(m_curDevUnit == 8)
			SelectTitle(0, true);

        // Invalidate cache
        m_nCacheSavedGameSize = -1;
        m_nCacheTitleSize = -1;

		return false;
	}
	else if ((int)pprd->pbOffset == offsetof(m_curTitle))
	{
        SelectTitle(*(int*)pvValue);
		return false;
	}
	else if ((int)pprd->pbOffset == offsetof(m_isActive))
	{
        if (*(bool*)pvValue == false && m_bSavedGameQueryPending)
        {
            SignalObjectAndWait(m_hCancelEvent, m_hStopEvent, INFINITE, FALSE);
            m_bSavedGameQueryPending = false;
            m_nCacheTitleSize = -1;
            m_nCacheSavedGameSize = -1;
        }
	}

	return true;
}

static void RenderNodeAt(CNode* pNode, D3DXMATRIX* pMatrix)
{
	if (pNode == NULL)
		return;

	XAppPushWorld();
	XAppMultWorld(pMatrix);

	XAppUpdateWorld();

	pNode->Render();

	XAppPopWorld();
}

void CSavedGameGrid::RenderIconRow(D3DXMATRIX* pMatrix, float y, int nTitle, int nFirstSavedGame, int nSavedGames)
{
	if (m_smallIcon == NULL)
		return;

	if (nTitle < 0)
	{
		// TODO: Render info for device
		return;
	}

	int nLim = nFirstSavedGame + m_iconsPerRow;
	if (nLim > nSavedGames)
		nLim = nSavedGames;

	ASSERT(nFirstSavedGame == 0 || nFirstSavedGame < nLim);

	float x = 0.0f;
	for (int i = nFirstSavedGame; i < nLim; i += 1)
	{
		D3DXMATRIX mat2;
		D3DXMatrixTranslation(&mat2, x, y, 0.0f);
		D3DXMatrixMultiply(&mat2, pMatrix, &mat2);

		// Felt cluttered with the smaller orb on each object
		//RenderNodeAt(m_smallIcon, &mat2);

		TCHAR szBuf [MAX_PATH];
		if (m_nSoundtrackTitle < 0 || nTitle < m_nSoundtrackTitle)
		{
			g_titles[m_curDevUnit].GetSavedGameImageName(nTitle, i, szBuf);
			g_szCurTitleImage = szBuf;
			RenderNodeAt(m_SavedIconPanel, &mat2);
		}
		else  // is a soundtrack
		{
			_tcscpy(szBuf, _T("soundtracksave64.tga"));
			g_szCurTitleImage = szBuf;
			RenderNodeAt(m_SoundtrackIconPanel, &mat2);
		}
		
		g_szCurTitleImage = NULL;

		if (!m_detachIcon && m_smallIconHilite != NULL && nTitle == m_curTitle && i == m_curSavedGame)
			RenderNodeAt(m_iconRing, &mat2);

		x += m_smallIconSpacing;
	}
}

void CSavedGameGrid::Render()
{
	if (m_header == NULL || m_firstRow == NULL || m_secondRow == NULL || m_otherRow == NULL)
		return;

	if (m_curDevUnit == -1 || !g_titles[m_curDevUnit].IsValid() || g_titles[m_curDevUnit].IsDirty())
		return;

    if (!m_isActive || m_busy)
        return;

	RenderLoop(true);
}

float CSavedGameGrid::RenderLoop(bool bRender)
{
	D3DXMATRIX mat, mat2, scrollMat;
	D3DXVECTOR3 v(-1.0f, 0.0f, 0.0f);
	D3DXMatrixRotationAxis(&mat, &v,  -1.571f);
	D3DXMatrixScaling(&mat2, 0.05942f, 0.05942f, 0.05942f);
	D3DXMatrixMultiply(&mat, &mat, &mat2);

	float y = m_scroll;
	float yLimit = -1.25f;

	int nTitleCount = GetTitleCount();

	int initLoop;
	if(m_curDevUnit == 8)  // do we render the memory panel at the top of the memory list
		initLoop = 0;
	else
		initLoop = -1;

	for (int nTitle = initLoop; nTitle < nTitleCount && (!bRender || y > yLimit); nTitle += 1)
	{
		float nEffectAlphaSave = g_nEffectAlpha;
		int nSavedGames = 0;

		// Check how many saved games (or soundtracks) this title has
		if (nTitle >= 0)
		{
			if (m_nSoundtrackTitle < 0 || nTitle < m_nSoundtrackTitle)
				nSavedGames = GetSavedGameCount(nTitle);
			else
				nSavedGames = m_nSoundtrackCount;
		}

		int nRowCount = (nSavedGames + m_iconsPerRow - 1) / m_iconsPerRow;

		int nIconRowScroll = 0;
		if (nTitle == m_curTitle)
		{
			// we keep track of the scroll position for the current title
			nIconRowScroll = m_iconRowScroll;
		}
		else if (nTitle < m_curTitle)
		{
			// this title is above the current one; show it scrolled to its bottom
			nIconRowScroll = nRowCount - VISIBLE_ICON_ROWS;
			if (nIconRowScroll < 0)
				nIconRowScroll = 0;
		}

		if (bRender)
		{
			float nSelectedAmount = 0.0f;
			float t = (float) (XAppGetNow() - m_timeScroll) / 0.25f;
			if (t > 1.0f)
				t = 1.0f;

			if (nTitle == m_curTitle)
			{
				if (m_timeScroll != 0.0f)
					nSelectedAmount = t;
				else
					nSelectedAmount = 1.0f;
			}
			else if (nTitle == m_curTitleLast)
			{
				if (m_timeScroll != 0.0f)
					nSelectedAmount = 1.0f - t;
			}

			g_nEffectAlpha *= 0.5f + (0.5f * nSelectedAmount);
		}
		else if (nTitle == m_curTitle)
		{
			return y - m_scroll;
		}

		// Render main Title Pod
		if (bRender && y - 0.25f < 0.5f && m_pod != NULL)
		{
			TCHAR szBuf [MAX_PATH];

			// Determine the texture for the Icon
			if(nTitle == -1) // Memory Unit
			{
				_tcscpy(szBuf, _T("memoryUnit128.tga"));
			}
			else if (nTitle >= 0 && (m_nSoundtrackTitle < 0 || nTitle < m_nSoundtrackTitle))
			{
				if (nTitle == m_curTitle && m_detachIcon && m_curSavedGame != -1)
				{
					g_titles[m_curDevUnit].GetSavedGameImageName(nTitle, m_curSavedGame, szBuf);
				}
				else
				{
					MakePath(szBuf, g_titles[m_curDevUnit].GetUData(), GetTitleID2(nTitle));
					MakePath(szBuf, szBuf, szTitleImageXBX);

					if (!DoesFileExist(szBuf)) //Title doesn't have an associated image
					{
#ifdef COOL_XDASH
						_tcscpy(szBuf, _T("xboxlogo128.xbx.cool"));
#else
						_tcscpy(szBuf, _T("xboxlogo128.xbx"));
#endif
					}
				}
			}
			else  // title is a soundtrack orb
			{
				_tcscpy(szBuf, _T("soundtracksave3.tga"));
			}

			g_szCurTitleImage = szBuf;

			float nEffectAlphaSave2 = g_nEffectAlpha;

			// Draw pod
			bool bRenderNormalPod = true;
			bool bRenderHackPod = false;
			bool bRenderHilite = false;
			bool bRenderMU = false;
			bool bRenderSoundtrack = false;

			if (nTitle == -1)  // do we render the MU in it's space?
				bRenderMU = true;

			if (nTitle == m_nSoundtrackTitle) // do we render the soundtrack icon?
				bRenderSoundtrack = true;

			if (nTitle == m_curTitle && m_scroll == m_nScrollTo)
			{
				bRenderNormalPod = false;
				bRenderHackPod = true;
				bRenderHilite = !m_detachIcon && m_podHilite != NULL && m_curSavedGame == -1;

				if (nTitle > -1 && nTitle == m_nDeletedTitle)
				{
					float t = (float) (XAppGetNow() - m_timeOfDelete) / 1.0f;

					if (t >= 1.0f)
					{
						m_timeOfDelete = 0.0f;
						m_nDeletedTitle = -1;
						bMatrixHackValid = false;
					}
					else
					{
						g_nEffectAlpha *= 1.0f - t;
						g_szCurTitleImage = NULL;
						bMatrixHackValid = true;
						bRenderHilite = false;
						bRenderNormalPod = true;
					}
				}
			}

			if (bRenderHackPod)
			{
				if (bMatrixHackValid)
				{
					XAppPushWorld();
					XAppIdentityWorld();
					XAppMultWorld(&matrixHack);

					XAppUpdateWorld();

					m_pod->Render();
					if(bRenderSoundtrack)
						m_podSoundtrackPanel->Render();
					else
						m_podSavePanel->Render();

					if (bRenderHilite)
						m_podRing->Render();

					XAppPopWorld();
				}
				else
				{
					D3DXMatrixTranslation(&mat2, -0.3292f, y, -0.0271f);
					D3DXMatrixMultiply(&mat2, &mat, &mat2);

					XAppPushWorld();
					XAppMultWorld(&mat2);
					matrixHack = *XAppGetWorld();
					bMatrixHackValid = true;

					XAppUpdateWorld();

					m_pod->Render();
					if(bRenderSoundtrack)
						m_podSoundtrackPanel->Render();
					else
						m_podSavePanel->Render();

					if (bRenderHilite)
						m_podRing->Render();

					XAppPopWorld();
				}
			}
			
			g_nEffectAlpha = nEffectAlphaSave2;
			g_szCurTitleImage = szBuf;

			if (bRenderNormalPod)
			{
				D3DXMatrixTranslation(&mat2, -0.3292f, y, -0.0271f);
				D3DXMatrixMultiply(&mat2, &mat, &mat2);
				RenderNodeAt(m_pod, &mat2);
				if(bRenderSoundtrack)
					RenderNodeAt(m_podSoundtrackPanel, &mat2);
				else
					RenderNodeAt(m_podSavePanel, &mat2);

			}

			g_szCurTitleImage = NULL;
		}

		// Draw header
		{
			if (bRender && y - 0.25f < 0.5f)
			{
				D3DXMatrixTranslation(&mat2, -0.3292f, y, -0.0271f);
				D3DXMatrixMultiply(&mat2, &mat, &mat2);

				if(nTitle >= 0 && nTitle == m_curTitle)      //is a game title or soundtrack
					RenderNodeAt(m_hiliteHeader, &mat2);
				else if(nTitle >= 0)
					RenderNodeAt(m_header, &mat2);
				else if(nTitle < 0 && nTitle == m_curTitle) // the title is the memory unit
					RenderNodeAt(m_MUhiliteHeader, &mat2);
				else if(nTitle < 0)
					RenderNodeAt(m_MUheader, &mat2);

				// Draw text
				if(m_renderIcons)  // are we on the proper screen?
				{
					XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
					XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
					XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
					XAppSetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
					XAppSetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
#ifdef COOL_XDASH
					XAppSetRenderState(D3DRS_TEXTUREFACTOR, (nTitle == m_curTitle) ? D3DCOLOR_RGBA(170, 170, 170, 255) : D3DCOLOR_RGBA(25, 114, 201, 255));
#else
					XAppSetRenderState(D3DRS_TEXTUREFACTOR, (nTitle == m_curTitle) ? D3DCOLOR_RGBA(170, 170, 170, 255) : D3DCOLOR_RGBA(140, 201, 25, 255));
#endif

					D3DXMATRIX mat3;
					//D3DXMatrixScaling(&mat3, 0.05942f, 0.05942f, 0.05942f);
					D3DXMatrixScaling(&mat3, 0.06442f, 0.06442f, 0.06442f);
					D3DXMatrixTranslation(&mat2, -0.3292f - 0.05942f, y + 0.05942f, -0.0271f + (0.6f * 0.05942f));
					D3DXMatrixMultiply(&mat2, &mat3, &mat2);

					TCHAR szBuf [256];
					GetTitleName2(nTitle, szBuf);
					RenderNodeAt(GetTextNode(szBuf, -11.2f), &mat2);
				}

				if (nIconRowScroll > 0)
				{
					// Draw the up arrow
					D3DXMatrixTranslation(&mat2, -0.17f, (y - 0.07f), 0.0f);
					D3DXMatrixMultiply(&mat2, &mat, &mat2);
					scrollMat = mat2;
					//RenderNodeAt(m_moreUp, &mat2);
				}
			}

			y -= 0.0092f;
		}

		// Draw first icon row
		{
			if (bRender && y - 0.25f < 0.5f)
			{
				D3DXMatrixTranslation(&mat2, -0.3292f, y, 0.01144f);
				D3DXMatrixMultiply(&mat2, &mat, &mat2);
				if(nTitle >= 0)  //is a game title or soundtrack
					RenderNodeAt(m_firstRow, &mat2);
				else  // the title is a memory unit
					RenderNodeAt(m_firstMURow, &mat2);

				if (nIconRowScroll > 0)
				{
					// Draw the up arrow
					RenderNodeAt(m_moreUp, &scrollMat);
				}

				RenderIconRow(&mat, y, nTitle, nIconRowScroll * m_iconsPerRow, nSavedGames);
			}

			y -= 0.2455f;
		}

		if (nSavedGames > m_iconsPerRow)
		{
			if (bRender && y - 0.25f < 0.5f)
			{
				D3DXMatrixTranslation(&mat2, -0.3301f, y, 0.01144f);
				D3DXMatrixMultiply(&mat2, &mat, &mat2);
				RenderNodeAt(m_secondRow, &mat2);
				RenderIconRow(&mat, y, nTitle, (nIconRowScroll + 1) * m_iconsPerRow, nSavedGames);
			}

			y -= 0.24499f;

			if (nSavedGames > m_iconsPerRow * 2)
			{
				for (int nRow = 2; nRow < (nRowCount - nIconRowScroll) && nRow < VISIBLE_ICON_ROWS && (!bRender || y > yLimit); nRow += 1)
				{
					if (bRender && y - 0.25f < 0.5f)
					{
						D3DXMatrixTranslation(&mat2, -0.3412f, y, 0.01144f);
						D3DXMatrixMultiply(&mat2, &mat, &mat2);
						RenderNodeAt(m_otherRow, &mat2);
						if (nRow == VISIBLE_ICON_ROWS - 1 && (nIconRowScroll + nRow) < nRowCount - 1)
						{
							D3DXMatrixTranslation(&mat2, -0.17f, (y - 0.015f), 0.0f);
							D3DXMatrixMultiply(&mat2, &mat, &mat2);
							RenderNodeAt(m_moreDown, &mat2);
						}
						RenderIconRow(&mat, y, nTitle, (nIconRowScroll + nRow) * m_iconsPerRow, nSavedGames);
					}

					y -= 0.24499f;
				}
			}
		}

		// Prepare for next title area
		y -= 0.09f;
		g_nEffectAlpha = nEffectAlphaSave;
	}

	return 0.0f;
}

void CSavedGameGrid::SelectTitle(int nTitle, bool bInstantScroll/*=false*/)
{
	m_curTitleLast = m_curTitle;
	m_curTitle = nTitle;

    if (g_titles[m_curDevUnit].IsDirty())
    {
        return;
    }

/*
	if (m_curTitle < 0)
	{
		m_curTitle = -1;
		m_curSavedGame = -1;
		m_nPrefColumn = -1;
		m_nScrollTo = 0.0f;
		m_scroll = 0.0f;
		m_timeScroll = 0.0f;
		m_iconRowScroll = 0;
		m_detachIcon = false;
		m_nTotalBlocks = -1;
		m_freeBlocks = -1;
	}
	else
*/
	{
		int nTitleCount = GetTitleCount();
		if (m_curTitle >= nTitleCount)
			m_curTitle = nTitleCount - 1;

        int nSavedGames = GetSavedGameCount(m_curTitle);
		int nRowCount = (nSavedGames + m_iconsPerRow - 1) / m_iconsPerRow;

		// BLOCK: Make sure the title is scrolled into view
		{
			float y = -RenderLoop(false);

			if (y != m_nScrollTo)
			{
				if (bInstantScroll)
				{
					m_nScrollTo = m_scroll = y;
					m_timeScroll = 0.001f;
				}
				else
				{
					m_nScrollTo = y;
					m_timeScroll = XAppGetNow();
				}
			}
		}

		// Select the right save icon based on the preferred column and
		// the direction the selection moved..
		if ((m_nPrefColumn > -1) && (m_curSavedGame != -1))
		{
			m_nPrefColumn = m_curSavedGame % m_iconsPerRow;
		}
		else if(m_nPrefColumn != -1)
		{
			m_nPrefColumn = 0;
		}
		
		if (m_nPrefColumn == -1)
		{
			m_curSavedGame = -1;
			m_iconRowScroll = 0;
		}
		else
		{
			if (m_curTitle > m_curTitleLast)
			{
				// going down...
				
				//if(m_nPrefColumn == 0)   // pop back out to the first saved game
				m_curSavedGame = m_nPrefColumn;

				if (m_curSavedGame > nSavedGames - 1)
					m_curSavedGame = nSavedGames - 1;

				m_iconRowScroll = 0;
			}
			else
			{
				// going up
				if (nSavedGames == 0)
				{
					m_curSavedGame = -1;
				}
				else
				{
					m_curSavedGame = m_nPrefColumn + (nRowCount - 1) * m_iconsPerRow;
					ASSERT(m_curSavedGame >= 0);
					if (m_curSavedGame > nSavedGames - 1)
						m_curSavedGame = nSavedGames - 1;
				}

				m_iconRowScroll = nRowCount - VISIBLE_ICON_ROWS;
				if (m_iconRowScroll < 0)
					m_iconRowScroll = 0;
			}
		}
	}
}

void CSavedGameGrid::selectUp()
{
	if (m_curSavedGame != -1)
	{
		int nIconRow = m_curSavedGame / m_iconsPerRow;
		if (nIconRow > 0)
		{
			nIconRow -= 1;
			m_curSavedGame = nIconRow * m_iconsPerRow + m_nPrefColumn;

			if (nIconRow < m_iconRowScroll)
				m_iconRowScroll = nIconRow;

			m_nPrefColumn = (m_curSavedGame % m_iconsPerRow);
			CallFunction(this, _T("OnSelChange"));
			g_pulseStartTime = XAppGetNow();
			return;
		}
	}

	// Only allow the user to navigate to the memory unit panels
	int initLoop;
	if(m_curDevUnit == 8)
		initLoop = 0;
	else
		initLoop = -1;

	if (m_curTitle > initLoop)
	{
		SelectTitle(m_curTitle - 1);
		CallFunction(this, _T("OnSelChange"));
		g_pulseStartTime = XAppGetNow();
	}
	m_nPrefColumn = (m_curSavedGame % m_iconsPerRow);
}

void CSavedGameGrid::selectDown()
{
	if (m_curSavedGame != -1)
	{
        int nSavedGameCount = GetSavedGameCount(m_curTitle);
		int nIconRow = m_curSavedGame / m_iconsPerRow;
		int nRowCount = (nSavedGameCount + m_iconsPerRow - 1) / m_iconsPerRow;

		if (nIconRow < nRowCount - 1)
		{
			nIconRow += 1;
			m_curSavedGame += m_iconsPerRow;
			if (m_curSavedGame > nSavedGameCount - 1)
				m_curSavedGame = nSavedGameCount - 1;

			if (nIconRow >= m_iconRowScroll + VISIBLE_ICON_ROWS - 1)
				m_iconRowScroll = nIconRow - (VISIBLE_ICON_ROWS - 1);

			m_nPrefColumn = (m_curSavedGame % m_iconsPerRow);
			CallFunction(this, _T("OnSelChange"));
			g_pulseStartTime = XAppGetNow();
			return;
		}
	}

	int nTitleCount = GetTitleCount();
	if (m_curTitle < nTitleCount - 1)
	{
		SelectTitle(m_curTitle + 1);
		CallFunction(this, _T("OnSelChange"));
		g_pulseStartTime = XAppGetNow();
	}
	m_nPrefColumn = (m_curSavedGame % m_iconsPerRow);
}

void CSavedGameGrid::selectLeft()
{
	int oldTitle = m_curTitle;
	int oldSavedGame = m_curSavedGame;

	if (m_curSavedGame >= 0)
	{
		// if the user is in the left most column, jump to title orb
		if (m_nPrefColumn == 0)
		{
			m_nPrefColumn = -1;
			m_curSavedGame = -1;
			m_iconRowScroll = 0;
		}
		else
		{
			m_curSavedGame -= 1;
			m_nPrefColumn = (m_curSavedGame % m_iconsPerRow);
		}

	}

	if (m_curTitle != oldTitle || m_curSavedGame != oldSavedGame)
	{
		CallFunction(this, _T("OnSelChange"));
		g_pulseStartTime = XAppGetNow();
	}
}

void CSavedGameGrid::selectRight()
{
	int oldTitle = m_curTitle;
	int oldSavedGame = m_curSavedGame;

    int nSavedGameCount = GetSavedGameCount(m_curTitle);
	if ((m_curSavedGame < nSavedGameCount - 1) && (m_nPrefColumn < VISIBLE_ICON_ROWS))
	{
		m_curSavedGame += 1;
		m_nPrefColumn = (m_curSavedGame % m_iconsPerRow);

		if (m_curSavedGame >= (m_iconRowScroll + VISIBLE_ICON_ROWS) * m_iconsPerRow)
			m_iconRowScroll = m_curSavedGame / m_iconsPerRow - (VISIBLE_ICON_ROWS - 1);
	}

	if (m_curTitle != oldTitle || m_curSavedGame != oldSavedGame)
	{
		CallFunction(this, _T("OnSelChange"));
		g_pulseStartTime = XAppGetNow();
	}
}

void CSavedGameGrid::setSelImage()
{
	g_szSelTitleImage = NULL;

	if(m_curTitle == -1) // Memory Unit
	{
		_tcscpy(szSelectionBuf, _T("memoryUnit128.tga"));
	}
	else if(m_curTitle >= 0 && (m_nSoundtrackTitle < 0 || m_curTitle < m_nSoundtrackTitle)) // Game title
	{
		if(m_curSavedGame != -1)
		{
			g_titles[m_curDevUnit].GetSavedGameImageName(m_curTitle, m_curSavedGame, szSelectionBuf);
		}
		else  // Title
		{
			MakePath(szSelectionBuf, g_titles[m_curDevUnit].GetUData(), GetTitleID2(m_curTitle));
			MakePath(szSelectionBuf, szSelectionBuf, szTitleImageXBX);

			if (!DoesFileExist(szSelectionBuf)) //Title doesn't have an associated image
			{
#ifdef COOL_XDASH
				_tcscpy(szSelectionBuf, _T("xboxlogo128.xbx.cool"));
#else
				_tcscpy(szSelectionBuf, _T("xboxlogo128.xbx"));
#endif
			}
		}
	}
	else //soundtrack
	{
		_tcscpy(szSelectionBuf, _T("soundtracksave3.tga"));
	}
	
	g_szSelTitleImage = szSelectionBuf;
}

CStrObject* CSavedGameGrid::FormatSavedGameName()
{
	if (m_curSavedGame < 0)
		return new CStrObject; // empty string

	if (m_nSoundtrackTitle >= 0 && m_curTitle == m_nSoundtrackTitle)
		return new CStrObject(GetSoundtrackName(m_curSavedGame));

	TCHAR szSavedGamePath [MAX_PATH];
	if (!GetSavedGamePath(szSavedGamePath, m_curTitle, m_curSavedGame))
		return new CStrObject; // empty string

	MakePath(szSavedGamePath, szSavedGamePath, szSaveDataXBX);

	TCHAR szSaveName [64];
    TCHAR szLangCode[MAX_LANGUAGE_CODE_LEN];
    TCHAR szTranslate[MAX_TRANSLATE_LEN];
	CSettingsFile settings;
	if (!settings.Open(szSavedGamePath) || !settings.GetValue(GetLanguageCode(szLangCode), _T("Name"), szSaveName, countof(szSaveName)))
		return new CStrObject(Translate(_T("Broken Save"), szTranslate));

	return new CStrObject(szSaveName);
}

CStrObject* CSavedGameGrid::FormatSavedGameTime()
{
	if (m_nSoundtrackTitle >= 0 && m_curTitle == m_nSoundtrackTitle)
		return new CStrObject; // empty string

	TCHAR szPath [MAX_PATH];
	if (m_curSavedGame < 0 || !GetSavedGamePath(szPath, m_curTitle, m_curSavedGame))
		return new CStrObject; // empty string

	HANDLE hFile = XAppCreateFile(szPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return new CStrObject; // empty string

	FILETIME ft;
	VERIFY(GetFileTime(hFile, NULL, NULL, &ft));
	CloseHandle(hFile);

	FILETIME lft;
	VERIFY(FileTimeToLocalFileTime(&ft, &lft));

	SYSTEMTIME st;
	VERIFY(FileTimeToSystemTime(&lft, &st));

	TCHAR szBuf [32];
	FormatTime(szBuf, countof(szBuf), &st);

	return new CStrObject(szBuf);
}

int CSavedGameGrid::GetSavedGameCount(int nTitle)
{
	ASSERT(m_curDevUnit >= 0 && m_curDevUnit <= 8);

	if (nTitle < 0)
		return 0;

    if (g_titles[m_curDevUnit].IsDirty())
    {
        return 0;
    }

	if (m_nSoundtrackTitle < 0 || nTitle < m_nSoundtrackTitle)
        return g_titles[m_curDevUnit].GetSavedGameCount(nTitle);

	return m_nSoundtrackCount;
}

CStrObject* CSavedGameGrid::FormatSavedGameSize()
{
	int nBlocks;

	if (m_nSoundtrackTitle >= 0 && m_curTitle == m_nSoundtrackTitle)
	{
		nBlocks = GetSoundtrackSize(m_curSavedGame);
	}
	else
	{
		TCHAR szSavedGameDir [MAX_PATH];

		if (m_curSavedGame < 0 || !GetSavedGamePath(szSavedGameDir, m_curTitle, m_curSavedGame))
			return new CStrObject; // empty string

        nBlocks = m_nCacheSavedGameSize;
	}

	m_gameBlocks = nBlocks;

	TCHAR szBuf [16];
	FormatInteger(szBuf, nBlocks);
	return new CStrObject(szBuf);
}

CStrObject* CSavedGameGrid::FormatTitleSize()
{
    int nBlocks = m_nCacheTitleSize;

	TCHAR szBuf [16];
	FormatInteger(szBuf, nBlocks);
	return new CStrObject(szBuf);
}

bool CSavedGameGrid::GetSavedGamePath(TCHAR* szBuf, int nTitle, int nSavedGame)
{
    if (!g_titles[m_curDevUnit].IsValid())
    {
        return false;
    }

	MakePath(szBuf, g_titles[m_curDevUnit].GetUData(), GetTitleID2(nTitle));
	MakePath(szBuf, szBuf, GetSavedGameID2(nTitle, nSavedGame));
	return true;
}

CStrObject* CSavedGameGrid::GetSavedGamePath(int nTitle, int nSavedGame)
{
	TCHAR szBuf [MAX_PATH];

	szBuf[0] = (m_curDevUnit == Dev0) ? 'U' : g_titles[m_curDevUnit].GetUData()[0];
	szBuf[1] = ':';
	szBuf[2] = '\\';
	_tcscpy(szBuf + 3, GetSavedGameID2(nTitle, nSavedGame));

	return new CStrObject(szBuf);
}

CStrObject* CSavedGameGrid::FormatFreeBlocks()
{
	TCHAR szBuf [16];
	FormatBlocks (szBuf, m_freeBlocks);
	return new CStrObject(szBuf);
}

CStrObject* CSavedGameGrid::FormatTotalBlocks()
{
	TCHAR szBuf [16];
	FormatBlocks(szBuf, m_nTotalBlocks);
	return new CStrObject(szBuf);
}

int CSavedGameGrid::GetTotalBlocks()
{
	return m_nTotalBlocks;
}

int CSavedGameGrid::CanDetachIcon()
{
	return m_timeScroll == 0.0f && bMatrixHackValid;
}

void CSavedGameGrid::StartCopy(int destDevUnit)
{
	if (m_curSavedGame == -1)
	{
		TRACE(_T("Attempted to copy a title!\n"));
		CallFunction(this, _T("OnCopyComplete"));
		return;
	}

	int nBlocks = 0;

	// REVIEW: If we have muti-select, do this for each item...
	{
		TCHAR szBuf [MAX_PATH];
		VERIFY(GetSavedGamePath(szBuf, m_curTitle, m_curSavedGame));
		nBlocks = GetDirectoryBlocks(szBuf, BLOCK_SIZE, true, NULL);

		// BLOCK: See if we need the title data XBX file...
		{
			MakePath(szBuf, g_titles[destDevUnit].GetUData(), GetTitleID2(m_curTitle));
			MakePath(szBuf, szBuf, szTitleDataXBX);

			if (!DoesFileExist(szBuf))
			{
				MakePath(szBuf, g_titles[m_curDevUnit].GetUData(), GetTitleID2(m_curTitle));
				MakePath(szBuf, szBuf, szTitleDataXBX);
				nBlocks += GetFileBlocks(szBuf, BLOCK_SIZE);
			}
		}

		// BLOCK: See if we need the title image XBX file...
		{
			MakePath(szBuf, g_titles[destDevUnit].GetUData(), GetTitleID2(m_curTitle));
			MakePath(szBuf, szBuf, szTitleImageXBX);

			if (!DoesFileExist(szBuf))
			{
				MakePath(szBuf, g_titles[m_curDevUnit].GetUData(), GetTitleID2(m_curTitle));
				MakePath(szBuf, szBuf, szTitleImageXBX);
				nBlocks += GetFileBlocks(szBuf, BLOCK_SIZE);
			}
		}

		// BLOCK: See if we need the default save image XBX file...
		{
			MakePath(szBuf, g_titles[destDevUnit].GetUData(), GetTitleID2(m_curTitle));
			MakePath(szBuf, szBuf, szSaveImageXBX);

			if (!DoesFileExist(szBuf))
			{
				MakePath(szBuf, g_titles[m_curDevUnit].GetUData(), GetTitleID2(m_curTitle));
				MakePath(szBuf, szBuf, szSaveImageXBX);
				nBlocks += GetFileBlocks(szBuf, BLOCK_SIZE);
			}
		}
	}

	m_bCopying = true;
    m_timeCopyingStarted = XAppGetNow();
	m_copyProgress = 0.0f;
	m_strProgress = GetTickCount();
	m_periodStatus = 0;
	m_szStatus[0] = 0;
	Translate(_T("Copying"), m_szStatus);

	theGameCopier.SetSource(m_curDevUnit);
	theGameCopier.SetDestination(destDevUnit);
	theGameCopier.AddGame(GetTitleID2(m_curTitle), GetSavedGameID2(m_curTitle, m_curSavedGame), GetSavedGameTime(m_curTitle, m_curSavedGame), nBlocks);
	theGameCopier.Start();
}

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

void WINAPI CSavedGameGrid::DeleteThread(CSavedGameGrid* p)
{
	ASSERT(p->m_curTitle >= 0);

    //
    //  Bug 6120: If we are asked to delete the very
    //  last soundtrack, then switch to the case of
    //  deleting all sound tracks.
    //
	if(p->m_curTitle == p->m_nSoundtrackTitle && 1==p->m_nSoundtrackCount)
    {
        p->m_curSavedGame = -1;
    }

	if (p->m_curSavedGame == -1)
	{
		// Remove an entire title...

		if (p->m_curTitle == p->m_nSoundtrackTitle)
		{
			DeleteAllSoundtracks();
			p->m_nSoundtrackTitle = -1;
			p->m_nSoundtrackCount = 0;
		}
		else
		{
			TCHAR szTitleID[MAX_PATH];
            TCHAR szPublisherID[16];
			TCHAR szBuf [MAX_PATH];

            lstrcpyn(szTitleID, p->GetTitleID2(p->m_curTitle), countof(szTitleID));
            lstrcpyn(szPublisherID, szTitleID, countof(szPublisherID));

            // Strip off low word so that it contains only publisher ID
            szPublisherID[4] = 0;

            g_titles[p->m_curDevUnit].RemoveTitle(p->m_curTitle);

            // Delete shared publisher directory if it was the last title from this publisher
            if (!g_titles[p->m_curDevUnit].IsPublisherExists(szPublisherID))
            {
                lstrcat(szPublisherID, _T("ffff"));
                MakePath(szBuf, g_titles[p->m_curDevUnit].GetTData(), szPublisherID);
                TRACE(_T("Cleaning up shared publisher directory: %s\n"), szBuf);
                theGameCopier.DeleteDirectory(szBuf);
            }

            // Delete saved game
			MakePath(szBuf, g_titles[p->m_curDevUnit].GetUData(), szTitleID);
			theGameCopier.DeleteDirectory(szBuf);

			if (p->m_curDevUnit == Dev0) // only the hard drive...
			{
                // Delete any data in TDATA
				MakePath(szBuf, g_titles[p->m_curDevUnit].GetTData(), szTitleID);
				theGameCopier.DeleteDirectory(szBuf);
				XapiDeleteCachePartition(ParseTitleID(szTitleID));
			}

			if (p->m_nSoundtrackTitle >= 0)
				p->m_nSoundtrackTitle -= 1;
		}

		p->m_timeOfDelete = XAppGetNow();
		p->m_nDeletedTitle = p->m_curTitle;
		ASSERT(bMatrixHackValid);

		// Fixup selection...
		p->SelectTitle(p->m_curTitle, true);
	}
	else
	{
		if (p->m_curTitle == p->m_nSoundtrackTitle)
		{
            DeleteSoundtrack(p->m_curSavedGame);
			p->m_nSoundtrackCount -= 1;
		}
		else
		{
			// Delete a saved game...

			TCHAR szBuf [MAX_PATH];
			VERIFY(p->GetSavedGamePath(szBuf, p->m_curTitle, p->m_curSavedGame));

			theGameCopier.m_error = false;
			theGameCopier.DeleteDirectory(szBuf);

			g_titles[p->m_curDevUnit].RemoveSavedGame(p->m_curTitle, p->m_curSavedGame);
		}

		// Fixup selection...
        int nSavedGameCount = p->GetSavedGameCount(p->m_curTitle);
		if (p->m_curSavedGame > nSavedGameCount - 1)
			p->m_curSavedGame = nSavedGameCount - 1;
		p->m_nPrefColumn = (p->m_curSavedGame % p->m_iconsPerRow);

        // Also fix up the scroll position (Bug 6090)
        // If not at the top and the last visible row
        // would now be empty scroll up.
        if(p->m_iconRowScroll && ((p->m_iconRowScroll + VISIBLE_ICON_ROWS - 1)*p->m_iconsPerRow >= nSavedGameCount))
        {
            p->m_iconRowScroll--;
        }
	}

    // Invalidate cache
    p->m_nCacheTitleSize = -1;
    p->m_nCacheSavedGameSize = -1;
}

void CSavedGameGrid::StartDelete()
{
    ASSERT(m_hDeleteThread == NULL);
    ASSERT(!m_bDeleting);

    // Cancel previous pending query
    if (m_bSavedGameQueryPending)
    {
        SignalObjectAndWait(m_hCancelEvent, m_hStopEvent, INFINITE, FALSE);
    }

    m_hDeleteThread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)DeleteThread, this, 0, 0);

    if (!m_hDeleteThread)
    {
        DeleteThread(this);
        VERIFY(CallFunction(this, _T("OnDeleteEnd")));
        VERIFY(CallFunction(this, _T("OnSelChange")));
        g_pulseStartTime = XAppGetNow();
    }
    else
    {
        m_bDeleting = true;
    }
}

int CSavedGameGrid::CanCopy()
{
	if (m_curTitle < 0 || IsSoundtrackSelected())
		return false; // can't copy devices or soundtracks

	TCHAR szSavedGamePath [MAX_PATH];

	if (m_curSavedGame < 0 || !GetSavedGamePath(szSavedGamePath, m_curTitle, m_curSavedGame))
		return false; // can't copy saves when one isn't selected

	MakePath(szSavedGamePath, szSavedGamePath, szSaveDataXBX);

	CSettingsFile settings;
	if (!settings.Open(szSavedGamePath))
		return false; // can't copy broken saves

	TCHAR szSaveName [64];
    TCHAR szLangCode[MAX_LANGUAGE_CODE_LEN];
	if (!settings.GetValue(GetLanguageCode(szLangCode), _T("Name"), szSaveName, countof(szSaveName)))
		return false; // can't copy broken saved game

	if(g_titles[m_curDevUnit].IsBroken(m_curTitle))
		return false; // can't copy a saved game in a broken title

	TCHAR szNoCopy [64];
	if (!settings.GetValue(GetLanguageCode(szLangCode), _T("NoCopy"), szNoCopy, countof(szNoCopy)))
		return true; // can copy if it doesn't say otherwise

	return _ttoi(szNoCopy) == 0;
}

int CSavedGameGrid::IsSoundtrackSelected()
{
	return (m_nSoundtrackTitle >= 0 && m_curTitle == m_nSoundtrackTitle);
}

int CSavedGameGrid::IsDevUnitReady(int nUnit)
{
    return !g_titles[nUnit].IsDirty();
}

int CSavedGameGrid::DoesSavedGameExists(int destDevUnit)
{
    if (m_curSavedGame < 0)
    {
        return false;
    }

	const TCHAR* szRoot = g_titles[destDevUnit].GetUData();
    ASSERT(szRoot[0] != 0);

    TCHAR szBuf[MAX_PATH];
	MakePath(szBuf, g_titles[destDevUnit].GetUData(), GetTitleID2(m_curTitle));
	MakePath(szBuf, szBuf, GetSavedGameID2(m_curTitle, m_curSavedGame));

    return DoesFileExist(szBuf);
}

