/*----------------------------------------------------------------------------*/
/*
 * nvgpio.c
 *
 *	General purpose parallel bus support using NV_EXTERNAL_PARALLEL_BUS.
 *
 * Build Info:
 *
 *	These routines can be compiled for either Ring3 or Ring0 use.
 *	Use -DDRIVER and -DVXD when compiling into a VxD and -DDRIVER and 
 *	-DWDM when compiling into WDM.
 */

/*----------------------------------------------------------------------------*/
/*
 * include files
 */

#ifdef VXD

#define WANTVXDWRAPS
#include <basedef.h>
#include <vmm.h>
#include <vxdwraps.h>

#else

#include <windows.h>

/* 
 * some defines/typedef's so we don't have to include all the windows stuff 

#define	TRUE	1

typedef unsigned char	BYTE;
typedef unsigned char	UCHAR;
typedef int 		BOOL;
typedef unsigned long	DWORD;
typedef unsigned int	UINT;
 */

#endif /* VXD */

/* NV includes */
#include <nv32.h>
#include <nvtypes.h>
#include <nvmacros.h>
#include <nvwin32.h>
#include <nvos.h>
#include <nvcm.h>

/* some register defines for direct access */
#define NV_PME_VIPREG_NBYTES                             0x00200300
#define NV_PME_VIPREG_ADDR                               0x00200304
#define NV_PME_VIPREG_DATA                               0x00200308
#define NV_PME_VIPREG_CTRL                               0x0020030c

#ifdef DRIVER
#include <nvrmr0api.h>
#else
#include <nvrmapi.h>
#endif
#include <nvrmarch.inc>

#include "nvgpio.h"

/*----------------------------------------------------------------------------*/
/*
 * debug stuff
 */

#ifdef DEBUG
#ifdef DRIVER
#ifdef VXD
/* VXD driver debug */
#define DPRINTF0(fmt) _Sprintf(gpioDbgBuf, fmt); Out_Debug_String(gpioDbgBuf)
#define DPRINTF1(fmt, arg1) _Sprintf(gpioDbgBuf, fmt, arg1); Out_Debug_String(gpioDbgBuf)
#define DPRINTF2(fmt, arg1, arg2) _Sprintf(gpioDbgBuf, fmt, arg1, arg2); Out_Debug_String(gpioDbgBuf)
#else
/* WDM driver debug */
#define DPRINTF0(fmt) DbgPrint(fmt )
#define DPRINTF1(fmt, arg1) DbgPrint(fmt, arg1 )
#define DPRINTF2(fmt, arg1, arg2) DbgPrint(fmt, arg1, arg2 )
#endif /* VXD */
#else
/* Ring3 debug */
#include <stdio.h>
#define DPRINTF0(fmt) sprintf(gpioDbgBuf, fmt); OutputDebugString(gpioDbgBuf)
#define DPRINTF1(fmt, arg1) sprintf(gpioDbgBuf, fmt, arg1); OutputDebugString(gpioDbgBuf)
#define DPRINTF2(fmt, arg1, arg2) sprintf(gpioDbgBuf, fmt, arg1, arg2); OutputDebugString(gpioDbgBuf)
#endif /* DRIVER */
#else
/* non-debug */
#define DPRINTF0(fmt)
#define DPRINTF1(fmt, arg1)
#define DPRINTF2(fmt, arg1, arg2)
#endif /* DEBUG */

#ifdef DEBUG
static char	gpioDbgBuf[80];
#endif

/*----------------------------------------------------------------------------*/
/*
 * some nvgpio defines/typedef's
 */

#define	NVGPIO_PARALLEL_BUS_INST	0x50000010
#define	NVGPIO_NOTIFY_DMA_CONTEXT_INST	0x50000011

typedef struct _NVGPIO {
    ULONG		root;
    ULONG		dev;
    ULONG		channel;
    NvChannel*		pNV;
    UINT		subchannel;
#ifdef USE_NV_LINEAR_BASE_ADDR
    ULONG		deviceBase;
#endif
    NvNotification	nvNotifier[3];
} NVGPIO, *PNVGPIO;

#define	VIPREG_TIMEOUT_CNT	1000		// arbitrary

#ifdef DRIVER
#define	NVRM_ALLOC_ROOT		NvRmR0AllocRoot
#define	NVRM_ALLOC_DEVICE	NvRmR0AllocDevice
#define	NVRM_ALLOC_CHANNEL_PIO	NvRmR0AllocChannelPio
#define	NVRM_ALLOC_CONTEXT_DMA	NvRmR0AllocContextDma
#define	NVRM_ALLOC_OBJECT	NvRmR0AllocObject
#define	NVRM_FREE		NvRmR0Free
#define	NVRM_INTERRUPT		NvRmR0Interrupt
#else
#define	NVRM_ALLOC_ROOT		NvRmAllocRoot
#define	NVRM_ALLOC_DEVICE	NvRmAllocDevice
#define	NVRM_ALLOC_CHANNEL_PIO	NvRmAllocChannelPio
#define	NVRM_ALLOC_CONTEXT_DMA	NvRmAllocContextDma
#define	NVRM_ALLOC_OBJECT	NvRmAllocObject
#define	NVRM_FREE		NvRmFree
#define	NVRM_INTERRUPT		NvRmInterrupt
#endif

void ourYield();

#ifdef DRIVER
#ifdef USE_NV_LINEAR_BASE_ADDR
extern ULONG callRmAPI(ULONG, DWORD, PVOID);
#endif
#endif

/*----------------------------------------------------------------------------*/
/*
 * global GPIO control object
 *
 * XXX it might be better to allocate this object elsewhere
 */

static NVGPIO	nvGPIO;
static PNVGPIO	pGPIO = &nvGPIO;

/*----------------------------------------------------------------------------*/
/*
 * nvGPIOInit
 */

DWORD nvGPIOInit ( 
    ULONG	ourRoot,
    ULONG	ourDev,
    ULONG	ourChannel,
    NvChannel*	pNV,
    UINT	subchan )
{

    UINT	i;
    ULONG	flags;
    DWORD	rc;
    DWORD	err;
    UCHAR*	pNotifyBuf;

    DPRINTF0("nvGPIOInit: entering\n");

    /*
     * create an EXTERNAL_PARALLEL_BUS object
     */

    err = NVRM_ALLOC_OBJECT(ourRoot, ourChannel, NVGPIO_PARALLEL_BUS_INST, NV01_EXTERNAL_PARALLEL_BUS);

    if ( err != NVOS05_STATUS_SUCCESS ) {
	DPRINTF0("nvGPIOInit: NVRM_ALLOC_OBJECT of EXTERNAL_PARALLEL_BUS failed\n");
	return(1);
    }

    /*
     * initialize notifier for parallel bus object
     */

    DPRINTF0("nvGPIOInit: clearing notifier memory\n");

    pNotifyBuf = (UCHAR*) &(pGPIO->nvNotifier);
    for ( i = 0; i < sizeof(NvNotification)*3; i++ )
	pNotifyBuf[i] = 0;

    DPRINTF0("nvGPIOInit: initializing NvNotification objects\n" );

    pGPIO->nvNotifier[NV04F_NOTIFIERS_NOTIFY].status = 0;
    pGPIO->nvNotifier[NV04F_NOTIFICATION_READ_GET_DATA_NOTIFY].status = 0;
    pGPIO->nvNotifier[NV04F_NOTIFICATION_SET_INTERRUPT_NOTIFY].status = 0;

    /*
     * create DMA-to-memory context for notifer and plug in notify memory
     */

    DPRINTF0("nvGPIOInit: creating CONTEXT_DMA_TO_MEMORY object\n");

    flags = ALLOC_CTX_DMA_FLAGS_ACCESS_RD_WR | ALLOC_CTX_DMA_FLAGS_COHERENCY_UNCACHED;
    rc = NVRM_ALLOC_CONTEXT_DMA(ourRoot,
    			     NVGPIO_NOTIFY_DMA_CONTEXT_INST,
			     NV01_CONTEXT_DMA,
			     flags,
			     (PVOID) pNotifyBuf,
			     sizeof(NvNotification)*3 - 1 );
    if ( rc != ALLOC_CTX_DMA_STATUS_SUCCESS ) {
	DPRINTF0( "nvGPIOInit: NVRM_ALLOC_CONTEXT_DMA failed\n" );
	return(1);
    }

    /*
     * plug notifier into parallel bus object
     */

    DPRINTF0("nvGPIOInit: plugging notifier into parallel bus object\n");

    while ( pNV->subchannel[subchan].control.Free < 2*4 );

    pNV->subchannel[subchan].SetObject = NVGPIO_PARALLEL_BUS_INST;
    pNV->subchannel[subchan].nv01ExternalParallelBus.SetContextDmaNotifies = 
	NVGPIO_NOTIFY_DMA_CONTEXT_INST;

#ifdef USE_NV_LINEAR_BASE_ADDR
#ifdef DRIVER
    pGPIO->deviceBase = callRmAPI(1050,0,NULL);	 /* 1050 = NVRM_API_GET_NV_ADDRESS */
#else
    pGPIO->deviceBase = NvDeviceBaseGet(NV_DEV_BASE,1);
#endif
#endif

    /*
     * misc wrap-up
     */

    DPRINTF0("nvGPIOInit: misc wrap-up\n");

    pGPIO->root = ourRoot;
    pGPIO->dev = ourDev;
    pGPIO->channel = ourChannel;
    pGPIO->pNV = pNV;
    pGPIO->subchannel = subchan;

    DPRINTF0("nvGPIOInit: exiting successfully\n");

    return(0);	/* return 0 to indicate success */

}

/*----------------------------------------------------------------------------*/
/*
 * nvGPIOUninit
 */

DWORD nvGPIOUninit()
{

    NvChannel*	pNV = pGPIO->pNV;
    UINT	subchan = pGPIO->subchannel;

    DPRINTF0("nvGPIOUninit: entering\n");

    /*
     * XXX The below commented out code is to prevent a race condition with
     *     the synchronous NvRmFree's that occur afterwards.  The solution is
     *     to use NvRmInterrupt and watch the free count to tell when it's safe
     *     to use the synchronous free function.
     */

#ifdef COMMENT

    /*
     * unplug the context dma object
     */

    pNV->subchannel[subchan].SetObject = NVGPIO_PARALLEL_BUS_INST;
    pNV->subchannel[subchan].nv01ExternalParallelBus.SetContextDmaNotifies = 0;

#endif /* COMMENT */

    /*
     * destroy objects created in nvGPIOInit
     */

    NVRM_FREE(pGPIO->root, pGPIO->channel, NVGPIO_PARALLEL_BUS_INST);
    NVRM_FREE(pGPIO->root, pGPIO->root, NVGPIO_NOTIFY_DMA_CONTEXT_INST);

    DPRINTF0("nvGPIOUninit: exiting successfully\n");

    return(0);	/* return 0 to indicate success */

}

/*----------------------------------------------------------------------------*/
/*
 * nvGPIOWrite - do a GPIO write of a byte
 *
 * Note: only single byte addresses are supported at this time
 */

DWORD nvGPIOWrite (
    DWORD	regAddr,
    BYTE	regData )
{

    NvChannel*	pNV = pGPIO->pNV;
    UINT	subchan = pGPIO->subchannel;
    UINT	cnt;
    DWORD	vipCtrl;

    DPRINTF0("nvGPIOWrite: entering\n");

#ifdef USE_NV_LINEAR_BASE_ADDR

#ifdef NV10
    // Make sure there isn't a write pending
    cnt = 0;
    while ( (vipCtrl = *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_NBYTES))) & 0x100 ) {
	if ( ++cnt > VIPREG_TIMEOUT_CNT )
	    return(1);
    }
    // Do the write now
    *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_NBYTES)) = 1;
    *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_ADDR)) = (regAddr & 0xffff);
    *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_DATA)) = (DWORD)regData;
    *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_CTRL)) = 0x100;
#else
    *((DWORD*)(pGPIO->deviceBase | 0x00200800 | regAddr*4)) = regData;
#endif

#else

    /*
     * send write request to the parallel bus object
     */

    while ( pNV->subchannel[subchan].control.Free < 4*4 );

    pNV->subchannel[subchan].SetObject = NVGPIO_PARALLEL_BUS_INST;
    pNV->subchannel[subchan].nv01ExternalParallelBus.WriteSize = 0x00010001;
    pNV->subchannel[subchan].nv01ExternalParallelBus.WriteAddress = (U032)regAddr;
    pNV->subchannel[subchan].nv01ExternalParallelBus.WriteData = (U032)regData;

#endif /* USE_NV_LINEAR_BASE_ADDR */

    DPRINTF0("nvGPIOWrite: exiting\n");

    return(0);

}

/*----------------------------------------------------------------------------*/
/*
 * nvGPIORead - do a GPIO read of a byte
 *
 * Note: only single byte addresses are supported at this time
 */

DWORD nvGPIORead (
    DWORD	regAddr,
    BYTE*	pData )
{

    NvChannel*	pNV = pGPIO->pNV;
    UINT	subchan = pGPIO->subchannel;
    UINT	cnt;
    DWORD	vipCtrl;

    DPRINTF0( "nvGPIORead: entering\n" );

#ifdef USE_NV_LINEAR_BASE_ADDR

#ifdef NV10
    // go ahead and setup the read (XXX assumes there isn't another read in progress
    *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_NBYTES)) = 1;
    *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_ADDR)) = (regAddr & 0xffff);
    *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_CTRL)) = 0x1;
    // wait for the read to complete
    cnt = 0;
    while ( (vipCtrl = *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_NBYTES))) & 0x1 ) {
	if ( ++cnt > VIPREG_TIMEOUT_CNT )
	    return(1);
    }
    *pData = *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_DATA));
#else
    *pData = *((DWORD*)(pGPIO->deviceBase | 0x00200800 | regAddr*4));
#endif

#else

    /*
     * clear the notifier
     */

    pGPIO->nvNotifier[NV04F_NOTIFICATION_READ_GET_DATA_NOTIFY].status = 
	NV04F_NOTIFICATION_STATUS_IN_PROGRESS;

    /*
     * send read request to the parallel bus object
     */

    DPRINTF0( "nvGPIORead: sending read request\n" );

    while ( pNV->subchannel[subchan].control.Free < 4*4 );

    pNV->subchannel[subchan].SetObject = NVGPIO_PARALLEL_BUS_INST;
    pNV->subchannel[subchan].nv04ExternalSerialBus.ReadSize = 0x00010001;
    pNV->subchannel[subchan].nv04ExternalSerialBus.ReadAddress = (U032)regAddr;
    pNV->subchannel[subchan].nv04ExternalSerialBus.ReadGetDataNotify = 
	NV04F_NOTIFY_WRITE_ONLY;

    /*
     * wait for read to complete
     */

    DPRINTF0( "nvGPIORead: waiting for read to complete\n" );

    while ( (volatile) (pGPIO->nvNotifier[NV04F_NOTIFICATION_READ_GET_DATA_NOTIFY].status) == 
	NV04F_NOTIFICATION_STATUS_IN_PROGRESS ) ourYield();

    /*
     * get the read data
     */

    *pData = (UCHAR)(volatile)(pGPIO->nvNotifier[NV04F_NOTIFICATION_READ_GET_DATA_NOTIFY].info32);

#endif /* USE_NV_LINEAR_BASE_ADDR */

    DPRINTF0( "nvGPIORead: exiting\n" );

    return(0);

}

/*----------------------------------------------------------------------------*/
/*
 * nvVIPWrite
 */

DWORD nvVIPWrite (
    DWORD	regAddr,
    DWORD	regData,
    DWORD	dataSize )
{

    NvChannel*	pNV = pGPIO->pNV;
    UINT	subchan = pGPIO->subchannel;
    UINT	cnt;
    DWORD	vipCtrl;

    DPRINTF0("nvVIPWrite: entering\n");

#ifdef USE_NV_LINEAR_BASE_ADDR

#ifdef NV10
    // Make sure there isn't a write pending
    cnt = 0;
    while ( (vipCtrl = *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_NBYTES))) & 0x100 ) {
	if ( ++cnt > VIPREG_TIMEOUT_CNT )
	    return(1);
    }
    // Do the write now
    *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_NBYTES)) = dataSize;
    *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_ADDR)) = (regAddr & 0xffff);
    *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_DATA)) = (DWORD)regData;
    *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_CTRL)) = 0x100;
#endif

#else

    /*
     * send write request to the parallel bus object
     */

    while ( pNV->subchannel[subchan].control.Free < 4*4 );

    pNV->subchannel[subchan].SetObject = NVGPIO_PARALLEL_BUS_INST;
    pNV->subchannel[subchan].nv01ExternalParallelBus.WriteSize = 0x00020000 | dataSize;
    pNV->subchannel[subchan].nv01ExternalParallelBus.WriteAddress = (U032)regAddr;
    pNV->subchannel[subchan].nv01ExternalParallelBus.WriteData = (U032)regData;

#endif /* USE_NV_LINEAR_BASE_ADDR */

    DPRINTF0("nvVIPWrite: exiting\n");

    return(0);

}

/*----------------------------------------------------------------------------*/
/*
 * nvVIPRead
 */

DWORD nvVIPRead (
    DWORD	regAddr,
    DWORD*	pData,
    DWORD	dataSize )
{

    NvChannel*	pNV = pGPIO->pNV;
    UINT	subchan = pGPIO->subchannel;
    UINT	cnt;
    DWORD	vipCtrl;

    DPRINTF0( "nvVIPRead: entering\n" );

#ifdef USE_NV_LINEAR_BASE_ADDR

#ifdef NV10
    // go ahead and setup the read (XXX assumes there isn't another read in progress
    *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_NBYTES)) = 1;
    *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_ADDR)) = (regAddr & 0xffff);
    *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_CTRL)) = 0x1;
    // wait for the read to complete
    cnt = 0;
    while ( (vipCtrl = *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_NBYTES))) & 0x1 ) {
	if ( ++cnt > VIPREG_TIMEOUT_CNT )
	    return(1);
    }
    *pData = *((DWORD*)(pGPIO->deviceBase | NV_PME_VIPREG_DATA));
#endif

#else

    /*
     * clear the notifier
     */

    pGPIO->nvNotifier[NV04F_NOTIFICATION_READ_GET_DATA_NOTIFY].status = 
	NV04F_NOTIFICATION_STATUS_IN_PROGRESS;

    /*
     * send read request to the parallel bus object
     */

    DPRINTF0( "nvVIPRead: sending read request\n" );

    while ( pNV->subchannel[subchan].control.Free < 4*4 );

    pNV->subchannel[subchan].SetObject = NVGPIO_PARALLEL_BUS_INST;
    pNV->subchannel[subchan].nv04ExternalSerialBus.ReadSize = 0x00020000 | dataSize;
    pNV->subchannel[subchan].nv04ExternalSerialBus.ReadAddress = (U032)regAddr;
    pNV->subchannel[subchan].nv04ExternalSerialBus.ReadGetDataNotify = 
	NV04F_NOTIFY_WRITE_ONLY;

    /*
     * wait for read to complete
     */

    DPRINTF0( "nvVIPRead: waiting for read to complete\n" );

    while ( (volatile) (pGPIO->nvNotifier[NV04F_NOTIFICATION_READ_GET_DATA_NOTIFY].status) == 
	NV04F_NOTIFICATION_STATUS_IN_PROGRESS ) ourYield();

    /*
     * get the read data
     */

    *pData = (UCHAR)(volatile)(pGPIO->nvNotifier[NV04F_NOTIFICATION_READ_GET_DATA_NOTIFY].info32);

#endif /* USE_NV_LINEAR_BASE_ADDR */

    DPRINTF0( "nvVIPRead: exiting\n" );

    return(0);

}

/*----------------------------------------------------------------------------*/
/*
 * nvRegWrite
 */

DWORD nvRegWrite (
    DWORD	regAddr,
    UINT	regData )
{

    DPRINTF0("nvRegWrite: entering\n");

#ifdef USE_NV_LINEAR_BASE_ADDR

    *((DWORD*)(pGPIO->deviceBase | regAddr)) = regData;

#endif

    DPRINTF0("nvRegWrite: exiting\n");

    return(0);

}

/*----------------------------------------------------------------------------*/
/*
 * nvRegRead
 */

DWORD nvRegRead (
    DWORD	regAddr,
    UINT*	pData )
{

    DPRINTF0( "nvRegRead: entering\n" );

#ifdef USE_NV_LINEAR_BASE_ADDR

    *pData = *((DWORD*)(pGPIO->deviceBase | regAddr));

#endif

    DPRINTF0( "nvRegRead: exiting\n" );

    return(0);

}

/*----------------------------------------------------------------------------*/
/*
 * __GetFlatDataSelector - duplicate of NvGetFlatDataSelector()
 *
 * XXX Eventually, this function will be provided to us.
 */

int __GetFlatDataSelector()
{
    DWORD	Selector;

    _asm xor eax, eax
    _asm mov ax, ds
    _asm mov Selector, eax
    return (Selector);
}

/*----------------------------------------------------------------------------*/
/*
 * ourYield()
 */

#ifdef VXD
VOID VXDINLINE
Time_Slice_Sleep(DWORD timeout)
{
    __asm mov eax, timeout
    VMMCall(Time_Slice_Sleep);
}
#endif

static void ourYield()
{
    NVRM_INTERRUPT(pGPIO->root, pGPIO->dev);
}

/*----------------------------------------------------------------------------*/
/*
 * nvGPIOSetControlObj - backdoor to set the ptr the global control object
 */

DWORD nvGPIOSetControlObj (
    VOID*	pObj )
{

    pGPIO = (NVGPIO*) pObj;

    DPRINTF0( "nvGPIOSetControlObj: exiting successfully\n" );

    return(0);	/* return 0 to indicate success */

}

/*----------------------------------------------------------------------------*/
/*
 * nvGPIOGetControlObj - backdoor to get the ptr the global control object
 */

VOID* nvGPIOGetControlObj ()
{

    DPRINTF0( "nvGPIOGetControlObj: exiting successfully\n" );

    return(pGPIO);

}

