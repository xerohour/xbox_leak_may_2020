/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    USBDesc.h

Abstract:

    Definitions for USB Descriptor application

Author:

    Dennis Krueger (a-denkru) November 2001

Revision History:

--*/
#ifndef __USBDESC_H_
#define __USBDESC_H_



#include <ntos.h>


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
#include <usb.h>

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


#endif //!defined (__SCREENTEST_H_)

