/////////////////////////////////////////////////////////////////////////
//
//	BPSMART.C - Smart (Do What I Mean) Breakpoint Setting
//
//	Copyright (C)1994, Microsoft Corporation
//
//	Purpose: Handle intelligent breakpoint setting.
//
//  Revision History:
//       Written by: Mark A. Brodsky
//
// 	Notes:
//
//////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

extern CXF 		cxfIp ;

extern int PASCAL BPGetContextOp (char FAR * );
extern void PASCAL BPStripSpace( char FAR * );
extern int PASCAL BPPastProlog (char FAR *, PCXF );
extern EESTATUS PASCAL CVParseBind ( PHTM, PCXT, int, char, char, char FAR * );

// Determine if lszCmd is a data expression,  If so, return
// the number of contexts in the expression.  If not, return 0
// (Lifted from BPMkDpi)
int PASCAL CCxtInExpr( char FAR * lszCmd ) {
	HTM			htm = (HTM)NULL;
	int			iRet = 0;
    EESTATUS    Err = EENOERROR;
    RTMI        rtmi;
    HTI         hTI;
    PTI         pTI;
    HCXTL       hCXTL;
    PCXTL       pCXTL;
    char *      szTmp;
    int         fTrace = 0;
    ushort      strIndex;
	CXF			cxf = *PcxfLocal();
	BOOL		fContext = FALSE;

	// if there is a problem get out
	if( lszCmd ) {
	    BPStripSpace( lszCmd );

	    // get over the ? or =
	    // handle quoting (had code '"' as quote string for now
	    lszCmd = CPAdvance(lszCmd, "");
	    if ( *lszCmd == chQuote) {
	        if ( (lszCmd = CPTrim(lszCmd, ' ') ) == NULL) {
	            return 0;		// was BADBKPTCMD; but that is a *lot* of contexts to return...
	        }
	    }

	    // get the TM for this guy
	    memset( &rtmi, 0, sizeof(rtmi) );
	    rtmi.fAddr    = TRUE;
	    rtmi.fSzBytes = TRUE;
	    rtmi.fLvalue  = TRUE;
	    rtmi.fSegType = TRUE;

	    is_assign = FALSE;

	    if(!(Err = EEParse(lszCmd, radixInput, fCaseSensitive, &htm, &strIndex))) {
	        if(Err = EEBindTM(&htm, SHpCXTFrompCXF(&cxf), TRUE, FALSE)) {
	            szTmp = (char *) malloc (_ftcslen (lszCmd) + 2);
	            if ( szTmp ) {
	                _ftcscpy ( (char FAR *)szTmp, lszCmd);
	                if ( BPGetContextOp (szTmp) ) {
	                    //
	                    // found the operator so append a 0 and bind it
	                    // to get an addr.  then get the addr past the
	                    // prolog code and rebind expression.
	                    //
	                    int iLen;

	                    iLen = _ftcslen (szTmp);
	                    szTmp[iLen] = '0';
	                    szTmp[iLen+1] = (char)NULL;

	                    if (!(Err = BPPastProlog (szTmp, &cxf))) {
	                        EEFreeTM (&htm);

	                        if (Err = CVParseBind (&htm,
	                                               SHpCXTFrompCXF(&cxf),
	                                               radixInput,
	                                               fCaseSensitive,
	                                               TRUE,
	                                               (lszCmd + iLen))) {
	                            CVExprErr (Err, MSGGERRSTR, &htm, NULL);
	                            Err = GEXPRERR;
	                        }

	                        fContext = TRUE;
	                    }
	                }
	                else {
	                    Err = BADBKPTCMD;
	                }
	            }
	            else {
	                Err = NOROOM;
	            }
	            free (szTmp);
	        }

	        if (!Err) {

	            EEvaluateTM (
	                &htm,
	                SHpFrameFrompCXF ( &cxf ),
	                EEHORIZONTAL
	            );

	            if (!(Err = EEInfoFromTM(&htm, &rtmi, &hTI)) &&
	                !(Err = EEGetCXTLFromTM(&htm, &hCXTL))) {

	                // get the TI
	                pTI = (PTI)BMLock (hTI);
	                pCXTL   = (PCXTL)BMLock (hCXTL);

	                ASSERT( pCXTL->cHCS );

					// Can't set a watchpoint or tracepoint on <void>
					if (pTI->cbValue == 0) {
						Err = BADBKPTCMD;
					}
					else {
						int		i;
						HCS *	phcs;

						for(
							i = 0, phcs = pCXTL->rgHCS;
							i < pCXTL->cHCS;
							 ++i, ++phcs
						) {
							// If there's an hsym, then non-trivial context
							if ( phcs->hSym != 0 ) {
								++iRet;
							}

							// No hsym, AND no EMI then a constant!
							else if ( !emiAddr( *SHpADDRFrompCXT( &phcs->CXT ) ) ) {
								++iRet;
							}
						}
					}
	                BMUnlock (hCXTL);
	                BMUnlock (hTI);
	                EEFreeCXTL(&hCXTL);
	                EEFreeTI(&hTI);
	            }
	            else {
	                CVExprErr (Err, MSGGERRSTR, &htm, NULL);
	                Err = GEXPRERR;
	            }
	        }
	    }
	    else {
	        CVExprErr (Err, MSGGERRSTR, &htm, NULL);
	        Err = GEXPRERR;
	    }
	}
	else {
		Err = BADBKPTCMD;
	}

    if (Err && htm) {
        EEFreeTM (&htm);
    }

    return(Err ? 0 : iRet);
}

//	FUNCTION:	CheckBpByName
// This handles eg function names out of the current
// scope, virtual addresses and other useful things the original code did not
// [DS96:10406 apennell]

static BOOL CheckBpByName( char *szExpr )
{
	BREAKPOINTNODE bbp;
	memset(&bbp, 0, sizeof(BREAKPOINTNODE));
	pbpnType(&bbp) = BPLOC;
	return ParseCV400Location(szExpr, &bbp) || ParseQC25Location (szExpr, &bbp);
}

// Determine if szExpr is a location.  Return TRUE if it is, FALSE otherwise.
// (Lifted from MWSetAddrFromExpr)
// (butchered heavily by apennell 9-23-96)

BOOL PASCAL FCodeAddress( char * psz ) {

	BOOL	fRet = FALSE;
	ADDR	addrT = {0};

	errno = CV_NOERROR;

	// Line number or simple address expression
   if ( line_number_address( &addrT, EEDATA | EECODE, psz ) )
		fRet = TRUE;
   // out-of-context function names
   else if (CheckBpByName( psz ))
	   return TRUE;							// skip context checking
    // Complex expression
	else if ( errno != NOCODE ) {
		HTM			htm;
		PCXF		pcxf = PcxfLocal();
		PCXT		pcxt = SHpCXTFrompCXF( pcxf );

		if ( CVParseBind (
				&htm,
				pcxt,
				radixInput,
				fCaseSensitive,
				TRUE,
				psz ) == EENOERROR
			&&
			 EEvaluateTM (
				&htm,
				SHpFrameFrompCXF( pcxf ),
				EEHORIZONTAL ) == EENOERROR
		) {

			// Get the address of the TM if it wasn't an lvalue
			if ( !fRet ) {
				USHORT	usSegType = EEANYSEG;

				if ( !BPADDRFromTM ( &htm, &usSegType, &addrT ) ) {
				    fRet = TRUE;
				}
			}
			EEFreeTM( &htm );
		}
	}

	// So far see if we can create a context for the address, if so, then it's
	// probably a code address.  This will NOT work for hard coded addresses in
	// dlls/exes w/o cvinfo
	if ( fRet ) {
		CXT	cxt = {0};

		if ( !SHSetCxt( &addrT, &cxt ) ) {
			fRet = FALSE;
		}
	}

	return fRet;
}

// Set a "smart" breakpoint.  Given a string (currently from the
// toolbar's edit control), set a breakpoint based upon the following
// criteria (all require SOME kind of symbol information):
//	
//		1) Code address -- set code breakpoint
//		2) Expression
//			a) If only 1 context (constants have contexts too),
//			   set a break when expression changes (length == 1);
//			b) If there is more than 1 context, set a break when
//			   expression is TRUE;
//			c) Fail
//		3) Fail
//
// A return of TRUE indicates that a breakpoint has been successfully set
//
BOOL PASCAL SetSmartBreakpoint( LPSTR szExpr ) {
	BREAKPOINTNODE	bpn;
	BOOL			fRet = FALSE;

	memset( &bpn, 0, sizeof( bpn ) );

	// For Java, attempt to load the class referenced by this breakpoint.
	// This is intended to load classes for method breakpoints.
	// For example: if the string java.util.Date.UTC() is given, we want
	// to try to load the class java.util.Date.
	// The approach here is to strip off any preceeding context, any trailing
	// method param types, and anything following the final dot.
	if (pDebugCurr && pDebugCurr->IsJava())
	{
		// Make a copy of the expression.
		LPTSTR pszExprCopy = new TCHAR[_tcslen(szExpr) + sizeof(TCHAR)];
		ASSERT(pszExprCopy != NULL);
		_tcscpy(pszExprCopy, szExpr);

		// Remove any leading context.
		LPTSTR pszClassName = _tcschr(pszExprCopy, _T('}'));
		if (pszClassName == NULL)
		{
			pszClassName = pszExprCopy;
		}
		else
		{
			// Skip the '}'.
			pszClassName = _tcsinc(pszClassName);
		}

		// Remove any leading white space.
		while (_istspace(*pszClassName))
		{
			pszClassName = _tcsinc(pszClassName);
		}

		// Remove any trailing param types.
		LPTSTR pszTrail = _tcschr(pszClassName, _T('('));
		if (pszTrail != NULL)
		{
			*pszTrail = _T('\0');
		}

		// Remove the method name (the stuff after the final dot).
		pszTrail = _tcsrchr(pszClassName, _T('.'));
		if (pszTrail != NULL)
		{
			*pszTrail = _T('\0');
		}

		// Attempt to load the class; ignore any failures.
		SHAddDll(pszClassName, TRUE);

		delete [] pszExprCopy;
	}

	if ( !DebuggeeAlive() || FCodeAddress( szExpr ) ) {
		// Code BP
		bpnType( bpn ) = BPLOC;

		// parse the string and instantiate the breakpoint
		if ( ParseCV400Location( szExpr, &bpn ) ||
			ParseQC25Location ( szExpr, &bpn )
		) {
			fRet = TRUE;
		}
	}
	else {
		int	ccxt = CCxtInExpr( szExpr );

		if ( ccxt != 0 ) {
			// Break when expression changes
			if ( ccxt == 1 ) {
				bpnType( bpn ) = BPEXPRCHGD;
				bpnExprLen( bpn ) = 1;
			}

			// Break when expression is TRUE
			else {
				bpnType( bpn ) = BPEXPRTRUE;
			}

			if ( ParseExpression( szExpr, &bpn ) ) {
				fRet = TRUE;
			}
		}
	}
	
	if ( fRet ) {
		bpnEnabled( bpn ) = TRUE;
		fRet = !!AddBreakpointNode( &bpn, TRUE, FALSE, FALSE, NULL);
	}

	CString str;
	if (fRet)
	{
		str.LoadString(IDS_BP_SET);
		str += szExpr;
		SetPromptUntilIdle(str);
	}
	else
	{
		str.LoadString(IDS_BP_NOT_SET);
		str += szExpr;
		SetPromptUntilIdle(str);
	}

	return fRet;
}
