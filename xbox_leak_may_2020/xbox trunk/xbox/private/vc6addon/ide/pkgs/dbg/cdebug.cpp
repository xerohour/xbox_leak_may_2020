#include "stdafx.h"
#pragma hdrstop

#include "idbgproj.h"
#include "prjguid.h"
#include "dbgenc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


LONG
OSDAPI
RegistryGetSet(
	LPTSTR		KeyName,
	LPTSTR		ValueName,
	DWORD*		pdwType,
	BYTE*		Data,
	DWORD		cbData,
	BOOL		fSet
	);

BOOL
WINAPI
GetRegistryRoot(
	OUT		LPTSTR	RegistryRoot,
	IN OUT	LPDWORD	pLength
	);
	

CDebug::CDebug ()
{
	m_fMtrcRemoteTL = FALSE;
	m_strExeExtension = ".EXE";
	m_platform = unknown_platform;
	m_fQuickRestart = FALSE;
	m_fEditedByENC = FALSE;

	m_hInstSH = m_hInstTL = m_hInstEM = m_hInstEE = NULL;

	for (int i = 0; i < MAX_NM_DLL; i += 1)
	{
		m_rghInstNM[i] = NULL;
		m_rgszNMName[i] = NULL;
	}

	m_nNMCount = 0;
	m_fInitialized = FALSE; // CDebug::Init not called yet.
}

CDebug::~CDebug ()
{
}

BOOL CDebug::Init(BOOL fSrcInit)
{
	// this routine should only be called once
	if (m_fInitialized)
		return TRUE;

	m_fInitialized = TRUE;


	// This routine should not be performed until both
	// src and bld packages have initialized
	ASSERT( IsPackageLoaded(PACKAGE_VCPP) && gpIBldSys);

    uniq_platform upID;


    if (!SUCCEEDED(gpIBldPlatforms->GetPrimaryPlatform(&upID)))
		return FALSE;

    ASSERT(upID != unknown_platform);

    gpIBldPlatforms->GetPlatformSHName(upID, &m_szSHName);
    gpIBldPlatforms->GetPlatformEMName(upID, &m_szEMName);
    gpIBldPlatforms->GetPlatformEEName(upID, &m_szEEName);

    UINT uTLIndex;

    gpIBldPlatforms->GetPlatformLocalTLIndex(upID, &uTLIndex);

    ASSERT(uTLIndex != -1);

	if ( uTLIndex == -1 )
		return FALSE;

    gpIBldPlatforms->GetPlatformTLName(upID, uTLIndex, &m_szTLName);

    // Update metrics with a NULL HPID, thus setting default values
    UpdateMetrics(0);

    return DEBUGInitialize();
}

void CDebug::OnConfigChange()
{
	uniq_platform upID;

	if (!FIsActiveProjectDebuggable( NULL, NULL, NULL, &upID ))
		return;

	ConfigChange(upID);
}

// 8-3-94 t-mattca: split ConfigChange from OnConfigChance to
//  enable programatic TL/EM specification (i.e. for the Debug Active..
//  menu item)

void CDebug::ConfigChange(uniq_platform upID)
{
	BOOL	fNewOSDebug = FALSE;
	BOOL	fBuildable = FALSE;

	if (gpIBldPlatforms->IsPlatformSupported(upID) == S_OK)
	{
		 if (gpIBldPlatforms->GetPlatformBuildable (upID, &fBuildable)
		 	 != NOERROR)
			{
				fBuildable = FALSE;
			}
	}
			

	if (!fBuildable) {
		 // ignore call if this is a non-buildable platform
		return;
	}

	UINT iTL;

	gpIBldPlatforms->GetPlatformCurrentTLIndex(upID, &iTL);

	// If the EM or TL changed, we will need to recalculate all our metrics.
	// (This is quite expensive, so we don't want to do so on every change
	// notification.)
    if ( theApp.m_jit.GetActive() && pDebugCurr && !pDebugCurr->MtrcRemoteTL() )
    {
        // fix for 14165
        // if JIT debugging  && not remote JIT
        // force local debugging
		gpIBldPlatforms->GetPlatformLocalTLIndex(upID, &iTL);

        ASSERT(iTL != -1);

		gpIBldPlatforms->SetPlatformCurrentTLIndex(upID, iTL);
    }

	const TCHAR *szNewTLName;
	const TCHAR *szNewEMName;

	gpIBldPlatforms->GetPlatformTLName(upID, iTL, &szNewTLName);
	gpIBldPlatforms->GetPlatformEMName(upID, &szNewEMName);

	fNewOSDebug = (m_szTLName == NULL || _tcsicmp( m_szTLName, szNewTLName ) ||
                   m_szEMName == NULL || _tcsicmp( m_szEMName, szNewEMName ));
	
	// If we're going to be loading new OSDebug DLLs, we must unload the old
	// ones
	if (fNewOSDebug && hpidCurr == 0)
		DEBUGDestroy();

    m_szTLName = szNewTLName;
	m_szEMName = szNewEMName;

	gpIBldPlatforms->GetPlatformSHName(upID, &m_szSHName);
	gpIBldPlatforms->GetPlatformEEName(upID, &m_szEEName);
	gpIBldPlatforms->GetPlatformNMCount(upID, (int *)&m_nNMCount);

	ASSERT(m_nNMCount < MAX_NM_DLL);

	for (UINT i = 0; i < m_nNMCount; i += 1)
		gpIBldPlatforms->GetPlatformNMName(upID, i, &(m_rgszNMName[i]));

	m_platform = upID;

	UINT tfl;

	gpIBldPlatforms->GetPlatformTLFlags(upID, iTL, &tfl);

    m_fMtrcRemoteTL = !(tfl & TFL_LOCAL);

	// We've called CVUninitForQCQP, so reinitialize the debugger's memory mgr
	if (fNewOSDebug && hpidCurr == 0)
		(VOID) DEBUGInitialize();
}

void CDebug::OnDebugStart (HPID hpid)
{
	UpdateMetrics (hpid);
}

BOOL CDebug::SupportsQuickRestart( ) const
{
	BOOL retVal = FALSE;

	switch (m_platform)
	{
		case win32x86:
		case win32mips:
		case mac68k:
		case macppc:
			retVal = TRUE;
			break;

		case win16x86:
		case win32alpha:
		case win32ppc:
		case java:
			retVal = FALSE;
			break;

		default:

			// Unkown platform.

			ASSERT( FALSE );
			break;
	}


	if (m_fMtrcRemoteTL) {
		retVal = FALSE;
	}
	
	return retVal;
}


BOOL CDebug::IsMacintosh() const
{
	return( m_platform == mac68k || m_platform == macppc );
}

BOOL CDebug::IsJava() const
{
	return m_platform == java;
}

void CDebug::UpdateMetrics (HPID hpid)
{
	// NOTE: some of these calls may fail, because hpid may be a
	// dummy hpid which was just created for the purpose of querying
	// metrics, or hpid might even be NULL.

	if (OSDGetDebugMetric( hpid, NULL, mtrcBreakPoints,&m_bptsMtrcBreakPoints ) != xosdNone)
		m_bptsMtrcBreakPoints = (BPTS) -1; // Allow user to set all types of BPs

	if (OSDGetDebugMetric( hpid, NULL, mtrcOleRpc,&m_fMtrcOleRpc ) != xosdNone)
		m_fMtrcOleRpc = TRUE;

	if (OSDGetDebugMetric( hpid, NULL, mtrcNativeDebugger, &m_fMtrcNativeDebugger ) != xosdNone)
		m_fMtrcNativeDebugger = FALSE;

	if (OSDGetDebugMetric( hpid, NULL, mtrcAsync, &m_asyncMtrcAsync ) != xosdNone)
		m_asyncMtrcAsync = 0;

	if (OSDGetDebugMetric( hpid, NULL, mtrcEndian,&m_endMtrcEndian ) != xosdNone)
		m_endMtrcEndian = endLittle;

	if (OSDGetDebugMetric( hpid, NULL, mtrcThreads,&m_fMtrcHasThreads ) != xosdNone)
		m_fMtrcHasThreads = TRUE;

	if (OSDGetDebugMetric( hpid, NULL, mtrcExceptionHandling, &m_fMtrcExceptionHandling ) != xosdNone)
		m_fMtrcExceptionHandling = TRUE;

	if (OSDGetDebugMetric( hpid, NULL, mtrcMultInstances, &m_fMtrcMultipleInstances ) != xosdNone)
		m_fMtrcMultipleInstances = FALSE;
}

extern int LOADDS PASCAL OSDAssert(char FAR *lszMsg, char FAR *lszFile, unsigned iln);


static KNF knf =
{
	sizeof (KNF),
	LDSFmalloc,
	LDSFrealloc,
	LDSFfree,
	LDShalloc,
	(LPFNFREEHUGE) LDShfree,

	(MMALLOCMEM) BMAlloc,
	BMFree,
	BMLock,
	BMUnlock,

	LDSLLHlliInit,
	LDSLLHlleCreate,
	LDSLLAddHlleToLl,
	LDSLLHlleAddToHeadOfLI,
	LDSLLInsertHlleInLl,
	LDSLLFDeleteHlleFromLl,
	LDSLLFRemoveHlleFromLl,
	LDSLLChlleDestroyLl,
	LDSLLHlleFindNext,
	LDSLLHlleFindLpv,
	LDSLLHlleGetLast,
	LDSLLChlleInLl,
	LDSLLLpvFromHlle,
	BMUnlock,

	OSDAssert,
	dsquit,

	SYOpen,
	SYClose,
	SYReadFar,
	SYSeek,
	SYFixupAddr,
	SYUnFixupAddr,
	SYProcessor,

	SYTell,
	SYFindExeFile,
	DLoadedSymbols,
	SYGetDefaultShe,
	SYFindDebugInfoFile,
	GetRegistryRoot,
};



BOOL CDebug::LoadComponent(int DLLtype, int nWhichOne /* = -1*/)
{
	const TCHAR* szDllName = NULL;
	HINSTANCE* phInst = NULL;

	switch (DLLtype)
	{
    	default:
    		ASSERT(FALSE); // Unknown DLLtype
    		return FALSE;

	case DLL_SH:
		szDllName = m_szSHName;
		phInst = &m_hInstSH;
		break;

	case DLL_EM:
		szDllName = m_szEMName;
		phInst = &m_hInstEM;
		break;

	case DLL_NM:
		ASSERT(nWhichOne >= 0 && nWhichOne < MAX_NM_DLL);
		szDllName = m_rgszNMName[nWhichOne];
		phInst = &m_rghInstNM[nWhichOne];
		break;

	case DLL_TL:
		ASSERT(m_szTLName != NULL);
		szDllName = m_szTLName;
		phInst = &m_hInstTL;
		break;

	case DLL_EE:
		szDllName = m_szEEName;
		phInst = &m_hInstEE;
		break;
	}

	// Did you forget something?
	ASSERT(szDllName != NULL && phInst != NULL);

	// Make sure we are loading a debug or release debugger .dll
	TCHAR rgchDllPath[MAX_PATH+1];
	_tcscpy(rgchDllPath, szDllName);
#ifdef _DEBUG
	_tcscat(rgchDllPath, "D.DLL");
#else
	_tcscat(rgchDllPath, ".DLL");
#endif

	UINT oldmode = ::SetErrorMode( 0 );

	// Load the DLL and report any loading errors
	*phInst = ::LoadLibrary( rgchDllPath );

	::SetErrorMode( oldmode );

	if (*phInst == NULL)
	{
		ReportMissingFile( rgchDllPath, stStandard );
		return FALSE;
	}

	// No error loading/initializing, validate the version number
	LPAVS (LOADDS PASCAL *lpfn)(void);

	// Must be able to get the version check function
	// (GetProcAddr is case sensitive!!!)

	*((FARPROC *)&lpfn) = GetProcAddress(*phInst, "DBGVersionCheck");

#if defined (OSDEBUG4)
	if( !lpfn )
		*((FARPROC *)&lpfn) = GetProcAddress(*phInst, DBGVERSIONPROCNAME );
#endif

	if( lpfn == NULL )
		return FALSE;

	LPAVS lpavs = lpfn();

	if( lpavs == NULL )
		return FALSE;

	static BOOL fDidItOnce = FALSE;

	if (!fDidItOnce &&
//
// OSDEBUG4 version has iRmj and iRmm as WORD members not BYTE
// so I've renamed/relaid out AVS to match dolphin's.
//
#if defined (OSDEBUG4)
		(lpavs->iApiVer != DBG_API_VERSION ||
		lpavs->iApiSubVer != DBG_API_SUBVERSION)
#else
		(lpavs->iRmj != DBG_API_VERSION ||
		lpavs->iRmm != DBG_API_SUBVERSION)
#endif
		)
	{
		char sz [512];
		CString str;
		
		fDidItOnce = TRUE;
		
		VERIFY( str.LoadString(ERR_BADOSDEBUGDLL ) );
		wsprintf( sz, (const char *)str, szDllName );
		// FUTURE: Nuke "WARNING"
		MessageBox(_SushiGetSafeOwner(NULL),sz,"WARNING",MB_ICONEXCLAMATION | MB_OK);

		return FALSE;
	}

	return TRUE;
}

BOOL CDebug::UnLoadComponent(int DLLtype, int nWhichOne /* = -1*/)
{
	HINSTANCE* phInst = NULL;

	switch (DLLtype)
	{
    	default:
    		ASSERT(FALSE); // Unknown DLLtype
    		return FALSE;

    	case DLL_SH:
    		phInst = &m_hInstSH;
    		break;

    	case DLL_EM:
    		phInst = &m_hInstEM;
    		break;

    	case DLL_NM:
    		ASSERT(nWhichOne >= 0 && nWhichOne < MAX_NM_DLL);
    		phInst = &m_rghInstNM[nWhichOne];
    		break;

    	case DLL_TL:
    		phInst = &m_hInstTL;
    		break;

    	case DLL_EE:
    		phInst = &m_hInstEE;
    		break;
	}

	ASSERT(phInst != NULL); // forget something?
	
	if (*phInst != NULL)
	{
		FreeLibrary(*phInst);
		*phInst = NULL;
	}

	return TRUE;
}

BOOL CDebug::InitializeComponent(int DLLtype, int nWhichOne /* = -1*/)
{
	const TCHAR* szThunk = NULL;
	HINSTANCE* phInst = NULL;

	switch (DLLtype)
	{
    	default:
    		ASSERT(FALSE); // Unknown DLLtype
    		return FALSE;

    	case DLL_SH:
    		phInst = &m_hInstSH;
    		szThunk = "SHInit";
    		break;

    	case DLL_EM:
    		phInst = &m_hInstEM;
    		szThunk = "EMFunc";
    		break;

    	case DLL_NM:
    		ASSERT(nWhichOne >= 0 && nWhichOne < MAX_NM_DLL);
    		phInst = &m_rghInstNM[nWhichOne];
    		szThunk = "EMFunc";
    		break;

    	case DLL_TL:
    		ASSERT(m_szTLName != NULL);
    		phInst = &m_hInstTL;
    		szThunk = "TLFunc";
    		break;

    	case DLL_EE:
    		phInst = &m_hInstEE;
    		szThunk = "EEInitializeExpr";
    		break;
	}

	// Did you forget something?
	ASSERT(szThunk != NULL && phInst != NULL);

	// Get initializing function
	LPFNSVC lpfn = (LPFNSVC)GetProcAddress(*phInst, szThunk);

	if (lpfn == NULL)
	{
		ASSERT(FALSE); // Missing initialization function
		return FALSE;
	}

	// Initialize each type of DLL
	XOSD xosd = xosdNone;
	switch (DLLtype)
	{
    	case DLL_SH :
    		// Initialize all the symbol handler functions
    		xosd = (XOSD)!((LPFNSHINIT)lpfn)(&lpshf, &knf);
    		break;

    	case DLL_EM :
    		xosd = OSDAddEM((EMFUNC)lpfn, lpdbf ,&hemCurr, emNative);
    		break;

    	case DLL_NM :
    		xosd = OSDAddEM((EMFUNC)lpfn, lpdbf, &hnmCurr, emNonNative);
    		break;

    	case DLL_TL :
    		xosd = OSDAddTL((TLFUNC)lpfn, lpdbf, &htlCurr);
    		break;

    	case DLL_EE :
    		ESLoadExpr(0, (unsigned int)*phInst, (LPFNEEINIT)lpfn);
    		ESSetFromIndex(0);
    		EEEnableAutoClassCast(FALSE);
    		break;
	}

	if (xosd == xosdNone)
	{
		BOOL        fOK = TRUE;

		// The EM or TL has changed, update the metrics with what is
		// now available.
		if( fOK && hemCurr && (DLLtype == DLL_EM || DLLtype == DLL_TL) )
		{
			HPID    hpid = 0;

			// Since we are loading a new EM or TL, we should not be debugging
			// and shouldn't have a current hpid yet
			ASSERT( hpidCurr == 0 );

			if (OSDCreateHpid(CallBack, hemCurr, (HTL)0, &hpid) != xosdNone)
				hpid = 0;

			// hpidNull is okay
			UpdateMetrics( hpid );

			// If we've created an hpid, destroy it
			if ( hpid )
				OSDDestroyHpid( hpid );
		}
	}

	// AuxPrintf("xosd=%d", (int)xosd);

	return xosd == xosdNone;
}

BOOL CDebug::UnInitializeComponent(int DLLtype, int nWhichOne /*= -1*/)
{
	switch (DLLtype)
	{
    	default:
    		ASSERT(FALSE); // Unknown DLLtype
    		return FALSE;

    	case DLL_SH:
    		if (m_hInstSH != NULL)
    			SHUnloadSymbolHandler(FALSE);
    		break;
    		
    	case DLL_TL:
    		if( htlCurr != NULL )
    		{
    			VERIFY( OSDDeleteTL(htlCurr) == xosdNone );
    			htlCurr = 0;
    		}
    		break;
    		
    	case DLL_EM:
    		if( hemCurr != NULL )
    		{
    			VERIFY( OSDDeleteEM(hemCurr) == xosdNone );
    			hemCurr = 0;
    		}
    		break;

    	case DLL_NM:
    		if( hnmCurr != NULL )
    		{
    			VERIFY( OSDDeleteEM(hnmCurr) == xosdNone );
    			hnmCurr = 0;
    		}
    		break;
    		
    	case DLL_EE:
    		if (m_hInstEE != NULL)
    			EEUnload();
    		break;
	}

	return TRUE;
}

BOOL CDebug::InitOSD(VOID)
{

	//
	// I hate the person that wrote this function.
	//
	
	// First, try to load all the component DLLs (apart from any NMs)

	if (LoadComponent (DLL_SH))
	{
		if (LoadComponent (DLL_EE))
		{
			if (LoadComponent (DLL_TL))
			{
				if (LoadComponent (DLL_EM))
				{
					// They all loaded correctly
					// Now try to initialize them
					if (InitializeComponent (DLL_SH))
					{
						if (InitializeComponent (DLL_EE) &&
							InitDBF () &&
							SYInitProcessList () &&
							(InitCallBack (), (OSDInit (lpdbf) == xosdNone)))

						{
							if (InitializeComponent (DLL_TL))
							{
								if (InitializeComponent (DLL_EM))
								{
									// Everything's fine so far,
									// Try to load and initialize any NMs
									// We dno't really care if they don't
									// load correctly
									for (UINT i = 0; i < m_nNMCount; i += 1)
									{
										if (LoadComponent(DLL_NM, i))
										{
											if (!InitializeComponent(DLL_NM, i))
												UnLoadComponent(DLL_NM, i);
										}
									}

									//
									// Load/Initialize proxy dll for edit &
									// continue.
									// This depends on SAPI and has to be done
									// after SAPI has been loaded.
									//

									//
									// NOTE: putting this initialization here
									// means that w/o ENC the debugger will
									// still work; which is fine.
									//
									// Maybe we need a messagebox in the
									// case that ENC failed to load
									//
								
									InitENCProxy ();

									return TRUE;
								}

								UnInitializeComponent (DLL_TL);
							}

							UnInitializeComponent (DLL_EE);
						}

						UnInitializeComponent (DLL_SH);
					}

					UnLoadComponent (DLL_EM);
				}

				UnLoadComponent (DLL_TL);
			}

			UnLoadComponent (DLL_EE);
		}

		UnLoadComponent (DLL_SH);
	}
	return FALSE;
}

BOOL CDebug::UnInitOSD(VOID)
{
	UINT i;

	UnInitializeComponent (DLL_EM);
	UnInitializeComponent (DLL_TL);
	UnInitializeComponent (DLL_EE);
	UnInitializeComponent (DLL_SH);

	for (i = 0; i < m_nNMCount; i += 1)
		UnInitializeComponent(DLL_NM, i);


	UnLoadComponent (DLL_EM);
	UnLoadComponent (DLL_TL);
	UnLoadComponent (DLL_SH);
	UnLoadComponent (DLL_EE);

	for (i = 0; i < m_nNMCount; i += 1)
		UnLoadComponent(DLL_NM, i);

	UnInitDBF ();
	UnInitENCProxy();

	OSDTerm();

	return TRUE;
}

#if defined (OSDEBUG4)

LPSTR
PASCAL
_SHGetSymbol(
    LPADDR a,
    DWORD b,
    LPADDR c,
    LPSTR d,
    LPDWORD e
    )
{
   ODR odr;
   odr.lszName = d;
   LPSTR lpstr = SHGetSymbol(a, c, (SOP)b, &odr);
   *e = odr.dwDeltaOff;
   return( lpstr );
}

MPT
MptFromPlatform(
	uniq_platform
	);


BOOL
WINAPI
GetRegistryRoot(
	OUT		LPTSTR	RegistryRoot,
	IN OUT	LPDWORD	pLength
	)
{
	ULONG			Length = 0;
	BOOL			fSuccess = FALSE;
	LPTSTR			Root;


	Root = GetRegistryKeyName ();
	Length = strlen (Root);

	if (Length <= *pLength)
	{
		*pLength = Length;
		strcpy (RegistryRoot, Root);
		fSuccess = TRUE;
	}

	
	return fSuccess;
}



MPT
WINAPI
GetTargetProcessor(
	HPID	hpid
	)
/*++

Routine Description:

	Get Target Processor type from HPID.
	
Comments:

	An HPID of NULL is OK.  If the EM has been initialized properly, we will
	get this value from the EM.  Otherwise, we will look at the current
	platform in pDebugCurr.  We will probablly have to be a little smarter
	about this when we do multi-process debugging.
	
--*/
{
	XOSD	xosd;
	MPT		mpt;
	UINT	up;
	

	xosd = OSDGetDebugMetric (hpid, NULL, mtrcProcessorType, &mpt);

	if (xosd != xosdNone)
	{
		if (pDebugCurr)
		{
			up = pDebugCurr->GetPlatform ();
			mpt = MptFromPlatform ( (uniq_platform) up);
		}
		else
		{
			ASSERT (FALSE);
			mpt = mptUnknown;
		}
	}

	return mpt;
}
	
	
DWORD
PASCAL
_SHGetPublicAddr(LPADDR a, LSZ s)
{
	ADDR addrTmp = *a;
	return (DWORD) SHPublicNameToAddr(&addrTmp, a, s);
}
#endif


typedef LSZ (LPFNSYM LPFNGETSYMBOL)(LPADDR, LPADDR, SHORT, LPVOID);

BOOL CDebug::InitDBF()
{
	UnInitDBF();

	lpdbf = (LPDBF) LDSFmalloc(sizeof(DBF));
	ASSERT( lpdbf != NULL );	// If this fails then there's no hope

	lpdbf->lpfnMHAlloc              = LDSFmalloc;
	lpdbf->lpfnMHRealloc            = LDSFrealloc;
	lpdbf->lpfnMHFree               = LDSFfree;
	lpdbf->lpfnSHLpGSNGetTable      = &SHLpGSNGetTable;
	lpdbf->lpfnLLInit               = LDSLLHlliInit;
	lpdbf->lpfnLLCreate             = LDSLLHlleCreate;
	lpdbf->lpfnLLAdd                = LDSLLAddHlleToLl;
	lpdbf->lpfnLLInsert             = LDSLLInsertHlleInLl;
	lpdbf->lpfnLLDelete             = LDSLLFDeleteHlleFromLl;
	lpdbf->lpfnLLNext               = LDSLLHlleFindNext;
	lpdbf->lpfnLLDestroy            = LDSLLChlleDestroyLl;
	lpdbf->lpfnLLFind               = LDSLLHlleFindLpv;
	lpdbf->lpfnLLSize               = LDSLLChlleInLl;
	lpdbf->lpfnLLLock               = LDSLLLpvFromHlle;
	lpdbf->lpfnLLUnlock             = BMUnlock;
	lpdbf->lpfnLLLast               = LDSLLHlleGetLast;
	lpdbf->lpfnLLAddHead            = LDSLLHlleAddToHeadOfLI;
	lpdbf->lpfnLLRemove             = LDSLLFRemoveHlleFromLl;
	lpdbf->lpfnLBAssert             = OSDAssert;
	lpdbf->lpfnLBQuit               = dsquit;

	lpdbf->lpfnSHGetSymbol          = (LPFNGETSYMBOL) lpshf->pSHGetSymbol;
	lpdbf->lpfnSHGetDebugData		= lpshf->pSHGetDebugData;
	lpdbf->lpfnSHGetPublicAddr		= _SHGetPublicAddr;
	lpdbf->lpfnGetTargetProcessor	= GetTargetProcessor;
	lpdbf->lpfnGetSet               = RegistryGetSet;

	lpdbf->lpSHF					= (PVOID)lpshf;

	return TRUE;
}

BOOL CDebug::UnInitDBF()
{
    if(lpdbf != NULL)
        LDSFfree(lpdbf);

	lpdbf = NULL;

	return TRUE;
}

// get the current debugger DLL settings
// if NM passed in, returns TRUE if found, FALSE if not
// if no debugger current, returns FALSE

BOOL CDebug::GetComponents( const TCHAR *szNM, const TCHAR**szTL, const TCHAR **szEM, const TCHAR**szSH, const TCHAR**szEE )
{
	if (this==NULL)
		return FALSE;

	BOOL bRet = TRUE;

	if (szNM)
	{
		int i;
		for (i=0; i<m_nNMCount; i++)
			if (_tcsicmp( m_rgszNMName[i], szNM)==0)
				break;
		if (i==m_nNMCount)
			bRet = FALSE;
	}

	*szTL = m_szTLName;
	*szEM = m_szEMName;
	*szSH = m_szSHName;
	*szEE = m_szEEName;

	return bRet;
}
