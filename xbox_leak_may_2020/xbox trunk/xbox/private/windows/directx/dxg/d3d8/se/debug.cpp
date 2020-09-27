/*==========================================================================
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       debug.cpp
 *  Content:    implementation for debug stuff.
 *
 ***************************************************************************/

#include "precomp.hpp"

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

#if DBG

VOID DXGRIP(PCHAR Format, ...)
{
    CHAR string[MAX_PATH];
    va_list arglist;

    va_start(arglist, Format);
    _vsnprintf(string, sizeof(string), Format, arglist);
    va_end(arglist);

#ifdef STARTUPANIMATION
    DbgPrint(string);
    __asm int 3;
#else
    XDebugError("D3D", string);
#endif
}

VOID WARNING(PCHAR Format, ...)
{
    CHAR string[MAX_PATH];
    va_list arglist;

    va_start(arglist, Format);
    _vsnprintf(string, sizeof(string), Format, arglist);
    va_end(arglist);

#ifdef STARTUPANIMATION
    DbgPrint(string);
#else
    XDebugWarning("D3D", string);
#endif
}

VOID ASSERTMSG(BOOL cond, PCHAR Format, ...)
{
    CHAR string[MAX_PATH];
    va_list arglist;

    if (cond)
    {
        return;
    }

    va_start(arglist, Format);
    _vsnprintf(string, sizeof(string), Format, arglist);
    va_end(arglist);

#ifdef STARTUPANIMATION
    DbgPrint(string);
    __asm int 3;
#else
    XDebugError("D3D", string);
#endif
}

#endif

} // end namespace
