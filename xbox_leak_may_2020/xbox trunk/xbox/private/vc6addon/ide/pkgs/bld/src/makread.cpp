//////////////////////////////////////////////////////////////////////
// MAKREAD.CPP
//
// Implementation of CMakFileReader and CMakFileWriter.
//
// History
// =======
// Date			Who			What
// ----			---			----
// 30-May-93	mattg		Created
// 07-Jun-93	mattg		Added to VSHELL
// 13-Aug-93	mattg		Handle '\' continuation character
//							for directives
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Include files

#include "stdafx.h"
#pragma hdrstop

//////////////////////////////////////////////////////////////////////
// IMPLEMENT_*
//
// These must be placed BEFORE the #ifdef _DEBUG stuff below

IMPLEMENT_DYNAMIC(CMakComment, CObject)
IMPLEMENT_DYNAMIC(CMakMacro, CObject)
IMPLEMENT_DYNAMIC(CMakDescBlk, CObject)
IMPLEMENT_DYNAMIC(CMakDirective, CObject)
IMPLEMENT_DYNAMIC(CMakError, CObject)
IMPLEMENT_DYNAMIC(CMakEndOfFile, CObject)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


#define MW_MAXDISPLAYLEN 32

//////////////////////////////////////////////////////////////////////
// Common makefile strings:

const TCHAR *szMkLineIndent = _TEXT ("    ");
const TCHAR cMkContinuation = _T('\\');

//////////////////////////////////////////////////////////////////////
// Helper routines

static BOOL g_bIsDBCSUsed = 0;

VOID StripLeadingWhite
(
	CString &	str
)
{
	TCHAR *	pchStart;
	TCHAR *	pchCur;
	INT		ich;

	pchCur = pchStart = (TCHAR *)(const TCHAR *)str;

	while ((*pchCur == _T(' ')) || (*pchCur == _T('\t')))
		++pchCur;

	ich = pchCur - pchStart;

	str = str.Right(str.GetLength() - ich);
}

VOID StripTrailingWhite
(
	CString &	str
)
{
	TCHAR *	pchCur;
	TCHAR *	pchStart;
	TCHAR *	pchWhite = NULL;
	INT		ichWhite;

	pchCur = pchStart = (TCHAR *)(const TCHAR *)str;

	while (*pchCur != _T('\0'))
	{
		if ((*pchCur == _T(' ')) || (*pchCur == _T('\t')))
		{
			if (pchWhite == NULL)
				pchWhite = pchCur;

			++pchCur;
		}
		else
		{
			pchCur = _tcsinc(pchCur);

			pchWhite = NULL;
		}
	}

	if (pchWhite != NULL)
	{
		ichWhite = pchWhite - pchStart;
		str = str.Left(ichWhite);
	}
}

VOID StripLeadingAndTrailingWhite
(
	CString &	str
)
{

#if 0
	// UNDONE: try to avoid this copy!!!
	// check and copy in place in one pass

	TCHAR *	pchStart;
	TCHAR *	pchCur;
	TCHAR *	pchWhite = NULL;

	// copy into temp buffer so fast assignment works (below)
	CString strTemp = str;

	pchCur = (TCHAR *)(const TCHAR *)strTemp;

	while ((*pchCur == _T(' ')) || (*pchCur == _T('\t')))
		++pchCur;

	pchStart = pchCur;

	while (*pchCur != _T('\0'))
	{
		if ((*pchCur == _T(' ')) || (*pchCur == _T('\t')))
		{
			if (pchWhite == NULL)
				pchWhite = pchCur;

			++pchCur;
		}
		else
		{
			pchCur = _tcsinc(pchCur);

			pchWhite = NULL;
		}
	}

	if (pchWhite != NULL)
	{
		*pchWhite = _T('\0');
	}


	str = pchStart;
#else
	TCHAR *	pchStart;
	TCHAR *	pchCur;
	TCHAR *	pchTemp;
	TCHAR *	pchWhite = NULL;

	pchStart = pchCur = str.GetBuffer(0);

	// first check for leading whitespace and skip
	while ((*pchCur == _T(' ')) || (*pchCur == _T('\t')))
	{
		++pchCur;
	}

	// different code if leading whitespace
	int nLeadingWhite = (pchCur - pchStart);
	char c;

	if (nLeadingWhite != 0)
	{
		pchTemp = pchStart;
		while ((c = *pchCur) != _T('\0') )
		{
			if ((c == _T(' ')) || (c == _T('\t')))
			{
				if (pchWhite == NULL)
					pchWhite = pchCur; // we adjust this later
			}
			else
			{
				pchWhite = NULL;
				if ((g_bIsDBCSUsed) && (IsDBCSLeadByte(c)))
					*(pchTemp++) = *(pchCur++);	// extra copy if DBCS
			}
			*(pchTemp++) = *(pchCur++);
		}
	}
	else
	{
		if (g_bIsDBCSUsed)
		{
			while ((c = *pchCur) != _T('\0') )
			{
				if ((c == _T(' ')) || (c == _T('\t')))
				{
					if (pchWhite == NULL)
						pchWhite = pchCur;
					pchCur++;
				}
				else
				{
					pchWhite = NULL;
					pchCur = _tcsinc(pchCur);
				}
			}
		}
		else
		{
			for ( c = *pchCur; c != _T('\0'); c = *++pchCur )
			{
				if ((c == _T(' ')) || (c == _T('\t')))
				{
					if (pchWhite == NULL)
						pchWhite = pchCur;
				}
				else
				{
					pchWhite = NULL;
				}
			}
		}
	}

	if (pchWhite!=NULL)
	{
		pchCur = pchWhite; 	// reset pchCur to actual end of string
		*pchCur = _T('\0'); // and null terminate
	}
	str.ReleaseBuffer((pchCur - pchStart) - nLeadingWhite);
#endif
}

VOID SkipWhite
(
	const TCHAR * &	pch
)
{
	while (*pch && ((*pch == _T(' ')) || (*pch == _T('\t'))))
		++pch;
}

VOID SkipNonWhite
(
	const TCHAR * &	pch
)
{
	while (*pch && (*pch != _T(' ')) && (*pch != _T('\t')) )
		++pch;
}

int  GetMakToken(const TCHAR * pBase, const TCHAR *& pTokenEnd)
		// throw CFileException
{
	int r = 0;
	BOOL bInQuotes = FALSE;
	pTokenEnd = pBase;

	// Skip whitespace at beginning:
	while (*pTokenEnd 
			&& 
		   (*pTokenEnd == _T(' ') || *pTokenEnd == _T('\t') )
		   )
	{
		r++;
		pTokenEnd++;
	}
     
	while (*pTokenEnd)
	{
		if (  *pTokenEnd ==  _T('"') ) bInQuotes = ~bInQuotes;
		else if (*pTokenEnd == _T(' ') || *pTokenEnd == _T('\t')
			|| *pTokenEnd == _T('\r')
#ifdef _UNICODE
			|| *pTokenEnd == _T('\n')  // should always get \r before \n unless Unicode
#endif
			 )
		{
			if (!bInQuotes) break;
		}
		r++;

		pTokenEnd = _tcsinc ( (char *) pTokenEnd);
    }  
	#ifdef _DEBUG
	if (bInQuotes && !*pTokenEnd) TRACE ("Unmatched quotes in GetToken\n");
	#endif 

	return r;
}

BOOL GetQuotedString(const TCHAR *& pBase, const TCHAR *& pEnd)
{				
	ASSERT (pBase);

	while (*pBase && *pBase != _T('"'))
		 pBase=_tcsinc((TCHAR *)pBase);

	if (*pBase != _T('"'))
		return FALSE;

	pEnd = (pBase =_tcsinc((TCHAR *)pBase));		// Go past quote.

	while (*pEnd && *pEnd != _T('"'))
		pEnd=_tcsinc ( (TCHAR *) pEnd );

	if (pBase == pEnd || *pEnd != _T('"')) 
		return FALSE;

	return TRUE;
} 

//////////////////////////////////////////////////////////////////////
// CNameMunger class

#ifdef _MBCS
#define MAX_EXPANSION 4   
//#define MAX_EXPANSION 4 / 2
#else
#define MAX_EXPANSION 1
#endif

void CNameMunger::MungeName (const TCHAR *pszName, CString &strResult)
{
	// Take a string and convert it to something that can be put in
	// a make file as part of a macro name.  In the sbcs world we,
	// do this by taking the first 5 characters, converting any
	// bad characters to underscores, and then adding characters
	// until we no longer collide.  If we reach the end of the string,
	// we just start adding unique numbers until there's no collision.

	// In the mbcs world, we convert ASCII characters to underscores
	// and double byte charactes to there hex representation.

	TCHAR *pbase, *pcur, *pupcase;
	const TCHAR *pinbase;
	int i;
	void *pdum;
	CString *pcstr;

	// No one should be giving us an empty string:
    ASSERT ( _tcslen (pszName) > 0 );

	// Go for quick kill:  Have we seen the name before
	if (m_InMap.Lookup ( pszName, (void *&) pcstr))
	{
		strResult = *pcstr;
		return;
	}

	i = _tcslen (pszName);
	pinbase = pszName;
	// Get a buffer which we know will be big enough to hold the 
	// expanded name:
	pcur = pbase = strResult.GetBuffer 
							( i * MAX_EXPANSION + 9);

	pupcase = new TCHAR [i+1];
	// Try looking up upcased version of supplied string:
	_tcscpy (pupcase, pszName );
	_tcsupr (pupcase);
		if (m_InMap.Lookup ( pbase, (void *&) pcstr))
	{
		strResult = *pcstr;

		// Enter the base string for future reference:
		m_InMap.SetAt (pszName, (void *) pcstr);
		delete [] (pupcase);
		return;
	}
 

	for ( ;*pszName != _T('\0'); pszName = _tcsinc( (TCHAR *) pszName) )
	{
		// Copy the next character out of the source string and tranlate
		// it if nessesary:

		if ( *pszName == _T('_')
			||
			(*pszName >= _T('0') && *pszName <= _T('9'))
			||
			(*pszName >= _T('A') && *pszName <= _T('Z'))
			)
		{
			*pcur++ = *pszName;	
		}
		else if (*pszName >= _T('a') && *pszName <= _T('z'))
		{
			*pcur++ = *pszName -  (_T('a') - _T('A'));	// Upcase
		}
#ifdef _MBCS
		else if ( IsDBCSLeadByte ( (BYTE)*pszName))
		{
			_ultoa ( (LONG) *((UNALIGNED DWORD *) pszName), pcur, 16 );
			while (*pcur) pcur++;			 
		}
#endif
		else 
		{
			// Shave off leading weird characters:  Ignore if we haven't
			// advanced in the buffer.
			if (pcur > pbase) *pcur++ = _T('_');	
		}

		*pcur = _T('\0');				
		if ((pcur - pbase) < 5) continue;	// Alway use at least 5 if we can.

		if ( !m_ResMap.Lookup ( pbase, pdum)) break;
	}

	// If we reached the end of the input string and couldn't find an
	// unused name, then default to unique number type scheme.  If there
	// are no valid characters at all, start the name with an "M_":
	if (*pszName == _T('\0'))		    
	{
		if (pcur == pbase )
		{
			_tcscpy (pbase, "M_");
			pcur += 2;
		}

		i = m_InMap.GetCount();
		do 
		{
			_ultoa ( i, pcur, 16);
			i++;
		} while (m_ResMap.Lookup ( pbase, pdum));
	}

	strResult.ReleaseBuffer ();

	ASSERT (!m_ResMap.Lookup ( pbase, pdum));

	pcstr = m_ResMap.SetAtAndReturnStringAddress (strResult);
	m_InMap.SetAt (pinbase, (void *) pcstr);
	// Also save upcased version:
	m_InMap.SetAt (pupcase, (void *) pcstr);
	delete [] (pupcase);

}
//////////////////////////////////////////////////////////////////////
// Constructors, destructors for CMak* objects

CMakComment::~CMakComment(){}
CMakComment::CMakComment
(
	const TCHAR * pszText
)
{
	m_strText = pszText;
	StripLeadingAndTrailingWhite(m_strText);
}
#ifdef _DEBUG
 void CMakComment::Dump ( CDumpContext& dc ) const
{
	CObject::Dump (dc);
	dc << "m_strText: " << m_strText << "\n";
}
#endif
CMakMacro::~CMakMacro(){}
CMakMacro::CMakMacro
(
	const CString & strMacName,
	const CString & strMacValue
)
{
	m_strName = strMacName;
	m_strValue = strMacValue;
}
#ifdef _DEBUG
 void CMakMacro::Dump ( CDumpContext& dc ) const
{
	CObject::Dump (dc);
	dc << "m_strName: " << m_strName << " m_strValue: " << m_strValue <<"\n";
}
#endif

CMakDescBlk::~CMakDescBlk(){}
CMakDescBlk::CMakDescBlk
(
	const CString & strTargets,
	const CString & strDeps
)
{
	m_strTargets = strTargets;
	m_strDeps = strDeps;
}
#ifdef _DEBUG
 void CMakDescBlk::Dump ( CDumpContext& dc ) const
{
	CObject::Dump (dc);
	dc << "m_strTargets: " << m_strTargets << " m_strDeps: " << m_strDeps <<
			"Commands: \n";
	int n = dc.GetDepth ();
	dc.SetDepth (1);
	dc << m_listCommands;
	dc.SetDepth (n);
}
#endif

#define CDIRECTIVES 13

struct
{
	TCHAR *					szDirKeyword;
	int						nByteLen;
	CMakDirective::DTYP		dtyp;
}

#define DIRTEXT(x) _TEXT(x), (sizeof (x)-1) 

//  Note: in this table, longer items must come before items with
//  the same prefix (e.g. IFDEF before IF):

g_rgdirtbl[CDIRECTIVES] =
{
	{ DIRTEXT("CMDSWITCHES"),	CMakDirective::DTYP_CMDSWITCHES},
	{ DIRTEXT("ERROR"),			CMakDirective::DTYP_ERROR		},
	{ DIRTEXT("MESSAGE"),		CMakDirective::DTYP_MESSAGE		},
	{ DIRTEXT("INCLUDE"),		CMakDirective::DTYP_INCLUDE		},
	{ DIRTEXT("IFNDEF"),		CMakDirective::DTYP_IFNDEF		},
	{ DIRTEXT("IFDEF"),			CMakDirective::DTYP_IFDEF		},
	{ DIRTEXT("IF"),			CMakDirective::DTYP_IF			},
	{ DIRTEXT("ELSEIFNDEF"),	CMakDirective::DTYP_ELSEIFNDEF	},
	{ DIRTEXT("ELSEIFDEF"),		CMakDirective::DTYP_ELSEIFDEF	},
	{ DIRTEXT("ELSEIF"),		CMakDirective::DTYP_ELSEIF		},
	{ DIRTEXT("ELSE"),			CMakDirective::DTYP_ELSE		},
	{ DIRTEXT("ENDIF"),			CMakDirective::DTYP_ENDIF		},
	{ DIRTEXT("UNDEF"),			CMakDirective::DTYP_UNDEF		}
};

extern BOOL g_bConvDontTranslate;	// defined in project.cpp

CMakDirective::~CMakDirective(){}
CMakDirective::CMakDirective
(
	DTYP			dtyp,
	const CString & strRemOfLine
)
{
	m_dtyp = dtyp;
	m_strRemOfLine = strRemOfLine;

#if 0	// This doesn't always work correctly, so skip it
	// message strings may have been localized in v6.0 and beyond
	if ((dtyp == CMakDirective::DTYP_MESSAGE /* || dtyp == CMakDirective::DTYP_ERROR */) && (!g_bConvDontTranslate))
	{
		m_strRemOfLine.OemToAnsi();
	}
#endif
}
#ifdef _DEBUG
 void CMakDirective::Dump ( CDumpContext& dc ) const
{
	CObject::Dump (dc);
	dc << "m_dtyp " << g_rgdirtbl[m_dtyp].szDirKeyword << 
		" m_strRemOfLine: " << m_strRemOfLine <<"\n";
}
#endif

CMakError::~CMakError(){}
CMakError::CMakError(){}

CMakEndOfFile::~CMakEndOfFile(){}
CMakEndOfFile::CMakEndOfFile(){}

//////////////////////////////////////////////////////////////////////
// Constructors, destructors for CMakFileReader()
// How often to update the status bar:
const int CMakFileReader::nStatusBarUpdate = 16;
CMakFileReader::CMakFileReader() 
{
	m_pUngotElement = NULL;
	m_nFileSize = 0;
	m_nProgressGoal = 0;
	g_bIsDBCSUsed = (theApp.GetCP() != 0);
}

CMakFileReader::~CMakFileReader()
{
	if (m_pUngotElement) delete (m_pUngotElement);
	if (m_nFileSize && (m_iAttrib & MakRW_ShowStatus))
	{
		::StatusPercentDone(100);	// Let the user see this.
		::StatusEndPercentDone();
	}
}

//////////////////////////////////////////////////////////////////////
// CMakFileWriter::Abort
void CMakFileReader::Abort()
{
	m_file.Abort ();
}

//////////////////////////////////////////////////////////////////////
// CMakFileReader::Close
void CMakFileReader::Close()
{
	m_file.Close ();
}

//////////////////////////////////////////////////////////////////////
// CMakFileReader::FillBuf

VOID CMakFileReader::FillBuf
(
)
{
	UINT	cchRead;

	cchRead = m_file.Read(m_rgchBuf, (CCH_BUF - 1) * sizeof(TCHAR));
	m_cchFile += cchRead;

	if (cchRead == 0)
	{
		/* At end-of-file.  Put a MAK_EOF TCHAR at the first position and
		** set the m_fEOF flag.
		*/

		m_rgchBuf[0] = MAK_EOF;
		m_rgchBuf[1] = _T('\0');
		m_fEOF = TRUE;
	}
	else
	{
		/* Terminate the buffer with a '\0'.
		**
		** Note that in the UNICODE case, cchRead may be odd, in which
		** case the file is bogus (second byte of a UNICODE character
		** is absent).  Anyway, we simply ignore the "half character"
		** at EOF.
		*/

		m_rgchBuf[cchRead / sizeof(TCHAR)] = _T('\0');
	}

	m_ichBuf = 0;
}

//////////////////////////////////////////////////////////////////////
// CMakFileReader::GetLine

BOOL CMakFileReader::GetLine
(
	CString &	strRet
)
{
	TCHAR	ch;
	UINT bufmax = 256;
	UINT	cch = 0;

	// let's alloc a buffer to use up-front, this'll save allocs when 
	// we 'strRet += ch'. we assume that most lines in the makefikle
	// are going to be less than 256 bytes.
	char * pszRet = strRet.GetBuffer(bufmax);

	while (((ch = GetChar()) != _T('\r')) && ((ch != MAK_EOF) || (!m_fEOF)))
	{
		pszRet[cch++] = ch;

		/* We count how many characters we've accumulated to deal with the
		** case where we've been asked to read a binary file and a '\r' may
		** not be seen for a long time.  To avoid accumulating huge strings,
		** limit the length of the string artificially.  This may end up
		** splitting a line in a legitimate makefile, but as long as the
		** maximum line length is on the order of 1K bytes, it seems unlikely.
		**
		** Set ch to _T('\r') to make the code outside of this loop think that
		** we've encountered end-of-line.
		*/

		// make sure that if we reach allocation limit, we grow it efficiently
		if (cch >= bufmax)
		{
			if (cch >= ((256 * 16))) // make limit multiple of 256
			{
				ch = _T('\r');
				break;
			}

			bufmax += 256;
			(void) strRet.ReleaseBuffer(cch);	// set data length, and terminate
			pszRet = strRet.GetBuffer(bufmax);  // get new larger buffer
		}
		
	}

	pszRet[cch] = _T('\0'); // ensure nul-terminated

	(void) strRet.ReleaseBuffer(cch);	// set data length, and terminate
	
	if (ch == _T('\r'))
	{
		if (PeekChar() == _T('\n'))
			GetChar();

		return(TRUE);
	}

	return(FALSE);
}

//////////////////////////////////////////////////////////////////////
// CMakFileReader::ParseDirective

CObject * CMakFileReader::ParseDirective
(
	const CString &	strLine
)
{
	const TCHAR *	pch;
	INT				idir, cch;
	CString			strRemOfLine, strT;

	ASSERT(strLine[0] == _T('!'));

	pch = (const TCHAR *)strLine + 1;

	/* Skip any whitespace following the '!'.
	*/

	SkipWhite(pch);

	if (*pch == _T('\0'))
		return(new CMakError);

	/* Now search for a matching keyword.
	*/

	for (idir=0 ; idir<CDIRECTIVES ; ++idir)
		if (_tcsnicmp(	g_rgdirtbl[idir].szDirKeyword,
						pch,
						cch = _tcslen(g_rgdirtbl[idir].szDirKeyword)) == 0)
			break;

	if (idir == CDIRECTIVES)
		return(new CMakError);

	strRemOfLine = pch + cch;

	if (!((m_iAttrib & MakR_IgnLineCont) || strRemOfLine.IsEmpty()))
	{
		for (;;)
		{
			pch = strRemOfLine;

			if (*_tcsdec(pch, (TCHAR *)pch + _tcslen(pch)) != cMkContinuation)
				break;

			if (!GetLine(strT))
				return(new CMakError);

			strRemOfLine.SetAt(strRemOfLine.GetLength() - 1, _T(' '));
			strRemOfLine += strT;
		}
	}

	return(new CMakDirective(g_rgdirtbl[idir].dtyp, strRemOfLine));
}

//////////////////////////////////////////////////////////////////////
// CMakFileReader::ParseMacro

CObject * CMakFileReader::ParseMacro
(
	const CString &	strLine,
	INT				ichEq
)
{
	// UNDONE: requires further optimization for buffer allocation

	int len;
	const TCHAR *		pch;
	CString				strName, strValue, strT, strRawData;
	CMakMacro * pMacro;
	// alloc buffers to use up-front, this'll save allocs when 
	// we 'strRawData += ch'.  Note: this can be quite large! (4000+)
	strRawData.GetBuffer(4096);
	strValue.GetBuffer(4096);
	strRawData.ReleaseBuffer(0);
	strValue.ReleaseBuffer(0);

	strRawData += strLine;
	strRawData += _T("\r\n");

	/* Everything up to, but not including, the '=' is considered
	** to be the macro name.
	*/

	strName = strLine.Left(ichEq);
	strValue += (LPCTSTR(strLine) + (ichEq + 1));

	/* While the last character of the value string is '\', fetch
	** the next line and append it (line continuation).
	*/

	if (!((m_iAttrib & MakR_IgnLineCont) || strValue.IsEmpty()))
	{
		for (;;)
		{

			len = strValue.GetLength();
			pch = strValue;
			if (*_tcsdec(pch, (TCHAR *)(pch + len)) != cMkContinuation)
				break;

			if (!GetLine(strT))
				return(new CMakError);

			strRawData += strT;
			strRawData += _T("\r\n");

			strValue.SetAt(len-1, _T(' '));
			strValue += strT;
		}
	}

	/* Now strip leading and trailing whitespace from both the macro
	** name and macro value.
	*/

	StripLeadingAndTrailingWhite(strName);
	StripLeadingAndTrailingWhite(strValue);

	/* Return a new CMakMacro object.
	*/

	strValue.FreeExtra();
	strRawData.FreeExtra();
	pMacro = new CMakMacro(strName, strValue);
	pMacro->m_strRawData = strRawData;
	return pMacro;
}

//////////////////////////////////////////////////////////////////////
// CMakFileReader::ParseDescBlk

CObject * CMakFileReader::ParseDescBlk
(
	const CString &	strLine,
	INT				ichColon
)
{
	TCHAR			ch;
	const TCHAR *	pch;
	BOOL			fDoubleColon = FALSE, fFirstLine;
	INT				ich, ich1;
	CString			strLHS, strRHS, strT, strRawData;
	CMakDescBlk *	pmdb;
	(void)strRHS.GetBuffer(255);
	(void)strRHS.ReleaseBuffer(0);
	(void)strRawData.GetBuffer(511);
	(void)strRawData.ReleaseBuffer(0);

	strRawData += strLine;
	strRawData += _T("\r\n");	// record raw data incase needed later

	/* Split the line into its left-hand side (targets) and
	** right-hand side (dependents).
	*/

	if	(
		(ichColon != strLine.GetLength() - 1)
		&&
		(strLine[ichColon + 1] == _T(':'))
		)
		fDoubleColon = TRUE;

	strLHS = strLine.Left(ichColon);
	StripLeadingAndTrailingWhite(strLHS); // used for unique matches
	strRHS += strLine.Right(strLine.GetLength() - ichColon - (fDoubleColon ? 2 : 1));

	/* While the last character of the RHS string is '\', fetch
	** the next line and append it (line continuation).
	*/

	if (!strRHS.IsEmpty())
	{
		for (;;)
		{
			pch = strRHS;

			if (*_tcsdec(pch, (TCHAR *)pch + _tcslen(pch)) != cMkContinuation)
				break;

			if (!GetLine(strT))
				return(new CMakError);

			strRawData += strT;
			strRawData += _T("\r\n");
			strRHS.SetAt(strRHS.GetLength() - 1, _T(' '));
			strRHS += strT;
		}
	}

	strRHS.FreeExtra();
	pmdb = new CMakDescBlk(strLHS, strRHS);

	/* Now fetch all subsequent command lines, and add them to the object.
	*/

	fFirstLine = TRUE;

	for (;;)
	{
		/* First peek at the next character.  If it's not whitespace,
		** not a blank line, we'll break out of the loop, since this 
		** will end the list of command lines.
		** Note that this means that a comment will terminate a 
		** descritption block, which is not strictly NMAKE like, but
		** otherwise we will swallow potientially interesting comments:
		*/

		ch = PeekChar();

		if	(
			((ch == MAK_EOF) && (m_fEOF))
			||
				(
				(ch != _T(' '))
				&&
				(ch != _T('\t'))
				&&
				(ch != _T('\r'))
				)
			)
			break;

		/* Now fetch the next line.
		*/

		if (!GetLine(strRHS))
			break;

		strRawData += strRHS;
		strRawData += _T("\r\n");

		/* Check for an empty line.  This is permitted (and ignored)
		** unless it is the first line after the dependency line, in
		** which case it ends the description block.
		*/

		if (strRHS.IsEmpty())
		{
			if (fFirstLine)
				break;
			else
				continue;
		}

		fFirstLine = FALSE;

		/* Check to see if this line is a comment line (possible leading
		** whitespace followed by a comment) or all whitespace.  If so,
		** just ignore it and continue.
		*/

		pch = strRHS;

		SkipWhite(pch);

		if ((*pch == _T('\0')) || (*pch == _T('#')))
			continue;

		/* Check to see if this line contains an inline file specification.
		** If so, accumulate all successive lines up to and including the
		** terminating line.
		*/

		if	(((ich = strRHS.Find(_T('<'))) != -1) &&
			 (ich < strRHS.GetLength() - 1) &&
			 (strRHS[ich+1] == _T('<'))
			)
		{
			for (;;)
			{
				if (!GetLine(strT))
				{
					delete pmdb;
					return(new CMakError);
				}

				strRawData += strT;
				strRawData += _T("\r\n");

				strRHS += _T('\n');
				strRHS += strT;

				if	((strT.GetLength() >= 2) &&
					 (strT[0] == _T('<')) &&
					 (strT[1] == _T('<'))
					)
					break;
			}
		}

		/* Now accumulate command lines while each ends in '\'.
		*/

		if (!(m_iAttrib & MakR_IgnLineCont))
		{
			for (;;)
			{
				pch = strRHS;

				if (*_tcsdec(pch, (TCHAR *)pch + _tcslen(pch)) != cMkContinuation)
					break;

				if (!GetLine(strT))
				{
					delete pmdb;
					return(new CMakError);
				}

				strRawData += strT + _T("\r\n");

				strRHS.SetAt(strRHS.GetLength() - 1, _T(' '));
				strRHS += strT;
			}
		}

		if (pmdb->m_strTool.IsEmpty())
		{
			// if we don't already have a tool, try to find one: $(TOOL)
			ich = strRHS.Find(_T("$("));
			if (ich!=-1)
			{
				ich += 2;
				ich1 = strRHS.Find(_T(')'));
				if (ich < ich1)
				{
					pmdb->m_strTool = strRHS.Mid(ich, (ich1 - ich));
					StripLeadingAndTrailingWhite(pmdb->m_strTool);
				}
			}
		}

		/* Finally, add this string to the list of commands.
		*/

		strRHS.FreeExtra();
		pmdb->m_listCommands.AddTail(strRHS);
	}

	strRawData.FreeExtra();
	pmdb->m_strRawData = strRawData;
	return pmdb;
}

//////////////////////////////////////////////////////////////////////
// CMakFileReader::Open

BOOL CMakFileReader::Open
(
	const TCHAR *		szFilename,
	UINT				nOpenFlags,
	CFileException *	exc,
	UINT				iAttrib	/* = MakRW_Default | MakRW_ShowStatus */
)
{
	// preserve our attributes
	m_iAttrib = iAttrib;

	m_nLineNum = 1;
	m_rgchBuf[0] = _T('\0');
	m_ichBuf = 0;
	m_fEOF = FALSE;
	m_cchFile = 0;
	CString str;

	if (m_file.Open(szFilename, nOpenFlags, exc))
	{
		if (m_iAttrib & MakRW_ShowStatus)
		{
			TRY
			{
				m_nFileSize	= m_file.GetLength ();
			}
			CATCH_ALL (e)
			{
				ASSERT (m_nFileSize	== 0);
			}
			END_CATCH_ALL	

			if (m_nFileSize && (m_iAttrib & MakRW_ShowStatus))
			{
				VERIFY (str.LoadString ( IDS_READING_PROJECT ));
				CPath path;
				if (path.Create(szFilename))
				{
					CString strProject;
					path.GetBaseNameString(strProject);
					if (strProject.GetLength() <= MW_MAXDISPLAYLEN)
						str += strProject;
				}
				::StatusBeginPercentDone ( str);
				m_nObjectsRead = 0;
			}
		}
		return TRUE;
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// ReOpen

void CMakFileReader::ReOpen()
{
	// seek to beginning of file
	(void)m_file.Seek(0, CFile::begin);

	m_nLineNum = 1;
	m_rgchBuf[0] = _T('\0');
	m_ichBuf = 0;
	m_fEOF = FALSE;
	m_cchFile = 0;

	// reset the number of objects read
	if (m_nFileSize && (m_iAttrib & MakRW_ShowStatus))
		m_nObjectsRead = 0;
}


//////////////////////////////////////////////////////////////////////
// GetNextElement

CObject * CMakFileReader::GetNextElement
(
)
{
	const TCHAR *		pch;
	const TCHAR *		pchT;
	BOOL				fInQuotes;
	CString				strLine;

	// Return the ungot element if there is one:
	if (m_pUngotElement)
	{
		ASSERT_VALID (m_pUngotElement);

		CObject *pUngotElement = m_pUngotElement;
		m_pUngotElement = NULL;
		return pUngotElement;
	}

	// Update the status bar if its time:
	if (m_nFileSize && (++m_nObjectsRead % nStatusBarUpdate) == 0)
	{
		DWORD dw;
		dw = m_file.GetPosition ();
		// This will fail with makefiles over 40 meg:
	 	dw = dw * 100 / m_nFileSize;
		if (dw >= m_nProgressGoal) // only update at most 20 times
		{
			::StatusPercentDone ( (int) dw);
			m_nProgressGoal += 5; // increment target by 5%
		}
	}

	for (;;)
	{
		/* Fetch the next line.
		*/

		if (!GetLine(strLine))
			return(new CMakEndOfFile);

		pch = (const TCHAR *)strLine;

		/* Find the first non-white character on the line, if any.
		*/

		SkipWhite(pch);

		/* Break out of the loop if this line is *not* entirely blank.
		** Otherwise, loop back to fetch another line.
		*/

		if (*pch != _T('\0'))
			break;
	}

	/* Check to see if the next character is '#'.  If so, the entire
	** line is a comment.
	*/

	if (*pch == _T('#'))
		return(new CMakComment(pch + 1));

	/* At this point, if there was any leading whitespace, an error ensues.
	*/

	if (pch != (const TCHAR *)strLine)
		return(new CMakError);

	/* If the first character is '!', process the directive.
	*/

	if (*pch == _T('!'))
		return(ParseDirective(strLine));

	/* This line must either be a macro definition or a dependency line
	** (the start of a description block).
	**
	** Look for ':' and '=' inside the string, ignoring any occurrences
	** inside double quotes.
	**
	** If a non-quoted '=' is found, this is a macro definition.
	**
	** If a non-quoted ':' is found, this is a dependency line as long
	** as it was preceded by whitespace or at least two non-whitespace
	** characters (this rule is necessary to permit filenames with drive
	** specifications, e.g., "c:\foo\bar\blix.c").
	**
	** If this line is neither a macro definition nor a dependency line,
	** return an error.
	*/

	fInQuotes = FALSE;

	while (*pch != _T('\0'))
	{
		if (*pch == _T('"'))
		{
			fInQuotes = !fInQuotes;
		}
		else if ((*pch == _T('=')) && (!fInQuotes))
		{
			break;
		}
		else if ((*pch == _T(':')) && (!fInQuotes))
		{
			pchT = pch;

			if (pchT == (const TCHAR *)strLine)
			{
				/* We found the ':' at the beginning of the line.
				** This is an error (no targets!).
				*/

				return(new CMakError);
			}

			/* Check previous character.
			*/

			pchT = _tcsdec((const TCHAR *)strLine, (TCHAR *)pchT);

			if ((*pchT == _T(' ')) || (*pchT == _T('\t')))
			{
				/* Previous character was whitespace.  This is a
				** legitimate separator.
				*/

				break;
			}

			/* The previous character was NOT whitespace.
			**
			** Check the character two characters back.  If it's
			** whitespace, or if it doesn't exist (i.e., the character
			** one character back is the first character of the string),
			** then this is NOT a legitimate separator.
			**
			** Thus, this is a legitimate separator only if the
			** previous character exists and is not whitespace.
			*/

			if (pchT != (const TCHAR *)strLine)
			{
				pchT = _tcsdec((const CHAR *)strLine, (TCHAR *)pchT);

				if ((*pchT != _T(' ')) && (*pchT != _T('\t')))
					break;
			}
		}

		++pch;

#ifdef _MBCS
		if (IsDBCSLeadByte(*(pch-1)))
			++pch;
#endif
	}

	if (*pch == _T('='))
	{
		return(ParseMacro(strLine, pch - (const TCHAR *)strLine));
	}
	else if (*pch == _T(':'))
	{
		return(ParseDescBlk(strLine, pch - (const TCHAR *)strLine));
	}
	else
		return(new CMakError);
}

void CMakFileReader::UngetElement (CObject *pUngotElement)
{
	// Only single depth:
	ASSERT_VALID (pUngotElement);
	ASSERT (m_pUngotElement == NULL );	
	m_pUngotElement = pUngotElement;
}
//////////////////////////////////////////////////////////////////////
//
// ------------------------ CMakFileWriter --------------------------
//
//////////////////////////////////////////////////////////////////////
// Constructors, destructors for CMakFileWriter()
const int CMakFileWriter::nLineLen = 1200;
const int CMakFileWriter::nStatusBarUpdate = 2;

CMakFileWriter::CMakFileWriter()
{
	m_CurrentPos = 0;
	m_bLastLineBlank = FALSE;
	m_nObjects = 0;
	m_bInit = FALSE;
	m_pFileSaver = NULL;
	m_nProgressGoal = 0;
	g_bIsDBCSUsed = (theApp.GetCP() != 0);
}

CMakFileWriter::~CMakFileWriter()
{
	if (m_pFileSaver)
	{
		delete m_pFileSaver;
		m_pFileSaver = NULL;
	}

	if (m_nObjects && (m_iAttrib & MakRW_ShowStatus))
	{
		::StatusPercentDone(100);	// Let the user see this.
		::StatusEndPercentDone();
	}
	m_bInit = FALSE;
}
//////////////////////////////////////////////////////////////////////
// CMakFileWriter::Create

BOOL CMakFileWriter::Create
(
	const TCHAR *		szFilename,
	CFileException *	exc,
	int 				nObjects, /* = 0 */
	UINT				iAttrib	/* = MakRW_Default | MakRW_ShowStatus */
)
{
	ASSERT (m_CurrentPos == 0 );
	m_nCharCount = 0;

	// preserve our attributes
	m_iAttrib = iAttrib;

	ASSERT(m_pFileSaver==NULL);
	// use temporary file for write in case of error
	if (!m_pFileSaver)
		m_pFileSaver = new CFileSaver(szFilename);

	// verify file is writable
	if ((m_pFileSaver) && (!m_pFileSaver->CanSave()))
		{
		// N.B. This stuff does not really use file error support.
		exc->m_cause = CFileException::accessDenied;
		SetFileError(exc->m_cause);
		return FALSE;
		}

	if (m_File.Open(
				((m_pFileSaver)	? m_pFileSaver->GetSafeName()
								: szFilename), 
				CFile::modeCreate | CFile::modeWrite,
				exc))
	{
		m_nObjects = nObjects;
		if (m_nObjects && (m_iAttrib & MakRW_ShowStatus))
		{
			CString str;
			if (g_bWriteProject)
			{
				VERIFY(str.LoadString(IDS_WRITING_PROJECT));
			}
			else
			{
				ASSERT(g_bExportMakefile);
				VERIFY(str.LoadString(IDS_EXPORTING_MAKEFILE));
			}
			CPath path;
			if (path.Create(szFilename))
			{
				CString strProject;
				if (g_bWriteProject)
				{
					path.GetBaseNameString(strProject);
					if (strProject.GetLength() <= MW_MAXDISPLAYLEN)
						str += strProject;
				}
				else
				{
					strProject = path.GetFileName();
					if (strProject.GetLength() <= (MW_MAXDISPLAYLEN+4)) // +.mak
						str += strProject;
				}

			}
			::StatusBeginPercentDone(str);
		}
		m_nObjectsWritten = 0;
		m_bInit = TRUE;
		return TRUE;
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
const TCHAR* CMakFileWriter::GetFileName()
{
	return m_pFileSaver->GetRealName();
}
//////////////////////////////////////////////////////////////////////
void CMakFileWriter::WroteObject ()
{
	if (!m_nObjects || ((++m_nObjectsWritten % nStatusBarUpdate) != 0)
		|| ((m_iAttrib & MakRW_ShowStatus)==0))
		return;

		DWORD dw =  m_nObjectsWritten * 100 / m_nObjects;
		if (dw >= m_nProgressGoal) // only update at most 20 times
		{
			::StatusPercentDone ( (int) dw);
			m_nProgressGoal += 5; // increment target by 5%
		}
}			
//////////////////////////////////////////////////////////////////////
// CMakFileWriter::Abort
void CMakFileWriter::Abort()
{
	m_File.Abort();
	m_bInit = FALSE;
}
//////////////////////////////////////////////////////////////////////
// CMakFileWriter::Close
BOOL CMakFileWriter::Close()
{
	if (m_CurrentPos > 0)
	{
		ASSERT (m_File.m_hFile != (UINT) CFile::hFileNull);
		m_File.Write (m_buf, m_CurrentPos );
		m_CurrentPos = 0; 
	}

	if (m_File.m_hFile != (UINT) CFile::hFileNull) m_File.Close ();

	m_bInit = FALSE;

	if (m_pFileSaver)
		return m_pFileSaver->Finish();
	return TRUE;
}
//////////////////////////////////////////////////////////////////////
// CMakFileWriter::WriteBytes
void CMakFileWriter::WriteBytes ( const BYTE *pByte, int nBytes )
{
	// FUTURE: broken for UNICODE
	ASSERT (pByte);
	int i;
	
	if (nBytes > CCH_BUF)
	{
		m_File.Write (m_buf, m_CurrentPos);
		m_CurrentPos = 0;
		m_File.Write (pByte, nBytes);
		return;
	}

	
	if (m_CurrentPos + nBytes  > CCH_BUF)
	{						   
		i = CCH_BUF- m_CurrentPos;
		memcpy (m_buf + m_CurrentPos, pByte, i);
		m_File.Write (m_buf, CCH_BUF);
		pByte += i;
		nBytes -= i;
        m_CurrentPos = 0;
	}

	memcpy (m_buf + m_CurrentPos, pByte, nBytes);
    m_CurrentPos += nBytes;
}						  	
//////////////////////////////////////////////////////////////////////
// CMakFileWriter::WriteStringEx
void CMakFileWriter::WriteStringEx ( const TCHAR *pc )
{
	if (m_nCharCount > 0) EndLine ();

	if  (m_bLastLineBlank)	// strip leading CR/LF if redundent
	{
		if (*pc == _T('\r'))
			pc++;
		if (*pc == _T('\n'))
			pc++;
	}
	int len = _tcslen(pc);
	WriteBytes ( (BYTE *) pc, len * sizeof(TCHAR));

	m_nCharCount = 0;
	if (len >= 4)
		m_bLastLineBlank = (_tcscmp(pc + len - 4, _T("\r\n\r\n"))==0);
}
//////////////////////////////////////////////////////////////////////
// CMakFileWriter::WriteString
void CMakFileWriter::WriteString(const TCHAR *pc)
{
	const char *pEndOfToken, *pcTokenStart;
	int nTokenLen;

	// Check to see if there is a "\" as the last character
	BOOL fLastNotBackWhack =
		(!*pc || (pEndOfToken = pc + _tcslen (pc), pEndOfToken = \
		 _tcsdec (pc, (char *)pEndOfToken), *pEndOfToken != cMkContinuation));
				 
	while (*pc)
	{
		pcTokenStart = pc;
		// Look for a line break before the start of the next token:
		while (*pcTokenStart 
				&& 
		   		(*pcTokenStart == _T(' ') || *pcTokenStart == _T('\t') 
		    	|| *pcTokenStart == cMkContinuation || *pcTokenStart == _T('\r') 
		    	|| *pcTokenStart == _T('\n')
		    	) 
		   	  )
		{
			if (*pcTokenStart == _T('\r') || *pcTokenStart == _T('\n'))	
			{
				m_nCharCount = 0;
			}
			else m_nCharCount++;
			pcTokenStart++;
		}

		nTokenLen = GetMakToken	(pcTokenStart, pEndOfToken);
		if ( m_nCharCount + nTokenLen >= nLineLen )
		{
			// split line only if it doen't end with a return already -- fix 11534
			if ((m_nCharCount > 0) && (*pEndOfToken != _T('\r'))) SplitLine ();
		   	// If the token's too big for a line, write it anyway:
			m_nCharCount = nTokenLen;
		}
		else m_nCharCount += nTokenLen;
		
		WriteBytes ( (BYTE *) pc, pEndOfToken - pc);
		if (!fLastNotBackWhack)
			WriteBytes ( (BYTE *) _TEXT ( " "), sizeof (TCHAR));

		pc = pEndOfToken;
	}
}
//////////////////////////////////////////////////////////////////////
// CMakFileWriter::EndLine
void CMakFileWriter::EndLine ()
{
	WriteBytes ( (BYTE *) _TEXT ( "\r\n"), sizeof (TCHAR) * 2); 
	m_bLastLineBlank = (m_nCharCount == 0);
	m_nCharCount = 0;
}
//////////////////////////////////////////////////////////////////////
void CMakFileWriter::InsureSpace ()
{
	if (!m_bLastLineBlank) EndLine ();
}
//////////////////////////////////////////////////////////////////////
// CMakFileWriter::SplitLine
void CMakFileWriter::SplitLine ()
{
	WriteBytes ( (BYTE *) _TEXT ( "\\\r\n"), sizeof (TCHAR) * 3); 
	m_nCharCount = 0;
}
//////////////////////////////////////////////////////////////////////
// CMakFileWriter::Indent
void CMakFileWriter::Indent ()
{
	ASSERT (m_nCharCount == 0);
	WriteBytes ( (BYTE *) szMkLineIndent, 
				sizeof (szMkLineIndent) - sizeof (TCHAR)
				); 
	m_nCharCount = sizeof (szMkLineIndent)/sizeof (TCHAR) - 1;
}
//////////////////////////////////////////////////////////////////////
// CMakFileWriter::WriteSeperatorString

void CMakFileWriter::WriteSeperatorString ()
{
	WriteBytes ( ( BYTE *) _TEXT (
					"########################################"
					"########################################"
					), sizeof (TCHAR) * 80);
	EndLine ();
}
//////////////////////////////////////////////////////////////////////
// CMakFileWriter::WriteComment

void CMakFileWriter::WriteComment(const TCHAR *pszText)
{
	CString str = pszText;
	StripTrailingWhite(str);

	if (m_nCharCount > 0) EndLine ();
	WriteBytes ( (BYTE *) _TEXT ( "# "), sizeof (TCHAR) * 2); 

 	WriteBytes ( (BYTE *)(const char *) str, str.GetLength());
	EndLine ();
	m_bLastLineBlank = FALSE;
}

void CMakFileWriter::WriteCommentEx(const TCHAR *pszFormat, ...)
{
	TCHAR szBuffer[1201];
	int nBuf;
	va_list args;
	va_start(args, pszFormat);
	
	if (m_nCharCount > 0) EndLine ();
	
	nBuf = _vsnprintf(szBuffer, 1201, pszFormat, args);
	szBuffer[1200] = 0;

	WriteBytes ( (BYTE *) _TEXT ( "# "), sizeof (TCHAR) * 2); 
 	WriteBytes ( (BYTE *) szBuffer, nBuf );
	EndLine ();
	m_bLastLineBlank = FALSE;
}
//////////////////////////////////////////////////////////////////////
// CMakFileWriter::WriteDirective
// everyone who calls WriteDirective with something that needs to be
// translated should use this version (not the char * version.
void CMakFileWriter::WriteDirective(CMakDirective::DTYP dtyp, CString &str )
{
	if (IsMakefile() && ((dtyp == CMakDirective::DTYP_MESSAGE) || (dtyp == CMakDirective::DTYP_ERROR)))
	{
		str.AnsiToOem();
	}

	WriteDirective( dtyp, (const TCHAR *)str );
}

void CMakFileWriter::WriteDirective(CMakDirective::DTYP dtyp, const TCHAR *pszText)
{
	ASSERT (dtyp >= 0 && dtyp < CDIRECTIVES );
	ASSERT (g_rgdirtbl[dtyp].dtyp == dtyp );  // Has enum been changed?
	ASSERT (g_rgdirtbl[dtyp].nByteLen + 4 < nLineLen); 

	if (m_nCharCount > 0) EndLine ();
	WriteBytes ( (BYTE *) _TEXT ( "!" ), sizeof ( TCHAR ));
	WriteBytes ( (BYTE *) g_rgdirtbl[dtyp].szDirKeyword, 
										g_rgdirtbl[dtyp].nByteLen );

	WriteBytes ( (BYTE *) _TEXT ( " " ), sizeof ( TCHAR ));

	m_nCharCount = g_rgdirtbl[dtyp].nByteLen + 2;

	if (pszText)
		WriteString(pszText);
	EndLine ();

}
//////////////////////////////////////////////////////////////////////
// CMakFileWriter::WriteMacro
void CMakFileWriter::WriteMacro(const TCHAR *pszLeftSide, const TCHAR *pszRightSide)
{
	// ignore 'null' macro names!
	if (pszLeftSide[0] != _T('\0'))
	{
		if (m_nCharCount > 0) EndLine();

		WriteString(pszLeftSide);
		WriteString(_TEXT ( "=" ));
		WriteString(pszRightSide);

		EndLine();
	}
}
//////////////////////////////////////////////////////////////////////
// CMakFileWriter::WriteDesc
void CMakFileWriter::WriteDesc(const TCHAR * pszTargets, const TCHAR * pszDependants)
{
	// ignore 'null' targets names!
	if (pszTargets[0] != _T('\0'))
	{
		if (m_nCharCount > 0) EndLine();
		InsureSpace ();

		WriteString(pszTargets);
		WriteString(_TEXT( " : " ));
		WriteString(pszDependants);

		EndLine ();
	}
}

//////////////////////////////////////////////////////////////////////
// CMakFileWriter::WriteDescAndAction
void CMakFileWriter::WriteDescAndAction
(
	const TCHAR * pszTargets,
	const TCHAR * pszDependants,
	const TCHAR * pszAction
)
{
	// multi-targets?
	// look for ','
	TCHAR * pch = (TCHAR *)pszTargets;
	BOOL fMultiTarg = FALSE;
	BOOL fMultiLine = FALSE;
	BOOL fHasAction = *pszAction != _T('\0');
	BOOL fWrittenCmds = FALSE;

	//
	// Only set fMultiTarg = TRUE if NOT writing a makefile.
	//
	if (IsProjectFile())
	{
		while (*pch != _T('\0'))
		{
			if (*pch == _T('\t'))
			{
				pch = _tcsinc(pch);
				if (*pch != _T('\0'))
					fMultiTarg = TRUE;
				break;
			}
			pch = _tcsinc(pch);
		}
	}

	pch = (TCHAR *)pszAction;
	while (*pch != _T('\0'))
	{
		if (*pch == _T('\t'))
		{
			pch = _tcsinc(pch);
			if (*pch != _T('\0'))
				fMultiLine = TRUE;
			break;
		}
		pch = _tcsinc(pch);
	}

	//
	// Just use "\t" delimted string "as is" when writing a makefile.
	//
	if (IsProjectFile())
		pch = _tcstok((TCHAR *)pszTargets, "\t");
	else
		pch = const_cast<TCHAR*>(pszTargets);

	while (pch != (TCHAR *)NULL)
	{
		// pad with line
		EndLine();

		// got multi?
		if (!fMultiTarg)
		{
			// write out desc.
			WriteDesc(pch, pszDependants);
		}

		// write out actions
		// as a $BuildCmd?

		if (fHasAction && !fWrittenCmds)
		{
			// write as a $BuildCmd?
			if (fMultiTarg)
			{
				WriteString(_TEXT("BuildCmds= \\\r\n\t"));	// end-line and indent here...
			} else {
				WriteString(_TEXT("\t"));	// add the tab
			}

			COptionList lstActions(_T('\t'), FALSE,TRUE);
			lstActions.SetString(pszAction);
			POSITION pos = lstActions.GetHeadPosition();
			while (pos != (POSITION)NULL)
			{
				const TCHAR * pcActionLine = lstActions.GetNext(pos);
				
				// write our action-line
				// if (!fMultiLine)
				//	Indent();	// indent here...
				
				WriteString(pcActionLine);

				if (fMultiLine){
					WriteString(_TEXT(" "));	// end-line and indent here...
					if (fMultiTarg){
						SplitLine();
						WriteString(_TEXT("\t"));	// end-line and indent here...
					}
					else
						WriteString(_TEXT("\r\n\t"));	// end-line and indent here...
				}
				else
					EndLine();							// end-line
			}

			fWrittenCmds = TRUE;
		}

		// got multi?
		if (fMultiTarg)
		{
			// write out desc. with $BuildCmds
			WriteDesc(pch, pszDependants);

			if (fHasAction)
			{	
				Indent();
				WriteString(_TEXT("$(BuildCmds)"));
			}
		}

		//
		// Just break from loop when writing a makefile since _tcstok is not being
		// used.
		//
		if (!IsProjectFile())
			break;
		else
			pch = _tcstok(NULL, "\t");
	}

	EndLine();
}
