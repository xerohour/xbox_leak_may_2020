/*++

Copyright (c) 1990-2000  Microsoft Corporation

Module Name:

    agpuma.c

Abstract:

    This module maps all physical memory into the AGP address space.  It is
    used by the XBOX to simulate UMA.

--*/

#if 1 // NUGOOP

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

static BOOLEAN s_fInitialized;

//
// Define the location of the GART aperture control registers
//

#define AGP440_TARGET_DEVICE_NUMBER             0
#define AGP440_TARGET_FUNCTION_NUMBER           0

// Defines what mode we're using.  If the hardware doesn't support this then
// we'll RIP.
//
#define AGP_APERTURE_SIZE       0x30
#define AGP_APERTURE_BYTES      (64*1024*1024)
#define AGP_APERTURE_BASE       0xF8000000

//
// The GART registers on the 440 live in the host-PCI bridge.
// This is unfortunate, since the AGP driver attaches to the PCI-PCI (AGP)
// bridge. So we have to get to the host-PCI bridge config space
// and this is only possible because we KNOW this is bus 0, slot 0.
//
#define AGP_440_GART_BUS_ID     0
#define AGP_440_GART_SLOT_ID    0

#define AGP_440LX_IDENTIFIER    0x71808086
#define AGP_440BX_IDENTIFIER    0x71908086

#define APBASE_OFFSET  0x10             // Aperture Base Address
#define APSIZE_OFFSET  0xB4             // Aperture Size Register
#define PACCFG_OFFSET  0x50             // PAC Configuration Register
#define AGPCTRL_OFFSET 0xB0             // AGP Control Register
#define ATTBASE_OFFSET 0xB8             // Aperture Translation Table Base

#define Read440Config(_buf_,_offset_,_size_)                \
{                                                           \
    ULONG _len_;                                            \
    _len_ = HalGetBusDataByOffset(PCIConfiguration,         \
                                  AGP_440_GART_BUS_ID,      \
                                  AGP_440_GART_SLOT_ID,     \
                                  (_buf_),                  \
                                  (_offset_),               \
                                  (_size_));                \
    ASSERT(_len_ == (_size_));                              \
}

#define Write440Config(_buf_,_offset_,_size_)               \
{                                                           \
    ULONG _len_;                                            \
    _len_ = HalSetBusDataByOffset(PCIConfiguration,         \
                                  AGP_440_GART_BUS_ID,      \
                                  AGP_440_GART_SLOT_ID,     \
                                  (_buf_),                  \
                                  (_offset_),               \
                                  (_size_));                \
    ASSERT(_len_ == (_size_));                              \
}

//
// Conversions from APSIZE encoding to MB
//
// 0x3F (b 11 1111) =   4MB
// 0x3E (b 11 1110) =   8MB
// 0x3C (b 11 1100) =  16MB
// 0x38 (b 11 1000) =  32MB
// 0x30 (b 11 0000) =  64MB
// 0x20 (b 10 0000) = 128MB
// 0x00 (b 00 0000) = 256MB

#define AP_SIZE_4MB     0x3F
#define AP_SIZE_8MB     0x3E
#define AP_SIZE_16MB    0x3C
#define AP_SIZE_32MB    0x38
#define AP_SIZE_64MB    0x30
#define AP_SIZE_128MB   0x20
#define AP_SIZE_256MB   0x00

#define AP_SIZE_COUNT 7
#define AP_MIN_SIZE (4 * 1024 * 1024)
#define AP_MAX_SIZE (256 * 1024 * 1024)

//
// Define the GART table entry.
//
typedef struct _GART_ENTRY_HW 
{
    ULONG Valid     :  1;
    ULONG Reserved  : 11;
    ULONG Page      : 20;
} GART_ENTRY_HW, *PGART_ENTRY_HW;


//
// GART Entry states are defined so that all software-only states
// have the Valid bit clear.
//
#define GART_ENTRY_VALID        1           //  001
#define GART_ENTRY_FREE         0           //  000

#define GART_ENTRY_WC           2           //  010
#define GART_ENTRY_UC           4           //  100

#define GART_ENTRY_RESERVED_WC  GART_ENTRY_WC
#define GART_ENTRY_RESERVED_UC  GART_ENTRY_UC

#define GART_ENTRY_VALID_WC     (GART_ENTRY_VALID)
#define GART_ENTRY_VALID_UC     (GART_ENTRY_VALID)


typedef struct _GART_ENTRY_SW 
{
    ULONG State     : 3;
    ULONG Reserved  : 29;
} GART_ENTRY_SW, *PGART_ENTRY_SW;

typedef struct _GART_PTE 
{
    union 
    {
        GART_ENTRY_HW Hard;
        ULONG      AsUlong;
        GART_ENTRY_SW Soft;
    };
} GART_PTE, *PGART_PTE;

//
// Define the layout of the hardware registers
//
typedef struct _AGPCTRL 
{
    ULONG Reserved1     : 7;
    ULONG GTLB_Enable   : 1;
    ULONG Reserved2     : 24;
} AGPCTRL, *PAGPCTRL;

typedef struct _PACCFG 
{
    USHORT Reserved1    : 9;
    USHORT GlobalEnable : 1;
    USHORT PCIEnable    : 1;
    USHORT Reserved2    : 5;
} PACCFG, *PPACCFG;


//
// Define the 440-specific extension
//
typedef struct _AGP440_EXTENSION 
{
    BOOLEAN             GlobalEnable;
    BOOLEAN             PCIEnable;
    PHYSICAL_ADDRESS    ApertureStart;
    ULONG               ApertureLength;
    PGART_PTE           GartCached;
    PGART_PTE           Gart;
    ULONG               GartLength;
    PHYSICAL_ADDRESS    GartPhysical;
} AGP440_EXTENSION, *PAGP440_EXTENSION;

//
// The highest memory address supported by AGP
//

#define MAX_MEM(_num_) _num_ = 1; \
                       _num_ = _num_*1024*1024*1024*4 - 1

typedef struct _AGP_RANGE 
{
    PHYSICAL_ADDRESS MemoryBase;
    ULONG NumberOfPages;
    MEMORY_CACHING_TYPE Type;
    PVOID Context;
    ULONG CommittedPages;
} AGP_RANGE, *PAGP_RANGE;


//
// The PCI_COMMON_CONFIG includes the 192 bytes of device specific
// data.  The following structure is used to get only the first 64
// bytes which is all we care about most of the time anyway.  We cast
// to PCI_COMMON_CONFIG to get at the actual fields.
//

typedef struct 
{
    ULONG Reserved[PCI_COMMON_HDR_LENGTH/sizeof(ULONG)];
} PCI_COMMON_HEADER, *PPCI_COMMON_HEADER;

typedef struct _BUS_SLOT_ID 
{
    ULONG BusId;
    ULONG SlotId;
} BUS_SLOT_ID, *PBUS_SLOT_ID;

typedef
NTSTATUS
(*PAGP_GETSET_CONFIG_SPACE)(
    IN PVOID Context,
    IN BOOLEAN Read,
    IN PVOID Buffer,
    IN ULONG Offset,
    IN ULONG Length
    );

//============================================================================
// AGP Code
//============================================================================


NTSTATUS
ApGetSetDeviceBusData(
    IN PVOID Context,
    IN BOOLEAN Read,
    IN PVOID Buffer,
    IN ULONG Offset,
    IN ULONG Length
    )
/*++

Routine Description:

    Reads or writes PCI config space for the specified device.

Arguments:

    Read - if TRUE, this is a READ IRP
           if FALSE, this is a WRITE IRP

    Buffer - Returns the PCI config data

    Offset - Supplies the offset into the PCI config data where the read should begin

    Length - Supplies the number of bytes to be read

Return Value:

    NTSTATUS

--*/

{
    PCI_SLOT_NUMBER SlotNumber;
    ULONG Transferred;

    SlotNumber.u.AsULONG = 0;
    SlotNumber.u.bits.DeviceNumber = AGP440_TARGET_DEVICE_NUMBER;
    SlotNumber.u.bits.FunctionNumber = AGP440_TARGET_FUNCTION_NUMBER;

    if (Read) {
        Transferred = HalGetBusDataByOffset(PCIConfiguration,
                                            1,
                                            SlotNumber.u.AsULONG,
                                            Buffer,
                                            Offset,
                                            Length);
    } else {
        Transferred = HalSetBusDataByOffset(PCIConfiguration,
                                            1,
                                            SlotNumber.u.AsULONG,
                                            Buffer,
                                            Offset,
                                            Length);

    }

    if (Transferred == Length) {
        return(STATUS_SUCCESS);
    } else {
        return(STATUS_UNSUCCESSFUL);
    }
}


NTSTATUS
ApGetSetBusData(
    IN PBUS_SLOT_ID BusSlotId,
    IN BOOLEAN Read,
    IN PVOID Buffer,
    IN ULONG Offset,
    IN ULONG Length
    )
/*++

Routine Description:

    Calls HalGet/SetBusData for the specified PCI bus/slot ID.

Arguments:

    BusSlotId - Supplies the bus and slot ID.

    Read - if TRUE, this is a GetBusData
           if FALSE, this is a SetBusData

    Buffer - Returns the PCI config data

    Offset - Supplies the offset into the PCI config data where the read should begin

    Length - Supplies the number of bytes to be read

Return Value:

    NTSTATUS

--*/

{
    ULONG Transferred;

    if (Read) {
        Transferred = HalGetBusDataByOffset(PCIConfiguration,
                                            BusSlotId->BusId,
                                            BusSlotId->SlotId,
                                            Buffer,
                                            Offset,
                                            Length);
    } else {
        Transferred = HalSetBusDataByOffset(PCIConfiguration,
                                            BusSlotId->BusId,
                                            BusSlotId->SlotId,
                                            Buffer,
                                            Offset,
                                            Length);

    }
    if (Transferred == Length) {
        return(STATUS_SUCCESS);
    } else {
        return(STATUS_UNSUCCESSFUL);
    }
}


NTSTATUS
ApFindAgpCapability(
    IN PAGP_GETSET_CONFIG_SPACE pConfigFn,
    IN PVOID Context,
    OUT PPCI_AGP_CAPABILITY Capability,
    OUT UCHAR *pOffset
    )
/*++

Routine Description:

    Finds the capability offset for the specified device and
    reads in the header.

Arguments:

    pConfigFn - Supplies the function to call for accessing config space
        on the appropriate device.

    Context - Supplies the context to pass to pConfigFn

    Capabilities - Returns the AGP Capabilities common header

    pOffset - Returns the offset into config space.

Return Value:

    NTSTATUS

--*/

{
    PCI_COMMON_HEADER Header;
    PPCI_COMMON_CONFIG PciConfig = (PPCI_COMMON_CONFIG)&Header;
    NTSTATUS Status;
    UCHAR CapabilityOffset;

    //
    // Read the PCI common header to get the capabilities pointer
    //
    Status = (pConfigFn)(Context,
                         TRUE,
                         PciConfig,
                         0,
                         sizeof(PCI_COMMON_HEADER));
    if (!NT_SUCCESS(Status)) {
        RIP(("ApFindAgpCapability - read PCI Config space failed %08lx",
                Status));

        return(Status);
    }

    //
    // Check the Status register to see if this device supports capability lists.
    // If not, it is not an AGP-compliant device.
    //
    if ((PciConfig->Status & PCI_STATUS_CAPABILITIES_LIST) == 0) {
        RIP(("ApFindAgpCapability - Does not support Capabilities list, not an AGP device"));
        return(STATUS_NOT_IMPLEMENTED);
    }

    //
    // The device supports capability lists, find the AGP capabilities
    //
    if ((PciConfig->HeaderType & (~PCI_MULTIFUNCTION)) == PCI_BRIDGE_TYPE) {
        CapabilityOffset = PciConfig->u.type1.CapabilitiesPtr;
    } else {
        ASSERT((PciConfig->HeaderType & (~PCI_MULTIFUNCTION)) == PCI_DEVICE_TYPE);
        CapabilityOffset = PciConfig->u.type0.CapabilitiesPtr;
    }
    while (CapabilityOffset != 0) {

        //
        // Read the Capability at this offset
        //
        Status = (pConfigFn)(Context,
                             TRUE,
                             Capability,
                             CapabilityOffset,
                             sizeof(PCI_CAPABILITIES_HEADER));
        if (!NT_SUCCESS(Status)) {
            RIP(("ApFindAgpCapability - read PCI Capability at offset %x failed %08lx",
                    CapabilityOffset,
                    Status));
            return(Status);
        }
        if (Capability->Header.CapabilityID == PCI_CAPABILITY_ID_AGP) {
            //
            // Found the AGP Capability
            //
            break;
        } else {
            //
            // This is some other Capability, keep looking for the AGP Capability
            //
            CapabilityOffset = Capability->Header.Next;
        }
    }
    if (CapabilityOffset == 0) {
        //
        // No AGP capability was found
        //
        RIP(("ApFindAgpCapability - Does have an AGP Capability entry, not an AGP device"));
        return(STATUS_NOT_IMPLEMENTED);
    }

    *pOffset = CapabilityOffset;
    return(STATUS_SUCCESS);
}


NTSTATUS
AgpLibGetAgpCapability(
    IN PAGP_GETSET_CONFIG_SPACE pConfigFn,
    IN PVOID Context,
    OUT PPCI_AGP_CAPABILITY Capability
    )
/*++

Routine Description:

    This routine finds and retrieves the AGP capabilities in the
    PCI config space of the AGP master (graphics card).

Arguments:

    pConfigFn - Supplies the function to call for accessing config space
        on the appropriate device.

    Context - Supplies the context to pass to pConfigFn

    Capabilities - Returns the current AGP Capabilities

Return Value:

    NTSTATUS

--*/

{
    NTSTATUS Status;
    UCHAR CapabilityOffset;

    Status = ApFindAgpCapability(pConfigFn,
                                 Context,
                                 Capability,
                                 &CapabilityOffset);
    if (!NT_SUCCESS(Status)) {
        return(Status);
    }

    //
    // Read the rest of the AGP capability
    //
    Status = (pConfigFn)(Context,
                         TRUE,
                         &Capability->Header + 1,
                         CapabilityOffset + sizeof(PCI_CAPABILITIES_HEADER),
                         sizeof(PCI_AGP_CAPABILITY) - sizeof(PCI_CAPABILITIES_HEADER));
    if (!NT_SUCCESS(Status)) {
        RIP(("AgpLibGetAgpCapability - read AGP Capability at offset %xfailed %08lx",
            CapabilityOffset,
            Status));

        return(Status);
    }

    return(STATUS_SUCCESS);
}


NTSTATUS
AgpLibGetMasterCapability(
    OUT PPCI_AGP_CAPABILITY Capability
    )
/*++

Routine Description:

    Retrieves the AGP capability for the AGP master (graphics card)

Arguments:

    AgpExtension - Supplies the AGP extension

    Capability - Returns the AGP capability

Return Value:

    NTSTATUS

--*/

{
    return(AgpLibGetAgpCapability(ApGetSetDeviceBusData,
                                  NULL,
                                  Capability));
}


NTSTATUS
AgpLibGetPciDeviceCapability(
    IN ULONG BusId,
    IN ULONG SlotId,
    OUT PPCI_AGP_CAPABILITY Capability
    )
/*++

Routine Description:

    Retrieves the AGP capability for the specified PCI slot.

    Caller is responsible for figuring out what the correct
    Bus/Slot ID is. These are just passed right to HalGetBusData.

Arguments:

    BusId - supplies the bus id

    SlotId - Supplies the slot id

    Capability - Returns the AGP capability

Return Value:

    NTSTATUS

--*/

{
    BUS_SLOT_ID BusSlotId;

    BusSlotId.BusId = BusId;
    BusSlotId.SlotId = SlotId;

    return(AgpLibGetAgpCapability(ApGetSetBusData,
                                  &BusSlotId,
                                  Capability));
}


NTSTATUS
AgpLibSetAgpCapability(
    IN PAGP_GETSET_CONFIG_SPACE pConfigFn,
    IN PVOID Context,
    OUT PPCI_AGP_CAPABILITY Capability
    )
/*++

Routine Description:

    This routine finds and retrieves the AGP capabilities in the
    PCI config space of the AGP master (graphics card).

Arguments:

    pConfigFn - Supplies the function to call for accessing config space
        on the appropriate device.

    Context - Supplies the context to pass to pConfigFn

    Capabilities - Returns the current AGP Capabilities

Return Value:

    NTSTATUS

--*/

{
    NTSTATUS Status;
    UCHAR CapabilityOffset;

    Status = ApFindAgpCapability(pConfigFn,
                                 Context,
                                 Capability,
                                 &CapabilityOffset);
    if (!NT_SUCCESS(Status)) {
        return(Status);
    }

    //
    // Now that we know the offset, write the supplied command register
    //
    Status = (pConfigFn)(Context,
                         FALSE,
                         &Capability->AGPCommand,
                         CapabilityOffset + FIELD_OFFSET(PCI_AGP_CAPABILITY, AGPCommand),
                         sizeof(Capability->AGPCommand));
    if (!NT_SUCCESS(Status)) {
        RIP(("AgpLibSetAgpCapability - Set AGP command at offset %x failed %08lx",
            CapabilityOffset,
            Status));
        return(Status);
    }

    return(STATUS_SUCCESS);
}


NTSTATUS
AgpLibSetMasterCapability(
    OUT PPCI_AGP_CAPABILITY Capability
    )
/*++

Routine Description:

    Sets the AGP capability for the AGP master (graphics card)

Arguments:

    AgpExtension - Supplies the AGP extension

    Capability - Returns the AGP capability

Return Value:

    NTSTATUS

--*/

{
    return(AgpLibSetAgpCapability(ApGetSetDeviceBusData,
                                  NULL,
                                  Capability));
}


NTSTATUS
AgpLibSetPciDeviceCapability(
    IN ULONG BusId,
    IN ULONG SlotId,
    OUT PPCI_AGP_CAPABILITY Capability
    )
/*++

Routine Description:

    Sets the AGP capability for the specified PCI slot.

    Caller is responsible for figuring out what the correct
    Bus/Slot ID is. These are just passed right to HalSetBusData.

Arguments:

    BusId - supplies the bus id

    SlotId - Supplies the slot id

    Capability - Returns the AGP capability

Return Value:

    NTSTATUS

--*/

{
    BUS_SLOT_ID BusSlotId;

    BusSlotId.BusId = BusId;
    BusSlotId.SlotId = SlotId;

    return(AgpLibSetAgpCapability(ApGetSetBusData,
                                  &BusSlotId,
                                  Capability));
}


NTSTATUS
AgpEnable()
/*++

Routine Description:

    Enable AGP for both the system and the video card.

Arguments:

    None.

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS Status;
    PCI_AGP_CAPABILITY MasterCap;
    PCI_AGP_CAPABILITY TargetCap;
    ULONG DataRate;

#if DBG
    PCI_AGP_CAPABILITY CurrentCap;
#endif

    //
    // Get the master and target AGP capabilities
    //
    Status = AgpLibGetMasterCapability(&MasterCap);
    if (!NT_SUCCESS(Status)) {
        RIP(("AgpVerifyCapabilities - AgpLibGetMasterCapability failed %08lx", Status));
        return Status;
    }

    //
    // Some broken cards (Matrox Millenium II "AGP") report no valid
    // supported transfer rates. These are not really AGP cards. They
    // have an AGP Capabilities structure that reports no capabilities.
    //
    if (MasterCap.AGPStatus.Rate == 0) {
        RIP(("AgpVerifyCapabilities - AgpLibGetMasterCapability returned no valid transfer rate"));
        return STATUS_INVALID_DEVICE_REQUEST;
    }

    Status = AgpLibGetPciDeviceCapability(0,0,&TargetCap);
    if (!NT_SUCCESS(Status)) {
        RIP(("AgpVerifyCapabilities   - AgpLibGetPciDeviceCapability failed %08lx", Status));
        return Status;
    }

    //
    // Check the data rate.
    //
    DataRate = TargetCap.AGPStatus.Rate & MasterCap.AGPStatus.Rate;

    if (!(DataRate & PCI_AGP_RATE_4X))
    {
        RIP(("AgpVerifyCapabilities   - Mainboard/graphic card does not support 4x AGP."));
    }

    // 
    // The current graphics card does not support sideband addressing.  Note if 
    // this changes.
    //
    if (TargetCap.AGPStatus.SideBandAddressing & MasterCap.AGPStatus.SideBandAddressing)
    {
        RIP(("AgpVerifyCapabilities   - Sideband addressing is now available."));
    }

    // 
    // The current mainboard does not support fast write.  Note if this changes.
    //

    if (TargetCap.AGPStatus.FastWrite & MasterCap.AGPStatus.FastWrite)
    {
        RIP(("AgpVerifyCapabilities   - Fast write is now available."));
    }

    //
    // Enable the Target first.
    //
    TargetCap.AGPCommand.Rate = PCI_AGP_RATE_4X;
    TargetCap.AGPCommand.AGPEnable = 1;
    TargetCap.AGPCommand.SBAEnable = 0;
    TargetCap.AGPCommand.FastWriteEnable = 0;
    TargetCap.AGPCommand.FourGBEnable = 0;  
    Status = AgpLibSetPciDeviceCapability(0, 0, &TargetCap);

    if (!NT_SUCCESS(Status)) {
        RIP(("InitializeAGPUMA - AgpLibSetPciDeviceCapability %08lx for target failed %08lx",
                &TargetCap,
                Status));
        return Status;
    }

    //
    // Now enable the Master
    //
    MasterCap.AGPCommand.Rate = PCI_AGP_RATE_4X;
    MasterCap.AGPCommand.AGPEnable = 1;
    MasterCap.AGPCommand.SBAEnable = 0;
    MasterCap.AGPCommand.FastWriteEnable = 0;
    MasterCap.AGPCommand.FourGBEnable = 0;  
    MasterCap.AGPCommand.RequestQueueDepth = TargetCap.AGPStatus.RequestQueueDepthMaximum;
    Status = AgpLibSetMasterCapability(&MasterCap);
    if (!NT_SUCCESS(Status)) {
        RIP(("InitializeAGPUMA - AgpLibSetMasterCapability %08lx failed %08lx",
                &MasterCap,
                Status));
        return Status;
    }

#if DBG

    //
    // Read them back, see if it worked
    //
    Status = AgpLibGetMasterCapability(&CurrentCap);
    ASSERT(NT_SUCCESS(Status));

    //
    // If the target request queue depth is greater than the master will
    // allow, it will be trimmed.   Loosen the assert to not require an
    // exact match.
    //
    ASSERT(CurrentCap.AGPCommand.RequestQueueDepth <= MasterCap.AGPCommand.RequestQueueDepth);
    CurrentCap.AGPCommand.RequestQueueDepth = MasterCap.AGPCommand.RequestQueueDepth;
    ASSERT(RtlEqualMemory(&CurrentCap.AGPCommand, &MasterCap.AGPCommand, sizeof(CurrentCap.AGPCommand)));

    Status = AgpLibGetPciDeviceCapability(0,0,&CurrentCap);
    ASSERT(NT_SUCCESS(Status));
    ASSERT(RtlEqualMemory(&CurrentCap.AGPCommand, &TargetCap.AGPCommand, sizeof(CurrentCap.AGPCommand)));

#endif

    return STATUS_SUCCESS;
}

//============================================================================
// GART code
//============================================================================


NTSTATUS
AgpCreateAndEnableGART()
/*++

Routine Description:

    Create a GART large enough to hold all of physical memory, map
    the memory and enable it.

Arguments:

    None.

Return Value:

    NTSTATUS

--*/
{

    ULONG GartLength;
    PGART_PTE Gart;
    ULONG_PTR GartPhysical;
    PACCFG PACConfig;
    AGPCTRL AgpCtrl;
    ULONG i;

#if DBG

    ULONG ApBase;
    UCHAR ApSize;

#endif DBG

#if DBG

    //
    // Verity the current APBASE and APSIZE settings
    //

    Read440Config(&ApBase, APBASE_OFFSET, sizeof(ApBase));
    Read440Config(&ApSize, APSIZE_OFFSET, sizeof(ApSize));

    if (ApSize != AP_SIZE_64MB)
    {
        RIP(("AgpCreateAndEnableGART - Bad aperture size."));
    }

    if ((ApBase & PCI_ADDRESS_MEMORY_ADDRESS_MASK) != AGP_APERTURE_BASE)
    {
        RIP(("AgpCreateAndEnableGART - Bad Aperature base."));
    }

#endif DBG

    //
    // Try and get a chunk of contiguous memory big enough to map the
    // entire aperture.
    //
    GartLength = BYTES_TO_PAGES(AGP_APERTURE_BYTES) * sizeof(GART_PTE);

    Gart = MmAllocateContiguousMemorySpecifyCache(GartLength, 0, MAXULONG_PTR, 0, MmNonCached);
    if (Gart == NULL) {
        RIP(("Agp440CreateGart - MmAllocateContiguousMemory %lx failed\n",
            GartLength));
        return(STATUS_INSUFFICIENT_RESOURCES);
    }

    //
    // We successfully allocated a contiguous chunk of memory.
    // It should be page aligned already.
    //
    ASSERT(((ULONG_PTR)Gart & (PAGE_SIZE-1)) == 0);

    //
    // Get the physical address.
    //
    GartPhysical = MmGetPhysicalAddress(Gart);

    //
    // Initialize all the PTEs
    //
    for (i = 0; i < GartLength/sizeof(GART_PTE); i++) {
        Gart[i].Hard.Valid = 1;
        Gart[i].Hard.Page = i;
    }

    //
    // Update the configuration
    //
    Write440Config(&GartPhysical, ATTBASE_OFFSET, sizeof(GartPhysical));

    // 
    // Enable the GTLB
    //
    Read440Config(&AgpCtrl, AGPCTRL_OFFSET, sizeof(AgpCtrl));
    AgpCtrl.GTLB_Enable = 1;
    Write440Config(&AgpCtrl, AGPCTRL_OFFSET, sizeof(AgpCtrl));

    //
    // Enable the GART arpeture.
    //
    Read440Config(&PACConfig, PACCFG_OFFSET, sizeof(PACConfig));
    PACConfig.GlobalEnable = 1;
    Write440Config(&PACConfig, PACCFG_OFFSET, sizeof(PACConfig));

    return STATUS_SUCCESS;
}

//============================================================================
// Main entrypoint
//============================================================================


BOOLEAN
AgpUmaEnable()

/*++

Routine Description:

    Entrypoint for master initialization. This is called after target initialization
    and should be used to initialize the AGP capabilities of both master and target.

    This is also called when the master transitions into the D0 state.

Return Value:

    S_OK

--*/

{
    NTSTATUS Status;

    if (!s_fInitialized)
    {
        //
        // Enable the devices.
        //
        Status = AgpEnable();
        if (!NT_SUCCESS(Status)) {
            return FALSE;
        }

        Status = AgpCreateAndEnableGART();
        if (!NT_SUCCESS(Status)) {
            return FALSE;
        }
    }

    s_fInitialized = TRUE;

    return TRUE;
}

#endif 1
