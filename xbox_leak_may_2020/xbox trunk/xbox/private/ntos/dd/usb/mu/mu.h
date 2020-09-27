/*++

Copyright (c) 1996-2000 Microsoft Corporation

Module Name:

    MU.H

Abstract:

    Header file for MU driver

Environment:

    kernel mode

Revision History:

    06-13-2000 : started rewrite : georgioc
    10-11-2000 : cleanup : mitchd

--*/

//*****************************************************************************
// Includes
//*****************************************************************************
extern "C" {
#include <ntos.h>
}
#include <ntrtl.h>
#include <nturtl.h>
#include <rbc.h>
#include <ntdddisk.h>
#include <xtl.h>
#include <xdbg.h>
#include <usb.h>
#define  MODULE_POOL_TAG '__UM'
#include <debug.h>

//*****************************************************************************
// Forward declaration
//*****************************************************************************


//*****************************************************************************
// Useful Macros
//*****************************************************************************

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#define SET_FLAG(Flags, Bit)    ((Flags) |= (Bit))
#define CLEAR_FLAG(Flags, Bit)  ((Flags) &= ~(Bit))
#define TEST_FLAG(Flags, Bit)   ((Flags) & (Bit))
#define TEST_ALL_FLAGS(Flags, Bits) ((Bits) == ((Flags) & (Bits)))

#if DBG || DEBUG_LOG

    extern ULONG               MU_DebugFlags;     // DBGF_* Flags
    extern ULONG               MU_DebugLevel;     // Level of debug output

#endif


// A better WhichBit written as an inline template function.
// It returns the lowest order bit set.  If only one bit is set
// 
template <class T>
inline UCHAR WhichBit(T data)
{
    UCHAR bit;
    T mask;
    for(
        bit=0, mask=1;
        bit < sizeof(T)*8;
        bit++, mask <<= 1)
    {
        if(mask&data) break;
    }
    ASSERT(bit != sizeof(T)*8);
    return bit;
}

template <class T>
inline BOOL IsPowerOf2(T data)
/*
    Routine Description:
        Returns TRUE if data is a power of 2.  Implemented
        as inline template to work on any data type, with
        the type safety of a real function, but the efficiency
        of a MACRO.

        bitwise ANDing a value with itself minus one
        drops the lowest bit.  If the result is zero
        then it is a power of 2, provided that it
        wasn't zero or one to begin with.

        Ex. 010110 => 010110 & 010101 == 010100 NOT Power of 2.
        Ex. 001000 => 001000 & 000111 == 000000 Power of 2.
*/
{
    return ((data > 1) && !(data&(data-1))) ? TRUE : FALSE;
}


//
//  ReverseEndian for ULONG and USHORT, could have
//  used a template instead of overloading, but
//  the loop would have been less efficient.
//
inline ULONG ReverseEndian(ULONG Data)
{
    ULONG RetData;
    ((BYTE *)&RetData)[0]=((BYTE *)&Data)[3];
    ((BYTE *)&RetData)[1]=((BYTE *)&Data)[2];
    ((BYTE *)&RetData)[2]=((BYTE *)&Data)[1];
    ((BYTE *)&RetData)[3]=((BYTE *)&Data)[0];
    return RetData;
}
inline USHORT ReverseEndian(USHORT Data)
{
    USHORT RetData;
    ((BYTE *)&RetData)[0]=((BYTE *)&Data)[1];
    ((BYTE *)&RetData)[1]=((BYTE *)&Data)[0];
    return RetData;
}


//*****************************************************************************
// Defintions for a debug only IRP watchdog
//*****************************************************************************
#if DBG
#define MU_DEBUG_DECLARE_WATCHDOG_PARAMETERS() \
    KTIMER        DbgIrpTimer;\
    KDPC          DbgIrpTimeoutDpc;\
    LARGE_INTEGER DbgIrpTimeoutTime;
#define MU_DEBUG_INIT_WATCHDOG_PARAMETERS(DeviceExtension) MUDebugInitWatchDogParameters(DeviceExtension)
#define MU_DEBUG_SET_WATCHDOG(DeviceExtension) MuDebugSetWatchDogTimer(DeviceExtension)
#define MU_DEBUG_PET_WATCHDOG(DeviceExtension) MuDebugPetWatchDogTimer(DeviceExtension)
#define MU_COMPLETE_REQUEST(DeviceExtension, Irp, PriorityBoost) MuDebugCompleteRequest(DeviceExtension, Irp, PriorityBoost)
#else
#define MU_DEBUG_INIT_WATCHDOG_PARAMETERS(DeviceExtension)
#define MU_DEBUG_DECLARE_WATCHDOG_PARAMETERS()
#define MU_DEBUG_SET_WATCHDOG(DeviceExtension)
#define MU_DEBUG_PET_WATCHDOG(DeviceExtension)
#define MU_COMPLETE_REQUEST(DeviceExtension, Irp, PriorityBoost) IoCompleteRequest((Irp), (PriorityBoost))
#endif


//*****************************************************************************
// USB Definitions that are not elsewhere
//*****************************************************************************

//
// Interface Descriptor values
//

#define MU_SUBCLASS_RBC                 0x01
#define MU_SUBCLASS_TRANSPARENT         42
#define MU_PROTOCOL_BULK_ONLY           0x50

//
// Bulk-Only class-specific bRequest codes
//

#define BULK_ONLY_MASS_STORAGE_RESET    0xFF


//*****************************************************************************
// Declarations for performing diagnostics on Memory Units (requires special build)
//*****************************************************************************
#ifdef MU_DIAGNOSTIC_IOCTLS

//
//  VENDOR Specific Debug Commands bRequest
//

#define MU_VSC_GET_BAD_BLOCK_TABLE   0
#define MU_VSC_MEMORY_TEST           1

//
//  Definitions for VENDOR  Specific Debug Commands
//

#define MU_VSC_BAD_BLOCK_TABLE_SIZE (sizeof(USHORT)*16)
#define MU_VSC_BAD_BLOCK_COUNT_SIZE sizeof(USHORT)

//
// IOCTL to expose the vendor specific commands
// (supported only in debug builds)
//

#define MU_IOCTL_GET_BAD_BLOCK_TABLE\
    CTL_CODE(FILE_DEVICE_DISK, MU_VSC_GET_BAD_BLOCK_TABLE, METHOD_NEITHER, FILE_ANY_ACCESS)
#define MU_IOCTL_MEMORY_TEST\
    CTL_CODE(FILE_DEVICE_DISK, MU_VSC_MEMORY_TEST, METHOD_NEITHER, FILE_ANY_ACCESS)

#endif  //end of DIAGNOSTIC IOCTLS


//*****************************************************************************
// USB Bulk-Only Protocol Definitions
//*****************************************************************************

//
//  The MU driver requires that MU BULK endpoints
//  have a wMaxPacket of MU_MAX_BULK_PACKET_SIZE
//  (extra stringent Xbox requirement)

#define MU_MAX_BULK_PACKET_SIZE         0x40

//
// Command Block Wrapper Signature 'USBC'
//

#define CBW_SIGNATURE               0x43425355
#define CBW_FLAGS_DATA_IN           0x80
#define CBW_FLAGS_DATA_OUT          0x00

//
// Command Status Wrapper Signature 'USBS'
//

#define CSW_SIGNATURE               0x53425355

#define CSW_STATUS_GOOD             0x00
#define CSW_STATUS_FAILED           0x01
#define CSW_STATUS_PHASE_ERROR      0x02


#pragma pack (push, 1)

//
// Command Block Wrapper
//
typedef struct _CBW
{
    ULONG   dCBWSignature;

    ULONG   dCBWTag;

    ULONG   dCBWDataTransferLength;

    UCHAR   bCBWFlags;

    UCHAR   bCBWLUN;

    UCHAR   bCDBLength;
    union
    {
        UCHAR   CBWCDB[16];
        CDB     Cdb;
    };
} CBW, *PCBW;


// Command Status Wrapper
//
typedef struct _CSW
{
    ULONG   dCSWSignature;

    ULONG   dCSWTag;

    ULONG   dCSWDataResidue;

    UCHAR   bCSWStatus;

} CSW, *PCSW;

#pragma pack (pop)

//******************************************************************************
//  Various Hard-coded sizes.
//
//      EMULATED_SECTOR_SIZE - This is the sector size reported to the filesystem.
//          It is the smallest unit of data the filesystem will ever try to
//          read or write.
//
//      MEDIA_BLOCK_SIZE - This is the size of a block on the media.  A block on the
//          media is the smallest unit which can be erased.  All writes must
//          start on a block boundary and end on a block boundary.  The Xbox MU
//          protocol does NOT currently provide a mechanism to query the device.
//          This value is is hardcoded as 8192 bytes.  Devices with smaller
//          media block sizes should work fine with this driver, devices with
//          larger media block sizes would not work.
//
//      The other type of block size is the logical block size.  This is reported
//      back as BytesPerBlock in the READ_CAPACITY_DATA structure received in
//      in response to a read capacity command.  Any power of 2 value less than
//      4096 is supported.  This is typically the page size of the media, which
//      is the smallest unit which may be read or written (but not erased,
//      or rewritten).  For the standard Xbox hawk this value is 512 bytes.
//
//      WRITE_BUFFER_SIZE - This buffer is used store the original contents
//          of the portion of a media block that we need to write over.  When
//          a write for a portion of a media block is requested, the original
//          contents are read into this block, and then the whole thing is
//          written back out.
//
//******************************************************************************
#define EMULATED_SECTOR_SIZE             4096
#define EMULATED_SECTOR_SHIFT            12
#define MAXIMUM_MEDIA_BLOCK_SIZE         16384
#define DEFAULT_MEDIA_BLOCK_SIZE         8192
#define WRITE_BUFFER_SIZE                (MAXIMUM_MEDIA_BLOCK_SIZE - EMULATED_SECTOR_SIZE)

//*****************************************************************************
// Forward declarations of pointer to structure types
//*****************************************************************************
typedef struct _MU_DRIVER_EXTENSION *PMU_DRIVER_EXTENSION;
typedef struct _MU_INSTANCE *PMU_INSTANCE;
typedef struct _MU_DEVICE_EXTENSION *PMU_DEVICE_EXTENSION;

//*****************************************************************************
// Driver object and driver(not device) specific definitions
//*****************************************************************************
#define MU_CONTROLLERS_PER_PORT 2
#define MU_DEFAULT_MAX_MOUNTED  8
/* DEVICE_INDEX_FROM_PORT_AND_SLOT depends on MU_CONTROLLERS_PER_PORT = 2 */
#define NODE_INDEX_FROM_PORT_AND_SLOT(port, slot) ((port<<1)+slot)

typedef struct MU_DRIVER_EXTENSION {
   
    PMU_DEVICE_EXTENSION DeviceObjectFreeList;
    //
    //  Resources for handling writes to only a portion of
    //  a media block.  Not that there is one global WriteBuffer
    //  and partial requests are queued.
    //
    
    LIST_ENTRY  PartialRequestQueue;
    UCHAR       WriteBuffer[WRITE_BUFFER_SIZE];
    
    PMU_INSTANCE Nodes;

}  MU_DRIVER_EXTENSION;

extern MU_DRIVER_EXTENSION MU_DriverExtension;

//*****************************************************************************
// MU Request Block (MRB) defintions
//*****************************************************************************

#define MRB_TIMEOUT_UNIT                      (-100000) //This constant represents 10 ms to the timer routines.
#define MRB_STANDARD_TIMEOUT                  (8)       //Standard Timeout is 80 ms (spec says 50 ms), this applies to CBW's
#define MRB_DATA_TIMEOUT                      (15)      //Time out is 150 ms for each 2k portion of a read or write.
#define MRB_RESET_TIMEOUT                     (10)      //Time out is 100 ms for each stage of the reset sequence.
                                                          
#define MRB_READ_CAPACITY_TIMEOUT             (10)      // a 100 ms delay for the CSW of read capacity.
#define MRB_VERIFY_TIMEOUT_PER_64K            (100)     // 1 second per 64 kbytes verified for CSW of VERIFY.
#define MRB_CALC_VERIFY_TIMEOUT(VerifyLength) ((USHORT)(MRB_VERIFY_TIMEOUT_PER_64K*((VerifyLength) >> 16)))

#define MRB_MAXIMUM_RETRIES                    2        //Maximum number of times an MRB is retried.

#define MRB_FLAGS_NO_DATA_TRANSFER  0x00
#define MRB_FLAGS_DATA_IN           0x01
#define MRB_FLAGS_DATA_OUT          0x02
#define MRB_FLAGS_SPLIT_WRITE       0x04
#define MRB_FLAGS_UNSPECIFIED_DIRECTION (MRB_FLAGS_DATA_IN | MRB_FLAGS_DATA_OUT)
#define MRB_TRANSFER_SIZE           1024 //size of each transfer
#define MU_BULK_MIN_TD_QUOTA        ((2*MRB_TRANSFER_SIZE)/MU_MAX_BULK_PACKET_SIZE)

typedef VOID (*PFMU_IO_COMPLETION)(PMU_DEVICE_EXTENSION DeviceExtension, NTSTATUS Status);

typedef struct _MU_REQUEST_BLOCK {
    // filled out by disk layer
    // (are guaranteed to remain intact)
    PUCHAR             DataBuffer;
    ULONG              TransferLength;
    PFMU_IO_COMPLETION CompletionRoutine;
    USHORT             TimeOutValue; //in 10's of milliseconds
    UCHAR              Retries;
    UCHAR              Flags;
    ULONG              UserStartOffset; //For partial writes (read\modify\write)
    ULONG              UserEndOffset;   //Specifies which portion of the read goes
                                        //into the user buffer.
    CBW                Cbw; // Disk layer fills out only CDB
    //  Used by the MRB state machine
    CSW                Csw;
    union
    {
    NTSTATUS            FailedStatus;    //used to hold failure during reset.       
    ULONG              BytesSubmitted;
    };
    
    KTIMER             Timer;
    KDPC               TimeoutDpcObject;
} MU_REQUEST_BLOCK, *PMU_REQUEST_BLOCK;

//*****************************************************************************
// MU DeviceExtension and related defintions
//*****************************************************************************

//
// PDEVICE_EXTENSION->DeviceFlags state flags
//
#define DF_PENDING_CLOSE           0x00000001
#define DF_PENDING_REMOVE          0x00000002
#define DF_REMOVED                 0x00000004

//
//  Flags used by the MRB state machine,
//  these are internal to it.  The flags
//  in the MRB is only a direction.
//

#define DF_MRB_TIMER_RUNNING       0x00000100
#define DF_PRIMARY_URB_PENDING     0x00000200
#define DF_SECONDARY_URB_PENDING   0x00000400
#define DF_ERROR_PENDING           0x00000800
#define DF_RESET_STEP1             0x00001000
#define DF_RESET_STEP2             0x00002000
#define DF_RESET_STEP3             0x00004000
#define DF_RESET_STEPS (DF_RESET_STEP1|DF_RESET_STEP2|DF_RESET_STEP3)

#define DF_ANY_URB_PENDING (DF_PRIMARY_URB_PENDING|DF_SECONDARY_URB_PENDING)

//
// PDEVICE_EXTENSION->DeviceFlags state flags
// related to the endpoint state machine.
//

#define DF_ENDPOINT_FLAGS          0x000F0000
#define DF_DEFAULT_ENDPOINT_OPEN   0x00010000
#define DF_BULK_IN_ENDPOINT_OPEN   0x00020000
#define DF_BULK_OUT_ENDPOINT_OPEN  0x00040000
#define DF_CLOSING_ENDPOINTS       0x00080000
#define DF_ENDPOINTS_READY         0x00070000

//
// PDEVICE_EXTENSION->DeviceFlags state flags
// related to the write state machine.
//

#define MU_WRITE_HAS_INITIAL_PORTION 0x02000000
#define MU_WRITE_HAS_FINAL_PORTION   0x04000000
#define MU_WRITE_HAS_MIDDLE_PORTION  0x08000000
#define MU_WRITE_HAS_FLAGS   (MU_WRITE_HAS_INITIAL_PORTION|MU_WRITE_HAS_FINAL_PORTION|MU_WRITE_HAS_MIDDLE_PORTION)

//
//  Write state machine flags for the normal
//  partial write state machine.
//
#define MU_WRITE_STATE_START         0x10000000
#define MU_WRITE_STATE_INITIAL_READ  0x20000000
#define MU_WRITE_STATE_INITIAL_WRITE 0x30000000
#define MU_WRITE_STATE_FINAL_READ    0x40000000
#define MU_WRITE_STATE_FINAL_WRITE   0x50000000
#define MU_WRITE_STATE_DONE          0x60000000
#define MU_WRITE_STATE_BITS          0xF0000000
#define MU_WRITE_STATE_INCREMENT     0x10000000

//
//  Write state machine flags for the "middle" partial
//  write state machine.
//
#define MU_WRITE_STATE_MIDDLE_START        0x10000000
#define MU_WRITE_STATE_MIDDLE_READ_BEFORE  0x20000000
#define MU_WRITE_STATE_MIDDLE_READ_AFTER   0x30000000
#define MU_WRITE_STATE_MIDDLE_WRITE        0x40000000
#define MU_WRITE_STATE_MIDDLE_DONE         0x50000000


//
//  Pattern that we use for marking corrupt sectors.
//  (Don't worry how long it is (except for code space;),
//  in the success case, we stop comparing on the first
//  mismatch.  In the error case we have plenty of time
//  anyway.)  The number pattern at the beginning is part
//  of GUID generated with GUIDGEN.
//
const char tagMU_CORRUPT_SECTOR_PATTERN[]="951F0EF630DC46d9_CORRUPT_SECTOR";
#define MU_CORRUPT_SECTOR_PATTERN ((PULONG)tagMU_CORRUPT_SECTOR_PATTERN)
#define MU_CORRUPT_SECTOR_PATTERN_SIZE sizeof(tagMU_CORRUPT_SECTOR_PATTERN)
#define MU_CORRUPT_SECTOR_PATTERN_ULONG_COUNT (MU_CORRUPT_SECTOR_PATTERN_SIZE/sizeof(ULONG))

//
// MU_INSTANCE contains context data for each MU.
// These are statically allocated at startup time.
// 

typedef struct _MU_INSTANCE
{
    //
    // Device class interface to USB core stack
    //

    IUsbDevice      *Device;
    
    //
    // USB related data retrieved during 
    // enumeration, or when opening endpoints
    //

    UCHAR  InterfaceNumber;
    UCHAR  BulkInEndpointAddress;
    UCHAR  BulkOutEndpointAddress;
    UCHAR  AddRemoveCount;  //DEBUG ONLY, catches double removes and\or adds.    
    
    //
    //  Pointer to assigned device extension
    //

    PMU_DEVICE_EXTENSION DeviceExtension;

} MU_INSTANCE; 


typedef struct _MU_DEVICE_EXTENSION
{
    //
    //  Points to the Device Object
    //
    PDEVICE_OBJECT DeviceObject;

    //
    //  Number (used to regenerate the name)
    //
    ULONG DeviceObjectNumber;

    //
    //  Points to our MU_INSTANCE
    //
    union {
        PMU_INSTANCE         MuInstance;
        PMU_DEVICE_EXTENSION NextFree;
    };
    //
    // Various DF_xxxx flags
    //

    ULONG   DeviceFlags;

    
    //***
    //*** Above here is maintained while the device object
    //*** is on the free list, below is always zeroed.
    //***

    //
    // At any time, each device is processing
    // one IRP, this is it.
    //

    PIRP    PendingIrp;
    ULONG   IrpCount;   //Count of outstanding IRPs

    //  Variables to cache results of calculations
    //  for partial writes
    ULONG   InitialWriteByteCount;
    ULONG   FinalWriteByteCount;

    PVOID   BulkInEndpointHandle;
    PVOID   BulkOutEndpointHandle;

    //
    // The Mrb for managing commands
    //

    MU_REQUEST_BLOCK    Mrb;

    //
    //  Each instance maitains three independent URBs
    //  for communication with the device.
    //  This is so that two bulk URBs can be outstanding
    //  at once (which better utilizes bandwidth).
    //  Furthermore, a dedicated close URB is provided
    //  so that the close state machine does not need
    //  to synchronize with the MRB state machine.
    //
    
    URB                             Urb;
    URB_BULK_OR_INTERRUPT_TRANSFER  BulkUrbSecondary;
    URB_CLOSE_ENDPOINT              CloseEndpointUrb;
    KEVENT                          CloseEvent; //signaled when a close completes

    //
    //  Geomerty Information
    //

    DISK_GEOMETRY       DiskGeometry;
    ULONG               LogicalBlockShift;
    LARGE_INTEGER       PartitionLength;
    ULONG               MediaBlockSize;

    //
    //  Watchdog timer parameters
    //  
    MU_DEBUG_DECLARE_WATCHDOG_PARAMETERS()

} MU_DEVICE_EXTENSION;


//*****************************************************************************
// F U N C T I O N    P R O T O T Y P E S
//*****************************************************************************
// Only those between translation units.

//
//  inline [acquire and release preallocate device objects.
//
__inline 
PMU_DEVICE_EXTENSION
MU_AcquireDeviceObject()
{ 
    PMU_DEVICE_EXTENSION retVal;
    retVal = MU_DriverExtension.DeviceObjectFreeList;
    if(retVal)
    {
        MU_DriverExtension.DeviceObjectFreeList = retVal->NextFree;
        retVal->DeviceFlags = 0;
        retVal->MuInstance = NULL;
    }
    return retVal;
}

__inline 
VOID
MU_ReleaseDeviceObject(PMU_DEVICE_EXTENSION DeviceExtension)
{
    RtlZeroMemory(&DeviceExtension->PendingIrp, sizeof(MU_DEVICE_EXTENSION)-FIELD_OFFSET(MU_DEVICE_EXTENSION, PendingIrp));
    DeviceExtension->DeviceFlags = DF_REMOVED;
    DeviceExtension->NextFree = MU_DriverExtension.DeviceObjectFreeList;
    MU_DriverExtension.DeviceObjectFreeList = DeviceExtension;
}




//
// mu.cpp  [These are all called from outside the MU driver]
//

EXTERNUSB VOID
MU_Init(IUsbInit *pUsbInit);

EXTERNUSB VOID
MU_AddDevice (
    IN IUsbDevice *Device
    );

EXTERNUSB VOID
MU_RemoveDevice (
    IN IUsbDevice *Device
    );

extern "C"
{
NTSTATUS
MU_CreateDeviceObject(
    IN  ULONG            Port,
    IN  ULONG            Slot,
    IN  POBJECT_STRING  DeviceName
    );

VOID
MU_CloseDeviceObject(
    IN  ULONG  Port,
    IN  ULONG  Slot
    );

PDEVICE_OBJECT
MU_GetExistingDeviceObject(
    IN  ULONG  Port,
    IN  ULONG  Slot
    );
}
//
// mrb.cpp
//

VOID
FASTCALL
MU_fStartMrb(
    IN PMU_DEVICE_EXTENSION DeviceExtension
    );

VOID
MU_MrbTimeout (
    IN PKDPC Dpc,
    IN PVOID fdoDeviceExtension,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    );

//
//  disk.cpp
//

NTSTATUS
MU_InternalIo (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    );

VOID
MU_StartIo (
    IN PDEVICE_OBJECT   DeviceObject,
    IN PIRP             Irp
    );

//
//  Debug Only Methods, defined at the bottom of disk.cpp
//
#if DBG
VOID
MUDebugWatchdogDpcRoutine(
    PKDPC,
    PMU_DEVICE_EXTENSION DeviceExtension,
    PVOID,
    PVOID
    );

VOID
MUDebugInitWatchDogParameters(
    PMU_DEVICE_EXTENSION DeviceExtension
    );

VOID 
MuDebugSetWatchDogTimer(
    PMU_DEVICE_EXTENSION DeviceExtension
    );
VOID
MuDebugPetWatchDogTimer(
    PMU_DEVICE_EXTENSION DeviceExtension
    );

VOID 
MuDebugCompleteRequest(
    PMU_DEVICE_EXTENSION DeviceExtension,
    PIRP Irp,
    CCHAR PriorityBoost
    );
#endif
