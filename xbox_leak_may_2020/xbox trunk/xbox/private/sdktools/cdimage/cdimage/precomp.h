// turn these on for lots of debug stuff

/*
#define DEBUG
#define _DEBUG
#define UDF_DBG
*/

#define NONAMELESSUNION

#pragma warning( disable: 4001 4035 4115 4200 4201 4204 4209 4214 4514 4699)

#if defined(_M_IX86)
    #if !defined(_X86_)
        #define _X86_
    #endif
#elif defined(_M_MRX000)
    #if !defined(_MIPS_)
        #define _MIPS_
    #endif
#elif defined(_M_ALPHA)
    #if !defined(_ALPHA_)
        #define _ALPHA_
    #endif
#elif defined(_M_PPC)
    #if !defined(_PPC_)
        #define _PPC_
    #endif
#else
    #error "unrecognized platform"
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif

#ifndef WIN32
#define WIN32 0x0400
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif

#include <excpt.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#include <windef.h>
#include <winbase.h>
#include <wincon.h>
#include <winnls.h>
#include <time.h>

#include <rpc.h>

#pragma warning( disable: 4001 4035 4115 4200 4201 4204 4209 4214 4514 4699 )

#ifdef _PPC_

    //
    //  Stupid Motorola PPC compiler complains about "local variable
    //  may be used without having been initialized" when it is simply
    //  not possible in the given code path.  For PPC, we'll disable
    //  this 4701 warning.
    //

    #pragma warning( disable: 4701 )

#endif // _PPC_

#ifndef try
    #define try     __try
    #define except  __except
    #define finally __finally
    #define leave   __leave
#endif

#ifndef strupr
    #define strupr _strupr
#endif

#ifndef stricmp
    #define stricmp _stricmp
#endif

#ifndef wcsnicmp
    #define wcsnicmp _wcsnicmp
#endif

#include "misc.h"
#include "..\crc\crc.h"
#include "..\md5\md5.h"
#include "..\rpcsig\rpcsigx.h"
#include "..\rpcsig\rpcsig.h"
#include "error.h"
#include "heap.h"
#include "buffer.h"
#include "thread.h"
#include "reader.h"
#include "writer.h"
#include "open.h"
#include "sign.h"
#include "ntcalls.h"
#include "iso13346.h"
#include "cdimage.h"
#include "udfbridg.h"

#pragma hdrstop
