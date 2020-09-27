//////////////////////////////////////////////////////////////////////
// MAKREAD.CPP
//
// Implementation of CMakFileReader
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
#include "makread.h"

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

VOID StripLeadingWhite
(
	CString &	str
)
{
	TCHAR *	pchStart;
	TCHAR *	pchCur;
	int		ich;

	pchCur = pchStart = (TCHAR *)(const TCHAR *)str;

	while ((*pchCur == _T(' ')) || (*pchCur == _T('\t')))
		++pchCur;

	ich = (int)(pchCur - pchStart);

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
	int		ichWhite;

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
		ichWhite = (int)(pchWhite - pchStart);
		str = str.Left(ichWhite);
	}
}

VOID StripLeadingAndTrailingWhite
(
	CString &	str
)
{
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
	INT_PTR nLeadingWhite = (pchCur - pchStart);
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
			}
			*(pchTemp++) = *(pchCur++);
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

	if (pchWhite!=NULL)
	{
		pchCur = pchWhite; 	// reset pchCur to actual end of string
		*pchCur = _T('\0'); // and null terminate
	}
	str.ReleaseBuffer((int)((pchCur - pchStart) - nLeadingWhite));
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
	VSASSERT (pBase, "NULL input parameter!");

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
    VSASSERT ( _tcslen (pszName) > 0, "Empty strings not allowed here!" );

	// Go for quick kill:  Have we seen the name before
	if (m_InMap.Lookup ( pszName, (void *&) pcstr))
	{
		strResult = *pcstr;
		return;
	}

	i = lstrlen (pszName);
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
			*pcur++ = (TCHAR)(*pszName -  (_T('a') - _T('A')));	// Upcase
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

		i = (int)m_InMap.GetCount();
		do 
		{
			_ultoa ( (unsigned long)i, pcur, 16);
			i++;
		} while (m_ResMap.Lookup ( pbase, pdum));
	}

	strResult.ReleaseBuffer ();

	VSASSERT (!m_ResMap.Lookup ( pbase, pdum), "Failed to find what we are looking for!");

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

#define CDirECTIVES 13

struct
{
	TCHAR *					szDirKeyword;
	int						nByteLen;
	CMakDirective::DTYP		dtyp;
}

#define DIRTEXT(x) _TEXT(x), (sizeof (x)-1) 

//  Note: in this table, longer items must come before items with
//  the same prefix (e.g. IFDEF before IF):

g_rgdirtbl[CDirECTIVES] =
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

CMakDirective::~CMakDirective(){}
CMakDirective::CMakDirective
(
	DTYP			dtyp,
	const CString & strRemOfLine
)
{
	m_dtyp = dtyp;
	m_strRemOfLine = strRemOfLine;
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
}

CMakFileReader::~CMakFileReader()
{
	if (m_pUngotElement) delete (m_pUngotElement);
	if (m_nFileSize && (m_iAttrib & MakRW_ShowStatus))
	{
//		::StatusPercentDone(100);	// Let the user see this.
//		::StatusEndPercentDone();
//		::SetPrompt();
	}
}

//////////////////////////////////////////////////////////////////////
// CMakFileReader::Abort
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
	UINT	cch = 0;
	char buf[100000];

	// NOTE: command lines longer than 1 Meg are absurd.
	char * pszRet = buf;

	while (((ch = GetChar()) != _T('\r')) && ((ch != MAK_EOF) || (!m_fEOF)))
	{
		pszRet[cch++] = ch;
		if (cch >= 100000)
		{
			ch = _T('\r');
			break;
		}
	}

	pszRet[cch] = _T('\0'); // ensure nul-terminated

	strRet = buf;	// set data length, and terminate
	
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
	INT_PTR			idir, cch = 0;
	CString			strRemOfLine, strT;

	VSASSERT(strLine[0] == _T('!'), "Directives must have an '!' in the first column!");

	pch = (const TCHAR *)strLine + 1;

	/* Skip any whitespace following the '!'.
	*/

	SkipWhite(pch);

	if (*pch == _T('\0'))
		return(new CMakError);

	/* Now search for a matching keyword.
	*/

	for (idir=0 ; idir<CDirECTIVES ; ++idir)
		if (_tcsnicmp(	g_rgdirtbl[idir].szDirKeyword,
						pch,
						cch = _tcslen(g_rgdirtbl[idir].szDirKeyword)) == 0)
			break;

	if (idir == CDirECTIVES)
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
	int				ichEq
)
{
	// UNDONE: requires further optimization for buffer allocation

	int	len;
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
	int				ichColon,
	BOOL            fHaveDependencies
)
{
	TCHAR			ch;
	const TCHAR *	pch;
	BOOL			fDoubleColon = FALSE, fFirstLine;
	int				ich, ich1;
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

	if	(fHaveDependencies && (ichColon != strLine.GetLength() - 1)
		&& (strLine[ichColon + 1] == _T(':')))
		fDoubleColon = TRUE;

	if (fHaveDependencies)
	{
		strLHS = strLine.Left(ichColon);
		StripLeadingAndTrailingWhite(strLHS); // used for unique matches
		strRHS += strLine.Right(strLine.GetLength() - ichColon - (fDoubleColon ? 2 : 1));
	}
	else
	{
		strLHS.Empty();
		strRHS = strLine;
	}

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

		if (!fFirstLine || fHaveDependencies)
		{
			/* Now fetch the next line -- but don't do it if this is the first line of
			** of something with no dependencies
			*/

			if (!GetLine(strRHS))
				break;

			strRawData += strRHS;
			strRawData += _T("\r\n");
		}

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
				VSASSERT (m_nFileSize == 0, "Something bad happened while we were trying to determine the file length!");
			}
			END_CATCH_ALL	

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
	BOOL fMustHaveOutputs /* = TRUE */
)
{
	const TCHAR *		pch;
	const TCHAR *		pchT;
	BOOL				fInQuotes;
	CString				strLine;
	BOOL                bLineWithWhiteSpace = FALSE;

	// Return the ungot element if there is one:
	if (m_pUngotElement)
	{
		ASSERT_VALID (m_pUngotElement);

		CObject *pUngotElement = m_pUngotElement;
		m_pUngotElement = NULL;
		return pUngotElement;
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
	 * Exception: build events go straight into the description block without
	 * any outputs to check for
	*/

	if (pch != (const TCHAR *)strLine)
	{
		if (fMustHaveOutputs)
			return(new CMakError);
		else
			bLineWithWhiteSpace = TRUE;
	}

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

	if (bLineWithWhiteSpace)
	{
		return ParseDescBlk(strLine, 0, fMustHaveOutputs);
	}
	else if (*pch == _T(':'))
	{
		return(ParseDescBlk(strLine, (int)(pch - (const TCHAR *)strLine), fMustHaveOutputs));
	}
	else if (*pch == _T('='))
	{
		return(ParseMacro(strLine, (int)(pch - (const TCHAR *)strLine)));
	}
	else
		return(new CMakError);
}

void CMakFileReader::UngetElement (CObject *pUngotElement)
{
	// Only single depth:
	ASSERT_VALID (pUngotElement);
	VSASSERT (m_pUngotElement == NULL, "Trying to unget more than one element at a time!" );	
	m_pUngotElement = pUngotElement;
}
//////////////////////////////////////////////////////////////////////
