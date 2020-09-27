#ifndef _DMCONSOLE_H_
#define _DMCONSOLE_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "alist.h"

// NEW - USED FOR FFT
#include "dmusicip.h"

#define MAX_FREQUENCY_BINS	16384
#define M_PI	3.14159265358979323846
#define PEAK_NUM_BINS	8
#define HISTORY_BUFFER_SIZE	8
// --END NEW

extern const char *g_szCmd;

//-----------------------------------------------------------------------------
// Global DirectMusic variables
//-----------------------------------------------------------------------------
extern IDirectMusicLoader8*      g_pLoader;
extern IDirectMusicPerformance8* g_pPerformance;



//-----------------------------------------------------------------------------
// Name: class CSegmentItem
// Desc: Class for hosting a segment in a list item.
//-----------------------------------------------------------------------------
class CSegmentItem : public AListItem
{
public:
	CSegmentItem() : AListItem(), m_pSegment8(NULL), m_pSegmentState8(NULL), m_dwSegmentID(0xFFFFFFFF) {}

    IDirectMusicSegment8		*m_pSegment8;
    IDirectMusicSegmentState8   *m_pSegmentState8;
    DWORD                        m_dwSegmentID;

    CSegmentItem *GetNext() const {return (CSegmentItem *)m_pNext;};
};


//-----------------------------------------------------------------------------
// Name: class CAudiopathItem
// Desc: Class for hosting an audiopath in a list item.
//-----------------------------------------------------------------------------
class CAudiopathItem : public AListItem
{
public:
	CAudiopathItem() : AListItem(), m_pAudiopath8(NULL), m_dwAudiopathID(0xFFFFFFFF) {}

    IDirectMusicAudioPath8		*m_pAudiopath8;
    DWORD                        m_dwAudiopathID;

    CAudiopathItem *GetNext() const {return (CAudiopathItem *)m_pNext;};
};


//-----------------------------------------------------------------------------
// Name: class CScriptItem
// Desc: Class for hosting a script in a list item.
//-----------------------------------------------------------------------------
class CScriptItem : public AListItem
{
public:
	CScriptItem() : AListItem(), m_pScript8(NULL), m_dwScriptID(0xFFFFFFFF) {}

    IDirectMusicScript8			*m_pScript8;
    DWORD                        m_dwScriptID;

    CScriptItem *GetNext() const {return (CScriptItem *)m_pNext;};
};


//-----------------------------------------------------------------------------
// Name: class CDownloadItem
// Desc: Class for hosting a download buffer in a list item.
//-----------------------------------------------------------------------------
class CDownloadItem : public AListItem
{
public:
	CDownloadItem()
    {
        m_hHandle = NULL;
        m_pbBuffer = NULL;
    }

//    IDirectMusicDownload		*m_pIDirectMusicDownload;
//    DWORD                        m_dwXboxDownloadID;
//	DWORD						 m_dwPCDownloadID;
	DWORD						 m_dwUniqueID;
    HANDLE                      m_hHandle;
    BYTE *                      m_pbBuffer;
    CDownloadItem *GetNext() const {return (CDownloadItem *)m_pNext;};
};

//-----------------------------------------------------------------------------
// Name: class CConnectionItem
// Desc: Class for hosting a connection application.
//-----------------------------------------------------------------------------
class CConnectionItem : public AListItem
{
public:
    CHAR m_szName[64];

	CConnectionItem()
    {
        m_szName[0] = 0;
    }

    CConnectionItem *GetNext() const {return (CConnectionItem *)m_pNext;};
};


//-----------------------------------------------------------------------------
// Name: class CXBoxSample
// Desc: Application class.
//-----------------------------------------------------------------------------
class CXBoxSample : public CXBApplication
{
    // Font
    CXBFont m_FontBig;      // Large font object
	CXBFont m_FontSmall;	// small font object
    CXBHelp m_Help;         // Help object
    BOOL    m_bDrawHelp;    // Should we draw help?
//	static CRITICAL_SECTION	m_criticalSection;

public:
    FLOAT   m_fGrooveLevel;
    FLOAT   m_fVolume;
	FLOAT	m_fMIDICounter;

	BOOL	m_bSynthInitialized;
	BOOL	m_bDisplayRMS;
    DWORD   m_dwEffectIndex;

	FLOAT	m_fLastMIDICounterValue;
	DWORD	m_dwMIDIIndicatorOnTime;

	LPDSEFFECTIMAGEDESC m_pEffectsImageDesc;

	FLOAT				m_fMaxLevels[PEAK_NUM_BINS];
	FLOAT				m_fLevelsHistoryBuffer[PEAK_NUM_BINS][HISTORY_BUFFER_SIZE];
	FLOAT				m_fHistoryBufferAverage[PEAK_NUM_BINS];
	FLOAT				m_fDisplayLevel[PEAK_NUM_BINS];
	DWORD				m_dwHistoryBufferIndex;

	AList	m_lstSegments;
	AList	m_lstAudiopaths;
	AList	m_lstScripts;
	AList	m_lstDownloads;
	AList	m_lstConnections;

    LPDIRECTSOUND8    m_pDSound;      // DSound object

    HRESULT Initialize();
    HRESULT FrameMove();
    HRESULT Render();
    void    HandleSegmentNotification( DMUS_NOTIFICATION_PMSG *pNotificationMsg );

    HRESULT DownloadScratch(PCHAR pszScratchFile);  // downloads a default DSP image to the GP

	// NEW - Methods for LED display for volume
	HRESULT UpdatePeakLevels();
	HRESULT RenderPeakLevels();

    CXBoxSample();

    void AddConnection(LPCSTR pszConnection);
    void RemoveConnection(LPCSTR pszConnection);
};



#endif // _DMCONSOLE_H_
