//      Copyright (c) 1996-2001 Microsoft Corporation

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
// dmsynth.cpp
//
// Dll entry points and IDirectMusicSynthFactory implementation
//
#include <objbase.h>
#include <mmsystem.h>
#include <dsoundp.h>
#include "debug.h"

#include "oledll.h"

#include "dmusicc.h"
#include "dmusics.h"
#include "umsynth.h"
#include "misc.h" 
#include <regstr.h>
#include "synth.h"

#include "validate.h"


// Globals
//


// Version information for our class
//
TCHAR g_szMSSynthFriendlyName[]    = TEXT("Microsoft Xbox Synthesizer");

TCHAR g_szSynthFriendlyName[]    = TEXT("XboxSynth");
TCHAR g_szSynthVerIndProgID[]    = TEXT("Microsoft.XboxSynth");
TCHAR g_szSynthProgID[]          = TEXT("Microsoft.XboxSynth.1");

// Dll's hModule
//
HMODULE g_hModule = NULL; 

// Count of active components and class factory server locks
//
long g_cComponent = 0;
long g_cLock = 0;


static char const g_szDoEmulation[] = "DoEmulation";

// CDirectMusicSynthFactory::QueryInterface
//
HRESULT __stdcall
CDirectMusicSynthFactory::QueryInterface(const IID &iid,
                                    void **ppv)
{
    V_INAME(IDirectMusicSynthFactory::QueryInterface);
    V_REFGUID(iid);
    V_PTRPTR_WRITE(ppv);

    if (iid == IID_IUnknown || iid == IID_IClassFactory) {
        *ppv = static_cast<IClassFactory*>(this);
    } else {
        *ppv = NULL;
        return E_NOINTERFACE;
    }

    reinterpret_cast<IUnknown*>(*ppv)->AddRef();
    return S_OK;
}

CDirectMusicSynthFactory::CDirectMusicSynthFactory()

{
	m_cRef = 1;
	InterlockedIncrement(&g_cLock);
}

CDirectMusicSynthFactory::~CDirectMusicSynthFactory()

{
	InterlockedDecrement(&g_cLock);
}

// CDirectMusicSynthFactory::AddRef
//
ULONG __stdcall
CDirectMusicSynthFactory::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

// CDirectMusicSynthFactory::Release
//
ULONG __stdcall
CDirectMusicSynthFactory::Release()
{
    if (!InterlockedDecrement(&m_cRef)) {
        delete this;
        return 0;
    }

    return m_cRef;
}

// CDirectMusicSynthFactory::CreateInstance
//
//
HRESULT __stdcall
CDirectMusicSynthFactory::CreateInstance(IUnknown* pUnknownOuter,
                                    const IID& iid,
                                    void** ppv)
{
//    OSVERSIONINFO osvi;
    HRESULT hr;

//    DebugBreak();
    
    if (pUnknownOuter) {
         return CLASS_E_NOAGGREGATION;
    }

    CUserModeSynth *pDM;
    
    try
    {
        pDM = new CUserModeSynth;
    }
    catch( ... )
    {
        return E_OUTOFMEMORY;
    }

    if (pDM == NULL) {
        return E_OUTOFMEMORY;
    }

    // Do initialiazation
    //
    hr = pDM->Init();
    if (!SUCCEEDED(hr)) {
        delete pDM;
        return hr;
    }

    hr = pDM->QueryInterface(iid, ppv);

    if( FAILED(hr) )
    {
        delete pDM;
    }
    
    return hr;
}

// CDirectMusicSynthFactory::LockServer
//
HRESULT __stdcall
CDirectMusicSynthFactory::LockServer(BOOL bLock)
{
    if (bLock) {
        InterlockedIncrement(&g_cLock);
    } else {
        InterlockedDecrement(&g_cLock);
    }

    return S_OK;
}



// Standard calls needed to be an inproc server
//
STDAPI  DllCanUnloadNow()
{
    if (g_cComponent || g_cLock) {
        return S_FALSE;
    }

    return S_OK;
}

STDAPI DllGetClassObject(const CLSID& clsid,
                         const IID& iid,
                         void** ppv)
{
        IUnknown* pIUnknown = NULL;


        if(clsid == CLSID_XboxSynth)
        {

                pIUnknown = static_cast<IUnknown*> (new CDirectMusicSynthFactory);
                if(!pIUnknown) 
                {
                        return E_OUTOFMEMORY;
                }
        }
        else
        {
			return CLASS_E_CLASSNOTAVAILABLE;
		}

        HRESULT hr = pIUnknown->QueryInterface(iid, ppv);
        pIUnknown->Release();

    return hr;
}

const TCHAR cszSynthRegRoot[] = TEXT(REGSTR_PATH_SOFTWARESYNTHS) TEXT("\\");
const TCHAR cszDescriptionKey[] = TEXT("Description");
const int CLSID_STRING_SIZE = 39;
HRESULT CLSIDToStr(const CLSID &clsid, TCHAR *szStr, int cbStr);

HRESULT RegisterSynth(REFGUID guid,
                      const TCHAR szDescription[])
{
    HKEY hk;
    TCHAR szCLSID[CLSID_STRING_SIZE];
    TCHAR szRegKey[256];
    
    HRESULT hr = CLSIDToStr(guid, szCLSID, sizeof(szCLSID));
    if (!SUCCEEDED(hr))
    {
        return hr;
    }

    lstrcpy(szRegKey, cszSynthRegRoot);
    lstrcat(szRegKey, szCLSID);

    if (RegCreateKey(HKEY_LOCAL_MACHINE,
                     szRegKey,
                     &hk))
    {
        return E_FAIL;
    }

    hr = S_OK;

    if (RegSetValueEx(hk,
                  cszDescriptionKey,
                  0L,
                  REG_SZ,
                  (CONST BYTE*)szDescription,
                  lstrlen(szDescription) + 1))
    {
        hr = E_FAIL;
    }

    RegCloseKey(hk);
    return hr;
}

STDAPI DllUnregisterServer()
{
    UnregisterServer(CLSID_XboxSynth,
                     g_szSynthFriendlyName,
                     g_szSynthVerIndProgID,
                     g_szSynthProgID);


    return S_OK;
}

STDAPI DllRegisterServer()
{
    RegisterServer(g_hModule,
                   CLSID_XboxSynth,
                   g_szSynthFriendlyName,
                   g_szSynthVerIndProgID,
                   g_szSynthProgID);


    RegisterSynth(CLSID_XboxSynth, g_szMSSynthFriendlyName);

    return S_OK;
}

extern void DebugInit();

// Standard Win32 DllMain
//

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
        Trace(0, "DllMain: %s\n", (LPSTR)aszReasons[dwReason]);
    }
    else
    {
        Trace(0, "DllMain: Unknown dwReason <%u>\n", dwReason);
    }
#endif
    if (dwReason == DLL_PROCESS_ATTACH) {
        if (++nReferenceCount == 1)
		{
            DisableThreadLibraryCalls(hModule);
            g_hModule = hModule;
#ifdef DBG
			DebugInit();
#endif
#ifdef DBG
//>>>>>>>>> remove these when done 
/*
			_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG );
			int iFlag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
			_CrtSetDbgFlag( iFlag | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF );
*/
#endif 
            if (!CControlLogic::InitCriticalSection())
            {
                TraceI(0, "Failed to initialize global critical section -- failing init\n");
                return FALSE;
            }            
		}
    }
	else if (dwReason == DLL_PROCESS_DETACH) 
	{
		if (--nReferenceCount == 0)
		{
            TraceI(-1, "Unloading g_cLock %d  g_cComponent %d\n", g_cLock, g_cComponent);
            // Assert if we still have some objects hanging around
            assert(g_cComponent == 0);
            assert(g_cLock == 0);
		}

#ifdef DBG
//>>>>>>>>> remove these when done 
/*
		if ( !_CrtCheckMemory() )
		    ::MessageBox(NULL,"Synth Heap Corupted","ERROR",MB_OK);

        if ( _CrtDumpMemoryLeaks() )
		    ::MessageBox(NULL,"Memory Leaks Detected","ERROR",MB_OK);
*/
#endif 
        CControlLogic::KillCriticalSection();
	}
    return TRUE;
}



