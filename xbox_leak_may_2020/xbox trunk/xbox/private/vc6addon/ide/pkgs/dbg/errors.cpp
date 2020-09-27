/*** errors.c - error and message tables
*
*   Copyright <C> 1989, Microsoft Corporation
*
*   Purpose:
*
*************************************************************************/
#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

typedef struct CVERRTORES {
	ushort   errornum;
	UINT	 idResource;
} CVERRTORES;

#define ERR(name)	{ name, CVERR_##name },

const static CVERRTORES mpErrToRes[] = {
	ERR(CV_NOERROR)
	ERR(BADBKPTCMD)
	ERR(NOTLVALUE)
	ERR(EACCES)
	ERR(EFAULT)
	ERR(EINVAL)
	ERR(EIO)
	ERR(EMFILE)
	ERR(EXDEV)
	ERR(ENOENT)
	ERR(ENOEXEC)
	ERR(ENOMEM)
	ERR(E2BIG)
	ERR(ENOSPC)
	ERR(ESRCH)
	ERR(MODLOADERROR)
	ERR(NOCODE)
	ERR(NOSOURCEHERE)
	ERR(NOSYMBOLS)
	ERR(FUTURESYMBOLS)
	ERR(NOTPACKED)
	ERR(MUSTRELINK)
	ERR(SYNTAXERROR)
	ERR(NOROOM)
	ERR(MODLOADED)
	ERR(BADTYPECAST)
	ERR(NOWATCHFUNC)
	ERR(CLSTKCHGD)
	ERR(NOADDRGIVEN)
	ERR(TOOMANYADDR)
	ERR(MODNOMATCH)
	ERR(EMMINSUFMEM)
	ERR(CATASTROPHICTM)
	ERR(EXPRERROR)
	ERR(BADADDR)
	ERR(NODATAMEMBERS)
	ERR(PDBNOTFOUND)
	ERR(PDBBADSIGNATURE)
	ERR(PDBINVALIDAGE)
	ERR(PDBOLDFORMAT)
	ERR(SYMSCONVERTED)
	ERR(EXPORTSCONVERTED)
};

#define MSGCNT	(sizeof (mpErrToRes)/sizeof (CVERRTORES))

/** 	fetch_error - get text of error message
 *
 *		pstr = fetch_error (id);
 *
 *		Entry	id = number of error message
 *				pbuf = pointer to message buffer
 *
 */

void fetch_error (ushort wmsgid, char FAR *pbuf)
{
	int 	   		i;
	CString			str;

	i = 0;
	while( i < MSGCNT ) {
		// Found a match, get the string from the resources and exit
		if (mpErrToRes[ i ].errornum == wmsgid ) {
			if ( str.LoadString( mpErrToRes[ i ].idResource ) ) {
				_ftcscpy( pbuf, (const char FAR *)str );
				return;
			}
		}

		// Next
		++i;
	}

	// Didn't find it, or LoadString failed, get the unknown
	// error string.  Possible OOM (LoadString failed).  If
	// it did, it will probably fail again!
	ASSERT(FALSE);
	if ( str.LoadString( CVERR_UNKNOWNERROR ) ) {
		_ftcscpy( pbuf, (const char FAR *)str );
	}

	// Unable to load the string from the resources, just put
	// a real bad error here!  Don't know what else to put in
	// here!
	else {
		_ftcscpy( pbuf, "Internal error" );
	}
}
