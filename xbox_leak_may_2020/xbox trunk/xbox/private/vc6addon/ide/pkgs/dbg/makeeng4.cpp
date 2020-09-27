#include "stdafx.h"
#pragma hdrstop

#define MAX_ATOM_LEN	1024

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// While debugging, szDebugTarget contains the full path name of the target
// which is currently being debugged (whether it be an executable or a DLL).
// The purpose of this is so that we can remember what the user is actually
// debugging even if the user changes the build target of his project while
// in the middle of a debugging session.
static char FAR szDebugTarget[_MAX_PATH];
static CString	strNull = "";

/****************************************************************************

	FUNCTION:	SzLastExeWindow()

	PURPOSE:	If the IDE currently has any windows open which are viewing
				an EXE (viewing resources through the VRES package), this
				will function return a pointer to the EXE name.  Otherwise
				it will return NULL.

	RETURNS:	A full path to an EXE, or NULL.

****************************************************************************/

const CString& SzLastExeWindow(VOID)
{
	CMDIChildWnd *pMDIChildWnd = ((CMDIFrameWnd *) theApp.m_pMainWnd)->MDIGetActive();

	if (pMDIChildWnd)
	{
		CDocument * pDoc = pMDIChildWnd->GetActiveDocument();
		if ( pDoc != NULL )
		{
			const CString& strPath = pDoc->GetPathName();

			if (strPath.Right(4).CompareNoCase(_T(".exe")) == 0)
				return strPath;
		}
	}
	return strNull;
}

/****************************************************************************
	FUNCTION:	GetLastDocWin

	PURPOSE:	Get a pointer to the last document whose window was active.
				This is primarily for VPROJ's benefit, although VCPP could
				conceivably make use of it as well.

	RETURNS:	A pointer to the last active document, or NULL if none.
****************************************************************************/

IDE_EXPORT CDocument * GetLastDocWin()
{
	if (pLastDocWin == NULL)
		return NULL;

	return(pLastDocWin);
}

/****************************************************************************

	FUNCTION:	GetExeOrTarg

	PURPOSE:	Gets the full path name of either the current executable file
				or the current target - from the project if there is one, else
				from the currently open exe if there is one, else from the
				current source file.

				If you are only interested in the return code (exefrom), you
				can pass NULL for the 'executable' argument.

				The difference between the current executable and the current
				target is, the target is what the makefile will build; the
				executable is what the debugger will run (never a DLL).

	RETURNS:	An enum value to indicate where the EXE name came from:
					exefromProj if it comes from the current project
					exefromExe if it comes from an open AppStudio window
							which is looking at the EXE's resources
					exefromLastDoc if it comes from the last document window
							to have had focus
					exefromPid if it comes from the PID on the command line
							(just-in-time debugging)
					exefromNone if no exe was found.
				Filename is returned in ANSI charset.

****************************************************************************/

EXEFROM PASCAL GetExeOrTarg ( PSTR executable, UINT size, BOOL fTarget )
{
	EXEFROM exefrom = exefromNone;
	UINT NumToCopy;
	TCHAR szPath[_MAX_PATH];

	// If asking for EXE, see if we're doing just-in-time debugging
	if (!fTarget && theApp.m_jit.GetActive())
    {
		if (executable)
        {
			_tcsncpy(executable, (LPCTSTR)theApp.m_jit.GetPath(), size);
			executable[size-1] = '\0';
		}
		return exefromPid;
	}
	else if ( gpIBldSys && (gpIBldSys->IsActiveBuilderValid() == S_OK) )
	{
		exefrom = exefromProj;

		// get the caller executable from the project
		CString str;

	    gpIBldSys->GetCallingProgramName(ACTIVE_BUILDER, str);
		_tcscpy(szPath, (LPCTSTR)str);

		if (*szPath == '\0')
			return exefromNone;

		// strip quotes
		str = szPath;
		if (!str.IsEmpty() && str[0]==_T('\"'))
		{
			str = str.Mid(1, str.GetLength()-2);
			_ftcscpy(szPath, str);
		}
	}
	else
	{
		const CString& szExe = SzLastExeWindow();

		if (!szExe.IsEmpty())
		{
			if (executable)
			{
				_ftcsncpy(executable, (LPCTSTR)szExe, size);
				executable[size-1] = '\0';
			}
			return exefromExe;
		}
		else
		{
			// Get current document
			if (pLastDocWin != NULL)
			{
				exefrom = exefromLastDoc;

				if (executable)
				{
					// strip the extension and replace with .exe
					_ftcscpy(szPath, (LPCTSTR)pLastDocWin->GetPathName());
					_splitpath(szPath, szDrive, szDir, szFName, szExt);
					if (_tcsicmp(szExt, _T(".java")) == 0)
						_makepath(szPath, szDrive, szDir, szFName, _T("class"));
					else
						_makepath(szPath, szDrive, szDir, szFName, "exe");
				}
			}
			else
			{
				// no executable available
				return( exefromNone );
			}
		}
	}

	// If we get to here szPath contains what we want
	// Copy it to our passed buffer
	if (executable)
	{
		NumToCopy = min(_ftcslen(szPath)+1, size-1);
		_ftcsncpy(executable, szPath, NumToCopy);
		executable[NumToCopy-1] = '\0';
	}

	ASSERT(exefrom != exefromNone);
	return exefrom;
}

/****************************************************************************

	FUNCTION:	GetExecutableFilename

	PURPOSE:	Gets the full path name of the current executable file -
				from the project if there is one, or from the current
				source file otherwise.

				If you are only interested in the return code (exefrom), you
				can pass NULL for the 'executable' argument.

	RETURNS:	An enum value to indicate where the EXE name came from:
					exefromProj if it comes from the current project
					exefromExe if it comes from an open AppStudio window
							which is looking at the EXE's resources
					exefromPid if it comes from the PID on the command line
							(just-in-time debugging)
					exefromLastDoc if it comes from the last document window
							to have had focus
					exefromNone if no exe was found.
				Filename is returned in ANSI charset.

****************************************************************************/

IDE_EXPORT EXEFROM GetExecutableFilename ( PSTR executable, UINT size )
{
	return GetExeOrTarg(executable, size, FALSE);
}

/****************************************************************************

	FUNCTION:	GetDebuggeeCommandLine

	PURPOSE:	Gets the command line

****************************************************************************/
void PASCAL GetDebuggeeCommandLine(CString& CommandLine)
{
	// get the command-line from the project
	if ( gpIBldSys && gpIBldSys->IsActiveBuilderValid() == S_OK )
	{
		gpIBldSys->GetProgramRunArguments(ACTIVE_BUILDER, CommandLine);
	}
	else
	{
		ASSERT(theApp.m_jit.GetActive());
		CommandLine = "";
	}
}

/****************************************************************************

	FUNCTION:	GetDebugTarget

	PURPOSE:	Gets the full path name of the file which is currently being
				debugged (or would be debugged if a debugging session were
				started).  This is slightly different from GetActiveProject()
				->GetTargetFilename(), in that if the user starts a debugging
				session and then modifies his project to build a different
				target, GetDebugTarget() will still get the name of the
				file which is actually being debugged.

	RETURNS:	TRUE if successful.  Filename is returned in ANSI charset.

****************************************************************************/

BOOL PASCAL GetDebugTarget(PSTR szTarget, UINT cch)
{
	// Check for a current debuggee first
	if (DebuggeeAlive())
	{
		if (theApp.m_jit.GetActive() && theApp.m_jit.FPathIsReal())
			_ftcsncpy(szTarget, (const TCHAR *)theApp.m_jit.GetPath(), cch);
		else
			_ftcsncpy(szTarget, szDebugTarget, cch);
		szTarget[cch-1] = '\0';

		return( TRUE );
	}
	else
		return (GetExeOrTarg(szTarget, cch, TRUE) != exefromNone);
}

/****************************************************************************

	FUNCTION:	SetDebugTarget

	PURPOSE:	Sets the filename of the target that's currently being
				debugged.  The only purpose of this is so that
				GetDebugTarget knows the name of the current target.

****************************************************************************/

VOID PASCAL SetDebugTarget(PCSTR szTarget)
{
	_ftcsncpy(szDebugTarget, szTarget, sizeof(szDebugTarget));
	szDebugTarget[sizeof(szDebugTarget)-1] = '\0';
}
