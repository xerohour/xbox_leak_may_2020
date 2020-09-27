
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
//===========================================================================

#pragma once

#define GF_AUDIOPATH        1
#define GF_SEGMENT8         2
#define GF_SEGMENTSTATE8    4

#include "dmth.h"


class CICMusic
{

protected:
    CtIDirectMusicLoader8           *m_ptLoader8;
    CtIDirectMusicPerformance8      *m_ptPerformance8;
    CtIUnknown                      *m_ptUnkAudioPathConfig;
    CtIDirectMusicAudioPath         *m_ptAudioPath;
    CtIDirectMusicAudioPath         *m_ptAudioPathEnv;
    CtIDirectMusicSegment8          *m_ptSegment8;
    CtIDirectMusicSegmentState8     *m_ptSegmentState8;

public :
    // *************************************************************
    CICMusic(void);

    // *************************************************************
	~CICMusic(void);

    // *************************************************************
	HRESULT Init      (CtIDirectMusicPerformance8 *ptPerf8,
                       LPCSTR szSegment,
                      DWORD dwDefaultPathType,
                      DWORD dwPChannelCount = 128);

    // *************************************************************
    HRESULT Init      (CtIDirectMusicPerformance8 *ptPerf8,
                       LPCSTR szSegment,
                      LPCSTR szConfig);

    // *************************************************************
    HRESULT Shutdown(void);

    // *************************************************************
	HRESULT GetInterface(CtIDirectMusicLoader8 **pptLoader8);

    // *************************************************************
	HRESULT GetInterface(CtIDirectMusicPerformance8 **pptPerformance8);

    // *************************************************************
	HRESULT GetInterface(CtIUnknown **pptUnkAudioPathConfig);

    // *************************************************************
	HRESULT GetInterface(CtIDirectMusicAudioPath **pptAudioPath);

    // *************************************************************
    HRESULT GetInterface(CtIDirectMusicSegment8** pptSegment8);
    
    // *************************************************************
    HRESULT GetInterface(CtIDirectMusicSegmentState8** pptSegmentState8);

    // *************************************************************
    HRESULT GetAudioPathEnv(CtIDirectMusicAudioPath **pptAudioPathEnv);


private:
    // *************************************************************
    HRESULT PrivateInitialize(LPCSTR wszSegment,
                                 LPCSTR wszConfig,
                                 DWORD dwDefaultPathType,
                                 DWORD dwPChannelCount
                                 );
    // *************************************************************
    HRESULT PrivateUninitialize(void);



};



