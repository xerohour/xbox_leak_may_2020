//
// (C) Copyright NVIDIA Corporation Inc., 1995,1996. All rights reserved.
//
/******************* Operating System Interface Routines *******************\
*                                                                           *
* Module: NVDSPTCH.C                                                        *
*         This is the VxD Dispatcher for the NvArchitecture                 *  
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Jim Vito (jvito)    03/04/98 - wrote it.                            *
*                                    - and stole from JeffW :)  			*
\***************************************************************************/
#include "basedef.h"
#include "vmmtypes.h"
#include "vmm.h"		// put this first. we redefine macro SF_BIT in nvrm.h
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <vblank.h>
#include <dma.h>
#include <fifo.h>
#include <gr.h>
#include <dac.h>
#include <os.h>
#include "nvrmwin.h"
#include "nvhw.h"
#include "vpicd.h"
#include "vdd.h"
#include "vnvrmd.h"
#include "oswin.h"
#include "vmm2.h"		// more vmm services
#include "vwin32.h"
//#include "tempnvos.h"

#include "nvos.h"
#include "nvarch.h"
#include "nv32.h"
#include "nvwin32.h"
#include "nvhw.h"

// Prototypes for WinNT functions added.
// extern CLASS classTable[NUM_CLASSES];

VOID NvArchRMDispatch
(
   U032  function,
   PVOID paramPtr
)
{
  switch (function){
    case NVRM_ARCH_NV01FREE:
	{
      Nv01Free((NVOS00_PARAMETERS *) paramPtr);
	}
    break;
    
    case NVRM_ARCH_NV04ALLOC:
	{
      Nv04Alloc((NVOS21_PARAMETERS *)paramPtr);
	}
    break;
    
    case NVRM_ARCH_NV01ALLOCROOT:
	{
      Nv01AllocRoot((NVOS01_PARAMETERS *)paramPtr);
	}
    break;
    
    case NVRM_ARCH_NV01ALLOCMEMORY:
	{
      Nv01AllocMemory((NVOS02_PARAMETERS *)paramPtr);
	}
    break;

    case NVRM_ARCH_NV01ALLOCCONTEXTDMA:
	{
	  NVOS03_PARAMETERS *parameters;

	  parameters = (NVOS03_PARAMETERS *) paramPtr;
      
      Nv01AllocContextDma(parameters);
	}
    break;

	// For each dispatch to the architecture there should be a methodic process.
    // First add the objects to the architecture object structure.
    case NVRM_ARCH_NV01ALLOCCHANNELPIO:
	{
      Nv01AllocChannelPio((NVOS04_PARAMETERS *)paramPtr);
	}
    break;

    case NVRM_ARCH_NV01ALLOCOBJECT:
	{
	  Nv01AllocObject((NVOS05_PARAMETERS *) paramPtr);
	}
    break;

    case NVRM_ARCH_NV01ALLOCDEVICE:
	{
      Nv01AllocDevice((NVOS06_PARAMETERS *)paramPtr);
	}
    break;

    case NVRM_ARCH_NV03ALLOCCHANNELDMA:
	{
      Nv03AllocChannelDma((NVOS07_PARAMETERS *)paramPtr);
	}
    break;

    case NVRM_ARCH_NV03DMAFLOWCONTROL:
	{
      Nv03DmaFlowControl((NVOS08_PARAMETERS *)paramPtr);
	}
    break;


    case NVRM_ARCH_NV01INTERRUPT:
	{
      Nv01Interrupt((NVOS09_PARAMETERS *)paramPtr);
	}
    break;

    case NVRM_ARCH_NV01ALLOCEVENT:
	{
	  Nv01AllocEvent((NVOS10_PARAMETERS *) paramPtr);
	}
    break;

    case NVRM_ARCH_NV03HEAP:
	{
	  Nv03ArchHeap((NVOS11_PARAMETERS *) paramPtr);
	}
    break;

    case NVRM_ARCH_CONFIG_VERSION:
	{
      Nv01ConfigVersion((NVOS12_PARAMETERS *)paramPtr);
	}
    break;

    case NVRM_ARCH_CONFIG_GET:
	{
      Nv01ConfigGet((NVOS13_PARAMETERS *)paramPtr);
	}
    break;

    case NVRM_ARCH_CONFIG_SET:
	{
      Nv01ConfigSet((NVOS14_PARAMETERS *)paramPtr);
	}
    break;

    case NVRM_ARCH_CONFIG_UPDATE:
	{
      Nv01ConfigUpdate((NVOS15_PARAMETERS *)paramPtr);
	}
    break;

    case NVRM_ARCH_RING0CALLBACK:
	{
      NvRing0Callback((NVRM_RING0CALLBACK_PARAMS *)paramPtr);
	}
    break;

    case NVRM_ARCH_CONFIG_GET_EX:
    {
      Nv04ConfigGetEx((NVOS_CONFIG_GET_EX_PARAMS *)paramPtr);
    }
    break;

    case NVRM_ARCH_CONFIG_SET_EX:
    {
      Nv04ConfigSetEx((NVOS_CONFIG_SET_EX_PARAMS *)paramPtr);
    }
    break;

    case NVRM_ARCH_I2C_ACCESS:
    {
      Nv04I2CAccess((NVOS_I2C_ACCESS_PARAMS *)paramPtr);
    }
    break;

    default:
    break;
  }
} // end of NvArchRMDispatch

