#ifndef _STATE_H_
#define _STATE_H_
//
// (C) Copyright NVIDIA Corporation Inc., 1995-2000. All rights reserved.
//
/***************************** HW State Routines ***************************\
*                                                                           *
* Module: STATE.H                                                           *
*       Hardware State is managed in this module.                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       David Schmenk (dschmenk)     1/06/95 - wrote it.                    *
*                                                                           *
\***************************************************************************/

//---------------------------------------------------------------------------
//
//  State transition message defines.
//
//---------------------------------------------------------------------------

#define STATE_UPDATE        1
#define STATE_LOAD          2
#define STATE_UNLOAD        3
#define STATE_DESTROY       4
#define STATE_INIT          5

//---------------------------------------------------------------------------
//
//  Real mode state transitions.
//
//---------------------------------------------------------------------------

RM_STATUS stateRModeInit(U032);
RM_STATUS stateRModeSave(U032);
RM_STATUS stateRModeRestore(U032);

//---------------------------------------------------------------------------
//
//  Nv device state transitions.
//
//---------------------------------------------------------------------------

RM_STATUS stateNv(PHWINFO, U032);
RM_STATUS stateAu(PHWINFO, U032);
RM_STATUS stateCodec(PHWINFO, U032);
RM_STATUS stateDac(PHWINFO, U032);
RM_STATUS stateDma(PHWINFO, U032);
RM_STATUS stateFb(PHWINFO, U032);
RM_STATUS stateFifo(PHWINFO, U032);
RM_STATUS stateGr(PHWINFO, U032);
RM_STATUS stateGp(PHWINFO, U032);
RM_STATUS stateMc(PHWINFO, U032);
RM_STATUS stateRMode(U032);
RM_STATUS stateTmr(PHWINFO, U032);
RM_STATUS stateVideo(PHWINFO, U032);
RM_STATUS stateSetMode(PHWINFO);
RM_STATUS stateSetMode_DacClassStyle(PHWINFO);
RM_STATUS stateSetModeMultiHead(PHWINFO, PVIDEO_LUT_CURSOR_DAC_OBJECT);

//---------------------------------------------------------------------------
//
//  Nv device state information.
//
//---------------------------------------------------------------------------

RM_STATUS stateConfigGet(PHWINFO, U032, U032*);
RM_STATUS stateConfigSet(PHWINFO, U032, U032, U032*);

#define CONFIG_GETEX_OK        0
#define CONFIG_GETEX_BAD_READ  1 // matches NVOS_CGE_STATUS_ERROR_OPERATING_SYSTEM
#define CONFIG_GETEX_BAD_INDEX 4 // matches NVOS_CGE_STATUS_ERROR_BAD_INDEX
#define CONFIG_GETEX_BAD_PARAM 6 // matches NVOS_CGE_STATUS_ERROR_BAD_PARAM_STRUCT
#define CONFIG_SETEX_OK        0
#define CONFIG_SETEX_BAD_WRITE 1 // matches NVOS_CGE_STATUS_ERROR_OPERATING_SYSTEM
#define CONFIG_SETEX_BAD_INDEX 4 // matches NVOS_CSE_STATUS_ERROR_BAD_INDEX
#define CONFIG_SETEX_BAD_PARAM 6 // matches NVOS_CSE_STATUS_ERROR_BAD_PARAM_STRUCT

RM_STATUS stateConfigGetEx(PHWINFO, U032, VOID *, U032);
RM_STATUS stateConfigSetEx(PHWINFO, U032, VOID *, U032);

//
// Global extern.
//
//extern HWINFO nvInfo;

#if (_WIN32_WINNT >= 0x0500)
extern U032 GetCurrentDPMSState(VOID *);
#endif


#endif // _STATE_H_
