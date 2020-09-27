/*----------------------------------------------------------------------------*/
/*
 * nvpeos.c
 *
 *  NVPE miniVDD code.
 *
 */

/*----------------------------------------------------------------------------*/
/*
 * include files
 */

// MS includes
#define WANTVXDWRAPS
#include <basedef.h>
#include <vmm.h>
#include <vxdwraps.h>
#include <vwin32.h>

// NV includes 
#include <nv32.h>
#include <nvtypes.h>
#include <nvmacros.h>
#include <nvwin32.h>
#include <nvos.h>
#include <nvrmr0api.h>

#include "ddkmmini.h"
#include "nvpeos.h"

//
// prototypes
//
VP_STATUS   NVPEProcessCommand      (PVOID, PVIDEO_REQUEST_PACKET);
VP_STATUS   NVPProcessState         (PHW_DEVICE_EXTENSION, int, BOOL);
DWORD       NVPCompleteTransfer     (PVOID);
VOID        _vwin32ResetWin32Event  (PVOID);
VOID        _vwin32SetWin32Event    (PVOID);


/*----------------------------------------------------------------------------*/
/*
 * globals
 */
struct _HW_DEVICE_EXTENSION nvpeDevExtension;
struct _VIDEO_REQUEST_PACKET nvpeRequestPacket;
#ifdef DEBUG
int cur_debuglevel = DBG_LEVEL_DEFAULTS;    // all modules set to warning level
#endif

/*----------------------------------------------------------------------------*/
/*
 * nvpeVDDCmdHandler()
 *
 *      Initial NVPE_COMMAND IOCTL handler.  Called from nvmini.asm.
 */

DWORD nvpeVDDCmdHandler(
    PDIOCPARAMETERS  pDIOC)
{

    /*
     * repackage IOCTL parameters and call the real NVPE handler
     */
    nvpeRequestPacket.IoControlCode = pDIOC->dwIoControlCode;
    nvpeRequestPacket.StatusBlock = 0;
    nvpeRequestPacket.InputBuffer = (PVOID)(pDIOC->lpvInBuffer);
    nvpeRequestPacket.InputBufferLength = pDIOC->cbInBuffer;
    nvpeRequestPacket.OutputBuffer = (PVOID)(pDIOC->lpvOutBuffer);
    nvpeRequestPacket.OutputBufferLength = pDIOC->cbOutBuffer;

    NVPEProcessCommand(&nvpeDevExtension, &nvpeRequestPacket);

    /*
     * nvmini.asm uses a return value of 1 to signal failure
     */
    if (nvpeRequestPacket.StatusBlock == 0) {
        return 0;
    }
    else {
        return 1;
    }

}

/*----------------------------------------------------------------------------*/
/*
 * VideoField0Notify()
 *
 *      Callback function for Field 0 capture.  Called by the RM.
 */

void VideoField0Notify()
{

    NVPProcessState(&nvpeDevExtension, 0, FALSE);

}

/*----------------------------------------------------------------------------*/
/*
 * VideoField1Notify()
 *
 *      Callback function for Field 1 capture.  Called by the RM.
 */

void VideoField1Notify()
{

    NVPProcessState(&nvpeDevExtension, 1, FALSE);

}

/*----------------------------------------------------------------------------*/
/*
 * VBIField0Notify()
 *
 *      Callback function for VBI Field 0 capture.  Called by the RM.
 */

void VBIField0Notify()
{

    NVPProcessState(&nvpeDevExtension, 0, TRUE);

}

/*----------------------------------------------------------------------------*/
/*
 * VBIField1Notify()
 *
 *      Callback function for VBI Field 1 capture.  Called by the RM.
 */

void VBIField1Notify()
{

    NVPProcessState(&nvpeDevExtension, 1, TRUE);

}

/*----------------------------------------------------------------------------*/
/*
 * NVPTransferNotify()
 *
 *      Callback function VBI/Video Transfer code.  Called by the RM.
 */

void NVPTransferNotify()
{
    NVPCompleteTransfer(&nvpeDevExtension);
}

/*----------------------------------------------------------------------------*/
/*
 * Misc os dependent utility functions.
 *
 */

U032 osNVPAllocDmaBuffer(
    PVOID pContext, 
    DWORD dwSize, 
    PVOID *pBuffer)
{

    *pBuffer = _HeapAllocate(dwSize, 0);
    if (*pBuffer == NULL) {
        return 1;
    }
    else {
        return 0;
    }

}

void osNVPReleaseDmaBuffer(
    PVOID pContext, 
    PVOID pBuffer)
{
    _HeapFree((BYTE *)pBuffer, 0);
}

void osNVPClearEvent(void* hEvent)
{
    _vwin32ResetWin32Event(hEvent);
}

U032 osNVPSetEvent(void *hEvent)
{
    _vwin32SetWin32Event(hEvent);
    return 0;
}

U032 rmNVPAllocContextDma(
    U032 hClient,
    U032 hDma,
    U032 hClass,
    U032 flags,
    U032 selector,
    U032 base,
    U032 limit)
{

    return NvRmR0AllocContextDma(hClient, hDma, hClass, flags, (PVOID) base, limit);

}

U032 rmNVPAllocChannelDma(
    U032 hClient,
    U032 hDevice,
    U032 hDmaChannel,
    U032 hClass,
    U032 hErrorContext,
    U032 hDataContext,
    U032 offset,
    U032 *ppControl)
{

    return NvRmR0AllocChannelDma(hClient, hDevice, hDmaChannel, hClass, hErrorContext, hDataContext, offset, (PVOID) ppControl);

}

U032 rmNVPAllocObject(
    U032 hClient,
    U032 hChannel,
    U032 hObject,
    U032 hClass)
{

    return NvRmR0AllocObject(hClient, hChannel, hObject, hClass);

}

U032 rmNVPAlloc(
    U032 hClient,
    U032 hChannel,
    U032 hObject,
    U032 hClass,
    PVOID pAllocParms)
{

    return NvRmR0Alloc(hClient, hChannel, hObject, hClass, pAllocParms);

}

U032 rmNVPFree(
    U032 hClient,
    U032 hParent,
    U032 hObject)
{

    return NvRmR0Free(hClient, hParent, hObject);

}

U032 rmNVPAllocEvent(
    NVOS10_PARAMETERS *pEvent)
{
    return NvRmR0AllocEvent(pEvent->hRoot, 
                            pEvent->hObjectParent, 
                            pEvent->hObjectNew, 
                            pEvent->hClass, 
                            pEvent->index, 
                            (PVOID)(pEvent->hEvent.low));
}

U032 rmNVPInterrupt(
    U032 hClient,
    U032 hDevice)
{
    return NvRmR0Interrupt(hClient, hDevice);
}

void nvpeVDDClearState()
{
    DWORD   i;
    unsigned char *pBuf;

    // clear our global HwDevExt manually since we can't use bzero()
    pBuf = (unsigned char*) &nvpeDevExtension;
    for (i = 0; i < sizeof(nvpeDevExtension); i++ ) {
        pBuf[i] = 0;
    }
}

#if DEBUG

//---------------------------------------------------------------------------
//
//  Debugging support.
//
//---------------------------------------------------------------------------

VOID osDbgPrintf_va(
    int     module,
    int     debuglevel,
    const char *printf_format,
    va_list arglist)
{
    int     moduleValue = cur_debuglevel;
    char    nv_error_string[MAX_ERROR_STRING];
    char    *p = nv_error_string;
    int     chars_written;

    // get the module
    // put the module we want in the first 2 bits
    moduleValue = moduleValue >> (module * 2);

    // get the value of those two bits
    // mask off the last 30 bits
    moduleValue = moduleValue & 0x00000003;

   if (debuglevel >= moduleValue)
   {
        chars_written = nvDbgSprintf(p, printf_format, arglist);

        _Debug_Printf_Service(p);
   }
}

#endif // DEBUG
