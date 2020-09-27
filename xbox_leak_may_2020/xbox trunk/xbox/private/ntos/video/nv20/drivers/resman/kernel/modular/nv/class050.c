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

/************************* Monitor Bus Manager ******************************\
*                                                                           *
* Module: CLASS050.C                                                        *
*   This module implements the NV_EXTERNAL_MONITOR_BUS object               *
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
#include <os.h>
#include <vga.h>
#include <i2c.h>
#include <nv32.h>

//
// Externs.
//

//---------------------------------------------------------------------------
//
//  Create/Destroy object routines.
//
//---------------------------------------------------------------------------

RM_STATUS class050Create
(
	PHWINFO      pDev,
    PCLASSOBJECT ClassObject,
    U032         UserName,
    POBJECT     *Object
)
{
    RM_STATUS           status;
    PMONITORBUSOBJECT   pBus;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class050Create\r\n");
    //
    // Create the NV_EXTERNAL_PARALLEL_BUS object.
    //
    status = osAllocMem((VOID **)Object, sizeof(MONITORBUSOBJECT));
    if (status)
        return (status);
    //
    // A nice cast to make the code more readable.
    //
    pBus = (PMONITORBUSOBJECT)*Object;

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

    return(RM_OK);
}

RM_STATUS class050Destroy
(
	PHWINFO pDev,
    POBJECT Object
)
{
    PMONITORBUSOBJECT  pBus;
    RM_STATUS status;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class050Destroy\r\n");
    //
    // A nice cast to make the code more readable.
    //
    pBus = (PMONITORBUSOBJECT)Object;

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

RM_STATUS class050SetContextDmaNotifies
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class050SetContextDmaNotifies\r\n");
    //
    // Set the notify DMA context.
    //
    return (mthdSetNotifyCtxDma(pDev, Object, Method, Offset, Data));
}
RM_STATUS class050Notify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class050Notify\r\n");
    //
    // Set the notification style.
    //
    return (mthdSetNotify(pDev, Object, Method, Offset, Data));
}
RM_STATUS class050StopTransfer
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    U032    Data
)
{
    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class050StopTransfer\r\n");
    return (RM_OK);
}


RM_STATUS class050Write
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PMONITORBUSOBJECT   pBus;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class050Write\r\n");
    //
    // A nice cast to make the code more readable.
    //
    pBus = (PMONITORBUSOBJECT)Object;

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
            break;
        //
        // Set Data
        //
        case 0x08:

            pBus->Write.Data = Data;

            //
            // I2C Bus
            //
            //i2cWrite( (U008)pPioBus->Write.Address,
            //          pPioBus->Write.AddressSize-1,
            //          ((U008 *)&(pPioBus->Write.Address))+1,
            //          pPioBus->Write.DataSize,
            //          (U008 *)&(pPioBus->Write.Data));
                              
            break;
            
        default:
            break;            

    }
    return (RM_OK);
}


RM_STATUS class050Read
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PMONITORBUSOBJECT   pBus;
    U032                data32;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class050Read\r\n");
    //
    // A nice cast to make the code more readable.
    //
    pBus = (PMONITORBUSOBJECT)Object;

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
            //i2cRead( (U008)pPioBus->Read.Address,
            //         pPioBus->Read.AddressSize-1,
            //         ((U008 *)&(pPioBus->Read.Address))+1,
            //         pPioBus->Read.DataSize,
            //         (U008 *)&(pPioBus->Read.Data));
            data32 = 0;
                                          
            //
            // Send back the data
            //
            notifyFillNotifierArray(pDev,  pBus->CBase.NotifyXlate, 
                                     data32, 
                                     0, 
                                     RM_OK, 
                                     NV050_NOTIFICATION_READ_GET_DATA_NOTIFY );
            
            break;
            
        default:
            break;            

    }
    return (RM_OK);
}

RM_STATUS class050SetInterruptNotify
(
	PHWINFO pDev,
    POBJECT Object,
    PMETHOD Method,
    U032    Offset,
    V032    Data
)
{
    PMONITORBUSOBJECT   pBus;

    DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: class050SetInterruptNotify\r\n");
    //
    // A nice cast to make the code more readable.
    //
    pBus = (PMONITORBUSOBJECT)Object;
    
    //
    // Make sure the interrupt is enabled
    //
    
    return (RM_OK);   
}    


