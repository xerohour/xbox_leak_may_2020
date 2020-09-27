/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    TextScreen.h

Abstract:

     Header for Text Screen and text line classes
Author:

    Dennis Krueger <a-denkru> November 2001

--*/
#ifndef __TextScreen_h_
#define __TextScreen_h_

#include "xtl.h"


#define MAX_LINES 100

#define WHITECOLOR	0xffffffff
#define	REDCOLOR	0xffDC143C
#define TITLECOLOR	0xffffd700
#define LTGREENCOLOR	0xff80ff80
#define DARKGREENCOLOR	0Xff228B22
#define YELLOWCOLOR		0xffFFFF00
#define CORALCOLOR		0xfff08080


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

	 ~CTextScreenLine() {;}  // do nothing destructor
};


class CTextScreenArray
{
public:
	CTextScreenArray();
	~CTextScreenArray();
	int		Add(CTextScreenLine *);
	int		Replace(CTextScreenLine *,int Index);
	CTextScreenLine * GetLine(int Index);	
	int		GetLineCount();
	void	ClearScreen();
		

private:
	
	CTextScreenLine * m_TextLines[MAX_LINES]; // rather than make this array dynamic
								      // I'm just making it a static "reasonable" size
	int		m_LineCount;

};


#endif // _TextScreen_h