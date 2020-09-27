// TxLineHlp.h - IVsTextLines helpers
//-----------------------------------------------------------------
// Microsoft Confidential
// Copyright 1999 Microsoft Corporation.  All Rights Reserved.
//
// Paul Chase Dempsey [paulde]
//-----------------------------------------------------------------

#pragma once
//#include "textmgr.h"


//-----------------------------------------------------------------
// TextLines_GetSize - return a count of WCHARS in the buffer
//
LONG WINAPI TextLines_GetSize (IVsTextLines * pText);


//-----------------------------------------------------------------
// TextLines_GetLineCount - return a count of lines in the buffer
//
LONG WINAPI TextLines_GetLineCount (IVsTextLines * pText);


//-----------------------------------------------------------------
// TextLines_GetLineLength 
//
// return a count of chars in the line, not including EOL
//
LONG WINAPI TextLines_GetLineLength (IVsTextLines * pText, LONG iLine);


//-----------------------------------------------------------------
// TextLines_GetLastLineAndLength
//
// return the last valid line in the buffer and fetch it's length
//
LONG WINAPI TextLines_GetLastLineAndLength (IVsTextLines * pText, LONG *pcch);


//-----------------------------------------------------------------
// TextLines_GetSpanLength
//
// return the length of the span
//
LONG WINAPI TextLines_GetSpanLength (IVsTextLines * pText, const TextSpan * pSpan);


//-----------------------------------------------------------------
// TextLines_GetLine - get a line
//
HRESULT WINAPI TextLines_GetLine (
    IVsTextLines *  pText, 
    LONG            iLine, 
    DWORD           glde,  // GLDE_FLAGS enum in TextMgr.idl
    LINEDATAEX &    ld
    );


//-----------------------------------------------------------------
// TextLines_ReleaseLine - release a line
//
// If GetLine fails, do NOT call ReleaseLine: it is an error to release a 
// line that you didn't get.
//
void WINAPI TextLines_ReleaseLine (IVsTextLines *  pText, LINEDATAEX & ld);


//-----------------------------------------------------------------
// TextLines_Append - append text to the end of the buffer
//
// EOL is always normalized
//
HRESULT WINAPI TextLines_Append (IVsTextLines * pText, LPCOLESTR psz, TextSpan *pts = NULL);

