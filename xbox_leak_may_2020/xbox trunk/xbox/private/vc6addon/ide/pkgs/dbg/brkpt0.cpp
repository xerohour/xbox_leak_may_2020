/*** brkpt0.c - breakpoint handlers                                     *
*
*   Copyright <C> 1989, Microsoft Corporation
*
*   Purpose:
*
*
*************************************************************************/
#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

int PASCAL BPFCxtForWpTp( LPDPI, LPDPF, HCS FAR *, LONG, BOOL, EESTATUS * );
EESTATUS PASCAL BPWatchPoint (LPDPI,LPDPF, PTI, PCXTL, PCXF, BOOL, BOOL );
EESTATUS PASCAL BPMkDPI (char FAR *, PCXF, LPDPI, LPDPF);
int PASCAL BPLoadValue ( LPBPI );
HBPI PASCAL BPXAdd (
    PCXF,
    HTHD,
    ushort,
    PADDR,
    ABPT,
    LPDPI,
    LPDPF,
    BOOL,
    BOOL,
    DWORD,
    DWORD,
    ushort,
    char FAR *,
    BOOL
);

// for other to use
HLLI    llbpi;
LPBPI   lpbpiFirst   = NULL;     // The begining of the breakpoint/watchpoint chain
LPBPI   lpbpiLast    = NULL;
int     fBPCCInExe   = FALSE;    // the break points have been set in the exe

void    SetupForWatch ( void );
LPBPS   EndWatch ( void );
BPNS    BpnsFromLpbpi ( LPBPI );

// ************************************************************************
// *                                                                      *
// *   SOME UTILITY ROUTINES                                              *
// *                                                                      *
// ************************************************************************

/*** BPCmpFunctionNames
*
* Purpose: strcmp, but ignore spaces. Also things like "main" and "main (void)" will match.
*
* Input:
*   sz1		function 1
*	sz2		function 2
*
* Output:
*  Returns FALSE if sz1 and sz2 are equivalent (same excluding spaces)
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
BOOL BPCmpFunctionNames( char * sz1, char * sz2 ) {
	while( *sz1 && *sz2 ) {
		while( _istspace( (_TUCHAR)*sz1 ) ) {
			++sz1;
		}

		while( _istspace( (_TUCHAR)*sz2 ) ) {
			++sz2;
		}

		if (*sz1 != *sz2)
		{
			break;
		}

		if ( *sz1 ) {
			++sz1;
			++sz2;
		}
	}

	if ( *sz1 != *sz2 ) {
		// They don't match, but one of them is end of string and the other one
		// is '('. Allow this case.
		if (*sz1 == '\0' || *sz2 == '\0' )
		{
			if (*sz1 == '(' || *sz2 == '(' )
				return FALSE;
			else
				return TRUE;
		}
		else
		{
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}


 /*** BPGetContextOp                           //[33]
*
* Purpose:  Get the context operator from expression.
*
* Input:    char * sz       pointer to expression
*
* Output:
*  Returns .....
*       fOp         TRUE if found a context operator.
*                   If found then the string is modified and
*                   null terminated after the operator.
*
*       fOp         FALSE if not found
*
*
* Exceptions:   This function looks for {} pairs outside of quotes.  Anything
*       more than that will be wishful thinking.
*
* Notes:
*
*************************************************************************/
int PASCAL BPGetContextOp (char FAR * lsz) {

    int fDone = FALSE;
    int fQuoted = 0;
    int cOpen = 0;
    int fOp   = FALSE;

    while ( !fDone && *lsz ) {

        switch ( *lsz ) {

            case chBeginCxtOp :
                if ( !fQuoted ) {
                    cOpen ++;
                }
                break;

            case chEndCxtOp :
                if ( !fQuoted ) {
                    cOpen--;
                    fOp = TRUE;
                }
                break;

            case chQuote :
                if ( fQuoted ) {
                    fQuoted--;
                }
                else {
                    fQuoted++;
                }
                break;
        }
        lsz++;

        if ( !cOpen ) {
            fDone = TRUE;
        }
    }
    if ( fOp ) {
        *lsz = (char)NULL;
    }
    return fOp;
}

/*** BPPastProlog
*
* Purpose: Set the PCXT addr past prolog code
*
* Input:    char * sz       pointer to expression NULL terminated.
*           PCXF            Pointer to full context packet used to set
*                           Breakpoints.  This functions uses this packet
*                           as a means of first finding out what the current
*                           context is and then to get the context a user may
*                           have specified. Modify it and return it to the caller.
* Detail:
*   Contexts with proc specified will always point at the proc entry.
*   However, variables are not visible to CodeView when In or on
*   function prolog code.  This functions will take the start address and
*   add the start offset to fake the expression evaluator into thinking that
*   BP is set and modify the CXF packet passed in so that it can be used to
*   override the lexical scoping rules determined by SHIsInProlog.
*
* Output:
*  Returns .....
*   int         iRet    TRUE if successful
*   EESTATUS    eest    If EE failure somewhere.
*
*
*
* Exceptions:
*
* Notes:
*   Warning: I munge the CXF to point past the prolog so before
*        before calling this function make a copy and restore.
*
*   Becareful if grp and mod are not the same subsequent evaluation
*   of the expression will use grp information over mod information.
*   Getting symbols etc.
*
*   In case of EE Error this function will store the message in CVExprErr
*   static buffer.
*
*************************************************************************/
int PASCAL BPPastProlog (char FAR * lsz, PCXF pcxf) {
    EESTATUS    eest;
    HTM         htm = (HTM)NULL;
    HCXTL       hctl;
    PCXTL       pctl;
    int         iRet = FALSE;

    //
    // given a context operator only expression, ie. {...}0. parse and
    // bind it and then get the context list for all contexts and symbols
    // in the users expression.
    // Then get the proc, mod, grp, and address for the given context
    // operator.
    // Once all information is gathered the htm for the above operator
    // only expression is freed.
    //
    if (!(eest = CVParseBindCxtl (
            &htm,
            SHpCXTFrompCXF ( pcxf ),
            radixInput,
            fCaseSensitive,
            TRUE,
            &hctl,
            lsz ))) {
        ASSERT (SHpCXTFrompCXF ( pcxf ) != NULL);
        pctl = (PCXTL)BMLock (hctl);

        if ( SHHPROCFrompCXT ( &pctl->rgHCS[0].CXT ) ) {
            PCXT    pcxt = &pctl->rgHCS[0].CXT;

            // Copy over the addr info (all of it)
            SHpCXTFrompCXF( pcxf )->addr = pcxt->addr;

            // Adjust the offset to be the debug start
            SetAddrOff (
                &SHpCXTFrompCXF ( pcxf )->addr,
                SHGetDebugStart ( SHHPROCFrompCXT( pcxt ) )
            );


            SHpCXTFrompCXF( pcxf )->hGrp  = SHHGRPFrompCXT( pcxt );
            SHpCXTFrompCXF( pcxf )->hMod  = SHHMODFrompCXT( pcxt );
            SHpCXTFrompCXF( pcxf )->hProc = SHHPROCFrompCXT( pcxt );
        }
        BMUnlock (hctl);
    }
    else {
        iRet = GEXPRERR;
        CVExprErr (eest, MSGGERRSTR, &htm, NULL);
    }

    if ( htm ) {
        EEFreeTM(&htm);
    }
    return iRet;
}


// ************************************************************************
// *                                                                      *
// *   THE USER INTERFACE ROUTINES TO THE BREAKPOINT COMMANDS             *
// *                                                                      *
// ************************************************************************

/*** BPGetDataExpr
*
* Purpose:
*
* Input:
*
* Output:
*  Returns
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
char FAR * PASCAL BPGetDataExpr(HBPI hbpi, char FAR *  lpch, int fNotDlg, unsigned short fNoTrunc)
{
    HCXTL       hCXTL = (HCXTL)NULL;
    PCXTL       pCXTL = NULL;
    EEHSTR      hStr  = (EEHSTR)NULL;
    char FAR *  lsz = (char FAR *)NULL;
	char FAR *	lszCxt	= NULL;
    char *      szExp  = NULL;
    char *      sz;
    EERADIX     Radix;
    LPBPI       lpbpi;
    ushort      strIndex;

    lpbpi = (LPBPI)LLLpvFromHlle(hbpi);
    //
    // get the user entered expression
    if( !EEGetExprFromTM(&lpbpi->lpDpi->hTM, &Radix, &hStr, &strIndex)) {
        lsz = (char FAR *)BMLock (hStr);

        // clear out this string
        if( lsz ) {
            szExp = (char *)malloc (_ftcslen (lsz) + 1);
            if (szExp) {
                _ftcscpy ((char FAR *)szExp, lsz);
                BMUnlock (hStr);
                EEFreeStr(hStr);
                lsz = NULL;

                //
                // make a copy because BPGetContextOp modifies string
                // See if the expression already has an expression.
                // If it does then don't get the expression associated
                // to the Terminal Node (TM), just display the users
                // version.
                //
				sz = _ftcsdup (szExp);
                if (!BPGetContextOp (sz)) {

                    // get the current context
                    if (!EEGetCXTLFromTM(&lpbpi->lpDpi->hTM, &hCXTL) &&
                        (pCXTL = (PCXTL)BMLock (hCXTL)) &&
                        !EEFormatCXTFromPCXT (&pCXTL->CXT, &hStr)) {

                        lsz = (char FAR *)BMLock (hStr);
                        if ( !*lsz ) {
                            BMUnlock (hStr);
                            EEFreeStr(hStr);
                            lsz = (char FAR *)NULL;
                        }
                    }
                    if (lsz) {
                        lszCxt = (char FAR *)LDSFmalloc(_ftcslen (lsz) + 1);
                        if (lszCxt) {
                            _ftcscpy (lszCxt, lsz);
                        }
                        else {
                            errno = NOROOM;
                        }

                        // free all of the strings
                        if (pCXTL) {
                            BMUnlock (hCXTL);
                            EEFreeCXTL(&hCXTL);
                        }
                        if (lsz) {
                            BMUnlock (hStr);
                            EEFreeStr(hStr);
                        }
                        else {
                            hCXTL = (HCXTL)NULL;
                        }
                    }
                }
                else {
                    int cch = _ftcslen( sz );

                    lszCxt = (char FAR *)LDSFmalloc(cch + 1);
                    if ( lszCxt ) {
                        _ftcscpy( lszCxt, sz );
                        memmove( szExp, szExp + cch, _ftcslen ( szExp ) - cch + 1 );
                    }
                    else {
                        errno = NOROOM;
                    }
                }
                free( sz );
            }
            else {
                BMUnlock ( hStr );
                errno = NOROOM;
            }
        }
    }

    // tracepoint or watch point
    if( fNotDlg ) {
        *lpch++ = (char)( lpbpi->dpf.f.fEvalExpr ? '?' : '=' );
    }

    // put in open quote
    *lpch++ = chQuote;

    if (lszCxt) {
        if (fNoTrunc) {
            // put in the context operator
            _ftcscpy(lpch, lszCxt);
            lpch += _ftcslen(lpch);
        }
        else {
            lpch = BPShortenSzCxt ( lpch, lszCxt );
        }
        *lpch++ = ' ';
    }

    if (szExp) {
        _ftcscpy(lpch, szExp);
        lpch += _ftcslen(lpch);
    }

    // put in close quote
    if ( *(lpch - 1) == ' ') {
        *(lpch - 1) = chQuote;
        lpch++;
    }
    else {
        *lpch++ = chQuote;
    }

    *lpch = '\0';
    if (lszCxt) {
        LDSFfree(lszCxt);
    }
    if (szExp) {
        free (szExp);
    }
    UnlockHlle(hbpi);
    return(lpch);
}

LSZ NEAR PASCAL LszNullContext ( LSZ lsz ) {
    _ftcscpy ( lsz, "{}" );
    return lsz + 2;
}

///////////////////////////////////////////////////////////////////////////
//
//  Return value for failure is a string containing "{}" NOT NULL!
//  Furthermore, the pointer returned points to the NULL terminator of the
//  return string not the beginning.
//
///////////////////////////////////////////////////////////////////////////
LSZ PASCAL BPShortenSzCxt ( LSZ lszTrunc, LSZ lszFull ) {
    SZ   szAlloc = NULL;
    PCH  pch     = NULL;
    PCH  pchFunc = NULL;
    PCH  pchFile = NULL;
    PCH  pchExe  = NULL;
    BOOL fDone   = FALSE;
    CHAR szFname [ _MAX_CVFNAME ];
    CHAR szExt   [ _MAX_CVEXT ];

    lszFull = _ftcschr ( lszFull, _T('{') );
    if ( !lszFull ) {
        return LszNullContext ( lszTrunc );
    }
    lszFull += 1;
    while ( *lszFull && *lszFull == ' ' ) lszFull++;
    if ( !*lszFull ) {
        return LszNullContext ( lszTrunc );
    }

    szAlloc = pch = (char *)malloc(_ftcslen( lszFull ) + 1);

    if ( !szAlloc ) {
        return LszNullContext ( lszTrunc );
    }

    _ftcscpy ( (LPCH) pch, lszFull );

    // Find & Null terminate funcname

    if ( *pch == ',' ) {
        pch += 1;
    }
    else if ( *pch == '}' ) {
        fDone = TRUE;
    }
    else {
        pchFunc = pch;
        pch     = _ftcspbrk ( pch, ",}(" );

        if ( pch == NULL || *pch == '}' ) {
            fDone = TRUE;
        }
        if ( pch != NULL ) {
			int	iLevel = *pch == '(' ? 1 : 0;

            *pch = '\0';
            pch += 1;
	
			// Skip whatever is enclosed by outer parens
			if ( iLevel ) {
				while( *pch && iLevel ) {
					if ( *pch == ')' ) {
						--iLevel;
					}
					else if ( *pch == '(' ) {
						++iLevel;
					}

					pch = _ftcsinc( pch );
				}

				// Since this is generated (not user typed in), the
				// context specifier doesn't need validation.  Now that
				// the function has been skipped over, find the next
				// position (or end) for the mod/exe.  Note: the function
				// has been Null terminated by now
				pch = _ftcspbrk ( pch, ",}" );

	            if ( pch == NULL || *pch == '}' ) {
	                fDone = TRUE;
	            }
	            if ( pch != NULL ) {
	               	pch += 1;
				}
			}
        }
    }

    // Find & Null terminate filename

    if ( !fDone ) {
        while ( *pch && *pch == ' ' ) pch++;
        if ( *pch == ',' ) {
            pch += 1;
        }
        else if ( *pch == '}' ) {
            fDone = TRUE;
        }
        else {
            pchFile = pch;
            pch = _ftcspbrk ( pch, ",}" );
            if ( pch == NULL || *pch == '}' ) {
                fDone = TRUE;
            }
            if ( pch != NULL ) {
                *pch = '\0';
               	pch += 1;
            }
        }
    }

    // Find & Null terminate Exe Name

    if ( !fDone ) {
        while ( *pch && *pch == ' ' ) pch++;
        if ( *pch != '\0' && *pch != '}' ) {
            pchExe = pch;
        }

        pch = _ftcschr ( pch, _T('}') );
        if ( pch != NULL ) {
            *pch = '\0';
        }
    }

    // Start building the output string
    *lszTrunc++ = '{';

    if ( pchFunc ) {
        _ftcscpy ( lszTrunc, (LSZ) pchFunc );
        lszTrunc += _ftcslen ( lszTrunc );
    }
    *lszTrunc++  = ',';

    if ( pchFile ) {
        SplitPath ( pchFile, NULL, NULL, szFname, szExt );
        _ftcscpy ( lszTrunc, (LSZ) szFname );
        lszTrunc += _ftcslen ( lszTrunc );
        _ftcscpy ( lszTrunc, (LSZ) szExt );
        lszTrunc += _ftcslen ( lszTrunc );
    }

    *lszTrunc++  = ',';

    if ( pchExe ) {
        // Under NT, these functions has been renamed.  Of course, we
        // should really just be including windows.h instead of doing
        // all this ridiculous nonsense.

        BOOL PASCAL OemToCharA(const CHAR FAR *, LPSTR);
        #define OemToAnsi OemToCharA

        LPSTR PASCAL CharLowerA(LPSTR);
        #define AnsiLower CharLowerA

        SplitPath ( pchExe, NULL, NULL, szFname, szExt );

        // Convert from OEM to ANSI
        OemToAnsi(szFname, szFname);
        AnsiLower(szFname);
        OemToAnsi(szExt, szExt);
        AnsiLower(szExt);

        _ftcscpy ( lszTrunc, (LSZ) szFname );
        lszTrunc += _ftcslen ( lszTrunc );
        _ftcscpy ( lszTrunc, (LSZ) szExt );
        lszTrunc += _ftcslen ( lszTrunc );
    }
    *lszTrunc++  = '}';
    *lszTrunc  = '\0';

    free(szAlloc);

    return lszTrunc;
}

/*** BPGetBpCmd
*
* Purpose: To format and address into symbol form if possble
*
* Input:
*
* Output:
*  Returns The pointer to the users buffer
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
char FAR * PASCAL BPGetBpCmd(HBPI hbpi, int mSegType, EBPT FAR * pBpSym, char FAR * lszCmd, unsigned short fNoTrunc) {
    HSYM        hSym;
	CXT 		CXTT;
    char FAR *  lpch;
    EEHSTR      CxtStr;
    HTM         hTM;
    LPBPI       lpbpi;
    int         fLang;
    HMOD        hmod = 0;
    ADDR        addr = {0};
    WORD        wLine;
    SHOFF       dbLine = 0;
	EBPT		ebptIn = *pBpSym;

    if( !lszCmd ) {
        return(NULL);
    }

    lpbpi = (LPBPI)LLLpvFromHlle(hbpi);
    fLang = lpbpi->fLang;

//  lszHeadCmd = lszCmd;
    *pBpSym = BPUNKNOWN;
    memset(&CXTT, 0, sizeof(CXT));
    SHHMODFrompCXT(&CXTT) = (HMOD) NULL;

    // load the line pkt
    if(SHSetCxt(&lpbpi->CodeAddr, &CXTT)) {
        hmod = SHHMODFrompCXT(&CXTT);
        addr = lpbpi->CodeAddr;
    }

	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////
	//
	// NOTE:
	//		Since the EE can't correctly parse a disambiguated function
	//		the following HACK is in.   The caller passes in a EBPT *
	// 		which is filled in here.  If the incoming value is BPLINE
	// 		then don't even try to get a nearest symbol or proc!!!
	//		
	//		markbro 10/14/93
	//
	///////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////

    // check the symbol table
    if ( ebptIn != BPLINE && hmod &&
        !SHGetNearestHsym(
            &lpbpi->CodeAddr,
            hmod,
            mSegType,
            &hSym
        )
       ) {

        if( hSym == (HSYM) SHHBLKFrompCXT(&CXTT) ) {
            SHHBLKFrompCXT(&CXTT)   = NULL;
        }
        else if( hSym == (HSYM) SHHPROCFrompCXT(&CXTT) ) {
            SHHPROCFrompCXT(&CXTT)  = NULL;
        }
    }

    // check the publics
    else if ( ebptIn != BPLINE && hmod  &&
             !PHGetNearestHsym(
                &lpbpi->CodeAddr,
                SHHexeFromHmod ( hmod ),
                &hSym
              )
            ) {

        SHHBLKFrompCXT(&CXTT)    = NULL;
        SHHPROCFrompCXT(&CXTT)  = NULL;
    }

    // check for a line
    else if (
        SLLineFromAddr ( &lpbpi->CodeAddr, &wLine, NULL, &dbLine ) &&
        dbLine == 0
    ) {

        SHHBLKFrompCXT(&CXTT)    = NULL;

		// Don't zero out the HPROC.  We want this to
		// ensure that source lines aren't overloaded!

        // SHHPROCFrompCXT(&CXTT)  = NULL;
        hSym = NULL;
    }
    else {
        hSym  = NULL;
        wLine = 0;
    }

    // quote the expression
    *lszCmd++ = chQuote;
    *lszCmd = '\0';

    // put in any context operator
    if( (hSym || wLine) && !EEFormatCXTFromPCXT(&CXTT, &CxtStr) ) {

        // copy over the context string
        if( (lpch = (char FAR *)BMLock (CxtStr)) ) {
            if (fNoTrunc) {

                // full context string
                while( *lpch ) {
                    *lszCmd++ = *lpch++;
                }
            }
            else {

                lszCmd = BPShortenSzCxt ( lszCmd, lpch );
            }
            BMUnlock (CxtStr);
        }
        EEFreeStr(CxtStr);

        // put in a space
        *lszCmd++ = ' ';
    }

    // we have a symbol
    if( hSym ) {

        ushort   strIndex;
        EEHSTR      hszName;
        EEHSTR      hszType;
        char FAR *  lszType;

        // skip beyond leading quoting char
//      lszHeadCmd++;
        *lszCmd = '\0';

        if ( ! EEGetTMFromHSYM ( hSym, &CXTT, &hTM, &strIndex, TRUE, TRUE) ) {

            EEGetNameFromTM(&hTM, &hszName);
            if ( lpbpi->bpf.f.fAmbig || lpbpi->bpf.f.fParamGiven ) {
                EEGetTypeFromTM(&hTM, hszName, &hszType, 1L);
            }
            else {
                hszType = hszName;
            }

            lszType = (char FAR *)BMLock(hszType);
//          *lszHeadCmd = '\0';

            // skip the structure EEGetTypeFromTM places at the front of
            // the buffer

            if ( lpbpi->bpf.f.fAmbig || lpbpi->bpf.f.fParamGiven ) {
                _ftcscpy(lszCmd, lszType + sizeof (HDR_TYPE));
            }
            else {
                _ftcscpy(lszCmd, lszType );
            }
            _ftcscat(lszCmd, "\"");
            EEFreeTM(&hTM);
            BMUnlock(hszType);
            if ( lpbpi->bpf.f.fAmbig ) {
                EEFreeStr(hszName);
            }
            EEFreeStr(hszType);
            UnlockHlle(hbpi);
			*pBpSym = BPSYMBOL;
            return(_ftcschr(lszCmd, _T('\0')));
        }
        else {

            // could not get TM
            if (hTM) {
                EEFreeTM(&hTM);
            }
            UnlockHlle(hbpi);
            return NULL;
        }
    }
    // we have a line
    else if ( wLine ) {
        char     LnNbr[20];

        *pBpSym = BPLINE;
		_ultoa((unsigned long) wLine, LnNbr, 10 );
        *lszCmd++ = '.';
        _ftcscpy(lszCmd, LnNbr);
        lszCmd += _ftcslen( LnNbr );
    }

    // its a cs:ip
    else {
        ADDR addrT = lpbpi->CodeAddr;

        *pBpSym = BPADDR;
        if ( ADDR_IS_LI ( addrT ) ) {
            SYFixupAddr ( &addrT );
        }
        EEFormatAddress ( &addrT, lszCmd );
        lszCmd += _ftcslen( lszCmd );
    }

    *lszCmd++ = chQuote;

    *lszCmd = '\0';

    UnlockHlle(hbpi);
    return(lszCmd);
}


/*** BPUIFormat
*
* Purpose: given a bpi, formant the breakpoint into an ascii string
*
* Input:
*   lpbpi   - A pointer to the specified bpi (breakpoint)
*   fPrtPc  - True is both the specified and current value of the
*             passcount should be specified. False is only the specified
*             value should be printed
*   szOut   - A place to put the string, the string should be at least
*             axMax in length.
*   cchMax  - Size of szOut buffer
*
* Output:
*  Returns szOut on success or NULL otherwise
*
* Exceptions:
*
*************************************************************************/
char FAR * PASCAL BPUIFormat (
HBPI            hbpi,
unsigned short  fPrtPc,
unsigned short  fNoTrunc,
char FAR *      lszOut,
unsigned short  cchMax ) {
#ifndef OLD_BPUIFORMAT
	PBREAKPOINTNODE	pbpn = BHFirstBPNode();

	while( pbpn && pbpnBPhbpi( pbpn ) != hbpi ) {
		pbpn = pbpnNext( pbpn );
	}

	ASSERT( pbpn );
	FormatDisplayBP( pbpn, (PSTR)lszOut, cchMax );

#else // !OLD_BPUIFORMAT
    char FAR *      lpch;
    char FAR *      lpbuf;
    EBPT            BpSym = BPUNKNOWN;
    unsigned short  cbComma = 0;
    char            buf1[10];
    char            buf2[10];
    LPBPI           lpbpi;
    HPRC            hprc = hprcCurr;

    if ( ! ( lpbuf = lpch = (char FAR *)_fmalloc ( 1024 ) ) ) {
        return NULL;
    }

    lpbpi = (LPBPI)LLLpvFromHlle(hbpi);

    SYSetContext ( lpbpi->hprc, (HTHD) NULL, FALSE );

    // initialize the pointer
    *lpch = '\0';

    // put in the flags
    *lpch++ = (char)( lpbpi->bpf.f.fVirtual ? 'V' : ' ' );
    *lpch++ = (char)( lpbpi->bpf.f.fActive ? 'E' : 'D' );
    *lpch++ = ' ';

    // under os2 put in the thread it
    if ( lpprcCurr->fHasThreads ) {

        if ( !fBlocking ) {
            lpch += GetProcId(
                    lpch,
                    lpbpi->hprc,
                    fPrtPc ? epiDefault : epiTitle,
                    20
                );

            *lpch++ = ' ';
        }

        if( lpbpi->hthd ) {
            CVsprintf(lpch, "%03d ", TidFromHthd ( lpbpi->hthd ) );
            lpch += 4;
        }
    }

    // code breakpoint
    if(lpbpi->bpf.f.fBpCode && !(lpbpi->lpDpi && lpbpi->dpf.f.fFuncBpSet)) {
        *lpch++ = ' ';
        // put in the formated string
        BPGetBpCmd( hbpi, EECODE, &BpSym, lpch, fNoTrunc );
        lpch = lpch + _ftcslen (lpch);
    }

    // data breakpoint
    if(lpbpi->bpf.f.fBpData) {
        if ( lpch != lpbuf ) {
            *lpch++ = ' ';
        }
        (void)BPGetDataExpr(hbpi, lpch, TRUE, fNoTrunc );
        lpch = lpch + _ftcslen (lpch);
    }

    if ( lpbpi->bpf.f.fAmbig && fIgnoreAmbigBPs) {
        // write out the magic /A (Ambiguous) flag
        lpch += CVsprintf(lpch, " /A");
    }

    // length
    if( lpbpi->dpf.f.fDataRange &&  lpbpi->lpDpi->cData ) {
        *lpch++ = ' ';
		lpch += CVsprintf(lpch, " /R%s", _itoa(lpbpi->lpDpi->cData,buf1,radix));
    }

    // put in the pass count
    if(lpbpi->bpf.f.fPass) {
        *lpch++ = ' ';
        if( fPrtPc ) {
			lpch += CVsprintf(lpch, " /P%s-%s", _ultoa((ULONG)lpbpi->cPass,buf1,radix), _ultoa(lpbpi->cPassCur,buf2,radix));
        }
        else {
			lpch += CVsprintf(lpch, " /P%s", _ultoa((ULONG)lpbpi->cPass,buf1,radix));
        }
    }

    // command
    if(lpbpi->lszCmd) {
        *lpch++ = ' ';
        lpch += CVsprintf(lpch, " /C\"%Fs\"", lpbpi->lszCmd);
    }

    if ( lpprcCurr->fHasMessages && lpbpi->bpf.f.fMessage ) {
        TCHAR rgch [ 80 ];
        BOOL fRealMsg = FALSE;
		DWORD iMap;

        if ( lpbpi->dwMask ) {
			LPMASKMAP lpMaskMap;

			rgch[0] = _T('\0');
			VERIFY ( OSDGetMessageMaskMap( hpidCurr, htidCurr, &lpMaskMap ) == xosdNone );
			for (iMap=0; iMap<lpMaskMap->dwCount; iMap++) {
				if ( lpbpi->dwMask & lpMaskMap->lpMaskInfo[iMap].dwMask ) {
					if ( rgch[ 0 ] != _T('\0') ) {
						_ftcscat( rgch, ", " );
					}
					
					_ftcsncat(rgch,
						lpMaskMap->lpMaskInfo[iMap].lszMaskText,
						sizeof(rgch) / sizeof(TCHAR));
				}
			}
            fRealMsg = TRUE;
        }
        else {
            if ( fRealMsg = (BOOL)( lpbpi->lMessage != 0xffffffffL ) ) {
				LPMESSAGEMAP lpMessageMap;
				VERIFY (OSDGetMessageMap( hpidCurr, htidCurr, &lpMessageMap ) == xosdNone );
				for (iMap = 0; iMap<lpMessageMap->dwCount; iMap++) {
					if (lpbpi->lMessage == lpMessageMap->lpMsgInfo[iMap].dwMsg) {

						_ftcsncpy(rgch,
							lpMessageMap->lpMsgInfo[iMap].lszMsgText,
							sizeof(rgch) / sizeof(TCHAR));
						break;
					}
				}
            }
        }

        if ( fRealMsg ) {
            *lpch++ = ' ';
            *lpch++ = '/';
            *lpch++ = 'M';

            _ftcscpy ( lpch, rgch );
            lpch += _ftcslen ( rgch );
        }

        if ( lpbpi->bpf.f.fNoStop ) {
            *lpch++ = ' ';
            *lpch++ = '/';
            *lpch++ = 'D';
        }
    }

    *lpch = '\0';

    UnlockHlle(hbpi);
    SYSetContext ( hprc, (HTHD) NULL, FALSE );

    _ftcsncpy ( lszOut, lpbuf, cchMax );
    if ( _ftcslen ( lpbuf ) >= cchMax ) {
        lszOut[cchMax] = '\0';
    }
    _ffree ( lpbuf );
#endif // !OLDBPUIFORMAT
    return(lszOut);
}


/*** BPGetNbr
*
* Purpose: Return a breakpoint Number given an address
*
* Input: lpbpi  - A pointer to the breakpoint info packet
*
* Output:
*  Returns The Breakpoints index or -1 on error
*
* Exceptions:
*
* Notes: This is the opposite of BPGetStruct
*
*************************************************************************/
int PASCAL BPGetNbr(
HBPI hbpi ) {
    int i = 0;
    HBPI    hbpiT = hbpiNull;

    while( hbpi != ( hbpiT = LLHlleFindNext( llbpi, hbpiT ) ) && hbpiT ) {
        ++i;
    }

    // if it was found
    return( hbpiT ? i : -1 );
}

/*** BPFCxtForWpTp
*
* Purpose:  Determin/set context if required for watch/tracepoints.
*
* Input:
*      LPDPI    lpDpi   Pointer to databp information like addrs...
*      LPDPF    lpDpf   databp flags
*      PHCS     phcs    Pointer to hcs to check context for.
*      PEESTAT  peest   Pointer to ee status return value for caller.
*
* Output:
*  Returns
*    Returns FALSE if phcs is not consumed, else TRUE.  Also,
*
*       EESTATUS    eest    0 if successfull
*                           !0 if failure.  If EE failure CVExprErr
*                              static buffer will have string message.
*
* Exception
*
* Notes:
*
*************************************************************************/
typedef SYMTYPE         FAR *SYMPTR;
typedef REGSYM          FAR *REGPTR;
typedef BPRELSYM16      FAR *BPRELPTR16;
typedef BLOCKSYM16      FAR *BLOCKPTR16;
typedef PROCSYM16       FAR *PROCPTR16;
typedef BPRELSYM32      FAR *BPRELPTR32;
typedef BLOCKSYM32      FAR *BLOCKPTR32;
typedef PROCSYM32       FAR *PROCPTR32;
typedef PROCSYMMIPS	FAR *PROCPTRMIPS;

int PASCAL BPFCxtForWpTp (
    LPDPI     lpDpi,
    LPDPF     lpDpf,
    HCS FAR  *phcs,
    LONG      off,
    BOOL      fContext,
    EESTATUS *peest
) {
    int     fRet = FALSE;
    HPROC   hps;
    HBLK    hblk;
    SYMPTR  pps;
    SYMPTR  pBlk;
    SYMPTR  psym;
    BOOL    fHsymLocked = FALSE;

    *peest = 0;

    if ( phcs && phcs->hSym ) {

		if (pDebugCurr && pDebugCurr->IsJava()) {

			// for Java, we'll treat all watchpoints as local (fContext)

			lpDpf->f.fContext = TRUE;

			// now put in the bounds of the watchpoint
			lpDpi->u.bp.BlkAddr = *(SHpADDRFrompCXT(&phcs->CXT));
			UOFFSET offBase;
			UOFFSET offLimit;
			if (OSDGetObjectLength(hpidCurr, htidCurr, &(lpDpi->u.bp.BlkAddr), &offBase, &offLimit) == xosdNone) {
				lpDpi->u.bp.oEnd = offLimit;
			}

			lpDpi->u.bp.ldbBp = off;
			lpDpf->f.fBpRel = TRUE;

			fRet = TRUE;

		}
		else {

			if (
				( psym = (SYMPTR) MHOmfLock ( phcs->hSym ) ) &&
				( fHsymLocked = TRUE ) &&
				( ( psym->rectyp == S_BPREL16 ) ||
				  ( psym->rectyp == S_BPREL32 ) ||
				  ( psym->rectyp == S_REGISTER ) ||
			  ( psym->rectyp == S_REGREL32 ) ||
				  fContext
				)
			) {

				fRet = TRUE;

				if ( psym->rectyp == S_REGISTER ) {
					lpDpi->iReg = ((REGPTR) psym)->reg;
					lpDpf->f.fReg = TRUE;
				} else if (
					psym->rectyp == S_BPREL16 || psym->rectyp == S_BPREL32 ||
			psym->rectyp == S_REGREL32
				) {
					lpDpi->u.bp.ldbBp = off;
					lpDpf->f.fBpRel = TRUE;
				} else if (
					psym->rectyp == S_LDATA16 || psym->rectyp == S_GDATA16 ||
					psym->rectyp == S_LDATA32 || psym->rectyp == S_GDATA32
				) {
					// Don't have to do anything specific for regular data

				} else {
					ASSERT ( FALSE );
				}

				MHOmfUnLock(phcs->hSym);
				fHsymLocked = FALSE;
				psym = (SYMPTR)NULL;

				lpDpf->f.fContext = TRUE;

				// now put in the bounds of the pps
				lpDpi->u.bp.BlkAddr = *(SHpADDRFrompCXT(&phcs->CXT));

				if ( ( hblk = SHHBLKFrompCXT(&phcs->CXT ) ) ) {
					pBlk = (SYMPTR) MHOmfLock ( hblk ) ;
					switch (pBlk->rectyp) {

						case S_BLOCK16:
							SetAddrOff ( &lpDpi->u.bp.BlkAddr, ((BLOCKPTR16)pBlk)->off );
							lpDpi->u.bp.oEnd = ((BLOCKPTR16)pBlk)->off +
							  ((BLOCKPTR16)pBlk)->len;
							break;
						case S_BLOCK32:
							SetAddrOff ( &lpDpi->u.bp.BlkAddr , ((BLOCKPTR32)pBlk)->off );
							lpDpi->u.bp.oEnd = ((BLOCKPTR32)pBlk)->off + ((BLOCKPTR32)pBlk)->len;
							break;
						default:
							ASSERT (FALSE);
					}
					MHOmfUnLock(hblk);
				}
				else if((hps = SHHPROCFrompCXT(&phcs->CXT))) {
					pps = (SYMPTR) MHOmfLock(hps);
					switch (pps->rectyp) {

						case S_LPROC16:
						case S_GPROC16:
							SetAddrOff (&lpDpi->u.bp.BlkAddr , ((PROCPTR16)pps)->off +
							  ((PROCPTR16)pps)->DbgStart);
							lpDpi->u.bp.oEnd = ((PROCPTR16)pps)->off + ((PROCPTR16)pps)->DbgEnd + 1;
							break;
						case S_LPROC32:
						case S_GPROC32:
							SetAddrOff (&lpDpi->u.bp.BlkAddr , ((PROCPTR32)pps)->off +
							  ((PROCPTR32)pps)->DbgStart);
							lpDpi->u.bp.oEnd = ((PROCPTR32)pps)->off + ((PROCPTR32)pps)->DbgEnd + 1;
							break;

				case S_LPROCMIPS:
						case S_GPROCMIPS:
							SetAddrOff (&lpDpi->u.bp.BlkAddr , ((PROCPTRMIPS)pps)->off +
							  ((PROCPTRMIPS)pps)->DbgStart);
							lpDpi->u.bp.oEnd = ((PROCPTRMIPS)pps)->off + ((PROCPTRMIPS)pps)->DbgEnd + 1;
							break;

				default:
							ASSERT (FALSE);
					}
					MHOmfUnLock(hps);
				}
				else {
					*peest = BADBKPTCMD;
				}
			}

			if ( fHsymLocked ) {
				fHsymLocked = FALSE;
				MHOmfUnLock(phcs->hSym);
			}
		}
    }
    return fRet;
}

/*** GetRealCountFrompCXTL
*
* Purpose : Count the number of non-trivial entries in a CXTL
* Input:
*	PCXTL	pCxtl  Pointer to a CXTL.
* Output:
*		Count of the number of entries in the CXTL which actually
*		have HSYM's associated with them
* Notes:
*		The cHCS in the pCXTL is useful when we want to determine
*		the actual number of addressable elements in a given expression.
*		This is because constants and structure elements in the expr
*		also contribute to this count.
* Returns  -- ushort  same type as that of the cCHS member.
***/

ushort 	PASCAL GetRealCountFrompCXTL ( PCXTL pCxtl )  {
	ushort retCount = 0 ;
	int i;

	for ( i = 0 ; i < pCxtl->cHCS ; i++ )  {
		if ( pCxtl->rgHCS[i].hSym != 0 )
			retCount++ ;
	} /* end for */
	return retCount ;
} /* end func */

/***  GetRealpHCSFrompCXTL
*		Purpose:
*			From a list of cxt's get the first one with a non-zero hsym.
*		Input:
*			PCXTL pCxtl pointer to a list of contexts.
*		Output:
*			pointer to the first HCS if one exists else NULL.
*
***/

HCS FAR * PASCAL GetRealpHCSFrompCXTL( PCXTL pCxtl ) {
	int i;

	for ( i = 0 ; i < pCxtl->cHCS ; i++ ) {
		if ( pCxtl->rgHCS[i].hSym != 0 ) {
			return (&(pCxtl->rgHCS[i]));
		}
	} /* end for */
	// None of the contexts were non-trivial. Possible if you have
	// an expression with just a constant say.
	return(NULL);
}

/*** BPTracePoint
*
* Purpose:  Process tracepoint entered by user.
*
* Input:
*       LPDPI       lpDpi   Pointer to databp information like addrs...
*       LPDPF       lpDpf   databp flags
*       PTI         pti     information about symbol
*       PCXTL       pcl     context list for all context for all symbols
*                           in the expression.
*       PCXF        pcxf    full context information packet.
*
* Output:
*  Returns
*       EESTATUS    eest    0 if successfull
*                           !0 if failure.  If EE failure CVExprErr
*                              static buffer will have string message.
*
* Exception
*
* Notes:
*
*
*
*
*
*************************************************************************/
EESTATUS PASCAL BPTracePoint (
    LPDPI lpDpi,
    LPDPF lpDpf,
    PTI   pti,
    PCXTL pcl,
    PCXF  pcxf,
    BOOL  fUserCxt,
    BOOL  fContext
) {

    EESTATUS    eest = EENOERROR;
	ushort		cxtCount = GetRealCountFrompCXTL(pcl) ;

    // if I don't have an address, get out
    if ( pti->fAddrInReg ) {

        lpDpi->iReg = pti->hReg;
        lpDpi->cbData = (ushort) pti->cbValue;
        lpDpf->f.fReg = TRUE;
    }
    else if (
        pti->fResponse.fLvalue ||
        pti->fResponse.fAddr   ||
        cxtCount -  fUserCxt <= 1
    ) {
        SYFixupAddr ( &pti->AI );

        lpDpi->cbData = (unsigned short)pti->cbValue;

		// The first check in the following "if" is to determine that the
		// bp could be set as a bp on a variable. To do this we see
		// how many contexts there are in a given expression, and if > 1
		// we just set a direct bp on the value. However the problem is
		// that constants/struct mbrs etc also contribute to this count.
		// So an expr such as foo.bar or &+ 1 has two contexts
		// by the cHCS count, although only one is significant.
		// cxtCount tracks the number of real contexts in a given expr.
		// [Cuda:3512  4/23/93 sanjays]

        if (
			((pDebugCurr && pDebugCurr->IsJava ()) || cxtCount - fUserCxt <= 1) &&
            BPFCxtForWpTp (
                lpDpi,
                lpDpf,
                (cxtCount?GetRealpHCSFrompCXTL(pcl):&pcl->rgHCS[0]),
                (long) (
                    GetAddrOff ( pti->AI ) -
                    SHpFrameFrompCXF ( pcxf )->BP.off
                ),
                fContext,
                &eest
            )
        ) {
            // OK from here!
        }
        // if it is an Lvalue
        else if( (pti->fResponse.fAddr || pti->fResponse.fLvalue) && !pti->fBPRel ) {
            lpDpi->u.d.DataAddr = pti->AI;
            SYUnFixupAddr ( &lpDpi->u.d.DataAddr );
        }

        // if it is a constant
        else if( pti->fResponse.fSzBytes && pti->cbValue >= sizeof(SHOFF)   ) {
            SetAddrOff ( &lpDpi->u.d.DataAddr , *((SHOFF FAR *) pti->Value) );

            SYSetEmi (
                hpidCurr,
                htidCurr,
                &lpDpi->u.d.DataAddr
            );
            lpDpi->cbData = sizeof(char);
            pti->cbValue = sizeof(char);

            // put in the segment value
            if (pti->cbValue    == (sizeof(SHOFF) + sizeof(SHSEG)) ) {
                SetAddrSeg ( &lpDpi->u.d.DataAddr ,
                *((SHSEG FAR *) &pti->Value[sizeof(SHOFF)]) );
            }
            else {
                SHREG   SHREG;

                SHREG.hReg = CV_REG_DS;
                DHGetReg(&SHREG, SHpCXTFrompCXF(pcxf));
                SetAddrSeg ( &lpDpi->u.d.DataAddr , (SHSEG) SHREG.Byte2 );
            }
            SYUnFixupAddr ( &lpDpi->u.d.DataAddr );
        }
    }
    else {
        eest = NOTLVALUE;
    }

    return eest;
}


/*** BPWatchPoint
*
* Purpose:  Process watchpoint entered by user.
*
* Input:
*       PDPI        lpDpi   Pointer to databp information like addrs...
*       LPDPF       lpDpf   databp flags
*       PTI         pti     information about symbol
*       PCXTL       pcl     context list for all context for all symbols
*                           in the expression.
*       PCXF        pcxf    full context information packet.
*
* Output:
*  Returns
*       EESTATUS    eest    0 if successfull
*                           !0 if failure.  If EE failure CVExprErr
*                              static buffer will have string message.
*
* Exception
*
* Notes:
*
*************************************************************************/
EESTATUS PASCAL BPWatchPoint (
    LPDPI lpDpi,
    LPDPF lpDpf,
    PTI   pti,
    PCXTL pcl,
    PCXF  pcxf,
    BOOL  fUserCxt,
    BOOL  fContext
) {
    EESTATUS        eest = EENOERROR;
    unsigned short  ihcs = 0;
    unsigned short  chcs = pcl->cHCS;
    unsigned short  fEmulate = TRUE;

    do {
        if ( BPFCxtForWpTp (
                lpDpi,
                lpDpf,
                &pcl->rgHCS[ ihcs++ ],
                (long) (
                    GetAddrOff ( pti->AI ) -
						SHpFrameFrompCXF ( pcxf )->BP.off
                ),
                fContext,
                &eest
            ) &&
            eest == EENOERROR
        ) {

            fEmulate = FALSE;
            eest = EENOERROR;
        }
    } while ( ihcs + fUserCxt < chcs && fEmulate );

    lpDpf->f.fEmulate = fEmulate;
    lpDpf->f.fEvalExpr = TRUE;
    return eest;
}

//*************************************************************************
//*                                                                       *
//*    STATE FILE ROUTINES                                                *
//*                                                                       *
//*************************************************************************

/*** BPStripSpace
*
* Purpose: Remove white spaces in beginning part of a BP command string
*          thru the 1st char after the end of a context so CPAdvance or
*          CPNextToken work as needed for BP parsing
*
* Input:   lsz - pointer to command string.  This has the white space
*                removed in the same buffer
*
* Output:  lsz - input string with white spaces selectively removed
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
void PASCAL BPStripSpace( char FAR * lsz ) {
    //
    // Remove white spaces only if there's a context here.  Doesn't
    // have to be in the beginning since we may get "= {cxt} blah"
    //
    if ( FindNonQuotedChar( lsz, chBeginCxtOp ) &&
        FindNonQuotedChar( lsz, chEndCxtOp ) ) {

        char FAR *  lpchIn = lsz + 1;
        char FAR *  lpchCur = lpchIn;
		int			iParen = 0;

		// CAV 5243: '"0x22:0x22" ?"{func,,} i"' wasn't parsing correctly -> GPF
		// this is because the space before the '?' was being stripped out
		// so that it looked like there was only one field (an address field)
		//
		// Replaced space stripper with a slightly more clever version of the same
		// the new version keeps one space before a '?', '=', or '/' because they
		// start a new token.... [rm]

        while (*lpchCur) {
			BOOL fInQuote = FALSE;

     		if (!fInQuote &&
				(lpchCur[0] == chEndCxtOp || lpchCur[1] == '?' ||
     			lpchCur[1] == '=' || lpchCur[1] == '/')) {
     			break;
			}
			// Don't use _isspace with extended characters !!!
			// CAVIAR #6792 01/10/93 v-natjm

			// Also, don't remove spaces inside of parens or quotes!
        	else if (iParen == 0 && !fInQuote && _istspace((unsigned char)*lpchCur)) {
	        	lpchCur++;
        	}
        	else {
				if ( *lpchCur == '(' ) {
					++iParen;
				}
				else if ( *lpchCur == ')' ) {
					--iParen;
				}
				else if ( *lpchCur == '\"' ) {
					fInQuote = !fInQuote;
				}
        		*lpchIn++ = *lpchCur++;
        	}
        }

        _fmemmove( lpchIn, lpchCur, _ftcslen( lpchCur ) + 1 );
    }
}

/*** BPParse
*
* Purpose: To parse a breakpoint command. This is used for the U,A,G,BP,and
*       V commands.
*
* Input:
*
* Output:
*  Returns
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
EESTATUS PASCAL BPParse( LPPBP  lpPbp ) {

    unsigned short  cPass           = 0;
    char FAR *      lpchNextTok;
    BOOL            fRangePossible  = FALSE;
    int             Ret             = EENOERROR;
    unsigned short  cRange          = 0;
	CXF				cxf				= *lpPbp->pCXF;
    BOOL            fWatchPoint = FALSE;
    BOOL            fTracePoint = FALSE;
    BOOL            fRange      = FALSE;
    BOOL            fPassCount  = FALSE;
    BOOL            fCommand    = FALSE;
    BOOL            fAddress    = FALSE;
    BOOL            fSpecial    = FALSE;
    BOOL            fParseMsg   = FALSE;
    BOOL            fMsg        = FALSE;

    // here we go!
    //Advance Past leading Delims if any.  This is necessary.
    lpPbp->lszCmd = CPAdvance(lpPbp->lszCmd, "");
	BPStripSpace( lpPbp->lszCmd );

    lpchNextTok = CPszToken(lpPbp->lszCmd, "");
    if ( *(lpPbp->lszCmd) == '\0') {

        // naked bp, this is == to bp cs:ip
        if (Ret = BPParseAddress(lpPbp,FALSE) ) {
            goto error;
        }

        return (EESTATUS) Ret;
    }

    // get breakaddress, ?, =, /R, /P or /C
    while (*lpPbp->lszCmd) {
        if ( fParseMsg ) {

			LPMESSAGEMAP 	lpMessageMap;
			LSZ 			lsz = lpPbp->lszCmd;
			BOOL 			fFound = FALSE;
			TCHAR *			semi = _ftcschr(lsz, _T(';'));
			TCHAR			tch;
			size_t 			len;
			DWORD 			iMap;

            fParseMsg   = FALSE;

			if (semi != (TCHAR*)0) {
				// Set the end to a zero terminator so we can call stricmp (strnicmp
				// doesn't work since "wm" will match a message starting with WMxxxx
				// which is undesired.
				tch = *semi;
				*( lsz + len ) = _T('\0');
				len = semi - lsz;
			}
			else {
				len = _ftcslen(lsz);
			}

			// Try to get a real message first
			VERIFY (OSDGetMessageMap( hpidCurr, htidCurr, &lpMessageMap ) == xosdNone );
			for (iMap = 0; iMap<lpMessageMap->dwCount; iMap++) {
				if (_ftcsicmp(lsz, lpMessageMap->lpMsgInfo[iMap].lszMsgText) == 0) {
					*((LPDW)&lpPbp->lMessage) = lpMessageMap->lpMsgInfo[iMap].dwMsg;
					fFound = TRUE;
					break;
				}
		  	}

			// Restore the original TCHAR
			if ( semi ) {
				*semi = tch;
			}

			// Try to get class(es) if not a single message
			if ( !fFound ) {
				DWORD	dwMask;
				// There is no way to completely unhook this from the EM
				// Since the IDE is translating msgXXX to the text below,
				// we should translate it back.
			  	lpPbp->dwMask = 0;

				while( len-- ) {
					switch( *lsz ) {
						case 'm':
							dwMask = msgMaskMouse;
							break;

						case 'w':
							dwMask = msgMaskWin;
							break;

						case 'n':
							dwMask = msgMaskInput;
							break;

						case 's':
							dwMask = msgMaskSys;
							break;

						case 'i':
							dwMask = msgMaskInit;
							break;

						case 'c':
							dwMask = msgMaskClip;
							break;

						case 'd':
							dwMask = msgMaskDDE;
							break;

						case 'z':
							dwMask = msgMaskNC;
							break;

						default:
							dwMask = 0;
							break;
					}

					lpPbp->dwMask |= dwMask;

#ifdef _DEBUG
				// Debugging versions:  ensure that the
				// mask is valid in the map!
				if ( dwMask ) {
					DWORD		iMask;
					LPMASKMAP 	lpmm;
					BOOL		fInMap = FALSE;

					VERIFY (
						OSDGetMessageMaskMap(
							hpidCurr,
							htidCurr,
							&lpmm
						) == xosdNone
					);

					for(iMask = 0; !fInMap && iMask<lpmm->dwCount; iMask++) {
						fInMap =
							(lpmm->lpMaskInfo[ iMask ].dwMask == dwMask);
					}

					ASSERT( fInMap );
				}
#endif	// _DEBUG

					lsz = _ftcsinc( lsz );
				}

				if ( lpPbp->dwMask ) {
					fFound = TRUE;
				}
			}
			
			if (!fFound) {

	            // Message and class name lookups failed
	            // Try for a message number
	            lpPbp->lMessage =
	                (DWORD) CVGetNbr (
	                    lpPbp->lszCmd,
	                    radixInput,
	                    fCaseSensitive,
	                    lpPbp->pCXF,
	                    NULL,
	                    &Ret
	            );

	            if ( Ret ) {
	                 goto error;
	            }
			}

            lpPbp->fMessage = TRUE;

        }
        else switch ( *lpPbp->lszCmd ) {

            case '=':

                if ( fWatchPoint || fTracePoint ) {
                    goto bperror;
                }

                fTracePoint = TRUE;
                fRangePossible = TRUE;
                lpPbp->lpDpi = (LPDPI)_fmalloc( sizeof( DPI ) );
				// [cuda#2748 3/29/93 mikemo] If we have a location, use it
				// as our context for evaluating the expression.  This allows
				// setting "break at location if expr changed" where the expr
				// has variables.
				//
				// NOTE, if we do this, the only part of the CXF that we
				// change is the CXT -- we don't change the FRAME.
				if (fAddress && lpPbp->abpt != abptAmbigExpr)
					(void) SHSetCxt(&lpPbp->Addr, &cxf.cxt);
                Ret = BPMkDPI( lpPbp->lszCmd, &cxf, lpPbp->lpDpi, &lpPbp->DPF );
                if ( Ret != (EESTATUS)0 ) {
                    lpPbp->iErr = BPDATAADDR;
                    goto error;
                }

                lpPbp->lpDpi->cData = 1;
                break;

            case '?':
                if ( fWatchPoint || fTracePoint ) {
                    goto bperror;
                }
                fWatchPoint = TRUE;
                lpPbp->lpDpi = (LPDPI)_fmalloc( sizeof( DPI ) );
				// [cuda#2748 3/29/93 mikemo] If we have a location, use it
				// as our context for evaluating the expression.  This allows
				// setting "break at location if expr is true" where the expr
				// has variables.
				//
				// NOTE, if we do this, the only part of the CXF that we
				// change is the CXT -- we don't change the FRAME.
				if (fAddress && lpPbp->abpt != abptAmbigExpr)
					(void) SHSetCxt(&lpPbp->Addr, &cxf.cxt);
                Ret = BPMkDPI( lpPbp->lszCmd, &cxf, lpPbp->lpDpi, &lpPbp->DPF );
                if ( Ret != (EESTATUS)0 ) {
                    lpPbp->iErr = BPDATAADDR;
                    goto error;
                }
                break;

            case '/':

                switch ( * ( ++lpPbp->lszCmd ) ) {

                    case 'A':
                        // the magic ambiguity flag
                        if ( fIgnoreAmbigBPs ) {
                            lpPbp->abpt = abptRestoredAmbig;
                        }
                        lpPbp->lszCmd++;
                        break;

                    case 'R':
                    case 'r':

                        if ( fRange ) {
                            goto bperror;
                        }

                        fRange = TRUE;
                        lpPbp->lszCmd++;
                        cRange = (unsigned short) CVGetNbr (
                        	lpPbp->lszCmd, radixInput, (int) fCaseSensitive,
                            lpPbp->pCXF, NULL, &Ret );
						if ( Ret ) {
							lpPbp->iErr = BPLENGTH;
							goto error;
						}
                        break;

                    case 'P':
                    case 'p':

                        if ( fPassCount ) {
                            goto bperror;
                        }
                        fPassCount = TRUE;

                        lpPbp->lszCmd++;
                        lpPbp->iErr = BPPASSCNT;
                        lpPbp->cPass = (unsigned short) CVGetNbr (
                        	lpPbp->lszCmd, radixInput, (int)fCaseSensitive,
                            lpPbp->pCXF, NULL, &Ret );
                        if ( Ret ) goto error;
                        break;

                    case 'C':
                    case 'c':

                        if ( fCommand ) {
                            goto bperror;
                        }
                        fCommand = TRUE;

                        lpPbp->lszCmd++;
                        if (*(lpPbp->lszCmd) != chQuote) {

                            // an error, command not quoted
                            Ret = BADBKPTCMD;
                            goto error;
                        }
                        lpPbp->lszOptCmd = lpPbp->lszCmd;
                        break;

                    case 'S':
                    case 's':
                        // search mode for addr-expr
                        break;

                    default: {

                        if ( lpprcCurr->fHasMessages ) {

                            fSpecial = TRUE;
                            switch ( * ( lpPbp->lszCmd ) ) {

                                case 'm':
                                case 'M':

                                    if ( lpPbp->lszCmd[1] ) {
                                        if ( !(*(lpchNextTok - 1) ) ) {
                                            *(lpchNextTok - 1) = ' ';
                                        }
                                        lpchNextTok = lpPbp->lszCmd + 1;
                                    }
                                    if ( _istalnum ( *(_TUCHAR *)lpchNextTok ) ) {
                                        fParseMsg    = TRUE;
                                    }
                                    else {
                                        lpPbp->fMessage = TRUE;
                                        lpPbp->lMessage = 0xFFFFFFFF;
                                    }
                                    fMsg = TRUE;
                                    break;

                                case 'd':
                                case 'D':

                                    lpPbp->fNoStop = TRUE;

                                    // If there's a message already,
                                    // don't overwrite!
                                    if ( !lpPbp->fMessage ) {
                                        lpPbp->fMessage = TRUE;
                                        lpPbp->lMessage = 0xFFFFFFFF;
                                    }
                                    break;

                                default:

                                    goto bperror;
                                    break;

                            }
                        }
                        else {
                            // an error
                            goto bperror;
                        }
                    }

                    if ( Ret ) goto error;
                    break;
                }
                break;

            default:

                if ( fAddress ) {
                    goto bperror;
                }
                fAddress = TRUE;

                if (Ret = BPParseAddress(lpPbp,FALSE) ) {
                    goto error;
                }
                break;

        }
        lpPbp->lszCmd = lpchNextTok;
 		if (!lpchNextTok) {
 			// This happens sometimes because the context parser is a little screwy.
 			// For example, try using a file context of '#file2, ,(@.c'
 			// This used to crash, so now at least we're a little more graceful.
 			//
 			// This was DS96:14233
 
 			ASSERT (FALSE);
 			goto bperror;
 		}
        if ( *lpchNextTok != '/' ) {
            BPStripSpace( lpPbp->lszCmd );
        }
        if ( !(*(lpchNextTok - 1) ) ) *(lpchNextTok - 1) = ' ';
        lpchNextTok = CPszToken(lpPbp->lszCmd, "");
    } // end while

    if ( fSpecial && !fAddress ) {

        // add cs:ip address for fSpecial with no address
        if (Ret = BPParseAddress(lpPbp,FALSE) ) {
            goto bperror;
        }
    }

    if ( !( fAddress || fWatchPoint || fTracePoint || fSpecial ) ) {
        goto bperror;
    }

    if( cRange && !fRangePossible ){    //Check to see if use of range is legal
        goto bperror;
    }
    else if ( cRange ) {
        lpPbp->DPF.f.fDataRange = TRUE;
        lpPbp->lpDpi->cData     = cRange;   //Set cData now that we can be sure lpDpi was allocated
    }
    return (EESTATUS) Ret;

bperror:
    Ret = BADBKPTCMD;

error:

    // free any allocated mem added to lpPbp
    if ( lpPbp->lpDpi ) {
        _ffree( (void FAR *)lpPbp->lpDpi );
    }
    return (EESTATUS) Ret;
}

#if defined (_MIPS_) || defined (_ALPHA_)
#if defined (_ALPHA_)
#include <alphaops.h>
#endif
//
// This hack is necessary because NT's RtlUnwind depends on being able to
// reverse execute the prolog on MIPS and Alpha. If the IDE leaves a BP over a
// prolog instruction like the stack adjustment then a call to RtlRaiseStatus
// will probably fail to reach the correct handler
//
// The second param is for updating the length in bytes of any line number
// address that is massaged
//
BOOL MassageBPAddr(LPADDR lpaddr, LPUOFFSET lpcb)
{
	BOOL retval = TRUE;
    CXF Cxf = { 0 };
    DWORD buffer[10];
    DWORD cbRead;
    ADDR addr = *lpaddr;

    if (SHSetCxt( lpaddr, SHpCXTFrompCXF(&Cxf))) {
        if (!ADDR_IS_LI(*SHpADDRFrompCXT (SHpCXTFrompCXF(&Cxf)))) {
            SYUnFixupAddr( SHpADDRFrompCXT(SHpCXTFrompCXF(&Cxf)));
        }
        HSYM hSym;
        UOFF32 Offset = SHGetNearestHsym( SHpADDRFrompCXT(SHpCXTFrompCXF(&Cxf)),
                                          SHHMODFrompCXT(SHpCXTFrompCXF(&Cxf)),
                                          EECODE, &hSym);

        if (Offset != ~0) {
			if (SHIsInProlog(SHpCXTFrompCXF(&Cxf))) {
				UOFFSET uoff = SHGetDebugStart ( SHHPROCFrompCXT( SHpCXTFrompCXF ( &Cxf )));
				if (uoff < GetAddrOff(*SHpADDRFrompCXT( SHpCXTFrompCXF( &Cxf )))) { // Don't slide backwards
					uoff = GetAddrOff(*SHpADDRFrompCXT( SHpCXTFrompCXF( &Cxf )));
				}
				if (lpcb) {
					retval = (uoff < (SHpADDRFrompCXT( SHpCXTFrompCXF( &Cxf ))->addr.off + *lpcb));
					if (retval) {
						*lpcb -= uoff - GetAddrOff(*SHpADDRFrompCXT( SHpCXTFrompCXF(&Cxf)));
					}
				}
				SetAddrOff( &SHpCXTFrompCXF( &Cxf )->addr, uoff);
			}
//                SHpADDRFrompCXT(SHpCXTFrompCXF(&Cxf))->addr.off += sizeof(LONG);
            *lpaddr = *SHpADDRFrompCXT( SHpCXTFrompCXF(&Cxf));
			return(retval);
        }
	}
#if defined (_MIPS_)
#define MIPS_THUNK_JUMP 0x01000008
#define MIPS_ILINK_THUNK 0x03e04025
    if ( ADDR_IS_LI ( addr ) ) {
            SYFixupAddr ( &addr );
    }
    cbRead = DHGetDebuggeeBytes( addr, sizeof(buffer), buffer);
    if ((cbRead < 4) || (cbRead < sizeof(buffer)-4)) {
		return retval;
    } else if ((buffer[2] == MIPS_THUNK_JUMP) || ((cbRead == sizeof(buffer)) && (buffer[3] == MIPS_THUNK_JUMP))) {
		ADDR addr2 = addr;
		DWORD imp_target;
		DWORD target;
		short lo = (short) (buffer[1] & 0xffff);
		imp_target = (buffer[0] & 0xffff) << 16L;
		imp_target += lo;
		SetAddrOff( &addr2, imp_target);
		if (DHGetDebuggeeBytes(addr2, sizeof(target), &target) == 0) {
			return retval;
		} else {
			ADDR addrPC = {0};
			ADDR addrTarget = {0};
			OSDGetAddr ( hpidCurr, htidCurr, adrPC, &addrPC );
			SetAddrSeg ( &addrTarget, (SHSEG) GetAddrSeg ( addrPC ) );
			SetAddrOff( &addrTarget, target);
			ADDR_IS_OFF32( addr2 ) = TRUE;
			ADDR_IS_FLAT( addr2 ) = TRUE;
			SYUnFixupAddr( &addrTarget );
			*lpaddr = addrTarget;
			return MassageBPAddr(lpaddr, 0);
		}
	} else if (buffer[0] == MIPS_ILINK_THUNK) {
		ADDR addr2 = addr;
		DWORD target;
		
		SetAddrOff( &addr2, GetAddrOff(addr2) + 0x20);
		if (DHGetDebuggeeBytes(addr2, sizeof(target), &target) == 0) {
			return retval;
		} else {
			ADDR addrPC = {0};
			ADDR addrTarget = {0};

			target += GetAddrOff(addr) + 0xc + 0x18;
			OSDGetAddr ( hpidCurr, htidCurr, adrPC, &addrPC );
			SetAddrSeg ( &addrTarget, (SHSEG) GetAddrSeg ( addrPC ) );
			SetAddrOff( &addrTarget, target);
			ADDR_IS_OFF32( addr2 ) = TRUE;
			ADDR_IS_FLAT( addr2 ) = TRUE;
			SYUnFixupAddr( &addrTarget );
			*lpaddr = addrTarget;
			return MassageBPAddr(lpaddr, 0);
		}
	}
#elif defined(_ALPHA_)
    // If we get here, it's because we are in either a DLL thunk or an
    // Ilink thunk.

	cbRead = DHGetDebuggeeBytes( addr, sizeof(buffer), buffer);
	PALPHA_INSTRUCTION pInst = (PALPHA_INSTRUCTION)buffer;
	ADDR addr2 = addr;
	DWORD imp_target;
	DWORD target;
    DWORD uoff;

    if (pInst->Jump.Opcode == BR_OP) {
        SYFixupAddr(&addr2);
        imp_target = GetAddrOff(addr2)+4;
		// Branches out of range return negative
		if ((pInst->Branch.BranchDisp < 0) || (pInst->Branch.BranchDisp > 6))
			return retval;
        pInst += (4*(pInst->Branch.BranchDisp));
        pInst++;

        if(pInst->Jump.Opcode == LDA_OP) {
            imp_target += pInst->Memory.MemDisp;
            pInst++;
        } else return retval;
        if(pInst->Jump.Opcode == LDL_OP) {
            uoff = imp_target + pInst->Memory.MemDisp;
            pInst++;
        } else return retval;

        SetAddrOff(&addr2, uoff);
        if(DHGetDebuggeeBytes(addr2, sizeof(target), &target) == 0) {
            return retval;
        }
        imp_target += target;

        if(pInst->Jump.Opcode == ARITH_OP) {
            if(pInst->OpReg.Function != ADDL_FUNC)
                return retval;
            pInst++;
        } else return retval;
        if(pInst->Jump.Opcode != JMP_OP) {
            return retval;
        }

		ADDR addrPC = {0};
		ADDR addrTarget = {0};
    	OSDGetAddr ( hpidCurr, htidCurr, adrPC, &addrPC );
	   	SetAddrSeg ( &addrTarget, (SHSEG) GetAddrSeg ( addrPC ) );
    	SetAddrOff( &addrTarget, imp_target);
    	ADDR_IS_OFF32( addrTarget ) = TRUE;
    	ADDR_IS_FLAT( addrTarget ) = TRUE;
    	SYUnFixupAddr( &addrTarget );
    	*lpaddr = addrTarget;
    	return MassageBPAddr(lpaddr, 0);
    } else if (pInst->Jump.Opcode == LDAH_OP) {
		imp_target = (pInst->Memory.MemDisp << 16);
		pInst++;
	    if (pInst->Jump.Opcode == LDA_OP) {
		    imp_target += pInst->Memory.MemDisp;
	    	pInst++;
    	}

	    if (pInst->Jump.Opcode == LDL_OP) {
		    imp_target += pInst->Memory.MemDisp;
	    	pInst++;
	    } else return retval;

	    if (pInst->Jump.Opcode != JMP_OP) {
		    return retval;
	    }
		ADDR addrData = {0};
	    OSDGetAddr ( hpidCurr, htidCurr, adrData, &addrData );
    	SetAddrOff(&addr2, imp_target);
	    SetAddrSeg ( &addr2, (SHSEG) GetAddrSeg ( addrData ) );

    	if (DHGetDebuggeeBytes(addr2, sizeof(target), &target) == 0) {
	    	return retval;
    	} else {
			ADDR addrPC = {0};
		    ADDR addrTarget = {0};
    		OSDGetAddr ( hpidCurr, htidCurr, adrPC, &addrPC );
	    	SetAddrSeg ( &addrTarget, (SHSEG) GetAddrSeg ( addrPC ) );
    		SetAddrOff( &addrTarget, target);
    		ADDR_IS_OFF32( addr2 ) = TRUE;
    		ADDR_IS_FLAT( addr2 ) = TRUE;
    		SYUnFixupAddr( &addrTarget );
    		*lpaddr = addrTarget;
    		return MassageBPAddr(lpaddr, 0);
    	}
	} else return retval;
#else
#error "MassageBPAddr: Undefined Target Platform"
#endif
	return(retval);
}
#elif defined(_PPC_)
#pragma message(__FILE__ " REVIEW v-ibmms: merge ppcnt; need to get this okayed")
/*
!!! HACK ALERT !!!
	This is a quick fix for V2
	V3 should do the following or some variant
	1) SAPI should return a non-null hproc when sitting at an import thunk. windbg's SH does this
		but dolphin returns null.
	2) x86 and mips backend should mark adjustor thunks as S_THUNK32 rather than S_GPROC32 and S_GPROCMIPS
	3) Then we could return CANSTEP_THUNK to DM and it would handle this nasty pseudo-emulation
	4) Dolphin EE does some thunk manipulation to print out function pointers symbolicly -- Perhaps use
		a thunkhlp.lib to deal with all this in one place
*/

class REGISTERS {
	UOFFSET m_regs[CV_PPC_CTR-CV_PPC_GPR0+1];
	BOOL m_fValid[CV_PPC_CTR-CV_PPC_GPR0+1];

	public:
	REGISTERS() {
		int i;
		memset(m_regs, 0, sizeof(m_regs));
		for (i=0; i<sizeof(m_fValid)/sizeof(m_fValid[0]); i++ ) {
			m_fValid[i] = FALSE;
		}
	}

	UOFFSET& operator[](int i) {
		if (!m_fValid[i]) { // initialize on first access.
			SHREG shreg;
			shreg.hReg = (unsigned short) (i+CV_PPC_GPR0);
			DHGetReg(&shreg, NULL);
			m_regs[i] = shreg.Byte4;
			m_fValid[i] = TRUE;
		}
		return(m_regs[i]);
	}
};
#include <ppcinst.h> // for _PPC_INSTRUCTION
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
 * ThunkTarget
 * Input
 *	hpid,htid
 *	lpThunkAddr: Address of thunk
 * Output
 *	BOOL: True if address was thunk and target was updated
 *  target: Destination of thunk
 * In/Out:
 *  regs: registers used by thunk [updated -- in case the next thunk uses them]
 *
 * Factor out common code for dbcCanStep and dbcExitedFunction.
 *--------------------------------------------------------------------------------
 */
static BOOL ThunkTarget (HPID hpid, HTID htid, LPADDR lpThunkAddr, DWORD& target, REGISTERS& regs)
{
	const int PPC_THUNK_SIZE = 10; // Largest thunk I can think of
	DWORD cbRead;
	PPC_INSTRUCTION buffer[PPC_THUNK_SIZE];
	PPPC_INSTRUCTION pInst = buffer;
	PPPC_INSTRUCTION pInstMax;
	UOFFSET source;
	BOOL isThunk = FALSE;
	source = GetAddrOff (*lpThunkAddr);

	cbRead = DHGetDebuggeeBytes (*lpThunkAddr, sizeof (buffer), buffer);
	if (cbRead < sizeof(buffer)) {
		return(FALSE);
	}
	pInstMax = pInst + (cbRead / sizeof (buffer[0]));

	for (; !isThunk && (pInst < pInstMax); pInst++)
	{
#pragma message(__FILE__ " used Primary_Op instead of i_form.Opcode -  v-ibmmb")		
		switch(pInst->Primary_Op) {
		//switch(pInst->i_form.Opcode) {
		case ADDI_OP:
		{
#pragma message(__FILE__ " going to old structures again -  v-ibmmb")		
//#if DEAD_V4		// v-ibmms code from v4 replaced with above se also dbgosd.cpp
		    if (pInst->Dform_RA != 0) {
				regs[pInst->Dform_RT] = regs[pInst->Dform_RA] + pInst->Dform_SI;
			} else {
				regs[pInst->Dform_RT] = pInst->Dform_SI;
			}
//#else
//			if (pInst->d_form.Ra != 0) {
//				regs[pInst->d_form.Rt] = regs[pInst->d_form.Ra] + pInst->d_form.Simmediate;
//			} else {
//				regs[pInst->d_form.Rt] = pInst->d_form.Simmediate;
//			}
//#endif
		}
		break;

		case LWZ_OP:
		{
#pragma message(__FILE__ " going to old structures again -  v-ibmmb")		
//#if DEAD_V4	// v-ibmms code from v4 replaced with above se also dbgosd.cpp
			if (pInst->Dform_RA == 1) { // SP
				pInst = pInstMax; // leave loop
				break;
			}

			UOFFSET uoff = regs[pInst->Dform_RA];
			uoff += pInst->Dform_D;
			ADDR addr2 = *lpThunkAddr;
			SetAddrOff(&addr2, uoff);
			if (DHGetDebuggeeBytes(addr2, sizeof(regs[0]), &regs[pInst->Dform_RT]) == 0) {
				ASSERT(FALSE);
			}
//#else
//			if (pInst->d_form.Ra == 1) { // SP
//				pInst = pInstMax; // leave loop
//				break;
//			}
//
//			UOFFSET uoff = regs[pInst->d_form.Ra];
//			uoff += pInst->d_form.Simmediate;
//			ADDR addr2 = *lpThunkAddr;
//			SetAddrOff(&addr2, uoff);
//			if (DHGetDebuggeeBytes(addr2, sizeof(regs[0]), &regs[pInst->d_form.Rt]) == 0) {
//				ASSERT(FALSE);
//			}
//#endif
		}
		break;

		case STW_OP:
			break; // ignore

		case B_OP:
			isThunk = TRUE;
#pragma message(__FILE__ " going to old structures again -  v-ibmmb")		
//#if DEAD_V4	// v-ibmms code from v4 replaced with above se also dbgosd.cpp
			target = pInst->Iform_LI << 2;
			if (pInst->Bform_AA) {
//#else
//			//?? INST_FIELD( *pInst, 6, 29) ??
//			target = pInst->i_form.Target << 2;
//			//if (INST_IS_BA(pInst)) { // is this the correct use? see ppcinst.h
//			if (pInst->b_form.Absolute) {
//#endif
				target += source + (pInst-buffer+1) * sizeof(_PPC_INSTRUCTION);
			}
			pInst = pInstMax; // leave loop
			break;

		case X19_OP:
#pragma message(__FILE__ " going to old structures again -  v-ibmmb")		
//#if DEAD_V4	// v-ibmms code from v4 replaced with above se also dbgosd.cpp
			switch (pInst->XLform_XO) {
			case BCCTR_OP:
				switch(pInst->XLform_BO) {
//#else
//			switch (pInst->xl_form.ExtendedOpcode) {
//			case BCCTR_OP:
//				switch(pInst->xl_form.Bt) {
//#endif
				case 20: // Branch always
					isThunk = TRUE;
					target = regs[CV_PPC_CTR-CV_PPC_GPR0];
				        break;
				}
			}
			pInst = pInstMax; // leave loop
			break;

		case X31_OP:
#pragma message(__FILE__ " going to old structures again -  v-ibmmb")		
//#if DEAD_V4	// v-ibmms code from v4 replaced with above se also dbgosd.cpp
			switch (pInst->XFXform_XO) {
				case MTSPR_OP:
					regs[CV_PPC_CTR-CV_PPC_GPR0] = regs[pInst->XFXform_RS];
//#else
//			switch (pInst->xfx_form.ExtendedOpcode) {
//				case MTSPR_OP:
//					regs[CV_PPC_CTR-CV_PPC_GPR0] = regs[pInst->xfx_form.Rd];
//#endif
					break;
			}
			break;

		default:
			pInst = pInstMax; // leave loop
			break;
		}
	}
	return(isThunk);
}

//
// This hack is necessary because NT's RtlUnwind depends on being able to
// reverse execute the prolog on PPC. If the IDE leaves a BP over a prolog
// instruction like the stack adjustment then a call to RtlRaiseStatus will
// probably fail to reach the correct handler
//
// The second param is for updating the length in bytes of any line number
// address that is massaged
//
BOOL MassageBPAddr(LPADDR lpaddr, LPUOFFSET lpcb)
{
	BOOL retval = TRUE;
    CXF Cxf = { 0 };
	UOFFSET uoff = 0; // for Procedure descriptor?
	REGISTERS regs;

	enum state { unknown, code_area, possible_thunk, procedure_descriptor, ok } theState = unknown;

	while ( theState != ok ) {
		switch ( theState ) {
		case unknown:
		case code_area:
			if (SHSetCxt( lpaddr, SHpCXTFrompCXF(&Cxf))) {
				if (!ADDR_IS_LI(*SHpADDRFrompCXT (SHpCXTFrompCXF(&Cxf)))) {
					SYUnFixupAddr( SHpADDRFrompCXT(SHpCXTFrompCXF(&Cxf)));
				}
				HSYM hSym;
				UOFF32 Offset = SHGetNearestHsym( SHpADDRFrompCXT(SHpCXTFrompCXF(&Cxf)),
												  SHHMODFrompCXT(SHpCXTFrompCXF(&Cxf)),
												  EECODE, &hSym);

				if (Offset != ~0) {
					if (SHIsInProlog(SHpCXTFrompCXF(&Cxf))) {
						UOFFSET uoff = SHGetDebugStart ( SHHPROCFrompCXT( SHpCXTFrompCXF ( &Cxf )));
						if (uoff < GetAddrOff(*SHpADDRFrompCXT( SHpCXTFrompCXF( &Cxf )))) { // Don't slide backwards
							uoff = GetAddrOff(*SHpADDRFrompCXT( SHpCXTFrompCXF( &Cxf )));
						}
						if (lpcb) {
							retval = (uoff < (SHpADDRFrompCXT( SHpCXTFrompCXF( &Cxf ))->addr.off + *lpcb));
							if (retval) {
								*lpcb -= uoff - GetAddrOff(*SHpADDRFrompCXT( SHpCXTFrompCXF(&Cxf)));
							}
						}
						SetAddrOff( &SHpCXTFrompCXF( &Cxf )->addr, uoff);
					}
					*lpaddr = *SHpADDRFrompCXT( SHpCXTFrompCXF(&Cxf));
					theState = ok;
				} else if (theState == unknown) {
					theState = procedure_descriptor;
				} else {
					theState = ok;
				}
			} else {
				theState = ok;
			}
			break;

		case possible_thunk:
			theState = code_area;
			SYFixupAddr( lpaddr );
			while (ThunkTarget(hpidCurr, htidCurr, lpaddr, uoff, regs)) {
				SetAddrOff(lpaddr, uoff);
			}
			emiAddr( *lpaddr ) = 0; // REVIEW: Why doesn't UnFixup do this?
			SYUnFixupAddr( lpaddr );
			break;

		case procedure_descriptor:
			{
				theState = ok;
				ADDR saveAddr = *lpaddr;
				SetAddrOff(&saveAddr, GetAddrOff(saveAddr)+4);
				if (DHGetDebuggeeBytes(saveAddr, sizeof(regs[0]), &regs[CV_PPC_GPR2-CV_PPC_GPR0])) {
					if (DHGetDebuggeeBytes(*lpaddr, sizeof(uoff), &uoff) && (uoff != 0)) {
						SYFixupAddr( lpaddr );
						SetAddrOff(lpaddr, uoff);
						SYUnFixupAddr( lpaddr );
						theState = possible_thunk;
					}
				}
			}
			break;
		}
	}
	return(retval);
}
#endif	// _M_PPC

/*** BPParseAddress
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
EESTATUS PASCAL BPParseAddress(LPPBP lpPbp, BOOL fUnAmbigOnly) {

    TML             tml;
    PHTM            TMList;
    size_t          index;
    char            szBP[axMax + 6];	// give ourselves 260 bytes.
    char *          pchBP = szBP;
    char            szCxt[axMax]    = {'\0'};
    char FAR *      lszCxt          = szCxt;
    EESTATUS        Ret             = EENOERROR;
    ushort          strIndex;
    int				ichCmd;

    _ftcsncpy( (char FAR *)szBP, lpPbp->lszCmd, sizeof(szBP) );
	szBP[ sizeof(szBP) - 1 ] = '\0';

    // we will hardcode chQuote as our quoting character for now
    if ( *pchBP == chQuote ) {

        // find the index of the last character
        *pchBP = ' ';
		ASSERT (_ftcsrchr ( pchBP, _TCHAR('\"')));
        ichCmd = _ftcsrchr ( pchBP, _TCHAR('\"') ) - pchBP;
        *( pchBP + ichCmd ) = '\0';

        if ( ( pchBP = CPTrim ( pchBP, ' ' ) ) == NULL ) {

            // ERROR - unbalanced quoteing characters
            lpPbp->iErr = BPCODEADDR;
            return BADBKPTCMD;
        }
        //pchBP = CPAdvance(pchBP, "");
        if ( *(pchBP) == '\0') {

            // ERROR - reached end of cmd
            lpPbp->iErr = BPCODEADDR;
            return BADBKPTCMD;
        }
    }
    else {
        ichCmd = _ftcslen( szBP ) + 1;
    }

    if ( *(pchBP) == chBeginCxtOp) {

        // consume the context operator
		LPSTR pCloseCurly;
		pCloseCurly = (LPSTR)FindNonQuotedChar(_ftcsinc(pchBP), '}');
		if (pCloseCurly == NULL || *(_ftcsinc(pCloseCurly)) == '\0')
		{
            // ERROR - reached end of cmd
            lpPbp->iErr = BPCODEADDR;
            return BADBKPTCMD;
        }
        else
		{
			// skip the close curly and the next char
			pCloseCurly = _ftcsinc(pCloseCurly);
			pCloseCurly = _ftcsinc(pCloseCurly);
			index = pCloseCurly - pchBP;
			_fmemcpy(lszCxt, pchBP, index);
			lszCxt[index] = '\0';
        }

        // advance beyond the context
        pchBP += index;
 		// If we did a CPAdvance here, we would accept invalid BP's, like ,{}()MyFunc
        // pchBP = CPAdvance( pchBP, "");
        if ( *pchBP == '\0') {

            // ERROR - reached end of cmd
            lpPbp->iErr = BPCODEADDR;
            return BADBKPTCMD;
        }
    }
    if ( (*pchBP == '.') ||
         (*pchBP == '@' && _istdigit ( (_TUCHAR)*((pchBP)+1) ) ) ||
         (*pchBP == '\0') ) {

        char FAR *  lpch;
        unsigned long       ulT;
        unsigned short      LNbr;

        // get beyond the . (if there is one)
        if ( *pchBP != '\0') {
            pchBP++;
        }

 		// If we did a CPAdvance here, we would accept invalid BP's, like ,{}()MyFunc
        // pchBP = CPAdvance( pchBP, "");
        if ( *pchBP == '\0') {

            // this is a bp on the current CS:IP
            lpPbp->Addr = cxfIp.cxt.addr;
            lpPbp->lszCmd += _ftcslen( szBP );
            return Ret;
        }

        // this had better be a number between 0 and 65535
        // is the expression a number?
        lpch = pchBP;
        do {
			if ( !_istdigit( (_TUCHAR)*lpch++ ) ) {
                lpPbp->iErr = BPCODEADDR;
                return BADBKPTCMD;
            }
        } while( CPQueryChar ( lpch, "" ) != CPISDELIM );

        // if szCmd is longer than 5 digits or > 65535 or == 0, throw it away
        ulT = (unsigned long)atol( (char *)FP_OFF( pchBP ) );
        LNbr = (unsigned short)ulT;
        if ( _ftcsspn( pchBP, "0123456789") > 5 || ulT > 65535 || LNbr == 0 ) {
            lpPbp->iErr = BPCODEADDR;
            return BADBKPTCMD;
        }

		if ( *lszCxt ) {
			BPCONTEXT	bpcxt = {0};

			if ( ExtractCV400Context( szCxt, &bpcxt ) ) {
				int		cslp;
				LPSLP	rgslp;

				cslp = SLCAddrFromLine(
					SHGethExeFromName( bpcxt.Exe ),
					(HMOD)NULL,
					bpcxt.Source,
					(int)ulT,
					&rgslp
				);
				if ( cslp ) {
					LPSLP	lpslp;
					int		islp;

					for( islp = 0, lpslp = rgslp; islp < cslp; ++islp, ++lpslp ) {
						CXT		cxt = {0};
						char	sz[ 1024 ];
						HSYM	hsym;

#if defined (_MIPS_) || defined (_ALPHA_) || defined(_PPC_)
#if defined(_PPC_)
#pragma message(__FILE__ " REVIEW v-ibmms: need code review")
#endif
						MassageBPAddr(&lpslp->addr, &lpslp->cb);
#endif
						// Get the bounding context
						SHSetCxt( &lpslp->addr, &cxt );

						// "Function"
						SHGetNearestHsym(
							&lpslp->addr,
							SHHMODFrompCXT( &cxt ),
							EECODE,
							&hsym
						);

						// Get a name from the hsym plus an arg list
						FnNameFromHsym( hsym, &cxt, (LPSTR)sz );

						// If there was no Function specified or it matches the address
						// being processed, get the address and get out
						if ( !*bpcxt.Function ||
							!BPCmpFunctionNames( bpcxt.Function, sz )
						) {
							lpPbp->Addr = lpslp->addr;
							break;
						}
					}

					if ( cslp == islp ) {
						SHFree( (LPV)rgslp );
						lpPbp->iErr = BPCODEADDR;
						return BADBKPTCMD;
					}
				}
				else {
					lpPbp->iErr = BPCODEADDR;
					return NOCODE;
				}

				if ( rgslp ) {
					SHFree( (LPV)rgslp );
				}
			}
			else {
				lpPbp->iErr = BPCODEADDR;
				return BADBKPTCMD;
			}
		}
		else {
			lpPbp->iErr = BPCODEADDR;
			return NOSOURCEHERE;
		}

        // skip past the line number
        index = _ftcsspn( pchBP, " 0123456789");
        if ( *(pchBP + index) == '\0') {

            // FINISHED - this is a bp on a line number
            lpPbp->lszCmd += _ftcslen( szBP );
            return Ret;
        }
        pchBP += index + 1;
    }
    else {
 		//
 		// We're looking at an expression, not a line number.
 		//
 
        // we may add "{,,foo.exe}" to the parse if we can't find it,
		// [cuda:5565, dans, 1-July-1993]
        char rgch [ MAXINPLINE + 1 + _MAX_PATH + 1 + 2 + 1 ];

        /*
        ** If the user gave us a paramter list to potentially disambiguate
        ** then set abpt to be abptAmbigUser.  This way when we list
        ** breakpoints we will show the parameter list since he/she gave us one
        */
        _ftcsncpy ( rgch, pchBP, ichCmd );
        if ( _ftcschr ( rgch, _T('(') ) ) {
            lpPbp->abpt = abptAmbigUser;
        }

        // an expression of some sort.  give it to the EE and let it sort
        //   things out

        // use lszCxt as buffer to pass to the EE
        _ftcscpy ( rgch, lszCxt );
        _ftcsncat( rgch, pchBP, ichCmd );

        Ret = EEParseBP (
            rgch,
            radixInput,
            fCaseSensitive,
            lpPbp->pCXF,
            &tml,
            0L,
            &strIndex,
            FALSE
        );

		// For Java, there's no reason to search all the other modules (classes)
		// because the original expression will specify enough information to 
		// successfully parse the BP or not.
		if (!pDebugCurr || (pDebugCurr && !pDebugCurr->IsJava()))
		{
			if ( Ret == EEGENERAL ) {
				HEXE hexe = (HEXE) NULL;
				int  ichExe;

				// We couldn't find the symbol in the current context
				//  and the user didn't specify a context so we'll
				//  try all the other dlls

				// first, we save away the error msg
				TMList = (PHTM) BMLock (tml.hTMList);
				CVExprErr ( Ret, MSGGERRSTR, &TMList[0], NULL);
	//          Ret = GEXPRERR;
				BMUnlock (tml.hTMList);
				EEFreeTML(&tml);

				rgch [ 0 ] = chBeginCxtOp; // chEndCxtOp
				rgch [ 1 ] = ',';
				rgch [ 2 ] = ',';

				while (
					( hexe = SHGetNextExe ( hexe ) ) != 0 &&
					Ret == EEGENERAL
				) {

					_ftcscpy ( &rgch [ 3 ], SHGetExeName ( hexe ) );


					ichExe = _ftcslen ( rgch );
					// chBeginCxtOp
					rgch [ ichExe     ] = chEndCxtOp;
					rgch [ ichExe + 1 ] = '\0';
					_ftcsncat ( &rgch [ ichExe + 1 ], pchBP, ichCmd );

					Ret = EEParseBP (
						rgch,
						radixInput,
						fCaseSensitive,
						lpPbp->pCXF,
						&tml,
						0L,
						&strIndex,
						FALSE
					);

					// If we failed, just delete the TM list
					if ( Ret != EENOERROR ) {
						EEFreeTML(&tml);
					}
				}
			}

            // if we could not find it in any of the dlls/exes then we are
            // done
            if ( Ret != EENOERROR ) {
                Ret = GEXPRERR;
                lpPbp->iErr = BPCODEADDR;
                return Ret;
            }
        }

        if ( !Ret ) {

            // if we have more then one TM in the TML, then find the ones
            //  we want to keep. Call the user dialog to determine this
            TMList = (PHTM) BMLock (tml.hTMList);

            if (tml.cTMListAct > 1) {

                if ( fIgnoreAmbigBPs ) {
                    BMUnlock (tml.hTMList);
                    EEFreeTML(&tml);

                    // return any error
                    return NOROOM;
                }

                // ambiguous case
                tml.cTMListAct = BPResolveDlg(TMList,tml.cTMListMax, TRUE);
                if ( tml.cTMListAct == 0 ) {
                    BMUnlock( tml.hTMList );
                    EEFreeTML( &tml );
                    return NOADDRGIVEN;
                }
                else if ( ( lpPbp->BPType == BPTMP ) &&
                    !lpPbp->fInsert &&
                    ( tml.cTMListAct > 1 )
                ) {
                    BMUnlock( tml.hTMList );
                    EEFreeTML( &tml );
                    return TOOMANYADDR;
                }
                else {
                    lpPbp->cBPMax = tml.cTMListMax;
                    lpPbp->tml = tml;
                    lpPbp->abpt = abptAmbigExpr;

                    if ( fUnAmbigOnly ||
                        ( lpPbp->BPType == BPTMP && !lpPbp->fInsert ) ) {

                        unsigned int    i = 0;

                        while ( TMList[i] == 0 && i < tml.cTMListMax ) {
                            i++;
                        }
                        ASSERT ( i != tml.cTMListMax );

                        if( !(Ret = EEvaluateTM(&TMList[i], SHpFrameFrompCXF(lpPbp->pCXF),
                            EEBPADDRESS)) &&
                            !(Ret = BPADDRFromTM(&TMList[i], &lpPbp->BPSegType,
                            &lpPbp->Addr))) {
                        }
                    }
                }
            }
            else {
				
    			// unambiguous case
				USHORT usSegType = lpPbp->BPSegType;
			    if( !(Ret = EEvaluateTM(&TMList[0], SHpFrameFrompCXF(lpPbp->pCXF),
					EEBPADDRESS)) &&
                    !(Ret = BPADDRFromTM(&TMList[0], &lpPbp->BPSegType,
                    &lpPbp->Addr))) {
					if ( (usSegType & EEANYSEG) == EECODE )
					{
						ETI eti;
						EEGetExtendedTypeInfo(&TMList[0], &eti);
						if (eti == ETIPOINTER)
						{	// Prevent setting location bps on code and data ptr variables.
							// To do this we
							// a) Check if the type is a pointer.
							// b) If it is a pointer and not a label, we don't
							//    allow bps to be set there.
							HTI		hti;
							RTMI	rtmi = {0};

							rtmi.fLabel = TRUE;
							if (!EEInfoFromTM(&TMList[0], &rtmi, &hti)) {
								ASSERT(hti != (HTI)NULL);
								PTI pTI = (PTI)BMLock(hti);

								if ( pTI->fResponse.fLabel && !pTI->fLabel )
									Ret = EEGENERAL;

								BMUnlock(hti);
								EEFreeTI(&hti);
							}
						}
					}
                }
            }

            // unlock the list
            BMUnlock (tml.hTMList);
        }

        // return error if there was one
        if ( Ret != EENOERROR ) {
            if ( Ret != NOROOM ) {
                TMList = (PHTM) BMLock (tml.hTMList);
                CVExprErr ( Ret, MSGGERRSTR, &TMList[0], NULL);
                Ret = GEXPRERR;
                BMUnlock (tml.hTMList);
                EEFreeTML(&tml);
                lpPbp->iErr = BPCODEADDR;

            }
            else if (Ret != EENOERROR) {
                EEFreeTML(&tml);
                lpPbp->iErr = BPCODEADDR;
            }
        }
    }

    return Ret;
}

/*** BPCommitBP
*
* Purpose: To commit a BP to our list of BP's
*
* Input:
*   lpPbp   - a pointer to a PBP
*
* Output:
*  Returns a lpbpi or NULL if error
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
HBPI PASCAL BPCommitBP ( LPPBP lpPbp) {

    HBPI            hbpi = (HBPI)NULL;
    PHTM            TMList;
    ADDR            Addr;
    unsigned int    i;

    if ( lpPbp->abpt == abptAmbigExpr ) {

        TMList = (PHTM) BMLock (lpPbp->tml.hTMList);

        for ( i=0; i<lpPbp->cBPMax; i++ ) {
            if ( TMList [ i ] ) {
                if (
                    !EEvaluateTM (
                        &TMList[i],
                        SHpFrameFrompCXF(lpPbp->pCXF),
                        EEBPADDRESS
                    ) &&
                    !BPADDRFromTM (
                        &TMList[i],
                        &lpPbp->BPSegType,
                        &Addr
                    )
                ) {
                    if ( !(hbpi = BPXAdd (
                        lpPbp->pCXF,
                        lpPbp->hthd,
                        lpPbp->BPType,
                        &Addr,
                        lpPbp->abpt,
                        lpPbp->lpDpi,
                        &lpPbp->DPF,
                        lpPbp->fNoStop,
                        lpPbp->fMessage,
                        lpPbp->lMessage,
                        lpPbp->dwMask,
                        lpPbp->cPass,
                        lpPbp->lszOptCmd,
                        lpPbp->fInsert
                    ) ) ) {
                        BMUnlock (lpPbp->tml.hTMList);
                        EEFreeTML(&lpPbp->tml);
                        errno = NOROOM;
                        return hbpiNull;
                    }
                }
                else {
                    BMUnlock (lpPbp->tml.hTMList);
                    EEFreeTML(&lpPbp->tml);
					if ( errno == CV_NOERROR ) {
                        errno = NOROOM;
                    }
                    return (HBPI)NULL;
                }
            }
        }
        BMUnlock (lpPbp->tml.hTMList);
        EEFreeTML(&lpPbp->tml);
    }
    else {

        if ( !(hbpi = BPXAdd (
            lpPbp->pCXF,
            lpPbp->hthd,
            lpPbp->BPType,
            &lpPbp->Addr,
            lpPbp->abpt,
            lpPbp->lpDpi,
            &lpPbp->DPF,
            lpPbp->fNoStop,
            lpPbp->fMessage,
            lpPbp->lMessage,
            lpPbp->dwMask,
            lpPbp->cPass,
            lpPbp->lszOptCmd,
            lpPbp->fInsert
        ) ) ) {

			if ( errno == CV_NOERROR ) {
                errno = NOROOM;
            }
            return hbpiNull;
        }
    }
    return hbpi;
}

/*** BPADDRFromTM
*
* Purpose: To get the address of an item represented by a TM
*
* Input:
*   phTM    - A pointer to a handle to a TM that represents
*             the item to get the address of
*
*   pwSegType - This indicates that the TM must be a member of this
*               segment type (Code, Data, Const).
*
*   pAddr   - The place to put the address when found
*
* Output:
*  Returns An Expr Eval Error, or EECATASTROPHIC if nothing was found
*
*          Modify pwSegType to reflect InfoFromTM response.
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
EESTATUS PASCAL BPADDRFromTM( PHTM phTM, WORD FAR * pwSegType, PADDR paddr ) {
    EESTATUS	Err = EENOERROR;
    RTMI		rtmi = {0};
    HTI			hTI;
    PTI			pTI;

    _fmemset(paddr, 0, sizeof(ADDR));
    rtmi.fAddr       = TRUE;
    rtmi.fSegType    = TRUE;
    rtmi.fSzBytes    = TRUE;

    // get the information
    if( !(Err = EEInfoFromTM(phTM, &rtmi, &hTI)) ) {

        // get the TI
        ASSERT(hTI != (HTI)NULL);
        pTI = (PTI)BMLock (hTI);

        // if an address was given.
        if( pTI->fResponse.fAddr && (pTI->SegType & *pwSegType) ) {
            *paddr = pTI->AI;
             SYUnFixupAddr ( paddr );
            *pwSegType = pTI->SegType;
        }
        else if(!pTI->fResponse.fAddr &&
            pTI->fResponse.fValue &&
            pTI->fResponse.fSzBytes &&
            pTI->cbValue >= sizeof(WORD) ) {

            // if a value was given
            // save the offset
            switch ( pTI->cbValue ) {

                case sizeof(BYTE):

                    SetAddrOff ( paddr , *((CHAR FAR *) pTI->Value) );
                    break;

                case sizeof(WORD):

                    SetAddrOff ( paddr , *((WORD FAR *) pTI->Value) );
                    break;

                default:
                case sizeof(DWORD):

                    // sizeof(SHOFF) == sizeof(DWORD) if ADDR_MIXED is defined
                    SetAddrOff ( paddr , *((SHOFF FAR *) pTI->Value) );
                    break;
                case sizeof(DWORD) + sizeof(WORD):
                    // 32-bit seg:offset addr.
                    SetAddrOff( paddr, *((SHOFF FAR *) pTI->Value) );
                    SetAddrSeg( paddr, *((SEGMENT *)((SHOFF FAR *)pTI->Value + 1)));
            }
#ifdef MAC
                    paddr->mode.flat32 = TRUE;
#endif

            // set the segment
            if( (*pwSegType & EEDATA) == EEDATA ) {
                ADDR addrData = {0};

                OSDGetAddr ( hpidCurr, htidCurr, adrData, &addrData );
                if (GetAddrSeg(*paddr) == 0 )
                    SetAddrSeg ( paddr, (SHSEG) GetAddrSeg ( addrData ) );
                SYUnFixupAddr ( paddr );
                *pwSegType &= EEDATA;
            }

            else if( (*pwSegType & EECODE) == EECODE ) {
                ADDR addrPC = {0};

                OSDGetAddr ( hpidCurr, htidCurr, adrPC, &addrPC );
                if (GetAddrSeg(*paddr) == 0 )
                    SetAddrSeg ( paddr, (SHSEG) GetAddrSeg ( addrPC ) );
                SYUnFixupAddr ( paddr );
                *pwSegType &= EECODE;
            }

            // assume bad address expression
            else {
                Err = EEBADADDR;
            }
        }
        else {
            Err = EEBADADDR;
        }

        // remove the TI
        BMUnlock (hTI);
        EEFreeTI(&hTI);
    }

    // thou shalt never return a physical address from this function!

    ASSERT ( Err || ADDR_IS_LI ( *paddr ));

#if defined (_MIPS_) || defined (_ALPHA_) || defined(_PPC_)
#if defined(_PPC_)
#pragma message(__FILE__ " REVIEW v-ibmms: need code review")
#endif
// Possible optimization: Check for EECODE
	if (!Err) {
		MassageBPAddr(paddr, 0);
	}
#endif

    return(Err);
}


/*** BPMkDPI
*
* Purpose:
*
* Input:
*
* Output:
*  Returns
*
* Exception
*
* Notes:
*
*
*************************************************************************/
EESTATUS PASCAL BPMkDPI( char FAR * lszCmd, PCXF pCXF, LPDPI lpDpi, LPDPF pDPF ) {

    EESTATUS    Err = EENOERROR;
    RTMI        rtmi;
    HTI         hTI;
    PTI         pTI;
    HCXTL       hCXTL;
    PCXTL       pCXTL;
    char *      szTmp;
    int         fTrace = 0;
    ushort      strIndex;

    // I need to clear out the DPI if there is one
    if( lpDpi ) {
        BOOL fContext = FALSE;
        BOOL fUserCxt = FALSE;

        // clean this guy out
        _fmemset(lpDpi, 0, sizeof(DPI));

        // if there is a problem get out
        if( lszCmd || pCXF  ||  (*lszCmd == '=' &&  *lszCmd == '?') ) {

            BPStripSpace( lszCmd );

            // get over the ? or =
            if (*lszCmd == '=') fTrace++;
            lszCmd++;

            // handle quoting (had code '"' as quote string for now
            lszCmd = CPAdvance(lszCmd, "");
            if ( *lszCmd == chQuote) {
                if ( (lszCmd = CPTrim(lszCmd, ' ') ) == NULL) {
                    return BADBKPTCMD;
                }
            }

            fUserCxt = (*lszCmd == chBeginCxtOp);

            // get the TM for this guy
            memset( &rtmi, 0, sizeof(rtmi) );
            rtmi.fAddr    = TRUE;
            rtmi.fSzBytes = TRUE;
            rtmi.fLvalue  = TRUE;
            rtmi.fSegType = TRUE;

            is_assign = FALSE;

            if(!(Err = EEParse(lszCmd, radixInput, fCaseSensitive, &lpDpi->hTM, &strIndex))) {
                if(Err = EEBindTM(&lpDpi->hTM, SHpCXTFrompCXF(pCXF), TRUE, FALSE)) {
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
                            CXF cxf;

                            iLen = _ftcslen (szTmp);
                            szTmp[iLen] = '0';
                            szTmp[iLen+1] = (char)NULL;

                            // save the pcxf before munging it.
                            cxf = *pCXF;
                            if (!(Err = BPPastProlog (szTmp, pCXF))) {
                                EEFreeTM (&lpDpi->hTM);

                                if (Err = CVParseBind (&lpDpi->hTM,
                                                       SHpCXTFrompCXF(pCXF),
                                                       radixInput,
                                                       fCaseSensitive,
                                                       TRUE,
                                                       (lszCmd + iLen))) {
                                    CVExprErr (Err, MSGGERRSTR, &lpDpi->hTM, NULL);
                                    Err = GEXPRERR;
                                }

                                fContext = TRUE;
                            }

                            // restore the old one.
                            *pCXF = cxf;
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

                    if ( !fTrace ) {
                        SetupForWatch ( );
                    }

                    EEvaluateTM (
                        &lpDpi->hTM,
                        SHpFrameFrompCXF ( pCXF ),
                        EEHORIZONTAL
                    );

                    if (!(Err = EEInfoFromTM(&lpDpi->hTM, &rtmi, &hTI)) &&
                        !(Err = EEGetCXTLFromTM(&lpDpi->hTM, &hCXTL))) {

                        if ( !fTrace ) {
                            lpDpi->lpbps = EndWatch ( );
                        }

                        // get the TI
                        pTI = (PTI)BMLock (hTI);
                        pCXTL   = (PCXTL)BMLock (hCXTL);

                        ASSERT( pCXTL->cHCS );
                        if ( fUserCxt && pCXTL->cHCS == 1 ) {
                            fUserCxt = 0;
                        }

						// Can't set a watchpoint or tracepoint on <void>
						if (pTI->cbValue == 0) {
							Err = BADBKPTCMD;
						}
						else {
	                        // If this is a break on change
	                        if( fTrace ) {
	                            Err = BPTracePoint (
	                                lpDpi,
	                                pDPF,
	                                pTI,
	                                pCXTL,
	                                pCXF,
	                                fUserCxt,
	                                fContext
	                            );
	                        }
	                        // if this is a break on TRUE
	                        else {

	                            extern BOOL fTouched;

	                            Err = BPWatchPoint (
	                                lpDpi,
	                                pDPF,
	                                pTI,
	                                pCXTL,
	                                pCXF,
	                                fUserCxt,
	                                fContext
	                            );

	                            if (  ( !lpDpi->lpbps && !pDPF->f.fContext ) ||
                                    fTouched ||
                                    is_assign
                                ) {
	                                Err = BADBKPTCMD;
	                            }
	                        }
						}
                        BMUnlock (hCXTL);
                        BMUnlock (hTI);
                        EEFreeCXTL(&hCXTL);
                        EEFreeTI(&hTI);
                    }
                    else {
                        if ( !fTrace ) {
                            lpDpi->lpbps = EndWatch ( );
                        }
                        CVExprErr (Err, MSGGERRSTR, &lpDpi->hTM, NULL);
                        Err = GEXPRERR;
                    }
                }
            }
            else {
                CVExprErr (Err, MSGGERRSTR, &lpDpi->hTM, NULL);
                Err = GEXPRERR;
            }
        }
        else {
            Err = BADBKPTCMD;
        }
    }
    else {
        Err = BADBKPTCMD;
    }

    if (Err) {
        EEFreeTM (&lpDpi->hTM);
    }

    return(Err);
}


HBPI PASCAL BPXAdd (
    PCXF      pCXF,
    HTHD      hthd,
    ushort    BPType,
    PADDR     paddr,
    ABPT      abpt,
    LPDPI     lpDpi,
    LPDPF     pDPF,
    BOOL      fNoStop,
    BOOL      fMessage,
    DWORD     lMessage,
    DWORD      dwMask,
    ushort    cPass,
    char FAR *lszCmd,
    BOOL      fInsert
) {
    LPBPI   lpbpi;
    HBPI    hbpi;
	ADDR	addr = *paddr;

	// We only accept un-fixed up addresses
#if defined (_MIPS_) || defined (_ALPHA_) || defined(_PPC_)
#if defined(_PPC_)
#pragma message(__FILE__ " REVIEW v-ibmms: need code review")
#endif
    BOOL    fixAddr = FALSE;
    if (GetAddrSeg(addr) == 0 && GetAddrOff(addr) == 0) {
       fixAddr = TRUE;
    }
#endif
	if (!SYUnFixupAddr(&addr))
		ASSERT(FALSE);
#if defined (_MIPS_) || defined (_ALPHA_) || defined(_PPC_)
#if defined(_PPC_)
#pragma message(__FILE__ " REVIEW v-ibmms: need code review")
#endif
   if (fixAddr && GetAddrOff(addr) == 0 && GetAddrSeg(addr) == 1) {
      GetAddrSeg(addr) = 0;
   }
#endif
	paddr = 0;	// precaution to make sure the following code only uses 'addr'

    // if a linked list for the breakpoints has not been initialized do it now.
    if ( !llbpi ) {
        llbpi = LLHlliInit( sizeof(BPI), llfNull, NULL, NULL );
    }

    // allocate the space for the bpi
    if ( (hbpi = LLHlleCreate(llbpi)) &&
         (!lpDpi||pDPF->f.fEvalExpr ||
         (lpDpi->pValue = (char *)malloc(lpDpi->cData * lpDpi->cbData))) ) {

        // put this breakpoint in the list
        // place him in at the end of the list

        if ( fInsert ) {
            int  ibpi  = 1;
            HBPI hbpiT = (HBPI) NULL;

            while ( hbpiT = LLHlleFindNext ( llbpi, hbpiT ) ) {
                LPBPI lpbpiT = (LPBPI)LLLpvFromHlle ( hbpiT );

                if ( lpbpiT->bpf.f.fUser ) {
                    lpbpiT->bpf.f.fUser = FALSE;
                    UnlockHlle ( hbpiT );
                    break;
                }
                else {
                    ibpi += 1;
                }
                UnlockHlle ( hbpiT );
            }

            if ( hbpiT ) {
                LLInsertHlleInLl ( llbpi, hbpi, ibpi + 1 );
            }
            else {
                LLAddHlleToLl ( llbpi, hbpi );
            }
        }
        else if ( BPType == BPTMP ) {
            LLHlleAddToHeadOfLI ( llbpi, hbpi );
        }
        else {
            LLAddHlleToLl ( llbpi, hbpi );
        }


        lpbpi = (LPBPI)LLLpvFromHlle(hbpi);

        // fill in common stuff to code and data BPs

        lpbpi->bpf.f.BpType   = BPType;
        lpbpi->hprc = hprcCurr;
        lpbpi->hthd = hthd;
        lpbpi->fLang    = ESilan();
        lpbpi->bpf.f.fActive = TRUE;

        if ( abpt == abptRestoredAmbig || abpt == abptAmbigExpr ) {
            lpbpi->bpf.f.fAmbig = TRUE;
        }
        else {
            lpbpi->bpf.f.fAmbig = FALSE;
        }

        if ( abpt == abptAmbigUser ) {
            lpbpi->bpf.f.fParamGiven = TRUE;
        }
        else {
            lpbpi->bpf.f.fParamGiven = FALSE;
        }


        // put in any optional commands
        if ( lszCmd ) {
            char FAR *  lszCmdEnd = NULL;
            char        ch;

            // take any leading white space off
            while ( _istspace( (_TUCHAR)*lszCmd ) ) {
                lszCmd++;
            }

            if ( *lszCmd == '"' ) {

                // take any leading white space off
				while( _istspace( (_TUCHAR)*++lszCmd ) );

                // search backward for the closing "
                if ( !( lszCmdEnd = _ftcschr(lszCmd, _T('"')) ) ) {
                    // No closing quote, get the end of the string!
                    lszCmdEnd = lszCmd + _ftcslen( lszCmd );
                }

                // clean out trailing white space
				while( lszCmdEnd > lszCmd && _istspace( (_TUCHAR)*--lszCmdEnd ) );

                lszCmdEnd++;

                // put the NULL terminator in
                ch = *lszCmdEnd;
                *lszCmdEnd = '\0';
            }

            // copy the string over to it own memory space
            if ( ( lpbpi->lszCmd = (char FAR *)_fmalloc( _ftcslen( lszCmd ) + 1 ) ) ) {
                _ftcscpy( lpbpi->lszCmd, lszCmd );
            }

            // no room for the new command
            else {
                if ( lszCmdEnd ) {
                    *lszCmdEnd = ch;
                }
                UnlockHlle ( hbpi );
                LLFDeleteHlleFromLl(llbpi, hbpi);
                return (HBPI)NULL;
            }

            if ( lszCmdEnd ) {
                *lszCmdEnd = ch;
            }
        }

        // put in the pass count stuff
        if ( cPass ) {
            lpbpi->bpf.f.fPass  = TRUE;
            lpbpi->cPass        = cPass;
            lpbpi->cPassCur     = cPass;
        }

        // fill in the parts that are required for a code breakpoint

        lpbpi->CodeAddr         = addr;

		// Since SYUnfixup is called above the EMI gets set so, make sure
		// that it's a legitimate address.  If it is unfixed up, then the
		// segment can't be zero, but just as a better check, we make sure
		// that the seg and offset aren't zero.  For cases where the user
		// wants to set a code breakpoint at 0:0 (real unfixed up address
		// which doesn't map to anything we have CV info for, this will
		// will NOT do the right thing.  Probably don't care?

		ASSERT( ADDR_IS_LI( addr ) );
        if ( emiAddr ( addr ) != 0 &&
        	( GetAddrSeg( addr ) || GetAddrOff( addr ))
           ) {
            lpbpi->bpf.f.fBpCode    = TRUE;
            lpbpi->bpf.f.fVirtual   = !SHFIsAddrNonVirtual( &lpbpi->CodeAddr );
            if ( !lpbpi->bpf.f.fVirtual ) {
                ADDR    addrT = addr;

                SYFixupAddr( &addrT );
                lpbpi->segFixed = GetAddrSeg( addrT );
            }
        }
        else {
            lpbpi->bpf.f.fBpCode = FALSE;
        }



        // if there are attached expressions, get them
        if ( lpDpi ) {
			CXT		cxt = {0};

            lpbpi->dpf = *pDPF;
            lpbpi->bpf.f.fBpData = TRUE;
            lpbpi->lpDpi = lpDpi;

			// If this is break at location when XXX (true/changed), use the
			// context of the addr as the default context
			if ( lpbpi->bpf.f.fBpCode && SHSetCxt( &lpbpi->CodeAddr, &cxt ) ) {
				lpbpi->lpDpi->cxf.cxt = cxt;
			}
			else {
	           	lpbpi->lpDpi->cxf = *pCXF;
			}

            // we are not context sensitive
            if ( !lpbpi->dpf.f.fContext ) {

                lpbpi->dpf.f.fContext = FALSE;
                if( !lpbpi->dpf.f.fEvalExpr ) {
                    if ( !lpbpi->dpf.f.fReg ) {
                        lpbpi->bpf.f.fVirtual =
                            !SHFIsAddrNonVirtual( &lpbpi->lpDpi->u.d.DataAddr );
                    }
                    if ( !lpbpi->bpf.f.fVirtual && !BPLoadValue(lpbpi)) {
						UnlockHlle ( hbpi );
						LLFDeleteHlleFromLl(llbpi, hbpi);
						return (HBPI)NULL;
					}
				}
            }

            // we are context sensitive
            else if ( lpbpi->dpf.f.fContext && !lpbpi->bpf.f.fBpCode == TRUE) {

                // save the breakpoint address
                lpbpi->CodeAddr = lpbpi->lpDpi->u.bp.BlkAddr;

                // we are within our current context
                if(SHIsAddrInCxt(SHSetCxt(&lpbpi->lpDpi->u.bp.BlkAddr, &cxt),
                   SHpADDRFrompCXT(SHpCXTFrompCXF((pCXF)))) &&
                   (!SHIsInProlog(SHpCXTFrompCXF((pCXF))))) {

					// CAVIAR  6671: we needed to check if we are in
					// the function prolog (above) otherwise we will bind
					// in the wrong context and never rebind thus driving
					// right through the watch/tracepoint we just set [dwg,rm]

                    if ( !lpbpi->dpf.f.fEvalExpr ) {
                        if (!BPLoadValue(lpbpi)) {
							UnlockHlle ( hbpi );
							LLFDeleteHlleFromLl(llbpi, hbpi);
							return (HBPI)NULL;
						}
                    }

                }

                // we don't have the current context, so say load on
                // function hit
                else {
                    lpbpi->bpf.f.fVirtual   = !SHFIsAddrNonVirtual( &lpbpi->CodeAddr );
                    lpbpi->bpf.f.fBpCode    = TRUE;
                    lpbpi->dpf.f.fFuncBpSet = TRUE;
                    lpbpi->dpf.f.fFuncLoad  = TRUE;
                }
            }

            // see if we need to emulate
            lpbpi->dpf.f.fEmulate = !lpbpi->bpf.f.fBpCode;

  			XOSD xosd;
            if ( xosd = SetWatch ( lpbpi, TRUE ) != xosdNone ) {
  				ASSERT (xosd < xosdNone);
                 UnlockHlle ( hbpi );
                LLFDeleteHlleFromLl(llbpi, hbpi);
                return FALSE;
            }
        }
    }
    else {
		// something went wrong! clean out the breakpoint
		// CAVIAR 6473: must return failure! [rm]
        UnlockHlle(hbpi);
        return (HBPI)NULL;
    }


    lpbpi->bpf.f.fNoStop  = fNoStop;
    lpbpi->bpf.f.fMessage = fMessage;
    lpbpi->lMessage       = lMessage;
    lpbpi->dwMask         = dwMask;

    if ( lpbpi && lpbpi->bpf.f.fBpCode && !lpbpi->bpf.f.fVirtual ) {
        HTID    htid = HtidFromHthd ( lpbpi->hthd );
        BOOL    fOneThread = ( htid != hmemNull );
        ADDR    addrT = lpbpi->CodeAddr;
        XOSD    xosd;

        SYFixupAddr ( &addrT );

        xosd = SetBreakPoint (
            hpidCurr,
            htid,
            TRUE,
            TRUE,
            &addrT,
            fOneThread,
            BpnsFromLpbpi ( lpbpi ),
            fMessage,
            lMessage,
            dwMask
        );

        if ( xosd != xosdNone && lpprcCurr->stp != stpDead ) {
            UnlockHlle ( hbpi );
            LLFDeleteHlleFromLl(llbpi, hbpi);
            errno = BADBKPTCMD;
            return (HBPI)NULL;
        }
    }

    UnlockHlle ( hbpi );
    return hbpi;
}

/*** BPFreeType
*
* Purpose: Free a particular type of BP in the BP list
*
* Input:    ushort BPType - the type of the BP
*
* Output:   none
*
* Exception:
*
* Notes:
*
*************************************************************************/

void PASCAL BPFreeType ( unsigned short BPType ) {
    HBPI    hbpi;
    HBPI    hbpiCur;
    LPBPI   lpbpi;

	if ( llbpi ) {
	    hbpiCur = LLHlleFindNext( llbpi, hbpiNull );
	    while( hbpiCur ) {
	        hbpiCur = LLHlleFindNext( llbpi, hbpi = hbpiCur );

	        // We're going to get the lp and then Unlock it BEFORE dereferencing
	        // it.  This assumes that no memory movement occurs between the
	        // unlocking and dereferencing.  Just don't do any allocations or
	        // memory compactions and life will be fine!

	        lpbpi = (LPBPI)LLLpvFromHlle( hbpi );
	        UnlockHlle( hbpi );
	        if ( lpbpi->bpf.f.BpType == BPType && lpbpi->hprc == hprcCurr ) {
	            BPDelete( hbpi );
	        }
		}
    }
}

/*** BPFirstBPI
*
* Purpose: returns a pointer to the first BPI in our list or NULL
*          if the list is empty
*
* Input:    NONE
*
* Output:   the value of lpbpiFirst
*
* Exception:
*
* Notes:
*
*************************************************************************/
HBPI PASCAL BPFirstBPI ( void ) {

    return( llbpi ? LLHlleFindNext( llbpi, hbpiNull ) : hbpiNull );
}

typedef BOOL (PASCAL NEAR *PFNBPAction) ( HBPI );

BOOL PASCAL NEAR BPEnable ( HBPI hbpi ) {
    LPBPI lpbpi;

    lpbpi = (LPBPI)LLLpvFromHlle(hbpi);

    if ( !lpbpi->bpf.f.fActive ) {
        BPF bpf = lpbpi->bpf;

        bpf.f.fActive = TRUE;

        if ( lpbpi->bpf.f.fBpCode && !lpbpi->bpf.f.fVirtual ) {
            HTID    htid = HtidFromHthd ( lpbpi->hthd );
            BOOL    fOneThread = ( htid != hmemNull );
            ADDR    addrT = lpbpi->CodeAddr;
            XOSD    xosd;
            HPRC    hprc = hprcCurr;

            if ( lpbpi->hprc != hprcCurr ) {
                SYSetContext ( lpbpi->hprc, (HTHD) NULL, FALSE );
            }
            SYFixupAddr ( &addrT );

            xosd = SetBreakPoint (
                hpidCurr,
                htid,
                TRUE,
                TRUE,
                &addrT,
                fOneThread,
                BpnsFromLpbpi ( lpbpi ),
                lpbpi->bpf.f.fMessage,
                lpbpi->lMessage,
                lpbpi->dwMask
            );
            if ( hprcCurr != hprc ) {
                SYSetContext ( hprc, (HTHD) NULL, FALSE );
            }
 			if (xosd != xosdNone && lpprcCurr -> stp != stpDead) {
 				ASSERT (FALSE); // Who knows if we handle this right?
 				UnlockHlle (hpbi);
 				LLFDeleteHlleFromLl(llbpi, hbpi);
 				return FALSE;
 			}
        }

        if ( lpbpi->bpf.f.fBpData ) {
            extern int  PASCAL BPTpWpComp( HPID, HTID, LPBPI, BOOL );

  			XOSD	xosd = xosdNone;
  
			if ( xosd = SetWatch ( lpbpi, TRUE ) != xosdNone ) {
 				ASSERT (xosd < xosdNone);
                UnlockHlle ( hbpi );
                LLFDeleteHlleFromLl(llbpi, hbpi);
                return FALSE;
            }

            BPTpWpComp ( hpidCurr, htidCurr, lpbpi, TRUE );
        }
        lpbpi->bpf.f.fActive = TRUE;
    }
    UnlockHlle(hbpi);

    return FALSE;
}

void PASCAL BPEnableFar ( HBPI hbpi ) { BPEnable ( hbpi ); }

BOOL PASCAL NEAR BPDisable ( HBPI hbpi ) {
    LPBPI lpbpi;

    lpbpi = (LPBPI)LLLpvFromHlle(hbpi);

    if ( lpbpi->bpf.f.fActive ) {
        //BPF bpf = lpbpi->bpf;

        //bpf.f.fActive = FALSE;

        if ( lpbpi->bpf.f.fBpCode && !lpbpi->bpf.f.fVirtual ) {
            HTID    htid = HtidFromHthd ( lpbpi->hthd );
            HPID    hpid = HpidFromHprc ( lpbpi->hprc );
            BOOL    fOneThread = ( htid != hmemNull );
            ADDR    addrT = lpbpi->CodeAddr;
            HPRC    hprc = hprcCurr;
            XOSD    xosd;

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
            if ( hprc != hprcCurr ) {
                SYSetContext ( hprc, (HTHD) NULL, FALSE );
            }
            ASSERT ( lpprcCurr->stp == stpDead || xosd == xosdNone );
        }


        if ( lpbpi->bpf.f.fBpData ) {
            SetWatch ( lpbpi, FALSE );
        }

        lpbpi->bpf.f.fActive = FALSE;
    }
    UnlockHlle(hbpi);

    return FALSE;
}

/*** BPResetAllPassCnt
*
* Purpose: To reinitialize all pass counts to there initial state
*
* Input:
*
* Output:
*  Returns .....
*
* Exceptions:
*
* Notes: Used on a restart of the child program
*
*************************************************************************/
void PASCAL BPResetAllPassCnt( void ) {
	if ( llbpi ) {
		HBPI     hbpi = hbpiNull;
	
		while( hbpi = LLHlleFindNext( llbpi, hbpi ) ) {
			LPBPI    lpbpi;
			
			lpbpi = (LPBPI)LLLpvFromHlle( hbpi );
			lpbpi->cPassCur = lpbpi->cPass;
			UnlockHlle( hbpi );
		}
	}
}

void PASCAL BPDisableFar ( HBPI hbpi ) { BPDisable ( hbpi ); }
