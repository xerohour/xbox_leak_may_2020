/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: PROFILER.CPP                                                      *
*   Profiler routines                                                       *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 07/06/98 - Created                      *
*                                                                           *
\***************************************************************************/
#include "precomp.h"
#include "profiler.h"

/*
 * local constants
 */
#define BTFT_1              0x42EC5201  // version 1 file header
#define BTRT_EOF            0xff

#define BTAA_EVENT          0x00000000  // event
#define BTAA_TIME           0x00000001  // time
#define BTAA_COUNT          0x00000002  // count

/*
 * attach
 *
 * bump reference count up
 */
NVUCL_RESULT NVUCL_STDCALL UCL_PROFILER::attach
(
    void
)
{
    m_dwRefCount++;
    return NVUCL_OK;
}

/*
 * detach
 *
 * bump reference count down & delete object if it reaches zero
 */
NVUCL_RESULT NVUCL_STDCALL UCL_PROFILER::detach
(
    void
)
{
    if (--m_dwRefCount)
    {
        return NVUCL_OK;
    }
    /*
     * kill self
     */
    delete this;
    return NVUCL_OK;
}

/*
 * registerEvent
 *
 * register a Locutus Event ID for profile usage
 */
NVUCL_RESULT NVUCL_STDCALL UCL_PROFILER::registerEvent
(
    LPCSTR cszName,
    DWORD  dwID
)
{
    /*
     * program translation
     */
    m_bTranslate[dwID] = (BYTE)m_dwTranslationCount;

    /*
     * make sure we have enough space
     */
    if (m_dwTranslationCount >= m_dwCount)
    {
        allocAlphabet (m_dwTranslationCount);
    }

    /*
     * register entry
     */
    strncpy (m_pAlphabet[m_dwTranslationCount].szName,cszName,15);
    m_pAlphabet[m_dwTranslationCount].szName[15]   = 0;
    m_pAlphabet[m_dwTranslationCount].dwRecordSize = 0;
    m_pAlphabet[m_dwTranslationCount].dwAttributes = BTAA_EVENT;
    m_dwTranslationCount++;

    /*
     * done
     */
    return NVUCL_OK;
}

/*
 * registerCount
 *
 * register a Locutus Count ID for profile usage
 */
NVUCL_RESULT NVUCL_STDCALL UCL_PROFILER::registerCount
(
    LPCSTR cszName,
    DWORD  dwID
)
{
    /*
     * program translation
     */
    m_bTranslate[dwID] = (BYTE)m_dwTranslationCount;

    /*
     * make sure we have enough space
     */
    if (m_dwTranslationCount >= m_dwCount)
    {
        allocAlphabet (m_dwTranslationCount);
    }

    /*
     * register entry
     */
    strncpy (m_pAlphabet[m_dwTranslationCount].szName,cszName,15);
    m_pAlphabet[m_dwTranslationCount].szName[15]   = 0;
    m_pAlphabet[m_dwTranslationCount].dwRecordSize = sizeof(DWORD);
    m_pAlphabet[m_dwTranslationCount].dwAttributes = BTAA_COUNT;
    m_dwTranslationCount++;

    /*
     * done
     */
    return NVUCL_OK;
}

/*
 * registerTime
 *
 * register a Locutus Time ID for profile usage
 */
NVUCL_RESULT NVUCL_STDCALL UCL_PROFILER::registerTime
(
    LPCSTR cszName,
    DWORD  dwID
)
{
    /*
     * program translation
     */
    m_bTranslate[dwID] = (BYTE)m_dwTranslationCount;

    /*
     * make sure we have enough space
     */
    if (m_dwTranslationCount >= m_dwCount)
    {
        allocAlphabet (m_dwTranslationCount);
    }

    /*
     * register entry
     */
    strncpy (m_pAlphabet[m_dwTranslationCount].szName,cszName,15);
    m_pAlphabet[m_dwTranslationCount].szName[15]   = 0;
    m_pAlphabet[m_dwTranslationCount].dwRecordSize = sizeof(__int64);
    m_pAlphabet[m_dwTranslationCount].dwAttributes = BTAA_TIME;
    m_dwTranslationCount++;

    /*
     * done
     */
    return NVUCL_OK;
}

/*
 * logEvent
 *
 * logs a Locutus event
 */
NVUCL_RESULT NVUCL_STDCALL UCL_PROFILER::logEvent
(
    DWORD dwID
)
{
    /*
     * log
     */
    m_pBuffer[m_dwIndex++] = m_bTranslate[dwID];

    /*
     * flush if buffer is full
     */
    if (m_dwIndex > CAPMARK) flush (FALSE);

    /*
     * done
     */
    return NVUCL_OK;
}

/*
 * logCount
 *
 * logs a Locutus count
 */
NVUCL_RESULT NVUCL_STDCALL UCL_PROFILER::logCount
(
    DWORD dwID,
    int   iCount
)
{
    /*
     * log
     */
    m_pBuffer[m_dwIndex++] = m_bTranslate[dwID];
    *(int*)(m_pBuffer + m_dwIndex) = iCount;
    m_dwIndex += 4;

    /*
     * flush if buffer is full
     */
    if (m_dwIndex > CAPMARK) flush (FALSE);

    /*
     * done
     */
    return NVUCL_OK;
}

/*
 * logTime
 *
 * logs a Locutus time
 */
NVUCL_RESULT NVUCL_STDCALL UCL_PROFILER::logTime
(
    DWORD   dwID,
    __int64 iTime
)
{
    /*
     * log
     */
    m_pBuffer[m_dwIndex++] = m_bTranslate[dwID];
    *(__int64*)(m_pBuffer + m_dwIndex) = iTime;
    m_dwIndex += 8;

    /*
     * flush if buffer is full
     */
    if (m_dwIndex > CAPMARK) flush (FALSE);

    /*
     * done
     */
    return NVUCL_OK;
}

/*
 * startCapture
 *
 * starts a capture session
 */
NVUCL_RESULT NVUCL_STDCALL UCL_PROFILER::startCapture
(
    LPCSTR cszFileName
)
{
    /*
     * make sure we have at least one alphabet entry
     */
    if (!m_dwCount)
    {
        return NVUCL_ERROR_INVALIDCALL;
    }

    /*
     * open up file
     */
    m_hFile = CreateFile(cszFileName,GENERIC_WRITE,0,0,CREATE_ALWAYS,0,0);
    if (m_hFile == INVALID_HANDLE_VALUE) {
        return NVUCL_ERROR_OUTOFRESOURCE;
    }

    /*
     * place header information
     */
    m_dwIndex = 0;
    *(DWORD*)(m_pBuffer + m_dwIndex) = BTFT_1;
    m_dwIndex += 4;
    DWORD dw = 1;
    getCPUSpeed (&dw);
    *(DWORD*)(m_pBuffer + m_dwIndex) = dw;
    m_dwIndex += 4;
    *(DWORD*)(m_pBuffer + m_dwIndex) = m_dwCount;
    m_dwIndex += 4;
    memcpy (m_pBuffer + m_dwIndex,m_pAlphabet,sizeof(ALPHABET) * m_dwCount);
    m_dwIndex += sizeof(ALPHABET) * m_dwCount;

    /*
     * done
     */
    return NVUCL_OK;
}

/*
 * stopCapture
 *
 * ends off a capture session
 */
NVUCL_RESULT NVUCL_STDCALL UCL_PROFILER::stopCapture 
(
    void
)
{
    /*
     * make sure we have a capture in progress
     */
    if (!m_hFile)
    {
        return NVUCL_ERROR_INVALIDCALL;
    }

    /*
     * finish off capture
     */
    m_pBuffer[m_dwIndex++] = BTRT_EOF;
    flush (FALSE);
    CloseHandle (m_hFile);
    m_hFile = NULL; // tag as closed

    /*                     
     * done
     */
    return NVUCL_OK;
}

/*
 * flush
 *
 * flush a Locutus batch to disk
 */
NVUCL_RESULT NVUCL_STDCALL UCL_PROFILER::flush
(
    BOOL bOptional
)
{
    /*
     * flush only when we have enough data (50% full)
     */
    if (bOptional && (m_dwIndex < (CAPMAX / 2)))
    {
        return NVUCL_OK;
    }

    /*
     * send data if any
     */
    if (m_dwIndex) {
        /*
         * write data
         */
        DWORD dw;
        if (!WriteFile(m_hFile,m_pBuffer,m_dwIndex,&dw,0)) {
            return NVUCL_ERROR_OUTOFRESOURCE;
        }
        /*
         * make sure it is comitted now
         */
        FlushFileBuffers (m_hFile);
        /*
         * reset index
         */
        m_dwIndex = 0;
    }

    /*
     * done
     */
    return NVUCL_OK;
}

/*
 * getCPUTime
 *
 * get # of clock ticks elasped since startup
 */
NVUCL_RESULT NVUCL_STDCALL UCL_PROFILER::getCPUTime
(
    __int64 *pdwTicks
)
{
    __asm 
    {
        mov ebx,[pdwTicks]
        _emit 0x0f 
        _emit 0x31
        mov dword ptr [ebx],eax
        mov dword ptr [ebx+4],edx
    }
    return NVUCL_OK;
}

/*
 * getCPUSpeed
 *
 * measure the CPU speed (& latch to know values)
 */
NVUCL_RESULT NVUCL_STDCALL UCL_PROFILER::getCPUSpeed
(
    DWORD *pdwSpeed
)
{
    double      speed;
    SYSTEM_INFO si;

    GetSystemInfo (&si);
    if (si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL) return NVUCL_ERROR_UNKNOWNENTITY; // unknown
    if (si.dwProcessorType        != PROCESSOR_INTEL_PENTIUM)      return NVUCL_ERROR_UNKNOWNENTITY; // not pentium

    SetPriorityClass  (GetCurrentProcess(),REALTIME_PRIORITY_CLASS);
    SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
    GetTickCount(); // break in cache
    {
	    DWORD   start,stop;
        __int64 clock_ticks;

	    start = GetTickCount();
	    __asm {
		    pushad
            lea esi,[clock_ticks]

		    _emit 0x0f
		    _emit 0x31
		    mov ebp,eax
		    mov ebx,edx

		    mov ecx,8000000h
	    here:
		    loop here

		    _emit 0x0f
		    _emit 0x31
		    sub eax,ebp
		    sbb edx,ebx

		    mov [dword ptr esi+0],eax
		    mov [dword ptr esi+4],edx

		    popad
	    }
	    stop = GetTickCount();
        speed = ((double)clock_ticks)/(1000.0*(double)(stop-start));
    }
    SetPriorityClass  (GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
    SetThreadPriority (GetCurrentThread(), THREAD_PRIORITY_NORMAL);
 
    /*
     * normalize to known values
     */
#define LATCH_VALUE(x,y)    if (fabs(((x)-(y))/(y)) < 0.05) { *pdwSpeed = (DWORD)(y); return NVUCL_OK; }
    LATCH_VALUE (speed, 90);
    LATCH_VALUE (speed,100);
    LATCH_VALUE (speed,166);
    LATCH_VALUE (speed,200);
    LATCH_VALUE (speed,233);
    LATCH_VALUE (speed,266);
    LATCH_VALUE (speed,300);
    LATCH_VALUE (speed,333);
    LATCH_VALUE (speed,400);
    LATCH_VALUE (speed,450);
    LATCH_VALUE (speed,500);

    /*
     * did not latch - return unaltered (can be up to 10% off)
     */
    *pdwSpeed = (DWORD)(speed);
    return NVUCL_OK;
}

/*
 * allocAlphabet
 *
 * allocates enough space in alphabet to register the dwPlace'th alphabet
 * entry
 */
void UCL_PROFILER::allocAlphabet
(
    DWORD dwPlace
)
{
    /*
     * bump alphabet count if needed
     */
    if (dwPlace >= m_dwCount)
    {
        m_dwCount = dwPlace + 1;
    }

    /*
     * Do we have enough space?
     */
    if (m_dwCount > m_dwMax)
    {
        /*
         * alloc enough space
         */
        m_dwMax     = (m_dwCount + 15) & ~15;        
        m_pAlphabet = (ALPHABET*)realloc(m_pAlphabet,sizeof(ALPHABET) * m_dwMax);
    }
}

/*
 * constructor
 */
UCL_PROFILER::UCL_PROFILER
(
    void
)
{
    m_dwRefCount = 1;
    m_dwMax      = 0;
    m_dwCount    = 0;
    m_pAlphabet  = NULL;
    m_hFile      = NULL;
    m_dwIndex    = 0;
    m_dwTranslationCount = 0;
}

UCL_PROFILER::~UCL_PROFILER
(
    void
)
{
    if (m_hFile)
    {
        CloseHandle (m_hFile);
    }
    if (m_pAlphabet)
    {
        free (m_pAlphabet);
    }
}