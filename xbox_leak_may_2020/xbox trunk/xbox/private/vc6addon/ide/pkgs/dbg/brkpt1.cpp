/*** brkpt1.c - breakpoint handlers
*
*   Copyright <C> 1989, Microsoft Corporation
*
*   Purpose:
*
*   Revision History:
*
*
*************************************************************************/
#include "stdafx.h"
#pragma hdrstop

#include "javadbg.h"
#include "dbgenc.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

extern BOOL GetSourceFrompADDR(LPADDR,LPSTR,WORD,LONG FAR *,SHOFF FAR *,HSF FAR *);

int  PASCAL BPTpWpComp( HPID, HTID, LPBPI, BOOL );
static int  PASCAL NEAR BPTpWpLoadValue( LPBPI, PADDR, int );

void PASCAL InsertCommand(char FAR *);
BOOL PASCAL CheckDataExpression ( LPBPI, BOOL );
void SetupForWatch ( void );
LPBPS EndWatch ( void );

#pragma optimize ( "", off )

/*** BPTpWpComp
*
* Purpose:  To compare the tracepoints memory. If I sense that the
*     tracepoint no longer is within scope, a breakpoint is
*     added for quicker execution.
*
* Input:
*   HBPI  - A handle to the breakpoint packet
*
* Output:
*  Returns TRUE if I am to stop execution
*
* Exceptions:
*
*************************************************************************/

int PASCAL
BPTpWpComp(
	HPID	hpid,
	HTID 	htid,
	LPBPI 	lpbpi,
	BOOL 	fUpdate
	)
{
    ADDR    addr = {0};
    FRAME   frame;
    int     i;

    // When we hit a function, we want to store the match value so
    // we have something to compare to. We know that we stopped
    // anywhere on the stack
    // check out the stack, if this is a bp rel. we want to make sure
    // we have the latest and greatest. The stack was set up by BPBrkExec

    if ( lpbpi->lpDpi == NULL ) {
        return FALSE;
    }

    if ( lpbpi->dpf.f.fContext ) {
        LPFME   lpfme;

        frame = *SHpFrameFrompCXF ( &lpbpi->lpDpi->cxf );

        // get out function address
        addr = lpbpi->lpDpi->u.bp.BlkAddr;

        // load the bp
        CLGetWalkbackStack ( hpid, htid, (UINT)NULL );

        if ( ( i = CLLookupAddress ( addr ) ) != -1 ) {
            HFME    hfme = CLHfmeGetNth( i );

            ASSERT( hfme );
            lpfme = (LPFME)LLLpvFromHlle( hfme );
            UnlockHlle( hfme );
        }
        else if ( lpbpi->bpf.f.fBpCode && lpbpi->bpf.f.fBpData ) {

            // It is a context sensitive breakpoint and it is not
            //  in context, so don't stop.
            return FALSE;
        }

		//math: If we cant get a context, just get out now

		if (i == -1 && lpbpi->dpf.f.fBpRel) {
			return FALSE;
		}

        if (
            i != -1 &&
            GetAddrOff ( lpfme->addrCSIP ) >= GetAddrOff ( addr ) &&
            GetAddrOff ( lpfme->addrCSIP ) < lpbpi->lpDpi->u.bp.oEnd ) {

            *SHpFrameFrompCXF ( &lpbpi->lpDpi->cxf ) =
                lpfme->Frame;
            SHSetCxt (
                &lpfme->addrCSIP,
                SHpCXTFrompCXF ( &lpbpi->lpDpi->cxf )
            );
        }
        else {
			PFRAME	pframe = SHpFrameFrompCXF( &lpbpi->lpDpi->cxf );

            SetFrameBPOff( *pframe, (UOFFSET)0 );
        }
    }

    if ( lpbpi->dpf.f.fFuncLoad ) {

        // we are not on the stack, bp is not set up!
        if (!GetFrameBPOff ( *SHpFrameFrompCXF (&lpbpi->lpDpi->cxf) )) {
            return FALSE;
        }

        // get the user requested check at breakpoint and load it
        // this is the initail load of the users tracepoint. Since
        // he specified a breakpoint in a function that wasn't in the
        // current scope, we must load it when we get into scope (now).

        // get the frames SS since when it was bound it may not have
        // been the right frame. and the user may have his/her own
        // stack.

		SetAddrSeg (&addr,
					(USHORT)SHpFrameFrompCXF (&lpbpi->lpDpi->cxf)->SS
			        );

        SetAddrOff (&addr,
					GetFrameBPOff (*SHpFrameFrompCXF( &lpbpi->lpDpi->cxf) ) +
			            lpbpi->lpDpi->u.bp.ldbBp
			        );
					
        ADDR_IS_LI ( addr ) = FALSE;

		//
		// Get the value loaded in.

        if ( lpbpi->dpf.f.fEvalExpr ) {
            LPDPI lpdpi = lpbpi->lpDpi;
            RTMI rtmi;
            HTI hti;

            SetupForWatch ();

            _fmemset( &rtmi, 0, sizeof(rtmi) );
            rtmi.fValue = TRUE;
            rtmi.fSzBytes = TRUE;

            // evaluate the expression

			EESTATUS	err;

            err = EEBindTM (&lpdpi->hTM,
							SHpCXTFrompCXF (&lpdpi->cxf),
							TRUE,
							FALSE);

			ASSERT (err == EENOERROR);
			
            err = EEvaluateTM (&lpdpi->hTM,
						 SHpFrameFrompCXF (&lpdpi->cxf),
						 EEHORIZONTAL);
				
			ASSERT (err == EENOERROR);

			err = EEInfoFromTM (&lpdpi->hTM, &rtmi, &hti);
			ASSERT (err == EENOERROR);


            lpdpi->lpbps = EndWatch ();
        }
        else {
            BPTpWpLoadValue ( lpbpi, &addr, TRUE );
        }

        lpbpi->dpf.f.fEmulate = TRUE;

		//
        // fEmulate must be TRUE or SetWatch will not do anything
		//
		
        SetWatch (lpbpi, TRUE);
        lpbpi->dpf.f.fFuncLoad = FALSE;
        lpbpi->dpf.f.fIsTrue = FALSE;

        if ( lpbpi->dpf.f.fFuncBpSet ) {
            // adjust my flags
            lpbpi->dpf.f.fFuncBpSet = FALSE;
            lpbpi->bpf.f.fBpCode  = FALSE;
        }

        // We want to continue now that we're in "emulate" mode
        {
            ADDR addr = lpbpi->CodeAddr;
            HPRC    hprc = hprcCurr;

            if ( lpbpi->hprc != hprcCurr ) {
                SYSetContext ( lpbpi->hprc, (HTHD) NULL, FALSE );
            }

            SYFixupAddr ( &addr );

            SetBreakPoint (
                hpidCurr,
                htidCurr,
                FALSE,
                TRUE,
                &addr,
                FALSE,
                bpnsCheck
                ,FALSE,0L,0
            );


            if ( hprcCurr != hprc ) {
                SYSetContext ( hprc, (HTHD) NULL, FALSE );
            }
        }

        if  ( lpbpi->dpf.f.fEvalExpr ) {
            return ( CheckDataExpression ( lpbpi, fUpdate ) );
        }
        else {
            return FALSE;
        }
    }


    // if we are in scope, say we hit a breakpoint, otherwise we
    // are out of scope, and we want to set the breakpoint.
    if (
        lpbpi->dpf.f.fContext  &&
        !(GetFrameBPOff ( *SHpFrameFrompCXF ( &lpbpi->lpDpi->cxf ) ) )
    ) {
        // Reinstall the code breakpoint & get rid of the data bps
        ADDR  addr   = lpbpi->CodeAddr;
        HPRC  hprc   = hprcCurr;
        FRAME frameT = *SHpFrameFrompCXF ( &lpbpi->lpDpi->cxf );

        if ( lpbpi->hprc != hprcCurr ) {
            SYSetContext ( lpbpi->hprc, (HTHD) NULL, FALSE );
        }

        *SHpFrameFrompCXF ( &lpbpi->lpDpi->cxf ) = frame;

        SetWatch ( lpbpi, FALSE );

        SYFixupAddr ( &addr );

        SetBreakPoint (
            hpidCurr,
            htidCurr,
            TRUE,
            TRUE,
            &addr,
            FALSE,
            bpnsCheck
            ,FALSE,0L,0
        );


        if ( hprcCurr != hprc ) {
            SYSetContext ( hprc, (HTHD) NULL, FALSE );
        }

        if ( lpbpi->dpf.f.fEvalExpr ) {
            ASSERT ( lpbpi->lpDpi->lpbps != NULL );
            _ffree ( lpbpi->lpDpi->lpbps );
            lpbpi->lpDpi->lpbps = NULL;
        }

        *SHpFrameFrompCXF ( &lpbpi->lpDpi->cxf ) = frameT;

        // adjust my flags
        lpbpi->dpf.f.fEmulate = FALSE;
        lpbpi->dpf.f.fFuncLoad  = TRUE;
        lpbpi->dpf.f.fFuncBpSet = TRUE;
        lpbpi->bpf.f.fBpCode  = TRUE;

        return FALSE;
    }


    // otherwise we must compare to if we want to stop

    return (CheckDataExpression ( lpbpi, fUpdate ));
}




int PASCAL
BPCheckBpt(
	PADDR	paddr,
	LPMSGI 	lpmsgi,
	HTID 	htid
	)
{
    HBPI hbpi = hbpiNull;
    BOOL fBreak = FALSE;

    // search all breakpoints for the marked one

    if ( !ADDR_IS_LI( *paddr ) ) {
        SYUnFixupAddr ( paddr );
    }

	if ( llbpi ) {
	    while ( hbpi = LLHlleFindNext ( llbpi, hbpi ) ) {
	        LPBPI lpbpi = (LPBPI)LLLpvFromHlle(hbpi);
	        BOOL  fCodeMatch = FALSE;

	        // get out if we are not to check this one
	        if ( !lpbpi->bpf.f.fActive || lpbpi->bpf.f.fVirtual ) {
	            continue;
	        }

	        // CODE BREAK POINT

	        if ( lpbpi->bpf.f.fBpCode ) {

	            // make sure seg and offset match
	            if ( ( GetAddrSeg ( lpbpi->CodeAddr ) == GetAddrSeg ( *paddr ) &&
	                 ( GetAddrOff ( lpbpi->CodeAddr ) == GetAddrOff ( *paddr )))
	            ) {
	                // if there is a data bp to match, indicate our status
	                if ( lpbpi->bpf.f.fBpData || lpbpi->bpf.f.fMessage ) {
	                    fCodeMatch = TRUE;
	                }
	                else if ( lpbpi->cPassCur > 0 ) {
	                    lpbpi->cPassCur--;
	                }
	                else {
	                    // otherwise we have found the breakpoint
	                    // save away the type

	                    fBreak = TRUE;
	                }
	            }
	        }

	        // CHECK DATA BREAKPOINTS

	        if (lpbpi->bpf.f.fBpData &&
				( !(lpbpi->bpf.f.fBpCode) || fCodeMatch))
			{
				BOOL fFire;

				fFire = BPTpWpComp (HpidFromHprc(lpbpi->hprc),
									htid,
									lpbpi,
									FALSE);
									
				if (fFire) {
				
	                if (lpbpi->cPassCur > 0) {
	                    lpbpi->cPassCur--;
	                } else {
	                    fBreak = TRUE;
	                }
	            }
	        }

			// CHECK MESSAGE BREAKPOINTS

			if (fCodeMatch && lpbpi->bpf.f.fMessage && lpmsgi &&
	            ( (lpbpi->dwMask && (lpbpi->dwMask & lpmsgi->dwMask ) ) ||
	              lpbpi->lMessage == lpmsgi->dwMessage
	            )
			) {
				if ( lpbpi->cPassCur > 0 ) {
					lpbpi->cPassCur--;
				}
				else {
					fBreak = TRUE;
				}
			}

	        UnlockHlle ( hbpi );

	        if ( fBreak ) {
	            break;
	        }
	    }
	}

    return fBreak;
}


/*** CheckAssertionFailed
* Purpose: Check if the break exception was an assert generated by the user's
*          program. ASSERT, VERIFY or assert
*
*/
BOOL
CheckAssertionFailed(
	LPADDR	lpaddr
	)
{
    ILINE    lineNo;
    HSF     hsf;
    CString strFileName;
    LPSTR   lpstr = strFileName.GetBuffer(_MAX_PATH);
    ADDR    addrT = *lpaddr;

    /*
    ** Sub 1 to get the addr assoc with the previous source line.
    */
    if ( GetAddrOff ( addrT ) ) {
        GetAddrOff ( addrT ) = GetAddrOff ( addrT ) - 1;
    }
    if ( GetSourceFrompADDR ( &addrT,
            lpstr,
            _MAX_PATH,
            &lineNo,
            NULL,
            &hsf )
    ) {
		LPSOURCEEDIT pISourceEdit = NULL;
		if (!SUCCEEDED(gpISrc->CreateSourceEditForHSF(hsf, &pISourceEdit)))
		{
			gpISrcQuery->CreateSourceEditForOpenDoc(strFileName, &pISourceEdit, FALSE);
			if (NULL == pISourceEdit)
				return FALSE;
		}
		ASSERT(pISourceEdit);

		PCSTR pcurLine;
		ULONG cbLine;
		pISourceEdit->GetLineTextSansCrLf(lineNo - 1, &pcurLine, &cbLine);
		pISourceEdit->Release();

		if (pcurLine)
		{
			CString strLine = CString(pcurLine, cbLine);
			strFileName.ReleaseBuffer();

			if ( _tcsstr ( (LPCSTR)strLine, "ASSERT" ) ||
				 _tcsstr ( (LPCSTR)strLine, "assert" ) ||
				 _tcsstr ( (LPCSTR)strLine, "VERIFY" )
			)
			{
				CAssertFailedDlg dlg;
				CString strDisplayName = GetDisplayName ( strFileName, 50 );

				dlg.m_strAssertText.Format (
					"File " + strDisplayName + ", Line %d", lineNo
				);

				dlg.DoModal();

				return TRUE;
			}
		}
    }
    strFileName.ReleaseBuffer();

    return FALSE;
}


/*** BPBrkExec
*
* Purpose: To see if we are to stop execution and why. This call
*      Should only be called after the child has returned from
*      ptrace. This will tell you why ptrace stoped. This routine
*      should not be used to actually stop ptrace in any way, use
*      BPIsAddr for that.
*
* Input:
*   pAddr - The address of interest, usually user_pc.
*   TheadId - The thread of interest.
*
* Output:
*  Returns The cause of the breakpoint. There are 3 reasons:
*       BPCONTINUE  - No reason to stop! This value is Zero.
*       BPDEADCHILD - A the child died.
*       BPABORT   - A cntr-C was hit.
*       BPBREAKPT - A code or data BP was hit.
*       BPTMP   - A temporary BP stopped us.
*
*
* Exceptions:
*   Pass counts, data breakpoints, can return type BPCONTINUE
*
* Notes:
*
*   This proc needs to be as fast as possible so some code from BPIsAddr
*   will be copied here. Also the overlay is not needed because in order
*   to get to this call a break must have already happened and the
*   overlay must already be loaded. The thread is needed because I may
*   have stopped because of multi breakpoints and I want to know which
*   breakpoint pertains to this thread.
*   If more than one breakpoint could have caused the stop, the first
*   breakpoint found will specify the action taken.
*
*************************************************************************/
int PASCAL
BPBrkExec(
	PADDR  	paddr,
	HTID	htid,
	BPI**	ppbpi
	)
{

	LPBPI     lpbpiCur;
	int       fCodeMatch, fPostCmd, fPostedCmd = FALSE;
	int       fStatus = BPCONTINUE, fStatusPrev = BPCONTINUE, fCmdQued = FALSE;
	char      arch[ CCHMAXPREFIXTOSZ + 11 ];
	HBPI      hbpi = hbpiNull;
	HBPI	  hbpiDel = hbpiNull;
	short     wBPCount = -1;    // Start at -1.  Loop increments count at top!

    if(lpprcCurr->stp == stpDead) {
        return(BPDEADCHILD);
    }

    // search all breakpoints for the marked one
    if ( !ADDR_IS_LI( *paddr ) ) {
        SYUnFixupAddr ( paddr );
    }

	if ( llbpi ) {
		while( hbpi = LLHlleFindNext( llbpi, hbpi ) ) {
			lpbpiCur = (LPBPI)LLLpvFromHlle(hbpi);

			if ( lpbpiCur->bpf.f.BpType != BPTMP ) {
				++wBPCount;
			}

			fPostCmd = FALSE;

			// get out if we are not to check this one
			if( !lpbpiCur->bpf.f.fActive  ||  lpbpiCur->bpf.f.fVirtual ) {
				UnlockHlle(hbpi);
				continue;
			}

			// CODE BREAK POINT

			if ( lpbpiCur->bpf.f.fBpCode ) {

				// Unless we explicitly match, say we don't
				fCodeMatch = FALSE;

				// make sure seg and offset match
				if (
					GetAddrSeg ( lpbpiCur->CodeAddr ) == GetAddrSeg ( *paddr ) &&
					GetAddrOff ( lpbpiCur->CodeAddr ) == GetAddrOff ( *paddr ) &&
					emiAddr ( lpbpiCur->CodeAddr ) == emiAddr ( *paddr  )
				) {

					// if there is a data bp to match, indicate our status
					if ( lpbpiCur->bpf.f.fBpData ) {
						fCodeMatch = TRUE;
						if (ppbpi) *ppbpi = lpbpiCur;
					}

					// Message case taken care of elsewhere
					else if ( !lpbpiCur->bpf.f.fMessage ) {
						// otherwise we have found the breakpoint
						// save away the type
						fStatus = lpbpiCur->bpf.f.BpType;
						if (ppbpi) *ppbpi = lpbpiCur;

						if ( fStatus != BPTMP && fStatus != BPTMPENC) {
						    fPostCmd = TRUE;
						}
					}
				}
			}

			// CHECK DATA BREAKPOINTS
			if (
				lpbpiCur->bpf.f.fBpData &&
				( !(lpbpiCur->bpf.f.fBpCode) || fCodeMatch )
			) {

				if ( BPTpWpComp (
					HpidFromHprc(lpbpiCur->hprc),
					htid,
					lpbpiCur,
					TRUE )
				) {
					fStatus = lpbpiCur->bpf.f.BpType;
					fPostCmd = TRUE;
					if (ppbpi) *ppbpi = lpbpiCur;
				}
			}

			// que up the command
			if( !fPostedCmd &&
				fPostCmd &&
				fStatus == BPBREAKPT &&
				lpprcCurr->fFuncExec != TRUE
			) {
				extern BOOL AutoTest;

				// We only report non-"location only" bps
				if (lpbpiCur->bpf.f.fBpData ||
					(lpbpiCur->bpf.f.fMessage) || AutoTest
				) {
					char	rgchT[ CCHMAXPREFIXTOSZ + 11 ];

					fPostedCmd = TRUE;

					PlayDevSound (IDS_SOUND_DEBUG_HITBP);

					// dolphin #9820 [chauv] use resource string
					int i1 = LoadString(
						hInst,
						DBG_Brk,
						(LPSTR)arch,
						sizeof( arch )
					);

					_tcscat(arch, _T(" "));
					i1 = _tcslen(arch);

					// notify the user that he had a breakpoint
					// make the string, but overwrite the flags
					BPUIFormat(
						hbpi,
						TRUE,
						FALSE,
						rgchT,
						sizeof( rgchT )
					);

					_tcsncpy(
						&arch[ i1 ],
						rgchT,
						sizeof( arch ) - i1 - 1
					);

					arch[ sizeof( arch ) - 1 ] = _T(0);

					CVMessage ( INFOMSG, (MSGID) arch, CMDWINDOW );
				}
				else {
					// It's a location bp - put in our signature
					CVMessage ( INFOMSG, (MSGID) "\x1\x2\x3\x4", CMDWINDOW );
				}
			}
			UnlockHlle(hbpi);

			if ( fStatus > fStatusPrev ) {
				fStatusPrev = fStatus;
			}
	    }

		fStatus = fStatusPrev;
	}

	if (!fPostedCmd &&
		fStatus == BPBREAKPT)
	{
		PlayDevSound (IDS_SOUND_DEBUG_HITBP);
	}

    if ( fStatus == BPCONTINUE ) {
        fStatus = BPHARDCODEDBP;
    }

	if (hbpiDel)
		BPDelete(hbpiDel);

    return fStatus;
}

/*** BPTpWpLoadValue
*
* Purpose: To reload, or reinitialize the tp match area in the dpi
*
* Input:
*   lpbpi - A pointer the breakpoint of interest
*   paddr - The address to load from, this is not needed, but since
*         it is already know, lets use it instead of recalculating
*   fLoad - Do we do the load or not, Again this is for convienence
*
* Output:
*  Returns  The value of fLoad
*
* Exceptions:
*
* Notes: This is really not a general routine, although it could be used
*    as one.
*
*************************************************************************/
static int PASCAL
BPTpWpLoadValue(
	LPBPI   lpbpi,
	PADDR   paddr,
	int     fLoad
	)
{
    if ( !fLoad ) {
        return FALSE;
    }

    if ( lpbpi->dpf.f.fReg ) {
		XOSD	xosd;

		xosd = OSDReadRegister(
			hpidCurr,
			htidCurr,
			lpbpi->lpDpi->iReg,
			lpbpi->lpDpi->pValue
		);

		ASSERT( xosd == xosdNone );
    }
    else {

		if (pDebugCurr && pDebugCurr->IsJava()) {
			JavaGetDebuggeeBytes (lpbpi->lpDpi->hTM, lpbpi->lpDpi->cData * lpbpi->lpDpi->cbData, lpbpi->lpDpi->pValue, SHpFrameFrompCXF (&lpbpi->lpDpi->cxf));
		}
		else {
			// just load the data
			DHGetDebuggeeBytes( *paddr,
						lpbpi->lpDpi->cData * lpbpi->lpDpi->cbData,
						lpbpi->lpDpi->pValue);
		}
    }

    return(TRUE);

}

/*** BPIsAddr
*
* Purpose: To Answer the question: Does this address have a breakpoint?
*
* Input:
*   paddr   - The address in question.
*   cbOffset  - Number of bytes from the offset of paddr to include
*           in the search
*   OvlNbr      - The overlay of the address
*   ThreadId    - The thread in question
*   fDisplay    - Report only displayable breakpoints (for source window)
*   fCheckDisabled - Also check disabled breakpoints
*
* Output:
*  Returns The Breakpoint address if found, NULL otherwise
*
* Exceptions:
*
* Notes: Replaces: is_breakpoint, is_thread_breakpoint, is_breakpoint_ovl
*    If you want to check for an exact match, use cbOffset = 1.
*
*************************************************************************/
HBPI PASCAL
BPIsAddr (
	PADDR   paddr,
    UOFFSET cbOffset,
    HTHD    hthd,
    int     fDisplay,
    BOOL    fCheckDisabled,
    UINT    *pWidgetFlags,
	BPC		bpcType
	)
{
    UOFFSET     OffsetLim = GetAddrOff ( *paddr ) + cbOffset;
    LPBPI       lpbpiCur;
    HBPI        hbpi;

    HBPI        hbpiActive, hbpiDisabled;
    BOOL        fActive     = FALSE;
    BOOL        fDisabled   = FALSE;

    // if not to check any bytes
    if( !cbOffset || !llbpi ) {
        return (HBPI)NULL;
    }

    hbpi  = LLHlleFindNext( llbpi, hbpiNull );

    // search all breakpoints
    while( hbpi ) {

        lpbpiCur = (LPBPI)LLLpvFromHlle(hbpi);

        // If target is a Win3.x 16-bit app, then if the hprc doesn't match,
        // the address may still match for multiple instances

        if ( lpbpiCur->hprc == hprcCurr ) {

            // make sure this one has a valid code area
            if ( lpbpiCur->bpf.f.fBpCode &&
                     !( lpbpiCur->bpf.f.fBpData && lpbpiCur->dpf.f.fFuncBpSet )
            ) {
                // We shouldn't need to do this, but...
                BOOL    fThreadOK = TRUE;

                if ( lpprcCurr->fHasThreads ) {
                    fThreadOK =
                        (BOOL)( lpbpiCur->hthd == 0  ||  hthd == lpbpiCur->hthd );
                }

                ////////////////////////////////////////////////////////
                //
                // EMI field check is done lower, sine Windows multiple
                // instances will FUCK this up royally.  See text at emi
                // check for more details...
                //
                ////////////////////////////////////////////////////////

                if( ( GetAddrSeg ( lpbpiCur->CodeAddr ) == GetAddrSeg ( *paddr ) )  &&
                    fThreadOK &&
                    // check the offset range
                    GetAddrOff( *paddr ) <= GetAddrOff( lpbpiCur->CodeAddr ) &&
                    GetAddrOff( lpbpiCur->CodeAddr ) <  OffsetLim    ) {

                    // otherwise only if it is an active one, displayable breakpoints
                    // are a subset of active ones
                    if (fCheckDisabled || lpbpiCur->bpf.f.fActive)
                    {

                        if( !fDisplay           ||
                            (lpbpiCur->bpf.f.BpType == bpcType    &&
                            (!lpbpiCur->bpf.f.fMessage) &&
                            (!lpbpiCur->lpDpi  ||  !lpbpiCur->dpf.f.fFuncBpSet)) ) {

                            // If the emi matches, then we have a valid
                            // unfixed addr compare.  If not, then for
                            // Windows 3, we need to check the fixed up
                            // address.  Non-Win3 targets, consider that
                            // we've failed already.  After fixing up the
                            // address, if the segments match, then we're
                            // looking at a different instance of an exe's
                            // address

                            if ( emiAddr ( lpbpiCur->CodeAddr ) ==
                                emiAddr ( *paddr ) ) {

                                if ( !fCheckDisabled ) {
                                    UnlockHlle(hbpi);
                                    return hbpi;
                                }
                                else
                                {
                                    if ( lpbpiCur->bpf.f.fActive )
                                    {
                                        *pWidgetFlags = BRKPOINT_LINE;
                                        fActive = TRUE;
                                        hbpiActive = hbpi;
                                    }
                                    else
                                    {
                                        fDisabled = TRUE;
                                        hbpiDisabled = hbpi;
                                    }
                                    if ( fActive && fDisabled )
                                    {
                                        *pWidgetFlags = MIXED_BRKPOINT_LINE;
                                        UnlockHlle(hbpi);
                                        return hbpiActive;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        // go to the next one
        UnlockHlle(hbpi);
        hbpi = LLHlleFindNext(llbpi,hbpi);
    }

    if ( fCheckDisabled && fDisabled )
    {
        // found only disabled BPs
        *pWidgetFlags = DISABLED_BRKPOINT_LINE;
        return hbpiDisabled;
    }

    if ( fActive )
    {
        return hbpiActive;
    }

    // didn't find one, return error
    return (HBPI)NULL;
}


/*** BPDelete
*
* Purpose: To Remove a breakpoint from the list
*
* Input:
*   lpbpi - A pointer to the breakpoint to be deleted
*
* Output:
*  Returns .....
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
void PASCAL
BPDelete(
	HBPI  hbpi
	)
{
    LPBPI lpbpi;

    if ( hbpi ) {

        lpbpi = (LPBPI)LLLpvFromHlle(hbpi);

        if ( lpbpi->bpf.f.BpType == BPCONTINUE ) {
            UnlockHlle ( hbpi );
            return;
        }

        if ( lpbpi && lpbpi->bpf.f.fActive ) {

            if ( lpbpi->bpf.f.fBpCode && ! lpbpi->bpf.f.fVirtual ) {
                HTID  htid = HtidFromHthd ( lpbpi->hthd );
                HPID  hpid = HpidFromHprc ( lpbpi->hprc );
                BOOL  fOneThread = ( htid != hmemNull );
                ADDR  addrT = lpbpi->CodeAddr;
                XOSD  xosd;
                HPRC  hprc = hprcCurr;

                if ( lpbpi->hprc != hprcCurr ) {
                    SYSetContext ( lpbpi->hprc, (HTHD) NULL, FALSE );
                }

                SYFixupAddr ( &addrT );

                xosd = SetBreakPoint (
                    hpid,
                    htid,
                    FALSE,
                    TRUE,
                    &addrT,
                    fOneThread,
                    BpnsFromLpbpi ( lpbpi ),
                    lpbpi->bpf.f.fMessage,
                    lpbpi->lMessage,
                    lpbpi->dwMask
                );

#ifndef OSDEBUG4	// (OSDEBUG4 doesn't define an xosdInvalidTL
					// Dolph:8564 If we did a FakeAsyncKill, the TL is gone.
				if ( xosd == xosdInvalidTL ) {
					xosd = xosdNone;
				}
#endif

                if ( hprcCurr != hprc ) {
                    SYSetContext ( hprc, (HTHD) NULL, FALSE );
                }

				// Need to allow xosdInvalidBreakPoint.  If the process
				// as been terminated (user initiated or otherwise) before
				// we've had a chance to delete the breakpoint, DMUHbptFromLpaddr
				// will fail since the DM may have already disabled all of the
				// breakpoints.  In any event, both conditions are OK.  If the
				// error xosdInvalidBreakPoint is returned, the BP has been
				// deleted (from the code, not the DMU list) anyway.
				//
				// See DOLPHIN: 1707   MarkBro 10/1/93
                ASSERT ( xosd == xosdNone || xosd == xosdInvalidBreakPoint );
            }
        }

        // check the emulation count
        if (
            lpbpi->bpf.f.fActive  &&
            lpbpi->bpf.f.fBpData  &&
            lpbpi->dpf.f.fEmulate
        ) {
            SetWatch ( lpbpi, FALSE );
        }

        // if this is one that was allocated, then free the memory.
        // remember, this takes the string out too because it was allocated
        // in one shot.

        // remove the data beakpoint
        if ( lpbpi->lpDpi ) {

            if ( lpbpi->lpDpi->pValue ) {
                free ( lpbpi->lpDpi->pValue );
            }

            if ( lpbpi->lpDpi->lpbps ) {
                _ffree ( lpbpi->lpDpi->lpbps );
            }

            EEFreeTM ( &lpbpi->lpDpi->hTM );
            _ffree(lpbpi->lpDpi);
        }

        // remove the code portion
        if ( lpbpi->lszCmd ) {
            _ffree(lpbpi->lszCmd);
        }

        UnlockHlle ( hbpi );
        LLFDeleteHlleFromLl ( llbpi, hbpi );
    }
}


//***************************************************************************
//
//      New watchpoint stuff
//
//***************************************************************************

extern CVF FAR *lpcvf;

LPW    rgregWP  = NULL;
LPADDR rgaddrWP = NULL;
LPW    rgcbWP   = NULL;
int    cregWP   = 0;
int    cmemWP   = 0;
BOOL   fTouched = FALSE;

#define cregWPMax 10
#define cmemWPMax 20

size_t LOADDS PASCAL
BPGetDebuggeeBytes(
	ADDR	addr,
	size_t 	cb,
	PVOID	lpv
	)
{

    ASSERT ( cmemWP < cmemWPMax );

    if ( ADDR_IS_LI ( addr ) ) {
        SYFixupAddr ( &addr );
    }

    rgaddrWP [ cmemWP ] = addr;
    rgcbWP [ cmemWP ] = cb;

    cmemWP += 1;

    return DHGetDebuggeeBytes ( addr, cb, (char FAR *)lpv );
}

PSHREG LOADDS PASCAL
BPGetReg(
	PSHREG 	pReg,
	PCXT 	pCxt
	)
{

    ASSERT ( cregWP < cregWPMax );

    rgregWP [ cregWP ] = pReg->hReg;

    cregWP += 1;

    return DHGetReg ( pReg, pCxt );
}

size_t LOADDS PASCAL
BPPutDebuggeeBytes(
	ADDR	addr,
	size_t 	cb,
	PVOID	pBuff
	)
{

    fTouched = TRUE;

    return cb;
}

PSHREG LOADDS PASCAL
BPSetReg(
	PSHREG	pReg,
	PCXT  	pCxt
	)
{

    fTouched = TRUE;

    return pReg;
}

void
SetupForWatch(
	)
{
    lpcvf->pDHGetDebuggeeBytes = &BPGetDebuggeeBytes;
    lpcvf->pDHGetReg = &BPGetReg;
    lpcvf->pDHPutDebuggeeBytes = &BPPutDebuggeeBytes;
    lpcvf->pDHSetReg = &BPSetReg;

    rgregWP  = (LPW)_fmalloc ( sizeof ( WORD ) * cregWPMax );
    rgaddrWP = (LPADDR)_fmalloc ( sizeof ( ADDR ) * cmemWPMax );
    rgcbWP   = (LPW)_fmalloc ( sizeof ( WORD ) * cmemWPMax );

    cregWP = 0;
    cmemWP = 0;

    fTouched = FALSE;
}


LPBPS
EndWatch(
	)
{
    LPBPS     lpbps  = NULL;
    int       ibpis  = 0;
    BPIS FAR *lpbpis = NULL;
    int       ireg;
    int       imem;

    lpcvf->pDHGetDebuggeeBytes = &DHGetDebuggeeBytes;
    lpcvf->pDHGetReg = &DHGetReg;
    lpcvf->pDHPutDebuggeeBytes = &DHPutDebuggeeBytes;
    lpcvf->pDHSetReg = &DHSetReg;

    if ( ( cregWP == 0 && cmemWP == 0 ) || fTouched ) {
        return NULL;
    }

    lpbps = (LPBPS)_fmalloc (
        sizeof ( BPS ) + ( sizeof ( BPI ) * ( cregWP + cmemWP + 1 ) )
    );
    _fmemset (
        lpbps, 0, sizeof ( BPS ) + ( sizeof ( BPI ) * ( cregWP + cmemWP + 1 ) )
    );

    lpbps->cbpis = cregWP + cmemWP;
    for ( ireg = 0; ireg < cregWP; ireg++ ) {

        lpbpis = &RgBpis(lpbps)[ ibpis ];

        lpbpis->bptp = bptpRegC;
        lpbpis->bpns = bpnsCheck;
        lpbpis->htid = htidCurr;

        lpbpis->reg.dwId = rgregWP [ ireg ];
        ibpis += 1;
    }

    for ( imem = 0; imem < cmemWP; imem++ ) {


        lpbpis = &RgBpis(lpbps)[ ibpis ];

        lpbpis->bptp = bptpDataC;
        lpbpis->bpns = bpnsCheck;
        lpbpis->htid = htidCurr;

        lpbpis->data.addr = rgaddrWP [ imem ];
        emiAddr ( lpbpis->data.addr ) = 0;
        lpbpis->data.cb   = rgcbWP [ imem ];

        ibpis += 1;

    }

    _ffree ( rgregWP );
    _ffree ( rgaddrWP );
    _ffree ( rgcbWP );

    return lpbps;
}

XOSD PASCAL
SetWatch (
	LPBPI	lpbpi,
	BOOL 	fSet
	)
{
    LPBPS lpbps = NULL;
    HPRC  hprc = hprcCurr;
    XOSD  xosd = xosdNone;
    BYTE  rgb [ sizeof ( BPS ) + 2 * sizeof ( BPIS ) ];
	BOOL  fEmulate = FALSE;

    if (!lpbpi->dpf.f.fEmulate || lpbpi->bpf.f.fVirtual) {
        return xosdNone;
    }

    SYSetContext (lpbpi->hprc, (HTHD) NULL, FALSE);

    if (lpbpi->dpf.f.fEvalExpr)
	{

        if (!lpbpi->lpDpi->lpbps)
		{

			//
            // When the user first entered the BP if it failed to evaluate or
            // get addresses we may not have a lpbps.

			return xosd;
        }
        else
		{
            lpbps = lpbpi->lpDpi->lpbps;

			//
			// Must force all BP relative breakpoints to be emulated.
			//
			
			if (lpbpi->dpf.f.fBpRel) {

				for (int i = 0; i < lpbps->cbpis; i++) {

					BPIS*	bpis = &RgBpis (lpbps) [i];

					if (bpis->bptp == bptpDataC || bpis->bptp == bptpDataW ||
						bpis->bptp == bptpDataR) {

						bpis->data.fEmulate = TRUE;
					}
				}
			}
        }
    }
    else
	{
        lpbps = (LPBPS) rgb;

        _fmemset ( lpbps, 0, sizeof ( BPS ) + 2 * sizeof ( BPIS ) );

        lpbps->cbpis = 1;

        RgBpis(lpbps)[ 0 ].bpns = BpnsFromLpbpi ( lpbpi );

        if (lpbpi->dpf.f.fReg)
		{
            RgBpis(lpbps) [ 0 ].bptp    = bptpRegC;
            RgBpis(lpbps) [ 0 ].htid    = htidCurr;
            RgBpis(lpbps) [ 0 ].reg.dwId = lpbpi->lpDpi->iReg;
        }
        else
		{
            ADDR	addr = { 0 };

			if (lpbpi->dpf.f.fBpRel)
			{
				CXF*	cxf = &lpbpi->lpDpi->cxf;
				FRAME*	frame = &cxf->Frame;
								
                SetAddrSeg (&addr, frame->BP.seg);
                SetAddrOff (&addr, frame->BP.off + lpbpi->lpDpi->u.bp.ldbBp);

				// We want to emulate for local BPs.  This works around w95
				// data bp problems that are just exacerbated by by making
				// the bps local.
				
				fEmulate = TRUE;
				
				ADDR_IS_LI ( addr ) = FALSE;
                SYSetEmi ( hpidCurr, htidCurr, &addr );
            }
            else
			{
                addr = lpbpi->lpDpi->u.d.DataAddr;
            }

            lpbpi->bpf.f.fVirtual   = !SHFIsAddrNonVirtual( &addr );

            RgBpis(lpbps) [ 0 ].bptp = bptpDataC;
            RgBpis(lpbps) [ 0 ].data.addr = addr;
			RgBpis(lpbps) [ 0 ].data.fEmulate = fEmulate;
            RgBpis(lpbps) [ 0 ].data.cb =
				lpbpi->lpDpi->cData * lpbpi->lpDpi->cbData;

			SYFixupAddr ( &RgBpis(lpbps) [ 0 ].data.addr );
        }
    }

    lpbps->fSet = fSet;

    if (
        lpbpi->dpf.f.fContext &&
        ( lpbps->cbpis == 1 ||
          RgBpis(lpbps) [ lpbps->cbpis - 1 ].bptp != bptpRange
        )
    ) {
        BPIS FAR *lpbpis = &RgBpis(lpbps) [ lpbps->cbpis ];
        _fmemset ( lpbpis, 0, sizeof ( BPIS ) );
        lpbpis->bptp = bptpRange;
        lpbpis->bpns = bpnsCheck;
        lpbpis->htid = htidCurr;

        lpbpis->rng.addr = lpbpi->lpDpi->u.bp.BlkAddr;
        lpbpis->rng.cb   =
            (WORD)( lpbpi->lpDpi->u.bp.oEnd -
            GetAddrOff ( lpbpi->lpDpi->u.bp.BlkAddr ) );
        SYFixupAddr ( &lpbpis->rng.addr );

        lpbps->cbpis += 1;
    }
	
    if (!lpbpi->bpf.f.fVirtual) {
        xosd = OSDBreakpoint ( hpidCurr, lpbps );
    }
    SYSetContext ( hprc, (HTHD) NULL, FALSE );
    return xosd;
}

#define cbbpsOne \
    sizeof ( BPS ) + sizeof ( BPIS ) + sizeof ( XOSD ) + sizeof ( DWORD )

XOSD
SetBreakPoint (
    HPID   hpid,
    HTID   htid,
    BOOL   fSet,
    BOOL   fRestore,
    LPADDR lpaddr,
    BOOL   fOneThread,
    BPNS   bpns,
    BOOL   fMessage,
    DWORD  dwMessage,
    DWORD  dwMask
	)
{
    BYTE      rgb [ cbbpsOne ];
    LPBPS     lpbps  = (LPBPS) rgb;
    BPIS FAR *lpbpis = &RgBpis(lpbps) [ 0 ];


    _fmemset ( rgb, 0, cbbpsOne );

    lpbps->fSet = fSet;


	//
	// OLD pre-OSD4 code:
	// lpbps->fRestore = fRestore;
	//
	
	#pragma message("OSDEBUG4: need to tell OSDebug not to restore old code byte")

	lpbps->cbpis = 1;
    lpbpis->bpns = bpns;
    lpbpis->fOneThd = fOneThread;
    lpbpis->htid = htid;
    lpbpis->exec.addr = *lpaddr;

    if ( !fMessage ) {
        lpbpis->bptp = bptpExec;
    }
    else if ( dwMask ) {
        lpbpis->bptp = bptpMClass;
        lpbpis->mcls.dwmask = dwMask;
    }
    else {
        lpbpis->bptp = bptpMessage;
        lpbps->cmsg = 1;
        lpbpis->msg.imsg = 0;
        lpbpis->msg.cmsg = 1;
        *( (DWORD FAR *) &rgb [ sizeof ( BPS ) + sizeof ( BPIS ) ] ) = dwMessage;
    }

    return OSDBreakpoint ( hpid, lpbps );
}

BPNS
BpnsFromLpbpi(
	LPBPI	lpbpi
	)
{
    BPNS bpns = bpnsStop;

    if ( lpbpi->bpf.f.fNoStop ) {
        bpns = bpnsContinue;
    }
    else if (
        lpbpi->bpf.f.fPass ||
        lpbpi->bpf.f.fBpData
    ) {
        bpns = bpnsCheck;
    }

    return bpns;
}

int fWindowsOutputOnly = FALSE;
extern char two_adapters;

BOOL PASCAL
BPMsgBpt(
	BOOL	fStop,
	LPMSGI 	lpmsgi
	)
{
    HMEM 	hbpi = hmemNull;
    UINT 	ibpi = 0;
    CHAR 	rgch [ CCHMAXPREFIXTOSZ + 20 ];
	BOOL	fMatched = FALSE;

    SYUnFixupAddr ( &lpmsgi->addr );

    if ( !fStop || !llbpi ) {
        return FALSE;
    }

    while ( ( hbpi = LLHlleFindNext ( llbpi, hbpi ) ) != hmemNull ) {
        LPBPI lpbpi = (LPBPI)LLLpvFromHlle ( hbpi );

        if (
			lpbpi->bpf.f.fActive &&
            !lpbpi->bpf.f.fNoStop &&
            lpbpi->bpf.f.fMessage &&
            GetAddrSeg ( lpbpi->CodeAddr ) == GetAddrSeg ( lpmsgi->addr ) &&
            GetAddrOff ( lpbpi->CodeAddr ) == GetAddrOff ( lpmsgi->addr ) &&
            ( ( lpbpi->dwMask && ( lpbpi->dwMask & lpmsgi->dwMask ) ) ||
              lpbpi->lMessage == lpmsgi->dwMessage
            )
        ) {
			// dolphin #9820 [chauv] use resource string
			int	i1;
			char	rgchT[ CCHMAXPREFIXTOSZ + 11 ];

			fMatched = TRUE;
			
			i1 = sprintf(rgch, _T("\n"));
			LoadString( hInst, DBG_Brk, (LPSTR)(&rgch[i1]), sizeof( rgch ) - i1 );
			_tcscat(rgch, _T(" "));
			i1 = _tcslen(rgch);

            BPUIFormat (hbpi, TRUE, FALSE, rgchT, sizeof (rgchT));

			_tcsncpy( &rgch[ i1 ], rgchT, sizeof( rgch ) - i1 - 1 );
			rgch[ sizeof( rgch ) - 1 ] = _T(0);

            CVMessage ( INFOMSG, (MSGID) rgch, CMDWINDOW );
		}

        UnlockHlle ( hbpi );

        ibpi += 1;
    }

	return fMatched;
}

BOOL PASCAL
CheckDataExpression(
	LPBPI	lpbpi,
	BOOL 	fUpdate
	)
{
    ADDR    addr = {0};
    char    *pch;
    char    rgRead[256];
    int     i;
    int     cbRead;

    // WATCHPOINT STUFF

    // if this is a watchpoint
    if ( lpbpi->dpf.f.fEvalExpr ) {

        RTMI	rtmi;
        HTI		hti;
        PTI		pti;
        int		i;
        int		fStop = FALSE;
        CXF		cxf = lpbpi->lpDpi->cxf;
        LPBPS	lpbps = NULL;


        SetupForWatch ();

        // get the TM for this guy
        _fmemset( &rtmi, 0, sizeof(rtmi) );
        rtmi.fValue = TRUE;
        rtmi.fSzBytes = TRUE;

        // evaluate the expression
        if (
            EEBindTM (
                &lpbpi->lpDpi->hTM,
                SHpCXTFrompCXF(&cxf),
                TRUE,
                FALSE
            ) ||
            EEvaluateTM (
                &lpbpi->lpDpi->hTM,
                SHpFrameFrompCXF(&cxf),
                EEHORIZONTAL
            ) ||
            EEInfoFromTM ( &lpbpi->lpDpi->hTM, &rtmi, &hti )
        ) {
			// since we're discarding the return value of EndWatch,
			// we must free it (return value may be NULL, but it's
			// okay to pass NULL to _ffree)
            _ffree(EndWatch());

            lpbpi->dpf.f.fIsTrue = FALSE;
            return ( FALSE );
        }

        lpbps = EndWatch();

		// No Update, just evaluate

		// Additional test for lpbps.  It is possible that when the
		// breakpoint was set it was out of context and the possible
		// fTouched (expression contained an assignment) would not be
		// set, but now, that it IS in context, lpbps will be NULL,
		// therefore, don't update the static information.

#pragma message ( "WARNING: need to add arg to EndWatch for assignment check" )
		if ( fUpdate && lpbps ) {
	        if ( lpbpi->lpDpi->lpbps != NULL ) {

	            // Reset the OSDebug data breakpoints if necessary
	            // is if we had a function load context sensitive WP or TP.
	            // In the case where the user specifies a location for function
	            // evaluation and the location is in the context required I don't
	            // think that we have to reset the OSDebug Watch Mechanism.

	            lpbps->fSet = FALSE;
	            lpbpi->lpDpi->lpbps->fSet = FALSE;

	            // The fSet attributes must be the same to avoid
	            //  unecessary failures of the following compare.

	            lpbps->fSet = FALSE;
	            lpbpi->lpDpi->lpbps->fSet = FALSE;

	            if (
	                ( lpbpi->dpf.f.fContext &&
	                  lpbps->cbpis + 1 != lpbpi->lpDpi->lpbps->cbpis
	                ) ||
	                ( !lpbpi->dpf.f.fContext &&
	                  lpbps->cbpis != lpbpi->lpDpi->lpbps->cbpis
	                ) ||

					_fmemcmp (
                        RgBpis(lpbps),
                        RgBpis(lpbpi->lpDpi->lpbps),
	                    sizeof ( BPIS ) * lpbps->cbpis
                    )
	            ) {
	                SetWatch ( lpbpi, FALSE );
	                _ffree ( lpbpi->lpDpi->lpbps );

	                lpbpi->lpDpi->lpbps = lpbps;
	                SetWatch ( lpbpi, TRUE );
	            }
	            else {
	                _ffree ( lpbps );
	            }
	        }
	        else {
	            /*
	            ** If the lpbps was previously NULL then set it to whatever we
	            ** now.  This will happen when it previously didn't evaluate or
	            ** failed before requesting addresses
	            */
	            lpbpi->lpDpi->lpbps = lpbps;
	            SetWatch ( lpbpi, TRUE );
	        }
		}
		else {
			_ffree(lpbps);
		}

        pti = (PTI)BMLock ( hti );

        // Or this thing together, if all zeros, then we will make a FALSE
        // if any bits set it won't be a false.
        if (
            pti->fResponse.fValue  &&
            (unsigned short) pti->fResponse.fSzBytes
        ) {
            for ( i = 0; i < (int)pti->cbValue; i++ ) {
                fStop |= pti->Value [ i ];
            }
        }

        BMUnlock(hti);
        EEFreeTI(&hti);

        // if there is no change or if the expression evaluator fails, don't stop


        if ( !fStop ) {
            // Last firing of the WP is no longer valid, so tell the
            //  user next time it becomes true.

            lpbpi->dpf.f.fIsTrue = FALSE;
        }
        else if ( lpbpi->dpf.f.fIsTrue ) {
            // Already told the user that this wp has fired

            fStop = FALSE;
        }

        if ( fStop && fUpdate && !lpbpi->bpf.f.fBpCode ) {
            lpbpi->dpf.f.fIsTrue = TRUE;
        }


        return fStop;
    }


    // TRACEPOINT STUFF

    // if we are a register
    if ( lpbpi->dpf.f.fReg ) {
		UR		ur = {0};
		BOOL	fT;

		ASSERT( lpbpi->lpDpi->cbData <= sizeof( ur ) );
		VERIFY( OSDReadRegister( hpidCurr, htidCurr, lpbpi->lpDpi->iReg, &ur ) == xosdNone );
		fT = _fmemcmp( lpbpi->lpDpi->pValue, &ur, lpbpi->lpDpi->cbData );

        if ( fUpdate ) {
            return BPTpWpLoadValue ( lpbpi, &addr, fT );
        }
        else {
            return fT;
        }
    }
    // else if we are bp relative
    else if ( lpbpi->dpf.f.fBpRel ) {
        SetAddrSeg (
            &addr ,
            GetFrameBPSeg ( *SHpFrameFrompCXF(&lpbpi->lpDpi->cxf) )
        );
        SetAddrOff (
            &addr,
            GetFrameBPOff (
                *SHpFrameFrompCXF(&lpbpi->lpDpi->cxf) ) +
                lpbpi->lpDpi->u.bp.ldbBp
            );
            ADDR_IS_LI ( addr ) = FALSE;
			ADDR_IS_OFF32( addr ) = TRUE;
            SYSetEmi ( hpidCurr, htidCurr, &addr );
    }

    // else easy breakpoint

    else {
        addr = lpbpi->lpDpi->u.d.DataAddr;
        SYFixupAddr ( &addr );
    }

    // initailize for the loop
    i = lpbpi->lpDpi->cbData  *  lpbpi->lpDpi->cData;
    pch = lpbpi->lpDpi->pValue;

    while ( i > 0 ) {

        // see how much we can do on this pass
        cbRead = (i > sizeof(rgRead)) ? sizeof(rgRead) : i;

		size_t cbDebuggeeBytes;

		if (pDebugCurr && pDebugCurr->IsJava()) {
			cbDebuggeeBytes = JavaGetDebuggeeBytes (lpbpi->lpDpi->hTM,
													cbRead,
													rgRead,
													SHpFrameFrompCXF (&lpbpi->lpDpi->cxf));
			
		}
		else {
			// read in the data
			cbDebuggeeBytes = DHGetDebuggeeBytes(addr, cbRead, rgRead);
        }
		
		if (cbDebuggeeBytes == 0) {
			return FALSE;
		}

        // If we have a change must update to new value
		
        if (_fmemcmp (rgRead, pch, cbRead)) {

            if (fUpdate) {
                return BPTpWpLoadValue (lpbpi, &addr, TRUE);
            }
            else {
                return TRUE;
            }
        }
        else if (fUpdate) {
            return FALSE;
        }

        // get ready for the next pass
        pch += cbRead;
        SetAddrOff ( &addr , GetAddrOff ( addr ) + cbRead );
        i -= cbRead;
    }
    return FALSE;
}
