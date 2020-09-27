//
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
//
/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: NVARCH.C                                                          *
*         This is the 32bit DLL interface to the RM for the new architecture*
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Jim Vito (jvito)    03/04/98 - wrote it.                            *
\***************************************************************************/
#include <windows.h>
#include <nvwin32.h>
#include <nvrmwin.h>

//#include "nv_ref.h"
//#include "nvrm.h"
//#include "nvrmwin.h"
//#include "nvcm.h"
//#include <class.h>
//#include <vblank.h>
//#include <gr.h>
//#include <vga.h>
//#include "nvsys.h"

typedef struct _def_rmParams FAR *LPRMPARAMS;
//extern  VOID PASCAL CallRM(LPRMPARAMS);

#define NVRM_DLL32_NV01FREE            NVRM_ARCH_NV01FREE			 
#define NVRM_DLL32_NV01ALLOCROOT       NVRM_ARCH_NV01ALLOCROOT       
#define NVRM_DLL32_NV01ALLOCMEMORY	   NVRM_ARCH_NV01ALLOCMEMORY	 
#define NVRM_DLL32_NV01ALLOCCONTEXTDMA NVRM_ARCH_NV01ALLOCCONTEXTDMA 
#define NVRM_DLL32_NV01ALLOCCHANNELPIO NVRM_ARCH_NV01ALLOCCHANNELPIO 
#define NVRM_DLL32_NV01ALLOCOBJECT	   NVRM_ARCH_NV01ALLOCOBJECT	 
#define NVRM_DLL32_NV01ALLOCDEVICE	   NVRM_ARCH_NV01ALLOCDEVICE	 
#define NVRM_DLL32_NV03ALLOCCHANNELDMA NVRM_ARCH_NV03ALLOCCHANNELDMA 
#define NVRM_DLL32_NV03DMAFLOWCONTROL  NVRM_ARCH_NV03DMAFLOWCONTROL  
#define NVRM_DLL32_NV01INTERRUPT       NVRM_ARCH_NV01INTERRUPT       
#define NVRM_DLL32_NV01ALLOCEVENT	   NVRM_ARCH_NV01ALLOCEVENT		 

#define NVRM_ARCHITECTURE_API          NVRM_API_ARCHITECTURE_32
#define NVOPEN_STATUS_SUCCESS 		   0
#define NVCLOSE_STATUS_SUCCESS		   0

DWORD PASCAL NvSysDispatch(LPRMPARAMS, DWORD);

VOID __stdcall NvIoControl
(
  U032 Function,
  VOID *pParameters
)
{
    RMPARAMS NvParams;

    NvParams.RegEAX = NVRM_ARCHITECTURE_API;
    NvParams.RegEDX = (DWORD)Function;
    NvParams.RegEBX = (DWORD)pParameters;
    NvParams.RegEDI = 1;				  // The device context will be assigned in the RM.
    NvParams.RegES  = 0;
    _asm xor eax, eax
    _asm mov ax, ds
    _asm mov NvParams.RegDS, eax
    _asm mov ax, cs
    _asm mov NvParams.RegCS, eax
    NvSysDispatch(&NvParams, 0);
}	


VOID Arch_NvOpen(U032 *status)                   
{
  *status = NVOPEN_STATUS_SUCCESS;
}

VOID Arch_NvClose(U032 *status)                  
{
  *status = NVCLOSE_STATUS_SUCCESS;
}

VOID Arch_Nv01Free(VOID *pParameters)            
{
  NvIoControl(NVRM_DLL32_NV01FREE,pParameters);
}

VOID Arch_Nv01AllocRoot(VOID *pParameters)       
{
  NvIoControl(NVRM_DLL32_NV01ALLOCROOT,pParameters);
}

VOID Arch_Nv01AllocDevice(VOID *pParameters)     
{
  NvIoControl(NVRM_DLL32_NV01ALLOCDEVICE,pParameters);
}

VOID Arch_Nv01AllocContextDma(VOID *pParameters) 
{
  NvIoControl(NVRM_DLL32_NV01ALLOCCONTEXTDMA,pParameters);
}

VOID Arch_Nv01AllocChannelPio(VOID *pParameters) 
{
  NvIoControl(NVRM_DLL32_NV01ALLOCCHANNELPIO,pParameters);
}

VOID Arch_Nv01AllocObject(VOID *pParameters)     
{
  NvIoControl(NVRM_DLL32_NV01ALLOCOBJECT,pParameters);
}

VOID Arch_Nv01AllocMemory(VOID *pParameters)     
{
  NvIoControl(NVRM_DLL32_NV01ALLOCMEMORY,pParameters);
}

VOID Arch_Nv01Interrupt(VOID *pParameters)       
{
  NvIoControl(NVRM_DLL32_NV01INTERRUPT,pParameters);
}

VOID Arch_Nv01AllocEvent(VOID *pParameters)      
{
  NvIoControl(NVRM_DLL32_NV01ALLOCEVENT,pParameters);
}

