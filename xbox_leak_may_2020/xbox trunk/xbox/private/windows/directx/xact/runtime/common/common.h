
#ifndef __COMMON_H__
#define __COMMON_H__

//
// Preprocessor definitions
//

#if DBG && !defined(DEBUG)
#define DEBUG
#endif

#if defined(DEBUG) && !defined(VALIDATE_PARAMETERS)
#define VALIDATE_PARAMETERS
#endif

//
// Public includes
//

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntos.h>
#include <pci.h>

#ifdef __cplusplus
}
#endif // __cplusplus

#define NODSOUND
#include <xtl.h>
#undef NODSOUND

#include <xboxp.h>
#include <dsoundp.h>

#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>

//
// Private includes
//

namespace XACT {

#include "macros.h"
#include "debug.h"
#include "drvhlp.h"
#include "ntlist.h"
#include "refcount.h"

}//namespace

#include "memmgr.h"

//
// New and delete overrides
//

#ifdef __cplusplus


static void *__cdecl operator new(size_t cbBuffer) 
{ 
    using namespace XACT;
    return XactMemAlloc(cbBuffer, FALSE); 
}

static void *__cdecl operator new[](size_t cbBuffer) 
{ 
    using namespace XACT;
    return XactMemAlloc(cbBuffer, FALSE);
}

#ifdef TRACK_MEMORY_USAGE

static void *__cdecl operator new(size_t cbBuffer, LPCSTR pszFile, ULONG nLine, LPCSTR pszClass)
{
    using namespace XACT;
    return XactTrackMemAlloc(pszFile, nLine, pszClass, cbBuffer, TRUE); 
}

static void *__cdecl operator new[](size_t cbBuffer, LPCSTR pszFile, ULONG nLine, LPCSTR pszClass)
{
    using namespace XACT;
    return XactTrackMemAlloc(pszFile, nLine, pszClass, cbBuffer, TRUE); 
}

#endif // TRACK_MEMORY_USAGE

static void __cdecl operator delete(void *pvBuffer) 
{ 
    using namespace XACT;
    XactMemFree(pvBuffer); 
}

static void __cdecl operator delete[](void *pvBuffer) 
{ 
    using namespace XACT;
    XactMemFree(pvBuffer); 
}

#ifdef TRACK_MEMORY_USAGE

#define NEW(type) \
    new(__FILE__, __LINE__, #type) type

#define NEW_A(type, count) \
    new(__FILE__, __LINE__, #type) type [count]

#else // TRACK_MEMORY_USAGE

#define NEW(type) \
    new type

#define NEW_A(type, count) \
    new type [count]

#endif // TRACK_MEMORY_USAGE

#undef DELETE
#define DELETE(p) \
    { \
        if(p) \
        { \
            delete (p); \
            (p) = NULL; \
        } \
    }

#define DELETE_A(p) \
    { \
        if(p) \
        { \
            delete [] (p); \
            (p) = NULL; \
        } \
    }


#endif // __cplusplus


#endif // __COMMON_H__
