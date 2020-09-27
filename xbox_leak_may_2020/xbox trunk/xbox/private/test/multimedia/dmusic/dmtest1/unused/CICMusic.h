
//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1999 - 1999
//
//  File:       CICMusic.h
//
//--------------------------------------------------------------------------

//===========================================================================
//	CICMusic.h
// Provides initialization and cleanup behavior for basic DirectMusic functionality
// Specifically, an IDirectMusic, IDirectMusicLoader, and IDirectMusicPerformance.
//
//	History:
//		03/09/2000 - danhaff/ruswoods - created
//      04/25/2000 - danhaff - added another Init() to specify whether we
//                             create an APATH_SHARED_ENV before
//                             creating the user-specified path.
//      01/05/2001 - danhaff - Ported to XBox.
//===========================================================================
#include <dmusici.h>

#pragma once 

#define GF_AUDIOPATH        1
#define GF_SEGMENT8         2
#define GF_SEGMENTSTATE8    4




class CICMusic
{

protected:
    IDirectSound                    *m_pDirectSound;
    IDirectMusic                  *m_pDirectMusic;
    IDirectMusicLoader8           *m_pLoader8;
    IDirectMusicPerformance8      *m_pPerformance8;
    IUnknown                      *m_pUnkAudioPathConfig;
    IDirectMusicAudioPath         *m_pAudioPath;
    IDirectMusicAudioPath         *m_pAudioPathEnv;
    IDirectMusicSegment8          *m_pSegment8;
    IDirectMusicSegmentState8     *m_pSegmentState8;
    IDirectMusicSegmentState      *m_pSegmentState;
//    HWND                             m_hWnd;
    BOOL                             m_bCOMInitialized;

public :
    // *************************************************************
    CICMusic(void);

    // *************************************************************
	~CICMusic(void);

    // *************************************************************
	HRESULT Init      (LPCWSTR wszSegment,
                      DWORD dwDefaultPathType,
  //                    DWORD dwUseAPathEnv,
                      DWORD dwPChannelCount = 16);

    // *************************************************************
    HRESULT Init      (LPCWSTR wszSegment,
                      LPCWSTR wszConfig);//,
    //                  DWORD dwUseAPathEnv);

    // *************************************************************
    HRESULT Shutdown(void);

    // *************************************************************
	HRESULT GetInterface(IDirectSound **ppDirectSound);
    
    // *************************************************************
	HRESULT GetInterface(IDirectMusic **pptDirectMusic);

    // *************************************************************
	HRESULT GetInterface(IDirectMusicLoader8 **pptLoader8);

    // *************************************************************
	HRESULT GetInterface(IDirectMusicPerformance8 **pptPerformance8);

    // *************************************************************
	HRESULT GetInterface(IUnknown **pptUnkAudioPathConfig);

    // *************************************************************
	HRESULT GetInterface(IDirectMusicAudioPath **pptAudioPath);

    // *************************************************************
    HRESULT GetInterface(IDirectMusicSegment8** pptSegment8);
    
    // *************************************************************
    HRESULT GetInterface(IDirectMusicSegmentState** pptSegmentState);

    // *************************************************************
    HRESULT GetInterface(IDirectMusicSegmentState8** pptSegmentState8);

    // *************************************************************
    HRESULT GetAudioPathEnv(IDirectMusicAudioPath **pptAudioPathEnv);


private:
    // *************************************************************
    HRESULT PrivateInitialize(LPCWSTR wszSegment,
                                 LPCWSTR wszConfig,
                                 DWORD dwDefaultPathType,
//                                 DWORD dwUseAPathEnv,
                                 DWORD dwPChannelCount
                                 );
    // *************************************************************
    HRESULT PrivateUninitialize(void);



};



