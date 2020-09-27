/*****************************************************************************\
*
* Module: mstream.cpp
*
* Message Stream support.
*
* History:
*   03/30/93 Byron Dazey	Created.
*
\*****************************************************************************/

#include "stdafx.h"
#pragma hdrstop

#include "msglog.h"
#include "msgview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

mstream& mstream::operator<<(WORD w)
{
	//
	// Save the original flags.
	//
	long flagsOld = this->flags();

	//
	// Print with leading zeros, width of 4, uppercase and hex.
	//
	this->width(4);
	this->setf(ios::hex);

	//
	// Print the word value in the standard way.
	//
	ostream::operator<<(w);

	//
	// Restore the state.
	//
	this->flags(flagsOld);

	return *this;
}


mstream& mstream::operator<<(DWORD dw)
{
	//
	// Save the original flags.
	//
	long flagsOld = this->flags();

	//
	// Print with leading zeros, width of 8, uppercase and hex.
	//
	this->width(8);
	this->setf(ios::hex);

	//
	// Print the dword value in the standard way.
	//
	ostream::operator<<(dw);

	//
	// Restore the state.
	//
	this->flags(flagsOld);

	return *this;
}


mstream& mstream::operator<<(HWND hwnd)
{
	//
	// Is the hwnd not null?
	//
	if (hwnd)
	{
		//
		// Print the hwnd as a DWORD.
		//
		*this << (DWORD)hwnd;
	}
	else
	{
		//
		// Just print out a string describing the hwnd as NULL.
		//
		*this << ids(IDS_NULL);
	}

	return *this;
}



void CMsgStream::EndLine()
{
	//
	// Don't add a blank line...
	//
	ASSERT(pcount());

	//
	// You must call SetMSDPointer before doing an "EndLine"!
	//
	ASSERT(GetMSDPointer());

	//
	// Null terminate the string in the buffer and
	// get a pointer to it.
	//
	*this << ends;
	LPSTR psz = str();

	m_pMsgView->AddLine(psz, GetMSDPointer());

	//
	// Reset the string pointer to the beginning of the buffer.
	//
	rdbuf()->freeze(0);
	seekp(0);
}



void CMsgParmStream::EndLine()
{
	CHAR szBuf[260];

	//
	// Don't add a blank line...
	//
	ASSERT(pcount());

	//
	// You must call SetOutputLB before doing an "EndLine"!
	//
	ASSERT(m_pListBox);

	//
	// Null terminate the string in the buffer and
	// get a pointer to it.
	//
	*this << ends;
	LPSTR psz = str();

	if (m_nIndent)
	{
		szBuf[0] = 0;
		for (int i = 0; i < m_nIndent; i++)
		{
			_tcscat(szBuf, "    ");
		}

		_tcscat(szBuf, psz);
		psz = szBuf;
	}

	m_pListBox->InsertString(-1, psz);

	//
	// Calculate the width of the string and update the max width member.
	//
	CDC* pDC = m_pListBox->GetDC();
	CFont* pFontOld = pDC->SelectObject(m_pListBox->GetFont());
	CSize Size = pDC->GetTextExtent(psz, strlen(psz));
	pDC->SelectObject(pFontOld);
	m_pListBox->ReleaseDC(pDC);

	if (Size.cx > m_cxMaxWidth)
	{
		m_cxMaxWidth = Size.cx;
	}

	//
	// Reset the string pointer to the beginning of the buffer.
	//
	rdbuf()->freeze(0);
	seekp(0);
}
