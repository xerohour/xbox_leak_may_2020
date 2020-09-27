// dbgnew.cpp
// TODO: change the name of this file -- it's only "new" for 5.0
//
// miscellaneous items falling out of Edit/Dbg split during MSDev 5.0

#include "stdafx.h"
#pragma hdrstop
//#include "inc\dbgnew.h"

// Create an HGLOBAL from the contents of str
HGLOBAL HandleFromString(const CString & str)
{
	HGLOBAL hMem = ::GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, str.GetLength()+1);

	if ( hMem != NULL )
	{
		LPVOID lpVoid = ::GlobalLock(hMem);
		_tcscpy((LPTSTR)lpVoid, str);
		VERIFY(::GlobalUnlock(hMem) == FALSE);
	}
	return hMem;	
}					

// Create an HGLOBAL from the contents of str
HGLOBAL HandleFromString(LPCTSTR str)
{
	HGLOBAL hMem = ::GlobalAlloc(GMEM_DDESHARE|GMEM_MOVEABLE, _tcsnbcnt(str,INT_MAX) + sizeof _TCHAR);

	if ( hMem != NULL )
	{
		LPVOID lpVoid = ::GlobalLock(hMem);
		_tcscpy((LPTSTR)lpVoid, str);
		VERIFY(::GlobalUnlock(hMem) == FALSE);
	}
	return hMem;	
}					


/****************************************************************************

	FUNCTION:   fScanAnyLong

	PURPOSE:    Converts the text form of a long value in
				to the binary representation.  The language
				parameter specifies the expected format of the
				number.  If it is C then decimal, octal (leading 0)
				and hex (leading 0x) are supported.  If it is
				Pascal then decimal and hex (leading $) are
				handled.  If it is AUTO then C is tried first
				and Pascal if the conversion is unsuccessful.

	RETURNS:    TRUE if conversion is successful, FALSE if
				not.  To return TRUE a number must be successfully
				scanned AND it must fall within the passed max and
				min values.

****************************************************************************/

BOOL PASCAL fScanAnyLong(LPCTSTR lpszSource, long *plDest, long lMin, long lMax)
{
	long lLong;
	PSTR pszTmp;
	char *pEnd;
	char szSource[80];

	_ftcsncpy(szSource, lpszSource, sizeof(szSource)-1);
	szSource[sizeof(szSource)-1] = '\0';

	// look for '$' first
	// skip whitespace
	pszTmp = szSource;
	while (whitespace(*pszTmp)) pszTmp++;
	if (*pszTmp == '$')
	{
		pszTmp++;
		lLong = _tcstol(pszTmp, &pEnd, 16);
	}
	else
	{
		lLong = _tcstol(szSource, &pEnd, 0);
	}

	if (*pEnd != '\0') {
		// conversion was stopped because of a bad char
		return FALSE;
	}

	if ((lLong >= lMin) && (lLong <= lMax))
	{
		*plDest = lLong;
		return TRUE;
	}

	return FALSE;

}

void PASCAL freeze_view()
{

	static BOOL fWasAlive = FALSE;

	if ( DebuggeeAlive() )
    {
		if ( fWasAlive )
        {
			CView   *pView = GetCurView();

			if ( pView != NULL )
				pView->SendMessage( WU_FLUSHEDITS, 0, 0L );

			if ( pViewCpu )
				pViewCpu->SendMessage( WU_FREEZEDATA, 0, 0L );

			if ( pViewMemory )
				pViewMemory->SendMessage( WU_FREEZEDATA, 0, 0L );

			if ( g_pVarsView )
				g_pVarsView->SendMessage(WU_FREEZEDATA, 0, 0L );
		}

		fWasAlive = TRUE;
	}
	else
		fWasAlive = FALSE;
}

void EnsureShellNotMinimized()
{
	if (IsIconic(g_hWndShell))
		ShowWindow(g_hWndShell, SW_RESTORE);
}

HWND GetShellWindow()
{
	return g_hWndShell;
}

#ifdef _DEBUG
void _KeyError(LPSTR pszFile, UINT nLine)
{
//	sprintf(szTmp, "_KeyError: Internal Error, File: %Fs, Line: %d",
//			  pszFile, nLine);
//	MsgBox(FatalError, szTmp);
	AfxAssertFailedLine(pszFile, nLine);
}
#endif

HWND GetDebugWindowHandle(UINT uType)
{
    CMultiEdit  *pme = CMultiEdit::m_pList[uType];

    if ( pme )
        return( pme->GetSafeHwnd() );

    return( NULL );
}

CView * PASCAL CreateFile(UINT type, LPSTR pName, LPWININFO win, HFONT font, BOOL readOnly, void far *hsf, BOOL bFromDebugger)
{
    CView   *pView = NULL;
    INT     iLength;

    //Font will be initialized when WM_CREATE msg received for this view
    //Views[view].font = font;

    // if its a .mak file and the corresponding project is opened, set
    // the file read only
    if ( pName )
        iLength = _ftcslen ( pName );

    pView = CreateView(type);

    return( pView );
}

/****************************************************************************

	FUNCTION:   OpenDebugWindow

	PURPOSE:    Opens Cpu, Watch, Locals, or Errors Window under MDI

****************************************************************************/
BOOL PASCAL OpenDebugWindow(UINT winType)
{
	const DOCK_IDS_TAB* pTab = DockTabFromType((DOCTYPE)winType);

	ASSERT(pTab != NULL);

	DkWShowWindow(MAKEDOCKID(PACKAGE_DEBUG, pTab->nID), TRUE);

	return( TRUE );
}

CView *GetCurView(void)
{
    HWND    hWnd = theApp.GetActiveView();
    CView   *pView = (CView *)CWnd::FromHandle(hWnd);

    if ( pView && pView->IsKindOf(RUNTIME_CLASS(CView)) )
        return pView;

	return( NULL );
}

static ERR_TYPE __far_error = 0 ;

// Set global internal far error
VOID PASCAL SetGlobalError( ERR_TYPE errnum, BOOL fForce )
{
	// We don't want this value to be changed until it's
	// explicitely cleared...
	if ( __far_error == ERR_NOERROR || fForce )
		__far_error = errnum ;
}

// Check global internal far error
ERR_TYPE PASCAL CheckGlobalError( BOOL fClear )
{
	ERR_TYPE errT = __far_error ;

	if ( fClear ) __far_error = ERR_NOERROR ;
	return errT ;
}

// Clear the global internal far error
VOID PASCAL ClearGlobalError( VOID)
{
	__far_error = 0 ;
}


static BOOL FFunctionTM(HTM htm)
{
    HTI		hti;
    RTMI	rtmi = {0};
	BOOL fFcn = FALSE;

	if (!EEInfoFromTM ( &htm, &rtmi, &hti )) {
		// get the TI
		ASSERT(hti != (HTI)NULL);

		fFcn = ((PTI)BMLock(hti))->fFunction;
		BMUnlock ( hti );
		EEFreeTI( &hti );
	}

	return fFcn;
}

BOOL FEvalExpr(char *szExpr, char *szOut, int cbOut)
{
	HTM htm = NULL;
	BOOL fCase = TRUE;
	USHORT iEnd;

	// parse expression, if can't be parsed we have no useable TM
	if (EEParse(szExpr, 10, fCase, &htm, &iEnd) != EENOERROR)
	    return FALSE;

	extern struct CXF CxfWatch();

	CXF curCxf(CxfWatch());	
	PCXF pCxf = &curCxf ;
	EEHSTR hstrValue;


	if (EEBindTM(&htm, SHpCXTFrompCXF(pCxf ),TRUE, FALSE) != EENOERROR ||
	    FFunctionTM(htm) ||
	    EEvaluateTM(&htm, SHpFrameFrompCXF(pCxf), EEVERTICAL) != EENOERROR || 
	    EEGetValueFromTM(&htm, ::radix, NULL, &hstrValue) != EENOERROR) {
	    EEFreeTM(&htm);
	    return FALSE;
	}

	EEFreeTM(&htm);

	char *buf = (char *)BMLock(hstrValue);

	if (!buf || !buf[0]) {
	    BMUnlock (hstrValue);
	    EEFreeStr (hstrValue);
	    return FALSE;
	}

	_tcsncpy(szOut, buf, cbOut);
	szOut[cbOut-1] = 0;

	BMUnlock (hstrValue);
	EEFreeStr (hstrValue);

	return TRUE;
}

// FUTURE: What about Fortran?  Basic?  etc...
static const char * rgszAssignments[] = { "-=", "+=", "*=", "/=", "&=", "|=", "^=", 0 };

void RemoveSideEffectsExpr(char *szExpr)
{
	int i = 0;
	char *p;
	while (rgszAssignments[i]) {
		if (p = _tcsstr(szExpr, rgszAssignments[i]))
			*p = 0;
		i++;
	}

	while (p = _tcsstr(szExpr, "++"))
		_tcscpy(p, p+2);

	while (p = _tcsstr(szExpr, "--"))
		_tcscpy(p, p+2);

	p = szExpr;
 	while (p = _tcschr(p, '=')) {
		if (p == szExpr) {
			p++;
			continue;
		}
		if (p[1] == '=') {	// found ==
			p += 2;
			continue;
		}
		
		char *prev = _ftcsdec(szExpr, p);
		if (prev) {
			switch (*prev)
			{
			case '!': case '<':	case '>':
			p++;	 			
			continue;	// found relational !=, <=, >=
			}
		}

		*p = 0;	// must be assignment, nuke it
		break;
	}	
}

/****************************************************************************

	FUNCTION:   FileExist

	PURPOSE:    Checks to see if a file exists with the path/filename
				described by the string pointed to by 'fileName'.

	RETURNS:    TRUE  - if the described file does exist.
				FALSE - otherwise.

****************************************************************************/
BOOL FileExist(LPCSTR fileName)
{
	struct _stat statbuf;

	if (!_stat (fileName, &statbuf) &&
		statbuf.st_mode & _S_IFREG)
	{
		return TRUE;
	}
	return FALSE;
}


// REVIEW [paulde] You're not supposed to attempt this kind
// of filename validation -- you don't really know what file
// systems are installed (Mac, NTFS, NFS, etc.). We need to
// do something different or do without -- just handle errors
// on create/open, etc.

/****************************************************************************

	FUNCTION:   ValidFilename()

	PURPOSE:    Enforces the restrictions listed below.  The purpose of this
				routine is to test that a pathname is "safe" to use with
				file operations for both FAT and NTFS, store in a makefile,
				use on a command line, etc.  The FAT 8.3 limitation is not
				enforced, and there can be more than 2 components in
				a filename or directory name (e.g. c:\a.b.c\a.b.c )

	RETURNS:    TRUE  - if the file pathname is valid.
				FALSE - otherwise.

		FAT restrictions:

			Only '\' or '/' can be path separators

	***		A subdirectory name can only have 2 components separated by a '.'
			   (e.g. first.second)

	***		8.3 for filename+extension

			Illegal FAT characters (for file and directory names):
				* ? " / \ [ ] : | < > + = ; ,

		Windows/DOS restrictions:

			Reserved characters:

			< > : " / \ |  (just a subset of FAT restrictions)


		Add'l restrictions for the IDE:

			No spaces allowed in the pathname (embedded, trailing OR leading)
			(this is mainly due to makefile, compiler utility, and
			 command line processing restrictions)

		NTFS restrictions:

			subdirectory names and filenames must be 124 characters or less
			(empirically determined)

		*** These restrictions are relaxed to provide support for NTFS
			long file and long directory names.



****************************************************************************/

BOOL PASCAL ValidFilename( LPCSTR lpszName, BOOL fWildOkay )
{
	int cLen;
	LPSTR lpstrPrev;

	int cchName= _ftcslen(lpszName);

	// Fail if it exceeds maximum complete pathname length
	if( cchName >= _MAX_PATH )
		return FALSE;

	// Fail if filename empty
	if (cchName == 0)
		return FALSE;

	// Validate drive if specified
	if (cchName >= 2 && lpszName[1] == ':')
	{
		// Check Drive
		if (!((*lpszName >= 'A' && *lpszName <= 'Z') ||
		      (*lpszName >= 'a' && *lpszName <= 'z')))
			return FALSE;
		lpszName += 2;
	}

	// Dolphin #4358: [kherold].  We want to accept UNC paths.  If there's
	// a leading "\\", strip it off.
	if (!_tcsncmp(lpszName, "\\\\", 2))
		lpszName += 2;

	if (*lpszName == '\\' || *lpszName == '/' )
		// skip leading '\'	or '/'
		lpszName++;

	if (*lpszName == '\0')
		return FALSE;

	do
	{
		// Validate characters of next subdirectory name or filename in
		// given pathname
		cLen = 0;
		while (*lpszName != '\0' && *lpszName != '\\' && *lpszName != '/' &&
			   *lpszName != '.')
		{
			cLen += _ftclen(lpszName);
			// Exit with failure if the maximum subdirectory/file name length
			// is exceeded or a non-alphanumeric character is encountered
			if ((unsigned char)*lpszName < 0x20)
				return FALSE;

			// Fail and exit if forbidden character has been entered
			switch (*lpszName)
			{
				case '*':
				case '?':
				case '\"':
				case ':':
				case '|':
				case '<':
				case '>':
					return FALSE;
			}
			lpszName = _ftcsinc(lpszName);
		}

		// If immediately encountered '.' or '\' or '/' or null terminator
		// while trying to parse the next subdirectory or the root filename
		// Possible cases:
		//		prevdir\.\			OK	  -- current directory spec
		//		prevdir\..\			OK	  -- parent directory spec
		//      prevdir\<null>		ERROR -- zero length filename
		//      prevdir\\			ERROR -- zero length subdirectory name
		if (cLen == 0)
		{
			// If we hit a '.'
			if (*lpszName == '.')
			{
				// otherwise a zero length file name has been specified
				// which is an error
				// Skip past if ".\"
				if (*(lpszName+1) == '\\' || *(lpszName+1) == '/' )
				{
					// current directory spec, this is ok
					lpszName +=2;
					goto CONTINUE; // start from after the ".\"
				}
				// Skip past if "..\"
				if ((*(lpszName+1) == '.') &&
				    (*(lpszName+2) == '\\' || *(lpszName+2) == '/'))
				{
					// parent directory spec, this is ok too
					lpszName += 2;
					goto CONTINUE;	//start from after the "..\"
								// while will increment lpszName 1
								// more char
				}
			}

			// Bad subdirectory spec or zero length file name specified
			return FALSE;
		}

		// Now we process the rest of the components
		// of the directory/file name...

		// While still encountering components of the last directory/file name
		while(*lpszName == '.' && *lpszName != '\\' && *lpszName != '/' )
        {
			// Move just past '.'
			lpszName++;

			// Don't allow adjacent '.'s
#if 0		// adjacent '.'s are allowed
			if( *lpszName == '.' )
				return FALSE;
#endif

			// Validate possible extension or component of a subdirectory name
			while (*lpszName != '\0' && *lpszName != '\\' && *lpszName != '/' &&
				   *lpszName != '.')
			{
				// Exit with failure if the max extension length
				// is exceeded or a non-alphanumeric character is encountered
				if ((unsigned char)*lpszName < 0x20)
					return FALSE;

				// Fail and exit if forbidden character has been entered
				switch (*lpszName)
				{
					case '*':
					case '?':
					case '\"':
					case ':':
					case '|':
					case '<':
					case '>':
						return FALSE;
				}
				lpszName = _ftcsinc(lpszName);
			}
		}

CONTINUE:
		lpstrPrev = (char *)lpszName;
		lpszName = _ftcsinc(lpszName);

	}
	while (*lpstrPrev != '\0');

	// Exit with success
	return TRUE;
}

/****************************************************************************

	FUNCTION:   EnsureFinalBackslash

	PURPOSE:    Appends '\' if the string doesn't end in a '\', '/', or ':'.

****************************************************************************/
void EnsureFinalBackslash(LPSTR path)
{
	char *pch;

	if (*path)
	{
		pch = _ftcsdec(path, path + _ftcslen (path));
		if (pch && (_ftcschr ( "/\\:", *(unsigned char *)pch) == NULL))
			_ftcscat( path, "\\");
	}
}

//-NormalizePath----------------------------------------------------------------------
//
// NormalizePath concatenates the project, search and file relative
// path directories, as appropriate, to form the final search directory
// Notice that the 3rd argument must be a relative file name!!

char *NormalizePath (LPSTR lpProjDir,	// Project working directory
					LPSTR lpSearchDir,	// Specified search directory
					LPSTR lpFileDir,	// File path (must be relative)
					LPSTR lpPath, int cbPath)
{
	char	szDrive[_MAX_DRIVE], szDir[_MAX_DIR],
			szRelpath[_MAX_PATH*2];
	char	*pszPath;

	_tsplitpath(lpSearchDir, szDrive, szDir, NULL, NULL);

	// If search is an absolute path
	if( szDir[0] == '\\' || szDir[0] == '/' || szDrive[0])
	{
		// Use the path as is
		_tcscpy(szRelpath, lpSearchDir);
	}
	else
	{
		// Make search path relative to the project directory
		_tcscpy(szRelpath, lpProjDir);
		_tcscat(szRelpath, szDir);
	}

	// Always append the file directory (it will always be relative)
	_tcscat(szRelpath, lpFileDir);

	if( pszPath = _tfullpath(lpPath, szRelpath, cbPath))
	{
		EnsureFinalBackslash(lpPath);
	}
	return pszPath;
}

BOOL IsNonLocalDecimalPoint()
{
	return ( theApp.m_CPLReg.Decimal() == ',' );
}


BOOL FsSetCurDir(LPCTSTR pathName)
{
	// Warning: Win32 SetCurrentDirectory() is "broken"--must use _chdir()
	//DBCS:
	if (pathName[1] == ':')
	{
		char ch = pathName[0];
		ASSERT(isalpha(ch));
		if (_chdrive(toupper(ch) + 1 - 'A') != 0)
			return FALSE;
	}
	BOOL bRet = (_chdir(pathName) == 0);
	// notify the app we changed the current directory
	theApp.OnDirChange(); 
	return bRet;
}


long hidDialog = 0;	// help context ID of the currently active dialog
extern int BoxCount; // count of nested dialogs

/****************************************************************************

	FUNCTION:   StartDialog

	PURPOSE:    Loads and execute the dialog box 'rcDlgNb' (resource
				file dialog id) associated with the dialog
				function 'dlgProc'

	RETURN :    Result of Dialog Box function Call


****************************************************************************/

BOOL StartDialog(UINT rcDlgNb,DLGPROC dlgProc)
{
	return StartDialogParam(rcDlgNb, dlgProc, 0, 0);
}


/****************************************************************************

	FUNCTION:   StartDialogParam

	PURPOSE:    Same as StartDialog, but using the DialogBoxParam Windows
				API function

	RETURN :    Result of Dialog Box function Call


****************************************************************************/

BOOL StartDialogParam (UINT rcDlgNb, DLGPROC dlgProc, LPVOID lpParam, UINT helpID)
{
	C3dDialogTemplate dt;
	VERIFY(dt.Load(MAKEINTRESOURCE(rcDlgNb))); // Cannot find the resource...
	SetStdFont(dt);

	int result;

// other IDE dialogs don't write to the status bar
//	StatusText ( STA_StatusHelpDlg, STATUS_INFOTEXT, TRUE) ;
	BoxCount++;

	long hidDialogOld = hidDialog;
	hidDialog = (helpID != 0 ? helpID : rcDlgNb);
	PreModalWindow();
	VERIFY((result = DialogBoxIndirectParam(hInst,
		(LPCDLGTEMPLATE)dt.GetTemplate(), _SushiGetSafeOwner(NULL),
		dlgProc, (DWORD)lpParam)) != -1);
	PostModalWindow();
	hidDialog = hidDialogOld;

	BoxCount--;
//	StatusText ( SYS_StatusClear, STATUS_INFOTEXT, FALSE );
	return (result);
}

/****************************************************************************

	FUNCTION:	DefaultDialogHandler

	PURPOSE:    All C-language dialogs (those which are not MFC-based) should
				call this function at the end of their DlgProcs, to do default
				dialog handling like responding to the Help button.

	RETURNS:    Just like DlgProcs in general, it returns TRUE if it handles
				the message, and FALSE otherwise.

****************************************************************************/
BOOL DefaultDialogHandler(HWND hDlg, unsigned message, UINT wParam, LONG lParam)
{
	switch(message)
	{
	case WM_COMMAND:
		if (GET_WM_COMMAND_ID(wParam, lParam) != ID_HELP)
			break;

		/* fall through */

	case WM_COMMANDHELP:
		
		ASSERT(hidDialog != 0);
		theApp.HelpOnApplication(hidDialog);
		::SetWindowLong(hDlg, DWL_MSGRESULT, TRUE);
		return TRUE;

#ifdef _DEBUG
	case WM_KEYDOWN:
		ASSERT(wParam != VK_F1);	// this doesn't ever seem to happen
#endif
	}
	return FALSE;
}

