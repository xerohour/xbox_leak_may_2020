#pragma once


#ifdef __cplusplus

template <class Interface>
HRESULT
VsLoaderCoCreateInstance(
    REFCLSID rclsid, 
    LPUNKNOWN pUnkOuter,
    DWORD dwClsContext, 
    REFIID riid, 
    Interface** ppInterface
    )
{

    HRESULT hr = NOERROR;
    IUnknown* punk = NULL;

    if (FAILED (hr = VsLoaderCoCreateInstanceUnknown(
        rclsid, 
        pUnkOuter,
        dwClsContext, 
        &punk
        )))
    {
        goto Error;
    }

    if (FAILED (hr = punk->QueryInterface (riid, (LPVOID*)ppInterface)))
    {
        goto Error;
    }

Error:

    if (punk)
    {
	    punk->Release();
    }

    return hr;
}

//
// To match ATL::CComPtr<T>::CoCreateInstance
//

template <class Interface>
HRESULT
VsLoaderCoCreateInstance(
    REFCLSID rclsid,
    Interface** ppInterface,
    LPUNKNOWN pUnkOuter = NULL, 
    DWORD dwClsContext = CLSCTX_ALL
    )
{
#ifdef DEBUG
    //
    // Make sure ppInterface is an IUnknown
    //
    static_cast <IUnknown*> (*ppInterface);
#endif

    return VsLoaderCoCreateInstance<Interface> (
        rclsid, 
        pUnkOuter, 
        dwClsContext, 
        __uuidof (Interface), 
        ppInterface
        );
}

//
// To use on an ATL::CComPtr<T>
//
template <class Interface>
HRESULT
VsLoaderCoCreateInstance(
    REFCLSID rclsid,
    CComPtr<Interface>** ppInterface,
    LPUNKNOWN pUnkOuter = NULL, 
    DWORD dwClsContext = CLSCTX_ALL
    )
{
    return VsLoaderCoCreateInstance<Interface> (
        rclsid,
        pUnkOuter,
        dwClsContext,
        __uuidof (Interface),
        &((*ppInterface)->p)
        );
}

#endif // _cplusplus

#ifdef __cplusplus
#define VSCOCO_CLINKAGE	extern "C"
#else
#define VSCOCO_CLINKAGE
#endif

//
// Always returns an IUnknown*
//
VSCOCO_CLINKAGE HRESULT
_cdecl
VsLoaderCoCreateInstanceUnknown(
    REFCLSID rclsid, 
    LPUNKNOWN pUnkOuter,
    DWORD dwClsContext, 
    IUnknown** ppunk
    );

VSCOCO_CLINKAGE void
_cdecl
VsLoaderSetRegistryRootA(
    LPCSTR sz
    );

VSCOCO_CLINKAGE void
_cdecl
VsLoaderSetRegistryRootW(
    LPCWSTR sz
    );

#ifdef _UNICODE
#define VsLoaderSetRegistryRoot VsLoaderSetRegistryRootW
#else // UNICODE
#define VsLoaderSetRegistryRoot VsLoaderSetRegistryRootA
#endif

