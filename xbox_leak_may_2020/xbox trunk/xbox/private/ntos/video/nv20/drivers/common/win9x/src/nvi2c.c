/*----------------------------------------------------------------------------*/
/*
 * nvi2c.c
 *
 *	I2C functionality using the NV_EXTERNAL_SERIAL_BUS object.
 *
 * Build Info:
 *
 *	These routines can be compiled for either Ring3 or Ring0 use.
 *	Use -DDRIVER and -DVXD when compiling into a VxD and -DDRIVER and 
 *	-DWDM when compiling into WDM.
 */

#ifdef nv3
#define	NV3
#endif

#ifdef nv4
#define	NV4
#endif

#ifdef nv10
#define	NV10
#endif

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

#include <stdio.h>
#include <windows.h>

/* 
 * some defines/typedef's so we don't have to include all the windows stuff 

#define	TRUE	1

typedef unsigned char	BYTE;
typedef unsigned char	UCHAR;
typedef int 		BOOL;
typedef unsigned long	DWORD;
typedef unsigned int	UINT;
typedef unsigned short	USHORT;
 */

#endif /* VXD */

/* NV includes */
#include <nv32.h>
#include <nvtypes.h>
#include <nvmacros.h>
#include <nvwin32.h>
#include <nvos.h>

#ifdef DRIVER
#include <nvrmr0api.h>
#else
#include <nvrmapi.h>
#endif

#include "nvi2c.h"

/*----------------------------------------------------------------------------*/
/*
 * debug stuff
 */

#ifdef DEBUG
#ifdef DRIVER
#ifdef VXD
/* VXD driver debug */
#define DPRINTF0(fmt) _Sprintf(i2cDbgBuf, fmt); Out_Debug_String(i2cDbgBuf)
#define DPRINTF1(fmt, arg1) _Sprintf(i2cDbgBuf, fmt, arg1); Out_Debug_String(i2cDbgBuf)
#define DPRINTF2(fmt, arg1, arg2) _Sprintf(i2cDbgBuf, fmt, arg1, arg2); Out_Debug_String(i2cDbgBuf)
#else
/* WDM driver debug */
#define DPRINTF0(fmt) DbgPrint(fmt )
#define DPRINTF1(fmt, arg1) DbgPrint(fmt, arg1 )
#define DPRINTF2(fmt, arg1, arg2) DbgPrint(fmt, arg1, arg2 )
#endif /* VXD */
#else
/* Ring3 debug */
#include <stdio.h>
#define DPRINTF0(fmt) sprintf(i2cDbgBuf, fmt); OutputDebugString(i2cDbgBuf)
#define DPRINTF1(fmt, arg1) sprintf(i2cDbgBuf, fmt, arg1); OutputDebugString(i2cDbgBuf)
#define DPRINTF2(fmt, arg1, arg2) sprintf(i2cDbgBuf, fmt, arg1, arg2); OutputDebugString(i2cDbgBuf)
#define DPRINTF3(fmt, arg1, arg2, arg3 ) sprintf(i2cDbgBuf, fmt, arg1, arg2, arg3); OutputDebugString(i2cDbgBuf)
#endif /* DRIVER */
#else
/* non-debug */
#define DPRINTF0(fmt)
#define DPRINTF1(fmt, arg1)
#define DPRINTF2(fmt, arg1, arg2)
#endif /* DEBUG */

static char	i2cDbgBuf[80];

/*----------------------------------------------------------------------------*/
/*
 * misc defines/typedef's
 */

#define	NVI2C_BASE_INST			0x50000001
#define	NVI2C_SERIAL_BUS		0
#define	NVI2C_NOTIFY_DMA_CONTEXT	1
#define	NVI2C_DEVICE			2
#define	NVI2C_CHANNEL			3

typedef struct _NVI2C {
    ULONG		externalAllocd;
    ULONG		root;
    ULONG		dev;
    ULONG		channel;
    NvChannel*		pNV;
    UINT		subchannel;
    UINT		baseInst;
    NvNotification	nvNotifier[3];
    UINT		addrSize;
} NVI2C, *PNVI2C;

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

#define	NVI2C_READ_TIMEOUT	10000

void __yield();

/*----------------------------------------------------------------------------*/
/*
 * global I2C control object
 *
 * XXX it might be better to allocate this object elsewhere
 */

static NVI2C	nvI2C;
static PNVI2C	pI2C = &nvI2C;


/*----------------------------------------------------------------------------*/
/*
 * nvI2CInit
 */

DWORD nvI2CInit (
    ULONG	ourRoot,
    ULONG	ourDev,
    ULONG	ourChannel,
    NvChannel*	pChannel,
    UINT	subchan,
    UINT	baseInst )
{

    UINT	i;
    ULONG	flags;
    DWORD	rc;
    DWORD	err;
    UCHAR*	pNotifyBuf;
    NvChannel*	pNV;
    char	devName[32];

    DPRINTF0( "nvI2CInit: entering\n" );

    /*
     * check if user wants to use a specific base instance for our objects
     */

    if ( baseInst == 0 ) {
    	baseInst = NVI2C_BASE_INST;
    }

    /*
     * check if we need to allocate root, device and channel
     */

    if ( pChannel == NULL ) {

	/*
	 * get a NV channel
	 */

	err = NVRM_ALLOC_ROOT(&ourRoot);
	if ( err != NVOS01_STATUS_SUCCESS ) {
	    DPRINTF0("nvI2CInit: NVRM_ALLOC_ROOT failed\n");
	    return(1);
	}

	ourDev = baseInst + NVI2C_DEVICE;
	err = NVRM_ALLOC_DEVICE(ourRoot, 
			        ourDev, 
			        NV01_DEVICE_0, 
			        devName);
	if ( err != NVOS06_STATUS_SUCCESS ) {
	    DPRINTF0("nvI2CInit: NVRM_ALLOC_DEVICE failed\n");
	    NVRM_FREE(ourRoot, ourRoot, ourRoot);
	    return(1);
	}

	ourChannel = baseInst + NVI2C_CHANNEL;
	err = NVRM_ALLOC_CHANNEL_PIO(ourRoot,
				     ourDev,
				     ourChannel,
    #ifdef NV3
				     NV03_CHANNEL_PIO,
    #else
				     NV03_CHANNEL_PIO,
    #endif
				     0,
				     (PVOID) &pNV,
				     0);

	if ( err != NVOS04_STATUS_SUCCESS ) {
	    DPRINTF0("nvI2CInit: NVRM_ALLOC_CHANNEL_PIO failed\n");
	    NVRM_FREE(ourRoot, ourRoot, ourDev);
	    NVRM_FREE(ourRoot, ourRoot, ourRoot);
	    return(1);
	}

	if ( pNV == NULL ) {
	    DPRINTF0("nvI2CInit: null channel ptr returned by NVRM_ALLOC_CHANNEL_PIO\n");
	    NVRM_FREE(ourRoot, ourDev, ourChannel);
	    NVRM_FREE(ourRoot, ourRoot, ourDev);
	    NVRM_FREE(ourRoot, ourRoot, ourRoot);
	    return(1);
	}

	pI2C->externalAllocd = FALSE;
    }
    else {

        /*
	 * use the channel ptr (and root and device and channel id) that was passed to us
	 */

        pNV = pChannel;

	pI2C->externalAllocd = TRUE;

    }

    /*
     * create an EXTERNAL_SERIAL_BUS object
     */

#ifdef NV3
    err = NVRM_ALLOC_OBJECT(ourRoot, ourChannel, baseInst+NVI2C_SERIAL_BUS, NV_EXTERNAL_SERIAL_BUS);
#else
    err = NVRM_ALLOC_OBJECT(ourRoot, ourChannel, baseInst+NVI2C_SERIAL_BUS, NV04_EXTERNAL_SERIAL_BUS);
#endif

    if ( err != NVOS05_STATUS_SUCCESS ) {
	DPRINTF0("nvI2CInit: NVRM_ALLOC_OBJECT of EXTERNAL_SERIAL_BUS failed\n");
	return(1);
    }

    /*
     * initialize notifier for serial bus object
     */

    DPRINTF0( "nvI2CInit: clearing notifier memory\n" );

    pNotifyBuf = (UCHAR*) &(pI2C->nvNotifier);
    for ( i = 0; i < sizeof(NvNotification)*3; i++ )
	pNotifyBuf[i] = 0;

    DPRINTF0( "nvI2CInit: initializing NvNotification objects\n" );

#ifdef NV3
    pI2C->nvNotifier[NV_051_NOTIFICATION_NOTIFY].status = 0;
    pI2C->nvNotifier[NV_051_NOTIFICATION_READ].status = 0;
    pI2C->nvNotifier[NV_051_NOTIFICATION_SET_INTERRUPT_NOTIFY].status = 0;
#else
    pI2C->nvNotifier[NV051_NOTIFIERS_NOTIFY].status = 0;
    pI2C->nvNotifier[NV051_NOTIFICATION_READ_GET_DATA_NOTIFY].status = 0;
    pI2C->nvNotifier[NV051_NOTIFICATION_SET_INTERRUPT_NOTIFY].status = 0;
#endif

    /*
     * create DMA-to-memory context for notifer and plug in notify memory
     */

    DPRINTF0( "nvI2CInit: creating CONTEXT_DMA_TO_MEMORY object\n" );

    flags = NVOS03_FLAGS_ACCESS_READ_WRITE | NVOS03_FLAGS_COHERENCY_UNCACHED;
    rc = NVRM_ALLOC_CONTEXT_DMA(ourRoot,
    			     baseInst + NVI2C_NOTIFY_DMA_CONTEXT,
			     NV01_CONTEXT_DMA,
			     flags,
			     (PVOID) pNotifyBuf,
			     sizeof(NvNotification)*3 - 1 );
    if ( rc != NVOS03_STATUS_SUCCESS ) {
	DPRINTF0( "nvI2CInit: NVRM_ALLOC_CONTEXT_DMA failed\n" );
	return(1);
    }

    /*
     * plug notifier into serial bus object
     */

    DPRINTF0( "nvI2CInit: plugging notifer into serial bus object\n" );

#ifdef NV3
    while ( pNV->subchannel[subchan].control.free < 2*4 );

    pNV->subchannel[subchan].control.object = baseInst + NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].externalSerialBus.SetContextDmaNotifies = 
	baseInst + NVI2C_NOTIFY_DMA_CONTEXT;
#else
    while ( pNV->subchannel[subchan].control.Free < 2*4 );

    pNV->subchannel[subchan].SetObject = baseInst + NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].nv04ExternalSerialBus.SetContextDmaNotifies = 
	baseInst + NVI2C_NOTIFY_DMA_CONTEXT;
#endif

    /*
     * misc wrap-up
     */

    DPRINTF0( "nvI2CInit: misc wrap-up\n" );

    pI2C->addrSize = 2;	/* default is 16-bit addresses */
    pI2C->root = ourRoot;
    pI2C->dev = ourDev;
    pI2C->channel = ourChannel;
    pI2C->pNV = pNV;
    pI2C->subchannel = subchan;
    pI2C->baseInst = baseInst;

    DPRINTF0( "nvI2CInit: exiting successfully\n" );

    return(0);	/* return 0 to indicate success */

}

/*----------------------------------------------------------------------------*/
/*
 * nvI2CUninit
 *
 * XXX for now don't compile into a Ring0 driver
 */

DWORD nvI2CUninit()
{

    NvChannel*	pNV = pI2C->pNV;
    UINT	subchan = pI2C->subchannel;

    DPRINTF0( "nvI2CUninit: entering\n" );

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

#ifdef NV3
    pNV->subchannel[subchan].control.object = pI2C->baseInst + NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].externalSerialBus.SetContextDmaNotifies = 0;
#else
    pNV->subchannel[subchan].SetObject = pI2C->baseInst + NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].nv04ExternalSerialBus.SetContextDmaNotifies = 0;
#endif

#endif /* COMMENT */

    /*
     * destroy objects created in nvI2CInit
     */

    NVRM_FREE(pI2C->root, pI2C->channel, pI2C->baseInst+NVI2C_SERIAL_BUS);
    NVRM_FREE(pI2C->root, pI2C->root, pI2C->baseInst+NVI2C_NOTIFY_DMA_CONTEXT);

    /*
     * free root, device, and channel if we allocated them
     */

    if ( pI2C->externalAllocd == FALSE ) {
	NVRM_FREE(pI2C->root, pI2C->dev, pI2C->channel);
	NVRM_FREE(pI2C->root, pI2C->root, pI2C->dev);
	NVRM_FREE(pI2C->root, pI2C->root, pI2C->root);
    }

    DPRINTF0( "nvI2CUninit: exiting\n" );

    return(0);

}
 
/*----------------------------------------------------------------------------*/
/*
 * nvI2CWriteByte
 */

DWORD nvI2CWriteByte (
    UCHAR	chipAddr,
    UCHAR	regAddr,
    UCHAR	data )
{

    NvChannel*	pNV = pI2C->pNV;
    UINT	subchan = pI2C->subchannel;
    UINT	writeSize = (pI2C->addrSize << 16) | 0x00000001;
    UINT	addr = (regAddr << 8) | chipAddr;

    DPRINTF0( "nvI2CWriteByte: entering\n" );

    /*
     * send write request to the serial bus object
     */

#ifdef NV3
    DPRINTF0("nvI2CWriteByte: doing NV3 write\n");

    while ( pNV->subchannel[subchan].control.free < 4*4 );

    pNV->subchannel[subchan].control.object = pI2C->baseInst+NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].externalSerialBus.Write.size = writeSize;
    pNV->subchannel[subchan].externalSerialBus.Write.address = addr;
    pNV->subchannel[subchan].externalSerialBus.Write.data = data;
#else
    DPRINTF0("nvWriteI2CByte: doing NV4 write\n");

    while ( pNV->subchannel[subchan].control.Free < 4*4 );

    pNV->subchannel[subchan].SetObject = pI2C->baseInst+NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].nv04ExternalSerialBus.WriteSize = writeSize;
    pNV->subchannel[subchan].nv04ExternalSerialBus.WriteAddress = addr;
    pNV->subchannel[subchan].nv04ExternalSerialBus.WriteData = data;
#endif

    DPRINTF0( "nvI2CWriteByte: exiting\n" );

    return(0);

}
    
/*----------------------------------------------------------------------------*/
/*
 * nvI2CReadByte
 */

DWORD nvI2CReadByte (
    UCHAR	chipAddr,
    UCHAR	regAddr,
    UCHAR*	pData )
{

    NvChannel*	pNV = pI2C->pNV;
    UINT	subchan = pI2C->subchannel;
    UINT	readSize = (pI2C->addrSize << 16) | 0x00000001;
    UINT	addr = (regAddr << 8) | chipAddr;
    UINT	timeOut = NVI2C_READ_TIMEOUT;

    DPRINTF0( "nvI2CReadByte: entering\n" );

    /*
     * clear the notifier
     */

#ifdef NV3
    pI2C->nvNotifier[NV_051_NOTIFICATION_READ].status = NV_STATUS_IN_PROGRESS;
#else
    pI2C->nvNotifier[NV051_NOTIFICATION_READ_GET_DATA_NOTIFY].status = 
	NV051_NOTIFICATION_STATUS_IN_PROGRESS;
#endif

    /*
     * send read request to the serial bus object
     */

    DPRINTF0( "nvI2CReadByte: sending read request\n" );

#ifdef NV3
    DPRINTF0("nvI2CReadByte: doing NV3 read\n");

    while ( pNV->subchannel[subchan].control.free < 4*4 );

    pNV->subchannel[subchan].control.object = pI2C->baseInst+NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].externalSerialBus.Read.size = readSize;
    pNV->subchannel[subchan].externalSerialBus.Read.address = addr;
    pNV->subchannel[subchan].externalSerialBus.Read.getDataNotify = 
	NV_051_NOTIFY_WRITE_ONLY;
#else
    DPRINTF0("nvI2CReadByte: doing NV4 read\n");

    while ( pNV->subchannel[subchan].control.Free < 4*4 );

    pNV->subchannel[subchan].SetObject = pI2C->baseInst+NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].nv04ExternalSerialBus.ReadSize = readSize;
    pNV->subchannel[subchan].nv04ExternalSerialBus.ReadAddress = addr;
    pNV->subchannel[subchan].nv04ExternalSerialBus.ReadGetDataNotify = 
	NV051_NOTIFY_WRITE_ONLY;
#endif

    /*
     * wait for read to complete
     */

    DPRINTF0( "nvI2CReadByte: waiting for read to complete\n" );

    while ( timeOut > 0 ) {

#ifdef NV3
	if ( (volatile) (pI2C->nvNotifier[NV_051_NOTIFICATION_READ].status) == NV_STATUS_IN_PROGRESS ) {
#else
	if ( (volatile) (pI2C->nvNotifier[NV051_NOTIFICATION_READ_GET_DATA_NOTIFY].status) == 
	    NV051_NOTIFICATION_STATUS_IN_PROGRESS ) {
#endif
	    /* data is not ready yet */
	    timeOut--;
	    __yield();
	}
	else {
	    /* stop waiting, the data is ready */
	    break; 
	}

    }

    if ( timeOut == 0 ) {
	DPRINTF0( "nvI2CReadByte: error, read timed out\n" );
	return(1);
    }

    /*
     * get the read data
     */

#ifdef NV3
    *pData = (UCHAR)(volatile)(pI2C->nvNotifier[NV_051_NOTIFICATION_READ].otherInfo32);
#else
    *pData = (UCHAR)(volatile)(pI2C->nvNotifier[NV051_NOTIFICATION_READ_GET_DATA_NOTIFY].info32);
#endif

    DPRINTF1( "nvI2CReadByte: exiting, with read data = 0x%x\n", *pData );

    return(0);

}

/*----------------------------------------------------------------------------*/
/*
 * nvI2CWriteWord
 */

DWORD nvI2CWriteWord (
    UCHAR	chipAddr,
    UCHAR	regAddr,
    USHORT	data )
{

    NvChannel*	pNV = pI2C->pNV;
    UINT	subchan = pI2C->subchannel;
    UINT	writeSize = (pI2C->addrSize << 16) | 0x00000002;
    UINT	addr = (regAddr << 8) | chipAddr;
    UINT	uiData;

    DPRINTF0( "nvI2CWriteWord: entering\n" );

    /*
     * swap data bytes
     */

    uiData = ((data & 0xff) << 8) | ((data >> 8) & 0xff);

    /*
     * send write request to the serial bus object
     */

#ifdef NV3
    DPRINTF0("nvI2CWriteWord: doing NV3 write\n");

    while ( pNV->subchannel[subchan].control.free < 4*4 );

    pNV->subchannel[subchan].control.object = pI2C->baseInst+NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].externalSerialBus.Write.size = writeSize;
    pNV->subchannel[subchan].externalSerialBus.Write.address = addr;
    pNV->subchannel[subchan].externalSerialBus.Write.data = uiData;
#else
    DPRINTF0("nvI2CWriteWord: doing NV4 write\n");

    while ( pNV->subchannel[subchan].control.Free < 4*4 );

    pNV->subchannel[subchan].SetObject = pI2C->baseInst+NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].nv04ExternalSerialBus.WriteSize = writeSize;
    pNV->subchannel[subchan].nv04ExternalSerialBus.WriteAddress = addr;
    pNV->subchannel[subchan].nv04ExternalSerialBus.WriteData = uiData;
#endif

    DPRINTF0( "nvI2CWriteWord: exiting\n" );

    return(0);

}
    
/*----------------------------------------------------------------------------*/
/*
 * nvI2CReadWord
 */

DWORD nvI2CReadWord (
    UCHAR	chipAddr,
    UCHAR	regAddr,
    USHORT*	pData )
{

    NvChannel*	pNV = pI2C->pNV;
    UINT	subchan = pI2C->subchannel;
    UINT	readSize = (pI2C->addrSize << 16) | 0x00000002;
    UINT	addr = (regAddr << 8) | chipAddr;
    USHORT	data;
    UINT	timeOut = NVI2C_READ_TIMEOUT;

    DPRINTF0( "nvI2CReadWord: entering\n" );

    /*
     * clear the notifier
     */

#ifdef NV3
    pI2C->nvNotifier[NV_051_NOTIFICATION_READ].status = NV_STATUS_IN_PROGRESS;
#else
    pI2C->nvNotifier[NV051_NOTIFICATION_READ_GET_DATA_NOTIFY].status = 
	NV051_NOTIFICATION_STATUS_IN_PROGRESS;
#endif

    /*
     * send read request to the serial bus object
     */

    DPRINTF0( "nvI2CReadWord: sending read request\n" );

#ifdef NV3
    DPRINTF0("nvI2CReadWord: doing NV3 read\n");

    while ( pNV->subchannel[subchan].control.free < 4*4 );

    pNV->subchannel[subchan].control.object = pI2C->baseInst+NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].externalSerialBus.Read.size = readSize;
    pNV->subchannel[subchan].externalSerialBus.Read.address = addr;
    pNV->subchannel[subchan].externalSerialBus.Read.getDataNotify = 
	NV_051_NOTIFY_WRITE_ONLY;
#else
    DPRINTF0("nvI2CReadWord: doing NV4 read\n");

    while ( pNV->subchannel[subchan].control.Free < 4*4 );

    pNV->subchannel[subchan].SetObject = pI2C->baseInst+NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].nv04ExternalSerialBus.ReadSize = readSize;
    pNV->subchannel[subchan].nv04ExternalSerialBus.ReadAddress = addr;
    pNV->subchannel[subchan].nv04ExternalSerialBus.ReadGetDataNotify = 
	NV051_NOTIFY_WRITE_ONLY;
#endif

    /*
     * wait for read to complete
     */

    DPRINTF0( "nvI2CReadWord: waiting for read to complete\n" );

    while ( timeOut > 0 ) {

#ifdef NV3
	if ( (volatile) (pI2C->nvNotifier[NV_051_NOTIFICATION_READ].status) == NV_STATUS_IN_PROGRESS ) {
#else
	if ( (volatile) (pI2C->nvNotifier[NV051_NOTIFICATION_READ_GET_DATA_NOTIFY].status) == 
	    NV051_NOTIFICATION_STATUS_IN_PROGRESS ) {
#endif
	    /* data is not ready yet */
	    timeOut--;
	    __yield();
	}
	else {
	    /* stop waiting, the data is ready */
	    break; 
	}

    }

    if ( timeOut == 0 ) {
	DPRINTF0( "nvI2CReadWord: error, read timed out\n" );
	return(1);
    }

    /*
     * get the read data
     */

#ifdef NV3
    data = (USHORT)(volatile)(pI2C->nvNotifier[NV_051_NOTIFICATION_READ].otherInfo32);
#else
    data = (USHORT)(volatile)(pI2C->nvNotifier[NV051_NOTIFICATION_READ_GET_DATA_NOTIFY].info32);
#endif

    *pData = ((data & 0xff) << 8) | ((data >> 8) & 0xff);

    DPRINTF1( "nvReadI2CWord: exiting, with read data = 0x%x\n", *pData );

    return(0);

}

/*----------------------------------------------------------------------------*/
/*
 * nvI2CWriteBuf - do a I2C "write" of a variable length buffer of bytes
 *
 * Note: the current implemenation of the external serial class object
 *	 does not allow for more then 8 bytes total (address and data)
 *	 to be transmitted in a single I2C "transaction".
 */

DWORD nvI2CWriteBuf (
    UCHAR*	pBuf,
    UINT	bufSize )
{

    NvChannel*	pNV = pI2C->pNV;
    UINT	subchan = pI2C->subchannel;
    UINT	i, tmp;
    UINT	writeSize;
    UINT	addr;
    UINT	data;

    DPRINTF0( "nvI2CWriteBuf: entering\n" );

    /*
     * construct proper addr and data values using contents of pBuf
     */

    addr = 0;

    for ( i = 0; (i < 4) && (i < bufSize); i++ ) {
        tmp = (pBuf[i] & 0xff) << (i*8);
	addr = addr | tmp;
    }

    writeSize = i << 16;

    data = 0;

    for ( i = 4; (i < 8) && (i < bufSize); i++ ) {
        tmp = (pBuf[i] & 0xff) << ((i-4)*8);
	data = data | tmp;
    }

    writeSize = writeSize | (i-4);

    DPRINTF1( "nvI2CWriteBuf: addr = %x\n", addr );
    DPRINTF1( "nvI2CWriteBuf: data = %x\n", data );
    DPRINTF1( "nvI2CWriteBuf: writeSize = %x\n", writeSize );
    
    /*
     * send write request to the serial bus object
     */

#ifdef NV3
    while ( pNV->subchannel[subchan].control.free < 4*4 );

    pNV->subchannel[subchan].control.object = pI2C->baseInst+NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].externalSerialBus.Write.size = writeSize;
    pNV->subchannel[subchan].externalSerialBus.Write.address = addr;
    pNV->subchannel[subchan].externalSerialBus.Write.data = data;
#else
    while ( pNV->subchannel[subchan].control.Free < 4*4 ) 
        __yield();

    pNV->subchannel[subchan].SetObject = pI2C->baseInst+NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].nv04ExternalSerialBus.WriteSize = writeSize;
    pNV->subchannel[subchan].nv04ExternalSerialBus.WriteAddress = addr;
    pNV->subchannel[subchan].nv04ExternalSerialBus.WriteData = data;
#endif

    DPRINTF0( "nvI2CWriteBuf: exiting\n" );

    return(0);

}
    
/*----------------------------------------------------------------------------*/
/*
 * nvI2CReadBuf - do a I2C "read" of a buffer of bytes using a variable length address
 *
 * Note: the address buffer and read buffer is limited to a maximum of 4 bytes
 */

DWORD nvI2CReadBuf (
    UCHAR*	pBuf,		/* address buffer */
    UINT	bufSize,	/* size of address buffer, max 4 */
    UCHAR*	pData,
    UINT	readBufSize)
{

    NvChannel*	pNV = pI2C->pNV;
    UINT	subchan = pI2C->subchannel;
    UINT	i, tmp;
    UINT	readSize;
    UINT	addr;
    UINT	data;
    UINT	timeOut = NVI2C_READ_TIMEOUT;
#ifdef DO_TIMING
    UINT	deltaLo, startLo;
#endif

    DPRINTF0( "nvI2CReadBuf: entering\n" );

    /*
     * construct proper addr and data values using contents of pBuf
     */

    addr = 0;

    for ( i = 0; (i < 4) && (i < bufSize); i++ ) {
        tmp = pBuf[i] << (i*8);
	addr = addr | tmp;
    }

    readSize = (i << 16) | (readBufSize & 0x0000ffff);

    /*
     * clear the notifier
     */

#ifdef NV3
    pI2C->nvNotifier[NV_051_NOTIFICATION_READ].status = NV_STATUS_IN_PROGRESS;
#else
    pI2C->nvNotifier[NV051_NOTIFICATION_READ_GET_DATA_NOTIFY].status = 
	NV051_NOTIFICATION_STATUS_IN_PROGRESS;
#endif

    /*
     * send read request to the serial bus object
     */

    DPRINTF1( "nvI2CReadBuf: addr = %x\n", addr );
    DPRINTF1( "nvI2CReadBuf: readSize = %x\n", readSize );
    
    DPRINTF0( "nvI2CReadBuf: sending read request\n" );

#ifdef NV3
    while ( pNV->subchannel[subchan].control.free < 4*4 );

    pNV->subchannel[subchan].control.object = pI2C->baseInst+NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].externalSerialBus.Read.size = readSize;
    pNV->subchannel[subchan].externalSerialBus.Read.address = addr;
    pNV->subchannel[subchan].externalSerialBus.Read.getDataNotify = 
	NV_051_NOTIFY_WRITE_ONLY;
#else
    while ( pNV->subchannel[subchan].control.Free < 4*4 )
        __yield();

#ifdef DO_TIMING
    startLo = clockop(1);	/* start timing */
#endif

    pNV->subchannel[subchan].SetObject = pI2C->baseInst+NVI2C_SERIAL_BUS;
    pNV->subchannel[subchan].nv04ExternalSerialBus.ReadSize = readSize;
    pNV->subchannel[subchan].nv04ExternalSerialBus.ReadAddress = addr;
    pNV->subchannel[subchan].nv04ExternalSerialBus.ReadGetDataNotify = 
	NV051_NOTIFY_WRITE_ONLY;
#endif

    /*
     * wait for read to complete
     */

#ifndef DO_TIMING
    DPRINTF0( "nvI2CReadBuf: waiting for read to complete\n" );
#endif

    while ( timeOut > 0 ) {

#ifdef NV3
	if ( (volatile) (pI2C->nvNotifier[NV_051_NOTIFICATION_READ].status) == NV_STATUS_IN_PROGRESS ) {
#else
	if ( (volatile) (pI2C->nvNotifier[NV051_NOTIFICATION_READ_GET_DATA_NOTIFY].status) == 
	    NV051_NOTIFICATION_STATUS_IN_PROGRESS ) {
#endif
	    /* data is not ready yet */
	    timeOut--;
	    __yield();
	}
	else {
	    /* stop waiting, the data is ready */
	    break; 
	}

    }

    if ( timeOut == 0 ) {
	DPRINTF0( "nvI2CReadWord: error, read timed out\n" );
	return(1);
    }

    /*
     * get the read data
     */

#ifdef NV3
    data = (UINT)(volatile)(pI2C->nvNotifier[NV_051_NOTIFICATION_READ].otherInfo32);
#else
    data = (UINT)(volatile)(pI2C->nvNotifier[NV051_NOTIFICATION_READ_GET_DATA_NOTIFY].info32);
#endif

#ifdef DO_TIMING
    deltaLo = clockop(0);	 /* stop timing */
#ifdef DRIVER
    _Sprintf( i2cDbgBuf, "nvi2CReadBuf: startLo = %d\n", startLo );
    Out_Debug_String ( i2cDbgBuf );
    _Sprintf( i2cDbgBuf, "nvi2CReadBuf: deltaLo = %d\n", deltaLo );
    Out_Debug_String ( i2cDbgBuf );
#else
    fprintf( stdout, "nvi2CReadBuf: startLo = %d\n", startLo );
    fprintf( stdout, "nvi2CReadBuf: deltaLo = %d\n", deltaLo );
#endif
#endif /* DO_TIMING */

    DPRINTF1( "nvI2CReadBuf: data = %x\n", data );

    /*
     * reorder the bytes since we're actually being passed a ptr to bytes
     */

    for ( i = 0; i < readBufSize; i++ ) {
	pData[i] = data & 0xff;
	data = data >> 8;
    }

    DPRINTF0( "nvI2CReadBuf: exiting\n" );

    return(0);

}

/*----------------------------------------------------------------------------*/
/*
 * nvI2CWrite
 *
 */

DWORD nvI2CWrite (
    UCHAR	chipAddr, 
    DWORD	subAddrLen, 
    UCHAR*	subAddr, 
    DWORD	dataLen, 
    UCHAR*	data)
{

    UINT	i;
    UINT	dindex;
    UCHAR	buf[8];
    UINT	bufSize;
    UINT	m;
    UINT	rc = 0;

    if ( (subAddrLen < 1) || (dataLen < 1) ) {
    	return(1);
    }

    buf[0] = chipAddr;
    for ( i = 1; i <= subAddrLen; i++ ) {
        buf[i] = subAddr[i-1];
    }
    dindex = i;

    i = 0;
    do {
        m = i & 0x3;
        buf[dindex+m] = data[i];
	if ( (m == 3) || (i == dataLen-1) ) {
	    bufSize = dindex+m+1;
	    rc |= nvI2CWriteBuf(buf, bufSize);
	}
    }
    while( ++i < dataLen );

    return(rc);

}

/*----------------------------------------------------------------------------*/
/*
 * nvI2CRead
 *
 */

DWORD nvI2CRead (
    UCHAR	chipAddr, 
    DWORD	subAddrLen, 
    UCHAR*	subAddr, 
    DWORD	dataLen, 
    UCHAR*	data)
{

    UINT	i;
    UINT	dindex;
    UCHAR	buf[8];
    UINT	bufSize;

    if ( (subAddrLen > 3) || (dataLen > 1) ) {
    	return(1);
    }

    buf[0] = chipAddr;
    for ( i = 1; i <= subAddrLen; i++ ) {
        buf[i] = subAddr[i-1];
    }
    dindex = i;

    bufSize = dindex;

    return ( nvI2CReadBuf(buf, bufSize, data, 1) );

}


/*----------------------------------------------------------------------------*/
/*
 * __GetFlatDataSelector - duplicate of NvGetFlatDataSelector()
 *
 * XXX Eventually, this function will be provided to us.
 */

static int __GetFlatDataSelector()
{
    DWORD	Selector;

    _asm xor eax, eax
    _asm mov ax, ds
    _asm mov Selector, eax
    return (Selector);
}

/*----------------------------------------------------------------------------*/
/*
 * __yield()
 */

#ifdef VXD
VOID VXDINLINE
Time_Slice_Sleep(DWORD timeout)
{
    __asm mov eax, timeout
    VMMCall(Time_Slice_Sleep);
}
#endif

static void __yield()
{
    NVRM_INTERRUPT(pI2C->root, pI2C->dev);
}

/*----------------------------------------------------------------------------*/
/*
 * nvI2CSetControlObj - backdoor to set the ptr the global control object
 */

DWORD nvI2CSetControlObj (
    VOID*	pObj )
{

    pI2C = (NVI2C*) pObj;

    DPRINTF0( "nvI2CSetControlObj: exiting successfully\n" );

    return(0);	/* return 0 to indicate success */

}

/*----------------------------------------------------------------------------*/
/*
 * nvI2CGetControlObj - backdoor to get the ptr the global control object
 */

VOID* nvI2CGetControlObj ()
{

    DPRINTF0( "nvI2CGetControlObj: exiting successfully\n" );

    return(pI2C);

}

/*----------------------------------------------------------------------------*/
/*
 * utility real-time clock routines
 */

#ifdef DO_TIMING

UINT
clockcount( UINT* phi, UINT* plo )
{

    UINT        hi, lo;

    /* emit RDTSC instruction to get clock count */
    __asm
    {
        _emit   0x0f 
        _emit   0x31
        mov         hi, edx
        mov         lo, eax
    }

    *phi = hi;
    *plo = lo;

    return(lo);

}

UINT
deltacount ( 
    UINT startHi, 
    UINT startLo, 
    UINT stopHi, 
    UINT stopLo, 
    UINT* deltaHi, 
    UINT* deltaLo )
{

    if ( stopLo > startLo ) {
        *deltaLo = stopLo - startLo;
	*deltaHi = stopHi - startHi;
    }
    else {
    	*deltaLo = 0xffffffff - ( startLo - stopLo );
	*deltaHi = stopHi - startHi - 1;
    }

    return(*deltaLo);

}

/* 
 * clockop() only works for delta processor clocks < 2^32 - 1 
 * which is ~14 seconds on 300MHz clock
 *
 * start > op = 1, stop -> op = 0
 */

UINT clockop( UINT op )	
{

    static UINT	startHi, startLo;
    static UINT	stopHi, stopLo;
    static UINT	deltaHi, deltaLo;

    if ( op ) {
    	clockcount ( &startHi, &startLo );
	return(startLo);
    }
    else {
    	clockcount ( &stopHi, &stopLo );
    	deltacount ( startHi, startLo, stopHi, stopLo, &deltaHi, &deltaLo );
	return(deltaLo);
    }

}

#endif /* DO_TIMING */

