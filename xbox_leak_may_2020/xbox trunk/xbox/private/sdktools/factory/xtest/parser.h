// ini_file.h

#pragma once

class CParseLine
{
public:
	virtual bool ParseLine (LPTSTR line) = 0;
};

class CParseINI
{
	int m_lineNumber;
	LPTSTR m_current;

	LPTSTR GetLine ();

public:
	CParseINI (LPTSTR pStringBuffer);
	~CParseINI ();

	bool Open (LPCTSTR filename);
	bool SeekSection (LPCTSTR sectionname);
	LPTSTR GetNextLine ();
	void ParseSection (LPCTSTR sectionName, CParseLine *parser);
	void Close ();
};

class CParseWord
{
	LPTSTR m_token;
	LPCTSTR m_terminals;

public:
	CParseWord (LPCTSTR terminals):
		m_terminals(terminals), m_token(NULL) {}
	_TCHAR GetToken (LPTSTR *cp);
	LPCTSTR GetText () {return m_token;}
	operator LPCTSTR () {return GetText ();}
};

class CParseSymbol
{
	_TCHAR m_symbol;

public:
	CParseSymbol (_TCHAR symbol):m_symbol(symbol) {}
	bool GetToken (LPTSTR *cp);
};




