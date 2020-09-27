/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    ScreenTest.h

Abstract:

    Definitions for ScreenTest application

Author:

    Dennis Krueger (a-denkru) July 2001

Revision History:

--*/
#ifndef __SCREENTEST_H_
#define __SCREENTEST_H_



#include <ntos.h>

// disk
#include <ntdddisk.h>
#include <ntddscsi.h>

#include <xtl.h>
#include <xgraphics.h>
#include <stdio.h>
#include <XBApp.h>
#include <XBInput.h>
#include <XBFont.h>
#include <XBUtil.h>
#include <XBSound.h>
#include <XBStopWatch.h>
#include <xboxp.h>
#include <xconfig.h>

#ifndef UNICODE
#define UNICODE
#endif

#ifdef __cplusplus
extern "C" {
#endif
int DebugPrint(char* format, ...);
#ifdef __cplusplus
}
#endif

#define MAX_LINES 100

#define CONSTANT_OBJECT_STRING(s)   { sizeof( s ) - sizeof( OCHAR ), sizeof( s ), s }

class CTextScreenLine
{
public:
	
	 FLOAT m_sx;
	 FLOAT m_sy;
	 DWORD m_dwColor;
	 WCHAR* m_strText;
	 DWORD m_dwFlags;


	 CTextScreenLine();
	 CTextScreenLine(float sx, float sy, DWORD Color, WCHAR * TextStr, DWORD dwFlags=0);

	 ~CTextScreenLine() {;};  // do nothing destructor
};


class CTextScreenArray
{
public:
	CTextScreenArray();
	~CTextScreenArray();
	int Add(CTextScreenLine *);
	int Replace(CTextScreenLine *,int Index);
	CTextScreenLine * GetLine(int Index);	
	int GetLineCount();
		

private:
	
	CTextScreenLine * m_TextLines[MAX_LINES]; // rather than make this array dynamic
								      // I'm just making it a static "reasonable" size
	int m_LineCount;

};

#define AUDIO_TRACK_COUNT 1

class CXBoxUtility : public CXBApplication
{
public:
    
    HRESULT				Initialize();
    HRESULT				Render();
    HRESULT				FrameMove();
	CXBFont				m_Font16;
	XBOX_REFURB_INFO	m_RefurbInfo;
	CTextScreenArray	m_ThisScreen;

	UINT				m_GamePadLineIndex;

	bool				m_fAudioOK;
	bool				m_fPlaying;
	CTextScreenLine *	m_pAudioLine;
	UINT				m_AudioLineIndex;
	UINT				m_iPlayIndex;
	DWORD				m_AudioStartTicks;
	CXBSound			m_LeftSound;
	CXBSound			m_RightSound;
	CXBSound			m_StereoSound;


	CXBoxUtility();
	~CXBoxUtility() {;};

};

#endif //!defined (__SCREENTEST_H_)

