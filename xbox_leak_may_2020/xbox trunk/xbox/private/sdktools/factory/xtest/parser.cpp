// parser.cpp 
//

#include "stdafx.h"

_TCHAR CParseWord::GetToken (LPTSTR *cp)
{
//	_tprintf (L"GetWord (%s) %s\n", m_terminals, *cp);

	// Skip leading whitespace
	//
	size_t length = _tcsspn (*cp, _T(" \t"));
	if (length > 0)
		*cp += length;

	// Record the first character before moving the 
	// char pointer forward.
	//
	m_token = *cp;
	if (_tcslen (m_token) == 0) // check for end of line
		return _T('\0');

	// Find the next terminal and move the line pointer
	// to the character after it, but if this is the
	// end of the line, move the pointer to the zero terminator
	//
	length = _tcscspn (m_token, m_terminals);
	*cp += length + ((length < _tcslen (*cp))? 1:0);

	// Record the terminal, zero terminate the token
	// and return the recorded terminal
	//
	_TCHAR terminal = m_token [length];
	m_token [length] = 0;
	return terminal;
}

bool CParseSymbol::GetToken (LPTSTR *cp)
{
	while (**cp == _T(' ')) // skip white space
		(*cp)++;

	if (**cp == m_symbol)
		(*cp)++;
	else
		return false;

	return true;
}

CParseINI::CParseINI (LPTSTR pStringBuffer)
{
	m_lineNumber = 0;
	m_current = pStringBuffer;
}

CParseINI::~CParseINI ()
{
}

LPTSTR CParseINI::GetLine ()
{
	int length, next;

	// Skip leading whitespace
	//
	length = _tcsspn (m_current, _T(" \t"));
	if (length > 0)
		m_current = &m_current [length];
	
	// Return if end of buffer found
	//
	if (*m_current == 0)
		return NULL;

	// The following loop concatenates lines joined by a backslash
	//
	for (;;)
	{
		// First take the length of the line and use the terminator
		// to determine the position of the next line in the buffer.
		//
		length = next = _tcscspn (m_current, _T("\n\r"));
		_TCHAR lineEnding = m_current [length];
		m_current [length] = 0; // terminate the line
		switch (lineEnding)
		{
			case _T('\r'): next++; // cr-lf pair
			case _T('\n'): next++; // line feed only
		}

		// Remove comments by readjusting the length
		//
		length = _tcscspn (m_current, _T(";#"));
		m_current [length] = 0; // terminate again
break;
// The rest of this loop was to concatenate lines.  This has been removed so that
// a backslash can be used in path names.
/*
		int backslash = _tcscspn (m_current, _T("\\"));
		if (backslash >= length)
			break;

		// Join the two lines by shifting the buffer, and removing 
		// everything between the slash and the next new line.
		//
		_tcscpy (&m_current [backslash], &m_current [next]);
		m_lineNumber++;
*/
	}

	// Remove trailing whitespace
	//
	while ((length > 0) && ((m_current [length - 1] == _T(' ')) 
							|| (m_current [length - 1] == _T('\t'))))
	{
		m_current [--length] = 0;
	}

	// Increment line number and return original position
	//
	LPTSTR lp = m_current;
	m_current = &m_current [next];
	m_lineNumber++;
	return lp;
}

bool CParseINI::SeekSection (LPCTSTR section)
{
	CAutoString name (_T("[%s]"), section);

	LPTSTR lp = GetLine ();
	while (lp)
	{
		if (_tcsicmp (lp, name) == 0)
			return true;
		lp = GetLine ();
	}

	return false;
}

#define BLANK_LINE(lp) (*lp == 0)
#define SECTION_END(cp) (*cp == 0 || *cp == _T('['))

// GetNextLine
//
// This function skips blank lines and stops at the
// end of a section.
//
LPTSTR CParseINI::GetNextLine ()
{
	while (!SECTION_END (m_current))
	{
		LPTSTR lp = GetLine ();
		if (!BLANK_LINE(lp))
			return lp;
	}

	return NULL;
}

void CParseINI::ParseSection (LPCTSTR sectionName, CParseLine *parser)
{
	LPTSTR lp = NULL;

	try 
	{
		if (!SeekSection (sectionName))
			g_error.ThrowSectionNotFound (sectionName);
		
		for (lp = GetNextLine (); lp != NULL; lp = GetNextLine ())
			parser->ParseLine (lp);
	}
	catch (CXmtaException x) 
	{
		throw CXmtaException (x.GetErrorCode(), 
			_T("%sOn line %d of the INI file:\n\"%s\"\n"),
			x.GetMessage(), m_lineNumber, lp);
	}
}

