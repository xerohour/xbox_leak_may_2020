/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    usbd.c

Abstract:

    USBD System Driver Main Entry Points
    and Initialization

Environment:

    XBOX kernel mode only

Notes:

Revision History:

    12-28-99 created by Mitchell Dernis (mitchd)

--*/


//
//  Pull in OS headers
//
extern "C" {
#include <ntos.h>
}
#include <ntrtl.h>
#include <nturtl.h>
#include <xtl.h>
#include <xapidrv.h>

//
//  Setup the debug information for this file (see ..\inc\debug.h)
//
#define MODULE_POOL_TAG          'DBSU'
#include <debug.h>
DEFINE_USB_DEBUG_FUNCTIONS("USBD");


#pragma data_seg(".XPP$ClassA")
extern "C" DWORD USBD_BeginClassDescriptionTable = 0;
#pragma data_seg(".XPP$ClassZ")
extern "C" DWORD USBD_EndClassDescriptionTable = 0;
#pragma data_seg(".XPP$Data")

//  Tells the linker to merge the USB sections together
#pragma comment(linker, "/merge:.XPPCODE=XPP")
#pragma comment(linker, "/merge:.XPPRDATA=XPP")
#pragma comment(linker, "/merge:.XPPCINIT=XPP")
#pragma comment(linker, "/merge:.XPPRINIT=XPP")
#pragma comment(linker, "/merge:.XPP=XPP")
#pragma comment(linker, "/merge:.XPPDINIT=XPP")

// Tell the linker that the code section contains read/write data.
#pragma comment(linker, "/section:XPP,ERW")

//
//  Pull in usbd headers
//
#include <usbd.h>


//----------------------------------------------------------------------------------
// Globals - all of which must be initialized in DriverEntry
//----------------------------------------------------------------------------------
UCHAR  GLOBAL_HostControllerCount;

//----------------------------------------------------------------------------------------------
//  Just a simple inline new and matching delete that uses RTL_ALLOCATE_HEAP
//----------------------------------------------------------------------------------------------
#pragma warning(disable:4211)
static __inline void * __cdecl operator new(size_t memSize){return RTL_ALLOCATE_HEAP(memSize);}
static __inline void __cdecl operator delete(void *buffer){RTL_FREE_HEAP(buffer);}
#pragma warning(default:4211)

//----------------------------------------------------------------------------------
//  Top level entry points called by the I\O Manager
//----------------------------------------------------------------------------------
#pragma code_seg(".XPPCINIT")
EXTERNUSB VOID USBD_Init(DWORD NumDeviceTypes, PXDEVICE_PREALLOC_TYPE DeviceTypes) 
/*++

Routine Description:
    
    Standard DriverEntry Routine - Let's libraries initialize globals,
    then fills out dispatch table and returns success.

Arguments:
    
    DriverObject - Pointer to DriverObject

    RegistryPath - Pointer to RegistryPath

Return Value:

    None.

--*/
{
    USB_DBG_ENTRY_PRINT(("Entering USBD_Init"));
    USB_DBG_TRACE_PRINT(("Built on %s @%s", __DATE__,__TIME__));

    IUsbInit *pUsbInit = new IUsbInit(NumDeviceTypes, DeviceTypes);

    //
    //  Walk the class drivers and call their init functions
    //
    PUSB_CLASS_DRIVER_DESCRIPTION *ppClassDriverDesc;
    ppClassDriverDesc = (PUSB_CLASS_DRIVER_DESCRIPTION *)((&USBD_BeginClassDescriptionTable)+1);
    while( (ULONG_PTR)ppClassDriverDesc < (ULONG_PTR)&USBD_EndClassDescriptionTable  )
    {
        if(*ppClassDriverDesc) (*ppClassDriverDesc)->Init(pUsbInit);
        ppClassDriverDesc++;
    }

    pUsbInit->Process();
    
    //
    //  Give the HCD a chance to initialize its globals.
    //
    HCD_DriverEntry(pUsbInit->GetHcdResourcePtr());

    //
    // Initialize the device tree
    // 
    g_DeviceTree.Init(pUsbInit->GetNodeCount(), pUsbInit->GetMaxCompositeInterfaces());

    //
    //  We are done with the IUsbInit object
    //
    delete pUsbInit;

    GLOBAL_HostControllerCount=0;
    //
    //  Call the HCD layer to enumerate the hardware.
    //
    HCD_EnumHardware();
    USB_DBG_EXIT_PRINT(("Exiting USBD_Init"));
}

VOID
USBD_NewHostController(
    IN PPCI_DEVICE_DESCRIPTOR PciDevice,
    IN ULONG HcdDeviceExtensionSize
    )
/*++

Routine Description:
    
    Called by the HCD_EnumHardware for every host controller
    it finds.

    We do the following:

    a1. Create a device. (Actually, we need only the extensions not the device objects)
    2. Initialize the USBD portion of the of the device
    3. Call HCD_NewHostController to initialize the hardware portion.

Arguments:
    
    PciDevice - PCI descriptor to pass back to the hardware initialization later.
    HcdDeviceExtensionSize - Size of HCD portion of host controller context.
   
Return Value:

    None - what good does it do to annouce failure anyway.

--*/
{
    PUSBD_HOST_CONTROLLER  hostController;

    USB_DBG_ENTRY_PRINT(("Entering USBD_NewHostController"));

    //
    //  Allocate the Host Controller
    //
    hostController = (PUSBD_HOST_CONTROLLER)RTL_ALLOCATE_HEAP(sizeof(USBD_HOST_CONTROLLER) + HcdDeviceExtensionSize);
    
    if(hostController)
    {
        //
        //  Zero out the whole memory we just allocated.  We do this for our extension
        //  and for the HCD's.
        //
        RtlZeroMemory(hostController, sizeof(USBD_HOST_CONTROLLER) + HcdDeviceExtensionSize);

        //
        //  We need to number our controllers, so the game can distinguish
        //
        hostController->ControllerNumber = ++GLOBAL_HostControllerCount;

        USB_DBG_TRACE_PRINT(("Host Controller #%d Found:", hostController->ControllerNumber));
        USB_DBG_TRACE_PRINT(("HCD extension = 0x%0.8x:", USBD_GetHCDExtension(hostController)));
        
        //
        //  Add a RootHub device for this host controller.
        //
        hostController->RootHub = g_DeviceTree.AllocDevice();
        ASSERT(hostController->RootHub);
        hostController->RootHub->m_Type = UDN_TYPE_ROOT_HUB;
        hostController->RootHub->m_FirstChild = UDN_INVALID_NODE_INDEX;
        hostController->RootHub->m_Parent = UDN_INVALID_NODE_INDEX;
        hostController->RootHub->m_Sibling = UDN_INVALID_NODE_INDEX;
		hostController->RootHub->m_HostController = hostController;
        
        //
        //  Initialize the hardware
        //
        HCD_NewHostController(USBD_GetHCDExtension(hostController), hostController->ControllerNumber, PciDevice);
    }
    USB_DBG_EXIT_PRINT(("Exiting USBD_NewHostController"));
    return;
}
#pragma code_seg(".XPPCODE")

VOID
USBD_CompleteRequest(
    IN  PURB Urb
    )
/*++

Routine Description:

    Entry point called by HCD (and a few places in USBD) to complete an URB.
    Basically, it calls the completion routine if there is one.
    
Arguments:
    
    Urb - The URB that was just completed.

Return Value:

--*/
{
    //
    //  If there is a completion routine than call it.
    //
    if(Urb->Header.CompleteProc)
    {
        USB_DBG_TRACE_PRINT(("Calling the completion routine @0x%0.8x for URB @0x%0.8x.",
                    Urb->Header.CompleteProc, Urb));
        Urb->Header.CompleteProc(Urb, Urb->Header.CompleteContext);
    }
}


USHORT
USBD_CalculateUsbBandwidth(
    IN USHORT   MaxPacketSize,
    IN UCHAR    EndpointType,
    IN BOOLEAN  LowSpeed
    )
/*++

Routine Description:

Arguments:

Return Value:

    banwidth consumed in bits/ms, returns 0 for bulk
    and control endpoints

--*/
{
    USHORT bw;

    //
    // control, iso, bulk, interrupt
    //

    ULONG overhead[4] = {
            0,
            USB_ISO_OVERHEAD_BYTES,
            0,
            USB_INTERRUPT_OVERHEAD_BYTES
          };

    ASSERT(EndpointType<4);

    //
    // Calculate bandwidth for endpoint.  We will use the
    // approximation: (overhead bytes plus MaxPacket bytes)
    // times 8 bits/byte times worst case bitstuffing overhead.
    // This gives bit times, for low speed endpoints we multiply
    // by 8 again to convert to full speed bits.
    //

    //
    // Figure out how many bits are required for the transfer.
    // (multiply by 7/6 because, in the worst case you might
    // have a bit-stuff every six bits requiring 7 bit times to
    // transmit 6 bits of data.)
    //

    // overhead(bytes) * maxpacket(bytes/ms) * 8
    //      (bits/byte) * bitstuff(7/6) = bits/ms

    bw = (USHORT)(((overhead[EndpointType]+MaxPacketSize) * 8 * 7) / 6);

    // return zero for control or bulk
    if (!overhead[EndpointType]) {
        bw = 0;
    }

    if (LowSpeed) {
        bw *= 8;
    }

    return bw;
}

PUSB_CLASS_DRIVER_DESCRIPTION USBD_FindClassDriver(PNP_CLASS_ID ClassId)
{
	PUSB_CLASS_DRIVER_DESCRIPTION *ppMatch;

    //
	//	Match only the class and class specific type.
	//  The class specific type has a slightly different meaning here.
    //  In this context ir is whether the device is interface level or
    //  device level.  Later the class driver will set it to be something
    //  which is really class specific.
    //
	ppMatch = (PUSB_CLASS_DRIVER_DESCRIPTION *)((&USBD_BeginClassDescriptionTable)+1);
    while( (ULONG_PTR)ppMatch < (ULONG_PTR)&USBD_EndClassDescriptionTable)
	{
		if(
            *ppMatch && 
            (ClassId.USB.bClass == (*ppMatch)->ClassId.USB.bClass) &&
            (ClassId.USB.bClassSpecificType == (*ppMatch)->ClassId.USB.bClassSpecificType)
        )
        {
            goto found_match;
        }
		ppMatch++;
	}
    return NULL;

found_match:

    return (*ppMatch);
}

