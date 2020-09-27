/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WmvPerfTest.h

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 15-Oct-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	15-Oct-2001 jeffsul
		Initial Version

--*/

#ifndef _WMVPERFTEST_H_
#define _WMVPERFTEST_H_

#define USE_WMVPLAYER

#include "..\WmvCommon\Test.h"

#ifdef USE_WMVPLAYER
#include "..\WmvCommon\WMVPlayer.h"
#else
#include <wmvxmo.h>
#include "..\WmvCommon\FrameQueue.h"
#define WMV_PERF_TEST_NUM_FRAMES	16
#define WMV_PERF_TEST_NUM_PACKETS	48
#define WMV_PERF_TEST_PACKET_SIZE	4096
#endif

//#include "..\WmvCommon\bitfont.h"
#include <xfont.h>

// Vertex definition for the screen-space quad
#define FVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_TEX1)
typedef struct {
    D3DXVECTOR4 p;
    D3DXVECTOR2 t;
} CUSTOMVERTEX;

typedef struct _TVERTEX
{
	FLOAT x, y, z, rhw; // The transformed position for the vertex
	FLOAT u, v;         // Texture coordinates
} TVERTEX;

class CWmvPerfTest : public CTest
{
public:
	CWmvPerfTest();
	~CWmvPerfTest();

protected:
	HRESULT ReadIni();
	HRESULT InitGraphics();
	HRESULT DestroyGraphics();
	HRESULT InitAudio();
	HRESULT DestroyAudio();
	HRESULT InitResources();
	HRESULT DestroyResources();

	HRESULT ProcessInput();
	HRESULT Update();
	HRESULT Render();

private:

#ifdef USE_WMVPLAYER
	CWMVPlayer				m_WMVPlayer;		
	LPDIRECT3DTEXTURE8  m_pOverlay[2];  // Overlay textures
    LPDIRECT3DSURFACE8  m_pSurface[2];  // Overlay Surfaces
    DWORD               m_dwCurrent;    // Current surface
#else //
	LPWMVDECODER			m_pWMVDecoder;				// WMV decoder 
	VIDEOFRAME              m_aVideoFrames[WMV_PERF_TEST_NUM_FRAMES]; // Video frames
	DWORD                   m_adwStatus[WMV_PERF_TEST_NUM_PACKETS];   // Audio packet status
    BYTE *                  m_pbSampleData;             // Audio sample data
	LPDIRECTSOUNDSTREAM     m_pStream;                  // DirectSound Stream
	LPDIRECT3DVERTEXBUFFER8 m_pvbQuad;                  // Quad for rendering video
	CFrameQueue				m_fqFree;					// Free frames - these can be filled
    CFrameQueue				m_fqBusy;					// Busy frames - possibly being rendered
    CFrameQueue				m_fqReady;					// Ready frames - filled, and can be displayed
#endif //


	// resources
	LPDIRECT3DDEVICE8		m_pD3DDevice;				// rendering device
	LPDIRECTSOUND8			m_pDSound;					// sound device	
	WMVVIDEOINFO			m_wmvVideoInfo;             // Video info
	WAVEFORMATEX			m_wfx;                      // Audio format
    VIDEOFRAME *            m_pCurrentFrame;            // Frame to render

	LPDIRECT3DTEXTURE8		m_pd3dtText;				// texture to render text to
	TVERTEX					m_prText[4];				// geometry to display text on
	//BitFont					m_BitFont;					// font interface
	XFONT*					m_pFont;
	HANDLE                  m_hInpDevice;				// input device
	XINPUT_STATE			m_PrevInputState;           // previous input state

	// file io
	CHAR					m_szPath[MAX_PATH];			// file path of video to play
	UINT					m_nNumFilesPlayed;			// number of videos played so far
	HANDLE					m_hFiles;					// handle to file search data
	WIN32_FIND_DATA			m_FindData;					// info about the next file to open

	// test info
	DWORD					m_dwCurrentFrame;			// Current frame number
	DWORD					m_dwVideoFrames;			// Total number of video frames decoded so far
	DWORD					m_dwRenderedFrames;			// Total number of frames rendered
	DWORD					m_dwRenderedFPS;			// Number of frames rendered since last sample
	DWORD					m_dwVideoFPS;				// Number of video frames decoded since last sample
	DWORD					m_dwDroppedFrames;			// Number of dropped frames during this video
	DWORD					m_dwLastDroppedFrame;		// Frame number of last dropped frame

	// test state
	WCHAR					m_szWideFileName[MAX_PATH]; // the wide character file name of the current file
	BOOL					m_bDecode;                  // Set to FALSE at end of video
	BOOL					m_bReset;					// set to TRUE at end of video
	BOOL					m_bGotData;					// TRUE after we've gotten a packet
	BOOL					m_bDrawText;				// whether or not to display text info	
	BOOL					m_bFullScreen;				// whether to display the movie full screen or original size
	DWORD					m_dwLastFPS;				// Last time we sampled the FPS

	REFERENCE_TIME			m_rtStartTime;				// Reference time at start
	REFERENCE_TIME			m_rtDuration;               // Duration of video
    REFERENCE_TIME			m_rtCurrentTime;			// Reference time of current frame
    REFERENCE_TIME			m_rtPreroll;				// Preroll time
    REFERENCE_TIME			m_rtSeekAdjustment;			// Seek adjustment
	DWORD					m_dwRefreshInterval;

	// private member functions
#ifndef USE_WMVPLAYER
	BOOL FindFreePacket( DWORD * pdwIndex );			// Find a free audio packet
    BOOL FindFreeFrame( VIDEOFRAME ** ppFrame );		// Find a free video frame
    HRESULT ProcessVideoQueues();						// Processes our video queues
#endif
	VOID	DumpPerfData();
};	

#endif //#ifndef _WMVPERFTEST_H_
