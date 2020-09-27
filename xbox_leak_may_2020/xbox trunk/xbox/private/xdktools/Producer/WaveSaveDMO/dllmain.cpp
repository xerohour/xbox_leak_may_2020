//
// dllmain.cpp
// 
// Copyright (c) 1997-1999 Microsoft Corporation
//

#include <objbase.h>
#include "debug.h"
#include "oledll.h"
#include "testdmo.h"
#include "dumpp.h"

DWORD g_amPlatform;

#define DefineClassFactory(x)                                               \
class x ## Factory : public IClassFactory                                   \
{                                                                           \
public:                                                                     \
	virtual STDMETHODIMP QueryInterface(const IID &iid, void **ppv);        \
	virtual STDMETHODIMP_(ULONG) AddRef();                                  \
	virtual STDMETHODIMP_(ULONG) Release();                                 \
                                                                            \
	virtual STDMETHODIMP CreateInstance(IUnknown* pUnknownOuter,            \
    	const IID& iid, void** ppv);                                        \
	virtual STDMETHODIMP LockServer(BOOL bLock);                            \
                                                                            \
	 x ## Factory() : m_cRef(1) {}                                          \
                                                                            \
	~ x ## Factory() {}                                                     \
                                                                            \
private:                                                                    \
	long m_cRef;                                                            \
};                                                                          \
STDMETHODIMP x ## Factory::QueryInterface(                                  \
    const IID &iid, void **ppv)                                             \
{                                                                           \
	if(iid == IID_IUnknown || iid == IID_IClassFactory)                     \
	{                                                                       \
		*ppv = static_cast<IClassFactory*>(this);                           \
    }                                                                       \
	else                                                                    \
    {                                                                       \
		*ppv = NULL;                                                        \
		return E_NOINTERFACE;                                               \
    }                                                                       \
                                                                            \
    reinterpret_cast<IUnknown*>(*ppv)->AddRef();                            \
                                                                            \
    return S_OK;                                                            \
}                                                                           \
STDMETHODIMP_(ULONG) x ## Factory::AddRef()                                 \
{                                                                           \
	return InterlockedIncrement(&m_cRef);                                   \
}                                                                           \
                                                                            \
STDMETHODIMP_(ULONG) x ## Factory::Release()                                \
{                                                                           \
    if(!InterlockedDecrement(&m_cRef))                                      \
	{                                                                       \
		delete this;                                                        \
		return 0;                                                           \
    }                                                                       \
                                                                            \
    return m_cRef;                                                          \
}                                                                           \
                                                                            \
STDMETHODIMP x ## Factory::LockServer(BOOL bLock)                           \
{                                                                           \
    if(bLock)                                                               \
	{                                                                       \
		InterlockedIncrement(&g_cLock);                                     \
    }                                                                       \
	else                                                                    \
	{                                                                       \
		InterlockedDecrement(&g_cLock);                                     \
    }                                                                       \
                                                                            \
    return S_OK;                                                            \
}

/*
STDMETHODIMP x ## Factory::CreateInstance(                                  \
    IUnknown* pUnknownOuter,  const IID& iid, void** ppv)                   \
{                                                                           \
    OutputDebugString("Create " #x  "\n");                                  \
	if (ppv == NULL)                                                        \
	{                                                                       \
		return E_POINTER;                                                   \
	}                                                                       \
                                                                            \
	if (pUnknownOuter)                                                      \
    {                                                                       \
		 return CLASS_E_NOAGGREGATION;                                      \
    }                                                                       \
                                                                            \
    x *p;                                                                   \
                                                                            \
    try                                                                     \
    {                                                                       \
        p = new x;                                                          \
    }                                                                       \
    catch( ... )                                                            \
    {                                                                       \
        return E_OUTOFMEMORY;                                               \
    }                                                                       \
                                                                            \
    if (p == NULL)                                                          \
	{                                                                       \
		return E_OUTOFMEMORY;                                               \
    }                                                                       \
                                                                            \
    HRESULT hr = p->QueryInterface(iid, ppv);                               \
                                                                            \
    p->Release();                                                           \
                                                                            \
    return hr;                                                              \
}                                                                           \

*/


#define DefineDMOClassFactory(x) DefineClassFactory(CDirectSound ## x ## DMO)

//////////////////////////////////////////////////////////////////////
// Globals
//

// Registry Info
//
#define DefineNames(x)                                                              \
TCHAR g_sz## x ##FriendlyName[]    = TEXT("DirectSound" #x "DMO");                  \
TCHAR g_sz## x ##VerIndProgID[]    = TEXT("Microsoft.DirectSound" #x "DMO");        \
TCHAR g_sz## x ##ProgID[]          = TEXT("Microsoft.DirectSound" #x "DMO.1");
/*
TCHAR g_sz## x ##PropFriendlyName[] = TEXT("DirectSound" #x "DMOPropPage");         \
TCHAR g_sz## x ##PropVerIndProgID[] = TEXT("Microsoft.DirectSound" #x "DMOPropPage");   \
TCHAR g_sz## x ##PropProgID[]      = TEXT("Microsoft.DirectSound" #x "DMOPropPage.1");
*/

DefineNames(FileOutput)

// Dll's hModule
HMODULE g_hModule = NULL;

// Count of active components and class factory server locks
long g_cComponent = 0;
long g_cLock = 0;

DefineDMOClassFactory(FileOutput)
STDMETHODIMP CDirectSoundFileOutputDMOFactory::CreateInstance(
    IUnknown* pUnknownOuter,  const IID& iid, void** ppv)
{
	if (ppv == NULL)
	{
		return E_POINTER;
	}

	if (pUnknownOuter)
    {
		 return CLASS_E_NOAGGREGATION;
    }

    CDirectSoundDumpDMO *p;

    try
    {
        p = new CDirectSoundDumpDMO;
    }
    catch( ... )
    {
        return E_OUTOFMEMORY;
    }

    if (p == NULL)
	{
		return E_OUTOFMEMORY;
    }

    HRESULT hr = p->QueryInterface(iid, ppv);

    p->Release();

    return hr;
}

/*
DefineClassFactory(CDumpProperties)
STDMETHODIMP CDumpPropertiesFactory::CreateInstance(
    IUnknown* pUnknownOuter,  const IID& iid, void** ppv)
{
	if (ppv == NULL)
	{
		return E_POINTER;
	}

	if (pUnknownOuter)
    {
		 return CLASS_E_NOAGGREGATION;
    }

    CDumpPropertyPage *p;

    try
    {
        p = new CDumpPropertyPage;
    }
    catch( ... )
    {
        return E_OUTOFMEMORY;
    }

    if (p == NULL)
	{
		return E_OUTOFMEMORY;
    }

    HRESULT hr = p->QueryInterface(iid, ppv);

    p->Release();

    return hr;
}
*/

//////////////////////////////////////////////////////////////////////
// Standard calls needed to be an inproc server

//////////////////////////////////////////////////////////////////////
// DllCanUnloadNow

STDAPI DllCanUnloadNow()
{
    if (g_cComponent || g_cLock) 
	{
		return S_FALSE;
    }

    return S_OK;
}

// Hack to make these macros continue to work:
#define GUID_DSFX_STANDARD_FileOutput       GUID_DSFX_STANDARD_DUMP

#define GetClassObjectCase(x,t) \
    if (clsid == x) { \
        p = static_cast<IUnknown*> ((IClassFactory*) (new t)); \
    } else 

#define GetClassObjectCaseEnd \
    { return CLASS_E_CLASSNOTAVAILABLE; }

#define GetClassObjectCaseFX(x) \
    GetClassObjectCase(GUID_DSFX_STANDARD_ ## x, CDirectSound ## x ## DMOFactory)

//////////////////////////////////////////////////////////////////////
// DllGetClassObject

STDAPI DllGetClassObject(const CLSID& clsid,
						 const IID& iid,
						 void** ppv)
{
	if (ppv == NULL)
	{
		return E_POINTER;
	}

	IUnknown* p = NULL;

    GetClassObjectCaseFX(FileOutput)

	//GetClassObjectCase(CLSID_DirectSoundPropDump,  CDumpPropertiesFactory)

    GetClassObjectCaseEnd

	if(!p) 
	{
		return E_OUTOFMEMORY;
	}

    HRESULT hr = p->QueryInterface(iid, ppv);
    p->Release();

    return hr;
}

#define DoUnregister(x)                                                 \
         UnregisterServer(GUID_DSFX_STANDARD_ ## x,                     \
                          g_sz ## x ## FriendlyName,                    \
                          g_sz ## x ## VerIndProgID,                    \
                          g_sz ## x ## ProgID)

#define DoRegister(x)                                                   \
         RegisterServer(g_hModule,                                      \
                        GUID_DSFX_STANDARD_ ## x,                       \
                        g_sz ## x ## FriendlyName,                      \
                        g_sz ## x ## VerIndProgID,                      \
                        g_sz ## x ## ProgID)

#define DoDMORegister(x)                                                \
         DMORegister(L#x,                                              \
         GUID_DSFX_STANDARD_ ## x,                                      \
         DMOCATEGORY_AUDIO_EFFECT,                                      \
         0, 1, &mt, 1, &mt)

#define DoDMOUnregister(x)                                              \
         DMOUnregister(GUID_DSFX_STANDARD_ ## x,                        \
         DMOCATEGORY_AUDIO_EFFECT)

#define Unregister(x)                                                   \
    if (SUCCEEDED(hr)) hr = DoDMOUnregister(x);                         \
    if (SUCCEEDED(hr)) hr = DoUnregister(x); 

#define Register(x)                                                     \
    if (SUCCEEDED(hr)) hr = DoRegister(x);                              \
    if (SUCCEEDED(hr)) hr = DoDMORegister(x);

/*
#define DoPropRegister(x)                                               \
    if (SUCCEEDED(hr)) hr = RegisterServer(                             \
        g_hModule,                                                      \
        CLSID_DirectSoundProp ## x,                                     \
        g_sz ## x ## PropFriendlyName,                                  \
        g_sz ## x ## PropVerIndProgID,                                  \
        g_sz ## x ## PropProgID);

#define DoPropUnregister(x)                                             \
    if (SUCCEEDED(hr)) hr = UnregisterServer(                           \
        CLSID_DirectSoundProp ## x,                                     \
        g_sz ## x ## PropFriendlyName,                                  \
        g_sz ## x ## PropVerIndProgID,                                  \
        g_sz ## x ## PropProgID);
*/

//////////////////////////////////////////////////////////////////////
// DllUnregisterServer

STDAPI DllUnregisterServer()
{
    HRESULT hr = S_OK;

    Unregister(FileOutput);
    //DoPropUnregister(Dump)

    return hr;
}

//////////////////////////////////////////////////////////////////////
// DllRegisterServer

STDAPI DllRegisterServer()
{
    HRESULT hr = S_OK;

    DMO_PARTIAL_MEDIATYPE mt;
    mt.type = MEDIATYPE_Audio;
    mt.subtype = MEDIASUBTYPE_PCM;

    Register(FileOutput);
    //DoPropRegister(Dump);

    return hr;
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
    if(dwReason < nReasons)
    {
		Trace(DM_DEBUG_STATUS, "DllMain: %s\n", (LPSTR)aszReasons[dwReason]);
    }
    else
    {
		Trace(DM_DEBUG_STATUS, "DllMain: Unknown dwReason <%u>\n", dwReason);
    }
#endif

    switch(dwReason)
    {
		case DLL_PROCESS_ATTACH:
		    if(++nReferenceCount == 1)
			{
			#ifdef DBG
				DebugInit();
			#endif

				//_Module.Init(NULL, hModule, NULL);
				if(!DisableThreadLibraryCalls(hModule))
				{
					Trace(DM_DEBUG_STATUS, "DisableThreadLibraryCalls failed.\n");
				}

				g_hModule = hModule;

                g_amPlatform = VER_PLATFORM_WIN32_WINDOWS; // win95 assumed in case GetVersionEx fails

                OSVERSIONINFO osInfo;
                osInfo.dwOSVersionInfoSize = sizeof(osInfo);
                if (GetVersionEx(&osInfo))
                {
                    g_amPlatform = osInfo.dwPlatformId;
                }
            }
			break;

		case DLL_PROCESS_DETACH:
		    if(--nReferenceCount == 0)
			{
				Trace(DM_DEBUG_STATUS, "Unloading\n");
			}
			//_Module.Term();
			break;
    }
	
    return TRUE;
}

