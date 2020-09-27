/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Music.h

Abstract:

	Music Playback engine for stress

Author:

	Dan Haffner(danhaff) 27-Apr-2001

Environment:

	Xbox only

Revision History:

	29-Apr-2001 danhaff
		Initial Version

--*/
#pragma once

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "xtl.h"
#include "dmusici.h"
#include "MusicHeap.h"

namespace WFVO {

#define FILES 5000

enum STATE {eWaiting, eTransitioning, eLoading};
//------------------------------------------------------------------------------
//	CMusic:
//------------------------------------------------------------------------------
class CMusic
{
public:
	CMusic(void);
    ~CMusic(void);
    HRESULT Initialize(void);
	HRESULT Update(void);
    
    //Get information.
    HRESULT GetSegmentTitleCurrent(WCHAR *szSongTitle);
    DWORD   GetBeat(void);

    //Set information
    HRESULT NextSegment(void);
    HRESULT SetMasterTranspose(short lMasterTranspose);
    HRESULT SetMasterTempo(float fMasterTempo);
    HRESULT SetMasterVolume(LONG lMasterVolume);
    HRESULT SetReverb(unsigned short nReverb);
    HRESULT GetReverb(DWORD *nReverb);

    //Get Information
    void GetHeapAllocation          (DWORD *pdwHeapAllocation,           DWORD *pdwHeapAllocationPhysical) ;
    void GetHeapAllocationPeak      (DWORD *pdwHeapAllocationPeak,       DWORD *pdwHeapAllocationPeakPhysical) ;
    void GetHeapAllocationBlocks    (DWORD *pdwHeapAllocationBlocks,     DWORD *pdwHeapAllocationBlocksPhysical) ;
    void GetHeapAllocationBlocksPeak(DWORD *pdwHeapAllocationBlocksPeak, DWORD *pdwHeapAllocationBlocksPeakPhysical) ;

	
private:
    IDirectMusicPerformance *m_pPerf;
    IDirectMusicAudioPath *m_pPath;
    IDirectMusicLoader    *m_pLoader;

    IDirectMusicSegment *m_pSegmentCurrent;
    IDirectMusicSegment *m_pSegmentNext;
    IDirectMusicSegment *m_pSegmentGMReset;
    IMusicHeap *m_pHeapNormal;
    IMusicHeap *m_pHeapPhysical;

    
    HRESULT LoadNextSegment(void);
    HRESULT UpdateBeatState(void);
    HRESULT UpdatePlaybackState(void);
    HRESULT TransitionSegments(void);

    //Current State
    LPSTR m_ppszFiles[FILES];
    DWORD m_dwMaxFiles;
    DWORD m_dwCurrentFile;
    CHAR m_szSegmentTitleCurrent[MAX_PATH];
    CHAR m_szSegmentTitleNext   [MAX_PATH];
    STATE m_eState;
    BOOL  m_dwBeat;
    DWORD m_dwStartTime;

    DWORD m_dwReverb;
   
    //Thread stuff    
    HANDLE m_hThreadLoad;    
    HANDLE m_hThreadTransition;    
    HRESULT m_hrThreadProcReturn;

    //Information to queue up and set.
    BOOL m_bNextSegment;
    
    friend DWORD LoadNextSegmentThreadProc(void *pvMusic);
    friend DWORD TransitionSegmentsThreadProc(void *pvMusic);
};

}