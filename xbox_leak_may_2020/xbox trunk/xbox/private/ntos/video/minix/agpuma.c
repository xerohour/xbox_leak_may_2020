/*++

Copyright (c) 1990-2000  Microsoft Corporation

Module Name:

    agpuma.c

Abstract:

    This module maps all physical memory into the AGP address space.  It is
    used by the XBOX to simulate UMA.

--*/

// Nasty path, but there are only two files that need to make kernel calls.
#include <ntos.h>
#include <pci.h>
#include <ntddvdeo.h>
#include <ntagp.h>
#include <stdio.h>

#if defined(DBG)

    #define RIP(msg)            \
        {                       \
            DbgPrint msg;     \
            DbgPrint("\n");   \
            _asm { int 3 };     \
        }

#else

    #define RIP(msg) {}

#endif

//
// Conversions from APSIZE encoding to MB
//

#define AP_SIZE_4MB     0x3F
#define AP_SIZE_8MB     0x3E
#define AP_SIZE_16MB    0x3C
#define AP_SIZE_32MB    0x38
#define AP_SIZE_64MB    0x30
#define AP_SIZE_128MB   0x20
#define AP_SIZE_256MB   0x00

//
// Define the location of the GART aperture control registers
//

#define HOST_BUS                0
#define HOST_SLOT               0

//
// Define the location of the video card
//

#define CARD_BUS                1
#define CARD_SLOT               0

//
// Defines what mode we're using.  If the hardware doesn't support this then
// we'll RIP.
//

#define AGP_APERTURE_SIZE       AP_SIZE_64MB
#define AGP_APERTURE_BYTES      (64*1024*1024)
#define AGP_APERTURE_BASE       0xF8000000

//
// Offsets for host the registers we need to poke directly.
//

#define APBASE_OFFSET   0x10            // Aperture Base Address
#define APSIZE_OFFSET   0xB4            // Aperture Size Register
#define AGPCTRL_OFFSET  0xB0            // AGP Control Register
#define ATTBASE_OFFSET  0xB8            // Aperture Translation Table Base
#define RDCR_OFFSET     0x51            // RDRAM Control register
#define AGPBCTRL_OFFSET 0xE8            // AGP buffer control register
//
// Define the layout of the hardware registers
//

typedef struct _AGPCTRL 
{
    ULONG FourXOverride:1;              // forces AGP 2x mode
    ULONG FastWriteEnable:1;            // enables fast writes
    ULONG Reserved1:5;
    ULONG EnableGART:1;                 // enables the GART
    ULONG Reserved2:24;
} AGPCTRL;

typedef struct _RDCR
{
    UCHAR Reserved1:1;
    UCHAR ApertureEnable:1;             // enable AGP aperture
    UCHAR Reserved2:4;
    UCHAR PBS:1;
    UCHAR Reserved3:1;
} RDCR;

//
// GART entry structure
//

typedef struct _GART_PTE 
{
    ULONG Valid     :  1;
    ULONG Reserved  : 11;
    ULONG Page      : 20;
} GART_PTE, *PGART_PTE;

//
// Simple wrappers to make reading/writing host register values
// a little easier.
//

#define ReadHostRegister(_buf_, _offset_, _size_)           \
                                                            \
    ReadDeviceData(HOST_BUS,                                \
                   HOST_SLOT,                               \
                   (_buf_),                                 \
                   (_offset_),                              \
                   (_size_));                       

#define WriteHostRegister(_buf_, _offset_, _size_)          \
                                                            \
    WriteDeviceData(HOST_BUS,                               \
                    HOST_SLOT,                              \
                    (_buf_),                                \
                    (_offset_),                             \
                    (_size_));                      

//============================================================================
// AGP Code
//============================================================================

//----------------------------------------------------------------------------
// Read data from a PCI device.
//
static ULONG ReadDeviceData(
    ULONG Bus,
    ULONG Slot,
    PVOID Buffer,
    ULONG Offset,
    ULONG Length
    )
{
    ULONG Transferred;

    Transferred = HalGetBusDataByOffset(PCIConfiguration,
                                        Bus,
                                        Slot,
                                        Buffer,
                                        Offset,
                                        Length);

    return (Transferred == Length);
}

//----------------------------------------------------------------------------
// Write data to a PCI device.
//
static void WriteDeviceData(
    ULONG Bus,
    ULONG Slot,
    PVOID Buffer,
    ULONG Offset,
    ULONG Length
    )
{
    ULONG Transferred;

    Transferred = HalSetBusDataByOffset(PCIConfiguration,
                                        Bus,
                                        Slot,
                                        Buffer,
                                        Offset,
                                        Length);

    ASSERT(Transferred == Length);
}

//----------------------------------------------------------------------------
// Walk an AGP device's configuration space to find a specific capability.
//
static UCHAR ReadAgpCapability(
    ULONG Bus,
    ULONG Slot,
    PCI_AGP_CAPABILITY *Capability
    )
{
    PCI_COMMON_CONFIG Header;
    UCHAR CapabilityOffset;

    //
    // Read the PCI common header to get the capabilities pointer
    //

    ReadDeviceData(Bus, Slot, &Header, 0, sizeof(PCI_COMMON_CONFIG));

    //
    // Check the Status register to see if this device supports capability lists.
    // If not, it is not an AGP-compliant device.
    //

    if ((Header.Status & PCI_STATUS_CAPABILITIES_LIST) == 0) 
    {
        RIP(("FindAgpCapability - Device does not support Capabilities list, not an AGP device"));
        return 0;
    }

    //
    // The device supports capability lists, find the AGP capabilities.
    //

    if ((Header.HeaderType & (~PCI_MULTIFUNCTION)) == PCI_BRIDGE_TYPE) 
    {
        CapabilityOffset = Header.u.type1.CapabilitiesPtr;
    } 
    else 
    {
        ASSERT((Header.HeaderType & (~PCI_MULTIFUNCTION)) == PCI_DEVICE_TYPE);
        CapabilityOffset = Header.u.type0.CapabilitiesPtr;
    }

    while (CapabilityOffset != 0) 
    {
        //
        // Read the Capability at this offset
        //

        ReadDeviceData(Bus, Slot, Capability, CapabilityOffset, sizeof(PCI_AGP_CAPABILITY));

        // Find it?
        if (Capability->Header.CapabilityID == PCI_CAPABILITY_ID_AGP) 
        {
            break;
        } 
        else 
        {
            CapabilityOffset = Capability->Header.Next;
        }
    }

    if (CapabilityOffset == 0) 
    {
        // No AGP capability was found
        RIP(("FindAgpCapability - Device does not have an AGP Capability entry, not an AGP device"));
        return 0;
    }

    return CapabilityOffset;
}

//----------------------------------------------------------------------------
// Write a capability to a device.
//
static void WriteAgpCapability(
    ULONG Bus,
    ULONG Slot,
    UCHAR CapabilityOffset,
    PCI_AGP_CAPABILITY *Capability
    )
{
    // Write the new information.
    WriteDeviceData(Bus, 
                    Slot, 
                    Capability, 
                    CapabilityOffset, 
                    sizeof(PCI_AGP_CAPABILITY));
}

//----------------------------------------------------------------------------
// Set up the hardware to talk to each other.
//
static void SetupAgp()
{
    AGPCTRL agpctrl;

    PCI_AGP_CAPABILITY HostCap;
    PCI_AGP_CAPABILITY CardCap;    

    UCHAR HostCapOffset;
    UCHAR CardCapOffset;

    ULONG DataRate;
    
    //
    // Initialize the host to support fast writes.
    //

    ReadHostRegister(&agpctrl, AGPCTRL_OFFSET, sizeof(agpctrl));

    agpctrl.FastWriteEnable = 1;

    WriteHostRegister(&agpctrl, AGPCTRL_OFFSET, sizeof(agpctrl));

    //
    // Get the capabilities of both the host and the card.
    //

    HostCapOffset = ReadAgpCapability(HOST_BUS, HOST_SLOT, &HostCap);
    CardCapOffset = ReadAgpCapability(CARD_BUS, CARD_SLOT, &CardCap);

    //
    // Check the data rate.
    //

    DataRate = HostCap.AGPStatus.Rate & CardCap.AGPStatus.Rate;

    if (!(DataRate & PCI_AGP_RATE_4X))
    {
        RIP(("SetupAgp   - Mainboard/graphic card does not support 4x AGP."));
    }

    // 
    // The current graphics card does not support sideband addressing.  Note if 
    // this changes.
    //
    if (HostCap.AGPStatus.SideBandAddressing & CardCap.AGPStatus.SideBandAddressing)
    {
        RIP(("SetupAgp   - Sideband addressing is now available."));
    }

    // 
    // Note if fast writes go away.
    //

    if (!(HostCap.AGPStatus.FastWrite & CardCap.AGPStatus.FastWrite))
    {
        RIP(("SetupAgp   - Fast write are not available."));
    }

    //
    // Enable the host first.
    //

    HostCap.AGPCommand.Rate = PCI_AGP_RATE_4X;
    HostCap.AGPCommand.AGPEnable = 1;
    HostCap.AGPCommand.SBAEnable = 0;
    HostCap.AGPCommand.FastWriteEnable = 1;
    HostCap.AGPCommand.FourGBEnable = 0;  

    WriteAgpCapability(HOST_BUS, HOST_SLOT, HostCapOffset, &HostCap);

    //
    // Now enable the card
    //

    CardCap.AGPCommand.Rate = PCI_AGP_RATE_4X;
    CardCap.AGPCommand.AGPEnable = 1;
    CardCap.AGPCommand.SBAEnable = 0;
    CardCap.AGPCommand.FastWriteEnable = 1;
    CardCap.AGPCommand.FourGBEnable = 0;  
    CardCap.AGPCommand.RequestQueueDepth = HostCap.AGPStatus.RequestQueueDepthMaximum;

    WriteAgpCapability(CARD_BUS, CARD_SLOT, CardCapOffset, &CardCap);
}

//============================================================================
// GART code
//============================================================================

//----------------------------------------------------------------------------
// Initialize and enable the GART
//
static BOOLEAN SetupGART()
{
    ULONG GartLength;
    GART_PTE *Gart;
    ULONG_PTR GartPhysical;
    AGPCTRL agpctrl;
    RDCR rdcr;
    ULONG i;

#if DBG

    ULONG ApBase;
    UCHAR ApSize;

    //
    // Verity the current APBASE and APSIZE settings
    //

    ReadHostRegister(&ApBase, APBASE_OFFSET, sizeof(ApBase));
    ReadHostRegister(&ApSize, APSIZE_OFFSET, sizeof(ApSize));

    if (ApSize != AGP_APERTURE_SIZE)
    {
        RIP(("SetupGART - Bad aperture size."));
    }

    if ((ApBase & PCI_ADDRESS_MEMORY_ADDRESS_MASK) != AGP_APERTURE_BASE)
    {
        RIP(("SetupGART - Bad Aperature base."));
    }

#endif DBG

    //
    // Try and get a chunk of contiguous memory big enough to map the
    // entire aperture.
    //

    GartLength = BYTES_TO_PAGES(AGP_APERTURE_BYTES) * sizeof(GART_PTE);

    Gart = MmAllocateContiguousMemorySpecifyCache(GartLength, 0, MAXULONG_PTR, 0, MmNonCached);

    if (Gart == NULL) 
    {
        RIP(("SetupGART - MmAllocateContiguousMemory failed\n"));
        return FALSE;
    }

    //
    // We successfully allocated a contiguous chunk of memory.
    // It should be page aligned already.
    //

    ASSERT(((ULONG_PTR)Gart & (PAGE_SIZE - 1)) == 0);

    //
    // Get the physical address.
    //

    GartPhysical = MmGetPhysicalAddress(Gart);

    //
    // Initialize all the PTEs
    //

    for (i = 0; i < GartLength/sizeof(GART_PTE); i++) 
    {
        Gart[i].Valid = 1;
        Gart[i].Page  = i;
    }

    //
    // Update the configuration
    //

    WriteHostRegister(&GartPhysical, ATTBASE_OFFSET, sizeof(GartPhysical));

    // 
    // Enable the gart apeture.
    //

    ReadHostRegister(&agpctrl, AGPCTRL_OFFSET, sizeof(agpctrl));
    agpctrl.EnableGART = 1;
    WriteHostRegister(&agpctrl, AGPCTRL_OFFSET, sizeof(agpctrl));

    ReadHostRegister(&rdcr, RDCR_OFFSET, sizeof(RDCR));
    rdcr.ApertureEnable = 1;
    WriteHostRegister(&rdcr, RDCR_OFFSET, sizeof(RDCR));

    return TRUE;
}

//============================================================================
// Main entrypoint
//============================================================================

//----------------------------------------------------------------------------
// Entrypoint for AGP initializes.  Sets up all AGP stuff for both the host
// and the card.
//
BOOLEAN AgpUmaEnable()
{
    SetupAgp();

    if (!SetupGART())
    {
        return FALSE; 
    }

    return TRUE;
}
