/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WmvVerificationTest.h

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 26-Sep-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	26-Sep-2001 jeffsul
		Initial Version

--*/

#ifndef _WMVVERIFICATION_H_
#define _WMVVERIFICATION_H_

#include "..\WmvCommon\Test.h"

#include <wmvxmo.h>

#include "..\WmvCommon\FrameQueue.h"
#include "..\WmvCommon\bitfont.h"

#define WMV_VER_TEST_NUM_FRAMES		16
#define WMV_VER_TEST_NUM_PACKETS	48
#define WMV_VER_TEST_PACKET_SIZE	4096

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

/*typedef enum {
	WMVTS_DECODE,
	WMVTS_COMPARE
} WMVTESTSTATE;*/

class CWmvVerificationTest : public CTest
{
public:
	CWmvVerificationTest();
	~CWmvVerificationTest();

protected:
	HRESULT ReadIni();
	HRESULT InitGraphics();
	HRESULT DestroyGraphics();
	HRESULT InitAudio();
	HRESULT DestroyAudio();
	HRESULT InitResources();
	HRESULT DestroyResources();

	HRESULT Update();
	HRESULT Render();

private:
	// resources
	LPDIRECT3DDEVICE8		m_pD3DDevice;				// rendering device
	LPDIRECTSOUND8			m_pDSound;					// sound device
	LPWMVDECODER			m_pWMVDecoder;				// WMV decoder 
	XMEDIAPACKET			m_xmpAudio, m_xmpVideo;		// decoded media packets
	DWORD					m_audStat, m_vidStat;		// decoded media status
	DWORD					m_audSize, m_vidSize;		// decoded media buffer size
	REFERENCE_TIME			m_rtAudio, m_rtVideo;		// decoded media timestamp
	LPDIRECT3DTEXTURE8		m_pd3dtText;				// texture to render text to
	TVERTEX					m_prText[4];				// geometry to display text on
	BitFont					m_BitFont;					// font interface

	// file io
	CHAR					m_szPath[MAX_PATH];			// file path of video to play
	WCHAR					m_szWideFileName[MAX_PATH]; // the wide character file name of the current file
	CHAR					m_szRawPath[MAX_PATH];		// file path to output .raw file
	CHAR					m_szPcmPath[MAX_PATH];		// file path to output .pcm file
	HANDLE					m_hRawFile;					// file to dump raw video frames to
	HANDLE					m_hPcmFile;					// file to dump raw audio stream to
	HANDLE					m_hFiles;					// handle to file search data
	WIN32_FIND_DATA			m_FindData;					// info about the next file to open

	// test state
	DWORD					m_dwVideoFrames;			// Total number of frames
	DWORD					m_dwAudioFrames;			// Total number of audio packets
	DWORD					m_dwBitsPerSample;			// Number of bits per audio sample
	BOOL					m_bReset;					// set to TRUE at end of video
	DWORD					m_dwLastFPS;				// Last time we sampled the FPS

	DWORD					m_dwMaxDiffAudio;
	DWORD					m_dwMaxDiffAudioFrame;
	DOUBLE					m_fMaxStdDevAudio;
	DWORD					m_dwMaxStdDevAudioFrame;

	DWORD					m_dwMaxDiffVideo;
	DWORD					m_dwMaxDiffVideoFrame;
	DOUBLE					m_fMaxStdDevVideo;
	DWORD					m_dwMaxStdDevVideoFrame;

	// private member functions
	HRESULT OutputRawData( IN LPCXMEDIAPACKET pVideoPacket );
	HRESULT OutputPcmData( IN LPCXMEDIAPACKET pAudioPacket );
	VOID	CompareFrames();
	VOID	CompareAudio();
};	

#endif //#ifndef _WMVVERIFICATION_H_
