#include "std.h"
#include "xapp.h"
#include "Parser.h"


CParser::CParser(const TCHAR* szFilePath, const TCHAR* pchFile, int cchFile/*=-1*/)
{
	m_pchFile = pchFile;

	if (cchFile == -1)
		m_cchFile = _tcslen(pchFile);
	else
		m_cchFile = cchFile;

	m_szFilePath = szFilePath;

	m_pch = m_pchFile;
	m_nLine = 1;
}

CParser::~CParser()
{
}

void CParser::SkipWhite()
{
	m_pch = ::SkipWhite(m_pch);
}

bool CParser::Token(const TCHAR*& pchToken, int& cchToken, bool bAllowPaths/*=false*/)
{
	m_pch = ::Token(m_pch, pchToken, cchToken, bAllowPaths);
	return cchToken != 0;
}

void CParser::SyntaxError(const TCHAR* szMsg, ...)
{
	va_list args;
	va_start(args, szMsg);

	TCHAR szBuffer [512];
	_vsntprintf(szBuffer, countof (szBuffer), szMsg, args);

	TCHAR szMessage [1024];
	_stprintf(szMessage, _T("Syntax Error\n\nFile: %s\nLine: %d\n\n%s"), m_szFilePath, m_nLine, szBuffer);

#ifdef _WINDOWS
	XAppMessageBox(szMessage);
#else
	Trace(_T("\007%s\n"), szMessage);
#endif

	va_end(args);
}


////////////////////////////////////////////////////////////////////////////


// REVIEW: This crap should be in CLexer

#define IsAlNum(ch) (((ch) >= 'a' && (ch) <= 'z') || ((ch) >= 'A' && (ch) <= 'Z') || ((ch) >= '0' && (ch) <= '9') || (ch) == '.' || (ch) == '-' || (ch) == '_')

int g_nLine;
const TCHAR* g_szFileName;
static TCHAR g_szFileNameBuf [MAX_PATH];
bool g_bParseError;

#if 1
#define TRACE_LINE(pch)
#else
void TraceLine(const TCHAR* pch)
{
	TRACE(_T("file: \"%s\" line %d\n"), g_szFileName, g_nLine);

	TCHAR szLine [256];
	_tcsncpy(szLine, pch, 255);
	szLine[255] = '\0';
	TCHAR* p = _tcschr(szLine, '\r');
	if (p != NULL)
		*p = '\0';
	TRACE(_T("\002    %s\n"), szLine);
}
#define TRACE_LINE(pch) TraceLine(pch)
#endif


void StartParse(const TCHAR* pch, const TCHAR* szFileName, int nLine/*=1*/)
{
	g_szFileName = szFileName;
	g_nLine = nLine;
	g_bParseError = false;

	TRACE_LINE(pch);
}

void EndParse()
{
	g_szFileName = NULL;
	g_nLine = 0;
}

void SyntaxError(const TCHAR* szMsg, ...)
{
	va_list args;
	va_start(args, szMsg);

	TCHAR szBuffer [512];
	_vsntprintf(szBuffer, countof (szBuffer), szMsg, args);

	TCHAR szMessage [1024];
	_stprintf(szMessage, _T("Syntax Error\n\nFile: %s\nLine: %d\n\n%s"), g_szFileName, g_nLine, szBuffer);

#ifdef _WINDOWS
	XAppMessageBox(szMessage);
#else
	Trace(_T("\007%s\n"), szMessage);
#endif

	va_end(args);

	g_bParseError = true;
}

const TCHAR* SkipWhite(const TCHAR* pch, COMMENT_TYPE ct)
{
	for (;;)
	{
		while (*pch != '\0' && *pch <= ' ')
		{
			if (*pch == '\n')
			{
				g_nLine += 1;
				TRACE_LINE(pch + 1);
			}

			pch += 1;
		}

		if (((ct & pound) && *pch == '#') || ((ct & slashslash) && *pch == '/' && *(pch + 1) == '/'))
		{
			if (*pch == '#')
			{
				pch += 1;
				while (*pch == ' ' || *pch == '\t')
					pch += 1;

				if (_tcsncmp(pch, _T("line"), 4) == 0)
				{
					pch += 4;
					while (*pch == ' ' || *pch == '\t')
						pch += 1;
					g_nLine = _ttoi(pch) - 1; // -1 since we will add one at eol...
				}
				else if (_tcsncmp(pch, _T("file"), 4) == 0)
				{
					pch += 4;
					while (*pch == ' ' || *pch == '\t')
						pch += 1;

					TCHAR* pchDest = g_szFileNameBuf;
					if (*pch == '"')
						pch += 1;
					while (*pch != '\0' && *pch != '\r' && *pch != '\n' && *pch != '"')
						*pchDest++ = *pch++;
					*pchDest = 0;

					g_szFileName = g_szFileNameBuf;
				}
			}

			while (*pch != '\0' && *pch != '\r' && *pch != '\n')
				pch += 1;
		}
		else if ((ct & slashstar) && *pch == '/' && *(pch + 1) == '*')
		{
			pch += 2;
			while (*pch != '\0' && !(*pch == '*' && *(pch + 1) == '/'))
			{
				if (*pch == '\n')
				{
					g_nLine += 1;
					TRACE_LINE(pch + 1);
				}

				pch += 1;
			}

			if (*pch != '\0')
				pch += 2;
		}
		else
		{
			return pch;
		}
	}
}

const TCHAR* Token(const TCHAR* pch, const TCHAR*& pchToken, int& cchToken, bool bAllowPaths/*=false*/, COMMENT_TYPE ct/*=any*/)
{
	pch = SkipWhite(pch, ct);

	BOOL bQuoted = FALSE;
	TCHAR chQuote = 0;
	if (*pch == '"' || *pch == '\'')
	{
		bQuoted = TRUE;
		chQuote = *pch;
		pch += 1;
	}

	pchToken = pch;

	if (bQuoted)
	{
		while (*pch != '\0' && *pch != chQuote)
		{
			if (*pch == '\\' && *(pch + 1) != '\0')
				pch += 1;
			pch += 1;
		}
	}
	else if (bAllowPaths)
	{
		while (*pch != '\0' && *pch > ' ')
			pch += 1;
	}
	else if ((*pch >= 'a' && *pch <= 'z' || *pch >= 'A' && *pch <= 'Z' || *pch == '_'))
	{
		while (*pch >= 'a' && *pch <= 'z' || *pch >= 'A' && *pch <= 'Z' || *pch >= '0' && *pch <= '9' || *pch == '_')
			pch += 1;
	}
	else
	{
		BOOL bAlNum = IsAlNum(*pch);
		if (bAlNum)
		{
			while (IsAlNum(*pch))
				pch += 1;
		}
		else
		{
			while (*pch > ' ' && !IsAlNum(*pch))
				pch += 1;
		}
	}

	cchToken = (int)(pch - pchToken);

	if (bQuoted && *pch == chQuote)
		pch += 1;

	return pch;
}


const TCHAR* ParseBoolean(const TCHAR* pch, bool& b)
{
	const TCHAR* pchToken;
	int cchToken;
	pch = Token(pch, pchToken, cchToken);

	if (_tcsnicmp(pchToken, _T("true"), cchToken) == 0)
		b = true;
	else if (_tcsnicmp(pchToken, _T("false"), cchToken) == 0)
		b = false;
	else
		SyntaxError(_T("Invalid boolean value"));

	return pch;
}

const TCHAR* ParseInteger(const TCHAR* pch, int& i)
{
	const TCHAR* pchToken;
	int cchToken;
	pch = Token(pch, pchToken, cchToken);

	TCHAR szBuf [64];
	ASSERT(cchToken < countof(szBuf));
	_tcsncpy(szBuf, pchToken, cchToken);
	szBuf[cchToken] = 0;

	i = _ttoi(szBuf);

	return pch;
}

const TCHAR* ParseNumber(const TCHAR* pch, float& n)
{
	const TCHAR* pchToken;
	int cchToken;
	pch = Token(pch, pchToken, cchToken);

	TCHAR szBuf [64];
	ASSERT(cchToken < countof(szBuf));
	_tcsncpy(szBuf, pchToken, cchToken);
	szBuf[cchToken] = 0;

	n = (float)_tcstod(szBuf, NULL);

	return pch;
}

const TCHAR* ParseString(const TCHAR* pch, TCHAR*& sz)
{
	const TCHAR* pchToken;
	int cchToken;
	pch = Token(pch, pchToken, cchToken);
	
	sz = new TCHAR [cchToken + 1];
	CopyChars(sz, pchToken, cchToken);
	sz[cchToken] = '\0';

	return pch;
}

const TCHAR* ParseVec3(const TCHAR* pch, float v[3])
{
	pch = ParseNumber(pch, v[0]);
	pch = ParseNumber(pch, v[1]);
	pch = ParseNumber(pch, v[2]);

	return pch;
}

const TCHAR* ParseVec4(const TCHAR* pch, float v[4])
{
	pch = ParseNumber(pch, v[0]);
	pch = ParseNumber(pch, v[1]);
	pch = ParseNumber(pch, v[2]);
	pch = ParseNumber(pch, v[3]);

	return pch;
}

const TCHAR* ParseIntArray(const TCHAR* pch, BYTE*& pbArray, int& cbArray)
{
	pch = SkipWhite(pch);

	if (*pch != '[')
	{
		SyntaxError(_T("Expected '[' to start array"));
		return _T("");
	}

	pch += 1;

	const TCHAR* pchStart = pch;
	int nLineStart = g_nLine;
	int nValues = 0;

	for (;;)
	{
		pch = SkipWhite(pch);

		if (*pch == ']')
			break;

		int n;
		pch = ParseInteger(pch, n);
		nValues += 1;

		pch = SkipWhite(pch);

		if (*pch == ']')
			break;

/*
		if (*pch != ',')
		{
			SyntaxError(_T("Expected ']' or ','"));
			return _T("");
		}

		pch += 1;
*/
		if (*pch == ',')
			pch += 1;
	}

	cbArray = nValues * sizeof (int);
	pbArray = new BYTE [cbArray];

	BYTE* pb = pbArray;

	pch = pchStart;
	g_nLine = nLineStart;
	for (;;)
	{
		pch = SkipWhite(pch);

		if (*pch == ']')
			break;

		int n;
		pch = ParseInteger(pch, n);
		*(int*)pb = n;
		pb += sizeof (int);

		pch = SkipWhite(pch);

		if (*pch == ']')
			break;

/*
		if (*pch != ',')
		{
			SyntaxError(_T("Expected ']' or ','"));
			return _T("");
		}

		pch += 1;
*/
		if (*pch == ',')
			pch += 1;
	}

	return pch + 1;
}

const TCHAR* ParseVecArray(const TCHAR* pch, BYTE*& pbArray, int& cbArray, int nVecSize)
{
	ASSERT(nVecSize > 0 && nVecSize <= 4);

	pch = SkipWhite(pch);

	if (*pch != '[')
	{
		SyntaxError(_T("Expected '[' to start array"));
		return _T("");
	}

	pch += 1;

	const TCHAR* pchStart = pch;
	int nLineStart = g_nLine;
	int nValues = 0;

	for (;;)
	{
		pch = SkipWhite(pch);

		if (*pch == ']')
			break;

		for (int i = 0; i < nVecSize; i += 1)
		{
			float n;
			pch = ParseNumber(pch, n);
			nValues += 1;
		}

		pch = SkipWhite(pch);

		if (*pch == ']')
			break;

/*
		if (*pch != ',')
		{
			SyntaxError(_T("Expected ']' or ','"));
			return _T("");
		}

		pch += 1;
*/
		if (*pch == ',')
			pch += 1;
	}

	cbArray = nValues * sizeof (float);
	pbArray = new BYTE [cbArray];

	BYTE* pb = pbArray;

	pch = pchStart;
	g_nLine = nLineStart;
	for (;;)
	{
		pch = SkipWhite(pch);

		if (*pch == ']')
			break;

		for (int i = 0; i < nVecSize; i += 1)
		{
			float n;
			pch = ParseNumber(pch, n);
			*(float*)pb = n;
			pb += sizeof (float);
		}

		pch = SkipWhite(pch);

		if (*pch == ']')
			break;

/*
		if (*pch != ',')
		{
			SyntaxError(_T("Expected ']' or ','"));
			return _T("");
		}

		pch += 1;
*/
		if (*pch == ',')
			pch += 1;
	}

	return pch + 1;
}


int ExpandCString(TCHAR* szString, int cchMaxString, const TCHAR* pchToken, int cchToken)
{
	TCHAR* pchString = szString;
	while (cchToken > 0)
	{
		if (pchString >= szString + cchMaxString)
		{
			SyntaxError(_T("string constant is too long"));
			return -1;
		}

		if (*pchToken == '\\')
		{
			pchToken += 1;
			cchToken -= 1;
			if (cchToken == 0)
			{
				SyntaxError(_T("backslash at end of string"));
				return -1;
			}

			TCHAR ch = *pchToken++;
			cchToken -= 1;
			switch (ch)
			{
			case 'b':
				*pchString++ = '\b';
				break;

			case 'f':
				*pchString++ = '\f';
				break;

			case 'n':
				*pchString++ = '\n';
				break;

			case 'r':
				*pchString++ = '\r';
				break;

			case 't':
				*pchString++ = '\t';
				break;

			case 'x':
				{
					if (cchToken < 2)
					{
						SyntaxError(_T("Invalid hex character escape sequence"));
						return -1;
					}

					ch = 0;
					for (int i = 0; i < 2; i += 1)
					{
						TCHAR chHex = *pchToken++;
						cchToken -= 1;

						if (chHex >= '0' && chHex <= '9')
						{
							ch = 16 * ch + chHex - '0';
						}
						else if (chHex >= 'A' && chHex <= 'F')
						{
							ch = 16 * ch + chHex - 'A' + 10;
						}
						else if (chHex >= 'a' && chHex <= 'f')
						{
							ch = 16 * ch + chHex - 'a' + 10;
						}
						else
						{
							SyntaxError(_T("Invalid hex character"));
							return -1;
						}
					}
					*pchString++ = ch;
				}
				break;

#ifdef _UNICODE
			case 'u':
				{
					if (cchToken < 4)
					{
						SyntaxError(_T("Invalid Unicode character escape sequence"));
						return -1;
					}

					ch = 0;
					for (int i = 0; i < 4; i += 1)
					{
						TCHAR chHex = *pchToken++;
						cchToken -= 1;

						if (chHex >= '0' && chHex <= '9')
						{
							ch = 16 * ch + chHex - '0';
						}
						else if (chHex >= 'A' && chHex <= 'F')
						{
							ch = 16 * ch + chHex - 'A' + 10;
						}
						else if (chHex >= 'a' && chHex <= 'f')
						{
							ch = 16 * ch + chHex - 'a' + 10;
						}
						else
						{
							SyntaxError(_T("Invalid Unicode character"));
							return -1;
						}
					}
					*pchString++ = ch;
				}
				break;
#endif

			default:
				if (ch >= '0' && ch <= '7')
				{
					if (ch == '0' && cchToken > 0 && *pchToken < '0' || *pchToken > '7')
					{
						*pchString++ = '\0';
					}
					else
					{
						ch -= '0';
						while (cchToken != 0 && *pchToken >= '0' && *pchToken <= '7')
						{
							ch = ch * 8 + *pchToken - '0';
							pchToken += 1;
							cchToken -= 1;
						}

						*pchString++ = ch;
					}
				}
				else
				{
					// REVIEW: Here we deal with \' \" and \\ as well as ignoring the \ for "unknown" escape sequences...
					// Should Unknown escapes cause an error?
					*pchString++ = ch;
				}
				break;
			}
		}
		else
		{
			*pchString++ = *pchToken++;
			cchToken -= 1;
		}
	}
	
	return (int)(pchString - szString);
}
