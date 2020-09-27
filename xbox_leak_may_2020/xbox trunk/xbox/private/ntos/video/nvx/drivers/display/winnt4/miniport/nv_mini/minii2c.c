/*----------------------------------------------------------------------------*/
/*
 * minii2c.c
 *
 *  I2C functionality for the miniport
 *
 */

/*----------------------------------------------------------------------------*/
/*
 * include files
 */

#ifdef NVPE

//#define INITGUID     // Instantiate GUID_I2C_INTERFACE

#include <ntstatus.h>
#include "dderror.h"
#include "miniport.h"
#include "ntddvdeo.h"
#include "video.h"

typedef unsigned long   DWORD;
typedef unsigned int    UINT;

#include "nv.h"
#include "nv32.h"
#include "nvos.h"

#ifndef NTSTATUS
typedef LONG NTSTATUS;
#endif
#define PDEVICE_OBJECT  PVOID
#include "i2cgpio.h"

#define WDM_TO_VPE_COMMAND_MASK 0xdead0000

// backdoor VPE command functions
ULONG nvVPECommand(PHW_DEVICE_EXTENSION pHwDevExt, ULONG cmd, PVOID pData);
U032 vddWriteVPEPrescale(PHW_DEVICE_EXTENSION pHwDevExt, U032 ulX, U032 ulY, 
                         U032 ulCap, U032 ulVPUseOverlay, U032 ulFlag);

////////////////////////
// BUGBUG: The following section is extracted from nvmisc.h.
//         The miniport has no access to the nvidia\sdk directory !!!
//
//#include "nvmisc.h"
//
// control struct and defines for NvRmI2CAccess()
//
typedef struct
{
    unsigned long   token;
    unsigned long   cmd;
    unsigned long   port;
    unsigned long   flags;
    unsigned long   data;
    unsigned long   status;
} NVRM_I2C_ACCESS_CONTROL;

// commands
#define NVRM_I2C_ACCESS_ACQUIRE         1
#define NVRM_I2C_ACCESS_RELEASE         2
#define NVRM_I2C_ACCESS_WRITE_BYTE      3
#define NVRM_I2C_ACCESS_READ_BYTE       4
#define NVRM_I2C_ACCESS_NULL            5
#define NVRM_I2C_ACCESS_RESET           6

// flags
#define NVRM_I2C_ACCESS_START_FLAG      0x1
#define NVRM_I2C_ACCESS_STOP_FLAG       0x2
#define NVRM_I2C_ACCESS_ACK_FLAG        0x4

// port
#define NVRM_I2C_ACCESS_PRIMARY_PORT    1
#define NVRM_I2C_ACCESS_SECONDARY_PORT  2

// End of extracted section
//////////////////////////////////////////////

/*----------------------------------------------------------------------------*/
/*
 * misc defines and prototypes
 */

#define MINI_I2C_UNINITIALIZED  0
#define MINI_I2C_INITIALIZED    1
#define MINI_I2C_ACQUIRED   2

#define MINI_I2C_BASE_INST  0xa50f0001

extern ULONG i2cAccess(PVOID, U032, NVRM_I2C_ACCESS_CONTROL *);

/*----------------------------------------------------------------------------*/
/*
 * globals
 *
 */

static UINT     miniI2Cstate = MINI_I2C_UNINITIALIZED;
extern PHWINFO NvDBPtr_Table[];
static NVRM_I2C_ACCESS_CONTROL  rmI2CCtrl;


/*----------------------------------------------------------------------------*/
/*
 * miniI2COpen() - NV version of I2COpen()
 */

NTSTATUS miniI2COpen (
    PDEVICE_OBJECT  pDev,
    UINT        acquire,
    PI2CControl     pI2CCtrl )
{

    UINT            rc;
    NTSTATUS            result = STATUS_SUCCESS;    /* just to be safe */
    PHWINFO         pNvInfo;
    PHW_DEVICE_EXTENSION pHwDevExt;
    ULONG           head = 0;

    pHwDevExt = (PHW_DEVICE_EXTENSION)(VideoPortGetAssociatedDeviceExtension(pDev));

    pNvInfo = NvDBPtr_Table[pHwDevExt->DeviceReference];

     /*
     * check for WDM backdoor to VPE request  (This is allow WDM drivers to set
     * some VPE parameters that aren't or can't be set by WDM's overlay mixer.)
     */

    if ( (pI2CCtrl->Command & 0xffff0000) == WDM_TO_VPE_COMMAND_MASK ) {
        rc = nvVPECommand (pHwDevExt, (pI2CCtrl->Command & 0x0000ffff), (void*) pI2CCtrl);
        pI2CCtrl->Status = rc;
        return rc;
    }

    /*
     * acquire/release I2C service
     */

    if ( acquire == TRUE ) {

    if ( miniI2Cstate == MINI_I2C_ACQUIRED ) {

        /* 
         * I2C has already been acquired, return error 
         */

        pI2CCtrl->dwCookie = 0;
        pI2CCtrl->Status = I2C_STATUS_BUSY;

        result = STATUS_OPEN_FAILED;

    }
    else {

        /* 
         * I2C is available so attempt to acquire it 
         */

        rmI2CCtrl.cmd = NVRM_I2C_ACCESS_ACQUIRE;
        rmI2CCtrl.port = NVRM_I2C_ACCESS_SECONDARY_PORT;
        rmI2CCtrl.flags = 0;

        rc = i2cAccess(pNvInfo, head, &rmI2CCtrl);

        if ( rc != 0 ) {

        /* unable to get I2C service */
        pI2CCtrl->dwCookie = 0;
        pI2CCtrl->Status = I2C_STATUS_ERROR;
        result = STATUS_OPEN_FAILED;

        }
        else {

        /* I2C service was obtained */
        pI2CCtrl->dwCookie = MINI_I2C_BASE_INST; /* for lack of better cookie */
        pI2CCtrl->Status = I2C_STATUS_NOERROR;
        miniI2Cstate = MINI_I2C_ACQUIRED;
        result = STATUS_SUCCESS;

        }

    }

    }
    else {

    if ( (miniI2Cstate != MINI_I2C_ACQUIRED) || (pI2CCtrl->dwCookie != MINI_I2C_BASE_INST) ) {

        /* 
         * I2C has not been previously "acquired" or cookie is wrong 
         */

        pI2CCtrl->Status = I2C_STATUS_ERROR;
        result = STATUS_INVALID_HANDLE;

    }
    else {

        /* 
         * I2C was previously acquired and cookie is valid, so ok to "release" 
         */

        rmI2CCtrl.cmd = NVRM_I2C_ACCESS_RELEASE;
        rmI2CCtrl.port = NVRM_I2C_ACCESS_SECONDARY_PORT;
        rmI2CCtrl.flags = 0;

        rc = i2cAccess(pNvInfo, head, &rmI2CCtrl);

        if ( rc != 0 ) {

        /* error in releasing I2C */
        pI2CCtrl->dwCookie = 0;
        pI2CCtrl->Status = I2C_STATUS_ERROR;
        result = STATUS_OPEN_FAILED;

        }
        else {

        /* I2C released */
        pI2CCtrl->dwCookie = 0;
        pI2CCtrl->Status = I2C_STATUS_NOERROR;
        result = STATUS_SUCCESS;
        miniI2Cstate = MINI_I2C_INITIALIZED;  /* ready to be acquired again */

        }

    }

    }

    return(result);

}

/*----------------------------------------------------------------------------*/
/*
 * miniI2CAccess - NV version of I2CAccess()
 */

NTSTATUS miniI2CAccess (
    PDEVICE_OBJECT  pDev,
    PI2CControl     pI2CCtrl )
{
    UINT            rc;
    ULONG           startFlag;
    ULONG           stopFlag;
    ULONG           ackFlag;
    NTSTATUS            result = STATUS_SUCCESS;
    ULONG           readData = 0;
    //NVRM_I2C_ACCESS_CONTROL   rmI2CCtrl;
    PHWINFO         pNvInfo;
    PHW_DEVICE_EXTENSION pHwDevExt;
    ULONG           head = 0;

    pHwDevExt = (PHW_DEVICE_EXTENSION)(VideoPortGetAssociatedDeviceExtension(pDev));
    pNvInfo = NvDBPtr_Table[pHwDevExt->DeviceReference];

    /*
     * make sure I2C has been acquired
     */

    if ( (miniI2Cstate == MINI_I2C_UNINITIALIZED) || 
     (miniI2Cstate == MINI_I2C_INITIALIZED) ||
     (pI2CCtrl->dwCookie != MINI_I2C_BASE_INST) ) {

        pI2CCtrl->Status = I2C_STATUS_ERROR;
    return(STATUS_INVALID_HANDLE);

    }

    /*
     * retrieve the flags we can deal with
     */

    startFlag = pI2CCtrl->Flags & I2C_FLAGS_START;
    stopFlag = pI2CCtrl->Flags & I2C_FLAGS_STOP;
    ackFlag = pI2CCtrl->Flags & I2C_FLAGS_ACK;

    rmI2CCtrl.flags = 0;

    if ( startFlag ) {
    rmI2CCtrl.flags |= NVRM_I2C_ACCESS_START_FLAG;
    }

    if ( stopFlag ) {
    rmI2CCtrl.flags |= NVRM_I2C_ACCESS_STOP_FLAG;
    }

    if ( ackFlag ) {
    rmI2CCtrl.flags |= NVRM_I2C_ACCESS_ACK_FLAG;
    }

    /*
     * handle the command
     */

    rmI2CCtrl.port = NVRM_I2C_ACCESS_SECONDARY_PORT;

    switch ( pI2CCtrl->Command ) {

      case I2C_COMMAND_WRITE:

    rmI2CCtrl.cmd = NVRM_I2C_ACCESS_WRITE_BYTE;
    rmI2CCtrl.data = pI2CCtrl->Data;
        rc = i2cAccess(pNvInfo, head, &rmI2CCtrl);

    if ( rc == 0 ) {
        pI2CCtrl->Status = I2C_STATUS_NOERROR;
        result = STATUS_SUCCESS;
    }
    else {
        pI2CCtrl->Status = I2C_STATUS_ERROR;
        result = STATUS_ADAPTER_HARDWARE_ERROR;
    }

        break;

      case I2C_COMMAND_READ:

    rmI2CCtrl.cmd = NVRM_I2C_ACCESS_READ_BYTE;
        rc = i2cAccess(pNvInfo, head, &rmI2CCtrl);

    if ( rc == 0 ) {
        pI2CCtrl->Data = (BYTE) (rmI2CCtrl.data & 0xFF);             // returned byte
        pI2CCtrl->Reserved[0] = (BYTE)((rmI2CCtrl.data >> 8) & 0xFF);
        pI2CCtrl->Reserved[1] = (BYTE)((rmI2CCtrl.data >> 16) & 0xFF);
        pI2CCtrl->Reserved[2] = (BYTE)((rmI2CCtrl.data >> 24) & 0xFF);
        pI2CCtrl->Status = I2C_STATUS_NOERROR;
        result = STATUS_SUCCESS;
    }
    else {
        pI2CCtrl->Status = I2C_STATUS_ERROR;
        result = STATUS_ADAPTER_HARDWARE_ERROR;
    }

        break;

      case I2C_COMMAND_NULL:

    rmI2CCtrl.cmd = NVRM_I2C_ACCESS_NULL;
        rc = i2cAccess(pNvInfo, head, &rmI2CCtrl);

    if ( rc == 0 ) {
        pI2CCtrl->Status = I2C_STATUS_NOERROR;
        result = STATUS_SUCCESS;
    }
    else {
        pI2CCtrl->Status = I2C_STATUS_ERROR;
        result = STATUS_ADAPTER_HARDWARE_ERROR;
    }

        break;

      case I2C_COMMAND_STATUS:

    if ( miniI2Cstate == MINI_I2C_ACQUIRED ) {
        /* nothing is going on just return success */
        pI2CCtrl->Status = I2C_STATUS_NOERROR;
        result = STATUS_SUCCESS;
    }
    else {
        /* not applicable since we're synchronous but return something reasonable just in case */
        pI2CCtrl->Status = I2C_STATUS_BUSY;
        result = STATUS_DEVICE_BUSY;
    }

        break;

      case I2C_COMMAND_RESET:

    rmI2CCtrl.cmd = NVRM_I2C_ACCESS_NULL;
        rc = i2cAccess(pNvInfo, head, &rmI2CCtrl);

    if ( rc == 0 ) {
        pI2CCtrl->Status = I2C_STATUS_NOERROR;
        result = STATUS_SUCCESS;
    }
    else {
        pI2CCtrl->Status = I2C_STATUS_ERROR;
        result = STATUS_ADAPTER_HARDWARE_ERROR;
    }

        break;

      default:
    
    pI2CCtrl->Status = I2C_STATUS_ERROR;
    result = STATUS_INVALID_PARAMETER;

        break;

    }

    return(result);

}

/*
 * nvVPECommand() is a backdoor function to allow other Ring0 programs (like WDM)
 * to change or request various VPE functions.
 */

#define NV_VPE_CMD_SET_PRESCALE_MODE    1
#define NV_VPE_CMD_SET_CROP_MODE        2

ULONG nvVPECommand (
    PHW_DEVICE_EXTENSION pHwDevExt,
    ULONG cmd,
    PVOID pData )
{

    switch ( cmd ) {

    case NV_VPE_CMD_SET_PRESCALE_MODE:

        /*
         * This command is to allow WDM drivers to tell VPE to prescale
         * as oppose to cropping.  This is because the overlay mixer does
         * not seem to pass on this information to us.
         */

        {
            PI2CControl pI2CCtrl = pData;

            /*
             * set prescale flag and size
             */
            //Data(lower nibble)    =   0   not capturing
            //                          1   capturing
            //Data(upper nibble)    =   2   X scale
            //                          4   Y scale
            //Status    =   prescale/height * 1000
            if((pI2CCtrl->Data)& 0x40){
                vddWriteVPEPrescale(pHwDevExt,
                                    0, 
                                    ((PI2CControl)pI2CCtrl)->Status,//set Y-prescale 
                                    ((PI2CControl)pI2CCtrl)->Data,  //capture flag
                                    0, 
                                    6);                             //prog. y-pre & cap. flag
            }else if((pI2CCtrl->Data)& 0x20){
                vddWriteVPEPrescale(pHwDevExt,
                                    ((PI2CControl)pI2CCtrl)->Status,//set X-prescale
                                    0, 
                                    ((PI2CControl)pI2CCtrl)->Data,  //capture flag
                                    0, 
                                    0xa);                           //prog. x-pre & cap.flag
            }else if(!((pI2CCtrl->Data)& 0x1)){
                vddWriteVPEPrescale(pHwDevExt,
                                    0,
                                    0, 
                                    0,  //capture flag=0 (not capturing)
                                    0, 
                                    0x2);   
            }
        }
        break;

    case NV_VPE_CMD_SET_CROP_MODE:

        /*
         * This command is to allow WDM drivers to tell VPE to crop
         * as oppose to prescaling.  This is because the overlay mixer does
         * not seem to pass on this information to us.
         */

        {
            PI2CControl pI2CCtrl = pData;

            /*
             * turn off prescale flag
             */

        }
        break;

    default:
        return 1;   
    }

    return 0;
}

void nvpeVDDInit(PHW_DEVICE_EXTENSION pHwDevExt)
{
    //Initialize VPE capture hack for WDM
    vddWriteVPEPrescale(pHwDevExt,
                        1000,   //setting ME X-prescale factor. 1000=no scale
                        1000,   //setting ME Y-prescale factor. 1000=no scale
                        0,      //set to not vpe capture mode 
                        0,      //set default VP surf type = overlay 
                        0xf);   //use all values

}

// A mechanism to populate the WDM prescale values
// This is a back door mechanism and should not really be in this module.
U032 vddWriteVPEPrescale(PHW_DEVICE_EXTENSION pHwDevExt, U032 ulX, U032 ulY, 
                         U032 ulCap, U032 ulVPNotUseOverlay, U032 ulFlag)
{
    PNVP_CONTEXT pVPEContext;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "nvpekmvt: vddWriteVPEPrescale()\n");

    // get a pointer to the appropriate VPE context (BUGBUG: use index 0 for now !!!)
    pVPEContext = &(pHwDevExt->avpContexts[0]);
    
    if(ulFlag & 0x8){
        pVPEContext->ulVPECapturePrescaleXFactor=ulX;
    }

    if(ulFlag & 0x4){
        pVPEContext->ulVPECapturePrescaleYFactor=ulY;
    }

    if(ulFlag & 0x2){
        pVPEContext->ulVPECaptureFlag=ulCap;
    }

    if(ulFlag & 0x1){
        pVPEContext->ulVPNotUseOverSurf=ulVPNotUseOverlay;
    }


    return 0;
}

#endif // #ifdef NVPE
