/*----------------------------------------------------------------------------*/
/*
 * vddi2c.c
 *
 *  I2C functionality for the miniVDD
 *
 */

/*----------------------------------------------------------------------------*/
/*
 * include files
 */

#define WANTVXDWRAPS
#include <basedef.h>
#include <vmm.h>
#include <vxdwraps.h>

/* NV includes */
#include <nv32.h>
#include <nvtypes.h>
#include <nvmacros.h>
#include <nvwin32.h>
#include <nvos.h>

#include "nvmisc.h"
#include "nvrmr0api.h"
#include "vddi2c.h"

/*----------------------------------------------------------------------------*/
/*
 * misc defines and prototypes
 */

//
// prototypes
//
U032    vddWriteVPEPrescale     (U032, U032, U032, U032, U032);

#define VDD_I2C_UNINITIALIZED   0
#define VDD_I2C_INITIALIZED     1
#define VDD_I2C_ACQUIRED        2
#define VDD_I2C_BASE_INST       0x55a50001
#define WDM_TO_VPE_COMMAND_MASK 0xdead0000

// backdoor VPE command function
ULONG nvVPECommand ( ULONG, void*);

/*
 * globals
 */

ULONG       vddI2Cstate;
ULONG       ourRoot;
ULONG       ourDevice = VDD_I2C_BASE_INST;  // for lack of better device instance

NVRM_I2C_ACCESS_CONTROL rmI2CCtrl;

static oneTimeInit = 0;

/*----------------------------------------------------------------------------*/
/*
 * vddI2COpen() - NV version of I2COpen()
 */

NTSTATUS vddI2COpen (
    DWORD           dummyReturnPtr, 
    PDEVICE_OBJECT  pDev,
    UINT        acquire,
    PI2CControl     pI2CCtrl )
{

    UINT                    rc;
    NTSTATUS                result = STATUS_SUCCESS;
    char                    devName[32];

    /*
     * check for WDM backdoor to VPE request  (This is allow WDM drivers to set
     * some VPE parameters that aren't or can't be set by WDM's overlay mixer.)
     */

    if ( (pI2CCtrl->Command & 0xffff0000) == WDM_TO_VPE_COMMAND_MASK ) {
        rc = nvVPECommand ( (pI2CCtrl->Command & 0x0000ffff), (void*) pI2CCtrl );
        pI2CCtrl->Status = rc;
        return rc;
    }


    /*
     * acquire/release I2C service
     */

    if ( acquire == TRUE ) {

        if ( vddI2Cstate == VDD_I2C_ACQUIRED ) {

            /* 
             * I2C has already been acquired, return error 
             */

            pI2CCtrl->dwCookie = 0;
            pI2CCtrl->Status = I2C_STATUS_BUSY;

            result = STATUS_OPEN_FAILED;

        }
        else {

            /* 
             * I2C is available so acquire it 
             */

            if ( oneTimeInit == 0 ) {
                /* get root and device */
                rc = NvRmR0AllocRoot(&ourRoot);
                if ( rc != NVOS01_STATUS_SUCCESS ) {
                    pI2CCtrl->dwCookie = 0;
                    pI2CCtrl->Status = I2C_STATUS_ERROR;
                    result = STATUS_OPEN_FAILED;
                    return(result);
                }

                rc = NvRmR0AllocDevice(ourRoot, ourDevice, NV01_DEVICE_0, devName);
                if ( rc != NVOS06_STATUS_SUCCESS ) {
                    pI2CCtrl->dwCookie = 0;
                    pI2CCtrl->Status = I2C_STATUS_ERROR;
                    result = STATUS_OPEN_FAILED;
                    NvRmR0Free(ourRoot, ourRoot, ourRoot);
                    return(result);
                }

                oneTimeInit = 1;
            }

            rmI2CCtrl.cmd = NVRM_I2C_ACCESS_CMD_ACQUIRE;
            rmI2CCtrl.port = NVRM_I2C_ACCESS_PORT_SECONDARY;
            rmI2CCtrl.flags = 0;

            rc = NvRmR0I2CAccess ( ourRoot, ourDevice, &rmI2CCtrl );

            if ( rc != 0 ) {

                /* unable to get I2C service */
                pI2CCtrl->dwCookie = 0;
                pI2CCtrl->Status = I2C_STATUS_ERROR;
                result = STATUS_OPEN_FAILED;

            }
            else {

                /* I2C service was obtained */
                pI2CCtrl->dwCookie = VDD_I2C_BASE_INST; /* for lack of better cookie */
                pI2CCtrl->Status = I2C_STATUS_NOERROR;
                vddI2Cstate = VDD_I2C_ACQUIRED;
                result = STATUS_SUCCESS;

            }

        }

    }
    else {

        if ( (vddI2Cstate != VDD_I2C_ACQUIRED) || (pI2CCtrl->dwCookie != VDD_I2C_BASE_INST) ) {

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

            rmI2CCtrl.cmd = NVRM_I2C_ACCESS_CMD_RELEASE;
            rmI2CCtrl.port = NVRM_I2C_ACCESS_PORT_SECONDARY;
            rmI2CCtrl.flags = 0;

            rc = NvRmR0I2CAccess ( ourRoot, ourDevice, &rmI2CCtrl );

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
                vddI2Cstate = VDD_I2C_INITIALIZED;  /* ready to be acquired again */

            }

        }

    }

    return(result);

}

/*----------------------------------------------------------------------------*/
/*
 * vddI2CAccess - NV version of I2CAccess()
 */

NTSTATUS vddI2CAccess (
    DWORD           dummyReturnPtr, 
    PDEVICE_OBJECT  pDev,
    PI2CControl     pI2CCtrl )
{

    UINT            rc;
    ULONG           startFlag;
    ULONG           stopFlag;
    ULONG           ackFlag;
    NTSTATUS        result = STATUS_SUCCESS;

    /*
     * make sure I2C has been acquired
     */

    if ( (vddI2Cstate == VDD_I2C_UNINITIALIZED) || 
         (vddI2Cstate == VDD_I2C_INITIALIZED) ||
         (pI2CCtrl->dwCookie != VDD_I2C_BASE_INST) ) {

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
        rmI2CCtrl.flags |= NVRM_I2C_ACCESS_FLAG_START;
    }

    if ( stopFlag ) {
        rmI2CCtrl.flags |= NVRM_I2C_ACCESS_FLAG_STOP;
    }

    if ( ackFlag ) {
        rmI2CCtrl.flags |= NVRM_I2C_ACCESS_FLAG_ACK;
    }

    /*
     * handle the command
     */

    rmI2CCtrl.port = NVRM_I2C_ACCESS_PORT_SECONDARY;

    switch ( pI2CCtrl->Command ) {

      case I2C_COMMAND_WRITE:

        rmI2CCtrl.cmd = NVRM_I2C_ACCESS_CMD_WRITE_BYTE;
        rmI2CCtrl.data = pI2CCtrl->Data;
        rc = NvRmR0I2CAccess(ourRoot, ourDevice, &rmI2CCtrl);

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

        rmI2CCtrl.cmd = NVRM_I2C_ACCESS_CMD_READ_BYTE;
        rmI2CCtrl.data = pI2CCtrl->Data;
        rc = NvRmR0I2CAccess(ourRoot, ourDevice, &rmI2CCtrl);

        if ( rc == 0 ) {
            pI2CCtrl->Data = (U008) rmI2CCtrl.data;
            pI2CCtrl->Status = I2C_STATUS_NOERROR;
            result = STATUS_SUCCESS;
        }
        else {
            pI2CCtrl->Status = I2C_STATUS_ERROR;
            result = STATUS_ADAPTER_HARDWARE_ERROR;
        }

        break;

      case I2C_COMMAND_NULL:

        rmI2CCtrl.cmd = NVRM_I2C_ACCESS_CMD_NULL;
        rmI2CCtrl.data = pI2CCtrl->Data;
        rc = NvRmR0I2CAccess(ourRoot, ourDevice, &rmI2CCtrl);

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

        if ( vddI2Cstate == VDD_I2C_ACQUIRED ) {
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

        rmI2CCtrl.cmd = NVRM_I2C_ACCESS_CMD_NULL;
        rmI2CCtrl.data = pI2CCtrl->Data;
        rc = NvRmR0I2CAccess(ourRoot, ourDevice, &rmI2CCtrl);

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
                vddWriteVPEPrescale(0, 
                                    ((PI2CControl)pI2CCtrl)->Status,//set Y-prescale 
                                    ((PI2CControl)pI2CCtrl)->Data,  //capture flag
                                    0, 
                                    6);                             //prog. y-pre & cap. flag
            }else if((pI2CCtrl->Data)& 0x20){
                vddWriteVPEPrescale(((PI2CControl)pI2CCtrl)->Status,//set X-prescale
                                    0, 
                                    ((PI2CControl)pI2CCtrl)->Data,  //capture flag
                                    0, 
                                    0xa);                           //prog. x-pre & cap.flag
            }else if(!((pI2CCtrl->Data)& 0x1)){
                vddWriteVPEPrescale(0,
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

void nvpeVDDInit()
{
    //Initialize VPE capture hack for WDM
    vddWriteVPEPrescale(1000,   //setting ME X-prescale factor. 1000=no scale
                        1000,   //setting ME Y-prescale factor. 1000=no scale
                        0,      //set to not vpe capture mode
                        0,      //set default VP surf type = overlay  
                        0xf);   //use all values

}




