/*
 *  CVMISC.C -- used to be UTILS.C and CVWIN32.C
 *
 *
 */
#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/*
 *  ERROR_STRING
 *
 *  returns an error string corresponding to errno
 */

static  BOOL  fAssertFirstTime = TRUE;
char errorstring[MAXERRMSG];

char *error_string(ushort errnum)
{
	fetch_error (errnum, (char FAR *)&errorstring);
	return (errorstring);
}

/*
 *  GET_STRING
 *
 *  Fetches the rest of the command pointed to by ptxt.
 */

char getstring[ MAXINPLINE ];

char *get_string()
{
	register char *sz = getstring;
	int cLevel = 0;
	int fQuote = FALSE;

	skip_white();
	while ( *ptxt != '\0' &&
		( fQuote || cLevel ||
			*ptxt != ';' && *ptxt != ',' && !_istspace( (_TUCHAR)*ptxt ) ) ) {

		fQuote ^= *ptxt == '"';

		if ( *ptxt == '(' || *ptxt == '{' || *ptxt == '[' ) {
			++cLevel;
		}
		else if ( *ptxt == ')' || *ptxt == '}' || *ptxt == ']' ) {
			--cLevel;
		}

		// If we're not quoted, remove spaces!

		if ((cLevel != 0) || !_istspace ((_TUCHAR)*ptxt) || fQuote) {
			*sz++ = *ptxt++;
		}
		else {
			++ptxt;
		}
	}
	*sz = '\0';
	return getstring;
}

/*
 *  EOL
 *
 *  Return TRUE if at end of command
 */
eol()
{
	skip_white();
	return(*ptxt == ';' || *ptxt == '\0');
}

/*
 *  SKIP_WHITE
 *
 *  bump text pointer past whitespace
 */

void skip_white(void)
{
	while (_istspace((_TUCHAR)*ptxt))
		ptxt++;
}

/*
 *  SKIP_NOT_WHITE
 *
 *  Bump text pointer past any non-white characters
 */

void skip_not_white(void)
{
	while (!_istspace((_TUCHAR)*ptxt) && (*ptxt != '\0') && (*ptxt != ';'))
		ptxt++;
}

/*
 *  SKIP_TO_EOL
 *
 *  Skip to end of line (or command)
 */

void skip_to_eol(void)
{
	while(TRUE) {

		while (*ptxt != ';' && *ptxt != '\0' && *ptxt != '"')
			ptxt++;

		if( *ptxt != '"' )
			break;

		ptxt++;
		// we are inside a quoted string
		while (*ptxt != '\0' && *ptxt != '"')
			ptxt++;

		if( *ptxt == '\0' )
			break;

		ptxt++;
	}

}

/*
 *  RESET_PTXT()
 *
 *  Reset the text pointer to standard null line
 */

void reset_ptxt(void)
{
		ptxt = input_text;
		input_text[0] = '\0';
}

/*
 *  SplitPath
 *
 *  Custom split path that allows parameters to be null
 *
 */

void SplitPath(
char *	szPath,
char *	szDrive,
char *	szDir,
char *	szName,
char *	szExt)
{
	static char rgchDrive[_MAX_CVDRIVE];
	static char rgchDir[_MAX_CVDIR];
	static char rgchName[_MAX_CVFNAME];
	static char rgchExt[_MAX_CVEXT];

	if (szDrive == NULL)
		szDrive = rgchDrive;
	if (szDir == NULL)
		szDir = rgchDir;
	if (szName == NULL)
		szName = rgchName;
	if (szExt == NULL)
		szExt = rgchExt;

	_splitpath(szPath,szDrive,szDir,szName,szExt);
}

int OSDAPI dsquit(
#ifdef OSDEBUG4
DWORD u
#else
unsigned u
#endif
)
{

	quit(u);
	return TRUE;
}

int LOADDS ds_eprintfT(const char * /*szFormat*/,
						char * /*szAssert*/,
						char *szFile,
						int  lnNbr) {

#ifdef _DEBUG
	if (AfxAssertFailedLine(szFile, lnNbr))
		AfxDebugBreak ();
#endif
	return 1;
}

int LOADDS ds_eprintf(const char FAR *f,
				 char FAR *str1,
				 char FAR *str2,
				 int     lnNbr) {
	char  szFormat[256], szAssert[256], szFile[256];

	// copy the strings near
	_ftcscpy(szFormat, f);
	_ftcscpy(szAssert, str1);
	_ftcscpy(szFile, str2);

	return ds_eprintfT(szFormat,szAssert,szFile,lnNbr);

}

int LOADDS ds_sprintf(char NEAR *s, const char FAR *f, ...) {
	char  szFormat[256];
	va_list lArg;

	_ftcscpy(szFormat, f);

	va_start(lArg, f);

		return ( vsprintf ( s, szFormat, lArg ) );
}

char * LOADDS ds_ultoa ( unsigned long value, char *string, int radix ) {
		return _ultoa ( value, string, radix );
}

char * LOADDS ds_itoa ( int value, char *string, int radix ) {
		return _itoa ( value, string, radix );
}

char * LOADDS ds_ltoa ( long value, char *string, int radix ) {
		return _ltoa ( value, string, radix );
}

#if defined (HOST32)
// no long double support
FLOAT10 __inline	Float10FromDouble ( double d ) {
	FLOAT10	f10Ret;
#if defined (_X86_)
	__asm {
		fld		qword ptr d
		fstp	tbyte ptr f10Ret
		fwait
		}
#else
    ASSERT(FALSE);
	memset(&f10Ret, 0, sizeof(f10Ret));
#endif
	return f10Ret;
	}
#else
// FLOAT10 is long double
FLOAT10 __inline	Float10FromDouble ( double d ) {
	return (FLOAT10) d;
	}

#endif

#if defined (HOST32)
extern "C" unsigned __cdecl
__STRINGTOLD (
	FLOAT10 *,
	const char **	ppchEnd,
	const char *	szFloat,
	int				fMultIn12
	);
#endif

FLOAT10 LOADDS ds_strtold ( const char *nptr, char **endptr ) {
#if defined (HOST32) && defined (_X86_)

	FLOAT10 f10;

	memset ( &f10, 0, sizeof(FLOAT10) );
	__STRINGTOLD ( &f10, (const char **)endptr, (const char *)nptr, 1 );
	return f10;

#else
#if defined (_MIPS_) || defined (_ALPHA_)
   FLOAT10 f10;

   ASSERT(FALSE);
   memset ( &f10, 0, sizeof(FLOAT10) );
   return(f10);
#else
	return ( _strtold ( nptr, endptr ) );
#endif
#endif
}

/******************************************************************************
*
*   _fswap(void FAR *lpv1, void FAR * lpv2, size_t cb)
*
*******************************************************************************
*
* PURPOSE:  Copy the contents of two non ovelapping areas of
*       memory one to another.
*
* PARAMETERS: lpv1  - Points to the first area of memory
*       lpv2  - Points to the second area of memory
*       cb    - The number of bytes in each area
*
* NOTES:    This routine allocates a temporary buffer on the
*       stack. If cb is large take care that you have
*       enough stack!
*
******************************************************************************/
void _fswap(void FAR *lpv1, void FAR * lpv2, size_t cb)
{
	void FAR *	lpvTemp;

	// allocate temporary buffer of cb bytes on the stack
	lpvTemp = _alloca(cb);

	if (lpvTemp != NULL)
	{
		// copy cb bytes from *lpv1 to *lpvTemp
		memcpy(lpvTemp, lpv1, cb);

		// copy cb bytes from *lpv2 to *lpv1
		memcpy(lpv1, lpv2, cb);

		// copy cb bytes from *lpvTemp to *lpv2
		memcpy(lpv2, lpvTemp, cb);
	}

	// buffer goes away on return
}

/******************************************************************************
*
* _fqsort(void FAR *lpvBase, size_t num, size_t width, int (*compare)
*     (const void FAR *, const void FAR *))
*
*
*******************************************************************************
*
* PURPOSE:  Like qsort but opertes on FAR data
*
* PARAMETERS: lpvBase - Points to the first element of an array to be sorted
*       num   - The number of elements in the array
*       width - The size of each element in bytes
*
* NOTES:    This routine uses _fswap which allocates a temporary buffer
*       on the stack. If width is large take care that you have
*       enough stack! _fqsort does not check there is enough stack.
*
******************************************************************************/
void _fqsort(void FAR *lpvBase, size_t num, size_t width, int (*compare)
	(const void FAR *, const void FAR *))
{
	if(num < 2) return;
	else {
		char FAR *lpb = (char FAR *)lpvBase + width * (num >> 1);
		char FAR *lpb1;
		char FAR *lpb2;

		if(2 == num) {
			if( 0 >= compare ( lpvBase , (void FAR *) lpb ) ) return;

// Swap the two items
			_fswap(lpvBase,lpb,width);

			return;
		}

		else {
			size_t numT;
			UINT   dbMinLim = width * (num - 1);

			lpb1 = (char FAR *)lpvBase;
			lpb2 = lpb1 + dbMinLim;

			while(TRUE) {
				while( (lpb1 < lpb2) &&
						 (0 >= compare((void FAR *)lpb1, (void FAR *) lpb) ) ) lpb1 += width;

				while( (lpb1 < lpb2) &&
						 (0 >= compare((void FAR *)lpb, (void FAR *) lpb2) ) ) lpb2 -= width;

				if(lpb1 >= lpb2) {
					if(lpb1 == (char FAR *)lpvBase) {
						_fswap(lpb1,lpb,width);
						lpb1 += width;
						break;
					}
					if(0 > compare((void FAR *)lpb1, (void FAR *) lpb) ) {
						if((UINT)(lpb1 - (char FAR *)lpvBase) < dbMinLim) {
							lpb1 += width;
						}
						else _fswap(lpb1,lpb,width);
					}
					break;
				}
				_fswap(lpb1,lpb2,width);
				lpb1 += width;
				if(lpb1 >= lpb2) break;
				lpb2 -= width;
			}

			_fqsort(lpvBase,numT = (lpb1 - (char FAR *)lpvBase) / width,width,compare);
			_fqsort((void FAR *) lpb1,num - numT,width,compare);
		}
	}
}


void FAR LOADDS PASCAL CVSplitPath(
LSZ  lsz1,
LSZ  lsz2,
LSZ  lsz3,
LSZ  lsz4,
LSZ  lsz5 ) {

    char    sz1[ _MAX_PATH ];
    char    sz2[ _MAX_DRIVE ];
    char    sz3[ _MAX_DIR ];
    char    sz4[ _MAX_FNAME];
    char    sz5[ _MAX_EXT];
    _ftcscpy( sz1, lsz1 );
    _splitpath( sz1, sz2, sz3, sz4, sz5 );
    // clear errno so we don't report dubious
    // errors especially in remote debugging
    // when we stat the dlls on mod loads.
    errno = 0;

    if ( lsz2 != NULL ) {
        _ftcsncpy( lsz2, sz2, _MAX_CVDRIVE );
        lsz2[ _MAX_CVDRIVE - 1 ] = '\0';
    }
    if ( lsz3 != NULL ) {
        _ftcsncpy( lsz3, sz3, _MAX_CVDIR );
        lsz3[ _MAX_CVDIR - 1 ] = '\0';
    }
    if ( lsz4 != NULL ) {
        _ftcsncpy( lsz4, sz4, _MAX_CVFNAME );
        lsz4[ _MAX_CVFNAME - 1 ] = '\0';
    }
    if ( lsz5 != NULL ) {
        _ftcsncpy( lsz5, sz5, _MAX_CVEXT );
        lsz5[ _MAX_CVEXT - 1 ] = '\0';
    }
}

void FAR LOADDS PASCAL CVSearchEnv(
LSZ  lszFile,
LSZ  lszVar,
LSZ  lszPath ) {
    char    szFile[ 256 ];
    char    szVar[ 256 ];
    char    szPath[ 256 ];

    _ftcscpy( szFile, lszFile );
    _ftcscpy( szVar, lszVar );
    _searchenv( szFile, szVar, szPath );
    _ftcscpy( lszPath, szPath );
}


LSZ FAR LOADDS PASCAL CVFullPath(
LSZ  lszBuf,
LSZ  lszRel,
UINT cbBuf ) {
    static char szBuf[500];
    static char szRel[500];

    _ftcscpy( szRel, lszRel );
    if ( _fullpath( szBuf, szRel, (size_t)cbBuf ) ) {
        _ftcscpy( lszBuf, szBuf );
    }
    else {
        lszBuf = (LSZ)NULL;
    }

    return lszBuf;
}

void FAR LOADDS PASCAL CVMakePath(
LSZ  lszPath,
LSZ  lszDrive,
LSZ  lszDir,
LSZ  lszFName,
LSZ  lszExt ) {
    char    szPath[500];
    char    szDrive[256];
    char    szDir[256];
    char    szFName[256];
    char    szExt[256];

    if ( lszDrive ) {
        _ftcscpy( szDrive, lszDrive );
    }
    else {
        *szDrive = '\0';
    }
    if ( lszDir ) {
        _ftcscpy( szDir, lszDir );
    }
    else {
        *szDir = '\0';
    }
    if ( lszFName ) {
        _ftcscpy( szFName, lszFName );
    }
    else {
        *szFName = '\0';
    }
    if ( lszExt ) {
        _ftcscpy( szExt, lszExt );
    }
    else {
        *szExt = '\0';
    }

    _makepath( szPath, szDrive, szDir, szFName, szExt );
    // clear errno so we don't report dubious
    // errors especially in remote debugging
    // when we stat the dlls on mod loads.
    errno = 0;
    _ftcscpy( lszPath, szPath );
}

UINT FAR LOADDS PASCAL LOADDS CVStat(
LSZ  lsz,
LPCH lpstat ) {
	char			sz[ 256 ];
	struct _stat	statT;
	WORD			wRet;

    _ftcscpy( sz, lsz );
	wRet = _stat( sz, &statT );
	*(struct _stat FAR *)lpstat = statT;
    // clear errno so we don't report dubious
    // errors especially in remote debugging
    // when we stat the dlls on mod loads.
    errno = 0;
    return wRet;
}

UINT FAR CDECL LOADDS CVsprintf(
LSZ  lszBuf,
LSZ  lszFmt,
... ) {
    va_list val;
    WORD    wRet;
    char    szBuf[256];
    char    szFmt[256];

    _ftcscpy( szFmt, lszFmt );

    va_start( val, lszFmt );
    wRet = (WORD)vsprintf( szBuf, szFmt, val );
    va_end( val );

    _ftcscpy( lszBuf, szBuf );
    return wRet;
}


//-----------------------------------------------------------------------------
//	cvwin32.c
//
//  Copyright (C) 1993, Microsoft Corporation
//
//  Purpose:
//		do stuff that can't be done in bm.c due to collisions
//		in types and such with windows.h
//
//  Functions/Methods present:
//
//  Revision History:
//
//	[]		05-Mar-1993 Dans	Created
//
//-----------------------------------------------------------------------------

#if defined (_WIN32)	/* { the whole file */

#if !((defined (NO_CRITSEC)))	/* { */

typedef CRITICAL_SECTION	CS, *_PCS;


CS	rgcsCv[icsMax];

#pragma warning(disable:4124)
void FASTCALL CVInitCritSection(ICS ics) {
	InitializeCriticalSection ( &rgcsCv[ics] );
	}

void FASTCALL CVEnterCritSection(ICS ics) {
	EnterCriticalSection ( &rgcsCv[ics] );
	}

void FASTCALL CVLeaveCritSection(ICS ics) {
	LeaveCriticalSection ( &rgcsCv[ics] );
	}

void FASTCALL CVDeleteCritSection(ICS ics) {
	DeleteCriticalSection ( &rgcsCv[ics] );
	}

PCS FASTCALL		PcsAllocInit() {
        // We used to turn off memory tracking here,
        // but tests with memory tracking on here pass fine,
        // so I turned it off.  If you find leaks here that
        // you don't think should be fixed (weird), uncomment
        // these lines.

	// BOOL bEnable = AfxEnableMemoryTracking(FALSE);
	_PCS pcs = (_PCS)calloc ( sizeof(CRITICAL_SECTION), 1 );
	// AfxEnableMemoryTracking(bEnable);

	if ( pcs )
		InitializeCriticalSection ( pcs );
	return pcs;
	}
void FASTCALL		FreePcs ( PCS pcs ) {
	DeleteCriticalSection ( (_PCS)pcs );
	free ( pcs );
	}

// Moved to inline code
/*
void FASTCALL		AcquireLockPcs ( PCS pcs ) {
	EnterCriticalSection ( (_PCS)pcs );
	}

void FASTCALL		ReleaseLockPcs ( PCS pcs ) {
	LeaveCriticalSection ( (_PCS)pcs );
	}
*/

#pragma warning(default:4124)

#endif	/* } NO_CRITSEC */

#endif	/* } the whole file */
