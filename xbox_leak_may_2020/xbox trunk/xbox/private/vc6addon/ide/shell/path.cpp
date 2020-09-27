//////////////////////////////////////////////////////////////////////
// PATH.CPP
//
// Implementation of CPath and CDir objects.
//
// History
// =======
// Date			Who			What
// ----			---			----
// 07-May-93	mattg		Created
// 12-May-93	danw		Add operator = and GetDisplayName.
// 19-May-93	danw		Fixed GetExtension to return after the '.'.
// 20-May-93	mattg		Added CDir object
//							GetExtension now again returns the '.'
//							PATH.H comment fixed)
// 11-Jul-93	mattg		New methods for CPath and CDir
//							Also "TCHAR'ified"
// 20-Jul-93    danw        Added relativization functions.
// 22-Nov-93	mattg		Fixed bug in CPath::CreateFromDirAndFilename
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "path.h"
#ifndef _WIN32
#include "direct.h"
#include "ctype.h"
#endif

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#ifndef _WIN32
#define MAX_PATH _MAX_PATH
#endif

static TCHAR *StripQuotes(LPCTSTR szFilePath);

//////////////////////////////////////////////////////////////////////
// size_t RemoveNewlines(_TCHAR * buffer);  -- helper function
//
// Scans a string in place, replacing any internal newlines with ' '
// and removing '\r' and any other control characters.
//
// Returns the number of _TCHAR's in the modified string
//
// DBCS-safe
//
// CONSIDER: find a better place for this function
//
size_t RemoveNewlines(_TCHAR * buffer)
{
	_TCHAR * r = buffer;
	_TCHAR * w = buffer;
	while (*r)
	{
		if (*r < (unsigned int)(unsigned char)' ')
		{
			// replace internal newlines; strip other control characters
			if ((*r == _T('\n')) && (*(r+1)!=_T('\0')))
			{
				*w++ = _T(' '); // replace newlines with spaces
			}
			r++;	// just ignore other control characters

		}
		else
		{
			// *w++ = *r++; DBCS-safe copy for normal characters
			_tccpy(w,r);
			w = _tcsinc(w);
			r = _tcsinc(r);
		}
	}
	*w = *r;	// copy final nul
	return((size_t)(w-buffer));
}


//////////////////////////////////////////////////////////////////////
// Scan a path in see if it contains special charaters that would
// required it to be quoted.

// Really, we're doing this mostly for nmake's benifit, so we're talking
// about nmakes' reserved characters which are:
// 			:  ;  #  (  )  $  ^  \  {  }  !  @
// We ignore ':' since it will only appear embedded in a filename nmake
// doesn't care about this case.
//
BOOL ScanPathForSpecialCharacters (const TCHAR *pPath)
{
	while (*pPath)
	{
		if (!_istalnum (*pPath)
			&&
			*pPath != _T ('.')
			&&
			*pPath != _T ('_')
			&&
			*pPath != _T ('~')
			&&
			*pPath != _T ('\\')
			&&
			*pPath != _T ('/')
			&&
			*pPath != _T (':')
			) return TRUE;
		pPath = _tcsinc ( pPath );
	}
	return FALSE;
}

//////////////////////////////////////////////////////////////////////
// Empty string used by construct element functions:
const CString AFX_EXT_DATADEF pthEmptyString;

LPCTSTR GetLastSlash(LPCTSTR szPath)
{
	LPCTSTR pSlash = _tcsrchr(szPath, _T('\\'));
	if ((pSlash != NULL) && (*pSlash==0))
		pSlash = NULL;

	return pSlash;
}

//////////////////////////////////////////////////////////////////////
// Constructors, destructors

IMPLEMENT_DYNAMIC(CPath, CObject)

CPath::~CPath
(
)
{
}

//////////////////////////////////////////////////////////////////////
// CPath::Create

BOOL CPath::Create
(
	const TCHAR *	szFileName
)
{
	TCHAR	rgchPath[_MAX_PATH];

	// check for unbalanced quotes--means that quote is being used as path char
	TCHAR *pch = _tcschr(szFileName, _T('\"'));
	if ((pch != NULL) && (pch == _tcsrchr(szFileName, _T('\"'))))
		return FALSE;
	
	// Strip any quotes from the filename - private copy. Must be freed
	TCHAR *szFilename = StripQuotes(szFileName);
		
	// First we may need to clean up the szFilename that was passed to us
	// Possible bad scenarios are
	// (1) <whitespace><drive>:<rest of path>	(need to strip whitespace)
 	// (2) paths containing multiple :'s		(bogus filenames)
 
	TCHAR * pStart = (TCHAR *)szFilename;
 	TCHAR * pColon = _tcschr(pStart, _T(':'));
	if (pColon)
 	{
		if (_tcschr(_tcsinc(pColon), _T(':')))
		{
			delete [] szFilename;
			return FALSE;	// Multiple colons
		}

		if (pColon > pStart)
			pStart = _tcsdec(pStart, pColon);	// Drive letters must be single byte chars
		else if (pColon == pStart)
		{
			delete [] szFilename;
			return FALSE;	// filename starts with colon
		}
	}

#ifdef _WIN32
	TCHAR *	pchFilePart;
	int nPathLen = GetFullPathName(pStart, sizeof(rgchPath), rgchPath, 
		&pchFilePart);
	if( (nPathLen <= 0) || (nPathLen > sizeof(rgchPath)) )
#else
	if (_fullpath(rgchPath, pStart, sizeof(rgchPath)) == NULL)
#endif
	{
		delete [] szFilename;
		return FALSE;
	}

	// Check for some weird cases that GetFullPathName() lets slip through.
	// The following resulting full paths are all invalid:
	//
	// (1) <drive>:\			(no filename)
	// (2) <drive>:\<dirs>\		(no filename)
	// (3) \\					(no servername, sharename or filename)
	// (4) \\<server>			(no sharename or filename)
	// (5) \\<server>\<share>	(no filename)
	// (6) \\<server>\<share>\	(no filename)
	// (7) \\\<filename>
	//
	// Cases (1), (2) and (6) can be caught by checking to see if the last
	// character is "\".  Cases (3), (4) and (5) can be caught by checking
	// to see if the first two characters are both "\", and if so, verifying
	// that there are at least two other "\" characters in the pathname (case
	// (6) does have two other "\" characters, but it will have been caught
	// earlier).
	//
	// I don't know if this behavior of NT is a bug or a feature.

	int ichPathStart = 0;
	LPCTSTR pSlash;
	pStart = rgchPath;
	if (*rgchPath==_T('\\'))
	{
		ASSERT(*(rgchPath+1)==_T('\\'));	// must be UNC path
		pStart += 2;
	}

	if (((pSlash = GetLastSlash(pStart))==NULL) || (*(pSlash+1)==0) || (pSlash==pStart))
	{
		// This covers cases (1), (2), (3), (4), (6) and (7) above, as well as
		// ensuring that there is at least one "\".
		delete [] szFilename;
		return FALSE;
	}

	// Check for case (5)
	if ((*rgchPath==_T('\\')) && (_tcschr(pStart, _T('\\'))==pSlash))
	{
		delete [] szFilename;
		return FALSE;
	}

	// REVIEW: others?
	// also reject illegal characters "<>|" 
	if (_tcspbrk(pStart, _T("<>|")))
	{
		delete [] szFilename;
		return FALSE;
	}

	// now validate that full path length and component lengths are valid
	if ((_tcslen(rgchPath) >= _MAX_PATH) || (_tcslen(pchFilePart) >= _MAX_FNAME))
	{
		delete [] szFilename;
		return FALSE;
	}
	
	m_ichLastSlash = (int)(pSlash - rgchPath);
	m_strCanon = rgchPath;

	delete [] szFilename;
	m_Flags &= ~eIsActualCase;
	return TRUE;
}

//////////////////////////////////////////////////////////////////////
// CPath::CreateFromDirAndFilename

BOOL CPath::CreateFromDirAndFilename
(
	const CDir &	dir,
	const TCHAR *	szFileName
)
{
	// Strip quotes from the filename
	TCHAR* szFilename = StripQuotes(szFileName);
	TCHAR* szAllocName = szFilename;
	BOOL fRet;

	// The basic idea is to simply append the filename to the directory
	// specification and pass this to the Create() method.  HOWEVER, if
	// the filename specifies a full path, just pass that instead.

	if (*szFilename && *_tcsinc(szFilename) == _T(':'))
	{
		// 'szFilename' has a drive letter.  Is a different drive from 'dir'?

		if (dir.IsUNC() || _totupper(*szFilename) != _totupper(*dir))
		{
			// Different drive, so just call Create, which will use
			// the current directory of the drive specified in
			// szFilename.

			fRet = Create(szFilename);
			delete [] szAllocName;
			return fRet;
		}

		// Same drive: skip over drive letter and colon.
		ASSERT(!_ismbblead(*szFilename));
		szFilename += 2;
	}

	if	(
		((*szFilename == _T('/')) || (*szFilename == _T('\\')))
		&&
		((*(szFilename + 1) == _T('/')) || (*(szFilename + 1) == _T('\\')))
		)
	{
		fRet = Create(szFilename);
		delete [] szAllocName;
		return fRet;
	}
	else if ((*szFilename == _T('/')) || (*szFilename == _T('\\')))
	{
		CString	strT;

		// Almost a full path -- we just need the volume name.  Extract
		// the volume from the specified directory.

		if (dir.IsUNC())
		{
			const	TCHAR *	pch = (const TCHAR *)dir;
					int		nCountBackslash = 0, nBytes;

			// Copy characters from the directory name until we
			// hit a fourth '\' or a terminating nul.
			//
			// \\server\share\dir
			// ^^      ^     ^
			// 12      3     4
			//
			// \\server\share
			//               ^ terminating nul

			while (nCountBackslash < 4 && *pch != _T('\0'))
			{
				if (*pch == _T('\\'))
					++nCountBackslash;

				nBytes = _tclen(pch);

				strT += *pch++;

				if (nBytes == 2)
					strT += *pch++;
			}

			if (nCountBackslash != 4)
				strT += _T('\\');

			strT += szFilename;

			fRet = Create(strT);
			delete [] szAllocName;
			return fRet;
		}
		else
		{
			ASSERT(*((const TCHAR *)dir + 1) == _T(':'));

			strT += *(const TCHAR *)dir;	// Extract drive letter
			strT += _T(':');
			strT += szFilename;

			fRet = Create(strT);
			delete [] szAllocName;
			return fRet;
		}
	}
	else
	{
		CString	strT;

		strT = dir.m_strDir;

		// The length of the 'dir' string is guaranteed to be 3 if and only if
		// it represents the root of a volume (e.g., "C:\").  We need to append
		// a backslash UNLESS the directory represents the root of a volume.

		if (strT.GetLength() != 3)
			strT += _TEXT("\\");

		strT += szFilename;

		fRet = Create(strT);
		delete [] szAllocName;
		return fRet;
	}
}

//////////////////////////////////////////////////////////////////////
// CPath::CreateTemporaryName

BOOL CPath::CreateTemporaryName
(
	const CDir &	dir,
	BOOL			fKeep
)
{
	TCHAR	rgchT[MAX_PATH];

#ifdef _WIN32
	if (!GetTempFileName((const TCHAR *)dir, _TEXT("MVC"), 0, rgchT))
		return(FALSE);
#else
	TCHAR* pchT;
	if ((pchT = _tempnam((TCHAR *)(const TCHAR *)dir, _TEXT("MVC"))) == NULL)
		return(FALSE);

	_tcscpy(rgchT, pchT);
	free(pchT);
#endif

	// At this point, GetTempFileName() has created the file on disk.  If
	// fKeep is FALSE, remove that file.

	if (!fKeep)
#ifdef _WIN32
		VERIFY(DeleteFile(rgchT));
#else
		remove(rgchT);
#endif

	return(Create(rgchT));
}

//////////////////////////////////////////////////////////////////////
// CPath::PostFixNumber

VOID CPath::PostFixNumber()
{
	CString strBaseName, strNumber;

	// Strip off any existing digits on the
	// end of the base name of the path
	GetBaseNameString(strBaseName);
	int nLength = strBaseName.GetLength();
	int nPos = nLength - 1;
	while (nPos > 0 && _istdigit(strBaseName[nPos]))
	{
		strNumber = strBaseName[nPos] + strNumber;
		nPos--;
	}

	// If we didn't have a number then initialize the number
	// to be zero (remember that we will increment it)
	if (strNumber.IsEmpty())
		strNumber = _T('0');

	ASSERT(strNumber.GetLength() < 10);	// We only support up to 10 digits in the number!!!!

	// Increment the number
	int nNumber = _ttoi(strNumber);
	nNumber++;
	LPTSTR lpszNumber = strNumber.GetBuffer(10);
	_itot(nNumber, lpszNumber, 10);
	strNumber.ReleaseBuffer();

	// Construct the new file name
	CString strNewFileName = strBaseName.Left(nPos + 1) + strNumber + GetExtension();

	// Change the file name
	ChangeFileName(strNewFileName);
}

//////////////////////////////////////////////////////////////////////
// CPath::GetBaseNameString

VOID CPath::GetBaseNameString
(
	CString &	strResult
)
const
{
	const TCHAR *	pchFileName;
	const TCHAR *	pchExt;

	ASSERT(IsInit());
	pchFileName = GetFileName();
	pchExt = GetExtension();

	strResult = pchFileName;
	strResult = strResult.Left(pchExt - pchFileName);
}

//////////////////////////////////////////////////////////////////////
// CPath::GetDisplayNameString

VOID CPath::GetDisplayNameString
(
	CString &	strResult,
	int			cchMax,
	BOOL bTakeAllAsDefault
)
const
{
			int		i, cchFullPath, cchFileName, cchVolName;
	const	TCHAR *	pchCur;
	const	TCHAR *	pchBase;
	const	TCHAR *	pchFileName;

	// Following is the desired behavior.  Take as an example the full pathname
	// C:\SUSHI\VCPP32\C\MEMWIN.C.
	//
	// cchMax	strResult
	// ------	---------
	//  1- 7	<empty>
	//  8-14	MEMWIN.C
	// 15-16	C:\...\MEMWIN.C
	// 17-23	C:\...\C\MEMWIN.C
	// 24-25	C:\...\VCPP32\C\MEMWIN.C
	// 26+		C:\SUSHI\VCPP32\C\MEMWIN.C

	ASSERT(IsInit());
	cchFullPath = m_strCanon.GetLength();
	cchFileName = _tcslen(pchFileName = GetFileName());

	// If cchMax is more than enough to hold the full path name, we're done.
	// This is probably a pretty common case, so we'll put it first.

	if (bTakeAllAsDefault || cchMax >= cchFullPath)
	{
		strResult = m_strCanon;
		return;
	}

	// If cchMax isn't enough to hold at least the basename, we're done (result
	// is empty string).

	if (cchMax < cchFileName)
	{
		strResult.Empty();
		return;
	}

	// Calculate the length of the volume name.  Normally, this is two
	// characters (e.g., "C:", "D:", etc.), but for a UNC name, it could
	// be more (e.g., "\\server\share").
	//
	// If cchMax isn't enough to hold at least <volume_name>\...\<base_name>,
	// the result is the base filename.

	pchBase = (const TCHAR *)m_strCanon;
	pchCur = pchBase + 2;					// Skip "C:" or leading "\\"

	if (IsUNC())
	{
		ASSERT((m_strCanon[0] == _T('\\')) && (m_strCanon[1] == _T('\\')));

		// First skip to the '\' between the server name and the share name,
		// then skip to the '\' after the share name.

		for (i=0 ; i<2 ; ++i)
		{
			while (*pchCur != _T('\\'))
			{
				pchCur = _tcsinc((TCHAR *)pchCur);
				ASSERT(*pchCur != _T('\0'));
			}

			if (i == 0)
				pchCur = _tcsinc((TCHAR *)pchCur);
		}
	}

	ASSERT(*pchCur == _T('\\'));

	cchVolName = pchCur - pchBase;

	if (cchMax < cchVolName + 5 + cchFileName)
	{
		strResult = pchFileName;
		return;
	}

	// Now loop through the remaining directory components until something
	// of the form <volume_name>\...\<one_or_more_dirs>\<base_name> fits.
	//
	// Assert that the whole filename doesn't fit -- we should have handled
	// this earlier.

	ASSERT(cchVolName + (int)_tcslen(pchCur) > cchMax);

	while (cchVolName + 4 + (int)_tcslen(pchCur) > cchMax)
	{
		do
		{
			pchCur = _tcsinc((TCHAR *)pchCur);
			ASSERT(*pchCur != _T('\0'));
		}
		while (*pchCur != _T('\\'));
	}

	// Form the resultant string and we're done.

	strResult = m_strCanon.Left(cchVolName) + _T("\\...") + pchCur;
}

//////////////////////////////////////////////////////////////////////
// CPath::operator ==

int CPath::operator ==
(
	const CPath &		pathCmp
)
const
{
	return (m_strCanon.CompareNoCase(pathCmp.m_strCanon) == 0);
}

//////////////////////////////////////////////////////////////////////
// CPath::ChangeFileName

VOID CPath::ChangeFileName
(
	const TCHAR *	szNewFileName
)
{
	TCHAR *	pchCanon;
	int		ichOldFileName;

	ASSERT(IsInit());
	ichOldFileName = GetFileName() - (const TCHAR *)m_strCanon;

	pchCanon = m_strCanon.GetBuffer(m_strCanon.GetLength() +
		_tcslen(szNewFileName));

	lstrcpy(pchCanon + ichOldFileName, szNewFileName);

	m_strCanon.ReleaseBuffer(-1);

}

//////////////////////////////////////////////////////////////////////
// CPath::ChangeExtension

VOID CPath::ChangeExtension
(
	const TCHAR *	szNewExt
)
{
	TCHAR *	pchCanon;
	TCHAR *	szFileName;
	int		ichOldExt;
	CString	strExt;

	ASSERT(IsInit());

	// Attempt to do something clever here.  Make a copy of the filename
	// and see if it's:
	//
	// (1) UPPERCASE
	// (2) MixedCase
	// (3) lowercase
	//
	// (1): Make the extension uppercase.
	// (2): Make the extension's first letter uppercase, and the remaining
	//      letters lowercase.
	// (3): Make the extension lowercase.
	szFileName = (TCHAR *)GetFileName();
	strExt = GetExtensionCase( szFileName, szNewExt );

	// Find the current extension, if any.  Convert this to a character offset
	// since the CString's buffer could move when we GetBuffer() it.
	ichOldExt = GetExtension() - (const TCHAR *)m_strCanon;
	pchCanon = m_strCanon.GetBuffer(m_strCanon.GetLength() + strExt.GetLength() + 1);
	pchCanon[ichOldExt] = _T('.');

	lstrcpy( pchCanon + ichOldExt + 1,
			strExt[0] == _T('.') ? (const TCHAR *)strExt + 1 :
			(const TCHAR *)strExt);

	m_strCanon.ReleaseBuffer(-1);
}

//////////////////////////////////////////////////////////////////////
// CPath::GetRelativeName
#define _tcicmp(x,y) _tcsnicmp((x),(y),1)

BOOL CPath::GetRelativeName
(
	const CDir &rDir,
	CString& strResult,
	BOOL bQuote /*= FALSE*/,
	BOOL bIgnoreAlwaysRelative /* = FALSE */
) const
{
	// Plan here is simple:  Find how many of the leading characters
	// match in the two m_strCanon and rDir.  In order to be able to
	// relativize, this must be all of rDir:

	const TCHAR *pcDir, *pcPt;
	TCHAR resPath[_MAX_PATH+3], *presPath;
	BOOL bRetval = FALSE;
	int nSlashCount;

	pcDir = rDir;
	pcPt = m_strCanon;
	presPath = resPath;

	ASSERT(IsInit());

	// Quote really only means quote if needed:
	if (bQuote) bQuote = ContainsSpecialCharacters () ||
							rDir.ContainsSpecialCharacters ();

	ASSERT ( _tcslen ( pcPt) <= _MAX_PATH);
	if (bQuote) *presPath++ = _T('"');

	int rDirLen = rDir.m_strDir.GetLength();
	if (!_tcsnicmp(pcDir, pcPt, rDirLen))
	{
		pcPt += rDirLen;
		// Now, the directory may or may not end with a backslash.  If
		// it does not then the next charcter in the path must be a backslash:
		if ((*pcPt == _T('\\')) || (*_tcsdec(pcDir, pcDir+rDirLen) == _T('\\')))
		{
			// the following is DBCS safe because we already know it's a '\\'
			if (*pcPt != _T('\\')) pcPt--; // back up to the slash if necessary

			*presPath++ = _T('.');	// insert '.' before '\\'
			while ( *presPath++ = *pcPt++ );
			bRetval = TRUE;
		}

	}
	if (!bRetval)
	{

		// See if our path begins with a drive letter
		// colon and backslash.  If it does, see if the first three characters
		// match the reference directory.  If they do, then the relative path
		// just our path minus the drive letter:
		pcDir = rDir;
		pcPt = m_strCanon;
		if  (
			(!_ismbblead(*pcPt))
			&&
			*(pcPt + 1) == _T(':') && *(pcPt + 2) == _T('\\')
			&&
			(!_tcsnicmp(pcPt, pcDir, 3))
			)

		{
			pcPt += 2;							// Skip over drive letters.
			if (GetAlwaysRelative() && !bIgnoreAlwaysRelative)
			{
				// if requested, attempt full relativization
				pcDir +=3;
				int len;
				const TCHAR * pcTemp = ++pcPt;
				while (((pcTemp = _tcschr(pcPt, _T('\\')))!=NULL) &&
						(!_tcsnicmp(pcPt, pcDir, (len = (pcTemp+1)-pcPt))))
				{
					pcPt = (pcTemp+1);
					pcDir += len;
				}

				// removed common part, now add appropriate # of "..\"
				do
				{
					*presPath++ = _T('.');
					*presPath++ = _T('.');
					*presPath++ = _T('\\');
				}
				while (((pcDir = _tcschr(pcDir, _T('\\')))!=NULL) && (*++pcDir));
				// now just append the rest
			}

			while ( *presPath++ = *pcPt++ );	// Copy
			bRetval = TRUE;
		}
		// See if these are UNC paths.  Then check if they have the same
		// server and share names:
		else if ( *pcPt++ == _T('\\') && *pcPt++ == _T('\\')
				  && *pcDir++ == _T('\\') && *pcDir++ == _T('\\') )
		{
			// Go until second \, or end of string:
			nSlashCount = 0;
			while ( *pcDir && *pcPt &&  _tcicmp (pcDir, pcPt)==0)
			{
				if (*pcDir == _T ('\\'))
					if (++nSlashCount > 1 ) break;
				pcDir = _tcsinc ( (TCHAR *) pcDir);
				pcPt  = _tcsinc ( (TCHAR *) pcPt);
			}

			if ( nSlashCount == 2
				||
				(*pcDir == 0 && *pcPt == _T ('\\') && nSlashCount == 1)
				)
			{
				if (*pcPt != _T ('\\')) *presPath++ = _T ('\\');
				while ( *presPath++ = *pcPt++ );
				bRetval = TRUE;
			}
		}
		if (bRetval == FALSE) // Failure!
		{
			pcPt = m_strCanon;
			while ( *presPath++ = *pcPt++ );
		}
	}

	if (bQuote)
	{
		ASSERT ( *(presPath-1) == 0 );
		*(presPath-1) = _T('"');
		*presPath = _T('\0');
	}
	strResult = resPath;
	return bRetval;
}
//////////////////////////////////////////////////////////////////////
// CPath::CreateFromDirAndRelative
BOOL CPath::CreateFromDirAndRelative
(
	const CDir &rDir,
	const TCHAR *pstr
)
{
	// THIS FUNCTION IS OBSOLETE.  The only thing it does that
	// CreateFromDirAndFilename doesn't do is, this function removes
	// quotes from around the relative path (if present).

	if (*pstr == _T('"'))
	{
		int cch = _tcslen(pstr);

		// MARTYNL: Badly formed string was passed in, probably caused by bad parsing at the outer layer
		if(pstr[cch-1] != '"')
		{
			return FALSE;
		}

		CString str(pstr+1, cch-2);
		return CreateFromDirAndFilename(rDir, str);
	}
	else
	{
		return CreateFromDirAndFilename(rDir, pstr);
	}
}

//////////////////////////////////////////////////////////////////////
// CPath::GetActualCase()
CMapStringToString CPath::c_DirCaseMap;
void CPath::GetActualCase(BOOL bEntirePath /*= FALSE*/)
{
	ASSERT(IsInit());

	// avoid expensive call if case already set
	if (!IsActualCase())
	{
		// Let's see if we can speed this up...
		CDir tempDir;
		tempDir.CreateFromPath( *this );
		tempDir.m_strDir.MakeUpper();

		CString strActualDirCase;
		if (!c_DirCaseMap.Lookup( tempDir, strActualDirCase ))
		{
			strActualDirCase = tempDir.m_strDir;
			GetActualFileCase( strActualDirCase );
			c_DirCaseMap.SetAt( tempDir, strActualDirCase );
		}
		GetActualFileCase( m_strCanon, strActualDirCase );

		m_Flags |= eIsActualCase;
		ASSERT(IsActualCase());

		// must update in case the length has changed
		LPCTSTR pSlash = GetLastSlash(m_strCanon);
		ASSERT((pSlash!=NULL) && (*(pSlash+1)!=0));
		m_ichLastSlash = (int)(pSlash - (LPCTSTR)m_strCanon);
	}
}

//////////////////////////////////////////////////////////////////////
// CPath::SetActualCase(LPCTSTR pszFileCase)
void CPath::SetActualCase(LPCTSTR pszFileCase)
{
	ASSERT(IsInit());

	// avoid expensive call if case already set
	if (!IsActualCase())
	{
		// Let's see if we can speed this up...
		CDir tempDir;
		tempDir.CreateFromPath( *this );
		tempDir.m_strDir.MakeUpper();

		CString strActualDirCase;
		if (!c_DirCaseMap.Lookup( tempDir, strActualDirCase ))
		{
			strActualDirCase = tempDir.m_strDir;
			GetActualFileCase( strActualDirCase );
			c_DirCaseMap.SetAt( tempDir, strActualDirCase );
		}

		if (pszFileCase != NULL)
		{
			// try to avoid getting the case if we already know it
			// ASSERT(stricmp(pszFileCase, GetFileName())==0);
			// ASSERT((UINT)m_strCanon.GetLength()==(UINT)(strActualDirCase.GetLength() + strlen(pszFileCase) + 1));
			m_strCanon = strActualDirCase + _T('\\');
			m_strCanon += pszFileCase;
		}
		else
		{
			GetActualFileCase( m_strCanon, strActualDirCase );
		}
		m_Flags |= eIsActualCase;
		ASSERT(IsActualCase());

		// must update in case the length has changed
		LPCTSTR pSlash = GetLastSlash(m_strCanon);
		ASSERT((pSlash!=NULL) && (*(pSlash+1)!=0));
		m_ichLastSlash = (int)(pSlash - (LPCTSTR)m_strCanon);
	}
}

//////////////////////////////////////////////////////////////////////
// CPath::ResetDirMap()
void CPath::ResetDirMap()
{
	c_DirCaseMap.RemoveAll();
}

//////////////////////////////////////////////////////////////////////
// CPath::GetFileTime()
BOOL CPath::GetFileTime(LPFILETIME lpftLastWrite)
{
	ASSERT(IsInit());

	HANDLE hFile = ::CreateFile(m_strCanon, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	BOOL bRC = ::GetFileTime(hFile, NULL, NULL, lpftLastWrite);
	CloseHandle(hFile);

	return bRC;
}

BOOL CPath::GetFileTime(CString& rstrLastWrite, DWORD dwFlags /*= DATE_SHORTDATE*/)
{
	FILETIME ft;
	if (!GetFileTime(&ft))
		return FALSE;

	CTime time(ft);
	rstrLastWrite = theApp.m_CPLReg.Format(time, DATE_ALL, dwFlags);

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////
//	Somewhat redundant global path utility functions.
//		But they are easier to use.

static void AbbreviateName(LPTSTR lpszCanon, int cchMax, BOOL bAtLeastName);

// Remove the drive and directory from a file name...
//
CString StripPath(LPCTSTR szFilePath)
{
    char szName [_MAX_FNAME + _MAX_EXT];
    char szExt [_MAX_EXT];
    _splitpath(szFilePath, NULL, NULL, szName, szExt);
    lstrcat(szName, szExt);
    return CString(szName);
}


// Remove the name part of a file path.  Return just the drive and directory.
//
CString StripName(LPCTSTR szFilePath)
{
    char szPath [_MAX_DRIVE + _MAX_DIR];
    char szDir [_MAX_DIR];
    _splitpath(szFilePath, szPath, szDir, NULL, NULL);
    lstrcat(szPath, szDir);
    return CString(szPath);
}


// Get the extension of a file path.
//
CString GetExtension(LPCTSTR szFilePath)
{
    char szExt [_MAX_EXT];
    _splitpath(szFilePath, NULL, NULL, NULL, szExt);
    return CString(szExt);
}


// Return the path to szFilePath relative to szDirectory.  (E.g. if szFilePath
// is "C:\FOO\BAR\CDR.CAR" and szDirectory is "C:\FOO", then "BAR\CDR.CAR"
// is returned.)  This will never use '..'; if szFilePath is not in szDirectory
// or a sub-directory, then szFilePath is returned unchanged.
// If szDirectory is NULL, the current directory is used.
//
CString GetRelativeName(LPCTSTR szFilePath, LPCTSTR szDirectory /*= NULL*/)
{
    CString strDir;

    if (szDirectory == NULL)
    {
        _getcwd(strDir.GetBuffer(_MAX_DIR), _MAX_DIR);
        strDir.ReleaseBuffer();
        strDir += '\\';
    } else {
		strDir = szDirectory;
		int len = strDir.GetLength();
		if( len > 0 ){
			TCHAR c = strDir[len-1];
			if(  c != _T('\\') && c != _T(':') )   // add a trailing slash unless drive letter
				strDir += '\\';
		}
	}
	szDirectory = strDir;

    int cchDirectory = lstrlen(szDirectory);
    if (_tcsnicmp(szFilePath, szDirectory, cchDirectory) == 0)
        return CString(szFilePath + cchDirectory);
    else if ( szFilePath[0] == szDirectory[0] &&
              szFilePath[1] == ':' && szDirectory[1] == ':' )    // Remove drive if same.
        return CString(szFilePath + 2);

    return CString(szFilePath);
}

//	GetDisplayName
//		Does the same thing to a file path as our MRU implemenation.

CString GetDisplayName(LPCTSTR szFilePath, int nMaxDisplayLength,
	LPCTSTR szDirectory /*= NULL*/)
{
    CString strDir;

    if ( szDirectory == NULL )
    {
        _getcwd(strDir.GetBuffer(_MAX_DIR), _MAX_DIR);
        strDir.ReleaseBuffer();
        strDir += '\\';
        szDirectory = strDir;
    }

    int cchDirectory = lstrlen(szDirectory);

	int cchFileDir = lstrlen(szFilePath) -
		(SushiGetFileTitle(szFilePath, NULL, 0)-1);
	if (cchFileDir == cchDirectory &&
			_tcsnicmp(szDirectory, szFilePath, cchDirectory) == 0)
		return CString(szFilePath + cchDirectory);
	else if (nMaxDisplayLength != -1)
	{
		CString strName;
		LPTSTR lpch = strName.GetBuffer(_MAX_PATH);

		lstrcpy(lpch, szFilePath);
		AbbreviateName(lpch, nMaxDisplayLength, TRUE);

		strName.ReleaseBuffer();

		return strName;
	}

    return CString(szFilePath);
}

/////////////////////////////////////////////////////////////////////////////
//	AbbreviateName
//		Taken from MFC\SRC\FILELIST.CPP
//
// lpszCanon = C:\MYAPP\DEBUGS\C\TESWIN.C
// cchMax	b	Result
// ------	-	---------
//  1- 7	F	<empty>
//  1- 7	T	TESWIN.C
//  8-14	x	TESWIN.C
// 15-16	x	C:\...\TESWIN.C
// 17-23	x	C:\...\C\TESWIN.C
// 24-25	x	C:\...\DEBUGS\C\TESWIN.C
// 26+		x	C:\MYAPP\DEBUGS\C\TESWIN.C
static void AbbreviateName(LPTSTR lpszCanon, int cchMax, BOOL bAtLeastName)
{
	int cchFullPath, cchFileName, cchVolName;
	const TCHAR* lpszCur;
	const TCHAR* lpszBase;
	const TCHAR* lpszFileName;

	lpszBase = lpszCanon;
	cchFullPath = lstrlen(lpszCanon);

	cchFileName = SushiGetFileTitle(lpszCanon, NULL, 0)-1;
	lpszFileName = lpszBase + (cchFullPath-cchFileName);

	// If cchMax is more than enough to hold the full path name, we're done.
	// This is probably a pretty common case, so we'll put it first.
	if (cchMax >= cchFullPath)
		return;

	// If cchMax isn't enough to hold at least the basename, we're done
	if (cchMax < cchFileName)
	{
		lstrcpy(lpszCanon, (bAtLeastName) ? lpszFileName : &afxChNil);
		return;
	}

	// Calculate the length of the volume name.  Normally, this is two
	// characters (e.g., "C:", "D:", etc.), but for a UNC name, it could
	// be more (e.g., "\\server\share").
	//
	// If cchMax isn't enough to hold at least <volume_name>\...\<base_name>,
	// the result is the base filename.

	lpszCur = lpszBase + 2;					// Skip "C:" or leading "\\"

	if (lpszBase[0] == '\\' && lpszBase[1] == '\\')	// UNC pathname
	{
		// First skip to the '\' between the server name and the share name,
		while (*lpszCur != '\\')
		{
			lpszCur = _tcsinc(lpszCur);
			ASSERT(*lpszCur != '\0');
		}

		// if a UNC get the share name, get at least one directory.  We don't need
		// to ensure a directory after c:, etc.
		ASSERT(*lpszCur == '\\');

		if (cchFullPath - cchFileName > 3)
		{
			lpszCur = _tcsinc(lpszCur);
			while (*lpszCur != '\\')
			{
				lpszCur = _tcsinc(lpszCur);
				ASSERT(*lpszCur != '\0');
			}
		}
	}

	ASSERT(*lpszCur == '\\');

	cchVolName = lpszCur - lpszBase;

	if (cchMax < cchVolName + 5 + cchFileName)
	{
		lstrcpy(lpszCanon,lpszFileName);
		return;
	}

	// Now loop through the remaining directory components until something
	// of the form <volume_name>\...\<one_or_more_dirs>\<base_name> fits.
	//
	// Assert that the whole filename doesn't fit -- this should have been
	// handled earlier.

	ASSERT(cchVolName + (int)lstrlen(lpszCur) > cchMax);

	while (cchVolName + 4 + (int)lstrlen(lpszCur) > cchMax)
	{
		do
		{
			lpszCur = _tcsinc(lpszCur);
			ASSERT(*lpszCur != '\0');
		}
		while (*lpszCur != '\\');
	}

	// Form the resultant string and we're done.
	lpszCanon[cchVolName] = '\0';
	lstrcat(lpszCanon, _T("\\..."));
	lstrcat(lpszCanon, lpszCur);
}
/////////////////////////////////////////////////////////////////////////////

BOOL FileExists(LPCTSTR szFilePath)
{
	// Use a full pathname or _access will search for the file.
	// We need to use _access instead of OpenFile() since OpenFile()
	// fails with string length > 128 characters
	LPTSTR lpszFile;
	TCHAR szBuf[_MAX_PATH];
	GetFullPathName(szFilePath, _MAX_PATH, szBuf, &lpszFile);

#if 0
	DWORD dwAttr = GetFileAttributes(szBuf);
	return (dwAttr != 0xFFFFFFFF && (dwAttr & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
	// surprisingly, this is much faster!
	WIN32_FILE_ATTRIBUTE_DATA find;

	if (!Compatible_GetFileAttributesEx( szBuf, GetFileExInfoStandard, &find))
	{
		return FALSE;
	}
	else
	{
		return ((find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
	}
#endif
}

BOOL IsFileWritable(LPCTSTR szFilePath)
{
    CFileStatus fs;
    if (!CFile::GetStatus(szFilePath, fs))
    {
        // It doesn't exist, so we'll assume it's writable...
        return TRUE;
    }

    return (fs.m_attribute & (CFile::readOnly | CFile::hidden |
        CFile::system | CFile::volume | CFile::directory)) != 0;
}

// Remember to free() the return value!
TCHAR *StripQuotes(LPCTSTR szFilePath)
{
	const TCHAR * pchLookup = szFilePath;
	TCHAR *pchSet = new TCHAR[lstrlen(szFilePath) + 1];
	TCHAR *pchRet = pchSet;

	ASSERT(pchSet != NULL);

	while (*pchLookup != _T('\0'))
	{
		if (*pchLookup != _T('"'))
 		{
			_tccpy(pchSet, pchLookup);
			pchSet = _tcsinc(pchSet);
 		}
		pchLookup = _tcsinc(pchLookup);
	}

    *pchSet = _T('\0');
	return pchRet;
}

UINT SushiGetFileTitle(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax)
{
	ASSERT(lpszTitle == NULL ||
		AfxIsValidAddress(lpszTitle, _MAX_FNAME));
	ASSERT(AfxIsValidString(lpszPathName, FALSE));

	// use a temporary to avoid bugs in GetFileTitle whne lpszTitle is NULL
	TCHAR szTemp[_MAX_PATH];
	LPTSTR lpszTemp = lpszTitle;
	if (lpszTemp == NULL)
	{
		lpszTemp = szTemp;
		nMax = (sizeof(szTemp) / sizeof(szTemp[0]));
	}
	// [Olympus 3582 - chauv]
	// GetFileTitle() doesn't return a name with extension
	// if user registers the extension to the system on Win95.
	// We have to rely on our cheap imitation to calculate this.
	//if (GetFileTitle(lpszPathName, lpszTemp, (WORD)nMax) != 0)
	{
		// when GetFileTitle fails, use cheap imitation
        lpszTemp = (LPTSTR)lpszPathName;
        for (LPCTSTR lpsz = lpszPathName; *lpsz != '\0'; lpsz = _tcsinc(lpsz))
        {
            // remember last directory/drive separator
            if (*lpsz == '\\' || *lpsz == '/' || *lpsz == ':')
                lpszTemp = _tcsinc(lpsz);
        }
        if (lpszTitle != NULL)
            lstrcpyn(lpszTitle, lpszTemp, nMax);
	}
	return lpszTitle == NULL ? lstrlen(lpszTemp)+1 : 0;
}

//////////////////////////////////////////////////////////////////////
// Constructors, destructors

IMPLEMENT_DYNAMIC(CDir, CObject)

CDir::~CDir
(
)
{
}

//////////////////////////////////////////////////////////////////////
// CDir::CreateFromCurrent

BOOL CDir::CreateFromCurrent
(
)
{
	TCHAR	rgchDir[MAX_PATH];

#ifdef _WIN32
	if (GetCurrentDirectory(sizeof(rgchDir), rgchDir) == 0)
#else
	if (_getcwd(rgchDir, sizeof(rgchDir)) == NULL)
#endif
		return(FALSE);

	m_strDir = rgchDir;

	return(TRUE);
}

//////////////////////////////////////////////////////////////////////
// CDir::CreateFromPath

BOOL CDir::CreateFromPath
(
	const CPath & path
)
{
	TCHAR * pch;
	ASSERT(path.IsInit());

	int ichLastSlash = path.m_ichLastSlash;
	ASSERT(ichLastSlash >= 2);

	pch = m_strDir.GetBuffer(ichLastSlash+2);
	memcpy(pch, (const TCHAR *)path.m_strCanon, ichLastSlash * sizeof(TCHAR));

	// If the resulting directory name is only two characters,
	// it must be <drive>:, and we should append a "\".
	if (ichLastSlash <= 2)
		pch[ichLastSlash++] = _T('\\');
	pch[ichLastSlash] = 0;
	m_strDir.ReleaseBuffer(ichLastSlash);

	return(TRUE);
}

//////////////////////////////////////////////////////////////////////
// CDir::CreateFromPath
// Warning: this is very expensive; Use CreateFromPath(const CPath&) if possible

BOOL CDir::CreateFromPath(const TCHAR *pszPath)
{
	CPath path;
	path.Create(pszPath);
	return(CreateFromPath(path));
}

//////////////////////////////////////////////////////////////////////
// CDir::CreateTemporaryName

BOOL CDir::CreateTemporaryName
(
)
{
	TCHAR	rgchT[MAX_PATH];
	TCHAR *	pch;
	int		cch;
	// To avoid a performance hit with checking the existence of this path, we do so only once per session
	static BOOL bTempPathChecked=FALSE;			// TRUE if we've established whether the temp path exists. If this is false, bTempPathValid is undefined.
	static BOOL bTempPathValid=FALSE;			// TRUE if we've established that the temp path does exist

	// If tmp is set, but points to a non-existent dir, then we return FALSE, as if tmp was unset
	if(bTempPathChecked && !bTempPathValid)
	{
		return FALSE;
	}

#ifdef _WIN32
	if (!GetTempPath(sizeof(rgchT), rgchT))
		return(FALSE);
#else
// HACK:
	_tcscpy(rgchT, "c:\\");
#endif

	// If the last character is '\' and this is NOT the root of
	// a drive (e.g., C:\ -- always 3 characters), remove the
	// last '\'.

	cch = _tcslen(rgchT);
	pch = rgchT + cch;
	pch = _tcsdec(rgchT, pch);

	if ((*pch == _T('\\')) && (cch != 3))
		*pch = _T('\0');

	m_strDir = rgchT;

	if(!bTempPathChecked)
	{
		bTempPathChecked=TRUE;

		// If the temporary path is set, ensure that it is usable
		if (!GetTempFileName((const TCHAR *)m_strDir, _TEXT("MVC"), 0, rgchT))
		{
			bTempPathValid=FALSE;
			m_strDir.Empty();
			return(FALSE);
		}
		else
		{
			VERIFY(DeleteFile(rgchT));
			bTempPathValid=TRUE;
		}
	}

	return(TRUE);
}


//////////////////////////////////////////////////////////////////////
// CDir::CreateFromStringEx - extended form of CreateFromString

BOOL CDir::CreateFromStringEx
(
	const TCHAR *	sz,
	BOOL fRootRelative	// if true, treat dir ending with colon as relative not root dir 
)
{
	TCHAR *	pch;
	CString	strT = sz;
	CPath	pathT;

	// Tack on a bogus filename, create a CPath object from the result,
	// and then return CreateFromPath().

	if (sz && *sz)
	{
		pch = (TCHAR *)sz + _tcslen(sz);
		pch = _tcsdec(sz, pch);
		if (!fRootRelative)
		{
			if ((*pch != _T('\\')) && (*pch != _T('/')))
				strT += _T('\\');

		}
		else
		{		// bobz 6/14/95 added check for : so c: would be left relative, not turned into "c:\"
			if ((*pch != _T('\\')) && (*pch != _T('/')) && (*pch != _T(':')) )
				strT += _T('\\');
		}

	}

	strT += _T("x");	// A filename!

	if (!pathT.Create(strT))
		return(FALSE);

	return(CreateFromPath(pathT));
}

//////////////////////////////////////////////////////////////////////
// CDir::IsRootDir

BOOL CDir::IsRootDir
(
)
const
{
	const	TCHAR *	pchT;

	if (IsUNC())
	{
		pchT = m_strDir;

		ASSERT((*pchT == _T('\\')) && (*(pchT + 1) == _T('\\')));

		// Find the '\' separating the server name and the share name.

		pchT = _tcschr(pchT + 2, _T('\\'));

		ASSERT(pchT != NULL);

		// If there is another '\' after the one we just found, it's
		// NOT a root directory.  Otherwise, it is.

		if (_tcschr(pchT, _T('\\')) == NULL)
			return(TRUE);
		else
			return(FALSE);
	}
	else
	{
		// If the name is exactly 3 characters, it's a root directory.
		// Otherwise, it's not.

		if (m_strDir.GetLength() == 3)
			return(TRUE);
		else
			return(FALSE);
	}
}

//////////////////////////////////////////////////////////////////////
// CDir::AppendSubdirName

VOID CDir::AppendSubdirName
(
	const TCHAR *	szSubdir
)
{
	// If the existing directory string DOESN'T end in '\', append one.

	if (m_strDir.GetLength() != 3)
		m_strDir += _T('\\');

	m_strDir += szSubdir;
}

//////////////////////////////////////////////////////////////////////
// CDir::RemoveLastSubdirName

VOID CDir::RemoveLastSubdirName
(
)
{
	const	TCHAR *		pchBase;
	const	TCHAR *		pchCur;
	const	TCHAR *		pchPrevBackslash;
			int			i;

	if (IsRootDir())
		return;

	// Find the first '\' following the volume name.

	pchBase = (const TCHAR *)m_strDir;
	pchCur = pchBase + 2;					// Skip "C:" or leading "\\"

	if (IsUNC())
	{
		ASSERT((m_strDir[0] == _T('\\')) && (m_strDir[1] == _T('\\')));

		// First skip to the '\' between the server name and the share name,
		// then skip to the '\' after the share name.

		for (i=0 ; i<2 ; ++i)
		{
			while (*pchCur != _T('\\'))
			{
				pchCur = _tcsinc((TCHAR *)pchCur);
				ASSERT(*pchCur != _T('\0'));
			}

			if (i == 0)
				pchCur = _tcsinc((TCHAR *)pchCur);
		}
	}

	ASSERT(*pchCur == _T('\\'));

	// Now look for '\' characters until we've found the last one.

	pchPrevBackslash = pchCur;

	for (;;)
	{
		do
		{
			pchCur = _tcsinc((TCHAR *)pchCur);
		}
		while ((*pchCur != _T('\\')) && (*pchCur != _T('\0')));

		if (*pchCur == _T('\\'))
			pchPrevBackslash = pchCur;
		else
			break;
	}

	// Truncate the directory name at that point (the last '\').

	ASSERT(*pchPrevBackslash == _T('\\'));

	m_strDir = m_strDir.Left(pchPrevBackslash - pchBase);

	// Finally, we may have truncated the directory to "C:".  If
	// this is the case, tack on the '\'.

	if (m_strDir.GetLength() == 2)
		m_strDir += _T('\\');
}

//////////////////////////////////////////////////////////////////////
// CDir::MakeCurrent

// Make this object the current working directory.  May fail
// if the directory no longer exists (e.g., a floppy drive).

BOOL CDir::MakeCurrent() const
{
	// Warning: Win32 SetCurrentDirectory() is "broken"--must use _chdir()!
	const TCHAR *szDir = m_strDir;
	if (*szDir && *_tcsinc(szDir) == _T(':'))
	{
		ASSERT( _tclen( szDir ) == 1 );
		UINT ch = _tcsnextc(szDir);
		ASSERT(_istalpha(ch));
		if (_chdrive(_totupper(ch) + 1 - 'A') != 0)
			return FALSE;
	}
	return (_tchdir(m_strDir) == 0);
}

//////////////////////////////////////////////////////////////////////
// CDir::operator ==

int CDir::operator ==
(
	const CDir &	dirCmp
)
const
{
	return (m_strDir.CompareNoCase(dirCmp.m_strDir) == 0);
}

//////////////////////////////////////////////////////////////////////
// CDir::MakeDirectory()
BOOL CDir::MakeDirectory (LPCTSTR lpszPathName) const
{
    CString strBuffer;
	strBuffer = lpszPathName;

    //
    // Check to see if the given directory already exists and 
    // return TRUE if successful.
    //
 
	DWORD dwAttribs = GetFileAttributes(strBuffer);
	if (dwAttribs != 0xFFFFFFFF && (dwAttribs & FILE_ATTRIBUTE_DIRECTORY))
           return TRUE;
 
    //
    // Try to create the subdirectories (if any) named in the path.
    //
    
 	TCHAR* pStart = strBuffer.GetBuffer(strBuffer.GetLength() + 1);
	TCHAR* pCurr = pStart;

    while (*pCurr != _T('\0'))
    {
        if (*pCurr == _T('\\'))
		{
            *pCurr = _T('\0');
            CreateDirectory(pStart,NULL);
            *pCurr = _T('\\');
        }
        pCurr = _tcsinc(pCurr);
    }
	CreateDirectory(pStart, NULL);
	strBuffer.ReleaseBuffer();

    //
    // Try again to see if the given directory exists and 
    // return TRUE if successful.
    //

	dwAttribs = GetFileAttributes(strBuffer);
	if (dwAttribs != 0xFFFFFFFF && (dwAttribs & FILE_ATTRIBUTE_DIRECTORY))
           return TRUE;
 
    //
    // If fopen failed again, the path name must be invalid.
    //
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//	CCurDir
//

CCurDir::CCurDir(const TCHAR* szPath, BOOL bFile /*=FALSE*/)
{
	CPath path;

	m_dir.CreateFromCurrent();

	if (szPath != NULL)
	{
		if (!bFile)
			CreateFromString(szPath);
		else if (path.Create(szPath))
			CreateFromPath(path);

		MakeCurrent();
	}
}

CCurDir::CCurDir(const CDir& dir)
	: CDir(dir)
{
	m_dir.CreateFromCurrent();
	MakeCurrent();
}

CCurDir::CCurDir()
{
	m_dir.CreateFromCurrent();
}

CCurDir::~CCurDir()
{
	m_dir.MakeCurrent();
}

// This function returns true if the path passed in is UNC.
BOOL IsUNCPath( LPCTSTR lpszPath )
{
	ASSERT( lpszPath != NULL );
	return (lpszPath[0] == _T('\\') && lpszPath[1] == _T('\\'));
}

// This function replaces all '/' characters with '\\' characters
void AdjustPathSymbols( LPTSTR lpszPath )
{
	ASSERT( lpszPath != NULL );
	PTCHAR psz = lpszPath;

	while (*psz != _T('\0'))
	{
		if (*psz == _T('/'))
			*psz = _T('\\');
		psz = _tcsinc(psz);
	}
}

// This function searches for lpszFind in lpszArray, which should be formatted
// as "<string1>\0<string2>\0<last string>\0\0"
int FindInStringArray( LPCTSTR lpszArray, LPCTSTR lpszFind )
{
	ASSERT( lpszArray != NULL && lpszFind != NULL );
	LPCTSTR psz = lpszArray;
	UINT nLen = lstrlen(lpszFind);

	while (*psz != _T('\0'))
	{
		if (_tcsnicmp(psz, lpszFind, nLen) == 0)
			return psz-lpszArray;
		psz = psz + lstrlen(psz) + 1;	
	}
	return -1;
}

// This routine returns a pointer to the character after the last '\' in the drive (or UNC)
LPTSTR SkipDrive( LPTSTR lpszPath )
{
	ASSERT( lpszPath != NULL );
	PTCHAR psz = lpszPath;

	if (IsUNCPath(lpszPath)) // this is a UNC path
	{
		// find the end of the computer/share name
		int cBkSp = 0;
		while (*psz != _T('\0'))
		{
			if (*psz == _T('\\'))
				if (++cBkSp == 4)	// psz now points to the \ after the share name
					break;
			psz = _tcsinc( psz );
		}
		if (cBkSp != 4 || *psz == _T('\0'))
			return NULL;	// invalid path
	}
	else
	{
		// find the end of the drive name (should always be in the form "C:\")
		psz = &lpszPath[2];
	}

	// psz should now point to the '\' after the share name if this is a UNC path,
	// or the '\' after the drive letter
	ASSERT( *psz == _T('\\') );

	return ++psz;
}

// On Win95, this function changes an all caps filename to Mixed case.
void BeautifyAllCaps(LPTSTR pszFilename)
{
	ASSERT(pszFilename != NULL);

	// Do nothing on NT...
	if (!theApp.m_bOnChicago)
		return;

	// Are there any lower case letters?
	LPTSTR psz = pszFilename;
	while (*psz != _T('\0') && *psz != _T('\\'))
	{
		if (_istlower(*psz))
			return;

		psz = _tcsinc(psz);
	}

	// Change all except the first to lower case.
	psz = _tcsinc(pszFilename);
	while (*psz != _T('\0') && *psz != _T('\\'))
	{
		if (_istupper(*psz))
			*psz = (TCHAR)_totlower(*psz);

		psz = _tcsinc(psz);
	}
}

// This function changes rFilename to match the case of the
// actual filename (the way it is in the directory entry).
BOOL GetActualFileCase( CString& rFilename, LPCTSTR lpszDir /*= NULL*/ )
{
#ifdef _DEBUG
	TCHAR szOldPath[MAX_PATH];
	::GetFullPathName( rFilename, MAX_PATH-1, szOldPath, NULL );
#endif

	if (rFilename.IsEmpty())
		return FALSE;

	// Get a fully qualified filename
	TCHAR szPath[MAX_PATH];
	DWORD dwLen = ::GetFullPathName( rFilename, MAX_PATH-1, szPath, NULL );
	if (dwLen == 0 || dwLen >= MAX_PATH)
		return FALSE;	// can't fix a path this long...

	if (lstrlen(szPath) < 4)	// need at least a drive and filename (shortest legal filename is "C:\A")
		return FALSE; // can't fix a path this short

	// replace all / with \ characters
	AdjustPathSymbols( szPath );

	// lpszDir can be used as a case hint; i.e. if you are checking the
	// case of several files in the same directory, you can get the case
	// of the directory, and then checking the case for each file is much
	// quicker.
	PTCHAR psz = NULL;
	BOOL bDirSet = FALSE;
	if (lpszDir != NULL)
	{
		LPCTSTR pSlash = GetLastSlash(szPath);
		ASSERT(pSlash != NULL);
		int chLen = _tcslen(lpszDir);
	// if lpszDir ends with a slash, strip this one, so we don't get
	// C:\\file_at_root.c
#ifndef _DEBUG
		if (chLen == 3)
#endif
		{
			LPCTSTR lastDirChar = _tcsdec(lpszDir, lpszDir+chLen);
			if (lastDirChar != NULL
				&& (lastDirChar[0] == '\\' || lastDirChar[0] == '/'))
			{
				ASSERT(chLen == 3);
				pSlash = _tcsinc(pSlash);
			}
		}
		CString strFile = pSlash; // save off '\' + filename

		_tcscpy( szPath, lpszDir );
		psz = szPath + chLen;
		_tcscpy(psz, strFile);
		bDirSet = TRUE;
	}

	if (!bDirSet)
	{
		psz = SkipDrive( szPath );	// skip psz to after the drive or UNC path
		if (psz == NULL)	// the path was invalid
			return FALSE;
	}


	CString strOldFileName = rFilename;
		
	// FUTURE: Don't know how to get the actual case for the computer/share names
	// used in a UNC path...

	// now fix each component of the path
	TCHAR cTemp;
	ASSERT( psz != NULL );

	cTemp = *psz;		// We build rFilename one component at a time.
	*psz = _T('\0');
	rFilename = szPath;
	*psz = cTemp;
	while (*psz != _T('\0'))
	{
		WIN32_FIND_DATA ffd;
		if ( *psz == _T('\\') )
		{
			rFilename += *psz;
			psz++;
		}

		// Get a pointer to the end of this path segment (either the next '\' or the end of the path)
		PTCHAR pszEnd = _tcschr( psz, _T('\\') );
		if (pszEnd == NULL)
			pszEnd = psz + lstrlen(psz);
		ASSERT( *pszEnd == _T('\\') || *pszEnd == _T('\0') );	// must be pointing at the end of a segment

		// Do a FindFirst to find the actual directory entry
		cTemp = *pszEnd;
		*pszEnd = _T('\0');
		PTCHAR szTrueName = psz;
		if (_tcschr(psz, _T('*')) == NULL && _tcschr(psz, _T('?')) == NULL )	// if it includes widecards, can't do anything
		{
			HANDLE hFind = FindFirstFile( szPath, &ffd );
			if (hFind == INVALID_HANDLE_VALUE)
			{
				rFilename = strOldFileName;
				return TRUE;
			}
			VERIFY( FindClose( hFind ) );

			szTrueName = ffd.cFileName;
		}
		BeautifyAllCaps(szTrueName);
		rFilename += szTrueName;
		*pszEnd = cTemp;

		psz = pszEnd;
	}

	//ASSERT( _tcsicmp(szPath, szOldPath) == 0 );
	//rFilename = szPath;

	return TRUE;
}

// This function returns a pointer to a string which contains lpszExtension
// in a case to match lpszFilename. I.E. if lpszFilename is uppercase, GetExtensionCase
// returns a pointer to a copy of lpszExtension which is all uppercase.
// Likewise, is lpszExtension is all lower or mixed case, it returns a pointer to
// a copy of lpszExtension which is all lower case.
LPCTSTR GetExtensionCase( LPCTSTR lpszFilename, LPCTSTR lpszExtension )
{
	TCHAR szFilename[ MAX_PATH ];
	if (SushiGetFileTitle( lpszFilename, szFilename, MAX_PATH ) != 0)
		return lpszExtension;	// invalid filename

	BOOL bFoundL = FALSE, bFoundU = FALSE;
	PTCHAR pChar = szFilename;
	while (pChar != NULL && *pChar != _T('\0'))
	{
		if (_istupper(*pChar))
			bFoundU = TRUE;
		else if (_istlower(*pChar))
			bFoundL = TRUE;

		pChar = _tcsinc( pChar );
	}

	static CString strExt;
	strExt = lpszExtension;
	if (bFoundU && !bFoundL)	// only upper
	{
		// make output upper
		strExt.MakeUpper();
	}
	else	// either mixed or all lower - make extension lower
	{
		strExt.MakeLower();
	}

	return strExt;
}

static TCHAR szEllipse[] = _T("...\\");
#define cbEllipse (sizeof(szEllipse) / sizeof(TCHAR) - sizeof(TCHAR))

BOOL GetDisplayFile(CString& rFilename, CDC* pDC, int& cxPels)
{
	// This function is used to calculate the best path string
	// to display that is cxPels pixels wide or narrower.
	ASSERT_VALID(pDC);

	if (rFilename.IsEmpty())
		return FALSE;

	// Get a fully qualified filename
	TCHAR szPath[MAX_PATH + 1];
	TCHAR *pszFilePart;
	DWORD dwLen = ::GetFullPathName(rFilename, MAX_PATH, szPath, &pszFilePart);
	if (dwLen == 0 || dwLen > MAX_PATH)
		return FALSE;	// can't fix a path this long

	if (lstrlen(szPath) < 4)	// need at least a drive and filename (shortest legal filename is "C:\A")
		return FALSE; // can't fix a path this short

	rFilename = szPath;
	CSize size = pDC->GetTextExtent(rFilename);
	int nExtent = size.cx;
	if (nExtent <= cxPels)
	{
		cxPels = nExtent;
		return TRUE;
	}

	if (nExtent > cxPels)
	{
		CString strTemp = szPath, strReturn;
		TCHAR  *lpszTemp = SkipDrive(szPath);
		strTemp = strTemp.Left(lpszTemp - szPath);
		strTemp += _TEXT("...\\");

		while (nExtent > cxPels)
		{
			strReturn = strTemp;

			// strip out directories until wExtent not greater
			lpszTemp = _tcschr(lpszTemp, _T('\\'));

			if (lpszTemp)
			{
				lpszTemp = _tcsinc(lpszTemp);
				strReturn += lpszTemp;
			}
			else
				break;

			size = pDC->GetTextExtent(strReturn);
			nExtent = size.cx;
		}

		rFilename = strReturn;
		cxPels = nExtent;
	}
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
//	CFileOpenReturn
//		This class represents the return value from the Common Dialogs
//		File.Open.  It handles both single and multiple select types.
//
//		The format of a multiple-file select string is:
//
//			<dir-name> <file1> <file2> ...
//

CFileOpenReturn::CFileOpenReturn (const _TCHAR * szRawString /* = NULL */)
{
	m_pchData = (szRawString != NULL) ? _tcsdup (szRawString) : NULL;
	m_bBufferInUse = FALSE;
	m_bArrayHasChanged = FALSE;

	GenArrayFromBuffer ();
}


CFileOpenReturn::~CFileOpenReturn ()
{
	if (m_pchData)
		free (m_pchData);

	ClearNamesArray ();
}		


void CFileOpenReturn::ReInit (const _TCHAR * szRawString)
{
	SetBuffer (szRawString);
	GenArrayFromBuffer ();
}


void CFileOpenReturn::ClearNamesArray ()
{
int cNames = m_rgszNames.GetSize();

	for (int i = 0; i < cNames; i++)
		free (m_rgszNames[i]);
		
	m_rgszNames.RemoveAll();
}


///// GenArrayFromBuffer - Set internal state to match current raw string value
//
///
void CFileOpenReturn::GenArrayFromBuffer ()
{
	// Assume no data in buffer
	m_cbData = 0;
	m_bSingle = TRUE;
	ClearNamesArray ();

	if (m_pchData != NULL)
	{
		m_cbData  = strlen (m_pchData) + 1;	// Bytes!

		if (!(m_bSingle = (NULL == _tcschr (m_pchData, ' '))))
		{
			int i;
			_TCHAR * pchWalk;
			for (pchWalk = m_pchData, i = 0; *pchWalk; i++)
			{
				_TCHAR * pchCurName = pchWalk;
				_TCHAR chPrev;
				if (NULL != (pchWalk = _tcschr (pchWalk, ' ')))
				{
					chPrev = *pchWalk;
					*pchWalk = '\0';
				}

				m_rgszNames.SetAtGrow (i, _tcsdup(pchCurName));

				if (pchWalk != NULL)
				{
					*pchWalk = chPrev;
					if (NULL == (pchWalk = _tcsspnp (pchWalk, " ")))
						break;
				}
				else
					break;

			}
		}
	}
}


///// GenBufferFromArray - Update buffer to match names array
//
///
void CFileOpenReturn::GenBufferFromArray ()
{
	if (!IsSingle())
	{
		ASSERT (m_pchData != NULL);
		ASSERT (!m_bBufferInUse);

		// First we need to know how long the buffer should be.
		// This is the sum of the individual strings, plus a space
		// between each one and a nul terminator.
  		int cNames = m_rgszNames.GetSize();
		for (int i = 0, cbrgNames = 0; i < cNames; i++)
		{
			cbrgNames += strlen ((const char *)m_rgszNames[i]) + 1;
		}

		// Now we can allocate a new one.
		free (m_pchData);
		m_pchData = (_TCHAR *)malloc (cbrgNames);

		// Now we copy each string into the buffer.
		_TCHAR * pchWalk = m_pchData;
		for (i = 0, cbrgNames = 0; i < cNames; i++)
		{
			_tcscpy (pchWalk, (const char *)m_rgszNames[i]);
			pchWalk += strlen ((const char *)m_rgszNames[i]);
			*pchWalk++ = ' ';
		}

		// The final space was put where we should really have a nul
		pchWalk[-1] = '\0';
	}
	else
		; // Single-choice case doesn't use the name array.
}


///// GetPathname - Fill a string with the full pathname of the i'th file
//
//	A FALSE return means either that there is no i'th file.
//
//	For multiple file selects, the full pathname must be assembled from the dir name
//	and filename.
//  For single file selects, COMMDLG has already generated the full path.
///
BOOL CFileOpenReturn::GetPathname (int i, CString& strPath) const
{
	if (!IsSingle())
	{
		// A multiple select has at least a dir name and a file name.
		ASSERT (m_rgszNames.GetSize() >= 2);
		ASSERT (!m_bBufferInUse);

   		if (i + 1 < m_rgszNames.GetSize())
		{
			strPath = (_TCHAR *)m_rgszNames[0];

			if (strPath.ReverseFind ('\\') != strPath.GetLength() - 1)
				strPath += '\\';

			strPath += (TCHAR *)m_rgszNames[i+1];	// +1 because [0] is the dir name

			// Chicago bogosity.  COMMDLG leaves dots at end of filenames.
			int cbStrLessOne = strPath.GetLength () - 1;
			if (strPath.ReverseFind ('.') == cbStrLessOne)
			{
				// The effect of these calls is to shorten the string by one character
				strPath.GetBuffer (cbStrLessOne);
				strPath.ReleaseBuffer (cbStrLessOne);
			}

			return TRUE;
		}
		else
			return FALSE;
	}
	else
	if (i == 0)
	{
		strPath = m_pchData;
		return TRUE;
	}
	else
		return FALSE;
}


///// GetBuffer - Ensure correct size for buffer and return ptr to it
//
//	This gives permission to the caller to do whatever it wants with our buffer.
//	When the caller is done with the buffer, it must call ReleaseBuffer() so we
//	can synchronize with the new contents.
//	This function also guarantees a minimum size for the buffer, preserving the
//	current contents.
///
_TCHAR * CFileOpenReturn::GetBuffer (int cbBufferNew)
{
	if (cbBufferNew > m_cbData)
	{
		_TCHAR * pchNew = (_TCHAR *)malloc (cbBufferNew);

		if (m_cbData)
		{
			ASSERT (m_pchData != NULL);
			memcpy (pchNew, m_pchData, m_cbData);
			free (m_pchData);
		}
		else
			*pchNew = '\0';

		m_cbData = cbBufferNew;
		m_pchData = pchNew;
	}

	m_bBufferInUse = TRUE;
	return m_pchData;
}


///// ChangeExtension - Change the extension of the i'th file
//
///
void CFileOpenReturn::ChangeExtension (int i, const CString& strExtNew)
{
	ASSERT (!m_bBufferInUse);

	CString strFileNameCur (IsSingle() ? m_pchData : (const char *)m_rgszNames[i+1]);

	if (strFileNameCur.IsEmpty())
		return;

	CString strExtCur = GetExtension (strFileNameCur);

	CString strFileNameNew ((strExtCur.IsEmpty()
								? strFileNameCur
								: strFileNameCur.Left(strExtCur.GetLength()))
							+ '.'	// Assume we need to add the dot.
							+ strExtNew);

	_TCHAR * szFileNameNew = _tcsdup (strFileNameNew);

	if (IsSingle())
	{
		free (m_pchData);
		m_pchData = szFileNameNew;
		m_cbData = strFileNameNew.GetLength() + 1;
	}
	else
	{
		free (m_rgszNames[i+1]);
		m_rgszNames[i+1] = szFileNameNew;
	}

	m_bArrayHasChanged = TRUE;
}
	

///// CopyBuffer
//
///
void CFileOpenReturn::CopyBuffer (_TCHAR * szTarget)
{
	if (IsDirty())
		GenBufferFromArray ();

	_tcscpy (szTarget, m_pchData);
}


///// SetBuffer
//
///
void CFileOpenReturn::SetBuffer (const _TCHAR * szRawString)
{
	if (m_pchData)
		free (m_pchData);

	if (szRawString == NULL)
	{
		m_pchData = NULL;
		m_cbData = 0;
	}

	_TCHAR * pchSpace = _tcschr (szRawString, ' ');
	int cbSzRaw = strlen (szRawString) + 1;

	if (pchSpace)
	{	// This should be a multi-file select, but we don't trust it.
		CString strDirName;
		_TCHAR * pchDirName = strDirName.GetBuffer (pchSpace - szRawString);
		_tcsncpy (pchDirName, szRawString, pchSpace - szRawString);
		strDirName.ReleaseBuffer (pchSpace - szRawString);

		struct _stat statbuf;

		if (_stat (strDirName, &statbuf) || !(statbuf.st_mode & _S_IFDIR))
		{	// First string is NOT a directory!
			CDir dirCur;

			VERIFY (dirCur.CreateFromCurrent ());

			int cbDirCur = strlen (dirCur) + 1;

			m_pchData = (TCHAR *)malloc (cbDirCur + cbSzRaw);

			_tcscpy (m_pchData, dirCur);
			m_pchData[cbDirCur-1] = ' ';
			_tcscpy (m_pchData + cbDirCur, szRawString);

			m_cbData = cbDirCur + cbSzRaw;

			return;
		}
	}

	m_pchData = _tcsdup (szRawString);
	m_cbData = cbSzRaw;
}

//
// Support for new win32 api, GetFileAttributesEx
//

static BOOL WINAPI Choose_GetFileAttributesEx( LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
									 LPVOID lpFileInformation);

static BOOL WINAPI Slow_GetFileAttributesEx( LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
									 LPVOID lpFileInformation);

// Initialize g_pGetFileAttributesEx = Choose_GetFileAttributesEx.
BOOL (WINAPI *g_pGetFileAttributesEx)( LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
									 LPVOID lpFileInformation) = Choose_GetFileAttributesEx;

static BOOL WINAPI Choose_GetFileAttributesEx( LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
									 LPVOID lpFileInformation)
{
	//
	// Block any other threads using mutex.  This is normally a one shot deal.
	//
	TCHAR szMutexName[50];

	//
	// Create a mutex name that is unique each process.
	//
	_stprintf( szMutexName, _T("Choose_GetFileAttributesEx:Shell:%lX"), GetCurrentProcessId() );

	HANDLE hMutex = CreateMutex( NULL, FALSE, szMutexName );
	if (NULL==hMutex)
	{
		ASSERT(0);
		if (&Choose_GetFileAttributesEx==g_pGetFileAttributesEx)
		{
			g_pGetFileAttributesEx = &Slow_GetFileAttributesEx;
			TRACE("::Choose_GetFileAttributesEx: Slow_GetFileAttributesEx was selected because of failure\n");
		}
	}
	else
	{
		// Time out after 10 seconds. 
		DWORD dwWait = WaitForSingleObject(hMutex, 10000L);

		switch(dwWait)
		{
		case WAIT_OBJECT_0:
			if (&Choose_GetFileAttributesEx==g_pGetFileAttributesEx)
			{
				HINSTANCE hLib = GetModuleHandle( _T("kernel32.dll") );
				if (NULL!=hLib)
				{
#ifdef _UNICODE
					FARPROC lpProc = GetProcAddress(hLib, "GetFileAttributesExW" );
#else
					FARPROC lpProc = GetProcAddress(hLib, "GetFileAttributesExA" );
#endif
					if (NULL!=lpProc)
					{
						g_pGetFileAttributesEx = 
							(BOOL (WINAPI *)( LPCTSTR, GET_FILEEX_INFO_LEVELS,LPVOID))lpProc;
						TRACE("::Choose_GetFileAttributesEx: GetFileAttributesEx was selected\n");
					}
					else
					{
						g_pGetFileAttributesEx = &Slow_GetFileAttributesEx;
						TRACE("::Choose_GetFileAttributesEx: Slow_GetFileAttributesEx was selected\n");
					}
				}

			}
			VERIFY(ReleaseMutex(hMutex));
			break;

		case WAIT_TIMEOUT:
		case WAIT_ABANDONED:
		default:
			//
			// Use Slow_GetFileAttributesEx when checking for GetFileAttributesEx fails.
			//
			ASSERT(0);
			if (&Choose_GetFileAttributesEx==g_pGetFileAttributesEx)
			{
				g_pGetFileAttributesEx = &Slow_GetFileAttributesEx;
			}
			TRACE("::Choose_GetFileAttributesEx: Slow_GetFileAttributesEx was selected because of failure\n");
			break;
		}
		CloseHandle(hMutex);
	}
	return (*g_pGetFileAttributesEx)( lpFileName, fInfoLevelId, lpFileInformation);
}

//
// Slow_GetFileAttributesEx is used when GetFileAttributesEx is unsupported.
//
BOOL WINAPI Slow_GetFileAttributesEx( LPCTSTR lpFileName, GET_FILEEX_INFO_LEVELS fInfoLevelId,
									 LPVOID lpFileInformation)
{
	WIN32_FILE_ATTRIBUTE_DATA*  pFileData = (WIN32_FILE_ATTRIBUTE_DATA*)lpFileInformation;

	ASSERT(GetFileExInfoStandard==fInfoLevelId);

	WIN32_FIND_DATA find;
	HANDLE hfind = ::FindFirstFile(lpFileName, &find);

	if (hfind != INVALID_HANDLE_VALUE)
	{
		::FindClose(hfind);

		pFileData->dwFileAttributes = find.dwFileAttributes;
		pFileData->ftCreationTime = find.ftCreationTime; 
		pFileData->ftLastAccessTime = find.ftLastAccessTime; 
		pFileData->ftLastWriteTime = find.ftLastWriteTime; 
		pFileData->nFileSizeHigh = find.nFileSizeHigh; 
		pFileData->nFileSizeLow = find.nFileSizeLow; 

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

