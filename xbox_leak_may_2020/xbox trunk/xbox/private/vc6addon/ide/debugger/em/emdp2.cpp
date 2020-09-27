/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    emdp2.c

Abstract:

    This file contains the some of the machine independent portions of the
    execution model.  The machine dependent portions are in other files.

Author:

    Kent Forschmiedt (kentf) 10-23-92

Environment:

    Win32 -- User

Notes:

    The original source for this came from the CodeView group.

--*/
#include "emdp.h"
#include "resource.h"

#include <stdio.h>

extern CRITICAL_SECTION csCache;


#define DECL_XOSD(n,v,s) {IDS_##n, NULL, n},
static struct _EMERROR {
    UINT    uID;
    LPTSTR  lpsz;
    XOSD    xosd;
} EmErrors[] = {
#include "xosd.h"
};
const int nErrors = (sizeof(EmErrors)/sizeof(*EmErrors));
#undef DECL_XOSD




LPCTSTR
EmError(
    XOSD    xosd
    )
{
    int i = -1; // Not a magic value
    BOOL fFound = FALSE;


    //
    // Do a linear search through the array.  This is not
    // a serious performance issue since there are less than 100
    // errors and we only do this at error time.  
    //
    for (i = 0; i < nErrors && !fFound; i++) {
	if (EmErrors[i].xosd == xosd) {
	    fFound = TRUE;
	}
    }
	i--; // We're left with one i too many otherwise

    if (!fFound) {
	//
	// If it still didn't work, use the first entry
	//
		// The zero'th error is supposed the be xosdNone.  
		//

	i = 0;
	assert (EmErrors[i].xosd == xosdNone);
    }

    if (EmErrors[i].lpsz == NULL) {
	//
	// The string hasn't been loaded, so do it now.
	// How long should it be?
	//
	EmErrors[i].lpsz = (LPTSTR) MHAlloc (_MAX_PATH + 1);
		assert (hInstance);
	if (LoadString (hInstance, EmErrors[i].uID, EmErrors[i].lpsz, _MAX_PATH) == 0) {
			//
			// Resource was not found!
			//
			assert (FALSE);
			return NULL;
		}
    }

    return EmErrors[i].lpsz;
}

VOID
FreeEmErrorStrings(
    VOID
    )
{

    int i;

    for (i = 0; i < nErrors; i++) {
	if (EmErrors[i].lpsz) {
	    MHFree (EmErrors[i].lpsz);
	}
	EmErrors[i].lpsz = NULL;
    }
}



XOSD
LoadFixups (
    HPID  hpid,
    MODULELOAD *lpmdl
    )
/*++

Routine Description:

    This function is called in response to a module load message.  It
    will cause information to be internally setup for doing fixups/
    unfixups ...

Arguments:

    hpid        - Supplies a handle for the process

    lpmdl       - Supplies a pointer to a module load message from the DM

Return Value:

    xosd Error code

--*/

{
    XOSD            xosd = xosdNone;
    HMDI            hmdi;
    MDI *                       lpmdi;
    LPTSTR          lpName;
    HPRC            hprc = HprcFromHpid( hpid );
    HLLI            llmdi = LlmdiFromHprc ( hprc );
    LPPRC           lpprc;
    //DWORD         fIsRemote;
    //LPSTR         p1;
	BOOL			fBar = FALSE;

    hmdi = LLCreate ( llmdi );
    if ( hmdi == 0 ) {
	assert( "load dll cannot create llmdi" && FALSE );
	return xosdOutOfMemory;
    }

    lpmdi = (MDI *)LLLock ( hmdi );

    lpmdi->mte  = lpmdl->mte;
    lpmdi->lpBaseOfDll = (DWORD)lpmdl->lpBaseOfDll;
    lpmdi->dwSizeOfDll = lpmdl->dwSizeOfDll;
    lpmdi->StartingSegment = lpmdl->StartingSegment;

    lpmdi->CSSel  = lpmdl->CSSel;
    lpmdi->DSSel  = lpmdl->DSSel;
    lpmdi->lpBaseOfData = (DWORD)lpmdl->uoffDataBase;
    lpmdi->fRealMode = lpmdl->fRealMode;
    lpmdi->fOffset32 = lpmdl->fOffset32;
    lpmdi->fFlatMode = lpmdl->fFlatMode;
#ifdef OMNI
	lpmdi->fOmniMap = lpmdl->fOmniMap;
#endif
    lpmdi->fSendNLG = FALSE;
    lpmdi->nlg.fEnable = FALSE;
    if (lpmdi->fFlatMode) {
	lpprc = (LPPRC) LLLock( hprc );
	lpprc->selFlatCs = lpmdi->CSSel;
	lpprc->selFlatDs = lpmdi->DSSel;
	LLUnlock( hprc );
    }

    // Thread local storage info (iTls is 1 based)

    lpmdi->isecTLS = lpmdl->isecTLS;
    //lpmdi->iTls = lpmdl->iTls;
    //lpmdi->uoffiTls = lpmdl->uoffiTls;

    //
    // REVIEW:BUG how do we get the count of tls indexes?
    //

    if (lpmdl->uoffiTls) {
	lpprc->cmdlTLS++;
    }

    lpmdi->cobj = lpmdl->cobj;
    lpName = ( (LPTSTR) &( lpmdl->rgobjd[lpmdl->cobj] ) );

	if (*lpName == '|') {
		lpName++;
		fBar = TRUE;
	}

	if (lpName [0] == '#' && lpName [1] == ':' && lpName [2] == '\\') {
		lpmdi->lszName = (char*) MHAlloc (_tcslen (lpName));

		if (fBar) {
			_tcscpy (&lpmdi->lszName [1], lpName + 3);
			lpmdi->lszName [0] = '|';
		} else {
			_tcscpy (lpmdi->lszName, lpName + 3);
		}
		
	} else {
		if (fBar) {
			lpName --;
		}
		lpmdi->lszName = MHStrdup ( lpName );
	}

	lpName = NULL;

    if ( lpmdi->lszName == NULL )  {
	LLUnlock( hmdi );
	assert( "load dll cannot dup mod name" && FALSE );
	return xosdOutOfMemory;
    }

    lpmdi->rgobjd = (LPOBJD) MHAlloc ( sizeof(OBJD) * lpmdi->cobj);
    if ( lpmdi->rgobjd == NULL ) {
	LLUnlock( hmdi );
	assert( "load cannot create rgobjd" && FALSE );
	return xosdOutOfMemory;
    }
    _fmemcpy ( lpmdi->rgobjd,
	       lpmdl->rgobjd,
	       sizeof(OBJD) * lpmdi->cobj);

#ifdef OMNI

#pragma message ("building OMNI")

	// look for section with jit map, load and fixup the pointer to it
	if (lpmdi->fOmniMap) 
	{
		DWORD i;
		for (i = 0; i < lpmdi->cobj; i++)
		{
			DWORD cbOff = 1;
			if (lpmdi->rgobjd[i].pOmniJITMap)
			{
				// read the jit map, its beyond the string and its size is determined 
				// by the size of this section
				lpmdi->rgobjd[i].pOmniJITMap = (DWORD *) MHAlloc(lpmdi->rgobjd[i].cb);
				if ( lpmdi->rgobjd[i].pOmniJITMap == NULL ) {
					LLUnlock( hmdi );
					assert( "load cannot create jitmap" && FALSE );
					return xosdOutOfMemory;
				}
				_fmemcpy((BYTE *)lpmdi->rgobjd[i].pOmniJITMap, lpName + strlen(lpName) + cbOff, 
					lpmdi->rgobjd[i].cb);
				cbOff += lpmdi->rgobjd[i].cb;
			}
		}
	}
#endif

    LLAdd ( llmdi, hmdi );

#if 0
    //
    // If there is an hfile here, regardless of whether the target is
    // remote the hfile must become an OSDEBUG hfile.
    //

    //
    // The string is either "name" or "|name|time|cksum|hfile|imagebase|"
    // If there is an hfile, translate it.
    //

    hfile = -1;

    p1 = _tcschr(p1+1, '|');           // name
    if (p1) {
	p1 = p1 && _tcschr(p1+1, '|'); // time
	p1 = p1 && _tcschr(p1+1, '|'); // cksum
	p1 = p1 && _tcschr(p1+1, '|'); // hfile
	p1 = p1 && (p1 + 1);          //   first digit

	assert(p1 && *p1);

	if (p1) {
	    DWORD dw;
	    assert(p1[8] == '|');  // what, me trust the DM?
	    dw = _tcstoul(p1, &p2, 16);
	    xosd = OSDMakeFileHandle(hpid, dw, &dw);
	    // this can only fail because of xosdOutOfMemory or a bad hpid.
	    assert(xosd == xosdNone);
	    sprintf(p1, "%08X", dw);
	    p1[8] = '|';
	}
    }

    DebugMetric ( hpid, NULL, mtrcRemote, &fIsRemote );

    hfile = SHLocateSymbolFile( lpmdi->lszName, fIsRemote );

    //
    // get the pointer to the debug data (fpo/pdata/omap)
    //

    lpmdi->lpDebug = GetDebugData( hfile );

    //
    // the hfile stays open.  the SH will finish with it later.
    //

#endif // later...

    ((LPPRC)LLLock(hprc))->fLoadingModule = 1;
    LLUnlock(hprc);

    xosd = CallDB (
	dbcModLoad,
	hpid,
	NULL,
	CEXM_MDL_native,
	(UINT) hmdi,
	lpmdi->lszName
    );

    LLUnlock ( hmdi );

    return xosd;
}



XOSD
UnLoadFixups (
    HPID hpid,
    HEMI hemi
    )
/*++

Routine Description:

    This function is called in response to a module unload message.

    It returns the emi of the module being unloaded

Arguments:

    hprc        - Supplies a handle for the process
    hemi        - Supplies hemi (if Unload)

Return Value:

    TRUE if deleted

--*/

{
    HLLI        hlli;
    HMDI        hmdi;


    hlli = LlmdiFromHprc( HprcFromHpid ( hpid ));
    hmdi = LLFind( hlli, 0, (LPVOID)&hemi, (LONG) emdiEMI);

    if (hmdi == hmdiNull) {
	return xosdUnknown;
    } else {
		LPMDI lpmdi = (LPMDI) LLLock( hmdi );
		if ( lpmdi->nlg.fEnable ) {
			XOSD    xosd;
			NLG     nlg;

			lpmdi->nlg.fEnable = FALSE;
			nlg = lpmdi->nlg;

//                      SwapNlg ( &nlg );

			xosd = SendRequestX (
				dmfNonLocalGoto,
				hpid,
				NULL,
				sizeof ( nlg ),
				&nlg
				);
		}
		LLUnlock ( hmdi );
		LLDelete( hlli, hmdi );
		return xosdNone;
	}
}                               /* UnLoadFixups() */




XOSD
CreateThreadStruct (
    HPID hpid,
    TID tid,
    HTID FAR *lphtid
    )
{
    HPRC  hprc  = HprcFromHpid ( hpid );
    LPPRC lpprc = (LPPRC) LLLock ( hprc );
    HTHD  hthd  = hthdNull;
    LPTHD lpthd = NULL;


    hthd = HthdFromTid ( hprc, tid );
    assert(hthd == NULL);
    if ( hthd == hthdNull ) {

	hthd  = LLCreate ( lpprc->llthd );
	lpthd = (LPTHD) LLLock ( hthd );

	CallDB (
	    dbcoCreateThread,
	    hpid,
	    NULL,
	    CEXM_MDL_native,
	    sizeof ( HTID ),
	    (LPVOID)lphtid
	);

	lpthd->htid   = *lphtid;
	lpthd->hprc   = hprc;
	lpthd->tid    = tid;
	lpthd->drt    = drtNonePresent;
	lpthd->dwcbSpecial = lpprc->dmi.cbSpecialRegs;
	if (lpthd->dwcbSpecial) {
	    lpthd->pvSpecial = MHAlloc(lpthd->dwcbSpecial);
	}

	lpthd->regs = MHAlloc(SizeOfContext(hpid));
	lpthd->frameRegs = MHAlloc(SizeOfContext(hpid));

	LLAdd ( lpprc->llthd, hthd );
    }
    else {
	lpthd = (LPTHD) LLLock ( hthd );
	assert ( lpthd->fVirtual == TRUE );
	*lphtid = lpthd->htid;
	lpthd->fVirtual = FALSE;
	lpthd->drt    = drtNonePresent;
    }

    lpthd->fFlat = TRUE;  // Assume flat to start off.

    LLUnlock ( hthd );
    LLUnlock ( hprc );

    return xosdNone;
}                              /* CreateThreadStruct() */


VOID
GetExceptionInfo(
    HPID hpid
    )
/*++

Routine Description:

	Get exception information from DM.

--*/
{
	// ideally this would be a struct of DBB plus an EXCMD, but padding gets
	// in the way
    struct {
		DBB dbb;
		BYTE pad[sizeof(EXCMD)];
	} rgb;
    LPDBB    pdbb = &rgb.dbb;
	LPEXCMD lpcmd = (LPEXCMD)&rgb.dbb.rgbVar;
    LPEXCEPTION_CONTROL lpexc = &lpcmd->exc;
    LPEXCEPTION_DESCRIPTION lpexd = &lpcmd->exd;
    LPEXCEPTION_DESCRIPTION lpexdr;
    HEXD hexd;
    HPRC hprc;
    HLLI llexc;
    LONG lT;
    XOSD xosd;
	LPPRC	lpprc = NULL;

    hprc = HprcFromHpid(hpid);

	if (!hprc) {
		return ;
    }

	lpprc = (LPPRC) LLLock (hprc);

	if (lpprc->fInitedExceptList) {

		//
		// The exception info has alread been initialzied
		//

		LLUnlock (hprc);
		return ;
	}
	
	llexc = lpprc->llexc;

    // force the DMINFO struct to get loaded

    DebugMetric ( hpid, NULL, mtrcProcessorType, &lT );


    // get exception info

    pdbb->dmf  = dmfGetExceptionState;
    pdbb->hpid = hpid;
    pdbb->htid = NULL;
    *lpexc = exfFirst;


    GuardTL();
	do {

		xosd = CallTL (tlfRequest, hpid, sizeof (rgb), &rgb);

		if ((xosd != xosdNone) || (LpDmMsg->xosdRet != xosdNone)) {
			break;
		}

		//
		// add to local exception list
		//

		hexd = LLCreate( llexc );
		LLAdd( llexc, hexd );
		lpexdr = (LPEXCEPTION_DESCRIPTION) LLLock( hexd );
		*lpexdr = *((LPEXCEPTION_DESCRIPTION)(LpDmMsg->rgb));
		LLUnlock( hexd );

		//
		// ask for the next one
		//

		*lpexd = *((LPEXCEPTION_DESCRIPTION)(LpDmMsg->rgb));
		*lpexc = exfNext;

    } while (1); // lpexd->dwExceptionCode != 0); /* 0 is valid exception code */
    ReleaseTL();

	lpprc->fInitedExceptList = TRUE;
	LLUnlock (hprc);
}


XOSD
CreateHprc (
    HPID hpid
    )
{
    XOSD  xosd = xosdNone;
    HPRC  hprc;
    LPPRC lpprc;

    hprc = LLCreate ( llprc );

    if ( hprc == 0 ) {
	return xosdOutOfMemory;
    }

    LLAdd ( llprc, hprc );

    lpprc = (LPPRC) LLLock ( hprc );

    lpprc->stat = statDead;
    lpprc->hpid = hpid;
    lpprc->pid  = (PID) 0;
    lpprc->fDmiCache = 0;

    lpprc->llthd = LLInit (
	sizeof ( THD ),
	llfNull,
	TiDKill,
	TDComp
    );

    if ( lpprc->llthd == 0 ) {
	xosd = xosdOutOfMemory;
    }

    lpprc->llmdi = LLInit ( sizeof ( MDI ), llfNull, MDIKill, MDIComp );

    if ( lpprc->llmdi == 0 ) {
	xosd = xosdOutOfMemory;
    }

    lpprc->llexc = LLInit ( sizeof(EXCEPTION_DESCRIPTION),
			    llfNull,
			    NULL,
			    EXCComp );
    if ( lpprc->llexc == 0 ) {
	xosd = xosdOutOfMemory;
    }

    LLUnlock ( hprc );

    return xosd;
}

VOID
DestroyHprc (
    HPRC hprc
    )
{
    EnterCriticalSection(&csCache);

    LLDelete ( llprc, hprc );
    FlushPTCache();

    LeaveCriticalSection(&csCache);
}

VOID
DestroyHthd(
    HTHD hthd
    )
{
    LPTHD lpthd;
    HPRC  hprc;

    EnterCriticalSection(&csCache);

    lpthd = (LPTHD) LLLock ( hthd );
    hprc = lpthd->hprc;
    LLUnlock ( hthd );
    LLDelete ( LlthdFromHprc ( hprc ), hthd );
    FlushPTCache();

    LeaveCriticalSection(&csCache);
}

void EMENTRY
PiDKill (
    LPVOID lpv
    )
{
    LPPRC lpprc = (LPPRC) lpv;
    LLDestroy ( lpprc->llthd );
    LLDestroy ( lpprc->llmdi );
    LLDestroy ( lpprc->llexc );
}

void EMENTRY
TiDKill (
    LPVOID lpv
    )
{
    LPTHD lpthd = (LPTHD) lpv;

    if (lpthd->pvSpecial) {
	MHFree(lpthd->pvSpecial);
    }

    if (lpthd->regs) {
	MHFree(lpthd->regs);
    }

    if (lpthd->frameRegs) {
	MHFree(lpthd->frameRegs);
    }
}

void EMENTRY
MDIKill(
    LPVOID lpv
    )
{
    LPMDI lpmdi = (LPMDI)lpv;
    if (lpmdi->lszName) {
	MHFree(lpmdi->lszName);
	lpmdi->lszName = NULL;
    }
    if (lpmdi->rgobjd) {
	MHFree(lpmdi->rgobjd);
	lpmdi->rgobjd = NULL;
    }
    if (lpmdi->lpDebug) {
	MHFree(lpmdi->lpDebug);
	lpmdi->lpDebug = NULL;
    }
}


int EMENTRY
PDComp (
    LPVOID lpv1,
    LPVOID lpv2,
    LONG lParam
    )
{

    Unreferenced(lParam);

    if ( ( (LPPRC) lpv1)->hpid == *( (LPHPID) lpv2 ) ) {
	return fCmpEQ;
    }
    else {
	return fCmpLT;
    }
}

int EMENTRY
TDComp (
    LPVOID lpv1,
    LPVOID lpv2,
    LONG lParam
    )
{

    Unreferenced(lParam);

    if ( ( (LPTHD) lpv1)->htid == *( (LPHTID) lpv2 ) ) {
	return fCmpEQ;
    }
    else {
	return fCmpLT;
    }
}


int EMENTRY
MDIComp (
    LPVOID lpv1,
    LPVOID lpv,
    LONG lParam
    )
{
    LPMDI lpmdi = (LPMDI) lpv1;

    switch ( lParam ) {

	case emdiName:
	    if ( !_ftcschr( (const char *) lpv, _T('|') ) ) {
		TCHAR Buffer[MAX_PATH];
		LPTSTR p1,p2;
		p1 = lpmdi->lszName;
		if ( *p1 == _T('|') ) {
		    p1 = _tcsinc(p1);
		}
		p2 = _ftcschr(p1, _T('|'));
		if ( !p2 ) {
		    p2 = p1 + _ftcslen(p1);
		}
		memcpy(Buffer, p1, (p2-p1)*sizeof(TCHAR));
		Buffer[p2-p1]=_T('\0');
		return _tcsicmp ( (const char *) lpv, Buffer );

	    } else {
		return _tcsicmp ( (const char *) lpv, lpmdi->lszName );
	    }

	case emdiEMI:
	    return !(lpmdi->hemi == *(( HEMI FAR * ) lpv ) );

	case emdiMTE:
	    return !(lpmdi->mte == *((LPWORD) lpv ));

	case emdiBaseAddr:
	    return !(lpmdi->lpBaseOfDll == *((OFFSET *) lpv));

		case emdiNLG:
			return !lpmdi->fSendNLG;

	default:
	    return (0);
	    break;
    }
}


int
EMENTRY EXCComp(
    LPVOID lpRec,
    LPVOID lpVal,
    LONG lParam
    )
{
    Unreferenced(lParam);
    if ( ((LPEXCEPTION_DESCRIPTION)lpRec)->dwExceptionCode ==
							*((LPDWORD)lpVal)) {
	return fCmpEQ;
    } else {
	return fCmpLT;
    }
}


DWORD
RvaOmapLookup(
    DWORD   rva,
    LPOMAP  rgomap,
    DWORD   comap
    )
{
    OMAP  *pomapLow;
    OMAP  *pomapHigh;

    pomapLow = rgomap;
    pomapHigh = rgomap + comap;

    while (pomapLow < pomapHigh) {
	unsigned    comapHalf;
	OMAP  *pomapMid;

	comapHalf = comap / 2;

	pomapMid = pomapLow + ((comap & 1) ? comapHalf : (comapHalf - 1));

	if (rva == pomapMid->rva) {
	    return(pomapMid->rvaTo);
	}

	if (rva < pomapMid->rva) {
	    pomapHigh = pomapMid;
	    comap = (comap & 1) ? comapHalf : (comapHalf - 1);
	}

	else {
	    pomapLow = pomapMid + 1;
	    comap = comapHalf;
	}
    }

    assert(pomapLow == pomapHigh);

    // If no exact match, pomapLow points to the next higher address

    if (pomapLow == rgomap) {
	// This address was not found

	return(0);
    }

    if (pomapLow[-1].rvaTo == 0) {
	// This address is in a deleted/inserted range

	return(0);
    }

    // Return the new address plus the bias

    return(pomapLow[-1].rvaTo + (rva - pomapLow[-1].rva));
}

XOSD
GetObjLength(
    HPID hpid,
    LPGOL lpgol
)
{
    SEGMENT segAddr;

    SetEmi ( hpid, lpgol->lpaddr );

    *(lpgol->lplBase) = 0L;
    segAddr  = GetAddrSeg ( *(lpgol->lpaddr) );

    // SwapEndian ( &wSegAddr, sizeof ( wSegAddr ) );

    GuardTL();
    SendRequestX ( dmfSelLim, hpid, NULL, sizeof ( SEGMENT ), &segAddr);

    // SwapEndian ( lpbBuffer, sizeof ( LONG ) );
    *(lpgol->lplLen) = * ( (LONG *) LpDmMsg->rgb );
    ReleaseTL();

    return(xosdNone);
}

// given a section number, find where it started pre-lego
// and return its physical segment
ULONG
FindPreLegoSection(
	LPMDI lpmdi,
	ULONG wSeg,
	WORD *pSeg
)
{
	ULONG iSeg;
	LPGSI lpgsi = lpmdi->lpgsi;
	DWORD LastSectionStart = *(DWORD*)lpmdi->lpDebug->lpOmapTo;
	DWORD LastSectionSize = 0;
	const DWORD ImageAlign = lpmdi->lpDebug->dwOrigAlign;

	for (iSeg=0; iSeg<lpgsi->csgMax; iSeg++)
	{
		if (iSeg==wSeg)
		{
			assert(lpgsi->rgsgi[iSeg].isgPhy>0);
			*pSeg = lpmdi->rgobjd[(lpgsi->rgsgi[iSeg].isgPhy - 1)].wSel;
			return LastSectionStart;
		}

		LastSectionSize = (lpgsi->rgsgi[iSeg].cbSeg + ImageAlign -1) & ~(ImageAlign-1);
		LastSectionStart += LastSectionSize;
	}

	return (ULONG)-1;
}

// given an address in the pre-lego file, find its segment and offset
// returns (ULONG)-1 if failed to find
ULONG
FindPreLegoSegment(
	LPMDI lpmdi,
	ULONG uoff,
	ULONG *pOffset
)
{
	ULONG iSeg;
	LPGSI lpgsi = lpmdi->lpgsi;
	DWORD LastSectionStart = *(DWORD*)lpmdi->lpDebug->lpOmapTo;
	DWORD LastSectionSize = 0;
	const DWORD ImageAlign = lpmdi->lpDebug->dwOrigAlign;

	uoff -= lpmdi->lpBaseOfDll;

	if (uoff >= LastSectionStart)
		for (iSeg=0; iSeg < lpgsi->csgMax; iSeg++)
		{

			LastSectionSize = (lpgsi->rgsgi[iSeg].cbSeg + ImageAlign -1) & ~(ImageAlign-1);

			if (uoff < (LastSectionStart+LastSectionSize) )
			{
				*pOffset = uoff - LastSectionStart;
				return iSeg;
			}

			LastSectionStart += LastSectionSize;
		}

	*pOffset = 0;
	return (ULONG)-1;
}

XOSD
FixupAddr (
    HPID   hpid,
    HTID   htid,
    LPADDR lpaddr
    )
/*++

Routine Description:

    This routine is used to convert addresses between linker index (section
    or segment relative) addresses and real addresses (segment:offset).



Arguments:

    hpid        - Supplies the handle to the process for context to convert
			the address.
    lpaddr      - Pointer to address packet to be converted.

Return Value:

    XOSD error code.

--*/

{
    HMDI  hmdi;
    HPRC  hprc;
    HTHD  hthd;

    hprc = ValidHprcFromHpid(hpid);
    hthd = HthdFromHtid( hprc, htid );


    /*
     *  Check to see if the address is already a segment:offset pair and
     *  return if it is.
     */

    if ( !ADDR_IS_LI(*lpaddr) ) {
	return xosdNone;
    }

    /*
     *  Now based on the emi field of the address (which uniquely defines
     *  the executable module in the symbol handler), get the conversion
     *  information.
     */

    assert( emiAddr( *lpaddr ) != 0 );

    if ( (HPID)emiAddr ( *lpaddr ) == hpid ) {

	ADDR_IS_LI(*lpaddr) = FALSE;

	if (MPTFromHthd(hthd) != mptix86) {
	    /*
	     * The opposite of the code in UnFixupAddr -- Remove the 1
	     *      which was stuck in to make sure we did not think it was
	     *      an absolute
	     */
	    lpaddr->addr.seg = 0;
	}

	emiAddr( *lpaddr ) = 0;
	SetEmi( hpid, lpaddr );
    } else {

	/*
	 * Based on the symbol handler handle find our internal data structure
	 *      for the dll.
	 */

	hmdi = LLFind ( LlmdiFromHprc ( HprcFromHpid(hpid) ), 0,
		       (LPVOID)&emiAddr ( *lpaddr ), (LONG) emdiEMI );

	if ( hmdi == 0 ) {

	    return xosdUnknown ; // Do we need a special xosd for this.
				 // A common case where this will happen is pre-loading symbols.

	} else {

	    LPMDI lpmdi = (LPMDI) LLLock ( hmdi );
	    WORD  wsel;
	    LPSGI lpsgi;
	    unsigned short seg;

	    /*
	     *  If we could not find an internal structure for the DLL
	     *  then it must be some type of error.
	     */

	    if ( lpmdi == NULL ) {
		return xosdUnknown;
	    }

#ifdef OMNI
			if (lpmdi->fOmniMap) {
				DWORD i;
				DWORD off = GetAddrOff(*lpaddr);
			
				for (i = 0; i < lpmdi->cobj; i++) {
					if ((off >= lpmdi->rgobjd[i].rvaOffset) && 
						(off < (lpmdi->rgobjd[i].rvaOffset + lpmdi->rgobjd[i].cb))) {
						GetAddrOff ( *lpaddr ) -= lpmdi->rgobjd[i].rvaOffset;
						if (lpmdi->rgobjd[i].pOmniJITMap) {
							GetAddrOff(*lpaddr) = 
								lpmdi->rgobjd[i].pOmniJITMap[GetAddrOff(*lpaddr) / sizeof(DWORD)]; 
						}
						else {
							GetAddrOff ( *lpaddr ) += lpmdi->rgobjd[i].offset;
						}
						break;
					}
				}
			}
			else
#endif
	    /*
	     *  If the segment/selector is 0 then it must be an absolute
	     *  symbol and we therefore don't need to do any conversion.
	     *
	     *  If we could get no information describing the symbol
	     *  information then we can't do any conversion.
	     */

	    if ( (GetAddrSeg( *lpaddr ) > 0) && (lpmdi->lpgsi) ) {

		/*
		 * Get the linker index number for the segment number
		 *      and assure that it is valid.
		 */

		wsel = (WORD) (GetAddrSeg( *lpaddr ) - 1);
		if ( wsel >= lpmdi->lpgsi->csgMax ) {
		    /*
		     * Linker index is either not valid or not yet loaded
		     */

		    return xosdUnknown;
		}
		else {

		    /*
		     *  We know which section it comes from.  To compute
		     *  the real offset we need to add the following
		     *  items together.
		     *
		     *  original offset                GetAddrOff( *lpaddr )
		     *  offset of index in section     lpsgi->doffseg
		     *      (this is the group offset)
		     *  offset of section from base of rgobjd[physSeg-1].offset
		     *          image
		     *
		     *
		     *  The segment can just be loaded from the MAP.  Notice
		     *  that we will infact "lose" information in this
		     *  conversion sometimes.  Specifically a cs:data address
		     *  after unfixup and fixup will come out ds:data.  This
		     *  is "expected" behavior.
		     */
			if (lpmdi->lpDebug && lpmdi->lpDebug->lpOmapFrom)
			{
				// component has been legoed, so translate logical address
				// using pre-lego section starts
				WORD pSeg;
				DWORD off = FindPreLegoSection( lpmdi, wsel, &pSeg );
				if (off!=(ULONG)-1)
				{
					off = RvaOmapLookup( off + GetAddrOff( *lpaddr ),
				     lpmdi->lpDebug->lpOmapFrom,
				     lpmdi->lpDebug->cOmapFrom
				   );
					if (off)
					{
						GetAddrOff ( *lpaddr ) = off + lpmdi->lpBaseOfDll;
						GetAddrSeg ( *lpaddr ) = pSeg;
						ADDR_IS_REAL(*lpaddr) = (BYTE)lpmdi->fRealMode;
						ADDR_IS_OFF32(*lpaddr) = (BYTE)lpmdi->fOffset32;
						ADDR_IS_FLAT(*lpaddr) = (BYTE)lpmdi->fFlatMode;
						ADDR_IS_LI(*lpaddr) = FALSE;
						return xosdNone;
					}
				}
			}

		    lpsgi = &lpmdi->lpgsi->rgsgi[ wsel ];

            /* The process may have loaded fewer sections than the debug info
             * references.  Throw away references to the lost sections */
            if(lpsgi->isgPhy > lpmdi->cobj) {
                if(!hthd || lpsgi->isgPhy != lpmdi->isecTLS) {
                    LLUnlock(hmdi);
                    return xosdUnknown;
                }
            } else if (lpmdi->rgobjd[(lpsgi->isgPhy-1)].wPad == 0) {
                LLUnlock(hmdi);
			    return xosdUnknown;
		    }

		    GetAddrOff ( *lpaddr ) += lpsgi->doffseg;

		    if ( hthd && lpsgi->isgPhy == lpmdi->isecTLS ) {
			GetAddrOff ( *lpaddr ) += GetTlsBase (
			    hprc,
			    hthd,
			    lpmdi
			    );
			seg = 0;
		    }
		    else {

			GetAddrOff( *lpaddr ) +=
			    (UOFFSET) (lpmdi->rgobjd[ (lpsgi->isgPhy - 1) ]. offset);

			seg = lpmdi->rgobjd[(lpsgi->isgPhy - 1)].wSel;
		    }
		}

		GetAddrSeg ( *lpaddr ) = seg;
	    }

	    /*
	     *  Set the bits describing the address
	     */

	    ADDR_IS_REAL(*lpaddr) = (BYTE)lpmdi->fRealMode;
	    ADDR_IS_OFF32(*lpaddr) = (BYTE)lpmdi->fOffset32;
	    ADDR_IS_FLAT(*lpaddr) = (BYTE)lpmdi->fFlatMode;
	    ADDR_IS_LI(*lpaddr) = FALSE;

	    /*
	     * Now release the module description
	     */

	    LLUnlock ( hmdi );
	}
    }

    return xosdNone;
}                               /* FixupAddr() */


#ifdef OMNI
DWORD GetOffFromOmniJITMap(DWORD offIn, DWORD* pOmniJITMap, DWORD cb)
{
	DWORD i, iLo, iHi;
	signed long iRet;
	iLo = 0;
	iHi = cb / sizeof(DWORD);
	assert(!(offIn & 0xfffffffc));
    while (iLo <= iHi) {
	i = iLo + ((iHi - iLo) / 2);
		iRet = pOmniJITMap[i];

		if (!iRet)
		{
			// try to find a valid midpoint - first look up
			while ((--i >= iLo) && !(iRet = pOmniJITMap[i]));
			if (!iRet) {
				// no luck looking up go down
				i = iLo + ((iHi - iLo) / 2);
				while ((++i <= iHi) && !(iRet = pOmniJITMap[i]));
				assert(!iRet);
			}
		}

		iRet -= offIn;
		if (iRet < 0) {
	    iLo = i + 1;
	} else if (iRet > 0) {
	    iHi = i - 1;
	} else 
			break;
    }
	assert(pOmniJITMap[i] == offIn);
	return (i * sizeof(DWORD));
}
#endif

#ifdef _DEBUG
#define	TEST_FIXUP
#endif

XOSD
UnFixupAddr(
    HPID   hpid,
    HTID   htid,
    LPADDR lpaddr
    )

/*++

Routine Description:

    This routine is called to convert addresses from Real Physical addresses
    to linker index addresses.  Linker index addresses have an advantage
    to the symbol handler in that we know which DLL the address is in.

    The result of calling UnFixupAddr should be one of the following:

    1.  A true Linker Index address.  In this case
	emi == the HEXE (assigned by SH) for the DLL containning the address
	seg == the Section number of the address
	off == the offset in the Section

    2.  Address not in a dll.  In this case
	emi == the HPID of the current process
	seg == the physical selector of the address
	off == the offset in the physical selector

    3.  An error

Arguments:

    hpid   - Supplies the handle to the process the address is in
    lpaddr - Supplies a pointer to the address to be converted.  The
	     address is converted in place

Return Value:

    XOSD error code

--*/

{
    HPRC        hprc;
    HTHD        hthd;
    LPPRC       lpprc;
    LDT_ENTRY   ldt;
    XOSD        xosd;
#ifdef TEST_FIXUP
	ADDR	testAddr = *lpaddr;
#endif

    /*
     *  If the address already has the Linker Index bit set then there
     *  is no work for use to do.
     */


    if ( ADDR_IS_LI(*lpaddr) ) {
	return xosdNone;
    }

    /*
     *  If the EMI field in the address is not already filled in, then
     *  we will now fill it in.
     */

    if ( emiAddr ( *lpaddr ) == 0 ) {
	SetEmi ( hpid, lpaddr );
    }

    /*
     *  Get the internal Process Descriptor structure
     */

    hprc = HprcFromHpid(hpid);
    hthd = HthdFromHtid( hprc, htid );

    /*
     *  Is the EMI we got from the address equal to the process handle?
     *  if so then we cannot unfix the address and should just set the
     *  bits in the mode field.
     */

    if ( (HPID)emiAddr ( *lpaddr ) != hpid ) {
	LPMDI lpmdi;
	HMDI  hmdi = LLFind (LlmdiFromHprc ( hprc ), 0,
			     (LPVOID)&emiAddr ( *lpaddr ), (LONG) emdiEMI);
	WORD            igsn;
	LPSGI           lpsgi;
	ULONG                   ulo;
	USHORT          seg;
	ULONG           iSeg;


	if (hmdi == 0) {
	    /*
	     * If we get here we are really messed up.  We have a valid (?)
	     *  emi field set in the ADDR packeet, it is not the process
	     *  handle, but it does not correspond to a known emi in the
	     *  current process.  Therefore bail out as an error
	     */

	    return xosdUnknown;
	}

	lpmdi = (LPMDI) LLLock ( hmdi );
	if ( lpmdi == NULL ) {
	    return xosdUnknown;
	}

	/*
	 * Start out by using the "default" set of fields.  These
	 *      are based on what our best guess is for the executable
	 *      module.  This is based on what the DM told use when
	 *      it loaded the exe.
	 */

	ADDR_IS_REAL(*lpaddr) = (BYTE)lpmdi->fRealMode;
	ADDR_IS_OFF32(*lpaddr) = (BYTE)lpmdi->fOffset32;
	ADDR_IS_FLAT(*lpaddr) = (BYTE)lpmdi->fFlatMode;

#ifdef OMNI
		if (lpmdi->fOmniMap) {
			DWORD i;
			DWORD off = GetAddrOff(*lpaddr);
			
			for (i = 0; i < lpmdi->cobj; i++) {
				if ((off >= lpmdi->rgobjd[i].offset) && 
					(off < (lpmdi->rgobjd[i].offset + lpmdi->rgobjd[i].cb))) {
					GetAddrOff(*lpaddr) += (lpmdi->rgobjd[i].rvaOffset - lpmdi->rgobjd[i].offset);
					break;
				}
				if (lpmdi->rgobjd[i].pOmniJITMap &&
					(off >=lpmdi->rgobjd[i].pOmniJITMap[0]) &&
					(off < lpmdi->rgobjd[i].pOmniJITMap[lpmdi->rgobjd[i].cb / sizeof (DWORD)])) {
					GetAddrOff(*lpaddr) = 
						GetOffFromOmniJITMap(GetAddrOff(*lpaddr), 
						lpmdi->rgobjd[i].pOmniJITMap, lpmdi->rgobjd[i].cb);
					GetAddrOff(*lpaddr) += lpmdi->rgobjd[i].rvaOffset;
					break;
				}
			}
		}
		else
#endif
	/*
	 *  If there is not table describing the layout of segments in
	 *      the exe, there is no debug information and there fore no
	 *      need to continue this process.
	 */

	if ( lpmdi->lpgsi == NULL ) {
	    LLUnlock( hmdi );
	    emiAddr( *lpaddr ) = (HEMI) hpid;
	    goto itsBogus;
	}

	if (lpmdi->lpDebug && lpmdi->lpDebug->lpOmapTo) {

	    ulo = RvaOmapLookup( lpaddr->addr.off - lpmdi->lpBaseOfDll,
				 lpmdi->lpDebug->lpOmapTo,
				 lpmdi->lpDebug->cOmapTo
			       );

	    if (!ulo) {

				//
		// Address can not be mapped.  Removed by Lego.
		//

				ADDR_IS_LI (*lpaddr) = TRUE;
				lpaddr->emi = (HEMI) hpid;
		LLUnlock ( hmdi );
		return xosdUnknown;
	    }

	    ulo += lpmdi->lpBaseOfDll;

		{
			ULONG seg2, off2;
			seg2 = FindPreLegoSegment( lpmdi, ulo, &off2 );
			if (seg2!=(ULONG)-1)
			{
				GetAddrSeg( *lpaddr ) = (USHORT) (seg2 + 1);
				GetAddrOff( *lpaddr ) = off2;
				ADDR_IS_LI( *lpaddr ) = TRUE;
#ifdef TEST_FIXUP
				{
					ADDR fixedaddr = *lpaddr;
					FixupAddr( hpid, htid, &fixedaddr );
					if (GetAddrSeg( testAddr ))
						assert( GetAddrSeg( fixedaddr ) == GetAddrSeg( testAddr ) );
					assert( GetAddrOff( fixedaddr ) == GetAddrOff( testAddr ) );
				}
#endif
				return xosdNone;
			}
		}

		} else {
		ulo = (ULONG) GetAddrOff( *lpaddr );
		}

	    seg = (USHORT) GetAddrSeg( *lpaddr );
	lpsgi = lpmdi->lpgsi->rgsgi;

		/*
	 *  First correct out the "segment" portion of the offset.
	 *
	 *  For flat addresses this means that we locate which section
	 *      number the address fell in and adjust back to that section
	 *
	 *  For non-flat address this mains locate which segment number
	 *      the selector matches
	 */

	if (ADDR_IS_FLAT( *lpaddr )) {
	    for ( iSeg=0; iSeg < lpmdi->cobj; iSeg++) {
		if ((lpmdi->rgobjd[ iSeg ].offset <= ulo) &&
		    (ulo < (OFFSET) (lpmdi->rgobjd[ iSeg ].offset +
				     lpmdi->rgobjd[ iSeg].cb))) {

		    ulo -= lpmdi->rgobjd[ iSeg ].offset;
		    break;
		}
	    }
	} else {
	    for (iSeg=0; iSeg < lpmdi->cobj; iSeg++) {
		if (lpmdi->rgobjd[iSeg].wSel == seg) {
		    break;
		}
	    }
	}

	if (iSeg == lpmdi->cobj) {
	    // This was not a normal section, so now check to see if it is a TLS section

		LPPRC lpprc = (LPPRC) LLLock ( hprc );

		if ( lpprc->cmdlTLS > 0 ) {
		    UOFFSET uoffT = GetTlsBase (
			hprc,
			hthd,
			lpmdi
			);

		    if (
			uoffT != 0 &&
			GetAddrOff ( *lpaddr ) >= uoffT &&
			GetAddrOff ( *lpaddr ) <  uoffT +
				lpmdi->rgobjd [ lpmdi->isecTLS ].cb
		    ) {
			iSeg = lpmdi->isecTLS - 1;
		    }
		    else {
			lpaddr->mode.fIsLI = TRUE;
			emiAddr ( *lpaddr ) = (HEMI)hpid;
			LLUnlock ( hprc );
			goto itsBogus;
		    }
		}

		LLUnlock ( hprc );

	    emiAddr( *lpaddr ) = (HEMI) hpid;
	    goto itsBogus;
	}

	iSeg += 1;

	for( igsn=0; igsn < lpmdi->lpgsi->csgMax; igsn++, lpsgi++ ) {

	    if ( (ULONG)lpsgi->isgPhy == iSeg &&
		lpsgi->doffseg <= ulo &&
		ulo < lpsgi->doffseg + lpsgi->cbSeg ) {

		GetAddrSeg( *lpaddr ) = (USHORT) (igsn + 1);
		GetAddrOff( *lpaddr ) = ulo - lpsgi->doffseg;

		break;
	    }
	}

	if (igsn == lpmdi->lpgsi->csgMax) {
	    LLUnlock ( hmdi );
	    emiAddr( *lpaddr ) = (HEMI) hpid;
	    goto itsBogus;
	}
	LLUnlock ( hmdi );

    } else {
    itsBogus:
	if (ADDR_IS_REAL( *lpaddr )) {
	    ADDR_IS_FLAT( *lpaddr ) = FALSE;
	    ADDR_IS_OFF32( *lpaddr ) = FALSE;
	} else {
	    /*
	     * See if the segment matches the flat segment.  If it does not
	     *      then we must be in a non-flat segment.
	     */

	    lpprc = (LPPRC) LLLock( hprc );

	    if ((lpaddr->addr.seg == 0) ||
		(lpprc->dmi.fAlwaysFlat) ||
		(lpaddr->addr.seg == lpprc->selFlatCs) ||
		(lpaddr->addr.seg == lpprc->selFlatDs)) {

		ADDR_IS_FLAT(*lpaddr) = TRUE;
		ADDR_IS_OFF32(*lpaddr) = TRUE;
		ADDR_IS_REAL(*lpaddr) = FALSE;

	    } else {

        GuardTL();
		xosd = SendRequestX(dmfQuerySelector, hpid, NULL,
				    sizeof(SEGMENT), &GetAddrSeg(*lpaddr)  );

		if (xosd != xosdNone) {
            ReleaseTL();
		    LLUnlock(hprc);
		    return xosd;
		}

		_fmemcpy( &ldt, LpDmMsg->rgb, sizeof(ldt));
        ReleaseTL();

		ADDR_IS_FLAT(*lpaddr) = FALSE;
		ADDR_IS_OFF32(*lpaddr) = (BYTE) ldt.HighWord.Bits.Default_Big;
		ADDR_IS_REAL(*lpaddr) = FALSE;
	    }
	    LLUnlock( hprc );
	}

	if ( MPTFromHthd(hthd) != mptix86) {

	    /*
	     *      This line is funny.  We assume that all addresses
	     *      which have a segment of 0 to be absolute symbols.
	     *      We therefore set the segment to 1 just to make sure
	     *      that it is not zero.
	     */

	    if (emiAddr(*lpaddr) == (HEMI) hpid) {
	       lpaddr->addr.seg = 1;
	    }
	}
    }

    ADDR_IS_LI(*lpaddr) = TRUE;
    return xosdNone;
}                               /* UnFixupAddr() */



void
UpdateRegisters (
    HPRC hprc,
    HTHD hthd
    )
{
    LPTHD lpthd = (LPTHD) LLLock ( hthd );
    HPID hpid = HpidFromHprc(hprc);
	XOSD xosd;

    GuardTL();
    xosd = SendRequest ( dmfReadReg, hpid, HtidFromHthd ( hthd ) );

	//assert(xosd == xosdNone);
	
	if(xosd == xosdNone){
		_fmemcpy ( lpthd->regs, LpDmMsg->rgb, SizeOfContext(hpid) );
		lpthd->drt = (DRT) (drtCntrlPresent | drtAllPresent);
	} else {
		lpthd->drt = (DRT) drtNonePresent ;
	}
    ReleaseTL();

    LLUnlock ( hthd );
}




void
RegisterEmi (
    HPID   hpid,
    HTID   htid,
    LPREMI lpremi
    )
{
    HLLI     llmdi;
    HMDI     hmdi;
    LPMDI    lpmdi;
    LPSGI    lpsgi;
    LPSGI    lpsgiMax;
    USHORT   usOvlMax = 0;
	ADDR     addr;
    llmdi = LlmdiFromHprc( HprcFromHpid ( hpid ) );
    assert( llmdi != 0 );

    hmdi = LLFind( llmdi, 0, lpremi->lsz, (LONG)emdiName );

    if (hmdi == 0) {
	hmdi = LLFind( llmdi, 0, lpremi->hemi, (LONG)emdiEMI );
    }

    assert( hmdi != 0 );

	if (hmdi == 0) {
		return ;
	}

    lpmdi = (LPMDI) LLLock ( hmdi );
    assert( lpmdi != NULL );

    assert( lpremi->hemi != 0 );

    lpmdi->hemi = lpremi->hemi;


    lpmdi->lpDebug = (LPDEBUGDATA)SHGetDebugData( (HIND)(lpremi->hemi) );

    // Get the GSN info table from the symbol handler
    if ( lpmdi->lpgsi = (LPGSI)SHLpGSNGetTable( (HIND)(lpremi->hemi) ) ) {

#ifdef _DEBUG
	// perform some validation on the segmap as best we can
	// check segnums are OK, and limit sections
	assert( lpmdi->lpgsi->csgMax >= lpmdi->lpgsi->csgLogical );
	assert( lpmdi->lpgsi->csgMax < 500 );
	assert( lpmdi->lpgsi->csgMax !=0 );

	if ( lpmdi->lpgsi->csgMax != lpmdi->lpgsi->csgLogical )
		assert( !"Who put a Group in a segmap?" );			// groups are 16-bit legacy crap

	for (int i=0; i<lpmdi->lpgsi->csgMax; i++)
	{
		assert( lpmdi->lpgsi->rgsgi[i].igr==0 );			// no Groups
		assert( lpmdi->lpgsi->rgsgi[i].iovl==0 );			// no Overlays

		if (lpmdi->lpgsi->rgsgi[i].sgf.u.u1.fSel)			// if Selector
		{
			// check for valid segment numbers
			assert( lpmdi->lpgsi->rgsgi[i].isgPhy>0);
		}
		else if (lpmdi->lpgsi->rgsgi[i].sgf.u.u1.fGroup)
		{
			// groups are obsolete
			assert( !"segment marked as Group was found" );		// strange?
		}
		else
		{
			// Absolutes are OK, but nothing else is
			assert( lpmdi->lpgsi->rgsgi[i].sgf.u.u1.fAbs);	// if not Select, must be Abs
		}
	}
#endif

	//
	//  If real mode, do some patch magic.
	//
	if ( lpmdi->fRealMode ) {

	    int i;

	    lpmdi->cobj   = lpmdi->lpgsi->csgMax+1;
	    lpmdi->rgobjd = (OBJD *) MHRealloc(lpmdi->rgobjd,
				      sizeof(OBJD)*lpmdi->cobj);
	    memset(lpmdi->rgobjd, 0, sizeof(OBJD)*(lpmdi->cobj));

	    lpsgi    = lpmdi->lpgsi->rgsgi;
	    lpsgiMax = lpsgi + lpmdi->lpgsi->csgMax;

	    for( i=0; lpsgi < lpsgiMax; lpsgi++, i++ ) {

		lpmdi->rgobjd[ i ].wSel = (WORD)(lpsgi->doffseg + lpmdi->StartingSegment);
		lpmdi->rgobjd[ i ].wPad = 1;
		lpmdi->rgobjd[ i ].cb   = (DWORD) -1;

		lpsgi->doffseg = 0;

	    }
	}

	// Determine if child is overlaid and, if so, how many overlays
	lpsgi = lpmdi->lpgsi->rgsgi;
	lpsgiMax = lpsgi + lpmdi->lpgsi->csgMax;
	for( ; lpsgi < lpsgiMax; lpsgi++ ) {

	    // iovl == 0xFF is reserved, it means no overlay specified.
	    // we should ignore 0xFF in iovl.  Linker uses it
	    // to (insert lots of hand-waving here) support COMDATS
	    if ( lpsgi->iovl < 0xFF ) {                             // [02]
		usOvlMax = max( usOvlMax, lpsgi->iovl );
	    }
	}
#ifndef TARGET32
	// Setup the overlay table
	if ( usOvlMax ) {
	    lpmdi->lpsel = MHRealloc( lpmdi->lpsel, sizeof( WORD ) * usOvlMax + 1 );
	    _fmemset( &lpmdi->lpsel [ 1 ], 0, sizeof( WORD ) * usOvlMax );
	}
#endif // !TARGET32
    }
	// SHGetPublicAddr will use emi of out parameter to specify
	// which exe/dll to search
	emiAddr( addr ) = lpremi->hemi;
	memset(&lpmdi->nlg, 0, sizeof(lpmdi->nlg));
	if ( SHGetPublicAddr ( &addr, "__NLG_Dispatch")) {
		FixupAddr( hpid, htid, &addr );
		lpmdi->nlg.addrNLGDispatch = addr;
		emiAddr( addr ) = lpremi->hemi;
		if ( SHGetPublicAddr ( &addr, "__NLG_Destination")) {
			FixupAddr( hpid, htid, &addr );
			lpmdi->nlg.addrNLGDestination = addr;
			emiAddr( addr ) = lpremi->hemi;
			lpmdi->fSendNLG = TRUE;
			lpmdi->nlg.fEnable = TRUE;
			lpmdi->nlg.hemi = lpmdi->hemi;
			if ( SHGetPublicAddr ( &addr, "__NLG_Return")) {
				FixupAddr( hpid, htid, &addr );
				lpmdi->nlg.addrNLGReturn = addr;
				emiAddr( addr ) = lpremi->hemi;
			}

			if ( SHGetPublicAddr ( &addr, "__NLG_Return2")) {
			     FixupAddr( hpid, htid, &addr );
			     lpmdi->nlg.addrNLGReturn2 = addr;
			}
		}
	}

	LLUnlock ( hmdi );

    // purge the emi cache (get rid of old, now invalid hpid/emi pairs)
    CleanCacheOfEmi();
}


void
UpdateProcess (
    HPRC hprc
    )
{
    assert ( hprc != NULL );
    EnterCriticalSection(&csCache);

    {
	LPPRC lpprc = (LPPRC) LLLock ( hprc );

	FlushPTCache();

	hprcCurr = hprc;
	hpidCurr = lpprc->hpid;
	pidCurr  = lpprc->pid;
	pointersCurr = PointersFromMPT(lpprc->dmi.Processor.Type);
	mptCurr = lpprc->dmi.Processor.Type;

	LLUnlock ( hprc );
    }
    LeaveCriticalSection(&csCache);
}


void
UpdateThread (
    HTHD hthd
    )
{
    EnterCriticalSection(&csCache);

    if ( hthd == NULL ) {
	FlushPTCache();
    } else {
	LPTHD lpthd = (LPTHD) LLLock ( hthd );

	UpdateProcess ( lpthd->hprc );

	hthdCurr = hthd;
	htidCurr = lpthd->htid;
	tidCurr  = lpthd->tid;

	LLUnlock ( hthd );
    }
    LeaveCriticalSection(&csCache);
}



HEMI
HemiFromHmdi (
    HMDI hmdi
    )
{
    LPMDI lpmdi = (LPMDI) LLLock ( hmdi );
    HEMI  hemi = lpmdi->hemi;

    LLUnlock ( hmdi );
    return hemi;
}


void
FlushPTCache (
    void
    )
{
    EnterCriticalSection(&csCache);

    hprcCurr = NULL;
    hpidCurr = NULL;
    pidCurr  = 0;
    pointersCurr = NULL;
    mptCurr = (MPT)-1;

    hthdCurr = NULL;
    htidCurr = NULL;
    tidCurr =  0;

    LeaveCriticalSection(&csCache);
}


EMEXPORT HPRC
ValidHprcFromHpid(
    HPID hpid
    )
/*++

Routine Description:

    only return an hprc if there is a real process for it.
    the other version will return an hprc whose process has
    not been created or has been destroyed.

Arguments:

    hpid  - Supplies hpid to look for in HPRC list.

Return Value:

    An HPRC or NULL.

--*/
{
    HPRC hprcT;
    HPRC hprc = NULL;
    LPPRC lpprc;

    EnterCriticalSection(&csCache);

    if ( hpid == hpidCurr ) {

	hprc = hprcCurr;

    } else {

	if ( hpid != NULL ) {
	    hprc = LLFind ( llprc, NULL, (LPVOID)&hpid, 0 );
	}

	if ( hprc != NULL ) {
	    lpprc = (LPPRC) LLLock( hprcT = hprc );
	    if (lpprc->stat == statDead) {
		hprc = NULL;
	    }
	    LLUnlock( hprcT );
	}
	if ( hprc != NULL ) {
	    UpdateProcess ( hprc );
	}
    }

    LeaveCriticalSection(&csCache);

    return hprc;
}


HPRC
HprcFromHpid (
    HPID hpid
    )
{
    HPRC hprc = NULL;

    EnterCriticalSection(&csCache);

    if ( hpid == hpidCurr ) {

	hprc = hprcCurr;

    } else {

	if ( hpid != NULL ) {
	    hprc = LLFind ( llprc, NULL, (LPVOID)&hpid, 0 );
	}

	if ( hprc != NULL ) {
	    UpdateProcess ( hprc );
	}
    }

    LeaveCriticalSection(&csCache);

    return hprc;
}


EMEXPORT HPRC
HprcFromPid (
    PID pid
    )
{
    HPRC hprc;

    EnterCriticalSection(&csCache);

    if ( pid == pidCurr ) {

	hprc = hprcCurr;

    } else {

	for ( hprc = LLNext ( llprc, 0 );
	      hprc != 0;
	      hprc = LLNext ( llprc, hprc ) ) {

	    LPPRC lpprc = (LPPRC) LLLock ( hprc );
	    if ( lpprc->pid == pid )
		break;
	    LLUnlock ( hprc );
	}

	if ( hprc) {
	    LLUnlock ( hprc );
	    UpdateProcess ( hprc );
	}
    }

    LeaveCriticalSection(&csCache);

    return hprc;
}


EMEXPORT HPID
HpidFromHprc (
    HPRC hprc
    )
{
    HPID hpid = NULL;

    EnterCriticalSection(&csCache);

    if ( hprc == hprcCurr ) {
	hpid = hpidCurr;
    } else if ( hprc != NULL ) {
	UpdateProcess ( hprc );
	hpid = hpidCurr;
    }

    LeaveCriticalSection(&csCache);

    return hpid;
}


PID
PidFromHprc (
    HPRC hprc
    )
{
    PID pid = 0;

    EnterCriticalSection(&csCache);

    if ( hprc == hprcCurr ) {
	pid = pidCurr;
    } else if ( hprc != NULL ) {
	UpdateProcess ( hprc );
	pid = pidCurr;
    }

    LeaveCriticalSection(&csCache);

    return pid;
}


EMEXPORT HTHD
HthdFromTid (
    HPRC hprc,
    TID tid
    )
{
    LPPRC lpprc;
    HTHD  hthd = NULL;

    EnterCriticalSection(&csCache);

    if ( hprc == hprcCurr && tid == tidCurr ) {
	hthd = hthdCurr;
    } else {
	lpprc = (LPPRC) LLLock ( hprc );

	for ( hthd = LLNext ( lpprc->llthd, 0 );
	      hthd != 0;
	      hthd = LLNext ( lpprc->llthd, hthd ) ) {

	    LPTHD lpthd = (LPTHD) LLLock ( hthd );
	    if ( lpthd->tid == tid )
		break;

	    LLUnlock ( hthd );
	}

	LLUnlock ( hprc );

	if ( hthd ) {
	    LLUnlock ( hthd );
	    UpdateThread ( hthd );
	} else {
	    hthd = NULL;
	}
    }

    LeaveCriticalSection(&csCache);

    return hthd;
}


EMEXPORT HTHD
HthdFromHtid (
    HPRC hprc,
    HTID htid
    )
{
    HTHD  hthd = NULL;

    EnterCriticalSection(&csCache);

    if ((int)htid & 1) {// HACK around vhtid
       htid = (HTID) (((int)htid) ^ 1);
    }
    if ( hprc == hprcCurr && htid == htidCurr ) {
	hthd = hthdCurr;
    } else if ( hprc != NULL ) {
	LPPRC lpprc = (LPPRC) LLLock ( hprc );
	hthd  = LLFind ( lpprc->llthd, NULL, (LPVOID)&htid, 0 );
	LLUnlock ( hprc );
    }
    UpdateThread ( hthd );

    LeaveCriticalSection(&csCache);

    return hthd;
}


EMEXPORT HTID
HtidFromHthd (
    HTHD hthd
    )
{
    HTID htid = NULL;

    EnterCriticalSection(&csCache);

    if ( hthd != hthdCurr ) {
	UpdateThread ( hthd );
    }
    htid = htidCurr;

    LeaveCriticalSection(&csCache);

    return htid;
}


TID
TidFromHthd (
    HTHD hthd
    )
{
    TID tid = 0;

    EnterCriticalSection(&csCache);

    if ( hthd != hthdCurr ) {
	UpdateThread ( hthd );
    }
    tid = tidCurr;

    LeaveCriticalSection(&csCache);

    return tid;
}

EMEXPORT HPID
HpidFromHthd(
    HTHD hthd
    )
{
    HPID hpid = NULL;

    EnterCriticalSection(&csCache);

    if (hthd != hthdCurr) {
	UpdateThread( hthd );
    }
    hpid = hpidCurr;

    LeaveCriticalSection(&csCache);

    return hpid;
}

HLLI
LlthdFromHprc (
    HPRC hprc
    )
{
    HLLI llthd = 0;

    if ( hprc != NULL ) {
	LPPRC lpprc = (LPPRC) LLLock ( hprc );
	llthd = lpprc->llthd;
	LLUnlock ( hprc );
    }

    return llthd;
}

HLLI
LlmdiFromHprc (
    HPRC hprc
    )
{
    HLLI llmdi = 0;

    if ( hprc != NULL ) {
	LPPRC lpprc = (LPPRC) LLLock ( hprc );
	llmdi = lpprc->llmdi;
	LLUnlock ( hprc );
    }

    return llmdi;
}

STAT
StatFromHprc (
    HPRC hprc
    )
{
    LPPRC lpprc = (LPPRC) LLLock ( hprc );
    STAT  stat  = lpprc->stat;
    LLUnlock ( hprc );
    return stat;
}

MPT
MPTFromHthd(
    HTHD hthd
    )
{
    MPT m = (MPT)-1;
    EnterCriticalSection(&csCache);
    if (hthd != hthdCurr) {
	UpdateThread(hthd);
    }
    m = mptCurr;
    LeaveCriticalSection(&csCache);
    return m;
}

MPT
MPTFromHprc(
    HPRC hprc
    )
{
    MPT m = (MPT)-1;
    EnterCriticalSection(&csCache);
    if (hprc != hprcCurr) {
	UpdateProcess(hprc);
    }
    m = mptCurr;
    LeaveCriticalSection(&csCache);
    return m;
}


PCPU_POINTERS
PointersFromMPT(
    MPT mpt
    )
{
    //_asm int 3
	extern CPU_POINTERS X86Pointers;
//	extern CPU_POINTERS PpcPointers;
//	extern CPU_POINTERS MipsPointers;

	//
	// NOTE: MIPS and PPC are no longer supported.
	//

	switch (mpt) {
	case mptix86:
	    return &X86Pointers;

	default:
	    return NULL;
    }
}

PCPU_POINTERS
PointersFromHpid(
    HPID hpid
    )
{
    PCPU_POINTERS p = NULL;

    EnterCriticalSection(&csCache);

    if (hpid == hpidCurr || ValidHprcFromHpid(hpid)) {
	p = pointersCurr;
    }

    LeaveCriticalSection(&csCache);

    return p;
}

//**************************************************************************
//
// global stack walking api support functions
//
// these are the callbacks used by imagehlp.dll
//
// there are custom callbacks in each of the emdpdev.c files
//
//**************************************************************************


BOOL
SwReadMemory(
    LPVOID  lpvhpid,
    LPCVOID lpBaseAddress,
    LPVOID  lpBuffer,
    DWORD   nSize,
    LPDWORD lpNumberOfBytesRead
    )
{
    ADDR   addr;
    DWORD  cb;
    XOSD   xosd;
    HPID   hpid = (HPID)lpvhpid;

    addr.addr.off     = (OFFSET)lpBaseAddress;
    addr.addr.seg     = 0;
    addr.emi          = 0;
    addr.mode.fFlat   = TRUE;
    addr.mode.fOff32  = FALSE;
    addr.mode.fIsLI   = FALSE;
    addr.mode.fReal   = FALSE;

    xosd = ReadBuffer( hpid, NULL, &addr, nSize, (LPBYTE) lpBuffer, &cb );
    if (xosd != xosdNone) {
	return FALSE;
    }

    if (lpNumberOfBytesRead) {
	*lpNumberOfBytesRead = cb;
    }

    return TRUE;
}


HMDI
SwGetMdi(
    HPID    hpid,
    DWORD   Address
    )
{
    HLLI        hlli  = 0;
    HMDI        hmdi  = 0;
    LPMDI       lpmdi = NULL;


    hlli = LlmdiFromHprc( HprcFromHpid ( hpid ));

    do {

	hmdi = LLNext( hlli, hmdi );
	if (hmdi) {
	    lpmdi = (LPMDI)LLLock( hmdi );
	    if (lpmdi) {
		//
		// we have a pointer to a module so lets see if its the one...
		//
		if (Address >= lpmdi->lpBaseOfDll &&
		    Address <  lpmdi->lpBaseOfDll+lpmdi->dwSizeOfDll ) {

		    LLUnlock( hmdi );
		    return hmdi;

		}
		LLUnlock( hmdi );
	    }
	}

    } while (hmdi);

    return 0;
}


DWORD
SwGetModuleBase(
    LPVOID  lpvhpid,
    DWORD   ReturnAddress
    )
{
    HMDI        hmdi  = 0;
    LPMDI       lpmdi = NULL;
    HPID        hpid = (HPID)lpvhpid;
    DWORD state;

    hmdi = SwGetMdi( hpid, ReturnAddress );
    if (hmdi) {
		lpmdi = (LPMDI) LLLock( hmdi );
		if (lpmdi) {
		LLUnlock( hmdi );
		return lpmdi->lpBaseOfDll;
		}
    }

	// [apennell] VS98:6541 (and others)
	// returning zero here causes premature truncation of the callstack, so
	// if we like the look of the address we return the exe start address
	// even if it doesn't lie in a recognised code area

	if (ReturnAddress < 0x00010000)
		return 0;							// all below 64k on both OSes are illegal

	// is it readable memory? Use VirtualQueryEx, via the DM
	ADDR addr = {0};
	SetAddrOff( &addr, ReturnAddress );
	ADDRLIN32( addr );

    GuardTL();
	XOSD xosd = SendRequestX( dmfVirtualQuery, hpid, 0, sizeof(ADDR),
                                                              (LPVOID)&addr );
    if (xosd != xosdNone) {
        ReleaseTL();
		return 0;
    }

	PMEMORY_BASIC_INFORMATION lpmbi = (PMEMORY_BASIC_INFORMATION) LpDmMsg->rgb;
    state = lpmbi->State;
    ReleaseTL();

	if (state!=MEM_COMMIT)
		return 0;

	// lets return the start of the exe, which is the first mdi in the list
    HLLI        hlli  = 0;

    hlli = LlmdiFromHprc( HprcFromHpid ( hpid ));

	hmdi = LLNext( hlli, hmdi );
	if (!hmdi)
		return 0;

	lpmdi = (LPMDI)LLLock( hmdi );
	if (!lpmdi)
		return 0;

	return lpmdi->lpBaseOfDll;
}

// TLS
#if 0
XOSD
InitTLS (
    HPRC hprc,
    HTHD hthd
    )
{
    LPPRC   lpprc = (LPPRC) LLLock ( hprc );
    LPTHD   lpthd = (LPTHD) LLLock ( hthd );
    ADDR    addr  = {0};
    UINT    cb    = 0;
    XOSD    xosd  = xosdNone;

    if ( lpthd->uoffTEB == 0 ) {
	xosd = xosdBadThread;
    }
    else {

	GetAddrOff ( addr ) = lpthd->uoffTEB;
	ADDR_IS_OFF32( addr ) = TRUE;
	ADDR_IS_FLAT ( addr ) = TRUE;

	ReadBuffer ( lpprc->hpid, lpthd->htid, &addr, sizeof ( UOFFSET ), (LPBYTE) &GetAddrOff ( addr ), (LPDWORD) &cb);

	if ( cb != sizeof ( UOFFSET ) ) {
	    xosd = xosdRead;
	}
    }

    if ( xosd == xosdNone && GetAddrOff ( addr ) != 0 ) {
	lpthd->rguoffTlsBase = (UOFFSET *) MHAlloc ( lpprc->cmdlTLS * sizeof ( UOFFSET ) );

	ReadBuffer ( lpprc->hpid, lpthd->htid, &addr, lpprc->cmdlTLS * sizeof ( UOFFSET ), (LPBYTE)lpthd->rguoffTlsBase, (LPDWORD) &cb);

	if ( cb != lpprc->cmdlTLS * sizeof ( UOFFSET ) ) {
	    xosd = xosdRead;
	}
    }

    LLUnlock ( hprc );
    LLUnlock ( hthd );

    return xosd;
}

XOSD
ReadTlsIndex(
    HPRC hprc,
    LPADDR lpAddr,
    DWORD FAR * lpiTls,
    UTLSCACHE *puTlsCache
    )
{
    LPPRC pprc = (LPPRC) LLLock( hprc );
    DWORD iTls;
    DWORD cb;
    XOSD xosd = xosdNone;

    assert(hprc != hprcInvalid && hprc != hprcNull);
    assert(!ADDR_IS_LI(*lpAddr));

    if ( pprc->stat == statRunning ) {
	    // Always fetch a new value if we are running.
	    xosd = ReadBuffer(pprc->hpid, NULL, lpAddr, sizeof (iTls), (LPBYTE)&iTls, &cb);
        puTlsCache->fValid = FALSE;

	    // This is not required but invalidate the tls cache anyway.
	    pprc->iTlsCache.fValid = FALSE;
    }
    else {

	    // If the cache is not valid or the addresses don't match fetch a new value.
	    if ( !pprc->iTlsCache.fValid || !FAddrsEq(*lpAddr, pprc->iTlsCache.addr) ) {
	        xosd = ReadBuffer(pprc->hpid, NULL, lpAddr, sizeof( iTls), (LPBYTE)&(pprc->iTlsCache.iTls), &cb);
	        if ( xosd == xosdNone && cb == sizeof(iTls) ) {
		        pprc->iTlsCache.addr = *lpAddr ;
		        pprc->iTlsCache.fValid = TRUE;
                puTlsCache->fValid = FALSE;
            } else {
    	        pprc->iTlsCache.fValid = FALSE;
	        }
    	}
    	iTls =  pprc->iTlsCache.iTls;
    }

    LLUnlock( hprc );

    if(iTls != *lpiTls)
        puTlsCache->fValid = FALSE;
    *lpiTls = iTls;
    return xosd;
}
#endif

UOFFSET
GetTlsBase (
    HPRC hprc,
    HTHD hthd,
    LPMDI lpmdi
    )
{
    LPTHD   lpthd = (LPTHD) LLLock ( hthd );
    UOFFSET uoffRet = 0;
    LPPRC   lpprc = (LPPRC) LLLock ( hprc );
    ADDR    addr = {0};
    XOSD xosd;
    DWORD cb;

    // freshen the iTls value with the value from the data section
    assert ( lpthd->uoffTEB );

#if 0
    GetAddrOff ( addr ) = lpmdi->uoffiTls;
    GetAddrSeg ( addr ) = 0; //lpthd->regx86.ds;
    emiAddr ( addr ) = 0;
    ADDR_IS_OFF32 ( addr ) = TRUE;
    ADDR_IS_FLAT( addr ) = TRUE;
    xosd = ReadTlsIndex ( hprc, &addr, &lpmdi->iTlsIndex, &lpthd->uTlsCache);
#endif

    GetAddrOff(addr) = lpthd->uoffTEB;
    ADDR_IS_FLAT(addr) = TRUE;
    xosd = ReadBuffer(lpprc->hpid, lpthd->htid, &addr, sizeof(UOFFSET),
        (LPBYTE)&lpthd->uTlsCache.uoffTls, &cb);
    if(xosd == xosdNone && cb == sizeof(UOFFSET))
	    uoffRet = lpthd->uTlsCache.uoffTls;

    LLUnlock ( hprc );
    LLUnlock ( hthd );

    return uoffRet;
}



XOSD
GetFrameRegValue (
    HPID hpid,
    HTID htid,
    DWORD ireg,
    LPVOID lpvRegValue
    )
{
    LPTHD       lpthd;
    HTHD hthd = HthdFromHtid(HprcFromHpid(hpid), htid);

    assert ( hthd != hthdNull );

    lpthd = (LPTHD) LLLock ( hthd );

    lpvRegValue = DoGetReg(hpid, lpthd->frameRegs, ireg & 0xff, lpvRegValue );

    if ( lpvRegValue != NULL ) {
	ireg = ireg >> 8;
	if ( ireg != CV_REG_NONE ) {
	    lpvRegValue = DoGetReg( hpid, lpthd->frameRegs, ireg, lpvRegValue );
	}
    }

    LLUnlock ( hthd );

    if ( lpvRegValue == NULL ) {
	return xosdInvalidParameter;
    }

    return xosdNone;
}                             /* GetFrameRegValue */




XOSD
XXSetFlagValue (
    HPID   hpid,
    HTID   htid,
    DWORD  iFlag,
    LPVOID lpvRegValue
    )
{
    HPRC        hprc;
    HTHD        hthd;
    LPTHD       lpthd;
    LPVOID      lpregs;
    LONG        mask;
    LONG        l;

    hprc = ValidHprcFromHpid(hpid);
    if (!hprc) {
	return xosdBadProcess;
    }
    hthd = HthdFromHtid(hprc, htid);
    assert ( hthd != hthdNull );

    lpthd = (LPTHD) LLLock( hthd );

    lpregs = lpthd->regs;

    if ( !( lpthd->drt & drtAllPresent )) {
	UpdateRegisters ( lpthd->hprc, hthd );
    }


    if ( DoGetReg( hpid, lpregs, Rgfd(hpid)[iFlag].fd.dwId, &l ) == NULL) {
	LLUnlock( hthd );
	return xosdInvalidParameter;
    }

    mask = (1 << Rgfd(hpid)[iFlag].fd.dwcbits) - 1;
    mask <<= Rgfd(hpid)[iFlag].iShift;
    l &= ~mask;
    l |= ((*((ULONG FAR *) lpvRegValue)) << Rgfd(hpid)[iFlag].iShift) & mask;
    DoSetReg(hpid, lpregs, Rgfd(hpid)[iFlag].fd.dwId, &l );

    lpthd->drt = (DRT) (lpthd->drt | drtAllDirty);
    LLUnlock ( hthd );
    return xosdNone;
}                             /* SetFlagValue */


XOSD
XXGetFlagValue (
    HPID hpid,
    HTID htid,
    DWORD iFlag,
    LPVOID lpvRegValue
    )
{
    HPRC      hprc;
    HTHD      hthd;
    LPTHD     lpthd;
    LPCONTEXT lpregs;
    DWORD     value;

    hprc = ValidHprcFromHpid(hpid);
    if (!hprc) {
	return xosdBadProcess;
    }
    hthd = HthdFromHtid(hprc, htid);

    assert ( hthd != NULL );
    lpthd = (LPTHD) LLLock ( hthd );

    lpregs = (LPCONTEXT) lpthd->regs;

    if ( !(lpthd->drt & drtAllPresent) ) {
	UpdateRegisters ( hprc, hthd );
    }

    if (DoGetReg ( hpid, lpregs, Rgfd(hpid)[iFlag].fd.dwId, &value ) == NULL) {
	LLUnlock( hthd );
	return xosdInvalidParameter;
    }

    value = (value >> Rgfd(hpid)[iFlag].iShift) & ((1 << Rgfd(hpid)[iFlag].fd.dwcbits) - 1);
    *( (LPLONG) lpvRegValue) = value;

    LLUnlock(hthd);
    return xosdNone;
}




DWORD
ConvertOmapFromSrc(
    LPMDI       lpmdi,
    DWORD       addr
    )
{
    DWORD   rva;
    DWORD   comap;
    LPOMAP  pomapLow;
    LPOMAP  pomapHigh;
    DWORD   comapHalf;
    LPOMAP  pomapMid;


    if (!lpmdi) {
	return addr;
    }

    if ( lpmdi->lpgsi == NULL ) {
	SHWantSymbols( (HEXE)lpmdi->hemi );
	lpmdi->lpgsi = (LPGSI)SHLpGSNGetTable( (HEXE)lpmdi->hemi );
    }

    if ((!lpmdi->lpDebug) || (!lpmdi->lpDebug->lpOmapFrom)) {
	return addr;
    }

    rva = addr - lpmdi->lpBaseOfDll;

    comap = lpmdi->lpDebug->cOmapFrom;
    pomapLow = lpmdi->lpDebug->lpOmapFrom;
    pomapHigh = pomapLow + comap;

    while (pomapLow < pomapHigh) {

	comapHalf = comap / 2;

	pomapMid = pomapLow + ((comap & 1) ? comapHalf : (comapHalf - 1));

	if (rva == pomapMid->rva) {
	    return lpmdi->lpBaseOfDll + pomapMid->rvaTo;
	}

	if (rva < pomapMid->rva) {
	    pomapHigh = pomapMid;
	    comap = (comap & 1) ? comapHalf : (comapHalf - 1);
	} else {
	    pomapLow = pomapMid + 1;
	    comap = comapHalf;
	}
    }

    assert(pomapLow == pomapHigh);

    //
    // If no exact match, pomapLow points to the next higher address
    //
    if (pomapLow == lpmdi->lpDebug->lpOmapFrom) {
	//
	// This address was not found
	//
	return 0;
    }

    if (pomapLow[-1].rvaTo == 0) {
	//
	// This address is not translated so just return the original
	//
	return addr;
    }

    //
    // Return the closest address plus the bias
    //
    return lpmdi->lpBaseOfDll + pomapLow[-1].rvaTo + (rva - pomapLow[-1].rva);
}


DWORD
ConvertOmapToSrc(
    LPMDI       lpmdi,
    DWORD       addr
    )
{
    DWORD   rva;
    DWORD   comap;
    LPOMAP  pomapLow;
    LPOMAP  pomapHigh;
    DWORD   comapHalf;
    LPOMAP  pomapMid;
    INT     i;


    if (!lpmdi) {
	return addr;
    }

    if ( lpmdi->lpgsi == NULL ) {
	SHWantSymbols( (HEXE)lpmdi->hemi );
	lpmdi->lpgsi = (LPGSI)SHLpGSNGetTable( (HEXE)lpmdi->hemi );
    }

    if ((!lpmdi->lpDebug) || (!lpmdi->lpDebug->lpOmapTo)) {
	return addr;
    }

    rva = addr - lpmdi->lpBaseOfDll;

    comap = lpmdi->lpDebug->cOmapTo;
    pomapLow = lpmdi->lpDebug->lpOmapTo;
    pomapHigh = pomapLow + comap;

    while (pomapLow < pomapHigh) {

	comapHalf = comap / 2;

	pomapMid = pomapLow + ((comap & 1) ? comapHalf : (comapHalf - 1));

	if (rva == pomapMid->rva) {
	    if (pomapMid->rvaTo == 0) {
		//
		// We are probably in the middle of a routine
		//
		i = -1;
		while ((&pomapMid[i] != lpmdi->lpDebug->lpOmapTo) && pomapMid[i].rvaTo == 0) {
		    //
		    // Keep on looping back until the beginning
		    //
		    i--;
		}
		return lpmdi->lpBaseOfDll + pomapMid[i].rvaTo;
	    } else {
		return lpmdi->lpBaseOfDll + pomapMid->rvaTo;
	    }
	}

	if (rva < pomapMid->rva) {
	    pomapHigh = pomapMid;
	    comap = (comap & 1) ? comapHalf : (comapHalf - 1);
	} else {
	    pomapLow = pomapMid + 1;
	    comap = comapHalf;
	}
    }

    assert(pomapLow == pomapHigh);

    //
    // If no exact match, pomapLow points to the next higher address
    //
    if (pomapLow == lpmdi->lpDebug->lpOmapTo) {
	//
	// This address was not found
	//
	return 0;
    }

    if (pomapLow[-1].rvaTo == 0) {
	return 0;
    }

    //
    // Return the new address plus the bias
    //
    return lpmdi->lpBaseOfDll + pomapLow[-1].rvaTo + (rva - pomapLow[-1].rva);
}

