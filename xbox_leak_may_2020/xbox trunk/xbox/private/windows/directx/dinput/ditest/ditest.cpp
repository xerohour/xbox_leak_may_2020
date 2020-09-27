/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    di.cpp

Abstract:

    Main module for testing port of DirectInput to XBOX.
	
	Methods tested:

		DirectInputCreate
		DirectInputCreateEx

		IDirectInput8:
			CreateDevice
			CreateDeviceEx
			EnumDevices
			GetDeviceStatus
		IDirectInputDevice8:
			Acquire
			GetCapabilities
			GetDeviceInfo
			GetDeviceState
			GetProperty
			Poll
			SendDeviceData
			SetProperty
			Unacquire

Environment:

	XAPI

Revision History:

    05-10-00 : created by mitchd

--*/
#include <windows.h>
#include <initguid.h>
#include <dinput.h>

//
//	Hack, I need InputPumpCallbacks temporarily
//
#include <input.h>
#include "ditest.h"



DWORD
WINAPI
main( VOID )
{
    HRESULT hr;
	ULONG	uRef;
	DebugPrint("Welcome to the DI Test Applet: Built on %s at %s\n", __DATE__,__TIME__);
	

	g_pDITest = new CDITest;

	g_pDITest->BasicTest();

	delete g_pDITest;
	g_pDITest = NULL;


	return 0;
}

BOOL CALLBACK
EnumDevicesCallback(
	LPCDIDEVICEINSTANCE lpddi,
	LPVOID pvRef
	)
{
	CDIDevice *pDevice;

	if(pvRef == ENUM_DEVICE_REMOVE_REF)
	{
		pDevice = g_pDITest->FindDevice(lpddi->guidInstance);
		g_pDITest->RemoveDevice(pDevice, FALSE);
		delete pDevice;
	}

	if(pvRef == ENUM_DEVICE_INSERT_REF)
	{
		pDevice = new CDIDevice(lpddi->guidInstance);
		if(pDevice)
		{
			pDevice->CreateDID8(g_pDITest->GetDI8());
			g_pDITest->AddDevice(pDevice);
		}
	}
	return TRUE;
}


CDITest::CDITest():
m_pDI8(NULL), m_pControlDevice(NULL), m_pOtherDevices(NULL), m_fRemoveNotifyOn(FALSE), m_fInsertNotifyOn(FALSE)
{
	
}

HRESULT CDITest::CreateDI8()
{
	HRESULT hr;
	VERIFY_SUCCESS(hr,DirectInput8Create(NULL, DIRECTINPUT_VERSION, IID_IDirectInput8,(PVOID *)&m_pDI8, NULL));
	return hr;
}


void CDITest::ReleaseDI8(BOOL fCleanup)
{
	if(fCleanup)
	{
		EnumAndNotifyInsertion(FALSE);
		NotifyRemove(FALSE);
	}
	if(0 == m_pDI8->Release())
	{
		m_pDI8 = NULL;
	} else
	{
		DebugPrint("Refcount on DI8 didn't go to zero\n");
	}
}
HRESULT CDITest::NotifyRemove(BOOL fTurnOn)
{
	HRESULT hr = E_FAIL;
	
	UsesDi8();

	if(fTurnOn && !m_fRemoveNotifyOn)
	{
		VERIFY_SUCCESS(hr, m_pDI8->EnumDevices(0, EnumDevicesCallback, ENUM_DEVICE_REMOVE_REF, DIEDFL_NOTIFYREMOVE));
		if(SUCCEEDED(hr)) m_fRemoveNotifyOn = TRUE;
	} else if(!fTurnOn && m_fRemoveNotifyOn)
	{
		VERIFY_SUCCESS(hr, m_pDI8->EnumDevices(0, EnumDevicesCallback, ENUM_DEVICE_REMOVE_REF, DIEDFL_HALTNOTIFYREMOVE));
		if(SUCCEEDED(hr)) m_fRemoveNotifyOn = FALSE;
	}
	return hr;
}
	
HRESULT CDITest::EnumAndNotifyInsertion(BOOL fTurnOn)
{
	HRESULT hr = E_FAIL;
	
	UsesDi8();

	if(fTurnOn && !m_fInsertNotifyOn)
	{
		VERIFY_SUCCESS(hr, m_pDI8->EnumDevices(0, EnumDevicesCallback, ENUM_DEVICE_INSERT_REF, DIEDFL_ENUMANDNOTIFYINSERT));
		if(SUCCEEDED(hr)) m_fInsertNotifyOn = TRUE;
	} else if(!fTurnOn && m_fInsertNotifyOn)
	{
		VERIFY_SUCCESS(hr, m_pDI8->EnumDevices(0, EnumDevicesCallback, ENUM_DEVICE_INSERT_REF, DIEDFL_HALTNOTIFYINSERT));
		if(SUCCEEDED(hr)) m_fInsertNotifyOn = FALSE;
	}
	return hr;
}

void CDITest::AddDevice(CDIDevice *pNewDevice)
{
	//
	//	The first device added always becomes the control device
	//
	if(!m_pControlDevice)
	{
		pNewDevice->m_pNextDevice = pNewDevice->m_pPreviousDevice = NULL;
		m_pControlDevice = pNewDevice;
		DebugPrint("New control device detected.\n");
	} else
	{
		//
		//	Add to list of devices
		//
		pNewDevice->m_pPreviousDevice = NULL;
		pNewDevice->m_pNextDevice = m_pOtherDevices;
		m_pOtherDevices = pNewDevice;
	}
}

void CDITest::RemoveDevice(CDIDevice *pDoomedDevice, BOOL fIfNotControlDevice)
{
	if(!pDoomedDevice)
	{
		DebugPrint("Nothing to remove.!\n");
		return;
	}
	//
	// Check to make sure that it is not the control device.
	//
	if(m_pControlDevice == pDoomedDevice)
	{
		m_pControlDevice = NULL;
		DebugPrint("Control Device Removed.\n");
	} else
	{
		if(NULL == pDoomedDevice->m_pPreviousDevice)
		{
			if(pDoomedDevice != m_pOtherDevices)
			{
				DebugPrint("Serious Error in Remove Device\n");
				DebugBreak();
			}
			m_pOtherDevices = pDoomedDevice->m_pNextDevice;
		} else
		{
			pDoomedDevice->m_pPreviousDevice->m_pNextDevice = pDoomedDevice->m_pNextDevice;
		}
		if(pDoomedDevice->m_pNextDevice)
		{
			pDoomedDevice->m_pNextDevice->m_pPreviousDevice = pDoomedDevice->m_pPreviousDevice;
		}
		pDoomedDevice->m_pNextDevice = pDoomedDevice->m_pPreviousDevice = NULL;
	}
}

CDIDevice  *CDITest::FindDevice(REFGUID guidInstance)
{
	if(m_pControlDevice)
	{
		if(guidInstance == m_pControlDevice->m_guidInstance)
		{
			return m_pControlDevice;
		}
	}
	CDIDevice *pDevice = m_pOtherDevices;
	while(pDevice)
	{
		if(pDevice->m_guidInstance == guidInstance) break;
		pDevice = pDevice->m_pNextDevice;
	}
	DebugPrint("CDITest::FindDevice returning 0x%0.8x\n", pDevice);
	return pDevice;
}


void CDITest::BasicTest()
{
	BOOL					fTestOver = FALSE;
	DIXBOXCONTROLLERSTATE	prevControllerState;
	DIXBOXCONTROLLERSTATE	controllerState;
	BOOL					fAutoPoll = TRUE;
	HRESULT					hr;
	//
	//	Try DirectInput8Create
	//
	CreateDI8();
	
	//
	//	Turn on notifications
	//
	hr = NotifyRemove(TRUE);
	if(FAILED(hr))
	{
		DebugPrint("Call to NotifyRemove failed with 0x%0.8x\n", hr);
	}
	hr = EnumAndNotifyInsertion(TRUE);
	if(FAILED(hr))
	{
		DebugPrint("Call to EnumAndNotifyInsertion failed with 0x%0.8x\n", hr);
	}
	
	//
	//	Wait 
	//
	if(!m_pControlDevice)
	{
		DebugPrint("Insert a device to use to control this test . . .\n");
	}
	while(!m_pControlDevice)
	{
		Sleep(16);
		InputPumpCallbacks();
	}

	//
	//	Initialize the control device
	//
	m_pControlDevice->Acquire(fAutoPoll);

	//
	//	OK for the most basic device, 
	//
	DebugPrint("\n");
	DebugPrint("Press Button 0 to adjust the motor speed of the left motor.\n");
	DebugPrint("Press Button 1 to lock\\unlock the speed of the left motor.\n");
	DebugPrint("Press Button 2 to adjust the motor speed of the right motor.\n");
	DebugPrint("Press Button 3 to lock\\unlock the speed of the right motor.\n");
	DebugPrint("Press Start Button to dump state of device.\n");
	DebugPrint("Press Select Button to end test.\n");

	DIDEVICEOBJECTDATA		DeviceObjectData[2];
	DWORD					dwDeviceObjectCount = 2;
	DeviceObjectData[0].dwOfs = DIXROFS_LEFTMOTOR;
	DeviceObjectData[0].dwData = 0;
	DeviceObjectData[1].dwOfs = DIXROFS_RIGHTMOTOR;
	DeviceObjectData[1].dwData = 0;
	
	BOOL fLeftMotorLocked = FALSE;
	BOOL fRightMotorLocked = FALSE;
	BOOL fButton1Down = FALSE;
	BOOL fButton3Down = FALSE;
	BOOL fStartButtonDown = FALSE;
	BOOL fSpeedModified = FALSE;
	
	m_pControlDevice->GetDeviceState(&prevControllerState);
	while(!fTestOver)
	{
		//
		// Pump callbacks
		//
		InputPumpCallbacks();

		if(!m_pControlDevice)
		{
			DebugPrint("Reinsert control device.\n");
			while(!m_pControlDevice)
			{
				Sleep(16);
				InputPumpCallbacks();
			}
			fAutoPoll = !fAutoPoll;
			m_pControlDevice->Acquire(fAutoPoll);
		}
		
		//
		//	Poll device if autopoll is off
		//
		if(!fAutoPoll) m_pControlDevice->Poll();

		//
		//	Right now we still have a lowspeed POC that takes 3 packets, so
		//	sleep 9 ms, rather than 3 ms.
		//
		//	Game would normally be rendering rather than sleeping.
		//
		Sleep(9);

		//
		//	Grab input from control device
		//
		m_pControlDevice->GetDeviceState(&controllerState);
				
		//
		//	Do something interesting
		//
		if(controllerState.rgbAnalogButtons[1] && !fButton1Down)
		{
			//
			//	Level detect on lock button
			fButton1Down = TRUE;
			//
			//
			//	Toggle lock
			//
			fLeftMotorLocked = !fLeftMotorLocked;
			DebugPrint("Left Motor speed is %s\n", fLeftMotorLocked ? "locked." : "not locked");
			
		} else if(0 == controllerState.rgbAnalogButtons[1])
		{
			fButton1Down = FALSE;
		}
		if(controllerState.rgbAnalogButtons[3] && !fButton3Down)
		{
			//
			//	Level detect on lock button
			//
			fButton3Down = TRUE;
			//
			//	Toggle lock
			//
			fRightMotorLocked = !fRightMotorLocked;
			DebugPrint("Right Motor speed is %s\n", fRightMotorLocked ? "locked." : "not locked");
		} else if(0 == controllerState.rgbAnalogButtons[3])
		{
			fButton3Down = FALSE;
		}

		//
		//	Adjust motor speeds
		//
		fSpeedModified = FALSE;
		if(!fLeftMotorLocked)
		{
			if(DeviceObjectData[0].dwData != (DWORD)controllerState.rgbAnalogButtons[0])
			{
				DeviceObjectData[0].dwData = (DWORD)controllerState.rgbAnalogButtons[0];
				fSpeedModified = TRUE;
			}
		}
		if(!fRightMotorLocked)
		{
			DeviceObjectData[1].dwData = (DWORD)controllerState.rgbAnalogButtons[2];
			fSpeedModified = TRUE;
		}
		if(fSpeedModified)
		{
			m_pControlDevice->m_pDID8->SendDeviceData(sizeof(DIDEVICEOBJECTDATA), DeviceObjectData, &dwDeviceObjectCount, 0);
		}
		if(controllerState.bmButtons && !fStartButtonDown)
		{
			fStartButtonDown = TRUE;
			DebugPrint("controllerState.bmButtons = 0x%0.4x\n", controllerState.bmButtons);
			for(int buttonNum=0; buttonNum < 8; buttonNum++)
			{
				DebugPrint("controllerState.rgbAnalogButtons[%d] = %d\n", buttonNum, controllerState.rgbAnalogButtons[buttonNum]);
			}
			DebugPrint("controllerState.wDPAD = 0x%0.4x\n", controllerState.wDPAD);
			DebugPrint("controllerState.wThumb1X = 0x%0.4x\n", controllerState.wThumb1X);
			DebugPrint("controllerState.wThumb1Y = 0x%0.4x\n", controllerState.wThumb1Y);
			DebugPrint("controllerState.wThumb2X = 0x%0.4x\n", controllerState.wThumb2X);
			DebugPrint("controllerState.wThumb2Y = 0x%0.4x\n", controllerState.wThumb2Y);
			DebugPrint("controllerState.wTiltX = 0x%0.4x\n", controllerState.wTiltX);
			DebugPrint("controllerState.wTiltY = 0x%0.4x\n", controllerState.wTiltY);
		} else if(!controllerState.bmButtons)
		{
			fStartButtonDown = FALSE;
		}
		/*
		if(controllerState.bmButtons&2)
		{
			DebugPrint("Terminating the test.\n");
			break;
		}*/
		
		//
		//	Sleep to simulate real work
		//
		Sleep(10);
	}
}

CDIDevice::CDIDevice(REFGUID guidInstance) : 
	m_pDID8(NULL), m_guidInstance(guidInstance),
	m_pNextDevice(NULL), m_pPreviousDevice(NULL),
	m_fAcquired(FALSE)
{

}

CDIDevice::~CDIDevice()
{
	Unacquire();
	ReleaseDID8();
}
HRESULT CDIDevice::CreateDID8(IDirectInput8 *pDI)
{
	HRESULT hr;
	VERIFY_SUCCESS(hr,pDI->CreateDevice(m_guidInstance, &m_pDID8, NULL));
	return hr;
}

void CDIDevice::ReleaseDID8()
{
	ULONG refCount = m_pDID8->Release();
	if(0==refCount)
	{
		m_pDID8 = NULL;
	} else
	{
		DebugPrint("Refcount (%d) for m_pDID7 is not zero!\n", refCount);
		DebugBreak();
	}
}


void CDIDevice::Acquire(BOOL fAutoPoll)
{
	
	if(m_fAcquired) return;
	HRESULT hr;
	DIPROPDWORD dipdw;
	dipdw.diph.dwSize = sizeof(DIPROPDWORD);
	dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	dipdw.diph.dwObj = 0;
	dipdw.diph.dwHow = DIPH_DEVICE;
	dipdw.dwData = fAutoPoll;
	DebugPrint("Acquiring device with autopoll %s\n", fAutoPoll ? "on" : "off");
	if(m_pDID8)
	{
		VERIFY_SUCCESS(hr, m_pDID8->SetProperty(DIPROP_AUTOPOLL, &dipdw.diph));
		VERIFY_SUCCESS(hr, m_pDID8->Acquire());
	}
	m_fAcquired = TRUE;
}

void CDIDevice::Unacquire()
{
	if(!m_fAcquired) return;
	HRESULT hr = E_FAIL;
	if(m_pDID8)
	{
		VERIFY_SUCCESS(hr, m_pDID8->Unacquire());
	}
	m_fAcquired = FALSE;
}


void CDIDevice::Poll()
{
	HRESULT hr = E_FAIL;
	if(m_pDID8)
	{
		VERIFY_SUCCESS(hr, m_pDID8->Poll());
	}
}
HRESULT CDIDevice::GetDeviceState(
	DIXBOXCONTROLLERSTATE *pDiXBoxControllerState
	)
{
	HRESULT hr = E_FAIL;
	if(m_pDID8)
	{
		VERIFY_SUCCESS(hr, m_pDID8->GetDeviceState(sizeof(DIXBOXCONTROLLERSTATE), pDiXBoxControllerState));
	}
	return hr;
}
void CDIDevice::GetDeviceInfo()
 //Call GetDeviceInfo and dumps the info to the debugger.
{
	HRESULT hr = E_FAIL;
	DIDEVICEINSTANCE deviceInfo;

	if(m_pDID8)
	{
		deviceInfo.dwSize = sizeof(DIDEVICEINSTANCE);
		hr =m_pDID8->GetDeviceInfo(&deviceInfo);
		if(SUCCEEDED(hr))
		{
			DebugPrint("Device Type: %s\n", deviceInfo.dwDevType);
			//DebugPrint("GuidInstance {0x%0.8x,deviceInfo.guidInstance);
			//	deviceInfo.guidProduct;
			//	deviceInfo.tszInstanceName;
			//	deviceInfo.tszProductName;
			//	deviceInfo.wUsage;
			//	deviceInfo.wUsagePage;
		} else
		{
			DebugPrint("Get device info failed: 0x%0.8x\n", hr);
		}
	}
	
}
void CDIDevice::GetCapabilities()
//Call GetDeviceCapabilities and dump the info to the debugger.
{
	HRESULT hr;
	DIDEVCAPS diDevCaps;
	if(m_pDID8)
	{
		diDevCaps.dwSize = sizeof(DIDEVCAPS);
		hr =m_pDID8->GetCapabilities(&diDevCaps);
		if(SUCCEEDED(hr))
		{
			DebugPrint("diDevCaps.dwAxes                = 0x%0.8x\n", diDevCaps.dwAxes);
			DebugPrint("diDevCaps.dwButtons             = 0x%0.8x\n", diDevCaps.dwButtons);
			DebugPrint("diDevCaps.dwDevType             = 0x%0.8x\n", diDevCaps.dwDevType);
			DebugPrint("diDevCaps.dwFFDriverVersion     = 0x%0.8x\n", diDevCaps.dwFFDriverVersion);
			DebugPrint("diDevCaps.dwFFMinTimeResolution = 0x%0.8x\n", diDevCaps.dwFFMinTimeResolution);
			DebugPrint("diDevCaps.dwFFSamplePeriod      = 0x%0.8x\n", diDevCaps.dwFFSamplePeriod);
			DebugPrint("diDevCaps.dwFirmwareRevision    = 0x%0.8x\n", diDevCaps.dwFirmwareRevision);
			DebugPrint("diDevCaps.dwFlags               = 0x%0.8x\n", diDevCaps.dwFlags);
			DebugPrint("diDevCaps.dwHardwareRevision    = 0x%0.8x\n", diDevCaps.dwHardwareRevision);
			DebugPrint("diDevCaps.dwPOVs                = 0x%0.8x\n", diDevCaps.dwPOVs);
		} else
		{
			DebugPrint("Get device capabilities: 0x%0.8x\n", hr);
		}
	}
}

/*

	//
	//	Repeat loop infinitely
	//
	while(1)
	{
		DebugPrint("Creating deviceCollection:\n");
		pDeviceCollection = new CXidDeviceCollection;
		if(pDeviceCollection)
		{
			DebugPrint("Detecting devices currently inserted . . .:\n");
			InputAdviseDeviceChanges((PFNINPUTDEVICECHANGEPROC)DeviceChanges, (PVOID)pDeviceCollection);
			//
			//	Poll for roughly 30 seconds
			//
			DebugPrint("Poll for roughly 30 seconds:\n");
			for(int loop=0; loop < 1800; loop++)
			{
				pDeviceCollection->PollDevices();
				Sleep(16);
				InputPumpCallbacks();
			}
			DebugPrint("Stop detecting devices. . .:\n");
			InputUnadviseDeviceChanges();
			DebugPrint("Deleteing deviceCollection:\n");
			delete pDeviceCollection;
		}
		DebugPrint("Pause for about 30 seconds before creating a new device colletion.\n");
		Sleep(30000);
		DebugPrint("Creating deviceCollection:\n");
		pDeviceCollection = new CXidDeviceCollection;
		if(pDeviceCollection)
		{
			DebugPrint("Detecting devices currently inserted . . .:\n");
			InputAdviseDeviceChanges((PFNINPUTDEVICECHANGEPROC)DeviceChanges, (PVOID)pDeviceCollection);
			//
			//	Poll for roughly 15 seconds
			//
			DebugPrint("Poll for roughly 15 seconds:\n");
			for(int loop=0; loop < 900; loop++)
			{
				pDeviceCollection->PollDevices();
				Sleep(16);
				InputPumpCallbacks();
			}
			//
			//	Close the devices
			//
			DebugPrint("Closing Devices:\n");
			pDeviceCollection->CloseDevices();
			//
			//	Pause for 15 seconds, continue pumping for messages.
			//
			DebugPrint("Pause for roughly 15 seconds:\n");
			for(loop=0; loop < 900; loop++)
			{
				Sleep(16);
				InputPumpCallbacks();
			}
			DebugPrint("Reopen devices:\n");
			pDeviceCollection->OpenDevices();
			//
			//	Poll for roughly 15 seconds
			//
			DebugPrint("Poll for roughly 15 seconds:\n");
			for(loop=0; loop < 900; loop++)
			{
				pDeviceCollection->PollDevices();
				Sleep(16);
				InputPumpCallbacks();
			}
			DebugPrint("Stop detecting devices. . .:\n");
			InputUnadviseDeviceChanges();
			DebugPrint("Deleteing deviceCollection:\n");
			delete pDeviceCollection;
			
		}
		DebugPrint("Pause for about 30 seconds before creating a new device colletion.\n");
		Sleep(30000);
	};

*/