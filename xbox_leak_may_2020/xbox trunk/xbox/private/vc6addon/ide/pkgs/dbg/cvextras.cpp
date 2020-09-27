#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

BOOL fIgnoreAmbigBPs = FALSE ;
BOOL fUseDebugRegs = FALSE ;
char fCaseSensitive = FALSE;
char flip_or_swap = FALSE ;
unsigned long ulChildFlags = 0L ;

/*
extern "C"
void LOADDS FAR PASCAL CVAssertFailed(
	char FAR * lszFile,
	char FAR * lszMsg,
	int iln)
{
#ifdef _DEBUG
	if (AfxAssertFailedLine(lszFile, iln))
		AfxDebugBreak ();
#endif
}
*/

// pointer on EE functions
PEXF pEXF ;

// SYSTEM.C
int update_watch_window(void)
{
return TRUE ;
}


// ISLEDATA.C
// Comand line info
#define MAXINPLINE	256
char *ptxt ;
int radix ;

// Visual C++ always uses decimal for user input, except
// whe the disassembly window's GOTO dialog is up
int radixInput = 10;

char input_text[MAXINPLINE] ;
char regs_changed = FALSE ;
char fTargetIs386 = FALSE ;
unsigned char fActiveScreen = 0 ;
char fTry386Mode = FALSE ;
char fIn386Mode = TRUE;
#define MAXREG    18
unsigned long debug_register_table[8];


// CV0.C
#define clanmax  2
LAN rglan[clanmax] ;    		// We only use index 0 for C++ !
int clan = 1 ;          		// Number of installed EE for languages
int ilan = 0 ;          		// language index
int iee = 0 ;           		// EE current index should be 0 !
ushort ArrayDefault = 10 ;
ADDR dump_start_address ;

void PASCAL go_until(PADDR paddr, HTHD hthd, int BpType, BOOL fGoException)
{
	GoUntil ( paddr, hthd, BpType, fGoException, FALSE );

	// If the process died, we must update all state info
	if (lpprcCurr->stp == stpDead)
		UpdateDebuggerState ( UPDATE_ALLSTATES );
}


// Return index to current EE in rglan structure
int PASCAL ESilan(void)
{
	return iee ;
}
// set the current ES from index in rglan
int PASCAL ESSetFromIndex(int ilan)
{
	iee = ilan ;
	pEXF = rglan[ilan].pexf ;
	fCaseSensitive = rglan[ilan].fCaseSensitive ;
	radix = rglan[ilan].Radix ;
	return(TRUE) ;
}


// CV1.C

int fBlocking = TRUE ;
BYTE fRecurCommand = FALSE ;

char is_assign = FALSE ;


// Refresh screen following return from child
void PASCAL display_pc(void)
{
}

// STATEFL.C
char * G_myname ;   // Buffer for codeview's invoked name

// CV31.C
void island_screen(void)
{
}


// LOAD.C
char input_redirected = FALSE ;
char cLines = 50 ;
void quit(int ec)
{
}

// CMDWIN.C
// Formatted output
void dprintf(char * format_string,...)
{
}
// prompt display
void PASCAL DispPrompt(void)
{
}
// Unformatted output
void dputs(char * s)
{
}

// LCLWN.C

typedef int lts ;
// print an error message to the status line
lts PASCAL FTError(lts ltsCode)
{
	// AuxPrintf2("FtError code: %d", (int)ltsCode);
	return ltsCode;
}

// ASM386.C
ADDR asmaddr ;


// REG.C
void update_register_display(char fForce)
{
}

// VIEW0.C
BOOL PASCAL FHAddSyn(char * szPath,char * szSyn)
{
   return TRUE ;
}

// CV31.C
void dos_screen(void)
{
}

// CLP.C

// Make CV400 modules callable for more than debugging session
void PASCAL ResetCV400Globals(void)
{
	extern HLLI llbpi;                        // brkpt0.c
	extern CXF cxfIp ;

	memset(&fEnvirGbl, 0, sizeof(fEnvirGbl)); // cvextras.c
	memset(&cxfIp, 0, sizeof(cxfIp));			// shsymbol.c

	hlliFME = (HLLI)NULL ;

	llbpi = NULL;
	fBPCCInExe = FALSE;
}



// CVMSG.C
typedef int MSGWHERE ;
typedef int MSGTYPE ;

void PASCAL CVExprErr(
   EESTATUS    Err,
   MSGWHERE    msgwhere,
   PHTM	    phTM,
   char FAR *  szErr)
{

   EEHSTR	    hErrStr;
   char FAR *  pErrStr;

   // check for internal catastrophic conditions
   if( !phTM  ||  !(*phTM) )
	  Err = EECATASTROPHIC;

   // now print this expression evaluator error
   switch( Err ) {
	  default:
	     // get the error string from the EE
	     if(!EEGetError(phTM, Err, &hErrStr) ) {
		    // lock the string in memory
		    if((pErrStr = (char *)BMLock( hErrStr )) ) {
		       if(msgwhere == MSGSTRING )
			      CVMessage(EXPREVALMSG, EXPRERROR, msgwhere, szErr, pErrStr);
		       else
			      CVMessage(EXPREVALMSG, EXPRERROR, msgwhere, pErrStr);
		       BMUnlock ( hErrStr );
		    }
		    // we really are having problems
		    else
		       Err = EECATASTROPHIC;
		    // free the error string
		    EEFreeStr( hErrStr );
	     }
	     // only exit if not catastrophic
	     if(Err != EECATASTROPHIC )
		    break;
         // otherwise say catastrophic error
      case EECATASTROPHIC:
	     CVMessage(ERRORMSG, CATASTROPHICTM, msgwhere, szErr);
	     break;

	  case EENOMEMORY:
	     CVMessage(ERRORMSG, NOROOM, msgwhere, szErr);
	     break;

	case EEBADADDR:
		CVMessage(ERRORMSG, BADADDR, msgwhere, szErr);
		break;

	  // don't print an error
	  case EENOERROR:
	     break;
   }
}

extern "C"
void CVMessage (
	MSGTYPE		msgtype,
	MSGID		msgid,
	MSGWHERE	msgwhere,
	... )
{
	static	char 		szCVErr[40];
	static	char 		szCVWarn[40];
	static	char 		szCVMsg[40];
	static	const char	szEmpty[] = "";
	static	const char	szFormat[] = "CV%04u %s:  ";
	static	char		gszErrStr[ MAXERRMSG ];
	static	MSGID		gMSGID;
	char FAR *	lszStringLoc;
	char		rgch[ MAXERRMSG ];
	va_list		va_mark;
	const char *szErr;
	char *		szHelpError ;

	//
	// Set the beginning of the variable argument marker, and call vsprintf
	// with that marker.  If no template vars in error_string (szHelpError), this
	// is just the same as using szHelpError.
	//
	va_start ( va_mark, msgwhere );

	// if we want it in a string, get the sting pointer
    if( msgwhere == MSGSTRING || msgwhere == CVMSGSTRING ) {
        lszStringLoc = va_arg(va_mark, char FAR *);
    }

	if( msgid == GEXPRERR ) {
		msgid = gMSGID;
		_ftcscpy( rgch, gszErrStr );
	}
	else {
		switch( msgtype ) {
			case INFOMSG:
				szHelpError = (char *) msgid;
				vsprintf ( rgch, szHelpError, va_mark );
				break;

			case EXPREVALMSG:
				_ftcscpy(rgch, va_arg(va_mark, char FAR *));
				break;

			case FMESSAGEMSG: {
					char rgchT[MAXERRMSG];

					_ftcscpy (
						rgchT,
						va_arg ( va_mark, char FAR * )
					);

					szHelpError = error_string ( (USHORT) msgid );
					sprintf ( rgch, szHelpError, rgchT );
				}
				break;

			default:
				szHelpError = error_string ( (USHORT) msgid );
				vsprintf ( rgch, szHelpError, va_mark );
				break;
		}
	}

	va_end ( va_mark );
	szHelpError = rgch;
	//
	// If not able to use cw or we want to go to the cmd window, go there.
	//

	switch( msgtype ) {
		case EXPREVALMSG:
		case ERRORMSG:
			LoadString (hInst, ERR_CVErr, szCVErr, sizeof(szCVErr));
			szErr = szCVErr;
			break;

		case WARNMSG:
			LoadString (hInst, ERR_CVWarn, szCVWarn, sizeof(szCVWarn));
			szErr = szCVWarn;
			break;

		case FMESSAGEMSG:
		case MESSAGEMSG:
			LoadString (hInst, ERR_CVMsg, szCVMsg, sizeof(szCVMsg));
			szErr = szCVMsg;
			break;

		default:
			szErr = szEmpty;
			break;
	}

    switch ( msgwhere ) {
    	case MSGSTRING :
			_ftcscpy( lszStringLoc, szHelpError );
			break ;

		case CVMSGSTRING :
        	CVsprintf ( lszStringLoc, (LSZ)szFormat, msgid, szErr );
        	_ftcscat ( lszStringLoc, szHelpError );
        	break ;
    	case MSGGERRSTR :
			gMSGID = msgid;
			_ftcscpy(gszErrStr, szHelpError);
			break ;
		case CMDWINDOW :
			if( msgtype != INFOMSG ) {
				;// AuxPrintf3 ( (LPSTR)szFormat, msgid, (LPSTR)szErr );
			}
			// AuxPrintf1 ( szHelpError );
			if (msgid == MODLOADED || msgid == NOSYMBOLS || msgid == MODNOMATCH || msgid == EXPORTSCONVERTED || msgid == SYMSCONVERTED)
				DebuggerMessage(Information, 0,(LPSTR)szHelpError,SHOW_REPORT) ;
			else
				DebuggerMessage(Information, 0,(LPSTR)szHelpError,SHOW_ASYNCHRONOUS) ;
			break ;
		case MSGBOX : {
			const char *szCaption;

			switch ( msgtype ) {
				case EXPREVALMSG:
				case ERRORMSG :
				case WARNMSG :
				case MESSAGEMSG :
					szCaption = szErr;
					break;

				case INFOMSG :
					szCaption = szHelpError;
					szHelpError = NULL;
					break;
			}
			if(szHelpError && szHelpError[0])
				// AuxPrintf2("MSG : %s",(LPSTR)szHelpError) ;
			DebuggerMessage(Information, 0,(LPSTR)szHelpError,SHOW_ASYNCHRONOUS) ;
		}
	}
}
