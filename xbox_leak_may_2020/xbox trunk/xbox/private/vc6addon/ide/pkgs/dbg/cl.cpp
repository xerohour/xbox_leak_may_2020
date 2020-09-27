/*** cl.c - CaLl stack routines
*
*   Copyright <C> 1989, Microsoft Corporation
*
*   Purpose:
*
*************************************************************************/
#include "stdafx.h"
#pragma hdrstop

#define _CRTBLD
#include "undname.h"
#undef _CRTBLD

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

HLLI    hlliFME = (HLLI)NULL;

CXF     cxfCalls = { 0 };
int     iCallsCxf = -1;

static	int		ihfmeMacLast = -1;
static	HFME	hfmeMacLast = (HFME)NULL;
static	int		chfme = 0;

char *  PASCAL CLGetProcFromHfme(HFME, char *,int,BOOL);
char *  PASCAL CLGetProcName(int, char *,int,BOOL);

/*** CLChfmeOnStack
*
*  Purpose: Get the number of entries in the call stack list
*
*  Input: n/a
*
*  Output: Number of items in list or zero if there's no list
*
*  Exceptions:
*
*  Notes:
*
**************************************************************************/
int  PASCAL CLChfmeOnStack( void ) {
	return chfme;
}

/*** CLSetProcAddr
*
*   Purpose: To set up the symbols part of the stack frame structure
*
*   Input:
*   pfme        addrCSIP must be set up.
*   Output:
*   pfme        The stack frame to fill in the result
*           Elements:
*           symbol
*           clt
*           addrProc
*           module
*
*
*   Returns:
*
*   Exceptions:
*
*   Notes:
*
*************************************************************************/
void PASCAL CLSetProcAddr ( LPFME lpfme ) {
	if ( !lpfme->fSetProc ) {
	    CXT  cxt;
		ADDR addrT = {0};

		lpfme->fSetProc = TRUE;

	    lpfme->addrProc = lpfme->addrCSIP;

	    _fmemset ( &cxt, 0, sizeof ( CXT ) );
	    SHSetCxt ( &lpfme->addrProc, &cxt );
	    if ( !SHHMODFrompCXT ( &cxt ) ) {
	        lpfme->symbol   = NULL;
	        lpfme->clt = cltNone;
	        return;
	    }

	    lpfme->addrProc = *SHpADDRFrompCXT ( &cxt );
	    lpfme->module   =  SHHMODFrompCXT ( &cxt );

	    // block also may have a name. If it does, use it as the symbol in the
	    // calls stack.


	    if ( SHHPROCFrompCXT ( &cxt ) ) {
	        lpfme->symbol = (char FAR *)SHHPROCFrompCXT ( &cxt );
	        lpfme->clt    = cltProc;
	        SHAddrFromHsym ( &addrT, lpfme->symbol );
	        SetAddrOff (
	            &lpfme->addrProc,
	            GetAddrOff ( addrT )
	        );
	    }
	    else if ( SHHBLKFrompCXT ( &cxt ) ) {
	        HBLK      hblk;
	        ADDR      addrT;
	        char      rgch [ 100 ];

	        hblk = SHHBLKFrompCXT ( &cxt );
	        _fmemset ( &addrT, 0, sizeof ( ADDR ) );

	        SHAddrFromHsym ( &addrT, hblk );
	        if ( SHGetSymName ( hblk, rgch ) != NULL ) {
	            lpfme->symbol = (char FAR *)hblk;
	            lpfme->clt = cltBlk;
	        }

	        SetAddrOff (
	            &lpfme->addrProc,
	            GetAddrOff ( addrT )
	        );
	    }
	    else if ( PHGetNearestHsym (
	                &lpfme->addrCSIP,
	                SHHexeFromHmod ( SHHMODFrompCXT ( &cxt ) ),
	                (PHSYM) &lpfme->symbol ) <
	              0xFFFFFFFF ) {
	        ADDR addrT;

	        _fmemset ( &addrT, 0, sizeof ( ADDR ) );

	        SHAddrFromHsym ( &addrT, lpfme->symbol );

	        lpfme->clt = cltPub;
	        SetAddrOff ( &lpfme->addrProc, GetAddrOff ( addrT ) );
	    }
	    else {
	        lpfme->symbol   = NULL;
	        lpfme->clt = cltNone;
	    }
	}
}

// Only destroy cached UI strings (radix probably changed)
void PASCAL CLClearCachedStrings() {
	if ( hlliFME ) {
		HFME	hfme = (HFME)NULL;

		while( hfme = LLHlleFindNext( hlliFME, hfme ) ) {
			LPFME	lpfme = (LPFME)LLLpvFromHlle( hfme );

			if ( lpfme->lsz ) {
				_ffree( lpfme->lsz );
				lpfme->lsz = (LSZ)NULL;
			}

			UnlockHlle( hfme );
		}
	}
}


/*** CLHfmeGetNth
*
*  Purpose: Get the nth item in the list.
*
*  Input:
*       i   index to item to get
*
*  Output: Returns a handle to the nth item or NULL if no list or out of
*       range.
*
*  Exceptions:
*
*  Notes:
*
**************************************************************************/
HFME PASCAL CLHfmeGetNth( int i ) {
    HFME    hfme = (HFME)NULL;

    if ( hlliFME ) {
		if ( i == ihfmeMacLast ) {
			hfme = hfmeMacLast;
		}
		else {
			int	ifme;

			if ( ihfmeMacLast != -1 && i > ihfmeMacLast ) {
				ifme = ihfmeMacLast;
				hfme = hfmeMacLast;
			}
			else {
	    		ifme = 0;
				hfme = LLHlleFindNext( hlliFME, hfme );
			}

	        while( hfme && ifme != i ) {
	            hfme = LLHlleFindNext( hlliFME, hfme );
				ifme++;
			}

			if ( hfme ) {
				hfmeMacLast = hfme;
				ihfmeMacLast = i;
			}
	    }
	}

	if ( hfme ) {
		LPFME	lpfme = (LPFME)LLLpvFromHlle( hfme );

		CLSetProcAddr( lpfme );
		UnlockHlle( hfme );
	}
    return hfme;
}

PCXF PcxfLocal ( void ) {
	
	if ( lpprcCurr == NULL)
		return &cxfIp;

	// If the debuggee is running, then don't reload the
	// walkback stack since this will no necessarily be a
	// valid thread (htidCurr may be dead)

	if ( lpprcCurr->stp == stpRunning ) {
		if ( iCallsCxf == -1 ) {
			return &cxfIp;
		}
	}
	else {
	    HFME    hfme;

	    /*
	    ** This has already been called by the locals window so it is really a do
	    ** nothing in the case where the locals window was open.  However, if
		** we are up the call chain, make sure that the entire call chain is
		** loaded
	    */
	    CLGetWalkbackStack ( hpidCurr, htidCurr, (UINT)( iCallsCxf == -1 ) );

	    if ( iCallsCxf == -1 ) {
	        /*
	        ** We need to get the virtualized frame because it may be Frameless
	        */
	        if ( hfme = CLHfmeGetNth ( 0 ) ) {
	            LPFME lpfme = (LPFME)LLLpvFromHlle( hfme );

	            _fmemset ( &cxfCalls, 0, sizeof ( CXF ) );

	             SHSetCxt ( &lpfme->addrCSIP, SHpCXTFrompCXF ( &cxfCalls ) );
	            *SHpFrameFrompCXF ( &cxfCalls ) = lpfme->Frame;
				UnlockHlle ( hfme );
	        }
	        else {
	            /*
	            ** This shouldn't happen, but during restart's in the IDE
				** it does (markbro 5/10/93)
	            */
	            return &cxfIp;
	        }
	    }
	}

    return &cxfCalls;
}

/*** CLLookupAddress
*
* Purpose: To lookup an address in the callback stack area
*
* Input: paddr  -  A pointer to an ADDR struct contining the address to
*           find in the calls stack
*
* Output:
*  Returns The index into the calls stack containing the address
*
* Exceptions:
*
* Notes:    -1 returned on error
*
*************************************************************************/

int PASCAL CLLookupAddress ( ADDR addr ) {
    HFME    hfme = (HFME)NULL;
    int     ifme = 0;
    BOOL    fFound = FALSE;

	if ( chfme ) {
    	PADDR   paddr = (PADDR)&addr;

		ASSERT( hlliFME );

		for( ifme = 0; !fFound && ifme < chfme; /*NOP*/ ) {
			LPFME	lpfme;

			hfme = CLHfmeGetNth( ifme );
            lpfme = (LPFME)LLLpvFromHlle( hfme );

            if ( lpfme->symbol &&
            	emiAddr ( lpfme->addrProc ) == emiAddr ( *paddr ) &&
                IsSegEqual (
                    GetAddrSeg ( lpfme->addrProc ),
                    GetAddrSeg ( *paddr )) &&
                GetAddrOff ( lpfme->addrProc ) <= GetAddrOff ( *paddr ) &&
                GetAddrOff ( lpfme->addrCSIP ) >= GetAddrOff ( *paddr ) ) {

                fFound = TRUE;
            }
            else {
                ++ifme;
            }
            UnlockHlle( hfme );
        }
    }
    return( fFound ? ifme : -1 );
}

/*** CLGetParams
*
*  Purpose: Get the paramater arguments for the TM specified
*
*  Input:
*
*  Output:
*
*  Exceptions:
*
*  Notes:  This code used to do memcpy's and attempt to keep track of the
*		max size, but it didn't do it very well and it was difficult to
*		read.  Now, it just does strcats.
*
**************************************************************************/
char *CLGetParams ( PHTM phtm, FRAME FAR *lpframe, int *pcbMax, char *pch, BOOL fShowArgs ) {
    ushort  cParm = 0;
    SHFLAG  shflag;

	strcpy( pch, "(" );

    if (  ( runDebugParams.fCallTypes || fShowArgs ) &&
    	EEcParamTM ( phtm, &cParm, &shflag ) == EENOERROR
    ) {
		ushort  i = 0;
		BOOL	fNeedComma = FALSE;

        for ( i = 0; i < cParm; i++ ) {
	    	HTM		htmParm;
    		WORD	strIndex;

            if ( *pcbMax > 0 &&
                 EEGetParmTM ( phtm, i, &htmParm, &strIndex, fCaseSensitive ) == EENOERROR
            ) {
				EEHSTR		hstr;
				EESTATUS	eest;

				if ( runDebugParams.fCallTypes ) {
					eest = EEGetTypeFromTM( &htmParm, (EEHSTR)NULL, &hstr, 0L );

					if ( fNeedComma ) {
						strcat( pch, ", " );
					}

					if ( eest == CV_NOERROR ) {
						LPCH	lpch;

						lpch = (LPCH)BMLock( hstr ) + sizeof( HDR_TYPE );

						// Remove the trailing space!
						if ( *lpch ) {
							if (*(lpch + _fstrlen(lpch) - 1) == _T(' ')) {
								*( lpch + _fstrlen( lpch ) - 1 ) = '\0';
							}
						}

						// Not enough room, bail!   5 == "...)\0"
						if ( _fstrlen( lpch ) + strlen( pch ) + 5 > (size_t)*pcbMax ) {
							strcat( pch, "..." );
    	    	            BMUnlock ( hstr );
	            	        EEFreeStr ( hstr );
							EEFreeTM ( &htmParm );
							break;
						}

						_fstrcat( (char FAR *)pch, lpch );

        	            BMUnlock ( hstr );
            	        EEFreeStr ( hstr );
						fNeedComma = TRUE;
					}
				}

				if ( fShowArgs ) {
					EEHSTR	hName;

                	EEvaluateTM ( &htmParm, lpframe, EEHORIZONTAL );
					eest = EEGetValueFromTM (
						&htmParm,
						(unsigned int)radix,
						(unsigned char FAR *)"p",
						&hName
					);

					if ( eest == EENOERROR ) {
						LPCH	lpch;

						if ( fNeedComma ) {
							if ( !runDebugParams.fCallTypes ) {
								_fstrcat( pch, "," );
							}
							_fstrcat( pch, " " );
						}

						lpch = (LPCH)BMLock( hName );

						// Not enough room, bail!   5 == "...)\0"
						if ( _fstrlen( lpch ) + strlen( pch ) + 5 > (size_t)*pcbMax ) {
							strcat( pch, "..." );
    	    	            BMUnlock ( hName );
	            	        EEFreeStr ( hName );
							EEFreeTM ( &htmParm );
							break;
						}

						_fstrcat( (char FAR *)pch, lpch );

        	            BMUnlock ( hName );
            	        EEFreeStr ( hName );
						fNeedComma = TRUE;
                    }
				}

                EEFreeTM (&htmParm);
            }
        }
	}

    strcat( pch, ")" );

    return pch + strlen( pch );
}

size_t AddProcName( const ADDR& addrT, char *pchT )
{
	unsigned int cb = 0;
	if ( emiAddr ( addrT )	&& emiAddr ( addrT ) != (HEMI)hpidCurr ) {
        LSZ  lsz = NULL;

		lsz = SHGetExeName ( (HEXE)emiAddr ( addrT ) );
        CVSplitPath ( lsz, NULL, NULL, pchT, NULL );
        cb = sprintf ( pchT, "%s! ", pchT );
    }
	return cb;
}

/*** CLGetProcFromHfme
*
* Purpose: To format a line to be diplayed in the calls menu.  This is
*          really the work to be done by CLGetProcName, but we take an
*           hfme instead of an index.  This is used internally for
*           performance
*
* Input:
*   hfme    - Handle to a stack frame
*   cbMax   - max size of pch buffer
*   fShowArgs - Show call arguments
*
* Output:
*  Returns NULL for failure, else pch
*   pch     - A pointer a buffer to put the string
*
* Exceptions:
*
* Notes:
*  to prevent output buffer overruns (pch) , we need to check the length
*  during format. but instead of checking for length every time we xfer into
*  output buffer, we check at selected points in the code and make certain
*  safe assumptions eg the length passed to us will be able to accomodate
*  the symbol name totally etc. Thus we remain ok and save some code & time
*
*************************************************************************/
char * PASCAL CLGetProcFromHfme (
HFME    hfme,
char *  pch,
int     cbMax,
BOOL    fShowArgs ) {
    char            *pchT;
    int             len = 0;
    HTM             htm;
	EEHSTR			hName;
    ushort          retval;
    CXT             cxt;
    ushort          strIndex;
    LPFME           lpfme;
    uchar FAR *     symbol;

    // This function is also being used by ENC for displaying function
    // names. Ether hlliFME or hlliFmeEnc shoud be non-null
    extern  HLLI hlliFmeEnc;
    ASSERT( hlliFME || hlliFmeEnc);
    ASSERT( hfme );

    lpfme = (LPFME)LLLpvFromHlle( hfme );
    symbol = (uchar FAR *)lpfme->symbol;
    UnlockHlle( hfme );

    cbMax -= 2;
    pchT   = pch;

    _fmemset ( &cxt, 0, sizeof ( cxt ) );
    if ( lpfme->clt != cltNone ) {
        *SHpADDRFrompCXT ( &cxt ) = lpfme->addrProc;
    }
    else {
        *SHpADDRFrompCXT ( &cxt ) = lpfme->addrCSIP;
    }
    SHHMODFrompCXT ( &cxt )   = lpfme->module;

    switch ( lpfme->clt ) {

        case cltNone:
            {
                ADDR addrT = lpfme->addrCSIP;

                SYFixupAddr ( &addrT );

				pchT += AddProcName( addrT, pchT );

				ASSERT( ADDR_IS_OFF32( addrT ) );
                len = sprintf ( pchT, "%08lx()", GetAddrOff ( addrT ) );
                pchT += len;
                break;

            }
        case cltProc:

            SHHPROCFrompCXT ( &cxt ) = (HPROC) symbol;
            goto MakeName;

        case cltBlk:

            SHHBLKFrompCXT ( &cxt )  = (HBLK) symbol;
			goto MakeName;

        case cltPub:
			len = AddProcName( lpfme->addrProc, pchT );
			pchT += len;
			cbMax -= len;

MakeName:
            retval = EEGetTMFromHSYM(
            	symbol,
            	&cxt,
            	&htm,
            	&strIndex,
            	TRUE,
            	FALSE
            );

			if ( retval != EENOERROR ) {

				// Old code, not sure that we want a dialog box here since
				// we can recover
            	if ( retval == EECATASTROPHIC ) {
	                char szError [ MAXERRMSG ];

    	            fetch_error ( CATASTROPHICTM, szError );
        	        CVExprErr( CATASTROPHICTM, MSGBOX, &htm, szError );
				}

				// must free the TM here, else we leak!
				EEFreeTM (&htm);

				// Just redisplay the node as if there were no symbols
				lpfme->clt = cltNone;
				return CLGetProcFromHfme( hfme, pch, cbMax, fShowArgs );
            }

            // should always be able to convert an hSym

            ASSERT ( retval == EENOERROR );

            if ( EEGetNameFromTM ( &htm, &hName ) == EENOERROR ) {

                LPCH    lpch = (LPCH)BMLock ( hName );

				if (*lpch=='?')
				{
					// try unmangling the symbol name
					char unmangled[512];
					unsigned short flags;

					// we can never show arg Values, but we can show arg Types if required
					if (runDebugParams.fCallTypes || fShowArgs)
						flags = UNDNAME_NO_LEADING_UNDERSCORES |
								UNDNAME_NO_MS_KEYWORDS |
								UNDNAME_NO_FUNCTION_RETURNS |
								UNDNAME_NO_ALLOCATION_MODEL |
								UNDNAME_NO_ALLOCATION_LANGUAGE |
								UNDNAME_NO_MS_THISTYPE |
								UNDNAME_NO_CV_THISTYPE |
								UNDNAME_NO_THISTYPE |
								UNDNAME_NO_ACCESS_SPECIFIERS |
								UNDNAME_NO_THROW_SIGNATURES |
								UNDNAME_NO_MEMBER_TYPE |
								UNDNAME_NO_RETURN_UDT_MODEL;
					else
						flags = UNDNAME_NAME_ONLY;

					char *pResult = __unDName( unmangled, lpch, sizeof(unmangled), malloc, free, flags );
					if (pResult)
						lpch = pResult;
				}

                if ( *lpch == '_' && lpfme->clt == cltPub ) {
                    ++lpch;
                }
                _fmemcpy (
                    pchT,
                    lpch,
                    len = min ( cbMax, (int) _ftcslen (lpch) )
                );
                pchT += len;
                cbMax -= len;
                BMUnlock ( hName );
                EEFreeStr ( hName );
            }

            /* Get the argument values, but only for procs */

            if ( ( cbMax > 0 ) && lpfme->clt == cltProc ) {

                pchT = CLGetParams ( &htm, &lpfme->Frame, &cbMax, pchT, fShowArgs );
            }
            EEFreeTM ( &htm );
            break;

        }

    *pchT = '\0';
    errno = 0;      /* clear out any I/O errors */
    return pch;
}

/*** CLGetProcName
*
* Purpose: To format a line to be diplayed in the calls menu
*
* Input:
*   ifme     - The index into the stack frame
*   pch     - A pointer a buffer to put the string
*   cbMax   - max size of pch buffer
*   fShowArgs - Show call arguments
*
* Output:
*  Returns .....
*
* Exceptions:
*
* Notes:
*  to prevent output buffer overruns (pch) , we need to check the length
*  during format. but instead of checking for length every time we xfer into
*  output buffer, we check at selected points in the code and make certain
*  safe assumptions eg the length passed to us will be able to accomodate
*  the symbol name totally etc. Thus we remain ok and save some code & time
*
*************************************************************************/
char * PASCAL CLGetProcName (
int     ifme,
char *  pch,
int     cbMax,
BOOL    fShowArgs ) {
    char *  pchRet = (char *)NULL;

    if ( hlliFME ) {
        HFME    hfme = CLHfmeGetNth( ifme );

        if ( hfme ) {
            pchRet = CLGetProcFromHfme( hfme, pch, cbMax, fShowArgs );
        }
    }
    return pchRet;
}

/*** CLFreeWalkbackStack
*
*  Purpose: Free up any memory associated with the walkback stack and
*           reset the linked list
*
*  Input:
*
*  Output:
*
*  Exceptions:
*
*  Notes:
*
**************************************************************************/
void PASCAL CLFreeWalkbackStack( BOOL fReset ) {
    if ( fReset ) {
        iCallsCxf = -1;
    }
    if ( hlliFME ) {
        LLChlleDestroyLl( hlliFME );
        hlliFME = (HLLI)NULL;
    }
	ihfmeMacLast = -1;
	hfmeMacLast = (HFME)NULL;
	chfme = 0;
}

/*** CLFreeFmeData
*
*   Purpose: linklist mgr callback to free up cached data
*
*   Input:
*
*   Output:
*   Returns:
*
*   Exceptions:
*
*   Notes:
*
**************************************************************************/
void FAR PASCAL CLFreeFmeData( LPV lpv ) {
	LPFME	lpfme = (LPFME)lpv;

	if ( lpfme->lsz ) {
		_ffree( lpfme->lsz );
		lpfme->lsz = NULL;
	}
}

/*** CLGetWalkbackStack
*
*   Purpose: To set up the calls walkback structure
*
*   Input:
*
*   Output:
*   Returns:
*
*   Exceptions:
*
*   Notes:
*   The rules are:
*   BP must point to the previous BP on the stack.
*   The return address must be at BP+2.
*
*   Currently we don't support the _saveregs options or
*   _fastcall with stack checking on.
*
*   Also any function without symbolics are skipped in the
*   the trace back.
*************************************************************************/

void PASCAL CLGetWalkbackStack ( HPID hpid, HTID htid, UINT cFrame ) {

    ADDR addrPC = {0};
    ADDR addrBP = {0};
    ADDR addrSP = {0};
    BOOL fDone;
    BOOL fIsStackSetup = TRUE;
    int  ifme;
    HTID hvtid;
    static ADDR addrIPS = {0};
    static ADDR addrSPS = {0};
    static UINT cFrameLast = 0;
	STP	stp;
	UINT cMaxFrames = 1000;

	ADDR		addrData;
	SEGMENT		segData;

	if ( !lpprcCurr ||
		(stp = lpprcCurr->stp) == stpDead  ||
		stp == stpNotStarted ||
		stp == stpGone	
		) {
		memset( (void *)&addrIPS, 0, sizeof( ADDR ) );
		memset( (void *)&addrSPS, 0, sizeof( ADDR ) );
		CLFreeWalkbackStack( TRUE );
		return;
	}

    SYGetAddr ( hpid, htid, adrPC, &addrPC );
    SYGetAddr ( hpid, htid, adrStack, &addrSP );
    if (
		FAddrsEq(addrPC, addrIPS) && FAddrsEq(addrSPS, addrSP) &&
        ( !cFrameLast || ( cFrame && ( cFrame <= cFrameLast ) ) ) &&
        hlliFME != (HLLI) NULL
      ) {
            return;
    }
    else {
        CLFreeWalkbackStack ( FALSE );
    }

    hlliFME = LLHlliInit( sizeof( FME ), llfNull, CLFreeFmeData, NULL );
    fDone = ( hlliFME == (HLLI)NULL );
    ifme = 0;

    hvtid = htid;

	// This information will be common for all items on call chain
	SYGetAddr ( hpid, htid, adrData, &addrData );
	SYFixupAddr ( &addrData );
	segData = GetAddrSeg ( addrData );

	while( !fDone && --cMaxFrames) {
#if !(defined (OSDEBUG4))
        fDone = OSDGetFrame ( hpid, hvtid, cFrame, &hvtid ) != xosdNone;
#else
        fDone = OSDGetFrame ( hpid, hvtid, 1, &hvtid ) != xosdNone;
#endif
        if ( !fDone ) {
            HFME    hfme = LLHlleCreate( hlliFME );

            if ( hfme ) {
				FRAME FAR *	lpframe;
                LPFME   	lpfme;

                lpfme = (LPFME)LLLpvFromHlle( hfme );

                LLAddHlleToLl( hlliFME, hfme );

                SYGetAddr ( hpid, hvtid, adrPC, &addrPC );
                SYGetAddr ( hpid, hvtid, adrBase, &addrBP );

                lpfme->addrCSIP = addrPC;

				// The following code used to be in CLFillFrame
				// it is here now to avoid unnecessary calls
				// to OSDebug to get the data address (segment)
	            // CLFillFrame ( &addrBP, &lpfme->Frame );
			    if ( ADDR_IS_LI ( addrBP ) ) {
        			SYFixupAddr ( &addrBP );
			    }
				lpframe = &lpfme->Frame;

    			lpframe->mode = addrBP.mode;
			    SetFrameBPSeg ( *lpframe , GetAddrSeg ( addrBP ) );
    			SetFrameBPOff ( *lpframe , GetAddrOff ( addrBP ) );
			    SetFrameSLPSeg ( *lpframe , 0 );
    			SetFrameSLPOff ( *lpframe , 0 );
    			lpframe->SS  = (SEG16)GetAddrSeg ( addrBP );
			    lpframe->DS = (SEG16)segData;
			    lpframe->TID = htid;
    			lpframe->PID = hpid;

				// End CLFillFrame


                UnlockHlle( hfme );
                ++ifme;
            }
            else {
                fDone = TRUE;
            }
		}
    }

    if ( hlliFME ) {
        chfme = (int)LLChlleInLl( hlliFME );
    }
    else {
        chfme = 0;
    }

    if ( iCallsCxf > ifme ) {
        CVMessage( WARNMSG, CLSTKCHGD, CMDWINDOW );
        iCallsCxf = -1;
        // We want to update all our debug windows
        // with cxfIp...
        UpdateAllDebugWindows(&cxfIp) ;
    }


    // Set up the cache
    if ( ifme ) {
        HFME    hfme = LLHlleFindNext( hlliFME, hlleNull );
        LPFME   lpfme = (LPFME)LLLpvFromHlle( hfme );

        addrIPS      = lpfme->addrCSIP;
        SYGetAddr ( hpid, htid, adrStack, &addrSPS );
        cFrameLast = cFrame;
        UnlockHlle( hfme );
    }
}

/*** CLGetFuncCXF
*
*   Purpose:  To get a frame given an address of a function on the
*       calls stack
*
*   Input:
*       paddr   - A pointer to the address of the function
*
*   Output:
*       pCXF    - A pointer to an empty CXF, This will be filled in with
*             the scope and frame of the function.
*   Returns:
*           - A pointer to the CXF if successful, NULL otherwise.
*
*   Exceptions:
*
*   Notes: A NULL is returned if the function could not be found on
*       the calls stack, OR if the function has no symbolic info.
*
*************************************************************************/
PCXF LOADDS PASCAL CLGetFuncCXF ( PADDR paddr, PCXF pcxf ) {
    HFME    hfme;
    PCXF    pcxfRet = NULL;
    int     ifme;

    ASSERT ( ADDR_IS_LI (*paddr));
    _fmemset ( pcxf, 0, sizeof ( CXF ) );

    CLGetWalkbackStack ( hpidCurr, htidCurr, (UINT)NULL );

    ifme = CLLookupAddress( *paddr );

    // get the stack element
    if ( ifme != -1 && ( hfme = CLHfmeGetNth( ifme ) ) ) {
	    LPFME   lpfme = (LPFME)LLLpvFromHlle( hfme );

    	if ( lpfme->clt == cltProc || lpfme->clt == cltBlk ) {
            pcxf->cxt.addr  = *paddr;

	        SHSetCxtMod ( paddr, &pcxf->cxt );
    	    ASSERT( pcxf->cxt.hMod  &&  pcxf->cxt.hGrp );
            if ( lpfme->clt == cltProc ) {
	            pcxf->cxt.hProc = (HPROC)lpfme->symbol;
    	        pcxf->cxt.hBlk  = NULL;
            }
	        else {
    	        pcxf->cxt.hProc = NULL;
                pcxf->cxt.hBlk  = (HBLK)lpfme->symbol;
        	}

	        // fill in the CXF
    	    pcxf->Frame     = lpfme->Frame;
            pcxf->cxt.addr  = lpfme->addrCSIP;

	        pcxfRet = pcxf;
    	}
        UnlockHlle( hfme );
    }

    return pcxfRet;
}

BOOL CLDispCallInfo( int iCall ) {
	HFME    	hfme = CLHfmeGetNth( iCall );
	BOOL    	fMove;

	if( !hfme )
		fMove = FALSE;
	else
    {
		CView       *ptv;
		LPADDR		paddr;
		LPFME   	lpfme;

		lpfme =  (LPFME)LLLpvFromHlle(hfme);

		// Get the CXT
		_fmemset ( &cxfCalls, 0, sizeof ( CXF ) );
		SHSetCxt ( &lpfme->addrCSIP, SHpCXTFrompCXF ( &cxfCalls ) );
		*SHpFrameFrompCXF ( &cxfCalls ) = lpfme->Frame;
		iCallsCxf = (iCall == 0)? -1 : iCall;

		// Try and load corresponding source file...
		paddr = (LPADDR)(SHpADDRFrompCXT(SHpCXTFrompCXF(&cxfCalls)));

		ptv = MoveEditorToAddr( paddr );

		if ( ptv )
        {
			WORD	iLine;

			if ( SLLineFromAddr( paddr, &iLine, NULL, NULL ) )
			{
				if (iCall == 0) {
					EraseCallerLine();
					RestoreTraceLine(ptv->GetDocument());
				}
				else
					SetCallerLine(ptv->GetDocument(), (int)iLine, TRUE);
			}
		}
		else
        {
			EraseCallerLine();

			// No source available, open the DAM window if it's not
			// already opened.  The call to UpdateAllDebugWindows
			// below will realign the DAM window with the current
			// address
			if ( !pViewDisassy )
				OpenDebugWindow( DISASSY_WIN );

			// Make sure that the DAM window gets the focus (since there's
			// no source window).  Note:  The OpenDebugWindow above may
			// have failed, so do the extra check.
			if ( pViewDisassy && 
				GetCurView() != pViewDisassy ) // vc98 13474
				theApp.SetActiveView(pViewDisassy->GetSafeHwnd());
		}

		UpdateAllDebugWindows(&cxfCalls);

		// Unlock the handle now
		UnlockHlle(hfme);

		fMove = TRUE;
	}

	// Can only fail if the call stack list is toast
	return fMove;
}
