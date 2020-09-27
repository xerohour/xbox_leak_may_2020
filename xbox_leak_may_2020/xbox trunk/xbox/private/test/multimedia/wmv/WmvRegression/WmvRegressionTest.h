/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WmvRegressionTest.h

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 07-Dec-2001

Revision History:

	07-Dec-2001 jeffsul
		Initial Version

--*/

#ifndef _WMVREGRESSIONTEST_H_
#define _WMVREGRESSIONTEST_H_

#include "..\WmvCommon\Test.h"
#include "..\WmvCommon\WMVPlayer.h"

#define WMV_REG_TEST_CRC_ELEMENTS	256

class CWmvRegTest : public CTest
{
public:
	CWmvRegTest();
	~CWmvRegTest();

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
	CWMVPlayer				m_WMVPlayer;		
	LPDIRECT3DTEXTURE8		m_pOverlay[2];				// Overlay textures
    LPDIRECT3DSURFACE8		m_pSurface[2];				// Overlay Surfaces
    DWORD					m_dwCurrent;				// Current surface

	LPDIRECT3DDEVICE8		m_pD3DDevice;				// rendering device
	LPDIRECTSOUND8			m_pDSound;					// sound device	
	HANDLE                  m_hInputDevice;				// input device
	XINPUT_STATE			m_PrevInputState;           // previous input state

	WMVVIDEOINFO			m_wmvVideoInfo;             // Video info
	WAVEFORMATEX			m_wfx;                      // Audio format

	REFERENCE_TIME			m_rtStartTime;				// Reference time at start
	REFERENCE_TIME			m_rtDuration;               // Duration of video
    REFERENCE_TIME			m_rtCurrentTime;			// Reference time of current frame

	CHAR					m_szPath[MAX_PATH];			// file path of video to play
	HANDLE					m_hFiles;					// handle to file search data
	WIN32_FIND_DATA			m_FindData;					// info about the next file to open
	HANDLE					m_hOutputFile;				// file to ouput crc data to
	HANDLE					m_hInputFile;				// file to read correct crc from

	BOOL					m_bReset;

	DWORD					m_dwCRCTable[WMV_REG_TEST_CRC_ELEMENTS];
	DWORD					m_dwChecksums[512];
	INT						m_nChecksumIndex;

	DWORD					ComputeCRC32( LPVOID pvData, UINT uNumBytes );
	BOOL					CompareCRC();
};


#endif //#ifndef _WMVREGRESSIONTEST_H_




