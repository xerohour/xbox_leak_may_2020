/*
==============================================================================

    NVIDIA Corporation, (C) Copyright 
    1996,1997,1998,1999. All rights reserved.

    nvpekmvt.h

    Local definitions used by kernel mode video transport

    Dec 17, 1999

==============================================================================
*/

#ifndef _NVPEKMVT_H
#define _NVPEKMVT_H

typedef unsigned long	DWORD;

#include "dxmini.h"


#ifndef NTSTATUS
typedef LONG NTSTATUS;
#endif
#define PDEVICE_OBJECT	PVOID
#include "i2cgpio.h"

extern NTSTATUS miniI2COpen (PDEVICE_OBJECT pDev, ULONG acquire, PI2CControl pI2CCtrl);
extern NTSTATUS miniI2CAccess (PDEVICE_OBJECT pDev, PI2CControl	pI2CCtrl);

/// redefinition/re-mapping of some KMVT data structures so that we can share same code with Win9x
typedef VOID (*PNVP_DX_IRQCALLBACK)(void);

typedef DDVIDEOPORTDATA              * PNVP_DDVIDEOPORTDATA;
typedef DDSKIPNEXTFIELDINFO          * PNVP_DDSKIPNEXTFIELDINFO;
typedef DDBOBNEXTFIELDINFO           * PNVP_DDBOBNEXTFIELDINFO;
typedef DDSETSTATEININFO             * PNVP_DDSETSTATEININFO;
typedef DDSETSTATEOUTINFO            * PNVP_DDSETSTATEOUTINFO;
typedef DDLOCKININFO                 * PNVP_DDLOCKININFO;
typedef DDLOCKOUTINFO                * PNVP_DDLOCKOUTINFO;
typedef DDFLIPOVERLAYINFO            * PNVP_DDFLIPOVERLAYINFO;
typedef DDFLIPVIDEOPORTINFO          * PNVP_DDFLIPVIDEOPORTINFO;
typedef DDGETPOLARITYININFO          * PNVP_DDGETPOLARITYININFO;
typedef DDGETPOLARITYOUTINFO         * PNVP_DDGETPOLARITYOUTINFO;
typedef DDGETCURRENTAUTOFLIPININFO   * PNVP_DDGETCURRENTAUTOFLIPININFO;
typedef DDGETCURRENTAUTOFLIPOUTINFO  * PNVP_DDGETCURRENTAUTOFLIPOUTINFO;
typedef DDGETPREVIOUSAUTOFLIPININFO  * PNVP_DDGETPREVIOUSAUTOFLIPININFO;
typedef DDGETPREVIOUSAUTOFLIPOUTINFO * PNVP_DDGETPREVIOUSAUTOFLIPOUTINFO;
typedef DDTRANSFERININFO             * PNVP_DDTRANSFERININFO;
typedef DDTRANSFEROUTINFO            * PNVP_DDTRANSFEROUTINFO;
typedef DDGETTRANSFERSTATUSOUTINFO   * PNVP_DDGETTRANSFEROUTINFO;

/////////////////
// prototypes
//
DWORD GetIrqInfo(
    PVOID HwDeviceExtension,
    PVOID lpInput,
    PDDGETIRQINFO GetIrqInfo);

VP_STATUS NVQueryInterface(
    PVOID HwDeviceExtension,
    PQUERY_INTERFACE QueryInterface);

/// external, defined in nvpecore.c
DWORD FlipVideoPort(
    PVOID HwDeviceExtension,
    PNVP_DDFLIPVIDEOPORTINFO pFlipVideoPort,
    PVOID lpOutput);
DWORD FlipOverlay(
    PVOID HwDeviceExtension,
    PNVP_DDFLIPOVERLAYINFO pFlipOverlayInfo,
    PVOID lpOutput);
DWORD BobNextField(
    PVOID HwDeviceExtension,
    PNVP_DDBOBNEXTFIELDINFO pBobNextFieldInfo,
    PVOID lpOutput);
DWORD SetState(
    PVOID HwDeviceExtension,
    PNVP_DDSETSTATEININFO pSetStateInInfo,
    PNVP_DDSETSTATEOUTINFO pSetStateOutInfo);
DWORD SkipNextField(
    PVOID HwDeviceExtension,
    PNVP_DDSKIPNEXTFIELDINFO pSkipNextFieldInfo,
    PVOID lpOutput);
DWORD GetPolarity(
    PVOID HwDeviceExtension,
    PDDGETPOLARITYININFO pGetPolarityInInfo,
    PDDGETPOLARITYOUTINFO pGetPolarityOutInfo);
DWORD GetCurrentAutoflip(
    PVOID HwDeviceExtension,
    PNVP_DDGETCURRENTAUTOFLIPININFO pGetCurrentAutoflipInInfo,
    PNVP_DDGETCURRENTAUTOFLIPOUTINFO pGetAutoFlipInfoOut);
DWORD GetPreviousAutoflip(
    PVOID HwDeviceExtension,
    PNVP_DDGETPREVIOUSAUTOFLIPININFO pGetAutoflipInInfo,
    PNVP_DDGETPREVIOUSAUTOFLIPOUTINFO pGetAutoFlipInfoOut);
DWORD NVPTransfer(
	PVOID pContext, 
	PNVP_DDTRANSFERININFO pInInfo, 
	PNVP_DDTRANSFEROUTINFO pOutInfo);
DWORD NVPGetTransferStatus(
	PVOID pContext, 
	PVOID pNotUsed, 
	PNVP_DDGETTRANSFEROUTINFO pOutInfo);

DWORD LockSurface(
    PVOID HwDeviceExtension,
    PNVP_DDLOCKININFO pLockInfoIn, 
    PNVP_DDLOCKOUTINFO pLockInfoOut);

#endif // _NVPEKMVT_H