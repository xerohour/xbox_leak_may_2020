// ---------------------------------------------------------------------------------------
// xn.h
//
// Exported shared definitions for use by xnet.lib and xonline.lib
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#ifndef __XN_H__
#define __XN_H__

#ifndef __cplusplus
#error "xn.h must only be included by a C++ file"
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
// Leak Tracker
// ---------------------------------------------------------------------------------------

typedef const char * (*PFNLEAKTAG)(ULONG tag);

struct CLeakInfo
{
    PFNLEAKTAG      _pfnLeakTag;
    BOOL            _fLeakDisable;
    UINT            _cLeakAlloc;
    UINT            _cLeak;
    struct CLeak *  _pLeak;
};

// ---------------------------------------------------------------------------------------
// VMem (Windows)
// ---------------------------------------------------------------------------------------

#ifdef XNET_FEATURE_VMEM

struct VMEMINFO
{
    size_t      cb;
    DWORD       dwFlags;
    size_t      cbFill1;
    size_t      cbFill2;
    void *      pv;
};

#define     VMEM_CHECKED            0x00000001
#define     VMEM_ENABLED            0x00000002
#define     VMEM_BACKSIDESTRICT     0x00000004
#define     VMEM_BACKSIDEALIGN8     0x00000008

BOOL        VMemIsEnabled();
void *      VMemAlloc(size_t cb);
void        VMemFree(void * pv);

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

struct XOKERBINFO;

#define CBEVENTTIMER    (5 * sizeof(DWORD)) // sizeof(CEventTimer)

// ---------------------------------------------------------------------------------------
// Windows Support
// ---------------------------------------------------------------------------------------

#ifndef _XBOX
    typedef STRING OBJECT_STRING;
    #define RtlInitObjectString RtlInitAnsiString
    #define __OTEXT(quote) quote
    #define OTEXT(quote) __OTEXT(quote)
    #undef InitializeObjectAttributes
    #define InitializeObjectAttributes( p, n, a, r, s ) { (p)->RootDirectory = (r); (p)->Attributes = (a); (p)->ObjectName = (PUNICODE_STRING)(n); }
    #define XBOX_KEY_LENGTH 16
    #define XBOXAPI
    typedef struct _XINPUT_STATE * PXINPUT_STATE;
#endif

#include <xconfig.h>
#include <xonlinep.h>

// ---------------------------------------------------------------------------------------
// Online Support
// ---------------------------------------------------------------------------------------

#if defined(XNET_FEATURE_SG) || defined(XNET_FEATURE_ONLINE)

struct XOUSERINFO
{
    ULONGLONG       _qwUserId;              // User Id
    DWORD           _dwQFlags;              // Queue flags
    DWORD           _dwSeqQFlags;           // Sequence number of queue flags
    DWORD           _dwPState;              // Presence state
    XNKID           _xnkid;                 // Game session user is currently playing
    UINT            _cbData;                // Count of bytes of title data
    BYTE            _abData[MAX_TITLE_STATE_BYTES]; // Custom presence title data
};

#define XN_LOGON_STATE_IDLE         0x0000  // XnLogon not called yet
#define XN_LOGON_STATE_PENDING      0x0001  // XnLogon connection to SG is in progress
#define XN_LOGON_STATE_ONLINE       0x0002  // XnLogon connection to SG is successful
#define XN_LOGON_STATE_OFFLINE      0x0003  // XnLogon connection to SG has been lost

#endif

#ifdef XNET_FEATURE_ONLINE

struct XOKERBINFO
{
    BYTE            _abDhX[CBDHG1];         // Diffie-hellman X
    BYTE            _abDhGX[CBDHG1];        // Diffie-hellman g^X
    BYTE            _abKey[16];             // Session key from ticket
    DWORD           _dwTicketId;            // Ticket identifier
    UINT            _cbApReqMax;            // Maximum size of AP request
    DWORD           _dwUserPerm;            // User permutation and guest bits
};

class CXoBase : public CXNet
{

public:

    // Definitions -----------------------------------------------------------------------

    #define SO_INSECURE                 0x4001  // Special socket option to set socket insecure

    // XNet Support for XOnline ----------------------------------------------------------

    void            XnSetXoBase(CXoBase * pXoBase);
    INLINE void     XnSetInsecure(SOCKET s) { setsockopt(s, SOL_SOCKET, SO_INSECURE, (char *)this, sizeof(CXoBase *)); }
    void            XnLogon(IN_ADDR inaLogon, ULONGLONG * pqwUserId, WSAEVENT hEventLogon);
    DWORD           XnLogonGetStatus(SGADDR * psgaddr);
    BOOL            XnLogonGetQFlags(UINT iUserId, ULONGLONG * pqwUserId, DWORD * pdwQFlags, DWORD * pdwSeqQFlags);
    BOOL            XnLogonSetQFlags(UINT iUserId, DWORD dwQFlags, DWORD dwSeqQFlags);
    BOOL            XnLogonSetPState(UINT iUserId, DWORD dwPState, const XNKID * pxnkid, UINT cbData, BYTE * pbData);
    void            XnLogoff();

    void *          XnLeakAdd(CLeakInfo * pli, void * pv, UINT cb, ULONG tag);
    void *          XnLeakDel(CLeakInfo * pli, void * pv);
    void            XnLeakTerm(CLeakInfo * pli);

    void            XnSetEventTimer(BYTE * pbEventTimer, WSAEVENT hEvent, DWORD dwTimeout);
    void            XnRaiseToDpc(BOOL fRaise);

    // XOnline Support for XNet ----------------------------------------------------------

    BOOL            XoKerbBuildApReq(DWORD dwServiceId, LARGE_INTEGER * pliTime, BYTE * pbSha, UINT cbSha, BYTE * pbApReq, UINT * pcbApReq);
    BOOL            XoKerbCrackApRep(DWORD dwServiceId, LARGE_INTEGER * pliTime, BYTE * pbSha, UINT cbSha, BYTE * pbApRep, UINT cbApRep);
    void            XoKerbGetAuthData(XKERB_AD_XBOX * pAuthData);
    XOKERBINFO *    XoKerbGetInfo(DWORD dwServiceId);

};

#endif

// ---------------------------------------------------------------------------------------

#endif
