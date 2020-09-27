/*** CVExpr.c - Number evaluation and parsing
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

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

BOOL CVDumpTMExpanded ( HTM, EEPDTYP, int, int, PCXF, int, BOOL );


//*************************************************************************
//*																		  *
//*    CV PARSE ROUTINES												  *
//*																		  *
//*************************************************************************

/*** CVGetCastNbr
*
* Purpose: To convert and expression into a number of a specific type
*
* Input:
*	szExpr	- The expression to evaluate
*	type	- The type to which the value should be cast
*
* Output:
*	pValue	- The numeric value is stuffed into pValue
*	szErrMsg- If this is non-null, an error message string is stuffed here
*
*  Returns The error message number
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
int	PASCAL CVGetCastNbr(
char FAR *  szExpr,
CV_typ_t	type,
int	    	Radix,
int	    	fCase,
PCXF	    pCxf,
char FAR *  pValue,
char FAR *  szErrMsg
) {
	HTM			hTM = (HTM)NULL;
	HTI			hTI = (HTI)NULL;
	PTI			pTI = NULL;
	long		vResult = 0;
	RTMI		rtmi;
	EESTATUS	Err;
	ushort		strIndex;

	// initialize some stuff
	Err = EENOERROR;
	if(szErrMsg)
		*szErrMsg = '\0';

	memset( &rtmi, 0, sizeof(rtmi) );
	rtmi.fValue = TRUE;
	rtmi.Type   = type;

	// parse the expression

	Err = EEParse(szExpr, Radix, fCase, &hTM, &strIndex);
	if(!Err) Err = EEBindTM(&hTM, SHpCXTFrompCXF(pCxf), TRUE, FALSE);
	if(!Err) Err = EEvaluateTM(&hTM, SHpFrameFrompCXF(pCxf), EEHORIZONTAL);
	if(!Err) Err = EEInfoFromTM(&hTM, &rtmi, &hTI);


	if (!Err) {
		// lock down the TI
        if( hTI  &&  (pTI = (PTI) BMLock (hTI)) ) {

			// now see if we have the value
			if( pTI->fResponse.fValue  &&  pTI->fResponse.Type == rtmi.Type ) {
				_fmemcpy (pValue, (char FAR *) pTI->Value, (short) pTI->cbValue);
			}
			else
				Err = BADTYPECAST;

            BMUnlock(hTI);
		}
		else
			Err = NOROOM;

		// get the error
		if( szErrMsg )
			CVMessage(ERRORMSG, Err, MSGSTRING, szErrMsg);
	}

	// get the error
	else {
		if( szErrMsg ) {
			CVExprErr(Err, MSGSTRING, &hTM, szErrMsg);
		}
		else {
			CVExprErr ( Err, MSGGERRSTR, &hTM, NULL );
			Err = GEXPRERR;
		}

	}
	// free any handles
	if(hTM)
		EEFreeTM(&hTM);

	if( hTI )
		EEFreeTI(&hTI);

	// return the error code

	return(Err);
}



/*** CVGetNbr
*
* Purpose: To convert and expression into a number
*
* Input:
*	szExpr	- The expression to evaluate
*
* Output:
*	pErr	- The Expression Evaluators error msg nbr.
*
*  Returns The numeric value of the expression. Or zero. If the result
*		   is zero, check the Err value to determine if an error occured.
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
long	PASCAL CVGetNbr(
char FAR *  szExpr,
int			Radix,
int			fCase,
PCXF	    pCxf,
char FAR *  szErrMsg,
int FAR *   pErr
) {
	long	    Value;

	// This function returns a LONG, but the type that we
	// ask for from the expression evaluator is a ULONG.  Which should
	// it be?

	*pErr = CVGetCastNbr(szExpr,
						 T_ULONG,
						 Radix,
						 fCase,
						 pCxf,
						 (char FAR *)&Value,
						 szErrMsg);

	return (*pErr == EENOERROR) ? Value : 0;
}


/*** CVGetList
*
* Purpose: To convert a string of expressions into a list of bytes
*
* Input:
*	szList - The list of expressions
*
* Output:
*	pErr	- The Expression Evaluators error msg nbr.
*
*  Returns The numeric value of the expression. Or zero. If the result
*		   is zero, check the Err value to determine if an error occured.
*
* Exceptions:
*
* Notes:
*
*************************************************************************/

int PASCAL CVGetList(
char	*szList,
int	    Radix,
int	    fCase,
PCXF	pCxf,
int	    iMax,
char	rgbList[],
int		*piCount
) {
	long	val;
	int 	error;
    char	*sz;

    int		i = 0;
    char	buf[128];


    sz = szList;

    while( (i < iMax) && *sz && (*sz != ';') )
		{
		int		cParen = 0;
		int 	fNotQuote = 1;
		int 	fBreak = FALSE;
		int 	fIsString;
		char	*szBuf = buf;

		while (_istspace((_TUCHAR)*sz)) sz++;

		// If the whole expression is a string.
		// We parse it ourselves using C syntax

		fIsString = '"' == *sz;

		do {
			switch(*szBuf++ = *sz++) {
				case '"': {
					if(!fNotQuote){

						// "" counts a s a single embedded quote

						if('"' != *sz) fNotQuote = 1;
						 else *szBuf++ = *sz++;
						}
					else {

						fNotQuote = 0;
						}

					// Eliminate quotes from a raw string.

					if(fIsString) szBuf--;
					break;
					}

				// Count depth of parens

				case '(': {
					cParen += fNotQuote;
					break;
					}

				case ')': {
					cParen -= fNotQuote;

					// Check syntax error

					if(cParen < 0) fBreak = TRUE;
					break;
					}

				// Whitespace is OK in parens or strings

				case ';':
				case '\t':
				case ' ': if((cParen > 0) || !fNotQuote) break;

				// Otherwise it terminates

				case '\0':
				case '\f':
				case '\n':
				case '\r':
				case '\v':
					szBuf--;
					sz--;
					fBreak = fTrue;
				}
			} while(!fBreak);

		if(!fNotQuote || cParen) {
			errno = SYNTAXERROR;
			return FALSE;
			}

		// Terminate expression in our buffer

		*szBuf = '\0';

		if(fIsString) {
				char *pch = buf;

				// Just copy from our buffer

				while( (i < iMax) && *pch ) rgbList[i++] = *(pch++);

				// Error if not room for data in list

				if(*pch) return FALSE;
			}
		else {
			val =  CVGetNbr( (char FAR *) buf,
						Radix,
						fCaseSensitive,
						&cxfIp,
						NULL,
						(int FAR *) &error );
			if(error == EENOERROR ) {

				// Check for overflow

				if((255 < val) || (-256 > val)) {
					// Value out of range
					errno = SYNTAXERROR;
					return FALSE;
					}
				rgbList[i++] = (char)val;
				}

			// EE error

			else return(FALSE);
	    	}
        }
	*piCount = i;
	return(sz-szList);
}



/*** CVEvaluate
*
* Purpose: To evaluate the pchExpr and return the formatted value in pchVal
*
* Input:
*	szList - The list of expressions
*
* Output:
*	pErr	- The Expression Evaluators error msg nbr.
*
*  Returns The numeric value of the expression. Or zero. If the result
*		   is zero, check the Err value to determine if an error occured.
*
* Exceptions:
*
* Notes:
*
*************************************************************************/

#pragma optimize ( "", off )

int PASCAL CVEvaluate (
    char FAR *szExpr,
    int Radix,
    int fCase,
    PCXF pcxf,
    char *szVal
) {
	EEHSTR		hError;
	EEHSTR		hValue;
    char FAR	*buf;
    ushort	    status;
    int         fRet = TRUE;
    HTM         htm;
    ushort      strIndex;
    char        rgchVal [ 256 ];

    rgchVal [ 0 ] = '\0';

    // parse bind and evaluate expression
    if (
        (status = EEParse (szExpr, Radix, fCase, &htm, &strIndex)) ||
        (status = EEBindTM (&htm, SHpCXTFrompCXF(pcxf), FALSE, FALSE)) ||
        (status = EEvaluateTM (&htm, SHpFrameFrompCXF (pcxf), EEVERTICAL))
    ) {

        if ( EEGetError (&htm, status, &hError) == EENOERROR ) {

            if ( hError != EENOERROR ) {
                buf = (char FAR *)BMLock ( hError );
                _ftcsncpy (
                    rgchVal,
                    buf,
                    min ( _ftcslen ( buf ) + 1, MAXSTRING + 35 - 1 )
                );
                BMUnlock ( hError );
                EEFreeStr ( hError );
                fRet = FALSE;
	    	}
		}
    }
    else {

        // Get the formatted value from the TM and copy to static buffer

        if ( szVal != NULL ) {

            if ( !(status = EEGetValueFromTM (&htm, Radix, (PEEFORMAT)"p", &hValue) ) ) {
                buf = (char FAR *)BMLock ( hValue );

                _ftcsncpy (
                    rgchVal,
                    buf,
                    min ( _ftcslen ( buf ) + 1, 128 + 34 )
                );

                // unlock and free the buffer
                BMUnlock ( hValue );
                EEFreeStr ( hValue );
            }
            else {

                // could not get value
                if ( EEGetError (&htm, status, &hError) == EENOERROR ) {
                    if ( hError != 0 ) {
                        buf = (char FAR *)BMLock ( hError );
                        _ftcsncpy (
                            rgchVal,
                            buf,
                            min (_ftcslen (buf)+1, MAXSTRING + 35 - 1)
                        );
                        BMUnlock (hError);
                        EEFreeStr (hError);
                        fRet = FALSE;
                    }
                }
            }
        }
        else {

            CVDumpTMExpanded (
                htm,
                EEIsExpandable ( &htm ),
                Radix,
                fCase,
                pcxf,
                0,
                (szExpr[strIndex] != '\0')
            );
        }
    }

    // if we have a TM, free it
    if ( htm ) {
        EEFreeTM ( &htm );
    }

    if ( szVal != NULL ) {
        _ftcscpy ( szVal, rgchVal );
    }
    else if ( !fRet ) {
        dputs ( rgchVal );
    }

    return fRet;
}


BOOL CVDumpTMExpanded (
    HTM     htm,
    EEPDTYP eepdtyp,
    int     Radix,
    int     fCase,
    PCXF    pcxf,
    int     iLevel,
    BOOL    fFormatStr
) {
    BOOL    fRet = TRUE;
    char    rgchVal [ 256 ];
    ushort  status;
	EEHSTR	hValue = (EEHSTR) NULL;
    LSZ     lszBuf;
    char *  pszEEFmt;
    EEHSTR  hAccess = 0;


    pszEEFmt = ( eepdtyp == EENOTEXP ? (char *)NULL : "p" );

    if (
        ( eepdtyp == EETYPE || eepdtyp == EETYPENOTEXP || eepdtyp == EETYPEPTR ) ||
        !(status = EEGetValueFromTM ( &htm, Radix, (PEEFORMAT)pszEEFmt, &hValue ) )
    ) {
		EEHSTR	hName,hPlainName;
		int 	iTab;

        for ( iTab = 0; iTab < iLevel; iTab++ ) {
            dprintf ( "  " );
        }

        switch( eepdtyp ) {

            case EETYPEPTR:
	    case EETYPENOTEXP:
		EEGetNameFromTM ( &htm, &hPlainName );
		EEGetTypeFromTM ( &htm, (EEHSTR) hPlainName, &hName, 0L );

		// skip structure at head of buffer
		lszBuf = (LSZ)BMLock ( hName ) + sizeof (HDR_TYPE);
		dprintf("%Fs ", lszBuf );

		BMUnlock ( hName );
		EEFreeStr ( hName );
		EEFreeStr ( hPlainName );

		break;

            case EENOTEXP:
            case EEAGGREGATE:
            case EETYPE:
            case EEPOINTER:
		if ( iLevel <= 10 ) {

		    EEGetNameFromTM ( &htm, &hPlainName );

		    if ( eepdtyp == EETYPE ) {

			EEGetTypeFromTM ( &htm, (EEHSTR) hPlainName, &hName, 0L );

			// skip structure at head of buffer
			lszBuf = (LSZ)BMLock ( hName ) + sizeof (HDR_TYPE);
			dprintf( "%Fs ", lszBuf );
			BMUnlock ( hName );
			EEFreeStr ( hName );
		    }

		    lszBuf = (char FAR *)BMLock ( hPlainName );

		    if ( eepdtyp != EENOTEXP || iLevel ) {
                        dprintf (
				"%Fs%s",
				// workaround for caviar #4495: if iLevel==0 do not print
				// a name, since it may be incorrect --gdp 10/22/92
				( eepdtyp == EETYPE || iLevel == 0) ? "" : lszBuf,
			    ( eepdtyp == EETYPENOTEXP ) ? " " : ( hValue ? " = " : " " )
                        );
		    }

		    BMUnlock ( hPlainName );
		    EEFreeStr ( hPlainName );


		}
                break;
        }

        if ( hValue ) {
            lszBuf = (char FAR *)BMLock ( hValue );

            _ftcsncpy (
                rgchVal,
                lszBuf,
                min ( _ftcslen ( lszBuf ) + 1, 128 + 34 )
            );

            // unlock and free the buffer
            BMUnlock ( hValue );
            EEFreeStr ( hValue );

	    dprintf ( "%s ", rgchVal );
	}

	EEGetAccessFromTM (&htm, &hAccess, 0L);

	if ( hAccess ) {
	    lszBuf = (char FAR *)BMLock ( hAccess );
	    dprintf ( "%Fs", lszBuf );
	    // unlock and free the buffer
	    BMUnlock ( hAccess );
	    EEFreeStr ( hAccess );
	}

	dputs(""); // new line

    }
    else {
		EEHSTR hError;

        // could not get value
        if ( EEGetError ( &htm, status, &hError ) == EENOERROR ) {
            if ( hError != 0 ) {
                lszBuf = (char FAR *)BMLock ( hError );
                _ftcsncpy (
                    rgchVal,
                    lszBuf,
                    min (_ftcslen (lszBuf)+1, MAXSTRING + 35 - 1)
                );
                BMUnlock ( hError );
                EEFreeStr ( hError );
                fRet = FALSE;

		dputs ( rgchVal );
            }
        }
    }


    if ( fRet && !fFormatStr ) {

		if (
			// restrict recursion to one level deep
			// (work around for caviar #927) --gdp 10/6/92
			iLevel <= 0 &&
            ( eepdtyp == EEAGGREGATE || eepdtyp == EETYPE )
        ) {
            long    ctm;
            long    itm;
            SHFLAG  fVariable;
            ushort  ich;
            EEPDTYP fExpandT;

            EEcChildrenTM ( &htm, &ctm, &fVariable );

            for ( itm = 0; itm < ctm; itm++ ) {
                HTM htmChild = (HTM) NULL;

                if ( ! (status = EEGetChildTM ( &htm, itm, &htmChild, &ich, Radix, fCaseSensitive ) ) &&
                   ( ( fExpandT = EEIsExpandable ( &htmChild ) ) == EETYPE ||
                     fExpandT == EETYPENOTEXP ||
                       ( ! ( status = EEvaluateTM (
                        &htmChild,
                        SHpFrameFrompCXF (pcxf),
                        EEVERTICAL
                ) ) ) ) ) {

                    fRet = CVDumpTMExpanded (
                        htmChild,
                        fExpandT,
                        Radix,
                        fCase,
                        pcxf,
                        iLevel + 1,
                        FALSE
                    );
                }
                else {
					EEHSTR hError;

                    if ( EEGetError ( htmChild ? &htmChild : &htm, status, &hError ) == EENOERROR ) {
                        if ( hError != 0 ) {
                            lszBuf = (char FAR *)BMLock ( hError );
                            _ftcsncpy (
                                rgchVal,
                                lszBuf,
                                min (_ftcslen (lszBuf)+1, MAXSTRING + 35 - 1)
                            );
                            BMUnlock ( hError );
                            EEFreeStr ( hError );
                            dputs ( rgchVal );
                        }
                    }
                    fRet = FALSE;
                }

                EEFreeTM ( &htmChild );
                if ( !fRet ) {
                    break;
                }
            }
        }
    }

    return fRet;
}

#pragma optimize ( "", on )


/*** CVParseBind
* Purpose: To parse and bind an expression returning tm and error value
*
* Input:
*	PHTM	    phtm	handle to expression.
*	PCXT	    pcxt	context to start symbol search if {} not supplied by users.
*	int	    	iRadix	radix to use.
*	char	    fCase	case sensitivity.
*	char	    fForce	force bind to new context.
*	char FAR *  sz		pointer to expression.
*
* Output:
*
*
*  Returns
*
*	EESTATUS    eest	EENOERROR / 0 if successfull
*						!0 Failuer CVExprErr has static buffer message.
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
EESTATUS PASCAL CVParseBind (
PHTM phtm,
PCXT pcxt,
int  iRadix,
char fCase,
char fForce,
char FAR * sz) {

	EESTATUS eest = EENOERROR;
	ushort	 strIndex;

	eest = EEParse(sz, iRadix, fCase, phtm, &strIndex);
    if ( !eest ) {
		eest = EEBindTM(phtm, pcxt, fForce, FALSE);
    }
    return eest;
}

/*** CVParseBindCxtl
*
* Purpose: To parse, bind, and get Context list for an expression
*	    returning tm and error value
*
* Input:
*	PHTM	    phtm	handle to expression.
*	PCXT	    pcxt	context to start symbol search if {} not supplied by users.
*	int	    	iRadix	radix to use.
*	char	    fCase	case sensitivity.
*	char	    fForce	force bind to new context.
*	PHCXTL	    phcl	pointer to context list handle - modified by this function.
*	char FAR *  sz		pointer to expression.
*
* Output:
*
*
*  Returns
*
*	EESTATUS    eest	EENOERROR / 0 if successfull
*						!0 Failuer CVExprErr has static buffer message.
*
* Exceptions:
*
* Notes:
*
*************************************************************************/
EESTATUS PASCAL CVParseBindCxtl(
PHTM		phtm,
PCXT		pcxt,
int			iRadix,
char		fCase,
char		fForce,
PHCXTL		phcl,
char FAR *	sz) {

	EESTATUS eest = EENOERROR;
	ushort	 strIndex;

	eest = EEParse(sz, iRadix, fCase, phtm, &strIndex);
    if ( !eest ) {
		eest = EEBindTM(phtm, pcxt, fForce, FALSE);
		if ( !eest ) {
		    eest = EEGetCXTLFromTM (phtm, phcl);
		}
    }
    return eest;
}
