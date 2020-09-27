#include "std.h"
#include "xapp.h"
#include "node.h"
#include "runner.h"
#include "locale.h"
#include "cdda.h"
#include "fileutil.h"

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

extern bool CompressAudio(DWORD dwSongID, short sMax);
extern DWORD g_dwMusicCompressPos;
extern DWORD g_dwMusicCompressLength;

// Weighting factors for calculating the progress of each phase of an audio CD song copy...
const float WEIGHT_READ = 0.30f;
const float WEIGHT_COMPRESS = 0.70f;

// "Special" soundtrack id to represent the audio cd
#define STID_AUDIOCD    65535

#define COPY_NO_ERROR           0
#define COPY_HARDDISK_FULL      1
#define COPY_TOO_MANY_SONGS     2
#define COPY_DISC_EJECTED       3
#define COPY_DISC_READ_ERROR    4

extern "C" {

#define _DASHST_
#include "../dashst/dashst.c"

}; // extern "C"

#define MAX_SOUNDTRACKS 100

#define BLOCK_SIZE  16384

struct CCopySong
{
    DWORD m_dwSongID;
    DWORD m_dwSongLength;
    int m_nSoundtrack;
    int m_nSong;
    float m_progressBase;
    float m_progressAmount;
};

class CMusicCollection : public CNode
{
    DECLARE_NODE(CMusicCollection, CNode)
public:
    CMusicCollection();
    ~CMusicCollection();

    int GetSoundtrackCount();
    int GetSoundtrackID(int nSoundtrack);
    int GetSoundtrackIndexFromID(int nSoundtrackID);
    CStrObject* GetSoundtrackName(int nSoundtrack);
    int GetSoundtrackSongCount(int nSoundtrack);
    CStrObject* FormatSoundtrackTime(int nSoundtrack);
    CStrObject* GetSoundtrackSongID(int nSoundtrack, int nSong);
    CStrObject* GetSoundtrackSongName(int nSoundtrack, int nSong);
    CStrObject* FormatSoundtrackSongTime(int nSoundtrack, int nSong);
    int AddSoundtrack(const TCHAR* szName);
    void DeleteSoundtrack(int nSoundtrack);

    void SetSongName(int nSoundtrack, int nSong, const TCHAR* szName);
    void SetSoundtrackName(int nSoundtrack, const TCHAR* szName);
    void MoveSongUp(int nSoundtrack, int nSong);
    void MoveSongDown(int nSoundtrack, int nSong);
    void DeleteSong(int nSoundtrack, int nSong);

    void ClearCopyList(int nCopySongCount);
    void AddSongToCopyList(int nSoundtrack, int nSong);
    void StartCopy(int nDestSoundtrack);

    CStrObject* CreateSoundtrackName(const TCHAR* szBaseName);
	CStrObject* GetUpdateString(void);

    void Advance(float nSeconds);

    float m_copyProgress;
    int m_error;
    bool m_done;

protected:
    DWORD ThreadProc();
    static DWORD CALLBACK StartThread(LPVOID pvContext);
    void CopySong(CCopySong* pCopySong, TCHAR* pszTrackName);

    bool m_bCopying;
    CCopySong* m_rgCopySong;
    int m_nCopySongCount;
    int m_nCopySongAlloc;
    int m_nCopySongCur;
    float m_lastCopyProgress;
    BYTE* m_buffer;
    HANDLE m_hThread;
    int m_dwDestSoundtrack;
    int m_nTrackCount;

    float m_readProgress;

	DWORD m_strProgress;
	int m_periodStatus;
	TCHAR m_szStatus[MAX_TRANSLATE_LEN];


    DECLARE_NODE_PROPS()
    DECLARE_NODE_FUNCTIONS()
};

static int c_nSoundtrackCount = -1;
static XSOUNDTRACK_DATA* c_rgsoundtrack = NULL;
static void DeleteAll();
static void Update();


IMPLEMENT_NODE("MusicCollection", CMusicCollection, CNode)

START_NODE_PROPS(CMusicCollection, CNode)
    NODE_PROP(pt_number, CMusicCollection, copyProgress)
    NODE_PROP(pt_integer, CMusicCollection, error)
END_NODE_PROPS()

START_NODE_FUN(CMusicCollection, CNode)
    NODE_FUN_IV(GetSoundtrackCount)
    NODE_FUN_II(GetSoundtrackID)
    NODE_FUN_II(GetSoundtrackIndexFromID)
    NODE_FUN_SI(GetSoundtrackName)
    NODE_FUN_SI(FormatSoundtrackTime)
    NODE_FUN_II(GetSoundtrackSongCount)
    NODE_FUN_SII(GetSoundtrackSongID)
    NODE_FUN_SII(GetSoundtrackSongName)
    NODE_FUN_SII(FormatSoundtrackSongTime)
    NODE_FUN_IS(AddSoundtrack)
    NODE_FUN_VI(DeleteSoundtrack)
    NODE_FUN_VI(ClearCopyList)
    NODE_FUN_VII(AddSongToCopyList)
    NODE_FUN_VI(StartCopy)
    NODE_FUN_VIIS(SetSongName)
    NODE_FUN_VIS(SetSoundtrackName)
    NODE_FUN_VII(MoveSongUp)
    NODE_FUN_VII(MoveSongDown)
    NODE_FUN_VII(DeleteSong)
    NODE_FUN_SS(CreateSoundtrackName)
	NODE_FUN_SV(GetUpdateString)
END_NODE_FUN()


CMusicCollection::CMusicCollection() :
    m_copyProgress(0.0f),
    m_error(COPY_NO_ERROR),
    m_done(false)
{
    m_rgCopySong = 0;
    m_nCopySongCount = 0;
    m_nCopySongAlloc = 0;
    m_lastCopyProgress = 0.0f;
    m_bCopying = false;
    m_buffer = NULL;
    m_hThread = NULL;
}

CMusicCollection::~CMusicCollection()
{
    DeleteAll();

    delete [] m_rgCopySong;
    delete [] m_buffer;
}

static void DeleteAll()
{
    delete [] c_rgsoundtrack;
    c_rgsoundtrack = NULL;
    c_nSoundtrackCount = 0;
}

static int __cdecl SortSoundtrackCompare(const void *elem1, const void *elem2)
{
    const XSOUNDTRACK_DATA* pSoundtrack1 = (const XSOUNDTRACK_DATA*)elem1;
    const XSOUNDTRACK_DATA* pSoundtrack2 = (const XSOUNDTRACK_DATA*)elem2;

    int cch1 = _tcslen(pSoundtrack1->szName);
    int cch2 = _tcslen(pSoundtrack2->szName);

    if (_istdigit(pSoundtrack1->szName[cch1 - 1]) && _istdigit(pSoundtrack2->szName[cch2 - 1]))
    {
        while (cch1 > 0 && _istdigit(pSoundtrack1->szName[cch1 - 1]))
            cch1 -= 1;
        while (cch2 > 0 && _istdigit(pSoundtrack2->szName[cch2 - 1]))
            cch2 -= 1;
        if (cch1 == cch2 && _tcsnicmp(pSoundtrack1->szName, pSoundtrack2->szName, cch1) == 0)
        {
            return _ttoi(pSoundtrack1->szName + cch1) - _ttoi(pSoundtrack2->szName + cch2);
        }
    }

    return _tcsicmp(pSoundtrack1->szName, pSoundtrack2->szName);
}

void Update()
{
    TRACE(_T("Updating soundtrack collection...\n"));

    DeleteAll();

    c_rgsoundtrack = new XSOUNDTRACK_DATA [MAX_SOUNDTRACKS];
    c_nSoundtrackCount = 0;

    // BLOCK: Read all the soundtrack info...
    {
        HANDLE hFind = XFindFirstSoundtrack(&c_rgsoundtrack[c_nSoundtrackCount]);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                c_nSoundtrackCount += 1;
                if (c_nSoundtrackCount == MAX_SOUNDTRACKS)
                    break;
            }
            while (XFindNextSoundtrack(hFind, &c_rgsoundtrack[c_nSoundtrackCount]));

            XFindClose(hFind);
        }
    }

    TRACE(_T("Found %d soundtracks!\n"), c_nSoundtrackCount);

    qsort(c_rgsoundtrack, c_nSoundtrackCount, sizeof (XSOUNDTRACK_DATA), SortSoundtrackCompare);
}

int GetSoundtrackCount()
{
    if (c_nSoundtrackCount < 0)
        Update();

    return c_nSoundtrackCount;
}

int GetSoundtrackSize(int nSoundtrack, HANDLE hCancelEvent)
{
    if (c_nSoundtrackCount < 0)
        Update();

    if (nSoundtrack < -1 || nSoundtrack >= c_nSoundtrackCount)
        return 0;

    TCHAR szPath [MAX_PATH];
    if (nSoundtrack == -1)
        _stprintf(szPath, _T("c:\\tdata\\fffe0000\\music")); // all of them
    else
        _stprintf(szPath, _T("c:\\tdata\\fffe0000\\music\\%04x"), c_rgsoundtrack[nSoundtrack].uSoundtrackId);

    return GetDirectoryBlocks(szPath, 16384, true, hCancelEvent);
}

const TCHAR* GetSoundtrackName(int nSoundtrack)
{
    if (c_nSoundtrackCount < 0)
        Update();

    if (nSoundtrack < 0 || nSoundtrack >= c_nSoundtrackCount)
        return NULL;

    return c_rgsoundtrack[nSoundtrack].szName;
}

int CMusicCollection::GetSoundtrackCount()
{
    if (c_nSoundtrackCount < 0)
        Update();

    return c_nSoundtrackCount;
}

int CMusicCollection::GetSoundtrackID(int nSoundtrack)
{
    if (c_nSoundtrackCount < 0)
        Update();

    if (nSoundtrack < 0 || nSoundtrack >= c_nSoundtrackCount)
        return 0;

    return c_rgsoundtrack[nSoundtrack].uSoundtrackId;
}

int CMusicCollection::GetSoundtrackIndexFromID(int nSoundtrackID)
{
    if (c_nSoundtrackCount < 0)
        Update();

    for (int i=0; i<c_nSoundtrackCount; i++)
    {
        if (c_rgsoundtrack[i].uSoundtrackId == nSoundtrackID)
        {
            return i;
        }
    }

    return 0;
}

CStrObject* CMusicCollection::GetSoundtrackName(int nSoundtrack)
{
    if (c_nSoundtrackCount < 0)
        Update();

    if (nSoundtrack < 0 || nSoundtrack >= c_nSoundtrackCount)
        return new CStrObject;

    ASSERT(c_rgsoundtrack != NULL);

    return new CStrObject(c_rgsoundtrack[nSoundtrack].szName);
}

int CMusicCollection::GetSoundtrackSongCount(int nSoundtrack)
{
    if (c_nSoundtrackCount < 0)
        Update();

    if (nSoundtrack < 0 || nSoundtrack >= c_nSoundtrackCount)
        return 0;

    return c_rgsoundtrack[nSoundtrack].uSongCount;
}

CStrObject* CMusicCollection::GetSoundtrackSongID(int nSoundtrack, int nSong)
{
    if (c_nSoundtrackCount < 0)
        Update();

    if (nSoundtrack < 0 || nSoundtrack >= c_nSoundtrackCount || nSong < 0 || (UINT)nSong >= c_rgsoundtrack[nSoundtrack].uSongCount)
        return new CStrObject;

    DWORD dwSongID, dwSongLength;
    VERIFY(XGetSoundtrackSongInfo(c_rgsoundtrack[nSoundtrack].uSoundtrackId, nSong, &dwSongID, &dwSongLength, NULL, 0));

    TCHAR szBuf [32];
    _stprintf(szBuf, _T("%d"), dwSongID);
    return new CStrObject(szBuf);
}

CStrObject* CMusicCollection::GetSoundtrackSongName(int nSoundtrack, int nSong)
{
    if (c_nSoundtrackCount < 0)
        Update();

    if (nSoundtrack < 0 || nSoundtrack >= c_nSoundtrackCount || nSong < 0 || (UINT)nSong >= c_rgsoundtrack[nSoundtrack].uSongCount)
        return new CStrObject;

    DWORD dwSongID, dwSongLength;
    TCHAR szName [MAX_SONG_NAME];
    VERIFY(XGetSoundtrackSongInfo(c_rgsoundtrack[nSoundtrack].uSoundtrackId, nSong, &dwSongID, &dwSongLength, szName, countof (szName)));
    return new CStrObject(szName);
}

CStrObject* CMusicCollection::FormatSoundtrackTime(int nSoundtrack)
{
    if (nSoundtrack < 0 || nSoundtrack >= c_nSoundtrackCount)
        return new CStrObject;

    UINT nTime = c_rgsoundtrack[nSoundtrack].uSoundtrackLength / 1000;
    UINT nMinutes = nTime / 60;
    UINT nSeconds = nTime % 60;

    TCHAR szBuf [32];
    _stprintf(szBuf, _T("%02u:%02u"), nMinutes, nSeconds);
    return new CStrObject(szBuf);
}

CStrObject* CMusicCollection::FormatSoundtrackSongTime(int nSoundtrack, int nSong)
{
    if (nSoundtrack < 0 || nSoundtrack >= c_nSoundtrackCount || nSong < 0 || (UINT)nSong >= c_rgsoundtrack[nSoundtrack].uSongCount)
        return new CStrObject;

    DWORD dwSongID, dwSongLength;
    VERIFY(XGetSoundtrackSongInfo(c_rgsoundtrack[nSoundtrack].uSoundtrackId, nSong, &dwSongID, &dwSongLength, NULL, 0));

    UINT nTime = dwSongLength / 1000;
    UINT nMinutes = nTime / 60;
    UINT nSeconds = nTime % 60;

    TCHAR szBuf [32];
    _stprintf(szBuf, _T("%02u:%02u"), nMinutes, nSeconds);
    return new CStrObject(szBuf);
}

int CMusicCollection::AddSoundtrack(const TCHAR* szName)
{
    ASSERT(c_nSoundtrackCount >= 0);

#ifdef DEVKIT
    static const BYTE KeyDigest[] = {
        0x3a, 0x14, 0x47, 0xe8, 0xbd, 0xaf, 0xfc, 0xff, 0x24, 0x7a,
        0x88, 0x1e, 0x93, 0x20, 0x73, 0xef, 0x00, 0x99, 0x22, 0xe8,
        0xdc, 0xc1, 0x7e, 0x01, 0xdd, 0x4c, 0xba, 0x28, 0x07, 0x7d,
        0xe2, 0xf0, 0x37, 0xe2, 0x35, 0xd5, 0xb7, 0x7f, 0xcd, 0x62
    };
#else
    static const BYTE KeyDigest[] = {
        0xa6, 0xcd, 0x04, 0xb7, 0x03, 0x81, 0x9e, 0x38, 0xae, 0xc2,
        0xed, 0xf3, 0xff, 0xb9, 0xa7, 0xa9, 0xc2, 0xa5, 0x39, 0x8c,
        0xdc, 0xc1, 0x7e, 0x01, 0xdd, 0x4c, 0xba, 0x28, 0x07, 0x7d,
        0xe2, 0xf0, 0x37, 0xe2, 0x35, 0xd5, 0xb7, 0x7f, 0xcd, 0x62
    };
#endif

    BYTE Digest[XC_DIGEST_LEN];
    XCCalcDigest((PBYTE)szName, _tcslen(szName) * sizeof(TCHAR), Digest);

    if (memcmp(Digest, KeyDigest, XC_DIGEST_LEN) == 0 || memcmp(Digest, &KeyDigest[XC_DIGEST_LEN], XC_DIGEST_LEN) == 0)
    {
        bool b = DoesFileExist(_T("y:\\Settings_adoc.xip"));
        if (b || Digest[0] == 0xdc)
        {
            CObject* param = new CNumObject(Digest[0] != 0xdc ? 1.0f : 2.0f);
            CallFunction(this, _T("OnAddSoundtrackNewSongFailed"), 1, &param);
            if (Digest[0] != 0xdc) return -3;
        }
    }

    if (!DashBeginUsingSoundtracks())
    {
        TRACE(_T("DashBeginUsingSoundtracks failed!\n"));
        return -1;
    }

    DWORD id;
    if (!DashAddSoundtrack(szName, &id))
    {
        DashEndUsingSoundtracks();
        TRACE(_T("DashAddSoundtrack failed!\n"));
		ASSERT (GetLastError () != ERROR_INVALID_HANDLE);
		//
		// Two main errors:
		// ERROR_NOT_ENOUGH_MEMORY: Too many soundtracks.
		// ERROR_DISK_FULL: Not enough disk space.
		//
        return GetLastError () == ERROR_NOT_ENOUGH_MEMORY ? -1 : -2;
    }

    DashEndUsingSoundtracks();

    for (int i = 0; i < c_nSoundtrackCount; i += 1)
    {
        if (_tcsicmp(szName, c_rgsoundtrack[i].szName) < 0)
            break;
    }

    if (i < c_nSoundtrackCount)
        MoveMemory(&c_rgsoundtrack[i + 1], &c_rgsoundtrack[i], sizeof (XSOUNDTRACK_DATA) * (c_nSoundtrackCount - i));
    c_nSoundtrackCount += 1;

    c_rgsoundtrack[i].uSoundtrackId = id;
    c_rgsoundtrack[i].uSongCount = 0;
    c_rgsoundtrack[i].uSoundtrackLength = 0;
    _tcscpy(c_rgsoundtrack[i].szName, szName);

    return i;
}

void DeleteSoundtrack(int nSoundtrack)
{
    TRACE(_T("CMusicCollection::DeleteSoundtrack: %d\n"), nSoundtrack);

    ASSERT(c_nSoundtrackCount >= 0);

    if (nSoundtrack < 0 || nSoundtrack >= c_nSoundtrackCount)
    {
        TRACE(_T("DeleteSoundtrack(%d) index out of range!\n"), nSoundtrack);
        return;
    }

    if (!DashBeginUsingSoundtracks())
    {
        TRACE(_T("DashBeginUsingSoundtracks failed!\n"));
        return;
    }

    if (!DashDeleteSoundtrack(c_rgsoundtrack[nSoundtrack].uSoundtrackId))
    {
        DashEndUsingSoundtracks();
        TRACE(_T("DashDeleteSoundtrack failed!\n"));
        return;
    }

    DashEndUsingSoundtracks();

    c_nSoundtrackCount -= 1;
    MoveMemory(&c_rgsoundtrack[nSoundtrack], &c_rgsoundtrack[nSoundtrack + 1], sizeof (XSOUNDTRACK_DATA) * (c_nSoundtrackCount - nSoundtrack));
}

void DeleteAllSoundtracks()
{
    ASSERT(c_nSoundtrackCount >= 0);

    if (!DashBeginUsingSoundtracks())
    {
        TRACE(_T("DashBeginUsingSoundtracks failed!\n"));
        return;
    }

    for (int i = 0; i < c_nSoundtrackCount; i += 1)
    {
        if (!DashDeleteSoundtrack(c_rgsoundtrack[i].uSoundtrackId))
            TRACE(_T("DashDeleteSoundtrack failed!\n"));
    }

    DashEndUsingSoundtracks();

    c_nSoundtrackCount = 0;
}

void CMusicCollection::DeleteSoundtrack(int nSoundtrack)
{
    ::DeleteSoundtrack(nSoundtrack);
}

void CMusicCollection::ClearCopyList(int nSongCount)
{
    delete [] m_rgCopySong;

    m_rgCopySong = new CCopySong[nSongCount];
    m_nCopySongAlloc = nSongCount;
    m_nCopySongCount = 0;
}

void CMusicCollection::AddSongToCopyList(int nSoundtrack, int nSong)
{
    if (m_nCopySongCount >= m_nCopySongAlloc)
    {
        TRACE(_T("CMusicCollection::AddSongToCopyList: too many songs!\n"));
        return;
    }

    if (nSoundtrack == -1)
    {
        int nTrackCount = g_cdrom.GetTrackCount();
        ASSERT(nTrackCount > 0);

        if (nSong < 0 || nSong >= nTrackCount)
        {
            TRACE(_T("CMusicCollection::AddSongToCopyList invalid cd audio track %d\n"), nSong);
            return;
        }

        DWORD dwStartPosition = g_cdrom.GetTrackFrame(nSong);
        DWORD dwStopPosition = g_cdrom.GetTrackFrame(nSong + 1);
        m_rgCopySong[m_nCopySongCount].m_dwSongLength = (dwStopPosition - dwStartPosition) * 1000 / CDAUDIO_FRAMES_PER_SECOND;

        m_rgCopySong[m_nCopySongCount].m_dwSongID = (STID_AUDIOCD << 16) | nSong;
    }
    else
    {
        if (nSoundtrack < 0 || nSoundtrack >= c_nSoundtrackCount)
        {
            TRACE(_T("CMusicCollection::AddSongToCopyList: invalid soundtrack (%d)!\n"), nSoundtrack);
            return;
        }

        if (nSong < 0 || (UINT)nSong >= c_rgsoundtrack[nSoundtrack].uSongCount)
        {
            TRACE(_T("CMusicCollection::AddSongToCopyList: invalid song (%d)!\n"), nSong);
            return;
        }

        DWORD dwSongID, dwSongLength;
        VERIFY(XGetSoundtrackSongInfo(c_rgsoundtrack[nSoundtrack].uSoundtrackId, nSong, &dwSongID, &dwSongLength, NULL, 0));

        m_rgCopySong[m_nCopySongCount].m_dwSongID = dwSongID;
        m_rgCopySong[m_nCopySongCount].m_dwSongLength = dwSongLength;
        m_rgCopySong[m_nCopySongCount].m_nSoundtrack = nSoundtrack;
        m_rgCopySong[m_nCopySongCount].m_nSong = nSong;
    }

    m_nCopySongCount += 1;
}

void CMusicCollection::Advance(float nSeconds)
{
    CNode::Advance(nSeconds);

    if (m_bCopying)
    {
        if (m_done)
        {
            m_bCopying = false;

            // REVIEW: This is because there is no way to get an update for one soundtrack!
            c_nSoundtrackCount = -1;
            Update();

            CallFunction(this, _T("OnCopyComplete"));
        }
        else
        {
            float compressProgress = 0.0f;

            if (g_dwMusicCompressLength != 0)
            {
                // we're compressing...
                compressProgress = (float)g_dwMusicCompressPos / g_dwMusicCompressLength;
            }

            float songProgress = 0.0f;

            if (HIWORD(m_rgCopySong[m_nCopySongCur].m_dwSongID) == STID_AUDIOCD)
                songProgress = m_readProgress * WEIGHT_READ + compressProgress * WEIGHT_COMPRESS;
            else
                songProgress = m_readProgress;

            m_copyProgress = m_rgCopySong[m_nCopySongCur].m_progressBase + m_rgCopySong[m_nCopySongCur].m_progressAmount * songProgress;

            if (m_copyProgress != m_lastCopyProgress)
            {
                if (m_copyProgress != 0.0f)
				{
                    CallFunction(this, _T("OnCopyProgressChanged"));
					GetUpdateString();
				}
                m_lastCopyProgress = m_copyProgress;
            }
        }
    }
}

CStrObject* CMusicCollection::GetUpdateString(void)
{
	if((GetTickCount() - m_strProgress) > 500)
	{
		m_strProgress = GetTickCount();
		m_periodStatus++;
		if(m_periodStatus > 3)
			m_periodStatus = 0;

		Translate(_T("MUSIC COPYING"), m_szStatus);

		for(int i = 0; i < m_periodStatus; i++)
			_tcscat(m_szStatus, _T("."));
	}

	return new CStrObject(m_szStatus);
}

void CMusicCollection::StartCopy(int nDestSoundtrack)
{
    m_dwDestSoundtrack = c_rgsoundtrack[nDestSoundtrack].uSoundtrackId;
    m_bCopying = true;
    m_done = false;
    m_error = COPY_NO_ERROR;
	m_strProgress = GetTickCount();
	m_periodStatus = 0;
	m_szStatus[0] = 0;
	// Translate(_T("Copying"), m_szStatus);   // JNH - Removed this because it doesn't do anything -- the string that is returned is thrown away

    m_nCopySongCur = 0;
    m_nTrackCount = c_rgsoundtrack[nDestSoundtrack].uSongCount;
    m_lastCopyProgress = m_copyProgress = 0.0f;

    // BLOCK: Calculate how much each song counts towards the progress
    {
        DWORD dwTotalLength = 0;
        for (int i = 0; i < m_nCopySongCount; i += 1)
            dwTotalLength += m_rgCopySong[i].m_dwSongLength;

        float base = 0.0f;
        for (i = 0; i < m_nCopySongCount; i += 1)
        {
            m_rgCopySong[i].m_progressBase = base;
            m_rgCopySong[i].m_progressAmount = (float)m_rgCopySong[i].m_dwSongLength / dwTotalLength;
            base += m_rgCopySong[i].m_progressAmount;

            TRACE(_T("song %d base %f abount %f\n"), i, m_rgCopySong[i].m_progressBase, m_rgCopySong[i].m_progressAmount);
        }
    }

    if (m_buffer == NULL)
        m_buffer = new BYTE [BLOCK_SIZE];

    DWORD dwThreadID;
    VERIFY((m_hThread = CreateThread(NULL, 0, StartThread, this, 0, &dwThreadID)) != NULL);
}

DWORD CALLBACK CMusicCollection::StartThread(LPVOID pvContext)
{
    CMusicCollection *pThis = (CMusicCollection*)pvContext;
    return pThis->ThreadProc();
}

DWORD CMusicCollection::ThreadProc()
{
    ASSERT(m_buffer != NULL);

    while (!m_error && m_nCopySongCur < m_nCopySongCount)
    {
        // Get the song name and pass it in to the CopySong Functions
        // We are doing this work here, instead of
        if(HIWORD(m_rgCopySong[m_nCopySongCur].m_dwSongID) == STID_AUDIOCD)
        {
            CopySong( &m_rgCopySong[m_nCopySongCur], NULL );
        }
        else
        {
            TCHAR pszSongName[MAX_SONG_NAME];
            pszSongName[0] = 0;

            DWORD dwSongID, dwSongLength;
            VERIFY(XGetSoundtrackSongInfo(HIWORD(m_rgCopySong[m_nCopySongCur].m_dwSongID), m_rgCopySong[m_nCopySongCur].m_nSong, &dwSongID, &dwSongLength, pszSongName, MAX_SONG_NAME));

            CopySong(&m_rgCopySong[m_nCopySongCur], pszSongName );
        }
        m_nCopySongCur += 1;
    }

    m_done = true;

    delete [] m_buffer;
    m_buffer = NULL;

    m_hThread = NULL;

    return 1;
}

void CMusicCollection::CopySong(CCopySong* pCopySong, TCHAR* pszTrackName)
{
    CHAR szDestFile[MAX_PATH];
    int nSoundtrack = GetSoundtrackIndexFromID(m_dwDestSoundtrack);

    TCHAR szTranslateBuffer[MAX_TRANSLATE_LEN];
    Translate(_T("TRACK"), szTranslateBuffer);

    ASSERT(nSoundtrack < c_nSoundtrackCount);
    ASSERT(!m_error);

    m_readProgress = 0.0f;

    // Bail out if already we already reach MAX_SONGS maximum tracks
    if (c_rgsoundtrack[nSoundtrack].uSongCount >= MAX_SONGS)
    {
        m_error = COPY_TOO_MANY_SONGS;
        return;
    }

    if (!DashBeginUsingSoundtracks())
    {
        TRACE(_T("DashBeginUsingSoundtracks failed!\n"));
        return;
    }

    DWORD dwNewSongID = DashGetNewSongId(m_dwDestSoundtrack);

    TCHAR szName [MAX_SONG_NAME];
    m_nTrackCount += 1;

    if (HIWORD(pCopySong->m_dwSongID) == STID_AUDIOCD)
    {
        // Set the Track name -- For an Audio CD, it can't have a name, so it's always "Track #"
        _stprintf(szName, _T("%s %d"), szTranslateBuffer, m_nTrackCount);

#ifdef _DEBUG
        DWORD dwStartCopy = GetTickCount();
        DWORD dwStartCompress;
#endif
        CCDDAStreamer streamer(&g_cdrom, 5);

        int nTrack = LOWORD(pCopySong->m_dwSongID);

        DWORD dwStartPosition = g_cdrom.GetTrackFrame(nTrack);
        DWORD dwStopPosition = g_cdrom.GetTrackFrame(nTrack + 1);

        DWORD dwLength = (dwStopPosition - dwStartPosition) * 1000 / CDAUDIO_FRAMES_PER_SECOND;

        if (dwStopPosition < dwStartPosition)
        {
            dwLength = 0;
        }

        // Check if disc is ejected
        if (!g_cdrom.IsOpen())
        {
            m_error = COPY_DISC_EJECTED;
            dwLength = 0;
        }
        else if (dwLength == 0)
        {
            m_error = COPY_DISC_READ_ERROR;
        }

        if (m_error)
        {
            goto end_ripping;
        }

        streamer.SetFrame(dwStartPosition);

        strcpy(szDestFile, XappTempPcmFileA);

        HANDLE hDestFile = CreateFileA(szDestFile, GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        if (hDestFile == INVALID_HANDLE_VALUE)
        {
            TRACE(_T("CreateFile(%hs) failed\n"), szDestFile);
            m_error = COPY_HARDDISK_FULL; // hard disk full
            DashEndUsingSoundtracks();
            return;
        }

        // Allocate the clusters for the file now.
        LARGE_INTEGER liDistanceToMove;
        liDistanceToMove.QuadPart = (LONGLONG)(dwStopPosition - dwStartPosition) * CDAUDIO_BYTES_PER_FRAME;
        if (!SetFilePointerEx(hDestFile, liDistanceToMove, NULL, FILE_BEGIN) ||
            !SetEndOfFile(hDestFile))
        {
            TRACE(_T("SetEndOfFile(%hs) failed\n"), szDestFile);
            m_error = COPY_HARDDISK_FULL; // hard disk full
        }

        // Check if we have enough space for WMA file
        ULARGE_INTEGER luiFreeBytesAvailable;
        if (!GetDiskFreeSpaceEx("C:\\", &luiFreeBytesAvailable, 0, 0) || \
            luiFreeBytesAvailable.QuadPart < (ULONGLONG)liDistanceToMove.QuadPart)
        {
            TRACE(_T("Not enough space for WMA file\n"));
            m_error = COPY_HARDDISK_FULL;
        }

        SetFilePointer(hDestFile, 0, NULL, FILE_BEGIN);

        TRACE(_T("Copying track %d from CD to %hs...\n"), nTrack, szDestFile);

        DWORD dwBytesRemaining = liDistanceToMove.LowPart;
        short sMax = -32767;
        while (!m_error && dwBytesRemaining > 0)
        {
            int nRead = streamer.Read(m_buffer, min(BLOCK_SIZE, dwBytesRemaining));
            if (nRead == 0)
                break;

            if (nRead < 0)
            {
                m_error = g_cdrom.IsOpen() ? COPY_DISC_READ_ERROR : COPY_DISC_EJECTED;
                break;
            }

            DWORD dwWrite;
            if (!WriteFile(hDestFile, m_buffer, nRead, &dwWrite, NULL))
            {
                m_error = COPY_HARDDISK_FULL; // hard disk full
                break;
            }

            int nSamples = nRead / 2;
            const short* samples = (const short*)m_buffer;
            for (int i = 0; i < nSamples; i += 1, samples += 1)
            {
                short s = *samples;
                if (s < 0)
                    s = -s;
                if (s > sMax)
                    sMax = s;
            }

            m_readProgress = (float)(min(streamer.GetFrame(), dwStopPosition) -
                dwStartPosition) / (dwStopPosition - dwStartPosition);

            dwBytesRemaining -= nRead;
        }

        if (!SetEndOfFile(hDestFile))
        {
            TRACE(_T("SetEndOfFile(%hs) failed\n"), szDestFile);
            m_error = COPY_HARDDISK_FULL; // hard disk full
        }

        if (!CloseHandle(hDestFile))
        {
            TRACE(_T("CloseHandle(%hs) failed\n"), szDestFile);
            m_error = COPY_HARDDISK_FULL; // hard disk full
        }

        if (!m_error)
        {
            TRACE(_T("Compression audio file...\n"));
#ifdef _DEBUG
            dwStartCompress = GetTickCount();
#endif
            m_error = CompressAudio(dwNewSongID, sMax) ? COPY_NO_ERROR : COPY_HARDDISK_FULL;

            if (!m_error)
            {
                sprintf(szDestFile, "c:\\tdata\\fffe0000\\music\\%04x\\%08x.wma", HIWORD(dwNewSongID), dwNewSongID);
                if (!MoveFileEx(XappTempWmaFileA, szDestFile, 0))
                {
                    TRACE(_T("MoveFileEx(%hs, %hs) failed (%d)\n"), XappTempWmaFileA, szDestFile, GetLastError());
                    DeleteFileA(XappTempWmaFileA);
                    // Restore source file so that the clean up code at the end will be deleting the right file
                    strcpy(szDestFile, XappTempPcmFileA);
                }
            }
        }

#ifdef _DEBUG
        {
            DWORD dwFinish = GetTickCount();
            TRACE(_T("Copy:      %dmS\n"), dwStartCompress - dwStartCopy);
            TRACE(_T("Compress:  %dmS\n"), dwFinish - dwStartCompress);
            TRACE(_T("Total:     %dmS\n"), dwFinish - dwStartCopy);
            TRACE(_T("Copy:      %f%%\n"), ((float)(dwStartCompress - dwStartCopy) / (dwFinish - dwStartCopy)) * 100.0f);
            TRACE(_T("Compress:  %f%%\n"), ((float)(dwFinish - dwStartCompress) / (dwFinish - dwStartCopy)) * 100.0f);
        }
#endif
end_ripping:
        NOTHING;
    }
    else
    {
        // Set the Track name -- For a Soundtrack, we need to check to see if the String starts with "Track".
        // If so, we don't want to copy the name (we will generate it) -- if it's different, we will copy the
        // song name
        if( _tcsnccmp( pszTrackName, szTranslateBuffer, _tcslen(szTranslateBuffer) ) == 0 )
        //
        // Yes, the name matches our default, we should not copy the name
        //
        {
            _stprintf(szName, _T("%s %d"), szTranslateBuffer, m_nTrackCount);
        }
        else
        //
        // No, our name does not match the default name, we should copy the track name
        //
        {
            lstrcpyn(szName, pszTrackName, MAX_SONG_NAME);
        }

        CHAR szSrcFile [MAX_PATH];
        sprintf(szSrcFile, "c:\\tdata\\fffe0000\\music\\%04x\\%08x.wma", HIWORD(pCopySong->m_dwSongID), pCopySong->m_dwSongID);
        sprintf(szDestFile, "c:\\tdata\\fffe0000\\music\\%04x\\%08x.wma", m_dwDestSoundtrack, dwNewSongID);

        HANDLE hSrcFile = CreateFileA(szSrcFile, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        if (hSrcFile == INVALID_HANDLE_VALUE)
        {
            TRACE(_T("CreateFile(%hs) failed (%d)\n"), szDestFile, GetLastError());
            m_error = COPY_HARDDISK_FULL; // hard disk full
            DashEndUsingSoundtracks();
            return;
        }

        HANDLE hDestFile = CreateFileA(szDestFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
        if (hDestFile == INVALID_HANDLE_VALUE)
        {
            CloseHandle(hSrcFile);

            TRACE(_T("CreateFile(%hs) failed\n"), szDestFile);
            m_error = COPY_HARDDISK_FULL; // hard disk full
            DashEndUsingSoundtracks();
            return;
        }

        TRACE(_T("Copying file from %hs to %hs\n"), szSrcFile, szDestFile);

        DWORD dwPos = 0;
        DWORD dwLen = GetFileSize(hSrcFile, NULL);

        // Allocate the clusters for the file now.
        LARGE_INTEGER liDistanceToMove;
        liDistanceToMove.QuadPart = (LONGLONG)dwLen;
        if (!SetFilePointerEx(hDestFile, liDistanceToMove, NULL, FILE_BEGIN) ||
            !SetEndOfFile(hDestFile))
        {
            TRACE(_T("SetEndOfFile(%hs) failed\n"), szDestFile);
            m_error = COPY_HARDDISK_FULL; // hard disk full
        }

        SetFilePointer(hDestFile, 0, NULL, FILE_BEGIN);

        if (!m_error)
        {
            for (;;)
            {
                DWORD dwRead, dwWrite;

                if (!ReadFile(hSrcFile, m_buffer, BLOCK_SIZE, &dwRead, NULL))
                {
                    TRACE(_T("ReadFile(%s) failed (%d)\n"), szSrcFile, GetLastError());
                    m_error = COPY_HARDDISK_FULL; // hard disk full
                    break;
                }

                if (dwRead == 0)
                    break;

                if (!WriteFile(hDestFile, m_buffer, dwRead, &dwWrite, NULL))
                {
                    TRACE(_T("WriteFile(%hs) failed (%d)\n"), szDestFile, GetLastError());
                    m_error = COPY_HARDDISK_FULL; // hard disk full
                    break;
                }

                dwPos += dwRead;
                m_readProgress = (float)dwPos / dwLen;
            }
        }

        if (!m_error)
        {
            FILETIME create, access, write;
            GetFileTime(hSrcFile, &create, &access, &write);
            SetFileTime(hDestFile, &create, &access, &write);
        }

        CloseHandle(hSrcFile);

        if (!CloseHandle(hDestFile))
        {
            TRACE(_T("CloseHandle(%hs) failed\n"), szDestFile);
            m_error = COPY_HARDDISK_FULL; // hard disk full
        }

        m_readProgress = 1.0f;
    }

    TRACE(_T("Adding the file to the soundtrack database...\n"));

    if (!m_error && !DashAddSongToSoundtrack(m_dwDestSoundtrack, dwNewSongID, pCopySong->m_dwSongLength, szName))
    {
        TRACE(_T("DashAddSongToSoundtrack failed!\n"));
        m_error = COPY_TOO_MANY_SONGS; // more than MAX_SONGS tracks
    }

    DashEndUsingSoundtracks();

    if (m_error)
    {
        ::DeleteFile(szDestFile);
    }
    else
    {
        c_rgsoundtrack[nSoundtrack].uSongCount++;
    }
}

void CMusicCollection::SetSongName(int nSoundtrack, int nSong, const TCHAR* szName)
{
    TRACE(_T("SetSongName(%d, %d, %s)\n"), nSoundtrack, nSong, szName);

    if (nSoundtrack < 0 || nSoundtrack >= c_nSoundtrackCount || nSong < 0 || (UINT)nSong >= c_rgsoundtrack[nSoundtrack].uSongCount)
    {
        TRACE(_T("CMusicCollection::SetSongName: soundtrack or song out of range!\n"));
        return;
    }

    if (!DashBeginUsingSoundtracks())
    {
        TRACE(_T("DashBeginUsingSoundtracks failed!\n"));
        return;
    }

    VERIFY(DashEditSongName(c_rgsoundtrack[nSoundtrack].uSoundtrackId, nSong, szName));

    DashEndUsingSoundtracks();
}

void CMusicCollection::SetSoundtrackName(int nSoundtrack, const TCHAR* szName)
{
    TRACE(_T("SetSoundtrackName(%d, %s)\n"), nSoundtrack, szName);

    if (nSoundtrack < 0 || nSoundtrack >= c_nSoundtrackCount)
    {
        TRACE(_T("CMusicCollection::SetSoundtrackName: soundtrack out of range!\n"));
        return;
    }

    if (!DashBeginUsingSoundtracks())
    {
        TRACE(_T("DashBeginUsingSoundtracks failed!\n"));
        return;
    }

    VERIFY(DashEditSoundtrackName(c_rgsoundtrack[nSoundtrack].uSoundtrackId, szName));
    DashEndUsingSoundtracks();
    Update();
}

void CMusicCollection::MoveSongUp(int nSoundtrack, int nSong)
{
    TRACE(_T("MoveSongUp(%d, %d)\n"), nSoundtrack, nSong);

    if (nSoundtrack < 0 || nSoundtrack >= c_nSoundtrackCount || nSong <= 0 || (UINT)nSong >= c_rgsoundtrack[nSoundtrack].uSongCount)
    {
        TRACE(_T("CMusicCollection::MoveSongUp: soundtrack or song out of range!\n"));
        return;
    }

    if (!DashBeginUsingSoundtracks())
    {
        TRACE(_T("DashBeginUsingSoundtracks failed!\n"));
        return;
    }

    VERIFY(DashSwapSongs(c_rgsoundtrack[nSoundtrack].uSoundtrackId, nSong, nSong - 1));

    DashEndUsingSoundtracks();
}

void CMusicCollection::MoveSongDown(int nSoundtrack, int nSong)
{
    TRACE(_T("MoveSongDown(%d, %d)\n"), nSoundtrack, nSong);

    if (nSoundtrack < 0 || nSoundtrack >= c_nSoundtrackCount || nSong < 0 || (UINT)nSong >= c_rgsoundtrack[nSoundtrack].uSongCount - 1)
    {
        TRACE(_T("CMusicCollection::MoveSongDown: soundtrack or song out of range!\n"));
        return;
    }

    if (!DashBeginUsingSoundtracks())
    {
        TRACE(_T("DashBeginUsingSoundtracks failed!\n"));
        return;
    }

    VERIFY(DashSwapSongs(c_rgsoundtrack[nSoundtrack].uSoundtrackId, nSong, nSong + 1));

    DashEndUsingSoundtracks();
}

void CMusicCollection::DeleteSong(int nSoundtrack, int nSong)
{
    if (nSoundtrack < 0 || nSoundtrack >= c_nSoundtrackCount || nSong < 0 || (UINT)nSong >= c_rgsoundtrack[nSoundtrack].uSongCount)
    {
        TRACE(_T("CMusicCollection::DeleteSong: soundtrack or song out of range!\n"));
        return;
    }

    DWORD dwSongID, dwSongLength;
    VERIFY(XGetSoundtrackSongInfo(c_rgsoundtrack[nSoundtrack].uSoundtrackId, nSong, &dwSongID, &dwSongLength, NULL, 0));

    if (!DashBeginUsingSoundtracks())
    {
        TRACE(_T("DashBeginUsingSoundtracks failed!\n"));
        return;
    }

    VERIFY(DashDeleteSongFromSoundtrack(c_rgsoundtrack[nSoundtrack].uSoundtrackId, nSong));

    c_rgsoundtrack[nSoundtrack].uSongCount -= 1;
    c_rgsoundtrack[nSoundtrack].uSoundtrackLength -= dwSongLength;

    DashEndUsingSoundtracks();
}

int FindSoundtrack(const TCHAR* szName)
{
    for (int i = 0; i < c_nSoundtrackCount; i += 1)
    {
        if (_tcsicmp(szName, c_rgsoundtrack[i].szName) == 0)
            return i;
    }

    return -1;
}

CStrObject* CMusicCollection::CreateSoundtrackName(const TCHAR* szBaseName)
{
    TCHAR szBuf [MAX_SOUNDTRACK_NAME];

    for (int n = 1; ; n += 1)
    {
        _stprintf(szBuf, _T("%s %d"), szBaseName, n);
        if (FindSoundtrack(szBuf) < 0)
            return new CStrObject(szBuf);
    }
}

