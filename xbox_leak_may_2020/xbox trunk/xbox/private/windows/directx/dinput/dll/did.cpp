/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    did.cpp

Abstract:

    Implementation of IDirectInputDevice8 for XBOX

Environment:

	XAPI

Revision History:

    05-10-00 : created

--*/

#define DEBUG_MODULE_NAME  _MODULE_DIRECTINPUTDEVICE_
#define DEBUG_TRACE_NAME "DIDEV"
#include "dinputi.h"


/****************************************************************************
****	c'tor, d'tor, and initialization
****
***/
CDirectInputDevice8::CDirectInputDevice8() : 
	m_uRefCount(0), m_pOuterUnknown(NULL),
    m_bDeviceID(0), m_dwPlayerSlot(0),
    m_dwDeviceType(0), m_dwDeviceHandle(0),
	m_fProperyAutoPoll(TRUE)
{
	m_OutputFormatRumble.Header.hEvent = NULL;
	m_OutputFormatRumble.Header.dwStatus = ERROR_SUCCESS;
	m_OutputFormatRumble.bReportId = 0;
	m_OutputFormatRumble.bSize = OUTPUT_FORMAT_RUMBLE_SIZE;
}

CDirectInputDevice8::~CDirectInputDevice8()
{
    if(m_dwDeviceHandle)
    {
        Unacquire();
    }
}

BOOL CDirectInputDevice8::InternalInit(LPUNKNOWN pOuterUnknown, BYTE bDeviceID)
{
	m_pOuterUnknown = pOuterUnknown;
    m_ImplInnerUnk.Init(this);
    m_bDeviceID = bDeviceID;
    
    //
    //  Now gather basic information about the device
    //
    INPUT_DEVICE_INFORMATION DeviceInformation;
    DeviceInformation.dwSize = sizeof(INPUT_DEVICE_INFORMATION);
    if(ERROR_SUCCESS == InputQueryDeviceInformation(m_bDeviceID, &DeviceInformation))
    {
        m_dwDeviceType = MAKE_DIDEVICE_TYPE(DeviceInformation.bDeviceType, DeviceInformation.bDeviceSubtype);
        m_dwPlayerSlot = (DWORD)DeviceInformation.bPlayerSlot;
        return TRUE;
    }
    return FALSE;
}

/****************************************************************************
****	CAggregable Methods
****
***/

HRESULT
CDirectInputDevice8::ImplQI(
	REFIID riid,
	LPVOID *ppvObj
	)
/*++
	Routine Descirption:
		The real implementation of QueryInterface for CDirectInputDevice8.
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
	if(IID_IDirectInputDevice8 == riid || IID_IUnknown == riid)
	{
		*ppvObj=dynamic_cast<IDirectInputDevice8 *>(this); //Make sure that they get the IDirectInputDevice8 VTABLE
	}
	if(NULL == *ppvObj)
	{
		return E_NOINTERFACE;
	}
	AddRef();
	return S_OK;
}

ULONG
CDirectInputDevice8::ImplAddRef()
/*++
	Routine Descirption:
		The real implementation of AddRef for CDirectInputDevice8.
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
CDirectInputDevice8::ImplRelease()
/*++
	Routine Descirption:
		The real implementation of Release for CDirectInputDevice8.
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
CDirectInputDevice8::QueryInterface(
	REFIID riid,
	LPVOID * ppvObj
	)
/*++
	Routine Descirption:
		PUBLIC version QueryInterface for CDirectInputDevice8.
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
CDirectInputDevice8::AddRef()
/*++
	Routine Descirption:
		PUBLIC version of AddRef for CDirectInputDevice8.
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
CDirectInputDevice8::Release()
/*++
	Routine Descirption:
		PUBLIC version of Release for CDirectInputDevice8.
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

/*** IDirectInputDevice (2A and 7) methods ***/
STDMETHODIMP
CDirectInputDevice8::GetCapabilities(
    LPDIDEVCAPS lpDiDevCaps
    )
/*++
	Routine Descirption:

        This differs from DI for Windows in that you need to acquire the
        device in order to get the number of access and stuff.  We could
        get around this by opening the device when the object is created,
        and closing it in the destructor, but YUCK!

	Parameters:
	Return Value:
        DI_OK
        DIERR_NOTACQUIRED
		
--*/
{
    //
    //  Does work in both free and checked.
    //
    CHECK_STRUCT_SIZE(lpDiDevCaps->dwSize, DIDEVCAPS, IDirectInputDevice8::GetCapabilities);
    lpDiDevCaps->dwDevType = m_dwDeviceType;
    
    //
    //  Make sure that the device is aqcuired.
    //
    if(!m_dwDeviceHandle)
    {
        return DIERR_NOTACQUIRED;
    } else
    {
        //
        // We really need to query the device for its
        // capabilities at this point.  Then parse out
        // what we get from it.
        // For right now, lets lie and tell them about
        // DUKE.
        //
        lpDiDevCaps->dwAxes = 4;
        lpDiDevCaps->dwButtons = 12;
        lpDiDevCaps->dwFFDriverVersion = 0;
        lpDiDevCaps->dwFFMinTimeResolution = 0;
        lpDiDevCaps->dwFFSamplePeriod = 0;
        lpDiDevCaps->dwFirmwareRevision = 0;
        lpDiDevCaps->dwFlags = 0;
        lpDiDevCaps->dwHardwareRevision = 0;
        lpDiDevCaps->dwPOVs = 1;
    }
    return DI_OK;
}

STDMETHODIMP 
CDirectInputDevice8::EnumObjects(
    LPDIENUMDEVICEOBJECTSCALLBACK,
    LPVOID,
    DWORD
    )
/*++
	Routine Descirption:
        Right now it doesn't seem prudent to implement
        this, or encourage people to call it, we may change
        our minds.

	Parameters:
	Return Value:
        E_NOTIMPL;
--*/
{
    return E_NOTIMPL;
}

STDMETHODIMP
CDirectInputDevice8::GetProperty(
	REFGUID rguidProp,
    LPDIPROPHEADER pdihph
    )
/*++
	Routine Descirption:
        This will eventually be implemented with three properties supported.

        DIPROP_BUFFERSIZE ???

        Also, two new properties:

        DIPROP_PLAYERSLOT - the player slot number.
        DIPROP_AUTOPOLL   - the automatic poll rate.

	Parameters:
		rguidProp - Property to perform get operation on.
		pdihph	  - Header for placing 
	Return Value:
		DI_OK				- for supported property
		DIERR_UNSUPPORTED	- for unsupported property.
        
--*/
{
	CHECK_STRUCT_SIZE(pdihph->dwHeaderSize, DIPROPHEADER, IDirectInputDevice8::GetProperty);
	LPDIPROPDWORD pdipdw = (LPDIPROPDWORD)pdihph;
	if( &rguidProp == &DIPROP_AUTOPOLL)
	{
		CHECK_STRUCT_SIZE(pdihph->dwSize, DIPROPDWORD, IDirectInputDevice8::GetProperty);
		pdipdw->dwData = m_fProperyAutoPoll;
	} else if( &rguidProp == &DIPROP_PLAYERSLOT)
	{
		CHECK_STRUCT_SIZE(pdihph->dwSize, DIPROPDWORD, IDirectInputDevice8::GetProperty);
		pdipdw->dwData = m_dwPlayerSlot;
	} else
	{
		return DIERR_UNSUPPORTED;
	}
    return DI_OK;
}
STDMETHODIMP
CDirectInputDevice8::SetProperty(
    REFGUID rguidProp,
    LPCDIPROPHEADER pdihph
    )
/*++
	Routine Descirption:
        This will eventually be implemented with two properties supported.

        DIPROP_BUFFERSIZE ??
        
        and a new property.

        DIPROP_AUTOPOLL   - the automatic poll rate.

        If setting the former property the device must not be acquired.

	Parameters:
	Return Value:
        E_NOTIMPL;
--*/
{
    CHECK_STRUCT_SIZE(pdihph->dwHeaderSize, DIPROPHEADER, IDirectInputDevice8::GetProperty);
	LPDIPROPDWORD pdipdw = (LPDIPROPDWORD)pdihph;
	if( &rguidProp == &DIPROP_AUTOPOLL)
	{
		if(!m_dwDeviceHandle)
		{
			CHECK_STRUCT_SIZE(pdihph->dwSize, DIPROPDWORD, IDirectInputDevice8::GetProperty);
			m_fProperyAutoPoll = pdipdw->dwData ? TRUE : FALSE;
		} else
		{
			return DIERR_ACQUIRED;
		}
	} else
	{
		return DIERR_UNSUPPORTED;
	}
    return DI_OK;
}

STDMETHODIMP
CDirectInputDevice8::Acquire()
/*++
	Routine Descirption:
       Opens the device for now.  In the future, we may mess around with
       the polling method.
	Parameters:
	Return Value:
       
--*/
{
    if(m_dwDeviceHandle)
        return DIERR_ACQUIRED;

    DWORD dwError = InputOpenDevice(m_bDeviceID, m_fProperyAutoPoll, &m_dwDeviceHandle);
    if(dwError == ERROR_SHARING_VIOLATION)
        return DIERR_OTHERAPPHASPRIO;
    if(dwError == ERROR_DEVICE_NOT_CONNECTED)
        return DIERR_UNPLUGGED;
    return HRESULT_FROM_WIN32(dwError);
}

STDMETHODIMP
CDirectInputDevice8::Unacquire()
/*++
	Routine Descirption:
      Closes the device
	Parameters:
	Return Value:
       
--*/
{
    DWORD dwError;
	if(!m_dwDeviceHandle)
        return DIERR_NOTACQUIRED;
	dwError = InputCloseDevice(m_dwDeviceHandle);
	m_dwDeviceHandle = 0;
    return HRESULT_FROM_WIN32(dwError);
}



STDMETHODIMP
CDirectInputDevice8::GetDeviceState(
    DWORD cbData,
    LPVOID lpvData
    )
{
    PINPUT_FORMAT pInputFormat = reinterpret_cast<PINPUT_FORMAT>(lpvData);
    DWORD dwError;
    pInputFormat->bReportId = 0;
    ASSERT(cbData < 0xFF);
    pInputFormat->bSize = static_cast<BYTE>(cbData);
    dwError = InputGetDeviceState(m_dwDeviceHandle, pInputFormat, NULL);
    if(dwError == ERROR_DEVICE_NOT_CONNECTED)
    {
        return DIERR_UNPLUGGED;
    }
    return HRESULT_FROM_WIN32(dwError);
}

STDMETHODIMP
CDirectInputDevice8::GetDeviceData(
    DWORD,
    LPDIDEVICEOBJECTDATA,
    LPDWORD,
    DWORD
    )
{
	return E_NOTIMPL;
}


STDMETHODIMP
CDirectInputDevice8::SetEventNotification(
    HANDLE
    )
{
    return E_NOTIMPL;
}

STDMETHODIMP
CDirectInputDevice8::GetObjectInfo(
    LPDIDEVICEOBJECTINSTANCE,
    DWORD,
    DWORD
    )
{
    return E_NOTIMPL;
}

STDMETHODIMP
CDirectInputDevice8::GetDeviceInfo(
    LPDIDEVICEINSTANCE lpDiDeviceInstance
    )
{
    //
    //  Does work in both free and checked.
    //
    CHECK_STRUCT_SIZE(lpDiDeviceInstance->dwSize, DIDEVICEINSTANCE, IDirectInputDevice8::GetDeviceInfo);
    lpDiDeviceInstance->dwDevType = m_dwDeviceType;
    lpDiDeviceInstance->guidFFDriver = GUID_NULL;
    INITINSTANCEGUID(lpDiDeviceInstance->guidInstance, m_bDeviceID);
    lpDiDeviceInstance->guidProduct = GUID_NULL;
    lpDiDeviceInstance->tszInstanceName[0] = NULL;
    lpDiDeviceInstance->tszProductName[0] = NULL;
    lpDiDeviceInstance->wUsage = 0;
    lpDiDeviceInstance->wUsagePage = 0;
    return DI_OK;
}

STDMETHODIMP
CDirectInputDevice8::Poll()
{
	DWORD dwErrorCode;
	if(!m_dwDeviceHandle)
        return DIERR_NOTACQUIRED;
    if(m_fProperyAutoPoll)
		return DIERR_UNSUPPORTED;
	dwErrorCode = InputPollDevice(m_dwDeviceHandle);
	return HRESULT_FROM_WIN32(dwErrorCode);
}

STDMETHODIMP
CDirectInputDevice8::SendDeviceData(
    DWORD cbObjectData,
    LPCDIDEVICEOBJECTDATA rgdod,
    LPDWORD pdwInOut,
    DWORD 
    )
{
	DWORD dwError;
	if(GET_DIDEVICE_TYPE(m_dwDeviceType) == INPUT_DEVTYPE_GAMECONTROLLER)
	{
		if(ERROR_SUCCESS != m_OutputFormatRumble.Header.dwStatus)
		{
			return DIERR_REPORTFULL;	
		} else
		{
			for(DWORD i=0; i < *pdwInOut; i++)
			{
				if(rgdod[i].dwOfs == DIXROFS_LEFTMOTOR)
				{
					m_OutputFormatRumble.bLeftMotorSpeed = (BYTE)rgdod[i].dwData;
				} else if (rgdod[i].dwOfs == DIXROFS_RIGHTMOTOR)
				{
					m_OutputFormatRumble.bRightMotorSpeed = (BYTE)rgdod[i].dwData;
				}
			}
			dwError = InputSendDeviceReport(m_dwDeviceHandle, (POUTPUT_HEADER)&m_OutputFormatRumble);
			return HRESULT_FROM_WIN32(dwError);
		}
	}
	return DIERR_INVALIDPARAM;
}


STDMETHODIMP
DirectInputCreateDevice(
    DWORD dwDeviceInstance,
    REFIID riid,
    LPVOID *ppvOut,
    LPUNKNOWN punkOuter
    )
/*++
	Routine Descirption:
        Factory method to create DirectInputDevice

    Parameters:
        dwDeviceInstance  - LPARAM from WM_USB_DEVICE_INSERTED message.
        riid              - Interface ID to create
        ppvOut            - receives interface pointer
        punkOuter         - outer ("controlling") unknown
--*/
{	
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
    if(pObj->InternalInit(punkOuter, dwDeviceInstance))
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


/*** Not needed for XBOX ***/
STDMETHODIMP CDirectInputDevice8::SetDataFormat(LPCDIDATAFORMAT){return E_NOTIMPL;}
STDMETHODIMP CDirectInputDevice8::SetCooperativeLevel(HWND,DWORD){return E_NOTIMPL;}
STDMETHODIMP CDirectInputDevice8::RunControlPanel(HWND,DWORD){return E_NOTIMPL;}
STDMETHODIMP CDirectInputDevice8::Initialize(HINSTANCE,DWORD,REFGUID){return E_NOTIMPL;}
STDMETHODIMP CDirectInputDevice8::CreateEffect(REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN){return E_NOTIMPL;}
STDMETHODIMP CDirectInputDevice8::EnumEffects(LPDIENUMEFFECTSCALLBACK,LPVOID,DWORD){return E_NOTIMPL;}
STDMETHODIMP CDirectInputDevice8::GetEffectInfo(LPDIEFFECTINFO,REFGUID){return E_NOTIMPL;}
STDMETHODIMP CDirectInputDevice8::GetForceFeedbackState(LPDWORD){return E_NOTIMPL;}
STDMETHODIMP CDirectInputDevice8::SendForceFeedbackCommand(DWORD){return E_NOTIMPL;}
STDMETHODIMP CDirectInputDevice8::EnumCreatedEffectObjects(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD){return E_NOTIMPL;}
STDMETHODIMP CDirectInputDevice8::Escape(LPDIEFFESCAPE){return E_NOTIMPL;}
STDMETHODIMP CDirectInputDevice8::EnumEffectsInFile(LPCTSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD){return E_NOTIMPL;}
STDMETHODIMP CDirectInputDevice8::WriteEffectToFile(LPCTSTR,DWORD,LPDIFILEEFFECT,DWORD){return E_NOTIMPL;}
STDMETHODIMP CDirectInputDevice8::BuildActionMap(LPDIACTIONFORMAT,LPCTSTR,DWORD){return E_NOTIMPL;}
STDMETHODIMP CDirectInputDevice8::SetActionMap(LPDIACTIONFORMAT,LPCTSTR,DWORD){return E_NOTIMPL;}
STDMETHODIMP CDirectInputDevice8::GetImageInfo(LPDIDEVICEIMAGEINFOHEADER){return E_NOTIMPL;}
