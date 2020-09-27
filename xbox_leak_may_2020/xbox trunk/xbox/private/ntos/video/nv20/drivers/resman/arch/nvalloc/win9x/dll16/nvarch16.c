//
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
//
/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: NVARCH.C                                                          *
*         This is the 16bit DLL interface to the RM for the new architecture*
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Jim Vito (jvito)    03/04/98 - wrote it.                            *
\***************************************************************************/
#include <windows.h>
#include "nv_ref.h"
#include "nvrm.h"
#include "nvrmwin.h"
#include "nvcm.h"
#include <class.h>
#include <vblank.h>
#include <gr.h>
#include <vga.h>
#include "nvsys.h"

extern  void CallRM_DeviceSetup(LPRMPARAMS lprmParams);

#define NVRM_DLL16_NV01FREE            NVRM_ARCH_NV01FREE            
#define NVRM_DLL16_NV01ALLOCROOT       NVRM_ARCH_NV01ALLOCROOT       
#define NVRM_DLL16_NV01ALLOCMEMORY     NVRM_ARCH_NV01ALLOCMEMORY     
#define NVRM_DLL16_NV01ALLOCCONTEXTDMA NVRM_ARCH_NV01ALLOCCONTEXTDMA 
#define NVRM_DLL16_NV01ALLOCCHANNELPIO NVRM_ARCH_NV01ALLOCCHANNELPIO 
#define NVRM_DLL16_NV01ALLOCOBJECT     NVRM_ARCH_NV01ALLOCOBJECT     
#define NVRM_DLL16_NV01ALLOCDEVICE     NVRM_ARCH_NV01ALLOCDEVICE     
#define NVRM_DLL16_NV03ALLOCCHANNELDMA NVRM_ARCH_NV03ALLOCCHANNELDMA 
#define NVRM_DLL16_NV03DMAFLOWCONTROL  NVRM_ARCH_NV03DMAFLOWCONTROL  
#define NVRM_DLL16_NV01INTERRUPT       NVRM_ARCH_NV01INTERRUPT       
#define NVRM_DLL16_NV01ALLOCEVENT      NVRM_ARCH_NV01ALLOCEVENT      
#define NVRM_DLL16_NVI2CACCESS         NVRM_ARCH_I2C_ACCESS      
#define NVRM_DLL16_NV04CONFIGGETEX     NVRM_ARCH_CONFIG_GET_EX       
#define NVRM_DLL16_NV04CONFIGSETEX     NVRM_ARCH_CONFIG_SET_EX       
#define NVRM_DLL16_NV04ALLOC           NVRM_ARCH_NV04ALLOC

#define NVRM_ARCHITECTURE_API          NVRM_API_ARCHITECTURE_16
#define NVOPEN_STATUS_SUCCESS          0
#define NVCLOSE_STATUS_SUCCESS         0

extern U032 FAR * lp16bitProcessDS;

VOID FAR PASCAL NvIoControl
(
  U032 Function,
  VOID FAR *pParameters
)
{
  WORD seg_,off_;
  DWORD seg_base,parameters_flat_offset;
  RMPARAMS rmParams;

  _asm push es
  _asm push di

  _asm les  di,pParameters
  _asm mov  seg_,es
  _asm mov  off_,di

  _asm pop  di
  _asm pop  es

  seg_base = GetSelectorBase((UINT) seg_);
  parameters_flat_offset = seg_base+((DWORD) off_);

  //
  // Call the resource manager and have all of the work done over there.
  // If needed this can send a pointer to an interface structure instead. I think we will run out of registers
  // otherwise. 
  rmParams.RegEAX = NVRM_ARCHITECTURE_API;
  rmParams.RegEBX = parameters_flat_offset;
  rmParams.RegEDX = Function;
  rmParams.RegESI = DEFAULT_DEVICEHANDLE;
  rmParams.RegES  = 0;
  CallRM_DeviceSetup(&rmParams);
}   


VOID FAR PASCAL Arch_NvOpen(U032 *status)                   
{
  *status = NVOPEN_STATUS_SUCCESS;
}

VOID FAR PASCAL Arch_NvClose(U032 *status)                  
{
  *status = NVCLOSE_STATUS_SUCCESS;
}

VOID FAR PASCAL Arch_Nv01Free(VOID FAR *pParameters)            
{
  NvIoControl(NVRM_DLL16_NV01FREE,pParameters);
}

VOID FAR PASCAL Arch_Nv01AllocRoot(VOID FAR *pParameters)       
{
  NvIoControl(NVRM_DLL16_NV01ALLOCROOT,pParameters);
}

VOID FAR PASCAL Arch_Nv04Alloc(VOID FAR *pParameters)       
{
  NvIoControl(NVRM_DLL16_NV04ALLOC,pParameters);
}

VOID FAR PASCAL Arch_Nv01AllocMemory(VOID FAR *pParameters)     
{
  NvIoControl(NVRM_DLL16_NV01ALLOCMEMORY,pParameters);
}

VOID FAR PASCAL Arch_Nv01AllocContextDma(VOID FAR *pParameters) 
{
  NvIoControl(NVRM_DLL16_NV01ALLOCCONTEXTDMA,pParameters);
}

VOID FAR PASCAL Arch_Nv01AllocChannelPio(VOID FAR *pParameters) 
{
  NvIoControl(NVRM_DLL16_NV01ALLOCCHANNELPIO,pParameters);
}

VOID FAR PASCAL Arch_Nv01AllocObject(VOID FAR *pParameters)     
{
  NvIoControl(NVRM_DLL16_NV01ALLOCOBJECT,pParameters);
}

VOID FAR PASCAL Arch_Nv01AllocDevice(VOID FAR *pParameters)     
{
  NvIoControl(NVRM_DLL16_NV01ALLOCDEVICE,pParameters);
}

VOID FAR PASCAL Arch_Nv01AllocChannelDma(VOID FAR *pParameters)     
{
  NvIoControl(NVRM_DLL16_NV03ALLOCCHANNELDMA,pParameters);
}
VOID FAR PASCAL Arch_Nv03DmaFlowControl(VOID FAR *pParameters)
{
  NvIoControl(NVRM_DLL16_NV03DMAFLOWCONTROL,pParameters);
}
VOID FAR PASCAL Arch_Nv01Interrupt(VOID FAR *pParameters)       
{
  NvIoControl(NVRM_DLL16_NV01INTERRUPT,pParameters);
}

VOID FAR PASCAL Arch_Nv01AllocEvent(VOID FAR *pParameters)      
{
  NvIoControl(NVRM_DLL16_NV01ALLOCEVENT,pParameters);
}

VOID FAR PASCAL Arch_NvI2CAccess(VOID FAR *pParameters)      
{
  NvIoControl(NVRM_DLL16_NVI2CACCESS,pParameters);
}

VOID FAR PASCAL Arch_Nv04ConfigGetEx(VOID FAR *pParameters)      
{
  NvIoControl(NVRM_DLL16_NV04CONFIGGETEX,pParameters);
}

VOID FAR PASCAL Arch_Nv04ConfigSetEx(VOID FAR *pParameters)      
{
  NvIoControl(NVRM_DLL16_NV04CONFIGSETEX,pParameters);
}

