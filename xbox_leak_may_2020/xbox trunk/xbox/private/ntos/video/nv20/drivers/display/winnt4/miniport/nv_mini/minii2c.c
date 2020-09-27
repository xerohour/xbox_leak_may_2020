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

/*----------------------------------------------------------------------------*/
/*
 * misc defines and prototypes
 */

DECLSPEC_IMPORT
VOID
KeQuerySystemTime (
    OUT PLARGE_INTEGER CurrentTime
    );

extern ULONG i2cAccess(PVOID, U032, NVRM_I2C_ACCESS_CONTROL *);

/*----------------------------------------------------------------------------*/
/*
 * globals
 *
 */

extern PHWINFO NvDBPtr_Table[];

/*
 * AcquireI2C() - Acquire I2C routine (Common)
 */
NTSTATUS AcquireI2C (
    PDEVICE_OBJECT          pDev,
    PHW_DEVICE_EXTENSION    pHwDevExt,
    PHWINFO                 pNvInfo,
    ULONG                   port,
    ULONG                   head )
{
    NTSTATUS                result;

    // Build command to acquire the I2C port

    pHwDevExt->I2C_Ports[port].rmI2CCtrl.cmd = NVRM_I2C_ACCESS_CMD_ACQUIRE;
    pHwDevExt->I2C_Ports[port].rmI2CCtrl.port = port;
    pHwDevExt->I2C_Ports[port].rmI2CCtrl.flags = 0;

    // Try to acquire the desired I2C port

    if (i2cAccess(pNvInfo, head, &pHwDevExt->I2C_Ports[port].rmI2CCtrl) == 0)
    {
        // I2C port acquired, update I2C structure (Including time)

        pHwDevExt->I2C_Ports[port].ulStatus = I2C_PORT_ACQUIRED;
        pHwDevExt->I2C_Ports[port].ulOwner = (ULONG) pDev;
        KeQuerySystemTime(&pHwDevExt->I2C_Ports[port].lTimeStamp);

        // Set the result status value

        result = STATUS_SUCCESS;
    }
    else    // Could not acquire the I2C port
    {
        // Set the result status value

        result = STATUS_OPEN_FAILED;
    }
    // Return result to the caller

    return(result);
}

/*
 * ReleaseI2C() - Release I2C routine (Common)
 */
NTSTATUS ReleaseI2C (
    PDEVICE_OBJECT          pDev,
    PHW_DEVICE_EXTENSION    pHwDevExt,
    PHWINFO                 pNvInfo,
    ULONG                   port,
    ULONG                   head )
{
    NTSTATUS                result;

    // Build command to release the I2C port

    pHwDevExt->I2C_Ports[port].rmI2CCtrl.cmd = NVRM_I2C_ACCESS_CMD_RELEASE;
    pHwDevExt->I2C_Ports[port].rmI2CCtrl.port = port;
    pHwDevExt->I2C_Ports[port].rmI2CCtrl.flags = 0;

    // Try to free the I2C port

    if (i2cAccess(pNvInfo, head, &pHwDevExt->I2C_Ports[port].rmI2CCtrl) == 0)
    {
        // I2C port is now release, update I2C structure

        pHwDevExt->I2C_Ports[port].ulStatus = I2C_PORT_FREE;
        pHwDevExt->I2C_Ports[port].ulOwner = 0;
        pHwDevExt->I2C_Ports[port].lTimeStamp.QuadPart = 0;

        // Set the result status value

        result = STATUS_SUCCESS;
    }
    else    // Error releasing the I2C port
    {
        // Set the result status value

        result = STATUS_OPEN_FAILED;
    }
    // Return result to the caller

    return(result);
}

/*
 * miniI2COpen() - Miniport I2C Open routine (Common)
 */

NTSTATUS miniI2COpen (
    PDEVICE_OBJECT  pDev,
    UINT            acquire,
    PI2CControl     pI2CCtrl,
    ULONG           port,
    ULONG           head )
{
    PHW_DEVICE_EXTENSION    pHwDevExt;
    PHWINFO                 pNvInfo;
    NTSTATUS                result;
    LARGE_INTEGER           time, elapsed;

    // Get hardware device extension and information based on the device object

    pHwDevExt = (PHW_DEVICE_EXTENSION)(VideoPortGetAssociatedDeviceExtension(pDev));
    pNvInfo = NvDBPtr_Table[pHwDevExt->DeviceReference];

    // Check for WDM backdoor to VPE request  (This is allow WDM drivers to set
    // some VPE parameters that aren't or can't be set by WDM's overlay mixer.)

    if ((pI2CCtrl->Command & 0xffff0000) == WDM_TO_VPE_COMMAND_MASK)
    {
        // Call routine to process the VPE command

        pI2CCtrl->Status = nvVPECommand(pHwDevExt, (pI2CCtrl->Command & 0x0000ffff), (void*) pI2CCtrl);

        // Set result to status of VPE command

        result = pI2CCtrl->Status;
    }
    else    // This is a normal I2C open command
    {
        // Check to see if this is an acquire/release request

        if (acquire == TRUE)
        {
            // This is an acquire request, check for I2c port free

            if (pHwDevExt->I2C_Ports[port].ulStatus == I2C_PORT_FREE)
            {
                // I2C port is free, try to acquire the desired I2C port

                if (AcquireI2C(pDev, pHwDevExt, pNvInfo, port, head) == STATUS_SUCCESS)
                {
                    // I2C port acquired, set the I2C and result status values

                    pI2CCtrl->dwCookie = (ULONG) pDev;
                    pI2CCtrl->Status = I2C_STATUS_NOERROR;
                    result = STATUS_SUCCESS;
                }
                else    // Could not acquire the I2C port
                {
                    // Set the I2C and result status values

                    pI2CCtrl->dwCookie = 0;
                    pI2CCtrl->Status = I2C_STATUS_ERROR;
                    result = STATUS_OPEN_FAILED;
                }
            }
            else    // This port is already acquired
            {
                // Compute the amount of time since last I2C access

                KeQuerySystemTime(&time);
                elapsed.QuadPart = time.QuadPart - pHwDevExt->I2C_Ports[port].lTimeStamp.QuadPart;

                // Check to see if port timeout has occurred

                if ((elapsed.HighPart != 0) || (elapsed.LowPart > I2C_PORT_TIMEOUT))
                {
                    // Port timeout has occurred, try to release current owner

                    if (ReleaseI2C((PDEVICE_OBJECT) pHwDevExt->I2C_Ports[port].ulOwner, pHwDevExt, pNvInfo, port, head) == STATUS_SUCCESS)
                    {
                        // Current owner released, try new owner acquire

                        if (AcquireI2C(pDev, pHwDevExt, pNvInfo, port, head) == STATUS_SUCCESS)
                        {
                            // I2C port acquired, set I2C and result status values

                            pI2CCtrl->dwCookie = (ULONG) pDev;
                            pI2CCtrl->Status = I2C_STATUS_NOERROR;
                            result = STATUS_SUCCESS;
                        }
                        else    // Unable to acquire for new owner
                        {
                            // Set the I2C and result status values

                            pI2CCtrl->dwCookie = 0;
                            pI2CCtrl->Status = I2C_STATUS_BUSY;
                            result = STATUS_OPEN_FAILED;
                        }
                    }
                    else    // Unable to release current owner
                    {
                        // Set the I2C and result status values

                        pI2CCtrl->dwCookie = 0;
                        pI2CCtrl->Status = I2C_STATUS_BUSY;
                        result = STATUS_OPEN_FAILED;
                    }
                }
                else    // Port access has not timed out
                {
                    // Set the I2C and result status values

                    pI2CCtrl->dwCookie = 0;
                    pI2CCtrl->Status = I2C_STATUS_BUSY;
                    result = STATUS_OPEN_FAILED;
                }
            }
        }
        else    // This is a release request
        {
            // Check to make sure port is acquired and the correct owner

            if ((pHwDevExt->I2C_Ports[port].ulStatus == I2C_PORT_ACQUIRED)  &&
                (pHwDevExt->I2C_Ports[port].ulOwner == (ULONG) pDev)        &&
                (pI2CCtrl->dwCookie == (ULONG) pDev))
            {
                // Valid release request, try to release the I2C port

                if (ReleaseI2C(pDev, pHwDevExt, pNvInfo, port, head) == STATUS_SUCCESS)
                {
                    // I2C port released, set the I2C and result status values

                    pI2CCtrl->Status = I2C_STATUS_NOERROR;
                    result = STATUS_SUCCESS;
                }
                else    // Error releasing the I2C port
                {
                    // Set the I2C and result status values

                    pI2CCtrl->Status = I2C_STATUS_ERROR;
                    result = STATUS_OPEN_FAILED;
                }
            }
            else    // Port not acquired or incorrect owner
            {
                // Set the I2C and result status values

                pI2CCtrl->Status = I2C_STATUS_ERROR;
                result = STATUS_INVALID_HANDLE;
            }
        }
    }
    // Return result to the caller

    return(result);
}

/*
 * miniI2CAccess - Miniport I2C Access routine (Common)
 */

NTSTATUS miniI2CAccess (
    PDEVICE_OBJECT  pDev,
    PI2CControl     pI2CCtrl,
    ULONG           port,
    ULONG           head )
{
    PHW_DEVICE_EXTENSION    pHwDevExt;
    PHWINFO                 pNvInfo;
    NTSTATUS                result;

    // Get hardware device extension and information based on the device object

    pHwDevExt = (PHW_DEVICE_EXTENSION)(VideoPortGetAssociatedDeviceExtension(pDev));
    pNvInfo = NvDBPtr_Table[pHwDevExt->DeviceReference];

    // Check to make sure port is acquired and the correct owner

    if ((pHwDevExt->I2C_Ports[port].ulStatus == I2C_PORT_ACQUIRED)  &&
        (pHwDevExt->I2C_Ports[port].ulOwner == (ULONG) pDev)        &&
        (pI2CCtrl->dwCookie == (ULONG) pDev))
    {
        // I2C port is owned, update the I2C port access time stamp

        KeQuerySystemTime(&pHwDevExt->I2C_Ports[port].lTimeStamp);

        // Start building the I2C command (Common flags and port)

        pHwDevExt->I2C_Ports[port].rmI2CCtrl.port = port;
        pHwDevExt->I2C_Ports[port].rmI2CCtrl.flags = 0;

        if (pI2CCtrl->Flags & I2C_FLAGS_START)
            pHwDevExt->I2C_Ports[port].rmI2CCtrl.flags |= NVRM_I2C_ACCESS_FLAG_START;
        if (pI2CCtrl->Flags & I2C_FLAGS_STOP)
            pHwDevExt->I2C_Ports[port].rmI2CCtrl.flags |= NVRM_I2C_ACCESS_FLAG_STOP;
        if (pI2CCtrl->Flags & I2C_FLAGS_ACK)
            pHwDevExt->I2C_Ports[port].rmI2CCtrl.flags |= NVRM_I2C_ACCESS_FLAG_ACK;

        // Switch on the requested I2C command        

        switch(pI2CCtrl->Command)
        {
            case I2C_COMMAND_WRITE:     // I2c write byte command

                // Complete building the I2C command

                pHwDevExt->I2C_Ports[port].rmI2CCtrl.cmd = NVRM_I2C_ACCESS_CMD_WRITE_BYTE;
                pHwDevExt->I2C_Ports[port].rmI2CCtrl.data = pI2CCtrl->Data;

                // Try to perform the requested write command

                if (i2cAccess(pNvInfo, head, &pHwDevExt->I2C_Ports[port].rmI2CCtrl) == 0)
                {
                    // Set the I2C and result status values

                    pI2CCtrl->Status = I2C_STATUS_NOERROR;
                    result = STATUS_SUCCESS;
                }
                else    // Error performing write command
                {
                    // Set the I2C and result status values

                    pI2CCtrl->Status = I2C_STATUS_ERROR;
                    result = STATUS_ADAPTER_HARDWARE_ERROR;
                }
                break;

            case I2C_COMMAND_READ:      // I2C read byte command

                // Complete building the I2C command

                pHwDevExt->I2C_Ports[port].rmI2CCtrl.cmd = NVRM_I2C_ACCESS_CMD_READ_BYTE;

                // Try to perform the requested read command

                if (i2cAccess(pNvInfo, head, &pHwDevExt->I2C_Ports[port].rmI2CCtrl) == 0)
                {
                    // Save the I2C read data (Complete DWORD)

                    pI2CCtrl->Data        = (BYTE) (pHwDevExt->I2C_Ports[port].rmI2CCtrl.data & 0xFF);
                    pI2CCtrl->Reserved[0] = (BYTE)((pHwDevExt->I2C_Ports[port].rmI2CCtrl.data >>  8) & 0xFF);
                    pI2CCtrl->Reserved[1] = (BYTE)((pHwDevExt->I2C_Ports[port].rmI2CCtrl.data >> 16) & 0xFF);
                    pI2CCtrl->Reserved[2] = (BYTE)((pHwDevExt->I2C_Ports[port].rmI2CCtrl.data >> 24) & 0xFF);

                    // Set the I2C and result status values

                    pI2CCtrl->Status = I2C_STATUS_NOERROR;
                    result = STATUS_SUCCESS;
                }
                else    // Error performing read command
                {
                    // Set the I2C and result status values

                    pI2CCtrl->Data = 0;
                    pI2CCtrl->Status = I2C_STATUS_ERROR;
                    result = STATUS_ADAPTER_HARDWARE_ERROR;
                }
                break;

            case I2C_COMMAND_NULL:      // I2C null command

                // Complete building the I2C command

                pHwDevExt->I2C_Ports[port].rmI2CCtrl.cmd = NVRM_I2C_ACCESS_CMD_NULL;

                // Try to perform the requested null command

                if (i2cAccess(pNvInfo, head, &pHwDevExt->I2C_Ports[port].rmI2CCtrl) == 0)
                {
                    // Set the I2C and result status values

                    pI2CCtrl->Status = I2C_STATUS_NOERROR;
                    result = STATUS_SUCCESS;
                }
                else    // Error performing null command
                {
                    // Set the I2C and result status values

                    pI2CCtrl->Status = I2C_STATUS_ERROR;
                    result = STATUS_ADAPTER_HARDWARE_ERROR;
                }
                break;

            case I2C_COMMAND_STATUS:    // I2C status command

                // Set the I2C and result status values

                pI2CCtrl->Status = I2C_STATUS_NOERROR;
                result = STATUS_SUCCESS;

                break;

            case I2C_COMMAND_RESET:     // I2C reset command

                // Complete building the I2C command

                pHwDevExt->I2C_Ports[port].rmI2CCtrl.cmd = NVRM_I2C_ACCESS_CMD_NULL;

                // Try to perform the requested reset command

                if (i2cAccess(pNvInfo, head, &pHwDevExt->I2C_Ports[port].rmI2CCtrl) == 0)
                {
                    // Set the I2C and result status values

                    pI2CCtrl->Status = I2C_STATUS_NOERROR;
                    result = STATUS_SUCCESS;
                }
                else    // Error performing reset command
                {
                    // Set the I2C and result status values

                    pI2CCtrl->Status = I2C_STATUS_ERROR;
                    result = STATUS_ADAPTER_HARDWARE_ERROR;
                }
                break;

            default:                    // Unknown I2C command

                // Set the I2C and result status values

                pI2CCtrl->Status = I2C_STATUS_ERROR;
                result = STATUS_INVALID_PARAMETER;

                break;
        }
    }
    else    // Port not acquired or incorrect owner
    {
        // Check for an I2C port status command (Only valid non-owner command)

        if (pI2CCtrl->Command == I2C_COMMAND_STATUS)
        {
            // Get the status of the request I2C port (Free/Acquired)

            if (pHwDevExt->I2C_Ports[port].ulStatus == I2C_PORT_FREE)
                pI2CCtrl->Status = I2C_STATUS_NOERROR;
            else
                pI2CCtrl->Status = I2C_STATUS_BUSY;

            // Setup the result status value

            result = STATUS_SUCCESS;
        }
        else    // Invalid I2C access command
        {
            // Set the I2C and result status values

            pI2CCtrl->Status = I2C_STATUS_ERROR;
            result = STATUS_INVALID_HANDLE;
        }
    }
    // Return result to the caller

    return(result);
}

/*
 * NVGetI2CPort(HwID) - Get I2C port based on hardware ID
 */

ULONG
NVGetI2CPort(
    IN ULONG HwID)
{
    ULONG I2CPort;

    // Switch on the hardware ID value

    switch(HwID)
    {
        case 0:

            // Special value indicating dynamic I2C port (Based on real ID)

            I2CPort = I2C_ACCESS_DYNAMIC_PORT;
            break;

        case QUERY_MONITOR_ID:
        case QUERY_MONITOR_ID2:
        case QUERY_NONDDC_MONITOR_ID:
        case QUERY_NONDDC_MONITOR_ID2:
        case QUERY_ACPI_CRT_ID:
        case QUERY_ACPI_DFP_ID:

            // This is a monitor, setup for primary port

            I2CPort = I2C_ACCESS_PRIMARY_PORT;
            break;

        case NVCAP_I2C_DEVICE_ID:
        case NVXBAR_I2C_DEVICE_ID:
        case NVTUNER_I2C_DEVICE_ID:
        case NVTVSND_I2C_DEVICE_ID:

            // This is an external device, setup for secondary port

            I2CPort = I2C_ACCESS_SECONDARY_PORT;
            break;

        default:

            // Unknown hardware device ID, default to secondary port

            VideoDebugPrint((1, "NVGetI2CPort: Unknown hardware ID!\n"));
            I2CPort = I2C_ACCESS_SECONDARY_PORT;
            break;
    }
    return I2CPort;
}

/*
 * NVGetI2CHead(HwID) - Get I2C head based on hardware ID
 */

ULONG
NVGetI2CHead(
    IN ULONG HwID)
{
    ULONG I2CHead;

    // Switch on the hardware ID value

    switch(HwID)
    {
        case QUERY_MONITOR_ID:
        case QUERY_NONDDC_MONITOR_ID:
        case QUERY_ACPI_CRT_ID:
        case QUERY_ACPI_DFP_ID:

            // This is the primary monitor, use head number 0

            I2CHead = 0;
            break;

        case QUERY_MONITOR_ID2:
        case QUERY_NONDDC_MONITOR_ID2:

            // This is the secondary monitor, use head number 1

            I2CHead = 1;
            break;

        case NVCAP_I2C_DEVICE_ID:
        case NVXBAR_I2C_DEVICE_ID:
        case NVTUNER_I2C_DEVICE_ID:
        case NVTVSND_I2C_DEVICE_ID:

            // This is an external device, use head number 0

            I2CHead = 0;
            break;

        default:

            // Unknown hardware device ID, default to head number 0

            VideoDebugPrint((1, "NVGetI2CHead: Unknown hardware ID!\n"));
            I2CHead = 0;
            break;
    }
    // Return the I2C head number

    return I2CHead;
}

/*
 * dynI2COpen() - Dynamic I2C port version of I2COpen()
 */

NTSTATUS dynI2COpen (
    PDEVICE_OBJECT  pDev,
    UINT            acquire,
    PI2CControl     pI2CCtrl )
{
    PHW_DEVICE_EXTENSION pHwDevExt;
    NTSTATUS result = STATUS_SUCCESS;
    ULONG Port;
    ULONG Head;
    ULONG HwID;

    // Get the hardware device extension for this device

    pHwDevExt = (PHW_DEVICE_EXTENSION)(VideoPortGetAssociatedDeviceExtension(pDev));

    // Check to make sure we can convert PDO to hardware ID

    if (pHwDevExt->pfnVideoPortGetAssociatedDeviceID)
    {
        // Convert the PDO value to a hardware ID

        HwID = (*pHwDevExt->pfnVideoPortGetAssociatedDeviceID)(pDev);

        // Get the requested I2C port and head values

        Port = NVGetI2CPort(HwID);
        Head = NVGetI2CHead(HwID);

        // Try to perform the requested I2C open operation

        result = miniI2COpen(pDev, acquire, pI2CCtrl, Port, Head);
    }
    else    // Unable to convert PDO, invalid dynamic I2C open
        result = STATUS_INVALID_PARAMETER;

    return(result);
}

/*
 * dynI2CAccess - Dynamic I2C port version of I2CAccess()
 */

NTSTATUS dynI2CAccess (
    PDEVICE_OBJECT  pDev,
    PI2CControl     pI2CCtrl )
{
    PHW_DEVICE_EXTENSION pHwDevExt;
    NTSTATUS result = STATUS_SUCCESS;
    ULONG Port;
    ULONG Head;
    ULONG HwID;

    // Get the hardware device extension for this device

    pHwDevExt = (PHW_DEVICE_EXTENSION)(VideoPortGetAssociatedDeviceExtension(pDev));

    // Check to make sure we can convert PDO to hardware ID

    if (pHwDevExt->pfnVideoPortGetAssociatedDeviceID)
    {
        // Convert the PDO value to a hardware ID

        HwID = (*pHwDevExt->pfnVideoPortGetAssociatedDeviceID)(pDev);

        // Get the requested I2C port and head values

        Port = NVGetI2CPort(HwID);
        Head = NVGetI2CHead(HwID);

        // Try to perform the requested I2C access operation

        result = miniI2CAccess(pDev, pI2CCtrl, Port, Head);
    }
    else    // Unable to convert PDO, invalid dynamic I2C access
        result = STATUS_INVALID_PARAMETER;

    return(result);
}

/*
 * priI2COpen() - Primary I2C port version of I2COpen()
 */

NTSTATUS priI2COpen (
    PDEVICE_OBJECT  pDev,
    UINT            acquire,
    PI2CControl     pI2CCtrl )
{
    ULONG Port = NVRM_I2C_ACCESS_PORT_PRIMARY;
    ULONG Head = 0;

    // Try to perform the requested I2C open operation

    return miniI2COpen(pDev, acquire, pI2CCtrl, Port, Head);
}

/*
 * priI2CAccess - Primary I2C port version of I2CAccess()
 */

NTSTATUS priI2CAccess (
    PDEVICE_OBJECT  pDev,
    PI2CControl     pI2CCtrl )
{
    ULONG Port = NVRM_I2C_ACCESS_PORT_PRIMARY;
    ULONG Head = 0;
    ULONG HwID;

    // Try to perform the requested I2C access operation

    return miniI2CAccess(pDev, pI2CCtrl, Port, Head);
}

/*
 * secI2COpen() - Secondary I2C port version of I2COpen()
 */

NTSTATUS secI2COpen (
    PDEVICE_OBJECT  pDev,
    UINT            acquire,
    PI2CControl     pI2CCtrl )
{
    ULONG Port = NVRM_I2C_ACCESS_PORT_SECONDARY;
    ULONG Head = 0;

    // Try to perform the requested I2C open operation

    return miniI2COpen(pDev, acquire, pI2CCtrl, Port, Head);
}

/*
 * secI2CAccess - Secondary I2C port version of I2CAccess()
 */

NTSTATUS secI2CAccess (
    PDEVICE_OBJECT  pDev,
    PI2CControl     pI2CCtrl )
{
    ULONG Port = NVRM_I2C_ACCESS_PORT_SECONDARY;
    ULONG Head = 0;

    // Try to perform the requested I2C access operation

    return miniI2CAccess(pDev, pI2CCtrl, Port, Head);
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
