//
// dmprfdll.cpp
// 
// Copyright (c) 1997-1999 Microsoft Corporation
//
// Note: Dll entry points as well as class factory implementations.
//

// READ THIS!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
// 4530: C++ exception handler used, but unwind semantics are not enabled. Specify -GX
//
// We disable this because we use exceptions and do *not* specify -GX (USE_NATIVE_EH in
// sources).
//
// The one place we use exceptions is around construction of objects that call 
// InitializeCriticalSection. We guarantee that it is safe to use in this case with
// the restriction given by not using -GX (automatic objects in the call chain between
// throw and handler are not destructed). Turning on -GX buys us nothing but +10% to code
// size because of the unwind code.
//
// Any other use of exceptions must follow these restrictions or -GX must be turned on.
//
// READ THIS!!!!!!!!!!!!!!!!!!!!!!!!!!!
//
#pragma warning(disable:4530)

#include <objbase.h>
#include <initguid.h>
#include <mmsystem.h>
#include <dsoundp.h>

#include "debug.h"
#define ASSERT  assert 
#include "debug.h"
#include "dmprfdll.h"
#include "dmperf.h"
#include "dmsegobj.h"
#include "song.h"
#include "seqtrack.h"
#include "sysextrk.h"
#include "tempotrk.h"
#include "tsigtrk.h"
#include "marktrk.h"
#include "wavtrack.h"
#include "segtrtrk.h"
#include "lyrictrk.h"
#include "ParamTrk.h"
#include "dmgraph.h"
#include "dmusicc.h"
#include "dmusici.h"
#include "dmusicf.h"
#include "oledll.h"
#include "dmstylep.h"
#include "dmbndtrk.h"
#include "dmstrm.h"
#include "Validate.h"
#include "dmksctrl.h"
#include "dmscriptautguids.h"
#include "audpath.h"
#include "dswave.h"
#include "dsoundp.h"            // For IDirectSoundPrivate
#include "..\shared\xguids.h"

bool g_fInitCS = false;
CRITICAL_SECTION g_CritSec;


//////////////////////////////////////////////////////////////////////
// Globals

// Dll's hModule
//
HMODULE g_hModule = NULL;

// Count of active components and class factory server locks
//
long g_cComponent = 0;
long g_cLock = 0;

// Flags DMI_F_xxx from dmusicp.h
//
DWORD g_fFlags;

static char const g_szDoEmulation[] = "DoEmulation";

// CClassFactory::QueryInterface
//
HRESULT __stdcall
CClassFactory::QueryInterface(const IID &iid,
                                    void **ppv)
{
    if (iid == IID_IUnknown || iid == IID_IClassFactory) {
        *ppv = static_cast<IClassFactory*>(this);
    } else {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    reinterpret_cast<IUnknown*>(*ppv)->AddRef();
    return S_OK;
}

CClassFactory::CClassFactory(DWORD dwClassType)

{
    m_cRef = 1;
    m_dwClassType = dwClassType;
    InterlockedIncrement(&g_cLock);
}

CClassFactory::~CClassFactory()

{
    InterlockedDecrement(&g_cLock);
}

// CClassFactory::AddRef
//
ULONG __stdcall
CClassFactory::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

// CClassFactory::Release
//
ULONG __stdcall
CClassFactory::Release()
{
    if (!InterlockedDecrement(&m_cRef)) {
        delete this;
        return 0;
    }

    return m_cRef;
}

// CClassFactory::CreateInstance
//
//
HRESULT __stdcall
CClassFactory::CreateInstance(IUnknown* pUnknownOuter,
                                    const IID& iid,
                                    void** ppv)
{
    HRESULT hr;

    if (pUnknownOuter) {
         return CLASS_E_NOAGGREGATION;
    }

    switch (m_dwClassType)
    {
    case CLASS_PERFORMANCE:
        {
            CPerformance *pInst;

            try
            {
                pInst = new CPerformance;
            }
            catch( ... )
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            if (pInst == NULL) {
                hr = E_OUTOFMEMORY;
                break;
            }

            hr = pInst->QueryInterface(iid, ppv);
            pInst->Release();
        }
        break;
    case CLASS_GRAPH:
        {
            CGraph *pInst = new CGraph;
            if (pInst == NULL) {
                hr = E_OUTOFMEMORY;
                break;
            }

            hr = pInst->QueryInterface(iid, ppv);
            pInst->Release();
        }
        break;
    case CLASS_SEGMENT:
        {
            CSegment *pInst = new CSegment;
            if (pInst == NULL) {
                hr = E_OUTOFMEMORY;
                break;
            }
            // The segment is initialized with a ref count of 0, so QI will set it to 1.
            hr = pInst->QueryInterface(iid, ppv);
        }
        break;
    case CLASS_SONG:
        {
            CSong *pInst = new CSong;
            if (pInst == NULL) {
                hr = E_OUTOFMEMORY;
                break;
            }

            hr = pInst->QueryInterface(iid, ppv);
            pInst->Release();
        }
        break;
    case CLASS_AUDIOPATH:
        {
            CAudioPathConfig *pInst;

            try
            {
                pInst = new CAudioPathConfig;
            }
            catch( ... )
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            if (pInst == NULL) {
                hr = E_OUTOFMEMORY;
                break;
            }

            hr = pInst->QueryInterface(iid, ppv);
            pInst->Release();
        }
        break;
    case CLASS_SEQTRACK:
        {
            CSeqTrack *pInst;

            try
            {
                pInst = new CSeqTrack;
            }
            catch( ... )
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            if (pInst == NULL) {
                hr = E_OUTOFMEMORY;
                break;
            }

            hr = pInst->QueryInterface(iid, ppv);
            pInst->Release();
        }
        break;
    case CLASS_SYSEXTRACK:
        {
            CSysExTrack *pInst;

            try
            {
                pInst = new CSysExTrack;
            }
            catch( ... )
            {
                hr = E_OUTOFMEMORY;
                break;
            }
            if (pInst == NULL) {
                hr = E_OUTOFMEMORY;
                break;
            }

            hr = pInst->QueryInterface(iid, ppv);
            pInst->Release();
        }
        break;
    case CLASS_TEMPOTRACK:
        {
            CTempoTrack *pInst;

            try
            {
                pInst = new CTempoTrack;
            }
            catch( ... )
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            if (pInst == NULL) {
                hr = E_OUTOFMEMORY;
                break;
            }

            hr = pInst->QueryInterface(iid, ppv);
            pInst->Release();
        }
        break;
    case CLASS_TIMESIGTRACK:
        {
            CTimeSigTrack *pInst;
    
            try
            {
                pInst = new CTimeSigTrack;
            }
            catch( ... )
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            if (pInst == NULL) {
                hr = E_OUTOFMEMORY;
                break;
            }

            hr = pInst->QueryInterface(iid, ppv);
            pInst->Release();
        }
        break;
    case CLASS_MARKERTRACK:
        {
            CMarkerTrack *pInst;
    
            try
            {
                pInst = new CMarkerTrack;
            }
            catch( ... )
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            if (pInst == NULL) {
                hr = E_OUTOFMEMORY;
                break;
            }
            hr = pInst->QueryInterface(iid, ppv);
            pInst->Release();
        }
        break;
    case CLASS_WAVETRACK:
        {
            CWavTrack *pInst;
    
            try
            {
                pInst = new CWavTrack;
            }
            catch( ... )
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            if (pInst == NULL) {
                hr = E_OUTOFMEMORY;
                break;
            }
            hr = pInst->QueryInterface(iid, ppv);
            pInst->Release();
        }
        break;
    case CLASS_TRIGGERTRACK:
        {
            try
            {
                hr = TrackHelpCreateInstance<CSegTriggerTrack>(pUnknownOuter, iid, ppv);
            }
            catch( ... )
            {
                hr = E_OUTOFMEMORY;
            }
        }
        break;
    case CLASS_LYRICSTRACK:
        {
            try
            {
                hr = TrackHelpCreateInstance<CLyricsTrack>(pUnknownOuter, iid, ppv);
            }
            catch( ... )
            {
                hr = E_OUTOFMEMORY;
            }
        }
        break;
    case CLASS_PARAMSTRACK:
        {
            try
            {
                hr = TrackHelpCreateInstance<CParamControlTrack>(pUnknownOuter, iid, ppv);
            }
            catch( ... )
            {
                hr = E_OUTOFMEMORY;
            }
        }
        break;
    case CLASS_SEGSTATE:
        {
            CSegState *pInst = new CSegState;
            if (pInst == NULL) {
                hr = E_OUTOFMEMORY;
                break;
            }

            hr = pInst->QueryInterface(iid, ppv);
            pInst->Release();
        }
    }
    return hr;
}

// CClassFactory::LockServer
//
HRESULT __stdcall
CClassFactory::LockServer(BOOL bLock)
{
    if (bLock) {
        InterlockedIncrement(&g_cLock);
    } else {
        InterlockedDecrement(&g_cLock);
    }

    return S_OK;
}


// Version information for our class
//
TCHAR g_szDMPerformanceFriendlyName[]    = TEXT("DirectMusicPerformance");
TCHAR g_szDMPerformanceVerIndProgID[]    = TEXT("Microsoft.DirectMusicPerformance");
TCHAR g_szDMPerformanceProgID[]          = TEXT("Microsoft.DirectMusicPerformance.1");

TCHAR g_szDMSegmentFriendlyName[]    = TEXT("DirectMusicSegment");
TCHAR g_szDMSegmentVerIndProgID[]    = TEXT("Microsoft.DirectMusicSegment");
TCHAR g_szDMSegmentProgID[]          = TEXT("Microsoft.DirectMusicSegment.1");

TCHAR g_szDMSegmentStateFriendlyName[]    = TEXT("DirectMusicSegmentState");
TCHAR g_szDMSegmentStateVerIndProgID[]    = TEXT("Microsoft.DirectMusicSegmentState");
TCHAR g_szDMSegmentStateProgID[]          = TEXT("Microsoft.DirectMusicSegmentState.1");

TCHAR g_szSongFriendlyName[]    = TEXT("DirectMusicSong");
TCHAR g_szSongVerIndProgID[]    = TEXT("Microsoft.DirectMusicSong");
TCHAR g_szSongProgID[]          = TEXT("Microsoft.DirectMusicSong.1");

TCHAR g_szAudioPathFriendlyName[]    = TEXT("DirectMusicAudioPath");
TCHAR g_szAudioPathVerIndProgID[]    = TEXT("Microsoft.DirectMusicAudioPath");
TCHAR g_szAudioPathProgID[]          = TEXT("Microsoft.DirectMusicAudioPath.1");

TCHAR g_szDMGraphFriendlyName[]    = TEXT("DirectMusicGraph");
TCHAR g_szDMGraphVerIndProgID[]    = TEXT("Microsoft.DirectMusicGraph");
TCHAR g_szDMGraphProgID[]          = TEXT("Microsoft.DirectMusicGraph.1");

TCHAR g_szDMSeqTrackFriendlyName[]    = TEXT("DirectMusicSeqTrack");
TCHAR g_szDMSeqTrackVerIndProgID[]    = TEXT("Microsoft.DirectMusicSeqTrack");
TCHAR g_szDMSeqTrackProgID[]          = TEXT("Microsoft.DirectMusicSeqTrack.1");

TCHAR g_szDMSysExTrackFriendlyName[]    = TEXT("DirectMusicSysExTrack");
TCHAR g_szDMSysExTrackVerIndProgID[]    = TEXT("Microsoft.DirectMusicSysExTrack");
TCHAR g_szDMSysExTrackProgID[]          = TEXT("Microsoft.DirectMusicSysExTrack.1");

TCHAR g_szDMTempoTrackFriendlyName[]    = TEXT("DirectMusicTempoTrack");
TCHAR g_szDMTempoTrackVerIndProgID[]    = TEXT("Microsoft.DirectMusicTempoTrack");
TCHAR g_szDMTempoTrackProgID[]          = TEXT("Microsoft.DirectMusicTempoTrack.1");

TCHAR g_szDMTimeSigTrackFriendlyName[]    = TEXT("DirectMusicTimeSigTrack");
TCHAR g_szDMTimeSigTrackVerIndProgID[]    = TEXT("Microsoft.DirectMusicTimeSigTrack");
TCHAR g_szDMTimeSigTrackProgID[]          = TEXT("Microsoft.DirectMusicTimeSigTrack.1");

TCHAR g_szMarkerTrackFriendlyName[]    = TEXT("DirectMusicMarkerTrack");
TCHAR g_szMarkerTrackVerIndProgID[]    = TEXT("Microsoft.DirectMusicMarkerTrack");
TCHAR g_szMarkerTrackProgID[]          = TEXT("Microsoft.DirectMusicMarkerTrack.1");

TCHAR g_szWaveTrackFriendlyName[]    = TEXT("DirectMusicWaveTrack");
TCHAR g_szWaveTrackVerIndProgID[]    = TEXT("Microsoft.DirectMusicWaveTrack");
TCHAR g_szWaveTrackProgID[]          = TEXT("Microsoft.DirectMusicWaveTrack.1");

TCHAR g_szSegTriggerTrackFriendlyName[]    = TEXT("DirectMusicSegTriggerTrack");
TCHAR g_szSegTriggerTrackVerIndProgID[]    = TEXT("Microsoft.DirectMusicSegTriggerTrack");
TCHAR g_szSegTriggerTrackProgID[]          = TEXT("Microsoft.DirectMusicSegTriggerTrack.1");

TCHAR g_szLyricsTrackFriendlyName[]    = TEXT("DirectMusicLyricsTrack");
TCHAR g_szLyricsTrackVerIndProgID[]    = TEXT("Microsoft.DirectMusicLyricsTrack");
TCHAR g_szLyricsTrackProgID[]          = TEXT("Microsoft.DirectMusicLyricsTrack.1");

TCHAR g_szParamControlTrackFriendlyName[]    = TEXT("DirectMusicParamControlTrack");
TCHAR g_szParamControlTrackVerIndProgID[]    = TEXT("Microsoft.DirectMusicParamControlTrack");
TCHAR g_szParamControlTrackProgID[]          = TEXT("Microsoft.DirectMusicParamControlTrack.1");

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Standard calls needed to be an inproc server

//////////////////////////////////////////////////////////////////////
// DllCanUnloadNow

STDAPI DllCanUnloadNow()
{
    if (g_cComponent || g_cLock) {
        return S_FALSE;
    }

    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// DllGetClassObject

STDAPI DllGetClassObject(const CLSID& clsid,
                         const IID& iid,
                         void** ppv)
{
    IUnknown* pIUnknown = NULL;
    DWORD dwTypeID = 0;

    if(clsid == CLSID_XDirectMusicPerformance)
    {
        dwTypeID = CLASS_PERFORMANCE;
    }
    else if(clsid == CLSID_XDirectMusicSegment) 
    {
        dwTypeID = CLASS_SEGMENT;
    }
    else if(clsid == CLSID_XDirectMusicSong) 
    {
        dwTypeID = CLASS_SONG;
    }
    else if(clsid == CLSID_XDirectMusicAudioPathConfig) 
    {
        dwTypeID = CLASS_AUDIOPATH;
    }
    else if(clsid == CLSID_XDirectMusicSeqTrack) 
    {
        dwTypeID = CLASS_SEQTRACK;
    }
    else if(clsid == CLSID_XDirectMusicGraph) 
    {
        dwTypeID = CLASS_GRAPH;
    }
    else if(clsid == CLSID_XDirectMusicSysExTrack) 
    {
        dwTypeID = CLASS_SYSEXTRACK;
    }
    else if(clsid == CLSID_XDirectMusicTempoTrack) 
    {
        dwTypeID = CLASS_TEMPOTRACK;
    }
    else if(clsid == CLSID_XDirectMusicTimeSigTrack) 
    {
        dwTypeID = CLASS_TIMESIGTRACK;
    }
    else if(clsid == CLSID_XDirectMusicMarkerTrack) 
    {
        dwTypeID = CLASS_MARKERTRACK;
    }
    else if(clsid == CLSID_XDirectMusicWaveTrack) 
    {
        dwTypeID = CLASS_WAVETRACK;
    }
    else if(clsid == CLSID_XDirectMusicSegmentTriggerTrack)
    {
        dwTypeID = CLASS_TRIGGERTRACK;
    }
    else if(clsid == CLSID_XDirectMusicLyricsTrack)
    {
        dwTypeID = CLASS_LYRICSTRACK;
    }
    else if(clsid == CLSID_XDirectMusicParamControlTrack)
    {
        dwTypeID = CLASS_PARAMSTRACK;
    }
    else if(clsid == CLSID_XDirectMusicSegmentState)
    {
        dwTypeID = CLASS_SEGSTATE;
    }
    else
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
    pIUnknown = static_cast<IUnknown*> (new CClassFactory(dwTypeID));
    if(pIUnknown) 
    {
        HRESULT hr = pIUnknown->QueryInterface(iid, ppv);
        pIUnknown->Release();
        return hr;
    }
    return E_OUTOFMEMORY;
}

//////////////////////////////////////////////////////////////////////
// DllUnregisterServer

STDAPI DllUnregisterServer()
{
    UnregisterServer(CLSID_XDirectMusicPerformance,
                     g_szDMPerformanceFriendlyName,
                     g_szDMPerformanceVerIndProgID,
                     g_szDMPerformanceProgID);

    UnregisterServer(CLSID_XDirectMusicSegment,
                     g_szDMSegmentFriendlyName,
                     g_szDMSegmentVerIndProgID,
                     g_szDMSegmentProgID);

    UnregisterServer(CLSID_XDirectMusicSegmentState,
                     g_szDMSegmentStateFriendlyName,
                     g_szDMSegmentStateVerIndProgID,
                     g_szDMSegmentStateProgID);

    UnregisterServer(CLSID_XDirectMusicSong,
                     g_szSongFriendlyName,
                     g_szSongVerIndProgID,
                     g_szSongProgID);

    UnregisterServer(CLSID_XDirectMusicAudioPathConfig,
                     g_szAudioPathFriendlyName,
                     g_szAudioPathVerIndProgID,
                     g_szAudioPathProgID);
    
    UnregisterServer(CLSID_XDirectMusicGraph,
                     g_szDMGraphFriendlyName,
                     g_szDMGraphVerIndProgID,
                     g_szDMGraphProgID);

    UnregisterServer(CLSID_XDirectMusicSeqTrack,
                     g_szDMSeqTrackFriendlyName,
                     g_szDMSeqTrackVerIndProgID,
                     g_szDMSeqTrackProgID);

    UnregisterServer(CLSID_XDirectMusicSysExTrack,
                     g_szDMSysExTrackFriendlyName,
                     g_szDMSysExTrackVerIndProgID,
                     g_szDMSysExTrackProgID);

    UnregisterServer(CLSID_XDirectMusicTempoTrack,
                     g_szDMTempoTrackFriendlyName,
                     g_szDMTempoTrackVerIndProgID,
                     g_szDMTempoTrackProgID);

    UnregisterServer(CLSID_XDirectMusicTimeSigTrack,
                     g_szDMTimeSigTrackFriendlyName,
                     g_szDMTimeSigTrackVerIndProgID,
                     g_szDMTimeSigTrackProgID);

    UnregisterServer(CLSID_XDirectMusicMarkerTrack,
                     g_szMarkerTrackFriendlyName,
                     g_szMarkerTrackVerIndProgID,
                     g_szMarkerTrackProgID);

    UnregisterServer(CLSID_XDirectMusicWaveTrack,
                     g_szWaveTrackFriendlyName,
                     g_szWaveTrackVerIndProgID,
                     g_szWaveTrackProgID);

    UnregisterServer(CLSID_XDirectMusicSegmentTriggerTrack,
                     g_szSegTriggerTrackFriendlyName,
                     g_szSegTriggerTrackVerIndProgID,
                     g_szSegTriggerTrackProgID);

    UnregisterServer(CLSID_XDirectMusicLyricsTrack,
                     g_szLyricsTrackFriendlyName,
                     g_szLyricsTrackVerIndProgID,
                     g_szLyricsTrackProgID);

    UnregisterServer(CLSID_XDirectMusicParamControlTrack,
                     g_szParamControlTrackFriendlyName,
                     g_szParamControlTrackVerIndProgID,
                     g_szParamControlTrackProgID);

    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// DllRegisterServer

STDAPI DllRegisterServer()
{
    RegisterServer(g_hModule,
                   CLSID_XDirectMusicPerformance,
                     g_szDMPerformanceFriendlyName,
                     g_szDMPerformanceVerIndProgID,
                     g_szDMPerformanceProgID);

    RegisterServer(g_hModule,
                   CLSID_XDirectMusicSegment,
                     g_szDMSegmentFriendlyName,
                     g_szDMSegmentVerIndProgID,
                     g_szDMSegmentProgID);

    RegisterServer(g_hModule,
                   CLSID_XDirectMusicSegmentState,
                     g_szDMSegmentStateFriendlyName,
                     g_szDMSegmentStateVerIndProgID,
                     g_szDMSegmentStateProgID);

    RegisterServer(g_hModule,
                   CLSID_XDirectMusicSong,
                     g_szSongFriendlyName,
                     g_szSongVerIndProgID,
                     g_szSongProgID);

    RegisterServer(g_hModule,
                   CLSID_XDirectMusicAudioPathConfig,
                     g_szAudioPathFriendlyName,
                     g_szAudioPathVerIndProgID,
                     g_szAudioPathProgID);

    RegisterServer(g_hModule,
                   CLSID_XDirectMusicGraph,
                     g_szDMGraphFriendlyName,
                     g_szDMGraphVerIndProgID,
                     g_szDMGraphProgID);

    RegisterServer(g_hModule,
                   CLSID_XDirectMusicSeqTrack,
                     g_szDMSeqTrackFriendlyName,
                     g_szDMSeqTrackVerIndProgID,
                     g_szDMSeqTrackProgID);

    RegisterServer(g_hModule,
                   CLSID_XDirectMusicSysExTrack,
                     g_szDMSysExTrackFriendlyName,
                     g_szDMSysExTrackVerIndProgID,
                     g_szDMSysExTrackProgID);

    RegisterServer(g_hModule,
                   CLSID_XDirectMusicTempoTrack,
                     g_szDMTempoTrackFriendlyName,
                     g_szDMTempoTrackVerIndProgID,
                     g_szDMTempoTrackProgID);

    RegisterServer(g_hModule,
                   CLSID_XDirectMusicTimeSigTrack,
                     g_szDMTimeSigTrackFriendlyName,
                     g_szDMTimeSigTrackVerIndProgID,
                     g_szDMTimeSigTrackProgID);
    RegisterServer(g_hModule,
                   CLSID_XDirectMusicMarkerTrack,
                     g_szMarkerTrackFriendlyName,
                     g_szMarkerTrackVerIndProgID,
                     g_szMarkerTrackProgID);
    RegisterServer(g_hModule,
                   CLSID_XDirectMusicWaveTrack,
                     g_szWaveTrackFriendlyName,
                     g_szWaveTrackVerIndProgID,
                     g_szWaveTrackProgID);
    RegisterServer(g_hModule,
                   CLSID_XDirectMusicSegmentTriggerTrack,
                     g_szSegTriggerTrackFriendlyName,
                     g_szSegTriggerTrackVerIndProgID,
                     g_szSegTriggerTrackProgID);
    RegisterServer(g_hModule,
                   CLSID_XDirectMusicLyricsTrack,
                     g_szLyricsTrackFriendlyName,
                     g_szLyricsTrackVerIndProgID,
                     g_szLyricsTrackProgID);
    RegisterServer(g_hModule,
                   CLSID_XDirectMusicParamControlTrack,
                     g_szParamControlTrackFriendlyName,
                     g_szParamControlTrackVerIndProgID,
                     g_szParamControlTrackProgID);
    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Standard Win32 DllMain

//////////////////////////////////////////////////////////////////////
// DllMain

#ifdef DBG
static char* aszReasons[] =
{
    "DLL_PROCESS_DETACH",
    "DLL_PROCESS_ATTACH",
    "DLL_THREAD_ATTACH",
    "DLL_THREAD_DETACH"
};
const DWORD nReasons = (sizeof(aszReasons) / sizeof(char*));
#endif

BOOL APIENTRY DllMain(HINSTANCE hModule,
                      DWORD dwReason,
                      void *lpReserved)
{
    static int nReferenceCount = 0;

#ifdef DBG
    if (dwReason < nReasons)
    {
        TraceI(0, "DllMain: %s\n", (LPSTR)aszReasons[dwReason]);
    }
    else
    {
        TraceI(0, "DllMain: Unknown dwReason <%u>\n", dwReason);
    }
#endif

    switch(dwReason)
    {
        case DLL_PROCESS_ATTACH:
            if (++nReferenceCount == 1)
            {
                g_fInitCS = false;
                try
                {
                    InitializeCriticalSection(&g_CritSec);
                    InitializeCriticalSection(&WaveItem::st_WaveListCritSect);
                }
                catch( ... )
                {
                    TraceI(0, "InitializeCriticalSection failed.\n");
                    return FALSE;
                }
                g_fInitCS = true;

                #ifdef DBG
                    DebugInit();
                #endif

                if (!DisableThreadLibraryCalls(hModule))
                {
                    TraceI(1, "DisableThreadLibraryCalls failed.\n");
                }

                g_hModule = hModule;
            }
            break;

        case DLL_PROCESS_DETACH:
            if (--nReferenceCount == 0)
            {
                TraceI(-1, "Unloading g_cLock %d  g_cComponent %d\n", g_cLock, g_cComponent);
                if (g_fInitCS)
                {
                    DeleteCriticalSection(&g_CritSec);
                    DeleteCriticalSection(&WaveItem::st_WaveListCritSect);
                }

                // Assert if we still have some objects hanging around
                assert(g_cComponent == 0);
                assert(g_cLock == 0);
            }
            break;
            
    }
        
    return TRUE;
}
