#ifdef _XBOX


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <stddef.h>

#include <ntos.h>

#include <ntdddisk.h>
#include <ntddcdrm.h>
#include <ntddscsi.h>
#include <ntddcdvd.h>
#include <smcdef.h>
#include <scsi.h>
#include <init.h>
#ifdef __cplusplus
}
#endif // __cplusplus
#define  NODSOUND
#include <xtl.h>
#include <dsound.h>

#include <xgraphics.h>
#include <xboxp.h>
#include <xapip.h>
#include <av.h>
#include "xonlinep.h"


#ifndef D3DLOCK_DISCARD
#define D3DLOCK_DISCARD 0
#endif

#endif

#ifdef _WINDOWS
#include <winsock2.h>
#include <windows.h>
#endif

#include <tchar.h>
typedef TCHAR* PTCHAR;

// "Filesystem" Character Type...What do *most* of the file system API's use...
#if defined(_XBOX)
typedef char FSCHAR;
#define _FS(s) s
#else
typedef TCHAR FSCHAR;
#define _FS(s) _T(s)
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include <crtdbg.h>

#if !defined(_NOD3D)
#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>

#endif


////////////////////////////////////////////////////////////////////////////

#ifdef  __cplusplus

#define EXTERN_C extern "C"


#define RELEASENULL(object) { if ((object) != NULL) { (object)->Release(); (object) = NULL; } }

#else // !__cplusplus

#define EXTERN_C extern
#define bool BOOL
#define true TRUE
#define false FALSE
#define inline _inline

#endif

#define CopyChars(dest, src, count) CopyMemory(dest, src, (count) * sizeof (TCHAR))
#define countof(n) (sizeof (n) / sizeof (n[0]))


#ifdef _UNICODE
extern void Unicode(TCHAR* wsz, const char* sz, int nMaxChars);
extern void Ansi(char* sz, const TCHAR* wsz, int nMaxChars);
#endif


#undef VERIFY


#undef ALERT

#ifdef _DEBUG

// #define ASSERT(f)		if (!(f)) RtlAssert(#f, __FILE__, __LINE__, NULL)
#define VERIFY(f)		ASSERT(f)


#define ALERT			Alert

EXTERN_C bool AssertFailed(const TCHAR* szFile, int nLine, HRESULT hr);


#else // !_DEBUG

// #define ASSERT(f)		((void)0)
#define VERIFY(f)		((void)(f))

#define ALERT			1 ? (void)0 : Alert



#endif

EXTERN_C void Alert(const TCHAR* szMsg, ...);


////////////////////////////////////////////////////////////////////////////

#ifdef _XBOX
#include "xprofp.h"

#ifdef _PROFILE
#define START_PROFILE() XProfpControl(XPROF_START, 0)
#define END_PROFILE() XProfpControl(XPROF_STOP, 0)
#else
#define START_PROFILE()
#define END_PROFILE()
#endif
#else
#define START_PROFILE()
#define END_PROFILE()
#endif

////////////////////////////////////////////////////////////////////////////
// Stuff from Windows that Xbox should have...
//
/*
#ifdef _XBOX
typedef  PVOID           HDEVNOTIFY;

typedef struct tagRGBQUAD {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD;
typedef RGBQUAD FAR* LPRGBQUAD;

#define MAX_COMPUTERNAME_LENGTH 15 // REVIEW: What is this really?
#endif
*/