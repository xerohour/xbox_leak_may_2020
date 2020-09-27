//-----------------------------------------------------------------------------
// File: USBDesc.cpp
//
// Desc: General purpose reporting tool for USB peripherals 
//
// Hist: November 2001 - Created
//
//
// Author:  Dennis Krueger <a-denkru>
//
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include <process.h>
#include "USBDescApp.h"
#include "draw.h"
#include <stdlib.h>
#include <usb.h>
#include "..\inc\i_slixdriver.h"	// shared header with slixd
#include "..\inc\slixdriver.h"		// shared header with slixd


#define WHITECOLOR 0xffffffff
#define CRIMSONCOLOR 0xffDC143C
//#define TITLECOLOR	0xffffd700
#define TITLECOLOR	0xff87CEFA


// Classes for saving and text screens 
// support methods for TextScreenLine and TextScreenArray 
CTextScreenLine::CTextScreenLine()
{
	m_sx = 0;
	m_sy = 0;
	m_dwColor = 0;
	m_strText = NULL;
	m_dwFlags = 0;
}

CTextScreenLine::CTextScreenLine(float sx, float sy, DWORD Color, WCHAR * pTextStr, DWORD dwFlags)
{
	m_sx = sx;
	m_sy = sy;
	m_dwColor = Color;
	m_strText = pTextStr;
	m_dwFlags = dwFlags;
};



CTextScreenArray::CTextScreenArray()
{
	m_LineCount = 0;
}


CTextScreenArray::~CTextScreenArray()
{
	;
}



int CTextScreenArray::Add(CTextScreenLine * pThisLine)
{
	if(m_LineCount >= MAX_LINES) return -1;
	m_TextLines[m_LineCount] = pThisLine;
	m_LineCount++;
	return m_LineCount-1;  // return index
}

int CTextScreenArray::Replace(CTextScreenLine * pThisLine, int Index)
{
	if(Index >= MAX_LINES) return -1;
	m_TextLines[Index] = pThisLine;
	return Index;  // return index
}

int CTextScreenArray::GetLineCount()
{
	return m_LineCount;
}

CTextScreenLine *
CTextScreenArray::GetLine(int Index)
{
	if(Index >= m_LineCount) return NULL; // out of range
	return m_TextLines[Index];
}


//-----------------------------------------------------------------------------
// Name: class CInputTestApp
// Desc: Main class to run this application. Most functionality is inherited
//       from the CXBApplication base class.
//-----------------------------------------------------------------------------
class CUtilityApp 
{
public:
    CUtilityApp();
	~CUtilityApp();

    virtual HRESULT Initialize();
	virtual void Run();

	CTextScreenArray	m_ThisScreen;

	BOOL	m_fStopRender;
	BOOL	m_fStopRefresh;
	HANDLE  m_hSynchRender;
	HANDLE  m_hSynchRefresh;
	BOOL	m_LivePorts[4];

};


DWORD WINAPI Render(void * pParam);
DWORD WINAPI Refresh(void * pParam);

//-----------------------------------------------------------------------------
// Name: main()
// Desc: Entry point to the program.
//-----------------------------------------------------------------------------
VOID __cdecl main()
{
    KeepCode(); // do nothing function to force linker not to throw away as
				// ... uncalled the functions in the Slix Driver

	XInitDevices(0,NULL); // initialize the devices

	drInit();
	// find all attached controllers
	Sleep(2000);  // delay a couple seconds to allow emumeration to complete

	while(1) // run until rebooted
	{

		CUtilityApp * xbApp = new CUtilityApp();
		if( FAILED( xbApp->Initialize() ) )
			return;
		drCls();
		drShowScreen();
		xbApp->Run();
		delete xbApp; // end of test for this controller

	}
}



//-----------------------------------------------------------------------------
// Name: CXBoxSample (constructor)
// Desc: Constructor for CInputTestApp class
//-----------------------------------------------------------------------------
CUtilityApp::CUtilityApp() 
{
;
}

CUtilityApp::~CUtilityApp()
{
}

void CompleteRequest(PURB pUrb, KEVENT * pEvent)
{

    KeSetEvent(pEvent, IO_NO_INCREMENT, FALSE);
	KdPrint(("URB Completion Status: 0x%x\n",pUrb->Header.Status));


	
}

BOOL
GetXidCapabilitiesOut(PDEVICE_EXTENSION pDevExt)
{
	IUsbDevice * pDevice = pDevExt->Device;
    KEVENT		event;
	URB_CONTROL_TRANSFER	urb;
    //
    //  Build the URB
    //
    KeInitializeEvent(&event, NotificationEvent, FALSE);
    USB_BUILD_CONTROL_TRANSFER(
            &urb,
            NULL,
            (PVOID)pDevExt->bCapsOutBuff/*(&pDevExt->InputCaps.Out-2) */,
            sizeof(XINPUT_RUMBLE)+2,
            USB_TRANSFER_DIRECTION_IN,
            (PURB_COMPLETE_PROC)CompleteRequest,
            &event,
            TRUE,
            USB_DEVICE_TO_HOST | USB_VENDOR_COMMAND | USB_COMMAND_TO_INTERFACE,
            XID_COMMAND_GET_CAPABILITIES,
            0x0200,
            pDevExt->InterfaceDescriptor.bInterfaceNumber,
            sizeof(XINPUT_RUMBLE)+2
            );

		pDevice->SubmitRequest((PURB) &urb);

        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
		return TRUE;


}

BOOL
GetXidCapabilitiesIn(PDEVICE_EXTENSION pDevExt)
{
	IUsbDevice * pDevice = pDevExt->Device;
	URB_CONTROL_TRANSFER urb;
	KEVENT event;

   //
    //  Build the URB
    //
    USB_BUILD_CONTROL_TRANSFER(
            &urb,
            NULL,
            (PVOID)(pDevExt->bCapsInBuff /* &pDevExt->InputCaps.In -2*/),
            sizeof(XINPUT_GAMEPAD)+2,
            USB_TRANSFER_DIRECTION_IN,
            (PURB_COMPLETE_PROC)CompleteRequest,
            &event,
            TRUE,
            USB_DEVICE_TO_HOST | USB_VENDOR_COMMAND | USB_COMMAND_TO_INTERFACE,
            XID_COMMAND_GET_CAPABILITIES,
            0x0100,
             pDevExt->InterfaceDescriptor.bInterfaceNumber,
            sizeof(XINPUT_GAMEPAD)+2
            );
    KeInitializeEvent(&event, NotificationEvent, FALSE);
    pDevice->SubmitRequest((PURB)&urb);

    //
    //  Wait for transfer to complete
    //
    KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
	return TRUE;

}

BOOL
GetDeviceDescriptor(PDEVICE_EXTENSION pDevExt)
{
	IUsbDevice * pDevice = pDevExt->Device;
    KEVENT		event;
	URB_CONTROL_TRANSFER	urb;
    //
    //  Build the URB
    //
    KeInitializeEvent(&event, NotificationEvent, FALSE);
    USB_BUILD_CONTROL_TRANSFER(
            &urb,
            NULL,
            (PVOID)&pDevExt->DeviceDescriptor/*(&pDevExt->InputCaps.Out-2) */,
            sizeof(USB_DEVICE_DESCRIPTOR),
            USB_TRANSFER_DIRECTION_IN,
            (PURB_COMPLETE_PROC)CompleteRequest,
            &event,
            TRUE,
            USB_DEVICE_TO_HOST ,
            USB_REQUEST_GET_DESCRIPTOR,
            0x0100,
            pDevExt->InterfaceDescriptor.bInterfaceNumber,
            sizeof(USB_DEVICE_DESCRIPTOR)
            );

	pDevice->SubmitRequest((PURB) &urb);

    KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
	if(urb.Hdr.Status != 0)
	{
		
		pDevExt->DeviceDescriptor.bLength = 0xff; // signal failure to output routine
		*(LONG *) &pDevExt->DeviceDescriptor.bcdUSB = urb.Hdr.Status;
	}
	return TRUE;


}


void
CUtilityApp::Run()
{
	HANDLE	hResult;
	DEVICE_EXTENSION * pDevExt;
	DEVICE_EXTENSION * dePorts[2] = {0,0};
	BOOL	fResult;
	URB		Urb;
	BYTE	bPort,bSlot,bInterfaceNumber;

// spawn a refresh thread so we don't need to maintain that either
	m_fStopRefresh = FALSE;

	hResult = CreateThread(NULL,0,Refresh,this,0,NULL);
	if(NULL == hResult)
	{
		DWORD dwResult = GetLastError();
		return;
	}
	Sleep(0); // allow refresh thread to run

// spawn the render thread so we don't need to maintain it
	m_fStopRender = FALSE;
	hResult = CreateThread(NULL,0,Render,this,0,NULL);
	if(NULL == hResult)
	{
		DWORD dwResult = GetLastError();
		return;
	}
	while(1)
	{
		// check for new device arrival
		if(gpDriverExtension)
		{
			if( 0 != gInsertedCount)
			{
				// got a new device
				
				// get device extension
				KIRQL OldIrql = KeRaiseIrqlToDpcLevel();
				pDevExt = gInsertedDevices[--gInsertedCount];
				KeLowerIrql(OldIrql);

				bPort = pDevExt->bPort;
				bSlot = pDevExt->bSlot;
				bInterfaceNumber = pDevExt->bInterfaceNumber;
				IUsbDevice * pDevice = pDevExt->Device;
				if(FALSE == pDevExt->fGotCaps && TRUE == pDevExt->fAvail)
				{

					// we're going to fill in capabilities and descriptor, open control pipe
					USB_BUILD_OPEN_DEFAULT_ENDPOINT((PURB_OPEN_ENDPOINT)&Urb);
					USBD_STATUS status = pDevice->SubmitRequest(&Urb);
				}

				if(0 == bSlot)
				{
					// got a port, check if already handling
					if(dePorts[bPort] == NULL)
					{
						// got a new port (controller)							
						dePorts[bPort] = pDevExt;
					}
					// check if add device is completed
					// have we gotten our capabilities info?
					if(FALSE == pDevExt->fGotCaps && TRUE == pDevExt->fAvail)
					{
						// get capabilities, must be done in the order, first in then out
						// ... the bottom of the out buffer is overwritten by the in call

						fResult = GetXidCapabilitiesIn(pDevExt);
						if(FALSE == fResult)
						{
							// report failure of get In capabilities
						}
						fResult = GetXidCapabilitiesOut(pDevExt);
						if(FALSE == fResult)
						{
							// report failure of get Out Capabilities
						}
					} //  if deGotCaps
				} // if 0 == slot
				// Have we filled in the device descriptor for this device?
				if(FALSE == pDevExt->fGotCaps && TRUE == pDevExt->fAvail)
				{
					fResult = GetDeviceDescriptor(pDevExt);
					if(FALSE == fResult)
					{
						// report failure of get Descriptor
					}
					// finally close control pipe
					// we're going to fill in capabilities and descriptor, open control pipe
					USB_BUILD_CLOSE_DEFAULT_ENDPOINT((PURB_OPEN_ENDPOINT)&Urb.CloseEndpoint,NULL,NULL);
					USBD_STATUS status = pDevice->SubmitRequest(&Urb);
				}
				pDevExt->fGotCaps = TRUE;
			} //  if device						
		} // gpDriverExtension
	} // while(1)

	m_fStopRefresh = TRUE; // turn off refresh
	m_fStopRender = TRUE; // stop rendering

	// wait for refresh and render to quit
	WaitForSingleObject(m_hSynchRefresh,INFINITE);
	WaitForSingleObject(m_hSynchRender,INFINITE);
	
	return;



}



//-----------------------------------------------------------------------------
// Name: Initialize
// Desc: Peforms initialization
//-----------------------------------------------------------------------------
HRESULT
CUtilityApp::Initialize()
{

	m_fStopRender = FALSE; // init flags
	m_fStopRefresh = FALSE;
    // init drawing from cpxlib
    // locate the controller to initialize
	m_hSynchRefresh = CreateEvent(NULL,FALSE,FALSE,NULL);
	if(NULL == m_hSynchRefresh) return E_FAIL;
	m_hSynchRender = CreateEvent(NULL,FALSE,FALSE,NULL);
	if(NULL == m_hSynchRender) return E_FAIL;
	for(int i = 0; i < 4; i++)
	{
		m_LivePorts[i] = FALSE;
	}


    return S_OK;
}



//-----------------------------------------------------------------------------
// Name: FrameMove
// Desc: Performs per-frame updates
//-----------------------------------------------------------------------------
DWORD
WINAPI Refresh(void * pThis)
{
	
	CUtilityApp * pApp = (CUtilityApp *) pThis;
	// make mask for mu test
	while(FALSE == pApp->m_fStopRefresh)
	{
		// do periodic activity
		Sleep(0); // release quantum
	}
	SetEvent(pApp->m_hSynchRefresh); // inform main loop we're done

	return S_OK;
}


USHORT MakeChars(BYTE bByte)
{
	USHORT usResult;
	BYTE bLNibble,bHNibble;

	bLNibble = bByte & 0x0f;
	bHNibble = bByte >> 4;
	if(bHNibble < 10)
	{
		usResult = bHNibble + 0x30;
	} else
	{
		usResult = bHNibble + 0x41 - 0x0a;
	}
	usResult <<= 8;
	if(bLNibble < 10)
	{
		usResult += bLNibble + 0x30;
	} else
	{
		usResult += bLNibble + 0x41 - 0x0a;
	}
	return usResult;
}

BOOL MakeWStringOfBytes(BYTE * pInBuff,int iInLen, WCHAR * pOutBuff, int iOutLen)
{
	USHORT	usByteChars;
	USHORT * pTemp = (USHORT *) pOutBuff; // going to move two chars at a time
	if(iOutLen < (iInLen * 4) + 2) return FALSE;
	for(int i = 0; i < iInLen; i++)
	{
		// get byte
		usByteChars = MakeChars(pInBuff[i]);

		*pTemp++ =(WCHAR) (usByteChars >> 8); // upper character
		*pTemp++ = (WCHAR) (usByteChars & 0xff); // lower character

	}
	pTemp[i] = 0x00; // terminate string
	return TRUE;
}


void Spacify(WCHAR * pBuffer)
{
	WCHAR	WorkBuff[1000] = {0};
	WCHAR	wcTemp[3];
	WCHAR	*pTemp;
	int		iLen,iValue;
	pTemp = pBuffer;
	WCHAR	*pWork = WorkBuff;
	wcTemp[2] = 0x00; // null terminator

	while(1)
	{
		wcTemp[0] = pTemp[0]; // get first char of length byte; example 0x0030 0x0039 = len 0x09
		wcTemp[1] = pTemp[1]; // get second char of length byte
		if(0 == wcTemp[0]) break; // done if hit string terminator
		iLen = _wtoi(wcTemp); // length of this descriptor
		memcpy(pWork,pTemp,iLen*4);
		pWork += iLen*2;
		pTemp += iLen*2;
		*pWork++ = L' ';
	}
	// finished spacing all descriptor, copy back over top of original
	// get new length
	wcscpy(pBuffer,WorkBuff);
	return;
}


//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the scene
//-----------------------------------------------------------------------------
DWORD WINAPI Render(void * pThis)
{
	BYTE * pTemp;
	CUtilityApp * pApp = (CUtilityApp *) pThis;
// setup frame rate
	WCHAR wcBuffer[500];
	DEVICE_EXTENSION * pDevExt;
	USB_DEVICE_DESCRIPTOR * pDevDesc;
	USB_CONFIGURATION_DESCRIPTOR * pCfgDesc;
	USB_INTERFACE_DESCRIPTOR * pIDesc;
	float BaseY;

	while(FALSE == pApp->m_fStopRender)
	{
		Sleep(100); // refresh 10 times a second
		drCls();
		drSetSize(50, 100, WHITECOLOR, 0xff000000); //set the color
		drPrintf(200,50,L"USB Peripheral Descriptors");
		drSetSize(50, 100, TITLECOLOR, 0xff000000); //set the color
		if(gpDriverExtension)
		{
			BOOL fSlotOnly = FALSE;
			int	 iDeviceCount = 0;
			// first check for any device other than controllers to control page display
			for(int port = 0; port < 2; port++) // only handling first two ports
			{
				for(int slot = 0; slot < 3; slot++) // only handling first two slots
				{
					for(int InterfaceNumber = 0; InterfaceNumber < 2; InterfaceNumber++)
					{
						if(gpDriverExtension->Nodes[port][slot][InterfaceNumber].Device && slot)
						{
							fSlotOnly = TRUE;
						}
					}
				}
			}

			for(int port = 0; port < 2; port++) // only handling first two ports
			{
				for(int slot = 0; slot < 3; slot++) // only handling first two slots
				{
					for(int InterfaceNumber = 0; InterfaceNumber < 2; InterfaceNumber++)
					{
						if(gpDriverExtension->Nodes[port][slot][InterfaceNumber].Device && iDeviceCount < 2)
						{

							pDevExt = &gpDriverExtension->Nodes[port][slot][InterfaceNumber];
							pDevDesc = &pDevExt->DeviceDescriptor;
							// is this a controller or an MU?

							memset(wcBuffer,0x00,sizeof(wcBuffer));
							if(0 == slot && FALSE == fSlotOnly) // got a controller and no slot-based device
							{
								BaseY = (float) 60 + (170 * iDeviceCount);
								swprintf(wcBuffer,L"Port %d",port);
								drPrintf(50, BaseY,wcBuffer);
								iDeviceCount++;
								// display device descriptor
								memset(wcBuffer,0x00,sizeof(wcBuffer));
								if(pDevDesc->bLength == 0xff) // failure signal
								{
									swprintf(wcBuffer,L"Device Desc:  Failure! Status = %x",*(ULONG *)&pDevDesc->bcdUSB);
								} else
								{
									swprintf(wcBuffer,
										L"Device Desc: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
										pDevDesc->bLength,
										pDevDesc->bDescriptorType,
										*(PBYTE)(&pDevDesc->bcdUSB),
										*((PBYTE)(&pDevDesc->bcdUSB)+1),
										pDevDesc->bDeviceClass,
										pDevDesc->bDeviceSubClass,
										pDevDesc->bDeviceProtocol,
										pDevDesc->bMaxPacketSize0,
										*(PBYTE)(&pDevDesc->idVendor),
										*((PBYTE)(&pDevDesc->idVendor)+1),
										*(PBYTE)(&pDevDesc->idProduct),
										*((PBYTE)(&pDevDesc->idProduct)+1),
										*(PBYTE)(&pDevDesc->bcdDevice),
										*((PBYTE)(&pDevDesc->bcdDevice)+1),
										pDevDesc->iManufacturer,
										pDevDesc->iProduct,
										pDevDesc->iSerialNumber,
										pDevDesc->bNumConfigurations
										);
								}

								drPrintf(50,BaseY + 20,wcBuffer);


								// display configuration descriptors
								// get a pointer to the descriptor data
								pCfgDesc = &pDevExt->ConfigurationDescriptor; 
								int iCfgLen = pCfgDesc->wTotalLength; // get the total length
								// new logic starts here
								memset(wcBuffer,0x00,sizeof(wcBuffer));
								MakeWStringOfBytes((BYTE *) pDevExt->bConfigData,iCfgLen,wcBuffer,sizeof(wcBuffer));
								Spacify(wcBuffer);  // add spaces between descriptors
								// now display bytes in WcBuffer 32 chars at a time
								int iChars = wcslen(wcBuffer);
								int iNumLines = iChars / 32;
								int LineInc = 40;
								drPrintf(50,BaseY+LineInc,L"Config Data:",0);
								WCHAR wcLineBuffer[50];
								WCHAR * pCurPtr = wcBuffer;
								for(int i = 0; i < iNumLines; i++)
								{
									memset(wcLineBuffer,0x00,sizeof(wcLineBuffer));
									memcpy(wcLineBuffer,pCurPtr,64);
									pCurPtr +=32;
									drPrintf(180,BaseY+LineInc,wcLineBuffer,0);
									LineInc += 20;
								}
								// do partial last line
								int iRem = iChars % 32;
								if(iRem)
								{
									memset(wcLineBuffer,0x00,sizeof(wcLineBuffer));
									memcpy(wcLineBuffer,pCurPtr,iRem*2);
									drPrintf(180,BaseY+LineInc,wcLineBuffer,0);
								}
								
								
								// print XID data
								memset(wcBuffer,0x00,sizeof(wcBuffer));
								pTemp = (BYTE *) &pDevExt->XidDescriptor;
								MakeWStringOfBytes(pTemp,sizeof(XID_DESCRIPTOR),wcBuffer,sizeof(wcBuffer));
								LineInc += 20;
								drPrintf(50,BaseY+LineInc,L"XID Desc:");
								drPrintf(180,BaseY+LineInc,wcBuffer,0);

								// print Capabilities
								memset(wcBuffer,0x00,sizeof(wcBuffer));
								pTemp = (BYTE *) pDevExt->bCapsInBuff /*&pDevExt->InputCaps.In*/;
								MakeWStringOfBytes(pTemp,sizeof(XINPUT_GAMEPAD)+2,wcBuffer,sizeof(wcBuffer));
								LineInc += 20;
								drPrintf(50,BaseY+LineInc,L"Input Caps:");
								drPrintf(180,BaseY+LineInc,wcBuffer);

								memset(wcBuffer,0x00,sizeof(wcBuffer));
								pTemp = (BYTE *) pDevExt->bCapsOutBuff /*&pDevExt->InputCaps.Out */;
								MakeWStringOfBytes(pTemp,sizeof(XINPUT_RUMBLE)+2,wcBuffer,sizeof(wcBuffer));
								LineInc += 20;
								drPrintf(50,BaseY+LineInc,L"Output Caps:");
								drPrintf(180,BaseY+LineInc,wcBuffer);

							} else if(slot)
							{
								// got other than controller
								swprintf(wcBuffer,L"Port %d - Slot %d",port,slot);
								BaseY = (float) 100 + (150 * iDeviceCount);
								drPrintf(50,BaseY,wcBuffer);
								iDeviceCount++;
							
								memset(wcBuffer,0x00,sizeof(wcBuffer));
								if(pDevDesc->bLength == 0xff) // failure signal
								{
									swprintf(wcBuffer,L"Device Desc:  Failure! Status = %x",*(ULONG *)&pDevDesc->bcdUSB);
								} else
								{
									swprintf(wcBuffer,
										L"Device Desc: %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
										pDevDesc->bLength,
										pDevDesc->bDescriptorType,
										*(PBYTE)(&pDevDesc->bcdUSB),
										*((PBYTE)(&pDevDesc->bcdUSB)+1),
										pDevDesc->bDeviceClass,
										pDevDesc->bDeviceSubClass,
										pDevDesc->bDeviceProtocol,
										pDevDesc->bMaxPacketSize0,
										*(PBYTE)(&pDevDesc->idVendor),
										*((PBYTE)(&pDevDesc->idVendor)+1),
										*(PBYTE)(&pDevDesc->idProduct),
										*((PBYTE)(&pDevDesc->idProduct)+1),
										*(PBYTE)(&pDevDesc->bcdDevice),
										*((PBYTE)(&pDevDesc->bcdDevice)+1),
										pDevDesc->iManufacturer,
										pDevDesc->iProduct,
										pDevDesc->iSerialNumber,
										pDevDesc->bNumConfigurations
										);
								}


								drPrintf(50,BaseY + 20,wcBuffer);

								// display configuration descriptors
								// get a pointer to the descriptor data
								pCfgDesc = &pDevExt->ConfigurationDescriptor; 
								int iCfgLen = pCfgDesc->wTotalLength; // get the total length
								// new logic starts here
								memset(wcBuffer,0x00,sizeof(wcBuffer));
								MakeWStringOfBytes((BYTE *) pDevExt->bConfigData,iCfgLen,wcBuffer,sizeof(wcBuffer));
								Spacify(wcBuffer);  // add spaces between descriptors
								// now display bytes in WcBuffer 32 chars at a time
								int iChars = wcslen(wcBuffer);
								int iNumLines = iChars / 32;
								int LineInc = 40;
								drPrintf(50,BaseY+LineInc,L"Config Data:",0);
								WCHAR wcLineBuffer[50];
								WCHAR * pCurPtr = wcBuffer;
								for(int i = 0; i < iNumLines; i++)
								{
									memset(wcLineBuffer,0x00,sizeof(wcLineBuffer));
									memcpy(wcLineBuffer,pCurPtr,64);
									pCurPtr +=32;
									drPrintf(180,BaseY+LineInc,wcLineBuffer,0);
									LineInc += 20;
								}
								// do partial last line
								int iRem = iChars % 32;
								if(iRem)
								{
									memset(wcLineBuffer,0x00,sizeof(wcLineBuffer));
									memcpy(wcLineBuffer,pCurPtr,iRem*2);
									drPrintf(180,BaseY+LineInc,wcLineBuffer,0);
								}
													
							} // if slot
						} // if nodes
					}  // for interface
				} // for slot
			} // for port
		} // if driverextension
		drShowScreen();
		Sleep(100);
	} // while
	SetEvent(pApp->m_hSynchRender); // inform main loop we're done
    return S_OK;
}


