/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    di.cpp

Abstract:

    Header file for testing port of DirectInput to XBOX.
	
Environment:

	XAPI

Revision History:

    05-10-00 : created by mitchd

--*/

// Hack! DbgPrint is much nicer than OutputDebugString so JonT exported it from xapi.lib
// (as DebugPrint) but the prototype isn't in the public headers...
extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}


class CDIDevice;
class CDITest;

CDITest *g_pDITest;

#define VERIFY_SUCCESS(_hr_, _expression_)\
_hr_ = (_expression_);\
if(FAILED(_hr_))\
{\
	DebugPrint("%s failed with code 0x%0.8x\n", #_expression_, _hr_);\
	DebugBreak();\
}

class CDITest
{
 public:
	CDITest();
	HRESULT			CreateDI8();
	void			ReleaseDI8(BOOL fCleanup);
	HRESULT			NotifyRemove(BOOL fTurnOn);
	HRESULT			EnumAndNotifyInsertion(BOOL fTurnOn);
	void			AddDevice(CDIDevice *pNewDevice);
	void			RemoveDevice(CDIDevice *pDoomedDevice, BOOL fIfNotControlDevice);
	CDIDevice		*FindDevice(REFGUID guidInstance);
	IDirectInput8	*GetDI8() {return m_pDI8;}
	void			BasicTest();
 private:
	void			UsesDi8() {if(!m_pDI8){DebugPrint("ERROR: m_pDI8 is NULL, and member function requires DI8!\n");DebugBreak();}}
	IDirectInput8	*m_pDI8;
	CDIDevice		*m_pControlDevice;
	CDIDevice		*m_pOtherDevices;
	BOOL			m_fRemoveNotifyOn;
	BOOL			m_fInsertNotifyOn;
};


class CDIDevice
{
public:
	CDIDevice(REFGUID guidInstance);
	~CDIDevice();
	HRESULT	CreateDID8(IDirectInput8 *pDI);
	void	ReleaseDID8();

	//
	//	Maps to DID8 commands
	//
	void Acquire(BOOL fAutoPoll);
	void Unacquire();
	void Poll();
	HRESULT GetDeviceState(DIXBOXCONTROLLERSTATE *pDiXBoxControllerState);
	void GetDeviceInfo(); //Call GetDeviceInfo and dumps the info to the debugger.
	void GetCapabilities(); //Call GetDeviceCapabilities and dump the info to the debugger.

	//
	//	Data
	//
	IDirectInputDevice8	*m_pDID8;
	GUID				m_guidInstance;
	CDIDevice			*m_pNextDevice;
	CDIDevice			*m_pPreviousDevice;
	BOOL				m_fAcquired;
};


//
//	Declare enumeration call back
//
#define ENUM_DEVICE_REMOVE_REF ((PVOID)0)
#define ENUM_DEVICE_INSERT_REF ((PVOID)1)

BOOL CALLBACK
EnumDevicesCallback(
	LPCDIDEVICEINSTANCE lpddi,
	LPVOID pvRef
	);

/*++
 Implementation of new and delete in-terms of LocalAlloc and FreeAlloc,
 we should revisit where this memory comes from
--*/

void * _cdecl operator new(size_t size)
{
	if(0==size) size = 1;
	return LocalAlloc(LMEM_FIXED, size);
}

void _cdecl operator delete(void *pvMem)
{
	if(pvMem) LocalFree(pvMem);
}
