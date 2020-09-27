#include "stdafx.h"
#pragma hdrstop


#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

int cmappath = 0;
MAPPATH rgmappath[cmappathMax];

CSemaphore g_evFindSymbols(0,1);

/****************************************************************************

	FUNCTION:	AddMapping()

	PURPOSE:	Records a mapping from one path to another, so that
				future attempts to find files down this same path
				subtree can succeed.

	INPUT:		lszFrom = path to map from
				lszTo   = path to map to

	RETURNS:	Nothing (failure is silent).

****************************************************************************/

VOID
AddMapping(LPSTR lszFrom, LPSTR lszTo)
{
	int	cchFrom, cchTo;
	char chFrom, chTo;
	char *pchFrom, *pchTo;

	// If our mapping table is full already, just boof it
	if (cmappath == cmappathMax)
		return;

	// Search back from the ends of the paths to see how many characters
	// they have in common

	EnsureFinalBackslash(lszFrom);
	EnsureFinalBackslash(lszTo);

	cchFrom = _tcslen(lszFrom);
	cchTo   = _tcslen(lszTo);
	while (cchFrom > 0 && cchTo > 0)
	{
		if (_tclen(lszFrom + cchFrom) == 1)
		{
			chFrom = _totlower((_TUCHAR)lszFrom[cchFrom]);	// has no effect on single-byte Kana
			pchFrom = &chFrom;
		}
		else
		{
			pchFrom = lszFrom + cchFrom;
		}

		if (_tclen(lszTo + cchTo) == 1)
		{
			chTo = _totlower((_TUCHAR)lszTo[cchTo]);
			pchTo = &chTo;
		}
		else
			pchTo = lszTo + cchTo;

		if (_tccmp(pchFrom, pchTo))
		{
			cchFrom += _tclen(lszFrom + cchFrom);
			cchTo += _tclen(lszTo + cchTo);
			break;
		}

		cchFrom -= _tclen(_tcsdec(lszFrom, lszFrom + cchFrom));
		cchTo -= _tclen(_tcsdec(lszTo, lszTo + cchTo));
	}

	// Search forward until we find a path-separator.  This is because
	// if the original path is "foo" and we're mapping to "boo", we want
	// the mapping to be "foo" => "boo" not "f" => "b".
	while (lszFrom[cchFrom] && !_tcschr(":/\\", (_TUCHAR)lszFrom[cchFrom]))
	{
		cchFrom += _tclen(lszFrom + cchFrom);
		cchTo += _tclen(lszTo + cchTo);
	}
	while (lszFrom[cchFrom] && _tcschr(":/\\", (_TUCHAR)lszFrom[cchFrom]))
	{
		cchFrom++;
		cchTo++;
	}

	rgmappath[cmappath].lszFrom = new char [cchFrom+1];

	if (rgmappath[cmappath].lszFrom)
	{
		_tcsncpy(rgmappath[cmappath].lszFrom, lszFrom, cchFrom);
		rgmappath[cmappath].lszFrom[cchFrom] = '\0';
		rgmappath[cmappath].lszTo = new char [cchTo+1];

		if (rgmappath[cmappath].lszTo)
		{
			_tcsncpy(rgmappath[cmappath].lszTo, lszTo, cchTo);
			rgmappath[cmappath].lszTo[cchTo] = '\0';

			++cmappath;
		}
		else
		{
			delete [] rgmappath[cmappath].lszFrom;
		}
	}
}

/****************************************************************************

	FUNCTION:	FSearchDirMappings()

	PURPOSE:	See if the file being looked for can be found with any of
				our existing directory mappings.

	INPUT:		AlternatePath = path to map from
				lszFilename = filename to look for
				lpfnFMapPathTest = function to call to test if this mapping
					is valid
				lParam = parameter passed to lpfnFMapPathTest, to pass
					any extra info that may be needed.

	RETURNS:	TRUE if a mapping was found, FALSE if not

****************************************************************************/

BOOL
FSearchDirMappings(
	CHAR AlternatePath[_MAX_PATH],
	LPSTR lszFilename,
	LPFNFMAPPATHTEST lpfnFMapPathTest,
	LONG lParam)
{
	int	imappath;
	int cch;
	CHAR szTmp[_MAX_PATH];

	for (imappath=0; imappath<cmappath; ++imappath)
	{
		cch = _tcslen(rgmappath[imappath].lszFrom);

		if (_tcsnicmp(rgmappath[imappath].lszFrom, AlternatePath, cch) == 0)
		{
			_tcscpy(szTmp, rgmappath[imappath].lszTo);
			_tcscat(szTmp, AlternatePath + cch);

			if (lpfnFMapPathTest(szTmp, lszFilename, lParam))
			{
				_tcscpy(AlternatePath, szTmp);
				return TRUE;
			}
		}
	}

	return FALSE;
}

/****************************************************************************

	FUNCTION:	FMapPath()

	PURPOSE:	Maps one path to another, trying to find a file.
				First looks in the list of mappings that we already
				have, then if it can't find a mapping there, prompts
				the user.

	INPUT:		lszProjDir = directory of the project
				AlternatePath = incoming path that we are mapping from
				lszFilename = name of file to find (no path)
				lpdlgparam = special parameter passed on to dialog
				lpfnFMapPathTest = name of callback function which will
					be called after the user types in a directory name,
					to determine if the specified directory is ok.
					It takes two arguments: the path typed by the user,
					and the filename which is being searched for.
				lParam = parameter passed to lpfnFMapPathTest, to pass
					any extra info that may be needed.

	RETURNS:	TRUE if a mapping was found, FALSE if the user pressed
				Cancel.

****************************************************************************/

BOOL
FMapPath(
	const CDir& dirProj,
	CHAR AlternatePath[_MAX_PATH],
	LPSTR lszFilename,
	LPDIALOGPARAM lpdlgparam,
	LPFNFMAPPATHTEST lpfnFMapPathTest,
	LONG lParam)
{
	int				cch;
	CHAR			szMapFrom[_MAX_PATH];
	char FAR *		pPrev;

	// filename only
	ASSERT(_tcschr(lszFilename, _T('\\')) == NULL);

	// if AlternatePath already points to correct path, there's nothing to do
	if (lpfnFMapPathTest(AlternatePath, lszFilename, lParam))
		return TRUE;

	// If the call to lpfnMapPathTest caused an ERR_FILEMAX error,
	// meaning the user has no hope of loading a file, then abort
	// (but don't clear the error flag, because our caller may be
	// interested in it)
	if (CheckGlobalError(FALSE) == ERR_FILEMAX)
		return FALSE;

	// search existing mappings to see if it's there
	EnsureFinalBackslash(AlternatePath);
	if (FSearchDirMappings(AlternatePath, lszFilename, lpfnFMapPathTest, lParam))
		return TRUE;

	if (CheckGlobalError(FALSE) == ERR_FILEMAX)
		return FALSE;

	// we'll have to prompt the user to find the file

	// save away the path that we're mapping from
	_tcscpy(szMapFrom, AlternatePath);

	// remove any trailing backslash from the path that we're going to
	// show the user (unless the path points to a root directory)
	cch = _ftcslen(AlternatePath);
	if (cch > 1 &&
		_tcschr(_T("/\\"), _TUCHAR(*(pPrev = _tcsdec(AlternatePath, AlternatePath + cch)))) &&
		*(_tcsdec(AlternatePath, pPrev)) != _T(':'))
	{
		*pPrev = '\0';
	}
	ClearGlobalError();
	
	BOOL bOk = TRUE;
	
	{	// push current directory & switch to project directory
		CCurDir	curdir(dirProj);

		CDir dir;
		CString strName;
		do
		{
			CFileFindDlg dlgFind(lszFilename, AlternatePath,
				lpdlgparam->lpCaption, lpdlgparam->lpParam);

			dlgFind.SetValidator (lpfnFMapPathTest, lParam);

			if (CheckGlobalError(FALSE) == ERR_FILEMAX)
				return FALSE;

			PreModalWindow();
			bOk = dlgFind.DoModal() != 0;
			PostModalWindow();

			if (bOk)
			{
				strName = dlgFind.GetPathName();
				if (dir.CreateFromString(strName))
					_tcscpy(AlternatePath, dir);
				else
					_tcscpy(AlternatePath, strName);
			}
		}
		while (bOk && !lpfnFMapPathTest(AlternatePath, lszFilename, lParam));
	
	}	// CCurDir object destroyed here, which pops current directory

	// record the new path mapping
	if (bOk)
		AddMapping(szMapFrom, AlternatePath);

	return bOk;
}

/****************************************************************************

	FUNCTION:	ClearPathMappings()

	PURPOSE:	Clears all path mappings that have been created by earlier
				calls to FMapPath.

****************************************************************************/

VOID ClearPathMappings(VOID)
{
	while (cmappath > 0)
	{
		--cmappath;
		delete [] rgmappath[cmappath].lszFrom;
		delete [] rgmappath[cmappath].lszTo;
	}
}


/*****
 * Validator for below
 *****/

PFNVALIDATEDEBUGINFOFILE pfnValidateDebugInfoFile;

BOOL ValidateFoundDebugInfo (LPCTSTR szPath, LPCTSTR szFileName, LONG ) {

	CDir	dir;
	CPath	path;
	ULONG	ulErrorCode;
	
	if (dir.CreateFromString(szPath) &&
		path.CreateFromDirAndFilename(dir, szFileName) &&
		path.ExistsOnDisk()) {
		if (pfnValidateDebugInfoFile) {
			return (*pfnValidateDebugInfoFile) (path, &ulErrorCode);
		} else {
			return TRUE;
		}
	} else {
		return FALSE;
	}
}


/*****
 * SYFindDebugInfoFile,
 *
 * Purpose:
 *		Seek out the debug info as requested
 *
 * Parameters:
 *		PSEARCHDEBUGINFO psdi containing:
 *	
 *			DWORD	cb;							// Side of struct.
 *			BOOL	fMainDebugFile; 			// indicates "core" or "ancilliary" file
 *
 *			LSZ 	szMod;						// exe/dll
 *			LSZ 	szLib;						// lib if appropriate
 *			LSZ 	szObj;						// object file
 *			LSZ *	rgszTriedThese; 			// list of ones that were tried,
 *
 *			PFNVALIDATEDEBUGINFOFILE  
 *					pfnValidateDebugInfoFile;	// validation function
 *
 * 
 * Returns:
 *		TRUE if found
 *		_TCHAR  psdi -> szValidatedFile[_MAX_PATH]; // output of validated filename,
 *	
 *****/
BOOL FAR PASCAL 
SYFindDebugInfoFile (PSEARCHDEBUGINFO psdi) {

	LSZ		szName;
	LSZ		szPath;
	DIALOGPARAM dlgParam;
	CString strPrompt, strTitle;
	CDir	dir;

	ASSERT (psdi -> cb == sizeof (SEARCHDEBUGINFO));
	
	pfnValidateDebugInfoFile = psdi -> pfnValidateDebugInfoFile;



	// We don't do these
	if (psdi -> fMainDebugFile) {
		return FALSE;
	}

	if (psdi -> szMod && _ftcslen (psdi -> szMod)) {
		ASSERT (!psdi -> szLib && !psdi -> szObj);
		szName = psdi -> szMod;
	} else 	if (psdi -> szLib && _ftcslen (psdi -> szLib)) {
		ASSERT (!psdi -> szMod && !psdi -> szObj);
		szName = psdi -> szLib;
	} else 	if (psdi -> szObj && _ftcslen (psdi -> szObj)) {
		ASSERT (!psdi -> szMod && !psdi -> szLib);
		szName = psdi -> szObj;
	} else {
		ASSERT (FALSE); // Gotta pass one of these in
		return FALSE;
	}

	// We can't deal with putting up UI outside of the main thread. 
	// But this could happen if the first thing that causes a lazy 
	// pdb to be pulled in is a return value callback. We'll post a 
	// message that handles it in the main thread.

	if (AfxGetApp () -> m_nThreadID != GetCurrentThreadId())
	{
		SEARCHDEBUGINFOANDRETURN sdir;

		memset (&sdir, 0, sizeof (SEARCHDEBUGINFOANDRETURN));
		sdir.psdi = psdi;
		// Note: this is essentially a recursive call across threads
		PostMessage (hwndFrame, WU_FINDSYMBOLS, 0, (LPARAM)&sdir);

		g_evFindSymbols.Lock();  // Wait until the main thread is done.

		return sdir.fSuccess; // Our result.
	}

    MsgText(strPrompt, IDS_ASKPATH_GETPATH, szName, "");
    VERIFY(strTitle.LoadString(IDS_ASKPATH_CAPTION_SYMBOLS));
    dlgParam.lpParam = (LPSTR) (const char *) strPrompt;
    dlgParam.lpCaption = (LPSTR) (const char *) strTitle;

	dir.CreateFromCurrent();

	BOOL fFound = FMapPath(
		dir,
		psdi -> szValidatedFile,
		szName,
		&dlgParam,
		ValidateFoundDebugInfo,
		0
		);

	if (fFound) {
		// DanS says this next line has side effects.  Hmmm.
		// ASSERT (ValidateFoundDebugInfo (psdi -> szValidatedFile, szName, 0));

		ASSERT (_ftcslen (psdi -> szValidatedFile) + _ftcslen (szName) < _MAX_PATH);
		_ftcscat (psdi -> szValidatedFile, szName);
	}

	return fFound;

}
