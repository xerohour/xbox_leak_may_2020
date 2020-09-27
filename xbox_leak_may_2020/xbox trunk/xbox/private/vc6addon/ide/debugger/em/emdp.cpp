/*++

Copyright (c) 1992  Microsoft Corporation

Module Name:

    emdp.c

Abstract:

    This file contains the main driver for the native execution models
    supported by us.  This file additionally contains the machine
    independent portions of the execution model.  The machine dependent
    portions are in other files.

Author:

    Jim Schaad (jimsch) 05-23-92

Environment:

    Win32 -- User

Notes:

    The orginal source for this came from the CodeView group.

--*/

#include "emdp.h"
#include "simpldis.h"

#include "dbgver.h"

/*************************** DEFINES  *****************************/

#define CBBUFFERDEF 1024
#define CEXM_MDL_native 0x20
#define cchErrMax   50

/******************* TYPEDEFS and STRUCTURE ***********************/


/*********************** LOCAL DATA *******************************/

CRITICAL_SECTION csCache;
CRITICAL_SECTION csInTL;

LPBYTE          LpSendBuf = NULL;
DWORD           CbSendBuf = 0;

DWORD           CbDmMsg = 0;
LPDM_MSG        LpDmMsg = NULL;

LPDBF           lpdbf = (LPDBF)NULL;

LPFNSVC         lpfnsvcTL = (LPFNSVC)NULL;
XOSD (PASCAL LOADDS *CallTL) ( TLF, HPID, DWORD, LPVOID );
XOSD (PASCAL LOADDS *CallDB) ( DBC, HPID, HTID, DWORD, DWORD, LPVOID );
XOSD (PASCAL LOADDS *CallNT) ( EMF, HPID, HTID, DWORD, LPVOID );

HLLI            llprc = (HLLI)NULL;

HPRC            hprcCurr = 0;
HPID            hpidCurr = 0;
PID             pidCurr  = 0;
PCPU_POINTERS   pointersCurr = 0;
MPT             mptCurr;

HTHD            hthdCurr = 0;
HTID            htidCurr = 0;
TID             tidCurr  = 0;

HLLI            HllEo = (HLLI) NULL;



///BUGBUG
// need to either agree on one name for the target dm dll
// or to implement a protocol to get the name via TL
// currently MSVC assumed host==target

#define DEFAULT_DMNAME  _T("dm" DM_TAIL)
//#define       DEFAULT_DMNAME  _T("dmkdx86" DM_TAIL)


#define DEFAULT_DMPARAMS _T("")

LOADDMSTRUCT LoadDmStruct = {
    NULL, NULL
};



/********************* EXTERNAL/GLOBAL DATA ************************/

HINSTANCE hInstance = NULL;

/*********************** PROTOTYPES *******************************/



/************************** &&&&&& ********************************/

/*
 *  This is the description of all registers and flags for the
 *      machine being debugged.  These files are machine dependent.
 */





/*************************** CODE *****************************************/
LPTSTR
MHStrdup(
    LPTSTR lpstr
    )
{
    LPTSTR retstr = (LPTSTR) MHAlloc((_ftcslen(lpstr) + 1) * sizeof(TCHAR));
    assert(retstr);
    if (retstr) {
        _ftcscpy(retstr, lpstr);
    }
    return(retstr);
}

/**** DBGVersionCheck                                                   ****
 *                                                                         *
 *  PURPOSE:                                                               *
 *                                                                         *
 *      To export out version information to the debugger.                 *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *      NONE.                                                              *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *      Returns - A pointer to the standard version information.           *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *      Just returns a pointer to a static structure.                      *
 *                                                                         *
 ***************************************************************************/

#ifdef DEBUGVER
DEBUG_VERSION('E','M',"Execution Model")
#else
RELEASE_VERSION('E','M',"Execution Model")
#endif

DBGVERSIONCHECK()

/**** SENDCOMMAND - Send a command to the DM                            ****
 *                                                                         *
 *  PURPOSE:                                                               *
 *      Send a DMF command to the DM.                                      *
 *                                                                         *
 *  INPUTS:                                                                *
 *      dmf - the command to send                                          *
 *      hpid - the process                                                 *
 *      htid - the thread                                                  *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *      xosd - error code indicating if command was sent successfully      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *      Sending commands is asynchronous: this function may return before  *
 *      the DM has actually processed the command.                         *
 *                                                                         *
 ***************************************************************************/
XOSD
SendCommand (
    DMF dmf,
    HPID hpid,
    HTID htid
    )
{
    DBB dbb;

    dbb.dmf  = dmf;
    dbb.hpid = hpid;
    dbb.htid = htid;

    return CallTL ( tlfDebugPacket, hpid, FIELD_OFFSET ( DBB, rgbVar ), (LPV)&dbb );
}


/**** SENDREQUEST - Send a request to the DM                            ****
 *                                                                         *
 *  PURPOSE:                                                               *
 *      Send a DMF request to the DM.                                      *
 *                                                                         *
 *  INPUTS:                                                                *
 *      dmf - the request to send                                          *
 *      hpid - the process                                                 *
 *      htid - the thread                                                  *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *      xosd - error code indicating if request was sent successfully      *
 *      LpDmMsg - global buffer filled in with returned data               *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *      Unlike SendCommand, this function will wait for data to be         *
 *      returned from the DM before returning to the caller.               *
 *                                                                         *
 ***************************************************************************/
EMEXPORT XOSD
SendRequest (
    DMF dmf,
    HPID hpid,
    HTID htid
    )
{
    DBB     dbb;
    XOSD    xosd;

    dbb.dmf  = dmf;
    dbb.hpid = hpid;
    dbb.htid = htid;

    GuardTL();
    xosd = CallTL ( tlfRequest, hpid, FIELD_OFFSET ( DBB, rgbVar ), &dbb );

    if (xosd == xosdNone) {
        xosd = (XOSD) LpDmMsg->xosdRet;
    }

    ReleaseTL();
    return xosd;
}


/**** SENDREQUESTX - Send a request with parameters to the DM           ****
 *                                                                         *
 *  PURPOSE:                                                               *
 *      Send a DMF request and its parameter info to the DM.               *
 *                                                                         *
 *  INPUTS:                                                                *
 *      dmf - the request to send                                          *
 *      hpid - the process                                                 *
 *      htid - the thread                                                  *
 *      wLen - number of bytes in lpv                                      *
 *      lpv - pointer to additional info needed by the DM; contents are    *
 *          dependent on the DMF                                           *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *      xosd - error code indicating if request was sent successfully      *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *      Unlike SendCommand, this function will wait for data to be         *
 *      returned from the DM before returning to the caller.               *
 *                                                                         *
 ***************************************************************************/
EMEXPORT XOSD
SendRequestX (
    DMF dmf,
    HPID hpid,
    HTID htid,
    DWORD wLen,
    LPVOID lpv
    )
{
    LPDBB   lpdbb;
    XOSD    xosd;

    if (wLen + FIELD_OFFSET(DBB, rgbVar) > CbSendBuf) {
        if (LpSendBuf) {
            MHFree(LpSendBuf);
        }
        CbSendBuf = FIELD_OFFSET(DBB, rgbVar) + wLen;
        LpSendBuf = (LPBYTE) MHAlloc(CbSendBuf);
    }

    if (!LpSendBuf) {
        return xosdOutOfMemory;
    }

    lpdbb = (LPDBB)LpSendBuf;

    lpdbb->dmf  = dmf;
    lpdbb->hpid = hpid;
    lpdbb->htid = htid;
    _fmemcpy ( lpdbb->rgbVar, lpv, wLen );

    GuardTL();
    xosd = CallTL ( tlfRequest, hpid, FIELD_OFFSET ( DBB, rgbVar ) + wLen, (LPVOID)lpdbb );

    if (xosd == xosdNone) {
        xosd = (XOSD) LpDmMsg->xosdRet;
    }

    ReleaseTL();
    return xosd;
}

// Helper function: repack dmfProgLoad/dmfSpawnOrphan arguments into the form:
//              DWORD dwChildFlags
//              TCHAR rgtchExe[]
//              TCHAR rgtchCmdLine[]
//              TCHAR rgtchDir[]
//
//      You probably want to free the lplpvPacket when you're done.
//
XOSD
RepackProgLoad (
        CONST LPPRL             lpprl,
        LPVOID                  *lplpvPacket,
        UINT                    *pcb
    )
{
    BYTE*   lpb;
    LPTSTR  lszRemoteDir = _T("");

    assert (lpprl);
    assert (lplpvPacket);
    assert (pcb);

    // lszRemoteDir is allowed to be NULL, in which case we pass ""
    if (lpprl -> lszRemoteDir != NULL) {
        lszRemoteDir = lpprl -> lszRemoteDir;
    }

    *pcb = sizeof(DWORD);
    *pcb += _ftcslen(lpprl -> lszRemoteExe) + 1;
    *pcb += _ftcslen(lpprl -> lszCmdLine) + 1;

    *pcb += _ftcslen(lszRemoteDir) + 1;
    *pcb += sizeof (SPAWNORPHAN);

#if defined(_UNICODE)
#pragma message("MHAlloc and *lplpvPacket+ctch need work")
#endif
    *lplpvPacket = MHAlloc(*pcb);
    lpb = (BYTE*) *lplpvPacket;

    if (!*lplpvPacket) {
        return xosdOutOfMemory;
    }

//  REVIEW:  SwapEndian ( &dwChildFlags, sizeof ( dwChildFlags ) );

    memcpy (lpb, &(lpprl -> dwChildFlags), sizeof (lpprl -> dwChildFlags));
    lpb += sizeof(DWORD);

    _ftcscpy((CHAR*) lpb, lpprl -> lszRemoteExe);
    lpb += _ftcslen (lpprl -> lszRemoteExe) + 1;

    _ftcscpy((CHAR*) lpb, lpprl -> lszCmdLine);
    lpb += _ftcslen (lpprl -> lszCmdLine) + 1;

    _ftcscpy((CHAR*) lpb, lszRemoteDir);
    lpb += _ftcslen (lszRemoteDir) + 1;

    if (lpprl -> lpso) {
        memcpy ((CHAR*) lpb, lpprl -> lpso, sizeof (SPAWNORPHAN));
    } else {
        *lpb = 0;
    }

    return xosdNone;
}
XOSD
SpawnOrphan (
    HPID  hpid,
    DWORD  cb,
    LPSOS lpsos
    )

/*++

Routine Description:

    This routine is called to cause a program to be loaded by the
    debug monitor, but not debugged.

Arguments:

    hpid  - Supplies the OSDEBUG handle to the process to be loaded
    cb    - Length of the command line
    lpsos - Pointer to structure containning the command line

Return Value:

    xosd error code

--*/

{
    LPVOID lpb;
    XOSD xosd;

    xosd = RepackProgLoad(lpsos, &lpb, (UINT*) &cb);
    if (xosd != xosdNone) {
        return (xosd);
    }

    assert (lpsos -> lpso);

    GuardTL();
    xosd = SendRequestX ( dmfSpawnOrphan,
                          hpid,
                          NULL,
                          cb,
                          lpb
                          );

    MHFree(lpb);

    memcpy (lpsos -> lpso, LpDmMsg->rgb, sizeof (SPAWNORPHAN));
    ReleaseTL();

    return xosd;
}                               /* SpawnOrphan() */




XOSD
ProgramLoad (
    HPID  hpid,
    DWORD  cb,
    LPPRL lpprl
    )

/*++

Routine Description:

    This routine is called to cause a program to be loaded by the
    debug monitor.

Arguments:

    hpid  - Supplies the OSDEBUG handle to the process to be loaded
    cb    - Length of the command line
    lpprl - Pointer to structure containning the command line

Return Value:

    xosd error code

--*/

{
    XOSD  xosd = xosdNone;
    LPPRC lpprc;
    HPRC  hprc = HprcFromHpid(hpid);
    LPVOID lpb;
    lpprc = (LPPRC) LLLock ( hprc );


	GetExceptionInfo ( hpid );
	
#if 0
    lpprc->efp  = efpNone;
#endif
    LLDestroy ( lpprc->llmdi );
    lpprc->llmdi = LLInit ( sizeof ( MDI ), llfNull, MDIKill, MDIComp );

    LLUnlock ( hprc );

    PurgeCache ();

    xosd = RepackProgLoad(lpprl, &lpb, (UINT*) &cb);
    if (xosd != xosdNone) {
        return (xosd);
    }
    assert (!lpprl->lpso);

    GuardTL();
    xosd = SendRequestX (
        dmfProgLoad,
        hpid,
        NULL,
        cb,
        lpb
    );

    MHFree(lpb);

    if (xosd == xosdNone) {
        xosd = LpDmMsg->xosdRet;
        lpprc = (LPPRC) LLLock ( hprc );
        lpprc->stat = statStarted;
        LLUnlock ( hprc );
    }
    ReleaseTL();

    return xosd;
}                               /* ProgramLoad() */


XOSD
DebugActive (
	HPID	hpid,
	DWORD	cb,
	LPVOID	lpv
	)
{
	XOSD	xosd;

	GetExceptionInfo ( hpid );
	
    GuardTL();
	xosd = SendRequestX ( dmfDebugActive, hpid, NULL, cb, lpv );

	if (xosd == xosdNone) {
		xosd = LpDmMsg->xosdRet;
	}
    ReleaseTL();

	return xosd;
}

		

/**** PROGRAMFREE - Terminate the program and free the pid              ****
 *                                                                         *
 *  PURPOSE:                                                               *
 *                                                                         *
 *  INPUTS:                                                                *
 *                                                                         *
 *  OUTPUTS:                                                               *
 *                                                                         *
 *  IMPLEMENTATION:                                                        *
 *                                                                         *
 *                                                                         *
 ***************************************************************************/

XOSD
ProgramFree (
    HPID hpid,
    HTID htid
    )
{
   // return SendCommand (dmfProgFree, hpid, NULL );

	return SendRequest (dmfProgFree, hpid, NULL);
}





XOSD
CompareAddrs(
    HPID hpid,
    HTID htid,
    LPCAS lpcas
    )
{
    ADDR a1 = *lpcas->lpaddr1;
    ADDR a2 = *lpcas->lpaddr2;
    XOSD xosd = xosdNone;
    LONG l;

    // if both are LI, see if they are comparable:
    if (ADDR_IS_LI(a1) && ADDR_IS_LI(a2)
          && emiAddr(a1) == emiAddr(a2)
          && GetAddrSeg(a1) == GetAddrSeg(a2))
    {
       if (GetAddrOff(a1) < GetAddrOff(a2)) {
           l = fCmpLT;
       } else if (GetAddrOff(a1) == GetAddrOff(a2)) {
           l = fCmpEQ;
       } else {
           l = fCmpGT;
       }
       *lpcas->lpResult = l;
    }

    else {

        // if neccessary, fixup addresses:
        if (ADDR_IS_LI(a1)) {
            FixupAddr(hpid, htid, &a1);
        }

        if (ADDR_IS_LI(a2)) {
            FixupAddr(hpid, htid, &a2);
        }


        // if real mode address, we can really compare
        if (ADDR_IS_REAL(a1) && ADDR_IS_REAL(a2)) {
            l =  ((GetAddrSeg(a1) << 4) + (GetAddrOff(a1) & 0xffff))
                - ((GetAddrSeg(a2) << 4) + (GetAddrOff(a2) & 0xffff));
            *lpcas->lpResult = (l < 0) ? -1 : ((l == 0) ? 0 : 1);
        }

        else if (ADDR_IS_FLAT(a1) != ADDR_IS_FLAT(a2)) {
            xosd = xosdInvalidParameter;
        }

        // if flat, ignore selectors
        else if (ADDR_IS_FLAT(a1)) {
            if (GetAddrOff(a1) < GetAddrOff(a2)) {
                l = fCmpLT;
            } else if (GetAddrOff(a1) == GetAddrOff(a2)) {
                l = fCmpEQ;
            } else {
                l = fCmpGT;
            }
            *lpcas->lpResult = l;
        }

        else if (GetAddrSeg(a1) == GetAddrSeg(a2)) {
            if (GetAddrOff(a1) < GetAddrOff(a2)) {
                l = fCmpLT;
            } else if (GetAddrOff(a1) == GetAddrOff(a2)) {
                l = fCmpEQ;
            } else {
                l = fCmpGT;
            }
            *lpcas->lpResult = l;
        }

        // not flat, different selectors
        else {
            xosd = xosdInvalidParameter;
        }

    }
    return xosd;
}



static BOOL fCacheDisabled = FALSE;

#define cbMaxCache CACHESIZE
typedef struct _MCI {
    WORD cb;
    HPID hpid;
    ADDR addr;
    BYTE rgb [ cbMaxCache ];
} MCI;  // Memory Cache Item

#define imciMax MAXCACHE
MCI FAR rgmci [ imciMax ] = {   { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };

// CACHESIZE *must* be a power of 2
#define	CACHE_ADDR_MASK	(~(CACHESIZE-1))

// Most recent used == 0, 2nd to last == 1, etc

int rgiUsage [ imciMax ] = {0};

void
InitUsage (
    void
    )
{
    int iUsage;

    for ( iUsage = 0; iUsage < imciMax; iUsage++ ) {
        rgiUsage [ iUsage ] = imciMax - ( iUsage + 1 );
    }
}

VOID
SetMostRecent (
    int imci
    )
{
    int i;

    if ( rgiUsage [ imci ] != 0 ) {
        for ( i = 0; i < imciMax; i++ ) {
            if ( rgiUsage [ i ] < rgiUsage [ imci ] ) {
                rgiUsage [ i ] ++;
            }
        }
        rgiUsage [ imci ] = 0;
    }
}

int
GetLeastRecent (
    VOID
    )
{
    int i;

    for ( i = 0; i < imciMax; i++ ) {
        assert ( rgiUsage [ i ] >= 0 && rgiUsage [ i ] < imciMax );
        if ( rgiUsage [ i ] == imciMax - 1 ) {
            return i;
        }
    }

    assert ( FALSE );

    return i;
}

VOID
SetLeastRecent (
    int imci
    )
{
    int i;

    if ( rgiUsage [ imci ] != imciMax - 1 ) {
        for ( i = 0; i < imciMax; i++ ) {
            if ( rgiUsage [ i ] > rgiUsage [ imci ] ) {
                rgiUsage [ i ] --;
            }
        }
        rgiUsage [ imci ] = imciMax-1;
    }
}


XOSD
ReadPhysical (
    HPID    hpid,
    DWORD   cb,
    LPBYTE  lpbDest,
    LPADDR  lpaddr,
    DWORD   iCache,
    LPDWORD lpcbr
    )
{
    LPDBB lpdbb;
    PRWP  prwp;
    XOSD  xosd = xosdNone;

    if (!ValidHprcFromHpid(hpid)) {
        return xosdBadProcess;
    }

    lpdbb = (LPDBB)MHAlloc(FIELD_OFFSET(DBB, rgbVar) + sizeof(RWP));
    prwp = (PRWP)lpdbb->rgbVar;

    lpdbb->dmf = dmfReadMem;
    lpdbb->hpid = hpid;
    lpdbb->htid = NULL;

    GuardTL();
    if ( cb + sizeof(DWORD) + FIELD_OFFSET(DM_MSG, rgb) > CbDmMsg ) {
        MHFree ( LpDmMsg );
        CbDmMsg = cb + sizeof ( DWORD ) + FIELD_OFFSET( DM_MSG, rgb );
        LpDmMsg = (LPDM_MSG) MHAlloc ( CbDmMsg );
        CallTL ( tlfSetBuffer, lpdbb->hpid, CbDmMsg, LpDmMsg );
    }

    prwp->cb   = cb;
    prwp->addr = *lpaddr;

    xosd = CallTL(tlfRequest, lpdbb->hpid, FIELD_OFFSET(DBB, rgbVar) + sizeof(RWP), lpdbb);

    if (xosd == xosdNone) {
        xosd = LpDmMsg->xosdRet;
        if (xosd == xosdNone) {
            *lpcbr = *( (LPDWORD) (LpDmMsg->rgb) );
            assert( *lpcbr <= cb );
            _fmemcpy ( lpbDest, LpDmMsg->rgb + sizeof ( DWORD ), *lpcbr );
        }
    }
    ReleaseTL();

    MHFree(lpdbb);

    return xosd;
}

XOSD
EnableCache (
    HPID  hpid,
    HTID  htid,
    BOOL  state
    )
{
    fCacheDisabled = state;

    if (fCacheDisabled) {
        PurgeCache();
    }

    return xosdNone;
}


void
PurgeCache (
    VOID
    )
{
    int imci;

    for ( imci = 0; imci < imciMax; imci++ ) {
        rgmci [ imci ].cb = 0;
    }
}

void
PurgeCacheHpid (
    HPID hpid
    )
{
    int imci;

    for ( imci = 0; imci < imciMax; imci++ ) {

        if ( rgmci [ imci ].hpid == hpid ) {
            rgmci [ imci ].cb = 0;
            SetLeastRecent ( imci );
        }
    }
}


XOSD
ReadForCache(
    HPID   hpid,
    DWORD  cbP,
    LPBYTE lpbDest,
    LPADDR lpaddr,
    LPDWORD lpcb
    )

/*++

Routine Description:

    This function will fill in a cache entry with the bytes requested
    to be read.  The function puts the bytes in both the cache and the
    memory buffer.

Arguments:

    hpid        - Supplies the process to do the read in

    cbP         - Supplies the number of bytes to be read

    lpbDest     - Supplies the buffer to place the bytes in

    lpaddr      - Supplies the address to read the bytes from

    lpcb        - Returns the number of bytes read

Return Value:

    XOSD error code

--*/

{
    assert(cbP <= cbMaxCache);
    DWORD       imci;
    DWORD       cbr;
    XOSD        xosd;
    ADDR        addrSave = *lpaddr;
    MCI *       pmci;
	const UOFFSET uAlignedOffset = GetAddrOff( *lpaddr ) & CACHE_ADDR_MASK;
	const DWORD dbOffset = GetAddrOff( addrSave ) - uAlignedOffset;

    /*
     *  Determine if the starting address is contained in a
     *  voided cache entry
     */

    for ( imci = 0, pmci = rgmci ; imci < imciMax; imci++, pmci++ ) {

        if ( (pmci->cb == 0) &&
             (pmci->hpid == hpid) &&
             (ADDR_IS_REAL( pmci->addr) == ADDR_IS_REAL( *lpaddr )) &&
             (GetAddrSeg ( pmci->addr ) == GetAddrSeg ( *lpaddr )) &&
             (uAlignedOffset == GetAddrOff ( pmci->addr ))
        ) {
            break;
        }
    }


    /*
     *  if we have not found a cache entry then just get one based on
     *  an LRU algorithm.
     */

    if ( imci == imciMax ) {
        imci = GetLeastRecent ( );
    }

    /*
     *  Do an actual read of memory from the debuggee
     */

	SetAddrOff( lpaddr, uAlignedOffset );

    xosd = ReadPhysical ( hpid, cbMaxCache, rgmci [ imci ].rgb, lpaddr, imci, &cbr );

    if ( xosd != xosdNone ) {
        return xosd;
    }

	assert(cbr==CACHESIZE);				// Win32 never gives us half-pages surely?

    /*
     *  touch the LRU table
     */

    SetMostRecent ( imci );

    /*
     *  set up the cache entry
     */

    assert(cbr <= cbMaxCache);
    rgmci [ imci ].cb = (WORD) cbr;
    rgmci [ imci ].addr = *lpaddr;
    rgmci [ imci ].hpid = hpid;

    *lpaddr = addrSave;

    /*
     *  compute the number of bytes read
     */

    DWORD cbT = min(cbMaxCache - dbOffset,cbP);

    /*
     *  copy from the cache entry to the users space
     */

    _fmemcpy ( lpbDest, rgmci [ imci ].rgb + dbOffset, cbT );

    /*
     *  return the number of bytes read
     */

    *lpcb = cbT;

    return xosdNone;
}                               /* ReadForCache() */


int
GetCacheIndex(
    HPID   hpid,
    LPADDR lpaddr
    )

/*++

Routine Description:

    This routine is given a process and an address and will locate
    which cache entry (if any) the address is in.

Arguments:

    hpid        - Supplies the handle to the process
    lpaddr      - Supplies the address to look for

Return Value:

    The index of the cache entry containing the address or imciMax if
    no cache entry contains the address

--*/

{
    int imci;

    for ( imci = 0; imci < imciMax; imci++ ) {
        LPADDR lpaddrT = &rgmci [ imci ].addr;

        /*
         *   To be in the cache entry check:
         *
         *      1.  The cache entry contains bytes
         *      2.  The cache entry is for the correct process
         *      3.  The cache entry if for the correct segment
         *      4.  The requested offset is between the starting and
         *              ending points of the cache
         */

        if ( (rgmci [ imci ].cb != 0) &&
             (rgmci [ imci ].hpid == hpid) &&
             (ADDR_IS_REAL( *lpaddrT ) == ADDR_IS_REAL( *lpaddr )) &&
             (GetAddrSeg ( *lpaddrT ) == GetAddrSeg ( *lpaddr )) &&
             (GetAddrOff ( *lpaddrT ) <= GetAddrOff ( *lpaddr )) &&
             (GetAddrOff ( *lpaddrT ) + rgmci[ imci ].cb > GetAddrOff ( *lpaddr ))) {

            break;
        }
    }

    return imci;
}                               /* GetCacheIndex() */


int
ReadFromCache (
    HPID hpid,
    DWORD cb,
    LPBYTE lpbDest,
    LPADDR lpaddr
    )

/*++

Routine Description:

    description-of-function.

Arguments:

    hpid        - Supplies a handle to the process
    cb          - Supplies the count of bytes to read from the cache
    lpbDest     - Suppiies the pointer to store bytes at
    lpaddr      - Supplies pointer to address to read at

Return Value:

    > 0         - The number of bytes read from the cache
    == 0        - No cache entry for the address was found

--*/

{
    int imci;

    /*
     *  See if the address for the start of the read is current contained
     *  in one of the cached buffers.
     */

    imci = GetCacheIndex ( hpid, lpaddr );

    /*
     *  If the starting address is in a cache entry then read as many
     *  bytes as is possible from that cache entry.
     */

    if ( imci != imciMax ) {
        DWORD ibStart;
        DWORD  cbT;

        /*
         *  Compute the difference between the address for the cache start
         *      and the address for the read request start and then
         *      the number of bytes which can be read in
         */

        ibStart = (DWORD)( GetAddrOff ( *lpaddr ) - GetAddrOff ( rgmci[imci].addr ) );
        cbT = min ( cb, rgmci [ imci ].cb - ibStart );

        /*
         *   Preform the copy
         */

        _fmemcpy ( lpbDest, rgmci [ imci ].rgb + ibStart, cbT );

        /*
         *   Return the number of bytes copied.  If it is less than
         *      zero then for some reason the current cache was not
         *      filled to capacity.
         */

        return cbT;
    }

    return 0;
}                               /* ReadFromCache() */


XOSD
ReadBuffer (
    HPID    hpid,
    HTID    htid,
    LPADDR  lpaddr,
    DWORD   cb,
    LPBYTE  lpbDest,
    LPDWORD lpcbRead
    )
/*++

Routine Description:

    This function is called in response to an emfReadBuf message.  The
    address to start the read at was set earlier and is stored in the
    adrCurrent Address Buffer.

Arguments:

    hpid        - Supplies the handle to the process to read memory for

    htid        - Supplies the handle to the thread to read memory for
                        (may be NULL)

    cb          - Supplies count of bytes to read

    lpbDest     - Supplies pointer to buffer to place bytes read

    lpcbRead    - Returns number of bytes read

Return Value:

    if >= 0 then it is the number of bytes actually read otherwise it
    is an xosdError code.

--*/

{
    XOSD        xosd = xosdNone;
    ADDR        addr;
    int         cbT = 0;
    int         cbRead = 0;
    HPRC        hprc = HprcFromHpid(hpid);
    LPPRC       lpprc;


    /*
     *  Retrieve the address to start the read at from the address buffer
     *  location
     */

    addr = *lpaddr;

        if (ADDR_IS_LI(addr)) {
                *lpcbRead = 0;
                return xosdBadAddress;      // can only do fixup addresses
        }

        /* If we are at the end of the memory address range and are trying to read
         * beyond the address range, just read till 0xFFFFFFFF
         */
        if ( cb != 0 && GetAddrOff(addr) + cb - 1 < GetAddrOff(addr))
        {
                assert(sizeof(GetAddrOff(addr)) == sizeof(DWORD));
                cb = 0xFFFFFFFF - GetAddrOff(addr) + 1;
        }

    /*
     *  Are we trying to read more bytes than is possible to store in
     *  a single cache?  If so then skip trying to hit the cache and
     *  go directly to asking the DM for the memory.
     *
     *  This generally is due to large memory dumps.
     */

    lpprc = (LPPRC) LLLock(hprc);
    if ( (cb > cbMaxCache) || (lpprc->fRunning ) || (fCacheDisabled) ) {
        LLUnlock(hprc);
        return ReadPhysical ( hpid, cb, lpbDest, &addr, MAXCACHE, lpcbRead );
    }
    LLUnlock(hprc);

    /*
     *  Read as much as possible from the set of cached memory reads.
     *  If cbT > 0 then bytes were read from a cache entry
     *  if cbT == 0 then no bytes were read in
     */

    while ((cb != 0) &&
           ( cbT = ReadFromCache ( hpid, cb, lpbDest, &addr ) ) > 0 ) {
        cbRead += cbT;
        lpbDest += cbT;
        GetAddrOff ( addr ) += cbT;
        cb -= cbT;
    }

    /*
     *  If there are still bytes left to be read then get the cache
     *  routines to read them in and copy both to a cache and to the
     *  buffer.
     */

    if ( cb > 0 ) {
        xosd = ReadForCache ( hpid, cb, lpbDest, &addr, (LPDWORD) &cbT );
        if (xosd == xosdNone) {
            cbRead += cbT;
			const DWORD nMore = cb - cbT;				// may straddle two aligned cache entries
			if (nMore)
			{
				// second block likely to already be in the cache so try that first
				ADDR addr2 = addr;
				SetAddrOff( &addr2, GetAddrOff( addr2 ) + cbT );
				DWORD cbCache = ReadFromCache( hpid, nMore, lpbDest+cbT, &addr2 );
				if (cbCache)
					cbRead += cbCache;
				else
				{
					xosd = ReadForCache( hpid, nMore, lpbDest+cbT, &addr2, (LPDWORD) &cbT );
					if (xosd==xosdNone)
						cbRead += cbT;
				}
			}
        }
    }

    if (lpcbRead) {
        *lpcbRead = cbRead;
    }

    return (cbRead!=0) ? xosdNone : xosd;
}                               /* ReadBuffer() */



XOSD
WriteBufferCache (
    HPID hpid,
    HTID htid,
    LPADDR lpaddr,
    DWORD cb,
    LPBYTE lpb,
    LPDWORD lpdwBytesWritten
    )
{
    PurgeCacheHpid ( hpid );
    return WriteBuffer ( hpid, htid, lpaddr, cb, lpb, lpdwBytesWritten );
}



XOSD
WriteBuffer (
    HPID hpid,
    HTID htid,
    LPADDR lpaddr,
    DWORD cb,
    LPBYTE lpb,
    LPDWORD lpdwBytesWritten
    )

/*++

Routine Description:

    This routine is used to send a request to the Debug Monitor to
    do a write to the debuggees memory.

Arguments:

    hpid        - Supplies the handle to the process to write memory in

    htid        - Supplies a thead handle

    lpaddr      - Supplies debuggee address to write at

    cb          - Supplies the number of bytes to be written

    lpb         - Supplies a pointer to the buffer to write

    lpdwBytesWritten - Returns number of bytes actually written

Return Value:

    an XOSD error code

--*/
{
    LPRWP lprwp = (LPRWP) MHAlloc( FIELD_OFFSET( RWP, rgb ) + cb );
    XOSD  xosd;

    lprwp->cb   = cb;
    lprwp->addr = *lpaddr;

    _fmemcpy ( lprwp->rgb, lpb, cb );

    GuardTL();
    xosd = SendRequestX (dmfWriteMem,
                         hpid,
                         htid,
                         FIELD_OFFSET ( RWP, rgb ) + cb,
                         lprwp
                         );

    MHFree ( lprwp );

    if (xosd == xosdNone) {
        *lpdwBytesWritten = *((LPDWORD)(LpDmMsg->rgb));
    } else {
        // REVIEW: what about partial writes. The DM will still send
        // the exact number of bytes written. Should we return that
        // back in lpdwBytesWritten
    }
    ReleaseTL();

    //
    //  Notify the shell that we changed memory. An error here is not
    //  tragic, so we ignore the return code.  The shell uses this
    //  notification to update all its memory breakpoints.
    //
    CallDB (
        dbcMemoryChanged,
        hpid,
        NULL,
        CEXM_MDL_native,
        cb,
        (LPVOID)lpaddr
        );

    return xosd;
}                               /* WriteBuffer() */



typedef struct _EMIC {
    HEMI hemi;
    HPID hpid;
    WORD sel;
} EMIC; // EMI cache item

#define cemicMax 4

EMIC rgemic [ cemicMax ] = {0};

XOSD
FindEmi (
    HPID   hpid,
    LPADDR lpaddr
    )
{
    XOSD        xosd = xosdNone;
    WORD        sel = (WORD)GetAddrSeg ( *lpaddr );
    HPRC        hprc = HprcFromHpid(hpid);
    HLLI        llmdi = LlmdiFromHprc ( hprc );
    BOOL        fFound = FALSE;
    ULONG       iobj = 0;
    HMDI        hmdi;
    LPPRC       lpprc = (LPPRC) LLLock( hprc );

    if ((lpprc->dmi.fAlwaysFlat) || (sel == lpprc->selFlatCs) || (sel == lpprc->selFlatDs)) {
        ADDR_IS_FLAT(*lpaddr) = TRUE;
		ADDR_IS_OFF32(*lpaddr) = TRUE;
    }
    LLUnlock( hprc );

    for ( hmdi = LLNext ( llmdi, hmdiNull );
          hmdi != hmdiNull;
          hmdi = LLNext ( llmdi, hmdi ) ) {

        LPMDI   lpmdi = (LPMDI) LLLock ( hmdi );
        OBJD *  rgobjd = &lpmdi->rgobjd[0];

        //  added && lpmdi->hemi to this conditional: the hemi can be null in
        //  the case that we recieved a dbcLoadModule, but did not end up
        //  loading the module  (a-math)

        for ( iobj = 0; iobj < lpmdi->cobj; iobj++ ) {
            if (((lpmdi->fFlatMode && ADDR_IS_FLAT(*lpaddr)) ||
                 (rgobjd[iobj].wSel == sel) && !ADDR_IS_FLAT(*lpaddr)) &&
                (rgobjd[iobj].offset <= GetAddrOff(*lpaddr)) &&
                (GetAddrOff(*lpaddr) < rgobjd[iobj].offset + rgobjd[iobj].cb) &&
                lpmdi->hemi) {

                fFound = TRUE;
                break;
            }
        }

        LLUnlock ( hmdi );

        // This break is here instead of in the "for" condition so
        //   that hmdi does not get advanced before we break

        if ( fFound ) {
            break;
        }
    }


    if ( !fFound ) {
        emiAddr ( *lpaddr ) = (HEMI) hpid;
    } else {
        emiAddr ( *lpaddr ) = (HEMI) HemiFromHmdi ( hmdi );

        if ( LLNext ( llmdi, hmdiNull ) != hmdi ) {

            // put the most recent hit at the head
            // this is an optimization to speed up the fixup/unfixup process
	     
	    // rajra: race condition here when multiple threads are in this routine       		
	    if (LLRemove ( llmdi, hmdi )) {
                LLAddHead ( llmdi, hmdi );
            }
        
	}
    }

    assert ( emiAddr ( *lpaddr ) != 0 );

    return xosd;
}

#pragma optimize ("", off)
XOSD
SetEmiFromCache (
    HPID   hpid,
    LPADDR lpaddr
    )
{
    XOSD xosd = xosdContinue;
#ifndef TARGET32
    int  iemic;

    for ( iemic = 0; iemic < cemicMax; iemic++ ) {

        if ( rgemic [ iemic ].hpid == hpid &&
             rgemic [ iemic ].sel  == GetAddrSeg ( *lpaddr ) ) {

            if ( iemic != 0 ) {
                EMIC emic = rgemic [ iemic ];
                int iemicT;

                for ( iemicT = iemic - 1; iemicT >= 0; iemicT-- ) {
                    rgemic [ iemicT + 1 ] = rgemic [ iemicT ];
                }
                rgemic [ 0 ] = emic;
            }

            xosd = xosdNone;
            emiAddr ( *lpaddr ) = rgemic [ 0 ].hemi;
            assert ( emiAddr ( *lpaddr ) != 0 );
            break;
        }
    }
#else
    Unreferenced( hpid );
    Unreferenced( lpaddr );
#endif // !TARGET32
    return xosd;
}
#pragma optimize ("", on)

XOSD
SetCacheFromEmi (
    HPID hpid,
    LPADDR lpaddr
    )
{
    int iemic;

    assert ( emiAddr ( *lpaddr ) != 0 );

    for ( iemic = cemicMax - 2; iemic >= 0; iemic-- ) {

        rgemic [ iemic + 1 ] = rgemic [ iemic ];
    }

    rgemic [ 0 ].hpid = hpid;
    rgemic [ 0 ].hemi = emiAddr ( *lpaddr );
    rgemic [ 0 ].sel  = (WORD)GetAddrSeg ( *lpaddr );

    return xosdNone;
}


/*** CleanCacheOfEmi
 *
 *  Purpose:
 *              To purge the emi cache
 *
 *  Notes:
 *              The emi cache must be purged whenever a RegisterEmi
 *              is done.  Unpredicable results can occur otherwise.
 *
 */
XOSD
CleanCacheOfEmi (
    void
    )
{
    int iemic;

    for ( iemic = 0; iemic < cemicMax; iemic++ ) {

        rgemic [ iemic ].hpid = NULL;
        rgemic [ iemic ].sel  = 0;
    }

    return xosdNone;
}



XOSD
SetEmi (
    HPID   hpid,
    LPADDR lpaddr
    )
{
    XOSD xosd = xosdNone;

    if ( emiAddr ( *lpaddr ) == 0 ) {

        //if (ADDR_IS_REAL(*lpaddr)) {
        //    emiAddr( *lpaddr ) = (HEMI) hpid;
        //    return xosd;
        //}

        if ( ( xosd = SetEmiFromCache ( hpid, lpaddr ) ) == xosdContinue ) {

            xosd = FindEmi ( hpid, lpaddr );
            if ( xosd == xosdNone ) {
                SetCacheFromEmi ( hpid, lpaddr );
            }
        }

        assert ( emiAddr ( *lpaddr ) != 0 );
    }

    return xosd;
}





/*
   Note: We are not guaranteed that the incoming address is actually
    on an instruction boundary.  When this happens, we derive the
    boundary and send back the difference in the return value and
    the address of the instruction previous to the DERIVED instruction
    in the address.

    Thus there are three classes of returns -

        ==0 - The incoming address was in fact on an instruction boundary
        > 0 - The case noted above
        < 0 - Error value - the most common "error" is that there is
                no previous instruction.

        When the return value >= 0, *lpaddr contains the address of the
            previous instruction.
*/


#define doffMax 60

static HPID hpidGPI = NULL;
static BYTE rgbGPI [ doffMax ];
static ADDR addrGPI;

XOSD
GPIBuildCache (
    HPID hpid,
    HTID htid,
    LPADDR lpaddr
    )
{
    XOSD xosd   =  xosdBadAddress;
    int  fFound =  FALSE;
    ADDR addr   = *lpaddr;
    ADDR addrT;
    int  ib = 0;


    _fmemset ( rgbGPI, 0, doffMax );

    addrGPI = *lpaddr;
    hpidGPI = hpid;

    GetAddrOff ( addr ) -= (int) min ( (UOFFSET) doffMax, GetAddrOff ( *lpaddr ) );

    while ( !fFound && GetAddrOff ( addr ) < GetAddrOff ( *lpaddr ) ) {
        SDI  sdi;

        sdi.dop    = dopNone;
        sdi.addr   = addr;

        addrT = addr;

        Disasm ( hpid, htid, &sdi );

        addr = sdi.addr;

        rgbGPI [ ib ] = (BYTE) ( GetAddrOff ( addrGPI ) - GetAddrOff ( addr ) );

        if ( GetAddrOff ( addr ) == GetAddrOff ( *lpaddr ) ) {
            xosd   = xosdNone;
            *lpaddr= addrT;
            fFound = TRUE;
        }

        ib += 1;
    }

    // We haven't synced yet, so *lpaddr is probably pointing
    //  to something that isn't really synchronous

    if ( !fFound ) {
        xosd   = (XOSD) ( GetAddrOff ( *lpaddr ) - GetAddrOff ( addrT ) );
        GetAddrOff ( *lpaddr ) -= xosd;
        if ( GetAddrOff ( *lpaddr ) != 0 ) {
            (void) GetPrevInst ( hpid, htid, lpaddr );
        }
    }

    return xosd;
}


VOID
GPIShiftCache (
    LPADDR lpaddr,
    int *pib
    )
{
    int doff = (int) ( GetAddrOff ( addrGPI ) - GetAddrOff ( *lpaddr ) );
    int ib   = 0;

    *pib = 0;
    while ( ib < doffMax && rgbGPI [ ib ] != 0 ) {
        rgbGPI [ ib ] = (BYTE) max ( (int) rgbGPI [ ib ] - doff, 0 );

        if ( rgbGPI [ ib ] == 0 && *pib == 0 ) {
            *pib = ib;
        }

        ib += 1;
    }

    addrGPI = *lpaddr;
}

XOSD
GPIUseCache (
    HPID hpid,
    HTID htid,
    LPADDR lpaddr
    )
{
    XOSD xosd   =  xosdBadAddress;
    int  fFound =  FALSE;
    ADDR addr   = *lpaddr;
    int  ib     =  0;
    int  ibCache=  0;
    int  ibMax  =  0;
    BYTE rgb [ doffMax ];


    GPIShiftCache ( lpaddr, &ibMax );

    _fmemset ( rgb, 0, doffMax );

    GetAddrOff ( addr ) -= (int) min ( (UOFFSET) doffMax, GetAddrOff ( *lpaddr ) );

    while ( !fFound && GetAddrOff ( addr ) < GetAddrOff ( *lpaddr ) ) {
        ADDR addrT;
        BYTE doff = (BYTE) ( GetAddrOff ( *lpaddr ) - GetAddrOff ( addr ) );

        // Attempt to align with the cache

        while ( doff < rgbGPI [ ibCache ] ) {
            ibCache += 1;
        }

        if ( doff == rgbGPI [ ibCache ] ) {

            // We have alignment with the cache

            addr  = *lpaddr;
            addrT = addr;
            GetAddrOff ( addrT ) -= rgbGPI [ ibMax - 1 ];
        }
        else {
            SDI  sdi;

            sdi.dop = dopNone;
            sdi.addr = addr;
            addrT = addr;

            Disasm ( hpid, htid, &sdi );


            addr = sdi.addr;

            rgb [ ib ] = (BYTE) ( GetAddrOff ( addrGPI ) - GetAddrOff ( addr ) );

            ib += 1;
        }

        if ( GetAddrOff ( addr ) == GetAddrOff ( *lpaddr ) ) {
            xosd   = xosdNone;
            *lpaddr= addrT;
            fFound = TRUE;
        }

    }

    // Rebuild the cache

    _fmemmove ( &rgbGPI [ ib - 1 ], &rgbGPI [ ibCache ], ibMax - ibCache );
    _fmemcpy  ( rgbGPI, rgb, ib - 1 );

    return xosd;
}

XOSD
GetPrevInst (
    HPID hpid,
    HTID htid,
    LPADDR lpaddr
    )
{

    if ( GetAddrOff ( *lpaddr ) == 0 ) {

        return xosdBadAddress;
    }
    else if (
        hpid == hpidGPI &&
        GetAddrSeg ( *lpaddr ) == GetAddrSeg ( addrGPI ) &&
        GetAddrOff ( *lpaddr ) <  GetAddrOff ( addrGPI ) &&
        GetAddrOff ( *lpaddr ) >  GetAddrOff ( addrGPI ) - doffMax / 2
    ) {

        return GPIUseCache ( hpid, htid, lpaddr );
    }
    else {

        return GPIBuildCache ( hpid, htid, lpaddr );
    }
}


//
// Return xosdContinue if overlay is loaded
// Else return xosdNone
//
XOSD
FLoadedOverlay(
    HPID   hpid,
    LPADDR lpaddr
    )
{
    XOSD    xosd = xosdContinue;
    Unreferenced( hpid );
    Unreferenced( lpaddr );
    return xosd;
}



XOSD
SetupExecute(
    HPID       hpid,
    HTID       htid,
    LPHIND     lphind
    )
/*++

Routine Description:

    This function is used to set up a thread for doing function evaluation.
    The first thing it will do is to

Arguments:

    argument-name - Supplies | Returns description of argument.
    .
    .

Return Value:

    return-value - Description of conditions needed to return value. - or -
    None.

--*/

{
    HLLI                        hlli;
    HTHD                        hthd;
    HPRC                        hprc;
    LPTHD                       lpthd;
    LP_EXECUTE_OBJECT_EM        lpeo;
    XOSD                        xosd;

    hprc = ValidHprcFromHpid(hpid);
    if (!hprc) {
        return xosdBadProcess;
    }
    hthd = HthdFromHtid(hprc, htid);
    if (!hthd || hthd == hthdInvalid) {
        return xosdBadThread;
    }

    /*
     *  If the list of execute objects has not yet been setup then it
     *  needs to be setup now.
     */

    if (HllEo == 0) {
        HllEo = LLInit(sizeof(EXECUTE_OBJECT_EM), llfNull, NULL, NULL);
    }

    /*
     *  Allocate an execute object for this working item.
     */

    if ((hlli = LLCreate( HllEo )) == 0) {
        return xosdOutOfMemory;
    }
    lpeo = (LP_EXECUTE_OBJECT_EM) LLLock( hlli );
    lpeo->regs = MHAlloc(SizeOfContext(hpid));

    /*
     *  Ask the DM to allocate a handle on its end for its low level
     *  execute object.
     */

    GuardTL();
    xosd = SendRequest(dmfSetupExecute, hpid, htid );

    if (xosd != xosdNone) {
        LLUnlock( hlli );
        LLDelete( HllEo, hlli );
        ReleaseTL();
        return xosd;
    }

    lpeo->heoDm = *(HIND *) LpDmMsg->rgb;
    ReleaseTL();

    /*
     *  Get the current register set for the thread on which we are going
     *  to do the exeucte.
     */

    lpthd = (LPTHD) LLLock( hthd );

    lpeo->hthd = hthd;

    if (!( lpthd->drt & drtAllPresent )) {
        UpdateRegisters( hprc, hthd );
    }

    _fmemcpy( lpeo->regs, lpthd->regs, SizeOfContext(hpid));

    LLUnlock( hthd );

    /*
     *  Unlock the execute object and return its handle
     */

    LLUnlock( hlli );

    *lphind =  (HIND)hlli;

    return xosdNone;
}                               /* SetupExecute() */



XOSD
StartExecute(
    HPID       hpid,
    HIND       hind,
    LPEXECUTE_STRUCT lpes
    )

/*++

Routine Description:

    description-of-function.

Arguments:

    argument-name - Supplies | Returns description of argument.
    .
    .

Return Value:

    return-value - Description of conditions needed to return value. - or -
    None.

--*/

{
    XOSD                        xosd;
    LP_EXECUTE_OBJECT_EM        lpeo;
    HTHD                        hthd;
    HTID                        htid;
    HPRC                        hprc;

    hprc = ValidHprcFromHpid(hpid);
    if (!hprc) {
        return xosdBadProcess;
    }

    lpeo = (LP_EXECUTE_OBJECT_EM) LLLock( (HLLE)hind );
    hthd= lpeo->hthd;
    htid = HtidFromHthd(hthd),
    lpes->hindDm = lpeo->heoDm;
    FixupAddr(hpid, htid, &lpes->addr);
    LLUnlock( (HLLE)hind );

    /*
     *  Cause any changes to registers to be written back
     */

    UpdateChild(hpid, htid, dmfGo);

    /*
     *  Issue the command to the DM
     */

    xosd = SendRequestX(dmfStartExecute, hpid, htid,
                        sizeof(EXECUTE_STRUCT), lpes);


    return xosd;
}                               /* StartExecute() */



XOSD
CleanUpExecute(
    HPID hpid,
    HIND hind
    )

/*++

Routine Description:

    description-of-function.

Arguments:

    argument-name - Supplies | Returns description of argument.
    .
    .

Return Value:

    return-value - Description of conditions needed to return value. - or -
    None.

--*/

{
    LPTHD                       lpthd;
    LP_EXECUTE_OBJECT_EM        lpeo;

    lpeo = (LP_EXECUTE_OBJECT_EM) LLLock( (HLLE)hind );

    lpthd = (LPTHD) LLLock( lpeo->hthd );

    _fmemcpy( lpthd->regs, lpeo->regs, SizeOfContext(hpid));

    lpthd->drt = (DRT) (drtAllPresent | drtCntrlPresent | drtAllDirty | drtCntrlDirty);

    SendRequestX(dmfCleanUpExecute, hpid, HtidFromHthd(lpeo->hthd),
                 sizeof(HIND), &lpeo->heoDm);

    LLUnlock( (lpeo->hthd) );
    LLUnlock( (HLLE)hind );

    LLDelete( HllEo, (HLLE)hind );

    return xosdNone;

}                               /* CleanUpExecute() */


void
UpdateNLGStatus(
    HPID    hpid,
    HTID    htid
    )
{
    HPRC    hprc = HprcFromHpid (hpid);
    LPPRC   lpprc = (LPPRC)LLLock (hprc);
    HMDI    hmdi;
    XOSD    xosd = xosdNone;


    while ( hmdi = LLFind (lpprc->llmdi, NULL, NULL, emdiNLG )) {
        LPMDI   lpmdi = (LPMDI) LLLock ( hmdi );
        NLG     nlg = lpmdi->nlg;

        FixupAddr ( hpid, htid, &nlg.addrNLGDispatch );
        FixupAddr ( hpid, htid, &nlg.addrNLGDestination );
        FixupAddr ( hpid, htid, &nlg.addrNLGReturn );

        xosd = SendRequestX(dmfNonLocalGoto,
                            HpidFromHprc ( hprc ),
                            NULL,
                            sizeof ( nlg ),
                            &nlg
                            );

        lpmdi->fSendNLG = FALSE;
        LLUnlock ( hmdi );
    }

    LLUnlock (hprc);
}



XOSD FAR PASCAL
EMFunc (
    EMF  emf,
    HPID hpid,
    HTID htid,
    DWORD wValue,
    LONG lValue
    )

/*++

Routine Description:

    This is the main dispatch routine for processing of commands to the
    execution model.

Arguments:

    emf    - Supplies the function to be performed. (Execution Model Function )
    hpid   - Supplies The process to be used.
    htid   - Supplies The thread to be used.
    wValue - Supplies Info about the command
    lValue - Supplies Info about the command

Return Value:

    returns an XOSD error code

Other:

       Hpid and htid can never be invalid.  In some cases, they can be
       null.  The entries under P and T marked with an 0 indicate that
       the null value is valid for this function, an X indicates that
       it is invalid.

       Brief descriptions of the wValue and lValue


       EMF                 P   T   WVALUE          LVALUE

       emfGo               X   X   ----            ----
       emfShowDebuggee     X   0   ----            ----
       emfStop             X   0   ----            ----
       emfWriteBuf         X   0   #of bytes       pointer to buffer
       emfReadBuf          X   0   #of bytes       pointer to buffer
       emfSingleStep       X   X   ----            -----
       emfStepOver         X   X   ----            ----
       emfSetBreakPoint    X   X   ----            ----
       emfRemoveBreakPoint X   X   ----            ----
       emfSetWatchPoint    X   X   ----            ----
       emfRemoveWatchPoint X   X   ----            ----
       emfRangeStep,
       emfRangeOver,
       emfThreadStatus     X   X   ----            pointer to status buf.
       emfProcStatus       X   X   ----            pointer to status buf.
       emfFreeze           X   X   ----            ----
       emfThaw             X   X   ----            ----
       emfRegisterDBF      0   0   ----            pointer to dbf
       emfInit             0   0   ----            pointer to em serv.
       emfUnInit           0   0   ----            ----
       emfCreatePid        X   0   ----            ----
       emfDestroyPid       X   0   ----            ----
       emfDestroyTid       X   X   ----            ----
       emfDestroy          0   0   hem             ----
       emfIsValid          X   X   hem             ----
       emfSetAddr          X   X   ----            pointer to addr
       emfGetAddr          X   X   ----            pointer to addr
       emfRegValue         X   X   register index  pointer to buffer
       emfSetReg           X   X   register index  pointer to buffer
       emfSetFrameContext  X   X   frame
       emfFrameRegValue    X   X   register index  pointer to buffer
       emfFrameSetReg      X   X   register index  pointer to buffer
       emfSpawnOrphan      X   0
       emfProgramLoad      X   0   length          pntr to cmd line
       emfProgramFree      X   0   ----            ----
       emfDebugPacket      X   X   ----            pointer to buffer
       emfMetric           X   0   ----            pointer to metric
       emfUnassemble       X   X   ----            pointer to buffer
       emfAssemble         X   X   ----            pointer to buffer
       emfGetObjLength     X   X   ----            pointer to addr
       emfIOCTL            X   X   IOCTL type      pointer to data
       emfGetRegStruct     0   0   register index  pointer to buffer
       emfGetFlagStruct    0   0   flag index      pointer to buffer
       emfGetFlag          X   X   flag index      pointer to buffer
       emfSetFlag          X   X   flag index      pointer to data
       emfIsStackSetup     X   X   ----            pointer to addr
       emfCompareAddr      ?   ?   ----            pointerr to rglpaddr[2]
       emfSetupExecute     X   X   ----            pointer to handle
       emfStartExecute     X   -   Handle          pointer to execute_struct
       emfCleanUpExecute   X   0   Handle          -----
       emfLoadDllAck       X   0   ----            -----
       emfUnLoadDllAck     X   0   ----            pointer to MDI
       emfAttach           X   0   ----
       emfStackWalkSetup   X   X   PC In Prolog    pointer to stack walk data
       emfStackWalkNext    X   X   ----            pointer to stack walk data
       emfStackWalkCleanup X   X   ----            pointer to stack walk data
       emfDebugActive      X   0   ----            LPDBG_ACTIVE_STRUCT
       emfConnect          X   0   ----            ----
       emfDisconnect       X   0   ----            ----
       emfEnableCache      X   0   ----            ----
       emfGetMemInfo       X   0   sizeof MEMINFO  LPMEMINFO

--*/

{
    XOSD xosd = xosdNone;

    switch ( emf ) {

    default:
        assert ( FALSE );
        xosd = xosdUnknown;
        break;

    case emfShowDebuggee :

        xosd = SendRequestX ( dmfSelect, hpid, htid, sizeof ( BOOL ), &wValue );
        break;

    case emfStop:

        xosd = SendRequestX ( dmfStop, hpid, htid, sizeof(BOOL), &wValue );
        break;

    case emfRegisterDBF:

        InitUsage ( );
        lpdbf = (LPDBF) lValue;
        break;

    case emfInit:

        llprc = LLInit ( sizeof ( PRC ), llfNull, PiDKill, PDComp );

        CallDB = ( (LPEMCB) lValue)->lpfnCallBackDB;
        CallTL = ( (LPEMCB) lValue)->lpfnCallBackTL;
        CallNT = ( (LPEMCB) lValue)->lpfnCallBackNT;

        LpDmMsg = (LPDM_MSG) MHAlloc ( CBBUFFERDEF );
        CbDmMsg = CBBUFFERDEF;
        CallTL ( tlfSetBuffer, hpid, CBBUFFERDEF, LpDmMsg );

        break;

    case emfUnInit:

        /*
         * do any uninitialization for the EM itself
         */
        FreeEmErrorStrings ();

        CleanupDisassembler();
        MHFree(LpDmMsg);
        LpDmMsg = NULL;
        CbDmMsg = 0;
        break;

    case emfSetAddr:

        xosd = SetAddr( hpid, htid, (ADR) wValue, (LPADDR) lValue );
        break;

    case emfGetAddr:

        xosd = GetAddr( hpid, htid, (ADR) wValue, (LPADDR) lValue );
        break;

    case emfSpawnOrphan:

        xosd = SpawnOrphan ( hpid, wValue, (LPSOS) lValue);
        break;

    case emfProgramLoad:

        xosd = ProgramLoad ( hpid, wValue, (LPPRL) lValue );
        break;

    case emfProgramFree:

        xosd = ProgramFree ( hpid, htid );
        break;

    case emfFixupAddr:

        xosd = FixupAddr ( hpid, htid, (LPADDR) lValue );
        break;

    case emfUnFixupAddr:

        xosd = UnFixupAddr ( hpid, htid, (LPADDR) lValue );
        break;

    case emfSetEmi:

        xosd = SetEmi ( hpid, (LPADDR) lValue );
        break;

    case emfMetric:

        xosd = DebugMetric ( hpid, htid, wValue, (LPLONG) lValue );
        break;

    case emfDebugPacket:
    {
        LPRTP lprtp = (LPRTP) lValue;

        xosd = DebugPacket(
                     lprtp->dbc,
                     lprtp->hpid,
                     lprtp->htid,
                     lprtp->cb,
                     (lprtp->cb == 0 ) ? NULL : (LPBYTE) lprtp->rgbVar
                     );
        break;
    }

    case emfSetMulti:

        xosd  = SendRequest ( (DMF) (wValue ? dmfSetMulti : dmfClearMulti),
                                                                  hpid, htid );
        break;

    case emfDebugger:

        xosd = SendRequestX ( dmfDebugger, hpid, htid, wValue, (LPVOID) lValue );
        break;

    case emfRegisterEmi:

        RegisterEmi ( hpid, htid, (LPREMI) lValue );
        break;

    case emfGetModel:
        *(WORD FAR *)lValue = CEXM_MDL_native;
        break;

    case emfGetRegStruct:
        if (wValue >= CRgrd(hpid)) {
            xosd = xosdInvalidParameter;
        } else {
            //
            // v-vadimp - check if the register is present, and mark invisible if not
            //            
            HPRC hprc = ValidHprcFromHpid (hpid);
            LPPRC lpprc = (LPPRC) LLLock (hprc);
            DWORD dwProcessorFlags = lpprc->dmi.Processor.Flags;
            LLUnlock (hprc);
            RD rd = Rgrd(hpid)[wValue];
            if (((rd.rt & rtProcessorMask) == rtMMX) && !(dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX)) {
                rd.rt |= rtInvisible;
            } else if (((rd.rt & rtProcessorMask) == rtKatmai) && !(dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX2)) {
                rd.rt |= rtInvisible;
            } else if (((rd.rt & rtProcessorMask) == rt3DNow) && !(dwProcessorFlags & PROCESSOR_FLAGS_I386_3DNOW)) {
                rd.rt |= rtInvisible;
            } else if (((rd.rt & rtProcessorMask) == rtWillamette) && !(dwProcessorFlags & PROCESSOR_FLAGS_I386_MMX3)) {
                rd.rt |= rtInvisible;
            }
            *((RD FAR *) lValue) = rd;

        }
        break;

    case emfGetFlagStruct:

        if (wValue >= CRgfd(hpid)) {
            xosd = xosdInvalidParameter;
        } else {
            *((FD FAR *) lValue) = Rgfd(hpid)[wValue].fd;
        }
        break;

    case emfGetFlag:
        xosd = GetFlagValue( hpid, htid, wValue, (LPVOID) lValue );
        break;

    case emfSetFlag:
        xosd = SetFlagValue( hpid, htid, wValue, (LPVOID) lValue );
        break;

    case emfSetupExecute:
        xosd = SetupExecute(hpid, htid, (LPHIND) lValue);
        break;

    case emfStartExecute:
        xosd = StartExecute(hpid, (HIND) wValue, (LPEXECUTE_STRUCT) lValue);
        break;

    case emfCleanUpExecute:
        xosd = CleanUpExecute(hpid, (HIND) wValue);
        break;

    case emfSetPath:
        xosd = SetPath (hpid, htid, wValue, (LPTSTR)lValue);
        break;

    case emfGo:
        xosd = Go(hpid, htid, (LPEXOP)lValue);
        break;

    case emfSingleStep:
        xosd = SingleStep ( hpid, htid, (LPEXOP)lValue );
        break;

    case emfRangeStep:
        xosd = RangeStep(hpid, htid, (LPRSS)lValue);
        break;

    case emfReturnStep:
        xosd = ReturnStep(hpid, htid, (LPEXOP)lValue);
        break;

    case emfWriteMemory:
    {
        LPRWMS lprwms = (LPRWMS)lValue;
        xosd = WriteBufferCache ( hpid, htid, lprwms->lpaddr, lprwms->cbBuffer,
                                               (LPBYTE) lprwms->lpbBuffer, lprwms->lpcb);
        break;
    }

    case emfReadMemory:
    {
        LPRWMS lprwms = (LPRWMS)lValue;
        xosd = ReadBuffer ( hpid, htid, lprwms->lpaddr, lprwms->cbBuffer,
                                               (LPBYTE) lprwms->lpbBuffer, lprwms->lpcb);
        break;
    }

    case emfGetMemoryInfo:
        xosd = GetMemoryInfo(hpid, htid, (LPMEMINFO)lValue);
        break;

    case emfBreakPoint:
        xosd = HandleBreakpoints( hpid, wValue, lValue );
        break;

    case emfProcessStatus:
        xosd = ProcessStatus(hpid, (LPPST)lValue);
        break;

    case emfThreadStatus:
        xosd = ThreadStatus(hpid, htid, (LPTST)lValue);
        break;

    case emfGetExceptionState:
        xosd = GetExceptionState(hpid, htid, (EXCEPTION_CONTROL) wValue,
                                              (LPEXCEPTION_DESCRIPTION) lValue);
        break;

    case emfSetExceptionState:
        xosd = SetExceptionState(hpid, htid, (LPEXCEPTION_DESCRIPTION)lValue);
        break;

    case emfFreezeThread:
        xosd = FreezeThread(hpid, htid, wValue);
        break;

    case emfCreateHpid:
        xosd = CreateHprc ( hpid );

        if ( xosd == xosdNone ) {
            xosd = SendRequest ( dmfCreatePid, hpid, NULL );
            //
            //  We're allowed to have an HPID with no TL, so special-case
            //  the xosdInvalidParameter return code, which is returned
            //  by CallTL if there is no TL.
            //
            if ( xosd == xosdInvalidParameter ) {
                xosd = xosdNone;
            }
        }

        break;

    case emfDestroyHpid:
        {
            HPRC hprc = HprcFromHpid(hpid);
            xosd = SendRequest ( dmfDestroyPid, hpid, NULL );
            if ( xosd == xosdLineNotConnected || xosd == xosdInvalidParameter ) {
                //
                //  xosdLineNotConnected:  Communication line broke, we'll
                //      ignore this error.
                //
                //  xosdInvalidParameter:  We're allowed to have an HPID with
                //      no TL, so special case this return code, which is
                //      returned by CallTL if there is no TL.
                //
                xosd = xosdNone;
            }
            DestroyHprc ( hprc );
        }
        break;

    case emfDestroyHtid:
        {
            HPRC hprc = HprcFromHpid(hpid);
            DestroyHthd( HthdFromHtid( hprc, htid ));
        }
        break;

    case emfUnassemble:
        Disasm ( hpid, htid, (LPSDI) lValue );
        break;

    case emfAssemble:
        xosd = Assemble ( hpid, htid, (LPADDR) wValue, (LPTSTR) lValue );
        break;

    case emfGetReg:
        xosd = GetRegValue( hpid, htid, wValue, (LPVOID) lValue );
        break;

    case emfSetReg:
        xosd = SetRegValue( hpid, htid, wValue, (LPVOID) lValue );
        break;

    case emfDebugActive:
		xosd = DebugActive( hpid, wValue, (LPVOID) lValue );
        break;

    case emfGetMessageMap:
        *((LPMESSAGEMAP *)lValue) = &MessageMap;
        break;

    case emfGetMessageMaskMap:
        *((LPMASKMAP *)lValue) = &MaskMap;
        break;

    case emfGetModuleList:
        xosd = GetModuleList( hpid, htid, (LPTSTR)wValue,
                                                 (LPMODULE_LIST FAR *)lValue );
        break;

    case emfCompareAddrs:
        xosd = CompareAddrs( hpid, htid, (LPCAS) lValue );
        break;

    case emfSetDebugMode:
#pragma message("Do something intelligent with emfSetDebugMode")
        xosd = xosdNone;
        break;

    case emfUnRegisterEmi:
        xosd = UnLoadFixups (hpid, (HEMI)lValue);
        break;

    case emfGetFrame:
        xosd = GetFrame(hpid, htid, wValue, lValue);
        break;

    case emfGetObjLength:
        xosd = GetObjLength(hpid, (LPGOL)lValue);
        break;

    case emfGetFunctionInfo:
        xosd = GetFunctionInfo(hpid, (LPGFI)lValue);
        break;

    case emfGetPrevInst:
        xosd = BackDisasm(hpid, htid, (LPGPIS)lValue );
        break;

    case emfConnect:
        if (!LoadDmStruct.lpDmName) {
            LoadDmStruct.lpDmName = MHStrdup(DEFAULT_DMNAME);
        }
        if (!LoadDmStruct.lpDmParams) {
            LoadDmStruct.lpDmParams = MHStrdup(DEFAULT_DMPARAMS);
        }
        xosd = CallTL( tlfLoadDM, hpid, sizeof(LOADDMSTRUCT), &LoadDmStruct);
        if (xosd == xosdNone) {
            xosd = SendRequest( dmfInit, hpid, htid );
        }
        break;

    case emfDisconnect:
        xosd = SendRequest( dmfUnInit, hpid, htid );
        break;

    case emfInfoReply:
        CallTL ( tlfReply, hpid, wValue, (LPVOID)lValue );
        xosd = xosdNone;
        break;

    case emfContinue:
        UpdateNLGStatus (hpid, htid);
        xosd = DoContinue( hpid );
        break;

    case emfSystemService:
        xosd = SystemService( hpid, htid, wValue, (LPSSS)lValue );
        break;

    case emfGetTimeStamp:
        xosd = GetTimeStamp (hpid, htid, (LPTCS) lValue);
        break;

	case emfCreateUserCrashDump:
		xosd = EMCreateUserCrashDump (hpid, htid, (LPTSTR) lValue);
		break;

    case emfSetup:

        {
            LPEMSS lpemss = (LPEMSS)lValue;

            // DWORD fLoad;
            // DWORD fInteractive;
            // DWORD fSave;
            // LPVOID lpvPrivate;
            // LPARAM lParam;
            // LPGETSETPROFILEPROC lpfnGetSet;
            //
            // typedef LONG (OSDAPI * LPGETSETPROFILEPROC)(
            //     LPSTR lpName,
            //     LPSTR lpValue,
            //     LPARAM lParam,
            //     DWORD fSet
            //     );

#if 0
            if (lpemss->fLoad) {
                lpemss->lpfnGetSet("DmName", String, lpemss->lParam, FALSE);
                if (LoadDmStruct.lpDmName) {
                    MHFree(LoadDmStruct.lpDmName);
                }
                LoadDmStruct.lpDmName = MHStrdup(String);

                lpemss->lpfnGetSet("DmParams", String, lpemss->lParam, FALSE);
                if (LoadDmStruct.lpDmParams) {
                    MHFree(LoadDmStruct.lpDmParams);
                }
                LoadDmStruct.lpDmParams = MHStrdup(String);
            }

            if (lpemss->fInteractive) {
            }

            if (lpemss->fSave) {
                lpemss->lpfnGetSet("DmName", LoadDmStruct.lpDmName, lpemss->lParam, FALSE);
                lpemss->lpfnGetSet("DmParams", LoadDmStruct.lpDmParams, lpemss->lParam, FALSE);
            }
#endif


            //
            // BUGBUG kentf this is a hackomatic version of emfSetup:
            //
            if (LoadDmStruct.lpDmName) {
                MHFree(LoadDmStruct.lpDmName);
                LoadDmStruct.lpDmName = 0;
            }
            if (LoadDmStruct.lpDmParams) {
                MHFree(LoadDmStruct.lpDmParams);
                LoadDmStruct.lpDmParams = 0;
            }

            if (lpemss->lpvPrivate) {
                LPTSTR p = (LPTSTR) lpemss->lpvPrivate;
                while (_istspace(*p)) {
                    p = _tcsinc(p);
                }
                if (_ftcsnicmp(p, _T("DM="), 3) == 0 || _ftcsnicmp(p, _T("DM:"), 3) == 0) {
                    LoadDmStruct.lpDmName = MHStrdup(_ftcstok(p+3, _T(" \t")));
                    p = p + 3 + _ftcslen(LoadDmStruct.lpDmName) + 1;
                    while (_istspace(*p)) {
                        p = _tcsinc(p);
                    }
                }
                LoadDmStruct.lpDmParams = MHStrdup(p);
            }
        }
        break;

    }

    return xosd;
}                               /* EMFunc() */


/*
**
*/

int
WINAPI
DllMain(
    HINSTANCE hModule,
    DWORD  dwReason,
    DWORD  dwReserved
    )
{
    Unreferenced(dwReserved);

    switch (dwReason) {

      case DLL_THREAD_ATTACH:
      case DLL_THREAD_DETACH:
        break;

      case DLL_PROCESS_DETACH:

        if (LpSendBuf) {
            MHFree(LpSendBuf);
            CbSendBuf = 0;
        }
        if (LpDmMsg) {
            MHFree(LpDmMsg);
            CbDmMsg = 0;
        }

        if (LoadDmStruct.lpDmName) {
            MHFree(LoadDmStruct.lpDmName);
            LoadDmStruct.lpDmName = 0;
        }
        if (LoadDmStruct.lpDmParams) {
            MHFree(LoadDmStruct.lpDmParams);
            LoadDmStruct.lpDmParams = 0;
        }

        CleanupDisassembler();

        DeleteCriticalSection(&csCache);
        DeleteCriticalSection(&csInTL);
        break;

      case DLL_PROCESS_ATTACH:

        InitializeCriticalSection(&csCache);
        InitializeCriticalSection(&csInTL);
                DisableThreadLibraryCalls(hModule);
        break;
    }

	hInstance = hModule;
    
	return TRUE;
}



XOSD
DebugPacket (
    DBC dbc,
    HPID hpid,
    HTID htid,
    DWORD wValue,
    LPBYTE lpb
    )
{
    XOSD        xosd = xosdContinue;
    HPRC        hprc = HprcFromHpid ( hpid );
    HTHD        hthd = HthdFromHtid ( hprc, htid );
    LONG        emdi;
    LPTHD       lpthd;
    LPPRC       lpprc = (LPPRC) LLLock(hprc);
    HTHD        hthdNext;

    for ( hthdNext = LLNext ( lpprc->llthd, hthdNull );
          hthdNext != hthdNull;
          hthdNext = LLNext ( lpprc->llthd, hthdNext ) ) {
        lpthd = (LPTHD) LLLock(hthdNext);
        lpthd->drt = drtNonePresent;
        LLUnlock(hthdNext);
    }

    LLUnlock(hprc);

    /* Do any preprocessing on the packet before sending the notification
     * on to the debugger.  For example, the wValue and lValue might need
     * some munging.  Also, if the notification shouldn't be passed on to
     * the debugger, then set xosd = xosdNone or some other value other
     * than xosdContinue.
     */

    switch ( dbc ) {
    case dbceAssignPID:
        {
            LPPRC lpprc = (LPPRC) LLLock ( hprc );

            assert ( wValue == sizeof ( PID ) );
            lpprc->pid = *( (PID FAR *) lpb );
            lpprc->stat = statStarted;
            LLUnlock ( hprc );
        }
        xosd = xosdNone;
        break;

    case dbcCreateThread:
        lpprc = (LPPRC) LLLock(hprc);
        lpprc->fRunning = FALSE;
        LLUnlock(hprc);

//		GetExceptionInfo (hpid);
        assert ( wValue == sizeof ( TCR ) );
        xosd = CreateThreadStruct ( hpid, *( (TID FAR *) lpb ), &htid );
        if ( ((LPTCR) lpb)->uoffTEB != 0 ) {
            HTHD  hthdT = HthdFromHtid ( hprc, htid );
            LPTHD lpthd = (LPTHD) LLLock ( hthdT );

            lpthd->uoffTEB = ((LPTCR) lpb)->uoffTEB;
            LLUnlock ( hthdT );
        }

        CallTL ( tlfReply, hpid, sizeof ( HTID ), (LPVOID)&htid );
        if ( xosd == xosdNone ) {
            xosd = xosdContinue;
        }
        break;

    case dbcNewProc:
        {
            HPRC  hprcT;
            HPID  hpidT;
            LPPRC lpprc;
            LPNPP lpnpp;

            /*
             * lpb points to an NPP (New Process Packet).  The PID is
             * the PID of the debuggee; fReallyNew indicates if this is
             * really a new process or if it already existed but hasn't
             * been seen before by OSDebug.
             */

            assert ( wValue == sizeof(NPP) );
            lpnpp = (LPNPP) lpb;

            // See EMCallBackDB in od.c

            CallDB ( dbcoNewProc, hpid, htid, CEXM_MDL_native,
                    sizeof ( HPID ), (LPVOID)&hpidT );

            (void) CreateHprc ( hpidT );

            hprcT       = HprcFromHpid ( hpidT );
            lpprc       = (LPPRC) LLLock ( hprcT );
            lpprc->pid  = lpnpp->pid;
            lpprc->stat = statStarted;
            LLUnlock ( hprcT );

            CallTL ( tlfReply, hpid, sizeof ( HPID ), (LPVOID)&hpidT );

            GetExceptionInfo( hpidT );

            wValue = (UINT)hpidT;
            lpb = (LPBYTE) (LONG) lpnpp->fReallyNew;
            if ( xosd == xosdNone ) {
                xosd = xosdContinue;
            }
        }
        break;


    case dbcThreadTerm:
        lpprc = (LPPRC) LLLock(hprc);
        lpprc->fRunning = FALSE;
        LLUnlock(hprc);
        lpthd = (LPTHD) LLLock(hthd);
        lpthd->fRunning = FALSE;
        LLUnlock(hthd);

    case dbcProcTerm:

        /*
         * For both of these notifications, the incoming wValue is
         * sizeof(ULONG), and lpb contains a ULONG which is the exit
         * code of the process or thread.  For the debugger, set
         * wValue = 0 and lValue = exit code.
         */

        assert ( wValue == sizeof(ULONG) );
        wValue = 0;
        lpb = (LPBYTE) (*(ULONG*)lpb);
        break;

    case dbcDeleteThread:

        lpthd = (LPTHD) LLLock(hthd);
        lpthd->tid    = (TID)-1;
        LLUnlock(hthd);

        assert ( wValue == sizeof(ULONG) );
        wValue = 0;
        lpb = (LPBYTE) (*(ULONG*)lpb);
        break;

    case dbcModLoad:
        lpprc = (LPPRC) LLLock(hprc);
        lpprc->fRunning = FALSE;
        LLUnlock(hprc);
        xosd = LoadFixups ( hpid, (LPMODULELOAD) lpb );
        break;

    case dbcModFree:            /* Should use dbceModFree*               */
        assert(FALSE);
        break;

    case dbceModFree32:
        emdi = emdiBaseAddr;
    modFree:
        {
            HMDI    hmdi;
            LPMDI   lpmdi;
            HLLI    llmdi;

            llmdi = LlmdiFromHprc ( hprc );
            assert( llmdi );

            hmdi = LLFind( llmdi, 0, lpb, emdi);

            ((LPPRC)LLLock(hprc))->fUnloadingModule = 1;
            LLUnlock(hprc);

            // If two dlls with the same name are loaded in the debuggee, this will
            // happen when we get the modFree for the second instance. The correct
            // fix for this is to make SHGetHexeFromName to not just depend on the basename
            // to decide the EMI. 

            if ( hmdi == NULL ) {
                UpdateNLGStatus(hpid, htid);
                xosd = DoContinue( hpid );
                return xosd;
            }
			
            lpmdi = (LPMDI) LLLock( hmdi );
            lpb = (LPBYTE) lpmdi->hemi;
            LLUnlock( hmdi );

            dbc = dbcModFree;
        }
        break;

    case dbceModFree16:
        emdi = emdiMTE;
        goto modFree;
        break;

    case dbcExecuteDone:
        lpprc = (LPPRC) LLLock(hprc);
        lpprc->fRunning = FALSE;
        LLUnlock(hprc);
        lpthd = (LPTHD) LLLock(hthd);
        lpthd->fRunning = FALSE;
        LLUnlock(hthd);
        break;

    case dbcStep:

    case dbcThreadBlocked:
    case dbcSignal:
    case dbcAsyncStop:
    case dbcBpt:
    case dbcEntryPoint:
    case dbcLoadComplete:
    case dbcCheckBpt:
        {
            LPBPR lpbpr = (LPBPR) lpb;
            LPTHD lpthd = (LPTHD) LLLock ( hthd );

            assert ( wValue == sizeof ( BPR ) );

            PurgeCache ( );
            lpprc = (LPPRC) LLLock(hprc);
            if (dbc != dbcCheckBpt) {
                lpprc->fRunning = FALSE;
                lpthd->fRunning = FALSE;
            }
            LLUnlock(hprc);

            CopyFrameRegs(hpid, lpthd, lpbpr);

            lpthd->fFlat         = lpbpr->fFlat;
            lpthd->fOff32        = lpbpr->fOff32;
            lpthd->fReal         = lpbpr->fReal;

            lpthd->drt = drtCntrlPresent;

            LLUnlock( hthd );

            wValue = lpbpr->dwNotify;
            lpb = NULL;
        }
        break;

    case dbcException:
        {
            LPEPR lpepr = (LPEPR) lpb;
            LPTHD lpthd = (LPTHD) LLLock ( hthd );

#if 0
            /*
              * This would be true if we did not pass parameters up
              */
            assert ( wValue == sizeof ( EPR ) );
#endif

            PurgeCache ( );
            lpprc = (LPPRC) LLLock(hprc);
            lpprc->fRunning = FALSE;
            LLUnlock(hprc);
            lpthd->fRunning = FALSE;


            CopyFrameRegs(hpid, lpthd, &lpepr->bpr);

            lpthd->fFlat        = lpepr->bpr.fFlat;
            lpthd->fOff32       = lpepr->bpr.fOff32;
            lpthd->fReal        = lpepr->bpr.fReal;

            lpthd->drt = drtCntrlPresent;

            LLUnlock( hthd );
        }
        break;

#if 0

    // BUGBUG kentf review this... do we need to support it?
    //              if so, it should turn into a dbcInfoAvail and
    //              a breakpoint or something.
    case dbcNtRip:
        {
            LPNT_RIP lprip   = (LPNT_RIP) lpb;
            LPTHD    lpthd   = LLLock ( hthd );
            ADDR     addr    = {0};

            assert ( wValue == sizeof ( NT_RIP ) );

            PurgeCache ( );
            lpprc = LLLock(hprc);
            lpprc->fRunning = FALSE;
            LLUnlock(hprc);
            lpthd->fRunning = FALSE;


            CopyFrameRegs(hpid, lpthd, lpbpr);

            lpthd->fFlat      = lprip->bpr.fFlat;
            lpthd->fOff32     = lprip->bpr.fOff32;
            lpthd->fReal      = lprip->bpr.fReal;

            lpthd->drt = drtCntrlPresent;

            LLUnlock (hthd );
        }
        break;
#endif

    case dbceCheckBpt:
        assert(FALSE);
        xosd = xosdNone;
        break;


    case dbcError:
        {
            static TCHAR sz[500];
            XOSD    xosdErr = *( (XOSD *)lpb );
            LPTSTR  str = (LPTSTR) (lpb + sizeof(XOSD));

            if (str[0]) {
                lpb = (LPBYTE) str;
            } else {
				LPCSTR lpszError = EmError (xosdErr);
				if (!lpszError) {
					assert (FALSE);
					lpszError = _T("Load failed for this error string.");
				}
                _stprintf(sz, _T("DM%04d: %s"), xosdErr, lpszError);
                lpb = (LPBYTE) sz;
            }
            wValue = xosdErr;
        }
        break;

    case dbceSegLoad:
        {
            SLI     sli;
            HMDI    hmdi;
            LPMDI   lpmdi;
            UINT    i;

            sli = *( (LPSLI) lpb );

            hmdi = LLFind( LlmdiFromHprc( hprc ), 0, &sli.mte,
                          (LONG) emdiMTE);

            assert( hmdi );

            lpmdi = (LPMDI) LLLock(hmdi );

            if (sli.wSegNo >= lpmdi->cobj) {
                i = lpmdi->cobj;
                lpmdi->cobj = sli.wSegNo+1;
                lpmdi->rgobjd = (OBJD *) MHRealloc(lpmdi->rgobjd,
                                        sizeof(OBJD)*lpmdi->cobj);
                memset(&lpmdi->rgobjd[i], 0, sizeof(OBJD)*(lpmdi->cobj - i));
            }
            lpmdi->rgobjd[ sli.wSegNo ].wSel = sli.wSelector;
            lpmdi->rgobjd[ sli.wSegNo ].wPad = 1;
            lpmdi->rgobjd[ sli.wSegNo ].cb = (DWORD) -1;

            LLUnlock( hmdi );

            //
            //  Let the shell know that a new segment was loaded, so it
            //  can try to instantiate virtual BPs.
            //
            xosd=CallDB( dbcSegLoad, hpid, htid, CEXM_MDL_native, 0,
                                                       (LPVOID)sli.wSelector );
            xosd=xosdNone;

        }
        break;

    case dbceSegMove:
        {
            SLI     sli;
            HMDI    hmdi;
            LPMDI   lpmdi;

            sli = *( (LPSLI) lpb );

            hmdi = LLFind( LlmdiFromHprc( hprc ), 0, &sli.mte,
                          (LONG) emdiMTE);

            assert( hmdi );

            lpmdi = (LPMDI) LLLock(hmdi );

            assert(sli.wSegNo > 0 );
            if (sli.wSegNo < lpmdi->cobj) {
                lpmdi->rgobjd[ sli.wSegNo - 1 ].wSel = sli.wSelector;
            }

            LLUnlock( hmdi );
        }
        break;

    case dbcCanStep:
        {
            CANSTEP CanStep;
            ADDR origAddr;

            assert ( wValue == sizeof ( ADDR ) );

            UnFixupAddr( hpid, htid, (LPADDR) lpb);
            origAddr = *(LPADDR)lpb;

            xosd=CallDB(dbc,hpid,htid,CEXM_MDL_native,0,(LPVOID)lpb);

            if ( xosd != xosdNone ) {
                CanStep.Flags = CANSTEP_NO;
            } else {
                CanStep = *((CANSTEP*)lpb);
                if (CanStep.Flags == CANSTEP_YES) {
                    AdjustForProlog(hpid, htid, &origAddr, &CanStep);
                }
            }

            CallTL ( tlfReply, hpid, sizeof( CanStep ), &CanStep );

            xosd = xosdNone;
        }
        break;

    case dbceGetOffsetFromSymbol:
        {
            ADDR addr = {0};
            if (SHGetPublicAddr ( &addr, (LSZ) lpb) && GetAddrOff(addr)) {
                FixupAddr(hpid, htid, &addr);
            }
            CallTL( tlfReply, hpid, sizeof(addr.addr.off), (LPVOID)&addr.addr.off );
            xosd = xosdNone;
        }
        break;

    case dbceGetSymbolFromOffset:
        {
            LPTSTR p;
#if defined(UNICODE) || defined(_UNICODE)
#pragma message("SHAddrToPublicName needs UNICODE api")
#endif
#ifdef NT_BUILD_ONLY
            ADDR addr;
            LPTSTR fname = (LPTSTR) SHAddrToPublicName( (LPADDR)lpb, &addr );
#else
            LPTSTR fname = (LPTSTR) SHAddrToPublicName( (LPADDR)lpb );
#endif
            HTID vhtid;

            GetFrame( hpid, htid, 1, (DWORD)&vhtid );
            lpthd = (LPTHD) LLLock(hthd);
            if (fname) {
                p = (LPTSTR) MHAlloc( (_ftcslen(fname) + 16) * sizeof(TCHAR) );
                _ftcscpy(p,fname);
                MHFree(fname);
            } else {
                p = (LPTSTR) MHAlloc( 32 );
                _stprintf( p, _T("<unknown>0x%08x"), GetAddrOff(*(LPADDR)lpb) );
            }
            fname = p;
            p += _ftcslen(p) + 1;
            *(UNALIGNED LPDWORD)p = lpthd->StackFrame.AddrReturn.Offset;
            LLUnlock(hthd);
            CallTL( tlfReply, hpid, (_ftcslen(fname)+1)*sizeof(TCHAR)+sizeof(DWORD), (LPVOID)fname );
            MHFree( fname );
            xosd = xosdNone;
        }
        break;

    case dbceEnableCache:
        EnableCache( hpid, htid, *(LPDWORD)lpb );
        CallTL( tlfReply, hpid, 0, NULL );
        xosd = xosdNone;
        break;

    case dbceGetMessageMask:
        {
            DWORD dwMsgMask = GetMessageMask( *(LPDWORD)lpb );
            CallTL( tlfReply, hpid, sizeof(DWORD), &dwMsgMask);
            xosd = xosdNone;
        }
        break;

	case dbceGetFrameWithSrc:
		{
			HTID	hvtid;
			ADDR	addr = {0};
			
			xosd = GetFrameWithSrc (hpid,
									htid,
									*((UINT*)lpb),
									&hvtid
									);

			if (xosd == xosdNone) {
				xosd = GetAddr (hpid, hvtid, adrPC, &addr);

				if (xosd == xosdNone) {
					xosd = FixupAddr (hpid, htid, &addr);
				}
			}
					
			if (xosd != xosdNone) {
				memset (&addr, 0, sizeof (addr));
			}
			
			CallTL (tlfReply, hpid, sizeof (addr), &addr);
		}
		break;

    case dbcLastAddr:
        assert( wValue == sizeof( ADDR ) );

        UnFixupAddr( hpid, htid, (LPADDR) lpb );

        xosd = CallDB(dbc, hpid, htid, CEXM_MDL_native, 0, (LPVOID) lpb);

        if ( xosd == xosdNone ) {
            FixupAddr( hpid, htid, (LPADDR) lpb );
        }

        CallTL( tlfReply, hpid, sizeof(ADDR), lpb);
        break;

    case dbceExceptionDuringStep:
        {
            DWORD cAddrsAllocated = 6, cbPacket;
            HTID vhtid = htid;
            LPEXHDLR lpexhdlr = (LPEXHDLR) MHAlloc(sizeof(EXHDLR)+sizeof(ADDR)*cAddrsAllocated);
            lpexhdlr->count = 0;
            GetFrameEH(hpid, htid, &lpexhdlr, &cAddrsAllocated);
            while (GetFrame(hpid, vhtid, 1, (DWORD)&vhtid)==xosdNone) {
                GetFrameEH(hpid, vhtid, &lpexhdlr, &cAddrsAllocated);
            }
            cbPacket = sizeof(EXHDLR)+(lpexhdlr->count * sizeof(ADDR));
            CallTL( tlfReply, hpid, cbPacket, (LPVOID) lpexhdlr);
            MHFree(lpexhdlr);
            xosd = xosdNone;
        }
        break;

    default:
        break;
    }

    if ((xosd == xosdContinue) && (dbc < dbcMax) && (dbc != dbcModLoad)) {
        xosd = CallDB ( dbc, hpid, htid, CEXM_MDL_native, wValue, (LPVOID)lpb );
    }

    switch ( dbc ) {

    case dbcProcTerm:
        {
            LPPRC lpprc = (LPPRC) LLLock ( hprc );
            lpprc->stat = statDead;
            LLUnlock ( hprc );
        }
        break;

    case dbcThreadTerm:
        {
            LPTHD lpthd = (LPTHD) LLLock ( hthd );
            lpthd->fVirtual = TRUE;
            LLUnlock ( hthd );
        }
        break;

    case dbcDeleteProc:
        break;

    case dbcDeleteThread:
        break;

        case dbcCheckBpt:
        case dbcCheckWatchPoint:
        case dbcCheckMsgBpt:
        {
            DWORD wContinue = xosd;
            xosd = xosdNone;
            CallTL( tlfReply, hpid, sizeof(wContinue), &wContinue);
        }
    }

    return xosd;
}                               /* DebugPacket() */


