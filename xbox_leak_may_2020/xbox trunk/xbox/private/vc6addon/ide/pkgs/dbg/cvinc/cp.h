/*** cp.h -- Header file for subsystem Command Parse Handler

Copyright <C> 1990, Microsoft Corporation

Purpose:

    Define all exported functions and data from Command Parsing Handler.

***********************************************************************/
typedef enum {
	CPNOERROR,
	CPNOARGS,
	CPISOPENQUOTE,
	CPISCLOSEQUOTE,
	CPISOPENANDCLOSEQUOTE,
	CPISDELIM,
	CPNOTINQUOTETABLE
}CPRETURNS;



char FAR * PASCAL CPToken ( char FAR * szSrc, char FAR * szUserDelim );
int PASCAL CPQueryChar ( char FAR * szSrc, char FAR * szUserDelim );
int PASCAL CPQueryQuoteIndex ( char FAR * szSrc );
char FAR * PASCAL CPAdvance ( char FAR * szSrc, char FAR * szUserDelim );
char FAR * PASCAL CPTrim ( char FAR * szSrc, char chTrim );
char FAR * PASCAL CPszToken ( char FAR * szSrc, char FAR * szUserDelim );
int PASCAL CPCreateDebuggeeArgs( char *, char *** );
void PASCAL CPCreateCmdLine( int, char *** );
