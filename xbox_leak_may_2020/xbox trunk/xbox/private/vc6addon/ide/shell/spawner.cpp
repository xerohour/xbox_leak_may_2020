// spawner.cpp
//
// low-level build routines to spawn tools

#include "stdafx.h"

#include "toolexpt.h"
#include "util.h"
#include "dllapi.h"
#include "resource.h"
#include "shell.h"

#include <tchar.h>
#include <string.h>
#include <bldapi.h>

#include "utilauto.h"

//REVIEW(PatBr): Moved from spawner.h because of conflicts.
#define MAX_TOOL_MACRO  16					// max length of macro name
#define MAX_LINE_SIZE   2048
#define MAX_USER_LINE   MAX_LINE_SIZE
#define TMP_STRING_SIZE MAX_LINE_SIZE + 60	// all purpose strings

struct TERROR
{
	LPLONG		lpTop;
	LPLONG		lpStack;
	LPLONG		lpEnd;
	UINT		nErrorMin;
	UINT		nErrorMax;
	KEYENTRY
};

PfnReadPipe pfnReadPipe = (GetVersion() & 0x80000000) ? FWin95ReadPipe : FWinNTReadPipe;

// characters that indicate to us that we need to spawn a command
// processor in front of tools (redirection and command chaining)
const char rgchCmdProcChar[] = "<>|";

LPSTR stristr(LPSTR szSearch, LPSTR szReqd)
{
	// case insensitive string inclusion (like _ftcsstr but insensitive)
	LPSTR pszRet = NULL;

	TCHAR *pszSrchTmp;
	TCHAR *pszReqdTmp;

	pszSrchTmp = (TCHAR *)malloc(_tcslen(szSearch)+1);
	pszReqdTmp = (TCHAR *)malloc(_tcslen(szReqd)+1);

	char *pstr;

	_ftcslwr(_ftcscpy(pszSrchTmp, szSearch));
	_ftcslwr(_ftcscpy(pszReqdTmp, szReqd));

	pstr = _ftcsstr(pszSrchTmp, pszReqdTmp);
	if (pstr)
		pszRet = (pstr - pszSrchTmp) + szSearch;

	free(pszSrchTmp);
	free(pszReqdTmp);

	return(pszRet);
}

BOOL SetDriveAndDir(LPCTSTR szPath)
{
	// this function only used to reset current drive and directory after
	// running a user-defined tool, so assume the string contains only 
	// drive and directory components--no file or extension present.
	char szTmp[_MAX_PATH];
	char szDrive[4];

	// if string is blank, get out
	if (*szPath == '\0')
		return TRUE;

	_ftcscpy(szTmp, szPath);

	// Set current drive and dir
	_splitpath(szTmp, szDrive, NULL, NULL, NULL);

	if (szDrive[0] != 0)
	{
		if (_chdrive((int)(_totupper((_TUCHAR)(szDrive[0])) - 'A' + 1)) != 0)
			return ErrorBox(IDS_ERR_CHANGE_DRIVE, (LPSTR)szDrive);
	}

	if (_chdir(szTmp) != 0)
		return ErrorBox(IDS_ERR_CHANGE_DIR, (LPSTR)szTmp);

	return TRUE;
}

LPCTSTR ParseMacroName(LPTSTR szMacro, LPCTSTR szIn, UINT cbMax)
{
	LPTSTR szOut = szMacro;

	// '$(' always single-cell characters
	if (*szIn != _T('$') || *(szIn + 1) != _T('('))
		return NULL;

	// copy the $(
	_tccpy(szOut, szIn);
	_tccpy(szOut + 1, szIn + 1);
	szOut += 2;
	szIn += 2;

	// Get possible macro name
	while (*szIn != _T(')'))
	{
		// copy char
		_tccpy(szOut, szIn);
		szOut = _tcsinc(szOut);
		szIn = _tcsinc(szIn);
	}
	ASSERT(*szIn == ')');
	ASSERT(szOut < (char*)szMacro + cbMax);
	// copy ')' char
	_tccpy(szOut, szIn);
	szOut = _tcsinc(szOut);
	szIn = _tcsinc(szIn);
	*szOut = _T('\0');

	return(szIn);
}

void RemoveTrailing(LPTSTR sz, _TCHAR ch = _T('\\'))
{
	ASSERT(sz != NULL);
	LPTSTR pch = sz + _tcslen(sz) - 1;
	while (pch > sz && *pch == ch)
	{
		if (pch - 1 > sz && !_istleadbyte(*(pch - 1)))
			*pch-- = 0;
	}
}

int GetProjectRCFile(TCHAR *lpRCFile)
{
	LPBUILDSYSTEM pBldSysIFace = g_IdeInterface.GetBuildSystem();

	if (pBldSysIFace != NULL)
	{
		// can we return a name? (can't do this for multiple .RC files!)
		CPtrList ptrlist;
		pBldSysIFace->GetResourceFileList(ptrlist, FALSE, NULL, ACTIVE_BUILDER);
		if (ptrlist.GetCount() == 1)
		{
			const CPath *pResPath = (const CPath *)ptrlist.GetHead();
			ASSERT(pResPath);
			_ftcscpy(lpRCFile, pResPath->GetFullPath());
			return _ftcslen(lpRCFile);
		}
	}
	return 0;	// no RC file...
}

BOOL ExpandToolArguments(LPSTR szArg, size_t size)
{
	char temp[TMP_STRING_SIZE];
	char szPath[_MAX_PATH], szDrive[_MAX_DRIVE], szDir[_MAX_DIR],
		 szFName[_MAX_FNAME], szExt[_MAX_EXT];
	char MacroName[MAX_TOOL_MACRO+1];
	UINT iOut;
	int i,lng;
	BOOL iRet = TRUE;
	int cb;
	BOOL bMacro;
	LPSTR szSrc;
	static BOOL bLoaded = FALSE;
	static _TCHAR Names[IDS_TOOLMACRO_Last-IDS_TOOLMACRO_First+1][MAX_TOOL_MACRO+1];

	if (!bLoaded)
	{
		for (int i = 0; i <= IDS_TOOLMACRO_Last-IDS_TOOLMACRO_First; i++)
			VERIFY(LoadString(GetResourceHandle(), i + IDS_TOOLMACRO_First, (LPTSTR)Names[i], MAX_TOOL_MACRO+1));
		bLoaded = TRUE;
	}

	ASSERT(szArg != NULL);
	szSrc = szArg;

	// we want to save one byte for the terminating '\0'
	size -= 1;

	iOut=0;
	while (*szArg != _T('\0') && (iOut + _tclen(szArg) < size) && iRet == TRUE)
	{
		bMacro = FALSE;
		// parse macro (if any) from current position
		LPCTSTR szArgNext = ParseMacroName(MacroName, szArg, MAX_TOOL_MACRO);
		if (szArgNext != NULL)
		{
			// search for macro name
			for (i = 0; i <= IDS_TOOLMACRO_Last - IDS_TOOLMACRO_First; i++)
			{
				if (!_tcsicmp(MacroName, Names[i]))
				{
					_tcscpy(MacroName, Names[i]);
					bMacro = TRUE;
					break;
				}
			}
		}

		// If no macro or not a known macro name, copy char and keep scanning
		if (!bMacro)
		{
			cb = _tclen(szArg);
			_tccpy(temp + iOut, szArg);
			iOut += cb;
			szArg += cb;
		}
		else
		{
			// advance input 
			szArg = (LPSTR)szArgNext;
			// Expand macro
			CPartView *pView = (CPartView *)CWnd::FromHandle(theApp.GetActiveView());

			switch(i)
			{
				case IDS_TOOLMACRO_RCFile-IDS_TOOLMACRO_First: // Current RC File in project
				{
					TCHAR szRC[_MAX_PATH] ;

					if (lng = GetProjectRCFile(szRC))
					{
						if (iOut+lng <size)
						{
							_ftcscpy(&temp[iOut], szRC) ;
							iOut += lng;
						}
						else
						{
							ErrorBox(IDS_ERR_EXPAND_MACRO, (LPSTR)MacroName);
							iRet = FALSE;
						}
					}
					break ;
				}

				case IDS_TOOLMACRO_FilePath-IDS_TOOLMACRO_First:	// file name (full)
				case IDS_TOOLMACRO_FileNameExt-IDS_TOOLMACRO_First:	// file base name + ext
				case IDS_TOOLMACRO_FileDir-IDS_TOOLMACRO_First:		// file directory + drive
				case IDS_TOOLMACRO_FileName-IDS_TOOLMACRO_First:	// file base name
				case IDS_TOOLMACRO_FileExt-IDS_TOOLMACRO_First:		// file extension
				{
					if (pView)
					{
						CPartDoc *pDoc = (CPartDoc *)(pView->GetDocument());
						if(pDoc)
						{
							if (pDoc->GetPathName().IsEmpty())
							{
								if (!pDoc->DoSave(pDoc->GetPathName()))
								{
									iRet = FALSE;
									break;
								}
								else	// Don't have a name, and don't want to save -- can't continue
								{
									CString strMsg;
									strMsg.LoadString(IDS_ERR_NO_DISK_IMAGE);
									MessageBox(NULL, (LPCTSTR)strMsg, NULL, MB_ICONEXCLAMATION|MB_OK);
									iRet = FALSE;
									break;
								}
							}

							_ftcscpy(szPath, (LPCTSTR)pDoc->GetPathName());

							if (i != IDS_TOOLMACRO_FilePath - IDS_TOOLMACRO_First)
							{
								_splitpath(szPath, szDrive, szDir, szFName, szExt);
								switch (i)
								{
									case IDS_TOOLMACRO_FileDir - IDS_TOOLMACRO_First:
										_ftcscpy(szPath, szDrive);
										RemoveTrailing(szDir);
										_ftcscat(szPath, szDir);
										break;

									case IDS_TOOLMACRO_FileName - IDS_TOOLMACRO_First:
										_ftcscpy(szPath, szFName);
										break;

									case IDS_TOOLMACRO_FileExt - IDS_TOOLMACRO_First:
										_ftcscpy(szPath, szExt);
										break;

									case IDS_TOOLMACRO_FileNameExt - IDS_TOOLMACRO_First:
										_ftcscpy(szPath, szFName);
										_ftcscat(szPath, szExt);
										break;
								}
							}

							lng = _ftcslen(szPath); // Get name

							if (iOut+lng < size) // is there enough space in dest string ?
							{
								_ftcscpy(&temp[iOut],szPath);
								iOut += lng;
								break;
							}
							else
							{
								ErrorBox(IDS_ERR_EXPAND_MACRO, (LPSTR)MacroName);
								iRet = FALSE;
							}
						}
					}
					break;
				}

				case IDS_TOOLMACRO_Dir		-IDS_TOOLMACRO_First:	// Current working directory
				case IDS_TOOLMACRO_WkspName	-IDS_TOOLMACRO_First:	// Workspace base name
				case IDS_TOOLMACRO_WkspDir	-IDS_TOOLMACRO_First:	// Workspace directory

					if (i == IDS_TOOLMACRO_Dir - IDS_TOOLMACRO_First)
					{
						_getcwd(szPath, _MAX_PATH);
						RemoveTrailing(szPath);
					}
					else
					{
						LPCSTR pszProjPath;
						LPBUILDSYSTEM pBldSysIFace = g_IdeInterface.GetBuildSystem();

						if ((pBldSysIFace == NULL) || (pBldSysIFace->IsActiveBuilderValid() != S_OK) || (FAILED(pBldSysIFace->GetBuilderFile(ACTIVE_BUILDER, &pszProjPath))) || (pszProjPath == NULL))
							break;

						_ftcscpy(szPath, pszProjPath);

						if ((i == IDS_TOOLMACRO_WkspDir - IDS_TOOLMACRO_First))
						{
							_splitpath(szPath, szDrive, szDir, NULL, NULL);

							_ftcscpy(szPath, szDrive);
							RemoveTrailing(szDir);
							_ftcscat(szPath, szDir);
						}
						else if ((i == IDS_TOOLMACRO_WkspName - IDS_TOOLMACRO_First))
						{
							_splitpath(szPath, NULL, NULL, szFName, NULL);

							_ftcscpy(szPath, szFName);
						}
					}

					lng = _ftcslen(szPath);

					if (lng != 0)
					{
						if((iOut+lng) < size)
						{
							_ftcsncpy(&temp[iOut],szPath,lng);
							iOut += lng;
						}
						else
						{
							ErrorBox(IDS_ERR_EXPAND_MACRO, (LPSTR)MacroName);
							iRet = FALSE;
						}
					}
					break;

				case IDS_TOOLMACRO_Line-IDS_TOOLMACRO_First:  // Line number
				case IDS_TOOLMACRO_Col-IDS_TOOLMACRO_First:   // Column number
					// don't display line number if no view active
					if (pView == NULL)
						break;
					else if (iOut + 5 < size) // is there enough space in dest string ?
					{
						LPSOURCEQUERY pSourceQuery;
						theApp.FindInterface(IID_ISourceQuery, (LPVOID FAR *)&pSourceQuery);
						if (pSourceQuery == NULL)
						{
							break;
						}
						LPSOURCEEDIT pSourceEdit;
						pSourceQuery->CreateSourceEditForActiveDoc(&pSourceEdit, FALSE);
						if (pSourceEdit == NULL)
						{
							pSourceQuery->Release();
							break;
						}
						if (i == IDS_TOOLMACRO_Line-IDS_TOOLMACRO_First)
						{
							ULONG ulLine;
							pSourceEdit->GetCaretLine(&ulLine);
							iOut += wsprintf((LPSTR)&temp[iOut], "%d", (ulLine + 1));
						}
						else
						{
							ULONG ulColumn;
							pSourceEdit->GetCaretColumn(&ulColumn);
							iOut += wsprintf((LPSTR)&temp[iOut], "%d", (ulColumn + 1));
						}
						pSourceEdit->Release();
						pSourceQuery->Release();
					}
					else
					{
						ErrorBox(IDS_ERR_EXPAND_MACRO, (LPSTR)MacroName);
						iRet = FALSE;
					}
					break;

				case IDS_TOOLMACRO_CurText-IDS_TOOLMACRO_First:  // Current text
				{
					CString str;
					if (pView == NULL || !pView->GetHelpWord(str))
						break;
					// note that str could easily be empty at this point
					if (iOut + str.GetLength() < size)
					{
						_ftcsncpy(&temp[iOut], str, str.GetLength());
						iOut += str.GetLength();
					}
					else
					{
						ErrorBox(IDS_ERR_EXPAND_MACRO, (LPSTR)MacroName);
						iRet = FALSE;
					}
					break;
				}

				case IDS_TOOLMACRO_TargetPath-IDS_TOOLMACRO_First:	// target
				case IDS_TOOLMACRO_TargetDir-IDS_TOOLMACRO_First:	// target directory
				case IDS_TOOLMACRO_TargetName-IDS_TOOLMACRO_First:	// target name
				case IDS_TOOLMACRO_TargetExt-IDS_TOOLMACRO_First:	// target name
				{
					LPTSTR pszTargPath;
					LPBUILDSYSTEM pBldSysIFace = g_IdeInterface.GetBuildSystem();

					if ((pBldSysIFace == NULL) || (pBldSysIFace->IsActiveBuilderValid() != S_OK) || (FAILED(pBldSysIFace->GetTargetFileName(ACTIVE_BUILDER, &pszTargPath))) || (pszTargPath == NULL))
						break;

					_ftcscpy(szPath, pszTargPath);
					delete [] pszTargPath;

					if (i != IDS_TOOLMACRO_TargetPath - IDS_TOOLMACRO_First)
					{
						_splitpath(szPath, szDrive, szDir, szFName, szExt);
						switch (i)
						{
						case IDS_TOOLMACRO_TargetDir - IDS_TOOLMACRO_First:
							_ftcscpy(szPath, szDrive);
							RemoveTrailing(szDir);
							_ftcscat(szPath, szDir);
							break;
						case IDS_TOOLMACRO_TargetName - IDS_TOOLMACRO_First:
							_ftcscpy(szPath, szFName);
							break;
						case IDS_TOOLMACRO_TargetExt - IDS_TOOLMACRO_First:
							_ftcscpy(szPath, szExt);
							break;
						}
					}

					lng = _ftcslen(szPath);

					if((iOut+lng) < size)
					{
						_ftcscpy(&temp[iOut], szPath);
						iOut += lng;
					}
					else
					{
						ErrorBox(IDS_ERR_EXPAND_MACRO, (LPSTR)MacroName);
						iRet = FALSE;
					}

					break;
				}

				case IDS_TOOLMACRO_TargetArgs-IDS_TOOLMACRO_First: // command line arguments
				{
					CString ProjArgs;
					LPBUILDSYSTEM pBldSysIFace = g_IdeInterface.GetBuildSystem();

					if ((pBldSysIFace == NULL) || (pBldSysIFace->IsActiveBuilderValid() != S_OK))
						break;

					pBldSysIFace->GetProgramRunArguments(ACTIVE_BUILDER, ProjArgs);

					lng = _ftcslen(LPCTSTR(ProjArgs));
					if((iOut+lng) < size)
					{
						_ftcscpy(&temp[iOut], LPCTSTR(ProjArgs));
						iOut += lng;
					}
					else
					{
						ErrorBox(IDS_ERR_EXPAND_MACRO, (LPSTR)MacroName);
						iRet = FALSE;
					}
					break;
				}
			}
		}
	}

	temp[iOut] = '\0';
	_tcscpy (szSrc, temp) ;
	return(iRet);
}

BOOL DoSaveBeforeSpawn()
{
	BOOL bPrompt = FALSE;
	// prompt before saving unsaved documents?
	LPSOURCEQUERY pInterface = g_IdeInterface.GetSourceQuery();
	if (pInterface != NULL && pInterface->IsPromptBeforeSaving() == S_OK)
		bPrompt = TRUE;

	/*
		DS96 #17352 [CFlaat]: don't record the save all, so that 'ExecuteCommand "[toolbar]"' will get recorded
	*/
	HALT_RECORDING();

	// save non-debugger files
	return(theApp.SaveAll(bPrompt, FILTER_DEBUG));
}

CDllApi::CDllApi(const _TCHAR *szDll, MPSZPFN *mpszpfn)
{
	_mpszpfn = mpszpfn;
	_tcscpy(_szDll, szDll);
	_fRoutinesOK = FALSE;
	_cRoutines = 0;

	HINSTANCE hInstance = LoadLibrary(szDll);

	if (hInstance)
	{
		_hInstance = hInstance;

		MPSZPFN *pmpszpfn;
		BOOL fRoutinesOK = TRUE;

		for (pmpszpfn = _mpszpfn; pmpszpfn->szRoutine; pmpszpfn++)
		{
			fRoutinesOK = fRoutinesOK && 
				(pmpszpfn->pfn = GetProcAddress(hInstance, pmpszpfn->szRoutine));
			_cRoutines++;
		}
		_fRoutinesOK = fRoutinesOK;
	}
}

CDllApi::~CDllApi()
{
	if (_hInstance)
		FreeLibrary(_hInstance);
	_hInstance = 0;
	_fRoutinesOK = FALSE;
}

BEGIN_MPSZPFN(Daytona)
MPSZPFN_FCN(SetProcessWorkingSetSize)
MPSZPFN_FCN(GetProcessWorkingSetSize)
END_MPSZPFN(Daytona)

#define ipfnSetWSSize  0
#define ipfnGetWSSize  1

typedef BOOL (WINAPI *PfnSetWSSize)(HANDLE, DWORD, DWORD);

#define cbTrimWorkingSet 0xffffffff

DECLARE_DLLAPI(dllApiDaytona, Daytona, kernel32.dll);

BOOL SetWorkingSetSize(DWORD cbMinWS, DWORD cbMaxWS)
{
	if (dllApiDaytona.FRoutinesLoaded())
		return PfnSetWSSize(dllApiDaytona(ipfnSetWSSize))(GetCurrentProcess(), cbMinWS, cbMaxWS);
	return FALSE;
}

//-----------------------------------------------------------------------------
// FWin95ReadPipe
//
// Necessary function to be able to always return from a ReadFile on a pipe
// when Win95 doesn't realize the pipe has been broken.  We use an auxilliary
// event to stop on.  In this case, it will be the handle to the process that
// we spawn.  When both the process handle is signalled AND the pipe is empty,
// we consider the connection broken.  Otherwise, we pseudo-block until either
// there is data to be read (and we read it) or the process is gone.
//-----------------------------------------------------------------------------
BOOL FWin95ReadPipe(HANDLE hpipe, LPVOID pvBuf, DWORD cbToRead, DWORD &cbRead, HANDLE hAuxStop)
{
	DWORD cbAvail = 0;
	
	// while the pipe is still valid, we don't have data, and the aux stop event
	// is not signalled, we pseudo-block
	while (PeekNamedPipe(hpipe, NULL, 0, NULL, &cbAvail, NULL) && cbAvail == 0 &&
			WaitForSingleObject(hAuxStop, 0) == WAIT_TIMEOUT)
	{
		Sleep(10);	// give up our timeslice in a pseudo-block
	}

	cbRead = 0;
	if (cbAvail)
	{
		return(ReadFile(hpipe, pvBuf, __min(cbToRead, cbAvail), &cbRead, NULL));
	}

	// else, the only other reason to get out of the loop above is if the pipe
	// has been broken or the aux stop event is signalled.
	return FALSE;
}

//-----------------------------------------------------------------------------
// FWinNTReadPipe
//
// Thunk to ReadFile
//-----------------------------------------------------------------------------
BOOL FWinNTReadPipe(HANDLE hpipe, LPVOID pvBuf, DWORD cbToRead, DWORD &cbRead, HANDLE /*hAuxStop*/)
{
	return(ReadFile(hpipe, pvBuf, cbToRead, &cbRead, NULL));
}

/////////////////////////////////////////////////////////////////////////////
// CConsoleSpawner

BOOL CConsoleSpawner::m_fUserWarned = FALSE;

// REVIEW(KPERRY) The spawner used to use the same string for 2 try's at
// launching the spawner. To minimize changes I added the NoQuote version
// of the string and left the %s in for no good reason. we can remove it
// and the coresponding blank argument to sprintf later on in the code at
// a later time.


CHAR CConsoleSpawner::m_szSpawnTool[] = "\"%svcspawn\" -e %lu %s ";
CHAR szSpawnToolNoQuote[] = "%svcspawn -e %lu %s ";

unsigned _stdcall ReadChildProcessOutput(LPVOID lpv)
{
	CConsoleSpawner::ReadChildProcessOutput2(lpv);
	_endthreadex(0);
	return 0;
}

void _cdecl CConsoleSpawner::ReadChildProcessOutput(LPVOID lpv){};

void _cdecl CConsoleSpawner::ReadChildProcessOutput2(LPVOID lpv)
{
	CConsoleSpawner *pSpawner = (CConsoleSpawner *)lpv;
	DWORD cb = 0;	// bytes in output buffer
	char rgchProcOutputBuf[cbProcOutputBuf];

	do {
		// wait until process has been spawned
		TRACE("spawner 0x%08x : waiting for process spawn event\n", pSpawner);
		WaitForSingleObject(pSpawner->m_hSpawnEvent, INFINITE);

		TRACE("spawner 0x%08x : returned from wait for process spawn event\n", pSpawner);
		if (pSpawner->m_fThreadDie)
			break;

		TRACE("spawner 0x%08x : going to read from pipe handle = 0x%08x\n", pSpawner, pSpawner->m_hReadPipe);

		// read pipe until it's empty
		while ((*pfnReadPipe)(pSpawner->m_hReadPipe, rgchProcOutputBuf, cbProcOutputBuf - 1,
						cb, pSpawner->m_procinfo.hProcess) && cb > 0)
		{
			// append '\0' to end of buffer
			// NOTE: always okay because only filled up to cbProcOutputBuf-1 bytes
			rgchProcOutputBuf[cb] = '\0';

			// write buffer to output window
			pSpawner->DisplayOutputFromConsole(rgchProcOutputBuf);
		}

		TRACE("spawner 0x%08x : GetLastError() from ReadFile = 0x%08x\n", pSpawner, GetLastError());

		// indicate that process finished output
		SetEvent(pSpawner->m_hDoneEvent);

		// send window message to main thread
		// PostMessage(hwndFrame, WM_NULL, 0, 0);
	} while (TRUE);

	TRACE("spawner 0x%08x : ReadChildProcessOutput thread committing suicide\n", pSpawner);
}

void _cdecl CConsoleSpawner::WaitChildProcessFinish(LPVOID lpv)
{
	CConsoleSpawner *pSpawner = (CConsoleSpawner *)lpv;

	// wait until process has been terminated
	TRACE("spawner 0x%08x : waiting for process to terminate\n", pSpawner);
	WaitForSingleObject(pSpawner->m_procinfo.hProcess, INFINITE);
	TRACE("spawner 0x%08x : returned from wait for process to terminate\n", pSpawner);

	GetExitCodeProcess(pSpawner->m_procinfo.hProcess, &(pSpawner->m_dwTermStatus));

	pSpawner->m_fThreadDie = TRUE;
	SetEvent(pSpawner->m_hSpawnEvent);

	TRACE("spawner 0x%08x : waiting for ReadChildProcessOutput thread to terminate\n", pSpawner);
	WaitForSingleObject(pSpawner->m_hSpawnThread, INFINITE);
	TRACE("spawner 0x%08x : returned from wait for ReadChildProcessOutput thread to terminate\n", pSpawner);
	CloseHandle(pSpawner->m_hSpawnThread);

	// if there's text in the output hold buffer (m_szOutput), flush it now
	if (pSpawner->m_ichOutput)
		pSpawner->DisplayOutputFromConsole("\r\n");

	if (pSpawner->m_hNotifyEvent != NULL)
	{
		// assume that notified owner will output termination messages
		ASSERT(pSpawner->m_hCleanEvent == NULL);
		pSpawner->m_hCleanEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		ASSERT(pSpawner->m_hCleanEvent != NULL);
		SetEvent(pSpawner->m_hNotifyEvent);
		WaitForSingleObject(pSpawner->m_hCleanEvent, INFINITE);
		ASSERT(pSpawner == *(pSpawner->m_ppSpawner));
		*(pSpawner->m_ppSpawner) = NULL;
		VERIFY(CloseHandle(pSpawner->m_hNotifyEvent));
		VERIFY(CloseHandle(pSpawner->m_hCleanEvent));
	}
	else
	{
		// check to see if we killed the process so we can put a message out
		if (WaitForSingleObject(pSpawner->m_hCancelEvent, 0) == WAIT_OBJECT_0)
		{
			pSpawner->m_strOutput.LoadString(IDS_TOOL_STOP_MSG);
		}
		else
		{
			pSpawner->m_strError.LoadString(IDS_ERR_TOOL_RETURN);
			pSpawner->m_strOutput.Format(pSpawner->m_strError, pSpawner->m_dwTermStatus);
		}
		if (theApp.m_bInvokedCommandLine)
		{
			theApp.WriteLog(pSpawner->m_strOutput, TRUE);
		}
		else
		{
			g_IdeInterface.GetOutputWindow()->OutputWindowQueueHit(pSpawner->m_nOutputWindowID, pSpawner->m_strOutput, FALSE, TRUE, FALSE);
		}
		ASSERT(pSpawner == *(pSpawner->m_ppSpawner));
		*(pSpawner->m_ppSpawner) = NULL;
	}
	
	TRACE("spawner 0x%08x : cleaning up spawner object\n", pSpawner);
	VERIFY(CloseHandle(pSpawner->m_procinfo.hThread));
	VERIFY(CloseHandle(pSpawner->m_procinfo.hProcess));
	VERIFY(CloseHandle(pSpawner->m_hNulStdin));
	VERIFY(CloseHandle(pSpawner->m_hReadPipe));
	VERIFY(CloseHandle(pSpawner->m_hCancelEvent));
	VERIFY(CloseHandle(pSpawner->m_hSpawnEvent));

	// reset flag enabling subsequent spawns
	pSpawner->m_fEnableSpawn = TRUE;

	TRACE("spawner 0x%08x : deleting spawner object\n", pSpawner);
	delete pSpawner;

	TRACE("spawner 0x%08x : WaitChildProcessFinish thread committing suicide\n", pSpawner);
	_endthread();
}

CConsoleSpawner::CConsoleSpawner(CConsoleSpawner **ppSpawner)
{
	// set up the back pointer which is set to NULL upon spawn termination
	m_ppSpawner = ppSpawner;
	// Create an error context
	m_hSpawnErrors = CreateErrorContext(SPAWN_ERROR_DEPTH, 0, 0);
	m_ichOutput = 0;
	memset((void *)&m_startinfo, 0, sizeof(STARTUPINFO));
	m_sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	m_sa.lpSecurityDescriptor = NULL;
	m_sa.bInheritHandle = TRUE;
	m_hDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ASSERT(m_hDoneEvent != NULL);
	m_fEnableSpawn = TRUE;
	m_hNotifyEvent = NULL;
	m_hCleanEvent = NULL;
	m_hSpawnThread = NULL;
	m_hWaitThread = NULL;
	m_hSpawnEvent = NULL;
	m_hCancelEvent = NULL;
}

CConsoleSpawner::~CConsoleSpawner()
{
	// Destroy the error context
	DestroyErrorContext(m_hSpawnErrors);
	VERIFY(CloseHandle(m_hDoneEvent));
}

BOOL CConsoleSpawner::InitializeSpawn(LPTSTR lpszSpawnDir, UINT nOutputWindowID, BOOL bClearOutputWindow, BOOL bSaveAll /* = TRUE */, CErrorContext *pEC )
{
	// Save modified documents before spawning tool/build?
	if (bSaveAll)
	{
		LPSOURCEQUERY pInterface = g_IdeInterface.GetSourceQuery();
		if (pInterface != NULL && pInterface->IsSaveBeforeRunning() == S_OK)
			DoSaveBeforeSpawn();
	}

	m_nFileWarnings = 0;
	m_nFileErrors = 0;
	m_ichOutput = 0;

	// Set the output to redirect to the correct output window
	m_nOutputWindowID = nOutputWindowID;
	m_pEC = pEC;

	if (m_nOutputWindowID != -1)
		VERIFY(InitializeInternal(bClearOutputWindow));

	// save away the project build directory for the call to CreateProcess
	m_strSpawnDir = lpszSpawnDir;
	return TRUE;
}

BOOL CConsoleSpawner::PerformSpawn(LPTSTR lpszCommandLine, BOOL bMultipleCommand)
{
	if (m_nOutputWindowID != -1)
		return(PerformInternal(lpszCommandLine, bMultipleCommand));
	else
		return(PerformExternal(lpszCommandLine, bMultipleCommand));
}

BOOL CConsoleSpawner::PerformSpawnMultiple(CStringList *plistCommands)
{
	static const TCHAR szCmdSep[] = { chCmdSep, 0 };
	CString strCommands;
	int  nCommands = plistCommands->GetCount();
	BOOL bMultipleCommand = nCommands > 1;

	POSITION pos = plistCommands->GetHeadPosition();
	while (pos)
	{
		strCommands += plistCommands->GetNext(pos);
		if (pos)
			strCommands += szCmdSep;
	}

	if (!strCommands.IsEmpty())
		return(PerformSpawn((LPTSTR)LPCTSTR(strCommands), bMultipleCommand));

	return(FALSE);
}

void CConsoleSpawner::CancelSpawn()
{
	if (m_hCancelEvent != (HANDLE)0)
		VERIFY(SetEvent(m_hCancelEvent));
}

void CConsoleSpawner::DisplayOutputFromConsole(LPTSTR lpszText)
{
	int cb;
	LPTSTR lpszBegin = lpszText;

	// loop through all characters in string
	while (*lpszText)
	{
		// process special characters
		//
		switch (*lpszText)
		{
			case '\t':
				// if enough room, convert tabs into spaces
				if (m_ichOutput < MAX_USER_LINE)
					m_szOutput[m_ichOutput ++] = ' ';
				else
					// no room
					goto OverFlow ;
				break;

			case '\r':
				// Ignore CR
				break;

			case '\n':
				// treat LF as end of line marker
				// terminate string*
				m_szOutput[m_ichOutput] = '\0';
				// add to output window
				IsItErrorOrWarning(m_szOutput);
			//	OemToChar((LPCTSTR)m_szOutput, (LPSTR)m_szOutput);
				if( m_pEC != NULL ){
					m_pEC->AddString(m_szOutput);
				}
				else
				{
					if (theApp.m_bInvokedCommandLine)
					{
						theApp.WriteLog(m_szOutput, TRUE);
					}
					else
					{
						if (g_IdeInterface.GetOutputWindow() != NULL){
							g_IdeInterface.GetOutputWindow()->OutputWindowQueueHit(m_nOutputWindowID, m_szOutput, TRUE, TRUE, FALSE);
						}
					}
				}
				// set up for next line
				m_ichOutput = 0;
				break;

			default:
				// we have a regular character.  Save it if
				// we've got the room
				if (m_ichOutput + (cb = _ftclen(lpszText)) < MAX_USER_LINE + 1)
				{
					// olympus 1281:  The read from the pipe may have truncated
					// a double-byte character.  Make sure we don't copy a bogus
					// trail byte into the buffer.  NOTE that the input buffer *MUST*
					// be null-terminated!
					cb = (*(lpszText + 1) == '\0') ? 1 : cb;
					_ftccpy(m_szOutput + m_ichOutput, lpszText);
					m_ichOutput += cb;
				}
				else
				{
					// we don't have the room.  Terminate the
					// string here
OverFlow:
					// terminate
					//
				    m_szOutput[m_ichOutput] = '\0' ;
					// Make sure that the character we just boofed gets included
					// in the next line.  (I.e., negate the inc that happens at the
					// bottom of the loop.)
					if (lpszText > lpszBegin)
						lpszText = _ftcsdec(lpszBegin, lpszText);
					// add to output window
					IsItErrorOrWarning(m_szOutput);
				//	OemToChar((LPCTSTR)m_szOutput, (LPSTR)m_szOutput);
					if( m_pEC != NULL ){
						m_pEC->AddString(m_szOutput);
					}
					else
					{
						if (theApp.m_bInvokedCommandLine)
						{
							theApp.WriteLog(m_szOutput, TRUE);
						}
						else
						{
							if (g_IdeInterface.GetOutputWindow() != NULL) {
								g_IdeInterface.GetOutputWindow()->OutputWindowQueueHit(m_nOutputWindowID, m_szOutput, TRUE, TRUE, FALSE);
							}
						}
					}
					// set up for next line
					m_ichOutput = 0;
				}
				break;
		} // switch

		// advance to next character
		//
		// olympus 1281:  We don't know if a DBC has been split...
		cb = _ftclen(lpszText);
		cb = (*(lpszText + 1) == '\0') ? 1 : cb;
		lpszText += cb;
	} // while
}

static char szErrorString[40] = { 0 };
static char szWarningString[40] = { 0 };

BOOL CConsoleSpawner::IsItErrorOrWarning(char *pszString)
{
	LPSTR pchCur, pchError, pchWarning, pchUse, pchEnd;
	BOOL  bWarning;
	static ULONG cbError;
	static ULONG cbWarning;

	if (!szErrorString[0])
	{
		VERIFY(LoadString(GetResourceHandle(), IDS_OUTPUT_ERROR, szErrorString, sizeof(szErrorString)));
		VERIFY(LoadString(GetResourceHandle(), IDS_OUTPUT_WARNING, szWarningString, sizeof(szWarningString)));
		cbError = _tcslen(szErrorString);
		cbWarning = _tcslen(szWarningString);
	}

	// See if string contains error or warning pattern.
	pchCur = pszString;

Rescan:
	pchError = stristr(pchCur, szErrorString);
	pchWarning = stristr(pchCur, szWarningString);

	if ((pchError != NULL) && (pchWarning != NULL))
	{
		// Found both.  Use the first one (earliest in the string).
		if (pchError < pchWarning)
		{
			pchUse = pchError;
			bWarning = FALSE;
			pchEnd = pchUse + cbError;
		}
		else
		{
			pchUse = pchWarning;
			bWarning = TRUE;
			pchEnd = pchUse + cbWarning;
		}
	}
	else if ((pchError == NULL) && (pchWarning == NULL))
	{
		// Didn't find either.  Set pchUse to NULL as well.
		pchUse = NULL;
	}
	else
	{
		// Found either "error" or "warning".  Use the one we found.

		if (pchError != NULL)
		{
			pchUse = pchError;
			bWarning = FALSE;
			pchEnd = pchUse + cbError;
		}
		else
		{
			pchUse = pchWarning;
			bWarning = TRUE;
			pchEnd = pchUse + cbWarning;
		}
	}

	if (pchUse != NULL)
	{
		char FAR *lpchPrev = _ftcsdec(pszString, pchUse);

		// if error string is not the 1st word && not prepended by whitespace
		// or not followed by whitespace or a ':' we should rescan
		if (((pchUse != pszString) && (*lpchPrev != ' ') && (*lpchPrev != '\t')) ||
			 ((*pchEnd != ' ') && (*pchEnd != '\t') && (*pchEnd != ':')))
		{
			pchCur = _ftcsinc(pchUse);
			goto Rescan;	// error is probably a subdir or a filename,
							// so scan further.
		}
    }
    else
	{
    	return FALSE;
	}

	// update appropriate error counts
	if (!bWarning)
	{
		++m_nFileErrors;
		PlayDevSound(IDS_SOUND_OUTPUT_ERROR);
	}
	else
	{
		++m_nFileWarnings;
		PlayDevSound(IDS_SOUND_OUTPUT_WARNING);
	}

	return TRUE;
}

void CConsoleSpawner::GetErrorWarningCount(DWORD *pdwErrors, DWORD *pdwWarnings)
{
	ASSERT(pdwErrors != NULL);
	ASSERT(pdwWarnings != NULL);
	*pdwErrors = m_nFileErrors;
	*pdwWarnings = m_nFileWarnings;
}

BOOL CConsoleSpawner::InitializeInternal(BOOL bClearOutputWindow)
{
	// Clear the error context
	VERIFY(ClearError((LPTERROR)m_hSpawnErrors));

	if (!theApp.m_bInvokedCommandLine)
	{
		// open the error window
		DkWShowWindow(MAKEDOCKID(PACKAGE_VCPP, IDDW_OUTPUT), TRUE);

		if (bClearOutputWindow)
		{
			// mark for clear in subsequent SelectVwin call
			g_IdeInterface.GetOutputWindow()->OutputWindowVwinClear(m_nOutputWindowID);
			// note: window not actually cleared until subsequent SelectVwin call

			// selected output window is brought to the top and cleared
			g_IdeInterface.GetOutputWindow()->OutputWindowSelectVwin(m_nOutputWindowID, TRUE);

		}
	}

	// create semaphore for indicating process spawned
	if ((m_hSpawnEvent = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL)
		return FALSE;	// REVIEW: UNDONE report error: could not create semaphore

	// create thread for reading output from process
	unsigned dwThreadID;
	if ((m_hSpawnThread = (HANDLE)_beginthreadex( NULL, cbThrdStack, ::ReadChildProcessOutput, this, 0, &dwThreadID )) == (HANDLE)0)
		return FALSE;	// REVIEW: UNDONE report error: could not create thread

	return TRUE;
}

BOOL CConsoleSpawner::PerformInternal(LPTSTR lpszCommandLine, BOOL bMultipleCommand)
{
	CHAR *pszCreate = NULL, *pszSpawn = NULL, *pszShell = NULL;
	BOOL fUseComspec = FALSE;
	BOOL fUtilIsShell = FALSE;
	BOOL fCreateSuccess;	// create process succeeded?
	DWORD dwError;

	char szMsvcPath[_MAX_PATH];

	if (GetModuleFileName(NULL, szMsvcPath, sizeof(szMsvcPath)))
	{
		char szDrive[_MAX_DRIVE], szDir[_MAX_DIR];
		_splitpath(szMsvcPath, szDrive, szDir, NULL, NULL);
		_makepath(szMsvcPath, szDrive, szDir, NULL, NULL);
	}
	
	// set flag disabling subsequent spawns
	m_fEnableSpawn = FALSE;

	m_CompileState = CS_START;
	// scan lpszCommandLine for redirection characters, and
	// check utility for being the shell.
	{
		char *pchT = lpszCommandLine + _ftcscspn(lpszCommandLine, rgchCmdProcChar);
		char *pszEnv;
		if (pszEnv = getenv("COMSPEC"))
		{
			pszShell = (char *)malloc(_ftcslen(pszEnv) + 5);
			_ftcscpy(pszShell, pszEnv);
			_ftcscat(pszShell, " /c ");
			fUtilIsShell = !!stristr(lpszCommandLine, pszShell);
		}
		else
		{
			VERIFY(PushError(m_hSpawnErrors, IDS_ERR_CANNOT_SPAWN_TOOL));
			goto CleanUp;
		}
		fUseComspec = !!*pchT && !fUtilIsShell;
	}

	pszSpawn = (CHAR *)malloc(_ftcslen(m_szSpawnTool) + _ftcslen(szMsvcPath) + _ftcslen(lpszCommandLine) + _ftcslen(pszShell));

	if (pszSpawn == NULL)
	{
		VERIFY(PushError(m_hSpawnErrors, ERROR_NOT_ENOUGH_MEMORY));
		goto CleanUp;
	}

	m_CompileState = CS_CREATEALLOC;

	// initialize that process not complete
	ResetEvent(m_hDoneEvent);

	// create an event object to signal vcspawn.exe when to kill
	//  the child process tree.  note that we have the manual reset
	//  option set so that we can detect when we killed the process
	//  w/o using another flag.
	if (!(m_hCancelEvent = CreateEvent(&m_sa, TRUE, FALSE, NULL)))
	{
		VERIFY(PushError(m_hSpawnErrors, SYSTEM_ERROR | GetLastError()));
		goto CleanUp;
	}

	m_CompileState = CS_CREATEEVENT;

	// Create the spawn string with the vcspawn prepended
	sprintf(pszSpawn, m_szSpawnTool, szMsvcPath, (DWORD)m_hCancelEvent, bMultipleCommand ? szMultCmd : "");
	pszCreate = pszSpawn + _ftcslen(pszSpawn);

	// create string for combined lpUtil and lpCmdLine
	// Need a command shell?
	if (fUseComspec)
		_ftcscat(pszSpawn, pszShell);


	// form the rest of the command line
	_ftcscat(pszSpawn, lpszCommandLine);

	if (!CreatePipe(&m_hReadPipe, &m_hpipeWriteOut, &m_sa, cbPipeBuf))
	{
		VERIFY(PushError(m_hSpawnErrors, SYSTEM_ERROR | GetLastError()));
		goto CleanUp;
	}

	m_CompileState = CS_CREATEPIPE;

	// must use a different handle for stderr because some apps (like cl.exe)
	// use dup2() which closes the current target handle if open.  this has
	// the effect of closing both handles if we only pass in the one handle
	// in both hStdOutput and hStdError fields of STARTUPINFO
	m_hGeneric = GetCurrentProcess();
	if (!DuplicateHandle(
			m_hGeneric,					// source process
			m_hpipeWriteOut,			// source handle to dupe
			m_hGeneric,					// target process
			&m_hpipeWriteErr,			// new handle
			DUPLICATE_SAME_ACCESS,		// access flags
			TRUE,						// make it inheritable
			DUPLICATE_SAME_ACCESS))		// option flags
	{
		VERIFY(PushError(m_hSpawnErrors, SYSTEM_ERROR | GetLastError()));
		goto CleanUp;
	}

	m_CompileState = CS_DUPLICATEHANDLES;

	// create a handle to /dev/nul
	m_hNulStdin = CreateFile("NUL", GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
		&m_sa, OPEN_ALWAYS, 0, INVALID_HANDLE_VALUE);

	if (m_hNulStdin == INVALID_HANDLE_VALUE)
	{
		VERIFY(PushError(m_hSpawnErrors, SYSTEM_ERROR | GetLastError()));
		goto CleanUp;
	}

	m_CompileState = CS_CREATENULFILE;

	m_startinfo.cb = sizeof(STARTUPINFO);
	m_startinfo.lpReserved = NULL;
	m_startinfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	m_startinfo.hStdOutput = m_hpipeWriteOut;
	m_startinfo.hStdError = m_hpipeWriteErr;
	m_startinfo.hStdInput = m_hNulStdin;
	m_startinfo.wShowWindow = SW_HIDE;

	fCreateSuccess = CreateProcess(NULL, pszSpawn, NULL, NULL, TRUE,
		CREATE_NEW_CONSOLE, NULL, m_strSpawnDir, &m_startinfo, &m_procinfo);

	dwError = GetLastError();
	// GetLastError() returns ERROR_ACCESS_DENIED on NT German when path contains extended chars!
	if (!fCreateSuccess && (dwError == ERROR_FILE_NOT_FOUND || dwError == ERROR_ACCESS_DENIED))
	{
		// Unable to run vcspawn with explicit path, try without...
		sprintf(pszSpawn, szSpawnToolNoQuote, "", (DWORD)m_hCancelEvent, bMultipleCommand ? szMultCmd : "");
		if (fUseComspec)
			_ftcscat(pszSpawn, pszShell);
		_ftcscat(pszSpawn, lpszCommandLine);
		fCreateSuccess = CreateProcess(NULL, pszSpawn, NULL, NULL, TRUE,
			CREATE_NEW_CONSOLE, NULL, m_strSpawnDir, &m_startinfo, &m_procinfo);
	}
	if (pszShell != NULL){
		free(pszShell);
		pszShell = NULL;
	}

	// Unable to run vcspawn (either explicitly pathed or not)...
	if (!fCreateSuccess && (GetLastError() == ERROR_FILE_NOT_FOUND))
	{
		if (!m_fUserWarned)
		{
			ReportMissingFile("VCSPAWN.EXE");
			m_fUserWarned = TRUE;
		}
	}

	if (fCreateSuccess)
	{
		// create thread which waits for process termination
		m_fThreadDie = FALSE;
		m_hWaitThread = (HANDLE)_beginthread(CConsoleSpawner::WaitChildProcessFinish, cbThrdStack, this);
		ASSERT(m_hWaitThread != (HANDLE)-1);	// REVIEW: UNDONE report error: could not create thread

		// indicate process spawned
		SetEvent(m_hSpawnEvent);
		free(pszSpawn);

		// close our copies of pipe write handles
		VERIFY(CloseHandle(m_hpipeWriteOut));
		VERIFY(CloseHandle(m_hpipeWriteErr));
		return(TRUE);
	}

CleanUp:
	dwError = GetLastError();
	VERIFY(PushError(m_hSpawnErrors, SYSTEM_ERROR | dwError));
	VERIFY(PushError(m_hSpawnErrors, IDS_ERR_CANNOT_SPAWN_TOOL));

	if (pszShell != NULL){
		free(pszShell);
		pszShell = NULL;
	}

	switch(m_CompileState)
	{
		default:
			ASSERT(FALSE);
			break;
		case CS_SUCCESSFUL:
		case CS_KILLEDPROCESS:
		case CS_CREATEPROCESS:
			// Successful CreateProcess
			VERIFY(CloseHandle(m_procinfo.hThread));
			VERIFY(CloseHandle(m_procinfo.hProcess));
		case CS_CREATENULFILE:
			// Successful CreateFile("NUL")
			VERIFY(CloseHandle(m_hNulStdin));
		case CS_DUPLICATEHANDLES:
		case CS_CREATEPIPE:
			// Successful DuplicateHandle
			// Successful CreatePipe
			VERIFY(CloseHandle(m_hReadPipe));
			m_hReadPipe = INVALID_HANDLE_VALUE;
		case CS_CREATEEVENT:
			// Successful CreateEvent
			VERIFY(CloseHandle(m_hCancelEvent));
			m_hCancelEvent = (HANDLE)0;
		case CS_CREATEALLOC:
			free(pszSpawn);
		case CS_START:
			VERIFY(CloseHandle(m_hSpawnEvent));
			break;
	}

	LPTSTR pszError = m_strError.GetBuffer(TMP_STRING_SIZE);
	int nError;
	while (nError = PopError(m_hSpawnErrors))
	{
		if(LoadError(GetResourceHandle(), nError, pszError, TMP_STRING_SIZE))
		{
			if (theApp.m_bInvokedCommandLine)
			{
				if (pszError != NULL)
				{
					theApp.WriteLog(pszError, TRUE);
				}
			}
			else
			{
				g_IdeInterface.GetOutputWindow()->OutputWindowQueueHit(m_nOutputWindowID, pszError, FALSE, TRUE, FALSE);
			}
		}
	}
	m_strError.ReleaseBuffer();

	// reset flag enabling subsequent spawns
	m_fEnableSpawn = TRUE;

	*m_ppSpawner = NULL;
	delete this;

	return(FALSE);
}

BOOL CConsoleSpawner::PerformExternal(LPTSTR lpszCommandLine, BOOL bMultipleCommand)
{
	BOOL fCreateSuccess;	// create process succeeded?

	CString strExecute;
	if (!m_fCloseWindow && !m_fGUITool)
		strExecute.Format(TEXT("%s -%c -%c %s"), szVCSpawnCmd, chToolFlag, chPauseFlag, lpszCommandLine);
//	else if (NotOnNT() && (exeType == EXE_DOS))
//		strExecute.Format(TEXT("%s -%c %s %s"), szVCSpawnCmd, chToolFlag, lpszCommandLine);
	else
		strExecute = lpszCommandLine;

	m_startinfo.cb = sizeof(STARTUPINFO);
	m_startinfo.lpReserved = NULL;

	TCHAR szPath[_MAX_PATH];
	// Preserve current directory
	if (_getcwd(szPath, _MAX_PATH) == NULL)
		return(FALSE);

	fCreateSuccess = CreateProcess(NULL, (LPSTR)(LPCTSTR)strExecute, NULL, NULL, FALSE,
		CREATE_NEW_CONSOLE, NULL, m_strSpawnDir, &m_startinfo, &m_procinfo);

	if (fCreateSuccess)
	{
		CloseHandle(m_procinfo.hProcess);
		CloseHandle(m_procinfo.hThread);
		if (m_fReloadNoPrompt && g_IdeInterface.GetEditDebugStatus() != NULL)
			// don't prompt when reloading next modified file
			g_IdeInterface.GetEditDebugStatus()->SetLoadNextFileNoPrompt(TRUE);
	}
	else
	{
		UINT idstr;

		// See if we can give a more specific reason for the failure
		switch(GetLastError())
		{
			case ERROR_DIRECTORY:
			case ERROR_INVALID_DRIVE:
			case ERROR_PATH_NOT_FOUND:
				idstr = IDS_ERR_PATH_NOT_FOUND;
				break;

			case ERROR_FILE_NOT_FOUND:
				idstr = IDS_ERR_FILE_NOT_FOUND;
				break;

			case ERROR_BAD_FORMAT:
				idstr = IDS_ERR_BAD_EXE_FORMAT;
				break;

			case ERROR_NOT_ENOUGH_MEMORY:
				idstr = IDS_ERR_NOT_ENOUGH_MEMORY;
				break;

			default:
				idstr = IDS_ERR_CANNOT_SPAWN_TOOL;
				break;
		}

		ErrorBox(idstr);
	}

	// Restore directory
	SetDriveAndDir(szPath);

	*m_ppSpawner = NULL;
	delete this;

	return(fCreateSuccess);
}

/////////////////////////////////////////////////////////////////////////////

//
// Error handling functions
//

#ifdef _DEBUG
void _KeyError(LPSTR pszFile, UINT nLine)
{
	AfxAssertFailedLine(pszFile, nLine);
}
#endif

// Create and destroy error context

LPTERROR CreateErrorContext(UINT nErrorDepth, UINT nErrorMin, UINT nErrorMax)
{
	LPTERROR lpError;

	// Allocate the context and stack and set the top pointer
	VERIFY(lpError = (LPTERROR)malloc(sizeof(TERROR)));
	VERIFY(lpError->lpStack = (LPLONG)malloc(sizeof(*lpError->lpStack) * nErrorDepth));

	lpError->lpTop = lpError->lpStack;
	lpError->lpEnd = lpError->lpStack + nErrorDepth;
	lpError->nErrorMin = nErrorMin;
	lpError->nErrorMax = nErrorMax;

	KEYSET(lpError, ERRORKEY);

	return lpError;
}

void DestroyErrorContext(LPTERROR lpError)
{
	// Deallocate the stack and context
	free(lpError->lpStack);
	free(lpError);
}

// Push the error onto the context
BOOL PushError(LPTERROR lpError, ULONG nError)
{
	KEYCHECKRET(lpError, ERRORKEY, 0);

	// Error number out of bounds
	if (lpError->nErrorMin && lpError->nErrorMax &&
		(nError < lpError->nErrorMin || nError > lpError->nErrorMax))
	{
		return FALSE;
	}

	// Topped out!
	if (lpError->lpTop >= lpError->lpEnd)
	{
		return FALSE;
	}

	*(lpError->lpTop++) = nError;
	return TRUE;
}

// Pop from the context
LONG PopError(LPTERROR lpError)
{
	KEYCHECKRET(lpError, ERRORKEY, 0);

	// Bottomed out!
	if (lpError->lpTop <= lpError->lpStack)
	{
		return 0;
	}

	(lpError->lpTop)--;

	return *(lpError->lpTop);
}

// Clear the error stack
BOOL ClearError(LPTERROR lpError)
{
	KEYCHECKRET(lpError, ERRORKEY, FALSE);

	lpError->lpTop = lpError->lpStack;
	return TRUE;
}

// Get the current stack depth

INT GetDepth(LPTERROR lpError)
{
	KEYCHECKRET(lpError, ERRORKEY, -1);

	return lpError->lpTop - lpError->lpStack;
}

// Index into the stack (positive from top)

LONG IndexError(LPTERROR lpError, UINT nIndex)
{
	INT	nStackIndex, nDepth;

	KEYCHECKRET(lpError, ERRORKEY, 0L);

	nDepth = lpError->lpTop - lpError->lpStack;
	nStackIndex =  nDepth - nIndex -1;

	if(nStackIndex < 0 || nStackIndex > nDepth)
	{
		return 0L;
	}

	return lpError->lpStack[nStackIndex];
}

//
// Load the error string into the string buffer
//	If the error is a system error use FormatMessage.
//	Else assume that the error has a string table reference associated with it
//
//	Return the number of chars copied into the string buffer
//

int LoadError(HANDLE hInstance, UINT nErrorNum, LPSTR szBuffer, size_t nBufsize)
{
	LPTSTR	lpBuffer;	// memory allocated by FormatMessage function
	int		nCharsCopied;

	if(nErrorNum & SYSTEM_ERROR)
	{
		if(FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
					  FORMAT_MESSAGE_IGNORE_INSERTS | 
					  FORMAT_MESSAGE_ALLOCATE_BUFFER,
					  NULL, nErrorNum & ~SYSTEM_ERROR,		//Mask syserror bit
					  MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL),
					  (LPTSTR)&lpBuffer, 0, NULL))
		{
			nCharsCopied = min(nBufsize, RemoveNewlines(lpBuffer));
			_ftcsncpy(szBuffer, lpBuffer, nBufsize);
			LocalFree(lpBuffer);
		}
		else
			nCharsCopied = 0;
	}
	else
	{
		nCharsCopied = LoadString((HINSTANCE)hInstance, nErrorNum, szBuffer, nBufsize);
	}

	return nCharsCopied;
}

/////////////////////////////////////////////////////////////////////////////
