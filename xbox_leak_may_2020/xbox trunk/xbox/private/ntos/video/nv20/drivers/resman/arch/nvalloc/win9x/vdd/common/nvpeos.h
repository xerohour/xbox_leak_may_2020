/*----------------------------------------------------------------------------*/
/*
 * nvpeos.h
 *
 * Include file for miniVDD side of NVPE
 */

#ifndef _NVPEOS_H
#define _NVPEOS_H

#define NVPE_WIN9X  1

#include "nvpecntx.h"

// debug levels and modules
#include <nv_dbg.h>


//
// prototypes
//
U032 osNVPAllocDmaBuffer    (VOID *pContext,U032 dwSize,VOID **pBuffer);
VOID osNVPReleaseDmaBuffer  (VOID *pContext,VOID *pBuffer);
VOID osNVPClearEvent        (VOID *hEvent);
U032 osNVPSetEvent          (VOID *hEvent);
U032 rmNVPAllocContextDma   (U032 hClient,U032 hDma,U032 hClass,U032 flags,U032 selector,U032 base,U032 limit);
U032 rmNVPAllocChannelDma   (U032 hClient,U032 hDevice,U032 hDmaChannel,U032 hClass,U032 hErrorContext,U032 hDataContext,U032 offset,U032 *ppControl);
U032 rmNVPAllocObject       (U032 hClient,U032 hChannel,U032 hObject,U032 hClass);
U032 rmNVPAllocEvent        (NVOS10_PARAMETERS *pEvent);
U032 rmNVPAlloc             (U032 hClient,U032 hChannel,U032 hObject,U032 hClass,PVOID pAllocParms);
U032 rmNVPFree              (U032 hClient,U032 hParent,U032 hObject);
U032 rmNVPInterrupt         (U032 hClient, U032 hDevice);

/* 
 * primary NVPE command IOCTL (must mirror ioctl define in nvpeos.inc) 
 *
 * XXX - need to move this define to a higher level shared include file (jsun) 
 */ 
     
#define VDD_IOCTL_NVPE_COMMAND      0x300CAFE0
 
/*
 * Win9x equivalents for some typedefs used in Win2K
 */
 
typedef struct _HW_DEVICE_EXTENSION {

    PVOID       IRQCallback;
    PVOID       pIRQData;
	ULONG		dwIRQSources;
	ULONG		dwIRQContext;

	NVP_CONTEXT	avpContexts[NVP_MAX_VPORTS];

} HW_DEVICE_EXTENSION, *PHW_DEVICE_EXTENSION;

typedef struct _VIDEO_REQUEST_PACKET {

    ULONG IoControlCode;
    ULONG StatusBlock;      // really a PSTATUS_BLOCK
    PVOID InputBuffer;
    ULONG InputBufferLength;
    PVOID OutputBuffer;
    ULONG OutputBufferLength;

} VIDEO_REQUEST_PACKET, *PVIDEO_REQUEST_PACKET;


/*
 * XXX misc things that might need to be put elsewhere
 */
 
typedef LONG VP_STATUS;

#ifndef NO_ERROR
#define	NO_ERROR			0	/* Success */
#endif

#ifndef ERROR_INVALID_PARAMETER
#define ERROR_INVALID_PARAMETER     87
#endif

// 
// XXX taken from NT's dxmini.h and ddraw.h
// 
#define DX_OK                                   0x0
#define DXERR_UNSUPPORTED                       0x80004001
#define DXERR_GENERIC                           0x80004005
#define DXERR_OUTOFCAPS                         0x88760168

#define DDOVER_AUTOFLIP                       	0x00100000l
#define DDOVER_BOB                       	0x00200000l
#define DDOVER_OVERRIDEBOBWEAVE			0x00400000l
#define DDOVER_INTERLEAVED			0x00800000l

//
// silly typedef because microsoft can't name things the same between NT and Win9x
//
typedef DDVIDEOPORTDATA            * PNVP_DDVIDEOPORTDATA;
typedef DDSKIPINFO                 * PNVP_DDSKIPNEXTFIELDINFO;
typedef DDBOBINFO                  * PNVP_DDBOBNEXTFIELDINFO;
typedef DDSTATEININFO              * PNVP_DDSETSTATEININFO;
typedef DDSTATEOUTINFO             * PNVP_DDSETSTATEOUTINFO;
typedef DDLOCKININFO               * PNVP_DDLOCKININFO;
typedef DDLOCKOUTINFO              * PNVP_DDLOCKOUTINFO;
typedef DDFLIPOVERLAYINFO          * PNVP_DDFLIPOVERLAYINFO;
typedef DDFLIPVIDEOPORTINFO        * PNVP_DDFLIPVIDEOPORTINFO;
typedef DDPOLARITYININFO           * PNVP_DDGETPOLARITYININFO;
typedef DDPOLARITYOUTINFO          * PNVP_DDGETPOLARITYOUTINFO;
typedef DDGETAUTOFLIPININFO        * PNVP_DDGETCURRENTAUTOFLIPININFO;
typedef DDGETAUTOFLIPOUTINFO       * PNVP_DDGETCURRENTAUTOFLIPOUTINFO;
typedef DDGETAUTOFLIPININFO        * PNVP_DDGETPREVIOUSAUTOFLIPININFO;
typedef DDGETAUTOFLIPOUTINFO       * PNVP_DDGETPREVIOUSAUTOFLIPOUTINFO;
typedef DDTRANSFERININFO           * PNVP_DDTRANSFERININFO;
typedef DDTRANSFEROUTINFO          * PNVP_DDTRANSFEROUTINFO;
typedef DDGETTRANSFERSTATUSOUTINFO * PNVP_DDGETTRANSFEROUTINFO;

typedef VOID (*PNVP_DX_IRQCALLBACK)(void);

#endif  /* _NVPEOS_H */

