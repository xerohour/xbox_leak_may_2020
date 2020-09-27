// this file contans the functions that implement display driver to RM comm

#include <ntddk.h>
#include <windef.h>
#undef WIN32
//#define NTMINIPORT
#include <nvos.h>
#include <nvntioctl.h>

typedef LONG VP_STATUS;
typedef struct _STATUS_BLOCK {
    VP_STATUS Status;
    ULONG Information;
} STATUS_BLOCK, *PSTATUS_BLOCK;
typedef struct _VIDEO_REQUEST_PACKET {
    ULONG IoControlCode;
    PSTATUS_BLOCK StatusBlock;
    PVOID InputBuffer;
    ULONG InputBufferLength;
    PVOID OutputBuffer;
    ULONG OutputBufferLength;
} VIDEO_REQUEST_PACKET, *PVIDEO_REQUEST_PACKET;

PFILE_OBJECT pRmFileObject;
PDEVICE_OBJECT pRmDeviceObject;

// for the new interface:
NVDESCRIPTOR fdNv;

BOOL NvRmOpen()
{
	NTSTATUS ntStatus = STATUS_SUCCESS;
	UNICODE_STRING rmUnicodeString;
	pRmFileObject = NULL;
	pRmDeviceObject = NULL;

	// open the resource manager
	//RtlInitUnicodeString(&rmUnicodeString, L"\\Device\\Nvrm");
	RtlInitUnicodeString(&rmUnicodeString, NVRM_NAME_STRING);
	ntStatus = IoGetDeviceObjectPointer(
		&rmUnicodeString,
	 	FILE_ALL_ACCESS,
		&pRmFileObject,
		&pRmDeviceObject
	);
	if (NT_SUCCESS(ntStatus))
	{
		ntStatus = ObReferenceObjectByPointer(
			pRmDeviceObject,
			FILE_ALL_ACCESS,
			NULL,
			KernelMode
		);
	}

	// for the new interface
	fdNv.pFileObject = pRmFileObject;
	fdNv.pDeviceObject = pRmDeviceObject;

	return (NT_SUCCESS(ntStatus)) ? TRUE : FALSE;

} // end of NvRmOpen()

BOOL NvRmClose()
{
	// close the RM by dereferencing both the device and file objects
	if (pRmDeviceObject)
		ObDereferenceObject(pRmDeviceObject);
	if (pRmFileObject)
		ObDereferenceObject(pRmFileObject);

	return TRUE;

} // end of NvRmClose()

BOOL NvRmIoControl(PVIDEO_REQUEST_PACKET RequestPacket)
{
    //PSTATUS_BLOCK StatusBlock;
	IO_STATUS_BLOCK status;
	PIRP pRmIrp;
	
	pRmIrp = IoBuildDeviceIoControlRequest(
		RequestPacket->IoControlCode,
		pRmDeviceObject,
		(PVOID)(*(DWORD *)(RequestPacket->InputBuffer)),
		RequestPacket->OutputBufferLength,
		(PVOID)(*(DWORD *)(RequestPacket->InputBuffer)),
		RequestPacket->OutputBufferLength,
		FALSE,
		NULL,
		&status
	);
	if (pRmIrp)
	{
		IoCallDriver(
			pRmDeviceObject,
			pRmIrp
		);
	}

	return TRUE;

} // end of NvRmIoControl()

BOOL NvrmIoctl(PVIDEO_REQUEST_PACKET RequestPacket)
{
    switch (RequestPacket->IoControlCode)
	{
	    case NV3_IOCTL_ALLOC_CHANNEL_DMA:
			Nv3AllocChannelDma(fdNv, (*(NVOS07_PARAMETERS **)(RequestPacket->InputBuffer)));
			break;

	    case NV3_IOCTL_DMA_FLOW_CONTROL:
			Nv3DmaFlowControl(fdNv, (*(NVOS08_PARAMETERS **)(RequestPacket->InputBuffer)));
			break;

		default:
			return FALSE;
	}
	return TRUE;

} // end of NvRmIoctl()


// end of nvrmcomm.c


