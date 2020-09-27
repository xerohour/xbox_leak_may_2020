/****************************************************************************
	Aug 1993 Split codemgr.c into dbg.h, dbgosd.c, and dbgmisc.c
	[MarkBro]

			 Revision J.M. Dec 1991
			 This file is derived from CV400
			 SYSTEM   layer ( SYSTEM.C LOAD.C CONFIG.C )

			 CODEMGR  code management
			 OSDEBUG  CallBack
			 CONFIG   Dll configuration
			 LOAD     OSDebug loading

****************************************************************************/
#include "stdafx.h"
#pragma hdrstop

#include <windows.h>
#include "sqlpkapi.h"
#include <initguid.h>
#include "sqlguid.h"
#include "idbgproj.h"
#include "dbgenc.h"
#include "image.h"
#define _XBDM_
#define XbeGetLocalModulePathA HrXbeGetLocalModulePath
#include <xboxdbg.h>

BOOL	g_fPromptNoSymbolInfo = TRUE;

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

static HMODULE hmodXbdm;
static HRESULT (__stdcall *pfnXbeGetLocalModulePathA)(LPCSTR szXbe,
	LPCSTR szModule, LPSTR szPath, DWORD cchPath);

DMHRAPI HrXbeGetLocalModulePath(LPCSTR szXbe, LPCSTR szModule,
	LPSTR szPath, DWORD cchPath)
{
	if(pfnXbeGetLocalModulePathA == NULL) {
		if(hmodXbdm == NULL)
			hmodXbdm = LoadLibrary(_T("xboxdbg.dll"));
		if(hmodXbdm != NULL)
			*(void **)&pfnXbeGetLocalModulePathA = GetProcAddress(hmodXbdm,
				_T("XbeGetLocalModulePathA"));
		if(pfnXbeGetLocalModulePathA == NULL)
			return E_FAIL;
	}
	return (*pfnXbeGetLocalModulePathA)(szXbe, szModule, szPath, cchPath);
}


ULONG
GetStepMode(
	);
	
// Utilities for debugging...
void AuxAddr(PADDR paddr)
{
//	AuxPrintf4("ADDR 0x%lX:0x%lX, emi:%lX",
//			   (LONG)GetAddrSeg(*paddr),
//			   (LONG)GetAddrOff(*paddr),
//			   (LONG)emiAddr(*paddr)) ;
}


// Private information for displaying the PC and the
// callstack indicator

CDocument   *pDocCaller = NULL;
CDocument   *pDocTrace  = NULL;
int			iCallerLine;
int			iTraceLine;

// Remember if we have already called DEBUGInitialize
BOOL fDebugInitialize = FALSE;

// Callback for edit package notification of closing textdoc
void TextDocCallback(CDocument *pDoc, UINT uAction)
{
    if ( uAction == DCB_CLOSE )
    {
        if ( pDoc == pDocCaller )
            pDocCaller = NULL;

        if ( pDoc == pDocTrace )
            pDocTrace = NULL;
    }

    return;
}


/****************************************************************************
	FAttemptLoadFile

	Purpose		: Callback function for FMapPath to decide if a path entered
				  by the user is valid.

	Arguments	: lszPath = the path entered by the user
				  lszFilename = the filename being searched for
				  lParam = pointer to an ATTEMPTLOADINFO structure which
					has a little extra stuff we need

	Returns		: TRUE for success, FALSE for failure

	Notes		: In addition to testing for the existence of the file,
				  we will also actually load it if it is found.

****************************************************************************/
// Our lParam points to an ATTEMPTLOADINFO structure
typedef struct ATTEMPTLOADINFO
{
	CDocument   **ppTDoc;
	HSF		    hsf;
	BOOL        *pfCxt;
	UINT	    docType;
} ATTEMPTLOADINFO;

BOOL FAttemptLoadFile(LPCSTR lszPath, LPCSTR lszFilename, LONG lParam)
{
	ATTEMPTLOADINFO FAR *pali = (ATTEMPTLOADINFO *)lParam;
	BOOL fFound = FALSE;

	if (lszPath[0])
	{
		_ftcscpy(szTmp, lszPath);
		fFound = LoadFileFromModule(szTmp, lszFilename, pali->ppTDoc,
			pali->hsf, pali->pfCxt, pali->docType);
	}

	return fFound;
}
static HSF hsfPrevFail = (HSF)NULL;
static HSF hsfPrev = (HSF) NULL;
static int iRetPrev = SF_NOERROR;

void PASCAL RetrySearchFile( )
{
	hsfPrevFail = (HSF)NULL;
}

/****************************************************************************
    Implementation:     J.M. Mar 1992
    SearchFileFromModule:  CODEMGR.C

    Purpose : When having the symbols, we've got a source and line for
			  tracing. Let's try first the EXE path, then the MFC path
			  and finally display a dialog box asking for the path...
			  The doc number will be either updated or created
			  and the source file name completed...

    Arguments : Source name
				Doc number updated
				Module handle for the document
				fCxt set to TRUE if document already opened and
				hMod has changed (INLINE FILES) to reload ASM

    Returns   : SF_NOERROR,
				SF_NOFILE, or
				SF_NODOCAVAIL
****************************************************************************/
int PASCAL SearchFileFromModule(LPSTR szName, CDocument **ppDoc, HSF hsf, BOOL *pfCxt, UINT docType)
{
	extern BOOL PASCAL DAMFOkToAskForPath( HSF );
	char        AlternatePath[_MAX_PATH];
	char        szTemp[_MAX_PATH] ;
	char        szFNameParam [_MAX_FNAME] ;	// originally passed-in filename
	char        szExtParam [_MAX_EXT] ;		// originally passed-in extension
	char        Name[_MAX_PATH] ;	// scratch variable
	CDir        dirProj;	// directory of the project (or the EXE if no proj exists)
	HSF	        hsfPrevSave = hsfPrevFail;
	BOOL        bFound = FALSE ;

	hsfPrev = hsf;

	// Make sure that there wasn't a previous error.  If so, this will prevent
	// us from correctly finding the file.
	ClearGlobalError();

	// if its open, go straight to it [apennell]
	// do this *before* looking in the hsf list as all SQL windows have a "" pathname
	if( gpISrc->FindDocument(szName, ppDoc, TRUE) )
    {
		DAMSaveDebugFile((char *)(const char *)(*ppDoc)->GetPathName(),hsf);
		return iRetPrev = SF_NOERROR ;
	}

	// First try to find it comparing our documents hsf info
	if(docType != DISASSY_WIN && (*ppDoc = gpISrc->GetDocFromHsf(hsf)) != NULL)
    {
//		gpISrc->SetCallback(*ppDoc, (FARPROC)TextDocCallback); // paranoid: just to make sure
		DAMSaveDebugFile((char *)(const char *)(*ppDoc)->GetPathName(),hsf);
		return iRetPrev = SF_NOERROR ;
	}

	// Try first without mangling the path.  this allows a pseudo-path search
	// for SQL or other non-disk based docs.
	if( LoadNonDiskDocument(szName, ppDoc) )
	{
		DAMSaveDebugFile((char *)(const char *)(*ppDoc)->GetPathName(),hsf);
		return iRetPrev = SF_NOERROR ;
	}

	// [bug #6792 1-10-93 v-natal]
	// Removed the conversion fron OEM to ANSI. At this point, we don't handle
	// directly names from the symbol handler. They have already been
	// converted to ANSI by the IDE.


    // Copy string in work buffer
    _ftcscpy(szTemp,szName) ;

    // Extract source and path name components...
    _splitpath(szTemp, szDrive, szPath , szFName, szExt);
    AnsiUpper(_ftcscpy( szFNameParam, szFName));
    AnsiUpper(_ftcscpy( szExtParam, szExt));

    // only remember the name
	_makepath(szTemp, NULL, NULL, szFName, szExt);
	_ftcscpy(Name,szTemp) ;

	// get the path
	_makepath(szTemp, szDrive, szPath, NULL, NULL);

	// Try the given path if any...
	if( LoadFileFromModule(szTemp,Name,ppDoc,hsf,pfCxt,docType) )
		return( iRetPrev = SF_NOERROR );

	if(CheckGlobalError(TRUE) == ERR_FILEMAX)
    {
		hsfPrev = (HSF)NULL;
		return( iRetPrev = SF_NODOCAVAIL );
	}

	if ( !docType )
		return( iRetPrev = SF_DONTOPENFILE );

	// Okay, we didn't find it yet, see if the file (just the basename.ext
	// part) is already loaded in the editor
	if ( BFHFindDoc(szName, ppDoc) )
    {
		gpISrc->SetHSF(*ppDoc, hsf);

		DAMSaveDebugFile((char *)(const char *)(*ppDoc)->GetPathName(),hsf);

		// DOLPHIN:9243, reset the previous failure to null since
		// we have now found it
		if ( hsf == hsfPrevFail )
			hsfPrevFail = (HSF)NULL;

		return( iRetPrev = SF_NOERROR );
	}

	// If we've just asked for this module and failed, return previous error
	if ( hsfPrevSave == hsf && iRetPrev != SF_NOERROR )
		return( iRetPrev );

	// See if this hsf is the previous failure.  If so, then abort now.
	// This should happen here since we wan't to call BHFHFindDoc to
	// see if the file is now in the editor under a different name
	// DOLPHIN 9243.
	if ( hsf == hsfPrevFail )
    {
		hsfPrev = hsfPrevSave;
		return( SF_NOFILE );
	}

	// [CAVIAR #4881 10/28/1992 v-natjm]
	// Try the makefile path...or executable if no current makefile,
	// case where we debug without project...
    LPCSTR pszPath = NULL;

    if (gpIBldSys && (gpIBldSys->IsActiveBuilderValid() == S_OK) && SUCCEEDED(gpIBldSys->GetBuilderFile(ACTIVE_BUILDER, &pszPath)) && pszPath != NULL)
        _ftcscpy(szTemp, pszPath) ;
	else
		_ftcscpy(szTemp,ExecutablePath) ;

    _splitpath(szTemp,szDrive,szPath,NULL,NULL) ;
	_makepath(szTemp,szDrive,szPath,NULL,NULL) ;

	// remember the "project" directory
	VERIFY(dirProj.CreateFromString(szTemp));

	// Better use OMF relative name with EXE path... ( passed szName !!! )
	bFound = LoadFileFromModule(szTemp,szName,ppDoc,hsf,pfCxt,docType) ;
	// [CAVIAR #4881 11/10/92 v-natjm]
	// szTemp contains the canonicalized or potentially fully
	// qualified path that we are going to propose as a default
	// Remember it
	_splitpath(szTemp,szDrive,szDir,NULL,NULL) ;
	_makepath(szTemp,szDrive,szDir,NULL,NULL) ;
	_ftcscpy(AlternatePath,szTemp) ;

	if( bFound )
        return iRetPrev = SF_NOERROR ;

	if( CheckGlobalError(TRUE) == ERR_FILEMAX )
    {
		hsfPrev = (HSF)NULL;
		return( iRetPrev = SF_NODOCAVAIL );
	}

	// Next try the project directory with just the filename (i.e not OMF
	// relative filename). This is useful for cases where we have the
	// absolute file name in the OMF and the project has moved to a
	// different location.

    if (pszPath != NULL)
        _ftcscpy(szTemp, pszPath) ;
    else
        _ftcscpy(szTemp,ExecutablePath) ;

    _splitpath(szTemp,szDrive,szPath,NULL,NULL) ;
	_makepath(szTemp,szDrive,szPath,NULL,NULL) ;

	bFound = LoadFileFromModule(szTemp,Name,ppDoc,hsf,pfCxt,docType);

	if ( bFound )
        return( iRetPrev = SF_NOERROR );

	if( CheckGlobalError(TRUE) == ERR_FILEMAX )
    {
		hsfPrev = (HSF)NULL;
		return( iRetPrev = SF_NODOCAVAIL );
	}

	// Another solution is to take only the source file name from the OMF
	// and concatenate it to the actual EXE path...
	// This is for the case you have moved your project, the EXE is not
	// rebuilt and the OMF gives us a full old path that doesn't exist
	// anymore !!!
	_ftcscpy(szTemp,ExecutablePath) ;
	_splitpath(szTemp,szDrive,szDir,NULL,NULL) ;
	_makepath(szTemp,szDrive,szDir,NULL,NULL) ;

	bFound = LoadFileFromModule(szTemp,Name,ppDoc,hsf,pfCxt,docType);
	if( bFound )
        return( iRetPrev = SF_NOERROR );

	if( CheckGlobalError(TRUE) == ERR_FILEMAX )
    {
		hsfPrev = (HSF)NULL;
		return( iRetPrev = SF_NODOCAVAIL );
	}


    // Check the Option tools directories source paths
	LPSTR lpszFullPath;

	COleRef<IBuildDirManager> srpBDM;
	HRESULT hrBDM;
	if (SUCCEEDED(hrBDM = theApp.FindInterface(IID_IBuildDirManager,
			(LPVOID*) &srpBDM)) &&
		srpBDM->FindFileOnPath(PLATFORM_CURRENT, DIRLIST_SOURCE,
			Name, &lpszFullPath) == S_OK)
	{
		bFound = TRUE;

        _splitpath(lpszFullPath, szDrive, szPath, NULL, NULL);
        _makepath(szTemp, szDrive, szPath, NULL, NULL);

        if ( LoadFileFromModule (szTemp,Name,ppDoc,hsf,pfCxt,docType))
            return( iRetPrev = SF_NOERROR );

        if ( CheckGlobalError ( TRUE ) == ERR_FILEMAX )
        {
            hsfPrev = (HSF)NULL;
            return( iRetPrev = SF_NODOCAVAIL );
        }
    }

	// We want to search on the Options.Directories source path using the
	// relative path given by the debug info.
	if (SUCCEEDED(hrBDM) && srpBDM->FindFileOnPath(PLATFORM_CURRENT, DIRLIST_SOURCE, szName, &lpszFullPath) == S_OK)
	{
		bFound = TRUE;

		_splitpath(lpszFullPath, szDrive, szPath, NULL, NULL );
		_makepath(szTemp, szDrive, szPath, NULL, NULL );
		if (LoadFileFromModule(szTemp, Name, ppDoc, hsf, pfCxt, docType)) {
			return iRetPrev = SF_NOERROR;
		}
		if (CheckGlobalError(TRUE) == ERR_FILEMAX) {
			hsfPrev = (HSF)NULL;
			return iRetPrev = SF_NODOCAVAIL;
		}
	}

	// If the user has already been asked for this file and they have pressed
	// cancel (no file), we don't want to ask them again.  So ask the DAM file
	// manager if it already knows about it.  If so, don't ask.  The DAM file
	// manager keeps track of the last 10 or so files needed in the DAM window
	// so it's possible that they will be asked again after the DAM cache gets
	// the appropriate entry flushed.
	if ( DAMFOkToAskForPath( hsf ) )
    {
		CString strDialogParam, strCaption;
		ATTEMPTLOADINFO ali;
		DIALOGPARAM dlgParam;

		// Build the askpath dialog box request string..
		MsgText(strDialogParam, IDS_ASKPATH_GETPATH, szFNameParam, szExtParam);
		dlgParam.lpParam = (LPSTR)(const char *)strDialogParam;

		VERIFY(strCaption.LoadString(IDS_ASKPATH_CAPTION_SOURCE));
		dlgParam.lpCaption = (LPSTR)(const char *)strCaption;

		// Set some globals that are used by the callback we pass to FMapPath...
		ali.ppTDoc  = ppDoc;
		ali.hsf     = hsf;
		ali.pfCxt   = pfCxt;
		ali.docType = docType;

		// Finally, search our list of path-to-path mappings, and possibly
		// ask the user for a path to search in...

		bFound = FMapPath(dirProj,AlternatePath,Name,&dlgParam,FAttemptLoadFile,(LONG) (VOID *) &ali);

		if ( !bFound && CheckGlobalError(TRUE) == ERR_FILEMAX )
        {
			hsfPrev = (HSF)NULL;
			return( iRetPrev = SF_NODOCAVAIL );
		}
	}

	// Remember the user specified path for further file search use
	// This list is also intended for the source debugging management
	// ( see UTIL.C function : GetDocFromFileName )
	if( bFound )
		iRetPrev = SF_NOERROR;
	else
    {
		iRetPrev = SF_NOFILE;
		hsfPrevFail = hsf;
	}

	return( iRetPrev );
}



/****************************************************************************
    Implementation:     J.M. Mar 1992
    LoadFileFromModule:  CODEMGR.C

    Purpose : Try to load a file with a given full name...

    Arguments : Source name
				Doc number updated
				Module handle for the document
				fCxt set to TRUE if document already opened and
				hsf has changed (INLINE FILES) to reload ASM

    Returns   : TRUE is successful
    NOTE      : This function is taking some ANSI strings
				The fully qualified ANSI name is contained in szPathName
				on return
****************************************************************************/
BOOL PASCAL LoadFileFromModule(LPSTR szPathName, LPCSTR szSrcName, CDocument **ppDoc, HSF hsf, BOOL *fCxt, UINT docType)
{
	CDir	dir;
	CPath	pathExeSrcName;
	BOOL	fLoaded = FALSE;

	if (dir.CreateFromString(szPathName))
	{
		if (!pathExeSrcName.CreateFromDirAndFilename(dir, szSrcName))
		{
			szPathName[0] = '\0';
		}
		else
		{
			_ftcscpy(szPathName, pathExeSrcName);
			// first try to find the file in our documents...
			if ( fLoaded = gpISrc->FindDocument(pathExeSrcName, ppDoc, TRUE) )
			{
				if ( docType != DISASSY_WIN && hsf != gpISrc->GetHSF(*ppDoc) )
				{
//					gpISrc->SetCallback(*ppDoc, (FARPROC)TextDocCallback);
					gpISrc->SetHSF(*ppDoc, hsf);
					*fCxt = TRUE ;
				}
			}
			else if (docType && FileExist(pathExeSrcName) )
			{
				if ( docType == DISASSY_WIN )
					fLoaded = TRUE;
				else
				{
					fLoaded = gpISrc->LoadDocument(pathExeSrcName, (FARPROC)TextDocCallback) != NULL && gpISrc->FindDocument(pathExeSrcName, ppDoc, TRUE);
					if ( fLoaded )
					{
//						gpISrc->SetCallback(*ppDoc, (FARPROC)TextDocCallback);
    					gpISrc->SetHSF(*ppDoc, hsf);
						*fCxt = TRUE;
					}
				}
			}
		}
	}

	if ( fLoaded )
	{
		DAMSaveDebugFile( pathExeSrcName, hsf );
	}

    // If I loaded a file, then set the step mode to SRCSTEPPING.
    if ( fLoaded )
	{
        TraceInfo.StepMode = SRCSTEPPING;
	}

	return( fLoaded );
}

void PASCAL EraseCallerLine(void)
{
	ILINE	iLineT = (ILINE)iCallerLine;

	// Erase it if we can identify the file correctly
	// 	1) Have a pTextDoc
	//	2) FindLineStatus returns true (has a CALLER LINE)
	//	3) There is a view (why this???? - from EraseTraceXXX)
	if ( pDocCaller && gpISrc->FirstView(pDocCaller) != NULL)
    {
		if ( gpISrc->FindLineStatus(pDocCaller, CALLER_LINE, FALSE, &iLineT) )
			gpISrc->SetLineStatus(pDocCaller, iLineT + 1, CALLER_LINE, LINESTATUS_OFF, FALSE, TRUE);
		else
        {
			// lines are to be 1 based, so this had better be non-zero
			ASSERT( iCallerLine );
			iLineT = (ILINE)iCallerLine - 1;

			if ( gpISrc->FindLineStatus(pDocCaller, CALLER_LINE, TRUE, &iLineT ) )
				gpISrc->SetLineStatus(pDocCaller, iLineT + 1, CALLER_LINE, LINESTATUS_OFF, FALSE, TRUE);
		}
	}
	pDocCaller = NULL;
}

void PASCAL SetCallerLine(CDocument *pDoc, int iLine, BOOL fMoveEditor )
{
	EraseCallerLine();

	if ( pDoc && (pDoc != pDocTrace || iTraceLine != iLine) )
    {
		gpISrc->SetLineStatus(pDoc, iCallerLine = iLine, CALLER_LINE, LINESTATUS_ON, fMoveEditor, TRUE);

		pDocCaller = pDoc;
		gpISrc->SetCallback(pDoc, (FARPROC)TextDocCallback); //paranoid: set make sure
    }
}

/***************************************************************************
	Implementation:  J.M. Oct 1992
	EraseTraceLine:  CODEMGR.C

	Purpose  : Remove the current CS:IP in main and dup views
	Return   : The doc that owned the CSIP
***************************************************************************/
CDocument *PASCAL EraseTraceLine(VOID)
{
	CDocument   *pCSIPDoc = NULL;
	ILINE	    iLineT = (ILINE)iTraceLine;

	// Erase it if we can identify the file correctly
	// 	1) Have a pTextDoc
	//	2) FindLineStatus returns true (has a CALLER LINE)
	//	3) There is a view (why this???? - from EraseTraceXXX)
	if ( pDocTrace && gpISrc->FirstView(pDocTrace) != NULL )
    {
		if ( gpISrc->FindLineStatus(pDocTrace, CURRENT_LINE, FALSE, &iLineT) )
        {
			gpISrc->SetLineStatus(pDocTrace, iLineT + 1, CURRENT_LINE, LINESTATUS_OFF, FALSE, TRUE);

#ifdef CODECOVERAGE
			if ( runDebugParams.fCodeCoverage ) {
				pTextDocTrace->SetLineStatus(
					iLineT + 1,
					COVERAGE_LINE,
					LINESTATUS_ON,
					FALSE,
					TRUE
				);
			}
#endif	// CODECOVERAGE
		}
		else
        {
			// lines are to be 1 based, so this had better be non-zero
			ASSERT( iTraceLine );
			iLineT = (ILINE)iTraceLine - 1;

			if ( gpISrc->FindLineStatus(pDocTrace, CURRENT_LINE, TRUE, &iLineT) )
             {
				gpISrc->SetLineStatus(pDocTrace, iLineT + 1, CURRENT_LINE, LINESTATUS_OFF, FALSE, TRUE);

#ifdef CODECOVERAGE
				if ( runDebugParams.fCodeCoverage ) {
					pTextDocTrace->SetLineStatus(
						iLineT + 1,
						COVERAGE_LINE,
						LINESTATUS_ON,
						FALSE,
						TRUE
					);
				}
#endif	// CODECOVERAGE
			}
		}
		pCSIPDoc = pDocTrace;
	}

	pDocTrace = NULL;
	TraceInfo.pDoc = NULL;

	return( pCSIPDoc );
}

// [CAVIAR #5293 11/10/92 v-natjm]
/***************************************************************************
	Implementation  :  J.M. Oct 1992
	RestoreTraceLine:  CODEMGR.C

	Purpose  : Restore the current CS:IP in main and dup views
	Return   : Nothing
***************************************************************************/

VOID PASCAL RestoreTraceLine(CDocument *pDoc, BOOL fEditing /* = FALSE */)
{

	if( pDoc )
    {
		// Make sure that we don't display two of them!
		EraseTraceLine();

		// If the caller is on the same line as the new PC, hide
		// the old caller widget
		if ( pDoc == pDocCaller && iCallerLine == TraceInfo.CurTraceLine)
			EraseCallerLine();

		if (fEditing)
			gpISrc->SetLineStatus(pDoc, iTraceLine = TraceInfo.CurTraceLine, CURRENT_LINE, LINESTATUS_ON, FALSE,TRUE);
		else
			gpISrc->SetLineStatus(pDoc, iTraceLine = TraceInfo.CurTraceLine, CURRENT_LINE, LINESTATUS_ON, TRUE,TRUE);

		pDocTrace = pDoc;
		TraceInfo.pDoc = pDocTrace;
		gpISrc->SetCallback(pDoc, (FARPROC)TextDocCallback);
	}
}

// [CAVIAR #4801 11/01/92 v-natjm]
/***************************************************************************
	Implementation:         J.M. Oct 1992
	UpdateAllDebugWindows:  CODEMGR.C

	Purpose  : Forces a repaint of all debug windows
			   with the given cxf
***************************************************************************/
VOID PASCAL UpdateAllDebugWindows(PCXF pCxf)
{
	UINT	wMsg;

	if ( !DebuggeeAlive() )
		wMsg = WU_CLEARDEBUG;
	else
		wMsg = WU_UPDATEDEBUG;

	if( pViewCpu )
		pViewCpu->SendMessage( wMsg, wPCXF, (DWORD)pCxf );

	if ( pViewMemory )
		pViewMemory->SendMessage(wMsg, wPCXF, (DWORD)pCxf);

	if ( pViewCalls )
		pViewCalls->SendMessage(wMsg, wPCXF, (DWORD)pCxf);

	if ( pViewDisassy )
		pViewDisassy->SendMessage(wMsg, wPCXF, (DWORD)pCxf);

	if ( g_pWatchView )
		g_pWatchView->SendMessage(wMsg, wPCXF, (DWORD)pCxf);
		
	if ( g_pVarsView )
		g_pVarsView->SendMessage(wMsg, wPCXF, (DWORD)pCxf);
}

/****************************************************************************

	FUNCTION:   SetDebugLines

	PURPOSE:    Given a doc, set the debug line highlights,
				(ie breakpoints, current_line) that refer to that
				doc.

	NOTE:       This can be called whether or not there is a current
				debuggee.  When there isn't, only source line bps
				are highlighted.

****************************************************************************/
void SetDebugLines(CDocument *pDoc, BOOL ResetTraceInfo, BOOL fBPOnly)
{
	PBREAKPOINTNODE pBpNode;
	char SrcName[_MAX_PATH];
	ILINE SrcLine;
	UOFFSET SrcDelta;
	HSF hsf ;

	extern CXF cxfCalls ;
	
//	gpISrc->SetCallback(pDoc, (FARPROC)TextDocCallback); //paranoid: set make sure

	// Breakpoints:
	pBpNode = BHFirstBPNode();
	while (pBpNode != NULL)
	{
		// Even without a debuggee we can highlight source/line breakpoints
		if ( pbpnFileLineNode(pBpNode) )
		{
			CDocument   *pCDocument;

			// Special searching algorithm
			// Again, see wow comments above [cbrown]
			CString str = GetDebugPathName(pDoc);
			if ( BFHFindDoc((LPTSTR)(LPCTSTR)str, &pCDocument) )
				BFHShowBPNode ( pBpNode );
		}
		else if (DebuggeeAlive())
		{
			// Highlight ALL location style breakpoints
			if (pbpnLocationNode(pBpNode))
				BFHShowCodeBP(pBpNode) ;
		}

		pBpNode = pbpnNext(pBpNode);
	}

	// [vs98 10794 10/26/97 georgiop]
	// Introduced fBPOnly to avoid changing traceinfo when called 
	// by AdjustBreakpointLines (Note that the ResetTraceInfo 
	// argument is not being used in the manner its name suggests)
	if (fBPOnly)
		return;

	// [CAVIAR #3810 11/09/92 v-natjm]
	// Install the current CS:IP line only when not running and when the
	// current document that owns the CS:IP is unknown
	// Current line:
	if ( DebuggeeAlive() && !DebuggeeRunning() )
	{
		// Set CallerLine before setting CurrentLine, in case they are same
		if ( PcxfLocal() == &cxfCalls )
		{
			LPADDR	lpaddr;

			lpaddr = (LPADDR)(SHpADDRFrompCXT( SHpCXTFrompCXF( &cxfCalls ) ) );

			if ( GetSourceFrompADDR(lpaddr,SrcName,sizeof(SrcName),(LONG *)&SrcLine,NULL,&hsf) )
			{
				if (( gpISrc->GetDocFromHsf(hsf) == pDoc) || 
					( gpISrc->GetDocFromFileName(SrcName) == pDoc ))
				{
					SetCallerLine(pDoc, (int)SrcLine, FALSE);
				}
				else
				{
					// try and find it based solely on the name
					// (this finds SQL documents)
					CDocument *pFound;
					if (gpISrc->FindDocument( SrcName, &pFound, TRUE ) && (pFound==pDoc))
						SetCallerLine(pDoc, (int)SrcLine, FALSE );
				}
			}
		}
		
//		if ( TraceInfo.pDoc == NULL )
		{
			if ( GetCurrentSource(SrcName,sizeof(SrcName),&SrcLine,&SrcDelta,&hsf) )
			{
				// show the current line if it exists in the file
				if (( gpISrc->GetDocFromHsf(hsf) == pDoc) || 
					( gpISrc->GetDocFromFileName(SrcName) == pDoc ))
			    {
					TraceInfo.CurTraceLine = SrcLine;
					TraceInfo.CurTraceDelta = SrcDelta ;
					RestoreTraceLine(pDoc, ResetTraceInfo);
				}
			}
		}
	}
}

/****************************************************************************

	FUNCTION:       AdjustDebugLines

	PURPOSE:        Updates source/line breakpoint nodes when lines are
					added/deleted to a file in the editor.  If Added is
					TRUE the lines have been added otherwise they've been
					deleted.  Also updates the TraceInfo var.
					
					When fAdded is TRUE, fLineBreak indicates if the inserted
					text starts with a line break. This is used to heuristically 
					update the location of a break point on the StartLine, either
					move or stay.

	NOTE:           This is called from the editor every time lines are
					added or deleted.
					Insertions are always performed BEFORE the StartLine.
					Deletions are always performed INCLUDING the StartLine.
					StartLine is passed 0 relative.

					Also note that for the TraceInfo, all we avoid is
					having multiple trace lines.  If lines are added
					or deleted to a file the current line will still
					seem wrong as this info. comes from the debugging
					info.

****************************************************************************/
void PASCAL AdjustDebugLines(CDocument *pDoc, int StartLine, int NumberLines, BOOL fAdded, BOOL fLineBreak, BOOL fWhiteSpaceOnly)
{
	gpISrc->SetCallback(pDoc, (FARPROC)TextDocCallback); //paranoid: set make sure

	CString strPath = GetDebugPathName(pDoc);
	if (strPath.IsEmpty())
		return;							// do nothing if not a real doc (eg SQL)

	// Update the breakpoints:
	AdjustBreakpointLines(strPath, StartLine, NumberLines, fAdded, fLineBreak);

	if (DebuggeeAlive())
	{
		// Update current line:
		StartLine++;
		if ((TraceInfo.pDoc != NULL) && (TraceInfo.pDoc == pDoc))
		{
			// This is our file so:
			if (StartLine <= TraceInfo.CurTraceLine)
			{
				if (fAdded && 
					// vc98 14373: Don't move the IP if the user breaks the current 
					// line, unless the CurTraceLine ends up containing empty space only
					(StartLine < TraceInfo.CurTraceLine || fWhiteSpaceOnly))
				{
					// Add NumberLines
					TraceInfo.CurTraceLine += NumberLines;
					RestoreTraceLine (pDoc, TRUE);

				}
				else // Deleted
				{
					// If the trace line itself is being deleted
					// then we lose it altogether
					if (TraceInfo.CurTraceLine <= (StartLine+NumberLines-1))
						TraceInfo.pDoc = NULL;
					else
					{
						// Subtract NumberLines
						TraceInfo.CurTraceLine -= NumberLines;
						RestoreTraceLine (pDoc, TRUE);

					}
				}
			}
		}
	}

}

BOOL
GetCurrentSource(
	LPSTR 		SrcName,
	WORD 		SrcLen,
	ILINE*		pSrcLine,
	UOFFSET*	pDelta,
	HSF*		lphsf
	)
{
	ADDR addr ;
	XOSD xosd ;

	xosd = SYGetAddr(hpidCurr,htidCurr,adrPC,(LPADDR)&addr) ;
	return(GetSourceFrompADDR(
	       &addr,
	       SrcName,
	       SrcLen,
	       (LONG *)pSrcLine,
	       pDelta,
	       lphsf)) ;
}

/****************************************************************************

	FUNCTION:   MoveEditorToAddr()

	PURPOSE:    Takes a PADDR structure and attempts to position
				the editor at the equivalent source line.

	RETURNS:    TRUE if (de)highlighted, FALSE otherwise

****************************************************************************/
CView *PASCAL MoveEditorToAddr(LPADDR pAddr)
{
	char szFname[_MAX_PATH];
	ILINE FnameLine;
	CDocument *pDoc;
	BOOL GotSource;
	HSF hsf ;
	BOOL fCxt ;
	CView *pView = NULL;


	if (!GetSourceFrompADDR(pAddr,szFname,sizeof(szFname),(LONG *)&FnameLine,NULL,&hsf))
    {
		ADDR	addrFixed = *pAddr;

		// Can't go to virtual addresses, e.g. if the user entered
		// "foo" where foo is some function in a DLL which is not
		// yet resident, but for which we have already loaded
		// symbols
		if (SYFixupAddr(&addrFixed))
        {
			// This wll make sure that the window is either
			// open or has the input focus
			if ( !pViewDisassy )
				OpenDebugWindow( DISASSY_WIN );
			else if (GetCurView() != pViewDisassy)	// vc98 13474
				theApp.SetActiveView( pViewDisassy->GetSafeHwnd() );			

			// Window is open, set to new address
			if( pViewDisassy )
            {
				CXF cxf = {0};

				// HACK: we have to pass a CXF to WU_UPDATEDEBUG, but we don't
				// have one available!  However, the ADDR field is the only part
				// of the CXT that the disassembly window actually looks at.
				*SHpADDRFrompCXT(SHpCXTFrompCXF(&cxf)) = *pAddr;

				pViewDisassy->SendMessage(WU_UPDATEDEBUG,wPCXF,(DWORD)&cxf);
			}
		}
	}
	else
    {
		GotSource = (((pDoc = gpISrc->GetDocFromHsf(hsf)) != NULL) || 
			         ((pDoc = gpISrc->GetDocFromFileName(szFname)) != NULL)) ;

		if (!GotSource)
        {
			// Better call our multi path-loading function
			GotSource = SearchFileFromModule(szFname,&pDoc,hsf,&fCxt,DOC_WIN) ;
		}
		else
			GotSource = SF_NOERROR ;

		if (GotSource == SF_NOERROR)
        {
//			gpISrc->SetCallback(pDoc, (FARPROC)TextDocCallback); //paranoid: set make sure
			CDocument   *pCDocument = pDoc;

			// If another view is on top let's get this one
			pView = gpISrc->FindTextDocActiveView(pCDocument);
			ASSERT(pView);

			// Make sure window active
        	theApp.SetActiveView(pView->GetSafeHwnd());

			// DOLPHIN:1094
			// Something changed such that the editor is zero based for
			// line numbers while the OMF is 1 based.  Now, subtract 1
			// from the OMF line number to set the editor at the correct
			// position.
			ASSERT(FnameLine);
			gpISrc->Select(pView, FnameLine - 1, 0, FALSE);
		}
		else
        {
			if(GotSource == SF_NOFILE)
            {
				// We already gave the user a chance to respecify the directory
				// with the Find Source dialog. She must have pressed cancel
				// for us to reach this stage. So there is no point in popping
				// up an Error box here.
			}
			else
            {
				// Don't notify if we are out of documents,
				// It's already handled in SearchFileFromModule()
			}
		}
	}
	return( pView );
}

/****************************************************************************

	FUNCTION:	FAddrFromSz()

	PURPOSE:	Attempts to parse a string into an address

	RETURNS:	TRUE if successful

****************************************************************************/
BOOL FAddrFromSz(char *sz, ADDR* paddr)
{
	BREAKPOINTNODE	bpn = {BPLOC,};
	PBP				pbp;
	CXF curCxf ( CxfWatch( ) );

	// try to parse string
	if (ParseCV400Location(sz, &bpn) || ParseQC25Location (sz, &bpn)) {
		// try to resolve it to an address
		bpnEnabled( bpn ) = TRUE;

		// Address expression, let the BPParseAddress function deal with
		// overloaded symbols for disambiguation.  If source lines, let
		// breakpts.c do it
		if ( bpnLocType( bpn ) == BPLOCADDREXPR ) {
		    memset( &pbp, 0, sizeof( PBP ) );

		    pbp.lszCmd = (char *)sz;
		    pbp.BPSegType = EECODE;
			pbp.pCXF = &curCxf;

			// Save for ambiguation dialog
			bpnBPTMindex( bpn ) = AMB_STS_UNRESOLVED ;
			BHSetLastBPNode( &bpn );

		    if ( BPParseAddress( &pbp, TRUE ) ) {
				return FALSE;
		    }

// Really should fail since the address isn't loaded yet?
//		    if (!SHFIsAddrNonVirtual(&pbp.Addr)) {
//				return FALSE;
//			}

		    *paddr = pbp.Addr;
			return TRUE;
		}
		else if (LoopExeGetCV400BPPacket(&bpn, &pbp)&& bpnBPGroup( bpn ) == 0 ) {
			// got it
			*paddr = pbp.Addr;
			return TRUE;
		}
	}

	return FALSE;
}

/****************************************************************************

	FUNCTION:	FAddrFromRibbonEdit()

	PURPOSE:	Attempts to convert the text of the ribbon edit box into an
				address

	RETURNS:	TRUE if successful

****************************************************************************/
BOOL FAddrFromRibbonEdit(ADDR* paddr)
{
	GCAINFO			gcai = {0};

	ASSERT(IsFindComboActive());

	SendMessage(GetFocus(), WM_GETTEXT,
		sizeof(gcai.szExpr), (LPARAM)gcai.szExpr);

	return FAddrFromSz(gcai.szExpr, paddr);
}

/***************************************************************************
		 Revision     : J.M. Dec 1991
		 UpdateRadix  : Old version

		 Purpose      : Set the current radix for Languages EE
		 Arguments    : TRUE / FALSE for decimal radix
		 Returns      : None
***************************************************************************/
void PASCAL UpdateRadix(BOOL Decimal)
{
	int ilan;

	if (Decimal) radix = 10 ;
	else radix = 16 ;

	// Update stored radix for EEs
	// In fact, we just update the first Language C++
	for (ilan = 0; ilan < clan; ilan++)
		rglan[ilan].Radix = radix;

	if ( DebuggeeAlive( ) && !DebuggeeRunning( ) )
		UpdateDebuggerState(UPDATE_WATCH | UPDATE_LOCALS | UPDATE_CALLS);
}

void PASCAL ZapInt3s(void)
{
}

#if 0
#define SCRNSAVE_BUF 10
/**************************************************************************
		Revision        : J.M. Apr. 1992
		SetScrnSaveStat : ( CODEMGR.C )

		Purpose         :       Handle the screen saver status while in
								Hard mode
		Arguments       :       BOOL fHardMode
**************************************************************************/
void PASCAL SetScrnSaveStat(BOOL fHardMode)
{
	const char szWin[] = "windows" ;
	const char szKey[] = "ScreenSaveActive" ;
	const char szDef[] = "None" ;
	char szBuf[SCRNSAVE_BUF] ;
	int nbChar ;
	char szStat[] = "X" ;
	static BOOL bChanged = FALSE ;

	nbChar = GetProfileString(szWin,szKey,szDef,szBuf,SCRNSAVE_BUF);
	if((nbChar > 0) && _ftcsicmp(szDef,szBuf)) {
		// Install the screen save active flag
		szStat[0] = (fHardMode? '0' : '1') ;
		// Check if different from the existing one
		if(szBuf[0] != szStat[0]) {
			// Write the new status
			if(fHardMode) {
				bChanged = TRUE ;
				WriteProfileString(szWin,szKey,szStat) ;
			}
			else {
				// Did us change the status ?
				if(bChanged) {
					bChanged = FALSE ;
					WriteProfileString(szWin,szKey,szStat) ;
				}
			}
		}
	}
}
#endif

/***************************************************************************
		 Implementation:        J.M. Fev 1992
		 GetLineOffsetFromAddr: Mixmode exported function

		 Purpose       : Retreive a line and offset from addr and mod
		 Arguments     : LPADDR   pointer to the address
						 USHORT *  pointer to the line number
						 USHORT *  pointer to the line offset

		 Returns       : FALSE if offset was rounded TRUE if not
						 line = 0 if not found
						 If we don't find a matching offset we round it
						 to a valid value ( CS:IP modified by user... )
***************************************************************************/
BOOL PASCAL GetLineOffsetFromAddr(void *lpaddr,int *pLine,long *pOffset)
{
	ADDR    addr = *(LPADDR)lpaddr ;
	ADDR    addrStart ;
	WORD    wLn = 0 ;
	SHOFF   cbLn ;
	SHOFF   dbLn ;
	SHOFF   cbOp ;
	SDI     sdi ;

	if (!SLLineFromAddr(&addr, &wLn, &cbLn, &dbLn))
		return TRUE ;
	*pOffset = (long) dbLn;
	*pLine = (int) wLn;

	GetAddrOff(addr) -= dbLn ;
	// Unassemble this line instruction by instruction to verify
	// if we have a valid offset !!!
	cbOp = 0 ;
	cbLn++ ;
	memset(&sdi,0,sizeof(SDI)) ;
	addrStart = sdi.addr = addr ;
	sdi.dop = (DOP)0 ;

	//AuxPrintf4("Line: %d Count: %lu Delta: %lu",wLn,cbLn,dbLn) ;
	while (cbOp < cbLn) {
		// We are on a frontier... everything is OK
		if (dbLn == cbOp) return TRUE ;
		// We have missed a frontier let's break...
		if (dbLn < cbOp) break ;
		addrStart = sdi.addr ;
		SYUnassemble(hpidCurr,htidCurr,(LPSDI)&sdi) ;
		cbOp = (GetAddrOff(sdi.addr)-GetAddrOff(addr)) ;
		AuxAddr(&(sdi.addr)) ;
    }
    // We should use the last cbOp as a valid offset
    *(LPADDR)lpaddr = addrStart ;
	return FALSE ;
}

/***************************************************************************
		 Revision      : J.M. Dec 1991
		 GetHsfFromDoc: utility for UnAssemble / Step to cursor

		 Purpose       : Get the HSF from the module name
		 Arguments     : LPSTR : module name with path or not
						 LPHSF : pointer to HSF
						 PBOOL : pointer to fAmbiguous flag

		 Returns       : DIS_RET for compatibility

***************************************************************************/
DIS_RET PASCAL GetHsfFromDoc(CDocument *pDoc, HSF *lpHsf)
{
    HMOD      hmod;
	HSF       hsfFound = NULL;
    HEXE      hexe = (HEXE)NULL;
	char      *szNoPath ;
	char      *sz;
	BOOL      fAmbiguous = FALSE;


    _ftcscpy(szTmp, pDoc->GetPathName()) ;

    // Make a near pointer
    szNoPath = szTmp ;

	if(sz = _ftcsrchr( szNoPath, _T('\\') )) szNoPath = sz + 1;
	if(sz = _ftcsrchr( szNoPath, _T('/') )) szNoPath = sz + 1;
	if(sz = _ftcsrchr( szNoPath, _T(':') )) szNoPath = sz + 1;
	sz = NULL;

	// see if the following is a module
	while ( !fAmbiguous && ( hexe = SHGetNextExe( hexe ) ) )
    {
		hmod = (HMOD) NULL;

		while ( !fAmbiguous && ( hmod = SHGetNextMod ( hexe, hmod ) ) )
        {
			BOOL fDone = FALSE ;
			WORD iFile = 1 ;
			while( !fDone && !fAmbiguous )
            {
				UCHAR *	lpch = (UCHAR *) SLNameFromHmod( hmod, iFile++ );

				if(!lpch)
					fDone = TRUE;
				else
                {
					char szFile[_MAX_PATH];
					char *sz;
					char *szNPath;

					_fmemcpy( szFile, lpch + 1, *lpch );
					szFile[ *lpch ] = '\0';
					szNPath = szFile;

					if(sz = _ftcsrchr(szNPath, _T('\\')))
                        szNPath = sz + 1;

					if(sz = _ftcsrchr(szNPath, _T('/')))
                        szNPath = sz + 1;

					if(sz = _ftcsrchr(szNPath, _T(':')))
                        szNPath = sz + 1;

					CharUpper(szNPath);
					if(!_ftcsicmp(szNPath,szNoPath))
                    {
						fDone = TRUE;
						if(hsfFound)
							fAmbiguous = TRUE;
						else
                        {
							hsfFound = SLHsfFromFile ( hmod, (LSZ)szNoPath );
							//gpISrc->SetCallback(pDoc, (FARPROC)TextDocCallback); //paranoid: set make sure
							gpISrc->SetHSF(pDoc, hsfFound);
						}
					}
				}
			}
		}
	}
	*lpHsf = hsfFound ;

	if(hsfFound == (HSF)NULL)
        return( DIS_NOMODULE );

	return( DIS_NOERROR );
}

BOOL DEBUGGlobalInit(VOID)
{
    lpdbf = (LPDBF)NULL ;

    // WARNING !!!
    // Don't change this dangerous variable...
    epiDef = epiDefault ;

	// Initialize critical section for callback queue
	QCInitCritSection(&csDbgTimerSet);

	return TRUE ;
}

BOOL
DEBUGInitialize(
	BOOL	fQuickRestart  // = FALSE
	)
/*++

Routine Description:

	Initialize some global state for the debugger.  This should really
	be a method in the CDebug object.

Arguments:

	fQuickRestart -- TRUE means that we are re-initializing in response to
					 a quick restart, otherwise a normal initialization.

--*/
{

	// in the quick restart case, we only need to re-init the TraceInfo
	// structure.
	
	if (fQuickRestart)
	{
	    TraceInfo.pDoc = NULL ;
    	TraceInfo.CurTraceLine = 0 ;
	    TraceInfo.CurTraceDelta = 0 ;
	    TraceInfo.StepMode = SRCSTEPPING ;
	    TraceInfo.fModeChanged = TRUE ;

		return TRUE;
	}

	// Initialize memory-manager
	if (!cvw3_minit())
		return FALSE;						// if very low on memory

	// allow multiple debugging sessions
	ResetCV400Globals();

    // ReInitialize the TraceInfo structure
    TraceInfo.pDoc = NULL ;
    TraceInfo.CurTraceLine = 0 ;
    TraceInfo.CurTraceDelta = 0 ;
    TraceInfo.StepMode = SRCSTEPPING ;
    TraceInfo.fModeChanged = TRUE ;


	// Initialize the memory manager

	if (!BMInit())
	{
		return FALSE;
	}

	// Non blocking mode
	fBlocking = TRUE ;

    // Restart OSDEBUG

        // We used to turn off memory tracking here,
        // but tests with memory tracking on here pass fine,
        // so I turned it off.  If you find leaks here that
        // you don't think should be fixed (weird), uncomment
        // these lines.

	// BOOL bEnable = AfxEnableMemoryTracking(FALSE);
	BOOL bSuccess = pDebugCurr->InitOSD();
	// AfxEnableMemoryTracking(bEnable);

    if(!bSuccess)
        return FALSE ;

	// Init linked list manager for the bps
	BPFirstBPI();

	fDebugInitialize = TRUE;
	return TRUE ;
}

void DEBUGDestroy(VOID)
{
	if (!fDebugInitialize)
		return;

	// Delete the CV400 breakpoints - our breakpoint list remains
	ClearCV400Breakpoints();

	// [CAVIAR #7413 01/25/93 v-natjm]
	// Reset the context
	CLFreeWalkbackStack(TRUE) ;

	//	Free all the OMF information loaded...
	
    if (lpprcCurr && lpprcCurr->IsOmfLoaded ())
	{
        SYUnFixupOmf() ;
        lpprcCurr->SetOmfLoaded (FALSE);
	}

    // If InitOSD failed on loading SH or EE we wont have any function
    // initialized in the lpdbf structure !
    // This also means that we don't have to execute the
    // unnecessary following code...except for unloading the SH DLL
    if (lpdbf == (LPDBF)NULL)
    {
		pDebugCurr->LoadComponent(DLL_SH);
		pDebugCurr->InitializeComponent(DLL_SH);
		return ;
    }

    // Discard all the EM attached to our PIDs
    if(LLChlleInLl(llprc) > 0) {
		HLLE  hprc = (HLLE)NULL ;
		while(hprc = LLHlleFindNext(llprc,hprc)) {
			LPPRC lpprc = (LPPRC)LLLpvFromHlle(hprc);
   			if (lpprc->stp != stpDead) {
				//AuxPrintf2("EM: PID %X",lpprc->hpid) ;
				if(hemCurr != 0 && lpprc->hpid != hpidNull) {
					OSDDiscardEM(lpprc->hpid, htidCurr, hemCurr) ;
				}

				if(hnmCurr != 0 && lpprc->hpid != hpidNull) {
					OSDDiscardEM(lpprc->hpid, htidCurr, hnmCurr) ;
				}

				if (lpprc->stp != stpNotStarted && lpprc->hpid != hpidNull)
					OSDDestroyHpid(lpprc->hpid);
					lpprc->hpid = hpidNull;
			}
			UnlockHlle(hprc) ;
		}
    }

	// We also have to free the process list
	LLChlleDestroyLl(llprc) ;
	llprc = hmemNull ;
	hprcCurr = 0;
	lpprcCurr = NULL;

	// Free Language structure
	for(int i=0;i<clan;i++)
		{
		// These are locally allocated !
		if(rglan[i].szFile)      free(rglan[i].szFile) ;
		if(rglan[i].szName)      free(rglan[i].szName) ;
		if(rglan[i].szTitle)     free(rglan[i].szTitle) ;
		if(rglan[i].szExt)       free(rglan[i].szExt) ;
		}

	// Free libraries loaded except TOOLHELP just flagged out
	pDebugCurr->UnInitOSD();

    // Free up the callback packets list
    LLChlleDestroyLl(qCallBack) ;
    qCallBack = hmemNull ;
	LLChlleDestroyLl(qCallBackReturns) ;
	qCallBackReturns = hmemNull;

    // Reset all the documents...
    gpISrc->ResetDocMod();

	// Free all left-over linked lists
	FreeDbgLinkedLists();

	// Free all allocated memory...
	// (unfortunately that includes that given to the expression evaluator)
	cvw3_mcleanup();

	fDebugInitialize = FALSE;
}

// [dolphin #2755 mikemo] remove this function

void BreakStatus(int bpt)
{
	switch(bpt) {
		case BPDEADCHILD:
			//AuxPrintf1(">> BP DEAD") ;
			break ;
		case BPABORT:
			//AuxPrintf1(">> BP ABORT") ;
			break ;
		case BPHARDCODEDBP:
			//AuxPrintf1(">> BP HARDCODEDBP") ;
			break;
		case BPCONTINUE:
			//AuxPrintf1(">> BP CONT") ;
			break ;
		case BPTMP:
			//AuxPrintf1(">> BP TMP") ;
			break ;
		case BPBREAKPT:
			//AuxPrintf1(">> BP BREAKPT") ;
			break ;
		default:
			//ASSERT(FALSE) ;
			break ;
	}
}

// This OSDAssert is to be called by a DLL
int LOADDS PASCAL OSDAssert(
    char FAR *lszMsg,
    char FAR *lszFile,
#ifdef OSDEBUG4
    DWORD iln)
#else
    unsigned iln)
#endif
{
// AuxPrintf4("OSDASSERT:%s,%s,%d",lszMsg,lszFile,iln);
#ifdef _DEBUG
	if (AfxAssertFailedLine(lszFile, iln))
		AfxDebugBreak ();
#endif
	return(1) ;
}

// Uses EE function to compare
SHFLAG PASCAL LOADDS FAR CVfnCmp(HVOID pName,HVOID pSym,char *stName,SHFLAG fCase)
{
	return(*pEXF->pfnCmp)(pName,pSym,stName,fCase) ;
}

// Uses EE function to compare
SHFLAG PASCAL LOADDS FAR CVtdCmp(HVOID pName,HVOID pSym,char *stName,SHFLAG fCase)
{
	return(*pEXF->ptdCmp)(pName,pSym,stName,fCase) ;
}

// Uses EE function to compare
SHFLAG PASCAL LOADDS FAR CVcsCmp(HVOID pName,HVOID pSym,char *stName,SHFLAG fCase)
{
	return(*pEXF->pcsCmp)(pName,pSym,stName,fCase) ;
}

// Compare Regular Expressions
BOOL PASCAL LOADDS CVRECompare(char FAR *lp1,char *lp2)
{
	return(gpISrc->RECompare(lp1,lp2)) ;
}

MPT
PASCAL
GetTargetProcessor(
	HPID	hpid
	);

BOOL
PASCAL
GetUnicodeStrings(
	)
{
	return runDebugParams.fUnicodeStrings;
}
	
/***********************************************************************
	  Revision    : J.M. Dec 1991
	  EsLoadExpr  : ( CV0.C )

	  Purpose     : load C++ expression evaluator
	  Arguments   : EE DLL module handle
						 Initialization function
						 Language index in EE table
	  Returns     : None
************************************************************************/
void PASCAL ESLoadExpr(int ilan,unsigned int hmod,LPFNEEINIT lpfnEEInitExpr)
{
	CI        Ci;
	EI        Ei;
	PEXF      pexf;

	static CVF  cvf = {
		LDSFmalloc,
		LDSFfree,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
        NULL,
        BMAlloc,
		BMRealloc,
		BMFree,
		BMLock,
		BMUnlock,
		(SHFLAG (LOADDS PASCAL *)(HDEP))BMIsLocked,
		MHOmfLock,
		MHOmfUnLock,
		NULL,
		DHGetDebuggeeBytes,
		DHPutDebuggeeBytes,
		DHGetReg,
		DHSetReg,
		DHSaveReg,
		DHRestoreReg,
		&fIn386Mode,
		&is_assign,
		(QUIT)dsquit,
		&ArrayDefault,
		NULL,
		(SHFLAG (LOADDS PASCAL *)(LPADDR))SYFixupAddr,
		(SHFLAG (LOADDS PASCAL *)(LPADDR))SYUnFixupAddr,
		CVfnCmp,
		CVtdCmp,
		CVcsCmp,
		(UINT (LOADDS PASCAL *)(char FAR *, char FAR *, UINT))OSDAssert,
		DHSetupExecute,
		DHCleanUpExecute,
		DHStartExecute,
    #ifdef OSDEBUG4
        NULL,
        SYProcessor
    #else
        NULL
    #endif
		};

static CRF  crf = {
	0,
	ds_ultoa,
	ds_itoa,
	ds_ltoa,
	ds_eprintf,
	ds_sprintf,
	ds_strtold
	};

	Ci.cbCI     = sizeof(CI);
	Ci.Version      = 1;

// Make our lpcvf point on the CVF structure
    lpcvf = &cvf ;

// Load the symbol handler module and functions
	if(lpshf == NULL)
	{
		ASSERT(pDebugCurr != NULL);
		pDebugCurr->LoadComponent(DLL_SH);
		if (!pDebugCurr->InitializeComponent(DLL_SH))
		{
			pDebugCurr->UnLoadComponent(DLL_SH);
			return;
		}
	}

	cvf.pSHGetNextExe         = lpshf->pSHGetNextExe;
	cvf.pSHHexeFromHmod       = lpshf->pSHHexeFromHmod;
	cvf.pSHGetNextMod         = lpshf->pSHGetNextMod;
	cvf.pSHGetCxtFromHmod     = lpshf->pSHGetCxtFromHmod;
	cvf.pSHSetCxt             = lpshf->pSHSetCxt;
	cvf.pSHSetCxtMod          = lpshf->pSHSetCxtMod;
#if defined(OSDEBUG4)
    cvf.pSHFindNameInGlobal   = (HSYM (PASCAL *)(HSYM, PCXT, HVOID, SHFLAG, PFNCMP, PCXT))lpshf->pSHFindNameInGlobal;
    cvf.pSHFindNameInContext  = (HSYM (PASCAL *)(HSYM, PCXT, HVOID, SHFLAG, PFNCMP, PCXT))lpshf->pSHFindNameInContext;
#else
    cvf.pSHFindNameInGlobal   = (HSYM (PASCAL *)(HSYM, PCXT, HVOID, SHFLAG, PFNCMP, SHFLAG, PCXT))lpshf->pSHFindNameInGlobal;
    cvf.pSHFindNameInContext  = (HSYM (PASCAL *)(HSYM, PCXT, HVOID, SHFLAG, PFNCMP, SHFLAG, PCXT))lpshf->pSHFindNameInContext;
#endif
    cvf.pSHFindNameInTypes    = (HSYM (PASCAL *)( PCXT, LPSSTR, SHFLAG, PFNCMP, PCXT ))lpshf->pSHFindNameInTypes;
	cvf.pSHGoToParent         = lpshf->pSHGoToParent;
	cvf.pSHHsymFromPcxt       = lpshf->pSHHsymFromPcxt;
	cvf.pSHNextHsym           = lpshf->pSHNextHsym;
	cvf.pSHGetFuncCxf         = CLGetFuncCXF;
	cvf.pSHGetModName         = lpshf->pSHGetModName;
	cvf.pSHGetExeName         = lpshf->pSHGetExeName;
	cvf.pSHGethExeFromName    = lpshf->pSHGethExeFromName;
	cvf.pSHGetNearestHsym     = lpshf->pSHGetNearestHsym;
	cvf.pSHIsInProlog         = lpshf->pSHIsInProlog;
	cvf.pSHIsAddrInCxt        = lpshf->pSHIsAddrInCxt;
	cvf.pSHModelFromAddr      = (UINT (PASCAL *)(PADDR,LPW,LPB,UOFFSET FAR *))lpshf->pSHModelFromAddr ;
    cvf.pSHFindSLink32        = lpshf->pSHFindSLink32;
	cvf.pSLLineFromAddr       = lpshf->pSLLineFromAddr;
	cvf.pSLFLineToAddr        = lpshf->pSLFLineToAddr;
	cvf.pSLNameFromHsf        = lpshf->pSLNameFromHsf;
	cvf.pSLHmodFromHsf        = lpshf->pSLHmodFromHsf;
	cvf.pSLHsfFromPcxt        = lpshf->pSLHsfFromPcxt;
	cvf.pSLHsfFromFile        = lpshf->pSLHsfFromFile;

	cvf.pPHGetNearestHsym     = lpshf->pPHGetNearestHsym;
	cvf.pPHFindNameInPublics  = (HSYM (PASCAL *)(HSYM, HEXE, HVOID, SHFLAG, PFNCMP))lpshf->pPHFindNameInPublics;
	cvf.pTHGetTypeFromIndex   = lpshf->pTHGetTypeFromIndex;
	cvf.pTHGetNextType        = lpshf->pTHGetNextType;
	cvf.pSHCompareRE          = (SHFLAG (PASCAL *)(char FAR *, char FAR *))CVRECompare;

    cvf.pTHAreTypesEqual      = lpshf->pTHAreTypesEqual;
	cvf.pGetTargetProcessor	  = GetTargetProcessor;
	cvf.pGetUnicodeStrings	  = GetUnicodeStrings;

	Ci.pStructCVAPI = &cvf ;
	Ci.pStructCRuntime = &crf ;

	Ei.cbEI = sizeof(EI) ;

	lpfnEEInitExpr(&Ci,&Ei) ;
	pexf = Ei.pStructExprAPI ;

	rglan[ilan].hDLL = (ushort) hmod ;              // DLL handle
	rglan[ilan].pexf = pexf ;                       // pointer on EE funcs
	rglan[ilan].lszIsIdChar = Ei.IdCharacters ;     // Valid ID chars
	rglan[ilan].AssignOp = Ei.Assign ;              // Assign operator "="
	if(rglan[ilan].szName = (char *)malloc(_ftcslen(Ei.EETitle)+1))
		_ftcscpy(rglan[ilan].szName,Ei.EETitle) ;   // EE name
	rglan[ilan].fCaseSensitive = TRUE ;             // For C++ language
	rglan[ilan].Radix =                             // Radix for evaluation
	   (runDebugParams.decimalRadix)? 10:16;        //  (output only)
	rglan[ilan].szFile = NULL ;                     // Complete path
	rglan[ilan].szTitle = NULL ;                    // name displayed to user
	rglan[ilan].fIsFound = TRUE ;                   // File has been found
}


/***************************************************************************
		 Revision      : J.M. Dec 1991
		 BPNbrAtAddr   : ( SWINHDL1.C )

		 Purpose       : Get the BREAKPOINT number at address if existing
                        if fCheckDisabled, in addition return the widgetflags
		 Arguments     : address and count bytes for the OPCODES
		 Returns       : number or -1 if not found
***************************************************************************/
int PASCAL BPNbrAtAddr(
    LPADDR lpaddr,
    UOFFSET cb,
    BOOL fCheckDisabled,
    UINT *pWidgetFlag)
{
	int ibpt = -1 ;

    if(BPFirstBPI()) {
        HLLE hbpt = BPIsAddr(lpaddr,cb,0,TRUE,fCheckDisabled,pWidgetFlag);
        LPBPI lpbpi = (LPBPI)NULL;

        if(hbpt) {
	        lpbpi= (LPBPI)LLLpvFromHlle(hbpt);
	        if(lpbpi && (lpbpi->bpf.f.BpType != BPTMP) ) {
	            ibpt = BPGetNbr ( hbpt );
	        }
	        UnlockHlle ( hbpt );
        }
    }
    return ibpt ;
}

/****************************************************************************
	Set up a temporary breakpoint
****************************************************************************/
BOOL FSetUpTempBp(PADDR paddr,HTHD hthd,int BpType)
{
PBP pbp ;

	memset(&pbp,0,sizeof(PBP)) ;
	pbp.hthd = hthd ;
	pbp.BPType = BpType ;
	pbp.pCXF = &cxfIp ;
	pbp.Addr = *paddr ;
	pbp.fInsert = FALSE ;
	return( BPCommitBP(&pbp) != (HLLE)NULL ) ;
}

/*************************************************************************
	FSetUpTempSourceLineBp

	Purpose		: Create temporary BPs for Go to cursor on ALL instances
				  of lszFile:.wLine.
*************************************************************************/
BOOL FSetUpTempSourceLineBp(LSZ lszFile, ILINE wLine, HTHD hthd) {
	BOOL			fSet = FALSE;
	BREAKPOINTNODE	bpn;

	// We call LoopExeGet... to let the breakpoint handler
	// deal with a source line search. (current line isn't
	// a code line, but line+xxx lines is

	// Setup a BP node for bp at source line
	memset( &bpn, 0, sizeof( bpn ) );
	_fstrcpy( bpnLocCxtSource( bpn ), lszFile );
	bpnInitLine( bpn ) = bpnCurLine( bpn ) = (int)wLine;
	bpnType( bpn ) = BPLOC;
	bpnLocType( bpn ) = BPLOCFILELINE;

	// try to resolve it to an address
	bpnEnabled( bpn ) = TRUE;
	if ( LoopExeGetCV400BPPacket( &bpn, NULL ) ) {
		int		cslp;
		LPSLP	rgslp;

		cslp = SLCAddrFromLine(
			(HEXE)NULL,
			(HMOD)NULL,
			lszFile,
			(WORD)bpnCurLine( bpn ),
			&rgslp
		);

		if ( cslp ) {
			int	islp;

			for( islp = 0; islp < cslp; ++islp ) {
#if defined (_MIPS_) || defined (_ALPHA_)
				if (!MassageBPAddr(&rgslp[islp].addr, &rgslp[islp].cb)) {
					continue;
				}
#endif
				fSet |= FSetUpTempBp( &rgslp[ islp ].addr, hthd, BPTMP );
			}
			SHFree( (LPV)rgslp );
		}
	}

	return fSet;
}

/*************************************************************************
	GoUntil

	Purpose		: runs the child until specified address
*************************************************************************/
void GoUntil(PADDR paddr, HTHD hthd, int BpType, BOOL fPassException,
	BOOL fFlipScreen)
{
	if( FSetUpTempBp( paddr, hthd, BpType ) )
	{
		Go(hthd, fPassException, fFlipScreen) ;
	}
}


BOOL WINAPI
CheckEXEForDebug(
	LPCSTR	lpszEXE,
	BOOL	InANSI,
	BOOL	fQuiet
	)
/*++

Routine Description:

	Checks the image type, loads symbols for the image if necessary.  This
	will prompt the user if fQuite != FALSE.

Comments:

	o	SQL always succeeds.

	o	CrashDump images are considered valid images.  Symbols are not loaded.

--*/
{
	char		szANSIEXE [_MAX_PATH];
	char		szPdbName [_MAX_PATH];
	char		szXbePeName [_MAX_PATH];
	char*		lpszUseEXE = NULL;
	SHE			she = sheNone;
	BOOL		fSucc = FALSE;
	LPCSTR lpszUseXbe;
	IMAGE_INFO	ImageInfo = {0};

	ImageInfo.Size = sizeof (ImageInfo);

	//
	// Do not bother checking for (or loading) debug info if SQL inproc.
	//
	
	if (FIsSqlInproc ()) {
		return TRUE;
	}

	if (!InANSI) {
		_ftcscpy (szANSIEXE, lpszEXE);
		lpszUseEXE = (LPSTR) szANSIEXE;
	} else {
		lpszUseEXE = (LPSTR)lpszEXE;
	}

	/* If we're on Xbox, we need to check for XBE and if so, locate the
	 * original EXE file and search it for symbols */
	if(pDebugCurr->GetPlatform() == xbox) {
		const char *pch;

		/* Check the extension */
		for(pch = lpszUseEXE; *pch; ++pch);
		if(pch - lpszUseEXE >= 4 && !_strnicmp(pch-4, ".xbe", 4)) {
			lpszUseXbe = lpszUseEXE;
			fSucc = SUCCEEDED(XbeGetLocalModulePath(lpszUseXbe, NULL,
				szXbePeName, sizeof szXbePeName));
			if(fSucc)
				lpszUseEXE = szXbePeName;
			else {
				/* We'll just assume that we should proceed without
				 * symbols */
				if(!fQuiet) {
PromptForXbe:
					CNoSymbolInfoDlg	dlg;

					dlg.m_strPrompt = lpszUseXbe;

					if (g_fPromptNoSymbolInfo) {
						if (dlg.DoModal () == IDOK) {
							fSucc = TRUE;
							g_fPromptNoSymbolInfo = !dlg.m_fNoPrompt;
						} else {
							fSucc = FALSE;
						}
					} else {
						fSucc = TRUE;
					}
				} else
					fSucc = TRUE;
				return fSucc;
			}
		}
	}

	fSucc = GetImageInfo (lpszUseEXE, &ImageInfo);

	//
	// Did we get a valid image?
	//
	
	if (!fSucc) {
	
		if (!fQuiet) {
			if(lpszUseXbe)
				/* If we didn't find the EXE pointed to by the XBE, we'll
				 * give the user the choice whether to proceed */
				goto PromptForXbe;
			ErrorBox (ERR_File_Not_Found, lpszUseEXE);
		}

		return FALSE;
		
	} else if (ImageInfo.ImageType != IMAGE_TYPE_WIN32 &&
			   ImageInfo.ImageType != IMAGE_TYPE_CRASHDUMP) {

		if (!fQuiet) {
			ErrorBox (ERR_Invalid_Exe, lpszUseEXE);
		}
		
		return FALSE;
	}


	//
	// Do not load symbols for CrashDump images.
	//
	
	if (ImageInfo.ImageType == IMAGE_TYPE_CRASHDUMP) {
		return TRUE;
	}
	
	//
	// Load symbols, prompting if necessary.  Fail if no symbols and quite
	// mode.
	//

	fSucc = TRUE;
	she = SHLoadDll  (lpszUseEXE, FALSE);

	if (fQuiet && she != sheNone) {

		return FALSE;
	}


	// Check NB0? signature

	switch (she) {
	
		case sheCorruptOmf:
			ErrorBox(ERR_Corrupt_Omf, lpszUseEXE);
			fSucc = FALSE;
			break;

		case sheNoSymbols: {

			CNoSymbolInfoDlg	dlg;

			dlg.m_strPrompt = lpszUseEXE;

			if (g_fPromptNoSymbolInfo) {
				if (dlg.DoModal () == IDOK) {
					fSucc = TRUE;
					g_fPromptNoSymbolInfo = !dlg.m_fNoPrompt;
				} else {
					fSucc = FALSE;
				}
			} else {
				fSucc = TRUE;
			}
		}
		break;


		case sheFutureSymbols:
			if (QuestionBox(ERR_Future_Symbols, MB_YESNO, lpszUseEXE) != IDYES)	{
				fSucc = FALSE;
			}
			break;

		case sheMustRelink:
			if (QuestionBox(ERR_Must_Relink, MB_YESNO, lpszUseEXE) != IDYES) {
				fSucc = FALSE;
			}
			break;

		case sheNotPacked:
			if (QuestionBox(ERR_Not_Packed, MB_YESNO, lpszUseEXE) != IDYES) {
				fSucc = FALSE;
			}
			break;

		case sheOutOfMemory:
			ErrorBox (ERR_SH_No_Memory, lpszUseEXE);
			fSucc = FALSE;
			break;

		case sheFileOpen:
			ErrorBox (ERR_File_Open, lpszUseEXE);
			fSucc = FALSE;
			break;

		case sheNone:
		case sheExportsConverted:
			break;

		case shePdbNotFound:
			SHPdbNameFromExe (lpszUseEXE, szPdbName, sizeof (szPdbName));
			
			if (QuestionBox(ERR_Pdb_Not_Found, MB_YESNO, szPdbName) != IDYES) {
				fSucc = FALSE;
			}
			break;

		case shePdbBadSig:
			SHPdbNameFromExe (lpszUseEXE, szPdbName, sizeof (szPdbName));
			if (QuestionBox (ERR_Pdb_Bad_Signature, MB_YESNO, szPdbName,
							lpszUseEXE) != IDYES) {

				fSucc = FALSE;
			}
			break;

		case shePdbInvalidAge:

			SHPdbNameFromExe (lpszUseEXE, szPdbName, sizeof (szPdbName));

			if (QuestionBox (ERR_Pdb_Invalid_Age, MB_YESNO, szPdbName,
							 lpszUseEXE) != IDYES) {

				fSucc = FALSE;
			}
			break;

		case shePdbOldFormat:
			SHPdbNameFromExe( lpszUseEXE, szPdbName, sizeof( szPdbName ) );
			if (QuestionBox(ERR_Pdb_Old_Format, MB_YESNO, szPdbName) != IDYES)
			{
				fSucc = FALSE;
			}
			break;

		default:
			ASSERT (FALSE);
	}

	return fSucc;
}

/********************************************************************
*** FFindDll - Always verifies that a DLL that is found with the given
***		name is also the correct platform.  Here's how we search:
***
*** For an x86 debuggee (try to match LoadLibrary behavior):
***	o If a full path is specified, just look there.
*** o Look in the exe's dir
*** o Look in the SYSTEM32 dir
*** o Look in the WINDOWS dir
*** o Look in directories listed in PATH environment variable
*** o Look in the debuggee's working dir
*** o Look in the project dir
***
*** For a 68k or ppc debuggee:
***	o If a full path is specified, just look there.
*** o Look in the exe's dir
*** o Look in directories listed in the project's bin directories
*** o Look in directories listed in the project's lib directories
***		(only 'cause the mac mfc dlls are currently in the lib dir!)
*** o Look in the debuggee's working dir
*** o Look in the project dir
********************************************************************/
BOOL PASCAL FFindDLL(LPCTSTR szDLL, CString& strDLLPath, BOOL fQuiet /* =FALSE */)
{
	CString str;
	CPath path;
	CDir dir;
	LPTSTR sz;
	char szTmp[_MAX_PATH];
	BOOL fFound = FALSE;	// TRUE if we find a matching name, regardless of platform
	TCHAR szDLLTemp[_MAX_PATH];
	UINT nPlatformID;

	ASSERT(pDebugCurr);
	nPlatformID = pDebugCurr->GetPlatform();

	/* BUG jlange -- isn't there a better way to do this? */
	if(nPlatformID == xbox)
		nPlatformID = win32x86;

	strDLLPath = szDLL;
	_ftcscpy(szDLLTemp, szDLL);

	if ( !ValidFilename(szDLLTemp, FALSE) )
	{
		// [bug #5369 12-2-92 v-natal] use the frame as parent whenever it's possible
		if (!fQuiet) {
			_ftcsupr(szDLLTemp);
			ErrorBox(ERR_Bad_File_Spec, szDLLTemp);
		}
		return FALSE;
	}

	// Add .DLL if not specified
	_splitpath(szDLLTemp, szDrive, szDir, szFName, szExt);

	if (*szExt == '\0')
		_makepath(szDLLTemp, szDrive, szDir, szFName, "dll");

	// if a full or relative path is specified (possibly UNC), just look there.
	if (*szDrive != '\0' || *szDir != '\0' ||
		(szDLLTemp[0] == '\\' && szDLLTemp[1] == '\\'))
	{

		path.Create(szDLLTemp);
		strDLLPath = path;

		if (!FileExist(path))
        {
			if (!fQuiet)
				ErrorBox(ERR_File_Not_Found, strDLLPath);

			return FALSE;
		}

		if (GetExePlatform(path) != nPlatformID)
        {
			if (!fQuiet)
				ErrorBox(ERR_File_Wrong_Platform, strDLLPath);

			return FALSE;
		}

		strDLLPath = path;
		return TRUE;
	}

	// Don't try to use any project info if we are trying to locate
	// the local exe for debugging (since nothing is set up yet)
	if (theApp.m_jit.GetActive() && !theApp.m_jit.FPathIsReal())
	{
		path.Create(szDLLTemp);
		strDLLPath = path;

		if (!FileExist(path))
        {
			if (!fQuiet)
				ErrorBox(ERR_File_Not_Found, strDLLPath);

			return FALSE;
		}

		if (GetExePlatform(path) != nPlatformID)
        {
			if (!fQuiet)
				ErrorBox(ERR_File_Wrong_Platform, strDLLPath);

			return FALSE;
		}

		return TRUE;
	}

	// Build this file name against where the executable lives
	ASSERT(gpIBldSys && gpIBldSys->IsActiveBuilderValid() == S_OK);

	gpIBldSys->GetCallingProgramName(ACTIVE_BUILDER, str);

	ASSERT(!str.IsEmpty());

	if (!str.IsEmpty())
	{
		VERIFY(path.Create(str));
		VERIFY(dir.CreateFromPath(path));
		if (path.CreateFromDirAndFilename(dir, szDLLTemp) && FileExist(path))
        {
			fFound = TRUE;

			if (GetExePlatform(path) == nPlatformID)
			{
				strDLLPath = path;
				return TRUE;
			}
		}				
	}

	if (nPlatformID == mac68k || nPlatformID == macppc)
	{
		//*** o Look in directories listed in the project's bin directories
		//*** o Look in directories listed in the project's lib directories
		//***		(only 'cause the mac mfc dlls are currently in the lib dir!)
		LPSTR lpszFullPath;

		COleRef<IBuildDirManager> srpBDM;
		if (SUCCEEDED(theApp.FindInterface(IID_IBuildDirManager,
				(LPVOID*) &srpBDM)))
		{
			if (srpBDM->FindFileOnPath(PLATFORM_CURRENT, DIRLIST_PATH,
				szDLLTemp, &lpszFullPath) == S_OK)
			{
				BOOL bPlatform = FALSE;
				fFound = TRUE;

				if (bPlatform = (GetExePlatform(lpszFullPath) == nPlatformID))
					strDLLPath = lpszFullPath;

				AfxFreeTaskMem(lpszFullPath);
				
				if (bPlatform)
					return TRUE;
			}				
			if (srpBDM->FindFileOnPath(PLATFORM_CURRENT, DIRLIST_LIB,
				szDLLTemp, &lpszFullPath) == S_OK)
			{
				BOOL bPlatform = FALSE;
				fFound = TRUE;

				if (bPlatform = (GetExePlatform(lpszFullPath) == nPlatformID))
					strDLLPath = lpszFullPath;

				AfxFreeTaskMem(lpszFullPath);
				
				if (bPlatform)
					return TRUE;
			}
		}
	}
	else
	{
		//*** o Look in the SYSTEM32 dir
		//*** o Look in the WINDOWS dir
		//*** o Look in directories listed in PATH environment variable

		// By using SearchPath, we may accidentally pick something up in
		// msvc's directory (slim chance), but it's safer code
		if (SearchPath(NULL, szDLLTemp, NULL, sizeof(szTmp), szTmp, &sz))
		{
			fFound = TRUE;
			if (GetExePlatform(szTmp) == nPlatformID)
			{
				strDLLPath = szTmp;
				return TRUE;
			}
		}	
	}

	// If the file doesn't exist in that directory, try the
	// working directory of the debuggee.
	CString	strWorkingDir;
	BOOL fGotDir = FALSE;

	// For local projects, use the debuggee's working directory.
	if (!(pDebugCurr && pDebugCurr->MtrcRemoteTL()))
	{
		gpIBldSys->GetWorkingDirectory(ACTIVE_BUILDER, strWorkingDir);

		if (!strWorkingDir.IsEmpty())
			fGotDir = dir.CreateFromString(strWorkingDir);
	}

	// If the project is remote, or if the project is local but
	// the above GetWorkingDirectory() returned a NULL string,
	// the use the directory of the project.
	if (!fGotDir)
    {
        LPCSTR pszPath;

        gpIBldSys->GetBuilderFile(ACTIVE_BUILDER, &pszPath);
        fGotDir = dir.CreateFromPath(pszPath);
    }

	if (fGotDir)
		fGotDir = path.CreateFromDirAndFilename(dir,	szDLLTemp);

	strDLLPath = path;

	// Make sure we've got a full path
	path.Create(strDLLPath);
	strDLLPath = path;

	if (!fGotDir || !FileExist(strDLLPath))
	{
		if (!fQuiet)
        {
			if (fFound)
				ErrorBox(ERR_File_Wrong_Platform, szDLL);
			else
				ErrorBox(ERR_File_Not_Found, szDLL);
		}
		return FALSE;
	}

	if (GetExePlatform(strDLLPath) != nPlatformID)
	{
		if (!fQuiet)
			ErrorBox(ERR_File_Wrong_Platform, strDLLPath);

		return FALSE;
	}

	return TRUE;
}

/****************************************************************************

	FUNCTION:       LoadAdditionalDLLs

	PURPOSE:        Load symbol info for the Additional DLLs specified
					in Project.Settings (Debug page)

	INPUT:			pszDLLs = list of DLLs
					fStarting = TRUE if this is being called at the beginning
						of a debugging session; FALSE if it's being called
						in the middle of a debugging session (e.g. via the
						Options.Debug dialog).
					pichBegin = pointer to ich: if we return FALSE, then
						this will be set to the index within pszDLLs of the
						DLL name that caused us to fail.
					pichEnd = pointer to ich: goes with pichBegin, and will
						be set to the index of the character immediately past
						the end of the DLL name that caused us to fail.

****************************************************************************/
BOOL PASCAL LoadAdditionalDLLs(CStringList *pslDLLs, BOOL fStarting, BOOL fQuiet, POSITION *piPos)
{
    CPath pathDLLName;
	int ProgType;
	CStringList sl;

	// FUTURE - For JIT, load symbols when PN_?? is received (possibly PN_WORKSPACE_INIT)

	if (piPos)
		*piPos = NULL;

	// never try and load symbols on start for SQL inproc
	if (fStarting && FIsSqlInproc())						
		return TRUE;

	if (fStarting)
	{
		// [CAVIAR #6956 01/06/1993 v-natjm
		if (gpIBldSys->IsActiveBuilderValid() == S_OK)
			gpIBldSys->GetTargetAttributes(ACTIVE_BUILDER, &ProgType);
		else
		{
			ASSERT(theApp.m_jit.GetActive());
			ProgType = ImageExe | TargetIsDebugable;
		}

		// First load symbols for the project dll (if it is one).
		// If pProject is NULL, we're doing JIT debugging and this
		// isn't relevant.
		if (ProgType & ImageDLL && gpIBldSys && gpIBldSys->IsActiveBuilderValid() == S_OK)
		{
			LPTSTR pszExePath;

			gpIBldSys->GetTargetFileName(ACTIVE_BUILDER, &pszExePath);

			if (pszExePath)
			{
				SHE she;

				she = (SHE)SHAddDll((LPSTR)pszExePath, TRUE);
				// undone: check she

				delete [] pszExePath;
			}
		}
	}

	// If pslDLLs was passed in as NULL, get it from the currently set options
	if (pslDLLs == NULL)
	{
		// If starting a debugging session, we only want DLLs marked as preload.
		BuildLocalDLLsStringList(sl, fStarting);
		pslDLLs = &sl;
	}

	// Try to load the DLLs
	POSITION pos = pslDLLs->GetHeadPosition();

	while (pos)
	{
		CString str = pslDLLs->GetNext(pos);

		if (piPos)
			*piPos = pos;

		// For Java, we just want to pass the name of the class to the SH.
		// We'll let the VM handle the errors if the class doesn't exist, etc.
		if (pDebugCurr && pDebugCurr->IsJava()) {
			SHE she = (SHE)SHAddDll((LSZ)(LPCSTR)str, TRUE);
			if (she != sheNone && !fQuiet) {
				ErrorBox(ERR_File_Not_Found, str);
			}
		}
		else {

			CString strDLLPath;

			if ( !FFindDLL(str, strDLLPath, fQuiet) )
				return FALSE;

			// We call save_libname or whatever its equivalent is
			// here with pathDLLName
			if (!CheckEXEForDebug(strDLLPath, TRUE, fQuiet))
				return FALSE;

			SHE she ;     // error returned by the symbol handler
			she = (SHE)SHAddDll((LSZ)(LPCSTR)strDLLPath, TRUE);
			if ( she == sheNone )
				CVMessage ( FMESSAGEMSG, MODLOADED, CMDWINDOW, (LSZ)(LPCSTR)strDLLPath );

			// undone: check she
		}
	}

	if (piPos)
		*piPos = NULL;

	return TRUE;
}


/****************************************************************************

	FUNCTION:   GrabFocusFromDebuggee

	PURPOSE:    Bring the debugger to the top.

	WARNING:	This is not to be done lightly!  WADG says an app should
				never take focus itself; it should let the user do it.
				Well, the debuggee/debugger relationship is a special case;
				but still we shouldn't use this function recklessly.

****************************************************************************/

VOID
GrabFocusFromDebuggee(
	)
{
	// NB: the seemingly redundant call to BringWindowToTop is necessary for
	// some strange Win95 things.
	
	DkLockWorker (TRUE);
	SetForegroundWindow (GetShellWindow ());
	BringWindowToTop (GetShellWindow ());
	DkLockWorker (FALSE);
	EnsureShellNotMinimized ();

#if 0

	// Old version which checks if we are not on top before bringing us on top
	ULONG	pidForeground;
	HWND	hWndForeground;
	ULONG	tid;
	BOOL	fDoSet = TRUE;

	hWndForeground = GetForegroundWindow ();

	if (hWndForeground) {
		//
		// NB: Chicago RIPs if you pass (HWND)0 to GetWindowThreadProcessId

		tid = GetWindowThreadProcessId (hWndForeground, &pidForeground);

		if (tid != 0 && pidForeground != GetCurrentProcessId ()) {
			fDoSet = TRUE;
		} else {
			fDoSet = FALSE;
		}
	}

	//
	// Make sure SetForegroundWindow doesn't steal the focus from an
	// active dock worker.
	
	DkLockWorker (TRUE);

	if (fDoSet) {
		SetForegroundWindow (GetShellWindow ());
	}

	// Make sure the worker has been unlocked.

	DkLockWorker (FALSE);
	EnsureShellNotMinimized ();
#endif
}

_inline
BOOL
IsDebuggerView(
	CView*	pView
	)
{
	if (pView)
	{
		return
			(pView == pViewCpu    ) ||
			(pView == pViewMemory ) ||
			(pView == pViewCalls  ) ||
			(pView == pViewDisassy) ||
			(pView == g_pWatchView) ||
			(pView == g_pVarsView );
	}
	else
		return FALSE;
}

			
/****************************************************************************

	FUNCTION:   UpdateDebuggerState

	PURPOSE:    According to the passed flags asks the various debug
				windows (Watch, Locals, etc) to update their displays.
				Also take care of handling system state when debuggee
				dies.

****************************************************************************/
void PASCAL UpdateDebuggerState(WORD UpdateFlags)
{
	BOOL 	fAlive ;
	BOOL 	fCxt = FALSE ;
	HSF 	hsf ;
	ADDR 	addr = {0} ;
	PCXF	pcxf = (PCXF)NULL;
	UINT	wMsg;


	fAlive = DebuggeeAlive() ;

	if (fAlive)
		EEInvalidateCache();

	if (UpdateFlags & UPDATE_DEBUGGEE)
	{
		EraseCallerLine();
// Do test for lpprcCurr->hpid cause OD4 clears it for dbcDeleteProc
		if (!fAlive && lpprcCurr /*&& lpprcCurr->hpid*/) {
			TerminateDebuggee() ;
		}
	}

	// If we've just returned from the debuggee, AND the caller hasn't
	// explicitly said not to change the focus, then make sure we have focus
	//
	// If debuggee has died, don't grab focus
	if ((UpdateFlags & (UPDATE_DEBUGGEE|UPDATE_NOSETFOCUS)) == UPDATE_DEBUGGEE
		&& fAlive)
	{
		GrabFocusFromDebuggee();
	}

	// Update the tid in the status window
//	StatusTid(TidFromHthd(hthdCurr));

	BOOL bStateChanged = FALSE;
	if (fAlive && UpdateFlags == UPDATE_ALLSTATES &&
		!IS_STATE_DEBUG(DkGetDockState()))
	{
		DkSetDockState(STATE_DEBUG); // set debug toolbars/palettes
		bStateChanged = TRUE;
	}

	// Set any breakpoint report if necessary....
	if (*DebugMessage)
    {
		if ( (DebugMessage[0] == '\1') && (DebugMessage[1] == '\2') &&
			 (DebugMessage[2] == '\3') && (DebugMessage[3] == '\4') )
        {
			// A location bp - put an indication of this in the status bar
			gpISrc->StatusText(SYS_Location_BP, STATUS_INFOTEXT, FALSE);
		}
		else
        {
			CString	str;
			DebuggerMsgBox(Information, MsgText(str, SYS_My_String, DebugMessage));
		}
		*DebugMessage = '\0';
	}

	if (UpdateFlags & UPDATE_DEBUGGEE)
    {
		if (fAlive) {
			if (IsCrashDump ()) {
				SetModeName (TBR_Mode_CrashDump);
			} else {
				SetModeName (TBR_Mode_Break);
			}
		} else {
			SetModeName ( (UINT)0);
		}
	}

	if (UpdateFlags & UPDATE_ENC)
    {
		// Updating after the completion of an ENC operation.
		// In most cases this is equivalent to restarting or updating
		//  o If equivalent to updating do nothing special here
		//  o If equivalent to restarting, send WU_CLEARDEBUG
		//  o Otherwise send WM_UPDATEENC (a new message specifically for ENC)

		if ((UpdateFlags & UPDATE_WATCH) && g_pWatchView)
			g_pWatchView->SendMessage( WU_CLEARDEBUG, wPCXF, (DWORD)pcxf );

		if ((UpdateFlags & UPDATE_DISASSY) && pViewDisassy)
			pViewDisassy->SendMessage( WU_UPDATEENC, wPCXF, (DWORD)pcxf );

		// make sure cxfIp gets updated
		set_addrs();

		// Reload CallStack
		CLFreeWalkbackStack (TRUE);
		CLGetWalkbackStack (hpidCurr, htidCurr, (UINT) NULL);
	}

	if ( fAlive )
    {
		wMsg = WU_UPDATEDEBUG;
		if (UpdateFlags & UPDATE_DEBUGGEE)
			pcxf = &cxfIp;
	}
	else
		wMsg = WU_CLEARDEBUG;

	if ((UpdateFlags & UPDATE_CPU) && pViewCpu)
		pViewCpu->SendMessage( wMsg, wPCXF, (DWORD)pcxf );

	if ((UpdateFlags & UPDATE_WATCH) && g_pWatchView)
		g_pWatchView->SendMessage( wMsg, wPCXF, (DWORD)pcxf );

	if ((UpdateFlags & UPDATE_MEMORY) && pViewMemory)
		pViewMemory->SendMessage( wMsg, wPCXF, (DWORD)pcxf );

	if ((UpdateFlags & UPDATE_CALLS) && pViewCalls)
		pViewCalls->SendMessage( wMsg, wPCXF, (DWORD)pcxf );

	if ((UpdateFlags & UPDATE_DISASSY) && pViewDisassy)
		pViewDisassy->SendMessage( wMsg, wPCXF, (DWORD)pcxf );

	if (!fAlive)
    {
		CLFreeWalkbackStack( TRUE );

		// I don't think this ClearAllDocStatus is necessary now that
		// we're calling EraseTraceLine above... [mikemo]
		// ClearAllDocStatus(CURRENT_LINE) ;

		// Clear the current highlight, if any
		EraseCallerLine();
		EraseTraceLine();

		// This doesn't do much other than reset a state variable
		// necessary for restart
		freeze_view();

		// Notify the symbolhandler that we are done debugging and
		// all open files should be closed and that we are NOT
		// unloading the dll itself

		if ( !(pDebugCurr && pDebugCurr->IsQuickRestart ()) ) {
			SHUnloadSymbolHandler( TRUE );

			if (!ENCIsRebuilding()) {
				// Now that the pdb has been closed
				// try to relink images affected by Edit & Continue

				// We relink for ENC in asynchronous mode, except when restarting
				bool fAsync = lpprcCurr && 
					!(edsKilling == lpprcCurr->GetDebuggeeStateEx () &&
					(krRestart & lpprcCurr->GetKillReason()));
				ENCRelink(fAsync);
				// Reset state for Edit & Continue
				ENCReset();
			}
		}

		return;
	}

	if( UpdateFlags & UPDATE_SOURCE )
    {
		char 		SrcName[_MAX_PATH];
		ILINE 		SrcLine;
		UOFFSET 	SrcDelta;
		CDocument*	pDoc = NULL;
		BOOL 		GotNext;
		int			StepMode = SRCSTEPPING;

		if (lpprcCurr)
		{
			STEP_MODE	DefaultStepMode;
			
			StepMode = lpprcCurr->GetStepMode (&DefaultStepMode);

			//
			// This is a little bit of a rude hack.  The first time we do an
			// UpdateDebuggerState we change the proc's step mode from
			// FORCE_SOURCE to FROM_WINDOW -- allowing it to figure out it's
			// stepping mode from the topmost window.  This would be better
			// placed somewhere else, like maybe when we get the entry point.
			//
			   
			if (DefaultStepMode == STEP_MODE_FORCE_SOURCE)
				lpprcCurr->SetStepMode (STEP_MODE_FROM_WINDOW);
		}

			
		// Clear the current highlight, if any
		EraseTraceLine();

		if (GetCurrentSource(SrcName, sizeof(SrcName), &SrcLine, &SrcDelta,
							 &hsf))
        {
			UINT 		docType = DOC_WIN;
			CView* 		pCurView = (CView*) CWnd::FromHandle(theApp.GetActiveView());
			CView* 		pTextView = (CView*) gpISrc->GetCurTextView ();

			// We are only interested in retaining focus in the old
			// window, if it is not a debugger window.
			// If we open a source file for the first time when we are starting
			// debugging, we always want to give it the focus.

			if (pCurView && pCurView != pTextView && IsDebuggerView(pCurView))
			{
				CPartFrame::LockFrame (bStateChanged);

				// If the we are ASMSTEPPING and the DAM window is MDI, we
				// want to prevent a source window from popping up on top
				// of the DAM window to retain the stepping mode.

				if (StepMode == ASMSTEPPING && pViewDisassy && 
					DkWGetDock (MAKEDOCKID(PACKAGE_DEBUG, IDDW_VCPP_DISASSY_WIN)) == dpMDI
					)
				{
					CPartFrame::LockFrame(pViewDisassy->GetParentFrame()->GetSafeHwnd());
				}
			}

			TraceInfo.CurTraceLine = SrcLine ;
			TraceInfo.CurTraceDelta = SrcDelta ;
			TraceInfo.pDoc = NULL ;

			GotNext = SearchFileFromModule (SrcName, &pDoc, hsf, &fCxt,
											docType);

			if (GotNext == SF_NOERROR)
            { 
				CView*	pActiveView;
				
				// Highlight new current line
				
				RestoreTraceLine (pDoc);

				// If another view is on top lets get this one

				pActiveView = gpISrc->FindTextDocActiveView (pDoc);
				ASSERT (pActiveView);

				// Only change the active view if we've got a context. If
				// there isnt one, then we are just repainting due to an
				// update of a local, watch, or memory change.
				// Additionally, if we are in ASMSTEP mode, we do NOT want
				// to activate a source view
				
				if (StepMode != ASMSTEPPING && pcxf)
				{
					//
					// If another view is opened go to this one instead.
					//
					
                	theApp.SetActiveView (pActiveView->GetSafeHwnd());
				}

				//
				// If the current view is one of the debugger's window
				// (except the disassembly window) We want to set the focus
				// back to that window afterwards.
				//
				
				if (IsDebuggerView (pCurView) && pCurView != pViewDisassy)
				{
					CPartFrame::LockFrame (FALSE);
					theApp.SetActiveView (pCurView->GetSafeHwnd());
				}
			}

			CPartFrame::LockFrame (FALSE);
		}
	
		//
		// Unable to open source file, open the disassembly window
		//
		
		if (pDoc == NULL) 
        {
			if (!pViewDisassy)
			{
				OpenDebugWindow (DISASSY_WIN);

				if (pViewDisassy)
				{
					theApp.SetActiveView (pViewDisassy->GetSafeHwnd ());
					pViewDisassy->SendMessage (wMsg, wPCXF, (DWORD)pcxf);
				}
			}
			else
			{
				if (UpdateFlags & UPDATE_DEBUGGEE)
				{
                	theApp.SetActiveView (pViewDisassy->GetSafeHwnd());
				}
			}
		}
	}

	// ENC case: send WU_CLEARDEBUG
	if((UpdateFlags & UPDATE_LOCALS) && (UpdateFlags & UPDATE_ENC) && g_pVarsView)
		g_pVarsView->SendMessage( WU_CLEARDEBUG, wPCXF, (DWORD)pcxf );

	// It is important that this happens after we have had a chance
	// to open the source file for this module. This is because
	// the Auto watch window looks for an open document to do its
	// parsing.
	if((UpdateFlags & UPDATE_LOCALS) && g_pVarsView)
		g_pVarsView->SendMessage( wMsg, wPCXF, (DWORD)pcxf );
}

/****************************************************************************

	FUNCTION:       DebuggerMsgBox

	PURPOSE:        Displays a debugger message box.  The only reason we
					need our own message box function instead of using the
					standard one is that unlike most message boxes,
					debugger message boxes ARE ALLOWED TO GRAB THE FOCUS.
					When the debuggee hits a breakpoint or something, the
					debugger should become the active process.

	WARNING:		USE THIS FUNCTION WITH DISCRETION!  In general, it is
					against WADG to do this, but this is a special case.
					Use this function ONLY for message boxes that occur
					due to the debuggee's becoming blocked (like breakpoint
					hit, exception occurred, and so on).

****************************************************************************/

int DebuggerMsgBox(
	MsgBoxTypes	MsgBoxType,
	LPCTSTR		pszMessage,
	UINT		nButtonIDs,		/* = DEFAULT_BUTTONS	*/
	UINT		nHelpContext)	/* = DEFAULT_HELP		*/
{
	GrabFocusFromDebuggee();
	return MsgBox(MsgBoxType, pszMessage, nButtonIDs, nHelpContext);
}

/* Another version of DebuggerMsgBox.  SEE WARNING ABOVE! */

int DebuggerMsgBox(
	MsgBoxTypes	MsgBoxType,
	int			nMessageID,
	UINT		nButtonIDs,		/* = DEFAULT_BUTTONS	*/
	UINT		nHelpContext)	/* = DEFAULT_HELP		*/
{
	CString strMsg;

	strMsg.LoadString(nMessageID);
	return DebuggerMsgBox(MsgBoxType, strMsg, nButtonIDs, nHelpContext);
}

BOOL GetSourceFrompADDR(
	LPADDR pAddr,
	LPSTR SrcName,
	WORD SrcLen,
	LONG *pSrcLine,
	UOFFSET *pDelta,
	HSF *lphsf
	)
{
	ADDR		addr;
	SHOFF		dbLn;
	UCHAR       *lpchName;
	CXT			cxtt;
	USHORT		TmpLine;

	memset ( &cxtt, 0, sizeof(cxtt) );
	AuxAddr(pAddr);
	addr = *pAddr;

	if (!SLLineFromAddr(&addr, &TmpLine, NULL, &dbLn))
		return FALSE;
	*pSrcLine = TmpLine;

	if (pDelta)
		*pDelta = (UOFFSET) dbLn;

	if (SHSetCxtMod(&addr, &cxtt) == NULL)
		return FALSE;

	AuxAddr(&addr);

	if ((*lphsf = SLHsfFromPcxt(&cxtt)) == (HSF) NULL)
		return FALSE;

	lpchName = (PUCHAR) SLNameFromHsf(*lphsf);
	_fmemcpy(SrcName, lpchName+1, *lpchName);
	SrcName[*lpchName] = '\0';

	return(TRUE);
}

/****************************************************************************

	FUNCTION:   AsyncBPCommitBP()

	PURPOSE:    Shells calls to BPCommitBP, setting the added BP
				into the debuggee if he is currently alive.

	NOTE:       This routine takes no account of the fact that
				multiple exes are supported.  The current value
				in childpid is used (in DHGet/PutDebugeeBytes)
				despite the fact we might be writing in a different
				exe.  Windebug doesn't use the pid for the
				read/write word commands so we're ok.  In other
				environments we might have to be more sensible
				and pass the pid to the read/write routines.
				(We could get the pid from the selector address
				where we are reading/writing.)

****************************************************************************/
HBPI PASCAL AsyncBPCommitBP(LPPBP ppbp)
{
	HBPI hbpiThis;

    // We never want to stop on MESSAGE BP
	hbpiThis = BPCommitBP(ppbp);

	return hbpiThis;
}

/****************************************************************************

	FUNCTION:   AsyncBPDelete()

	PURPOSE:    Shells calls to BPDelete, restoring debuggee op-code
				if the BP is currently set.  (Delete while child is
				running.)

****************************************************************************/
void PASCAL AsyncBPDelete(HBPI hbpi)
{
	LPBPI lpbpi;

	lpbpi = (LPBPI)LLLpvFromHlle(hbpi);
	// Don't try to write the opcode if debuggee is dead...
	if (!DebuggeeAlive())
		lpbpi->bpf.f.fActive = FALSE ;

#if 0
	// this is never done since DebuggeeInCallBack is always TRUE
	if (lpbpi->bpf.f.fBPLoaded) {
		if (DebuggeeAlive() && !DebuggeeInCallBack()) {
			// Restore saved opcode
			DHPutDebuggeeBytes(lpbpi->CodeAddr, 1, &(lpbpi->OpCode));
			// For cleanlinesses sake
			lpbpi->bpf.f.fBPLoaded = FALSE;
		}
	}
#endif
	UnlockHlle(hbpi);
	BPDelete(hbpi);
}

void SwitchToMixedMode()
{
	ADDR		addr;
	BOOL		fGotAddr = FALSE;
	BOOL		fSwitchToSource = FALSE;
	CView       *pViewCur = GetCurView();

	// See if the user wants to "Unassemble at xxx"
	if ( IsFindComboActive() )
		fGotAddr = FAddrFromRibbonEdit( &addr );
	else if ( pViewCur != NULL )
    {
    	// Try to get an address from the current view.
		if ( gpISrc->ViewBelongsToSrc(pViewCur) )
        {
			char        szCurLine[ 20 ];

			// Make a current line BP node
			*szCurLine = BP_LINELEADER;
			_itoa(gpISrc->GetCurrentLine(pViewCur)+1, szCurLine+1, 10 );

			// Parse the address (must succeed)
			fGotAddr = FAddrFromSz( szCurLine, &addr );
		}
		else if ( pViewCur == pViewCalls || pViewCur == pViewDisassy )
        {
			GCAINFO	gcai;

			if ( (DWORD)gcafAddress == pViewCur->SendMessage(WU_GETCURSORADDR,0,(DWORD)&gcai) )
            {
				addr = gcai.addr;
				fGotAddr = TRUE;

				// Disassembly window is a special case: instead of
				// switching focus TO the disassembly window, this
				// will switch focus FROM the disassembly window, to
				// a source window for the current cursor location.
				fSwitchToSource = TRUE;
			}
		}
		else if ( !pViewDisassy || pViewCur != pViewDisassy )
        {
    		// Other debug window, just reset to the CS:IP (just don't
    		// reset the disassy window if already there
			fGotAddr = TRUE;
			addr = *SHpADDRFrompCXT( SHpCXTFrompCXF( &cxfIp ) );
		}
	}
	else
    {
    	// Other window, just reset to the CS:IP (just don't
    	// reset the disassy window if already there
		fGotAddr = TRUE;
		addr = *SHpADDRFrompCXT( SHpCXTFrompCXF( &cxfIp ) );
	}

	if ( fGotAddr )
    {
		if ( fSwitchToSource )
        {
			if ( MoveEditorToAddr( &addr ) == NULL ) {
				if (ENCReplacesAddr(&addr)) {
					gpISrc->StatusText(IDS_Enc_SourceChanged,STATUS_ERRORTEXT,TRUE);
				}
				MessageBeep( 0 );
			}
		}
		else
        {
			ADDR	addrFixed = addr;

			// Can't go to virtual addresses, e.g. if the user entered
			// "foo" where foo is some function in a DLL which is not
			// yet resident, but for which we have already loaded
			// symbols
			if (!SYFixupAddr(&addrFixed))
				MessageBeep( 0 );
			else
            {
				CXF		cxf = {0};

				*SHpADDRFrompCXT( SHpCXTFrompCXF( &cxf ) ) = addr;

				// This will make sure that the window is either
				// open or has the input focus
				if ( !pViewDisassy )
					OpenDebugWindow(DISASSY_WIN);
				else
					theApp.SetActiveView(pViewDisassy->GetSafeHwnd());

				// Window is open, set to new address
				if( pViewDisassy )
					pViewDisassy->SendMessage(WU_UPDATEDEBUG,wPCXF,(DWORD)&cxf);
			}
		}
	}
	else
		MessageBeep( 0 );
}

/***************************************************************************
	 Exact compare between two strings...
***************************************************************************/
SHFLAG PASCAL PHExactCmp (LPSSTR lpsstr, HSYM hsym, LSZ lpb, SHFLAG fCase)
{
	unsigned char cb;
	SHFLAG shf = TRUE;

	(VOID) hsym;
	if (lpb)
	{
		cb = *lpb;
		// if length is diff, they are not equal
		if (lpsstr && lpsstr->cb == cb)
		{
			if (fCase)
				shf = (SHFLAG) _fmemcmp (lpb + 1, lpsstr->lpName, cb);
			else
				shf = (SHFLAG) _ftcsnicmp (lpb + 1, (const char *) lpsstr->lpName, cb);
		}
	}
	return shf;
}

/***************************************************************************
	 Get symbolic information on a procedure...
***************************************************************************/
BOOL get_a_procedure( PCXT pcxt, const char * szName )
{
	BOOL	fRet = FALSE;
	SSTR	sstr = {0} ;

	sstr.lpName = (LPB)szName ;
	sstr.cb = (unsigned char)_ftcslen( szName );

	if( pcxt && szName && *szName )
	{
		HEXE	hexe = (HEXE)NULL;

		_fmemset( pcxt, 0, sizeof( CXT ) );
		while( !fRet && ( hexe = SHGetNextExe( hexe ) ) )
		{
			HSYM	hsym ;
			CXT		cxt = {0};
			CXT		cxtOut = {0};

			// We used to call SHFindSymInExe here, but that has
			// the very undesirable effect of loading symbols for
			// every module in the EXE. Instead we just try to find the
			// Name in the globals.

			if ( (cxt.hMod = SHGetNextMod(hexe, cxt.hMod)) != 0 )
			{
                if ( hsym = SHFindNameInGlobal(0, &cxt, &sstr,
#if defined(OSDEBUG4)
                    FALSE, (PFNCMP)PHExactCmp, &cxtOut) )
#else
                    FALSE, (PFNCMP)PHExactCmp, FALSE, &cxtOut) )
#endif
				{
					ADDR	addr = {0} ;

					SHAddrFromHsym( &addr, hsym );
					emiAddr( addr ) = (HEMI)hexe ;
					ADDR_IS_LI( addr ) = TRUE;
					if( SHSetCxt( &addr, pcxt ) != NULL )
					{
						fRet = TRUE;
					}
				}
			}
		}
	}
	return fRet;
}

static const char *	rgszWinEntries[] = {
	"WinMain",
	"_WinMain@16",
	"WINMAIN"
};
#define cszWinEntries	(sizeof(rgszWinEntries)/sizeof(rgszWinEntries[0]))

static const char *	rgszEntries[] = {
	"MAIN",
	"_main",
	"main",
    "wmain",
    "_wmain",
	"ENTGQQ"
};
#define cszEntries	(sizeof(rgszEntries)/sizeof(rgszEntries[0]))

static const TCHAR* rgszJavaEntries[] = {
	"main(java.lang.String[])",
	"init()"
};
const int cszJavaEntries = sizeof(rgszJavaEntries)/sizeof(rgszJavaEntries[0]);

/***************************************************************************
	Get the entry point of our application
***************************************************************************/
int PASCAL get_initial_context(PCXT pcxt)
{
	int	iRet = FALSE;

	ASSERT( lpprcCurr );

	// If we've been here before, just return the cached value
	if ( lpprcCurr->fHaveInitialContext )
    {
		if ( iRet = lpprcCurr->iReturnInitialContext )
			*pcxt = lpprcCurr->cxtInitial;
	}
	else
    {
		int		isz;
		BOOL	fDoWinEntries = TRUE;
		UINT	uiPlatform = pDebugCurr->GetPlatform();

		// Java: the check for initial context is completely different on Java.
		// First, we'll get an HEXE for the class the user entered in
		// the debug EXE edit field
		// Next, we'll look for init (applets) or main (applications) in the HEXE
		if (pDebugCurr && pDebugCurr->IsJava()) {

			// get the project
			ASSERT(gpIBldSys != NULL);

			// get the class name
			// (this should never fail because we wouldn't be debugging if we didn't have a class name)
			CString strClassName;
			gpIBldSys->GetJavaClassName(ACTIVE_BUILDER, strClassName);

			// determine which entry point to look for based on whether the user
			// is debugging his/her project as an applet (init) or application (main)
			ULONG nDebugUsing;
			gpIBldSys->GetJavaDebugUsing(ACTIVE_BUILDER, &nDebugUsing);
			BOOL bApplet = (nDebugUsing == Java_DebugUsing_Browser);
			if (!bApplet)
			{
				ULONG nStandaloneDebug;
				gpIBldSys->GetJavaStandaloneDebug(ACTIVE_BUILDER, &nStandaloneDebug);
				bApplet = (nDebugUsing == Java_DebugUsing_Standalone &&
						nStandaloneDebug == Java_DebugStandalone_Applet);
			}

			// construct class name.method name
			CString strEntryPoint = strClassName + _T(".") + rgszJavaEntries[bApplet? 1 : 0];
			TRACE("Attempting to set initial breakpoint on %s.\n", (LPCTSTR)strEntryPoint);

			// get the addr of this method from the EE
			HTM htmEntryPoint;
			USHORT nIndex;
			if (EEParse((LPTSTR)(LPCTSTR)strEntryPoint, 10, TRUE, &htmEntryPoint, &nIndex) == EENOERROR) {

				CXT cxtEntryPoint = { 0 };
				if (EEBindTM(&htmEntryPoint, &cxtEntryPoint, FALSE, FALSE) == EENOERROR) {
				
					RTMI rtmiEntryPoint = { 0 };
					HTI htiEntryPoint;
					if (EEInfoFromTM(&htmEntryPoint, &rtmiEntryPoint, &htiEntryPoint) == EENOERROR) {
						PTI ptiEntryPoint = (PTI)BMLock(htiEntryPoint);

						ADDR addrEntryPoint = ptiEntryPoint->AI;
						if (SHSetCxt(&addrEntryPoint, pcxt) != NULL) {
							iRet = TRUE;
						}

						EEFreeTI(&htiEntryPoint);
					}
				}

				EEFreeTM(&htmEntryPoint);
			}
		}
		else {
			// On REAL WIN32 systems (MIPS, x86, Alpha) if WinMainCRTStartup
			// is NOT found, skip the searches for the WinMain entry points
			if (uiPlatform == win32x86 || uiPlatform == win32mips || uiPlatform == win32alpha)
			{
				SSTR	sstr = {0} ;

				if ( uiPlatform == win32mips || uiPlatform == win32alpha)
				{
   	     			// MIPS and Alpha don't decorate C names with leading '_'
  	    			sstr.lpName = (LPB)"WinMainCRTStartup";
				}
				else
					sstr.lpName = (LPB)"_WinMainCRTStartup";

				sstr.cb = (unsigned char)_tcslen( (TCHAR *)sstr.lpName );

				if (!PHFindNameInPublics((HSYM)NULL, SHGetNextExe( (HEXE)NULL ), &sstr, FALSE, (PFNCMP)PHExactCmp))
					fDoWinEntries = FALSE;
			}

			if ( fDoWinEntries )
			{
				for( isz = 0; isz < cszWinEntries; ++isz )
				{
					if ( get_a_procedure( pcxt, rgszWinEntries[ isz ] ) )
					{
						iRet = TRUE;
						break;
					}
				}
			}

			if ( !iRet ) {
				for( isz = 0; isz < cszEntries; ++isz )
				{
					if ( get_a_procedure( pcxt, rgszEntries[ isz ] ) )
					{
						iRet = TRUE;
						break;
					}
				}
			}
		}

		// Only save the initial context if we found one
		if ( iRet )
			lpprcCurr->cxtInitial = *pcxt;

		// No matter what, setup the cached information
		lpprcCurr->fHaveInitialContext = TRUE;
		lpprcCurr->iReturnInitialContext = iRet;
	}
	return iRet;
}

// if you cast the result of this to a char* or similar, remember that the
// pointer will only be valid for the life of the resulting CString.
// Do NOT simply cast the result of this to a char* and expect it to be valid

CString GetDebugPathName(CDocument* pDoc)
{
	ASSERT(pDoc != NULL);
	ISourceEdit * lpse=NULL;
	CString strDocName;
	ULONG cbLen=0;

	HRESULT hr = gpISrc->CreateSourceEditFromDoc(pDoc, &lpse, FALSE);
	if(SUCCEEDED(hr)){
		lpse->GetPseudoPathName(strDocName.GetBuffer(300),300,&cbLen);
		ASSERT(300 >= cbLen);
		lpse->Release();
		strDocName.ReleaseBuffer();
		}

	return strDocName;
}

BOOL LoadNonDiskDocument(LPCTSTR szName, CDocument **ppDoc)
{
	HRESULT hr = gpISrc->LoadNonDiskDocument(szName, ppDoc);
	return SUCCEEDED(hr);
}

//	FUNCTION: FIsSqlInproc
//	Returns TRUE if SQL inproc, FALSE for all other cases

BOOL FIsSqlInproc()
{
	struct ISqlExec *pISqlExec;

	if (SUCCEEDED(theApp.FindInterface( IID_ISqlExec, (LPVOID*)&pISqlExec )))
	{
		BOOL bInProc = pISqlExec->InProcSql(NULL);
		pISqlExec->Release();

		return bInProc;
	}
	return FALSE;
}

// Is the active Project debuggable in any way?
// Also can return different attributes of the project, pass NULL if not interested

BOOL FIsActiveProjectDebuggable( BOOL *pUnknownTarget, BOOL *pIsExternal, BOOL *pTryToBuild, uniq_platform *puID )
{

	if ( gpIBldSys && gpIBldSys->IsActiveBuilderValid() == S_OK )
	{
		int iAttrs;
        int iType;

		// is the current target unknown?
		if (pUnknownTarget)
		{
			gpIBldSys->GetTargetAttributes(ACTIVE_BUILDER, &iAttrs);
			*pUnknownTarget = (iAttrs & TargetUnknown);
		}

		if (pTryToBuild)
	        *pTryToBuild = (gpIBldSys->TargetIsCaller(ACTIVE_BUILDER) == S_OK);

		if (pIsExternal)
		{
			gpIBldSys->GetBuilderType(ACTIVE_BUILDER, &iType);
			*pIsExternal = (iType == ExeBuilder);
		}

		if (puID)
			gpIBldPlatforms->GetCurrentPlatform(ACTIVE_BUILDER, puID);

		// Got a buildable project -> ok!
		return TRUE;
	}
	
	uniq_platform uID = unknown_platform;

	if(GetActiveIDBGProj())
	{
		GetActiveIDBGProj()->SupportsDebugging( (UINT*)&uID );

		if (uID == unknown_platform)
			return FALSE;

		if (pUnknownTarget)
			*pUnknownTarget = FALSE;			// we are not unknown
		if (pIsExternal)
			*pIsExternal = TRUE;				// always external
		if (pTryToBuild)
			*pTryToBuild = FALSE;				// never try to build non-buildable projects
		if (puID)
			*puID = uID;
		return TRUE;
	}
	else
	{
		return FALSE;						// cannot debug
	}
}
