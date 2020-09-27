#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <share.h>
#include <io.h>
#include <dos.h>
#include <malloc.h>
#include <errno.h>
#include <windows.h>

#if !defined(UNALIGNED)
#if defined(_MIPS_) || defined(_ALPHA_)
#define UNALIGNED __unaligned
#else
#define UNALIGNED
#endif
#endif

#ifdef _MBCS
#include <mbstring.h>
#else // _MBCS
#include <string.h>
#endif // _MBCS

#include <tchar.h>

#include <memory.h>
#include <stdarg.h>
#include <fcntl.h>

#include "vcbudefs.h"
#include "pdb.h"
#include "types.h"
#include "cvtypes.h"
#include "cvinfo.h"
#include "cv.h"
//#include "cvexefmt.h"
#include "shapi.h"
#include "sapi.hxx"
#include "cvproto.hxx"
#include "shiproto.hxx"
#ifndef ACTIVEDBG
#include "shassert.h"
#else
#include "assert.h"
#endif

typedef REGREL32 *LPREGREL32;

#ifdef DEBUGVER
#undef LOCAL
#define LOCAL
#else
#define LOCAL static
#endif

//
// For IceCAP builds, make everything public.
//

#if PROFILE
	#ifdef LOCAL
		#undef LOCAL
	#endif
	#define LOCAL
#endif

#ifdef ACTIVEDBG
	#if DBG_API_VERSION !=7
	#error DBG_API_VERSION
	#error Building AD2 version with VC5/VC6 langapi
	#endif
#else // ACTIVEDBG
	#if DBG_API_VERSION !=6
	#error Building VC6 OSDebug version with VC7 langapi
	#endif
#endif
