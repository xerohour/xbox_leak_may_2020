////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef VDGENDRV_H
#define VDGENDRV_H

#define ONLY_EXTERNAL_VISIBLE

#include "library\common\prelude.h"

class GenericHook;


extern "C" {
DLLCALL	void	WINAPI VDR_RegisterCallback(GenericHook * hook);
DLLCALL	void	WINAPI VDR_UnregisterCallback(GenericHook * hook);
#define H20_RegisterCallback	VDR_RegisterCallback
}


#include "library\common\gnerrors.h"
#include "library\common\tags.h"
#include "library\common\tagunits.h"
#include "library\common\winports.h"

#include "library\hardware\videodma\generic\pip.h"

#include "library\hardware\video\generic\vidchip.h"


#include "library\hardware\video\generic\videnc.h"

#include "library\hardware\mpeg2dec\generic\mpeg2dec.h"
#ifndef NO_INTERNAL_INCLUDE
#include "library\hardware\mpeg2dec\generic\mp2dcryp.h"
#endif

#if BOARD_HAS_MPEG2_ENCODER
#include "library\hardware\mpeg2enc\generic\mpeg2enc.h"
#endif



typedef class __far UnitSetClass __far *UnitSet;
typedef class __far VDRHandleClass __far *VDRHandle;

extern "C" {

DLLCALL void VDR_ServiceProcess(void);

DLLCALL Error WINAPI VDR_OpenDriver(TCHAR * name, DWORD boardID, VDRHandle __far & handle);
DLLCALL Error WINAPI VDR_CloseDriver(VDRHandle handle);

DLLCALL DWORD WINAPI VDR_AvailUnits(VDRHandle handle);

DLLCALL Error WINAPI VDR_ReconfigureDriver(VDRHandle handle);

DLLCALL Error WINAPI VDR_OpenUnits(VDRHandle handle, DWORD requnits, UnitSet &units);
DLLCALL Error WINAPI VDR_OpenSubUnits(UnitSet parent, DWORD requnits, UnitSet &units);
DLLCALL Error WINAPI VDR_CloseUnits(UnitSet units);


DLLCALL Error WINAPI VDR_ConfigureUnits(UnitSet units, TAG * tags);
inline Error __cdecl VDR_ConfigureUnitsTags(UnitSet units, TAG tags, ...) {return VDR_ConfigureUnits(units, &tags);}

DLLCALL Error WINAPI VDR_LockUnits(UnitSet units);
DLLCALL Error WINAPI VDR_UnlockUnits(UnitSet units);
DLLCALL Error WINAPI VDR_ActivateUnits(UnitSet units);
DLLCALL Error WINAPI VDR_PassivateUnits(UnitSet units);


DLLCALL Error WINAPI VDR_EnablePIP(UnitSet units, BOOL enable);
DLLCALL Error WINAPI VDR_UpdatePIP(UnitSet units);
DLLCALL Error WINAPI VDR_GrabFrame(UnitSet units, FPTR base,
                                   WORD width, WORD height,
                                   WORD stride,
                                   GrabFormat fmt);

DLLCALL Error WINAPI VDR_OptimizeBuffer(UnitSet units, WORD __far & minPixVal);


//
// MPEG Functions
//
DLLCALL DWORD	WINAPI VDR_SendMPEGData(UnitSet units, HPTR data, DWORD size);
DLLCALL void	WINAPI VDR_CompleteMPEGData(UnitSet units);
DLLCALL DWORD	WINAPI VDR_SendMPEGDataMultiple(UnitSet units, MPEGDataSizePair * data, DWORD size);

DLLCALL DWORD	WINAPI VDR_SendMPEGDataSplit(UnitSet units, MPEGElementaryStreamType streamType, HPTR data, DWORD size, LONG timeStamp);
DLLCALL void	WINAPI VDR_CompleteMPEGDataSplit(UnitSet units, MPEGElementaryStreamType streamType);
DLLCALL DWORD  WINAPI VDR_SendMPEGDataSplitMultiple(UnitSet units, MPEGElementaryStreamType streamType, MPEGDataSizePair * data, DWORD size, int timeStamp);
DLLCALL void	WINAPI VDR_RestartMPEGDataSplit(UnitSet units, MPEGElementaryStreamType streamType);

DLLCALL Error	WINAPI VDR_SendMPEGCommand(UnitSet units, MPEGCommand com, long param, DWORD __far &tag);
DLLCALL Error	WINAPI VDR_DoMPEGCommand(UnitSet units, MPEGCommand com, long param);
DLLCALL Error	WINAPI VDR_CompleteMPEGCommand(UnitSet units, DWORD tag);
DLLCALL BOOL	WINAPI VDR_MPEGCommandPending(UnitSet units, DWORD tag);

DLLCALL DWORD	WINAPI VDR_CurrentMPEGLocation(UnitSet units);
DLLCALL DWORD	WINAPI VDR_CurrentMPEGTransferLocation(UnitSet units);
DLLCALL MPEGState WINAPI VDR_CurrentMPEGState(UnitSet units);

DLLCALL Error	WINAPI VDR_InstallMPEGWinHooks(UnitSet units, HWND hwnd, WORD refillMsg, WORD signalMsg, WORD doneMsg);
DLLCALL Error	WINAPI VDR_RemoveMPEGWinHooks(UnitSet units);
DLLCALL Error	WINAPI VDR_CompleteMPEGRefillMessage(UnitSet units);

DLLCALL Error  WINAPI VDR_InstallMessagePort(UnitSet units, GenericMsgPort * port);
DLLCALL Error  WINAPI VDR_RemoveMessagePort(UnitSet units);

#ifndef NO_INTERNAL_INCLUDE
DLLCALL Error	WINAPI VDR_DoAuthenticationCommand(UnitSet units, MPEG2DVDAuthenticationCommand com, DWORD sector, BYTE __far * key);
DLLCALL BOOL	WINAPI VDR_CheckRegionCodeValid(UnitSet units, BYTE regionSet);
#endif


#if BOARD_HAS_MPEG2_ENCODER

DLLCALL Error  WINAPI VDR_DoCommand(UnitSet units, MPEGCommand com, long param);
DLLCALL Error	WINAPI VDR_PutBuffer(UnitSet units, HPTR data, DWORD size);
DLLCALL Error  WINAPI VDR_InstallMPEGTransferDoneHook(UnitSet units, HWND hwnd, WORD transferDoneMsg);
DLLCALL Error  WINAPI VDR_RemoveMPEGTransferDoneHook(UnitSet units);
DLLCALL Error  WINAPI VDR_InstallMPEGEncoderBufferStarvingHook(UnitSet units, HWND hwnd, WORD bufferStarvingMsg);
DLLCALL Error  WINAPI VDR_RemoveMPEGEncoderBufferStarvingHook(UnitSet units);

#endif



	}

#endif
