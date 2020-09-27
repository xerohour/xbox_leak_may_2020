/**** DBGUTIL.C - System Support Generic using osdebug                  ****
 *                                                                         *
 *                                                                         *
 *  Copyright <C> 1990, Microsoft Corp                                     *
 *                                                                         *
 *  Created: October 15, 1990 by David W. Gray                             *
 *                                                                         *
 *  Purpose:                                                               *
 *                                                                         *
 *  [0] - always include os2def.h -- dn                                    *
 *  [01] - ss dll support v-griffk 14-jan-1992                             *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

#include "stdafx.h"
#pragma hdrstop

#include "cvinc\cvwmem.h"
#include "dbgenc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

#define SECONDS 1000

EPI epiDef = epiTitle;

HLLE           LOADDS PASCAL LDSLLHlleGetLast ( HLLI );
void           LOADDS PASCAL LDSLLHlleAddToHeadOfLI ( HLLI, HLLE );
BOOL           LOADDS PASCAL LDSLLFRemoveHlleFromLl ( HLLI, HLLE );

void _HUGE_ * PASCAL LOADDS LDShalloc(long, size_t);
void PASCAL LOADDS LDShfree(void _HUGE_ * );

HEM  hemCurr  = hemNull;
HEM  hnmCurr  = hemNull;
HTL  htlCurr  = htlNull;

HMEM hprcCurr = hmemNull;
LPPRC lpprcCurr = NULL;
HMEM hthdCurr = hmemNull;
HLLI llprc    = hmemNull;
WORD wModelTemp;

int   fSource = FALSE;
LPDBF lpdbf = NULL;
// Default is 15ms, for quick watch we increase these timeouts.
DWORD dwFuncTimeOut = 15000 ;

void SYContinue ( HPRC );
void SYStop ( HPRC );
void SYPause ( HPRC );

VOID FAR PASCAL PDKill ( LPV );

enum _ACTIVESCREEN {    // [ds]
    CVSCREEN = 0,               // [ds]
    CHILDSCREEN,                // [ds]
    REDRAWCMDWIN                // [ds]
};                                              // [ds]

#define cmpHtid  0
#define cmpTid   1
#define cmpLpthd 2

#define cmpHpid  0
#define cmpPid   1
#define cmpTitle 2
#define cmpName  3

#define cbMaxTitle (8)

// [CAVIAR #6358 12/12/92 v-natjm]
// MSVC IDE has to trap every call to OSDEBUG
// when not debugging...
extern BOOL DebuggeeAlive(void) ;

// Specific api's for symbol handler file handling

UINT LOADDS PASCAL SYOpen ( LSZ lsz ) {

    HANDLE h = CreateFile( lsz, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL, NULL );

    errno = 0;      // clear errno to suppress bogus errors

    if (h==INVALID_HANDLE_VALUE)
        return (UINT)-1;
    else
        return (UINT)h;
}

VOID LOADDS PASCAL SYClose ( UINT hfile ) {

    // Verify, because caller must not have passed a bogus handle
    VERIFY(CloseHandle((HANDLE)hfile));
}

UINT LOADDS PASCAL SYReadFar ( UINT w, LPB lpb, UINT w2 ) {

    DWORD dwRead;

    if (ReadFile( (HANDLE)w, lpb, w2, &dwRead, NULL ))
        return dwRead;
    else
        return 0;
}

LONG LOADDS PASCAL SYSeek ( UINT hfile, LONG offset, UINT origin ) {

    DWORD dwWhere = SetFilePointer( (HANDLE)hfile, offset, NULL, origin );
    if (dwWhere==0xFFFFFFFF)
        return -1;
    else
        return dwWhere;
}

#if defined(OSDEBUG4)
LONG LOADDS PASCAL SYTell ( UINT hfile) {

    return SetFilePointer( (HANDLE)hfile, 0, NULL, FILE_CURRENT );
}


UINT PASCAL
SYFindExeFile(
    LSZ               lpFile,
    LSZ               lpFound,
    UINT              cchFound,
    LPVOID            lpv,
    PFNVALIDATEEXE    pfn
    )
{
    UINT hFile = -1;
    TCHAR szTmpDbgName[_MAX_PATH];
    
    // We don't currently handle searching for .exe/.dlls etc.
    // We should enhance this function to ask users/search along path
    // etc for these exes/dlls as well. 

    // Test for existence of the file. If there is 
    UINT hOrigFile = SYOpen(lpFile);
    if (hOrigFile == -1)
    {
    // File doesn't exist
    // We are expected to search for the .exe/.dll first and then
    // the .dbg file. 
    // This is not yet supported.
    return hFile;
    }

    SYClose(hOrigFile);

    TCHAR szDrive[_MAX_DRIVE];
    TCHAR szDir[_MAX_PATH];
    TCHAR szFName[_MAX_FNAME];
    TCHAR szOrigExt[_MAX_EXT];
    TCHAR szDbgExt[] = ".dbg";

    // First look for the .dbg file in the same directory as the 
    // original .exe/.dll

    _tsplitpath(lpFile, szDrive, szDir, szFName, szOrigExt);
    _tmakepath(szTmpDbgName, szDrive, szDir, szFName, szDbgExt);

    hFile = SYOpen(szTmpDbgName);

    // Search along the path, if we didn't find it in the same directory as the exe/dll.
    if (hFile == -1)
    {
    TCHAR szRelName[_MAX_PATH];
    TCHAR szSymbols[] = _T("symbols\\");
    TCHAR pathSep = __T('\\');  // Separates dir names within the PATH env variable
    LPTSTR lpszCurr;

    lpszCurr = szRelName;

        // create the symbols\ext\file.dbg part of the filename

        _tcscpy(lpszCurr, szSymbols);
        lpszCurr += _tcslen(szSymbols);
        
        _tcscpy(lpszCurr, (szOrigExt + 1));
        lpszCurr += _tcslen(szOrigExt + 1);
    
        *lpszCurr++ = pathSep;

        _tcscpy(lpszCurr, szFName);
        lpszCurr += _tcslen(szFName);

        _tcscpy(lpszCurr, szDbgExt);
    
    // We will look for the .dbg file along these environment variables in
        // order.
        TCHAR * rgszDbgPath[]       =
        {
            __T("_NT_ALT_SYMBOL_PATH"),
            __T("_NT_SYMBOL_PATH"),
            __T("SystemRoot"),
        };

        const int MAX_SEARCH_PATH_LEN = 512;
    TCHAR szSearchPath[MAX_SEARCH_PATH_LEN];

        for ( int i = 0; i < sizeof(rgszDbgPath)/sizeof(rgszDbgPath[0]); i++ )
        {
            DWORD cbStr;

            if ( (cbStr = GetEnvironmentVariable(rgszDbgPath[i], szSearchPath,
                    MAX_SEARCH_PATH_LEN)) != 0 )
            {
                if (cbStr > MAX_SEARCH_PATH_LEN)
                {
                    ASSERT(FALSE);  // Shouldn't happen in nature.
                    return -1 ;
                }

                if ( (cbStr = SearchPath(szSearchPath, szRelName, NULL,
                            _MAX_PATH, szTmpDbgName, NULL)) != 0)
                {
                    if ( cbStr > _MAX_PATH )
                    {
                        return -1;
                    }

                    hFile = SYOpen(szTmpDbgName);
                    
                    if ( hFile != -1 )
                    {
                        break;
                    }
                }
            }
        }
    }

    if (hFile != -1)
    {
    SHE she = pfn(hFile, lpv, NULL);

    // REVIEW: need to retry if we get sheBadSymbols etc.
    if (she == sheNone && _tcslen(szTmpDbgName) < cchFound )
    {
        _tcscpy(lpFound, szTmpDbgName);
        return hFile;
    }
	SYClose( hFile );
    }

    return (UINT)-1;
}

VOID PASCAL
DLoadedSymbols(
    SHE   /*she*/,
    HPID  /*hpid*/,
    LSZ   /*s*/
    )
{
#pragma message("OSDEBUG4: NYI - does this need to be implemented")
}

BOOL PASCAL SYGetDefaultShe( LSZ /* Name */, SHE * She)
{
#pragma message("OSDEBUG4: NYI")
    *She = sheNone;
    return TRUE;
}

#endif


#if defined(OSDEBUG4)
UINT PASCAL
SYProcessor (
    DWORD dwPid
    )
{
    UINT Processor;
    OSDGetDebugMetric((HPID) dwPid, NULL, mtrcProcessorType, (LPVOID) &Processor);
    return(Processor);
}
#else
UINT LOADDS PASCAL SYProcessor ( VOID ) {
    static MPT mpt = 0;
    HPID hpid = hpidCurr;

    OSDGetDebugMetric ( hpid, NULL, mtrcProcessorLevel, &mpt );

    return (UINT) mpt;
}

#endif

void UpdateIn386Mode(void) {
    static long l = 0;       // microprocessor type

#if !(defined (OSDEBUG4))
    l = (LONG) SYProcessor();
    fTargetIs386 = (uchar) ( l == mpt386 || l == mpt486 || l == mptPentium );
#else
    l = (LONG) SYProcessor((DWORD) hpidCurr);
    fTargetIs386 = (uchar) ( l == mptix86 );
#endif

    if (fTry386Mode) {
    fIn386Mode = fTargetIs386;
    }
}


/*
 *      DebuggingWin32s
 *              Goes against all the OSDebug tenets but we need this information to give
 *              a very Win32s specific message.
 */

BOOL DebuggingWin32s(  ) {

    if ( DebuggeeAlive( ) && pDebugCurr) {
        // HACK-O-RAMA -- we know that Win32s supports
        // exception but does not support asyncKill.
        
        // Win32s supported only remotely from the IDE.
        if ( pDebugCurr->MtrcRemoteTL( ) ) {

            if ( !(pDebugCurr->MtrcAsync( ) & asyncKill) &&
                  (pDebugCurr->MtrcHasThreads( )) &&
                  (pDebugCurr->MtrcFirstChanceExc())
                ) {
                
                return TRUE;
            }
        }
    }
    return FALSE;
}
                                
        
void SYSetContext ( HPRC hprc, HTHD hthd, BOOL fUpdateUI ) {
    BOOL fChanged = FALSE;

    if ( hprc != hprcCurr ) {
    fChanged = TRUE;

    // Take all of the relevant globals & store them hprcCurr

    if ( hprcCurr != (HPRC) NULL ) {
        UnlockHlle ( hprcCurr );
    }

    hprcCurr = hprc;

    UpdateIn386Mode ( );

    if ((lpprcCurr = (LPPRC)LLLpvFromHlle ( hprc )) != NULL ) {
        SHChangeProcess ( lpprcCurr->hpds );
    }

    if ( hthd != (HTHD) NULL ) {

        // Do we have to do similar state stuff for the thread???

        hthdCurr = hthd;
    }
    else {
        hthdCurr = lpprcCurr->hthdCurr;
    }
    }
    else if ( hthd != hthdCurr && hthd != (HTHD) NULL ) {

    fChanged = TRUE;
    hthdCurr = hthd;
    lpprcCurr->hthdCurr = hthd;
    }

    if ( hthdCurr == (HTHD) NULL ) {

    hthdCurr = GetNthThread ( 0 );
    lpprcCurr->hthdCurr = hthdCurr;
    }

    if ( fChanged ) {
        CLFreeWalkbackStack ( TRUE );
    }

    if ( fChanged && lpprcCurr->fSymLoaded ) {

    BOOL fFoundCxt;

    if ( hprcCurr != 0 && hthdCurr != 0 && lpprcCurr->stp != stpNotStarted ) {

        fEnvirGbl.fAll = FALSE;
        UpdateUserEnvir ( mUserPc | mAsmaddr );
    }
    else {
        memset ( &cxfIp, 0, sizeof ( CXF ) );

        fFoundCxt = get_initial_context ( SHpCXTFrompCXF ( &cxfIp ) );
    }

    if ( fUpdateUI && lpprcCurr->fLoaded ) {

        if ( lpprcCurr->stp != stpNotStarted ) {
        display_pc ( );
        }
        update_watch_window ( );
    }
    }
}

/*
 *  SET_ADDRS
 *
 *  Sets global addresses after run/trace or load
 */
void set_addrs (void)
{

    /* have the old function work as it did before, but update my flags */

    ADDR addr;

    _fmemset ( &addr, 0, sizeof (ADDR));

    SYGetAddr ( hpidCurr, htidCurr, adrPC, &addr );
    if ( SYCmpAddr (
        &addr,
        SHpADDRFrompCXT ( SHpCXTFrompCXF ( &cxfIp ) ) ) != 0 ) {

    fEnvirGbl.fAll = FALSE;
    UpdateUserEnvir ( mUserPc | mAsmaddr );
    }
    else
    fEnvirGbl.fAll |= mUserPc | mAsmaddr;
}


HPRC FindUnusedHprc ( char *szName, LPF lpfAmb ) {
    HPRC hprc = 0;
    BOOL fFound = FALSE;

    *lpfAmb = 0;

    do {
    LPPRC lpprc;

    hprc = LLHlleFindLpv ( llprc, hprc, szName, cmpName );

    if ( hprc != 0 ) {

        lpprc = (LPPRC)LLLpvFromHlle ( hprc );

        if ( !lpprc->fLoaded ) {
        fFound = TRUE;
        }
        else {
        *lpfAmb += 1;
        lpprc->fAmbName = TRUE;
        }

        UnlockHlle ( hprc );
    }
    else {
        break;
    }

    } while ( !fFound );

    return hprc;
}


BOOL
PASCAL
SYDisplayTLError(
    )
/*++

Routine Description:

    Display the last TL error if any.


Return Value:

    TRUE if a message was displayed.

--*/
{
    CString strErr;
    TCHAR*  lpsz = strErr.GetBuffer (TL_ERROR_BUFFER_LENGTH);
    XOSD    xosd;

    xosd = OSDGetLastTLError (htlCurr,
                              hpidCurr,
                              lpsz,
                              TL_ERROR_BUFFER_LENGTH
                              );
    strErr.ReleaseBuffer ();

    // If we have a valid return, and a None Null string.
    
    if ( xosd == xosdNone && lpsz[0] != '\0' )
    {
        MsgBox(Error, strErr);
        return TRUE;
    }
    return FALSE;
}


XOSD 
OSDAPI
SYUtilCreateHpid(
    LPFNSVC lpfnSvc,
    HEM hEM,
    HTL hTL,
    LPHPID lpHpid
) {

    if ( pDebugCurr && pDebugCurr->MtrcRemoteTL()) {
        // Update status bar to indicate we are trying
        // to connect.

        gpISrc->StatusText(DBG_Connecting,STATUS_INFOTEXT,FALSE);
    }

    XOSD xosd = xosdNone;

    xosd = OSDStartTL(hTL);

    if (xosd == xosdNone)
        xosd = OSDCreateHpid(lpfnSvc,hEM,hTL,lpHpid);

    // Display message if not succesful.

    if (xosd != xosdNone)
    {
        if (!SYDisplayTLError())
        {
            // Display the Generic message
            ErrorBox(ERR_Debugger_Init);
        }               
    }


    if ( pDebugCurr && pDebugCurr->MtrcRemoteTL())
    {
        // Reset status bar.
        gpISrc->StatusText(SYS_StatusClear, STATUS_INFOTEXT, FALSE);
    }

    return (xosd);
}

// PRC Constructor: _PRC::_PRC

BOOL PASCAL SYCreateHprc (
    char *szName,
    char *szArgs,
    HPRC hprcParent,
    HPID hpid
) {
    HPRC  hprc;
    LPPRC lpprc;
    char  rgchTitle [ _MAX_CVFNAME ];
    char FAR * lszTitle = rgchTitle;
    BOOL  fNew = FALSE;
    BOOL  fAmbName = FALSE;
    BPTS  bptsSupported;
    XOSD  xosd = xosdNone;

    int ich = 0;

    CVSplitPath ( szName, NULL, NULL, lszTitle, NULL );
    while (
    ich < cbMaxTitle &&
    rgchTitle [ ich ] &&
    rgchTitle [ ich ] != ' '
    ) {
    ich += _ftclen(rgchTitle + ich);
    }

    if ( hprcParent == (HPRC) NULL ) {

    if ( ( xosd = SYUtilCreateHpid (
        &CallBack,
        hemCurr,
        htlCurr,
        &hpid )) != xosdNone ) {

        return FALSE;
    }
    }

    hprc = FindUnusedHprc ( szName, &fAmbName );
    if ( hprc == (HPRC) NULL ) {
    fNew = TRUE;
    hprc = LLHlleCreate ( llprc );
    }
    lpprc = (LPPRC)LLLpvFromHlle ( hprc );

    if ( !fNew ) {
        HPDS    hpds = lpprc->hpds;

        if ( lpprc->llthd ) {
            LLChlleDestroyLl( lpprc->llthd );
        }
        if ( lpprc->szTitle ) {
            _ffree( lpprc->szTitle );
        }
        if ( lpprc->szArgs ) {
            _ffree( lpprc->szArgs );
        }
        _fmemset( lpprc, 0, sizeof( PRC ) );
        lpprc->hpds = hpds;
    }

    lpprc->fAmbName = fAmbName != 0;
    lpprc->hpid   = hpid;
    lpprc->pid    = 0;

    lpprc->fSymLoaded   = FALSE;
    lpprc->llthd        = LLHlliInit (
    sizeof ( THD ), llfAscending, NULL, (LPFNFCMPNODE)TDCompare
    );
    lpprc->stp          = stpNotStarted;
    lpprc->exp          = expStopped;
    lpprc->htidExecuting= (HTID) NULL;
    lpprc->hthdCurr     = (HTHD) NULL;
    lpprc->wEM          = CEXM_MDL_native;
    lpprc->fReallyNew   = TRUE;
    lpprc->fPcodePresent= FALSE;
    lpprc->SetKillReason (krNone);
    lpprc->SetBeyondEntryPoint (FALSE);
    lpprc->SetBPsBound (FALSE);
    lpprc->m_bInsideEvent = FALSE;
    lpprc->m_bGoDeferred = FALSE;
    lpprc->m_bStopDeferred = FALSE;
	lpprc->m_bEncGoPending = FALSE;
	lpprc->fSqlInproc = FIsSqlInproc();

    //
    // when the process first starts, we force source mode
        
    lpprc->SetStepMode (STEP_MODE_FORCE_SOURCE);

    OSDGetDebugMetric ( hpid, htidNull, mtrcThreads, &(lpprc->fHasThreads) );
    OSDGetDebugMetric ( hpid, htidNull, mtrcBreakPoints, &bptsSupported );
    lpprc->fHasMessages = (bptsSupported & ( bptsMessage | bptsMClass )) != 0;

    //
    //      When we start a new debuggee, tell the EE what the target machine
    //      type is.        NOTE: since the EE currently is keeping global state
    //      about what the target machine is, this will break for multiproces
    //      debugging.  It is a workaround for the EE.
    //
    
    MTRC    mtrc;
    if (OSDGetDebugMetric (hpid, NULL, mtrcProcessorType, &mtrc) == xosdNone)
    {
        EESetTarget (mtrc);
    }


    /////////////////////////////////////////////////////////////////////////
    //  WARNING: Disabling memory tracking.

    BOOL bEnable = AfxEnableMemoryTracking(FALSE);
    lpprc->szName  = _ftcsdup ( szName );
    if ( fAmbName ) {
        char FAR *lszT = rgchTitle + _ftcslen ( rgchTitle );
        
        if ( fAmbName < 10 ) {
            *lszT = (char) ('0' + fAmbName);
            *(lszT + 1 ) = '\0';
        }
        else {
            *lszT = (char) ('0' + fAmbName / 10);
            *(lszT + 1) = (char) ('0' + fAmbName % 10);
            *(lszT + 2) = '\0';
        }
    }
    
    lpprc->szTitle = _ftcsdup ( rgchTitle );
    
    if ( fNew ) {
        LLAddHlleToLl ( llprc, hprc );
    }
    
    if ( szArgs != NULL ) {
        lpprc->szArgs = _ftcsdup ( szArgs );
    }
    else {
        lpprc->szArgs = NULL;
    }
    AfxEnableMemoryTracking(bEnable);

    // Re-enabling memory tracking.
    /////////////////////////////////////////////////////////////////////////


    UnlockHlle ( hprc );

    if ( hprcCurr != (HPRC) NULL ) {
    UnlockHlle ( hprcCurr );
    }

    hprcCurr = hprc;
    lpprcCurr = (LPPRC)LLLpvFromHlle ( hprc );
    hthdCurr = (HTHD) NULL;

    UpdateIn386Mode ( );

    if ( fNew ) {
    lpprcCurr->hpds = SHCreateProcess ( );
    }
    else {
    SHChangeProcess ( lpprcCurr->hpds );
    }

    SHSetHpid ( hpid );

    return TRUE;
}


ULONG
GetStepMode(
    );
    

BOOL
SYLoadAllOmf(
	);

ULONG
_PRC::GetStepMode(
    STEP_MODE*      StepMode
    )
{
    if (StepMode)
        *StepMode = m_StepMode;

    if (m_StepMode == STEP_MODE_FORCE_SOURCE)
        return SRCSTEPPING;
    else if (m_StepMode == STEP_MODE_FORCE_ASM)
        return ASMSTEPPING;
    else
        return ::GetStepMode ();
}
    

BOOL WINAPI
SYInitProcessList(
	)
{
    llprc = LLHlliInit (sizeof (PRC),
						NULL,
						PDKill,
						(LPFNFCMPNODE)PDCompare
						);

    return (llprc != 0);
}

VOID
CVDisconnect(
	)
{
    CallBack ( (WORD)dbcRemoteQuit, NULL, NULL, 0, 0L);
}


BOOL WINAPI
SYLoadDebugInfo(
	)
{

	if (!lpprcCurr->fSymLoaded) {

		char *pchExe = lpprcCurr->szName;
		lpprcCurr->fSymLoaded = TRUE;


		if ( SHAddDll ( pchExe, FALSE ) != sheNone ) {
			return FALSE;
		} else {
			return SYLoadAllOmf ( );
		}
    } else {
		return TRUE;
	}
}

#if 0
SHE PASCAL
SheRegisterModule(
    HPID    hpid,
    HTID    htid,
    LSZ     lsz,
    BOOL    fRegEmi
    )
{
    HEXE    hexe = SHGethExeFromName( lsz );
    BOOL    bSql = FIsSqlInproc();
    SHE     she = bSql ? sheNoSymbols : SHLoadDll( lsz, TRUE );     // dont load symbols for SQL

    LPTSTR fix = 0;

    if (*(LPTSTR)lsz == '|') {
		fix = _ftcschr(lsz+1, _T('|'));
		lsz++;
		*fix = '\0';
    }

    if ( hexe && she == sheNone ) {
		LSZ     lszOldPath;
		CHAR    szOMFDrv[_MAX_DRIVE];
		CHAR    szOMFDir[_MAX_DIR];
		CHAR    szNewDrv[_MAX_DRIVE];
		CHAR    szNewDir[_MAX_DIR];

		lszOldPath = SHGetExeName( hexe );
		_splitpath ( lsz, szNewDrv, szNewDir, NULL, NULL );
		_splitpath ( lszOldPath, szOMFDrv, szOMFDir, NULL, NULL );

		if ( _ftcsicmp ( szNewDrv, szOMFDrv ) ||
			_ftcsicmp ( szNewDir, szOMFDir ) ) {

			CVMessage (
				WARNMSG,
				MODNOMATCH,
				CMDWINDOW,
				lsz
				);
		}
		
		fRegEmi = TRUE;
	} else if ( she != sheOutOfMemory && she != sheFileOpen ) {

		if ( she == sheNone ) {
			CVMessage ( FMESSAGEMSG, MODLOADED, CMDWINDOW, lsz );
		} else if ( (she == sheNoSymbols) && !bSql ) {
			CVMessage ( FMESSAGEMSG, NOSYMBOLS, CMDWINDOW, lsz );
		} else if (she==sheSymbolsConverted)
			CVMessage ( WARNMSG, SYMSCONVERTED, CMDWINDOW, lsz );
		else if (she==sheExportsConverted)
			CVMessage ( WARNMSG, EXPORTSCONVERTED, CMDWINDOW, lsz );

		if (fix) {
			*fix = '|';
			lsz--;
			fix = NULL;
		}

		if ( fRegEmi ) {
			hexe = SHGethExeFromName( lsz );
		}
	}

    if (fix) {
		*fix = '|';
		lsz--;
		fix = NULL;
    }

    if ( hexe && fRegEmi ) {

        if (!pDebugCurr || (pDebugCurr && !pDebugCurr->IsJava())) {

            ADDR tAddrEmi = {0};
            ADDR tAddr = {0};

            emiAddr ( tAddrEmi ) = (HEMI)hexe;
            if ( SHPublicNameToAddr(&tAddrEmi, &tAddr, "___pcd_start" ) ) {
                lpprcCurr->fPcodePresent = TRUE;
            }
        }

    if  ( lpprcCurr->fPcodePresent ) {
        OSDNativeOnly ( hpid, htid, !lpprcCurr->fPcodePresent );
    }

        if ( pDebugCurr->MtrcRemoteTL() ) {
            CString strRemote;

            if (*lsz == '|') {      // OSDEBUG4
                lsz++;
                fix = _ftcschr (lsz, _T('|'));
                *fix = '\000';
            }

            MapLocalFileToRemote(lsz, strRemote);

            VERIFY(OSDRegisterEmi(hpid, (HEMI)hexe, (LPTSTR)(LPCTSTR)strRemote) == xosdNone);

        }
        else
        {
			VERIFY(OSDRegisterEmi(hpid, (HEMI)hexe, lsz) == xosdNone);
        }
    }

    return she;
}
#endif



SHE WINAPI
SheRegisterModule(
    HPID    hpid,
    HTID    htid,
    LSZ		lsz
	)
/*++

Routine Description:

	Load the dll and symbols for the dll.  Do the proper local/remote mapping.
	Warn to debug output about status of the loaded dll.

Comments:

	The name that is passed down in lsz MUST be the name passed to
	OSDRegisterEmi ().

--*/

{
	CString	strLocal;
	CString	strLocalUndecorated;
	CString	strUndecorated;
	CString	strT;
	CHAR    szDriveOld [_MAX_DRIVE];
	CHAR    szDriveNew [_MAX_DRIVE];
	CHAR    szDirOld [_MAX_DIR];
	CHAR	szDirNew [_MAX_DIR];
	SHE		she = sheNone;
	XOSD	xosd = xosdNone;
    LPTSTR	fix = NULL;
	HEXE	hexe = NULL;
    ADDR	tAddrEmi = {0};
    ADDR	tAddr = {0};
	BOOL	fDoEncReloadImage = FALSE;


	
	if (*lsz == '|') {
		lsz++;
		fix = _ftcschr (lsz, _T('|'));
		*fix = '\000';
	}

	strUndecorated = lsz;

	if (fix) {
		lsz--;
		*fix = '|';
		fix = NULL;
	}
	
	emiAddr (tAddrEmi) = (HEMI) hexe;

	//
	// Do not be fooled: this check is for the benefit of SQL to see if we
	// should load the SQL NM or not.  In this context 'p-code' means sqlnm.
	//
	
    if (SHPublicNameToAddr (&tAddrEmi, &tAddr, "___pcd_start")) {
		lpprcCurr->fPcodePresent = TRUE;
    }

	if (FIsSqlInproc ()) {

		//
		// Don't bother calling SHLoadDll if SQL debugging (speed up startup),
		// and also force non-native so the SQL NM stays around. Doesn't
		// bother with the p-code symbol lookup stuff either.
		//
		
		strLocal = lsz;
		she = sheNoSymbols;
        lpprcCurr->fPcodePresent = TRUE;
		
	} else {

		ASSERT (pDebugCurr);

		//
		// If remote, map the remote dll to a local dll
		//
		

		if (pDebugCurr->MtrcRemoteTL ()) {

			if (FGetLocalDLLFromRemoteDLL (strLocal, strUndecorated)) {
				ASSERT (!strLocal.IsEmpty ());
				strLocalUndecorated = strLocal;
				ASSERT (strLocalUndecorated [0] != '|');
			} else {
				strLocalUndecorated = strUndecorated;
				strLocal = lsz;
			}

		} else {

			strLocalUndecorated = strUndecorated;
			strLocal = lsz;
		}

		hexe = SHGethExeFromName ((LPSTR) (LPCSTR) strLocal);
		she = SHLoadDll ((LPSTR) (LPCSTR) strLocal, TRUE);

		if (she == sheNone && hexe) {

			//
			// Symbols for this DLL were already loaded.  Make sure the
			// module that was already loaded is the same as the module
			// we now have loaded.
			//
			
			strT = SHGetExeName (hexe);
			ASSERT (strT [0] != '|');

			_splitpath (strT, szDriveNew, szDirNew, NULL, NULL);
			_splitpath (strLocalUndecorated, szDriveOld, szDirOld, NULL, NULL);

			if (_ftcsicmp (szDriveNew, szDriveOld) != 00 ||
				_ftcsicmp (szDirNew, szDirOld) != 00) {

				CVMessage (WARNMSG, MODNOMATCH, CMDWINDOW, strLocalUndecorated);
			}
			else if (ENCIsEditedImage(hexe)) {
				fDoEncReloadImage = TRUE;
			}

		} else {

			switch (she) {

				case sheNone:
					CVMessage (FMESSAGEMSG, MODLOADED, CMDWINDOW, strLocalUndecorated);
					break;

				case sheNoSymbols:
					CVMessage (FMESSAGEMSG, NOSYMBOLS, CMDWINDOW, strLocalUndecorated);
					break;

				case sheSymbolsConverted:			
					CVMessage (WARNMSG, SYMSCONVERTED, CMDWINDOW, strLocalUndecorated);
					break;

				case sheExportsConverted:
					CVMessage (WARNMSG, EXPORTSCONVERTED, CMDWINDOW, strLocalUndecorated);

			}
		}

	}
	
	hexe = SHGethExeFromName ((LPSTR) (LPCSTR) strLocal);


	//
	// This is for SQL -- not real pcode
	//
	
	if (lpprcCurr->fPcodePresent) {
        OSDNativeOnly (hpid, htid, !lpprcCurr->fPcodePresent);
    }

	//
	// The string that is passed down to OSDRegisterModule must be the
	// same as the string that was passed by the EM to callback on
	// dbcModLoad.  Decoration and all.
	//

	if (hexe) {
		xosd = OSDRegisterEmi (hpid, (HEMI) hexe, lsz);
		ASSERT (xosd == xosdNone);

		if (fDoEncReloadImage && !ENCReloadImage(hexe)) {
			// The dll was edited by ENC during this debugging session
			// but the ENC engine was not able to reload the dll
			CString strMsg;
			MsgText(strMsg, IDS_Enc_CantReloadImage, strT);
			OutputWindowQueueHit(OwinGetDebugCommandID(), strMsg, FALSE, TRUE);
		}
 	}

	return she;
}

		

void
SYInstallVirtual(
    HPID    hpid,
    LSZ     lszLibName,
    BOOL    fLoad
    )
{
    HEXE hexe = SHGethExeFromName ( lszLibName );

	//
    // If loading always register the module
	//

	if ( fLoad ) {
		SheRegisterModule (hpid, htidNull, lszLibName);
    }

	//
    // If there's an hexe, then update the breakpoint list
	//
	
    if (hexe) {
		BPInstallVirtual ( hpid, hexe, fLoad );
    }

	//
    // If unloading, unregister the dll and unload
	//

	if ( hexe && !fLoad ) {

		XOSD    xosd;
    
        //      
        // BUGBUG: this used to assert when xosd != xosdNone.  We cannot do
        // this because of a bug when loading multiple DLLs with the same
        // name (but different actual images).  We will need to revisit this
        // problem in the future.  For a repro case, have two versions of
        // some Dll. In the extra's dialog, load one of the versions of the
        // Dll and make sure that the program loads the other one.  Note
        // that the UnRegisterEmi call gets confused below and fails.
        //
           
        xosd = OSDUnRegisterEmi (hpid, (HEMI) hexe);
		SHUnloadDll (hexe);
    }
}

VOID
SYUnFixupOmf(
	)
{
    HEXE    hexeCurr = NULL;
    HEXE    hexeNext = NULL;
    HPID    hpid = hpidCurr;

    hexeCurr = SHGetNextExe (NULL);

    while (hexeCurr) {

		hexeNext = SHGetNextExe (hexeCurr);

		if (SHIsDllLoaded (hexeCurr)) {

            LPTSTR pszExeName = SHGetExeName (hexeCurr);

			if (pszExeName != NULL) {
                SYInstallVirtual (hpid, pszExeName, FALSE);
            } else {
				SHUnloadDll (hexeCurr);
            }
		}
		hexeCurr = hexeNext;
    }
}


/***************************************************************************
 *                                                                         *
 * Memory and List manager load ds thunks                                  *
 *                                                                         *
 ***************************************************************************/

//#define TRACK_DEBUGGER_LEAKS

void FAR * PASCAL LOADDS LDSFmalloc( size_t size ) {
#if defined (TRACK_DEBUGGER_LEAKS)
    return BMAlloc (size);
#else
    return cvw3_fmalloc (size);
#endif
}

void FAR * PASCAL LOADDS LDSFrealloc ( void FAR * buffer, size_t size ) {
#if defined (TRACK_DEBUGGER_LEAKS)
    return BMRealloc (buffer, size);
#else
    return cvw3_frealloc (buffer, size);
#endif
}

void PASCAL LOADDS LDSFfree ( void FAR * buffer ) {
#if defined (TRACK_DEBUGGER_LEAKS)
    BMFree (buffer);
#else
    cvw3_ffree (buffer);
#endif
}

// This is a list of all HLLIs which were allocated by the
// debugger DLLs.  It's kept as a CMapPtrToWord in order to
// allow for fast location of particular elements; however,
// the only part we're actually using is the Ptr (which is
// actually the HLLI); we're not interested in the Word to
// which it maps.
CMapPtrToWord ListOfDbgLists;

void FreeDbgLinkedLists(void)
{
    POSITION pos;

    pos = ListOfDbgLists.GetStartPosition();
    while (pos)
    {
        void* key;
        WORD value;
        HLLI hlli;
        LLI* plli;

        // query key and value, and also advance pos to next position
        ListOfDbgLists.GetNextAssoc(pos, key, value);

        // the key is actually an HLLI
        hlli = (HLLI) key;

        // set the HLLI's lpfnKill and lpfnCmp function pointers
        // to NULL.  We do this because all we're trying to do
        // here is free up resources which were used by the
        // linked list; we must not call the kill or compare
        // functions, because the debuggee code may not be
        // expecting that to happen right now, and also because
        // those code addresses may in fact be invalid!  (They
        // may be in DLLs which have since been released.)

        plli = LockHlli(hlli);
        plli->lpfnKill = NULL;
        plli->lpfnCmp = NULL;
        UnlockHlli(hlli);

        // Free up the list's resources
        LLChlleDestroyLl(hlli);
    }

    ListOfDbgLists.RemoveAll();
}

HLLI
OSDAPI
LDSLLHlliInit (
#ifdef OSDEBUG4
    DWORD cb,
#else
    UINT cb,
#endif
    LLF llf,
    LPFNKILLNODE lpfnKillNode,
    LPFNFCMPNODE lpfnCmpNode
) {
    HLLI hlli = LLHlliInit ( cb, llf, lpfnKillNode, lpfnCmpNode );

    if (hlli) {
        // Store the HLLI in our list of all HLLIs which were
        // created by the debugging DLLs, so that we can delete
        // it ourselves later if necessary.

#ifdef _DEBUG
        WORD value;
        // this HLLI shouldn't already be in the list
        ASSERT(!ListOfDbgLists.Lookup( (void*) hlli, value ));
#endif

        ListOfDbgLists [ (void*) hlli ] = 0;
    }

    return hlli;
}

HLLE
OSDAPI
LDSLLHlleCreate (
    HLLI hlli
    )
{
    return LLHlleCreate ( hlli );
}

void
OSDAPI
LDSLLAddHlleToLl (
    HLLI hlli,
    HLLE hlle
    )
{
    LLAddHlleToLl ( hlli, hlle );
}

void
OSDAPI
LDSLLInsertHlleInLl (
    HLLI hlli,
    HLLE hlle,
    DWORD dword
    )
{

    LLInsertHlleInLl ( hlli, hlle, dword );
}

BOOL
OSDAPI
LDSLLFDeleteHlleFromLl (
    HLLI hlli,
    HLLE hlle
    )
{

    return LLFDeleteHlleFromLl ( hlli, hlle );
}

HLLE
OSDAPI
LDSLLHlleFindNext (
    HLLI hlli,
    HLLE hlle
    )
{
    return LLHlleFindNextFast ( hlli, hlle );
}

DWORD
OSDAPI
LDSLLChlleDestroyLl (
    HLLI hlli
    )
{

    // Since the creator has now destroyed this list, we no longer
    // need to keep track of it for the purpose of destroying it
    // ourselves later.
    ListOfDbgLists.RemoveKey( (void*) hlli );

    return LLChlleDestroyLl ( hlli );
}

HLLE
OSDAPI
LDSLLHlleFindLpv ( HLLI hlli, HLLE hlle, LPV lpv, DWORD dword )
{

    return LLHlleFindLpv ( hlli, hlle, lpv, dword );
}

DWORD
OSDAPI
LDSLLChlleInLl ( HLLI hlli ) 
{
    return LLChlleInLl ( hlli );
}

LPV
OSDAPI
LDSLLLpvFromHlle ( HLLE hlle ) 
{
    return LLLpvFromHlle ( hlle );
}

HLLE
OSDAPI
LDSLLHlleGetLast ( HLLI hlli ) 
{
    return LLHlleGetLast ( hlli );
}

void
OSDAPI
LDSLLHlleAddToHeadOfLI ( HLLI hlli, HLLE hlle ) 
{

    LLHlleAddToHeadOfLI ( hlli, hlle );
}

BOOL
OSDAPI
LDSLLFRemoveHlleFromLl ( HLLI hlli, HLLE hlle ) 
{
    return LLFRemoveHlleFromLl ( hlli, hlle );
}

/***************************************************************************
 *                                                                         *
 *  Callback handling routines                                             *
 *                                                                         *
 ***************************************************************************/

BOOL
HasSource(
    ADDR    addr
    )
{
    WORD  wLine;
    SHOFF dbLine;
    CXT cxt;

    if ( !ADDR_IS_LI( addr ) ) {
        SYUnFixupAddr ( &addr );
    }

    if ( SHSetCxtMod ( &addr, &cxt ) ) {
        emiAddr ( addr ) = emiAddr ( cxt.addr );
#if 0
		// REVIEW: this is the CORRECT usage of SLLineFromAddr
        return SLLineFromAddr ( &addr, &wLine, NULL, &dbLine );
#else
		// this is bad
        SLLineFromAddr ( &addr, &wLine, NULL, &dbLine );
		return (wLine != 0);
#endif
    } else {
        return FALSE;
    }
}

void
AddThread(
    HPID    hpid,
    HTID    htid
    )
{
    HPRC          hprc  = HprcFromHpid ( hpid );
    HTHD          hthd  = 0;
    LPTHD         lpthd = NULL;
    unsigned long tid   = 0;
    BOOL          fHasThreads;


    // OD4 and !OD4 both process the mtrcTidValue message

    //
    // HACK, REVIEW: using OSDGetDebugMetric here instead of
    // OSDGetThreadStatus because of a synchronization problem in the
    // TCP/IP TL.  During a callback, it is not possible to have another
    // thread call into the TL.  Therefore, we cannot use OSDGetThreadStatus
    // which ends up doing a request (tlfRequest).  We need to look at this
    // problem more closely

#if 1                   // !(defined (OSDEBUG4))
    OSDGetDebugMetric ( hpid, htid, mtrcTidValue, &tid );
#else
    {
       TST tst;
       VERIFY(OSDGetThreadStatus( hpid, htid, &tst) == xosdNone);
       tid = tst.dwThreadID;
    }
#endif

    hthd = HthdFromHtid ( hprc, htid );

    if ( hthd == 0 ) {

    hthd =  LLHlleCreate ( LlthdFromHprc ( hprc ) );
    lpthd = (LPTHD)LLLpvFromHlle ( hthd );

    lpthd->htid = htid;
    lpthd->tid  = tid;

    LLInsertHlleInLl ( LlthdFromHprc ( hprc ), hthd, cmpLpthd );
    }
    else {
    lpthd = (LPTHD)LLLpvFromHlle ( hthd );
    }

    lpthd->tdf = tdfUnknown;

    UnlockHlle ( hthd );

    VERIFY(OSDGetDebugMetric( hpid, htid, mtrcThreads, &fHasThreads ) == xosdNone);
    if ( htidCurr == hmemNull || !fHasThreads ) {
    SethtidCurr ( htid );
    }
}


void
ProgLoad (
    HPRC hprcParent,
    HPID hpid,
    HTID htid,
    LSZ  lszName,
    BOOL fReallyNew
    )
{
    char rgchName [ _MAX_CVPATH ];
    HPRC hprc;
    HTHD hthd;
    PID  pid;

    _ftcscpy ( rgchName, lszName );

    SYCreateHprc ( rgchName, NULL, hprcParent, hpid );

    lpprcCurr->fLoaded = TRUE;
    lpprcCurr->fReallyNew = fReallyNew;
    lpprcCurr->hprcParent = hprcParent;

    AddThread ( hpid, htid );
    SethtidCurr ( htid );
    lpprcCurr->hthdCurr = hthdCurr;

    if ( fReallyNew ) {
    lpprcCurr->stp = stpNotStarted;
    }
    else {
    lpprcCurr->stp = stpStopped;
    }

#if !(defined (OSDEBUG4))
    OSDGetDebugMetric ( hpidCurr, htidNull, mtrcPidValue, &pid );
#else
    {
        PST pst;
        if (OSDGetProcessStatus(hpidCurr, &pst) == xosdNone) {
            pid = pst.dwProcessID;
        }
        else ASSERT(FALSE);
    }
#endif
    lpprcCurr->pid = pid;

    set_addrs ( );
    SYGetAddr ( hpidCurr, htidCurr, adrData, &dump_start_address );
    SYGetAddr ( hpidCurr, htidCurr, adrPC, &asmaddr );

    // If this really is a new process, we'll mention it to the user.  (It
    // might not really be new if, for example, a CVW user hit ctrl-alt-
    // sysreq while some process other than the debuggee was running, and
    // this is the first time we've seen this process.)
    if (fReallyNew) {

    hprc = HprcFromHpid ( hpid );
    hthd = HthdFromHtid ( hprc, htid );
    fEnvirGbl.fAll = 0;
    UpdateUserEnvir ( mCtxtIp | mAsmaddr );

    CVMessage (
        INFOMSG,
        (MSGID) "New Process: %Fs",
        CMDWINDOW,
        lszName
    );

    SYSetContext ( hprcParent, 0, FALSE );

    SYContinue ( hprcParent );
    SYSetContext ( hprc, 0, TRUE );
    }
}

/***************************************************************************
 *                                                                         *
 *                           DEBUGGEE ROUTINES                             *
 *                                                                         *
 ***************************************************************************/

/*** DHGetDebuggeeBytes
*
*   Purpose: To read bytes from the debuggee.
*
*   Input:
*   paddr - A debuggee's address to read from.
*   cb    - The number of bytes to read.
*
*   Output:
*   bBuff - A place to read the byte to.
*
*   Returns:
*   The number of bytes actually read.
*
*   Exceptions:
*
*   Notes:
*
*************************************************************************/
size_t LOADDS PASCAL
DHGetDebuggeeBytes( ADDR addr, size_t cb, void FAR *bBuff ) {
    int terrno = errno;
    DWORD cbT = 0;
    XOSD xosd;

    if ( ADDR_IS_LI ( addr ) ) {
    SYFixupAddr ( &addr );
    }

    // check (native) addresses are 32-bit, flat, protected mode
    ASSERT( addr.mode.fSql || addr.mode.fJava || (ADDR_IS_FLAT( addr ) && ADDR_IS_OFF32( addr) && !ADDR_IS_REAL( addr ) ) );

    xosd = OSDReadMemory(hpidCurr, htidCurr, &addr, (LPBYTE)bBuff, cb, &cbT);
    errno = terrno;
    return (size_t) (xosd != xosdNone ? 0 : cbT);
}

/*** DHPutDebuggeeBytes
*
*   Purpose: To +write bytes to the debuggee.
*
*   Input:
*   paddr - A debuggee's address to write to.
*   cb    - The number of bytes to write.
*
*   Output:
*   bBuff - The bytes to write.
*
*   Returns:
*   The number of bytes actually written.
*
*   Exceptions:
*
*   Notes:
*
*************************************************************************/
size_t LOADDS PASCAL
DHPutDebuggeeBytes( ADDR addr, size_t cb, void FAR *bBuff ) {
    int terrno = errno;

    if ( lpprcCurr->stp == stpStopped || lpprcCurr->stp == stpNotStarted ) {

    if ( ADDR_IS_LI ( addr ) ) {
        SYFixupAddr ( &addr );
    }
    {
            // check (native) addresses are 32-bit, flat, protected mode
            ASSERT( addr.mode.fSql || addr.mode.fJava || (ADDR_IS_FLAT( addr ) && ADDR_IS_OFF32( addr) && !ADDR_IS_REAL( addr ) ) );

            DWORD cbWritten;
            if (OSDWriteMemory( hpidCurr, htidCurr, &addr, (LPBYTE)bBuff, cb, &cbWritten) != xosdNone) {
                cb = 0;
            }
            else {
                cb = cbWritten;
            }
        }
    CLFreeWalkbackStack( FALSE );
    }
    else {
    cb = 0;
    }

    errno = terrno;
    return cb;
}



VOID CALLBACK
FuncExecTimerProc(
    HWND,
    UINT,
    UINT,
    DWORD
    )
/*++

Routine Description:

    The function exec has not returned for a long time.  We will stop the
    execution of the function in case if has gone into an infinte loop.

--*/
{
    HTHD    hthd = NULL;
    HPRC    hprc = hprcCurr;
    LPTHD   lpthd;
    LPPRC   lpprc;
    XOSD    xosd = xosdNone;
    
    hthd = HthdFromHtid (hprc, htidCurr);
    lpthd = (LPTHD) LLLpvFromHlle (hthd);
    lpprc = (LPPRC) LLLpvFromHlle (hprc);

    // the process could've died since this we set this timer
    
    if (lpthd == NULL || lpthd->tdf == tdfDead ||
        lpprc == NULL || lpprc->stp == stpDead || lpprc->stp == stpRunning)
    {
        UnlockHlle (hthd);
        UnlockHlle (hprc);
        return;
    }

    if (lpprc->fFuncExec)
    {
        KillTimer (NULL, lpprc->funcExecTimerID);
        lpprc->funcExecTimerID = 0;
    }

    xosd = OSDAsyncStop (hpidCurr, TRUE);

    UnlockHlle (hthd);
    UnlockHlle (hprc);
}       

SHFLAG PASCAL
DHSetupExecute(
    LPHIND  lphdep
    )
/*++

Routine Description:

    This function is called from the expression evaluator to setup things
    up for a function evaluation call.  This request is passed on to the
    execution model.

Arguments:

    lphdep      - Supples a pointer to where a handle is returned

Return Value:

    FALSE on failure.

--*/

{
    HTHD    hthd;
    HPRC    hprc;
    LPTHD   lpthd;
    LPPRC   lpprc;
    XOSD    xosd;
    

    hprc = hprcCurr;
    hthd = HthdFromHtid (hprc, htidCurr);
    lpthd = (LPTHD) LLLpvFromHlle (hthd);
    lpprc = (LPPRC) LLLpvFromHlle (hprc);

    if (lpthd == NULL || lpthd->tdf == tdfDead ||
        lpprc == NULL || lpprc->stp == stpDead || lpprc->stp == stpRunning)
    {
        UnlockHlle (hthd);
        UnlockHlle (hprc);
        return FALSE;
    }

    xosd = OSDSetupExecute (lpprc->hpid, lpthd->htid, lphdep);
    
    UnlockHlle (hthd);
    UnlockHlle (hprc);

    return (xosd == xosdNone);
}



SHFLAG PASCAL
DHStartExecute(
    HIND     hdep,
    LPADDR   lpaddr,
    BOOL     fIgnoreEvents,
    SHCALL   shcall
    )
/*++

Routine Description:

    This function is called when the expression evaluator starts
    the debugging running a function evaluation.  It must be preceded
    by a call to DHSetupExecute.

Arguments:

    hdep        - Supplies the handle to the Execute Function object
    lpaddr      - Supplies the address to start execution at
    fIgnoreEvents - Supplies
    fFarRet     - Supplies TRUE if a far return should be executed

Return Value:

    FALSE if something fails

--*/

{
    XOSD            xosd = xosdNone;
    HTHD            hthd = NULL;
    HPRC            hprc = NULL;
    LPTHD           lpthd;
    LPPRC           lpprc;

    hprc = hprcCurr;
    hthd = HthdFromHtid (hprc, htidCurr);
    lpthd = (LPTHD) LLLpvFromHlle (hthd);
    lpprc = (LPPRC) LLLpvFromHlle (hprc);

    if (lpthd == NULL || lpthd->tdf == tdfDead ||
        lpprc == NULL || lpprc->stp == stpDead || lpprc->stp == stpRunning)
    {
        UnlockHlle (hthd);
        UnlockHlle (hprc);
        return FALSE;
    }

    PrimeDbgMessagePump();

    xosd = OSDStartExecute (lpprc->hpid,
                            hdep,
                            lpaddr,
                            fIgnoreEvents,
                            shcall == SHFar);

    if (xosd != xosdNone) {
        UnlockHlle (hthd);
        UnlockHlle (hprc);
        return FALSE;
    }

    SYFlipScreen (lpprc->hpid, lpthd->htid, CHILDSCREEN, TRUE);

    ASSERT (lpprc->funcExecTimerID == 0);
    lpprc->fFuncExec = TRUE;
    lpprc->funcExecTimerID = SetTimer (NULL, 0, 15 * SECONDS, FuncExecTimerProc);

    DbgMessagePump (FALSE);  // don't pump UI messages as EE not re-entrant

    lpprc->fFuncExec = FALSE;

    if (lpprc->funcExecTimerID != 0)
    {
        KillTimer (NULL, lpprc->funcExecTimerID);
        lpprc->funcExecTimerID = 0;
    }

    UnlockHlle (hthd);
    UnlockHlle (hprc);
    
    return (xosd == xosdNone);
}



SHFLAG PASCAL
DHCleanUpExecute(
    HIND   hdep
    )

/*++

Routine Description:

    This function is used to clean up after doing a function evalution.

Arguments:

    hdep - Supplies the handle to the function evaluation object

Return Value:

    TRUE if something fails

--*/

{
    LPPRC   lpprc;
    XOSD    xosd;

    lpprc = (LPPRC) LLLpvFromHlle (hprcCurr);

    if (lpprc == NULL) {
        return FALSE;
    }
    
    xosd = OSDCleanUpExecute (lpprc->hpid, hdep);

    UnlockHlle (hprcCurr);
    return (xosd == xosdNone);
}


/*** DHGetReg
*
* Purpose: To get a register from the debuggee.
*
*   Input:
*   pReg  The register structure. The member hReg must contain
*       the handle to the register to get.
*
*   pCxt  The context packet to use.
*
*   Output:
*   pReg  The Register field is filled in.
*
*   Returns:
*   pReg if successful, NULL if the call could not be completed.
*
*   Exceptions:
*
*   Notes:
*   Currently the 8087 registers are not implemented. In the future
*   only ST0 will be implemented.
*
*************************************************************************/
PSHREG LOADDS PASCAL
DHGetReg(
    PSHREG  pReg,
    PCXT    pCxt
    )
{
    XOSD    xosd;

    // NOTE: we can get GetReg calls when we are not debugging.
    
    if (!DebuggeeAlive ()) {
        return NULL ;
    }
        
    xosd = OSDReadRegister (hpidCurr,
                            htidCurr,
                            pReg->hReg,
                            &pReg->Byte1);

    if (xosd != xosdNone) {
        return NULL;
    }

    
    return pReg;
}

/*** DHSetReg
*
*   Purpose: To set a register in the debuggee.
*
*   Input:
*   pReg  The register structure. The member hReg must contain
*       the handle to the register to set.
*
*   pCxt  The context packet to use.
*
*   Output:
*
*   Returns:
*   pReg if successful, NULL if the call could not be completed.
*
*   Exceptions:
*
*   Notes:
*   Currently the 8087 registers are not implemented. In the future
*   only ST0 will be implemented.
*
*************************************************************************/
PSHREG LOADDS PASCAL
DHSetReg(
    PSHREG  pReg,
    PCXT    pCxt
    )
{
    XOSD    xosd;
    
    // IDE evaluates expression even if debuggee not started

    if (!DebuggeeAlive ()) {
        return NULL;
    }

	if (!lpprcCurr->IsBeyondEntryPoint())
		return NULL;

    CLFreeWalkbackStack (FALSE);
    
    xosd = OSDWriteRegister (hpidCurr,
                             htidCurr,
                             pReg->hReg,
                             (void*)&pReg->Byte1);

    if (xosd != xosdNone) {
        return NULL;
    }

    set_addrs();
    return pReg;
}


int   reg87Size( void );
void *reg87Save( void );
void  reg87Restore( void FAR * );
void  reg87Reset( void );


/*** DHSaveReg
*
* Purpose: To save the debuggee resgister set
*
* Input:
*   pCxt  The context packet to use.
*
* Output:  register set saved in memory block pReg
*
*   Returns:
*   pReg if successful, NULL if the call could not be completed.
*
*   Exceptions:
*
*   Notes:
*   Currently the 8087 registers are not implemented. In the future
*   only ST0 will be implemented.
*
*************************************************************************/
HMEM LOADDS PASCAL
DHSaveReg(
    PCXT    pCxt
    )
{
    HMEM    hReg = (HMEM)NULL;
    XOSD    xosd;

    if ( DebuggeeAlive() ) {
        xosd = OSDSaveRegs (hpidCurr, htidCurr, (LPHIND)&hReg);
    }

    if (xosd != xosdNone) {
        return NULL;
    }

    return hReg;
}

/*** DHRestoreReg
*
* Purpose: To restore the debuggee register set
*
*   Input:
*   pReg  the memory handle of the register set returned by DHSaveReg.
*       the memory handle will be freed at the end of the call
*
*   pCxt  The context packet to use.
*
* Output:
*
*   Returns:
*   none
*
*   Exceptions:
*
*   Notes:
*   Currently the 8087 registers are not implemented. In the future
*   only ST0 will be implemented.
*
*************************************************************************/
VOID LOADDS PASCAL
DHRestoreReg(
    HMEM    hReg,
    PCXT    pCxt
    )
{
    XOSD    xosd;
    
    if( DebuggeeAlive()) {
        CLFreeWalkbackStack (FALSE);
    xosd = OSDRestoreRegs (hpidCurr, htidCurr, (HIND)hReg);

        if (xosd != xosdNone) {
            return ;
        }

        
        if (wModelTemp != lpprcCurr->wEM) {

        // Return to native if we left it
        SYNativeOnly ( FALSE );
        }
    }
}

void
SYSetFrame(
    FRAME*  pFrame
    )
{
    XOSD    xosd;
    ADDR    addrBase = {0};
    ADDR    addrData = {0};

    xosd = SYGetAddr  (hpidCurr, htidCurr, adrBase, &addrBase);

    if (xosd != xosdNone) {
        return ;
    }
    
    SYFixupAddr (&addrBase);

    xosd = SYGetAddr (hpidCurr, htidCurr, adrData, &addrData);

    if (xosd != xosdNone) {
        return ;
    }
    
    SYFixupAddr ( &addrData );

    SetFrameBPOff ( *pFrame, GetAddrOff ( addrBase ) );
    FrameFlat ( cxfIp.Frame ) = ADDR_IS_FLAT ( addrBase );
    FrameOff32 ( cxfIp.Frame ) = ADDR_IS_OFF32 ( addrBase );

    SetFrameBPSeg ( *pFrame, GetAddrSeg ( addrBase ) );
    pFrame->SS  = (SEG16) GetAddrSeg ( addrBase );
    pFrame->DS  = (SEG16) GetAddrSeg ( addrData );
    pFrame->PID = hpidCurr;
    pFrame->TID = htidCurr;
}

int
SYCmpAddr(
    LPADDR  lpaddr1,
    LPADDR  lpaddr2
    )
{
    SEGMENT seg1 = GetAddrSeg ( *lpaddr1 );
    SEGMENT seg2 = GetAddrSeg ( *lpaddr2 );

#ifdef _DEBUG
    ADDR addrNil = {0};
#endif

    ASSERT ( ( ADDR_IS_LI ( *lpaddr1 ) == ADDR_IS_LI ( *lpaddr2 ) ) ||
             ( _fmemcmp(lpaddr1, &addrNil, sizeof(ADDR)) == 0 ) ||
             ( _fmemcmp(lpaddr2, &addrNil, sizeof(ADDR)) == 0 ) );

    if ( ( seg1 != seg2 ) ||
     ( emiAddr ( *lpaddr1 ) != emiAddr ( *lpaddr2 ) ) ||
     ( GetAddrOff ( *lpaddr1 ) < GetAddrOff ( *lpaddr2 ) ) )
    {
        return -1;
    }
    else if (GetAddrOff ( *lpaddr1 ) > GetAddrOff ( *lpaddr2 ))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void
SYSetAddr(
    HPID    hpid,
    HTID    htid,
    ADR     adr,
    LPADDR  lpaddr
    )
{

    if ( ADDR_IS_LI ( *lpaddr ) ) {
        SYFixupAddr ( lpaddr );
        OSDSetAddr ( hpid, htid, adr, lpaddr );
        SYUnFixupAddr ( lpaddr );
    }
    else {
        OSDSetAddr ( hpid, htid, adr, lpaddr );
    }
}

void
SYUnassemble(
    HPID    hpid,
    HTID    htid,
    LPSDI   lpsdi
    )
{
    XOSD    xosd;
    
    if ( ADDR_IS_LI ( lpsdi->addr ) ) {
        SYFixupAddr ( &lpsdi->addr );
    }

    xosd = OSDUnassemble ( hpid, htid, lpsdi);

    if (xosd != xosdNone) {
        return ;
    }

    if ( !ADDR_IS_LI ( lpsdi->addr ) ) {
        SYUnFixupAddr ( &lpsdi->addr );
    }
}

void SYFormatSdi ( LPSDI lpsdi, char * pch, BOOL fIgnoreEA ) {
    DOP dop = lpsdi->dop;
    char *pchBeg = pch, *pchPrev;
#if defined(_M_MRX000)
    const int CCHRAW = 9;
#else
    const int CCHRAW = 21;
#endif

    pch += sprintf (
    pch,
    "%Fs %-*.20Fs%-9Fs %Fs %Fs %Fs",
    ( dop & dopAddr ) && lpsdi->ichAddr != -1 ?
        &lpsdi->lpch [ lpsdi->ichAddr ] :
        (LPCH) " ",
        ( dop & dopRaw ) && lpsdi->ichBytes != -1 ? CCHRAW : 2,
    ( dop & dopRaw ) && lpsdi->ichBytes != -1 ?
        &lpsdi->lpch [ lpsdi->ichBytes ] :
        (LPCH) "",
     lpsdi->ichOpcode != -1 ?
        &lpsdi->lpch [ lpsdi->ichOpcode ] :
        (LPCH)"???",
        lpsdi->ichOperands != -1 ?
        &lpsdi->lpch [ lpsdi->ichOperands ] :
        (LPCH) "",
    lpsdi->ichComment != -1 ?
        &lpsdi->lpch [ lpsdi->ichComment ] :
        (LPCH) "",
    !fIgnoreEA && lpsdi->ichEA0 != -1 ?
        &lpsdi->lpch [ lpsdi->ichEA0 ] :
        (LPCH) ""
    );

    // strip pesky trailing blanks
    while ( *(pchPrev = _ftcsdec(pchBeg, pch)) == ' ')
    pch = pchPrev;
    *pch = '\0';
}


XOSD
SYAssemble(
    LPADDR  lpaddr
    )
{
    return OSDAssemble ( hpidCurr, htidCurr, lpaddr, NULL );
}

BOOL
SYGetPrevInst(
    HPID    hpid,
    HTID    htid,
    PADDR   paddr
    )
{
    UOFFSET uoff;
    BOOL fReturn = FALSE;

    ASSERT ( ADDR_IS_LI ( *paddr ) );

    SYFixupAddr ( paddr );
    fReturn = OSDGetPrevInst ( hpid, htid, paddr, &uoff );

    if ( emiAddr ( *paddr ) ) {
        if ( !SYUnFixupAddr ( paddr ) ) {
           return( -1 ); // for DAMFillAsmFromNext
        }
    }

    return fReturn;
}

void
SYSetEmi(
    HPID    hpid,
    HTID    htid,
    LPADDR  lpaddr
    )
{
    ASSERT ( ! ADDR_IS_LI ( *lpaddr ) );
    VERIFY ( OSDSetEmi( hpid, htid, lpaddr ) == xosdNone );
}



XOSD
SYGetAddr(
    HPID    hpid,
    HTID    htid,
    ADR     adr,
    LPADDR  lpaddr
    )
{
    XOSD    xosd;

    // Clear the segment part of the addr.
    // This is an added safety measure for 32-bit segments:
    // we'll clear the segment before calling OSDGetAddr, so if only the
    // bottom two bytes are filled in, the segment will still be valid

    GetAddrSeg(*lpaddr) = 0;

    xosd = OSDGetAddr (hpid, htid, adr, lpaddr);

    if (xosd != xosdNone) {
        return xosd;
    }

    ADDR_IS_LI ( *lpaddr ) = FALSE;

    if (emiAddr (*lpaddr)) {
        SYUnFixupAddr (lpaddr);
    }

    return xosd;
}

/*  SYFlipScreen
 *
 *  Purpose:  Flip the screen between the debugger and CV, if local, or display
 *        the debuggee if remote
 *
 *  Input:  fScreen - True if you want the OS screen, false if you want the
 *            CV screen
 *      fCmd  - True if OSDebug CMD that requested the screen flip is
 *            a command that should always flip
 */

BOOL fSmartSwap = TRUE;
BOOL fLastScreen = FALSE;
BOOL fLocalTransport = TRUE;

void
SYFlipScreen(
    HPID    hpid,
    HTID    htid,
    BOOL    fScreen,
    BOOL    fCmdType
    )
{
    HPRC hprc;

    StartTimer (itmSYFlipScreen);

    if ( hpid != hpidNull && ( hprc = HprcFromHpid ( hpid ) ) ) {

        LPPRC lpprc = (LPPRC)LLLpvFromHlle ( hprc );
        BOOL fFuncExec = lpprc->fFuncExec;

        UnlockHlle ( hprc );
        if ( fFuncExec ) {

            // don't flip if we are executing a function for the EE
            return;
        }
    }

    if ( flip_or_swap )
    {
        if ( fCmdType || ! ( !fCmdType && fSmartSwap ) ||
            ( !fCmdType && !fSmartSwap) )
        {
            if ( !fLocalTransport )
            {
                // remote operation
                VERIFY(OSDShowDebuggee( hpid, fScreen) == xosdNone);
            }
            else
            {
                // Local operation: only flip if we have to.
                
                if ( fLastScreen != fScreen)
                {
                    fLastScreen = fScreen;

                        if ( fScreen == CHILDSCREEN )
                        {
                            dos_screen();
                        }
                        else
                        {
                            island_screen();
                        }
                }
            }
        }
    }
    
    StopTimer (itmSYFlipScreen);
}

HLLI LlthdFromHprc ( HPRC hprc ) {

    if (hprc != 0 && hprc == hprcCurr) {    // optimization
        return lpprcCurr->llthd;
    }
    else {
        LPPRC lpprc;
        HLLI  hlli;
        ASSERT ( hprc != (HPRC) NULL );

        lpprc = (LPPRC)LLLpvFromHlle ( hprc );
        hlli = lpprc->llthd;
        UnlockHlle ( hprc );

        return hlli;
    }
}

HTID HtidFromHthd ( HTHD hthd ) {

    LPTHD lpthd;
    HTID  htid = htidNull;

    if ( hthd != hmemNull ) {
    lpthd = (LPTHD)LLLpvFromHlle ( hthd );
    htid = lpthd->htid;
    UnlockHlle ( hthd );
    }

    return htid;
}

unsigned long TidFromHthd ( HTHD hthd ) {
    unsigned long tid = 0;
    LPTHD lpthd;

    if ( hthd != (HTHD) NULL ) {
    lpthd = (LPTHD)LLLpvFromHlle ( hthd );

    tid = lpthd->tid;
    UnlockHlle ( hthd );
    }

    return tid;
}

HTID HtidFromTid ( HPRC hprc, unsigned long tid ) {

    return HtidFromHthd (
    LLHlleFindLpv ( LlthdFromHprc ( hprc ), hmemNull, &tid, cmpTid )
    );
}

unsigned long TidFromHtid ( HPRC hprc, HTID htid ) {

    return TidFromHthd ( HthdFromHtid ( hprc, htid ) );
}

HTHD HthdFromHtid ( HPRC hprc, HTID htid ) {

    if ( htid != 0 ) {
    return LLHlleFindLpv (
        LlthdFromHprc ( hprc ),
        hmemNull,
        &htid,
        cmpHtid
    );
    }
    else {
    return (HTHD) NULL;
    }
}

HTHD HthdFromTid ( HPRC hprc, unsigned long tid ) {

    return HthdFromHtid ( hprc, HtidFromTid ( hprc, tid ) );
}

HTHD GetNthThread ( WORD ithd ) {
    HTHD hthd = hmemNull;

    for ( hthd = LLHlleFindNext ( LlthdFromHprc ( hprcCurr ), hmemNull );
      hthd != hmemNull;
      hthd = LLHlleFindNext ( LlthdFromHprc ( hprcCurr ), hthd ) ) {

    if ( ithd == 0 ) {
        break;
    }

    ithd -= 1;
    }

    return hthd;
}

unsigned long PidFromHprc ( HPRC hprc ) {

    if (hprc != 0 && hprc == hprcCurr) {    // optimization
        return lpprcCurr->pid;
    }
    else {
        unsigned long pid = 0;

        if ( hprc != (HPRC) NULL ) {
            LPPRC lpprc = (LPPRC)LLLpvFromHlle ( hprc );

            pid = lpprc->pid;

            UnlockHlle ( hprc );
        }

        return pid;
    }
}

HPID HpidFromPid ( unsigned long pid ) {

    return HpidFromHprc ( HprcFromPid ( pid ) );
}

HPID HpidFromHprc ( HPRC hprc ) {

    if (hprc != 0 && hprc == hprcCurr) {    // optimization
        return lpprcCurr->hpid;
    }
    else {
        HPID hpid = hpidNull;

        if ( hprc != (HPRC) NULL ) {
            LPPRC lpprc = (LPPRC)LLLpvFromHlle ( hprc );

            hpid = lpprc->hpid;

            UnlockHlle ( hprc );
        }

        return hpid;
    }
}

HPRC HprcFromHpid ( HPID hpid ) {

    return LLHlleFindLpv ( llprc, 0, &hpid, cmpHpid );
}

HPRC HprcFromPid ( unsigned long pid ) {

    return LLHlleFindLpv ( llprc, 0, &pid, cmpPid );
}

unsigned long PidFromHpid ( HPID hpid ) {
    PID pid = 0;

    if ( hpid != hpidNull ) {
#if !(defined (OSDEBUG4))
    OSDGetDebugMetric ( hpidCurr, htidNull, mtrcPidValue, &pid );
#else
        PST pst;
        if (OSDGetProcessStatus(hpidCurr, &pst) == xosdNone) {
            pid = pst.dwProcessID;
        }
        else ASSERT(FALSE);
#endif
    }

    return (ULONG) pid;
}

int PASCAL TDCompare ( LPTHD lpthd, unsigned long FAR *lpl, LONG l ) {

    switch ( l ) {

    case cmpHtid:

        if ( lpthd->htid == * ( (HTID FAR *) lpl ) ) {
        return fCmpEQ;
        }
        else {
            return fCmpLT;
        }
        break;

    case cmpTid:

    if ( lpthd->tid < *lpl ) {
        return fCmpLT;
        }
    else if ( lpthd->tid > *lpl ) {
        return fCmpGT;
    }
    else {
        return fCmpEQ;
    }
    break;

    case cmpLpthd: {
        LPTHD lpthd2 = (LPTHD) lpl;

        if ( lpthd->tid < lpthd2->tid ) {
        return fCmpLT;
        }
        else if ( lpthd->tid > lpthd2->tid ) {
        return fCmpGT;
        }
        else {
        return fCmpEQ;
        }
    }
    break;

    default:

        return fCmpGT;
    }
}

VOID FAR PASCAL PDKill ( LPV lpv ) {
    LPPRC lpprc = (LPPRC) lpv;

    if ( lpprc->szTitle ) {
    free ( lpprc->szTitle );
    }
    if ( lpprc->szName ) {
    free ( lpprc->szName );
    }
    if ( lpprc->szArgs ) {
    free ( lpprc->szArgs );
    }

    if ( lpprc->llthd ) {
    LLChlleDestroyLl ( lpprc->llthd );
    }
}

static int PASCAL PDCompare ( LPV lpv, unsigned long FAR *lpl, LONG l ) {
    LPPRC lpprc = (LPPRC) lpv;

    switch ( l ) {

    case cmpHpid:

        if ( lpprc->hpid == * ( (HPID FAR *) lpl ) ) {
        return fCmpEQ;
        }
        else {
        return fCmpLT;
        }
        break;

    case cmpPid:

        if ( lpprc->pid == *lpl ) {
        return fCmpEQ;
        }
        else {
        return fCmpLT;
        }
        break;

    case cmpTitle:

        return _ftcsicmp ( lpprc->szTitle, (char FAR *) lpl );
        break;

    case cmpName:

        return _ftcsicmp ( lpprc->szName, (char FAR *) lpl );
        break;

    default:

        ASSERT ( FALSE );
    }
    return fCmpLT;
}

int  GetProcId ( char FAR * lsz, HPRC hprc, EPI epi, int isz ) {
    LPPRC lpprc = (LPPRC)LLLpvFromHlle ( hprc );
    int iReturn = 0;

    if ( epi == epiDefault ) {
    epi = epiDef;
    }

    switch ( epi ) {

       // We use epiDefault through epiDef to get control of
       // an invalid lpprc when no debugging session is started !
    case epiDefault :
        iReturn = 0 ;
        break ;

    case epiTitle:

        CVsprintf ( lsz, "%-8s", lpprc->szTitle );
        iReturn = 8;
        break;

    case epiName:

        _ftcsncpy ( lsz, lpprc->szName, isz );
        *(lsz+isz-1) = '\0';
        iReturn = _ftcslen ( lpprc->szName );
        break;

    case epiPID:

        CVsprintf ( lsz, "%04X", lpprc->pid );
        iReturn = 4;
        break;

    default:

        ASSERT ( FALSE );
        break;
    }

    UnlockHlle ( hprc );

    return iReturn;
}

int ProcStatus ( HPRC hprc, char *sz, int cbLen ) {
    LPPRC lpprc = (LPPRC)LLLpvFromHlle ( hprc );
    char  pchStatus[40];

    switch ( lpprc->stp ) {

    case stpRunning:

        LoadString( hInst, DBG_RUNNING, pchStatus, sizeof( pchStatus ) );
        break;

    case stpDead:

        LoadString( hInst, DBG_TERMINATED, pchStatus, sizeof( pchStatus ) );
        break;

    case stpNotStarted:

        LoadString( hInst, DBG_NOT_STARTED, pchStatus, sizeof( pchStatus ) );
        break;

    case stpStopped:

        LoadString( hInst, DBG_STOPPED, pchStatus, sizeof( pchStatus ) );
        break;

    case stpGone:

        LoadString( hInst, DBG_NOT_EXIST, pchStatus, sizeof( pchStatus ) );
        break;

    default:

        ASSERT ( FALSE );
        break;
    }

    sprintf (
    sz,
    "%04X  %-8s  %-10s  ",
    (WORD) lpprc->pid,
    lpprc->szTitle,
    pchStatus
    );

    _ftcsncat ( sz, lpprc->szName, cbLen - _ftcslen ( sz ) - 1 );

    UnlockHlle ( hprc );

    return TRUE;
}

BOOL SYAfterReturn ( STO sto, HPID hpid, HTID htid ) {
    BOOL fReturn = FALSE;
    LPPRC lpprc = NULL;
    HPRC hprc;
    XOSD xosd;

    ASSERT ( hpid != (HPID) NULL );

	//
	// Cannot step when crash dump debugging
	//
	
	if (IsCrashDump ()) {
		MessageBeep (0);
		return FALSE;
	}

    hprc = HprcFromHpid ( hpid );
    lpprc = (LPPRC)LLLpvFromHlle ( hprc );

    if ( lpprc->stp == stpNotStarted ) {
    lpprc->stp = stpStopped;
    }

    if ( lpprc->stp == stpStopped )
    {
        STP stp = lpprc->stp;
        EXP exp = lpprc->exp;
        HTID htidOld = lpprc->htidExecuting;
        BOOL fStepOver = lpprc->fStepOver;
        EXOP exop = {0};

        freeze_view();

        lpprc->stp = stpRunning;
        lpprc->exp = expAfterRet;
        lpprc->htidExecuting = htid;
        lpprc->fStepOver = TRUE;

        exop.fSetFocus = TRUE;  // Set the debuggee Focus and Active window
        exop.fStepOver = TRUE;
        exop.fSingleThread = !!(sto & stoOneThread);
        exop.fQueryStep = !!(sto & stoQueryStep);
        exop.fInitialBP = !!(sto & stoInitialBP);
        exop.fPassException = !!(sto & stoPassEx);
        exop.fReturnValues = runDebugParams.fReturnValues;

        PrimeDbgMessagePump();
        xosd = OSDReturnStep( hpid, htid, &exop);
        fReturn = (xosd == xosdNone);

        if(!fReturn) {
            // if this fails, then we need to restore the last state...
            lpprc->stp = stp;
            lpprc->exp = exp;
            lpprc->htidExecuting = htidOld;
            lpprc->fStepOver = fStepOver;
        }
        else {
            DbgMessagePump();
        }
    }

    UnlockHlle ( hprc );
    return fReturn;
}

BOOL SYSingleStep ( BOOL fOverCalls, STO sto, HPID hpid, HTID htid ) {
    BOOL fReturn = FALSE;
    LPPRC lpprc = NULL;
    HPRC hprc;
    XOSD xosd;

    ASSERT ( hpid != (HPID) NULL );

	//
	// cannot step in crash dump
	//
	
	if (IsCrashDump ()) {
		MessageBeep (0);
		return FALSE;
	}

    hprc = HprcFromHpid ( hpid );
    lpprc = (LPPRC)LLLpvFromHlle ( hprc );

    if ( lpprc->stp == stpNotStarted ) {
    lpprc->stp = stpStopped;
    }

    if ( lpprc->stp == stpStopped ) {
        EXOP exop = {0};

        freeze_view();
        lpprc->stp = stpRunning;
        lpprc->exp = expStep;
        lpprc->htidExecuting = htid;
        lpprc->fStepOver = fOverCalls;
        lpprc->sto = sto;

        exop.fStepOver = fOverCalls;
        exop.fSingleThread = !!(sto & stoOneThread);
        exop.fQueryStep = !!(sto & stoQueryStep);
        exop.fInitialBP = FALSE;
        exop.fPassException = !!(sto & stoPassEx);
        exop.fReturnValues = runDebugParams.fReturnValues;

        PrimeDbgMessagePump();
        xosd = OSDSingleStep( hpid, htid, &exop);
        fReturn = (xosd == xosdNone);

#if !(defined (OSDEBUG4))
        if (xosd == xosdBrokenPipe) {
            return FALSE;
        }
#endif
        if (!fReturn) {
            lpprc->stp = stpStopped;
        }
        else {
            DbgMessagePump();
        }
    }

    UnlockHlle ( hprc );
    return fReturn;
}

BOOL SYRangeStep (
    BOOL fOverCalls,
    STO  sto,
    ADDR addrStart,
    ADDR addrEnd,
    HPID hpid,
    HTID htid
) {
    BOOL fReturn = FALSE;
    LPPRC lpprc = NULL;
    HPRC hprc;
    XOSD xosd;

    ASSERT ( hpid != (HPID) NULL );

	//
	// Cannot step in crash dump
	//
	
	if (IsCrashDump ()) {
		MessageBeep (0);
		return FALSE;
	}
	
    StartTimer(itmSYRangeStep);

    hprc = HprcFromHpid ( hpid );
    lpprc = (LPPRC)LLLpvFromHlle ( hprc );

    if ( lpprc->stp == stpNotStarted ) {
    lpprc->stp = stpStopped;
    }

    if ( lpprc->stp == stpStopped ) {
        EXOP exop = {0};

        freeze_view();
        lpprc->stp = stpRunning;
        lpprc->exp = expRange;
        lpprc->htidExecuting = htid;
        lpprc->fStepOver = fOverCalls;
        lpprc->addrStart = addrStart;
        lpprc->addrEnd = addrEnd;
        lpprc->sto = sto;

        ASSERT ( ADDR_IS_LI ( addrStart ) );
        ASSERT ( ADDR_IS_LI ( addrEnd ) );

        SYFixupAddr ( &addrStart );
        SYFixupAddr ( &addrEnd );

        StartTimer(itmOSDRangeStep);

        exop.fStepOver = fOverCalls;
        exop.fSingleThread = !!(sto & stoOneThread);
        exop.fQueryStep = !!(sto & stoQueryStep);
        exop.fInitialBP = !!(sto & stoInitialBP);
        exop.fPassException = !!(sto & stoPassEx);
        exop.fReturnValues = runDebugParams.fReturnValues;

        PrimeDbgMessagePump();
        xosd = OSDRangeStep( hpid, htid, &addrStart, &addrEnd, &exop);
        fReturn = (xosd == xosdNone);

        if (!fReturn) {
            lpprc->stp = stpStopped;
        }
        else {
            DbgMessagePump();
        }

		StopTimer(itmOSDRangeStep);
    }


    UnlockHlle ( hprc );
    StopTimer(itmSYRangeStep);
    return fReturn;
}

BOOL SYGo ( const EXOP* pexop, HPID hpid, HTID htid ) {
    BOOL fReturn = FALSE;
    LPPRC lpprc = NULL;
    HPRC hprc;
    HTHD hthd;
    LPTHD lpthd;
    EXOP exop = {0};
    XOSD xosd;

	//
	// Crash dump can't go.
	//
	
	if (IsCrashDump ()) {
		MessageBeep (0);
		return FALSE;
	}
	
    exop = *pexop;
    pexop = NULL;   // from here to end of fn, use exop, not pexop

    ASSERT ( hpid != (HPID) NULL );

    hprc = HprcFromHpid ( hpid );
    lpprc = (LPPRC)LLLpvFromHlle ( hprc );

    // Must find a thread that isn't dead.
    //
    // [FUTURE: I'M NOT SURE IF THIS IS NECESSARY]
    hthd = HthdFromHtid ( hprc, htid );
    lpthd = (LPTHD)LLLpvFromHlle ( hthd );
    ASSERT (lpthd );
    if ( lpthd && lpthd->tdf & tdfDead ) {
        HLLI    llthd = LlthdFromHprc ( hprc );

        UnlockHlle ( hthd );
        hthd = 0;
        while ( (hthd = LLHlleFindNext(llthd, hthd)) != 0 ) {
            lpthd = (LPTHD)LLLpvFromHlle ( hthd );
            if (!(lpthd->tdf & tdfDead))
                break;
            UnlockHlle ( hthd );
        }
    }
    UnlockHlle ( hthd );

    if ( lpprc->stp == stpNotStarted ) {
    lpprc->stp = stpStopped;
    }

    if ( lpprc->stp == stpStopped ) {

        freeze_view();
        lpprc->stp = stpRunning;
        lpprc->exp = expGo;
        lpprc->htidExecuting = htid;
        lpprc->sto = exop.fSingleThread ? stoOneThread : stoNone;
        exop.fQueryStep = TRUE;
        PrimeDbgMessagePump();

        exop.fGo = TRUE;
        xosd = OSDGo(hpid, htid, &exop);
        fReturn = (xosd == xosdNone);

        if (!fReturn) {
            lpprc->stp = stpStopped;
        }
        else {
            DbgMessagePump();
        }
    }

    UnlockHlle ( hprc );

    return fReturn;
}

void SYContinue ( HPRC hprc ) {
    LPPRC lpprc = (LPPRC)LLLpvFromHlle ( hprc );
    HPID  hpid = lpprc->hpid;
    EXOP  exop = {0};                       // Set all fields to false

    exop.fSingleThread = !!( lpprc->sto & stoOneThread );
    exop.fStepOver = lpprc->fStepOver;
    exop.fPassException = !!(lpprc->sto & stoPassEx);
    exop.fQueryStep = !!(lpprc->sto & stoQueryStep);
    exop.fInitialBP = FALSE;
    exop.fReturnValues = runDebugParams.fReturnValues;

    switch ( lpprc->exp ) {

    case expStopped:
        break;

    case expGo:

            PrimeDbgMessagePump();
        if ( OSDGo( hpid, lpprc->htidExecuting, &exop ) != xosdNone ) {
                ASSERT ( FALSE );
            }
            else {
                DbgMessagePump();
            }
        break;

    case expStep:
            PrimeDbgMessagePump();
        if ( OSDSingleStep( hpid, lpprc->htidExecuting, &exop)
                != xosdNone ) {
                ASSERT ( FALSE );
            }
            else {
                DbgMessagePump();
            }
        break;

    case expRange: {
                ADDR addrStart = lpprc->addrStart;
                ADDR addrEnd = lpprc->addrEnd;

                SYFixupAddr ( &addrStart );
                SYFixupAddr ( &addrEnd );
                PrimeDbgMessagePump();
                if ( OSDRangeStep( hpidCurr, lpprc->htidExecuting, &addrStart,
                    &addrEnd, &exop) != xosdNone) {
                    ASSERT(FALSE);
                }
                else {
                    DbgMessagePump();
                }

        }
        break;

    case expAfterRet:
            PrimeDbgMessagePump();
        if ( OSDReturnStep( hpid, lpprc->htidExecuting, &exop)
                != xosdNone ) {
                ASSERT ( FALSE );
            }
            else {
                DbgMessagePump();
            }
        break;

    default:
        ASSERT ( FALSE );
        break;
    }

    UnlockHlle ( hprc );
}

void SYPause ( HPRC hprc ) {
    LPPRC lpprc = (LPPRC)LLLpvFromHlle ( hprc );

    if ( lpprc->stp != stpNotStarted ) {

        if (lpprc->stp != stpDead) {
            lpprc->stp = stpStopped;
        }

    lpprc->exp = expStopped;
    lpprc->fHitBP = TRUE;
    UnlockHlle ( hprc );

    BPFreeType ( BPTMP );

    fEnvirGbl.fs.user_pc = FALSE;
    fEnvirGbl.fs.ctxtIp = FALSE;

    UpdateUserEnvir ( mUserPc | mAsmaddr );
    }
    else {
        UnlockHlle ( hprc );
    }
}

void SYStop ( HPRC hprc ) {

    LPPRC lpprc = (LPPRC)LLLpvFromHlle ( hprc );
    BOOL  fFuncExec;
    BOOL  fDead;

    StartTimer(itmSYStop);

    if ( lpprc->stp != stpNotStarted ) {

        if (lpprc->stp != stpDead) {
            lpprc->stp = stpStopped;
        }

    lpprc->exp = expStopped;
    fFuncExec = lpprc->fFuncExec;
        fDead = (lpprc->stp == stpDead);
    lpprc->fHitBP = TRUE;
    UnlockHlle ( hprc );

        // [cuda#5172 6/23/93 mikemo]  Even if we're doing a function
        // execution, we still want to execute the below block if the
        // function execution caused the debuggee to die.
    if ( !fFuncExec || fDead ) {
        BPFreeType ( BPTMP );

        fEnvirGbl.fs.user_pc = FALSE;
        fEnvirGbl.fs.ctxtIp = FALSE;

        UpdateUserEnvir ( mUserPc | mAsmaddr );
    }
    }
    else {
    // If we're trying to stop a loaded app, assert and die.
    // Otherwise, the app load failed.  That's OK continue
    ASSERT( !lpprc->fLoaded );
    UnlockHlle ( hprc );
    }

    StopTimer(itmSYStop);
}


XOSD SYNativeOnly ( BOOL fNative ) {
    XOSD xosd = xosdNone;

    if ( lpprcCurr->stp == stpRunning ) {
        return xosdProcRunning;
    }
    xosd = OSDNativeOnly ( hpidCurr, htidCurr, fNative );
    CheckCallBack ( TRUE );

    return xosd;
}

/*** SYFixupAddr
*
* Purpose: To fixup and address packet
*
* Input:
*   pADDR - The address in question
*
* Output:
* Returns:
*   TRUE if address fixed up, FALSE otherwise.
*
* Exceptions:
*
* Notes:
*
*
*************************************************************************/

BOOL LOADDS PASCAL SYFixupAddr ( PADDR paddr ) {

    // If this ASSERT fires, it means the segment portion of the addr
    // was not cleared before using/constructing the addr.
    // We check the SQL and Java bits because Galileo and Jakarta set these
    // bits to identify their addr packets.
    ASSERT( modeAddr(*paddr).fSql == 1 || modeAddr(*paddr).fJava == 1 ||
            (GetAddrSeg(*paddr) & 0xFFFF0000) == 0 );

#ifndef _SHIP
    // To help catch the case where the upper two bytes of a seg are
    // not zero, we'll put up a message box to alert the user.
    if (modeAddr(*paddr).fSql == 0 && modeAddr(*paddr).fJava == 0 && (GetAddrSeg(*paddr) & 0xFFFF0000) != 0) {
        if (MsgBox(Error, "Warning! ADDR packet detected with two high bytes non-zero. Please contact the Jakarta Debugger team. (Click Cancel to debug.)", MB_OKCANCEL) == IDCANCEL) {
            DebugBreak();
        }
    }
#endif

    // Clear the upper two bytes of the segment (if the addr isn't SQL or Java).
    // This is an added safety measure for 32-bit segments:
    // we'll clear the upper two bytes to help eliminate any bogus addrs
    if (modeAddr(*paddr).fSql == 0 && modeAddr(*paddr).fJava == 0) {
        GetAddrSeg(*paddr) = (GetAddrSeg(*paddr) & 0x0000FFFF);
    }

    XOSD xosd = xosdNone;

    StartTimer(itmSYFixupAddr);
    if ( ADDR_IS_LI ( *paddr ) ) {
    xosd = OSDFixupAddr( hpidCurr, htidCurr, paddr );
    }
    StopTimer(itmSYFixupAddr);

    return ( xosd == xosdNone && paddr->emi != 0 );
}

/*** SYUnFixupAddr
*
* Purpose: To unfixup an address packet
*
* Input:
*   pADDR - The address in question
*
* Output:
* Returns:
*   TRUE if address unfixed up, FALSE otherwise.
*
* Exceptions:
*
* Notes:
*
*
*************************************************************************/
BOOL LOADDS PASCAL SYUnFixupAddr ( PADDR paddr ) {

    // If this ASSERT fires, it means the segment portion of the addr
    // was not cleared before using/constructing the addr.
    // We check the SQL and Java bits because Galileo and Jakarta set these
    // bits to identify their addr packets.
    ASSERT( modeAddr(*paddr).fSql == 1 || modeAddr(*paddr).fJava == 1 ||
        (GetAddrSeg(*paddr) & 0xFFFF0000) == 0 );

#ifndef _SHIP
    // To help catch the case where the upper two bytes of a seg are
    // not zero, we'll put up a message box to alert the user.
    if (modeAddr(*paddr).fSql == 0 && modeAddr(*paddr).fJava == 0 && (GetAddrSeg(*paddr) & 0xFFFF0000) != 0) {
        if (MsgBox(Error, "Warning! ADDR packet detected with two high bytes non-zero. Please contact the Jakarta Debugger team. (Click Cancel to debug.)", MB_OKCANCEL) == IDCANCEL) {
            DebugBreak();
        }
    }
#endif

    // Clear the upper two bytes of the segment (if the addr isn't SQL or Java).
    // This is an added safety measure for 32-bit segments:
    // we'll clear the upper two bytes to help eliminate any bogus addrs
    if (modeAddr(*paddr).fSql == 0 && modeAddr(*paddr).fJava == 0) {
        GetAddrSeg(*paddr) = (GetAddrSeg(*paddr) & 0x0000FFFF);
    }

    XOSD  xosd = xosdNone;

    StartTimer(itmSYUnFixupAddr);
    if ( !ADDR_IS_LI ( *paddr ) ) {
    xosd = OSDUnFixupAddr( hpidCurr, htidCurr, paddr );
    }

    StopTimer(itmSYUnFixupAddr);
    return  ( xosd == xosdNone && paddr->emi != 0 );
}

BOOL SYProgramHasRun ( void ) {

    return lpprcCurr->stp != stpNotStarted;
}

static  HVOID   hLocked = NULL;

/*** MHOmfLock
*
*   Purpose:
*   To lock the omf in place. This is an EMS swap
*
*   Input:
*   hHandle - The symbol or type handle to swap in.
*
*   Output:
*   Returns:
*   The physical memory location
*
*   Exceptions:
*
*   Notes:
*
*************************************************************************/
HVOID LOADDS PASCAL MHOmfLock ( HVOID hHandle ) {
// VCNT is a hack for now, because we were hitting this assert;
// we'll track it down later

    return (HVOID) hHandle;
}
/*** MHOmfUnLock
*
*   Purpose:
*
*   Input:
*   hHandle     -The handle to unlock
*
*   Output:
*   Returns:
*
*   Exceptions:
*
*   Notes:
*   This is only for the debug version
*
*************************************************************************/
VOID  LOADDS PASCAL MHOmfUnLock( HVOID hHandle ) {
// VCNT is a hack for now, because we were hitting this assert;
// we'll track it down later
}

static int XlateSheErr[sheMax] = {
    0,
    NOSYMBOLS,
    FUTURESYMBOLS,
    MUSTRELINK,
    NOTPACKED,
    EMMINSUFMEM,
    0,
    0,
	0,
	0,
	SYMSCONVERTED,
	0,
	0,
    PDBNOTFOUND,
    PDBBADSIGNATURE,
    PDBINVALIDAGE,
    PDBOLDFORMAT
};

#define IsPdbSheErr(she)        ((BOOL)(shePdbNotFound<=(she) && (she)<=shePdbOldFormat))


BOOL
SYLoadAllOmf(
    )
/*++

Routine Description:

    Load all the DLLs and the EXE that have been registered with the symbol
    handler.

Comments:

--*/
{
    HEXE hexe;
    SHE  she = sheNone;

    she = SHAddDllsToProcess ( );

    for ( hexe = SHGetNextExe ( (HEXE) NULL );
      hexe != (HEXE) NULL && she == sheNone;
      hexe = SHGetNextExe ( hexe )
    )
    {

        LSZ lszExeName;

    lszExeName = SHGetExeName ( hexe );
    she = SHLoadDll ( lszExeName, FALSE );

    if (she == sheOutOfMemory) {
       errno = EMMINSUFMEM;
    }
        else if (XlateSheErr[she] != 0) {

            if ( IsPdbSheErr(she) )
            {
                char    szPdbName[_MAX_PATH];

                SHPdbNameFromExe( lszExeName, szPdbName, sizeof( szPdbName ) );
                CVMessage( WARNMSG, XlateSheErr[she], CMDWINDOW, szPdbName, lszExeName );
            }
            else {

                if (she != sheNoSymbols)
                    // temp hack for sniff test and pending SYOpen fix
                    CVMessage( WARNMSG, XlateSheErr[she], CMDWINDOW, lszExeName );
            }

       // the caller of SYLoadAllOmf expects an error return only
       // if something really serious happens that requires an
       // abort, so for errors where there is no symbol info we
       // set she = sheNone so just the queued warning is
       // displayed

           she = sheNone;
    }
    }

    return she == sheNone;
}

void _HUGE_ * PASCAL LOADDS LDShalloc(long n, size_t size) {
    void _HUGE_ * hpret;

    hpret = _fmalloc ( n * size );
    return hpret;
}

void PASCAL LOADDS LDShfree(void _HUGE_ * buffer) {
    _ffree ( buffer );
}


BOOL LOADDS PASCAL SYFHasSource( HPID hpid, LPADDR lpaddr ) {
    return HasSource(*lpaddr);
}
