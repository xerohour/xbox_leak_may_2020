/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Helpers.cpp

Abstract:

	Helper functions for logging, etc

Author:

	Daniel Haffner (danhaff) 04/29/01

Revision History:
--*/


#pragma once

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "xtl.h"
#include <xgmath.h>
#include <dmusici.h>
#include "stdio.h"

namespace WFVO {

//TESTPARAMS STUFF
struct TESTPARAMS
{
    
    DWORD dwSeed;
    DWORD dwBallDetail;
    DWORD dwWallDetail;
    DWORD dwLightDetail;
    DWORD dwNumLayers;	
    DWORD dwNumBalls;	
    DWORD dwMaxBalls;
    DWORD dwNumLights;
    DWORD dwMaxLights;
    FLOAT fRadius;
    FLOAT fMaxSpeed;
    FLOAT fMinSpeed;
    BOOL  bTextureBalls;
    CHAR  szBallBaseTex[MAX_PATH];
    CHAR  szBallBumpTex[MAX_PATH];
    CHAR  szSphereBaseTex[MAX_PATH];
    CHAR  szLightVSH[MAX_PATH];
    CHAR  szSparkVSH[MAX_PATH];
    BOOL  bDMusic;
    BOOL  bDMDoWorkLocally;
    BOOL  bDMUseGM;
    DWORD dwDMVoiceCount;
    CHAR  szDMStressDir [MAX_PATH];
    CHAR  szDMSupportDir[MAX_PATH];
    CHAR  szDMAudioPath[MAX_PATH];
    CHAR  szDMHeap[MAX_PATH];
    DWORD dwDMFixedHeapSizeNorm;
    DWORD dwDMFixedHeapSizePhys;
    LPSTR *ppDMSegments;
    DWORD dwDMSegments;
    BOOL  bDMLoaderThread;
	BOOL  bDrawWireframe;
	BOOL  bRelativeSpeed;
	BOOL  bD3DPerf;
    DWORD dwMaxSparks;
    BOOL  bSparksAreLights;
    DWORD dwSparksPerCollision;
	DWORD dwSparkMaxAge;
    DWORD dwFileIOSize;
    LONG  lDMMasterVolume;
    float fDMMasterTempo;
    short lDMMasterTranspose;
    DWORD dwDMDebugLevel;
    DWORD dwDMRIPLevel;
    DWORD dwDMSelected;

    CHAR  szDSScratchImage[MAX_PATH];
    CHAR  szDSHRTF[MAX_PATH];

    CHAR szDSEnvironment[MAX_PATH];
    BOOL b3DSound;
    CHAR szLightSound[MAX_PATH];
    CHAR szBounceSound[MAX_PATH];
    BOOL bSubMixBallSound;
    BOOL bLightSound;
	LONG lDSVolume;
	DWORD dwDSFreq;
    LONG lAmbientVolume;
    BOOL bCopyMediaFromNetwork;

    DWORD dwMultiSampleType;
    DWORD dwBackBufferCount;
    BOOL bCycleDisplayModes;
    DWORD dwCycleTime;

    //Stuff for new DSound thread.
    DWORD dwDSSThreads;
    DWORD dwDSSTotalBuffers2D;
    DWORD dwDSSTotalBuffers3D;
    DWORD dwDSSTotalStreams2D;
    DWORD dwDSSTotalStreams3D;
    DWORD dwDSSMixinBuffers2D;
    DWORD dwDSSMixinBuffers3D;
    DWORD dwDSSSends2D;
    DWORD dwDSSSends3D;
    DWORD dwDSSWait;
};


extern TESTPARAMS g_TestParams;
HRESULT LoadTestParams(void);
HRESULT FreeTestParams(void);
HRESULT SanityCheckTestParams(void);
HRESULT SanityCheckFiles(void);
HRESULT DownloadScratch(IDirectSound *pDSound, PCHAR pszScratchFile);


//OTHER STUFF
HRESULT Log(LPSTR szFormat, ...);
typedef HRESULT (*LOADPROC)(LPSTR szFullPath, LPVOID pVoid);
HRESULT RecurseDirectory(LPSTR szDir, LOADPROC LoadFile, LPVOID pVoid);
HRESULT ChopPath(const char *szFullString, LPSTR szPathOut, LPSTR szFileOut);
HRESULT TranslatePath(const char *szFullString, LPSTR szRemotePath, LPSTR szLocalDir );
void WaitForThreadEnd(HANDLE *phThread);


//DMusic segment playback stuff
HRESULT WaitForSegmentStop (IN IDirectMusicPerformance8* ptPerf8, IN IDirectMusicSegment8* ptSegment8, IN IDirectMusicSegmentState8* ptSegmentState8,IN DWORD dwTimeout);
HRESULT WaitForSegmentStart(IN IDirectMusicPerformance8* ptPerf8, IN IDirectMusicSegment8* ptSegment8, IN IDirectMusicSegmentState8* ptSegmentState8,IN DWORD dwTimeout);
HRESULT ExpectSegmentStart(IDirectMusicPerformance8 *ptPerf8, IDirectMusicSegment8 *ptSegment8, IDirectMusicSegmentState8 *ptSegmentState8, DWORD dwTimeout, LPSTR szSegname);
HRESULT ExpectSegmentStop (IDirectMusicPerformance8 *ptPerf8, IDirectMusicSegment8 *ptSegment8, IDirectMusicSegmentState8 *ptSegmentState8, DWORD dwTimeout, LPSTR szSegname);
void OptionalDoWork(void);




inline DWORD F2DW(float f) { return *((LPDWORD)&f); }

inline D3DCOLOR VectorToColor(XGVECTOR3* pv) {
    
    return ((BYTE)((pv->x + 1.0f) / 2.0f * 255.0f) << 16) |
           ((BYTE)((pv->y + 1.0f) / 2.0f * 255.0f) << 8)  |
            (BYTE)((pv->z + 1.0f) / 2.0f * 255.0f);
}

D3DCOLOR ModulateColors(D3DCOLORVALUE* pdcv1, D3DCOLORVALUE* pdcv2);

struct AUDIOPATHPAIR
{
    LPSTR szString;
    DWORD dwValue;
};

extern AUDIOPATHPAIR AudioPathPairs[4];
extern HANDLE g_hLogMedia;



}

