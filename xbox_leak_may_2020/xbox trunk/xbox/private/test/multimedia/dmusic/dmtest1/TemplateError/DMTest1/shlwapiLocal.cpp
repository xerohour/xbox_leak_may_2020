//===========================================================================
// SHLWAPILOCAL.CPP
//
//  Because there are no good path functions in SHLWAPI, and because
//  that DLL isn't on Win95, and because we have to run our tests on Win95,
//  I have no choice but to copy that code in here and statically link it.
//
//===========================================================================
//#include "dmcs.h"
#include "globals.h"
#include "shlwapilocal.h"



#define TEXTW(quote) L##quote      

#define ARRAYSIZE(a)    (sizeof(a)/sizeof(a[0]))                          /* ;Internal */

//danhaff - guessed this myself.
#define CH_WHACK TEXT('\\')

#define FAST_CharNext(p)    (DBNotNULL(p) + 1)

#define DBNotNULL(p)    (p)

void NearRootFixups (LPTSTR lpszPath, BOOL fUNC);

#define OFFSETOF(x)     ((UINT)(x))

/*
LPTSTR StrChr (LPTSTR lpsz, TCHAR ch)
{
	LPTSTR p = NULL;
	while (*lpsz)
	{
		if (*lpsz == ch)
		{
			p = lpsz;
			break;
		}
		lpsz = CharNext(lpsz);
	}
	return p;
}

*/


/*
LPTSTR StrChr(LPTSTR lpsz, TCHAR ch)
{
    LPTSTR p = NULL;
    while (*lpsz)
    {
        if (*lpsz == ch)
        {
            p = lpsz;
            break;
        }
        lpsz = CharNext(lpsz);
    }
    return p;
}


BOOL ChrCmp(WORD w1, WORD wMatch)
{
  // Most of the time this won't match, so test it first for speed.
   //
  if (LOBYTE(w1) == LOBYTE(wMatch))
    {
      if (IsDBCSLeadByte(LOBYTE(w1)))
        {
          return(w1 != wMatch);
        }
      return FALSE;
    }
  return TRUE;
}



LPSTR FAR PASCAL StrRChr(LPCSTR lpStart, LPCSTR lpEnd, WORD wMatch)
{
  LPCSTR lpFound = NULL;

  if (!lpEnd)
      lpEnd = lpStart + lstrlen(lpStart);

  for ( ; OFFSETOF(lpStart) < OFFSETOF(lpEnd); lpStart = AnsiNext(lpStart))
    {
      if (!ChrCmp(*(WORD FAR *)lpStart, wMatch))
          lpFound = lpStart;
    }
  return ((LPSTR)lpFound);
}


LPCTSTR GetPCEnd(LPCTSTR lpszStart)
{
    LPCTSTR lpszEnd;

    lpszEnd = StrChr((char *)lpszStart, CH_WHACK);
    if (!lpszEnd)
    {
        lpszEnd = lpszStart + lstrlen(lpszStart);
    }

    return lpszEnd;
}




//
// Given a pointer to the end of a path component, return a pointer to
// its begining.
// ie return a pointer to the previous backslash (or start of the string).
//
LPCTSTR PCStart(LPCTSTR lpszStart, LPCTSTR lpszEnd)
{
    LPCTSTR lpszBegin = StrRChr(lpszStart, lpszEnd, CH_WHACK);
    if (!lpszBegin)
    {
        lpszBegin = lpszStart;
    }
    return lpszBegin;
}





static __inline BOOL DBL_BSLASH(LPCTSTR psz)
{
    return (psz[0] == TEXT('\\') && psz[1] == TEXT('\\'));
}


BOOL PathIsUNC(LPCTSTR pszPath)
{
    return DBL_BSLASH(pszPath);
}

__inline BOOL PathIsUNC(
    LPWSTR psz)
{
    return (psz[0] == L'\\' && psz[1] == L'\\');
}




BOOL PathIsUNCServer(LPCTSTR pszPath)
{
    if (DBL_BSLASH(pszPath))
    {
        int i = 0;
        LPTSTR szTmp;

        for (szTmp = (LPTSTR)pszPath; szTmp && *szTmp; szTmp = CharNext(szTmp) )
        {
            if (*szTmp==TEXT('\\'))
            {
                i++;
            }
        }

       return (i == 2);
    }

    return FALSE;
}



BOOL PathIsDirectory(LPCTSTR pszPath)
{
    DWORD dwAttribs;

    // SHELL32's PathIsDirectory also handles server/share
    // paths, but calls WNet APIs, which we cannot call.

    if (PathIsUNCServer(pszPath))
    {
        return FALSE;
    }
    else
    {
        dwAttribs = GetFileAttributes(pszPath);
        if (dwAttribs != (DWORD)-1)
            return (BOOL)(dwAttribs & FILE_ATTRIBUTE_DIRECTORY);
    }

    return FALSE;
}















STDAPI_(BOOL) PathIsRelative(LPCTSTR lpszPath)
{
//    RIPMSG(lpszPath && IS_VALID_STRING_PTR(lpszPath, -1), "PathIsRelative: caller passed bad lpszPath");

    if (!lpszPath || *lpszPath == 0)
    {
        // The NULL path is assumed relative
        return TRUE;
    }


    if (lpszPath[0] == CH_WHACK)
    {
        // Does it begin with a slash ?
        return FALSE;
    }
    else if (!IsDBCSLeadByte(lpszPath[0]) && lpszPath[1] == TEXT(':'))
    {
        // Does it begin with a drive and a colon ?
        return FALSE;
    }
    else
    {
        // Probably relative.
        return TRUE;
    }
}














STDAPI_(LPTSTR) PathAddBackslash(LPTSTR lpszPath)
{

    if (lpszPath)
    {
        LPTSTR lpszEnd;

        // perf: avoid lstrlen call for guys who pass in ptr to end
        // of buffer (or rather, EOB - 1).
        // note that such callers need to check for overflow themselves.
        int ichPath = (*lpszPath && !*(lpszPath + 1)) ? 1 : lstrlen(lpszPath);

        // try to keep us from tromping over MAX_PATH in size.
        // if we find these cases, return NULL.  Note: We need to
        // check those places that call us to handle their GP fault
        // if they try to use the NULL!
        if (ichPath >= (MAX_PATH - 1))
        {
            return(NULL);
        }

        lpszEnd = lpszPath + ichPath;

        // this is really an error, caller shouldn't pass
        // an empty string
        if (!*lpszPath)
            return lpszEnd;

        // Get the end of the source directory
        switch(*CharPrev(lpszPath, lpszEnd))
        {
            case CH_WHACK:
                break;

            default:
                *lpszEnd++ = CH_WHACK;
                *lpszEnd = TEXT('\0');
        }

        return lpszEnd;
    }

    return NULL;
}





STDAPI_(BOOL) PathStripToRoot(LPTSTR szRoot)
{
    if (szRoot)
    {
        while(!PathIsRoot(szRoot))
        {
            if (!PathRemoveFileSpec(szRoot))
            {
                // If we didn't strip anything off,
                // must be current drive
                return(FALSE);
            }
        }

        return(TRUE);
    }
    return(FALSE);
}







STDAPI_(LPTSTR) PathCombine(LPTSTR lpszDest, LPCTSTR lpszDir, LPCTSTR lpszFile)
{

    if (lpszDest && (lpszDir || lpszFile))
    {
        TCHAR szTemp[MAX_PATH];
        LPTSTR pszT;

        if (!lpszFile || *lpszFile==TEXT('\0'))
        {
            lstrcpyn(szTemp, lpszDir, ARRAYSIZE(szTemp));       // lpszFile is empty
        }
        else if (lpszDir && *lpszDir && PathIsRelative(lpszFile))
        {
            lstrcpyn(szTemp, lpszDir, ARRAYSIZE(szTemp));
            pszT = PathAddBackslash(szTemp);
            if (pszT)
            {
                int iLen = lstrlen(szTemp);

                if ((iLen + lstrlen(lpszFile)) < ARRAYSIZE(szTemp))
                    lstrcpy(pszT, lpszFile);
                else
                    return NULL;
            }
            else
            {
                return NULL;
            }
        }
        else if (lpszDir && *lpszDir && *lpszFile == CH_WHACK && !PathIsUNC(lpszFile))
        {
            lstrcpyn(szTemp, lpszDir, ARRAYSIZE(szTemp));
            // BUGBUG: Note that we do not check that an actual root is returned;
            // it is assumed that we are given valid parameters
            PathStripToRoot(szTemp);

            pszT = PathAddBackslash(szTemp);
            if (pszT)
            {
                // Skip the backslash when copying
                // Note: We don't support strings longer than 4GB, but that's
                // okay because we already barf at MAX_PATH
                lstrcpyn(pszT, lpszFile+1, ARRAYSIZE(szTemp) - 1 - (int)(pszT-szTemp));
            }
            else
            {
                return NULL;
            }

        }
        else 
        {
            lstrcpyn(szTemp, lpszFile, ARRAYSIZE(szTemp));     // already fully qualified file part
        }

        PathCanonicalize(lpszDest, szTemp); // this deals with .. and . stuff
    }

    return lpszDest;
}




STDAPI_(BOOL) PathIsRoot(LPCTSTR pPath)
{

    if (!pPath || !*pPath)
    {
        return FALSE;
    }

    if (!IsDBCSLeadByte(*pPath))
    {
        if (!lstrcmpi(pPath + 1, TEXT(":\\")))
        {
            // "X:\" case
            return TRUE;
        }
    }

    if ((*pPath == CH_WHACK) && (*(pPath + 1) == 0))
    {
        // "/" or "\" case
        return TRUE;
    }

    if (DBL_BSLASH(pPath))      // smells like UNC name
    {
        LPCTSTR p;
        int cBackslashes = 0;

        for (p = pPath + 2; *p; p = FAST_CharNext(p))
        {
            if (*p == TEXT('\\') && (++cBackslashes > 1))
            {
               // not a bare UNC name, therefore not a root dir 
               return FALSE;
            }
        }
        return TRUE;    // end of string with only 1 more backslash 
                        // must be a bare UNC, which looks like a root dir 
    }
    return FALSE;
}




STDAPI_(BOOL) PathRemoveFileSpec(LPTSTR pFile)
{

    if (pFile)
    {
        LPTSTR pT;
        LPTSTR pT2 = pFile;

        for (pT = pT2; *pT2; pT2 = FAST_CharNext(pT2))
        {
            if (*pT2 == CH_WHACK)
            {
                pT = pT2;             // last "\" found, (we will strip here)
            }
            else if (*pT2 == TEXT(':'))     // skip ":\" so we don't
            {
                if (pT2[1] ==TEXT('\\'))    // strip the "\" from "C:\"
                {
                    pT2++;
                }
                pT = pT2 + 1;
            }
        }

        if (*pT == 0)
        {
            // didn't strip anything
            return FALSE;
        }
        else if (((pT == pFile) && (*pT == CH_WHACK)) ||                        //  is it the "\foo" case?
                 ((pT == pFile+1) && (*pT == CH_WHACK && *pFile == CH_WHACK)))  //  or the "\\bar" case?
        {
            // Is it just a '\'?
            if (*(pT+1) != TEXT('\0'))
            {
                // Nope.
                *(pT+1) = TEXT('\0');
                return TRUE;        // stripped something
            }
            else
            {
                // Yep.
                return FALSE;
            }
        }
        else
        {
            *pT = 0;
            return TRUE;    // stripped something
        }
    }
    return  FALSE;
}



//----------------------------------------------------------
Purpose: Canonicalize a path.

Returns:
Cond:    --
//
STDAPI_(BOOL) PathCanonicalize(LPTSTR lpszDst, LPCTSTR lpszSrc)
{
    LPCTSTR lpchSrc;
    LPCTSTR lpchPCEnd;      // Pointer to end of path component.
    LPTSTR lpchDst;
    BOOL fUNC;
    int cbPC;


    if (!lpszDst || !lpszSrc)
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    //DEBUGWhackPathBuffer(lpszDst, MAX_PATH);
    
    fUNC = PathIsUNC(lpszSrc);    // Check for UNCness.

    // Init.
    lpchSrc = lpszSrc;
    lpchDst = lpszDst;

    while (*lpchSrc)
    {
        // REVIEW: this should just return the count
        lpchPCEnd = GetPCEnd(lpchSrc);
        cbPC = (int) (lpchPCEnd - lpchSrc)+1;

        if (cbPC == 1 && *lpchSrc == CH_WHACK)                                      // Check for slashes.
        {
            // Just copy them.
            *lpchDst = CH_WHACK;
            lpchDst++;
            lpchSrc++;
        }
        else if (cbPC == 2 && *lpchSrc == TEXT('.'))                                // Check for dots.
        {
            // Skip it...
            // Are we at the end?
            if (*(lpchSrc+1) == TEXT('\0'))
            {
                lpchDst--;
                lpchSrc++;
            }
            else
            {
                lpchSrc += 2;
            }
        }
        else if (cbPC == 3 && *lpchSrc == TEXT('.') && *(lpchSrc + 1) == TEXT('.')) // Check for dot dot.
        {
            // make sure we aren't already at the root
            if (!PathIsRoot(lpszDst))
            {
                // Go up... Remove the previous path component.
                lpchDst = (LPTSTR)PCStart(lpszDst, lpchDst - 1);
            }
            else
            {
                // When we can't back up, remove the trailing backslash
                // so we don't copy one again. (C:\..\FOO would otherwise
                // turn into C:\\FOO).
                if (*(lpchSrc + 2) == CH_WHACK)
                {
                    lpchSrc++;
                }
            }

            // skip ".."
            lpchSrc += 2;       
        }
        else                                                                        // Everything else
        {
            // Just copy it.
            lstrcpyn(lpchDst, lpchSrc, cbPC);
            lpchDst += cbPC - 1;
            lpchSrc += cbPC - 1;
        }

        // Keep everything nice and tidy.
        *lpchDst = TEXT('\0');
    }

    // Check for weirdo root directory stuff.
    NearRootFixups(lpszDst, fUNC);

    return TRUE;
}



//
// Fix up a few special cases so that things roughly make sense.
//
void NearRootFixups (LPTSTR lpszPath, BOOL fUNC)
{
    // Check for empty path.
    if (lpszPath[0] == TEXT('\0'))
    {
        // Fix up.
        lpszPath[0] = CH_WHACK;
        lpszPath[1] = TEXT('\0');
    }
    // Check for missing slash.
    if (!IsDBCSLeadByte(lpszPath[0]) && lpszPath[1] == TEXT(':') && lpszPath[2] == TEXT('\0'))
    {
        // Fix up.
        lpszPath[2] = TEXT('\\');
        lpszPath[3] = TEXT('\0');
    }
    // Check for UNC root.
    if (fUNC && lpszPath[0] == TEXT('\\') && lpszPath[1] == TEXT('\0'))
    {
        // Fix up.
        //lpszPath[0] = TEXT('\\'); // already checked in if guard
        lpszPath[1] = TEXT('\\');
        lpszPath[2] = TEXT('\0');
    }
}
*/