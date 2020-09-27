/*++

Copyright (c) 2000 Microsoft Corporation

Module Name: 

    usbkdx.cpp

Abstract:

    Debugger extension for the XBox USB stack

Author:

    Josh Poley (jpoley)

Revision History:


USB Globals:
    ohcd\isr.c
        int IsrEntryCount=0;

    xid\xid.cpp
        XID_GLOBALS XID_Globals;
        USB_PNP_CLASS_DRIVER_DESCRIPTION GLOBAL_ClassDescription
        USB_PNP_DEVICE_TYPE_DESCRIPTION GLOBAL_DeviceTypes[XINPUT_DEVTYPE_COUNT];

    usbd\usbd.cpp
        UCHAR                    GLOBAL_HostControllerCount;
        PDRIVER_OBJECT           GLOBAL_DriverObject;
        XAPI_NOTIFICATION_SOURCE GLOBAL_NotificationSource;

    usbd\tree.cpp
        CDeviceTree g_DeviceTree;

    usbd\notify.cpp
        USBD_NOTIFICATIONS g_Notifications={0};

    usbpnp\pnp.cpp
        PFNUSBHUB_ResetHubPort				g_ResetRoutine = NULL;
        PUSB_PNP_CLASS_DRIVER_DESCRIPTION	g_FirstClassDriver = NULL;
        BOOLEAN								g_BaseDriverCalledYet = FALSE;

    usbhub\hub.cpp
        USBHUB_INFO						 GLOBAL_HubInfo;
        USBHUB_ENUM_BLOCK				 GLOBAL_HubEnum;
        USB_PNP_CLASS_DRIVER_DESCRIPTION GLOBAL_ClassDescription;

--*/

#define KDEXTMODE

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <ntos.h>
#include <zwapi.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <imagehlp.h>

#include <memory.h>
#pragma hdrstop

#include <wdbgexts.h>
extern WINDBG_EXTENSION_APIS ExtensionApis;

#include <ntverp.h>
#include <imagehlp.h>

//
// include the necessary USB headers
//
#include "usb.h"
#include "urb.h"
//#include <xusbenum.h>
#include <xinput.h>

#include "xid.h"
#include "hub.h"
#include "usbd.h"


//
//  Globals
//
EXT_API_VERSION apiVersion = {
    (VER_PRODUCTVERSION_W >> 8),
    (VER_PRODUCTVERSION_W & 0xff),
    EXT_API_VERSION_NUMBER,
    0
    };

WINDBG_EXTENSION_APIS   ExtensionApis;
USHORT                  SavedMajorVersion;
USHORT                  SavedMinorVersion;

PSTR apiDescriptions =
    "\nUSB Debugging Extensions:\n\n"
    "help - show this help text\n"

    "device - displays a CUsbDevice from the CDeviceTree class\n"
    "    !usbkdx.device <&g_DeviceTree> <index>\n"
    "    dd g_DeviceTree l1\n"

    "hubclass - displays the global USB_PNP_CLASS_DRIVER_DESCRIPTION struct\n"
    "    !usbkdx.hubclass <&GLOBAL_ClassDescription>\n"
    "    dd GLOBAL_ClassDescription l1\n"

    "hubenum - displays the global USBHUB_ENUM_BLOCK struct\n"
    "    !usbkdx.hubenum <&GLOBAL_HubEnum>\n"
    "    dd GLOBAL_HubEnum l1\n"

    "hubinfo - displays the global USBHUB_INFO struct\n"
    "    !usbkdx.hubinfo <&GLOBAL_HubInfo>\n"
    "    dd GLOBAL_HubInfo l1\n"

    "tree - displays the global CDeviceTree class\n"
    "    !usbkdx.tree <&g_DeviceTree>\n"
    "    dd g_DeviceTree l1\n"

    "xidglobals - displays the global XID_GLOBALS struct\n"
    "    !usbkdx.xidglobals <&XID_Globals>\n"
    "    dd XID_Globals l1\n"
    "\n";


void PrintHexDump(const void* buffer, DWORD length);


//
//  DLLMain
//
BOOL WINAPI DLLMain(HINSTANCE hInstance, ULONG ulReason, LPVOID pvReserved)
    {
    switch(ulReason)
        {
        case DLL_PROCESS_ATTACH:
        case DLL_PROCESS_DETACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
        }

    return(TRUE);
    }


/*****************************************************************************

Routine Description:

    help

    Describes the extensions available from this library

Arguments:

    HANDLE hCurrentProcess
    HANDLE hCurrentThread
    ULONG  dwCurrentPc
    ULONG  dwProcessor
    PCSTR  args

Return Value:

    none

Notes:

    This function is ONLY called via the debugger

*****************************************************************************/
DECLARE_API(help)
    {
    dprintf(apiDescriptions);
    return;
    } // help


/*****************************************************************************

Routine Description:

    xidglobals

    Dumps out the values of the global XID_GLOBALS struct

Arguments:

    HANDLE hCurrentProcess
    HANDLE hCurrentThread
    ULONG  dwCurrentPc
    ULONG  dwProcessor
    PCSTR  args

Return Value:

    none

Notes:

    This function is ONLY called via the debugger

*****************************************************************************/
DECLARE_API(xidglobals)
    {
    XID_GLOBALS *globals;
    ULONG address;

    if(0 == args[0])
        {
        dprintf("xidglobals <address>\n");
        return;
        }

    sscanf(args, "%lx", &address);
    globals = (XID_GLOBALS *)address;

    dprintf("\nXID_GLOBALS:\n");
    dprintf("    DeviceNodeCount:       %u\n", globals->DeviceNodeCount);
    dprintf("    DeviceNodeInUseCount:  %u\n", globals->DeviceNodeInUseCount);
    dprintf("    DeviceNodes:           0x%X\n", globals->DeviceNodes);
    dprintf("    OpenDeviceList:        0x%X\n", globals->OpenDeviceList);
    dprintf("    EnumNode:              %u\n", globals->EnumNode);
    dprintf("    EnumUrb.Header:\n");
    dprintf("        Length:            %u\n", globals->EnumUrb.Header.Length);
    dprintf("        Function:          %u\n", globals->EnumUrb.Header.Function);
    dprintf("        Status:            0x%X\n", globals->EnumUrb.Header.Status);
    dprintf("        CompleteProc:      0x%X\n", globals->EnumUrb.Header.CompleteProc);
    dprintf("        CompleteContext:   0x%X\n", globals->EnumUrb.Header.CompleteContext);

    //union
    //{
    //    XID_DESCRIPTOR          EnumXidDescriptor;
    //    USB_DEVICE_DESCRIPTOR   EnumDeviceDescriptor;
    //};
    //UCHAR                       EnumMdlBuffer[sizeof(MDL)+sizeof(PFN_NUMBER)*2];
    //KTIMER                      EnumWatchdogTimer;
    //KDPC                        EnumWatchdogTimerDpc;

    return;
    } // xidglobals


/*****************************************************************************

Routine Description:

    hubinfo

    Dumps out the values of the global USBHUB_INFO struct

Arguments:

    HANDLE hCurrentProcess
    HANDLE hCurrentThread
    ULONG  dwCurrentPc
    ULONG  dwProcessor
    PCSTR  args

Return Value:

    none

Notes:

    This function is ONLY called via the debugger

*****************************************************************************/
DECLARE_API(hubinfo)
    {
    USBHUB_INFO *globals;
    ULONG address;

    if(0 == args[0])
        {
        dprintf("hubinfo <address>\n");
        return;
        }

    sscanf(args, "%lx", &address);
    globals = (USBHUB_INFO *)address;

    dprintf("\nUSBHUB_INFO:\n");
    dprintf("    NodeCount:             %u\n", globals->NodeCount);
    dprintf("    NodesInUse:            %u\n", globals->NodesInUse);
    dprintf("    EndpointMemory:        0x%X\n", globals->EndpointMemory);
    dprintf("    Nodes:\n");
    for(unsigned i=0; i<USBHUB_MAXIMUM_HUB_COUNT; i++)
        {
        dprintf("      %02d InUse:                  %u\n", i, globals->Nodes[i].InUse);
        dprintf("         OutstandingRequest:     %u\n", globals->Nodes[i].OutstandingRequest);
        dprintf("         RemovePending:          %u\n", globals->Nodes[i].RemovePending);
        dprintf("         PoweringPorts:          %u\n", globals->Nodes[i].PoweringPorts);
        dprintf("         UsbNodeIndex:           %u\n", globals->Nodes[i].UsbNodeIndex);
        dprintf("         PortCount:              %u\n", globals->Nodes[i].PortCount);
        dprintf("         PortProcessIndex:       %u\n", globals->Nodes[i].PortProcessIndex);
        dprintf("         PortStatusChangeBitmap: %u\n", globals->Nodes[i].PortStatusChangeBitmap);
        dprintf("         PortConnectedBitmap:    %u\n", globals->Nodes[i].PortConnectedBitmap);
        dprintf("         InterruptEndpointHandle:0x%X\n", globals->Nodes[i].InterruptEndpointHandle);
        //URB				Urb;
        //union
        //{
        //    UCHAR			RequestBuffer[4];
        //    struct {
        //	    USHORT		Status;
        //	    USHORT		StatusChange;
        //    }				PortStatus;
        //};
	    //UCHAR			MdlBuffer[sizeof(MDL)+sizeof(PFN_NUMBER)*2];
        }

    return;
    } // hubinfo


/*****************************************************************************

Routine Description:

    hubenum

    Dumps out the values of the global USBHUB_ENUM_BLOCK struct

Arguments:

    HANDLE hCurrentProcess
    HANDLE hCurrentThread
    ULONG  dwCurrentPc
    ULONG  dwProcessor
    PCSTR  args

Return Value:

    none

Notes:

    This function is ONLY called via the debugger

*****************************************************************************/
DECLARE_API(hubenum)
    {
    USBHUB_ENUM_BLOCK *globals;
    ULONG address;

    if(0 == args[0])
        {
        dprintf("hubenum <address>\n");
        return;
        }

    sscanf(args, "%lx", &address);
    globals = (USBHUB_ENUM_BLOCK*)address;

    dprintf("\nUSBHUB_ENUM_BLOCK:\n");
    dprintf("    EnumBuffer:\n");
    PrintHexDump(globals->EnumBuffer, 32);
    dprintf("    ResetUrb.Header:\n");
    dprintf("        Length:            %u\n", globals->ResetUrb.Header.Length);
    dprintf("        Function:          %u\n", globals->ResetUrb.Header.Function);
    dprintf("        Status:            0x%X\n", globals->ResetUrb.Header.Status);
    dprintf("        CompleteProc:      0x%X\n", globals->ResetUrb.Header.CompleteProc);
    dprintf("        CompleteContext:   0x%X\n", globals->ResetUrb.Header.CompleteContext);
    dprintf("    ResetContext:          %u\n", globals->ResetContext);

    //UCHAR						MdlBuffer[sizeof(MDL)+sizeof(PFN_NUMBER)*2];
    //KTIMER                      WatchdogTimer;
    //KDPC                        WatchdogTimerDpc;

    return;
    } // hubenum

/*****************************************************************************

Routine Description:

    hubclass

    Dumps out the values of the global USB_PNP_CLASS_DRIVER_DESCRIPTION struct

Arguments:

    HANDLE hCurrentProcess
    HANDLE hCurrentThread
    ULONG  dwCurrentPc
    ULONG  dwProcessor
    PCSTR  args

Return Value:

    none

Notes:

    This function is ONLY called via the debugger

*****************************************************************************/
DECLARE_API(hubclass)
    {
    USB_PNP_CLASS_DRIVER_DESCRIPTION *globals;
    ULONG address;

    if(0 == args[0])
        {
        dprintf("hubclass <address>\n");
        return;
        }

    sscanf(args, "%lx", &address);
    globals = (USB_PNP_CLASS_DRIVER_DESCRIPTION *)address;

    dprintf("\nUSB_PNP_CLASS_DRIVER_DESCRIPTION:\n");
    dprintf("    ClassId:\n");
    dprintf("        AsLong:            %u\n", globals->ClassId.AsLong);
    dprintf("        bClassSpecificType:%u\n", globals->ClassId.USB.bClassSpecificType);
    dprintf("        bClass:            %u\n", globals->ClassId.USB.bClass);
    dprintf("        bSubClass:         %u\n", globals->ClassId.USB.bSubClass);
    dprintf("        bProtocol:         %u\n", globals->ClassId.USB.bProtocol);
    dprintf("    Context:               0x%X\n", globals->Context);
    dprintf("    Reserved:              0x%X\n", globals->Reserved);
    dprintf("    DeviceTypeCount:       %u\n", globals->DeviceTypeCount);

/*
    PFNADD_USB_DEVICE                AddDevice;
    PFNREMOVE_USB_DEVICE             RemoveDevice;
    PUSB_PNP_DEVICE_TYPE_DESCRIPTION DeviceTypes;
*/
    return;
    } // hubclass


/*****************************************************************************

Routine Description:

    tree

    Dumps out the values of the global CDeviceTree class

Arguments:

    HANDLE hCurrentProcess
    HANDLE hCurrentThread
    ULONG  dwCurrentPc
    ULONG  dwProcessor
    PCSTR  args

Return Value:

    none

Notes:

    This function is ONLY called via the debugger

*****************************************************************************/
DECLARE_API(tree)
    {
    CDeviceTree *globals;
    ULONG address;

    if(0 == args[0])
        {
        dprintf("tree <address>\n");
        return;
        }

    sscanf(args, "%lx", &address);
    globals = (CDeviceTree *)address;

    dprintf("\nCDeviceTree:\n");
    dprintf("    m_InProgress:          %u\n", globals->m_InProgress);
    dprintf("    m_DeviceRemoved:       %u\n", globals->m_DeviceRemoved);
    dprintf("    m_EnumUrb.Header:\n");
    dprintf("        Length:            %u\n", globals->m_EnumUrb.Header.Length);
    dprintf("        Function:          %u\n", globals->m_EnumUrb.Header.Function);
    dprintf("        Status:            0x%X\n", globals->m_EnumUrb.Header.Status);
    dprintf("        CompleteProc:      0x%X\n", globals->m_EnumUrb.Header.CompleteProc);
    dprintf("        CompleteContext:   0x%X\n", globals->m_EnumUrb.Header.CompleteContext);
    dprintf("    m_TimerReason:         %u\n", globals->m_TimerReason);
    dprintf("    m_FirstFree:           %u\n", globals->m_FirstFree);
    dprintf("    m_Topology:            %02X %02X %02X %02X\n", globals->m_Topology[0], globals->m_Topology[1], globals->m_Topology[2], globals->m_Topology[3]);
    dprintf("    m_DeviceDescriptor:\n");
    dprintf("        bLength:           %u\n", globals->m_DeviceDescriptor.bLength);
    dprintf("        bDescriptorType:   %u\n", globals->m_DeviceDescriptor.bDescriptorType);
    dprintf("        bcdUSB:            %u\n", globals->m_DeviceDescriptor.bcdUSB);
    dprintf("        bDeviceClass:      %u\n", globals->m_DeviceDescriptor.bDeviceClass);
    dprintf("        bDeviceSubClass:   %u\n", globals->m_DeviceDescriptor.bDeviceSubClass);
    dprintf("        bDeviceProtocol:   %u\n", globals->m_DeviceDescriptor.bDeviceProtocol);
    dprintf("        bMaxPacketSize0:   %u\n", globals->m_DeviceDescriptor.bMaxPacketSize0);
    if(globals->m_InterfaceDescriptor)
        {
        dprintf("    m_InterfaceDescriptor:\n");
        dprintf("        bLength:           %u\n", globals->m_InterfaceDescriptor->bLength);
        dprintf("        bDescriptorType:   %u\n", globals->m_InterfaceDescriptor->bDescriptorType);
        dprintf("        bInterfaceNumber:  %u\n", globals->m_InterfaceDescriptor->bInterfaceNumber);
        dprintf("        bAlternateSetting: %u\n", globals->m_InterfaceDescriptor->bAlternateSetting);
        dprintf("        bNumEndpoints:     %u\n", globals->m_InterfaceDescriptor->bNumEndpoints);
        dprintf("        bInterfaceClass:   %u\n", globals->m_InterfaceDescriptor->bInterfaceClass);
        dprintf("        bInterfaceSubClass:%u\n", globals->m_InterfaceDescriptor->bInterfaceSubClass);
        dprintf("        bInterfaceProtocol:%u\n", globals->m_InterfaceDescriptor->bInterfaceProtocol);
        dprintf("        iInterface:        %u\n", globals->m_InterfaceDescriptor->iInterface);
        }

/*
    UCHAR                       m_MdlBuffer[sizeof(MDL)+sizeof(PFN_NUMBER)*2];
    KDPC                        m_EnumDpc;
    KTIMER                      m_EnumTimer;
    CUsbDevice                  *m_FirstPendingEnum;
    CUsbDevice                  *m_CurrentEnum;
    UCHAR                       m_ConfigurationDescriptorBuffer[USBD_MAX_CONFIG_DESC_SIZE];
    CUsbDevice                  m_Devices[UDN_MAX_NODES];
*/

    return;
    } // tree


/*****************************************************************************

Routine Description:

    device

    Dumps out the values of a CUsbDevice from the global CDeviceTree class

Arguments:

    HANDLE hCurrentProcess
    HANDLE hCurrentThread
    ULONG  dwCurrentPc
    ULONG  dwProcessor
    PCSTR  args

Return Value:

    none

Notes:

    This function is ONLY called via the debugger

*****************************************************************************/
DECLARE_API(device)
    {
    CDeviceTree *globals;
    ULONG address;
    unsigned index;

    if(0 == args[0])
        {
        dprintf("device <address> <index>\n");
        return;
        }

    sscanf(args, "%lx %u", &address, &index);
    globals = (CDeviceTree *)address;

    dprintf("\nCDeviceTree.CUsbDevice[%u]:\n", index);
    dprintf("    m_Type:                %u\n", globals->m_Devices[index].m_Type);
    dprintf("    m_NextFree:            %u\n", globals->m_Devices[index].m_NextFree);
    dprintf("    m_bInterfaceNumber:    %u\n", globals->m_Devices[index].m_bInterfaceNumber);
    dprintf("    m_Sibling:             %u\n", globals->m_Devices[index].m_Sibling);
    dprintf("    m_PortNumber:          %u\n", globals->m_Devices[index].m_PortNumber);
    dprintf("    m_Address:             %u\n", globals->m_Devices[index].m_Address);
    dprintf("    m_MaxPacket0:          %u\n", globals->m_Devices[index].m_MaxPacket0);
    dprintf("    m_ClassSpecificType:   %u\n", globals->m_Devices[index].m_ClassSpecificType);
    dprintf("    m_DefaultEndpoint:     0x%X\n", globals->m_Devices[index].m_DefaultEndpoint);
    if(globals->m_Devices[index].m_HostController)
        {
        dprintf("    m_HostController:\n");
        dprintf("        ControllerNumber:  %u\n", globals->m_Devices[index].m_HostController->ControllerNumber);
        dprintf("        RootHub:           0x%X\n", globals->m_Devices[index].m_HostController->RootHub);
        dprintf("        AddressList:       %X %X %X %X\n", globals->m_Devices[index].m_HostController->AddressList[0], globals->m_Devices[index].m_HostController->AddressList[1], globals->m_Devices[index].m_HostController->AddressList[2], globals->m_Devices[index].m_HostController->AddressList[3]);
        dprintf("        HcdExtension:      0x%X\n", &globals->m_Devices[index].m_HostController->HcdExtension);
        }
    dprintf("    m_ClassDriverExtension:0x%X\n", globals->m_Devices[index].m_ClassDriverExtension);
    dprintf("    m_ExternalPort:         %u\n", globals->m_Devices[index].m_ExternalPort);

    return;
    } // device

void WinDbgExtensionDllInit(PWINDBG_EXTENSION_APIS pExtensionApis, USHORT MajorVersion, USHORT MinorVersion)
    {
    ExtensionApis = *pExtensionApis;

    SavedMajorVersion = MajorVersion;
    SavedMinorVersion = MinorVersion;

    return;
    }

void CheckVersion(void)
    {
    }

LPEXT_API_VERSION ExtensionApiVersion(void)
    {
    return &apiVersion;
    }


/*****************************************************************************

Routine Description:

    PrintHexDump

    Outputs a hex dump of memory

Arguments:

        const void* buffer - pointer to start of the memory
        DWORD length - how many bytes to dump

Return Value:

    none

*****************************************************************************/
void PrintHexDump(const void* buffer, DWORD length)
    {
    DWORD i,count,index;
    CHAR rgbLine[100];
    char cbLine;
    const char *buff = (const char*)buffer;

    for(index = 0; length; length -= count, buff += count, index += count)
        {
        count = (length > 16) ? 16:length;

        sprintf(rgbLine, "%4.4X  ",index);
        cbLine = 6;

        for(i=0;i<count;i++)
            {
            sprintf(rgbLine+cbLine, "%02X", buff[i]);
            cbLine +=2;

            if(i == 7)
                {
                rgbLine[cbLine++] = ' ';
                }
            else
                {
                rgbLine[cbLine++] = ' ';
                }
            }
        for(; i < 16; i++)
            {
            rgbLine[cbLine++] = ' ';
            rgbLine[cbLine++] = ' ';
            rgbLine[cbLine++] = ' ';
            }

        rgbLine[cbLine++] = ' ';

        for(i = 0; i < count; i++)
            {
            if(buff[i] < 32 || buff[i] > 126)
                {
                rgbLine[cbLine++] = '.';
                }
            else
                {
                rgbLine[cbLine++] = buff[i];
                }
            }

        rgbLine[cbLine++] = 0;
        dprintf("    %s\n", rgbLine);
        }

    return;
    }
