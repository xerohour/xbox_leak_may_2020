/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       dsapi.h
 *  Content:    DirectSound API objects and entry points.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  12/06/00    dereks  Created.
 *
 ****************************************************************************/

#ifndef __DSAPI_H__
#define __DSAPI_H__


//
// Global critical section
//

EXTERN_C BOOL g_fDirectSoundTestInFinalRelease;

EXTERN_C CRITICAL_SECTION g_DirectSoundTestCriticalSection;

__inline BOOL DirectSoundTestEnterCriticalSection(void)
{
    if(PASSIVE_LEVEL != KeGetCurrentIrql())
    {
        return FALSE;
    }

    EnterCriticalSection(&g_DirectSoundTestCriticalSection); 

    return TRUE;
}

__inline void DirectSoundTestLeaveCriticalSection(void)
{
    LeaveCriticalSection(&g_DirectSoundTestCriticalSection);
}

//
// 3D parameters
//

BEGIN_DEFINE_STRUCT()
    DS3DLISTENER    HrtfParams;       // 3D source parameters
    DSI3DL2LISTENER I3dl2Params;      // I3DL2 source parameters
    DWORD           dwParameterMask;  // 3D source properties that have changed
END_DEFINE_STRUCT(DS3DLISTENERPARAMS);

BEGIN_DEFINE_STRUCT()
    DS3DBUFFER      HrtfParams;       // 3D source parameters
    DSI3DL2BUFFER   I3dl2Params;      // I3DL2 source parameters
    DWORD           dwParameterMask;  // 3D source properties that have changed
END_DEFINE_STRUCT(DS3DSOURCEPARAMS);

#ifdef __cplusplus

//
// Forward declarations
//

class CMcpxAPUTest;

//
// Validation object and corresponding macros
//

#ifdef VALIDATE_PARAMETERS

template <DWORD dwSignature> class CValidObject
{
private:
    const DWORD             m_dwSignature;      // Object signature

public:
    CValidObject(void);
    virtual ~CValidObject(void);

public:
    void __AssertValidObject(LPCSTR pszFile, DWORD dwLine, LPCSTR pszFunction);
};

template <DWORD dwSignature> CValidObject<dwSignature>::CValidObject(void)
    : m_dwSignature(dwSignature)
{
}

template <DWORD dwSignature> CValidObject<dwSignature>::~CValidObject(void)
{
}

template <DWORD dwSignature> void CValidObject<dwSignature>::__AssertValidObject(LPCSTR pszFile, DWORD dwLine, LPCSTR pszFunction)
{
    if(!this)
    {
        DwDbgSetContext(DPRINTF_DEFAULT, DPFLVL_ERROR, pszFile, dwLine, pszFunction, DPF_LIBRARY);
        DwDbgPrint("NULL \"this\" pointer (%x)", this);
    }
    else if(m_dwSignature != dwSignature)
    {
        DwDbgSetContext(DPRINTF_DEFAULT, DPFLVL_ERROR, pszFile, dwLine, pszFunction, DPF_LIBRARY);
        DwDbgPrint("Invalid \"this\" pointer (%x != %x)", m_dwSignature, dwSignature);
    }
}

#define _AssertValidObject(fname) \
    __AssertValidObject(__FILE__, __LINE__, fname)

#define AssertValidObject() \
    _AssertValidObject(DPF_FNAME)

#else // VALIDATE_PARAMETERS

#define _AssertValidObject(fname)
#define AssertValidObject()

#endif // VALIDATE_PARAMETERS

//
// Automatic (functon-scope) locking mechanism
//

class CAutoLockTest
{
private:
    BOOL                    m_fLocked;

public:
    CAutoLockTest(void);
    ~CAutoLockTest(void);
};

__inline CAutoLockTest::CAutoLockTest(void)
{
    m_fLocked = (BOOLEAN)DirectSoundTestEnterCriticalSection();
}

__inline CAutoLockTest::~CAutoLockTest(void)
{
    if(m_fLocked)
    {
        DirectSoundTestLeaveCriticalSection();
    }
}

#define AutoLockTest() \
    CAutoLockTest __AutoLockTest

//
// API helper macros
//

#define CHECK_FINAL_RELEASE_HRESULT() \
    if(g_fDirectSoundTestInFinalRelease) \
    { \
        DPF_WARNING("DirectSound is in the final stages of shutting down.  No more API calls are allowed."); \
        return DSERR_GENERIC; \
    }

#define CHECK_FINAL_RELEASE_VOID() \
    if(g_fDirectSoundTestInFinalRelease) \
    { \
        DPF_WARNING("DirectSound is in the final stages of shutting down.  No more API calls are allowed."); \
        return; \
    }

#define ENTER_EXTERNAL_FUNCTION() \
    AutoLockTest();

#define _ENTER_EXTERNAL_METHOD(name) \
    AutoLockTest(); \
    CHECK_FINAL_RELEASE_HRESULT(); \
    _AssertValidObject(name)

#define _ENTER_EXTERNAL_METHOD_VOID(name) \
    AutoLockTest(); \
    CHECK_FINAL_RELEASE_VOID(); \
    _AssertValidObject(name)

#define ENTER_EXTERNAL_METHOD() \
    _ENTER_EXTERNAL_METHOD(DPF_FNAME)

#define ENTER_EXTERNAL_METHOD_VOID() \
    _ENTER_EXTERNAL_METHOD_VOID(DPF_FNAME)

//
// DirectSound settings
//

class CDirectSoundTestSettings
    : public CRefCountTest
{
public:
    DWORD               m_dwSpeakerConfig;                      // Speaker configuration
    DSEFFECTIMAGELOC    m_EffectLocations;                      // Effect locations
    BYTE                m_abMixBinHeadroom[DSMIXBIN_COUNT];     // MixBin headroom

#ifndef MCPX_BOOT_LIB                                           
                                                                
    DS3DLISTENERPARAMS  m_3dParams;                             // 3D parameters

#endif // MCPX_BOOT_LIB

public:
    CDirectSoundTestSettings(void);
    virtual ~CDirectSoundTestSettings(void);

public:
    void SetEffectImageLocations(LPCDSEFFECTIMAGELOC pImageLoc);
};

//
// DirectSound implementation
//

#ifdef VALIDATE_PARAMETERS

class CDirectSoundTest
    : public IDirectSound, public CValidObject<'DSND'>, public CRefCountTest

#else // VALIDATE_PARAMETERS

class CDirectSoundTest
    : public IDirectSound, public CRefCountTest

#endif // VALIDATE_PARAMETERS

{

public:
    static CDirectSoundTest *       m_pDirectSound;                                 // The one-and-only DirectSound object
                                                                                
#ifdef DEBUG                                                                    
                                                                                
    static LPVOID               m_pvEncoderImageData;                           // Encoder image data
    static DWORD                m_dwEncoderImageSize;                           // Encoder image size, in bytes
                                                                                
#endif // DEBUG                                                                 
                                                                                
protected:                                                                      
    CDirectSoundTestSettings *      m_pSettings;                                    // Shared settings object
    CMcpxAPUTest *                  m_pDevice;                                      // Device implementation
    LIST_ENTRY                  m_lst3dVoices;                                  // 3D voice list

#ifdef USE_KEEPALIVE_BUFFERS

    LPDIRECTSOUNDBUFFER         m_apKeepAliveBuffers[USE_KEEPALIVE_BUFFERS];    // Dummy buffers used to keep the mixbins working

#endif // USE_KEEPALIVE_BUFFERS

public:
    CDirectSoundTest(void);
    virtual ~CDirectSoundTest(void);

public:
    // Initialization
    HRESULT STDMETHODCALLTYPE Initialize(void);

    // IUnknown methods
    ULONG STDMETHODCALLTYPE AddRef(void);
    ULONG STDMETHODCALLTYPE Release(void);

};

__inline ULONG CDirectSoundTest::AddRef(void)
{
    _ENTER_EXTERNAL_METHOD("CDirectSoundTest::AddRef");
    return CRefCountTest::AddRef();
}

#ifndef USE_KEEPALIVE_BUFFERS

__inline ULONG CDirectSoundTest::Release(void)
{
    _ENTER_EXTERNAL_METHOD("CDirectSoundTest::Release");
    return CRefCountTest::Release();
}

#endif // USE_KEEPALIVE_BUFFERS



#endif // __cplusplus

#endif // __DSAPI_H__
