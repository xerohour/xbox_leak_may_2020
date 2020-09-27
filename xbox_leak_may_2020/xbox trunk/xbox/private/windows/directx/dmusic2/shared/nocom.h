#ifndef __NOCOM_H__
#define __NOCOM_H__

#ifdef XBOX

__inline HRESULT
CreateInstance(LPCWSTR pszModuleName, LPCSTR pszFunctionName, REFIID iid, LPVOID *ppvInterface)

{
    typedef HRESULT (STDAPICALLTYPE *LPFNCREATEINSTANCE)(LPUNKNOWN *ppObject, LPUNKNOWN pUnknownOuter);

    LPUNKNOWN pUnknown;
    HINSTANCE hModule;
    LPFNCREATEINSTANCE pfnCreateInstance;
    HRESULT hr;

    Trace(0,"ERROR: No CreateInstance yet!\n");
    return E_FAIL;
    // Need to rewrite this, but we currently aren't calling this at all anyway...
/*    if(!(hModule = LoadLibraryW(pszModuleName)))
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    if(!(pfnCreateInstance = (LPFNCREATEINSTANCE)GetProcAddress(hModule, pszFunctionName)))
    {
        return CLASS_E_CLASSNOTAVAILABLE;
    }

    if(FAILED(hr = pfnCreateInstance(&pUnknown, NULL)))
    {
        return hr;
    }

    hr = pUnknown->QueryInterface(iid, ppvInterface);

    pUnknown->Release();

    return hr;*/
}

// HRESULT
// CreateInstance(REFCLSID clsid, REFIID iid, LPVOID *ppvInterface);

#endif // XBOX

#endif // __NOCOM_H__
