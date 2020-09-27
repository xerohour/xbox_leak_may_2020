/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	WinDspImageBuilder.h

Abstract:

	Windowed version of CDspImageBuilder

Author:

	Robert Heitkamp (robheit) 08-Jun-2001

Revision History:

	08-Jun-2001 robheit
		Initial Version

--*/
#ifndef _WINDSPIMAGEBUILDER_H_
#define _WINDSPIMAGEBUILDER_H_

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "..\XGPIMAGE\fxdspimg.h"

//------------------------------------------------------------------------------
//	CWinDspImageBuilder
//------------------------------------------------------------------------------
class CWinDspImageBuilder : public CDspImageBuilder  
{
public:

	CWinDspImageBuilder(void);
	virtual ~CWinDspImageBuilder(void);

	void ShowDialog(void);
	void ClearText(void);

	virtual void Print(PCHAR pFormat, ...);

private:

	CString	m_text;
	BOOL	m_showDialog;
};

#endif

