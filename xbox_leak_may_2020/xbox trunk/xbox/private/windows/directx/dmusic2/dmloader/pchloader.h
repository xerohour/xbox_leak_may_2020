
#ifdef XBOX
#include <xtl.h>
#define COM_NO_WINDOWS_H
#define RPC_NO_WINDOWS_H
#define _WINGDI_
#include <mmsystem.h>
#undef timeGetTime 
#define timeGetTime GetTickCount
#include "..\shared\dmusiccp.h"
#include "..\shared\xsoundp.h" // For GUID_All_Objects
extern "C" ULONG _cdecl DbgPrint(PCH Format, ...);
#else // XBOX
#include <windows.h>
#include <objbase.h> // Need IClassFactory
#include <mmsystem.h>
#endif // XBOX
#include "..\shared\critsec.h"
#include "dmusicc.h" 
#include "dmusicip.h" 
#include "dmusicf.h" 
#include "validate.h"
#include "loader.h"
#include "container.h"
#include "debug.h"
#include "riff.h"
#include "dmscriptautguids.h"
#include "smartref.h"
#include "miscutil.h"
#ifdef UNDER_CE
#include "dragon.h"
#else
extern BOOL g_fIsUnicode;
#endif
#ifdef DXAPI
#include <regstr.h>
#include <share.h>
extern long g_cComponent;
#endif


