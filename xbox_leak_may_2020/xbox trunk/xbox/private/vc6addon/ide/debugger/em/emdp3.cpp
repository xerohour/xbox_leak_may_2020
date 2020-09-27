/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    emdp3.c

Abstract:

    This file contains the some of the machine independent portions of the
    execution model.  The machine dependent portions are in other files.

Author:

    Kent Forschmiedt (kentf) 11-8-93

Environment:

    Win32 -- User

Notes:

    The orginal source for this came from the CodeView group.

--*/

#include "emdp.h"

#include "fbrdbg.h"


//
// This list is only used when there is no DM present.  Whenever an
// hpid is created, the real list is obtained from the DM and stored
// in a list bound to the hprc.
//
static EXCEPTION_DESCRIPTION DefaultExceptionList[] = {
    {EXCEPTION_ACCESS_VIOLATION,        efdStop,  _T("Access Violation")},
    {EXCEPTION_ARRAY_BOUNDS_EXCEEDED,   efdStop,  _T("Array Bounds Exceeded")},
    {EXCEPTION_FLT_DENORMAL_OPERAND,    efdStop,  _T("FP Denormal Operand")},
    {EXCEPTION_FLT_DIVIDE_BY_ZERO,      efdStop,  _T("FP Divide by Zero")},
    {EXCEPTION_FLT_INEXACT_RESULT,      efdStop,  _T("FP Inexact Result")},
    {EXCEPTION_FLT_INVALID_OPERATION,   efdStop,  _T("FP Invalid Operation")},
    {EXCEPTION_FLT_OVERFLOW,            efdStop,  _T("FP Overflow")},
    {EXCEPTION_FLT_STACK_CHECK,         efdStop,  _T("FP Stack Check")},
    {EXCEPTION_FLT_UNDERFLOW,           efdStop,  _T("FP Underflow")},
    {EXCEPTION_INT_DIVIDE_BY_ZERO,      efdStop,  _T("Int Divide by zero")},
    {EXCEPTION_INT_OVERFLOW,            efdStop,  _T("Int Overflow")},
    {EXCEPTION_PRIV_INSTRUCTION,        efdStop,  _T("Insufficient Privilege")},
    {EXCEPTION_IN_PAGE_ERROR,           efdStop,  _T("I/O Error in Paging")},
    {EXCEPTION_ILLEGAL_INSTRUCTION,     efdStop,  _T("Illegal Instruction")},
    {EXCEPTION_NONCONTINUABLE_EXCEPTION,efdStop,  _T("Noncontinuable Exception")},
    {EXCEPTION_STACK_OVERFLOW,          efdStop,  _T("Stack Overflow")},
    {EXCEPTION_INVALID_DISPOSITION,     efdStop,  _T("Invalid Disposition")},
    {DBG_CONTROL_C,                     efdStop,  _T("Control-C break")},
};


#define DECL_MASK(n,v,s) n = v,
#define DECL_MSG(n,s,m)

enum {
#include "win32msg.h"
};

#undef DECL_MASK
#define DECL_MASK(n,v,s) { n, _T(s) },

MASKINFO MaskInfo[] = {
#include "win32msg.h"
};

#define MASKMAPSIZE (sizeof(MaskInfo)/sizeof(MASKINFO))
MASKMAP MaskMap = {MASKMAPSIZE, MaskInfo};

#undef DECL_MASK
#undef DECL_MSG


#define DECL_MASK(n,v,s)
#define DECL_MSG(n,s,m) { n, _T(s), m },

MESSAGEINFO MessageInfo[] = {
#include "win32msg.h"
};

#define MESSAGEMAPSIZE (sizeof(MessageInfo)/sizeof(MESSAGEINFO))
MESSAGEMAP MessageMap = {MESSAGEMAPSIZE,MessageInfo};

#undef DECL_MASK
#undef DECL_MSG




XOSD
HandleBreakpoints(
    HPID hpid,
    DWORD wValue,
    LONG lValue
    )
/*++

Routine Description:


Arguments:


Return Value:


--*/
{
    LPBPS lpbps = (LPBPS) lValue;
    LPDBB lpdbb = (LPDBB) MHAlloc(FIELD_OFFSET(DBB, rgbVar) + wValue);
    XOSD xosd;

    // let the DM handle everything?
    lpdbb->hpid = hpid;
    lpdbb->htid = NULL;
    lpdbb->dmf  = dmfBreakpoint;
    memcpy(lpdbb->rgbVar, lpbps, wValue);
    GuardTL();
    CallTL ( tlfRequest, hpid, FIELD_OFFSET ( DBB, rgbVar ) + wValue, (LPVOID)lpdbb );
    MHFree(lpdbb);
    xosd = LpDmMsg->xosdRet;
    ReleaseTL();
    return xosd;
}


XOSD
Go (
    HPID hpid,
    HTID htid,
    LPEXOP lpexop
    )
{
    UpdateChild( hpid, htid, dmfGo );
    return SendRequestX(dmfGo, hpid, htid, sizeof(EXOP), lpexop);
}


XOSD
ReturnStep (
    HPID hpid,
    HTID htid,
    LPEXOP lpexop
    )
{
    RTRNSTP rtrnstp;
    XOSD xosd = xosdNone;
    HTID vhtid = htid;

    rtrnstp.exop = *lpexop;
    if ((((DWORD)htid) & 1) == 0) {
        xosd = GetFrame( hpid, vhtid, 1, (DWORD)&vhtid );
    }
    if ( xosd == xosdNone ) {
       xosd = GetFrame( hpid, vhtid, 1, (DWORD)&vhtid );
       if ( xosd == xosdNone ) {
          xosd = GetAddr( hpid, vhtid, adrPC, &(rtrnstp.addrRA) );
          if ( xosd == xosdNone ) {
              xosd = GetAddr( hpid, htid, adrStack, &(rtrnstp.addrStack) );
          }
       }
    }
    if ( xosd != xosdNone ) {
       return( xosd );
    }
    return SendRequestX ( dmfReturnStep, hpid, htid, sizeof(rtrnstp), &rtrnstp);

}

XOSD
ThreadStatus (
    HPID hpid,
    HTID htid,
    LPTST lptst
    )
{
    XOSD xosd;

    GuardTL();
    xosd = SendRequest ( dmfThreadStatus, hpid, htid );
    if (xosd == xosdNone) {
        xosd = LpDmMsg->xosdRet;
    }
    if (xosd == xosdNone) {
        memcpy(lptst, LpDmMsg->rgb, sizeof(TST));
    } else {
                HPRC hprc = ValidHprcFromHpid(hpid);
                if (hprc) {
                        HTHD hthd = HthdFromHtid(hprc, htid);
                        if (hthd) {
                                LPTHD lpthd = (LPTHD) LLLock(hthd);
                                lptst->dwThreadID = lpthd->tid;
                                LLUnlock(hthd);
                        }
                }
        }
    ReleaseTL();
    return xosd;
}


XOSD
ProcessStatus(
    HPID hpid,
    LPPST lppst
    )
{
    XOSD xosd;
    GuardTL();
    xosd = SendRequest(dmfProcessStatus, hpid, NULL );
    if (xosd == xosdNone) {
        xosd = LpDmMsg->xosdRet;
    }
    if (xosd == xosdNone) {
        memcpy(lppst, LpDmMsg->rgb, sizeof(PST));
    }
    ReleaseTL();
    return xosd;
}


XOSD
GetTimeStamp(
	HPID	hpid,
	HTID	htid,
	LPTCS	lptcs
	)
{
	XOSD	xosd;
	ULONG	len;
	LPTCSR	lptcsr;

	len = _tcslen (lptcs->ImageName) + 1;
	
    GuardTL();
	lptcsr = (LPTCSR) LpDmMsg->rgb;
	xosd = SendRequestX (dmfGetTimeStamp, hpid, htid, len, lptcs->ImageName);

	if (xosd == xosdNone) {
		xosd = LpDmMsg->xosdRet;
	}

	if (xosd == xosdNone) {
		lptcs->TimeStamp = lptcsr->TimeStamp;
		lptcs->CheckSum = lptcsr->CheckSum;
	}
    ReleaseTL();

	return xosd;
}


XOSD
EMCreateUserCrashDump(
	HPID	hpid,
	HTID	htid,
	LPTSTR	szCrashFileName
	)
{
	XOSD	xosd;
	ULONG	len;

	len = _tcslen (szCrashFileName) + 1;

    GuardTL();
	xosd = SendRequestX (dmfCreateUserCrashDump,
						 hpid,
						 htid,
						 len,
						 szCrashFileName);

	if (xosd == xosdNone) {
		xosd = LpDmMsg->xosdRet;
	}
    ReleaseTL();

	return xosd;
}
		
		

XOSD
Freeze (
    HPID hpid,
    HTID htid
    )
{
    XOSD xosd;
    HTHD hthd;
    HPRC hprc = ValidHprcFromHpid(hpid);
    if (!hprc) {
        return xosdBadProcess;
    }
    if ( (hthd = HthdFromHtid(hprc, htid)) == hthdInvalid || hthd == NULL ) {
        return xosdBadThread;
    }

    GuardTL();
    SendRequest ( dmfFreeze, hpid, htid);

    xosd = LpDmMsg->xosdRet;
    ReleaseTL();
    return xosd;
}


XOSD
Thaw (
    HPID hpid,
    HTID htid
    )
{
    XOSD xosd;
    HTHD hthd;
    HPRC hprc = ValidHprcFromHpid(hpid);
    if (!hprc) {
        return xosdBadProcess;
    }
    if ( (hthd = HthdFromHtid(hprc, htid)) == hthdInvalid || hthd == NULL ) {
        return xosdBadThread;
    }

    GuardTL();
    SendRequest ( dmfResume, hpid, htid);

    xosd = LpDmMsg->xosdRet;
    ReleaseTL();
    return xosd;
}


XOSD
DebugMetric (
    HPID hpid,
    HTID htid,
    MTRC mtrc,
    LPLONG lpl
    )
/*++

Routine Description:

    The debugger queries this function to find out the size of OS and machine
    dependent values, e.g. the size of a process ID.

Arguments:

    hpid

    htid

    mtrc   - metric identifier

    lpl    - answer buffer

Return Value:

    xosdNone if the request succeeded, xosd error code otherwise.

--*/
{
    HPRC hprc;
    HTHD hthd;
    LPPRC lpprc = NULL;
    XOSD xosd = xosdNone;


    hprc = HprcFromHpid(hpid);

    if (hprc) {

        lpprc = (LPPRC) LLLock( hprc );
        assert( lpprc );

        switch ( mtrc ) {

          default:
            break;

          case mtrcProcessorType:
          case mtrcProcessorLevel:
          case mtrcEndian:
          case mtrcThreads:
          case mtrcAsync:
          case mtrcAsyncStop:
          case mtrcBreakPoints:
          case mtrcReturnStep:
          case mtrcRemote:
          case mtrcOSVersion:
            if (!lpprc->fDmiCache) {
                GuardTL();
                xosd = SendRequest ( dmfGetDmInfo, hpid, htid );
                if (xosd == xosdNone) {
                    memcpy(&lpprc->dmi, LpDmMsg->rgb, sizeof(DMINFO));
                    lpprc->fDmiCache = TRUE;
                }
                ReleaseTL();
            }
            break;

        }

        if (xosd != xosdNone) {
            LLUnlock( hprc );
            return xosd;
        }


    }

    switch ( mtrc ) {

      default:
        assert(FALSE);
        xosd = xosdInvalidParameter;
        break;

      case mtrcProcessorType:

        assert(lpprc);
        *lpl = lpprc->dmi.Processor.Type;
        break;

      case mtrcProcessorLevel:

        assert(lpprc);
        *lpl = lpprc->dmi.Processor.Level;
        break;

      case mtrcEndian:

        assert(lpprc);
        *lpl = lpprc->dmi.Processor.Endian;
        break;

      case mtrcThreads:

        assert(lpprc);
        *lpl = lpprc->dmi.fHasThreads;
        break;

      case mtrcCRegs:

        *lpl = CRgrd(hpid);
        break;

      case mtrcCFlags:

        *lpl = CRgfd(hpid);
        break;

      case mtrcExtRegs:

        assert(0 && "do something with this");
        break;

      case mtrcExtFP:

        assert(0 && "do something with this");
        break;

      case mtrcExtMMU:

        assert(0 && "do something with this");
        break;

      case mtrcExceptionHandling:

        *( (LPDWORD) lpl) = TRUE;
        break;

      case mtrcAssembler:
#if 0
        switch(MPTFromHprc(hprc)) {
            case mptix86:
            case mptmips:
            case mptdaxp:
            case mptmppc:
            default:
                *( (LPDWORD) lpl) = FALSE;
                break;
        }
#else
        *( (LPDWORD) lpl) = FALSE;
#endif
        break;

      case mtrcAsync:

        assert(lpprc);
#if defined(DOLPHIN) // HACK!!! Need to get mtrc bits into od.h
        *(LPWORD)lpl = (WORD)lpprc->dmi.mAsync;
#else
        *lpl = !!lpprc->dmi.mAsync;
#endif
        break;

      case mtrcAsyncStop:

        assert(lpprc);
        *lpl = !!(lpprc->dmi.mAsync & asyncStop);
        break;

      case mtrcBreakPoints:

        assert(lpprc);
        //
        // Message BPs are implemented in the EM
        // on top of the exec BP implemented by the DM.
        //
        *lpl = lpprc->dmi.Breakpoints |
                bptsMessage |
                bptsMClass;
        break;

      case mtrcReturnStep:

        assert(lpprc);
        *lpl = lpprc->dmi.fReturnStep;
        break;

      case mtrcShowDebuggee:

        *lpl = FALSE;
        break;

      case mtrcHardSoftMode:

        *lpl = FALSE;
        break;

      case mtrcRemote:

        assert(lpprc);
        *lpl = lpprc->dmi.fRemote;
        break;

      case mtrcOleRpc:

        *lpl = TRUE;
        break;

      case mtrcNativeDebugger:

        *lpl = FALSE;
        break;

      case mtrcOSVersion:

        *lpl = (lpprc->dmi.MajorVersion << 16) | lpprc->dmi.MinorVersion;
        break;

      case mtrcMultInstances:

        *(BOOL*) lpl = TRUE;
        break;

      case mtrcTidValue:
        HTHD hthd = HthdFromHtid(hprc, htid);
        if (hthd) {
            LPTHD lpthd = (LPTHD) LLLock(hthd);
            *lpl = lpthd->tid;
            LLUnlock(hthd);
        } else {
            *lpl = 0;
        }
        break;
    }

    LLUnlock( hprc );

    return xosdNone;
}


XOSD
FakeGetExceptionState(
    EXCEPTION_CONTROL exc,
    LPEXCEPTION_DESCRIPTION lpexd
    )
/*++

Routine Description:

    Handle the GetExceptionState call when there is no DM connected.

Arguments:

    exc - Supplies exfFirst, exfSpecified or exfNext

    lpexd - Returns EXCEPTION_DESCRIPTION record

Return Value:

    xosdNone except when exc is exfNext and lpexd->dwExceptionCode
    was not in the list.

--*/
{
    DWORD dwT;
    int i;

    if (exc == exfFirst) {
        *lpexd = DefaultExceptionList[0];
        return xosdNone;
    }

    for (i = 0; DefaultExceptionList[i].dwExceptionCode != 0; i++) {
        if (DefaultExceptionList[i].dwExceptionCode == lpexd->dwExceptionCode) {
            break;
        }
    }

    if (exc == exfSpecified) {
        dwT = lpexd->dwExceptionCode;
        *lpexd = DefaultExceptionList[i];
        lpexd->dwExceptionCode = dwT;
        return xosdNone;
    }

    if (DefaultExceptionList[i].dwExceptionCode != 0) {
        *lpexd = DefaultExceptionList[++i];
        return xosdNone;
    }

    return xosdInvalidParameter;
}


XOSD
GetExceptionState(
    HPID hpid,
    HTID htid,
    EXCEPTION_CONTROL exc,
    LPEXCEPTION_DESCRIPTION lpexd
    )
{
    HPRC hprc;
    LPPRC lpprc;
    XOSD xosd = xosdNone;
    HEXD hexd;

    if (!hpid) {
        return FakeGetExceptionState(exc, lpexd);
    }

    hprc = HprcFromHpid( hpid );
    assert(hprc);
    lpprc = (LPPRC) LLLock(hprc);

    switch (exc) {

      default:
        assert( 0 && "Invalid arg to em!GetExceptionState" );
        xosd = xosdInvalidParameter;
        break;

      case exfFirst:

        hexd = LLNext( lpprc->llexc, NULL );
        if (!hexd) {
            // get the default exception record
            DWORD dwT = 0;
            hexd = LLFind( lpprc->llexc, NULL, &dwT, 0 );
        }
        if (!hexd) {
           memset(lpexd, 0, sizeof(EXCEPTION_DESCRIPTION));
        }
        else {
           *lpexd = *(LPEXCEPTION_DESCRIPTION)LLLock(hexd);
           LLUnlock(hexd);
        }
        break;


      case exfSpecified:

        hexd = LLFind( lpprc->llexc, NULL, &lpexd->dwExceptionCode, 0 );
        if (!hexd) {
            // get the default exception record
            DWORD dwT = 0;
            hexd = LLFind( lpprc->llexc, NULL, &dwT, 0 );
        }
        if (!hexd) {
            memset(lpexd, 0, sizeof(EXCEPTION_DESCRIPTION));
            xosd = xosdInvalidParameter;
        }
        else {
           *lpexd = *(LPEXCEPTION_DESCRIPTION)LLLock(hexd);
           LLUnlock(hexd);
        }
        break;


      case exfNext:

        hexd = LLFind( lpprc->llexc, NULL, &lpexd->dwExceptionCode, 0 );
        if (!hexd) {
            //
            // origin must exist
            //
            xosd = xosdInvalidParameter;
        } else {
            //
            // but the next one need not
            //
            hexd = LLNext( lpprc->llexc, hexd );
            if (!hexd) {
                memset(lpexd, 0, sizeof(EXCEPTION_DESCRIPTION));
                xosd = xosdEndOfStack;
            } else {
                *lpexd = *(LPEXCEPTION_DESCRIPTION)LLLock(hexd);
                LLUnlock(hexd);
            }
        }
        break;

    }

    LLUnlock(hprc);
    return xosd;
}


XOSD
SetExceptionState(
    HPID hpid,
    HTID htid,
    LPEXCEPTION_DESCRIPTION lpexd
    )
{
    HPRC hprc = HprcFromHpid( hpid );
    HLLI llexc;
    HEXD hexd;

    assert(lpexd->efd == efdIgnore ||
           lpexd->efd == efdNotify ||
           lpexd->efd == efdCommand ||
           lpexd->efd == efdStop);

    if (!hprc) {
        return xosdBadProcess;
    }

    llexc = ((LPPRC)LLLock(hprc))->llexc;
    LLUnlock(hprc);

    hexd = LLFind( llexc, NULL, &lpexd->dwExceptionCode, 0 );

    if (!hexd) {
        hexd = LLCreate( llexc );
        if (!hexd) {
            return xosdOutOfMemory;
        }
        LLAdd( llexc, hexd );
    }

    *(LPEXCEPTION_DESCRIPTION)LLLock(hexd) = *lpexd;
    LLUnlock(hexd);

    return SendRequestX( dmfSetExceptionState, hpid, htid,
                                        sizeof(EXCEPTION_DESCRIPTION), lpexd);
}


XOSD
GetMemoryInfo(
    HPID hpid,
    HTID htid,
    LPMEMINFO lpmi
    )
{
    PMEMORY_BASIC_INFORMATION lpmbi;
    ADDR addr;
    XOSD xosd = xosdNone;

    Unreferenced(htid);

    addr = lpmi->addr;

    if (ADDR_IS_LI(addr)) {
        xosd = FixupAddr(hpid, htid, &addr);
    }

    GuardTL();
    if (xosd == xosdNone) {
        xosd = SendRequestX( dmfVirtualQuery, hpid, 0, sizeof(ADDR),
                                                              (LPVOID)&addr );
    }

    if (xosd == xosdNone) {
        lpmbi = (PMEMORY_BASIC_INFORMATION) LpDmMsg->rgb;
        lpmi->addrAllocBase = addr;
        lpmi->addrAllocBase.addr.off = (UOFF32)lpmbi->AllocationBase;
        lpmi->uRegionSize = (UOFF32)lpmbi->RegionSize;
        lpmi->dwProtect = lpmbi->Protect;
        lpmi->dwState = lpmbi->State;
        lpmi->dwType = lpmbi->Type;
    }
    ReleaseTL();

    return xosd;
}


XOSD
FreezeThread(
    HPID hpid,
    HTID htid,
    BOOL fFreeze
    )
{
    XOSD xosd;
    HTHD hthd;
    HPRC hprc = ValidHprcFromHpid(hpid);
    if (!hprc) {
        return xosdBadProcess;
    }
    if ( (hthd = HthdFromHtid(hprc, htid)) == hthdInvalid || hthd == NULL ) {
        return xosdBadThread;
    }

    GuardTL();
    if (fFreeze) {
        SendRequest ( dmfFreeze, hpid, htid);
    } else {
        SendRequest ( dmfResume, hpid, htid);
    }

    xosd = LpDmMsg->xosdRet;
    ReleaseTL();
    return xosd;
}



#define FreeModuleList(m)                       MHFree(m)


XOSD
GetModuleList(
    HPID                    hpid,
    HTID                    htid,
    LPTSTR                  lpModuleName,
    LPMODULE_LIST FAR *     lplpModuleList
    )
{
    XOSD            xosd = xosdNone;
    HLLI            llmdi;
    HMDI            hmdi;
    LPMDI           lpmdi;
    DWORD           Count;
    LPMODULE_LIST   ModList;
    LPMODULE_LIST   TmpList;
    LPMODULE_ENTRY  Entry;
    LDT_ENTRY       Ldt;
    DWORD           MaxSize;
    DWORD           Delta;
    DWORD           i;
    SEGMENT         Selector;
    DWORD           Base;
    DWORD           Limit;
    OBJD           *ObjD;
    LPTSTR          p;
    TCHAR           WantedName[ MAX_PATH ];
    TCHAR           WantedExt[ MAX_PATH ];
    TCHAR           ModName[ MAX_PATH ];
    TCHAR           ModExt[ MAX_PATH ];
    TCHAR           Name[ MAX_PATH ];

    *WantedName = '\0';
    *WantedExt  = '\0';

    if ( !lplpModuleList ) {
        xosd = xosdInvalidParameter;
        goto Done;
    }

    *lplpModuleList = NULL;

    llmdi = LlmdiFromHprc( HprcFromHpid ( hpid ));

    if ( !llmdi ) {
        xosd = xosdBadProcess;
        goto Done;
    }


    //
    //  Estimate the list size, to minimize the calls to realloc.
    //
    if ( lpModuleName ) {

        Count = 20;
        _tsplitpath( lpModuleName, NULL, NULL, WantedName, WantedExt );

    } else {

        hmdi  = hmdiNull;
        Count = 0;

        while ( (hmdi = LLNext( llmdi, hmdi )) != hmdiNull ) {
            lpmdi = (LPMDI) LLLock( hmdi );
            Count += lpmdi->fFlatMode ? 1 : lpmdi->cobj;
            LLUnlock( hmdi );
        }
    }

    //
    //  Allocate the list
    //
    MaxSize = sizeof(MODULE_LIST) + Count * sizeof(MODULE_ENTRY);

    ModList = (LPMODULE_LIST) MHAlloc( MaxSize );

    if ( !ModList ) {
        xosd = xosdOutOfMemory;
        goto Done;
    }

    //
    //  Build the list
    //
    Count = 0;

    for ( hmdi = NULL; (hmdi = LLNext( llmdi, hmdi )); LLUnlock( hmdi ) ) {

        lpmdi = (LPMDI) LLLock( hmdi );

        //
        //  Get the module name
        //
        p = (*(lpmdi->lszName) == _T('|')) ? lpmdi->lszName+1 : lpmdi->lszName;
        _ftcscpy( Name, p );
        p = _ftcschr( Name, _T('|') );
        if ( p ) {
            *p = _T('\0');
        }

        if ( lpModuleName ) {

            //
            //  Add if base name matches
            //
            _tsplitpath( Name, NULL, NULL, ModName, ModExt );

            if (_ftcsicmp(WantedName, ModName) || _ftcsicmp(WantedExt, ModExt) ) {
                continue;
            }
        }

        Delta = lpmdi->fFlatMode ? 1 : lpmdi->cobj;

        //
        //  Reallocate buffer if necessary
        //
        if ( (Count + Delta) * sizeof(MODULE_ENTRY) > MaxSize ) {

            MaxSize += Delta * sizeof(MODULE_ENTRY);
            TmpList = (LPMODULE_LIST) MHRealloc( ModList, MaxSize );
            if ( !TmpList ) {
                FreeModuleList(ModList);
                xosd = xosdOutOfMemory;
                break;
            }

            ModList = TmpList;
        }

        //
        //  have buffer, fill it up
        //
        if ( lpmdi->fFlatMode ) {

            Entry = NthModuleEntry(ModList,Count);

            ModuleEntryFlat(Entry)          = TRUE;
            ModuleEntrySegment(Entry)       = 0;
            ModuleEntrySelector(Entry)      = 0;
            ModuleEntryBase(Entry)          = lpmdi->lpBaseOfDll;
            ModuleEntryLimit(Entry)         = lpmdi->dwSizeOfDll;
            ModuleEntryType(Entry)          = 0;
            ModuleEntrySectionCount(Entry)  = lpmdi->cobj;
            ModuleEntryEmi(Entry)           = lpmdi->hemi;
            _ftcscpy(ModuleEntryName(Entry), Name);

            Count++;

        } else {

            for ( i=0, ObjD = lpmdi->rgobjd; i < Delta; i++, ObjD++ ) {

                if ( ObjD->wSel ) {

                    Selector = ObjD->wSel;

                    Entry    = NthModuleEntry(ModList,Count);

                    ModuleEntrySegment(Entry)       = i+1;
                    ModuleEntrySelector(Entry)      = Selector;
                    ModuleEntryType(Entry)          = 0;
                    ModuleEntrySectionCount(Entry)  = 0;
                    ModuleEntryEmi(Entry)           = lpmdi->hemi;

                    _ftcscpy(ModuleEntryName(Entry), Name);

                    if ( lpmdi->fRealMode ) {

                        xosd = xosdNone;

                        ModuleEntryFlat(Entry)          = FALSE;
                        ModuleEntryReal(Entry)          = TRUE;
                        ModuleEntryBase(Entry)          = 0xBAD00BAD;
                        ModuleEntryLimit(Entry)         = 0xBAD00BAD;

                        Count++;

                    } else {

                        GuardTL();
                        xosd = SendRequestX( dmfQuerySelector,
                                             hpid,
                                             NULL,
                                             sizeof(SEGMENT),
                                             &Selector );

                        if (xosd == xosdNone) {


                            _fmemcpy( &Ldt, LpDmMsg->rgb, sizeof(Ldt));

                            Base = (Ldt.HighWord.Bits.BaseHi  << 0x18) |
                                   (Ldt.HighWord.Bits.BaseMid << 0x10) |
                                   Ldt.BaseLow;

                            Limit = (Ldt.HighWord.Bits.LimitHi << 0x10) |
                                                    Ldt.LimitLow;

                            ModuleEntryFlat(Entry)          = FALSE;
                            ModuleEntryReal(Entry)          = FALSE;
                            ModuleEntryBase(Entry)          = Base;
                            ModuleEntryLimit(Entry)         = Limit;

                            Count++;

                        } else {

                            xosd = xosdNone;

                            ModuleEntryFlat(Entry)          = FALSE;
                            ModuleEntryReal(Entry)          = FALSE;
                            ModuleEntryBase(Entry)          = 0xBAD00BAD;
                            ModuleEntryLimit(Entry)         = 0xBAD00BAD;
                            Count++;
                        }
                        ReleaseTL();
                    }
                }
            }
        }
    }

    if (hmdi) {
        LLUnlock(hmdi);
    }

    ModuleListCount(ModList) = Count;
    *lplpModuleList = ModList;

Done:
    return xosd;
}


XOSD
DoContinue(
    HPID hpid
    )
{
    LPPRC       lpprc;
    HPRC        hprc = HprcFromHpid(hpid);
    XOSD        xosd = xosdUnknown;
    BYTE        b = 1;

    assert(hprc);

    lpprc = (LPPRC) LLLock(hprc);

    if (lpprc->fLoadingModule) {

        lpprc->fRunning = TRUE;
        lpprc->fLoadingModule = FALSE;
        CallTL ( tlfReply, hpid, 1, &b );
        xosd = xosdNone;

    } else if (lpprc->fUnloadingModule) {

        lpprc->fRunning = TRUE;
        lpprc->fUnloadingModule = FALSE;
        CallTL ( tlfReply, hpid, 1, &b );
        xosd = xosdNone;
    }

    LLUnlock(hprc);

    return xosd;
}


XOSD
DoCustomCommand(
    HPID   hpid,
    HTID   htid,
    DWORD  wValue,
    LPSSS  lpsss
    )
{
    LPTSTR  lpsz = (LPTSTR)lpsss->rgbData;
    LPTSTR  p;
    XOSD   xosd;
    TCHAR  cmd[256];

    //
    // parse the command from the command line
    //
    p = cmd;
    while (*lpsz && !_istspace(*lpsz)) {
                _tccpy(p,lpsz);
                p = _tcsinc(p);
                lpsz = _tcsinc(lpsz);
    }
    *p = _T('\0');

    //
    // this is where you would stricmp() for your custom em command
    // otherwise it is passed to the dm
    //

    return SendRequestX( dmfSystemService, hpid, htid, wValue, (LPVOID) lpsss );

    //
    // this is what would be executed if you have a custom em command
    // instead of the above sendrequest()
    //

#if 0
    _tcscpy( lpiol->rgbVar, lpsz );
    xosd = IoctlCmd(hpid, htid, wValue, lpiol);
#endif

    return xosd;
}                                    /* DoCustomCommand */




XOSD
SystemService(
    HPID   hpid,
    HTID   htid,
    DWORD  wValue,
    LPSSS  lpsss
    )

/*++

Routine Description:

    This function examines SystemService requests (escapes) and deals
    with those which the EM knows about.  All others are passed on to
    the DM for later processing.

Arguments:

    argument-name - Supplies | Returns description of argument.
    .
    .

Return Value:

    return-value - Description of conditions needed to return value. - or -
    None.

--*/

{
    XOSD        xosd;
    DWORD       dw;
    HPRC        hprc;
    HTHD        hthd;
    LPTHD       lpthd;


    switch( lpsss-> ssvc ) {

      case ssvcGetStackFrame:
        hprc = HprcFromHpid( hpid );
        hthd = HthdFromHtid ( hprc, htid );
        assert(hthd);
        lpthd = (LPTHD) LLLock(hthd);
        _fmemcpy(lpsss->rgbData, &lpthd->StackFrame, sizeof(STACKFRAME));
        lpsss->cbReturned = sizeof(STACKFRAME);
        xosd = xosdNone;
        break;

      case ssvcGetThreadContext:

        GuardTL();
        xosd = SendRequest ( dmfReadReg, hpid, htid );
        if (xosd == xosdNone) {
            xosd = LpDmMsg->xosdRet;
            dw = min(lpsss->cbSend, (unsigned) SizeOfContext(hpid));
            _fmemcpy (lpsss->rgbData, LpDmMsg->rgb, dw);
            lpsss->cbReturned = dw;
        }
        ReleaseTL();
        break;

      case ssvcSetThreadContext:
        xosd = SendRequestX( dmfWriteReg, hpid, htid, lpsss->cbSend,
                                                              lpsss->rgbData );
        break;

      case ssvcGetProcessHandle:
      case ssvcGetThreadHandle:
        GuardTL();
        xosd = SendRequestX(dmfSystemService,hpid, htid, wValue, (LPVOID)lpsss);
        if (xosd == xosdNone) {
            xosd = LpDmMsg->xosdRet;
            dw = min(lpsss->cbSend, sizeof(HANDLE));
            _fmemcpy (lpsss->rgbData, LpDmMsg->rgb, dw);
            lpsss->cbReturned = dw;
        }
        ReleaseTL();
        break;


      case ssvcCustomCommand:
        xosd = DoCustomCommand(hpid, htid, wValue, lpsss);
        break;

      case ssvcGetPrompt:
        GuardTL();
        xosd = SendRequestX(dmfSystemService, hpid, htid, wValue, (LPVOID)lpsss);
        if (xosd == xosdNone) {
            xosd = LpDmMsg->xosdRet;
            lpsss->cbReturned = ((LPPROMPTMSG)((LPSSS)LpDmMsg->rgb))->len + sizeof(PROMPTMSG);
            if (lpsss->cbReturned) {
                _fmemcpy((LPVOID)lpsss->rgbData,
                         LpDmMsg->rgb,
                         lpsss->cbReturned);
            }
        }
        ReleaseTL();
        break;
      case ssvcFiberDebug:
        {
            OFBRS ofbrs = *((OFBRS *) lpsss->rgbData);
            GuardTL();
            xosd = SendRequestX(dmfSystemService,hpid, htid, wValue, (LPVOID)lpsss);
            if ((xosd == xosdNone) && 
                    ((ofbrs.op == OFBR_GET_LIST) ||
                    (ofbrs.op == OFBR_QUERY_LIST_SIZE))) {
                xosd = LpDmMsg->xosdRet;
                dw = min(lpsss->cbSend, *((DWORD *) LpDmMsg->rgb)-4);
                _fmemcpy (lpsss->rgbData, ((DWORD *)LpDmMsg->rgb)+1, dw);
                lpsss->cbReturned = dw;
            }
            ReleaseTL();
        }
        break;


      case ssvcGeneric:
        GuardTL();
        xosd = SendRequestX(dmfSystemService,hpid,htid,wValue,(LPVOID)lpsss);
        if (xosd == xosdNone) {
            xosd = LpDmMsg->xosdRet;
            lpsss->cbReturned = *((LPDWORD)LpDmMsg->rgb);
            if (lpsss->cbReturned) {
                _fmemcpy ( (LPVOID)lpsss->rgbData,
                          LpDmMsg->rgb + sizeof(DWORD),
                          lpsss->cbReturned);
            }
        }
        ReleaseTL();
        break;

      default:
        xosd = SendRequestX(dmfSystemService,hpid,htid,wValue,(LPVOID)lpsss);
        break;
    }

    return xosd;

}

XOSD
RangeStep (
    HPID   hpid,
    HTID   htid,
    LPRSS  lprss
    )

/*++

Routine Description:

    This function is called to implement range steps in the EM.  A range
    step is defined as step all instructions as long as the program counter
    remains within the starting and ending addresses.

Arguments:

    hpid      - Supplies the handle of the process to be stepped
    htid      - Supplies the handle of thread to be stepped

Return Value:

    XOSD error code

--*/

{
    RST		rst = {0};


    UpdateChild( hpid, htid, dmfRangeStep );

#if 0
    rst.fStepOver = lprss->lpExop->fStepOver;
    rst.fAllThreads = !lprss->lpExop->fSingleThread;
    rst.fInitialBP = lprss->lpExop->fInitialBP;
    rst.fReturnValues = lprss->lpExop->fReturnValues;
#endif

	rst.exop = *lprss->lpExop;
    rst.offStart = lprss->lpaddrMin->addr.off;
    rst.offEnd = lprss->lpaddrMax->addr.off;

#if defined(TARGET_MIPS) || defined(TARGET_PPC)
  	rst.fPassException = lprss->lpExop->fPassException;
#endif

    return SendRequestX (
        dmfRangeStep,
        hpid,
        htid,
        sizeof ( RST ),
        &rst
    );

}                           /* RangeStep() */

XOSD
SingleStep (
    HPID   hpid,
    HTID   htid,
    LPEXOP lpexop
    )
{
    assert ( hpid != NULL );
    assert ( htid != NULL );

    UpdateChild( hpid, htid, dmfSingleStep );

    return SendRequestX (
        dmfSingleStep,
        hpid,
        htid,
        sizeof(EXOP),
        lpexop
    );
}


int
__cdecl
CompMsg(
    void const *lpdwKeyMsg,
    void const *lpMessageInfo
    )
{
    if (*((LPDWORD)lpdwKeyMsg) < ((LPMESSAGEINFO)lpMessageInfo)->dwMsg) {
        return -1;
    } else if (*((LPDWORD)lpdwKeyMsg) > ((LPMESSAGEINFO)lpMessageInfo)->dwMsg) {
        return 1;
    } else {
        return 0;
    }
}

DWORD
GetMessageMask(
    DWORD dwMessage
)
{
    LPMESSAGEINFO lpMessageInfo;

    lpMessageInfo = (LPMESSAGEINFO)bsearch((const void*)&dwMessage,
                                           (const void*)MessageInfo,
                                           sizeof(MessageInfo)/sizeof(MESSAGEINFO),
                                           sizeof(MESSAGEINFO),
                                           CompMsg);

    if (lpMessageInfo) {
        return lpMessageInfo->dwMsgMask;
    } else {
        return 0;
    }
}

int
__cdecl
CompMESSAGEINFO(
    void const *lpMessageInfo1,
    void const *lpMessageInfo2
    )
{
    // a bit of code reuse
    return CompMsg(&((LPMESSAGEINFO)lpMessageInfo1)->dwMsg, lpMessageInfo2);
}

void
SortMessages (
    void
    )
{
    qsort(MessageInfo,
          sizeof(MessageInfo) / sizeof(MESSAGEINFO),
          sizeof(MESSAGEINFO),
          CompMESSAGEINFO);
}




XOSD
SetPath(
    HPID   hpid,
    HTID   htid,
    BOOL   Set,
    LPTSTR Path
    )
/*++

Routine Description:

    Sets the search path in the DM

Arguments:

    hpid    -   process
    htid    -   thread
    Set     -   set flag
    Path    -   Path to search, PATH if null


Return Value:

    xosd error code

--*/

{
    TCHAR    Buffer[ MAX_PATH ];
    SETPTH  *SetPth = (SETPTH *)&Buffer;

    if ( Set ) {

        SetPth->Set = TRUE;
        if ( Path ) {
            _ftcscpy(SetPth->Path, Path );
        } else {
            SetPth->Path[0] = _T('\0');
        }
    } else {
        SetPth->Set     = FALSE;
        SetPth->Path[0] = _T('\0');
    }

    return SendRequestX( dmfSetPath, hpid, htid,
                           sizeof(SETPTH) + _ftcslen(SetPth->Path)*sizeof(TCHAR),
                                                   SetPth );
}

