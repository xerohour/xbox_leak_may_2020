/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) 2002 by Microsoft Corporation.

Module Name:

  debug.h

Abstract:

  This header contains useful debug macros and other aids.  Adapted from xn.h.

Author:

  David Hefner (2-7-2002)

Environment:

  XBox

Revision History:

------------------------------------------------------------------------------*/

#pragma once

#ifdef _XBOX

#ifndef __cplusplus
#error "debug.h must only be included by a C++ file"
#endif

// ---------------------------------------------------------------------------------------
// Definitions
// ---------------------------------------------------------------------------------------

#define INLINE __forceinline

#if DBG
#define IFDBG(x)    x
#define IFNDBG(x)
#else
#define IFDBG(x)
#define IFNDBG(x)   x
#endif

// ---------------------------------------------------------------------------------------
// Asserts
// ---------------------------------------------------------------------------------------

INLINE void DbgBreak()
{
    _asm int 3;
}

#ifdef XNET_FEATURE_ASSERT
#ifdef _XBOX
#define DbgAssert(expr,file,line,msg) RtlAssert((PVOID)((msg)?(msg):(expr)),file,line,NULL)
#else
int DbgAssertPop(const char * pszExpr, const char * pszFile, int iLine, const char * pszMsg);
#define DbgAssert(expr,file,line,msg) do { if (DbgAssertPop(expr,file,line,msg)) { DbgBreak(); } } while (0)
#endif
char * __cdecl DbgAssertFmt(char const * pszFmt, ...);
#define ASSERT_BEG(x)   do { if (!(x)) { char * __pch__ = DbgAssertFmt(
#define ASSERT_END(x)   ); DbgAssert(#x,__FILE__,__LINE__,__pch__); } } while (0)
#define Assert(x) \
        do { if (!(x)) { DbgAssert(#x,__FILE__,__LINE__,NULL); } } while (0)
#define AssertSz(x,sz) \
        ASSERT_BEG(x) sz ASSERT_END(x)
#define AssertSz1(x,sz,a1) \
        ASSERT_BEG(x) sz,a1 ASSERT_END(x)
#define AssertSz2(x,sz,a1,a2) \
        ASSERT_BEG(x) sz,a1,a2 ASSERT_END(x)
#define AssertSz3(x,sz,a1,a2,a3) \
        ASSERT_BEG(x) sz,a1,a2,a3 ASSERT_END(x)
#define AssertSz4(x,sz,a1,a2,a3,a4) \
        ASSERT_BEG(x) sz,a1,a2,a3,a4 ASSERT_END(x)
#define AssertSz5(x,sz,a1,a2,a3,a4,a5) \
        ASSERT_BEG(x) sz,a1,a2,a3,a4,a5 ASSERT_END(x)
#define AssertSz6(x,sz,a1,a2,a3,a4,a5,a6) \
        ASSERT_BEG(x) sz,a1,a2,a3,a4,a5,a6 ASSERT_END(x)
#define AssertSz7(x,sz,a1,a2,a3,a4,a5,a6,a7) \
        ASSERT_BEG(x) sz,a1,a2,a3,a4,a5,a6,a7 ASSERT_END(x)
#define AssertSz8(x,sz,a1,a2,a3,a4,a5,a6,a7,a8) \
        ASSERT_BEG(x) sz,a1,a2,a3,a4,a5,a6,a7,a8 ASSERT_END(x)
#define AssertSz9(x,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9) \
        ASSERT_BEG(x) sz,a1,a2,a3,a4,a5,a6,a7,a8,a9 ASSERT_END(x)
#define AssertList(ple) \
        do { const char * __psz__ = DbgVerifyList(ple,NULL); \
        if (__psz__) { DbgAssert("AssertList("#ple")",__FILE__,__LINE__,__psz__); } } while (0)
#define AssertListEntry(ple,pleReq) \
        do { const char * __psz__ = DbgVerifyList(ple,pleReq); \
        if (__psz__) { DbgAssert("AssertListEntry("#ple","#pleReq")",__FILE__,__LINE__,__psz__); } } while (0)
#else
#define Assert(x)
#define AssertSz(x,sz)
#define AssertSz1(x,sz,a1)
#define AssertSz2(x,sz,a1,a2)
#define AssertSz3(x,sz,a1,a2,a3)
#define AssertSz4(x,sz,a1,a2,a3,a4)
#define AssertSz5(x,sz,a1,a2,a3,a4,a5)
#define AssertSz6(x,sz,a1,a2,a3,a4,a5,a6)
#define AssertSz7(x,sz,a1,a2,a3,a4,a5,a6,a7)
#define AssertSz8(x,sz,a1,a2,a3,a4,a5,a6,a7,a8)
#define AssertSz9(x,sz, a1,a2,a3,a4,a5,a6,a7,a8,a9)
#define AssertList(ple)
#define AssertListEntry(ple,pleReq)
#endif

#undef  ASSERT
#define ASSERT(x)   Assert(x)

// ---------------------------------------------------------------------------------------
// Traces
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_TRACE
void __cdecl DbgTrace(const char * szTag, const char * szFmt, ...);
#ifdef XNET_FEATURE_XBDM_SERVER
#define Tag(t)  m_##t
#else
#define Tag(t)  t_##t
#endif
#if defined(_XBOX) || !defined(__cplusplus)
#define TAGINIT(t)
#else
class CTagInit { public: CTagInit(int * pi, const char * pszTag); int i; };
#define TAGINIT(t)          ; CTagInit __init__##t(&Tag(t),#t)
#endif
#define TAG_ENABLE      1
#define TAG_BREAK       2
#define DefineTag(t,i)      int Tag(t) = i TAGINIT(t)
#define ExternTag(t)        extern int Tag(t)
#define TRACE_BEG(t)        do { if (Tag(t) > 0) { DbgTrace(#t,
#define TRACE_END(t)        ); if (Tag(t) > 1) DbgBreak(); } } while (0)
#define TraceSz(t,sz) \
        TRACE_BEG(t) sz TRACE_END(t)
#define TraceSz1(t,sz,a1) \
        TRACE_BEG(t) sz,a1 TRACE_END(t)
#define TraceSz2(t,sz,a1,a2) \
        TRACE_BEG(t) sz,a1,a2 TRACE_END(t)
#define TraceSz3(t,sz,a1,a2,a3) \
        TRACE_BEG(t) sz,a1,a2,a3 TRACE_END(t)
#define TraceSz4(t,sz,a1,a2,a3,a4) \
        TRACE_BEG(t) sz,a1,a2,a3,a4 TRACE_END(t)
#define TraceSz5(t,sz,a1,a2,a3,a4,a5) \
        TRACE_BEG(t) sz,a1,a2,a3,a4,a5 TRACE_END(t)
#define TraceSz6(t,sz,a1,a2,a3,a4,a5,a6) \
        TRACE_BEG(t) sz,a1,a2,a3,a4,a5,a6 TRACE_END(t)
#define TraceSz7(t,sz,a1,a2,a3,a4,a5,a6,a7) \
        TRACE_BEG(t) sz,a1,a2,a3,a4,a5,a6,a7 TRACE_END(t)
#define TraceSz8(t,sz,a1,a2,a3,a4,a5,a6,a7,a8) \
        TRACE_BEG(t) sz,a1,a2,a3,a4,a5,a6,a7,a8 TRACE_END(t)
#define TraceSz9(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9) \
        TRACE_BEG(t) sz,a1,a2,a3,a4,a5,a6,a7,a8,a9 TRACE_END(t)
#define TraceSz10(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10) \
        TRACE_BEG(t) sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10 TRACE_END(t)
#define TraceSz11(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11) \
        TRACE_BEG(t) sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11 TRACE_END(t)
#define TraceSz12(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12) \
        TRACE_BEG(t) sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12 TRACE_END(t)
#define TraceSz13(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13) \
        TRACE_BEG(t) sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13 TRACE_END(t)
#define TraceSz14(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14) \
        TRACE_BEG(t) sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14 TRACE_END(t)
#define TraceSz15(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15) \
        TRACE_BEG(t) sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15 TRACE_END(t)
#define TraceSz16(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16) \
        TRACE_BEG(t) sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16 TRACE_END(t)
#else
#define DefineTag(t,i)
#define ExternTag(t)
#define TraceSz(t,sz)
#define TraceSz1(t,sz,a1)
#define TraceSz2(t,sz,a1,a2)
#define TraceSz3(t,sz,a1,a2,a3)
#define TraceSz4(t,sz,a1,a2,a3,a4)
#define TraceSz5(t,sz,a1,a2,a3,a4,a5)
#define TraceSz6(t,sz,a1,a2,a3,a4,a5,a6)
#define TraceSz7(t,sz,a1,a2,a3,a4,a5,a6,a7)
#define TraceSz8(t,sz,a1,a2,a3,a4,a5,a6,a7,a8)
#define TraceSz9(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9)
#define TraceSz10(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10)
#define TraceSz11(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11)
#define TraceSz12(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12)
#define TraceSz13(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13)
#define TraceSz14(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14)
#define TraceSz15(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15)
#define TraceSz16(t,sz,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16)
#endif

ExternTag(Warning);
ExternTag(Verbose);

// ---------------------------------------------------------------------------------------
// Rip
// ---------------------------------------------------------------------------------------

#ifdef _XBOX
#define Rip(sz) RIP(sz)
#elif defined(XNET_FEATURE_ASSERT)
#define Rip(sz) AssertSz1(0, "RIP: %s", sz)
#else
#define Rip(sz) { DbgPrint("%s", sz); DbgBreak(); }
#endif


// ---------------------------------------------------------------------------------------
// Utilities
// ---------------------------------------------------------------------------------------

char * HexStr(const void * pv, size_t cb);

#define HTONL_(ul) \
    ((ULONG)((((ul) >> 24) & 0x000000FFL) | (((ul) >>  8) & 0x0000FF00L) | \
             (((ul) <<  8) & 0x00FF0000L) | (((ul) << 24))))

#define NTOHL_(ul) HTONL_(ul)

#define HTONS_(us) \
    ((USHORT)((((us) >> 8) & 0x00FF) | (((us) << 8) & 0xFF00)))

#define NTOHS_(us) HTONS_(us)

INLINE ULONG  HTONL(ULONG  ul) { return(HTONL_(ul)); }
INLINE ULONG  NTOHL(ULONG  ul) { return(NTOHL_(ul)); }
INLINE USHORT HTONS(USHORT us) { return(HTONS_(us)); }
INLINE USHORT NTOHS(USHORT us) { return(NTOHS_(us)); }

// ---------------------------------------------------------------------------------------
// Globals
// ---------------------------------------------------------------------------------------

#define CBDHG1 96
extern "C" const BYTE g_abOakleyGroup1Mod[CBDHG1];
extern "C" const BYTE g_abOakleyGroup1Base[CBDHG1];



#endif // _XBOX


