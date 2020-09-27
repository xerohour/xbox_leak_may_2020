 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.        *|
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
|*     Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents     *|
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

/******************************* EDID Functions *****************************
*                                                                           *
* Module: EDID.C                                                            *
*   All EDID related fuctions are in this module.                           *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#include <nvrm.h>
#include <i2c.h>
#include <tmr.h>
#include <edid.h>
#include "nvhw.h"
#include <dac.h>
#include <devinit.h>
#include <vga.h>


U032 getI2CPort(PHWINFO pDev, U032 Head, U032 dispType )
{
    if (IsNV11(pDev)) 
    {
        // We have to know what the mappings of ports to display devices are.
        // Until we can get this from the BIOS, assume the following:
        switch (dispType)
        {
            case DISPLAY_TYPE_MONITOR:
            case DISPLAY_TYPE_DUALSURFACE:
                if (Head == 0) 
                    return pDev->Dac.CRTPortID; // VGA Head 0 use port 0
                else
                    return pDev->Dac.CRT2PortID; // DVI Head 1 use port 1
            case DISPLAY_TYPE_FLAT_PANEL:
                if (Head == 0) 
                    return pDev->Dac.DFPPortID; // DVI Head 0
                else
                    return pDev->Dac.DFP2PortID; // DVI Head 1
            case DISPLAY_TYPE_TV:
                return pDev->Dac.TVOutPortID;
            default:
                return NV_I2C_PORT_PRIMARY;
        }
    }
    else
    {
        // TODO: Use Head as an index into the Dac array
        switch (dispType)
        {
            case DISPLAY_TYPE_MONITOR:
            case DISPLAY_TYPE_DUALSURFACE:
                return pDev->Dac.CRTPortID;
            case DISPLAY_TYPE_FLAT_PANEL:
                return pDev->Dac.DFPPortID;
            case DISPLAY_TYPE_TV:
                return pDev->Dac.TVOutPortID;
            default:
                return NV_I2C_PORT_PRIMARY;
        }
    }
}

static U032 edidTranslateI2CPort(PHWINFO pDev, U008 I2CReadPort)
{
    switch (I2CReadPort)
    {
        case I2C_PHYSICAL_PORT_A_STATUS:
            return NV_I2C_PORT_PRIMARY;
        case I2C_PHYSICAL_PORT_B_STATUS:
            return NV_I2C_PORT_SECONDARY;
        case I2C_PHYSICAL_PORT_C_STATUS:
            return NV_I2C_PORT_TERTIARY;
        default:
            return NV_I2C_PORT_PRIMARY;
    }
}

static U032 edidTranslateDisplayType(U032 displayType)
{
    switch (displayType)
    {
        case DCB_DEV_REC_TYPE_CRT:
        {
            return DISPLAY_TYPE_MONITOR;
        }
        case DCB_DEV_REC_TYPE_TV:
        {
            return DISPLAY_TYPE_TV;
        }
        case DCB_DEV_REC_TYPE_DD_SLINK:
        case DCB_DEV_REC_TYPE_DD_DLINK:
        {
            return DISPLAY_TYPE_FLAT_PANEL;
        }
        default:
            return DISPLAY_TYPE_NONE;
    }
}

static RM_STATUS getDefaultEDIDFromBIOS(PHWINFO, U032, U032);

// Get the monitor to stop driving the DDC bus (DDC1)
// This may only stop the CRT momentarily; we must send a valid DDC2 address
// to be sure it stops permanently.
RM_STATUS StopDDC1(PHWINFO pDev, U032 Head, U032 dispType)
{
U032	i2cPortId = 0;	
U016	status = 1, i;
U008	Data;

    i2cPortId = getI2CPort(pDev, Head, dispType);
    i2cHardwareInit(pDev, Head, i2cPortId);
    tmrDelay(pDev, 15000);
    ResetSCLLine(pDev, Head, i2cPortId);     // drive clock low (data tristated)
    tmrDelay(pDev, 2000000);
    SetSCLLine(pDev, Head, i2cPortId);       // clock high
    SetSDALine(pDev, Head, i2cPortId);       // data high
    for (i=0; i<16; i++)	// give it 16 times
    {
        tmrDelay(pDev, 15000);
        ReadSDA(pDev, Head, i2cPortId, &Data);
        if(Data)        	// SDA = 1?
        {
            status=0;
            break;
        }
    }
    tmrDelay(pDev, 10000);
    i2cStop(pDev, Head, i2cPortId);
    
    // Leave it with clock low (problem with ViewSonic monitors)
    ResetSCLLine(pDev, Head, i2cPortId);     // drive clock low (data tristated)
    tmrDelay(pDev, 2000000);
	
    if (status)
        return (RM_ERROR);
    return (RM_OK);
}

//****************************************************************************************
/*	Determine if monitor is DDC, and in doing so, get it ready to read the EDID or VDIF
	This means get it in DDC2 mode and send the address. Try addresses A0 and A2.
*/
RM_STATUS IsMonitorDDC(PHWINFO pDev, U032 Head, U032 dispType, U008 *dispAddr)
{
    RM_STATUS status;
    U008	Adr, Data;
    U032    i2cPortId;

    i2cPortId = getI2CPort(pDev, Head, dispType);
    // NV11 can drive a CRT on either head; earlier devices only on port 0.
    if (IsNV11(pDev) || (i2cPortId == 0))  
    {
        StopDDC1(pDev, Head, dispType);	// stop the CRT from driving the bus
    }

    // now monitor is in DDC2 mode; we can talk to it
    // send address 0 (start of EDID), no other data
    Adr = 0;
    *dispAddr = DDC_CRT_ADR1;
    if (dispType == DISPLAY_TYPE_MONITOR) {
        status = i2cWrite(pDev, Head, i2cPortId, DDC_CRT_ADR1, 1, &Adr, 0, &Data);   // if it fails, its not DDC
    }
    else if (dispType == DISPLAY_TYPE_FLAT_PANEL)
    {
        // if flat panel, device address can be A0, A2, or A6 (50/51/53 shifted left)
	    status = i2cWrite(pDev, Head, i2cPortId, DDC_CRT_ADR1, 1, &Adr, 0, &Data);   
        if (status)
        {
            *dispAddr = DDC_CRT_ADR2;
		    status = i2cWrite(pDev, Head, i2cPortId, DDC_CRT_ADR2, 1, &Adr, 0, &Data);  
            if (status)
            {
                *dispAddr = DDC_CRT_ADR3;
    		    status = i2cWrite(pDev, Head, i2cPortId, DDC_CRT_ADR3, 1, &Adr, 0, &Data);  
            }
        }
    }
    else
    {
        // not a DDC device
        status = RM_ERROR;
    }
    return status;
}

U032 edidGetVersion(PHWINFO pDev, U032 Head)
{
    U008 i, index = 0;

    // check for version 1
    if (pDev->Dac.CrtcInfo[Head].EDID[0]	== 0x00)
    {
        for (i=1; i<8; i++)
            if (pDev->Dac.CrtcInfo[Head].EDID[i] == 0xff)
                index = i;
            else 
                break;
        if (pDev->Dac.CrtcInfo[Head].EDID[index+1]	== 0x00)
            return 1;		
    }	
    if ((pDev->Dac.CrtcInfo[Head].EDID[EDID_V2_VERSION_NUMBER_INDEX] & 0xf0) == 0x20)
        return 2;

    return 0;	// Invalid or unknown EDID
}

//****************************************************************************************
/*	Checksum an EDID
 */
static BOOL EDIDCheckSumOk(
    PHWINFO pDev, 
    U032    Head, 
    U008   *edid,
    U032    length
)
{
    U032 j;
    int  chksum;

    // This is version 1 header
    // checksum the block
    chksum = 0;

    for (j=0; j<length; j++)
        chksum += *edid++;
 
    return ((U008) chksum) == 0;
}

//****************************************************************************************
/*	Read EDID
    Read the EDID of the specified device into the caller's buffer
    The device is specified by the DWORD which is a bit map of up to 8 CRT's, 8 DFP's, and 8 TV's.
*/
RM_STATUS edidReadDevEDID(PHWINFO pDev, U032 device, U008 *pEdid)
{
    RM_STATUS    status = RM_ERROR;
#ifndef IKOS    // This is just too painfully slow
    int	    i,j;
    U008    block_ID;
    U032    i2cPortID, Head, dispType;
	U008 	dispAddr;



    status = dacGetDisplayInfo(pDev, device, &Head, &dispType, 0, 0);

    if (status != RM_OK)
    {
        return status;
    }

    dispType = edidTranslateDisplayType(dispType);

    if (dispType == DISPLAY_TYPE_NONE)
    {
        status = RM_ERROR;
        return status;
    }
    //
    // Look for EDID overrides for mobile operation
    //
    if (dispType == DISPLAY_TYPE_FLAT_PANEL && pDev->Power.MobileOperation)
    {
        status = edidConstructMobileInfo(pDev, Head, pEdid);
        if (status == RM_OK)
            return RM_OK;
    }

	// See if device supports DDC and which address it uses
    status = IsMonitorDDC(pDev, Head, dispType, &dispAddr);
    if (status != RM_OK)
    {
		DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Display is not DDC; head ", Head);
   	    // Monitor is not DDC compliant.
        goto done;
    }

    i2cPortID = getI2CPort(pDev, Head, dispType);

	for (i=0; i<DDC_RETRIES; i++)
	{
        block_ID = 0;
        // send a write command and read back a block 
        // NOTE: the device address is shifted up in our function (50->A0)
        status = i2cRead(pDev, Head, i2cPortID, dispAddr, 1, &block_ID, 128, pEdid);
        if (status == RM_OK)
        {
            // Check which version of the header. If it was version 2, we need to read
            // another 128 bytes.
            if (pEdid[0] == 0 && pEdid[1] == 0xff &&
                pEdid[EDID_V1_VERSION_NUMBER_INDEX] == 0x1)
            {
            	if (EDIDCheckSumOk(pDev, Head, pEdid, 128))
            	    goto done;
            }
            else if ((pEdid[0] >> 4) == 2)
            {
                // This is a version 2 header. Read the next 128 byte block.
                block_ID = 128;
                // send a write command and read the next block 
                // NOTE: the device address is shifted up in our function (50->A0)
                status = i2cRead(pDev, Head, i2cPortID, dispAddr, 1, &block_ID, 128, 
                                 &(pEdid[128]));
                if (status == RM_OK)
	            	if (EDIDCheckSumOk(pDev, Head, pEdid, 256))
	            	    goto done;
            }
    	}
        // if we failed, try to reset the slave
        for (j=0; j< 10; j++) 
        {
            i2cStart(pDev, Head, i2cPortID);
            tmrDelay(pDev, 10000);
            i2cStop(pDev, Head, i2cPortID);
            tmrDelay(pDev, 10000);
        }


    } // retry

	// if we fall out of the retry loop, then its an error
	status = RM_ERROR;
	
done:
	if (status == RM_OK)
	    return RM_OK;

#ifdef MACOS
    // one last chance to get an EDID
    status = getDefaultEDIDFromBIOS(pDev, Head, dispType);
#endif
#endif // defined IKOS

    return status;
}


//****************************************************************************************
/*	Read EDID
	dispType is the display we want to read, not necessarily the current device.
    This will alwasy return RM_OK, since we'll get the BIOS default EDID if none if found,
    UNLESS this if for mobile?
*/
RM_STATUS EDIDRead(PHWINFO pDev, U032 Head, U032 dispType)
{
    RM_STATUS    status = RM_ERROR;
#ifndef IKOS    // This is just too painfully slow
    int	    i,j;
    U008    block_ID;
    U032    i2cPortId;
	U008 	dispAddr;

    if (IsNV11(pDev)) 
    {
        AssocDDC(pDev, Head);  // Connect the head to the I2C pins
    }
    
    //
    // Look for EDID overrides for mobile operation
    //
    if ((dispType == DISPLAY_TYPE_FLAT_PANEL) && pDev->Power.MobileOperation)
    {
        status = edidConstructMobileInfo(pDev, Head, pDev->Dac.CrtcInfo[Head].EDID);
        if (status == RM_OK)
            return RM_OK;
    }

	// See if device supports DDC and which address it uses
    status = IsMonitorDDC(pDev, Head, dispType, &dispAddr);
    if (status != RM_OK)
    {
		DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Display is not DDC; head ", Head);
   	    // Monitor is not DDC compliant.
        goto done;
    }

    i2cPortId = getI2CPort(pDev, Head, dispType);

	for (i=0; i<DDC_RETRIES; i++)
	{
        block_ID = 0;
        // send a write command and read back a block 
        // NOTE: the device address is shifted up in our function (50->A0)
        status = i2cRead(pDev, Head, i2cPortId, dispAddr, 1, &block_ID, 128, pDev->Dac.CrtcInfo[Head].EDID);
        if (status == RM_OK)
        {
            // Check which version of the header. If it was version 2, we need to read
            // another 128 bytes.
            if (pDev->Dac.CrtcInfo[Head].EDID[0] == 0 && pDev->Dac.CrtcInfo[Head].EDID[1] == 0xff &&
                pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_VERSION_NUMBER_INDEX] == 0x1)
            {
            	if (EDIDCheckSumOk(pDev, Head, pDev->Dac.CrtcInfo[Head].EDID, 128))
            	    goto done;
            }
            else if ((pDev->Dac.CrtcInfo[Head].EDID[0] >> 4) == 2)
            {
                // This is a version 2 header. Read the next 128 byte block.
                block_ID = 128;
                // send a write command and read the next block 
                // NOTE: the device address is shifted up in our function (50->A0)
                status = i2cRead(pDev, Head, i2cPortId, dispAddr, 1, &block_ID, 128, 
                                 &(pDev->Dac.CrtcInfo[Head].EDID[128]));
                if (status == RM_OK)
	            	if (EDIDCheckSumOk(pDev, Head, pDev->Dac.CrtcInfo[Head].EDID, 256))
	            	    goto done;
            }
    	}
        // if we failed, try to reset the slave (may be hung up and not responding)
        for (j=0; j< 20; j++) 
        {
            i2cStart(pDev, Head, i2cPortId);
            tmrDelay(pDev, 20000);      // 20 us
            i2cStop(pDev, Head, i2cPortId);
            tmrDelay(pDev, 20000);
            // one last desparate measure 
            StopDDC1(pDev, Head, dispType);	// stop the CRT from driving the bus
            tmrDelay(pDev, 2000000);    // 2 ms
        }


    } // retry

	// if we fall out of the retry loop, then its an error
	status = RM_ERROR;
	
done:
	if (status == RM_OK)
	    return RM_OK;

#ifdef MACOS
    // one last chance to get an EDID
    status = getDefaultEDIDFromBIOS(pDev, Head, dispType);
#endif
#endif // defined IKOS

    return status;
}

//****************************************************************************************
/*	See if a device has an EDID 
	dispType is the display we want to read, not necessarily the current device.
*/
RM_STATUS EDIDDetect(PHWINFO pDev, U032 Head, U032 dispType)
{
#ifndef IKOS    // This is just too painfully slow
    int	    i;
    RM_STATUS    status;
    U008    block_ID, lock;
    U032    i2cPortId;
	U008 	dispAddr;
    U008    EdidBuffer[128];

    lock = ReadCRTCLock(pDev, Head);	// Unlock the registers

    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);
    
    if (IsNV11(pDev)) 
    {
        AssocDDC(pDev, Head);  // Connect the head to the I2C pins
    }
	// See if device supports DDC and which address it uses
    if (IsMonitorDDC(pDev, Head, dispType, &dispAddr) != RM_OK)
    {
		DBG_PRINT_STRING_VALUE(DEBUGLEVEL_ERRORS, "NVRM: Display is not DDC; head ", Head);
   	    // Monitor is not DDC compliant.
        return RM_ERROR;
    }

    i2cPortId = getI2CPort(pDev, Head, dispType);

	for (i=0; i<DDC_RETRIES; i++)
	{
        block_ID = 0;
        // send a write command and read back a block (at least 0x50 bytes to get version 2 display technology byte).
        // NOTE: the device address is shifted up in our function (50->A0)
        status = i2cRead(pDev, Head, i2cPortId, dispAddr, 1, &block_ID, 0x50, EdidBuffer);
        if (status == RM_OK)
        {
            // Check which version of the header. If it was version 2,
            if (EdidBuffer[0] == 0 && EdidBuffer[1] == 0xff &&
                EdidBuffer[EDID_V1_VERSION_NUMBER_INDEX] == 0x1)
            {
                // This is version 1 header, get video input definition
                if (EdidBuffer[EDID_V1_VIDEO_INPUT_DEF_INDEX] & BIT(7))     // digital device?
                {
                    if (dispType == DISPLAY_TYPE_FLAT_PANEL) 
                    {
                        status = RM_OK;                                       // we're looking for digital
                        goto   done;
                    }
                    else
                    {
                        status = RM_ERROR;
                        goto   done;
                    }
                }
                else
                {
                    if (dispType == DISPLAY_TYPE_MONITOR)                   // we're looking for analog
                    {
                        status = RM_OK;                                       // we're looking for digital
                        goto   done;
                    }
                    else
                    {
                        status = RM_ERROR;
                        goto   done;
                    }
                }
            }
            else if ((EdidBuffer[0] >> 4) == 2)
            {
                // This is a version 2 header.
                if ((EdidBuffer[EDID_V2_DISPLAY_TECHNOLOGY_INDEX] & DISPLAY_TECHNOLOGY_TYPE_MASK) == 0) // analog device?
                {
                    if (dispType == DISPLAY_TYPE_MONITOR)                   // we're looking for analog
                    {
                        status = RM_OK;                                       // we're looking for digital
                        goto   done;
                    }
                    else
                    {
                        status = RM_ERROR;
                        goto   done;
                    }
                }
                else
                {
                    if (dispType == DISPLAY_TYPE_FLAT_PANEL) 
                    {
                        status = RM_OK;                                       // we're looking for digital
                        goto   done;
                    }
                    else
                    {
                        status = RM_ERROR;
                        goto   done;
                    }
                }
            }
            else
            {
                // Unknown edid header.
                status = RM_ERROR;
            }
    	} // Successfully read the first 128 byte block.
    } // retry
done:    
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, Head);

	return status;
#else   // defined IKOS
    return RM_ERROR;
#endif  // IKOS
}

static RM_STATUS getDefaultEDIDFromBIOS(
    PHWINFO pDev,
    U032    Head,
    U032    dispType
)
{
    RM_STATUS rmStatus = RM_OK;
    U032 offset;
    BMP_Control_Block bmpCtrlBlk;

#ifdef MACOS

    U032 edid_len;               //012345
    char edid_registry_name[16] = "EDID,x";

    // on the Mac, a fallback EDID could be in the registry (published by fcode)
    // Check here before falling back to the BIOS image
    // If it is in the registry
    // XXX this could be generic, I guess.
    
    edid_len = sizeof(pDev->Dac.CrtcInfo[Head].EDID);
    edid_registry_name[5] = 'A' + Head;     // "EDID,A" or "EDID,B"
    
	if ((RM_OK == osReadRegistryBinary(pDev, "", edid_registry_name, pDev->Dac.CrtcInfo[Head].EDID, &edid_len)))
	{
    	if (EDIDCheckSumOk(pDev, Head, pDev->Dac.CrtcInfo[Head].EDID, edid_len))
        {
        	// Ok, it looks good so far, but is it the right "type" (fp or crt)
        	// Can't be too careful, you know....
        	BOOL analog_display;
        	BOOL expected_analog;

        	analog_display  = edidIsAnalogDisplay(pDev, Head);
        	expected_analog = dispType == DISPLAY_TYPE_MONITOR;
        	
        	if (analog_display != expected_analog)
        	{
	            DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: found 'canned' EDID; but wrong type\n");
        	    return RM_ERROR;
        	}
        	
	        DBG_PRINT_STRING(DEBUGLEVEL_TRACEINFO, "NVRM: using 'canned' EDID from registry\n");
    	    return RM_OK;
    	}
	}
#endif //  MACOS

    rmStatus = DevinitGetBMPControlBlock(pDev, &bmpCtrlBlk, &offset);

    if (rmStatus == RM_OK) 
    {
        // we only care about version 5 bmp tables
        switch ( bmpCtrlBlk.CTL_Version )
        {
            case BMP_INIT_TABLE_VER_0:
            case BMP_INIT_TABLE_VER_1:
            case BMP_INIT_TABLE_VER_2:
            case BMP_INIT_TABLE_VER_3:
            case BMP_INIT_TABLE_VER_4:
                 break;
            case BMP_INIT_TABLE_VER_5:
            {
               BMP_Struc_Rev5 bmp_InitTableInfo ;
               rmStatus = BiosReadStructure(pDev,
                                            (U008 *) &bmp_InitTableInfo,
                                            offset,
                                            (U032 *) 0,
                                            BMP_INIT5_FMT);
               if (rmStatus == RM_OK) 
               {
                   rmStatus = BiosReadBytes(pDev,
                                            pDev->Dac.CrtcInfo[Head].EDID,
                                            bmp_InitTableInfo.BMP_LCDEDID,
                                            EDID_V1_SIZE);
               }
               break;
            }
            default:
                rmStatus = RM_ERROR;
        }
    }

    return rmStatus;
}


//****************************************************************************************
/*	Read VDIF
	VDIF follows after last EDID block, so to read a VDIF block we need to get the
	number of EDID blocks (*128) and add the VDIF block (*64)
*/
RM_STATUS ReadVDIF(PHWINFO pDev, U032 Head, U032 dispType, U008 *buffer, U008 device, U008 address)
{
    int     i;
    RM_STATUS  status;
    U008    Adr;
    U032    i2cPortId;

// FILL IN THIS !!!

    // get number of EDID blocks

    // calc VDIF block address

    // read VDIF
    i2cPortId = getI2CPort(pDev, Head, dispType);
    status = RM_OK;       /* macosX's version of gcc thinks 'status' may not be init before use */
    for (i=0; i<DDC_RETRIES; i++)
    {
        // send a write command and read back a block (NOTE: the device address is shifted up in our function, so 50->A0)
        status = i2cRead(pDev, Head, i2cPortId, 0x50, 0, &Adr, 128, buffer);
        if (status == RM_OK)
            break;		
    }

    return (status);
}

// Checks to see if the EDID is present in the pDev and if it matches a valid version.
// The version and revision is returned, as well as a pointer to the requested EDID buffer.
static BOOL EDIDIsValid
(
    PHWINFO pDev,
    U008    **edidPtr,
    U032    *version,
    U032    *revision,
    U032    Head
)
{
    U008 *edid;


    edid = pDev->Dac.CrtcInfo[Head].EDID;
    // Check the version of the edid header.
	if (!(edid[0] == 0 && edid[1] == 0xff))
	{
		// this is not version 1 edid header
		// check if it is version 2 header.
        // Bits 7-4 of byte 0 is the version and bits 3-0 of byte 0 is the revision
		if (((edid[0] & 0xf0) >> 4) == 2)
		{
            *version = 2;
            *revision = edid[0] & 0x0f;
            *edidPtr = edid;
            return TRUE;
		}

		// Unknown edid header.
        return FALSE;
	}

    // Check for version 1 edid header.
    if (edid[EDID_V1_VERSION_NUMBER_INDEX] == 1)
    {
        *version = 1;
        *revision = edid[EDID_V1_REVISION_NUMBER_INDEX];
        *edidPtr = edid;
        return TRUE;
    }

    // Unknown edid header.
    return FALSE;

}

// Checks to see if the EDID is present in the pDev and if it matches a valid version.
// The version and revision is returned, as well as a pointer to the requested EDID buffer.
BOOL edidLogicalEDIDIsValid
(
    U008    **edidPtr,
    U032    *version,
    U032    *revision
)
{
    U008 *edid;


    edid = *edidPtr;
    // Check the version of the edid header.
	if (!(edid[0] == 0 && edid[1] == 0xff))
	{
		// this is not version 1 edid header
		// check if it is version 2 header.
        // Bits 7-4 of byte 0 is the version and bits 3-0 of byte 0 is the revision
		if (((edid[0] & 0xf0) >> 4) == 2)
		{
            *version = 2;
            *revision = edid[0] & 0x0f;
            *edidPtr = edid;
            return TRUE;
		}

		// Unknown edid header.
        return FALSE;
	}

    // Check for version 1 edid header.
    if (edid[EDID_V1_VERSION_NUMBER_INDEX] == 1)
    {
        *version = 1;
        *revision = edid[EDID_V1_REVISION_NUMBER_INDEX];
        *edidPtr = edid;
        return TRUE;
    }

    // Unknown edid header.
    return FALSE;

}

static RM_STATUS EDIDV1Copy
(
    PHWINFO pDev,
    U008    *edid,
    U008    *buffer,
    U032    *bufLen
)
{
    // version 1
    if (*bufLen < EDID_V1_SIZE)
    {
        // There is not enough space in the buffer.
        *bufLen = 0;
        return RM_ERROR;
    }

    // There is enough space in the buffer.
    *bufLen = EDID_V1_SIZE;

	osMemCopy((U008*) buffer, (U008*) edid, *bufLen);
    return RM_OK;
}

static RM_STATUS EDIDV2Copy
(
    PHWINFO pDev,
    U008    *edid,
    U008    *buffer,
    U032    *bufLen
)
{
    // Version 2
    if (*bufLen < EDID_V2_SIZE)
    {
        // There is not enough space in the buffer.
        *bufLen = 0;
        return RM_ERROR;
    }

    // There is enough space in the buffer.
    *bufLen = EDID_V2_SIZE;
    
    osMemCopy((U008*) buffer, (U008*) edid, *bufLen);
    return RM_OK;
}

// This function copies the edid from the pDev into the buffer passed.
// Currently only version 1 edid header is supported.
RM_STATUS EDIDCopy
(
    PHWINFO pDev,
    U008    *buffer,
    U032    *bufLen,
    U032    Head
)
{
    U032 edidVersion, edidRevision;
    U008 *edid;

    if (!EDIDIsValid(pDev, &edid, &edidVersion, &edidRevision, Head)) // verify EDID and get a pointer to it
    {
        // edid not valid
        *bufLen = 0;
        return RM_ERROR;
    }

    if (edidVersion == 1)
    {
        return EDIDV1Copy (pDev, edid, buffer, bufLen);
    }
    else if (edidVersion == 2)
    {
        return EDIDV2Copy (pDev, edid, buffer, bufLen);
    }

    // Unknown edid header.
    *bufLen = 0;
    return RM_ERROR;
} // end of EDIDCopy

#if 0 // no one calling this, so turn it off 
static RM_STATUS EDIDV1GetMaxRefreshRate
(
    PHWINFO pDev,
    U008   *edid,
    U032    hRes,
    U032    vRes,
    U032   *maxRefreshRate
)
{
    U032      i, j, tmpHRes, tmpVRes = ~0, tmpRefresh;

    *maxRefreshRate = 0;
    // Get the max from the established timing section
    switch (hRes)
    {
        case 720:
            if (vRes == 400)
            {
                if (edid[EDID_V1_ESTABLISHED_TIMING_1_INDEX] & 0x40)
                {
                    // Supports 720x400 @ 88Hz.
                    *maxRefreshRate = 88;
                }
                else if (edid[EDID_V1_ESTABLISHED_TIMING_1_INDEX] & 0x80)
                {
                    // supports 720x400 @ 70Hz
                    *maxRefreshRate = 70;
                }

            }
            break;
        case 640:
            if (vRes == 480)
            {
                if (edid[EDID_V1_ESTABLISHED_TIMING_1_INDEX] & 0x04)
                {
                    // Supports 640x480 @ 75Hz.
                    *maxRefreshRate = 75;
                }
                else if (edid[EDID_V1_ESTABLISHED_TIMING_1_INDEX] & 0x08)
                {
                    // supports 640x480 @ 72Hz
                    *maxRefreshRate = 72;
                }
                else if (edid[EDID_V1_ESTABLISHED_TIMING_1_INDEX] & 0x10)
                {
                    // supports 640x480 @ 67Hz
                    *maxRefreshRate = 67;
                }
                else if (edid[EDID_V1_ESTABLISHED_TIMING_1_INDEX] & 0x20)
                {
                    // supports 640x480 @ 60Hz
                    *maxRefreshRate = 60;
                }
            }
            break;
        case 800:
            if (vRes == 600)
            {
                if (edid[EDID_V1_ESTABLISHED_TIMING_2_INDEX] & 0x40)
                {
                    // Supports 800x600 @ 75Hz.
                    *maxRefreshRate = 75;
                }
                else if (edid[EDID_V1_ESTABLISHED_TIMING_2_INDEX] & 0x80)
                {
                    // supports 800x600 @ 72Hz
                    *maxRefreshRate = 72;
                }
                else if (edid[EDID_V1_ESTABLISHED_TIMING_1_INDEX] & 0x01)
                {
                    // supports 800x600 @ 60Hz
                    *maxRefreshRate = 60;
                }
                else if (edid[EDID_V1_ESTABLISHED_TIMING_1_INDEX] & 0x02)
                {
                    // supports 800x600 @ 56Hz
                    *maxRefreshRate = 56;
                }
            }
            break;
        case 832:
            if (vRes == 624)
            {
                if (edid[EDID_V1_ESTABLISHED_TIMING_2_INDEX] & 0x20)
                {
                    // Supports 832x624 @ 75Hz.
                    *maxRefreshRate = 75;
                }
            }
            break;
        case 1024:
            if (vRes == 768)
            {
                if (edid[EDID_V1_ESTABLISHED_TIMING_2_INDEX] & 0x10)
                {
                    // Supports 1024x768 @ 87Hz
                    *maxRefreshRate = 87;
                }
                else if (edid[EDID_V1_ESTABLISHED_TIMING_2_INDEX] & 0x02)
                {
                    // supports 1024x768 @ 75Hz
                    *maxRefreshRate = 75;
                }
                else if (edid[EDID_V1_ESTABLISHED_TIMING_2_INDEX] & 0x04)
                {
                    // supports 1024x768 @ 70Hz
                    *maxRefreshRate = 70;
                }
                else if (edid[EDID_V1_ESTABLISHED_TIMING_2_INDEX] & 0x08)
                {
                    // supports 1024x768 @ 60Hz
                    *maxRefreshRate = 60;
                }
            }
            break;
        case 1280:
            if (vRes == 1024)
            {
                if (edid[EDID_V1_ESTABLISHED_TIMING_2_INDEX] & 0x01)
                {
                    // Supports 1280x1024 @ 75Hz
                    *maxRefreshRate = 75;
                }
            }
            break;
        case 1152:
            if (vRes == 870)
            {
                if (edid[EDID_V1_ESTABLISHED_TIMING_3_INDEX] & 0x80)
                {
                    // Supports 1152x870 @ 75Hz 
                    *maxRefreshRate = 75;
                }
            }
            break;
        default:
            break;
    } // switch on hRes


    // Now check the standard timings to see if there is a higher refresh rate specified.
   	for (i = 0; i < 8; i++)
	{
		if (edid[EDID_V1_STANDARD_TIMINGS_START_INDEX + (i * 2)] != 0x01)
		{
			// there is a standard timing specification.
			tmpHRes = (edid[EDID_V1_STANDARD_TIMINGS_START_INDEX + (i*2)] + 31) * 8;
			tmpRefresh = (edid[EDID_V1_STANDARD_TIMINGS_START_INDEX + (i*2) + 1] & 0x3f) + 60;
			switch ((edid[EDID_V1_STANDARD_TIMINGS_START_INDEX + (i*2) + 1] & 0xc0) >> 6)
			{
			case 0x0:
				// aspect ratio 1:1
				tmpVRes = tmpHRes;
				break;
			case 0x1:
				// aspect ratio 4:3
				tmpVRes = (tmpHRes * 3) / 4;
				break;
			case 0x2:
				// aspect ratio 5:4
				tmpVRes = (tmpHRes * 4) / 5;
				break;
			case 0x3:
				// aspect ratio 16:9
				tmpVRes = (tmpHRes * 9) / 16;
				break;
			}

            // Check if this standard timing matches the resolution we want.
            if (tmpHRes == hRes && tmpVRes == vRes)
            {
                // Resolution matches. 
                // If this refresh rate is > than the one found in established timings, use it.
                if (tmpRefresh > *maxRefreshRate)
                {
                    *maxRefreshRate = tmpRefresh;
                }
            }
		} // A valid standard timing spec
	} // iterate over all 8 standard timings

    // Now check the detailed timing specification for a macting resolution.
	for (i = 0; i < 4; i++)
	{
		int startIndex = EDID_V1_DETAILED_TIMINGS_START_INDEX + (i * EDID_V1_DETAILED_TIMINGS_SIZE);
		if (edid[startIndex] == 0 && edid[startIndex + 1] == 0)
		{
			// This is a Monitor descriptor instead of a detailed timing descriptor.
			switch (edid[startIndex + 3])
			{
                case 0xFA:
                    // Descriptor contains 6 additional standard timing identifications
                    for (j = 0; j < 6; j++)
                    {
                        if (edid[startIndex + 5 + (j * 2)] != 0x01)
                        {
                            // there is a standard timing specification.
                            tmpHRes = (edid[startIndex + 5 + (j*2)] + 31) * 8;
                            tmpRefresh = (edid[startIndex + 5 + (j*2) + 1] & 0x3f) + 60;
                            switch ((edid[startIndex + 5 + (j*2) + 1] & 0xc0) >> 6)
                            {
                            case 0x0:
                                // aspect ratio 1:1
                                tmpVRes = tmpHRes;
                                break;
                            case 0x1:
                                // aspect ratio 4:3
                                tmpVRes = (tmpHRes * 3) / 4;
                                break;
                            case 0x2:
                                // aspect ratio 5:4
                                tmpVRes = (tmpHRes * 4) / 5;
                                break;
                            case 0x3:
                                // aspect ratio 16:9
                                tmpVRes = (tmpHRes * 9) / 16;
                                break;
                            } // switch on aspect ratio

                            // Check if this standard timing matches the resolution we want.
                            if (tmpHRes == hRes && tmpVRes == vRes)
                            {
                                // Resolution matches. 
                                // If this refresh rate is > than the one found in established timings, use it.
                                if (tmpRefresh > *maxRefreshRate)
                                {
                                    *maxRefreshRate = tmpRefresh;
                                }
                            }
                        } // a valid standard timing spec
                    } // iterate over the 6 additional standard timings
                    break;
                default:
                    break;
			}
		} // monitor descriptor.
		else
		{
			// This is a detailed timing descriptor.
            // The detailed timing specification does not include the refresh rate.
			U032 pixelClock;
			U032 hActive, vActive, hBlank, vBlank;
			U032 hTotal, vTotal;


			hActive = edid[startIndex + 2] | ((edid[startIndex + 4] & 0xf0) << 4);
			vActive = edid[startIndex + 5] | ((edid[startIndex + 7] & 0xf0) << 4);
            if (hActive == hRes && vActive == vRes)
            {
                // Resolution matches the detailed spec.
                // Get the rest of the detailed timing info needed to calculate the refresh rate.
                pixelClock = edid[startIndex] | (edid[startIndex + 1] << 8);
                hBlank  = edid[startIndex + 3] | ((edid[startIndex + 4] & 0x0f) << 8);
                vBlank  = edid[startIndex + 6] | ((edid[startIndex + 7] & 0x0f) << 8);

                // Horizontal Total (hActive + hBlank) gives # of pixels / line
                // Vertical total (vActive + vBlank) gives # of lines / frame
                // so HT * VT = total # pixels per frame.
                // Pixel clock is # pixels drawn / sec
                // so pixelClock / (HT * VT) gives the refresh rate (frames / sec).
                hTotal = hActive + hBlank;
                vTotal = vActive + vBlank;
                pixelClock = pixelClock * 10000;
                tmpRefresh = pixelClock / (hTotal * vTotal);

                if (tmpRefresh > *maxRefreshRate)
                {
                    *maxRefreshRate = tmpRefresh;
                }
            } // resolution matches the detailed spec
		} // This is a detailed timing spec
	} // parse detailed section

    return RM_OK;
}

static RM_STATUS EDIDV2GetMaxRefreshRate
(
    PHWINFO pDev,
    U008   *edid,
    U032    hRes,
    U032    vRes,
    U032   *maxRefreshRate
)
{
    U032 lumTabSize, numLumTables;
    U032 numFreqRanges, numDetailedRanges, numTimingCodes, numDetailedTimings;
    U032 detailedTimingIndex, timingCodesIndex;
    U008 timingMap1, timingMap2;
    U032 index, i;

    // We need to look in the 4-byte timing codes section and the detailed timing section for
    // the given resolution.

    // The way the 127 byte detailed information is used is given by reading bytes 0x7e and 0x7f.
    // The size and the order of the 127 byte timing section that start at byte 0x80 is:
    // x*A + 8*B + 27*c + 4*D + 18*E where
    // A = number of luminance tables (0 or 1), (given by byte 0x7e bit 5)
    // x = size of the luminance table (given by the first byte of the table itself)
    // B = number of frequency rages listed (0-7), (given by bits 4-2 of byte 0x7e)
    // C = number of detailed rage limits (0-3), (given by (bits 1-0 of byte 0x7e)
    // D = number of timing codes (0-31), (given by bits 7-3 of byte 0x7f)
    // E = number of detailed timing descriptions (0-7) (given by bits 2-0 of byte 0x7f)
    timingMap1 = edid[EDID_V2_TIMING_MAP1_INDEX];
    timingMap2 = edid[EDID_V2_TIMING_MAP2_INDEX];
    numLumTables       = (timingMap1 & 0x20) >> 5; // Bit 5
    numFreqRanges      = (timingMap1 & 0x1C) >> 2; // Bits 4-2
    numDetailedRanges  = (timingMap1 & 0x03);      // Bits 1-0
    numTimingCodes     = (timingMap2 & 0xf8) >> 3; // Bits 7-3
    numDetailedTimings = (timingMap2 & 0x07);      // Bits 2-0
    if (numLumTables != 0)
    {
        // Calculate the size of the lum table by reading byte 0x80
        U032 numLumValues;

        numLumValues = edid[EDID_V2_TIMING_SECTION_START_INDEX] & 0x1f; // bits 4-0
        if (edid[EDID_V2_TIMING_SECTION_START_INDEX] & 0x80)
        {
            // size of lum table is ((3 * numLumValues) + 1) bytes
            lumTabSize = 3 * numLumValues + 1;
        }
        else
        {
            // size of lum table is (numLumValues + 1) bytes
            lumTabSize = numLumValues + 1;
        }
    }
    else
        lumTabSize = 0;

    timingCodesIndex = EDID_V2_TIMING_SECTION_START_INDEX 
                       + lumTabSize 
                       + (8 * numFreqRanges)
                       + (27 * numDetailedRanges);
    detailedTimingIndex = timingCodesIndex + (8 * numTimingCodes);

    *maxRefreshRate = 0;

    {
        // Look in the timing codes section.
        U032 hActive;
        U032 aspectRatio;
        U032 vActive;
        for (i = 0; i < numTimingCodes; i++)
        {
            index = timingCodesIndex + (4 * i);
            hActive = (edid[index] * 16) + 256;
            aspectRatio = edid[index + 2];
            // Aspect ratio is stored in terms of N:100.
			// This is not very useful because we don't get the standard vertical values.
			// So we will use the standard aspect ratios whenever we can.
			switch (aspectRatio)
			{
			case 133:
				// 4:3
				vActive = (hActive * 3) / 4;
				break;
			case 125:
				// 5:4
				vActive = (hActive * 4) / 5;
				break;
			case 177:
				// 16:9
				vActive = (hActive * 9) / 16;
				break;
			default:
                vActive = (hActive * 100) / aspectRatio;
				break;
			}
            if (hActive == hRes && vActive == vRes)
            {
                // This is the needed resolution.
                if (edid[index + 3] > *maxRefreshRate)
                {
                    *maxRefreshRate = edid[index + 3];
                }
            } // resolution matches
        } // loop over all the timing codes
    } // look in timing codes

    {
        // look in the detailed timing section
        // The detailed timing specification does not include the refresh rate.
        U032 pixelClock;
        U032 hActive, vActive, hBlank, vBlank;
        U032 hTotal, vTotal;
        U032 tmpRefresh;

        for (i = 0; i < numDetailedTimings; i++)
        {
            index = detailedTimingIndex + (18 * i);
            hActive = edid[index + 2] | ((edid[index + 4] & 0xf0) << 4);
            vActive = edid[index + 5] | ((edid[index + 7] & 0xf0) << 4);
            if (hActive == hRes && vActive == vRes)
            {
                // Resolution matches the detailed spec.
                // Get the rest of the detailed timing info needed to calculate the refresh rate.
                pixelClock = edid[index] | (edid[index + 1] << 8);
                hBlank  = edid[index + 3] | ((edid[index + 4] & 0x0f) << 8);
                vBlank  = edid[index + 6] | ((edid[index + 7] & 0x0f) << 8);

                // Horizontal Total (hActive + hBlank) gives # of pixels / line
                // Vertical total (vActive + vBlank) gives # of lines / frame
                // so HT * VT = total # pixels per frame.
                // Pixel clock is # pixels drawn / sec
                // so pixelClock / (HT * VT) gives the refresh rate (frames / sec).
                hTotal = hActive + hBlank;
                vTotal = vActive + vBlank;
                pixelClock = pixelClock * 10000;
                tmpRefresh = pixelClock / (hTotal * vTotal);

                if (tmpRefresh > *maxRefreshRate)
                {
                    *maxRefreshRate = tmpRefresh;
                }
            } // resolution matches the detailed spec
        } // loop over all the detailed timing specs
    } // look in detailed timing section

    return RM_OK;
}
#endif  // #if 0; no one calling this puppy


// EDID Version 1.3
// Super set of 1.2, allows range limits descriptor to define coeficients for secondary GTF curve,
// and mandates a certain set of monitor descriptors.
RM_STATUS ParseEDID13(PHWINFO pDev, U032 Head)
{
	return ParseEDID11(pDev, Head);	// we can probably treat it the same as rev 1.1
}

// EDID Version 1.2
RM_STATUS ParseEDID12(PHWINFO pDev, U032 Head)
{
	return ParseEDID11(pDev, Head);	// we can probably treat it the same as rev 1.1
}

//	Parse a detailed timing block in the current EDID
//	The EDID has been saved in the global device data structure (nvInfo).
//	When native mode is requested, it is necessary to find the correct detailed timing;
//  otherwise fpParseEDID will be used to get timing info.
U032 edidParseDetailedTimingBlock(PHWINFO pDev, U008 block, PDACFPTIMING fpTim, U032 Head)
{
	switch (pDev->Dac.CrtcInfo[Head].EdidVersion)
	{
		case EDID_VERSION_10:
		case EDID_VERSION_11:
		case EDID_VERSION_12:
		case EDID_VERSION_13:
			return edidParseDetailed1(pDev, block, fpTim, Head);
		case EDID_VERSION_20:
			return edidParseDetailed2(pDev, block, fpTim, Head);
	}

	return(0);
}

//	Parse a specific detailed timing block for EDID version 1.x
//  Detailed timings are at a fixed offset from the start of the buffer.
U032 edidParseDetailed1(PHWINFO pDev, U008 block, PDACFPTIMING fpTim, U032 Head)
{
U008	HActive1, HActive2, HBlank1, HSyncOff1, HSyncPW, offset;
U008	VActive1, VActive2, VBlank1, VSyncOff1, SyncOvf, Flags;
U016	pClk, HRes, VRes, HSyncOffset, VSyncOffset, HSyncPulse, VSyncPulse, HBlank, VBlank;
U016	HTotal, VTotal;

		// detailed timing block is 18 (0x12) bytes starting at offset 0x36.
		pClk = pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_DETAILED_TIMINGS_START_INDEX + 18 * block];
		pClk |= (pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_DETAILED_TIMINGS_START_INDEX + 18 * block + 1])<<8;
		if ((pClk != 0) && (pClk != 0x0101))	// 0 and 1 could be fillers or alternate info
		{
			// Seems valid, get info
			offset = EDID_V1_DETAILED_TIMINGS_START_INDEX + (18 * block);
			HActive1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 2];
			HActive2 = pDev->Dac.CrtcInfo[Head].EDID[offset + 4];
			HBlank1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 3];
			VActive1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 5];
			VBlank1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 6];
			VActive2 = pDev->Dac.CrtcInfo[Head].EDID[offset + 7];
			HSyncOff1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 8];
			HSyncPW = pDev->Dac.CrtcInfo[Head].EDID[offset + 9];
			VSyncOff1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 10];
			SyncOvf = pDev->Dac.CrtcInfo[Head].EDID[offset + 11];
			Flags = pDev->Dac.CrtcInfo[Head].EDID[offset + 17];

			// Get timing into useful format
			HRes = HActive1 | (HActive2 & 0xf0)<<4;
			VRes = VActive1 | (VActive2 & 0xf0)<<4;
			HBlank = HBlank1 | (HActive2 & 0x0f)<<8;
			VBlank = VBlank1 | (VActive2 & 0x0f)<<8;
			HTotal = HRes + HBlank;
			VTotal = VRes + VBlank;
			HSyncPulse = HSyncPW | (SyncOvf & 0x30)<<4;
			HSyncOffset = HSyncOff1 | (SyncOvf & 0xc0)<<2;
			VSyncOffset = (VSyncOff1>>4) | (SyncOvf & 0x0c)<<2;
			VSyncPulse = (VSyncOff1 & 0xf) | (SyncOvf & 0x30)<<4;

			// Return this resolution
			fpTim->PixClk = pClk;
			fpTim->HActive = HRes;
 			fpTim->HDispEnd = HRes - 1;
			fpTim->HValidEnd = HRes - 1;
	 		fpTim->HSyncStart = HSyncOffset + fpTim->HDispEnd;
 			fpTim->HSyncEnd = HSyncPulse + fpTim->HSyncStart;
 			fpTim->HValidStart = 0;
	 		fpTim->HTotal = HTotal;
				
	 		fpTim->VActive = VRes;
 			fpTim->VDispEnd = VRes - 1;
 			fpTim->VValidEnd = VRes - 1;
	 		fpTim->VSyncStart =  VSyncOffset + fpTim->VDispEnd;
	 		fpTim->VSyncEnd = VSyncPulse + fpTim->VSyncStart;
 			fpTim->VValidStart = 0;
 			fpTim->VTotal = VTotal;
			fpTim->Flags = Flags;	// includes sync polarities
			return TRUE;
		}
		return FALSE;
} // edidParseDetailedTimingBlock

// Parse detailed timings for EDID Version 2.x
// This gets a specific timing block.
// For version 2, we have to parse from the start to find the location of the block.
U032 edidParseDetailed2(PHWINFO pDev, U008 block, PDACFPTIMING fpTim, U032 Head)
{
U008	map1, map2, index, lum, rangeLimits, detailedRangeLimits;
U008	detailedTimings, timingCodes, timingCode[4];
U008	HActive1, HActive2, HBlank1, HSyncOff1, HSyncPW, offset;
U008	VActive1, VActive2, VBlank1, VSyncOff1, SyncOvf, Flags, i;
U016	pClk, HRes, VRes, HSyncOffset, VSyncOffset, HSyncPulse, VSyncPulse, HBlank, VBlank;
U016	HTotal, VTotal;

	map1 = pDev->Dac.CrtcInfo[Head].EDID[EDID_V2_TIMING_MAP1_INDEX];
	map2 = pDev->Dac.CrtcInfo[Head].EDID[EDID_V2_TIMING_MAP1_INDEX+1];
	index =  EDID_V2_TIMING_SECTION_START_INDEX;
	// if there is a lum table, bump index beyond
	if (map1 & EDID_V2_LUM_TABLE_EXIST)
	{
		// 1st byte in timing section gives # of bytes in lum table
		lum = pDev->Dac.CrtcInfo[Head].EDID[EDID_V2_TIMING_SECTION_START_INDEX];
		if (lum & TRIPLE_LUM_ENTRIES)
			index += (lum & NUM_LUM_ENTRIES*3) + 1;
		else
			index += (lum & NUM_LUM_ENTRIES) + 1;
	}
	// if there are timing range limits, bump index
	if ((rangeLimits = (map1 & EDID_V2_FREQ_RANGES)>>EDID_V2_FREQ_RANGE_SHIFT))
		index += rangeLimits * 8;
	// if there are detailed range limits, bump index
	if ((detailedRangeLimits = map1 & EDID_V2_DETAILED_RANGE_LIMITS))
		index += detailedRangeLimits * 27;
	// if there are timing codes, save them
	if ((timingCodes = (map2 & EDID_V2_TIMING_CODES)>>EDID_V2_TIMING_CODES_SHIFT))
		for (i=0; i!=timingCodes; i++)
		{
			timingCode[i] = pDev->Dac.CrtcInfo[Head].EDID[index];
			timingCode[i+1] = pDev->Dac.CrtcInfo[Head].EDID[index+1];
			timingCode[i+2] = pDev->Dac.CrtcInfo[Head].EDID[index+2];
			timingCode[i+3] = pDev->Dac.CrtcInfo[Head].EDID[index+3];
			index += 4;
		}
	// now index points to the detailed timings (if present)
	if ((detailedTimings = (map2 & EDID_V2_DETAILED_TIMINGS)))
	{
		// detailed timing block is 18 (0x12) bytes starting at offset 0x36.
		pClk = pDev->Dac.CrtcInfo[Head].EDID[index + 18 * block];
		pClk |= (pDev->Dac.CrtcInfo[Head].EDID[index + 18 * block + 1])<<8;
		if ((pClk != 0) && (pClk != 0x0101))	// 0 and 1 could be fillers or alternate info
		{
			// Seems valid, get info
			offset = index + 18 * block;
			HActive1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 2];
			HActive2 = pDev->Dac.CrtcInfo[Head].EDID[offset + 4];
			HBlank1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 3];
			VActive1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 5];
			VBlank1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 6];
			VActive2 = pDev->Dac.CrtcInfo[Head].EDID[offset + 7];
			HSyncOff1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 8];
			HSyncPW = pDev->Dac.CrtcInfo[Head].EDID[offset + 9];
			VSyncOff1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 10];
			SyncOvf = pDev->Dac.CrtcInfo[Head].EDID[offset + 11];
			Flags = pDev->Dac.CrtcInfo[Head].EDID[offset + 17];

			// Get timing into useful format
			HRes = HActive1 | (HActive2 & 0xf0)<<4;
			VRes = VActive1 | (VActive2 & 0xf0)<<4;
			HBlank = HBlank1 | (HActive2 & 0x0f)<<8;
			VBlank = VBlank1 | (VActive2 & 0x0f)<<8;
			HTotal = HRes + HBlank;
			VTotal = VRes + VBlank;
			HSyncPulse = HSyncPW | (SyncOvf & 0x30)<<4;
			HSyncOffset = HSyncOff1 | (SyncOvf & 0xc0)<<2;
			VSyncOffset = (VSyncOff1>>4) | (SyncOvf & 0x0c)<<2;
			VSyncPulse = (VSyncOff1 & 0x0f) | (SyncOvf & 0x30)<<4;

			// Return this resolution
			fpTim->PixClk = pClk;
			fpTim->HActive = HRes;
	 		fpTim->HDispEnd = HRes - 1;
			fpTim->HValidEnd = HRes - 1;
	 		fpTim->HSyncStart = HSyncOffset + fpTim->HDispEnd;
	 		fpTim->HSyncEnd = HSyncPulse + fpTim->HSyncStart;
	 		fpTim->HValidStart = 0;
	 		fpTim->HTotal = HTotal;
				
	 		fpTim->VActive = VRes;
	 		fpTim->VDispEnd = VRes - 1;
	 		fpTim->VValidEnd = VRes - 1;
	 		fpTim->VSyncStart =  VSyncOffset + fpTim->VDispEnd;
	 		fpTim->VSyncEnd = VSyncPulse + fpTim->VSyncStart;
	 		fpTim->VValidStart = 0;
	 		fpTim->VTotal = VTotal;

			fpTim->Flags = Flags;
			return RM_OK;
		} // if 
	} // if detailed timings
	// check 4 byte timing codes

	return RM_ERROR;	// not supported yet
}

// EDID Version 2.0 
// Get flat panel timing from 4 byte timing code
// Use VESA GTF timing.
RM_STATUS GetTimingFromCode(U008 code, PDACFPTIMING fpTim)
{
U016	HorizCode;

	// We should be able to determine the mode from the first byte only, UNLESS
	// we start seeing panels with variable refresh rates.
	// byte 1: active pixels/line (pixels - 256)/16. Range = 256-4336.
	// byte 2: arcane info, don't know what it means. See EDID standard.
	// byte 3: aspect ration H:V.
	// byte 4: refresh rate.
	HorizCode = code * 16 + 256;
	fpTim->Flags = 0;	// sync polarities: neg, neg
	switch (HorizCode)
	{
		case 1280:
			// 1280x1024x75;
			fpTim->HActive = 1280;
			fpTim->HDispEnd = 1279;
			fpTim->HValidStart = 0;
			fpTim->HValidEnd = 1279;
			fpTim->HSyncStart = 1280+15;
			fpTim->HSyncEnd = 1280+15+2;
			fpTim->HTotal = 1280+320;
			fpTim->VActive = 1024;
			fpTim->VDispEnd = 1023;
			fpTim->VValidStart = 0;
			fpTim->VValidEnd = 1023;
			fpTim->VSyncStart = 1023+2;
			fpTim->VSyncEnd = 1023+2+24;
			fpTim->VTotal = 1024+40;
			fpTim->PixClk = 10800;	// 108 MHz
			fpTim->Flags = VSYNC_POS | HSYNC_POS;

			break;
		case 1024:
			fpTim->HActive = 1024;
			fpTim->HDispEnd = 1023;
			fpTim->HValidStart = 0;
			fpTim->HValidEnd = 1023;
			fpTim->HSyncStart = 1040;
			fpTim->HSyncEnd = 1089;
			fpTim->HTotal = 1344;
			fpTim->VActive = 768;
			fpTim->VDispEnd = 767;
			fpTim->VValidStart = 0;
			fpTim->VValidEnd = 767;
			fpTim->VSyncStart = 770;
			fpTim->VSyncEnd = 776;
			fpTim->VTotal = 805;
			fpTim->PixClk = 6500;	// 65 MHz
			fpTim->Flags = VSYNC_NEG | HSYNC_NEG;
			break;
		case 800:
			fpTim->HActive =  800;
			fpTim->HDispEnd = 799;
			fpTim->HValidStart = 0;
			fpTim->HValidEnd = 799;
			fpTim->HSyncStart = 799+16;
			fpTim->HSyncEnd =  799+16+24;
			fpTim->HTotal = 800+320;
			fpTim->VActive = 600;
			fpTim->VDispEnd = 599;
			fpTim->VValidStart = 0;
			fpTim->VValidEnd =  599;
			fpTim->VSyncStart = 599+2;
			fpTim->VSyncEnd = 599+2+24;
			fpTim->VTotal = 600+38;
			fpTim->PixClk = 4000;	// 40 MHz
			fpTim->Flags = VSYNC_POS | HSYNC_POS;
			break;
		default:
			return RM_ERROR;
	}
	return RM_OK;
}
							
// EDID Version 1.1
// Return error if no valid resolution found
RM_STATUS ParseEDID11(PHWINFO pDev, U032 Head)
{
U016	pClk, HRes, VRes, HSyncOffset, VSyncOffset, HSyncPulse, VSyncPulse, HBlank, VBlank;
U016	HTotal, VTotal;
U008	est_tim1, est_tim2, block, offset, mode_count;
U008	HActive1, HActive2, HBlank1, HSyncOff1, HSyncPW;
U008	VActive1, VActive2, VBlank1, VSyncOff1, SyncOvf, Flags;
U032	est_tim_total, i;

	// Look for multiple resolutions in established timings to see if it supports native mode.
	// meaning the monitor can do the scaling.
 	est_tim1 = pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_ESTABLISHED_TIMING_1_INDEX];
 	est_tim2 = pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_ESTABLISHED_TIMING_2_INDEX];
	est_tim_total = est_tim1 | (est_tim2<<8);
	mode_count = 0;
	pDev->Dac.fpNative = 0;
	for (i=1; i<0x10000; i<<=1)
		if (est_tim_total & i)
			mode_count++;
	if (mode_count > 1)	// if more than 1 mode, monitor supports native mode.
		pDev->Dac.fpNative = 1;

	// We'll check detailed timings first. If none, look for established timings.
	// Search for largest resolution. 
	// NEED TO ADD: There could be extensions in next EDID block, too.
	pDev->Dac.fpHActive = 0;
	mode_count = 0; // check again--multiple detailed timing block also indicates native
	for (block = 0; block <4; block++)
	{
		// detailed timing block is 18 (0x12) bytes starting at offset 0x36.
		pClk = pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_DETAILED_TIMINGS_START_INDEX + 18 * block];
		pClk |= (pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_DETAILED_TIMINGS_START_INDEX + 18 * block + 1])<<8;
		if ((pClk != 0) && (pClk != 0x0101))	// 0 and 1 could be fillers or alternate info
		{
			// Seems valid, get info
			offset = EDID_V1_DETAILED_TIMINGS_START_INDEX + (18 * block);
			HActive1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 2];
			HActive2 = pDev->Dac.CrtcInfo[Head].EDID[offset + 4];
			HBlank1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 3];
			VActive1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 5];
			VBlank1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 6];
			VActive2 = pDev->Dac.CrtcInfo[Head].EDID[offset + 7];
			HSyncOff1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 8];
			HSyncPW = pDev->Dac.CrtcInfo[Head].EDID[offset + 9];
			VSyncOff1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 10];
			SyncOvf = pDev->Dac.CrtcInfo[Head].EDID[offset + 11];
			Flags = pDev->Dac.CrtcInfo[Head].EDID[offset + 17];

			// Get timing into useful format
			HRes = HActive1 | (HActive2 & 0xf0)<<4;
			VRes = VActive1 | (VActive2 & 0xf0)<<4;
			HBlank = HBlank1 | (HActive2 & 0x0f)<<8;
			VBlank = VBlank1 | (VActive2 & 0x0f)<<8;
			HTotal = HRes + HBlank;
			VTotal = VRes + VBlank;
			HSyncPulse = HSyncPW | (SyncOvf & 0x30)<<4;
			HSyncOffset = HSyncOff1 | (SyncOvf & 0xc0)<<2;
			VSyncOffset = (VSyncOff1>>4) | (SyncOvf & 0x0c)<<6;
			VSyncPulse = (VSyncOff1 & 0x0f) | (SyncOvf & 0x30)<<4;
#if 0
			// Do some error checking
			if ((HRes + HSyncOffset + HSyncPulse) > (HTotal))
			{
				HSyncPulse &= 0x0f;		// pulse is too wide
				if (HSyncPulse == 0)
					HSyncPulse = 24;		// should work
			}
			if ((VRes + VSyncOffset + VSyncPulse) > (VTotal))
			{
				HSyncPulse &= 0x0f;		// pulse is too wide
				if (VSyncPulse == 0)
					VSyncPulse = 24;		// should work
			}
#endif
			if (HRes > pDev->Dac.fpHActive)	// largest yet?
			{
				// Does this fp support native mode?
				mode_count++;	
				if (mode_count > 1)
					pDev->Dac.fpNative = 1;	
				// Save this resolution as potential maximum.
				pDev->Dac.fpPixClk = pClk;
				pDev->Dac.fpHActive = HRes;
		 		pDev->Dac.fpHDispEnd = HRes - 1;
				pDev->Dac.fpHValidEnd = HRes - 1;
		 		pDev->Dac.fpHSyncStart = HSyncOffset + pDev->Dac.fpHDispEnd;
		 		pDev->Dac.fpHSyncEnd = HSyncPulse + pDev->Dac.fpHSyncStart;
		 		pDev->Dac.fpHValidStart = 0;
		 		pDev->Dac.fpHTotal = HTotal;
					
		 		pDev->Dac.fpVActive = VRes;
		 		pDev->Dac.fpVDispEnd = VRes - 1;
		 		pDev->Dac.fpVValidEnd = VRes - 1;
		 		pDev->Dac.fpVSyncStart =  VSyncOffset + pDev->Dac.fpVDispEnd;
		 		pDev->Dac.fpVSyncEnd = VSyncPulse + pDev->Dac.fpVSyncStart;
		 		pDev->Dac.fpVValidStart = 0;
		 		pDev->Dac.fpVTotal = VTotal;
				pDev->Dac.fpFlags = Flags;

			}
		}
	}
	// save the max resolution this fp supports 
	pDev->Dac.fpHMax = pDev->Dac.fpHActive;
	pDev->Dac.fpVMax = pDev->Dac.fpVActive;

	// If we found a valid detailed timing descriptor, use it.
	if (pDev->Dac.fpHActive)
		return RM_OK;

	// Check the "established timings" and if more than one, pick the highest.
	// There are currently no flat panels supporting refresh other than 60 Hz, so 
	// treat timings with different refresh but the same resolution as the same.
	if(est_tim1 & 0x3c)
 	{
		pDev->Dac.fpHActive = 0;	// no 640 panels. Return error unless we find higher res.
	}
	if(est_tim1 & 0x03)
	{
		// 800x600 60
		pDev->Dac.fpHActive =  800;
		pDev->Dac.fpHDispEnd = 799;
		pDev->Dac.fpHValidStart = 0;
		pDev->Dac.fpHValidEnd = 799;
		pDev->Dac.fpHSyncStart = 799+16;
		pDev->Dac.fpHSyncEnd =  799+16+24;
		pDev->Dac.fpHTotal = 800+320;
		pDev->Dac.fpVActive = 600;
		pDev->Dac.fpVDispEnd = 599;
		pDev->Dac.fpVValidStart = 0;
		pDev->Dac.fpVValidEnd =  599;
		pDev->Dac.fpVSyncStart = 599+2;
		pDev->Dac.fpVSyncEnd = 599+2+24;
		pDev->Dac.fpVTotal = 600+38;
		pDev->Dac.fpPixClk = 4000;	// 40 MHz
		pDev->Dac.fpFlags = VSYNC_POS | HSYNC_POS;
	}

	// established timing 2 should take precedence over 1, since it has higher resolutions
	if(est_tim2 & 0xc0)
 	{
		// 800x600 60
		pDev->Dac.fpHActive =  800;
		pDev->Dac.fpHDispEnd = 799;
		pDev->Dac.fpHValidStart = 0;
		pDev->Dac.fpHValidEnd = 799;
		pDev->Dac.fpHSyncStart = 799+16;
		pDev->Dac.fpHSyncEnd =  799+16+24;
		pDev->Dac.fpHTotal = 800+320;
		pDev->Dac.fpVActive = 600;
		pDev->Dac.fpVDispEnd = 599;
		pDev->Dac.fpVValidStart = 0;
		pDev->Dac.fpVValidEnd =  599;
		pDev->Dac.fpVSyncStart = 599+2;
		pDev->Dac.fpVSyncEnd = 599+2+24;
		pDev->Dac.fpVTotal = 600+38;
		pDev->Dac.fpPixClk = 4000;	// 40 MHz
		pDev->Dac.fpFlags = VSYNC_POS | HSYNC_POS;
	}
	if(est_tim2 & 0x1e)	// lumping together 60Hz, 70 Hz, 75 Hz, and 87 Hz under the assumption
						// that DFP's which support > 60 Hz (if any) will have detailed timings.
	{
		// 1024x768x60;
		pDev->Dac.fpHActive = 1024;
		pDev->Dac.fpHDispEnd = 1023;
		pDev->Dac.fpHValidStart = 0;
		pDev->Dac.fpHValidEnd = 1023;
		pDev->Dac.fpHSyncStart = 1040;
		pDev->Dac.fpHSyncEnd = 1089;
		pDev->Dac.fpHTotal = 1344;
		pDev->Dac.fpVActive = 768;
		pDev->Dac.fpVDispEnd = 767;
		pDev->Dac.fpVValidStart = 0;
		pDev->Dac.fpVValidEnd = 767;
		pDev->Dac.fpVSyncStart = 770;
		pDev->Dac.fpVSyncEnd = 776;
		pDev->Dac.fpVTotal = 805;
		pDev->Dac.fpPixClk = 6500;	// 65 MHz
		pDev->Dac.fpFlags = VSYNC_NEG | HSYNC_NEG;
	}
	if(est_tim2 & 0x01)
	{
		// 1280x1024x75;
		pDev->Dac.fpHActive = 1280;
		pDev->Dac.fpHDispEnd = 1279;
		pDev->Dac.fpHValidStart = 0;
		pDev->Dac.fpHValidEnd = 1279;
		pDev->Dac.fpHSyncStart = 1280+15;
		pDev->Dac.fpHSyncEnd = 1280+15+2;
		pDev->Dac.fpHTotal = 1280+320;
		pDev->Dac.fpVActive = 1024;
		pDev->Dac.fpVDispEnd = 1023;
		pDev->Dac.fpVValidStart = 0;
		pDev->Dac.fpVValidEnd = 1023;
		pDev->Dac.fpVSyncStart = 1023+2;
		pDev->Dac.fpVSyncEnd = 1023+2+24;
		pDev->Dac.fpVTotal = 1024+40;
		pDev->Dac.fpPixClk = 10800;	// 108 MHz
		pDev->Dac.fpFlags = VSYNC_POS | HSYNC_POS;
	}

	if (pDev->Dac.fpHActive)
	{
		// save the max resolution this fp supports 
		pDev->Dac.fpHMax = pDev->Dac.fpHActive;
		pDev->Dac.fpVMax = pDev->Dac.fpVActive;
		return RM_OK;
	}
	else
		return RM_ERROR;
 }

// EDID Version 2.x
// The first 128 bytes is not very interesting to us (I think), except the last 2 bytes,
// which are the map into second 128 bytes, which contain the luminance table and 
// timing descriptions.
// In order to get to the detailed timings, we have to parse thru the lum table, frequency
// ranges, detailed range limits, and timing codes. These are optional fields, so nothing is
// found at a fixed offset. The map bytes tell whether the fields are present.
RM_STATUS ParseEDID2(PHWINFO pDev, U032 Head)
{
RM_STATUS status;
U008	map1, map2, index, lum, rangeLimits, detailedRangeLimits;
U008	detailedTimings, block, timingCodes;
U008	HActive1, HActive2, HBlank1, HSyncOff1, HSyncPW, offset;
U008	VActive1, VActive2, VBlank1, VSyncOff1, SyncOvf, Flags, i;
U016	pClk, HRes, VRes, HSyncOffset, VSyncOffset, HSyncPulse, VSyncPulse, HBlank, VBlank;
U016	HTotal, VTotal;
U008    mode_count = 0;
U008	timingCode[4*31];
DACFPTIMING fpTim;

	map1 = pDev->Dac.CrtcInfo[Head].EDID[EDID_V2_TIMING_MAP1_INDEX];
	map2 = pDev->Dac.CrtcInfo[Head].EDID[EDID_V2_TIMING_MAP1_INDEX+1];
	index =  EDID_V2_TIMING_SECTION_START_INDEX;
	// if there is a lum table, bump index beyond
	if (map1 & EDID_V2_LUM_TABLE_EXIST)
	{
		// 1st byte in timing section gives # of bytes in lum table
		lum = pDev->Dac.CrtcInfo[Head].EDID[EDID_V2_TIMING_SECTION_START_INDEX];
		if (lum & TRIPLE_LUM_ENTRIES)
			index += ((lum & NUM_LUM_ENTRIES)*3) + 1;
		else
			index += (lum & NUM_LUM_ENTRIES) + 1;
	}
	// if there are timing range limits, bump index
	if ((rangeLimits = (map1 & EDID_V2_FREQ_RANGES)>>EDID_V2_FREQ_RANGE_SHIFT))
		index += rangeLimits * 8;
	// if there are detailed range limits, bump index
	if ((detailedRangeLimits = map1 & EDID_V2_DETAILED_RANGE_LIMITS))
		index += detailedRangeLimits * 27;
	// if there are timing codes, save them
	if ((timingCodes = (map2 & EDID_V2_TIMING_CODES)>>EDID_V2_TIMING_CODES_SHIFT))
		for (i=0; i!=timingCodes; i++)
		{
			timingCode[i] = pDev->Dac.CrtcInfo[Head].EDID[index];
			timingCode[i+1] = pDev->Dac.CrtcInfo[Head].EDID[index+1];
			timingCode[i+2] = pDev->Dac.CrtcInfo[Head].EDID[index+2];
			timingCode[i+3] = pDev->Dac.CrtcInfo[Head].EDID[index+3];
			index += 4;
		}
	// now index points to the detailed timings (if present)
	if ((detailedTimings = (map2 & EDID_V2_DETAILED_TIMINGS)))
	{
		for (block = 0; block < detailedTimings; block++)
		{
			// A detailed timing block is 27 bytes
			// There can be up to 7 detailed timing blocks, immediately following the timing codes
			pClk = pDev->Dac.CrtcInfo[Head].EDID[index + 18 * block];
			pClk |= (pDev->Dac.CrtcInfo[Head].EDID[index + 18 * block + 1])<<8;
			if ((pClk != 0) && (pClk != 0x0101))	// 0 and 1 could be fillers or alternate info
			{
				// Does this fp support native mode?
				// Another way to check would be to see if detailedTimings > 1,
				// but what if we find multiple detailed timings for the same resolution?
				mode_count++;	
				if (mode_count > 1)
					pDev->Dac.fpNative = 1;	

				// Seems valid, get info
				offset = index + 18 * block;
				HActive1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 2];
				HActive2 = pDev->Dac.CrtcInfo[Head].EDID[offset + 4];
				HBlank1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 3];
				VActive1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 5];
				VBlank1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 6];
				VActive2 = pDev->Dac.CrtcInfo[Head].EDID[offset + 7];
				HSyncOff1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 8];
				HSyncPW = pDev->Dac.CrtcInfo[Head].EDID[offset + 9];
				VSyncOff1 = pDev->Dac.CrtcInfo[Head].EDID[offset + 10];
				SyncOvf = pDev->Dac.CrtcInfo[Head].EDID[offset + 11];
				Flags = pDev->Dac.CrtcInfo[Head].EDID[offset + 17];

				// Get timing into useful format
				HRes = HActive1 | (HActive2 & 0xf0)<<4;
				VRes = VActive1 | (VActive2 & 0xf0)<<4;
				HBlank = HBlank1 | (HActive2 & 0x0f)<<8;
				VBlank = VBlank1 | (VActive2 & 0x0f)<<8;
				HTotal = HRes + HBlank;
				VTotal = VRes + VBlank;
				HSyncPulse = HSyncPW | (SyncOvf & 0x30)<<4;
				HSyncOffset = HSyncOff1 | (SyncOvf & 0xc0)<<2;
				VSyncOffset = (VSyncOff1>>4) | (SyncOvf & 0x0c)<<2;
				VSyncPulse = (VSyncOff1 & 0x0f) | (SyncOvf & 0x30)<<4;

				if (HRes > pDev->Dac.fpHActive)	// largest yet?
				{
					// Return this resolution
					pDev->Dac.fpPixClk = pClk;
					pDev->Dac.fpHActive = HRes;
	 				pDev->Dac.fpHDispEnd = HRes - 1;
					pDev->Dac.fpHValidEnd = HRes - 1;
			 		pDev->Dac.fpHSyncStart = HSyncOffset + pDev->Dac.fpHDispEnd;
			 		pDev->Dac.fpHSyncEnd = HSyncPulse + pDev->Dac.fpHSyncStart;
	 				pDev->Dac.fpHValidStart = 0;
	 				pDev->Dac.fpHTotal = HTotal;
				
		 			pDev->Dac.fpVActive = VRes;
			 		pDev->Dac.fpVDispEnd = VRes - 1;
		 			pDev->Dac.fpVValidEnd = VRes - 1;
	 				pDev->Dac.fpVSyncStart =  VSyncOffset + pDev->Dac.fpVDispEnd;
		 			pDev->Dac.fpVSyncEnd = VSyncPulse + pDev->Dac.fpVSyncStart;
		 			pDev->Dac.fpVValidStart = 0;
		 			pDev->Dac.fpVTotal = VTotal;
	
					// save the max resolution this fp supports 
					pDev->Dac.fpHMax = pDev->Dac.fpHActive;
					pDev->Dac.fpVMax = pDev->Dac.fpVActive;
					pDev->Dac.fpFlags = Flags;
				}
			} // if 
		} // for
		return RM_OK;
	} // if detailed timings

	// If no detailed timings, check 4 byte timing codes
	// byte 1: active pixels/line (pixels - 256)/16. Range = 256-4336.
	// byte 2: arcane info, don't know what it means. See EDID standard.
	// byte 3: aspect ration H:V.
	// byte 4: refresh rate.
	pDev->Dac.fpHActive = 0;
	mode_count = 0;
	for (i=0; i < timingCodes; i++)	// search all codes for max res
	{
		status =  GetTimingFromCode(timingCode[i*4], &fpTim);		
		if (status == RM_OK)
			if (fpTim.HTotal > pDev->Dac.fpHActive) // new max?
			{
				mode_count++;
				pDev->Dac.fpPixClk = fpTim.PixClk;
				pDev->Dac.fpHActive = fpTim.HActive;
			 	pDev->Dac.fpHDispEnd = fpTim.HDispEnd;
				pDev->Dac.fpHValidEnd = fpTim.HValidEnd;
	 			pDev->Dac.fpHSyncStart = fpTim.HSyncStart;
			 	pDev->Dac.fpHSyncEnd = fpTim.HSyncEnd;
			 	pDev->Dac.fpHValidStart = fpTim.HValidStart;
			 	pDev->Dac.fpHTotal = fpTim.HTotal;
					
	 			pDev->Dac.fpVActive = fpTim.VActive;
			 	pDev->Dac.fpVDispEnd = fpTim.VDispEnd;
			 	pDev->Dac.fpVValidEnd = fpTim.VValidEnd;
			 	pDev->Dac.fpVSyncStart = fpTim.VSyncStart;
		 		pDev->Dac.fpVSyncEnd = fpTim.VSyncEnd;
			 	pDev->Dac.fpVValidStart = fpTim.VValidStart;
	 			pDev->Dac.fpVTotal = fpTim.VTotal;
				pDev->Dac.fpFlags = fpTim.Flags;
				pDev->Dac.fpHMax = pDev->Dac.fpHActive;
				pDev->Dac.fpVMax = pDev->Dac.fpVActive;
			}
	}
	if (pDev->Dac.fpHActive)	// if we found a mode
	{
		if (mode_count > 1)
			pDev->Dac.fpNative = 1;	
		return RM_OK;
	}
	return RM_ERROR;
}

// Parse the EDID buffer (read earlier) to determine if we have valid timings.
// 
RM_STATUS fpParseEDID(PHWINFO pDev, U032 Head)
{
U032 status = RM_ERROR;	
    // The EDID may contain "established timings", a bit map of flags showing supported 
	// resolutions, or "standard timings", a 2-byte description of horizontal res + aspect ratio,
	// or "detailed timings", an 18-byte description.
	// Version 1.x EDID starts with an 8-byte header: 00 FF FF FF FF FF FF 00.
    if (pDev->Dac.CrtcInfo[Head].EDID[0] == 0)
        if (pDev->Dac.CrtcInfo[Head].EDID[1] == 0xFF)
            if (pDev->Dac.CrtcInfo[Head].EDID[7] == 0)
            {
                // Version 1 header
                if (pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_VERSION_NUMBER_INDEX] == 1)
                {
                    switch (pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_REVISION_NUMBER_INDEX]) 
                    {
                        case 0:
                            pDev->Dac.CrtcInfo[Head].EdidVersion = EDID_VERSION_10;
                            status = ParseEDID11(pDev, Head);	// Version 1.0 just like 1.1
                            break;
                        case 1:
                            pDev->Dac.CrtcInfo[Head].EdidVersion = EDID_VERSION_11;
                            status = ParseEDID11(pDev, Head);	// Version 1.1
                            break;
                        case 2:
                            pDev->Dac.CrtcInfo[Head].EdidVersion = EDID_VERSION_12;
                            status = ParseEDID12(pDev, Head);
                            break;
                        case 3:
                            pDev->Dac.CrtcInfo[Head].EdidVersion = EDID_VERSION_13;
                            status = ParseEDID13(pDev, Head);
                            break;
                        default:
                            pDev->Dac.CrtcInfo[Head].EdidVersion = EDID_VERSION_14; // This doesn't currently exist, but just in case
                            status = ParseEDID13(pDev, Head);
                            break;
                    }

                    // Is this is a digital device or analog?
                    if (!(pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_VIDEO_INPUT_DEF_INDEX] & BIT(7)))
                        pDev->Dac.CrtcInfo[Head].EdidDisplayType = DISPLAY_TYPE_MONITOR;    
                    else    
                        pDev->Dac.CrtcInfo[Head].EdidDisplayType = DISPLAY_TYPE_FLAT_PANEL;    
                    return status;	

                }
                else // Version 1 header, but not version 1 ID?
                {
                    return RM_ERROR;
                }
            }
	// Does not have version 1 header, check version 2
    if ((pDev->Dac.CrtcInfo[Head].EDID[EDID_V2_DISPLAY_TECHNOLOGY_INDEX] & DISPLAY_TECHNOLOGY_TYPE_MASK) == 0)
    {
        pDev->Dac.CrtcInfo[Head].EdidDisplayType = DISPLAY_TYPE_MONITOR;    
        return RM_ERROR; // its a CRT	
    }
    else    
    {
        pDev->Dac.CrtcInfo[Head].EdidDisplayType = DISPLAY_TYPE_FLAT_PANEL;    
    }

    pDev->Dac.CrtcInfo[Head].EdidVersion = EDID_VERSION_20;
    return	ParseEDID2(pDev, Head);
}

BOOL edidIsAnalogDisplay(PHWINFO pDev, U032 Head) 
{
U032 version;

 	version = edidGetVersion(pDev, Head);
	if (version == 1)
	{
		// Verify this is an analog device
		if (pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_VIDEO_INPUT_DEF_INDEX] & BIT(7))
			return FALSE; // 1=digital
		else
			return TRUE;
	}
	if (version == 2)
		if ((pDev->Dac.CrtcInfo[Head].EDID[EDID_V2_DISPLAY_TECHNOLOGY_INDEX] & DISPLAY_TECHNOLOGY_TYPE_MASK) == 0)
			return TRUE;
	return FALSE;
}

BOOL edidIsDigitalDisplay(PHWINFO pDev, U032 Head) 
{
U032 version;

 	version = edidGetVersion(pDev, Head);
	if (version == 1)
	{
		// Verify this is an analog device
		if (pDev->Dac.CrtcInfo[Head].EDID[EDID_V1_VIDEO_INPUT_DEF_INDEX] & BIT(7))
			return TRUE;
		else
            return FALSE; // 1=digital
	}
	if (version == 2)
		if ((pDev->Dac.CrtcInfo[Head].EDID[EDID_V2_DISPLAY_TECHNOLOGY_INDEX] & DISPLAY_TECHNOLOGY_TYPE_MASK) != 0)
			return TRUE;
	return FALSE;
}

//
// Generate an EDID appropriate for the current mobile panel in use
//
RM_STATUS edidConstructMobileInfo
(
    PHWINFO pDev,
    U032    Head,
    U008    *pEdid
)
{

    //
    // We need to generate an EDID given the current BIOS default panel
    // info, but until the BIOS is ready for this, just return the
    // default EDID for the mobile panels
    //
    // 
    // We need to construct the EDID from the BIOS data, but for now just drop in a std panel EDID
    //
    U032 i, panel_type;
    U008 dfp_edid_SXGA[] = {
         0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x3A, 0xC4,
         0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1C, 0x09, 0x01, 0x02,
         0x80, 0x1E, 0x17, 0xBC, 0xEA, 0xA8, 0xE0, 0x99, 0x57, 0x4B,
         0x92, 0x25, 0x1C, 0x50, 0x54, 0x00, 0x00, 0x00, 0x01, 0x41,
         0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
         0x01, 0x01, 0x01, 0x01, 0x30, 0x2A, 0x78, 0x20, 0x51, 0x1A,
         0x10, 0x40, 0x30, 0x70, 0x13, 0x00, 0x2C, 0xE4, 0x10, 0x00,
         0x00, 0x1E, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x3c, 0x3c, 0x1E,
         // original 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x28, 0x4B, 0x1E,
         0x64, 0x0C, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
         0x00, 0x00, 0x00, 0xFF, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30,
         0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x0A, 0x00, 0x00,
         0x00, 0xFC, 0x00, 0x4E, 0x56, 0x49, 0x44, 0x49, 0x41, 0x5F,
         0x53, 0x58, 0x47, 0x41, 0x0A, 0x20, 0x00, 0xCF
         // original 0x53, 0x58, 0x47, 0x41, 0x0A, 0x20, 0x00, 0xD4
    };

    U008 dfp_edid_UXGA[] = {
         0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x3A, 0xC4,
         0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1C, 0x09, 0x01, 0x02,
         0x80, 0x1E, 0x17, 0xBC, 0xEA, 0xA8, 0xE0, 0x99, 0x57, 0x4B,
         0x92, 0x25, 0x1C, 0x50, 0x54, 0x00, 0x00, 0x00, 0x01, 0x41,
         0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
         0x01, 0x01, 0x01, 0x01, 0x48, 0x3F, 0x40, 0x30, 0x62, 0xB0,
         0x32, 0x40, 0x40, 0xC0, 0x13, 0x00, 0x2C, 0xE4, 0x10, 0x00,
         0x00, 0x1E, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x3c, 0x3c, 0x1E,
         // original 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x28, 0x4B, 0x1E,
         0x64, 0x11, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
         // original 0x64, 0x0C, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
         0x00, 0x00, 0x00, 0xFF, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30,
         0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x0A, 0x00, 0x00,
         0x00, 0xFC, 0x00, 0x4E, 0x56, 0x49, 0x44, 0x49, 0x41, 0x5F,
         0x55, 0x58, 0x47, 0x41, 0x0A, 0x20, 0x00, 0x9A
         // original 0x55, 0x58, 0x47, 0x41, 0x0A, 0x20, 0x00, 0xA4
    };

    U008 dfp_edid_Toshiba_10x7[] = {
         0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x3A, 0xC4, 
         0x02, 0x00, 0x02, 0x00, 0x00, 0x00, 0x1C, 0x09, 0x01, 0x02, 
         0x80, 0x1E, 0x17, 0xBC, 0xEA, 0xA8, 0xE0, 0x99, 0x57, 0x4B, 
         0x92, 0x25, 0x1C, 0x50, 0x54, 0x00, 0x00, 0x00, 0x01, 0x41, 
         0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
         0x01, 0x01, 0x01, 0x01, 0x64, 0x19, 0x00, 0x40, 0x41, 0x00, 
         0x26, 0x30, 0x18, 0x88, 0x36, 0x00, 0x2C, 0xE4, 0x10, 0x00, 
         0x00, 0x18, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x3C, 0x3C, 0x1E,
         // original 0x00, 0x18, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x28, 0x4B, 0x1E,
         0x46, 0x08, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
         0x00, 0x00, 0x00, 0xFF, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 
         0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x0A, 0x00, 0x00, 
         0x00, 0xFC, 0x00, 0x4E, 0x56, 0x49, 0x44, 0x49, 0x41, 0x5F, 
         0x58, 0x47, 0x41, 0x0A, 0x20, 0x20, 0x00, 0x60
         // original 0x58, 0x47, 0x41, 0x0A, 0x20, 0x20, 0x00, 0x65
    };


    // Toshiba?
    if (pDev->Power.MobileOperation == 2)
    {
        for (i=0;i<128;i++)
            //pDev->Dac.CrtcInfo[Head].EDID[i] = dfp_edid_Toshiba_10x7[i];
            pEdid[i] = dfp_edid_Toshiba_10x7[i];
    } 
    else
    {

        //
        // Dell Strapping assignments
        //
        //- SXGA+ panels            Panel ID     Timing        
        //    Hitachi  15"           0xE          SXGA+ (pclk = 108Mhz)      
        //    Sharp 15"              0x8          SXGA+       
        //    Unknown supplier       0x2          SXGA+       
        //    Unknown supplier       0x7          SXGA+      
        //  
        //- UXGA panels                
        //    Unknown supplier       0x3          UXGA  ( pclk =162Mhz )
        //    Unknown supplier       0x4          UXGA  ( pclk =162Mhz )
        //    Unknown supplier       0x5          UXGA  ( pclk =162Mhz )
        //    Unknown supplier       0xA          UXGA  ( pclk =162Mhz )
        //    Unknown supplier       0xB          UXGA  ( pclk =162Mhz )
    
        //
        // Strapped for UXGA???
        //
        panel_type = REG_RD_DRF(_PEXTDEV, _BOOT_0, _STRAP_USER_MOBILE) & 0x0F; 
        if ((panel_type == 0x03) || (panel_type == 0x04) || (panel_type == 0x05) 
            || (panel_type == 0x0A) || (panel_type == 0x0B))
        {
            for (i=0;i<128;i++)
                //pDev->Dac.CrtcInfo[Head].EDID[i] = dfp_edid_UXGA[i];
                pEdid[i] = dfp_edid_UXGA[i];
        }
        //
        // Strapped for SXGA+???
        //
        else    // assume SXGA+ if nothing else
        {
            for (i=0;i<128;i++)
                //pDev->Dac.CrtcInfo[Head].EDID[i] = dfp_edid_SXGA[i];
                pEdid[i] = dfp_edid_SXGA[i];
        }
    }

    return RM_OK;

}

//  Get the VESA DDC Capabilites status word
//  The lower 5 bits is a bit map of addresses at which a display will respond (A0, A2, A6);
//  bit 7 is set if there is more than one controller;
//  the upper byte is the response latency in seconds.
U032 edidGetDDCCaps
(
    PHWINFO pDev
)
{
    U008 lock, Adr = 0, Data = 0;
    U032 Caps = 0x100;  // set 1 second response (minimum)
    U032 Head, i2cPortId;

    if (IsNV5orBetter(pDev)) 
    {
        Caps |= 0x80;   // multiple DDC ports
    }
    // Currently we can access any device from Head 0, but in the future, who knows?
//    for (Head = 0; Head < pDev->Dac.HalInfo.NumCrtcs; Head++)   // for all heads
    for (Head = 0; Head < 1; Head++) 
    {
        lock = ReadCRTCLock(pDev, Head);	// Unlock the registers
        if (IsNV11(pDev)) 
        {
           AssocDDC(pDev, Head);  // Connect the head to the I2C pins
        }
        // Try all I2c ports
        // NOTE: we have to know how many I2C ports there are--there is no define.
        for (i2cPortId = 0; i2cPortId < 3; i2cPortId++) 
        {
            // Send a byte (any data) to subaddress 0 and see if the device acks it.
            // Try all three monitor addresses
            if (!(Caps & 0x02)) // if we already set this bit, no need to do it again, as there is a noticeable delay
                if (!i2cWrite(pDev, Head, i2cPortId, DDC_CRT_ADR1, 1, &Adr, 0, &Data))
                    Caps |= 0x02;
            if (!(Caps & 0x08))
                if (!i2cWrite(pDev, Head, i2cPortId, DDC_CRT_ADR2, 1, &Adr, 0, &Data))
                    Caps |= 0x08;
            if (!(Caps & 0x10))
                if (!i2cWrite(pDev, Head, i2cPortId, DDC_CRT_ADR3, 1, &Adr, 0, &Data))
                    Caps |= 0x10;
        }
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, lock, Head);
    }
    return Caps;
}
