/*** cp.c -- Command Parsing Subsystem API


Copyright <C> 1990, Microsoft Corporation

Purpose:


*************************************************************************/
#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/*** CPToken - Get token from string

Purpose:	parse string given static token and delimiter tables This
			function uses a stack based method to determe the level of
			open and closing quote pairs.

Input:		szSrc	- command line entered by user

Output:

	Returns: pointer to last char in token + 1
			 null if error

Exceptions:

Notes:


*************************************************************************/
char FAR * PASCAL CPToken ( char FAR * szSrc, char FAR * szUserDelim )
{
    int rgiStack[MAXNESTING];
    int iSP = 0;
    int fCharType;
    int iQuoteIndex;
    int fDone = 0;

	ASSERT( szSrc != NULL );

	while ( *szSrc && !fDone )
    {

		fCharType = CPQueryChar ( szSrc, szUserDelim );
		if ( fCharType	== CPISOPENQUOTE )
        {

			iQuoteIndex = CPQueryQuoteIndex ( szSrc );
			rgiStack[iSP++] = iQuoteIndex;
		}
		else if ( fCharType == CPISCLOSEQUOTE )
        {
			if ( *szSrc == rgchCloseQuote[rgiStack[iSP - 1]] && iSP)
				iSP--;
			else
				szSrc = NULL;
		}
		else if ( fCharType == CPISOPENANDCLOSEQUOTE )
        {
			if ( iSP && rgiStack[iSP - 1] == (iQuoteIndex = CPQueryQuoteIndex ( szSrc ) ) )
            {
				if ( *szSrc == rgchCloseQuote[rgiStack[iSP - 1]] && iSP)
					iSP--;
				else
					szSrc = NULL;
			}
			else
            {
				iQuoteIndex = CPQueryQuoteIndex ( szSrc );
				rgiStack[iSP++] = iQuoteIndex;
			}
		}
		else if ( fCharType == CPISDELIM )
        {
			if ( !iSP )
				fDone = 1;
		}

		szSrc = _ftcsinc( szSrc );
	}

	return szSrc;
}

/*** CPszToken - Get token from string and terminate it with '\0'

Purpose:	parse string given static token and delimiter tables This
			function uses a stack based method to determe the level of
			open and closing quote pairs.

Input:		szSrc	- command line entered by user

Output: we modify szSrc to null terminate the token currently pointed to

	Returns: pointer to last char in token + 1
			 null if error

Exceptions:

Notes:


*************************************************************************/
char FAR * PASCAL
CPszToken ( char FAR * szSrc, char FAR * szUserDelim ) {

int rgiStack[MAXNESTING];
int iSP = 0;
int fCharType;
int iQuoteIndex;
int fDone = 0;

	ASSERT( szSrc != NULL );
	szSrc = CPAdvance(szSrc, szUserDelim);
	while ( (szSrc != NULL) && *szSrc && !fDone ) {

		fCharType = CPQueryChar ( szSrc, szUserDelim );
		if ( fCharType	== CPISOPENQUOTE ) {

			iQuoteIndex = CPQueryQuoteIndex ( szSrc );
			rgiStack[iSP++] = iQuoteIndex;
		}
		else if ( fCharType == CPISCLOSEQUOTE ) {
			if ( iSP && (*szSrc == rgchCloseQuote[rgiStack[iSP - 1]]) ) {
				iSP--;
			}
			else {
				szSrc = NULL;
			}
		}
		else if ( fCharType == CPISOPENANDCLOSEQUOTE ) {
			if ( iSP && rgiStack[iSP - 1] == (iQuoteIndex = CPQueryQuoteIndex ( szSrc ) ) ) {
				if ( *szSrc == rgchCloseQuote[rgiStack[iSP - 1]] && iSP) {
					iSP--;
				}
				else {
					szSrc = NULL;
				}
			}
			else {
				iQuoteIndex = CPQueryQuoteIndex ( szSrc );
				rgiStack[iSP++] = iQuoteIndex;
			}
		}
		else if ( fCharType == CPISDELIM ) {
			if ( !iSP ) {
				fDone = 1;
			}
		}
        if ( szSrc != NULL ) {
    		szSrc = _ftcsinc( szSrc );
        }
	}

    if ( szSrc == NULL ) {
        return szSrc;
    }
	else if ( *szSrc == '\0') {
		// APENNELL check this someday? szSrc == NULL;
	}
	else {
		*(szSrc - 1) = '\0';
		szSrc = CPAdvance(szSrc, "");
	}
	return szSrc;
}


/*** CPTrim - removes the quoting charaters at either end of szSrc

Purpose:

Input:		szSrc	- command line entered by user

Output:

	Returns: pointer to last char in token + 1
			 null if unbalanced

Exceptions:

Notes:	szSrc had better be pointing to an CPISOPENQUOTE character!


*************************************************************************/
char FAR * PASCAL
CPTrim ( char FAR * szSrc, char chTrim ) {

int			rgiStack[MAXNESTING];
int			iSP = 0;
int			fCharType;
int			iQuoteIndex;
int			fDone = 0;
char FAR *	szRetSrc = szSrc + 1;

	ASSERT( szSrc != NULL );
	while (szSrc != NULL && *szSrc && !fDone ) {

		fCharType = CPQueryChar ( szSrc, "" );
		if ( fCharType == CPISOPENQUOTE ) {

			if (iSP == 0) *szSrc = chTrim;
			iQuoteIndex = CPQueryQuoteIndex ( szSrc );
			rgiStack[iSP++] = iQuoteIndex;

		}
		else if ( fCharType == CPISCLOSEQUOTE ) {
			if ( *szSrc == rgchCloseQuote[rgiStack[iSP - 1]] ) {
				iSP--;
				if (iSP == 0) *szSrc = chTrim;
			}
			else {
				szSrc = szRetSrc = NULL;
			}
		}
		else if ( fCharType == CPISOPENANDCLOSEQUOTE ) {
			if ( iSP && rgiStack[iSP - 1] == (iQuoteIndex = CPQueryQuoteIndex ( szSrc ) )) {
				if ( *szSrc == rgchCloseQuote[rgiStack[iSP - 1]] && iSP) {
					iSP--;
					if (iSP == 0) *szSrc = chTrim;
				}
				else {
					szSrc = szRetSrc = NULL;
				}
			}
			else {
				iQuoteIndex = CPQueryQuoteIndex ( szSrc );
				if (iSP == 0) *szSrc = chTrim;
				rgiStack[iSP++] = iQuoteIndex;
			}
		}

		if (iSP == 0) return szRetSrc;

        if ( szSrc != NULL ) {
    		szSrc = _ftcsinc( szSrc );
        }
	}
	return NULL;
}
/*** CPQueryChar - Check the delimiter and Quote table for given char

Purpose: Given a character return whether or not it is a character in our
	 delimiter table or our Quoting table

Input:   szSrc   - command line entered by user

Output:

	Returns:

Exceptions:

Notes:


*************************************************************************/
int PASCAL
CPQueryChar ( char FAR * szSrc, char FAR * szUserDelim ) {

int i, nUserDelim;

	ASSERT( szSrc != NULL );
	nUserDelim = _ftcslen( szUserDelim );
	for ( i = 0; i < MAXQUOTE; i++ ) {
		if (*szSrc == rgchOpenQuote[i] && *szSrc == rgchCloseQuote[i] ) {
			return CPISOPENANDCLOSEQUOTE;
		}
		else if (*szSrc == rgchOpenQuote[i]	) {
			return CPISOPENQUOTE;
		}
		else if ( *szSrc == rgchCloseQuote[i] ) {
			return CPISCLOSEQUOTE;
		}
	}

	for ( i = 0; i < MAXDELIM; i++ ) {
		if ( *szSrc == rgchDelim[i] ) {
			return CPISDELIM;
		}
	}

	for ( i = 0; i <= nUserDelim; i++ ) {
		if ( *szSrc == szUserDelim[i] ) {
			return CPISDELIM;
		}
	}
	return CPNOERROR;
}


/*** CPQueryQuoteIndex - Given a Character return the index

Purpose: Given a character we must be able to get the index in the quote table
		 for the character

Input:   szSrc   - command line entered by user

Output:

	Returns:

Exceptions:

Notes:


*************************************************************************/
int PASCAL
CPQueryQuoteIndex ( char FAR * szSrc ) {

int i;

	ASSERT( szSrc != NULL );
	for ( i = 0; i < MAXQUOTE; i++ ) {
		if (*szSrc == rgchOpenQuote[i] ) {
			return i;
		}
	}
	return CPNOTINQUOTETABLE;
}


/*** CPAdvance - Advance over all leading delimiters

Purpose: Given a string return a pointer to the next non-delimiter character

Input:   szSrc   - command line entered by user

Output:

	Returns: pointer to the next non-delimiter character in szSrc

Exceptions:

Notes:


*************************************************************************/
char FAR * PASCAL
CPAdvance ( char FAR * szSrc, char FAR * szUserDelim ) {
    char FAR *  szSrcSav = szSrc;

	while ( szSrc && *szSrc && CPQueryChar ( szSrc, szUserDelim ) == CPISDELIM ) {
		szSrc = _ftcsinc( szSrc );
	}

    if ( !szSrc ) {
        szSrc = szSrcSav + _ftcslen( szSrcSav );
    }

	return szSrc;
}

int PASCAL
CPCreateDebuggeeArgs(
char *		sz,
char ***	prgsz ) {
	int		csz = 0;
	int		fInQuote = FALSE;
	char *	pchStart = sz;
	char *	pch = sz;
	int		cch;
	char **	rgsz = (char **)NULL;
	void *	pv;

	do {
		if ( *pch == '\"' ) {
				fInQuote = !fInQuote;
		}
		// End of arg
		if ( pch != pchStart && ( !*pch || ( !fInQuote && *pch == ' ' ) ) ) {
			// remove leading spaces
			while( *pchStart == ' ' ) {
				++pchStart;
			}
			cch = pch - pchStart;

			// Realloc don't work so well under windows.
			pv = malloc( ( csz + 1 ) * sizeof( char * ) );
			if ( pv ) {
				if ( rgsz ) {
					memcpy( pv, (void *)rgsz, csz * sizeof( char * ) );
					free( rgsz );
				}
				rgsz = (char **)pv;
				if ( rgsz[ csz ] = (char *)malloc( cch + 1 ) ) {
					memcpy( rgsz[ csz ], pchStart, cch );
					rgsz[ csz ][ cch ] = '\0';
				}
				++csz;
				pchStart = pch + 1;
			}
		}
	} while( *pch++ );
	*prgsz = rgsz;
	return csz;
}

void PASCAL CPCreateCmdLine(
int			cCmd,
char ***	prgsz ) {
	int		iCur;
	char *	sz = (char *)NULL;
	int		cch;
	int		ichLast = 0;
	char **	rgsz = *prgsz;

	for( iCur = 0; iCur < cCmd; ++iCur ) {
		cch = _ftcslen( rgsz[ iCur ] ) + 1;
		if ( sz = (char *)realloc( sz, cch + ichLast ) ) {
			memcpy( sz + ichLast, rgsz[ iCur ], cch );
			free( rgsz[ iCur ] );
			ichLast += cch;
		}
	}
	sz = (char *)realloc( sz, ichLast + 1 );
	*( sz + ichLast ) = '\0';
	*prgsz = rgsz = (char **)realloc( rgsz, sizeof( char ** ) );
	rgsz[ 0 ] = sz;
}
