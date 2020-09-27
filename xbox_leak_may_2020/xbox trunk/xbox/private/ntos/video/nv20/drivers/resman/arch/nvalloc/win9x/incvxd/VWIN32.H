/****************************************************************************
*                                                                           *
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
* KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
* PURPOSE.                                                                  *
*                                                                           *
* Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
*                                                                           *
****************************************************************************/

/******************************************************************************
 *
 *   Title:	vwin32.h
 *
 *   Version:	4.00
 *
 ******************************************************************************/

/*INT32*/

#ifndef Not_VxD

/*XLATOFF*/
#define VWIN32_Service	Declare_Service
#pragma warning (disable:4003)		// turn off not enough params warning
/*XLATON*/

/*MACROS*/
Begin_Service_Table(VWIN32)

VWIN32_Service	(VWIN32_Get_Version, LOCAL)
VWIN32_Service	(VWIN32_DIOCCompletionRoutine, LOCAL)
VWIN32_Service	(_VWIN32_QueueUserApc)
VWIN32_Service	(_VWIN32_Get_Thread_Context)
VWIN32_Service	(_VWIN32_Set_Thread_Context)
VWIN32_Service	(_VWIN32_CopyMem, LOCAL)
VWIN32_Service	(_VWIN32_Npx_Exception)
VWIN32_Service	(_VWIN32_Emulate_Npx)
VWIN32_Service	(_VWIN32_CheckDelayedNpxTrap)
VWIN32_Service	(VWIN32_EnterCrstR0)
VWIN32_Service	(VWIN32_LeaveCrstR0)
VWIN32_Service	(_VWIN32_FaultPopup)
VWIN32_Service	(VWIN32_GetContextHandle)
VWIN32_Service	(VWIN32_GetCurrentProcessHandle, LOCAL)
VWIN32_Service	(_VWIN32_SetWin32Event)
VWIN32_Service	(_VWIN32_PulseWin32Event)
VWIN32_Service	(_VWIN32_ResetWin32Event)
VWIN32_Service	(_VWIN32_WaitSingleObject)
VWIN32_Service	(_VWIN32_WaitMultipleObjects)
VWIN32_Service	(_VWIN32_CreateRing0Thread)
VWIN32_Service	(_VWIN32_CloseVxDHandle)
VWIN32_Service	(VWIN32_ActiveTimeBiasSet, LOCAL)
VWIN32_Service	(VWIN32_GetCurrentDirectory, LOCAL)
VWIN32_Service	(VWIN32_BlueScreenPopup)
VWIN32_Service	(VWIN32_TerminateApp)
VWIN32_Service	(_VWIN32_QueueKernelAPC)
VWIN32_Service	(VWIN32_SysErrorBox)
VWIN32_Service	(_VWIN32_IsClientWin32)
VWIN32_Service	(VWIN32_IFSRIPWhenLev2Taken, LOCAL)

End_Service_Table(VWIN32)
/*ENDMACROS*/

/*XLATOFF*/
#pragma warning (default:4003)		// turn on not enough params warning
/*XLATON*/

#endif // Not_VxD

//
// structure for VWIN32_SysErrorBox
//

typedef struct vseb_s {
    DWORD vseb_resp;
    WORD vseb_b3;
    WORD vseb_b2;
    WORD vseb_b1;
    DWORD vseb_pszCaption;
    DWORD vseb_pszText;
} VSEB;

typedef VSEB *PVSEB;

// VWIN32_QueueKernelAPC flags

#define KERNEL_APC_IGNORE_MC 		0x00000001
#define KERNEL_APC_STATIC 		0x00000002
#define	KERNEL_APC_WAKE			0x00000004

// for DeviceIOControl support
// On a DeviceIOControl call vWin32 will pass following parameters to
// the Vxd that is specified by hDevice. hDevice is obtained thru an
// earlier call to hDevice = CreateFile("\\.\vxdname", ...);
// ESI = DIOCParams STRUCT (defined below)
typedef struct DIOCParams	{
	DWORD	Internal1;		// ptr to client regs
	DWORD	VMHandle;		// VM handle
	DWORD	Internal2;		// DDB
	DWORD	dwIoControlCode;
	DWORD	lpvInBuffer;
	DWORD	cbInBuffer;
	DWORD	lpvOutBuffer;
	DWORD	cbOutBuffer;
	DWORD	lpcbBytesReturned;
	DWORD	lpoOverlapped;
	DWORD	hDevice;
	DWORD	tagProcess;
} DIOCPARAMETERS;

typedef DIOCPARAMETERS *PDIOCPARAMETERS;

// dwIoControlCode values for vwin32's DeviceIOControl Interface
// all VWIN32_DIOC_DOS_ calls require lpvInBuffer abd lpvOutBuffer to be
// struct * DIOCRegs
#define	VWIN32_DIOC_GETVERSION DIOC_GETVERSION
#define	VWIN32_DIOC_DOS_IOCTL	1
#define	VWIN32_DIOC_DOS_INT25	2
#define	VWIN32_DIOC_DOS_INT26	3
#define	VWIN32_DIOC_DOS_INT13	4
#define VWIN32_DIOC_SIMCTRLC	5
#define	VWIN32_DIOC_CLOSEHANDLE DIOC_CLOSEHANDLE

// DIOCRegs
// Structure with i386 registers for making DOS_IOCTLS
// vwin32 DIOC handler interprets lpvInBuffer , lpvOutBuffer to be this struc.
// and does the int 21
// reg_flags is valid only for lpvOutBuffer->reg_Flags
typedef struct DIOCRegs	{
	DWORD	reg_EBX;
	DWORD	reg_EDX;
	DWORD	reg_ECX;
	DWORD	reg_EAX;
	DWORD	reg_EDI;
	DWORD	reg_ESI;
	DWORD	reg_Flags;		
} DIOC_REGISTERS;

// if we are not included along with winbase.h
#ifndef FILE_FLAG_OVERLAPPED
  // OVERLAPPED structure for DeviceIOCtl VxDs
  typedef struct _OVERLAPPED {
          DWORD O_Internal;
          DWORD O_InternalHigh;
          DWORD O_Offset;
          DWORD O_OffsetHigh;
          HANDLE O_hEvent;
  } OVERLAPPED;
#endif

