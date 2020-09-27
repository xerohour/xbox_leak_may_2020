/*----------------------------------------------------------------------------*/
/*
 * nvRmR0Api.c
 *
 *  Routines to interact with the resource manager VxD's API services.
 *  
 */

/*----------------------------------------------------------------------------*/
/*
 * include files
 */

// #define WANTVXDWRAPS 
#include <basedef.h>
#include <vmm.h>
#include <vmmreg.h>
#include <vxdldr.h>
//#include <debug.h>
//#include <vxdwraps.h>
#include <vwin32.h>

#include "nvtypes.h"
#include "nvrmr0api.h"
#include "nvrmwin.h"
#include "nvos.h"
#include "files.h"
#include "nvmisc.h"
#include "nvReg.h"

/*----------------------------------------------------------------------------*/
/*
 * pragma's to put the code and data into locked memory (XXX not sure if needed)
 */

#pragma VxD_LOCKED_CODE_SEG
#pragma VxD_LOCKED_DATA_SEG

/*----------------------------------------------------------------------------*/
/*
 * globals
 */

typedef DWORD (* LPDWFN)();
static LPDWFN apiEntry = NULL;
LPDWFN getApiProcR0(char*);

static char strDevNodeRM[128] = NV4_REG_GLOBAL_BASE_PATH "\\" NV4_REG_RESOURCE_MANAGER_SUBKEY;
static char strRmCoreName[] = STR_RM_CORE_NAME;

static CHAR VxdName[32];
#ifdef NV3_HW
static CHAR defaultVxd[] = VXDLDR_NV3_RESMAN_VXD_NAME;
#else
static CHAR defaultVxd[] = VXDLDR_NVX_RESMAN_VXD_NAME;
#endif

#define DEFAULT_DEVICE 0
    
/*----------------------------------------------------------------------------*/
/*
 * nvGetResManAPIEntry - get the API entry point for the resource manager VxD
 */

static PVOID nvGetResManAPIEntry()
{
    ULONG*      pdevinfo = NULL;
    ULONG       size = 32;
    ULONG       hkSW;
    ULONG       cbType;
    ULONG       pos;
    ULONG       status;
    
#ifdef NV3_HW

    //
    // For NV3, don't check registry for RM Core VxD's name. Use the default name.
    //

    // (do string manipulation directly so we don't have to link in the standard str functions)
    pos = 0;
    while ( defaultVxd[pos] ) {
      VxdName[pos] = defaultVxd[pos];
      pos++;
    }
    VxdName[pos] = '\0';

#else

    //
    // For NV4 and up, attempt to get the RM Core VxD's name from the registdry
    //
    status = VMM_RegOpenKey(HKEY_LOCAL_MACHINE, strDevNodeRM, (PVMMHKEY)&hkSW);

    if ( (status == 0) && hkSW ) {
        cbType = REG_BINARY;
        status = VMM_RegQueryValueEx(hkSW, strRmCoreName, NULL, &cbType, &(VxdName[0]), &size);
        VMM_RegCloseKey(hkSW);
    }

    // status = 1; // force use of default VxD name

    if ( status != 0 ) {
        // no registry entry so use default VxD name
        // (do string manipulation directly so we don't have to link in the standard str functions)
        pos = 0;
        while ( defaultVxd[pos] ) {
          VxdName[pos] = defaultVxd[pos];
          pos++;
        }
        VxdName[pos] = '\0';
    }

#endif

    apiEntry = getApiProcR0(VxdName);
    if ( apiEntry == NULL ) {
        // try again with default Vxd name
        apiEntry = getApiProcR0(defaultVxd);
    }

    return(apiEntry);
}

/*----------------------------------------------------------------------------*/
/*
 * callRmAPI - call Resman VxD's API entry point
 */

ULONG callRmAPI(ULONG service, ULONG function, PVOID params)
{

    CLIENT_STRUCT cs;
    CLIENT_STRUCT * pcs = &cs;
    RMUSRAPI_STRUCT rm_struct;
    
    rm_struct.function = service;
    rm_struct.device = DEFAULT_DEVICE;
    cs.CRS.Client_EAX = (ULONG) &rm_struct;
    //cs.CRS.Client_EAX = service;
    

    cs.CRS.Client_EDX = function;
    cs.CRS.Client_EBX = (ULONG)params;
    
    _asm push ebp
    _asm mov  ebp, pcs      ; point ebp to client reg struct 
    
    (*apiEntry)();
    _asm pop  ebp
    
    /* return EAX since resman seems to return most of the "good" stuff there */
    return(cs.CRS.Client_EAX);
}

/*----------------------------------------------------------------------------*/
/*
 * callRmArchAPI - call Resman VxD's API entry point and use the "architecture" 
 *         service
 */

VOID callRmArchAPI(ULONG function, PVOID params)
{

    CLIENT_STRUCT cs;
    CLIENT_STRUCT * pcs = &cs;
    RMUSRAPI_STRUCT rm_struct;
    
    rm_struct.function = NVRM_API_ARCHITECTURE_32;
    rm_struct.device = DEFAULT_DEVICE;
    cs.CRS.Client_EAX = (ULONG) &rm_struct;
    //cs.CRS.Client_EAX = NVRM_API_ARCHITECTURE_32;

    cs.CRS.Client_EDX = function;
    cs.CRS.Client_EBX = (ULONG)params;
    
    _asm push ebp
    _asm mov  ebp, pcs      ; point ebp to client reg struct 
    
    (*apiEntry)();
    _asm pop  ebp
    
    return;
}

/*----------------------------------------------------------------------------*/
/*
 * __GetFlatDataSelector - duplicate of NvGetFlatDataSelector()
 *
 */

static int __GetFlatDataSelector()
{
    DWORD   Selector;

    _asm xor eax, eax
    _asm mov ax, ds
    _asm mov Selector, eax
    return (Selector);
}

/*----------------------------------------------------------------------------*/
/*
 * NvRmR0AllocRoot - allocate root of device tree
 */

ULONG __stdcall NvRmR0AllocRoot (
    ULONG *phClient)
{

    NVOS01_PARAMETERS   parms;
    
    if ( apiEntry == NULL ) {
        nvGetResManAPIEntry();
    }

    /*
     * fill in parameters
     */

    parms.hClass = NV01_ROOT;

    callRmArchAPI(NVRM_ARCH_NV01ALLOCROOT, (PVOID)(&parms));

    *phClient = (ULONG)parms.hObjectNew;

    return(parms.status);

}                                               

/*----------------------------------------------------------------------------*/
/*
 * NvRmR0AllocDevice - allocate a device
 */

ULONG __stdcall NvRmR0AllocDevice (
    ULONG hClient,
    ULONG hDevice,
    ULONG hClass,
    PUCHAR szName )
{

    NVOS06_PARAMETERS   parms; 

    if ( apiEntry == NULL ) {
        nvGetResManAPIEntry();
    }

    /*
     * fill in parameters
     */

    parms.hObjectParent = hClient;
    parms.hObjectNew    = hDevice;
    parms.hClass        = hClass;
    parms.szName        = (NvV32)szName;

    callRmArchAPI(NVRM_ARCH_NV01ALLOCDEVICE, (PVOID)(&parms));

    return(parms.status);

}

/*----------------------------------------------------------------------------*/
/*
 * NvRmR0AllocChannelPio - allocate a PIO channel
 */

ULONG __stdcall NvRmR0AllocChannelPio (
    ULONG hClient,
    ULONG hDevice,
    ULONG hChannel,
    ULONG hClass,
    ULONG hErrorCtx,
    PVOID *ppChannel,
    ULONG flags )
{

    NVOS04_PARAMETERS   parms; 

    if ( apiEntry == NULL ) {
        nvGetResManAPIEntry();
    }

    /*
     * fill in parameters
     */

    parms.hRoot         = hClient;
    parms.hObjectParent = hDevice;
    parms.hObjectNew    = hChannel;
    parms.hClass        = hClass;
    parms.hObjectError  = hErrorCtx;
    parms.flags         = flags;
  
    callRmArchAPI(NVRM_ARCH_NV01ALLOCCHANNELPIO, (PVOID)(&parms));

    *ppChannel = (PVOID)parms.pChannel.offset;

    return(parms.status);

}                                               

/*----------------------------------------------------------------------------*/
/*
 * NvRmR0AllocChannelDMA - allocate a DMA channel
 */

ULONG __stdcall NvRmR0AllocChannelDma(
    ULONG hClient, 
    ULONG hDevice, 
    ULONG hChannel, 
    ULONG hClass, 
    ULONG hErrorCtx, 
    ULONG hDataCtx, 
    ULONG offset, 
    PVOID *ppChannel)
{

    NVOS07_PARAMETERS   parms; 

    if ( apiEntry == NULL ) {
        nvGetResManAPIEntry();
    }

    /*
     * fill in parameters
     */

    parms.hRoot         = hClient;
    parms.hObjectParent = hDevice;
    parms.hObjectNew    = hChannel;
    parms.hClass        = hClass;
    parms.hObjectError  = hErrorCtx;
    parms.hObjectBuffer = hDataCtx;
    parms.offset        = offset;

    callRmArchAPI(NVRM_ARCH_NV03ALLOCCHANNELDMA, (PVOID)(&parms));

    *ppChannel = (PVOID)parms.pControl.offset;

    return(parms.status);

}                                               

/*----------------------------------------------------------------------------*/
/*
 * NvRmR0AllocContextDma - get a context DMA
 */

ULONG __stdcall NvRmR0AllocContextDma(
    ULONG hClient, 
    ULONG hDma, 
    ULONG hClass, 
    ULONG flags, 
    PVOID base, 
    ULONG limit)
{

    NVOS03_PARAMETERS   parms;

    if ( apiEntry == NULL ) {
        nvGetResManAPIEntry();
    }

    /*
     * fill in parameters for the context dma (see nvdsptch.c)
     */

    parms.hObjectParent  = hClient;
    parms.hObjectNew     = hDma;
    parms.hClass         = hClass;
    parms.flags          = flags;
    parms.pBase.selector = __GetFlatDataSelector();
    parms.pBase.offset   = (DWORD)base;
    parms.limit.low      = limit;
  
    callRmArchAPI(NVRM_ARCH_NV01ALLOCCONTEXTDMA, (PVOID)(&parms));

    return(parms.status);

}

/*----------------------------------------------------------------------------*/
/*
 * NvRmR0AllocMemory - Allocate a chunk of memory using the RM
 */

ULONG __stdcall NvRmR0AllocMemory
(
    ULONG hClient, 
    ULONG hParent, 
    ULONG hMemory, 
    ULONG hClass, 
    ULONG flags, 
    PVOID *ppAddress, 
    ULONG *pLimit
)
{
    
    NVOS02_PARAMETERS parms;

    /*
     * fill in parameters for the context dma (see nvdsptch.c)
     */

    parms.hRoot             = hClient;
    parms.hObjectParent     = hParent;
    parms.hObjectNew        = hMemory;
    parms.hClass            = hClass;
    parms.flags             = flags;
    parms.pMemory.selector  = __GetFlatDataSelector();
    parms.pMemory.offset    = (NvU32)(*ppAddress);
    parms.pLimit.high       = 0x00000000;
    parms.pLimit.low        = *pLimit;

    callRmArchAPI(NVRM_ARCH_NV01ALLOCMEMORY, (PVOID)(&parms));

    *ppAddress = (PVOID)parms.pMemory.offset;
    *pLimit    = (ULONG)parms.pLimit.low;

    return(parms.status);
}

/*----------------------------------------------------------------------------*/
/*
 * NvRmR0AllocObject - Allocate a "thing" allocated using RM
 */


ULONG __stdcall NvRmR0AllocObject
(
    ULONG hClient,
    ULONG hChannel,
    ULONG hObject,
    ULONG hClass
)
{
    NVOS05_PARAMETERS parms;

    /*
     * fill in parameters for the context dma (see nvdsptch.c)
     */
    parms.hRoot             = hClient;
    parms.hObjectParent     = hChannel;
    parms.hObjectNew        = hObject;
    parms.hClass            = hClass;
    
    callRmArchAPI(NVRM_ARCH_NV01ALLOCOBJECT, (PVOID)(&parms));

    return(parms.status);
}

/*----------------------------------------------------------------------------*/
/*
 * NvRmR0Alloc - allocate arbitrary objects
 */

ULONG __stdcall NvRmR0Alloc
(
    ULONG hClient, 
    ULONG hChannel,
    ULONG hObject, 
    ULONG hClass, 
    PVOID pAllocParms
)
{
    NVOS21_PARAMETERS parms;

    // set input parameters
    parms.hRoot                 = hClient;
    parms.hObjectParent         = hChannel;
    parms.hObjectNew            = hObject;
    parms.hClass                = hClass;
    parms.pAllocParms.selector  = __GetFlatDataSelector();
    parms.pAllocParms.offset    = (NvU32)pAllocParms;

    callRmArchAPI(NVRM_ARCH_NV04ALLOC, (PVOID)(&parms));

    return parms.status;
}

/*----------------------------------------------------------------------------*/
/*
 * NvRmR0Free - free a "thing" allocated using RM
 */

ULONG __stdcall NvRmR0Free (
    ULONG   hClient,
    ULONG   hParent,
    ULONG   hObject)
{

    NVOS00_PARAMETERS   parms;
    
    if ( apiEntry == NULL ) {
        nvGetResManAPIEntry();
    }

    /*
     * fill in parameters for the root
     */

    parms.hRoot         = hClient;
    parms.hObjectParent = hParent;
    parms.hObjectOld    = hObject;

    callRmArchAPI(NVRM_ARCH_NV01FREE, (PVOID)(&parms));

    return(parms.status);

}

/*
 * NvRmR0Interrupt - force interrupt handling.
 */

ULONG __stdcall NvRmR0Interrupt (
    ULONG   hClient,
    ULONG   hDevice)
{

    NVOS09_PARAMETERS   parms;
    
    if ( apiEntry == NULL ) {
        nvGetResManAPIEntry();
    }

    /*
     * fill in parameters.
     */

    parms.hClient  = hClient;
    parms.hDevice  = hDevice;

    callRmArchAPI(NVRM_ARCH_NV01INTERRUPT, (PVOID)(&parms));

    return(parms.status);
}

/*----------------------------------------------------------------------------*/
/*
 * NvRmR0AllocEvent - Allocate an NV Event
 */


ULONG __stdcall NvRmR0AllocEvent
(
    ULONG hClient,
    ULONG hObjectParent,
    ULONG hObjectNew,
    ULONG hClass,
    ULONG index,
    PVOID data
)
{
    NVOS10_PARAMETERS parms;

    parms.hRoot         = hClient;
    parms.hObjectParent = hObjectParent;
    parms.hObjectNew    = hObjectNew;
    parms.hClass        = hClass;
    parms.index         = index;
    parms.hEvent.low    = (ULONG)data;

    callRmArchAPI(NVRM_ARCH_NV01ALLOCEVENT, (PVOID)(&parms));

    return(parms.status);
}

#ifndef NV3_HW

ULONG  __stdcall NvRmR0I2CAccess
(
    ULONG hClient, 
    ULONG hDevice, 
    VOID  *paramStructPtr
)
{
    NVOS_I2C_ACCESS_PARAMS    parms;

    parms.hClient = hClient;
    parms.hDevice = hDevice;
    parms.paramStructPtr = (ULONG)paramStructPtr;

    callRmArchAPI(NVRM_ARCH_I2C_ACCESS, (PVOID)(&parms));

    return(parms.status);
}

#endif

