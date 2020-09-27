/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    TextScreen.cpp

Abstract:

     Class implementation of general text screen and text line support
Author:

    Dennis Krueger <a-denkru> November 2001

--*/

#include "TextScreen.h"


// support methods for TextScreenLine and TextScreenArray 
CTextScreenLine::CTextScreenLine()
{
	m_sx = 0;
	m_sy = 0;
	m_dwColor = 0;
	m_strText = NULL;
	m_dwFlags = 0;
}



CTextScreenLine::CTextScreenLine(float sx, float sy, DWORD Color, WCHAR * pTextStr, DWORD dwFlags)
{
	m_sx = sx;
	m_sy = sy;
	m_dwColor = Color;
	m_strText = pTextStr;
	m_dwFlags = dwFlags;
};


CTextScreenArray::CTextScreenArray()
{
	m_LineCount = 0;
}


CTextScreenArray::~CTextScreenArray()
{
	CTextScreenLine * pThisLine;
	// delete all lines in the array
	for(int i = 0; i < m_LineCount; i++)
	{
		pThisLine = m_TextLines[i];
		if(pThisLine)
		{
			delete pThisLine;
		}
	}
}



int CTextScreenArray::Add(CTextScreenLine * pThisLine)
{
	if(m_LineCount >= MAX_LINES) return -1;
	m_TextLines[m_LineCount] = pThisLine;
	m_LineCount++;
	return m_LineCount-1;  // return index
}

int CTextScreenArray::Replace(CTextScreenLine * pThisLine, int Index)
{
	if(Index >= MAX_LINES) return -1;
	m_TextLines[Index] = pThisLine;
	return Index;  // return index
}

int CTextScreenArray::GetLineCount()
{
	return m_LineCount;
}

CTextScreenLine *
CTextScreenArray::GetLine(int Index)
{
	if(Index >= m_LineCount) return NULL; // out of range
	return m_TextLines[Index];
}

void
CTextScreenArray::ClearScreen()
{
	CTextScreenLine * pThisLine;
	for(int i = 0; i < m_LineCount; i++)
	{
		if(pThisLine = m_TextLines[i])
		{
			delete pThisLine;
		}
	}
	m_LineCount = 0;
}