/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: PROFILER.H                                                        *
*   Profiler private interface                                              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 07/06/98 - Created                      *
*                                                                           *
\***************************************************************************/
#ifndef _profiler_h
#define _profiler_h

/*
 * constants
 */
#define CAPMAX          65536*2
#define CAPMARK         (CAPMAX - 32)

/*
 * implementation interface
 */
struct UCL_PROFILER : public NVUCL_PROFILER
{
    /*
     * exported
     */
public:
    NVUCL_RESULT NVUCL_STDCALL attach        (void);
    NVUCL_RESULT NVUCL_STDCALL detach        (void);

    NVUCL_RESULT NVUCL_STDCALL registerEvent (LPCSTR cszName,DWORD dwID);
    NVUCL_RESULT NVUCL_STDCALL registerCount (LPCSTR cszName,DWORD dwID);
    NVUCL_RESULT NVUCL_STDCALL registerTime  (LPCSTR cszName,DWORD dwID);

    NVUCL_RESULT NVUCL_STDCALL logEvent      (DWORD  dwID);
    NVUCL_RESULT NVUCL_STDCALL logCount      (DWORD  dwID,int iCount);
    NVUCL_RESULT NVUCL_STDCALL logTime       (DWORD  dwID,__int64 iTime);

    NVUCL_RESULT NVUCL_STDCALL startCapture  (LPCSTR cszFileName);
    NVUCL_RESULT NVUCL_STDCALL stopCapture   (void);
    NVUCL_RESULT NVUCL_STDCALL flush         (BOOL bOptional);

    NVUCL_RESULT NVUCL_STDCALL getCPUTime    (__int64 *pdwTicks);
    NVUCL_RESULT NVUCL_STDCALL getCPUSpeed   (DWORD *pdwSpeed);

    /*
     * private
     */
protected:

    /*
     * structures
     */
    struct ALPHABET
    {
        char  szName[16];               // human readable description
        DWORD dwRecordSize;             // size of record
        DWORD dwAttributes;             // Record description
    };
    
    /*
     * member variables
     */
    DWORD     m_dwRefCount;             // reference count
    DWORD     m_dwMax;                  // alphabet max allocated
    DWORD     m_dwCount;                // alphabet count
    HANDLE    m_hFile;                  // capture file handle
    DWORD     m_dwIndex;                // index in local capture file
    ALPHABET *m_pAlphabet;              // alphabet (may be NULL)
    DWORD     _pad[2];
    BYTE      m_pBuffer[CAPMAX];        // capture buffer
    DWORD     m_dwTranslationCount;     // max number registered
    BYTE      m_bTranslate[256];        // translation table

    /*
     * methods
     */
    void allocAlphabet (DWORD dwPlace);

    /*
     * constructors
     */
public:
    UCL_PROFILER (void);
    virtual ~UCL_PROFILER (void);
};
  
#endif //_profiler_h