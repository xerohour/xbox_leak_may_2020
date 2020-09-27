// ---------------------------------------------------------------------------------------
// xonp.h
//
// Copyright (C) Microsoft Corporation
// ---------------------------------------------------------------------------------------

#ifndef __XONP_H__
#define __XONP_H__

// ---------------------------------------------------------------------------------------
// To Do List
//
// @@@ [tonychen] Add pool allocator (startup params, private heap)
// @@@ [tonychen] Call XnRaiseToDpc when modifying shared data (XOKERBINFO)
// ---------------------------------------------------------------------------------------

#pragma code_seg("XONLINE")
#pragma const_seg("XON_RD")

// ---------------------------------------------------------------------------------------
// Configuration
// ---------------------------------------------------------------------------------------

#ifndef DBG
#define DBG 0
#endif

#define XNET_FEATURE_ONLINE

#if DBG
    #define XNET_FEATURE_ASSERT
    #define XNET_FEATURE_TRACE
#endif

#if defined(XONLINE_BUILD_LIBS)
    #define XONLINE_FEATURE_XBOX
    #if DBG
        #pragma message("Building xonlinesd.lib")
    #else
        #pragma message("Building xonlines.lib")
    #endif
#elif defined(XONLINE_BUILD_LIBX)
    #define XONLINE_FEATURE_XBOX
    #define XONLINE_FEATURE_INSECURE
    #if DBG
        #pragma message("Building xonlined.lib")
    #else
        #pragma message("Building xonline.lib")
    #endif
#elif defined(XONLINE_BUILD_LIBW)
    #define XNET_FEATURE_WINDOWS
    #define XONLINE_FEATURE_WINDOWS
    #define XONLINE_FEATURE_INSECURE
    #if DBG
        #pragma message("Building xonlinewd.lib")
        #define XNET_FEATURE_VMEM
    #else
        #pragma message("Building xonlinew.lib")
    #endif
#else
    #error "Don't know which xonline library to build"
#endif

#ifdef XONLINE_FEATURE_INSECURE
#pragma message("  XONLINE_FEATURE_INSECURE")
#endif

#ifdef XONLINE_FEATURE_WINDOWS
#pragma message("  XONLINE_FEATURE_WINDOWS")
#endif

#ifdef XONLINE_FEATURE_XBOX
#pragma message("  XONLINE_FEATURE_XBOX")
#endif

// ---------------------------------------------------------------------------------------
// Includes
// ---------------------------------------------------------------------------------------

#ifdef XONLINE_FEATURE_XBOX
    #include <xapip.h>
    #include <xtl.h>
#endif

#ifdef XONLINE_FEATURE_WINDOWS
    #include <nt.h>
    #include <ntrtl.h>
    #include <nturtl.h>
    #include <windows.h>
    struct cab_file_list_header;
#endif

#include <winsockx.h>
#include <winsockp.h>
#include <xbox.h>
#include <xn.h>
#include <rsa.h>
#include <xcrypt.h>
#include <cabinet.h>
#include <xcabinet.h>
#include <dgstfile.h>
#include <krb5.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <winnls.h>

#ifndef FIELD_OFFSET
#define FIELD_OFFSET(type, field)    ((LONG)&(((type *)0)->field))
#endif

// ---------------------------------------------------------------------------------------
// Parameter Checking
// ---------------------------------------------------------------------------------------

#undef RIP_ON_NOT_TRUE
#undef RIP_ON_NOT_TRUE_WITH_MESSAGE

extern BOOL g_ParamCheckReturnsError;

#ifdef XNET_FEATURE_ASSERT
    #define XoEnter_(_fname, _ret) \
        const char * __pszFName__ = _fname; \
        do { if (this == NULL) { \
                 TraceSz1(Warning, "%s: You must call XOnlineStartup before calling this function.", _fname); \
                 return(_ret); } } while (0)
    #define XoCheck_(_pcheck, _ret) \
        do { if (!(_pcheck)) { \
            TraceSz2(Warning, "%s: Expression '%s' failed", __pszFName__, #_pcheck); \
            if (!g_ParamCheckReturnsError) { Rip("bad XOnline parameters\n"); } \
            return(_ret); } \
        } while (0)
    INLINE HRESULT XoLeave_(HRESULT hr, const char * pszFName) { \
        if (FAILED(hr)) { TraceSz2(Warning, "%s failed: %08lX", pszFName, hr); } \
        return(hr); }
    #define XoLeave(_hr)     XoLeave_(_hr, __pszFName__)
#else
    #define XoEnter_(_fname, _ret) do { if (this == NULL) return(_ret); } while (0)
    #define XoCheck_(_pcheck, _ret)
    #define XoLeave(_hr) _hr
#endif

#define XoEnter(_fname)  XoEnter_(_fname, XONLINE_E_NOT_INITIALIZED)
#define XoCheck(_pcheck) XoCheck_(_pcheck, E_INVALIDARG)

// ---------------------------------------------------------------------------------------
// Memory Tags
// ---------------------------------------------------------------------------------------

#define XONPTAGLIST() \
    XONPTAG(CXo,                                'XON_') \
    XONPTAG(XONLINETASK_AUTOUPD_DVD,            'XON0') \
    XONPTAG(XONLINETASK_FILEIO,                 'XON1') \
    XONPTAG(XONLINETASK_SOCKIO,                 'XON2') \
    XONPTAG(XO_PURCHASE_TASK,                   'XON3') \
    XONPTAG(XO_CANCEL_TASK,                     'XON4') \
    XONPTAG(XO_DETAILS_TASK,                    'XON5') \
    XONPTAG(XO_OFFERING_VERIFY_LICENSE_TASK,    'XON6') \
    XONPTAG(XONLINETASK_CONTENT_DOWNLOAD,       'XON7') \
    XONPTAG(XONLINETASK_OFFERING_ENUM,          'XON8') \
    XONPTAG(XONLINECONTENT_ENUM_TITLES,         'XON9') \
    XONPTAG(XONLINETASK_CONTENT_REMOVE,         'XONa') \
    XONPTAG(XONLINETASK_CONTENT_VERIFY,         'XONb') \
    XONPTAG(XONLINETASK_DIROPS,                 'XONd') \
    XONPTAG(XONLINETASK_DOWNLOAD_MEMORY,        'XONe') \
    XONPTAG(XONLINETASK_DOWNLOAD_FILE,          'XONf') \
    XONPTAG(XKERB_PA_XBOX_ACCOUNT_CREATION,     'XONg') \
    XONPTAG(XONLINETASK_CACHE,                  'XONh') \
    XONPTAG(CLocalCache,                        'XONi') \
    XONPTAG(CSector,                            'XONj') \
    XONPTAG(XKERB_TGT_CONTEXT,                  'XONk') \
    XONPTAG(XONLINETASK_GET_SINGLE_TICKET,      'XONl') \
    XONPTAG(XONLINETASK_GET_MACHINE_ACCOUNT,    'XONm') \
    XONPTAG(XONLINE_TGS_COMBINE_REQUEST_BUFFER, 'XONn') \
    XONPTAG(XKERB_COMBINED_TGT_CONTEXT,         'XONo') \
    XONPTAG(XONLINETASK_GET_COMBINED_TICKET,    'XONp') \
    XONPTAG(XKERB_SERVICE_CONTEXT,              'XONq') \
    XONPTAG(XONLINETASK_GET_SERVICE_TICKET,     'XONr') \
    XONPTAG(XONLINETASK_LOGON,                  'XONs') \
    XONPTAG(XMATCH_EXTENDED_HANDLE,             'XONt') \
    XONPTAG(XMATCH_POST_BUFFER,                 'XONu') \
    XONPTAG(XMATCH_RESULTS_BUFFER,              'XONv') \
    XONPTAG(XMATCH_SEARCHRESULT,                'XONw') \
    XONPTAG(XONLINECTXT_TITLECACHE,             'XONx') \
    XONPTAG(XONLINE_USERDATA,                   'XONy') \
    XONPTAG(XPRESENCE_EXTENDED_HANDLE,          'XONz') \
    XONPTAG(XPRESENCE_MESSAGE_BUFFER,           'XONA') \
    XONPTAG(XPRESENCE_HANDLE_LIST,              'XONB') \
    XONPTAG(OVERRIDE_INFO,                      'XONC') \
    XONPTAG(XONLINETASK_UPLOAD_MEMORY,          'XOND') \
    XONPTAG(XONLINETASK_UPLOAD_FILE,            'XONE') \
    XONPTAG(XONLINE_PIN_INPUT_STATE,            'XONF') \
    XONPTAG(XO_CREATE_ACCT_TASK,                'XONG') \
    XONPTAG(GET_TAGS_TASK,                      'XONH') \
    XONPTAG(GET_TAGS_REQUEST,                   'XONI') \
    XONPTAG(GET_TAGS_RESPONSE,                  'XONJ') \
    XONPTAG(GET_TAGS_RESULTS,                   'XONK') \
    XONPTAG(XONLINETASK_AUTOUPD_ONLINE,         'XONL') \
    XONPTAG(XPRESENCE_GAMEINVITE_RECORD,        'XONM') \
    XONPTAG(RESERVE_NAME_TASK,                  'XONN') \
    XONPTAG(RESERVE_NAME_REQUEST,               'XONO') \
    XONPTAG(RESERVE_NAME_RESPONSE,              'XONP') \
    XONPTAG(RESERVE_NAME_RESULTS,               'XONQ') \
    XONPTAG(XONMSGCLIENT_REQUEST,               'XONR') \
    XONPTAG(XONMSGCLIENT_WORKBUFFER,            'XONS') \
    XONPTAG(CREATE_ACCT_TASK,                   'XONT') \
    XONPTAG(CREATE_ACCT_REQUEST,                'XONU') \
    XONPTAG(CREATE_ACCT_RESPONSE,               'XONV') \

#undef  XONPTAG
#define XONPTAG(_name, _tag) PTAG_##_name = NTOHL_(_tag),

enum { XONPTAGLIST() };

// ---------------------------------------------------------------------------------------
// Scratch memory management routines
// ---------------------------------------------------------------------------------------

extern const XBOX_LIBRARY_VERSION g_ClientVersion;

//
// If Count is not already aligned, then
// round Count up to an even multiple of "Pow2".  "Pow2" must be a power of 2.
//
#define ROUND_UP_COUNT(Count,Pow2) \
        ( ((Count)+(Pow2)-1) & (~(((LONG)(Pow2))-1)) )

#define SCRATCH_MEMORY_ALIGNMENT 4

ExternTag(ScratchMemoryVerbose);
ExternTag(ScratchMemoryAlloc);

#define SCRATCH_MEMORY_STACK_SIZE 8

//@@@ drm: take some of these inlines away and make normal functions

class CScratchMemoryAllocator
{
private:
    PBYTE m_pbStart;
    PBYTE m_pbEnd;
    PBYTE m_pbCurrent;
    PBYTE m_pbTempBuffer;
    DWORD m_dwTempBufferSize;
    DWORD m_dwStackPointer;
    PBYTE m_Stack[SCRATCH_MEMORY_STACK_SIZE];
    #ifdef XNET_FEATURE_TRACE
    PBYTE m_pbPeakUsage;
    #endif    
    CXo * m_pXo;

public:
    INLINE CScratchMemoryAllocator()
    {
        m_pbStart = NULL;
    }
    
    //
    // This must be a macro or else _alloca won't work
    //
    #define ScratchMemoryInitialize( size, tempBuf, tempBufSize, pXo ) ScratchMemoryInitializeHelper(_alloca( (size) ), (size), (tempBuf), (tempBufSize), (pXo))

    INLINE PVOID ScratchMemoryInitializeHelper( IN PVOID ptr, IN size_t size, IN PVOID tempBuffer, IN size_t tempBufferSize , IN CXo * pXo)
    {
        AssertSz(m_pbStart == NULL,"Scratch Memory already in use");
        m_pbStart = (PBYTE)ptr;
        m_dwTempBufferSize = 0;
        AssertSz(m_pbStart != NULL,"Scratch Memory Initialize _alloca failed");
        if (m_pbStart != NULL)
        {
            m_pbCurrent = m_pbStart;
            m_pbEnd = m_pbStart + size;
            m_dwStackPointer = 0;
            RtlZeroMemory( m_pbStart, size );
            m_pbTempBuffer = (PBYTE)tempBuffer;
            if (m_pbTempBuffer != NULL)
            {
                Assert( tempBufferSize > 0 );
                m_dwTempBufferSize = tempBufferSize;
            }
            else
            {
                Assert( tempBufferSize == 0 );
            }
            #ifdef XNET_FEATURE_TRACE
            m_pbPeakUsage = m_pbStart;
            #endif    
            m_pXo = pXo;
        }
        return m_pbStart;
    }

    INLINE ~CScratchMemoryAllocator()
    {
        TraceSz2(ScratchMemoryVerbose, "Destructing, peak %d out of %d used",
                m_pbPeakUsage - m_pbStart,
                m_pbEnd - m_pbStart);
    }

    INLINE CXo * GetXoPtr()
    {
        return(m_pXo);
    }

    INLINE PVOID TempBuffer()
    {
        return m_pbTempBuffer;
    }
        
    INLINE DWORD TempBufferSize()
    {
        return m_dwTempBufferSize;
    }
        
    INLINE PVOID Alloc( IN size_t BufferSize )
    {
        PBYTE CurrentPosition;
        Assert(m_pbStart != NULL);
        CurrentPosition = m_pbCurrent;
        BufferSize = ROUND_UP_COUNT(BufferSize,SCRATCH_MEMORY_ALIGNMENT);
        m_pbCurrent += BufferSize;
        TraceSz3(ScratchMemoryAlloc, "Alloc %d, now used %d out of %d", BufferSize,
                m_pbCurrent - m_pbStart,
                m_pbEnd - m_pbStart);
        AssertSz(m_pbCurrent <= m_pbEnd,"Out of Scratch Memory");
        if (m_pbCurrent > m_pbEnd)
        {
            m_pbCurrent = CurrentPosition;
            CurrentPosition = NULL;
        }
        #ifdef XNET_FEATURE_TRACE
        if (m_pbPeakUsage < m_pbCurrent)
        {
            m_pbPeakUsage = m_pbCurrent;
        }
        #endif    
        return CurrentPosition;
    }

    INLINE PVOID ReAlloc( IN PVOID Buffer, IN size_t BufferSize )
    {
        PBYTE CurrentPosition;
        Assert(m_pbStart != NULL);
        CurrentPosition = m_pbCurrent;
        BufferSize = ROUND_UP_COUNT(BufferSize,SCRATCH_MEMORY_ALIGNMENT);
        m_pbCurrent += BufferSize;
        TraceSz3(ScratchMemoryAlloc, "ReAlloc %d, now used %d out of %d", BufferSize,
                m_pbCurrent - m_pbStart,
                m_pbEnd - m_pbStart);
        AssertSz(m_pbCurrent <= m_pbEnd,"Out of Scratch Memory");
        if (m_pbCurrent > m_pbEnd)
        {
            m_pbCurrent = CurrentPosition;
            CurrentPosition = NULL;
        }
        else
        {
            RtlCopyMemory( CurrentPosition, Buffer, BufferSize );
        }
        #ifdef XNET_FEATURE_TRACE
        if (m_pbPeakUsage < m_pbCurrent)
        {
            m_pbPeakUsage = m_pbCurrent;
        }
        #endif    
        return CurrentPosition;
    }

    INLINE PVOID ImmediateReAlloc( IN PVOID Buffer, IN size_t OldBufferSize, IN size_t NewBufferSize )
    {
        Assert(m_pbStart != NULL);
        OldBufferSize = ROUND_UP_COUNT(OldBufferSize,SCRATCH_MEMORY_ALIGNMENT);
        if ( m_pbCurrent == (PBYTE)Buffer + OldBufferSize )
        {
            //
            // Nothing else was allocated in between, we can do things better.
            //
            PBYTE CurrentPosition;
            CurrentPosition = m_pbCurrent;
            NewBufferSize = ROUND_UP_COUNT(NewBufferSize,SCRATCH_MEMORY_ALIGNMENT);
            m_pbCurrent += (NewBufferSize - OldBufferSize);
            TraceSz3(ScratchMemoryAlloc, "ImmediateReAlloc %d, now used %d out of %d", NewBufferSize,
                    m_pbCurrent - m_pbStart,
                    m_pbEnd - m_pbStart);
            AssertSz(m_pbCurrent <= m_pbEnd,"Out of Scratch Memory");
            if (m_pbCurrent > m_pbEnd)
            {
                m_pbCurrent = CurrentPosition;
                return NULL;
            }
            else
            {
                #ifdef XNET_FEATURE_TRACE
                if (m_pbPeakUsage < m_pbCurrent)
                {
                    m_pbPeakUsage = m_pbCurrent;
                }
                #endif    
                return Buffer;
            }
        }
        else
        {
            return ReAlloc( Buffer, NewBufferSize );
        }
    }

    INLINE VOID Free( IN PVOID Buffer )
    {
        Assert(m_pbStart != NULL);
    }

    INLINE VOID Push()
    {
        Assert(m_pbStart != NULL);
        AssertSz(m_dwStackPointer < SCRATCH_MEMORY_STACK_SIZE,"Scratch Memory Stack Overflow");
        m_Stack[m_dwStackPointer++] = m_pbCurrent;
    }

    INLINE VOID Pop()
    {
        PVOID pTemp;
        Assert(m_pbStart != NULL);
        AssertSz(m_dwStackPointer != 0,"Scratch Memory Stack Underflow");
        pTemp = m_pbCurrent;
        m_pbCurrent = m_Stack[--m_dwStackPointer];
        Assert( m_pbCurrent <= pTemp);
        RtlZeroMemory( m_pbCurrent, (PBYTE)pTemp - m_pbCurrent );
    }
};

class CReleaseAllScratchMemoryUponDestruction
{
public:

    INLINE CReleaseAllScratchMemoryUponDestruction( CScratchMemoryAllocator* pScratchMemoryAllocator )
    {
        m_pScratchMemoryAllocator = pScratchMemoryAllocator;
        m_pbPreservedMemory = NULL;
        //
        // Anything allocated after the constructed will be released later
        //
        m_pScratchMemoryAllocator->Push();
    }

    INLINE CScratchMemoryAllocator* ScratchMemoryAllocator()
    {
        return m_pScratchMemoryAllocator;
    }
        
    //
    // Need to use #define or _alloca scope will be incorrect
    //
    #define PreserveThisMemory( pObject, ppMemoryPointer, dwPreservedMemoryLength ) \
        (pObject)->PreserveThisMemoryHelper( (void**)ppMemoryPointer, (pObject)->ScratchMemoryAllocator()->TempBufferSize() >= dwPreservedMemoryLength ? NULL : _alloca( dwPreservedMemoryLength ), dwPreservedMemoryLength )
    
    INLINE PVOID PreserveThisMemoryHelper( void** ppMemoryPointer, void* pbAllocatedMemory, DWORD dwPreservedMemoryLength )
    {
        Assert( dwPreservedMemoryLength != 0 );
        Assert( ppMemoryPointer != NULL );
        Assert( *ppMemoryPointer != NULL );
        if (pbAllocatedMemory == NULL && m_pScratchMemoryAllocator->TempBufferSize() >= dwPreservedMemoryLength)
        {
            pbAllocatedMemory = m_pScratchMemoryAllocator->TempBuffer();
        }
        else
        {
            AssertSz(m_pScratchMemoryAllocator->TempBuffer() == NULL,"Had Temp Buffer, but not big enough");
        }
        if (pbAllocatedMemory != NULL)
        {
            RtlCopyMemory(pbAllocatedMemory, *ppMemoryPointer, dwPreservedMemoryLength);
            m_ppMemoryPointer = ppMemoryPointer;
            m_dwPreservedMemoryLength = dwPreservedMemoryLength;
            m_pbPreservedMemory = pbAllocatedMemory;
        }
        return pbAllocatedMemory;
    }

    INLINE PVOID PreserveThisMemoryToTempBuffer( PBYTE* ppMemoryPointer, DWORD dwPreservedMemoryLength )
    {
        PVOID pbDestinationMemory = NULL;
        
        Assert( dwPreservedMemoryLength != 0 );
        Assert( ppMemoryPointer != NULL );
        Assert( *ppMemoryPointer != NULL );
        if (m_pScratchMemoryAllocator->TempBuffer() != NULL &&
            m_pScratchMemoryAllocator->TempBufferSize() >= dwPreservedMemoryLength )
        {
            pbDestinationMemory = m_pScratchMemoryAllocator->TempBuffer();
            RtlCopyMemory(pbDestinationMemory, *ppMemoryPointer, dwPreservedMemoryLength);
            *ppMemoryPointer = (PBYTE)pbDestinationMemory;
        }
        else
        {
            AssertSz(FALSE, "Temp Buffer can't fit PreserveThisMemoryToTempBuffer");
        }
        return pbDestinationMemory;
    }

    INLINE ~CReleaseAllScratchMemoryUponDestruction()
    {
        m_pScratchMemoryAllocator->Pop();
        
        if (m_pbPreservedMemory != NULL)
        {
            Assert( m_dwPreservedMemoryLength != 0 );
            *m_ppMemoryPointer = m_pScratchMemoryAllocator->Alloc(m_dwPreservedMemoryLength);
            RtlCopyMemory(*m_ppMemoryPointer, m_pbPreservedMemory, m_dwPreservedMemoryLength);
        }
    }
    
private:

    CScratchMemoryAllocator* m_pScratchMemoryAllocator;
    PVOID m_pbPreservedMemory;
    PVOID* m_ppMemoryPointer;
    DWORD m_dwPreservedMemoryLength;
};

//
// From msr_md5.h
//

#define MD5Init msr_MD5Init
#define MD5Update msr_MD5Update
#define MD5Final msr_MD5Final
#define MD5_CTX msr_MD5_CTX

#define ECAPI // __stdcall

typedef int tBool;
typedef unsigned char tUint8;
typedef unsigned short tUint16;
typedef unsigned long tUint32;

//  -------------- MD5 message digest --------------------

/* Two names for number of bytes output by MD5 */
#define MD5DIGESTLEN 16
#define MD5_LEN MD5DIGESTLEN

typedef struct {
        tUint32 magic_md5;     // Magic value for MD5_CTX
        tUint32 awaiting_data[16];
                             // Data awaiting full 512-bit block.
                             // Length (nbit_total[0] % 512) bits.
                             // Unused part of buffer (at end) is zero.
        tUint32 partial_hash[4];
                             // Hash through last full block
        tUint32 nbit_total[2];       
                             // Total length of message so far
                             // (bits, mod 2^64)
        tUint8 digest[MD5DIGESTLEN];
                             // Actual digest after MD5Final completes
        tBool FinishFlag;
} msr_MD5_CTX;

typedef const msr_MD5_CTX msr_MD5_CTXc;

extern tBool ECAPI msr_MD5Init(msr_MD5_CTX*);
extern tBool ECAPI msr_MD5Update(msr_MD5_CTX*, const tUint8*, const tUint32);
extern tBool ECAPI msr_MD5Final(msr_MD5_CTX*);

// ====================================================================
// header for logon.cpp
// ====================================================================

#define XBOX_REALM      "xbox.com"
#define MACS_REALM      "macs.xbox.com"

typedef LARGE_INTEGER TimeStamp, *PTimeStamp;

#define MAX_KERB_RETRIES 4
#define MAX_TIME_ADJUSTMENTS MAX_KERB_RETRIES

extern DWORD g_dwNumKerbRetry;
extern DWORD g_dwNumMACSRetry;
extern DWORD g_dwKerbRetryDelayInMilliSeconds[MAX_KERB_RETRIES];
extern DWORD g_dwMACSRetryDelayInMilliSeconds[MAX_KERB_RETRIES];

//
// At most 2 service requests at the end of the array that is not linear
//
#define XONLINE_MAX_SERVICE_REQUEST 9
#define XONLINE_NUMBER_SEQUENTIAL_SERVICES 7

#pragma pack(push, 4)

typedef struct XONLINE_RECENT_LOGON_TIME {
    ULONGLONG qwUserID;
    FILETIME lastLogon;
} *PXONLINE_RECENT_LOGON_TIME;

typedef struct XKERB_TGT_CONTEXT
{
    // The following fields are used to index this entry in the cache
    ULONGLONG UserID[XONLINE_MAX_LOGON_USERS];
    TimeStamp StartTime;
    TimeStamp ExpirationTime;

    // The following fields are in memory only and not stored in the cache
    DWORD ctNumNonces;
    DWORD Nonce[MAX_KERB_RETRIES + MAX_TIME_ADJUSTMENTS];
    BYTE ClientKey[XONLINE_KEY_LENGTH];
    BYTE CompoundKey[XONLINE_KEY_LENGTH];
    OPTIONAL IN XKERB_TGT_CONTEXT* pInputTGTContext;
    OPTIONAL OUT PXKERB_PA_XBOX_ACCOUNT_CREATION pAccountCreationPreAuth;

    // The following fields are cached to disk
    CHAR ClientName[XONLINE_KERBEROS_PRINCIPLE_NAME_SIZE];
    CHAR ClientRealm[XONLINE_REALM_NAME_SIZE];
    CHAR TargetRealm[XONLINE_REALM_NAME_SIZE];
    BYTE SessionKey[XONLINE_KEY_LENGTH];
    DWORD dwTicketLength;
    BYTE Ticket[XONLINE_MAX_TICKET_LENGTH];
} *PXKERB_TGT_CONTEXT;

typedef struct XKERB_SERVICE_CONTEXT
{
    // The following fields are used to index this entry in the cache
    ULONGLONG UserID[XONLINE_MAX_LOGON_USERS];
    DWORD dwTitleID;
    DWORD dwNumServices;
    DWORD dwServiceID[XONLINE_MAX_NUMBER_SERVICE];
    TimeStamp StartTime;
    TimeStamp ExpirationTime;

    // The following fields are in memory only and not stored in the cache
    DWORD ctNumNonces;
    DWORD Nonce[MAX_KERB_RETRIES + MAX_TIME_ADJUSTMENTS];
    TimeStamp AuthenticatorTime[MAX_KERB_RETRIES + MAX_TIME_ADJUSTMENTS];
    HRESULT hr;
    XOKERBINFO xoKerbInfo;

    // The following fields are cached to disk
    IN_ADDR siteIPAddress;
    DWORD dwSiteID;
    HRESULT hrService[XONLINE_MAX_NUMBER_SERVICE];
    WORD wServicePort[XONLINE_MAX_NUMBER_SERVICE];
    CHAR ClientName[XONLINE_KERBEROS_PRINCIPLE_NAME_SIZE];
    CHAR ClientRealm[XONLINE_REALM_NAME_SIZE];
    CHAR ServiceRealm[XONLINE_REALM_NAME_SIZE];
    BYTE SessionKey[XONLINE_KEY_LENGTH];
    DWORD dwTicketLength;
    BYTE Ticket[XONLINE_MAX_TICKET_LENGTH];
} *PXKERB_SERVICE_CONTEXT;

typedef struct SERVICE_CONTEXT
{
    XONLINE_SERVICE_INFO serviceInfo;
    HRESULT hr;
    XKERB_SERVICE_CONTEXT* pXKerbServiceContext;
} *PSERVICE_CONTEXT;

#pragma pack(pop)

/* going away */
BOOL
KerbPasswordToKey(
    IN LPSTR pszPassword,
    OUT BYTE* Key
    );

typedef struct _KERB_MESSAGE_BUFFER {
    DWORD BufferSize;
    BYTE* Buffer;
} KERB_MESSAGE_BUFFER, *PKERB_MESSAGE_BUFFER;

// ====================================================================
// User signature related stuff
//

//
// @@@ TODO: This string needs to be reved every release
// until we feel the XC_ONLINE_USER_ACCOUNT_STRUCT is stable
//
#define ONLINE_USER_SIGNATURE_KEY "Mar/2002 SDK"

#if 0
#define XCHMAC_DIGEST_SIZE 20

void XcHMAC(
    IN PBYTE pbKeyMaterial,
    IN ULONG cbKeyMaterial,
    IN PBYTE pbData,
    IN ULONG cbData,
    IN PBYTE pbData2,
    IN ULONG cbData2,
    OUT PBYTE pbDigest
    );
#endif

//
// Some time on January/2000, The Xbox time cannot be adjusted to earlier than this
//
#define BASE_SIGNATURE_TIME ((ULONGLONG)0x01BF5C72FEFB6A60)

//
// Granularity of measurement (2 seconds)
//
#define SIGNATURE_TIME_INCREMENT ((ULONGLONG)20000000)

INLINE VOID TimeStampOnlineUserStruct( XC_ONLINE_USER_ACCOUNT_STRUCT* pOnlineUser )
{
    FILETIME CurrentTime;
    
    GetSystemTimeAsFileTime(&CurrentTime);
    pOnlineUser->dwSignatureTime = (DWORD)((*((ULONGLONG*)&CurrentTime) - BASE_SIGNATURE_TIME) / SIGNATURE_TIME_INCREMENT);
    Assert( *((ULONGLONG*)&CurrentTime) > BASE_SIGNATURE_TIME );
    Assert( ((*((ULONGLONG*)&CurrentTime) - BASE_SIGNATURE_TIME) / SIGNATURE_TIME_INCREMENT) <= ((ULONGLONG)0xFFFFFFFF) );
}

INLINE VOID SignOnlineUserStruct( XC_ONLINE_USER_ACCOUNT_STRUCT* pOnlineUser )
{
    BYTE digest[XC_SERVICE_DIGEST_SIZE];

    XcHMAC( (BYTE*)ONLINE_USER_SIGNATURE_KEY, sizeof(ONLINE_USER_SIGNATURE_KEY),
        (BYTE*)pOnlineUser, sizeof(XC_ONLINE_USER_ACCOUNT_STRUCT) - sizeof(pOnlineUser->signature), NULL, 0, digest );

    RtlCopyMemory( pOnlineUser->signature, digest, sizeof(pOnlineUser->signature) );
}

INLINE BOOL VerifyOnlineUserSignature( XC_ONLINE_USER_ACCOUNT_STRUCT* pOnlineUser )
{
    BYTE digest[XC_SERVICE_DIGEST_SIZE];

    XcHMAC( (BYTE*)ONLINE_USER_SIGNATURE_KEY, sizeof(ONLINE_USER_SIGNATURE_KEY),
        (BYTE*)pOnlineUser, sizeof(XC_ONLINE_USER_ACCOUNT_STRUCT) - sizeof(pOnlineUser->signature), NULL, 0, digest );

    return RtlEqualMemory( pOnlineUser->signature, digest, sizeof(pOnlineUser->signature) );
}

// ====================================================================
// XOnline Work Pump
//

typedef HRESULT (CXo::*PFNXONLINE_TASK_CONTINUE)(
    IN XONLINETASK_HANDLE hTask
    );

typedef XBOXAPI void (CXo::*PFNXONLINE_TASK_CLOSE)(
    IN XONLINETASK_HANDLE hTask
    );

typedef struct {
#if DBG
    DWORD                   dwSignature;        // DEBUG ONLY: Signature
    DWORD                   dwThreadId;         // DEBUG_ONLY: ThreadId of creator thread
#endif  
    PFNXONLINE_TASK_CONTINUE pfnContinue;
    PFNXONLINE_TASK_CLOSE   pfnClose;
    HANDLE                  hEventWorkAvailable;
} XONLINETASK_CONTEXT, *PXONLINETASK_CONTEXT;

// ---------------------------------------------------------------------------------------
// Definitions
// ---------------------------------------------------------------------------------------

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

// BASEIO --------------------------------------------------------------------------------

//
// =============================================================
// Basic async file/socket I/O primitives
//
// These versions of the primitives allocate and return a task
// handle to the caller. Title developers may use these primitives
// to perform basic async file and socket I/O
//

//
// Define context structure shared by file I/O primitives
//
typedef struct
{
    XONLINETASK_CONTEXT       xontask;    // Task context

    HRESULT                   hr;            // Status code
    LPBYTE                    pbBuffer;    // R/W buffer
    DWORD                     cbBuffer;    // Buffer size
    DWORD                     cbDone;        // Bytes completed 
    HANDLE                    hFile;        // File handle
    OVERLAPPED                overlapped;    // OVERLAPPED struct

} XONLINETASK_FILEIO, *PXONLINETASK_FILEIO;

//
// Define context structure shared by socket I/O primitives
//
typedef struct
{
    XONLINETASK_CONTEXT      xontask;    // Task context
    
    HRESULT                  hr;            // Status code
    SOCKET                   socket;        // socket
    LPWSABUF                 pBuffers;    // WSA buffers
    DWORD                    dwBuffers;    // Number of buffers
    DWORD                    cbDone;        // Bytes completed
    DWORD                    dwFlags;    // Flags
    WSAOVERLAPPED            wsaoverlapped;    // OVERLAPPED struct

} XONLINETASK_SOCKIO, *PXONLINETASK_SOCKIO;

//
// =============================================================
// Basic async file/socket I/O primitives
//
// These versions of the primitives take in a context object that
// is assumed to be initialized. These versions of the primitives
// are for task implementors, and are not generally available to
// title developers.
//

// ---------------------------------------------------------------------------------------

//
// Define macros to round up and down for sector alignment
//
#define SECTOR_ALIGNMENT_ROUND_UP(x)    \
        (((x) + (XBOX_HD_SECTOR_SIZE - 1)) & (~(XBOX_HD_SECTOR_SIZE - 1)))

#define SECTOR_ALIGNMENT_ROUND_DOWN(x)    \
        ((x) & (~(XBOX_HD_SECTOR_SIZE - 1)))

#define IS_HD_SECTOR_ALIGNED(x)            \
        ((((x) & (XBOX_HD_SECTOR_SIZE - 1)) == 0)?TRUE:FALSE)

// ===========================================================
// Download and upload extensions
//
// These extensions are specifically designed to maximize
// code sharing and to aviod redundancy as much as possible.
// The basic idea is that we only need one instance of the code
// that downloads and uploads data. 
//
// By decoupling the code that strictly talks to the server and
// the code that processes the actual data, we create an extensible
// framework that allows arbitrary implementations on top of the
// download and uplaod framework. 
//
// The main challenge here is that these extensions themselves must
// also be asynchronous tasks that use XOnlineTaskContinue to make
// progress. This allows the underlying implementation to be 
// completely oblivious to what the subtasks do - all it needs to
// do is to keep calling XOnlineTaskContinue on the subtask until
// the subtask is done, and then it moves on with its own next
// state of processing. The subtask may also contain other smaller
// subtasks, creating the possibility of dependent cascading tasks, 
// each waiting for its child to complete before moving on, and
// all work is achieved through a unified work API -
// XOnlineTaskContinue, which also supports timing control through
// the use of dwMaxExecutionTimeSlice.
//
// The implementation of XOnlineDownloadFile, XOnlineDownloadToMemory,
// XOnlineUploadFile and XOnlineUploadFromMemory are all implemented
// as extensions to illustrate how this is done.
//
// New extensions can be built by wrapping similar funcitonality 
// around the core XOnlineDownload and XOnlineUpload APIs.
//

//
// Define the set of download extension types
//
typedef enum
{
    xrldlextPreConnect = 0,
    xrldlextProcessData,
    xrldlextPostDownload

} XRLDOWNLOAD_EXTTYPE;

//
// Function prototypes for the donwload extension callbacks
//
// Note: the PVOIDs are actually PXRL_ASYNC pointers,
// but I couldn't get the circular reference
// to compile properly in C.
//
typedef HRESULT (CXo::*PFNXRLDOWNLOAD_DONEHEADERS)(
            PVOID                pvxrlasync
            );

typedef BOOL (CXo::*PFNXRLDOWNLOAD_PROCESSDATA)(
            PVOID                pvxrlasync
            );

typedef HRESULT (CXo::*PFNXRLDOWNLOAD_COMPLETED)(
            PVOID                pvxrlasync
            );

typedef VOID (CXo::*PFNXRLDOWNLOAD_CLEANUP)(
            PVOID                pvxrlasync
            );

typedef HRESULT (CXo::*PFNXRLDOWNLOAD_INITTASK)(
            XRLDOWNLOAD_EXTTYPE    xrldlext,
            PVOID                pvxrlasync,
            XONLINETASK_HANDLE    hTask
            );

//
// Structure representing extensions to the bare-bones download
// framework. Both XOnlineDownloadToMemory and XOnlineDownloadFile
// are implemented as extensions. This maximizes code sharing and
// creates a lot of flexibility
//
typedef struct
{
    PFNXRLDOWNLOAD_DONEHEADERS    pfnDoneHeaders;
    PFNXRLDOWNLOAD_PROCESSDATA    pfnProcessData;
    PFNXRLDOWNLOAD_COMPLETED    pfnCompleted;
    PFNXRLDOWNLOAD_INITTASK        pfnInitializeTask;
    PFNXRLDOWNLOAD_CLEANUP        pfnCleanup;

    XONLINETASK_HANDLE            hTaskPreConnect;
    XONLINETASK_HANDLE            hTaskProcessData;
    XONLINETASK_HANDLE            hTaskPostDownload;

} XRL_DOWNLOAD_EXTENSION, *PXRL_DOWNLOAD_EXTENSION;

//
// Define the set of upload extension types
//
typedef enum
{
    xrlulextPreConnect = 0,
    xrlulextReadData,
    xrlulextPostUpload

} XRLUPLOAD_EXTTYPE;

//
// Function prototypes for the upload extension callbacks
//
// Note: the PVOIDs are actually PXRL_ASYNC pointers,
// but I couldn't get the circular reference
// to compile properly in C.
//
typedef HRESULT (CXo::*PFNXRLUPLOAD_COMPLETED)(
            PVOID                pvxrlasync
            );

typedef HRESULT (CXo::*PFNXRLUPLOAD_RESPONSEHEADERS)(
            PVOID                pvxrlasync
            );

typedef VOID (CXo::*PFNXRLUPLOAD_CLEANUP)(
            PVOID                pvxrlasync
            );
            
typedef HRESULT (CXo::*PFNXRLUPLOAD_INITTASK)(
            XRLUPLOAD_EXTTYPE    xrlulext,
            PVOID                pvxrlasync,
            XONLINETASK_HANDLE    hTask
            );

//
// Structure representing extensions to the bare-bones upload
// framework. Both XOnlineUploadToMemory and XOnlineUploadFile
// are implemented as extensions. This maximizes code sharing and
// creates a lot of flexibility
//
typedef struct
{
    PFNXRLUPLOAD_COMPLETED            pfnCompleted;
    PFNXRLUPLOAD_RESPONSEHEADERS    pfnResponseHeaders;
    PFNXRLUPLOAD_INITTASK            pfnInitializeTask;
    PFNXRLUPLOAD_CLEANUP            pfnCleanup;

    XONLINETASK_HANDLE                hTaskPreConnect;
    XONLINETASK_HANDLE                hTaskReadData;
    XONLINETASK_HANDLE                hTaskPostUpload;

} XRL_UPLOAD_EXTENSION, *PXRL_UPLOAD_EXTENSION;

//
// Define a union between the download and upload extensions
// and call it a generice XRL_EXTENSION
//
typedef union
{
    XRL_DOWNLOAD_EXTENSION    dl;
    XRL_UPLOAD_EXTENSION    ul;

} XRL_EXTENSION, *PXRL_EXTENSION;

//
// The following success code is used when a pre-up/download extension
// wants to skip the actual up/download and jump right to the 
// post-up/download phase. This is typically used when the pre-task
// determines that the desired data is already there and there is no need
// to actually send/get it over the wire.
//
#define XONLINE_S_XRL_EXTENSION_SKIP_TO_POST	(MAKE_HRESULT(0, FACILITY_XONLINE, 0xffff))

//
// Context structure for resource access operations. This is allocated by 
// a call to download/upload a resource (either into memory or to a file).
// This is returned to the caller as an opaque handle. 
//
typedef struct
{
    // "Inherit from" the main context.
    XONLINETASK_CONTEXT    xontask;            // XON task context

    // Context information specific to download operations
    HRESULT                hrFinal;            // Final HRESULT
    HRESULT                hrXErr;                // Optional X-Err code
    DWORD                dwCurrentState;        // Current state of execution
    DWORD                dwFlags;            // Any special flags
    DWORD                dwTimeoutMs;        // Timeout value in milliseconds
    DWORD                dwLastStateChange;    // Tick on last state change
    XONLINE_SERVICE_INFO serviceInfo;            // Pointer to service information

    XONLINETASK_FILEIO    fileio;                // Async File I/O context
    XONLINETASK_SOCKIO    sockio;                // Async Socket I/O context
    WSABUF                wsabuf;                // WSA buffer structure

    PBYTE                pBuffer;            // Beginning of buffer
    DWORD                cbBuffer;            // Size of buffer specified
    DWORD                dwExtendedStatus;    // Extended status code
    FILETIME            ftLastModified;        // Last modified time
    FILETIME            ftResponse;            // Server response timestamp
    ULARGE_INTEGER        uliContentLength;    // Content length
    ULARGE_INTEGER        uliTotalReceived;    // Total received
    ULARGE_INTEGER        uliTotalWritten;    // Total written
    ULARGE_INTEGER        uliFileSize;        // Current file size
    DWORD                dwCurrent;            // Currency marker
    DWORD                dwTemp;                // Generic storage

    XRL_EXTENSION        xrlext;                // Extension descriptor
    BOOL                fDownload;            // Whether this is a download

} XRL_ASYNC, *PXRL_ASYNC;

//
// Special flag to indicate that the hEventWorkAvailable is created
// by the call to XOnlineDownload or XOnlineUpload
//
#define XRL_HEVENT_CREATED_BY_API            ((DWORD)0x00000001)
#define XRL_SEND_DATA_CONTAINS_HEADERS        ((DWORD)0x00000002)
#define XRL_CONNECTION_INSECURE               ((DWORD)0x40000000)
#define XRL_OP_IS_DOWNLOAD                    ((DWORD)0x80000000)

//
// Define a context structure for extensions, which 
// contains an XRL_ASYNC structure, and an extra task
// context for the 'subtask'. This is specific to the 
// XRL upload and download extension implementations. 
// Extension writers should define their own context.
//
// The general idea is that &xontask is actually a valid
// task handle and can be used with XOnlineTaskContinue,
// provided that the task context is filled in properly.
//
// The subtask gains access to the outside xrlasync struct
// by using the CONTAINING_RECORD(p, XRL_ASYNC_EXTENDED, xontask)
// macro, which returns a pointer to PXRL_ASYNC_EXTENDED.
//
typedef struct
{
    XRL_ASYNC                xrlasync;        // Main context

    XONLINETASK_CONTEXT        xontask;        // subtask handle
    DWORD                    dwCurrentState;    // State variable
    
} XRL_ASYNC_EXTENDED, *PXRL_ASYNC_EXTENDED;

//
// A particular characteristic of Xbox asynchronous WriteFile calls is
// that the call is asynchronous so long as the file size is not exceeded.
// To keep the writes asynchronous, SetEndOfFile is used to grow the file
// in chunks. The following is the increment size used to grow files.
//
// BUGBUG: Need to find an optimal value
//
#define XRL_INCREMENTAL_GROW_FILE_SIZE        ((XBOX_HD_SECTOR_SIZE) << 12)    // 2MBytes

//
// Define an upper bound for the size of any set of extra headers passed
// into any of the up/download functions.
//
// The value is limited to 128k bytes, which is consistent with the maximum
// size of all headers allowed in IIS.
//
#define XRL_MAX_EXTRA_HEADER_SIZE            ((DWORD)(1 << 17))    // 128KBytes   


//
// This provides the ability to specify an alternate IP address for 
// a given service, and to talk to that service via an insecure channel.
//
// To use, compile with the following contants defined:
//      XRL_SERVICE_OVERRIDE_ID=<service id you wish to override>
//      XRL_SERVICE_OVERRIDE_IP=binary form of ip address (i.e. inet_addr("a.b.c.d"))
//
// currently this only affects UploadFromMemoryInternal(), but is easily added
// to any of the other xrl apis. 
//

#if defined(XONLINE_FEATURE_INSECURE) && \
    defined(XRL_SERVICE_OVERRIDE_ID) && \
    defined(XRL_SERVICE_OVERRIDE_IP)
    INLINE VOID XoOverrideServiceIP_(PXRL_ASYNC pXrlAsync) { \
        if (pXrlAsync->serviceInfo.dwServiceID == XRL_SERVICE_OVERRIDE_ID) { \
            TraceSz6(Warning, "Overriding Service ID %d: Using IP address %d.%d.%d.%d:%d", \
                     XRL_SERVICE_OVERRIDE_ID,\
                     XRL_SERVICE_OVERRIDE_IP >> 24, \
                     (XRL_SERVICE_OVERRIDE_IP & 0xff0000) >> 16, \
                     (XRL_SERVICE_OVERRIDE_IP & 0x00ff00) >> 8, \
                     (XRL_SERVICE_OVERRIDE_IP & 0x0000ff), \
                     80); \
            pXrlAsync->dwFlags |= XRL_CONNECTION_INSECURE; \
            pXrlAsync->serviceInfo.serviceIP.S_un.S_addr = XRL_SERVICE_OVERRIDE_IP; \
            pXrlAsync->serviceInfo.wServicePort = 80; \
        } \
     } 
    #define XoOverrideServiceIP(_pxrl) XoOverrideServiceIP_(_pxrl)
#else
    #define XoOverrideServiceIP(_pxrl) 
#endif




// SYMMDEC.H -----------------------------------------------------------------------------

//
// Define a context that allows piecemeal decryption and
// SHA1-Hmac calculation
//
typedef struct
{
    DWORD        cbLength;        // Total length of encrypted block
                                    // This includes the confounder (CF)
    DWORD        dwCurrent;        // Bytes decrypted, incl. CF
    
    PBYTE        pbKey;            // Symmetric key
    DWORD        cbKey;            // Key length
    BYTE         rgbChecksum[XC_DIGEST_LEN];
                                // Original checksum
    BYTE        shactx[XC_SERVICE_SHA_CONTEXT_SIZE];
                                // SHA1 context for Hmac
    BYTE        rc4ctx[XC_SERVICE_RC4_KEYSTRUCT_SIZE];
                                // RC4 decryption context
} XONLINESYMMDEC_CONTEXT, *PXONLINESYMMDEC_CONTEXT;

//
// If the length of the data is unknown to begin with, specify
// this length to skip the length check during SymmdecVerify
//
#define XONLINESYMMDEC_UNKNOWN_LENGTH    ((DWORD)0xffffffff)

// XRLUTIL.H -----------------------------------------------------------------------------

//
// Define the valid values for dwFlags in XRL_ASYNC
//
#define XRL_FLAG_HEADER_MODE            ((DWORD)0x00000001)

#define IS_HEADER_MODE(pxrlasync)        \
        (((pxrlasync)->dwTemp & XRL_FLAG_HEADER_MODE) != 0)

//
// Define a type for our own work handlers
//
typedef HRESULT (CXo::*PFNXRL_HANDLER)(PXRL_ASYNC);

// LZX -----------------------------------------------------------------------------------
//
// IMPORTANT PLEASE READ:
//
// This is NOT the stock LZX decompressor code that is used in the ROM! This is a special
// version designed for the Online team. This code has been stripped down and the
// underlying data structure has been changed. Please do not use this code if you intend
// to use LDI. Instead, please refer to \private\lzx for the stock LZX/LDI sources.
//
// If you decide to use this code instead, please take the time to read and understand
// the code. That is, use at your own risk.
// ---------------------------------------------------------------------------------------

/*
 * Definitions for LZX
 */
typedef unsigned char    byte;
typedef unsigned short    ushort;
typedef unsigned long    ulong;
typedef unsigned int    uint;

#ifndef __cplusplus
typedef enum
{
    false = 0,
    true = 1
} bool;
#endif

#ifndef HUGE
#    define HUGE
#endif

#ifndef FAR
#    define FAR
#endif

#ifndef NEAR
#   define NEAR
#endif

/*
 * Smallest allowable match length 
 */
#define MIN_MATCH 2

/* 
 * Maximum match length 
 */
#define MAX_MATCH (MIN_MATCH+255)

/*
 * Number of uncompressed symbols 
 */
#define NUM_CHARS 256

/*
 * Number of match lengths which are correlated with match positions 
 */
#define NUM_PRIMARY_LENGTHS     7

/*
 * Primary lengths plus the extension code
 */
#define NUM_LENGTHS             (NUM_PRIMARY_LENGTHS+1)

/*
 * Equals number of different possible match lengths minus primary lengths 
 */
#define NUM_SECONDARY_LENGTHS   ((MAX_MATCH-MIN_MATCH+1)-NUM_PRIMARY_LENGTHS)

/* NL_SHIFT = log2(NUM_LENGTHS) */
#define NL_SHIFT                3

/*
 * Number of repeated offsets 
 */
#define NUM_REPEATED_OFFSETS    3

/*
 * Number of elements in the aligned offset tree
 */
#define ALIGNED_NUM_ELEMENTS 8


/*
 * Repeat codes for outputting trees
 */

/* Minimum number of repetitions of anything we're interested in */
#define TREE_ENC_REP_MIN                4

/* Maximum repetitions for "type A" repetition of zeroes */
/* (min...min+REP_ZERO_FIRST) */
#define TREE_ENC_REP_ZERO_FIRST        16

/* Maximum repetitions for "type B" repetition of zeroes */
/* (min+REP_ZERO_FIRST...min+REP_ZERO_FIRST+REP_ZERO_SECOND) */
#define TREE_ENC_REP_ZERO_SECOND       32

/* Maximum repetitions for "type C" repetition of anything */
/* (min...min_REP_SAME_FIRST) */
#define TREE_ENC_REP_SAME_FIRST         2

/* Bits required to output the above numbers */
#define TREE_ENC_REPZ_FIRST_EXTRA_BITS  4
#define TREE_ENC_REPZ_SECOND_EXTRA_BITS 5
#define TREE_ENC_REP_SAME_EXTRA_BITS    1

/* Number of cfdata frames before E8's are turned off automatically */
#define E8_CFDATA_FRAME_THRESHOLD       32768


/*
 * Block types 
 */
typedef enum
{
        BLOCKTYPE_INVALID       = 0,
        BLOCKTYPE_VERBATIM      = 1, /* normal block */
        BLOCKTYPE_ALIGNED       = 2, /* aligned offset block */
        BLOCKTYPE_UNCOMPRESSED  = 3  /* uncompressed block */
} lzx_block_type;

typedef enum
{
    DEC_STATE_UNKNOWN,
    DEC_STATE_START_NEW_BLOCK,
    DEC_STATE_DECODING_DATA
} decoder_state;

/*
 * Size of uncompressed data chunks
 */
#define CHUNK_SIZE  32768

/*
 * Main tree decoding table parameters 
 */

/* # elements in main tree */
#define MAIN_TREE_ELEMENTS            (NUM_CHARS+(context->dec_num_position_slots<<NL_SHIFT))

/*
 * Decoding table size allows a direct lookup on the first 
 * MAIN_TREE_TABLE_BITS bits of the code (max len 16).
 * Any potential remaining bits are decoded using left/right.
 */
#define MAIN_TREE_TABLE_BITS        10 

/*
 * Secondary length tree decoding table parameters
 * Decoding table size allows a direct lookup on the first
 * SECONDARY_LEN_TREE_TABLE_BITS of the code (max len 16).
 * Any potential remaining bits are decoded using left/right.
 */
#define SECONDARY_LEN_TREE_TABLE_BITS    8 

/* 
 * Aligned offset tree decoding table parameters 
 */
#define ALIGNED_NUM_ELEMENTS    8

/*
 * Must be 7, since we do not use left/right for this tree;
 * everything is decoded in one lookup.
 */
#define ALIGNED_TABLE_BITS        7

/* Declare some external references */
extern const byte dec_extra_bits[];
extern const long MP_POS_minus2[];

/*
 * MAX_MAIN_TREE_ELEMENTS should be >= 256 + 8*num_position_slots
 * (that comes out to 256 + 8*51 right now, for a 2 MB window).
 *
 * Make divisible by 4 so things are longword aligned.
 */
#define MAX_MAIN_TREE_ELEMENTS (256 + (8 * 291))   // 32MB
//#define MAX_MAIN_TREE_ELEMENTS 672

typedef struct
{
    /* pointer to beginning of window buffer */
    byte                *dec_mem_window;

    /* window/decoding buffer parameters */
    ulong               dec_window_size;
    ulong                dec_window_mask;

    /* previous match offsets */
    ulong               dec_last_matchpos_offset[NUM_REPEATED_OFFSETS];

    /* main tree table */
    short                dec_main_tree_table[1 << MAIN_TREE_TABLE_BITS];

    /* secondary length tree table */
    short               dec_secondary_length_tree_table[1 << SECONDARY_LEN_TREE_TABLE_BITS];

    /* main tree bit lengths */
    byte                dec_main_tree_len[MAX_MAIN_TREE_ELEMENTS];

    /* secondary tree bit lengths */
    byte                dec_secondary_length_tree_len[NUM_SECONDARY_LENGTHS];
    byte                pad1[3]; /* NUM_SECONDARY_LENGTHS == 249 */

    /* aligned offset table */
    char                dec_aligned_table[1 << ALIGNED_TABLE_BITS];
    byte                dec_aligned_len[ALIGNED_NUM_ELEMENTS];

    /* left/right pointers for main tree (2*n shorts left, 2*n shorts for right) */
    short               dec_main_tree_left_right[MAX_MAIN_TREE_ELEMENTS*4];

    /* left/right pointers for secondary length tree */
    short               dec_secondary_length_tree_left_right[NUM_SECONDARY_LENGTHS*4];

    /* input (compressed) data pointers */
    byte *              dec_input_curpos;
    byte *              dec_end_input_pos;

    /* output (uncompressed) data pointer */
    byte *              dec_output_buffer;

    /* position in data stream at start of this decode call */
    long                dec_position_at_start;

    /* previous lengths */
    byte                dec_main_tree_prev_len[MAX_MAIN_TREE_ELEMENTS];
    byte                dec_secondary_length_tree_prev_len[NUM_SECONDARY_LENGTHS];

    /* bitwise i/o */
    ulong               dec_bitbuf;
    signed char         dec_bitcount;

    /* number of distinct position (displacement) slots */
    byte                dec_num_position_slots;

    bool                dec_first_time_this_group;
    bool                dec_error_condition;

    /* misc */
    long                  dec_bufpos;
    ulong                dec_current_file_size;
    ulong                dec_instr_pos;
    ulong               dec_num_cfdata_frames;

    /* original size of current block being decoded (in uncompressed bytes) */
    long                dec_original_block_size;

    /* remaining size of current block being decoded (in uncompressed bytes) */
    long                dec_block_size;

    /* type of current block being decoded */
    lzx_block_type        dec_block_type;

    /* current state of decoder */
    decoder_state        dec_decoder_state;

    /* Indicates whether extra long matches are supported */
    bool            dec_extra_long_matches;

} t_decoder_context;

/* Define some XOnline-friendly types */
typedef t_decoder_context    XONLINE_LZXDECODER_CONTEXT;
typedef t_decoder_context    *PXONLINE_LZXDECODER_CONTEXT;

/*
 * decode an element from the aligned offset tree, without checking 
 * for the end of the input data
 */
#define DECODE_ALIGNED_NOEOFCHECK(j) \
    (j) = context->dec_aligned_table[dec_bitbuf >> (32-ALIGNED_TABLE_BITS)]; \
    FILL_BUF_NOEOFCHECK(context->dec_aligned_len[(j)]);


/*
 * Decode an element from the main tree
 * Check for EOF
 */
#define DECODE_MAIN_TREE(j) \
    j = context->dec_main_tree_table[dec_bitbuf >> (32-MAIN_TREE_TABLE_BITS)];    \
    if (j < 0)                                                            \
    {                                                                    \
        ulong mask = (1L << (32-1-MAIN_TREE_TABLE_BITS));               \
        do                                                                \
        {                                                                \
             j = -j;                                                        \
             if (dec_bitbuf & mask)                                        \
                j = context->dec_main_tree_left_right[j*2+1];                   \
            else                                                        \
                j = context->dec_main_tree_left_right[j*2];                     \
            mask >>= 1;                                                    \
        } while (j < 0);                                                \
    }                                                                    \
    FILL_BUF_FULLCHECK(context->dec_main_tree_len[j]);


/*
 * Decode an element from the secondary length tree
 * No checking for EOF
 */
#define DECODE_LEN_TREE_NOEOFCHECK(matchlen) \
    matchlen = context->dec_secondary_length_tree_table[dec_bitbuf >> (32-SECONDARY_LEN_TREE_TABLE_BITS)]; \
    if (matchlen < 0)                                                    \
    {                                                                    \
        ulong mask = (1L << (32-1-SECONDARY_LEN_TREE_TABLE_BITS));      \
        do                                                              \
        {                                                                \
             matchlen = -matchlen;                                          \
             if (dec_bitbuf & mask)                                      \
                matchlen = context->dec_secondary_length_tree_left_right[matchlen*2+1];\
            else                                                        \
                matchlen = context->dec_secondary_length_tree_left_right[matchlen*2];  \
            mask >>= 1;                                                 \
        } while (matchlen < 0);                                            \
    }                                                                    \
    FILL_BUF_NOEOFCHECK(context->dec_secondary_length_tree_len[matchlen]);      \
    matchlen += NUM_PRIMARY_LENGTHS;


/*
 * read n bits from input stream into dest_var, but don't
 * check for EOF
 */
#define GET_BITS_NOEOFCHECK(N,DEST_VAR) \
{                                               \
   DEST_VAR = dec_bitbuf >> (32-(N));            \
   FILL_BUF_NOEOFCHECK((N));                    \
}


/* same as above, but don't check for EOF */
#define GET_BITS17_NOEOFCHECK(N,DEST_VAR) \
{                                               \
   DEST_VAR = dec_bitbuf >> (32-(N));            \
   FILL_BUF17_NOEOFCHECK((N));                    \
}


/*
 * Remove n bits from the input stream
 * handles 1 <= n <= 17
 *
 * FORCE an EOF check ALWAYS, whether or not we read in more
 * bytes from memory.
 *
 * This is used to ensure that we always get an EOF check often enough
 * to not overrun the extra bytes in the buffer.
 *
 * This routine is used ONLY when decoding the main tree element,
 * where we know that the code we read in will be 16 bits or less
 * in length.  Therefore we don't have to check for bitcount going
 * less than zero, twice.
 */
#define FILL_BUF_FULLCHECK(N) \
{                                            \
    if (dec_input_curpos >= dec_end_input_pos)    \
        return -1; \
    dec_bitbuf <<= (N);                        \
    dec_bitcount -= (N);                    \
    if (dec_bitcount <= 0)                  \
    {                                         \
        dec_bitbuf |= ((((ulong) *dec_input_curpos | (((ulong) *(dec_input_curpos+1)) << 8))) << (-dec_bitcount)); \
        dec_input_curpos += 2;              \
        dec_bitcount += 16;                   \
    }                                       \
}

/*
 * Same as above, but no EOF check 
 *
 * This is used when we know we will not run out of input
 */
#define FILL_BUF_NOEOFCHECK(N)             \
{                                        \
    dec_bitbuf <<= (N);                    \
    dec_bitcount -= (N);                \
    if (dec_bitcount <= 0)              \
    {                                     \
        dec_bitbuf |= ((((ulong) *dec_input_curpos | (((ulong) *(dec_input_curpos+1)) << 8))) << (-dec_bitcount)); \
        dec_input_curpos += 2; \
        dec_bitcount += 16;                \
    }                                   \
}

/*
 * Same as above, but handles n=17 bits
 */
#define FILL_BUF17_NOEOFCHECK(N)        \
{                                        \
    dec_bitbuf <<= (N);                    \
    dec_bitcount -= (N);                \
    if (dec_bitcount <= 0)              \
    {                                     \
        dec_bitbuf |= ((((ulong) *dec_input_curpos | (((ulong) *(dec_input_curpos+1)) << 8))) << (-dec_bitcount)); \
        dec_input_curpos += 2; \
        dec_bitcount += 16;                \
        if (dec_bitcount <= 0) \
        { \
            dec_bitbuf |= ((((ulong) *dec_input_curpos | (((ulong) *(dec_input_curpos+1)) << 8))) << (-dec_bitcount)); \
            dec_input_curpos += 2; \
            dec_bitcount += 16;         \
        } \
    }                                   \
}

/* decblk.c */
long NEAR decode_data(t_decoder_context *context, long bytes_to_decode);

/* decin.c */
void NEAR initialise_decoder_bitbuf(t_decoder_context *context);
void NEAR init_decoder_input(t_decoder_context *context);
void NEAR fillbuf(t_decoder_context *context, int n);
ulong NEAR getbits(t_decoder_context *context, int n);

/* decout.c */
void NEAR copy_data_to_output(t_decoder_context *context, long amount, const byte *data);

/* dectree.c */
bool NEAR read_main_and_secondary_trees(t_decoder_context *context);
bool NEAR read_aligned_offset_tree(t_decoder_context *context);

/* maketbl.c */
bool NEAR make_table(
    t_decoder_context *context, 
    int            nchar,
    const byte    *bitlen,
    byte        tablebits,
    short        *table,
    short        *leftright
);

bool NEAR make_table_8bit(t_decoder_context *context, byte *bitlen, byte *table);
    
/* decxlat.c */
void NEAR init_decoder_translation(t_decoder_context *context);
void NEAR decoder_translate_e8(t_decoder_context *context, byte *mem, long bytes);

/* decalign.c */
int NEAR decode_aligned_offset_block(t_decoder_context *context, long bufpos, int amount_to_decode);

/* decverb.c */
int NEAR decode_verbatim_block(t_decoder_context *context, long bufpos, int amount_to_decode);

/* decuncmp.c */
int NEAR decode_uncompressed_block(t_decoder_context *context, long bufpos, int amount_to_decode);
bool NEAR handle_beginning_of_uncompressed_block(t_decoder_context *context);

// FDI -----------------------------------------------------------------------------------

#ifndef INCLUDED_TYPES_FCI_FDI
#define INCLUDED_TYPES_FCI_FDI 1

#ifndef HUGE
#define HUGE
#endif

#ifndef FAR
#define FAR
#endif

#ifndef DIAMONDAPI
#define DIAMONDAPI __cdecl
#endif


//** Specify structure packing explicitly for clients of FDI
#pragma pack(4)

//** Don't redefine types defined in Win16 WINDOWS.H (_INC_WINDOWS)
//   or Win32 WINDOWS.H (_WINDOWS_)
//
#if !defined(_INC_WINDOWS) && !defined(_WINDOWS_)
typedef int            BOOL;     /* f */
typedef unsigned char  BYTE;     /* b */
typedef unsigned int   UINT;     /* ui */
typedef unsigned short USHORT;   /* us */
typedef unsigned long  ULONG;    /* ul */
#endif   // _INC_WINDOWS

typedef unsigned long  CHECKSUM; /* csum */

typedef unsigned long  UOFF;     /* uoff - uncompressed offset */
typedef unsigned long  COFF;     /* coff - cabinet file offset */


#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef NULL
#define NULL    0
#endif


/***    ERF - Error structure
 *
 *  This structure returns error information from FCI/FDI.  The caller should
 *  not modify this structure.
 */
typedef struct {
    int     erfOper;            // FCI/FDI error code -- see FDIERROR_XXX
                                //  and FCIERR_XXX equates for details.

    int     erfType;            // Optional error value filled in by FCI/FDI.
                                // For FCI, this is usually the C run-time
                                // *errno* value.

    BOOL    fError;             // TRUE => error present
} ERF;      /* erf */
typedef ERF FAR *PERF;  /* perf */

#define CB_MAX_CHUNK            32768U
#define CB_MAX_DISK         0x7ffffffL
#define CB_MAX_FILENAME            256
#define CB_MAX_CABINET_NAME        256
#define CB_MAX_CAB_PATH            256
#define CB_MAX_DISK_NAME           256

/***    tcompXXX - Compression types
 *
 *  These are passed to FCIAddFile(), and are also stored in the CFFOLDER
 *  structures in cabinet files.
 *
 *  NOTE: We reserve bits for the TYPE, QUANTUM_LEVEL, and QUANTUM_MEM
 *        to provide room for future expansion.  Since this value is stored
 *        in the CFDATA records in the cabinet file, we don't want to
 *        have to change the format for existing compression configurations
 *        if we add new ones in the future.  This will allows us to read
 *        old cabinet files in the future.
 */

typedef unsigned short TCOMP; /* tcomp */

#define tcompMASK_TYPE          0x000F  // Mask for compression type
#define tcompTYPE_NONE          0x0000  // No compression
#define tcompTYPE_MSZIP         0x0001  // MSZIP
#define tcompTYPE_QUANTUM       0x0002  // Quantum
#define tcompTYPE_LZX           0x0003  // LZX
#define tcompBAD                0x000F  // Unspecified compression type

#define tcompMASK_LZX_WINDOW    0x1F00  // Mask for LZX Compression Memory
#define tcompLZX_WINDOW_LO      0x0F00  // Lowest LZX Memory (15)
#define tcompLZX_WINDOW_HI      0x1500  // Highest LZX Memory (21)
#define tcompSHIFT_LZX_WINDOW        8  // Amount to shift over to get int

#define tcompMASK_QUANTUM_LEVEL 0x00F0  // Mask for Quantum Compression Level
#define tcompQUANTUM_LEVEL_LO   0x0010  // Lowest Quantum Level (1)
#define tcompQUANTUM_LEVEL_HI   0x0070  // Highest Quantum Level (7)
#define tcompSHIFT_QUANTUM_LEVEL     4  // Amount to shift over to get int

#define tcompMASK_QUANTUM_MEM   0x1F00  // Mask for Quantum Compression Memory
#define tcompQUANTUM_MEM_LO     0x0A00  // Lowest Quantum Memory (10)
#define tcompQUANTUM_MEM_HI     0x1500  // Highest Quantum Memory (21)
#define tcompSHIFT_QUANTUM_MEM       8  // Amount to shift over to get int

#define tcompMASK_RESERVED      0xE000  // Reserved bits (high 3 bits)



#define CompressionTypeFromTCOMP(tc) \
            ((tc) & tcompMASK_TYPE)

#define CompressionLevelFromTCOMP(tc) \
            (((tc) & tcompMASK_QUANTUM_LEVEL) >> tcompSHIFT_QUANTUM_LEVEL)

#define CompressionMemoryFromTCOMP(tc) \
            (((tc) & tcompMASK_QUANTUM_MEM) >> tcompSHIFT_QUANTUM_MEM)

#define TCOMPfromTypeLevelMemory(t,l,m)           \
            (((m) << tcompSHIFT_QUANTUM_MEM  ) |  \
             ((l) << tcompSHIFT_QUANTUM_LEVEL) |  \
             ( t                             ))

#define LZXCompressionWindowFromTCOMP(tc) \
            (((tc) & tcompMASK_LZX_WINDOW) >> tcompSHIFT_LZX_WINDOW)

#define TCOMPfromLZXWindow(w)      \
            (((w) << tcompSHIFT_LZX_WINDOW ) |  \
             ( tcompTYPE_LZX ))


//** Revert to default structure packing
#pragma pack()

#endif // !INCLUDED_TYPES_FCI_FDI

/*
 *  Concepts:
 *      A *cabinet* file contains one or more *folders*.  A folder contains
 *      one or more (pieces of) *files*.  A folder is by definition a
 *      decompression unit, i.e., to extract a file from a folder, all of
 *      the data from the start of the folder up through and including the
 *      desired file must be read and decompressed.
 *
 *      A folder can span one (or more) cabinet boundaries, and by implication
 *      a file can also span one (or more) cabinet boundaries.  Indeed, more
 *      than one file can span a cabinet boundary, since FCI concatenates
 *      files together into a single data stream before compressing (actually,
 *      at most one file will span any one cabinet boundary, but FCI does
 *      not know which file this is, since the mapping from uncompressed bytes
 *      to compressed bytes is pretty obscure.  Also, since FCI compresses
 *      in blocks of 32K (at present), any files with data in a 32K block that
 *      spans a cabinet boundary require FDI to read both cabinet files
 *      to get the two halves of the compressed block).
 *
 *  Overview:
 *      The File Decompression Interface is used to simplify the reading of
 *      cabinet files.  A setup program will proceed in a manner very
 *      similar to the pseudo code below.  An FDI context is created, the
 *      setup program calls FDICopy() for each cabinet to be processed.  For
 *      each file in the cabinet, FDICopy() calls a notification callback
 *      routine, asking the setup program if the file should be copied.
 *      This call-back approach is great because it allows the cabinet file
 *      to be read and decompressed in an optimal manner, and also makes FDI
 *      independent of the run-time environment -- FDI makes *no* C run-time
 *      calls whatsoever.  All memory allocation and file I/O functions are
 *      passed into FDI by the client.
 *
 *      main(...)
 *      {
 *          // Read INF file to construct list of desired files.   
 *          //  Ideally, these would be sorted in the same order as the
 *          //  files appear in the cabinets, so that you can just walk
 *          //  down the list in response to fdintCOPY_FILE notifications.
 *
 *          // Construct list of required cabinets. 
 *
 *          hfdi = FDICreate(...);          // Create FDI context
 *          For (cabinet in List of Cabinets) {
 *              FDICopy(hfdi,cabinet,fdiNotify,...);  // Process each cabinet
 *          }
 *          FDIDestroy(hfdi);
 *          ...
 *      }
 *
 *      // Notification callback function 
 *      fdiNotify(fdint,...)
 *      {
 *          If (User Aborted)               // Permit cancellation
 *              if (fdint == fdintCLOSE_FILE_INFO)
 *                  close open file
 *              return -1;
 *          switch (fdint) {
 *              case fdintCOPY_FILE:        // File to copy, maybe
 *                  // Check file against list of desired files 
 *                  if want to copy file
 *                      open destination file and return handle
 *                  else
 *                      return NULL;        // Skip file
 *              case fdintCLOSE_FILE_INFO:
 *                  close file
 *                  set date, time, and attributes
 *
 *              case fdintNEXT_CABINET:
 *                  if not an error callback
 *                      Tell FDI to use suggested directory name
 *                  else
 *                      Tell user what the problem was, and prompt
 *                          for a new disk and/or path.
 *                      if user aborts
 *                          Tell FDI to abort
 *                      else
 *                          return to FDI to try another cabinet
 *
 *              default:
 *                  return 0;               // more messages may be defined
 *              ...
 *      }
 *
 *  Error Handling Suggestions:
 *      Since you the client have passed in *all* of the functions that
 *      FDI uses to interact with the "outside" world, you are in prime
 *      position to understand and deal with errors.
 *
 *      The general philosophy of FDI is to pass all errors back up to
 *      the client.  FDI returns fairly generic error codes in the case
 *      where one of the callback functions (PFNOPEN, PFNREAD, etc.) fail,
 *      since it assumes that the callback function will save enough
 *      information in a static/global so that when FDICopy() returns
 *      fail, the client can examine this information and report enough
 *      detail about the problem that the user can take corrective action.
 *
 *      For very specific errors (CORRUPT_CABINET, for example), FDI returns
 *      very specific error codes.
 *
 *      THE BEST POLICY IS FOR YOUR CALLBACK ROUTINES TO AVOID RETURNING
 *      ERRORS TO FDI!
 *
 *      Examples:
 *          (1) If the disk is getting full, instead of returning an error
 *              from your PFNWRITE function, you should -- inside your
 *              PFNWRITE function -- put up a dialog telling the user to free
 *              some disk space.
 *          (2) When you get the fdintNEXT_CABINET notification, you should
 *              verify that the cabinet you return is the correct one (call
 *              FDIIsCabinet(), and make sure the setID matches the one for
 *              the current cabinet specified in the fdintCABINET_INFO, and
 *              that the disk number is one greater.
 *
 *              NOTE: FDI will continue to call fdintNEXT_CABINET until it
 *                    gets the cabinet it wants, or until you return -1
 *                    to abort the FDICopy() call.
 *
 *      The documentation below on the FDI error codes provides explicit
 *      guidance on how to avoid each error.
 *
 *      If you find you must return a failure to FDI from one of your
 *      callback functions, then FDICopy() frees all resources it allocated
 *      and closes all files.  If you can figure out how to overcome the
 *      problem, you can call FDICopy() again on the last cabinet, and
 *      skip any files that you already copied.  But, note that FDI does
 *      *not* maintain any state between FDICopy() calls, other than possibly
 *      memory allocated for the decompressor.
 *
 *      See FDIERROR for details on FDI error codes and recommended actions.
 *
 *
 *  Progress Indicator Suggestions:
 *      As above, all of the file I/O functions are supplied by you.  So,
 *      updating a progress indicator is very simple.  You keep track of
 *      the target files handles you have opened, along with the uncompressed
 *      size of the target file.  When you see writes to the handle of a
 *      target file, you use the write count to update your status!
 *      Since this method is available, there is no separate callback from
 *      FDI just for progess indication.
 */

//** Specify structure packing explicitly for clients of FDI
#pragma pack(4)


/***    FDIERROR - Error codes returned in erf.erfOper field
 *
 *  In general, FDI will only fail if one of the passed in memory or
 *  file I/O functions fails.  Other errors are pretty unlikely, and are
 *  caused by corrupted cabinet files, passing in a file which is not a
 *  cabinet file, or cabinet files out of order.
 *
 *  Description:    Summary of error.
 *  Cause:          List of possible causes of this error.
 *  Response:       How client might respond to this error, or avoid it in
 *                  the first place.
 */
typedef enum {
    FDIERROR_NONE,
        // Description: No error
        // Cause:       Function was successfull.
        // Response:    Keep going!

    FDIERROR_CABINET_NOT_FOUND,
        // Description: Cabinet not found
        // Cause:       Bad file name or path passed to FDICopy(), or returned
        //              to fdintNEXT_CABINET.
        // Response:    To prevent this error, validate the existence of the
        //              the cabinet *before* passing the path to FDI.

    FDIERROR_NOT_A_CABINET,
        // Description: Cabinet file does not have the correct format
        // Cause:       File passed to to FDICopy(), or returned to
        //              fdintNEXT_CABINET, is too small to be a cabinet file,
        //              or does not have the cabinet signature in its first
        //              four bytes.
        // Response:    To prevent this error, call FDIIsCabinet() to check a
        //              cabinet before calling FDICopy() or returning the
        //              cabinet path to fdintNEXT_CABINET.

    FDIERROR_UNKNOWN_CABINET_VERSION,
        // Description: Cabinet file has an unknown version number.
        // Cause:       File passed to to FDICopy(), or returned to
        //              fdintNEXT_CABINET, has what looks like a cabinet file
        //              header, but the version of the cabinet file format
        //              is not one understood by this version of FDI.  The
        //              erf.erfType field is filled in with the version number
        //              found in the cabinet file.
        // Response:    To prevent this error, call FDIIsCabinet() to check a
        //              cabinet before calling FDICopy() or returning the
        //              cabinet path to fdintNEXT_CABINET.

    FDIERROR_CORRUPT_CABINET,
        // Description: Cabinet file is corrupt
        // Cause:       FDI returns this error any time it finds a problem
        //              with the logical format of a cabinet file, and any
        //              time one of the passed-in file I/O calls fails when
        //              operating on a cabinet (PFNOPEN, PFNSEEK, PFNREAD,
        //              or PFNCLOSE).  The client can distinguish these two
        //              cases based upon whether the last file I/O call
        //              failed or not.
        // Response:    Assuming this is not a real corruption problem in
        //              a cabinet file, the file I/O functions could attempt
        //              to do retries on failure (for example, if there is a
        //              temporary network connection problem).  If this does
        //              not work, and the file I/O call has to fail, then the
        //              FDI client will have to clean up and call the
        //              FDICopy() function again.

    FDIERROR_ALLOC_FAIL,
        // Description: Could not allocate enough memory
        // Cause:       FDI tried to allocate memory with the PFNALLOC
        //              function, but it failed.
        // Response:    If possible, PFNALLOC should take whatever steps
        //              are possible to allocate the memory requested.  If
        //              memory is not immediately available, it might post a
        //              dialog asking the user to free memory, for example.
        //              Note that the bulk of FDI's memory allocations are
        //              made at FDICreate() time and when the first cabinet
        //              file is opened during FDICopy().

    FDIERROR_BAD_COMPR_TYPE,
        // Description: Unknown compression type in a cabinet folder
        // Cause:       [Should never happen.]  A folder in a cabinet has an
        //              unknown compression type.  This is probably caused by
        //              a mismatch between the version of FCI.LIB used to
        //              create the cabinet and the FDI.LIB used to read the
        //              cabinet.
        // Response:    Abort.

    FDIERROR_MDI_FAIL,
        // Description: Failure decompressing data from a cabinet file
        // Cause:       The decompressor found an error in the data coming
        //              from the file cabinet.  The cabinet file was corrupted.
        //              [11-Apr-1994 bens When checksuming is turned on, this
        //              error should never occur.]
        // Response:    Probably should abort; only other choice is to cleanup
        //              and call FDICopy() again, and hope there was some
        //              intermittent data error that will not reoccur.

    FDIERROR_TARGET_FILE,
        // Description: Failure writing to target file
        // Cause:       FDI returns this error any time it gets an error back
        //              from one of the passed-in file I/O calls fails when
        //              writing to a file being extracted from a cabinet.
        // Response:    To avoid or minimize this error, the file I/O functions
        //              could attempt to avoid failing.  A common cause might
        //              be disk full -- in this case, the PFNWRITE function
        //              could have a check for free space, and put up a dialog
        //              asking the user to free some disk space.

    FDIERROR_RESERVE_MISMATCH,
        // Description: Cabinets in a set do not have the same RESERVE sizes
        // Cause:       [Should never happen]. FDI requires that the sizes of
        //              the per-cabinet, per-folder, and per-data block
        //              RESERVE sections be consistent across all the cabinets
        //              in a set.
        // Response:    Abort.

    FDIERROR_WRONG_CABINET,
        // Description: Cabinet returned on fdintNEXT_CABINET is incorrect
        // Cause:       NOTE: THIS ERROR IS NEVER RETURNED BY FDICopy()!
        //              Rather, FDICopy() keeps calling the fdintNEXT_CABINET
        //              callback until either the correct cabinet is specified,
        //              or you return ABORT.
        //              When FDICopy() is extracting a file that crosses a
        //              cabinet boundary, it calls fdintNEXT_CABINET to ask
        //              for the path to the next cabinet.  Not being very
        //              trusting, FDI then checks to make sure that the
        //              correct continuation cabinet was supplied!  It does
        //              this by checking the "setID" and "iCabinet" fields
        //              in the cabinet.  When MAKECAB.EXE creates a set of
        //              cabinets, it constructs the "setID" using the sum
        //              of the bytes of all the destination file names in
        //              the cabinet set.  FDI makes sure that the 16-bit
        //              setID of the continuation cabinet matches the
        //              cabinet file just processed.  FDI then checks that
        //              the cabinet number (iCabinet) is one more than the
        //              cabinet number for the cabinet just processed.
        // Response:    You need code in your fdintNEXT_CABINET (see below)
        //              handler to do retries if you get recalled with this
        //              error.  See the sample code (EXTRACT.C) to see how
        //              this should be handled.

    FDIERROR_USER_ABORT,
        // Description: FDI aborted.
        // Cause:       An FDI callback returnd -1 (usually).
        // Response:    Up to client.

} FDIERROR;


/*
 * FAT file attribute flag used by FCI/FDI to indicate that
 * the filename in the CAB is a UTF string
 */
#ifndef _A_NAME_IS_UTF
#define _A_NAME_IS_UTF  0x80
#endif

/*
 * FAT file attribute flag used by FCI/FDI to indicate that
 * the file should be executed after extraction
 */
#ifndef _A_EXEC
#define _A_EXEC         0x40
#endif


/***    HFDI - Handle to an FDI context
 *
 *  FDICreate() creates this, and it must be passed to all other FDI
 *  functions.
 */
typedef void FAR *HFDI; /* hfdi */


/***    FDICABINETINFO - Information about a cabinet
 *
 */
typedef struct {
    long        cbCabinet;              // Total length of cabinet file
    USHORT      cFolders;               // Count of folders in cabinet
    USHORT      cFiles;                 // Count of files in cabinet
    USHORT      setID;                  // Cabinet set ID
    USHORT      iCabinet;               // Cabinet number in set (0 based)
    BOOL        fReserve;               // TRUE => RESERVE present in cabinet
    BOOL        hasprev;                // TRUE => Cabinet is chained prev
    BOOL        hasnext;                // TRUE => Cabinet is chained next
} FDICABINETINFO; /* fdici */
typedef FDICABINETINFO FAR *PFDICABINETINFO; /* pfdici */


/***    FDIDECRYPTTYPE - PFNFDIDECRYPT command types
 *
 */
typedef enum {
    fdidtNEW_CABINET,                   // New cabinet
    fdidtNEW_FOLDER,                    // New folder
    fdidtDECRYPT,                       // Decrypt a data block
} FDIDECRYPTTYPE; /* fdidt */


/***    FDIDECRYPT - Data for PFNFDIDECRYPT function
 *
 */
typedef struct {
    FDIDECRYPTTYPE    fdidt;            // Command type (selects union below)
    void FAR         *pvUser;           // Decryption context
    union {
        struct {                        // fdidtNEW_CABINET
            void FAR *pHeaderReserve;   // RESERVE section from CFHEADER
            USHORT    cbHeaderReserve;  // Size of pHeaderReserve
            USHORT    setID;            // Cabinet set ID
            int       iCabinet;         // Cabinet number in set (0 based)
        } cabinet;

        struct {                        // fdidtNEW_FOLDER
            void FAR *pFolderReserve;   // RESERVE section from CFFOLDER
            USHORT    cbFolderReserve;  // Size of pFolderReserve
            USHORT    iFolder;          // Folder number in cabinet (0 based)
        } folder;

        struct {                        // fdidtDECRYPT
            void FAR *pDataReserve;     // RESERVE section from CFDATA
            USHORT    cbDataReserve;    // Size of pDataReserve
            void FAR *pbData;           // Data buffer
            USHORT    cbData;           // Size of data buffer
            BOOL      fSplit;           // TRUE if this is a split data block
            USHORT    cbPartial;        // 0 if this is not a split block, or
                                        //  the first piece of a split block;
                                        // Greater than 0 if this is the
                                        //  second piece of a split block.
        } decrypt;
    };
} FDIDECRYPT; /* fdid */
typedef FDIDECRYPT FAR *PFDIDECRYPT; /* pfdid */


/***    FNALLOC - Memory Allocation
 *      FNFREE  - Memory Free
 *
 *  These are modeled after the C run-time routines malloc() and free()
 *  FDI expects error handling to be identical to these C run-time routines.
 *
 *  As long as you faithfully copy the semantics of malloc() and free(),
 *  you can supply any functions you like!
 *
 *  WARNING: You should never assume anything about the sequence of
 *           PFNALLOC and PFNFREE calls -- incremental releases of
 *           FDI may have radically different numbers of
 *           PFNALLOC calls and allocation sizes!
 */
//** Memory functions for FDI
typedef void HUGE * (FAR DIAMONDAPI *PFNALLOC)(ULONG cb); /* pfna */
#define FNALLOC(fn) void HUGE * FAR DIAMONDAPI fn(ULONG cb)

typedef void (FAR DIAMONDAPI *PFNFREE)(void HUGE *pv); /* pfnf */
#define FNFREE(fn) void FAR DIAMONDAPI fn(void HUGE *pv)


/***    PFNOPEN  - File I/O callbacks for FDI
 *      PFNREAD
 *      PFNWRITE
 *      PFNCLOSE
 *      PFNSEEK
 *
 *  These are modeled after the C run-time routines _open, _read,
 *  _write, _close, and _lseek.  The values for the PFNOPEN oflag
 *  and pmode calls are those defined for _open.  FDI expects error
 *  handling to be identical to these C run-time routines.
 *
 *  As long as you faithfully copy these aspects, you can supply
 *  any functions you like!
 *
 *  WARNING: You should never assume you know what file is being
 *           opened at any one point in time!  FDI will usually
 *           stick to opening cabinet files, but it is possible
 *           that in a future implementation it may open temporary
 *           files or open cabinet files in a different order.
 *
 *  Notes for Memory Mapped File fans:
 *      You can write wrapper routines to allow FDI to work on memory
 *      mapped files.  You'll have to create your own "handle" type so that
 *      you can store the base memory address of the file and the current
 *      seek position, and then you'll allocate and fill in one of these
 *      structures and return a pointer to it in response to the PFNOPEN
 *      call and the fdintCOPY_FILE call.  Your PFNREAD and PFNWRITE
 *      functions will do memcopy(), and update the seek position in your
 *      "handle" structure.  PFNSEEK will just change the seek position
 *      in your "handle" structure.
 */
//** File I/O functions for FDI
typedef int  (FAR DIAMONDAPI *PFNOPEN) (char FAR *pszFile, int oflag, int pmode);
typedef UINT (FAR DIAMONDAPI *PFNREAD) (int hf, void FAR *pv, UINT cb);
typedef UINT (FAR DIAMONDAPI *PFNWRITE)(int hf, void FAR *pv, UINT cb);
typedef int  (FAR DIAMONDAPI *PFNCLOSE)(int hf);
typedef long (FAR DIAMONDAPI *PFNSEEK) (int hf, long dist, int seektype);

#define FNOPEN(fn) int FAR DIAMONDAPI fn(char FAR *pszFile, int oflag, int pmode)
#define FNREAD(fn) UINT FAR DIAMONDAPI fn(int hf, void FAR *pv, UINT cb)
#define FNWRITE(fn) UINT FAR DIAMONDAPI fn(int hf, void FAR *pv, UINT cb)
#define FNCLOSE(fn) int FAR DIAMONDAPI fn(int hf)
#define FNSEEK(fn) long FAR DIAMONDAPI fn(int hf, long dist, int seektype)



/***    PFNFDIDECRYPT - FDI Decryption callback
 *
 *  If this function is passed on the FDICopy() call, then FDI calls it
 *  at various times to update the decryption state and to decrypt FCDATA
 *  blocks.
 *
 *  Common Entry Conditions:
 *      pfdid->fdidt  - Command type
 *      pfdid->pvUser - pvUser value from FDICopy() call
 *
 *  fdidtNEW_CABINET:   //** Notification of a new cabinet
 *      Entry:
 *        pfdid->cabinet.
 *          pHeaderReserve  - RESERVE section from CFHEADER
 *          cbHeaderReserve - Size of pHeaderReserve
 *          setID           - Cabinet set ID
 *          iCabinet        - Cabinet number in set (0 based)
 *      Exit-Success:
 *          returns anything but -1;
 *      Exit-Failure:
 *          returns -1; FDICopy() is aborted.
 *      Notes:
 *      (1) This call allows the decryption code to pick out any information
 *          from the cabinet header reserved area (placed there by DIACRYPT)
 *          needed to perform decryption.  If there is no such information,
 *          this call would presumably be ignored.
 *      (2) This call is made very soon after fdintCABINET_INFO.
 *
 *  fdidtNEW_FOLDER:    //** Notification of a new folder
 *      Entry:
 *        pfdid->folder.
 *          pFolderReserve  - RESERVE section from CFFOLDER
 *          cbFolderReserve - Size of pFolderReserve
 *          iFolder         - Folder number in cabinet (0 based)
 *      Exit-Success:
 *          returns anything but -1;
 *      Exit-Failure:
 *          returns -1; FDICopy() is aborted.
 *      Notes:
 *          This call allows the decryption code to pick out any information
 *          from the folder reserved area (placed there by DIACRYPT) needed
 *          to perform decryption.  If there is no such information, this
 *          call would presumably be ignored.
 *
 *  fdidtDECRYPT:       //** Decrypt a data buffer
 *      Entry:
 *        pfdid->folder.
 *          pDataReserve  - RESERVE section for this CFDATA block
 *          cbDataReserve - Size of pDataReserve
 *          pbData        - Data buffer
 *          cbData        - Size of data buffer
 *          fSplit        - TRUE if this is a split data block
 *          cbPartial     - 0 if this is not a split block, or the first
 *                              piece of a split block; Greater than 0 if
 *                              this is the second piece of a split block.
 *      Exit-Success:
 *          returns TRUE;
 *      Exit-Failure:
 *          returns FALSE; error during decrypt
 *          returns -1; FDICopy() is aborted.
 *      Notes:
 *          FCI will split CFDATA blocks across cabinet boundaries if
 *          necessary.  To provide maximum flexibility, FDI will call the
 *          fdidtDECRYPT function twice on such split blocks, once when
 *          the first portion is read, and again when the second portion
 *          is read.  And, of course, most data blocks will not be split.
 *          So, there are three cases:
 *
 *           1) fSplit == FALSE
 *              You have the entire data block, so decrypt it.
 *
 *           2) fSplit == TRUE, cbPartial == 0
 *              This is the first portion of a split data block, so cbData
 *              is the size of this portion.  You can either choose to decrypt
 *              this piece, or ignore this call and decrypt the full CFDATA
 *              block on the next (second) fdidtDECRYPT call.
 *
 *           3) fSplit == TRUE, cbPartial > 0
 *              This is the second portion of a split data block (indeed,
 *              cbPartial will have the same value as cbData did on the
 *              immediately preceeding fdidtDECRYPT call!).  If you decrypted
 *              the first portion on the first call, then you can decrypt the
 *              second portion now.  If you ignored the first call, then you
 *              can decrypt the entire buffer.
 *              NOTE: pbData points to the second portion of the split data
 *                    block in this case, *not* the entire data block.  If
 *                    you want to wait until the second piece to decrypt the
 *                    *entire* block, pbData-cbPartial is the address of the
 *                    start of the whole block, and cbData+cbPartial is its
 *                    size.
 */
typedef int (FAR DIAMONDAPI *PFNFDIDECRYPT)(PFDIDECRYPT pfdid); /* pfnfdid */
#define FNFDIDECRYPT(fn) int FAR DIAMONDAPI fn(PFDIDECRYPT pfdid)


/***    FDINOTIFICATION - Notification structure for PFNFDINOTIFY
 *
 *  See the FDINOTIFICATIONTYPE definition for information on usage and
 *  meaning of these fields.
 */
typedef struct {
// long fields
    long      cb;
    char FAR *psz1;
    char FAR *psz2;
    char FAR *psz3;                     // Points to a 256 character buffer
    void FAR *pv;                       // Value for client

// int fields
    int       hf;

// short fields
    USHORT    date;
    USHORT    time;
    USHORT    attribs;

    USHORT    setID;                    // Cabinet set ID
    USHORT    iCabinet;                 // Cabinet number (0-based)
    USHORT    iFolder;                  // Folder number (0-based)

    FDIERROR  fdie;
} FDINOTIFICATION, FAR *PFDINOTIFICATION;  /* fdin, pfdin */


/***    FDINOTIFICATIONTYPE - FDICopy notification types
 *
 *  The notification function for FDICopy can be called with the following
 *  values for the fdint parameter.  In all cases, the pfdin->pv field is
 *  filled in with the value of the pvUser argument passed in to FDICopy().
 *
 *  A typical sequence of calls will be something like this:
 *      fdintCABINET_INFO     // Info about the cabinet
 *      fdintENUMERATE        // Starting enumeration
 *      fdintPARTIAL_FILE     // Only if this is not the first cabinet, and
 *                            // one or more files were continued from the
 *                            // previous cabinet.
 *      ...
 *      fdintPARTIAL_FILE
 *      fdintCOPY_FILE        // The first file that starts in this cabinet
 *      ...
 *      fdintCOPY_FILE        // Now let's assume you want this file...
 *      // PFNWRITE called multiple times to write to this file.
 *      fdintCLOSE_FILE_INFO  // File done, set date/time/attributes
 *
 *      fdintCOPY_FILE        // Now let's assume you want this file...
 *      // PFNWRITE called multiple times to write to this file.
 *      fdintNEXT_CABINET     // File was continued to next cabinet!
 *      fdintCABINET_INFO     // Info about the new cabinet
 *      // PFNWRITE called multiple times to write to this file.
 *      fdintCLOSE_FILE_INFO  // File done, set date/time/attributes
 *      ...
 *      fdintENUMERATE        // Ending enumeration
 *
 *  fdintCABINET_INFO:
 *        Called exactly once for each cabinet opened by FDICopy(), including
 *        continuation cabinets opened due to file(s) spanning cabinet
 *        boundaries. Primarily intended to permit EXTRACT.EXE to
 *        automatically select the next cabinet in a cabinet sequence even if
 *        not copying files that span cabinet boundaries.
 *      Entry:
 *          pfdin->psz1     = name of next cabinet
 *          pfdin->psz2     = name of next disk
 *          pfdin->psz3     = cabinet path name
 *          pfdin->setID    = cabinet set ID (a random 16-bit number)
 *          pfdin->iCabinet = Cabinet number within cabinet set (0-based)
 *      Exit-Success:
 *          Return anything but -1
 *      Exit-Failure:
 *          Returns -1 => Abort FDICopy() call
 *      Notes:
 *          This call is made *every* time a new cabinet is examined by
 *          FDICopy().  So if "foo2.cab" is examined because a file is
 *          continued from "foo1.cab", and then you call FDICopy() again
 *          on "foo2.cab", you will get *two* fdintCABINET_INFO calls all
 *          told.
 *
 *  fdintCOPY_FILE:
 *        Called for each file that *starts* in the current cabinet, giving
 *        the client the opportunity to request that the file be copied or
 *        skipped.
 *      Entry:
 *          pfdin->psz1    = file name in cabinet
 *          pfdin->cb      = uncompressed size of file
 *          pfdin->date    = file date
 *          pfdin->time    = file time
 *          pfdin->attribs = file attributes
 *          pfdin->iFolder = file's folder index
 *      Exit-Success:
 *          Return non-zero file handle for destination file; FDI writes
 *          data to this file use the PFNWRITE function supplied to FDICreate,
 *          and then calls fdintCLOSE_FILE_INFO to close the file and set
 *          the date, time, and attributes.  NOTE: This file handle returned
 *          must also be closeable by the PFNCLOSE function supplied to
 *          FDICreate, since if an error occurs while writing to this handle,
 *          FDI will use the PFNCLOSE function to close the file so that the
 *          client may delete it.
 *      Exit-Failure:
 *          Returns 0  => Skip file, do not copy
 *          Returns -1 => Abort FDICopy() call
 *
 *  fdintCLOSE_FILE_INFO:
 *        Called after all of the data has been written to a target file.
 *        This function must close the file and set the file date, time,
 *        and attributes.
 *      Entry:
 *          pfdin->psz1    = file name in cabinet
 *          pfdin->hf      = file handle
 *          pfdin->date    = file date
 *          pfdin->time    = file time
 *          pfdin->attribs = file attributes
 *          pfdin->iFolder = file's folder index
 *          pfdin->cb      = Run After Extract (0 - don't run, 1 Run)
 *      Exit-Success:
 *          Returns TRUE
 *      Exit-Failure:
 *          Returns FALSE, or -1 to abort;
 *
 *              IMPORTANT NOTE IMPORTANT:
 *                  pfdin->cb is overloaded to no longer be the size of
 *                  the file but to be a binary indicated run or not
 *
 *              IMPORTANT NOTE:
 *                  FDI assumes that the target file was closed, even if this
 *                  callback returns failure.  FDI will NOT attempt to use
 *                  the PFNCLOSE function supplied on FDICreate() to close
 *                  the file!
 *
 *  fdintPARTIAL_FILE:
 *        Called for files at the front of the cabinet that are CONTINUED
 *        from a previous cabinet.  This callback occurs only when FDICopy is
 *        started on second or subsequent cabinet in a series that has files
 *        continued from a previous cabinet.
 *      Entry:
 *          pfdin->psz1 = file name of file CONTINUED from a PREVIOUS cabinet
 *          pfdin->psz2 = name of cabinet where file starts
 *          pfdin->psz3 = name of disk where file starts
 *      Exit-Success:
 *          Return anything other than -1; enumeration continues
 *      Exit-Failure:
 *          Returns -1 => Abort FDICopy() call
 *
 *  fdintENUMERATE:
 *        Called once after a call to FDICopy() starts scanning a CAB's
 *        CFFILE entries, and again when there are no more CFFILE entries.
 *        If CAB spanning occurs, an additional call will occur after the
 *        first spanned file is completed.  If the pfdin->iFolder value is
 *        changed from zero, additional calls will occur next time it reaches
 *        zero.  If iFolder is changed to zero, FDICopy will terminate, as if
 *        there were no more CFFILE entries.  Primarily intended to allow an
 *        application with it's own file list to help FDI advance quickly to
 *        a CFFILE entry of interest.  Can also be used to allow an
 *        application to determine the cb values for each file in the CAB.
 *      Entry:
 *        pfdin->cb        = current CFFILE position
 *        pfdin->iFolder   = number of files remaining
 *        pfdin->setID     = current CAB's setID value
 *      Exit-Don't Care:
 *        Don't change anything.
 *        Return anything but -1.
 *      Exit-Forcing a skip:
 *        pfdin->cb        = desired CFFILE position
 *        pfdin->iFolder   = desired # of files remaining
 *        Return anything but -1.
 *      Exit-Stop:
 *        pfdin->iFolder    = set to 0
 *        Return anything but -1.
 *      Exit-Failure:
 *        Return -1 => Abort FDICopy call ("user aborted".)
 *      Notes:
 *        This call can be ignored by applications which want normal file
 *        searching.  The application can adjust the supplied values to
 *        force FDICopy() to continue it's search at another location, or
 *        to force FDICopy() to terminate the search, by setting iFolder to 0.
 *        (FDICopy() will report no error when terminated this way.)
 *        FDI has no means to verify the supplied cb or iFolder values.
 *        Arbitrary values are likely to cause undesirable results.  An
 *        application should cross-check pfdin->setID to be certain the
 *        external database is in sync with the CAB.  Reverse-skips are OK
 *        (but may be inefficient) unless fdintNEXT_CABINET has been called.
 *
 *  fdintNEXT_CABINET:
 *        This function is *only* called when fdintCOPY_FILE was told to copy
 *        a file in the current cabinet that is continued to a subsequent
 *        cabinet file.  It is important that the cabinet path name (psz3)
 *        be validated before returning!  This function should ensure that
 *        the cabinet exists and is readable before returning.  So, this
 *        is the function that should, for example, issue a disk change
 *        prompt and make sure the cabinet file exists.
 *
 *        When this function returns to FDI, FDI will check that the setID
 *        and iCabinet match the expected values for the next cabinet.
 *        If not, FDI will continue to call this function until the correct
 *        cabinet file is specified, or until this function returns -1 to
 *        abort the FDICopy() function.  pfdin->fdie is set to
 *        FDIERROR_WRONG_CABINET to indicate this case.
 *
 *        If you *haven't* ensured that the cabinet file is present and
 *        readable, or the cabinet file has been damaged, pfdin->fdie will
 *        receive other appropriate error codes:
 *
 *              FDIERROR_CABINET_NOT_FOUND
 *              FDIERROR_NOT_A_CABINET
 *              FDIERROR_UNKNOWN_CABINET_VERSION
 *              FDIERROR_CORRUPT_CABINET
 *              FDIERROR_BAD_COMPR_TYPE
 *              FDIERROR_RESERVE_MISMATCH
 *              FDIERROR_WRONG_CABINET
 *
 *      Entry:
 *          pfdin->psz1 = name of next cabinet where current file is continued
 *          pfdin->psz2 = name of next disk where current file is continued
 *          pfdin->psz3 = cabinet path name; FDI concatenates psz3 with psz1
 *                          to produce the fully-qualified path for the cabinet
 *                          file.  The 256-byte buffer pointed at by psz3 may
 *                          be modified, but psz1 may not!
 *          pfdin->fdie = FDIERROR_WRONG_CABINET if the previous call to
 *                        fdintNEXT_CABINET specified a cabinet file that
 *                        did not match the setID/iCabinet that was expected.
 *      Exit-Success:
 *          Return anything but -1
 *      Exit-Failure:
 *          Returns -1 => Abort FDICopy() call
 *      Notes:
 *          This call is almost always made when a target file is open and
 *          being written to, and the next cabinet is needed to get more
 *          data for the file.
 */
typedef enum {
    fdintCABINET_INFO,              // General information about cabinet
    fdintPARTIAL_FILE,              // First file in cabinet is continuation
    fdintCOPY_FILE,                 // File to be copied
    fdintCLOSE_FILE_INFO,           // close the file, set relevant info
    fdintNEXT_CABINET,              // File continued to next cabinet
    fdintENUMERATE,                 // Enumeration status
} FDINOTIFICATIONTYPE; /* fdint */

typedef int (FAR DIAMONDAPI *PFNFDINOTIFY)(FDINOTIFICATIONTYPE fdint,
                                           PFDINOTIFICATION    pfdin); /* pfnfdin */

#define FNFDINOTIFY(fn) int FAR DIAMONDAPI fn(FDINOTIFICATIONTYPE fdint, \
                                              PFDINOTIFICATION    pfdin)


/*** cpuType values for FDICreate()
 *
 *  (Ignored by 32-bit FDI.)
 */
#define     cpuUNKNOWN         (-1)    /* FDI does detection */
#define     cpu80286           (0)     /* '286 opcodes only */
#define     cpu80386           (1)     /* '386 opcodes used */


/***    FDICreate - Create an FDI context
 *
 *  Entry:
 *      pfnalloc
 *      pfnfree
 *      pfnopen
 *      pfnread
 *      pfnwrite
 *      pfnclose
 *      pfnlseek
 *      cpuType  - Select CPU type (auto-detect, 286, or 386+)
 *                 NOTE: For the 32-bit FDI.LIB, this parameter is ignored!
 *      perf
 *
 *  Exit-Success:
 *      Returns non-NULL FDI context handle.
 *
 *  Exit-Failure:
 *      Returns NULL; perf filled in with error code
 *
 */
HFDI FAR DIAMONDAPI FDICreate(PFNALLOC pfnalloc,
                              PFNFREE  pfnfree,
                              PFNOPEN  pfnopen,
                              PFNREAD  pfnread,
                              PFNWRITE pfnwrite,
                              PFNCLOSE pfnclose,
                              PFNSEEK  pfnseek,
                              int      cpuType,
                              PERF     perf);


/***    FDIIsCabinet - Determines if file is a cabinet, returns info if it is
 *
 *  Entry:
 *      hfdi   - Handle to FDI context (created by FDICreate())
 *      hf     - File handle suitable for PFNREAD/PFNSEEK, positioned
 *               at offset 0 in the file to test.
 *      pfdici - Buffer to receive info about cabinet if it is one.
 *
 *  Exit-Success:
 *      Returns TRUE; file is a cabinet, pfdici filled in.
 *
 *  Exit-Failure:
 *      Returns FALSE, file is not a cabinet;  If an error occurred,
 *          perf (passed on FDICreate call!) filled in with error.
 */
BOOL FAR DIAMONDAPI FDIIsCabinet(HFDI            hfdi,
                                 int             hf,
                                 PFDICABINETINFO pfdici);


/***    FDICopy - extracts files from a cabinet
 *
 *  Entry:
 *      hfdi        - handle to FDI context (created by FDICreate())
 *      pszCabinet  - main name of cabinet file
 *      pszCabPath  - Path to cabinet file(s)
 *      flags       - Flags to modify behavior
 *      pfnfdin     - Notification function
 *      pfnfdid     - Decryption function (pass NULL if not used)
 *      pvUser      - User specified value to pass to notification function
 *
 *  Exit-Success:
 *      Returns TRUE;
 *
 *  Exit-Failure:
 *      Returns FALSE, perf (passed on FDICreate call!) filled in with
 *          error.
 *
 *  Notes:
 *  (1) If FDICopy() fails while a target file is being written out, then
 *      FDI will use the PFNCLOSE function to close the file handle for that
 *      target file that was returned from the fdintCOPY_FILE notification.
 *      The client application is then free to delete the target file, since
 *      it will not be in a valid state (since there was an error while
 *      writing it out).
 */
BOOL FAR DIAMONDAPI FDICopy(HFDI          hfdi,
                            char FAR     *pszCabinet,
                            char FAR     *pszCabPath,
                            int           flags,
                            PFNFDINOTIFY  pfnfdin,
                            PFNFDIDECRYPT pfnfdid,
                            void FAR     *pvUser);


/***    FDIDestroy - Destroy an FDI context
 *
 *  Entry:
 *      hfdi - handle to FDI context (created by FDICreate())
 *
 *  Exit-Success:
 *      Returns TRUE;
 *
 *  Exit-Failure:
 *      Returns FALSE;
 */
BOOL FAR DIAMONDAPI FDIDestroy(HFDI hfdi);


//** Revert to default structure packing
#pragma pack()

// CFCACHE -------------------------------------------------------------------------------

//
// We don't keep all CFFILE records in memory, define a cache.
// CFFILE entries are not fixed-size, so we will have to parse a byte
// buffer ...
//
// We choose a seemingly unusual number because async disk reads must be
// sector-aligned (512 bytes). However, since CFFILEs are variable sized,
// it is not likely that we will completely use up the buffer the every
// time. Since the cost of not utilizing the whole buffer is quite steep
// (minimum cost is 512 bytes), I am adding an additional 64 bytes to
// accomodate for leftovers. 64 is chosen because I expect a typical 
// CFFILE entry to be about that size. This should give us 4 full sectors
// plus an incomplete CFFILE entry.
//
#define CFFILECACHE_SIZE        (2048 + 64)

//
// Define a CFFILE cache
//
typedef struct
{
    HANDLE          hFile;            // File handle to read from
    ULARGE_INTEGER  uliReadOffset;    // Offset to read from file
    PBYTE           pbBuffer;        // Buffer pointer
    DWORD           cbBuffer;        // Buffer length
    WORD            wCurrentIndex;    // Index of current CFFILE in cache
    WORD            wCffiles;        // Number of CFFILES in package
    WORD            wCursor;        // Current offset in the buffer
    WORD            wDataSize;        // Bytes of data in the buffer
    WORD            wAlignment;        // Alignment padding
    BYTE            bFlags;            // Informational flags
    BYTE            bPadding;        // Special buffer padding
    BYTE            rgbBuffer[CFFILECACHE_SIZE];
                                    // Default data buffer
    XONLINESYMMDEC_CONTEXT    symmdec; // Decryption context                    

    XONLINETASK_FILEIO        fileio;  // Async file I/O context
    HANDLE                    hEventWork;    // Work event

} XONLINECONTENT_CFFILECACHE, *PXONLINECONTENT_CFFILECACHE;

//
// Define the set of flags for bFlags
//
#define CFFILECACHE_FLAG_READ_PENDING        ((BYTE)0x01)
#define CFFILECACHE_FLAG_DISCARD_PREDATA    ((BYTE)0x02)

//
// Define an I/O pending success code for GetNextCffileEntry
//
#define S_CFFILECACHE_IO_PENDING        ERROR_IO_PENDING

//
// Define an EOF success code for GetNextCffileEntry
//
#define S_CFFILECACHE_NO_MORE_ITEMS        ERROR_NO_MORE_ITEMS

// DIROPS --------------------------------------------------------------------------------

//
// Directory operations
//

//
// Function to kick off an asynchronous recursive create directory. This
// function recursively creates all the directories up till the specified
// path.
//
// Arguments:
// szDirectoryPath - Path of the directory to create. This path may
//            or may not contain a file name, see dwFlags for details.
// dwFlags - Flags for specifying options. The following flag(s) are
//            defined:
//            XONLINETASK_CDFLAG_FILE_PATH - this flag indicates that
//                szDirectoryPath contains a file name. Only the directories
//                up to the file name will be created. If this flag is not
//                present, then entire path will be treated as a directory.
// hWorkEvent - Caller-supplied event to signal work. This event
//            will be signalled when the overlapped read completes
//            This can be NULL for a polling model.
// phTask - receives the task handle on success
//
// Return Values:
// S_OK - Task successfully started
// or any other appropriate error code.
//
// Remarks:
// This function will not validate directory paths
//

#define XONLINETASK_CDFLAG_FILE_PATH        (0x00000001)

#define XONLINETASK_EDFLAG_ENUMERATE_FILES    (0x00000001)
#define XONLINETASK_EDFLAG_ENUMERATE_DIRS    (0x00000002)
#define XONLINETASK_EDFLAG_ENUMERATE_ALL    \
    (XONLINETASK_EDFLAG_ENUMERATE_FILES | XONLINETASK_EDFLAG_ENUMERATE_DIRS)

//
// Define the directory ops
//
typedef enum
{
    diropCreateDirectory = 0,
    diropRemoveDirectory,
    diropEnumerateDirectory,
    diropMaxOp
    
} XONLINEDIROPS_OPS;

//
// Define a structure describing a 'skip' list
//
#define DIROPS_MAX_SKIP_ENTRIES        2

typedef struct
{
    DWORD    dwEntries;
    PSTR    rgszSkipPath[DIROPS_MAX_SKIP_ENTRIES];

} XONLINEDIROPS_SKIP_LIST, *PXONLINEDIROPS_SKIP_LIST;

//
// Define context structure shared by directory ops
//
typedef struct
{
    XONLINETASK_CONTEXT        xontask;    // Task context

    HRESULT                    hr;            // Status code
    XONLINEDIROPS_OPS        dirop;        // Operation to perform
    PSTR                    szPath;        // Path string
    DWORD                    cbPath;        // Path size (excluding NULL)
    DWORD                    cbMax;        // Max buffer size
    PSTR                    szDir;        // Next directory to process
    DWORD                    cbDir;        // Directory length
    DWORD                    cbCurrent;    // Current path length
    DWORD                    dwIndex;    // Index pointer
    DWORD                    dwFlags;    // Flags

    HANDLE                    hFind;        // Find handle
    
    XONLINETASK_HANDLE        hTaskOnData;// OnData subtask
    XONLINETASK_HANDLE        hTaskOnDone;// OnDone subtask

    XONLINEDIROPS_SKIP_LIST    DirsToSkip;    // Dirs to skip
    XONLINEDIROPS_SKIP_LIST    FilesToSkip;// Files to skip

} XONLINETASK_DIROPS, *PXONLINETASK_DIROPS;

//
// Special task context for directory crawling
//
#define MAX_CRAWL_DEPTH        128

typedef struct
{
    XONLINETASK_CONTEXT        xontask;    // Task context
    DWORD                    dwState;    // Current state
    HRESULT                    hr;            // Status code

    PSTR                    szPath;        // Path string
    DWORD                    cbPath;        // Path size (excluding NULL)
    PSTR                    szTarget;    // Next directory to process
    DWORD                    cbTarget;    // Directory length
    DWORD                    cbMax;        // Max length for both path buffers
    DWORD                    dwFlags;    // Flags

    WIN32_FIND_DATA            wfd;        // File/Dir information                                        
    DWORD                    dwDepth;    // Crawl depth
    HANDLE                    rghFind[MAX_CRAWL_DEPTH];        
                                        // Find file handle array
    XONLINETASK_HANDLE        hTaskOnDir;    // OnDir subtask
    XONLINETASK_HANDLE        hTaskOnFile;// OnFile subtask

} XONLINETASK_DIRCRAWL, *PXONLINETASK_DIRCRAWL;

//
// Task context for copying a directory hierarchy
//
typedef struct
{
    XONLINETASK_DIRCRAWL    dircrawl;
    XONLINETASK_CONTEXT        xontaskOnDir;
    XONLINETASK_CONTEXT        xontaskOnFile;

    DWORD                    dwFilesCopied;
    ULARGE_INTEGER            uliBytesCopied;

} XONLINETASK_DIRCOPY, *PXONLINETASK_DIRCOPY;

//
// Define some internal flags. Each flag has a specific meaning
// so do not try to reuse bits
//
#define XONLINETASK_CDFLAG_CREATE_MODE    (0x80000000)
#define XONLINETASK_RDFLAG_POPDIR        (0x40000000)
#define XONLINETASK_EDFLAG_SUBTASK        (0x20000000)

//
// Define the user flags
//
#define XONLINETASK_CDFLAGS_USER        (0x0000ffff)

//
// Define dircopy flags
//
#define XONLINETASK_DCFLAG_NOREPLACE    (0x00000001)

// CONTENT -------------------------------------------------------------------------------

//
// Define the current CATREF protocol version that is supported
//
#define XONLINECONTENT_PROTOCOL_VERSION        ((DWORD)0x11112222)

//
// Content delivery
//

//
// The CFFILE structure has a WORD field for the file time which 
// we will hijack for our special flags. Xbox content will use thie
// field to indicate special options instead of the file time.
// The following bits are defined for the time field, and honored
// by the content installation API.
//

// This flag indicates that the file should be copied to the
// user partition rather than the title area
#define XONLINECONTENT_A_USER_DATA                (0x0001)

// This flag indicates that this file is a continuation of
// a file in the previous folder. The data in this file should
// be appended to the previous file. This file must be the first
// file in the folder.
#define XONLINECONTENT_A_CONTINUED_FROM_PREV    (0x0002)

// This flag indicates that this file will be continued into the
// next folder. The data in the next folder should be appended to 
// this file. This file must be the last file in the folder.
#define XONLINECONTENT_A_CONTINUED_INTO_NEXT    (0x0004)

//
// Define the size of the download buffer. This cannot be 
// less than XONLINECONTENT_LZX_OUTPUT_SIZE
//
#define XONLINECONTENT_DL_BUFFER_SIZE    (1024 * 64)

//
// Define the LZX decoder history window size, this should be no 
// less than 32k (ROM decompressor uses 128k)
//
#define XONLINECONTENT_LZX_WINDOW_SIZE    CHUNK_SIZE

//
// Define the output buffer for decompress. This must not be
// less than 32k
#define XONLINECONTENT_LZX_OUTPUT_SIZE    \
            ((CHUNK_SIZE) + XBOX_HD_SECTOR_SIZE)

//
// Define the base path for all content
//
#define XONLINECONTENT_BASE_PATH            "%c:\\$c\\%08x.%08x\\"
#define XONLINECONTENT_DVD_PATH                "d:\\$c\\%08x\\%08x.%08x\\"
#define XONLINECONTENT_OPEN_TEMPLATE        "%c:\\$c\\%08x.*"
#define XONLINECONTENT_OPEN_PATH            "%c:\\$c\\%s\\"

//
// Define the search paths for local content enumeration
//
#define XONLINECONTENT_HD_SEARCH_PATH        "t:\\$c\\*.*"
#define XONLINECONTENT_DVD_SEARCH_PATH       "d:\\%08x\\$c\\*.*"
#define XONLINECONTENT_HD_ROOT_PATH          "t:\\$c\\"
#define XONLINECONTENT_TITLE_CONTENT_F       "f:\\$c\\%08x.%08x\\"

//
// Define the set of hard paths for autoupdate
//
#define AUTOUPD_DASH_TARGET_PATH            "y:\\$%c\\"
#define AUTOUPD_TITLE_READ_PATH            "t:\\$u\\"
#define AUTOUPD_TITLE_TARGET_PATH            "f:\\$u\\"
#define AUTOUPD_TEMP_DIFF                    "z:\\$t\\"
#define AUTOUPD_TEMP_UPDATE                    "z:\\$u\\"
#define AUTOUPD_DASH_ROOT                    "y:\\"
#define AUTOUPD_DVD_TITLE_ROOT                "d:\\"

//
// Define a macro to get the DWORD represenation of a version
// and its compliement
//
#define MAKE_DWORD_VERSION(major, minor) \
            ((((DWORD)(major)) << 16) | (minor))
#define MAKE_COMPOSITE_VERSION(version, pmajor, pminor) \
            {*(pmajor) = (WORD)((version) >> 16); \
             *(pminor) = (WORD)((version) & 0xffff);}

//
// Define the different types of directories that we know about
//
typedef enum
{
    dirtypeTitleContent = 0,
    dirtypeUserContent,
    dirtypeDVDContent,
    dirtypeTitleUpdate,
    dirtypeDashUpdate0,
    dirtypeDashUpdate1,
    dirtypeTempDiff,
    dirtypeTempUpdate,
    dirtypeDashRoot,
    dirtypeDVDTitleRoot,
    dirtypeTitleUpdateOnF,
    dirtypeTitleContentOnF,
    dirtypeNone,
    dirtypeMax

} XONLINEDIR_TYPE;

//
// Define the referral request XRL
//
#define XONLINECONTENT_REFERRAL_XRL        "/xbos/ContentReferral.ashx"

//
// Define the offering enumeration request XRL
//
#define XONLINEOFFERING_ENUMERATE_XRL    "/xbos/OfferingEnumerate.ashx"

//
// Define the autoupdate referral request XRL
//
#define XONLINEAUTOUPD_REFERRAL_XRL		"/autoupd/AutoupdReferral.ashx"


//
// Define the maximum path size
//
#define XONLINECONTENT_MAX_PATH            (512)

//
// Define the phases for content verification
//
typedef enum
{
    verphaseReadDrm = 0,    // Read the DRM file
    verphaseWriteDrm,        // Write new DRM
    verphaseDone            // Done

} XONLINECONTENT_VERPHASE;

//
// Define the context for content verification
//
typedef struct
{
    XONLINETASK_CONTEXT        xontask;        // Master task context

    XONLINECONTENT_VERPHASE    verphase;        // Processing phase
    XONLINEDIR_TYPE            dirtype;        // Type of directory
    DWORD                    dwFileSize;        // File size
    DWORD                    dwBytesRead;    // Bytes read
    PBYTE                    pbHeader;        // First sector of data
    PBYTE                    pbBuffer;        // Buffer
    DWORD                    cbBuffer;        // Size of buffer
    DWORD                    dwFlags;        // Flags
    DGSTFILE_IDENTIFIER        dfi;

    PBYTE                    pbKeyPackage;    // Per-package key
    PBYTE                    pbKeyPerBox;    // Per-box key

    BYTE                    shactxPackage[XC_SERVICE_SHA_CONTEXT_SIZE];
                                            // SHA context for package
    BYTE                    shactxPerBox[XC_SERVICE_SHA_CONTEXT_SIZE];    
                                            // SHA context for per-box
    
    XONLINETASK_FILEIO        fileio;            // File I/O context

} XONLINETASK_CONTENT_VERIFY, *PXONLINETASK_CONTENT_VERIFY;

//
// Define the internal flags for content verification
//
#define XONLINECONTENT_VERIFY_FLAG_SIGN                    ((DWORD)0x00000001)
#define XONLINECONTENT_VERIFY_FLAG_USER_KEY                ((DWORD)0x00000002)
#define XONLINECONTENT_VERIFY_FLAG_ALLOW_SAME_VERSION    ((DWORD)0x00000004)

//
// Define the different processing phases
//
typedef enum
{
    // Pre-Connect phases
    dlphaseCatref = 0,        // Contact catref for referral
    dlphaseCheckCache,        // Check if a valid cache file exists
    dlphaseReadCache,        // Read header from disk
    dlphaseWipeTarget,        // Wipe the target before fresh install
    dlphaseCreateCache,        // Create cache file
    dlphaseSetupCache,        // Setup CFFILE cache
    dlphaseVerifyInstall,    // Verify if content already installed
    dlphasePreCustom,        // Optional custom pre work

    // Process data phases
    dlphaseRecvHeader,        // Receive header from socket
    dlphaseInstallFiles,    // Install files (may kick off read CFFILE)
    dlphaseCreateTarget,    // Create new target file
    dlphaseWriteTarget,        // Write to target file
    dlphaseCheckpoint,        // Write checkpoint
    dlphaseCreateDirectory,    // Create directory
    dlphaseVerifyCffiles,    // Verify CFFILE entries

    // Post-Download phases
    dlphaseSignDrm,            // Sign the per-box DRM
    dlphaseDeleteCache,        // Delete the cached header
    dlphasePostCustom,        // Optional custom post work

    // Final state
    dlphaseDone,            // Done

} XONLINECONTENT_DLPHASE;

//
// Refactored global function to verify content digests. This is
// for code that exists outside of CXo
//
HRESULT DigestVerifySynchronous(
			PSTR					szManifestFilePath,
			XONLINEDIR_TYPE			dirtype,
			PDGSTFILE_IDENTIFIER	pdfi,
			DWORD					dwFlags,
			PBYTE					pbAlternateKey
			);

//
// Define the master subtask context for content download
//
typedef struct
{
    XONLINETASK_CONTEXT            xontask;    // Main task context

    // Title for which the content is designated
    DWORD                        dwTitleId;    // Title ID

    // Content ID to install
    XONLINEOFFERING_ID            OfferingId;    // Content ID

	// Specifies the install target directory type
	XONLINEDIR_TYPE					dirtypeTarget;

    // Resource path to content (obtained from CATREF)
    CHAR                        szResourcePath[XONLINECONTENT_MAX_PATH];

    // Cryptographic keys
    BYTE                        rgbSymmetricKey[XONLINE_KEY_LENGTH];
    BYTE                        rgbPublicKey[XONLINECONTENT_MAX_PUBLIC_KEY_SIZE];

    // Status info
    XONLINECONTENT_DLPHASE        dlphase;    // Processing phase
    DWORD                        dwExpected;    // Expected bytes for this phase
    DWORD                        dwWritten;    // Bytes written so far
    DWORD                        dwTemp;        // Temp scratchpad
    DWORD                        dwFlags;    // phase-specific flags
    DWORD                       dwBitFlags; // Package bit flags
    DWORD                        dwPackageSize; // Wire package size
    DWORD                        dwInstallSize; // Install size
    DWORD                        dwLastXrl;    // Last XRL accessed

    // Download server info
    XONLINE_SERVICE_INFO        xsiDownload;// Download server info

    // CAB header cache
    XONLINECONTENT_HEADER        header;        // Header cache

    // CAB processing context information
    DWORD                        dwCurrentFolder;// Index of current CFFOLDER
    DWORD                        dwFolderOffset;    // Uncompressed offset in fldr
    CFFILE                        *pcffile;    // Current CFFILE 
    PSTR                        szFileName;    // Current file name
    DWORD                        cbFileName;    // Length of file name
    DWORD                        dwFileSize;    // Size of file
    CFDATA                        cfdata;        // Current CFDATA
    DWORD                        cbUncompData;// Uncompressed data available

    // RC4-SHA1 decryption context
    XONLINESYMMDEC_CONTEXT        symmdecData;// RC4 decryption context

    // Async File I/O context
    XONLINETASK_FILEIO            fileioData;    // Write target file

    // Async directory operations context
    XONLINETASK_DIROPS            dirops;        // Directory management

    // Async verification context
    XONLINETASK_CONTENT_VERIFY    verify;        // Verification of manifest

    // CFFILE cache
    XONLINECONTENT_CFFILECACHE    cffilecache;// CFFILE cache

    // CFFOLDER and HMAC
    CFFOLDER_HMAC                rgcffolderhmac[XONLINECONTENT_MAX_CFFOLDER];
                                            // CFFOLDER with HMAC
    // Receive buffer
    BYTE                        rgbBuffer[XONLINECONTENT_DL_BUFFER_SIZE];

    // Scratchpad for file names and directory names
    CHAR                        szPath[XONLINECONTENT_MAX_PATH];

    // LZX decompression context
    XONLINE_LZXDECODER_CONTEXT    lzx;        // Decompressor context

    // Decompression input and output buffers
    BYTE                        rgbDecompress[XONLINECONTENT_LZX_OUTPUT_SIZE];

    BYTE                        rgbLzxWindow[XONLINECONTENT_LZX_WINDOW_SIZE + (MAX_MATCH+4)];
                                            // Decompressor history window
                                            
    // Optional task handles for pre-install and post-install custom work
    XONLINETASK_HANDLE            hTaskPreCustom;
    XONLINETASK_HANDLE            hTaskPostCustom;
    XONLINECONTENT_DLPHASE        dlphaseNext;    // Next state after custom

#if DBG
    // Debug stuff
    XONLINECONTENT_DLPHASE        dlphaseError;// Last phase before error
#endif

} XONLINECONTENT_INSTALL, *PXONLINECONTENT_INSTALL;

//
// Define the flags
//
#define XONLINECONTENT_FLAG_READ_PENDING         ((DWORD)0x00000001)
#define XONLINECONTENT_FLAG_WRITE_PENDING        ((DWORD)0x00000002)
#define XONLINECONTENT_FLAG_OFFLINE_INSTALL      ((DWORD)0x08000000)
#define XONLINECONTENT_FLAG_NOT_INSTALLED        ((DWORD)0x10000000)
#define XONLINECONTENT_FLAG_CONTINUE_FILE        ((DWORD)0x20000000)
#define XONLINECONTENT_FLAG_RESUME               ((DWORD)0x40000000)
#define XONLINECONTENT_FLAG_DONE_FILES           ((DWORD)0x80000000)

//
// Define the full context for content download
//
typedef struct
{
    // Note: XRL_ASYNC must be the first member of this struct. The
    // reason is because if the title tries to cancel this operation,
    // we want the cancel call to go to the handler for XOnlineDownload
    // because it knows how to handle the cancel (and free the memory
    // at the right location).
    
    XRL_ASYNC                xrlasync;        // XRL download context

    XRL_ASYNC_EXTENDED        xrlasyncCatref; // Catref context

    XONLINECONTENT_INSTALL    install;        // Content install context

} XONLINETASK_CONTENT_DOWNLOAD, *PXONLINETASK_CONTENT_DOWNLOAD;

//
// Define the phases for content removal
//
typedef enum
{
    rmphaseRemoveDrm = 0,        // Remove the DRM file
    rmphaseRemoveTitleData,        // Remove all title content data
    rmphaseRemoveUserData,        // Remove all user content data
    rmphaseDone                    // Done

} XONLINECONTENT_RMPHASE;

//
// Define the context for content removal
//
typedef struct
{
    XONLINETASK_CONTEXT        xontask;    // Master task context
    XONLINECONTENT_RMPHASE    rmphase;    // Current phase
    DWORD                    dwFlags;    // Flags
    DWORD                    dwTitleId;    // Title ID
    XONLINEOFFERING_ID        OfferingId;    // Content ID

    XONLINETASK_DIROPS        dirops;        // Directory operations context

} XONLINETASK_CONTENT_REMOVE, *PXONLINETASK_CONTENT_REMOVE;

// =============================================================
// Autoupdate structures
//

//
// Structure representing an interleave entry
//
typedef struct
{
    DWORD    dwOldOffset;    // Starting offset in old file
    DWORD    dwOldLength;    // Old Length
    DWORD    dwNewLength;    // New Length

} AUTOUPD_INTERLEAVE_ENTRY, *PAUTOUPD_INTERLEAVE_ENTRY;

//
// Structure representing a patch file header
//
typedef struct
{
    DWORD    dwNewFileSize;            // Size of the new file
    DWORD    dwOldFileSize;            // Size of the old (source) file
    DWORD    dwWindowSizeBits;        // Dictionary window size (2^bits)
    DWORD    dwPatchDataSize;        // Remaining size of patch data    
    DWORD    dwDataSize;                // Size of patch data read
    DWORD    dwCurrent;                // Current offset into patch data window
    DWORD    dwTargetWritten;        // Bytes written for current interleave
    DWORD    dwInterleaveEntries;    // Interleave map entries
    DWORD    dwInterleaveCurrent;    // Current interleave map entry

    // There are actually dwInterleaveEntries of them
    AUTOUPD_INTERLEAVE_ENTRY    rgInterleaveInfo[1];

} AUTOUPD_PATCH_FILE_INFO, *PAUTOUPD_PATCH_FILE_INFO;

//
// Progress implementation structure
//
typedef struct
{
    DWORD            dwFilesDone;
    DWORD            dwFilesTotal;
    ULARGE_INTEGER    uliBytesProcessed;
    ULARGE_INTEGER    uliBytesTotal;
    ULARGE_INTEGER    uliActualBytesRequired;
    
} AUTOUPD_PROGRESS_CONTEXT, *PAUTOUPD_PROGRESS_CONTEXT;

#define AUTOUPD_BUMP_PROGRESS(pprogress, bytes)    \
    ((pprogress)->uliBytesProcessed.QuadPart += (bytes))

//
// Define the dash title ID
//
#define AUTOUPD_DASH_TITLE_ID            ((DWORD)0xfffe0000)

//
// Define a structure that holds the dash configuration information
//
typedef struct
{
    DWORD    dwActiveDirectory;        // Active boot directory

} AUTOUPD_DASH_CONFIG, *PAUTOUPD_DASH_CONFIG;

//
// Define the task context for removing old dash files
//
typedef struct
{
	XONLINETASK_CONTEXT	xontask;
	DWORD				iCurrent;

} XONLINETASK_DEL_OLD_DASH, *PXONLINETASK_DEL_OLD_DASH;

//
// Function to read the current dash boot directory 
// from the config file
//
HRESULT XoUpdateReadDashConfig(
            PAUTOUPD_DASH_CONFIG    pConfig,
            BOOL					fFailIfNotFound
            );

//
// Function to write the current dash boot directory 
// to the config file
//
HRESULT XoUpdateWriteDashConfig(
			PAUTOUPD_DASH_CONFIG	pConfig
			);

//
// Function to replace the original dash XBE with the switcher
//
HRESULT XoUpdateInstallSwitcher(
			XONLINEDIR_TYPE	dirtype
			);
			
//
// This function mounts the dash partition as Y: drive. 
//
HRESULT XoUpdateMountDashPartition();

#ifdef XONLINE_FEATURE_XBOX

//
// Function to mount a title's TDATA to the specified drive
//
HRESULT XoUpdateMountTitleTDataToPrivateDrive(
			DWORD	dwTitleId
			);

//
// Function to unmount a drive previously mounted with
// XoMountTitleTdataToDriveLetter. Since this does not do any
// routine cleanup, don't call this function to unmpa any drives
// that is not mapped with XoMountTitleTdataToDriveLetter.
// 
HRESULT XoUpdateUnmountPrivateDrive();

DWORD RebootToUpdaterWhilePreservingDDrive(
		    LPCSTR lpTitlePath,
		    PLAUNCH_UPDATE_INFO plui
		    );

#else

#define XoUpdateMountTitleTDataToPrivateDrive(dwTitleId)	(S_OK)
#define XoUpdateUnmountPrivateDrive()						(S_OK)

#endif

//
// Helper function to get the unmunged title key for the given title ID
//
HRESULT XoUpdateGetRawTitleKey(
            DWORD    dwTitleId,
            PBYTE    pbTitleKey,
            DWORD    *pcbTitleKey
            );

// =============================================================
// Autoupdate and offering wire structures 
//

//
// Common header for wire structures
//
typedef struct
{
    DWORD       cbSize;
    DWORD       dwReserved;

} XONLINE_WIRE_HEADER, *PXONLINE_WIRE_HEADER;

//
// Wire structure for content referral
//
typedef struct
{
    XONLINE_WIRE_HEADER         Header;         // Wire struct header
    ULONGLONG                    MachineId;        // Machine ID
    ULONGLONG                   Puid[XONLINE_MAX_LOGON_USERS];  
                                                // PUIDs of logged on users
    DWORD                       dwTitleId;      // Title ID
    XONLINEOFFERING_ID          OfferingId;     // Offering ID
    DWORD                       dwRating;       // Rating on Xbox
} XONLINECONTENT_REFERRAL_REQ, *PXONLINECONTENT_REFERRAL_REQ;    

//
// Wire structure for autoupdate referral
//
typedef struct
{
    XONLINE_WIRE_HEADER         Header;         // Wire struct header
    ULONGLONG					qwMachineId;	// Machine account
    DWORD                       dwTitleId;      // Title ID 
    DWORD						dwDashVersion;	// Old version of title (dash only)
} XONLINEUPDATE_REFERRAL_REQ, *PXONLINEUPDATE_REFERRAL_REQ;	

//
// Wire structure for offering enumeration
//
typedef struct
{
    XONLINE_WIRE_HEADER         Header;         // Wire struct header
    ULONGLONG                   Puid;           // PUID of user in context
    DWORD                       dwTitleId;      // Title ID
    DWORD                       cbClientBuffer; // Size of client buffer for responses
    DWORD                       dwRating;       // Rating on Xbox
    XONLINEOFFERING_ID          ResumePosition; // Last offering ID received
    XONLINEOFFERING_ENUM_PARAMS Params;         // Caller enum params
} XONLINEOFFERING_ENUM_REQ, *PXONLINEOFFERING_ENUM_REQ;

//
// Wire structure for offering enumeration header
//
typedef struct
{
    XONLINE_WIRE_HEADER         Header;     // Wire struct header
    WORD                        cRecords;   // Number of records returned
    WORD                        fFlags;     // Enumeration response flags
} XONLINEOFFERING_ENUM_RESP_HEADER, *PXONLINEOFFERING_ENUM_RESP_HEADER;

//
// Supported enumeration response flags
//
#define XONLINEOFFERING_RESP_FLAG_MORE_RESULTS      ((WORD)0x0001)
#define XONLINEOFFERING_RESP_FLAG_DONT_CACHE        ((WORD)0x0002)

//
// Wire structure for offering enumeration info records
//
typedef struct
{
    DWORD                   cbRecordSize;   // Size of THIS record (including blob)
    WORD                    iRecordIndex;   // Index of this record (wrt. entire set)
    WORD                    wPadding;       // Alignment padding
    XONLINEOFFERING_INFO    OfferingInfo;   // Offering information
} XONLINEOFFERING_ENUM_RESP_RECORD, *PXONLINEOFFERING_ENUM_RESP_RECORD;

#define    XONLINEREFERRAL_SYM_KEY_SIZE             XONLINE_KEY_LENGTH
#define    XONLINEREFERRAL_PUB_KEY_SIZE             284 
#define    XONLINEREFERRAL_MAX_XRL_SIZE             MAX_PATH

//
// Structure representing a referral location
//
typedef struct
{
    WORD    Rank;                                   // Rank of location
    WORD    cbXrl;                                  // Length of XRL
    CHAR    szXrl[XONLINEREFERRAL_MAX_XRL_SIZE];    // Unterminated XRL
} XONLINEREFERRAL_LOCATION, *PXONLINEREFERRAL_LOCATION;

//
// Wire structure for a referral response
//
typedef struct
{
    XONLINE_WIRE_HEADER         Header;     // Wire struct header
    WORD                        fFlags;     // Referral flags
    WORD                        cLocations; // Number of locations returned
    DWORD                       dwPackageSize; // Wire size of package
    DWORD                       dwInstallSize; // Accurate install size
    DWORD                       dwBitFlags; // Package flags
    BYTE                        rgbSymKey[XONLINEREFERRAL_SYM_KEY_SIZE];
    BYTE                        rgbPubKey[XONLINEREFERRAL_PUB_KEY_SIZE];

    // This is followed by cLocations XONLINEREFERRAL_LOCATION records. The
    // strings will be marshalled using variable lenghts to conserve bandwidth,
    // and each record will be packed and thus may not be on a DWORD-aligned 
    // boundary.
    
} XONLINEREFERRAL_RESP, *PXONLINEREFERRAL_RESP;

//
// Expose a constant string for Content-type: xon/xxxxxxxx
//
#define XONLINE_MAX_CONTENT_TYPE_SIZE	32

//
// Define the states for online offering enumeration
//
typedef enum
{
    enumsrvUpload = 0,    // XRL upload phase
    enumsrvResults,        // Results available
    enumsrvContinue,    // Get more results
    enumsrvDone            // Final phase, done

} XONLINEOFFERING_ENUM_SERVER_STATE;

//
// Define a context for online offering enumeration
//
typedef struct
{
    XONLINETASK_CONTEXT            xontaskEnum;    // Enum task context
    XRL_ASYNC_EXTENDED            xrlasyncext;    // Extended Upload context
    XONLINETASK_CONTEXT            xontaskPostUpload;// Post-upload subtask

    XONLINEOFFERING_ENUM_REQ    enumreq;        // Enumerate request context

    XONLINEOFFERING_ID            OfferingIdLast;    // Last offering ID received
    XONLINEOFFERING_ENUM_SERVER_STATE    State;    // Current state

} XONLINEOFFERING_ENUM_SERVER, *PXONLINEOFFERING_ENUM_SERVER;

//
// Define a context for Local enumeration
//
typedef struct
{
    XONLINETASK_DIROPS    dirops;            // dirops context
    XONLINETASK_CONTEXT    xontask;        // hTaskOnData context
    XONLINETASK_CONTEXT    xontaskDone;    // hTaskOnDone context

    PBYTE                pbBuffer;        // Supplied buffer
    DWORD                cbBuffer;        // Buffer size
    DWORD                dwBitFilter;    // Filtering flags

    CHAR                szPath[XONLINECONTENT_MAX_PATH];
    
} XONLINEOFFERING_ENUM_LOCAL, *PXONLINEOFFERING_ENUM_LOCAL;

//
// Define a generic content enumeration task context
//
typedef struct
{
    union 
    {
        XONLINEOFFERING_ENUM_SERVER    server;        // Server enum context
        XONLINEOFFERING_ENUM_LOCAL     local;         // Local enum context
        
    }                            enumdata;    // enum context
    XONLINEOFFERING_ENUM_DEVICE    edDevice;    // Device

    BOOL                        fPartial;    // Indicates partial results
    DWORD                        dwResults;    // Results received & parsed
    DWORD                        dwMaxResults;// Max results desired

} XONLINETASK_OFFERING_ENUM, *PXONLINETASK_OFFERING_ENUM;

//
// A particular characteristic of Xbox asynchronous WriteFile calls is
// that the call is asynchronous so long as the file size is not exceeded.
// To keep the writes asynchronous, SetEndOfFile is used to grow the file
// in chunks. The following is the increment size used to grow files.
//
// BUGBUG: Need to find an optimal value
//
#define XONLINECONTENT_INCREMENTAL_GROW_FILE_SIZE    ((XBOX_HD_SECTOR_SIZE) << 12)

//
// Define the internal timeout values that we will use for different
// operations
//
#define XONLINECONTENT_REFERRAL_TIMEOUT				((DWORD)(10000))
#define XONLINECONTENT_INSTALL_TIMEOUT				((DWORD)(30000))
#define XONLINEOFFERING_ENUMERATE_TIMEOUT			((DWORD)(10000))
#define XONLINEAUTOUPD_REFERRAL_TIMEOUT				((DWORD)(10000))

//
// Define a context structure for title enumeration
//
typedef struct
{
    XONLINETASK_DIROPS    dirops;            // dirops context
    
    XONLINETASK_CONTEXT    xontask;        // hTaskOnData context

    PDWORD                rgdwTitleId;    // Title ID array
    DWORD                dwTitleId;        // Count of title IDs
    DWORD                dwMaxResults;    // Max results desired

} XONLINECONTENT_ENUM_TITLES, *PXONLINECONTENT_ENUM_TITLES;

//
// Trace tags used by Online Auth APIs
//
ExternTag(AuthVerbose);
ExternTag(AuthWarn);

#ifdef XONLINE_FEATURE_INSECURE

//
// InitOverrideInfo allows you to load overriding IP/Port and KDC info
// from an ini file so that we can more easily test the online services.
// This function can only be used in the insecure build.
//
// This function reads the file E:\xonline.ini on devkits and processes
// each line one by one and stores retults in global variables
// g_pszMACSKDC, g_pszPassportKDC, g_pszXboxKDC, and g_rgServiceInfoOverride.
// The format for each line in the configuration file is one of following:
//
// MKDC:a.b.c.d<CR><LF>
// PKDC:a.b.c.d<CR><LF>
// XKDC:a.b.c.d<CR><LF>
// MKDC<CR><LF>
// PKDC<CR><LF>
// XKDC<CR><LF>
// SX:a.b.c.d:Port<CR><LF>
//
// The ?KDC lines specify the location of the MACS/Passport/Xbox KDCs,
// while SX line specify the IP/Port of service X.
//
// The empty ?KDC lines instruct the client code to
// not perform any operations related to that KDC.
//
// No whitespaces or blank lines are allowed in the file. The
// size of the file must remain less than or equal to 4096 bytes.
//

#endif

#define PASSPORT_AS_DNS     "passport.as.xbox.com"
#define PASSPORT_WEB_DNS    "passport.web.xbox.com"
#define PASSPORT_TGS_DNS    "passport.tgs.xbox.com"
#define XBOX_KDC_DNS        "kdc.xbox.com"
#define MACS_KDC_DNS        "macs.xbox.com"

struct XONLINE_DNS_CACHE_ENTRY
{
    char* name;

    #define DNS_LOOKUP_IN_PROGRESS  1
    #define DNS_LOOKUP_COMPLETE     2
    #define DNS_LOOKUP_ERROR        3
    DWORD dwState;

    XNDNS address;
};

#define XONLINE_NUM_DNS_CACHE_ENTRYS 5

//----------------------------------------------------------------------------
//
// Local cache
//
//----------------------------------------------------------------------------

typedef enum
{
    XONLC_TYPE_RECENT_LOGON_TIME = 0,
    XONLC_TYPE_COMBINED_USER_TICKETS = 1,
    XONLC_TYPE_SERVICE_TICKETS = 2,
    XONLC_TYPE_FRIEND_LIST = 3,
    XONLC_TYPE_LOCKOUT_LIST = 4,
    XONLC_TYPE_LICENSING = 5,
    XONLC_TYPE_BILLING = 6,
    XONLC_TYPE_TITLEDATA = 7,
    XONLC_TYPE_GAMEINVITE = 8,
    XONLC_TYPE_TEST1 = 9,
    XONLC_TYPE_TEST2 = 10,
    XONLC_MAX_CACHE_TYPES = 11
} XONLC_TYPE;

typedef struct
{
    BOOL                                 bValidDataInCache;
    BOOL                                 bCallRetrieveNext;
    LPBYTE                               pbIndexData;   
    DWORD                                dwRecordBufferSize;
    LPBYTE                               pbRecordBuffer;    
} XONLC_CONTEXT, *PXONLC_CONTEXT;

typedef BOOL (CXo::*PFNXONLC_COMPARE_HANDLER)(
    IN const BYTE                        *pbTarget,
    IN const BYTE                        *pbCandidate
    );

/////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
//
// Message Client
//
//----------------------------------------------------------------------------
typedef enum
{

    XONMSGCLIENT_TYPE_TITLEID = 0,
    XONMSGCLIENT_TYPE_ERRMSG = 1,
    XONMSGCLIENT_TYPE_MAXTYPES

}XONMSGCLIENT_TYPES;

#pragma pack(push, 1)
typedef struct
{
    WORD       wRecordsCount;
}XONMSGCLIENT_HEADER, *PXONMSGCLIENT_HEADER;

typedef struct 
{
    WORD        wType;
    DWORD       dwID;
    WORD        wLocale;

}XONMSGCLIENT_REQUEST_RECORD, *PXONMSGCLIENT_REQUEST_RECORD;

#pragma pack(pop)

typedef struct 
{
    DWORD       cbBuffer;
    PBYTE       pbBuffer;

}XONMSGCLIENT_RESPONSE_RECORD, *PXONMSGCLIENT_RESPONSE_RECORD;


typedef struct
{
    XONMSGCLIENT_HEADER              Header;
    PXONMSGCLIENT_REQUEST_RECORD     pReqRecords;
}XONMSGCLIENT_REQUEST, *PXONMSGCLIENT_REQUEST;

typedef struct 
{
    DWORD                            cbBuffer;
    PBYTE                            pbBuffer;
}XONMSGCLIENT_RESPONSE, *PXONMSGCLIENT_RESPONSE;

typedef struct 
{
    XONLINETASK_CONTEXT     xonTask;            // Standard Task Pump context info
    XRL_ASYNC_EXTENDED      xonTaskUpload;      // Subsumed context for upload

    HRESULT                 hrFinal;            // Final HRESULT

}XONLINECTXT_MSGCLIENT, *PXONLINECTXT_MSGCLIENT;

typedef PXONLINECTXT_MSGCLIENT XONLINETASK_MSGCLIENT;
typedef PXONLINECTXT_MSGCLIENT *PXONLINETASK_MSGCLIENT;

// Title data API

#define     XONLINE_TITLEDATA_SIZE         32

typedef struct 
{
    char        name[XONLINE_TITLEDATA_SIZE];
}XONLINE_TITLEDATA, *PXONLINE_TITLEDATA;

enum XONTITLECACHE_STATE;

typedef struct
{
    XONLINETASK_CONTEXT     xonTask;            // Standard Task Pump context info
    XONLINECTXT_MSGCLIENT   xonTaskMsgClient;   // Subsumed context for msg client

    XONLINETASK_HANDLE      hTaskLocalCache;    // Task handle for local caching
    HANDLE                  hCache;             // Handle to the local cache

    HRESULT                 hrFinal;            // Final HRESULT

    DWORD                   dwTitleID;          // Info to be passed between pump sessions
    PXONLINE_TITLEDATA      pTitleData;         // Info to be passed between pump sessions

    XONLC_CONTEXT           context;

    PXONMSGCLIENT_REQUEST   pMsgServerReq;      // The request structure
    PBYTE                   pbMsgServerResp;    // Buffer for the response
    DWORD                   cbMsgServerResp;    // Size of the buffer for the response

    XONLINETASK_HANDLE      hSubtask;           // Unified way to pump subtasks
    XONTITLECACHE_STATE     state;              // State of the task


}XONLINECTXT_TITLECACHE, *PXONLINECTXT_TITLECACHE; 

typedef PXONLINECTXT_TITLECACHE XONLINETASK_TITLECACHE;
typedef PXONLINECTXT_TITLECACHE *PXONLINETASK_TITLECACHE;

//////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------------------
// CXo
// ---------------------------------------------------------------------------------------

struct  XONLINETASK_AUTOUPD_DVD;
typedef XONLINETASK_AUTOUPD_DVD * PXONLINETASK_AUTOUPD_DVD;
struct  XONLINETASK_AUTOUPD_ONLINE;
typedef XONLINETASK_AUTOUPD_ONLINE * PXONLINETASK_AUTOUPD_ONLINE;
struct  XONLINEAUTOUPD_ONLINE;
typedef XONLINEAUTOUPD_ONLINE * PXONLINEAUTOUPD_ONLINE;
struct  XONLINETASK_DNS_LOOKUP;
typedef XONLINETASK_DNS_LOOKUP * PXONLINETASK_DNS_LOOKUP;
struct  XONLINETASK_GET_SINGLE_TICKET;
typedef XONLINETASK_GET_SINGLE_TICKET * PXONLINETASK_GET_SINGLE_TICKET;
struct  XONLINETASK_GET_MACHINE_ACCOUNT;
typedef XONLINETASK_GET_MACHINE_ACCOUNT * PXONLINETASK_GET_MACHINE_ACCOUNT;
struct  XONLINETASK_GET_COMBINED_TICKET;
typedef XONLINETASK_GET_COMBINED_TICKET  * PXONLINETASK_GET_COMBINED_TICKET;
struct  XONLINETASK_GET_SERVICE_TICKET;
typedef XONLINETASK_GET_SERVICE_TICKET  * PXONLINETASK_GET_SERVICE_TICKET;
struct  XONLINETASK_LOGON;
typedef XONLINETASK_LOGON * PXONLINETASK_LOGON;
struct  XMATCH_EXTENDED_HANDLE;
typedef XMATCH_EXTENDED_HANDLE * PXMATCH_EXTENDED_HANDLE;
enum    XMATCH_TYPE;
enum    SERVER_TYPE;
enum    XPRESENCE_REQUESTTYPE;
struct  XPRESENCE_EXTENDED_HANDLE;
typedef XPRESENCE_EXTENDED_HANDLE * PXPRESENCE_EXTENDED_HANDLE;
struct  XPRESENCE_CACHEEXTENDED_HANDLE;
typedef XPRESENCE_CACHEEXTENDED_HANDLE * PXPRESENCE_CACHEEXTENDED_HANDLE;
enum    XPRESENCE_CACHESTATE;
struct  XPRESENCE_USER;
typedef XPRESENCE_USER * PXPRESENCE_USER;
struct  XO_OFFERING_VERIFY_LICENSE_TASK;
struct  XO_OFFERING_LICENSE;
class   CLocalCache;
typedef CLocalCache *XONLC_CACHE_HANDLE;
typedef CLocalCache **PXONLC_CACHE_HANDLE;
typedef HRESULT (CXo::*PFNDIRCRAWL_HANDLER)(PXONLINETASK_DIRCRAWL pdircrawl);
typedef HRESULT (CXo::*PFNAUTOUPD_DVD_HANDLER)(PXONLINETASK_AUTOUPD_DVD    pautoupd);
typedef HRESULT (CXo::*PFNCONTDL_HANDLER)(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
typedef HRESULT (CXo::*PFNAUTOUPD_ONLINE_HANDLER)(PXONLINEAUTOUPD_ONLINE pautoupd);
struct  XPRESENCE_GAMEINVITE_RECORD;
typedef XPRESENCE_GAMEINVITE_RECORD * PXPRESENCE_GAMEINVITE_RECORD;

HRESULT BuildTargetDirectory(DWORD dwTitleId, XONLINEOFFERING_ID OfferingId, DWORD dwBitFlags, XONLINEDIR_TYPE dirtype, DWORD * pcbTargetDirectory, CHAR * szTargetDirectory);
HRESULT BuildTargetPath(DWORD dwTitleId, XONLINEOFFERING_ID OfferingId, DWORD dwBitFlags, CHAR * szPostfix, XONLINEDIR_TYPE dirtype, DWORD * pcbTargetPath, CHAR * szTargetPath);
HRESULT BuildExistingContentPath(DWORD dwTitleId, XONLINEOFFERING_ID OfferingId, CHAR *szPostfix, XONLINEDIR_TYPE dirtype, DWORD *pcbTargetPath, CHAR *szTargetPath);
HRESULT ParsePatchHeader(PBYTE pbData, DWORD *pcbData, PBYTE pbParsedData, DWORD *pcbParsedData);


//
// Structure to store extra context about each logged in user, as it pertains to friends and enumeration.
//
struct XPRESENCE_USER
{
    ULONGLONG qwUserID;                                 // UserID
    CHAR szUserName[XONLINE_USERNAME_SIZE];             // User name
    DWORD dwSeqNum;                                     // Sequence number of last sent friend SYNC request
    BOOL fFriendListSynced;                             // Flag indicating whether the friend list for this user is in sync or not
    WORD cNumFriends;                                   // Number of friends in this user's friend list
    DWORD dwFriendListVersion;                          // Version of current friend list
    XONLINE_FRIEND FriendList[MAX_FRIENDS];             // Friend list
    WORD cNumLockoutUsers;                              // Number of users in this lockout list
    DWORD dwLockoutListVersion;                         // Version of current lockout list
    XONLINE_LOCKOUTUSER LockoutList[MAX_LOCKOUTUSERS];  // Lockout list
    WORD cbUserData;                                    // Size of userdata
    PBYTE pbUserData;                                   // Userdata
};

class CXo : public CXoBase
{
    friend class CLocalCache;
    friend class CCacheTask;
    friend class CXoBase;
    friend class CXoTest;

public:

    // XOnline API -----------------------------------------------------------------------

    #undef  XONAPI
    #define XONAPI(ret, fname, arglist, paramlist) ret fname arglist;
    #undef  XONAPI_
    #define XONAPI_(ret, fname, arglist, paramlist)

    XONLINEAPILIST()

    // Public accessors ------------------------------------------------------------------

    INLINE DWORD        GetTitleId()            { return(m_dwTitleId); }
    INLINE DWORD        GetTitleVersion()       { return(m_dwTitleVersion); }
    INLINE DWORD        GetTitleGameRegion()    { return(m_dwTitleGameRegion); }
    INLINE ASN1module_t GetASN1Module()         { return(&m_KRB5_Module); }

    INLINE DWORD        AddRef()                { return(++m_cRefs); }
    INLINE DWORD        Release()               { return(--m_cRefs); }

    HRESULT             Init(PXONLINE_STARTUP_PARAMS pxosp);
    void                Term();

    BOOL                SetMachineAccount(PXONLINEP_USER pUser);

protected:

    // Memory Management -----------------------------------------------------------------

    INLINE void *   SysAlloc(size_t cb, ULONG tag);
    INLINE void *   SysAllocZ(size_t cb, ULONG tag);
    INLINE void     SysFree(void * pv);

    void *          PoolAlloc(size_t cb, ULONG tag);
    void *          PoolAllocZ(size_t cb, ULONG tag);
    void            PoolFree(void * pv);

    // autoupd.cpp -----------------------------------------------------------------------

#ifdef XONLINE_FEATURE_XBOX

    HRESULT         UpdateDVDCountFilesContinue(XONLINETASK_HANDLE hTask);
    HRESULT         dvdupdCountFilesHandler(PXONLINETASK_AUTOUPD_DVD pautoupd);
    HRESULT         dvdupdWipeDestinationHandler(PXONLINETASK_AUTOUPD_DVD pautoupd);
    HRESULT         dvdupdCreateTargetHandler(PXONLINETASK_AUTOUPD_DVD pautoupd);
    HRESULT         dvdupdCopyFilesHandler(PXONLINETASK_AUTOUPD_DVD pautoupd);
    HRESULT         dvdupdWriteConfigHandler(PXONLINETASK_AUTOUPD_DVD pautoupd);
    HRESULT         dvdupdCleanupHandler(PXONLINETASK_AUTOUPD_DVD pautoupd);
    HRESULT         UpdateFromDVDContinue(XONLINETASK_HANDLE hTask);
    VOID            UpdateFromDVDClose(XONLINETASK_HANDLE hTask);
    HRESULT         UpdateFromDVD(DWORD dwTitleID, WORD wVersionHi, WORD wVersionLo, DWORD dwFlags, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask);
    HRESULT         UpdateDashFromDVD(WORD wVersionHi, WORD wVersionLo, DWORD dwDashId, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask);
    HRESULT         UpdateTitleFromDVD(DWORD dwTitleID, WORD wVersionHi, WORD wVersionLo, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask);
    HRESULT         UpdateFromDVDGetProgress(XONLINETASK_HANDLE hTask, DWORD * pdwPercentDone, ULARGE_INTEGER * puliNumerator, ULARGE_INTEGER * puliDenominator);
    HRESULT         UpdateCheckAndBumpRetryCount(DWORD dwUpdateVersion);
    VOID            UpdateResetRetryCount();
    VOID            UpdateDashIfNecessary();

#endif

    // diffpatch.cpp -----------------------------------------------------------------------

#ifdef XONLINE_FEATURE_XBOX

    HRESULT         UpdateOnlineCountFilesContinue(XONLINETASK_HANDLE hTask);
    HRESULT         xoupdPreInitializeHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdCountFilesHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdFormatTempHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdDoneDVDPreInitHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdPostInitializeHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdVerifyPatchDigestHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdVerifyTargetDigestHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdWipeTargetHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdOpenPatchHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdOpenSourceHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdCreateTargetHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdCreateDirectoryHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdReadSourceHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdReadPatchHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdDecodeDataHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdWriteTargetHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdCloseFilesHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdCopyDigestHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdCopyOtherFilesHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdSignDigestHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdWriteConfigHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
	HRESULT         xoupdCleanupTempHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdCleanupOfflineDashFilesHandler(PXONLINEAUTOUPD_ONLINE pautoupd);
    HRESULT         xoupdContinue(XONLINETASK_HANDLE hTask);
    VOID            AutoupdateCleanup(PVOID pvxrlasync);
    HRESULT         AutoupdateInitializeContext(PXONLINETASK_AUTOUPD_ONLINE pxoupd, DWORD dwTitleId, DWORD dwTitleOldVersion, BOOL fOnline, PBYTE pbTitleKey, HANDLE hWorkEvent);
    HRESULT         AutoupdateOnline(DWORD dwTitleId, DWORD dwTitleOldVersion, PBYTE pbTitleKey, HANDLE hWorkEvent, PXONLINETASK_HANDLE pHandle);
    HRESULT         AutoupdateFromDVD(PSTR szUpdatePath, DWORD dwTitleId, DWORD dwTitleOldVersion, PBYTE pbTitleKey, PBYTE pbSymmetricKey, DWORD cbSymmetricKey, PBYTE pbPublicKey, DWORD cbPublicKey, HANDLE hWorkEvent, PXONLINETASK_HANDLE phTask);
    HRESULT         AutoupdateGetProgress(XONLINETASK_HANDLE hTask, DWORD *pdwPercentDone, ULONGLONG *pqwNumerator, ULONGLONG *pqwDenominator);

#endif

    // kerberos.cpp ----------------------------------------------------------------------

    NTSTATUS        XkerbBuildAsRequest(BOOL fMachineAccount, PXONLINEP_USER User, PUCHAR pbRequestMessage, PULONG pdwRequestMessageLength, PXKERB_TGT_CONTEXT pTGTContext);
    NTSTATUS        XkerbVerifyAsReply(PUCHAR pbReplyMessage, ULONG dwReplyMessageLength, PXKERB_TGT_CONTEXT pTGTContext);
    NTSTATUS        XkerbBuildTgsRequest(PXKERB_TGT_CONTEXT pCombinedTGTContext, PUCHAR pbRequestMessage, PULONG pdwRequestMessageLength, PXKERB_SERVICE_CONTEXT pXKerbServiceContext);
    NTSTATUS        XkerbVerifyTgsReply(PXONLINEP_USER pUsers, PXKERB_TGT_CONTEXT pCombinedTGTContext, PUCHAR pbReplyMessage, ULONG dwReplyMessageLength, PXKERB_SERVICE_CONTEXT pXKerbServiceContext);
    NTSTATUS        XkerbBuildApRequest(PXKERB_SERVICE_CONTEXT pServiceContext, LARGE_INTEGER * pliTime, BYTE * pbSha, UINT cbSha, PUCHAR pbRequestMessage, UINT * pdwRequestMessageLength);
    NTSTATUS        XkerbVerifyApReply(PXKERB_SERVICE_CONTEXT pServiceContext, LARGE_INTEGER * pliTime, BYTE * pbSha, UINT cbSha, PUCHAR pbReplyMessage, UINT dwReplyMessageLength);
    XOKERBINFO *    XkerbGetInfo(DWORD dwServiceId);
    NTSTATUS        XkerbBuildApReq(DWORD dwServiceId, LARGE_INTEGER * pliTime, BYTE * pbSha, UINT cbSha, BYTE * pbApReq, UINT * pcbApReq);
    NTSTATUS        XkerbCrackApRep(DWORD dwServiceId, LARGE_INTEGER * pliTime, BYTE * pbSha, UINT cbSha, BYTE * pbApRep, UINT cbApRep);
    void            XkerbGetAuthData(XKERB_AD_XBOX * pAuthData);

    // baseio.cpp ------------------------------------------------------------------------

    HRESULT         ReadWriteFileContinue(XONLINETASK_HANDLE hTask);
    VOID            ReadWriteFileClose(XONLINETASK_HANDLE hTask);
    HRESULT         ReadWriteFileBegin(PXONLINETASK_FILEIO pfileio, BOOL fRead);
    INLINE HRESULT  ReadFileInternal(PXONLINETASK_FILEIO pfileio) { return(ReadWriteFileBegin(pfileio, TRUE)); }
    INLINE HRESULT  WriteFileInternal(PXONLINETASK_FILEIO pfileio) { return(ReadWriteFileBegin(pfileio, FALSE)); }
    VOID            ReadWriteFileInitializeContext(HANDLE hFile, LPBYTE pbBuffer, DWORD cbBytes, ULARGE_INTEGER uliStartingOffset, HANDLE hWorkEvent, PXONLINETASK_FILEIO pfileio);
    HRESULT         ReadWriteFile(HANDLE hFile, LPBYTE pbBuffer, DWORD cbBytes, ULARGE_INTEGER uliStartingOffset, HANDLE hWorkEvent, XONLINETASK_HANDLE* phTask, BOOL fRead);
    HRESULT         GetReadWriteFileResults(XONLINETASK_HANDLE hTask, DWORD * pdwBytesTransferred, LPBYTE * ppbBuffer);
    HRESULT         StreamConnectContinue(XONLINETASK_HANDLE hTask);
    VOID            StreamConnectClose(XONLINETASK_HANDLE hTask);
    HRESULT         StreamConnectBegin(SOCKADDR_IN * psockaddr, PXONLINETASK_SOCKIO psockio);
    VOID            StreamConnectInitializeContext(SOCKET socketIn, HANDLE hWorkEvent, PXONLINETASK_SOCKIO psockio);
    HRESULT         StreamConnect(SOCKET socketIn, SOCKADDR_IN * psockaddr, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask);
    HRESULT         GetStreamConnectResults(XONLINETASK_HANDLE hTask);
    HRESULT         SendRecvContinue(XONLINETASK_HANDLE hTask);
    VOID            SendRecvClose(XONLINETASK_HANDLE hTask);
    HRESULT         SendRecvBegin(PXONLINETASK_SOCKIO psockio, BOOL fSend);
    INLINE HRESULT  SendInternal(PXONLINETASK_SOCKIO psockio) { return(SendRecvBegin(psockio, TRUE)); }
    INLINE HRESULT  RecvInternal(PXONLINETASK_SOCKIO psockio) { return(SendRecvBegin(psockio, FALSE)); }
    VOID            SendRecvInitializeContext(SOCKET socket, LPWSABUF pBuffers, DWORD dwBufferCount, DWORD dwFlags, HANDLE hWorkEvent, PXONLINETASK_SOCKIO psockio);
    HRESULT         SendRecv(SOCKET socket, LPWSABUF pBuffers, DWORD dwBufferCount, DWORD dwFlags, HANDLE hWorkEvent, XONLINETASK_HANDLE    * phTask, BOOL fSend);
    HRESULT         GetSendRecvResults(XONLINETASK_HANDLE hTask, DWORD * pdwBytesTransferred, DWORD * pdwFlags, LPWSABUF * ppBuffers);
    
    // cfcache.cpp -----------------------------------------------------------------------

    HRESULT         CffileCacheInitialize(HANDLE hFile, DWORD dwCffileOffset, WORD wCffiles, PBYTE pbKey, DWORD cbKey, DWORD cbLength, PBYTE pbBuffer, DWORD cbBuffer, PRC4_SHA_HEADER pHeader, HANDLE hEventWorkAvailable, PXONLINECONTENT_CFFILECACHE pcffilecache);
    HRESULT         CffileCacheGetNextEntry(PXONLINECONTENT_CFFILECACHE pcffilecache, DWORD * pdwIndex, CFFILE ** ppcffile, PSTR * ppszName, DWORD * pcbName);
    HRESULT         CffileCacheVerifyChecksum(PXONLINECONTENT_CFFILECACHE pcffilecache);
    VOID            CffileCacheDeinitialize(PXONLINECONTENT_CFFILECACHE pcffilecache);
    HRESULT         CffileCacheVerify(PXONLINECONTENT_CFFILECACHE pcffilecache);

    // contutil.cpp ----------------------------------------------------------------------

    HRESULT         SetEndOfFileNt(HANDLE hFile, DWORD dwSize);
    HRESULT         HexStringToUlonglong(PSTR szHexValue, DWORD cbHexValue, ULONGLONG * pullValue);
    HRESULT         ConvertToTitleId(PSTR szTitleId, DWORD * pdwTitleId);
    HRESULT         ConvertToOfferingId(PSTR szOfferingId, XONLINEOFFERING_ID * pOfferingId);
    HRESULT         ParseContentDirName(PSTR szContentDir, XONLINEOFFERING_ID * pOfferingId, DWORD * pdwBitFilter);
    HRESULT         InitiateReferralRequest(XONLINEOFFERING_ID OfferingId, PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         InitiateAutoupdateReferralRequest(DWORD dwTitleId, ULONGLONG qwMachineId, DWORD dwCurrentDashVersion, PXONLINETASK_AUTOUPD_ONLINE pxoupd);
    HRESULT         ParseReferralResults(PXONLINETASK_CONTENT_DOWNLOAD pcontdl, PBYTE pbBuffer, DWORD cbBuffer);

    // contdl.cpp ------------------------------------------------------------------------

    VOID            CheckCorruption(PXONLINECONTENT_INSTALL pinstall);
    HRESULT         VerifyOrSignManifestBegin(PXONLINECONTENT_INSTALL pinstall, PBYTE pbPackageKey);
    HRESULT         VerifyCabHeader(PXONLINECONTENT_HEADER pheader);
    HRESULT         VerifyHeaderSignature(PXONLINECONTENT_INSTALL pinstall, PXONLINECONTENT_HEADER pheader, PBYTE pbSignature);
    HRESULT         ProcessFixedHeader(PXONLINETASK_CONTENT_DOWNLOAD pcontdl, PBYTE pbheader);
    HRESULT         ProcessCffolderEntries(PXONLINETASK_CONTENT_DOWNLOAD pcontdl, PBYTE pbDataBegin);
    HRESULT         SetupInstallFolder(PXONLINECONTENT_INSTALL pinstall, DWORD dwFolderIndex);
    VOID            ResetAfterUnsuccessfulResume(PXONLINECONTENT_INSTALL pinstall);
    VOID            contdlCleanup(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlDoneHeaders(PVOID pvxrlasync);
    BOOL            contdlProcessData(PVOID pvxrlasync);
    HRESULT         contdlCompleted(PVOID pvxrlasync);
    VOID            contdlCleanupHandler(PVOID pvxrlasync);
    HRESULT         contdlRecvHeader(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlInstallFiles(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlCreateTarget(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlWriteTarget(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlCreateDirectory(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlCreateCache(PXONLINETASK_CONTENT_DOWNLOAD    pcontdl);
    HRESULT         contdlCatref(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlCheckCache(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlReadCache(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlWipeTarget(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlSetupCache(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlVerifyCffiles(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlCheckpoint(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlVerifyInstall(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlPreCustom(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlPostCustom(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlSignDrm(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlDeleteCache(PXONLINETASK_CONTENT_DOWNLOAD pcontdl);
    HRESULT         contdlContinue(XONLINETASK_HANDLE hTask);
    HRESULT         ContentInstallInitializeContext(PXONLINETASK_CONTENT_DOWNLOAD pcontdl, DWORD dwTitleId, XONLINEOFFERING_ID OfferingId, XONLINEDIR_TYPE dirtypeTarget, DWORD dwFlags, DWORD dwTimeout, XONLINETASK_HANDLE hTaskPreCustom, XONLINETASK_HANDLE hTaskPostCustom, HANDLE hWorkEvent);
    HRESULT         BuildExistingContentPath(DWORD dwTitleId, XONLINEOFFERING_ID OfferingId, CHAR *szPostfix, XONLINEDIR_TYPE dirtype, DWORD *pcbTargetPath, CHAR *szTargetPath);

    // contrm.cpp ------------------------------------------------------------------------

    VOID            CheckCorruption(PXONLINETASK_CONTENT_REMOVE pcontrm);
    HRESULT         contrmRemoveDrm(PXONLINETASK_CONTENT_REMOVE pcontrm);
    HRESULT         contrmRemoveTitleData(PXONLINETASK_CONTENT_REMOVE pcontrm);
    HRESULT         contrmRemoveUserData(PXONLINETASK_CONTENT_REMOVE pcontrm);
    HRESULT         contrmCleanup(PXONLINETASK_CONTENT_REMOVE pcontrm);
    HRESULT         contrmContinue(XONLINETASK_HANDLE hTask);
    VOID            contrmClose(XONLINETASK_HANDLE hTask);
    HRESULT         ContentRemoveInternal(DWORD dwTitleId, XONLINEOFFERING_ID OfferingId, DWORD dwFlags, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask);
    
    // contver.cpp -----------------------------------------------------------------------

    VOID            CheckCorruption(PXONLINETASK_CONTENT_VERIFY pcontver);
    HRESULT         contverReadDrm(PXONLINETASK_CONTENT_VERIFY pcontver);
    HRESULT         contverWriteDrm(PXONLINETASK_CONTENT_VERIFY pcontver);
    HRESULT         contverCleanup(PXONLINETASK_CONTENT_VERIFY pcontver);
    HRESULT         contverContinue(XONLINETASK_HANDLE hTask);
    VOID            contverClose(XONLINETASK_HANDLE hTask);
    HRESULT         ContentVerifyInitializeContext(HANDLE hFile, DWORD dwFileSize, PBYTE pbBuffer, DWORD cbBuffer, PBYTE pbKeyPackage, DWORD dwFlags, PDGSTFILE_IDENTIFIER pdfi, XONLINEDIR_TYPE dirtype, HANDLE hWorkEvent, PXONLINETASK_CONTENT_VERIFY pcontver);
    HRESULT         ContentVerifyBegin(PXONLINETASK_CONTENT_VERIFY pcontver);
    HRESULT         ContentVerifyInternal(PDGSTFILE_IDENTIFIER pdfi, PBYTE pbBuffer, DWORD * pcbBuffer, DWORD dwFlags, XONLINEDIR_TYPE dirtype, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask);
    HRESULT         DigestVerify(PSTR szManifestFileName, XONLINEDIR_TYPE dirtype, PBYTE pbBuffer, DWORD *pcbBuffer, PBYTE pbAlternateKey, HANDLE hWorkEvent, PXONLINETASK_CONTENT_VERIFY pcontver);

    // contenum.cpp ----------------------------------------------------------------------

    HRESULT         ValidateAndRelocateCatalogRecord(PXONLINEOFFERING_ENUM_RESP_RECORD prechdr, DWORD cbRecord);
    HRESULT         ParseEnumerationResults(PXONLINETASK_OFFERING_ENUM pcontenum);
    HRESULT         contenumPostUploadContinue(XONLINETASK_HANDLE hTask);
    HRESULT         InitiateEnumerationRequest(PXONLINEOFFERING_ENUM_SERVER penum, PBYTE pbBuffer, DWORD cbBuffer, HANDLE hWorkEvent);
    HRESULT         contenumContinue(XONLINETASK_HANDLE hTask);
    HRESULT         contenumClose(XONLINETASK_HANDLE hTask);
    HRESULT         ContentEnumerateServerBegin(DWORD dwTitleId, DWORD dwUserIndex, PXONLINEOFFERING_ENUM_PARAMS pEnumParams, PBYTE pbBuffer, DWORD cbBuffer, HANDLE hWorkEvent, PXONLINETASK_OFFERING_ENUM pcontenum);
    HRESULT         ContentEnumerateLocalContinue(XONLINETASK_HANDLE hTask);
    HRESULT         ContentEnumerateLocalDone(XONLINETASK_HANDLE hTask);
    HRESULT         ContentEnumerateLocalBegin(PSTR szRootPath, PXONLINEOFFERING_ENUM_PARAMS pEnumParams, PBYTE pbBuffer, DWORD cbBuffer, HANDLE hWorkEvent, PXONLINETASK_OFFERING_ENUM pcontenum);
    HRESULT         EnumerateTitlesContinue(XONLINETASK_HANDLE hTask);

    // dirops.cpp ------------------------------------------------------------------------
    
    HRESULT         FindParentPath(PSTR szPath, DWORD * pdwIndex);
    HRESULT         RestoreChildPath(PSTR szPath, DWORD cbPath, DWORD * pdwIndex);
    HRESULT         DoCreateDirectories(PXONLINETASK_DIROPS pdirops);
    BOOL            IsInSkipList(PXONLINEDIROPS_SKIP_LIST pSkipList, PSTR szPath);
    HRESULT         DoRemoveDirectory(PXONLINETASK_DIROPS pdirops);
    HRESULT         DoEnumerateDirectory(PXONLINETASK_DIROPS pdirops);
    VOID            DiropsCleanup(PXONLINETASK_DIROPS pdirops);
    HRESULT         DiropsContinue(XONLINETASK_HANDLE hTask);
    VOID            DiropsClose(XONLINETASK_HANDLE hTask);
    HRESULT         DiropsInitializeContext(XONLINEDIROPS_OPS dirop, PSTR szPath, DWORD cbPath, DWORD cbMax, PSTR szDir, DWORD dwFlags, XONLINETASK_HANDLE hTaskOnData, XONLINETASK_HANDLE hTaskOnDone, HANDLE hWorkEvent, PXONLINETASK_DIROPS pdirops);
    INLINE HRESULT  CreateDirectoryInitializeContext(PSTR szPath, DWORD cbPath, DWORD dwFlags, HANDLE hWorkEvent, PXONLINETASK_DIROPS pdirops) { return(DiropsInitializeContext(diropCreateDirectory, szPath, cbPath, 0, NULL, dwFlags, NULL, NULL, hWorkEvent, pdirops)); }
    INLINE HRESULT  RemoveDirectoryInitializeContext(PSTR szPath, DWORD cbPath, DWORD cbMax, PSTR szDir, DWORD dwFlags, HANDLE hWorkEvent, PXONLINETASK_DIROPS pdirops) { return(DiropsInitializeContext(diropRemoveDirectory, szPath, cbPath, cbMax, szDir, dwFlags, NULL, NULL, hWorkEvent, pdirops)); }
    INLINE HRESULT  EnumerateDirectoryInitializeContext(PSTR szPath, DWORD cbPath, DWORD cbMax, DWORD dwFlags, XONLINETASK_HANDLE hTaskOnData, XONLINETASK_HANDLE hTaskOnDone, HANDLE hWorkEvent, PXONLINETASK_DIROPS pdirops) { return(DiropsInitializeContext(diropEnumerateDirectory, szPath, cbPath, cbMax, NULL, dwFlags, hTaskOnData, hTaskOnDone, hWorkEvent, pdirops)); }
    HRESULT         dircrawlCrawlDirHandler(PXONLINETASK_DIRCRAWL pdircrawl);
    HRESULT         dircrawlOnDirHandler(PXONLINETASK_DIRCRAWL pdircrawl);
    HRESULT         dircrawlOnFileHandler(PXONLINETASK_DIRCRAWL pdircrawl);
    VOID            DircrawlCleanup(PXONLINETASK_DIRCRAWL pdircrawl);
    HRESULT         dircrawlCleanupHandler(PXONLINETASK_DIRCRAWL pdircrawl);
    HRESULT         DircrawlContinue(XONLINETASK_HANDLE hTask);
    VOID            DircrawlClose(XONLINETASK_HANDLE hTask);
    HRESULT         DircrawlInitializeContext(PSTR szPath, PSTR szTarget, DWORD cbMaxPath, DWORD dwFlags, XONLINETASK_HANDLE hTaskOnDir, XONLINETASK_HANDLE hTaskOnFile, HANDLE hWorkEvent, PXONLINETASK_DIRCRAWL pdircrawl);
    HRESULT         DirCopyDirContinue(XONLINETASK_HANDLE hTask);
    HRESULT         DirCopyFileContinue(XONLINETASK_HANDLE hTask);
    HRESULT         CopyDirectoryInitializeContext(PSTR szSourceDirectory, PSTR szTargetDirectory, DWORD cbMaxPath, DWORD dwFlags, HANDLE hWorkEvent, PXONLINETASK_DIRCOPY pdircopy);
    HRESULT         DiropsCreateDirectory(PSTR szDirectoryPath, DWORD dwFlags, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask);
    HRESULT         DiropsRemoveDirectory(PSTR szDirectoryPath, DWORD cbMaxPathSize, DWORD dwFlags, HANDLE hWorkEvent, XONLINETASK_HANDLE * phTask);

    // download.cpp ----------------------------------------------------------------------

    HRESULT         xrldlPreConnectHandler(PXRL_ASYNC pxrlasync);
    HRESULT         xrldlConnectHandler(PXRL_ASYNC pxrlasync);
    HRESULT         xrldlSendHandler(PXRL_ASYNC pxrlasync);
    HRESULT         xrldlReceiveHandler(PXRL_ASYNC pxrlasync);
    HRESULT         xrldlProcessDataHandler(PXRL_ASYNC pxrlasync);
    HRESULT         xrldlPostDownloadHandler(PXRL_ASYNC pxrlasync);
    HRESULT         xrldlCleanupHandler(PXRL_ASYNC pxrlasync);
    HRESULT         xrldlContinue(XONLINETASK_HANDLE hTask);
    HRESULT         Download(LPCSTR szResourcePath, PBYTE pbExtraHeaders, DWORD cbExtraHeaders, FILETIME * pftModifiedSince, LARGE_INTEGER liResumeFrom, PXRL_ASYNC pxrlasync);
    HRESULT         xrldtmDoneHeaders(PVOID pvxrlasync);    
    BOOL            xrldtmProcessData(PVOID pvxrlasync);
    HRESULT         xrldtmCompleted(PVOID pvxrlasync);
    HRESULT         xrldfDoneHeaders(PVOID pvxrlasync);
    BOOL            xrldfProcessData(PVOID pvxrlasync);
    HRESULT         xrldfCompleted(PVOID pvxrlasync);
    HRESULT         xrldfInitializeTask(XRLDOWNLOAD_EXTTYPE xrldlext, PVOID pvxrlasync, XONLINETASK_HANDLE hTask);
    HRESULT         xrldfGrowFileHandler(PXRL_ASYNC_EXTENDED pxrlasyncext);
    HRESULT         xrldfWriteFileHandler(PXRL_ASYNC_EXTENDED pxrlasyncext);
    HRESULT         xrldfContinue(XONLINETASK_HANDLE hTask);

    // dvdload.cpp -----------------------------------------------------------------------

    HRESULT         dvddlPreConnectHandler(PXRL_ASYNC pxrlasync);
    HRESULT         dvddlEmulateHeadersHandler(PXRL_ASYNC pxrlasync);
    HRESULT         dvddlProcessDataHandler(PXRL_ASYNC pxrlasync);
    HRESULT         dvddlPostDownloadHandler(PXRL_ASYNC pxrlasync);
    HRESULT         dvddlCleanupHandler(PXRL_ASYNC pxrlasync);
    HRESULT         dvddlContinue(XONLINETASK_HANDLE hTask);
    HRESULT         DownloadFromDVD(LPCSTR szResourcePath, FILETIME *pftModifiedSince, LARGE_INTEGER liResumeFrom, PXRL_ASYNC pxrlasync);

    // symmdec.cpp -----------------------------------------------------------------------

    HRESULT         SymmdecInitialize(PBYTE pbKey, DWORD cbKey, DWORD cbLength, PRC4_SHA_HEADER pHeader, PXONLINESYMMDEC_CONTEXT psymmdec);
    HRESULT         SymmdecDecrypt(PXONLINESYMMDEC_CONTEXT psymmdec, PBYTE pbData, DWORD cbData);
    HRESULT         SymmdecVerify(PXONLINESYMMDEC_CONTEXT psymmdec);
    HRESULT         SymmdecGetRemainingCount(PXONLINESYMMDEC_CONTEXT psymmdec, PDWORD pdwBytesRemaining);

    // upload.cpp ------------------------------------------------------------------------

    HRESULT         xrlulPreConnectHandler(PXRL_ASYNC pxrlasync);
    HRESULT         xrlulConnectHandler(PXRL_ASYNC pxrlasync);
    HRESULT         xrlulReadDataHandler(PXRL_ASYNC pxrlasync);
    HRESULT         xrlulSendDataHandler(PXRL_ASYNC pxrlasync);
    HRESULT         xrlulReceiveStatusHandler(PXRL_ASYNC pxrlasync);
    HRESULT         xrlulPostUploadHandler(PXRL_ASYNC pxrlasync);
    HRESULT         xrlulCleanupHandler(PXRL_ASYNC pxrlasync);
    HRESULT         xrlulContinue(XONLINETASK_HANDLE hTask);
    HRESULT         Upload(LPCSTR szTargetPath, PBYTE pbExtraHeaders, DWORD cbExtraHeaders, PXRL_ASYNC pxrlasync);
    HRESULT         xrlulsCompleted(PVOID pvxrlasync);
    HRESULT         xrlulsResponseHeaders(PVOID pvxrlasync);
    HRESULT         xrlufmInitializeTask(XRLUPLOAD_EXTTYPE xrlulext, PVOID pvxrlasync, XONLINETASK_HANDLE hTask);
    HRESULT         xrlufmContinue(XONLINETASK_HANDLE hTask);
    HRESULT         UploadFromMemoryInternal(DWORD dwServiceID, LPCSTR szTargetPath, PBYTE pbBuffer, DWORD cbBuffer, PBYTE pbExtraHeaders, DWORD cbExtraHeaders, PBYTE pbDataToUpload, DWORD cbDataToUpload, DWORD dwTimeout, HANDLE hWorkEvent, PXRL_ASYNC_EXTENDED pxrlasyncext);
    HRESULT         xrlufInitializeTask(XRLUPLOAD_EXTTYPE xrlulext, PVOID pvxrlasync, XONLINETASK_HANDLE hTask);
    HRESULT         xrlufReadDataHandler(PXRL_ASYNC_EXTENDED pxrlasyncext);
    HRESULT         xrlufContinue(XONLINETASK_HANDLE hTask);

    // xontask.cpp -----------------------------------------------------------------------

    VOID            TaskInitializeContext(PXONLINETASK_CONTEXT pContext);
    VOID            TaskVerifyContext(PXONLINETASK_CONTEXT pContext);

    // xrlutil.cpp -----------------------------------------------------------------------

    HRESULT         XRL_FileTimeAsString(LPSTR pBuffer, DWORD * pcbBuffer, PFILETIME pft);
    BOOL            XRL_FileTimeFromString(LPCSTR sz, PFILETIME pft);
    HRESULT         XRL_BuildGetRequestWithResume(DWORD dwServiceID, LPCSTR szResourcePath, DWORD dwIPAddress, char * pBuffer, DWORD * pcbBuffer, PBYTE pbExtraHeaders, DWORD cbExtraHeaders, FILETIME * pModifiedSince, LARGE_INTEGER liResumeFrom);
    HRESULT         XRL_BuildPostRequest(DWORD dwServiceID, LPCSTR szTargetPath, DWORD dwIPAddress, char * pBuffer, DWORD * pcbBuffer, PBYTE pbExtraHeaders, DWORD cbExtraHeaders, ULARGE_INTEGER uliFileSize);
    HRESULT         XRL_LookupServiceIPAddress(PXONLINE_SERVICE_INFO pService, SOCKADDR_IN * psockaddr);
    HRESULT         XRL_SkipToURI(PBYTE pbXRL, DWORD cbXRL, PBYTE * ppbURI, DWORD * pcbURI);
    HRESULT         XRL_ConvertToIPAddressAndPort(PBYTE pbHost, DWORD cbHost, DWORD * pdwIPAddress, WORD *pwPort);
    HRESULT         XRL_AsyncConnect(PXRL_ASYNC pxrlasync);
    BOOL            XRL_ParseOneHeader(LPSTR pStart, PXRL_ASYNC pxrlasync);
    HRESULT         XRL_ProcessHttp(PXRL_ASYNC pxrlasync, DWORD dwNewBytesRead, BOOL * pfDoneHeaders);
    HRESULT         XRL_CreateWorkEventIfNecessary(PXRL_ASYNC pxrlasync, HANDLE * pHandle);
    void            XRL_CleanupHandler(PXRL_ASYNC pxrlasync);
    HRESULT         XRL_CheckDiskSpaceAccurate(char * szFilename, ULARGE_INTEGER uliFileSize);
    HRESULT         XRL_MainContinue(XONLINETASK_HANDLE hTask, const PFNXRL_HANDLER rgpfnxrlHandlers[], DWORD dwFinalState);
    VOID            XRL_MainClose(XONLINETASK_HANDLE hTask);

    // xonlzx.cpp ------------------------------------------------------------------------

    void            LzxResetDecoder(PXONLINE_LZXDECODER_CONTEXT pContext);
    bool            LzxDecodeInit(byte * pbWindowBuffer, ulong ulWindowSize, PXONLINE_LZXDECODER_CONTEXT pContext);
    void            LzxEnableExtraLongMatches(PXONLINE_LZXDECODER_CONTEXT    pContext);
    bool            LzxDecode(PXONLINE_LZXDECODER_CONTEXT pContext, ulong cbDecodedSize, byte * pbInput, ulong cbInput, byte * pbOutput, ulong *     pcbBytesDecoded);
    bool            LzxPopulateDictionary(PXONLINE_LZXDECODER_CONTEXT pContext, byte *pbData, ulong cbData);

    // logon.cpp -------------------------------------------------------------------------

    HRESULT         DNSLookupContinue(XONLINETASK_HANDLE hTask);
    void            DNSLookupClose(XONLINETASK_HANDLE hTask);
    HRESULT         DNSLookup(char * pszDNSName, HANDLE hWorkEvent, XONLINETASK_DNS_LOOKUP * pDNSLookupTask);
    HRESULT         DNSLookupGetResults(XONLINETASK_HANDLE hTask, XNDNS ** ppDestAddr);
    BOOL            DNSLookupIsValidTask(XONLINETASK_HANDLE hTask);
    HRESULT         GetSingleTicketSendAsRequest(PXONLINETASK_GET_SINGLE_TICKET pGetSingleTicketTask);
    HRESULT         GetSingleTicketContinue(XONLINETASK_HANDLE hTask);
    void            GetSingleTicketClose(XONLINETASK_HANDLE hTask);
    HRESULT         GetSingleTicket(BOOL fMachineAccount, PXKERB_TGT_CONTEXT pInputTGTContext, PXONLINE_USER pUser, HANDLE hWorkEvent, PXONLINETASK_HANDLE phGetSingleTicketTask);
    HRESULT         GetSingleTicketGetResults(XONLINETASK_HANDLE hTask, PXKERB_TGT_CONTEXT * ppSingleTGTContext);
    HRESULT         GetMachineAccountSendAsRequest(PXONLINETASK_GET_MACHINE_ACCOUNT pGetMachineAccountTask);
    HRESULT         GetMachineAccountContinue(XONLINETASK_HANDLE hTask);
    void            GetMachineAccountClose(XONLINETASK_HANDLE hTask);
    HRESULT         GetMachineAccount(HANDLE hWorkEvent, PXONLINETASK_HANDLE phGetMachineAccountTask);
    HRESULT         GetMachineAccountGetResults(XONLINETASK_HANDLE hTask, PXONLINE_USER pMachineUser);
    HRESULT         GetCombinedTicketContinue(XONLINETASK_HANDLE hTask);
    void            GetCombinedTicketClose(XONLINETASK_HANDLE hTask);
    HRESULT         GetCombinedTicket(PXONLINE_USER pMachineUser, BOOL fUseCache, HANDLE hWorkEvent, PXONLINETASK_HANDLE phGetCombinedTicketTask);
    HRESULT         GetCombinedTicketGetResults(XONLINETASK_HANDLE hTask, PXKERB_TGT_CONTEXT* ppCombinedTGTContext, BOOL* pfTicketFromCache);
    HRESULT         GetServiceTicketSendTgsRequest(PXONLINETASK_GET_SERVICE_TICKET pGetServiceTicketTask);
    HRESULT         GetServiceTicketContinue(XONLINETASK_HANDLE hTask);
    void            GetServiceTicketClose(XONLINETASK_HANDLE hTask);
    HRESULT         GetServiceTicket(PXKERB_TGT_CONTEXT pCombinedTGTContext, DWORD* pdwServiceIDs, DWORD cServices, BOOL fUseCache, HANDLE hWorkEvent, PXONLINETASK_HANDLE phGetServiceTicketTask);
    HRESULT         GetServiceTicketGetResults(XONLINETASK_HANDLE hTask, PXKERB_SERVICE_CONTEXT * ppXKerbServiceContext, BOOL* pfTicketFromCache);
    HRESULT         LogonTaskContinue(XONLINETASK_HANDLE hTask);
    void            LogonTaskClose(XONLINETASK_HANDLE hTask);

    // match.cpp -------------------------------------------------------------------------

    HRESULT         SessionCreate(XNKID SessionID, DWORD dwPublicCurrent, DWORD dwPublicAvailable, DWORD dwPrivateCurrent, DWORD dwPrivateAvailable, DWORD dwNumAttributes, PXONLINE_ATTRIBUTE pAttributes, HANDLE hWorkEvent, PXONLINETASK_HANDLE phTask);
    HRESULT         MatchSearch(DWORD dwProcedureIndex, DWORD dwNumResults, DWORD dwNumAttributes, PXONLINE_ATTRIBUTE pAttributes, DWORD dwResultsLen, HANDLE hWorkEvent, PXONLINETASK_HANDLE phTask);
    HRESULT         CreateContext(PXMATCH_EXTENDED_HANDLE *ppExtendedHandle, DWORD dwMsgLen, DWORD dwResultsLen, XNKID SessionID, HANDLE hWorkEvent, DWORD dwNumResults, XMATCH_TYPE Type);
    DWORD           TotalAttributeSize(DWORD dwNumAttributes, PXONLINE_ATTRIBUTE pAttributes);
    VOID            WriteAttributes(PBYTE pbBufferStart, DWORD dwNumAttributes, PXONLINE_ATTRIBUTE pAttributes, DWORD * pdwAttributeOffset, PBYTE pbAttribute);
    HRESULT         MatchContinue(XONLINETASK_HANDLE hTask);
    VOID            MatchClose(XONLINETASK_HANDLE hTask);
    VOID            FixupAttributeIDs(DWORD dwNumAttributes, PXONLINE_ATTRIBUTE pAttributes);
    BOOL            ValidateAttributeIDs(DWORD dwNumAttributes, PXONLINE_ATTRIBUTE pAttributes, BOOL fSessionCreate);
    BOOL            IsValidAttributeID(DWORD dwAttributeID, BOOL fSessionCreate);

    // service.cpp -----------------------------------------------------------------------

    HRESULT         InitOverrideInfo(LPSTR pszXOnlineIniFile = NULL);

    // users.cpp -------------------------------------------------------------------------

    VOID            CompleteRecentLogonTimeCacheOpenTask();
    VOID            CopyAndAdjustUser(PXC_ONLINE_USER_ACCOUNT_STRUCT pUserAccount, DWORD dwMUOptions, PXONLINEP_USER pUser);
    VOID            CollectValidUser(PXC_ONLINE_USER_ACCOUNT_STRUCT pUserAccount, DWORD dwMUOptions, FILETIME* pLastLogon, PXONLINEP_USER pUsers, DWORD *pcUsers);

    // presence.cpp ----------------------------------------------------------------------

    HRESULT         NotificationEnumeratePriv(DWORD dwUserIndex, PXONLINE_NOTIFICATION_MSG pbBuffer, WORD wBufferCount, DWORD dwNotificationTypes, PXPRESENCE_EXTENDED_HANDLE pExtendedHandle);
    HRESULT         NotificationSendAlive(PXONLINE_USER pUsers, HANDLE hEvent, PXONLINETASK_HANDLE phTask);
    HRESULT         CreateContext(PXPRESENCE_EXTENDED_HANDLE *ppExtendedHandle, HANDLE hEvent, XPRESENCE_REQUESTTYPE Type, DWORD dwUserIndex);
    HRESULT         CreateMsg(DWORD dwMsgType, DWORD dwMsgLen, PXPRESENCE_EXTENDED_HANDLE pExtendedHandle, SERVER_TYPE Server, PBYTE * ppbMsgBuffer, UINT * pcbMsgBuffer, PBYTE *ppbMsg, DWORD *pdwSeqNum);
    HRESULT         SendMsg(PBYTE pbMsgBuffer, UINT cbMsgBuffer, DWORD dwBufferSize, PXPRESENCE_EXTENDED_HANDLE pExtendedHandle, SERVER_TYPE Server);
    HRESULT         NotificationContinue(XONLINETASK_HANDLE hTask);
    HRESULT         FriendsEnumerateContinue(XONLINETASK_HANDLE hTask);
    HRESULT         FriendsContinue(XONLINETASK_HANDLE hTask);
    HRESULT         HandleGameInvite(XONLINETASK_HANDLE hTask);
    HRESULT         GameInviteContinue(XONLINETASK_HANDLE hTask);
    VOID            GameInviteClose(XONLINETASK_HANDLE hTask);
    VOID            PresenceClose(XONLINETASK_HANDLE hTask);
    PXPRESENCE_USER GetUserFromID(ULONGLONG qwUserID);
    DWORD           GetUserIndexFromID(ULONGLONG qwUserID);
    PXPRESENCE_USER GetUserFromSeqNum(DWORD dwSeqNum);
    HRESULT         FriendListRefresh(PXPRESENCE_EXTENDED_HANDLE pExtendedHandle);
    VOID            ResetFriendList(DWORD dwSeqNum, DWORD dwFriendListVersion, WORD cFriends, P_REPLY_BUDDY * pFriend);
    VOID            ResetLockoutList(DWORD dwSeqNum, DWORD dwLockoutListVersion, WORD cLockouts, P_REPLY_BLOCK *pLockout);
    DWORD           PreprocessNotifications(Q_LIST_REPLY_MSG * pQListReply);
    HRESULT         UpdateUser(PXPRESENCE_USER pUser, ULONGLONG qwFriendID, PBYTE pUserName, DWORD cbUserName, DWORD dwTitleID, DWORD dwState, DWORD dwFriendFlags, ULONGLONG qwMatchSessionID, WORD cbStateData, PBYTE pStateData, WORD cbUserData, PBYTE pUserData, DWORD dwVersion);
    VOID            SetUserFlags(PXPRESENCE_USER pUser, ULONGLONG qwFriendID, DWORD dwFlags, BOOL fSet);
    HRESULT         RemoveUser(PXPRESENCE_USER pUser, ULONGLONG qwFriendID, DWORD dwVersion);
    HRESULT         AddLockoutUser(PXPRESENCE_USER pUser, ULONGLONG qwLockoutID, LPSTR pUserName);
    HRESULT         RemoveLockoutUser(PXPRESENCE_USER pUser, ULONGLONG qwLockoutID);

    BOOL            IsValidStateFlags(DWORD dwStateFlags);
    BOOL            IsValidNotificationType(DWORD dwType);
    WORD            NumGuests(ULONGLONG qwUserID);

    // billing.cpp -----------------------------------------------------------------------

    HRESULT         DoPurchaseContinue(XONLINETASK_HANDLE hTask);
    VOID            DoPurchaseClose(XONLINETASK_HANDLE hTask);
    HRESULT         DoCancelContinue(XONLINETASK_HANDLE hTask);
    VOID            DoCancelClose(XONLINETASK_HANDLE hTask);
    HRESULT         DoDetailsContinue(XONLINETASK_HANDLE hTask);
    VOID            DoDetailsClose(XONLINETASK_HANDLE hTask);
    VOID            XOParsePrices(XONLINETASK_HANDLE hTask);
    HRESULT         VerifyLicenseTaskContinue(XONLINETASK_HANDLE hTask);
    VOID            VerifyLicenseTaskClose(XONLINETASK_HANDLE hTask);
    HRESULT         DoVerifyCacheOpen(XO_OFFERING_VERIFY_LICENSE_TASK* pVerifyTask);
    HRESULT         DoVerifyCacheLookup(XO_OFFERING_VERIFY_LICENSE_TASK* pVerifyTask);
    HRESULT         DoVerifyServerRequest(XO_OFFERING_VERIFY_LICENSE_TASK* pVerifyTask);
    HRESULT         DoVerifyCacheUpdate(XO_OFFERING_VERIFY_LICENSE_TASK* pVerifyTask);
    HRESULT         DoVerifyCacheClose(XO_OFFERING_VERIFY_LICENSE_TASK* pVerifyTask);
    HRESULT         VerifyRights(XO_OFFERING_LICENSE* lic, ULONGLONG puid);

    // localcache.cpp --------------------------------------------------------------------

    HRESULT         CacheOpen(XONLC_TYPE cacheType, PFNXONLC_COMPARE_HANDLER pfnCompareFunc, HANDLE hWorkEvent, PHANDLE phCache, PXONLINETASK_HANDLE phTask);
    HRESULT         CacheUpdate(HANDLE hCache, XONLINETASK_HANDLE hTask, PBYTE pbIndexData, PBYTE pbRecord, BOOL bWriteIndex = TRUE);
    HRESULT         CacheIndexFlush(HANDLE hCache, XONLINETASK_HANDLE hTask);
    HRESULT         CacheLookup(HANDLE hCache, PXONLC_CONTEXT pContext);
    HRESULT         CacheRetrieve(HANDLE hCache, XONLINETASK_HANDLE hTask, PXONLC_CONTEXT pContext);
    HRESULT         CacheDelete(HANDLE hCache, PXONLC_CONTEXT pContext);
    HRESULT         CacheFileDelete(XONLC_TYPE cacheType);
    HRESULT         CacheReset(HANDLE hCache, XONLINETASK_HANDLE hTask);
    HRESULT         CacheClose(HANDLE hCache, XONLINETASK_HANDLE hTask);
    HRESULT         CacheClose(HANDLE hCache);
    HRESULT         CacheContinue(XONLINETASK_HANDLE hTask);
    VOID            CacheTaskClose(XONLINETASK_HANDLE hTask);
    HRESULT         CacheUpdateMRUListPriv(XONLC_CACHE_HANDLE hCache, XONLINETASK_HANDLE hTask, PBYTE pbIndexData, PBYTE pbRecord);
    HRESULT         CacheUpdateIndexPriv(XONLC_CACHE_HANDLE hCache, XONLINETASK_HANDLE hTask, PBYTE pbIndexData);
    HRESULT         CacheUpdateRecordPriv(XONLC_CACHE_HANDLE hCache, XONLINETASK_HANDLE hTask, PBYTE pbRecord);

    // msgclient.cpp --------------------------------------------------------------------

    VOID            InitializeMsgClientContext( XONLINETASK_MSGCLIENT hMsgTask, HANDLE hWorkEvent);
    HRESULT         MsgClientInternal(XONLINETASK_MSGCLIENT hMsgTask, PXONMSGCLIENT_REQUEST pRequest, PXONMSGCLIENT_RESPONSE pResponse);
    HRESULT         DoMsgClientContinue(XONLINETASK_HANDLE hTask);
    VOID            DoMsgClientTaskClose(XONLINETASK_HANDLE hTask);
    HRESULT         ParseMsgServerResults( PXRL_ASYNC pXrlAsync );
        
    HRESULT         TitleCacheInit( HANDLE hWorkEvent, PXONLINETASK_HANDLE phTask);
    BOOL            TitleCacheRetrieve( XONLINETASK_HANDLE hTask, DWORD dwTitleID, PXONLINE_TITLEDATA pTitleData);
    VOID            InitializeTitleCacheContext( XONLINETASK_TITLECACHE hTITLECACHETask, HANDLE hWorkEvent );
    BOOL            DoTitleCacheRetrieveBegin( XONLINETASK_TITLECACHE hTITLECACHETask );
    HRESULT         StartLocalCacheRetrieve( XONLINETASK_TITLECACHE   hTITLECACHETask );
    HRESULT         StartMsgServerRetrieve( XONLINETASK_TITLECACHE   hTITLECACHETask );
    HRESULT         StartLocalCacheUpdate( XONLINETASK_TITLECACHE   hTITLECACHETask );
    HRESULT         DoTitleCacheContinue(XONLINETASK_HANDLE hTask);
    VOID            DoTitleCacheTaskClose(XONLINETASK_HANDLE hTask);

    // accounts.cpp --------------------------------------------------------------------

    HRESULT         DoAcctTempCreateContinue(XONLINETASK_HANDLE hTask);
    void            DoAcctTempCreateClose(XONLINETASK_HANDLE hTask);
    HRESULT         DoGetTagsContinue( XONLINETASK_HANDLE hTask );
    void            DoGetTagsClose(XONLINETASK_HANDLE hTask);
    HRESULT         DoReserveNameContinue( XONLINETASK_HANDLE hTask );
    void            DoReserveNameClose(XONLINETASK_HANDLE hTask);
    HRESULT         DoCreateAccountContinue( XONLINETASK_HANDLE hTask );
    void            DoCreateAccountClose(XONLINETASK_HANDLE hTask);
    LPBYTE          WToUtf8(PWORD pwLen, LPBYTE pbData, LPCWSTR pwszStr);

    // olddash.cpp --------------------------------------------------------------------

    HRESULT         xododContinue(XONLINETASK_HANDLE hTask);
    VOID            xododClose(XONLINETASK_HANDLE hTask);
    HRESULT         XOnlineUpdateDeleteOfflineDash(HANDLE hWorkEvent, PXONLINETASK_DEL_OLD_DASH	pxodod);

    // Windows support -------------------------------------------------------------------

    #ifdef XONLINE_FEATURE_WINDOWS
    DWORD           XGetParentalControlSetting();
    DWORD           XGetLanguage();
    INLINE BYTE *   GetHdKey()          { return(m_abHdKey); }
    #else
    INLINE BYTE *   GetHdKey()          { return((BYTE *)(*XboxHDKey)); }
    #endif

    // Global Data -----------------------------------------------------------------------

    DWORD                               m_cRefs;
    DWORD                               m_dwTitleId;
    DWORD                               m_dwTitleVersion;
    DWORD                               m_dwTitleGameRegion;
    BOOL                                m_fWsaStartup;
    BOOL                                m_fGetNewMachineAccount;
    
    ULONGLONG                           m_qwMachineId;
    
    HANDLE                              m_hConfigVolume;
    
    #ifdef XONLINE_FEATURE_INSECURE
    IN_ADDR                             m_sgAddr;
    XONLINE_SERVICE_INFO                m_rgServiceInfoOverride[XONLINE_MAX_SERVICE_REQUEST];
    DWORD                               m_ctServiceInfoOverride;
    BOOL                                m_fBypassAuth;
    BOOL                                m_fBypassSG;
    BOOL                                m_fForceNewMachineAccount;
    #endif

    struct tagASN1module_t              m_KRB5_Module;

    #if DBG
    BOOL                                m_fSessionCreateInProgress;
    #endif

    #ifdef XNET_FEATURE_ASSERT
    CLeakInfo                           m_LeakInfo;
    #endif

    // Logon Data ------------------------------------------------------------------------

    PXONLINETASK_LOGON                  m_pLogonTask;
    BOOL                                m_fLogonCompletedSuccessfully;
    SERVICE_CONTEXT                     m_rgServiceContexts[XONLINE_MAX_SERVICE_REQUEST];
    XONLINE_USER                        m_rgLogonUsers[XONLINE_MAX_LOGON_USERS];
    DWORD                               m_dwUniqueLogonUsers;
    PXONLINE_USER                       m_apUniqueLogonUsers[XONLINE_MAX_LOGON_USERS];
    DWORD                               m_ctAllocatedServiceContexts;
    PXKERB_SERVICE_CONTEXT              m_pAllocatedServiceContexts[XONLINE_MAX_SERVICE_REQUEST];
    XONLINE_DNS_CACHE_ENTRY             m_DNSCache[XONLINE_NUM_DNS_CACHE_ENTRYS];

    //
    // Hard coded to only match 2 services beyond the sequential ones.
    //
    INLINE DWORD GetServiceArrayIndex(DWORD dwServiceID)
    {
        Assert( XONLINE_MAX_SERVICE_REQUEST == XONLINE_NUMBER_SEQUENTIAL_SERVICES + 2 );
        
        if (dwServiceID < XONLINE_NUMBER_SEQUENTIAL_SERVICES)
        {
            return dwServiceID;
        }
        if (m_rgServiceContexts[XONLINE_NUMBER_SEQUENTIAL_SERVICES].serviceInfo.dwServiceID == dwServiceID)
        {
            return XONLINE_NUMBER_SEQUENTIAL_SERVICES;
        }
        if (m_rgServiceContexts[XONLINE_NUMBER_SEQUENTIAL_SERVICES+1].serviceInfo.dwServiceID == dwServiceID)
        {
            return XONLINE_NUMBER_SEQUENTIAL_SERVICES+1;
        }
        return XONLINE_INVALID_SERVICE;
    }
    
    // Local Cache usage
    #define XON_CACHE_STATE_INITIAL     0
    #define XON_CACHE_STATE_OPENING     1
    #define XON_CACHE_STATE_OPENED      2
    #define XON_CACHE_STATE_UPDATING    3
    #define XON_CACHE_STATE_UPDATED     4
    #define XON_CACHE_STATE_FLUSHING    5
    #define XON_CACHE_STATE_DONE        6
    #define XON_CACHE_STATE_ERROR       7
    
    HANDLE                              m_hLogonTimeCache;
    XONLINETASK_HANDLE                  m_hLogonTimeCacheTask;
    DWORD                               m_dwLogonTimeCacheState;

    // Presence Data ---------------------------------------------------------------------

    XPRESENCE_EXTENDED_HANDLE *         m_pLockoutlistHandle;
    XPRESENCE_EXTENDED_HANDLE *         m_pFriendsHandle;
    // User list for storing friend lists and other context information
    // @@@ drm: This is HUGE.  Should be made smaller or allocated on demand.
    XPRESENCE_USER                      m_Users[4];
    // Global sequence number used to match up requests with responses (either direct
    // responses or notifications through the notification queue).
    DWORD                               m_dwSeqNum;
    // Flag indicated if ALIVE messages have been sent yet.  Used to determine whether SetUserData()
    // API should send the userdata in the ALIVE message or send it as a NICKNAME message.
    BOOL                                m_fAliveDone;
    HANDLE                              m_hCacheGameInvite;
    XONLINETASK_HANDLE                  m_hTaskGameInvite;
    XPRESENCE_GAMEINVITE_RECORD *       m_precordGameInvite;
    XONLC_CONTEXT                       m_contextGameInvite;
    XPRESENCE_CACHESTATE                m_cachestateGameInvite;
    BOOL                                m_fGameInvite;

    // Local Cache Data ------------------------------------------------------------------

    XONLC_CACHE_HANDLE                  m_xonCacheHandles[XONLC_MAX_CACHE_TYPES];

    // Debugging Data --------------------------------------------------------------------

    #if DBG
    LARGE_INTEGER                       m_liTotalTime;
    LARGE_INTEGER                       m_liServiceCount;
    LARGE_INTEGER                       m_liAverageTime;
    LARGE_INTEGER                       m_lirmTotalTime;
    LARGE_INTEGER                       m_lirmServiceCount;
    LARGE_INTEGER                       m_lirmAverageTime;
    LARGE_INTEGER                       m_liverTotalTime;
    LARGE_INTEGER                       m_liverServiceCount;
    LARGE_INTEGER                       m_liverAverageTime;
    #endif

    // Windows Data ----------------------------------------------------------------------

    #ifdef XONLINE_FEATURE_WINDOWS
    BYTE                                m_abHdKey[16];
    #endif

    // Static Data -----------------------------------------------------------------------

    static const PFNAUTOUPD_DVD_HANDLER s_rgpfndvdupdHandlers[];
    static const PFNAUTOUPD_ONLINE_HANDLER s_rgpfnxoupdHandlers[];
    static const PFNCONTDL_HANDLER      s_rgpfncontdlHandlers[];
    static const PFNDIRCRAWL_HANDLER    s_rgpfndircrawlHandlers[];
    static const PFNXRL_HANDLER         s_rgpfnxrldlHandlers[];
    static const PFNXRL_HANDLER         s_rgpfnxrlulHandlers[];
    static const PFNXRL_HANDLER         s_rgpfndvddlHandlers[];
    #if DBG
    static const char * const           s_rgszPhases[];
    static const char * const           s_rgszRemovePhases[];
    static const char * const           s_rgszVerifyPhases[];
    #endif
};

// ---------------------------------------------------------------------------------------
// Inlines
// ---------------------------------------------------------------------------------------

INLINE void * XoSysAlloc(size_t cb, ULONG tag)
{
#ifdef XNET_FEATURE_VMEM
    if (VMemIsEnabled())
        return(VMemAlloc(cb));
#endif
#ifdef XONLINE_FEATURE_WINDOWS
    return(LocalAlloc(LMEM_FIXED, cb));
#else
    return(ExAllocatePoolWithTag(cb, tag));
#endif
}

INLINE void XoSysFree(void * pv)
{
    if (pv != NULL)
    {
    #ifdef XNET_FEATURE_VMEM
        if (VMemIsEnabled())
        {
            VMemFree(pv);
            return;
        }
    #endif

    #ifdef XONLINE_FEATURE_WINDOWS
        LocalFree(pv);
    #else
        ExFreePool(pv);
    #endif
    }
}

INLINE void * CXo::SysAlloc(size_t cb, ULONG tag)
{
    void * pv = XoSysAlloc(cb, tag);

#ifdef XNET_FEATURE_ASSERT
    XnLeakAdd(&m_LeakInfo, pv, cb, tag);
#endif

    return(pv);
}

INLINE void * CXo::SysAllocZ(size_t cb, ULONG tag)
{
    void * pv = XoSysAlloc(cb, tag);

    if (pv != NULL)
    {
        #ifdef XNET_FEATURE_ASSERT
        XnLeakAdd(&m_LeakInfo, pv, cb, tag);
        #endif

        memset(pv, 0, cb);
    }

    return(pv);
}

INLINE void CXo::SysFree(void * pv)
{
    #ifdef XNET_FEATURE_ASSERT
    XnLeakDel(&m_LeakInfo, pv);
    #endif

    XoSysFree(pv);
}

#ifdef XONLINE_FEATURE_XBOX
extern CXo * g_pXo;
#define GetXo()      g_pXo
#define GetXoRef()  &g_pXo
#endif

// ---------------------------------------------------------------------------------------

#endif
