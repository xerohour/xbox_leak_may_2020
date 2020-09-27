 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/************************* Serial Bus Manager *******************************\
*                                                                           *
* Module: CLASS051.C                                                        *
*   This module implements the NV_EXTERNAL_SERIAL_BUS object                *
*   class and its corresponding methods.                                    *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <nvhw.h>
#include <class.h>
#include <dma.h>   
#include <modular.h>
#include <mp.h>
#include <os.h>
#include <vga.h>
#include <i2c.h>
#include <tmr.h>
#include <nv32.h>

// XXX hack - since BusType is unused, use it as a flag to indicate autoincrement mode
#define SET_AUTO_INCREMENT_FLAG(v)    pBus->BusType = (v)
#define AUTO_INCREMENT_FLAG()         pBus->BusType

//
// Externs.
//

extern char strDevNodeRM[];
char strExternalSerialOnPrimary[] = "ExternalSerialOnPrimary";

//---------------------------------------------------------------------------
//
//  Create/Destroy object routines.
//
//---------------------------------------------------------------------------

RM_STATUS class051Create
(
	PHWINFO      pDev,
    PCLASSOBJECT ClassObject,
    U032         UserName,
    POBJECT     *Object
)
{
    RM_STATUS           status;
    PSERIALBUSOBJECT    pBus;
    U032                data32;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class051Create\r\n");
    //
    // Create the NV_EXTERNAL_PARALLEL_BUS object.
    //
    status = osAllocMem((VOID **)Object, sizeof(SERIALBUSOBJECT));
    if (status)
        return (status);
    //
    // A nice cast to make the code more readable.
    //
    pBus = (PSERIALBUSOBJECT)*Object;

    //
    // Init common (software) state.
    //
    classInitCommonObject(*Object, ClassObject, UserName);

    //
    // Initialize the new object structure.
    //
    pBus->Write.AddressSize = 0;
    pBus->Write.DataSize = 0;
    pBus->Write.Address  = 0;
    pBus->Write.Data     = 0;
    pBus->Read.AddressSize = 0;
    pBus->Read.DataSize  = 0;
    pBus->Read.Address   = 0;
    pBus->Read.Notify    = 0;
    pBus->InterruptNotifyPending = 0;
	//
	// initialize the Default Port ID (only affects hw with multiple I2C ports)
	//
    if ( OS_READ_REGISTRY_DWORD(pDev, strDevNodeRM, strExternalSerialOnPrimary, &data32) == RM_OK) {
        pBus->DefaultPortID = NV_I2C_PORT_PRIMARY;
    }
    else {
        pBus->DefaultPortID = NV_I2C_PORT_SECONDARY;
    }

    //
    // Build a FIFO/Hash context for this object
    //
    status = fifoUpdateObjectInstance(pDev,
                                      &pBus->CBase, 
                                      pBus->CBase.ChID, 
                                      pBus->CInstance);
    if (status != RM_OK)
    {
        osFreeMem(*Object);
    }

    SET_AUTO_INCREMENT_FLAG(0);



    return(RM_OK);

}

RM_STATUS class051Destroy
(
	PHWINFO pDev,
    POBJECT Object
)
{
    PSERIALBUSOBJECT  pBus;
    RM_STATUS status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class051Destroy\r\n");
    //
    // A nice cast to make the code more readable.
    //
    pBus = (PSERIALBUSOBJECT)Object;

    //
    // Delete the FIFO/Hash context for this object
    //
    status = fifoDeleteObjectInstance(pDev, &pBus->CBase, pBus->CBase.ChID); 
    if (status)
        return (status);

    return (osFreeMem(Object));
}

//---------------------------------------------------------------------------
//
//  Class method routines.
//
//---------------------------------------------------------------------------

RM_STATUS class051SetContextDmaNotifies
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class051SetContextDmaNotifies\r\n");
    //
    // Set the notify DMA context.
    //
    return (mthdSetNotifyCtxDma(pDev, Object, Method, Offset, Data));
}
RM_STATUS class051Notify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class051Notify\r\n");
    //
    // Set the notification style.
    //
    return (mthdSetNotify(pDev, Object, Method, Offset, Data));
}
RM_STATUS class051StopTransfer
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class051StopTransfer\r\n");
    return (RM_OK);
}


RM_STATUS class051Write
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PSERIALBUSOBJECT   pBus;
    U008               lock;
    U032               NoStopFlag;
    U032               portID;
    U032               Head = 0;    // TO DO: make the class usable on other heads
    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class051Write\r\n");
    //
    // A nice cast to make the code more readable.
    //
    pBus = (PSERIALBUSOBJECT)Object;

    //
    // Process the write request.
    //
    switch (Offset - Method->Low)
    {
        //
        // Set Size
        //
        case 0x00:
            pBus->Write.DataSize = (U016)(Data & 0x0000FFFF);
            pBus->Write.AddressSize = (U016)((Data >> 16) & 0x0000FFFF);
            break;
        //
        // Set Address
        //
        case 0x04:
            pBus->Write.Address = Data;
            SET_AUTO_INCREMENT_FLAG(0);
            break;
        //
        // Set Data
        //
        case 0x08:

            pBus->Write.Data = Data;

            //
            // I2C Bus
            //
    		// Unlock CRTC extended regs
            CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock, Head);
            CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);

            //
            // I2C Bus
            //
            //
            // Check for the hack I2C interface on the MPC daughterboard
            //
            if ((pBus->Write.Address & 0xFF) == pDev->MediaPort.HalInfo.MPCDeviceAddr) {
                i2cWrite_ALT( pDev, Head, pBus->DefaultPortID, 
                          (U008)pBus->Write.Address,
                          (U016)(pBus->Write.AddressSize-1),
                          ((U008 *)&(pBus->Write.Address))+1,
                          pBus->Write.DataSize,
                          (U008 *)&(pBus->Write.Data));
			}
            else {
				// check for override of DefaultPortID 
				// (XXX temporary until PortID is added as a method )
			    if ( pBus->Write.AddressSize & 0x8000 ) {
				    pBus->Write.AddressSize &= 0x7fff;
					portID = NV_I2C_PORT_PRIMARY;
				}
				else {
					portID = pBus->DefaultPortID;
				}

                NoStopFlag = Data & 0xFF000000;
                if ( ((pBus->Write.DataSize <= 3) && NoStopFlag) || AUTO_INCREMENT_FLAG() ) {
                    if ( AUTO_INCREMENT_FLAG() ) {
                        i2cSend ( pDev, Head, portID,
                                  (U008)0,
                                  (U016)0,
                                  NULL,
                                  pBus->Write.DataSize,
                                  (U008 *)&(pBus->Write.Data),
                                  NoStopFlag);
                    }
                    else {
                        i2cSend ( pDev, Head, portID,
                                  (U008)pBus->Write.Address,
                                  (U016)(pBus->Write.AddressSize-1),
                                  ((U008 *)&(pBus->Write.Address))+1,
                                  pBus->Write.DataSize,
                                  (U008 *)&(pBus->Write.Data),
                                  NoStopFlag);
                        SET_AUTO_INCREMENT_FLAG(1);
                    }
                    if ( NoStopFlag == 0 )
                        SET_AUTO_INCREMENT_FLAG(0);
                }
                else {
                    i2cWrite( pDev, Head, portID,
                              (U008)pBus->Write.Address,
                              (U016)(pBus->Write.AddressSize-1),
                              ((U008 *)&(pBus->Write.Address))+1,
                              pBus->Write.DataSize,
                              (U008 *)&(pBus->Write.Data));
                }
			}
               
    		if (lock == 0)
                CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, Head);
                              
            break;
            
        default:
            break;            

    }
    return (RM_OK);
}


RM_STATUS class051Read
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PSERIALBUSOBJECT   pBus;
    U032               val32;
    U008               lock;
    U032               Head = 0;    // TO DO: make the class usable on other heads

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class051Read\r\n");
    //
    // A nice cast to make the code more readable.
    //
    pBus = (PSERIALBUSOBJECT)Object;

    //
    // Process the write request.
    //
    switch (Offset - Method->Low)
    {
        //
        // Set Size
        //
        case 0x00:
            pBus->Read.DataSize = (U016)(Data & 0x0000FFFF);
            pBus->Read.AddressSize = (U016)((Data >> 16) & 0x0000FFFF);
            break;
        //
        // Set Address
        //
        case 0x04:
            pBus->Read.Address = Data;
            break;
        //
        // Get Data
        //
        case 0x08:

            pBus->Read.Notify = Data;

            //
            // I2C Bus
            //
        	// Unlock CRTC extended regs
            CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock, Head);
            CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);
            
            //
            // Check for the hack I2C interface on the MPC daughterboard
            //
            if ((pBus->Read.Address & 0xFF) == pDev->MediaPort.HalInfo.MPCDeviceAddr) {
                i2cRead_ALT( pDev, Head, pBus->DefaultPortID, 
                         (U008)pBus->Read.Address,
                         (U016)(pBus->Read.AddressSize-1),
                         ((U008 *)&(pBus->Read.Address))+1,
                         pBus->Read.DataSize,
                         (U008 *)&(val32));
			}
            else {
				// check for override of DefaultPortID 
				// (XXX temporary until PortID is added as a method )
			    if ( pBus->Read.AddressSize & 0x8000 ) {
				    pBus->Read.AddressSize &= 0x7fff;
					i2cRead( pDev, Head, NV_I2C_PORT_PRIMARY,
							 (U008)pBus->Read.Address,
							 (U016)(pBus->Read.AddressSize-1),
							 ((U008 *)&(pBus->Read.Address))+1,
							 pBus->Read.DataSize,
							 (U008 *)&(val32));
				}
				else {
					i2cRead( pDev, Head, pBus->DefaultPortID,
							 (U008)pBus->Read.Address,
							 (U016)(pBus->Read.AddressSize-1),
							 ((U008 *)&(pBus->Read.Address))+1,
							 pBus->Read.DataSize,
							 (U008 *)&(val32));
				}
			}
                           
    		if (lock == 0)
                CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, Head);
                              
            //
            // Send back the data
            //
            notifyFillNotifierArray( pDev, pBus->CBase.NotifyXlate, 
                                     val32,
                                     0, 
                                     RM_OK, 
                                     NV051_NOTIFICATION_READ_GET_DATA_NOTIFY );
            
            break;
            
        default:
            break;            

    }
    return (RM_OK);
}

RM_STATUS class051SetInterruptNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PSERIALBUSOBJECT   pBus;

    //DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class051SetInterruptNotify\r\n");
    //
    // A nice cast to make the code more readable.
    //
    pBus = (PSERIALBUSOBJECT)Object;
    
    //
    // Make sure the interrupt is enabled
    //
    
    return (RM_OK);   
}    

