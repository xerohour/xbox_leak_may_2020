/*++

Copyright (c) 1996  Microsoft Corporation

Module Name:

    filter.h

Abstract:
  Filter driver to adapt the SectorSize of USB Mass Storage devices to effectively
  be 512 bytes.  Design to work with up to 8 kbyte sector size.


  Based on NULL filter driver -- boilerplate code -- from NTDDK.

Author:

    ervinp (original boiler plate)
	mitchd (sctrsize)

Environment:

    Kernel mode

Revision History:


--*/


//
// If this driver is going to be a filter in the paging, hibernation, or dump
// file path, then HANDLE_DEVICE_USAGE should be defined.
//
// #define HANDLE_DEVICE_USAGE 1

enum deviceState {
        STATE_INITIALIZED,
        STATE_STARTING,
        STATE_STARTED,
        STATE_START_FAILED,
        STATE_STOPPED,  // implies device was previously started successfully
        STATE_SUSPENDED,
        STATE_REMOVING,
        STATE_REMOVED
};


/*
 *  Memory tag for memory blocks allocated by this driver
 *  (used in ExAllocatePoolWithTag() call).
 *  This DWORD appears as "Filt" in a little-endian memory byte dump.
 *
 *  NOTE:  PLEASE change this value to be unique for your driver!  Otherwise,
 *  your allocations will show up with every other driver that uses 'tliF' as
 *  an allocation tag.
 *  
 */
#define FILTER_TAG (ULONG)'tliF'

#undef ExAllocatePool
#define ExAllocatePool(type, size) \
            ExAllocatePoolWithTag (type, size, FILTER_TAG)


/*
 *	SCTRSIZE specific extension to the boiler-plate
 *	filter device extension.
 *
 */
typedef struct _SCTRSZ_EXT
{
	ULONG BlockSize;		//Block Size Reported by Device
	ULONG BlockCount;		//Block Count Reported by Device
	ULONG BlocksPerSector;	//Sectors (512 byte blocks) per Block
	ULONG SectorCount;		//Sector Count
	PVOID BlockBuffer;		//Allocated during read capacity.
	PVOID BlockBufferMdl;	//Mdl to describe the block buffer
	BOOLEAN CapacityKnown;	//TRUE if we have seen READ_CAPACITY go by yet.
	
	PETHREAD ThreadObj;		//Thread object for pumping the IrpQueue.
	KEVENT NewIrpEvent;				//Event to signal that a new IRP has been queued
	KEVENT TerminateThreadEvent;	//Event to signal termination of the thread.

	LIST_ENTRY IrpQueue;	//IrpQueue
	KSPIN_LOCK IrpQueueLock;//Spin for IrpQueue

} SCTRSZ_EXT, *PSCTRSZ_EXT;


/*
 *	Structure for breaking up transfers into three:
 *	Initial (Partial) Block
 *	Whole Blocks
 *	Final (Partial) Block
 */
typedef struct _SCRTSZ_BLOCK_DESC
{
	ULONG FirstSector;		//First sector requested in CDB
	ULONG InitialBlock;		//The initial (partial) block we need to get.
	ULONG FirstWholeBlock;	//The first while block we need.
	ULONG FinalBlock;		//The final (partial) block we need.
	USHORT InitialSectorOffset;		//Offset of first sector requested in initial block
	USHORT InitialBlockSectorCount;	//Number of sectors need from the Initial Block
	USHORT WholeBlockCount;			//Number of whole blocks needed.
	USHORT FinalBlockSectorCount;	//Number of sectors needed from the Final Block
} SCRTSZ_BLOCK_DESC, *PSCRTSZ_BLOCK_DESC;


#define DEVICE_EXTENSION_SIGNATURE 'rtlF'

typedef struct DEVICE_EXTENSION {

    /*
     *  Memory signature of a device extension, for debugging.
     */
    ULONG signature;

    /*
     *  Plug-and-play state of this device object.
     */
    enum deviceState state;

    /*
     *  The device object that this filter driver created.
     */
    PDEVICE_OBJECT filterDevObj;

    /*
     *  The device object created by the next lower driver.
     */
    PDEVICE_OBJECT physicalDevObj;

    /*
     *  The device object at the top of the stack that we attached to.
     *  This is often (but not always) the same as physicalDevObj.
     */
    PDEVICE_OBJECT topDevObj;

    /*
     *  deviceCapabilities includes a
     *  table mapping system power states to device power states.
     */
    DEVICE_CAPABILITIES deviceCapabilities;

    /*
     *  pendingActionCount is used to keep track of outstanding actions.
     *  removeEvent is used to wait until all pending actions are
     *  completed before complete the REMOVE_DEVICE IRP and let the
     *  driver get unloaded.
     */
    LONG pendingActionCount;
    KEVENT removeEvent;

#ifdef HANDLE_DEVICE_USAGE
    /*
     *  Keep track of the number of paging/hibernation/crashdump
     *  files that are opened on this device.
     */
    ULONG  pagingFileCount, hibernationFileCount, crashdumpFileCount;
    KEVENT deviceUsageNotificationEvent;
    PVOID  pagingPathUnlockHandle;  /* handle to lock certain code as non-pageable */

    /*
     *  Also, might need to lock certain driver code as non-pageable, based on
     *  initial conditions (as opposed to paging-file considerations).
     */
    PVOID  initUnlockHandle;
    ULONG  initialFlags;
#endif // HANDLE_DEVICE_USAGE 

	/*
	 *	All the SCTRSZ specific fields.
	 *
	 */
	SCTRSZ_EXT ext;
};




#if DBG
    #define DBGOUT(params_in_parentheses)   \
        {                                               \
            DbgPrint("'SCTRSIZE> "); \
            DbgPrint params_in_parentheses; \
            DbgPrint("\n"); \
        }
    #define TRAP(msg)  \
        {   \
            DBGOUT(("TRAP at file %s, line %d: '%s'.", __FILE__, __LINE__, msg)); \
            DbgBreakPoint(); \
        }
#else
    #define DBGOUT(params_in_parentheses)
    #define TRAP(msg)
#endif


/*
 *  Function externs
 */
NTSTATUS    DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
NTSTATUS    SCTRSZ_AddDevice(IN PDRIVER_OBJECT driverObj, IN PDEVICE_OBJECT pdo);
VOID        SCTRSZ_DriverUnload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS    SCTRSZ_Dispatch(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp);
NTSTATUS    SCTRSZ_PnP(struct DEVICE_EXTENSION *devExt, PIRP irp);
#ifdef HANDLE_DEVICE_USAGE
NTSTATUS    SCTRSZ_DeviceUsageNotification(struct DEVICE_EXTENSION *devExt, PIRP irp);
#endif // HANDLE_DEVICE_USAGE 
NTSTATUS    SCTRSZ_Power(struct DEVICE_EXTENSION *devExt, PIRP irp);
NTSTATUS    SCTRSZ_PowerComplete(IN PDEVICE_OBJECT devObj, IN PIRP irp, IN PVOID context);
NTSTATUS    GetDeviceCapabilities(struct DEVICE_EXTENSION *devExt);
NTSTATUS    CallNextDriverSync(struct DEVICE_EXTENSION *devExt, PIRP irp);
NTSTATUS    CallDriverSync(PDEVICE_OBJECT devObj, PIRP irp);
NTSTATUS    CallDriverSyncCompletion(IN PDEVICE_OBJECT devObj, IN PIRP irp, IN PVOID Context);
VOID        IncrementPendingActionCount(struct DEVICE_EXTENSION *devExt);
VOID        DecrementPendingActionCount(struct DEVICE_EXTENSION *devExt);
NTSTATUS    QueryDeviceKey(HANDLE Handle, PWCHAR ValueNameString, PVOID Data, ULONG DataLength);
//VOID        RegistryAccessSample(struct DEVICE_EXTENSION *devExt, PDEVICE_OBJECT devObj);

/*
 *	Function externs not in original boiler plate
 */
NTSTATUS	SCTRSZ_Ioctl(struct DEVICE_EXTENSION *devExt, PIRP irp, BOOLEAN *passIrpDown);
NTSTATUS	SCTRSZ_Scsi(struct DEVICE_EXTENSION *DevExt, PIRP Irp, BOOLEAN *PassIrpDown);
NTSTATUS	SCTRSZ_ScsiExecute(struct DEVICE_EXTENSION *DevExt, PIRP Irp);
VOID		SCTRSZ_SrbThreadRoutine(PVOID StartContext);
PIRP		SCTRSZ_DequeueIrp(struct DEVICE_EXTENSION *DevExt);
VOID		SCTRSZ_ScsiReadCapacity(struct DEVICE_EXTENSION *DevExt, PIRP Irp);
VOID		SCTRSZ_ScsiRead(struct DEVICE_EXTENSION *DevExt, PIRP Irp);
VOID		SCTRSZ_ScsiWrite(struct DEVICE_EXTENSION *DevExt, PIRP Irp);
VOID		SCTRSZ_BreakUpRequest(union _CDB *Cdb, PSCRTSZ_BLOCK_DESC BlockDesc, UCHAR BlocksPerSector);
VOID		SCTRSZ_ScsiCancel(PDEVICE_OBJECT DeviceObject, PIRP Irp);


/*
 *	A little utility function
 */
_inline long ChangeEndian(long x)
{	
	return(	(((x) >> 24) & 0x000000FFL) |
			(((x) >>  8) & 0x0000FF00L) |
			(((x) <<  8) & 0x00FF0000L) |
			(((x) << 24) & 0xFF000000L)
		);
}
