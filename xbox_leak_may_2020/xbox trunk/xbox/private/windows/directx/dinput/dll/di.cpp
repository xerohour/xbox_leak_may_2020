/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    di.cpp

Abstract:

    Implementation of IDirectInput8 for XBOX

Environment:

	XAPI

Revision History:

    05-10-00 : created

--*/

#define DEBUG_MODULE_NAME  _MODULE_DIRECTINPUT_
#define DEBUG_TRACE_NAME "DI"
#include "dinputi.h"


/****************************************************************************
****	c'tor, d'tor, and initialization
****
***/
CDirectInput8::CDirectInput8() : 
	m_uRefCount(0), m_pOuterUnknown(NULL),
	m_pAddDeviceCallback(NULL), m_pAddDeviceContext(NULL),
    m_pRemoveDeviceCallback(NULL), m_pRemoveDeviceContext(NULL)
{
}

CDirectInput8::~CDirectInput8()
{
}

void CDirectInput8::InternalInit(LPUNKNOWN pOuterUnknown)
{
	m_pOuterUnknown = pOuterUnknown;
    m_ImplInnerUnk.Init(this);
}

/****************************************************************************
****	CAggregable Methods
****
***/

HRESULT
CDirectInput8::ImplQI(
	REFIID riid,
	LPVOID *ppvObj
	)
/*++
	Routine Descirption:
		The real implementation of QueryInterface for CDirectInput8.
		It is not implemented directly, so that we can support
		aggregation.

	Parameters:
		riid	- interface requested
		ppvObj	- buffer to receive interface pointer.
	
	Return Value:
		One of the following:
			S_OK
			E_NOINTERFACE
--*/
{
	*ppvObj = NULL;
	if(IID_IDirectInput8 == riid || IID_IUnknown == riid)
	{
		*ppvObj=dynamic_cast<IDirectInput8 *>(this); //Make sure that they get the IDirectInput8 VTABLE
	}
	if(NULL == *ppvObj)
	{
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

ULONG
CDirectInput8::ImplAddRef()
/*++
	Routine Descirption:
		The real implementation of AddRef for CDirectInput8.
		It is not implemented directly, so that we can support
		aggregation.

	Parameters:
		None
	
	Return Value:
		The new reference count.
--*/
{
	return InterlockedIncrement((PLONG)&m_uRefCount);
}

ULONG
CDirectInput8::ImplRelease()
/*++
	Routine Descirption:
		The real implementation of Release for CDirectInput8.
		It is not implemented directly, so that we can support
		aggregation.

	Parameters:
		None
	
	Return Value:
		The new reference count.
	Note:
		The temporary variable "refCount" may seem silly,
		but it ensures that you do not attempt to
		access m_uRefCount after killing yourself.
--*/
{
	ULONG refCount = InterlockedDecrement((PLONG)&m_uRefCount);
	if(0==refCount)
	{
		delete this;
	}
	return refCount;
}

/****************************************************************************
****	IUnknown Methods
****
***/

STDMETHODIMP
CDirectInput8::QueryInterface(
	REFIID riid,
	LPVOID * ppvObj
	)
/*++
	Routine Descirption:
		PUBLIC version QueryInterface for CDirectInput8.
		 When aggregated, it delegates to the outer unknown.
		 Otherwise it calls ImpQI.

	Parameters:
		riid	- interface requested
		ppvObj	- buffer to receive interface pointer.
	
	Return Value:
		One of the following:
			S_OK
			E_NOINTERFACE
--*/
{
	if(m_pOuterUnknown)
	{
		return m_pOuterUnknown->QueryInterface(riid, ppvObj);
	}
	return ImplQI(riid, ppvObj);
}

STDMETHODIMP_(ULONG)
CDirectInput8::AddRef()
/*++
	Routine Descirption:
		PUBLIC version of AddRef for CDirectInput8.
		 When aggregated, it delegates to the outer unknown.
		 Otherwise it calls ImpAddRef.

	Parameters:
	
	Return Value:
		New Reference Count
--*/
{
	if(m_pOuterUnknown)
	{
		return m_pOuterUnknown->AddRef();
	}
	return ImplAddRef();
}

STDMETHODIMP_(ULONG)
CDirectInput8::Release()
/*++
	Routine Descirption:
		PUBLIC version of Release for CDirectInput8.
		 When aggregated, it delegates to the outer unknown.
		 Otherwise it calls ImpRelease.

	Parameters:
	
	Return Value:
		New Reference Count
--*/
{
	if(m_pOuterUnknown)
	{
		return m_pOuterUnknown->Release();
	}
	return ImplRelease();
}
	
/****************************************************************************
****	IDirectInput8 Methods
****
***/

STDMETHODIMP
CDirectInput8::CreateDevice(
        REFGUID guidInstance,
        LPDIRECTINPUTDEVICE8W *ppDirectInputDevice,
        LPUNKNOWN pUnkOuter
        )
/*++
	Routine Descirption:
		Delegates to CreateDeviceEx. Very similar to
        aggregation issues as with DirectInputCreate.
--*/
{
    if(pUnkOuter)
    {
       return CreateDeviceEx(
            guidInstance,
            IID_IDirectInputDevice8,
            reinterpret_cast<LPVOID *>(ppDirectInputDevice),
            pUnkOuter
            );
    }

   return CreateDeviceEx(
        guidInstance,
        IID_IUnknown,
        reinterpret_cast<LPVOID *>(ppDirectInputDevice),
        NULL
        );
}

STDMETHODIMP
CDirectInput8::EnumDevices(
  DWORD dwDevType,              
  LPDIENUMDEVICESCALLBACKW lpCallback,  
  LPVOID pvRef,                 
  DWORD dwFlags
  )
/*++
	Routine Descirption:
        XBOX Implementation is different in a couple of major ways:
        
        1) Enum devices has been overloaded to provide continuous hot-plug notification.

        2) New flags have been defined to aid in this new functionality:
            DIEDFL_ENUMANDNOTIFYINSERT - Call EnumDevices with this flag to begin enumeration and continue to get
                                      notification to the same callback when a device is inserted.
            DIEDFL_NOTIFYREMOVE - Call EnumDevices with this flag to setup a callback for device removal notifications.
            DIEDFL_HALTNOTIFYINSERT - Call EnumDevices with this flag to stop getting device insertion notifications.
            DIEDFL_HALTNOTIFYREMOVE - Call EnumDevices with this flag to stop getting device removal notifications.

        3) Removal notification occur only when insertion notifications are on.

        4) None of the previous flags apply.

        5) The flags cannot be or'd.

        6) The return value of the callback is now ignored.

        7) The device type is ignored.

        8) One callback can be registered for device insertion notification,
           and one callback can be registered for device removal notification.
           Removal should be registered first and unregistered last to avoid
           missing removal notifications.
        
--*/
{
	CHECK_PARAMETER_NOT_NULL(CDirectInput8::EnumDevices, lpCallback);

    DWORD dwError = DI_OK;
    switch(dwFlags)
    {
        case DIEDFL_ENUMANDNOTIFYINSERT:
            if(m_pAddDeviceCallback)
            {
                dwError = DIERR_CALLBACKEXISTS;
            } else
            {
                m_pAddDeviceCallback = lpCallback;
                m_pAddDeviceContext = pvRef;
                dwError = InputAdviseDeviceChanges( (PFNINPUTDEVICECHANGEPROC)InputDeviceChangeProc, (PVOID)this);
            }
            break;
        case DIEDFL_NOTIFYREMOVE:
            if(m_pRemoveDeviceCallback)
            {
                dwError = DIERR_CALLBACKEXISTS;
            } else
            {
                m_pRemoveDeviceCallback = lpCallback;
                m_pRemoveDeviceContext = pvRef;
            }
            break;
        case DIEDFL_HALTNOTIFYINSERT:
            if(!m_pAddDeviceCallback)
            {
                dwError = DIERR_NOCALLBACK;
            } else
            {
                m_pAddDeviceCallback = NULL;
                m_pAddDeviceContext = NULL;
                InputUnadviseDeviceChanges();
            }
            break;
        case DIEDFL_HALTNOTIFYREMOVE:
            if(!m_pRemoveDeviceCallback)
            {
                dwError = DIERR_NOCALLBACK;
            } else
            {
                m_pRemoveDeviceCallback = NULL;
                m_pRemoveDeviceContext = NULL;
            }       
            break;
        default:
            TRACE_ERROR(("CDirectInput8::EnumDevices was called with an illegal value for dwFlags.\n"));
			dwError = DIERR_INVALIDPARAM;
            DEBUG_BREAK();
    };
    return dwError;
}

STDMETHODIMP CDirectInput8::GetDeviceStatus(REFGUID guidInstance)
{
    CHECK_PARAMETER_VALID_INSTANCE_GUID(CDirectInput8::GetDeviceStatus, guidInstance);
	DWORD dwError;
    INPUT_DEVICE_INFORMATION	DeviceInformation;
    DeviceInformation.dwSize = sizeof(INPUT_DEVICE_INFORMATION);
	dwError = InputQueryDeviceInformation(DEVICEIDFROMGUID(guidInstance), &DeviceInformation);
    if(dwError == ERROR_DEVICE_NOT_CONNECTED)
    {
        return DI_NOTATTACHED;
    }
    return HRESULT_FROM_WIN32(dwError);
}

/*** None of these apply to XBOX ***/
STDMETHODIMP CDirectInput8::RunControlPanel(HWND,DWORD){return E_NOTIMPL;}
STDMETHODIMP CDirectInput8::Initialize(HINSTANCE,DWORD){return E_NOTIMPL;}
STDMETHODIMP CDirectInput8::FindDevice(REFGUID,LPCWSTR,LPGUID){return E_NOTIMPL;}
STDMETHODIMP CDirectInput8::EnumDevicesBySemantics(LPCWSTR,LPDIACTIONFORMATW,LPDIENUMDEVICESBYSEMANTICSCBW, LPVOID, DWORD){return E_NOTIMPL;}
STDMETHODIMP CDirectInput8::ConfigureDevices(LPDICONFIGUREDEVICESCALLBACK, LPDICONFIGUREDEVICESPARAMSW, DWORD, LPVOID){return E_NOTIMPL;}


/****************************************************************************
****	IDirectInput8 Methods
****
***/

STDMETHODIMP
CDirectInput8::CreateDeviceEx(
    REFGUID guidInstance,
    REFIID riid,
    LPVOID *ppvOut,
    LPUNKNOWN punkOuter
    )
/*++
	Routine Descirption:
        Factory method to create DirectInputDevice

    Parameters:
        guidInstance- Instance GUID obtained from EnumDevices.
        riid        - Interface ID to create
        ppvOut      - receives interface pointer
        punkOuter   - outer ("controlling") unknown
--*/
{	
    CHECK_PARAMETER_VALID_INSTANCE_GUID(CDirectInput8::CreateDeviceEx,guidInstance);
	CDirectInputDevice8 *pObj;
    
    //
    //  NULL out our return value here, because
    //  not all failure paths would otherwise null
    //  it.
    //
    *ppvOut = NULL;

    //
    //  Make sure that they are asking for
    //  IUnknown if they want to aggregate.
    //
    if( NULL!=punkOuter && riid!=IID_IUnknown)
    {
        return DIERR_INVALIDPARAM;
    }

    //
    //  Create a DirectInput8 object
    //
    pObj = new CDirectInputDevice8();
    if(NULL==pObj) return DIERR_OUTOFMEMORY;
    
    //
    //  Call the init function (which tells
    //  it about aggregation.)
    //
    if(pObj->InternalInit(punkOuter, DEVICEIDFROMGUID(guidInstance)))
    {
        //
        //  If we need to aggregate, we look inside the object
        //  (we are a friend) and pull out the special "inner unknown"
        //  implementation.
        //
        if(NULL!=punkOuter)
        {
            pObj->ImplAddRef();
            *ppvOut = &pObj->m_ImplInnerUnk;
            return S_OK;
        }

        //
        // If we are not aggregated,
        // than we just call QI
        // as usual.
        //
        HRESULT hr = pObj->QueryInterface(riid, ppvOut);
        if(FAILED(hr))
        {
            delete pObj;
        }
        return hr;
    }
    delete pObj;
    return DIERR_DEVICENOTREG;
}




/****************************************************************************
****	Factory Methods
****
***/
#if 0
HRESULT WINAPI
DirectInputCreate(
	HINSTANCE hinst,
	DWORD dwVersion,
	LPDIRECTINPUT* ppDirectInput,
	LPUNKNOWN punkOuter
	)
/*++
	Routine Descirption:
		DirectInputCreate pretty much delegates to DirectInputCreateEx,
        but the signiture is not totally on the up and up with COM.
        When aggregating *ppDirectInput really returns an IUnknown.
        So we delegate slightly differently depending punkOuter.
--*/
{
    if(punkOuter)
    {
        return DirectInputCreateEx(
            hinst,
            dwVersion,
            IID_IUnknown,
            reinterpret_cast<LPVOID*>(ppDirectInput),
            punkOuter
            );
    }

    return DirectInputCreateEx(
            hinst,
            dwVersion,
            IID_IDirectInput8,
            reinterpret_cast<LPVOID*>(ppDirectInput),
            NULL
            );
}
#endif

HRESULT WINAPI
//DirectInputCreateEx
DirectInput8Create(
	HINSTANCE /*hinst*/,
	DWORD /*dwVersion*/,
	REFIID riid,
	LPVOID *ppvOut,
	LPUNKNOWN punkOuter
	)
/*++
	Routine Descirption:
        Factory method to create DirectInput

    Parameters:
        hinst       - Ignored.
        dwVersion   - Ignored.
        riid        - Interface ID to create
        ppvOut      - receives interface pointer
        punkOuter   - outer ("controlling") unknown
--*/
{
    CDirectInput8 *pObj;
    
    //
    //  NULL out our return value here, because
    //  not all failure paths would otherwise null
    //  it.
    //
    *ppvOut = NULL;

    //
    //  Make sure that they are asking for
    //  IUnknown if they want to aggregate.
    //
    if( NULL!=punkOuter && riid!=IID_IUnknown)
    {
        return DIERR_INVALIDPARAM;
    }

    //
    //  Create a DirectInput8 object
    //
    pObj = new CDirectInput8();
    if(NULL==pObj) return DIERR_OUTOFMEMORY;
    
    //
    //  Call the init function (which tells
    //  it about aggregation.)
    //
    pObj->InternalInit(punkOuter);

    //
    //  If we need to aggregate, we look inside the object
    //  (we are a friend) and pull out the special "inner unknown"
    //  implementation.
    //
    if(NULL!=punkOuter)
    {
        pObj->ImplAddRef();
        *ppvOut = &pObj->m_ImplInnerUnk;
        return S_OK;
    }

    //
    // If we are not aggregated,
    // than we just call QI
    // as usual.
    //
    HRESULT hr = pObj->QueryInterface(riid, ppvOut);
    if(FAILED(hr))
    {
        delete pObj;
    }
    return hr;
}


/****************************************************************************
****	Private Methods
****
***/
void CALLBACK
CDirectInput8::InputDeviceChangeProc(
	PCINPUT_DEVICE_INFORMATION DeviceInformation,
	BOOL fAdd,
	BOOL fEnum,
	CDirectInput8 *pThis
	)
{
	DIDEVICEINSTANCE diDeviceInstance;

	diDeviceInstance.dwSize = sizeof(DIDEVICEINSTANCE);
	diDeviceInstance.dwDevType = MAKE_DIDEVICE_TYPE(DeviceInformation->bDeviceType, DeviceInformation->bDeviceSubtype);
	diDeviceInstance.guidFFDriver = GUID_NULL;
	INITINSTANCEGUID(diDeviceInstance.guidInstance, DeviceInformation->bDeviceID);
	diDeviceInstance.guidProduct = GUID_NULL;
	diDeviceInstance.tszInstanceName[0] = NULL;
	diDeviceInstance.tszProductName[0] = NULL;
	diDeviceInstance.wUsage = 0;
	diDeviceInstance.wUsagePage = 0;
	
	if(fAdd && pThis->m_pAddDeviceCallback)
    {
	    pThis->m_pAddDeviceCallback(&diDeviceInstance, pThis->m_pAddDeviceContext);
    } else if(!fAdd && pThis->m_pRemoveDeviceCallback)
    {
	    pThis->m_pRemoveDeviceCallback(&diDeviceInstance, pThis->m_pRemoveDeviceContext);
    }
}
