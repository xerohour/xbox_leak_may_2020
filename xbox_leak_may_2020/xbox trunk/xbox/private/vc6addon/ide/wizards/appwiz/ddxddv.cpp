#include "stdafx.h"
#include "resource.h"
#include "ddxddv.h"

/////////////////////////////////////////////////////////////////////////////
// Japanese-specific functions

// This tests whether we're on a Japanese system
inline BOOL IsJapaneseSystem(void)
{
	return (PRIMARYLANGID(GetSystemDefaultLangID()) == LANG_JAPANESE);
}

// This function assumes we're on a Japanese system
BOOL IsSBKatakana(unsigned char c)
{
	return c >= 0xa1 && c <= 0xdf;
}


/////////////////////////////////////////////////////////////////////////////

BOOL IsValidSymbol(const char* psz, BOOL bCanBeginWithNumber, BOOL bCanHaveAngleBrackets )
{
	if (!_istalpha(*psz) && *psz != '_'
		&& !(bCanBeginWithNumber && _istdigit(*psz))
		&& !(bCanHaveAngleBrackets && ( (*psz=='<') || (*psz=='>') )))
	{
		return FALSE;
	}
	if (strlen(psz) >= _MAX_SYMBOL)
		return FALSE;

	psz = _tcsinc(psz);
	while (*psz != NULL)
	{
		if( bCanHaveAngleBrackets ) 
		{
			if (!_istalnum(*psz) && *psz != '_' && *psz != '<' && *psz != '>')
				return FALSE;
		}
		else 
		{
			if (!_istalnum(*psz) && *psz != '_' )
				return FALSE;
		}
		psz = _tcsinc(psz);
	}
	return TRUE;
}

void PASCAL DDV_Symbol(CDataExchange* pDX, CString const& s)
{
	if (!pDX->m_bSaveAndValidate || IsValidSymbol(s, FALSE))
		return;

	AfxMessageBox(IDP_INVALID_SYMBOL, MB_OK, 0);
	pDX->Fail();
}

void PASCAL DDV_SymbolCanBeginWithNumber(CDataExchange* pDX, CString const& s)
{
	if (!pDX->m_bSaveAndValidate || IsValidSymbol(s, TRUE))
		return;

	AfxMessageBox(IDP_INVALID_SYMBOL, MB_OK, 0);
	pDX->Fail();
}

void PASCAL DDV_ClassName(CDataExchange* pDX, CString const& s)
{
	if (!pDX->m_bSaveAndValidate || IsValidSymbol(s, TRUE, TRUE))
		return;

	AfxMessageBox(IDP_INVALID_SYMBOL, MB_OK, 0);
	pDX->Fail();
}

/*
static BOOL LooksLikeAPath(const TCHAR* psz)
{
    while (_istspace(*psz))
        psz = _tcsinc(psz);
    if (*psz == '.')
        return TRUE;        // starting . or ..
    TCHAR ch;
    // we will allow '.' in the middle of a project name
    while ((ch = *psz) != '\0')
    {
		psz = _tcsinc(psz);
        if (ch == '/' || ch == '\\' || ch == ':')
            return TRUE;        // look like a path to me
    }
    return FALSE;       // try it as a project name
}
*/

BOOL IsValidFileName(const char* psz, BOOL bAllowDot)
	// NO PATH (allow suffix optionally)
{
	/*if (LooksLikeAPath(psz))
		return FALSE;

	HANDLE hFile = CreateFile(psz, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_NEW, 
								FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	CloseHandle(hFile);
	return TRUE;*/

	if (*psz == '\0' || *psz == '.')
		return FALSE;
	
	int cchLeft = 8;                // 8 for file name
	DWORD dwMaxLen = 8;
	DWORD  dwDummy1;
	if (::GetVolumeInformation(NULL, NULL, 0, NULL, &dwMaxLen,
		&dwDummy1, NULL, 0))
	{
		// succesfully got info from file system -- use it.
		cchLeft = (int)dwMaxLen;
	}
	
	char ch;
	while ((ch = *psz) != '\0')
	{
		if (_istalnum(ch) || ch == '_' || ch == ' ' || IsDBCSLeadByte(ch)
			|| (IsJapaneseSystem() && IsSBKatakana(ch)) || ch < 0)
		{
			// count the character
			cchLeft -= _tclen(psz);
			if (cchLeft < 0)
				return FALSE;		// to long
		}
		else if (ch != '.' || !bAllowDot)
		{
			// illegal
			return FALSE;
		}
		psz = _tcsinc(psz);
	}
	return TRUE;
}

void PASCAL DDV_FileName(CDataExchange* pDX, CString& s,
			const char* pszExtension)
{
	if (!pDX->m_bSaveAndValidate)
		return;
	if (!IsValidFileName(s, pszExtension != NULL))
	{
		AfxMessageBox(IDP_INVALID_FILENAME, MB_OK, 0);
		pDX->Fail();
	}

	if (pszExtension == NULL)
		return;

	// make sure the suffix is ok
	char szExt[_MAX_EXT];
	_splitpath(s, NULL, NULL, NULL, szExt);
	if (szExt[0] == '\0')
	{
		// no extension - add it
		s += pszExtension;
		return;
	}
	if (_stricmp(szExt, pszExtension) != 0)
	{
		AfxMessageBox(IDP_INVALID_EXTENSION, MB_OK, 0);
		pDX->Fail();
	}
}

BOOL IsValidProgID(const char* psz)
{
	if (!_istalpha(*psz))
		return FALSE;

	psz = _tcsinc(psz);
	while (*psz != NULL)
	{
		if (!_istalnum(*psz) && *psz != '.')
			return FALSE;
		psz = _tcsinc(psz);
	}
	return TRUE;
}

BOOL IsValidTitle(const char* szTitle);		// Defined in miscdlgs.cpp

void PASCAL DDV_Title(CDataExchange* pDX, CString const& s)
{
	if (!pDX->m_bSaveAndValidate || IsValidTitle(s))
		return;
	
	AfxMessageBox(IDP_BAD_TITLE);
	pDX->Fail();
}


void PASCAL DDV_ProgID(CDataExchange* pDX, CString const& s)
{
	if (!pDX->m_bSaveAndValidate || IsValidProgID(s))
		return;

	AfxMessageBox(IDP_INVALID_PROGID, MB_OK, 0);
	pDX->Fail();
}

void PASCAL DDV_ExactChars(CDataExchange* pDX, CString const& s, int nLen)
{
	if (!pDX->m_bSaveAndValidate || s.GetLength() == nLen)
		return;

	CString strPrompt;
	TCHAR tszLen[128];
	sprintf(tszLen, "%d", nLen);
	AfxFormatString1(strPrompt, IDP_EXACT_CHARS, tszLen);
	AfxMessageBox(strPrompt, MB_OK, 0);
	pDX->Fail();
}
		

/////////////////////////////////////////////////////////////////////////////

CString Upper(const char* psz)
{
	CString s = psz;
	ASSERT(!s.IsEmpty());
	s.MakeUpper();
	return s;
}

CString Lower(const char* psz)
{
	CString s = psz;
	ASSERT(!s.IsEmpty());
	s.MakeLower();
	return s;
}

// Returns the largest left substring of psz with <= nMaxTChars TCHARS.
CString TcsLeft(const TCHAR* psz, int nMaxTChars)
{
	CString s = psz;
	TCHAR* pszRet = s.GetBuffer(s.GetLength() + 1);
	int nLength = 0;
	while (*pszRet != '\0')
	{
		nLength += _tclen(pszRet);
		if (nLength > nMaxTChars)
			break;
		pszRet = _tcsinc(pszRet);
	}
	// If we broke out, we need to shorten the string.  End at pszRet.
	*pszRet = '\0';
	s.ReleaseBuffer();
	return s;
}

#ifdef _DEBUG

DWORD MyGetLastError()
{
	return ::GetLastError();
}

#endif //_DEBUG
