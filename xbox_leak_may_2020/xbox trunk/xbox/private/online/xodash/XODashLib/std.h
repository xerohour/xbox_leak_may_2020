#ifdef _XBOX
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
#include <ntos.h>
#include <ntrtl.h>
#include <nturtl.h>
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
#include <dsoundp.h>
#include <xtl.h>
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

#define ASSERTHR(f)		do { HRESULT hrverify = (f); if (FAILED(hrverify) && AssertFailed(_T(__FILE__), __LINE__, hrverify)) _CrtDbgBreak(); } while (0)
#define VERIFYHR(f)		ASSERTHR(f)


EXTERN_C bool AssertFailed(const TCHAR* szFile, int nLine, HRESULT hr);
#define BREAKONFAIL(a,b)  {if(FAILED(a)) {DbgPrint(b);break;} }
#define BREAKONFAIL2(a,b,c)  {if(FAILED(a)) {DbgPrint(b,c);break;} }

