/*==========================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       debug.cpp
 *  Content:    implementation for debug stuff.
 *
 ***************************************************************************/

#if XBOX
#include "xgrphseg.h"
#include "xtl.h"
#else
#include "windows.h"
#include "d3d8-xbox.h"
#endif

#include "stdio.h"
#include "xgraphics.h"
#include "debug.h"

namespace XGRAPHICS
{

#if DBG

VOID DXGRIP(PCHAR Format, ...)
{
    CHAR string[MAX_PATH];
    va_list arglist;

    va_start(arglist, Format);
    _vsnprintf(string, sizeof(string), Format, arglist);
    va_end(arglist);

#if XBOX
    XDebugError("D3D", string);
#else
    OutputDebugStringA(string);
    __asm int 3;
#endif
}

void DPF3(const char* message, va_list list)
{
    char buf[500];
    _vsnprintf(buf, sizeof(buf)-1, message, list);
    buf[sizeof(buf)-1] = '\0';
    OutputDebugStringA(buf);
    Sleep(1);
}

void DPF2(const char* message,...)
{
	va_list list;
	va_start(list, message );
	DPF3(message, list);
	va_end(list);
}

void DPF(int level,const char* message,...)
{
    va_list list;
    va_start(list, message );
    DPF3(message, list);
    va_end(list);
}

#endif

} // end namespace
