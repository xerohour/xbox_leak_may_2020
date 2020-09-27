/*** brkptSY.c - breakpoint handlers called by callback system.
*
*   Copyright <C> 1989, Microsoft Corporation
*
*   Purpose:
*
*
*************************************************************************/
#include "stdafx.h"
#pragma hdrstop

#include "javadbg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/*** BPLOADVALUE
*
* Purpose:
*
* Input:
*
* Output:
*
* Exceptions:
*
*************************************************************************/
int PASCAL BPLoadValue ( LPBPI lpbpi ) {

    SHREG       SHREG;
    ADDR        addr;
    unsigned short  cb;

    if ( !lpbpi->lpDpi  || lpbpi->dpf.f.fEvalExpr ) {
        return FALSE;
    }

    _fmemset (&addr, 0, sizeof (ADDR));
    cb = lpbpi->lpDpi->cData * lpbpi->lpDpi->cbData;

    // if this is a register, get the register
    if( lpbpi->dpf.f.fReg ) {

        // read the register
        SHREG.hReg = lpbpi->lpDpi->iReg;
        if( DHGetReg( &SHREG, NULL ) ) {

            // copy the register to the current place
            _fmemcpy ( lpbpi->lpDpi->pValue, &SHREG.Byte1, lpbpi->lpDpi->cbData) ;
            return(TRUE);
        }
        return(FALSE);
    }

	if (pDebugCurr && pDebugCurr->IsJava()) {

		return (JavaGetDebuggeeBytes(lpbpi->lpDpi->hTM, cb, lpbpi->lpDpi->pValue, SHpFrameFrompCXF (&lpbpi->lpDpi->cxf))) == cb;
			
	}
	else {
		// if it is a bp relative
		if ( lpbpi->dpf.f.fBpRel ) {
			VERIFY(OSDGetAddr( hpidCurr, htidCurr, adrBase, &addr ) == xosdNone);

			if (addr.addr.off == 0 && addr.addr.seg == 0) {
				return FALSE;
			}
			
			SetAddrOff ( &addr , GetAddrOff( addr ) + lpbpi->lpDpi->u.bp.ldbBp );

			return( (DHGetDebuggeeBytes( addr, cb, lpbpi->lpDpi->pValue)) == cb );
		}

		// just load the value
		else{
			return( (DHGetDebuggeeBytes( lpbpi->lpDpi->u.d.DataAddr,
			   cb, lpbpi->lpDpi->pValue)) == cb );
		}
	}
}



/*** BPINSTALLVIRTUAL OSDEBUG version
*
* Purpose:	OSDEBUG - set the breakpoint if it was virtual when entered
*
* Input:
*
* Output:
*
* Exceptions:
*
*************************************************************************/
void PASCAL BPInstallVirtual ( HPID hpid, HEXE hexe, BOOL fLoad ) {

	if ( llbpi ) {
	    HBPI	hbpi = hbpiNull;
	    LPBPI	lpbpi;

		while( hbpi = LLHlleFindNext( llbpi, hbpi ) ) {
	        BOOL fNewVirtual;

	        lpbpi = (LPBPI)LLLpvFromHlle(hbpi);
	        fNewVirtual = lpbpi->bpf.f.fVirtual;

	        if ( lpbpi->bpf.f.fBpCode &&
	             (BOOL)lpbpi->bpf.f.fVirtual == fLoad &&
				 emiAddr ( lpbpi->CodeAddr ) == (HEMI) hexe
	        ) {
                fNewVirtual = !fLoad;

	            if ( lpbpi->bpf.f.fActive ) {
	                HTID	htid = HtidFromHthd ( lpbpi->hthd );
	                BOOL	fOneThread = ( htid != hmemNull );
					ADDR	addrT = lpbpi->CodeAddr;
					XOSD	xosd = xosdNone;

	                SYFixupAddr ( &addrT );

	                // I fixed FixupAddr so that we don't necessarily have to
	                // have the right hprc.  Now we can, as a last resort
	                // try to find the process with the right EMI and then try
	                // the fixup again.


	                xosd = SetBreakPoint (
	                    hpid,
	                    htid,
						fLoad,
	                    TRUE,
						&addrT,
						fOneThread,
	                    BpnsFromLpbpi( lpbpi ),
	                    lpbpi->bpf.f.fMessage,
	                    lpbpi->lMessage,
	                    lpbpi->dwMask
					);

					// FUTURE: must deal with error return properly!!
					ASSERT ( xosd == xosdNone );
	            }
	        }

	        if (
	            lpbpi->bpf.f.fBpData &&
	            (BOOL)lpbpi->bpf.f.fVirtual == fLoad &&
				emiAddr ( lpbpi->lpDpi->u.d.DataAddr ) == (HEMI)hexe
	        ) {
	            CXT     CXT;
	            PCXF    pCXF;
				XOSD	xosd;

	            memset ( &CXT, 0, sizeof(CXT));
	            pCXF = &lpbpi->lpDpi->cxf;

	            lpbpi->bpf.f.fVirtual = !fLoad;

	            if ( !lpbpi->dpf.f.fContext && !lpbpi->dpf.f.fEvalExpr && fLoad ) {
	                if (!BPLoadValue (lpbpi)) {
						xosd = xosdGeneral;
					}
	            }
	            else if ( lpbpi->dpf.f.fContext && !lpbpi->bpf.f.fBpCode && fLoad ) {

	                // we are within our current context
	                if(SHIsAddrInCxt(SHSetCxt(&lpbpi->lpDpi->u.bp.BlkAddr, &CXT),
	                   SHpADDRFrompCXT(SHpCXTFrompCXF((pCXF)))) &&
	                   (!SHIsInProlog(SHpCXTFrompCXF((pCXF))))) {

						// CAVIAR  6671: we needed to check if we are in
						// the function prolog (above) otherwise we will bind
						// in the wrong context and never rebind thus driving
						// right through the watch/tracepoint we just set [dwg,rm]

	                    if ( !lpbpi->dpf.f.fEvalExpr ) {

							if (!BPLoadValue (lpbpi)) {
								xosd = xosdGeneral;
							}
	                    }

	                }
	            }

				if (xosd != xosdNone) {
					continue;
				}

				// If you want to understand that hack please call SanjayS ||
				// math.
				

				if (!fLoad && lpbpi->dpf.f.fContext && lpbpi->bpf.f.fBpData &&
					lpbpi->dpf.f.fBpRel && lpbpi->dpf.f.fEmulate)
				{
					lpbpi->dpf.f.fEmulate = FALSE;
					lpbpi->dpf.f.fFuncLoad  = TRUE;
					lpbpi->dpf.f.fFuncBpSet = TRUE;
					lpbpi->bpf.f.fBpCode  = TRUE;
				}

				if (lpbpi->bpf.f.fActive)
				{
			    	xosd = SetWatch( lpbpi, fLoad );
				}

				// FUTURE: must deal with error return properly!!
				ASSERT ( xosd == xosdNone );

	        }
	        else {
	            lpbpi->bpf.f.fVirtual = fNewVirtual;
	        }

	        UnlockHlle ( hbpi );
		}
	}
}
