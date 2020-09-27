#ifndef __DINPUTI_H__
#define __DINPUTI_H__
/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    di.cpp

Abstract:

    Internal header file for the XBOX implementation of DirectInput8.

Environment:

	XAPI

Revision History:

    05-10-00 : created

--*/

#include <objbase.h>
#include <dinput.h>
#include <dinputp.h>
#include <input.h>
#include "didbg.h"
#include "aggregat.h"

class CDirectInputDevice8 : public IDirectInputDevice8, public CAggregable
{
    /*** factory method is a friend ***/
    friend HRESULT DirectInputCreateDevice(
                            DWORD dwDeviceInstance,
                            REFIID riid,
                            LPVOID *ppvOut,
                            LPUNKNOWN punkOuter
                            );


    /*** private c'tor, d'tor and initialization ***/
	CDirectInputDevice8();
	~CDirectInputDevice8();
	BOOL InternalInit(LPUNKNOWN pOuterUnknown, DWORD dwDeviceInstance);

    /*** CAggregable methods ***/
	virtual HRESULT ImplQI(REFIID riid, LPVOID *ppvObj);
	virtual ULONG ImplAddRef();
	virtual ULONG ImplRelease();

	public:
	/*** IUnknown methods ***/
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID * ppvObj);
	STDMETHOD_(ULONG,AddRef)();
	STDMETHOD_(ULONG,Release)();
    
    /*** IDirectInputDevice2 methods ***/
    STDMETHOD(GetCapabilities)(LPDIDEVCAPS);
    STDMETHOD(EnumObjects)(LPDIENUMDEVICEOBJECTSCALLBACK,LPVOID,DWORD);
    STDMETHOD(GetProperty)(REFGUID,LPDIPROPHEADER);
    STDMETHOD(SetProperty)(REFGUID,LPCDIPROPHEADER);
    STDMETHOD(Acquire)();
    STDMETHOD(Unacquire)();
    STDMETHOD(GetDeviceState)(DWORD,LPVOID);
    STDMETHOD(GetDeviceData)(DWORD,LPDIDEVICEOBJECTDATA,LPDWORD,DWORD);
    STDMETHOD(SetDataFormat)(LPCDIDATAFORMAT);
    STDMETHOD(SetEventNotification)(HANDLE);
    STDMETHOD(SetCooperativeLevel)(HWND,DWORD);
    STDMETHOD(GetObjectInfo)(LPDIDEVICEOBJECTINSTANCE,DWORD,DWORD);
    STDMETHOD(GetDeviceInfo)(LPDIDEVICEINSTANCE);
    STDMETHOD(RunControlPanel)(HWND,DWORD);
    STDMETHOD(Initialize)(HINSTANCE,DWORD,REFGUID);
    STDMETHOD(CreateEffect)(REFGUID,LPCDIEFFECT,LPDIRECTINPUTEFFECT *,LPUNKNOWN);
    STDMETHOD(EnumEffects)(LPDIENUMEFFECTSCALLBACK,LPVOID,DWORD);
    STDMETHOD(GetEffectInfo)(LPDIEFFECTINFO,REFGUID);
    STDMETHOD(GetForceFeedbackState)(LPDWORD);
    STDMETHOD(SendForceFeedbackCommand)(DWORD);
    STDMETHOD(EnumCreatedEffectObjects)(LPDIENUMCREATEDEFFECTOBJECTSCALLBACK,LPVOID,DWORD);
    STDMETHOD(Escape)(LPDIEFFESCAPE);
    STDMETHOD(Poll)();
    STDMETHOD(SendDeviceData)(DWORD,LPCDIDEVICEOBJECTDATA,LPDWORD,DWORD);

    /*** IDirectInputDevice7 methods ***/
    STDMETHOD(EnumEffectsInFile)(LPCTSTR,LPDIENUMEFFECTSINFILECALLBACK,LPVOID,DWORD);
    STDMETHOD(WriteEffectToFile)(LPCTSTR,DWORD,LPDIFILEEFFECT,DWORD);

	/*** IDirectInputDevice8 methods ***/
	STDMETHOD(BuildActionMap)(LPDIACTIONFORMAT,LPCTSTR,DWORD);
    STDMETHOD(SetActionMap)(LPDIACTIONFORMAT,LPCTSTR,DWORD);
    STDMETHOD(GetImageInfo)(LPDIDEVICEIMAGEINFOHEADER);


    private:
	/*** private IUnknown stuff***/
	ULONG					m_uRefCount;
	LPUNKNOWN				m_pOuterUnknown;
	CImplInnerUnk			m_ImplInnerUnk;
    /*** private instance data***/
    BYTE                    m_bDeviceID;
	BOOL					m_fProperyAutoPoll;
    DWORD                   m_dwPlayerSlot;
    DWORD                   m_dwDeviceType;
    DWORD                   m_dwDeviceHandle;
	
	union {
	OUTPUT_FORMAT_RUMBLE	m_OutputFormatRumble;
	};
};



#endif __DINPUTI_H__