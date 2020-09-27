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

/******************************* I2C Control *******************************\
*                                                                           *
* Module: I2C.C                                                             *
*   The I2C port is managed within this module.                             *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#include <nv_ref.h>
#include <nvrm.h>
#include <state.h>
#include <class.h>
#include <dma.h>
#include <gr.h>
#include <dac.h>
#include <os.h>
#include "nvhw.h"
#include <vga.h>
#include "i2c.h"

#include <nvmisc.h>
#define xxx(d, fmt) nv_debug(99, fmt)

//---------------------------------------------------------------------------
//
//  I2C support routines.
//
//---------------------------------------------------------------------------

// Serial Port Bits
#define I2C_SRCK        0x20    // Serial Clock write
#define I2C_SRD         0x10    // Serial Data  write
#define I2C_SRCK_IN     0x04    // Serial Clock read
#define I2C_SRD_IN      0x08    // Serial Data  read
#define I2C_ENABLE      0x01    // Enable Serial Port Function

///////////////////////////////////////////////////////////////////

// Constants used by I2C Protocol:

#define SDA_REG         0x02
#define SCL_REG         0x01


#define ACK             0   // I2C Acknowledge
#define NACK            1   // I2C No Acknowledge

#define I2CDELAY 10000   // 10usec
	
// Module Global Variables

// variables could be modified via INI file
// U008 iniFlag = FALSE;
#ifdef  IKOS
U032 wMaxWaitStates=2;     // max wait states for I2C bus syncronisation
#else   // IKOS
U032 wMaxWaitStates=3000;  // max wait states for I2C bus syncronisation
#endif  // IKOS
U032 wGpo=0;               // GPO bits for switching to I2C operation

//
// Insure register updates have been flushed out of the write buffer
// by performing a cpuid instruction
//
// XXX this should be in common/nvUtil/ or somewhere else
#ifdef WIN31

#ifdef NEWWIN31

void __cdecl FlushWB(void)
{
    __asm push esi
    __asm push edi
    __asm push ebx
    __asm mov eax, 0x0
        __asm _emit 0x0f
        __asm _emit 0xa2
    __asm pop ebx
    __asm pop edi
    __asm pop esi
}

#else  //old win31

void FlushWB(void);
#pragma aux FlushWB =				\
            "mov    EAX, 0x0"			\
            0x0F 0xA2
#endif

#elif defined(__PPC__)

void FlushWB(void)
{
    __eieio();
}


#elif defined(__GNUC__)		// UNIX and DJPP
void FlushWB(void)
{
#ifndef IA64
    // save regs (especially ebx) that might get stomped by 'cpuid'
    __asm__("push %esi");
    __asm__("push %edi");
    __asm__("push %ebx");
        __asm__("xorl %eax, %eax");
        __asm__("cpuid");
    __asm__("pop %ebx");
    __asm__("pop %edi");
    __asm__("pop %esi");
#endif
}

#else   // !WIN31 !UNIX !DJPP

void __cdecl FlushWB(void)
{
// TO DO: what to do about inline asm for IA-64?       
#ifndef IA64
    __asm push esi
    __asm push edi
    __asm push ebx
    __asm mov eax, 0x0
        __asm _emit 0x0f
        __asm _emit 0xa2
    __asm pop ebx
    __asm pop edi
    __asm pop esi
#endif // IA-64 cannot tolerate inline asm
}

#endif

void i2cWriteCRTCWrIndex
(
    PHWINFO pDev,
    U032 Head,
    U032 portID,
    U008 data
)
{
    switch (portID) 
    {
    case NV_I2C_PORT_PRIMARY:
        CRTC_WR(pDev->Dac.I2CPrimaryWriteIndex, data, Head);
        break;
    case NV_I2C_PORT_SECONDARY:
        CRTC_WR(pDev->Dac.I2CSecondaryWriteIndex, data, Head);
        break;
    case NV_I2C_PORT_TERTIARY:
        CRTC_WR(pDev->Dac.I2CTertiaryWriteIndex, data, Head);
        break;
    }
}

   
U008 i2cReadCRTCWrIndex
(
    PHWINFO pDev,
    U032 Head,
    U032 portID
)
{
    U008 data = 0xFF;

    switch (portID) 
    {
    case NV_I2C_PORT_PRIMARY:
        CRTC_RD(pDev->Dac.I2CPrimaryWriteIndex, data, Head);
        break;
    case NV_I2C_PORT_SECONDARY:
        CRTC_RD(pDev->Dac.I2CSecondaryWriteIndex, data, Head);
        break;
    case NV_I2C_PORT_TERTIARY:
        CRTC_RD(pDev->Dac.I2CTertiaryWriteIndex, data, Head);
        break;
    }
    return data;
}
   

U008 i2cReadCRTCStatusIndex
(
    PHWINFO pDev,
    U032 Head,
    U032 portID
)
{
    U008 data = 0xFF;

    switch (portID) 
    {
    case NV_I2C_PORT_PRIMARY:
        CRTC_RD(pDev->Dac.I2CPrimaryStatusIndex, data, Head);
        break;
    case NV_I2C_PORT_SECONDARY:
        CRTC_RD(pDev->Dac.I2CSecondaryStatusIndex, data, Head);
        break;
    case NV_I2C_PORT_TERTIARY:
        CRTC_RD(pDev->Dac.I2CTertiaryStatusIndex, data, Head);
        break;
    }
    return data;
}

U008 i2cHardwareInit
(
    PHWINFO pDev,
    U032 Head,
    U032 portID
)
{
    // On dual-headed devices, enable I2C interface
    if (IsNV11(pDev)) 
    {
        AssocDDC(pDev, Head);
    }
    
    //
    // If the initialization has already been done then just return
    //
    // if (iniFlag == TRUE)
    //    return TRUE;

    i2cWriteCRTCWrIndex(pDev, Head, portID, 0x31);
	
	pDev->Dac.I2cIniFlag = TRUE;
	return TRUE;
}


VOID i2cWriteCtrl
(
    PHWINFO pDev,
    U032 Head,
    U032 portID,
    U008 reg,
    U008 bit
)
{
    U008 data;

    //
    // Get the current status and toggle
    //
	data = i2cReadCRTCWrIndex(pDev, Head, portID);

	data &= 0xf0;
    data |= I2C_ENABLE;
    
    if (reg == SCL_REG)
    {
      if (bit)
        data |=  I2C_SRCK;
      else
        data &= ~I2C_SRCK;
    }
    else
    {
      if (bit)
        data |=  I2C_SRD;
      else
        data &= ~I2C_SRD;
    }    

	i2cWriteCRTCWrIndex(pDev, Head, portID, data);
    FlushWB();
}

U008 i2cReadCtrl
(
    PHWINFO pDev,
    U032 Head,
    U032 portID,
    U008 reg
)
{
    U008 data;
   
    data = i2cReadCRTCStatusIndex(pDev, Head, portID);    

    if (reg == SCL_REG)
        return ( (data & I2C_SRCK_IN) != 0);
    else
        return ( (data & I2C_SRD_IN) != 0);
}
   
/**********************************************************************/

VOID ReadSDA(PHWINFO pDev, U032 Head, U032 portID, U008 *data)
{
	*data = i2cReadCtrl(pDev, Head, portID, SDA_REG);
}

VOID ReadSCL(PHWINFO pDev, U032 Head, U032 portID, U008 *data)
{
	*data = i2cReadCtrl(pDev, Head, portID, SCL_REG);
}   

VOID SetSCLLine(PHWINFO pDev, U032 Head, U032 portID)
{
	i2cWriteCtrl(pDev, Head, portID, SCL_REG, 1);
}

VOID ResetSCLLine(PHWINFO pDev, U032 Head, U032 portID)
{
	i2cWriteCtrl(pDev, Head, portID, SCL_REG, 0);
}

VOID SetSDALine(PHWINFO pDev, U032 Head, U032 portID)
{
	i2cWriteCtrl(pDev, Head, portID, SDA_REG, 1);
}

VOID ResetSDALine(PHWINFO pDev, U032 Head, U032 portID)
{
	i2cWriteCtrl(pDev, Head, portID, SDA_REG, 0);
} 
 
/*
 * waits for a specified line til it goes high
 * giving up after MAX_WAIT_STATES attempts
 * return:  0 OK
 *         -1 fail (time out)
 */
U008 WaitHighSDALine(PHWINFO pDev, U032 Head, U032 portID)
{
    U008    data_in;
    U032    retries = wMaxWaitStates;

    do
    {
        ReadSDA(pDev, Head, portID, &data_in);      // wait for the line going high
        if (data_in)
            break;
        tmrDelay(pDev, I2CDELAY); 
    } while (--retries);        // count down is running

    if (!retries)
        return((U008)-1);
    return 0;
}                    

U008 WaitHighSCLLine(PHWINFO pDev, U032 Head, U032 portID)
{
    U008    data_in;
    U032    retries = wMaxWaitStates;

    do
    {
        tmrDelay(pDev, 1000);              // 1.0 us delay   NEEDED?? 
        ReadSCL(pDev, Head, portID, (U008 *)&data_in);   // wait for the line going high
        if (data_in)
            break;
    } while (--retries);            // count down is running

    if (!retries)
        return((U008)-1);
    return(0);    
}


VOID i2cStart(PHWINFO pDev, U032 Head, U032 portID)
{
    SetSDALine(pDev, Head, portID); 
    tmrDelay(pDev, I2CDELAY); 
    SetSCLLine(pDev, Head, portID); 
    tmrDelay(pDev, I2CDELAY);     // spec requires clock to be high min of 4us
    WaitHighSCLLine(pDev, Head, portID);
    ResetSDALine(pDev, Head, portID);
    tmrDelay(pDev, I2CDELAY);
    ResetSCLLine(pDev, Head, portID);
}


VOID i2cStop(PHWINFO pDev, U032 Head, U032 portID)
{
    tmrDelay(pDev, I2CDELAY * 20);
    ResetSCLLine(pDev, Head, portID);
    ResetSDALine(pDev, Head, portID);
    tmrDelay(pDev, I2CDELAY); 
    SetSCLLine(pDev, Head, portID); 
    tmrDelay(pDev, I2CDELAY);     // spec requires clock to be high min of 4us
    WaitHighSCLLine(pDev, Head, portID); 
    SetSDALine(pDev, Head, portID); 
    tmrDelay(pDev, I2CDELAY);
}


/*
 * I2cAck() returns 1: fail
 *                  0: acknolege
 */

U008 i2cAck(PHWINFO pDev, U032 Head, U032 portID)
{
    U008 ack;

    ResetSCLLine(pDev, Head, portID); 
    tmrDelay(pDev, I2CDELAY);
    SetSDALine(pDev, Head, portID); 
    tmrDelay(pDev, I2CDELAY);
    SetSCLLine(pDev, Head, portID); 
    tmrDelay(pDev, I2CDELAY);     // spec requires clock to be high min of 4us
    WaitHighSCLLine(pDev, Head, portID);
    ReadSDA(pDev, Head, portID, &ack);
    ResetSCLLine(pDev, Head, portID);
    return (ack);
}


VOID i2cInit(PHWINFO pDev, U032 Head, U032 portID)
{
    SetSCLLine(pDev, Head, portID); 
    tmrDelay(pDev, I2CDELAY);     // spec requires clock to be high min of 4us
    WaitHighSCLLine(pDev, Head, portID);
    SetSDALine(pDev, Head, portID); 
}


U008 i2cSendByte(PHWINFO pDev, U032 Head, U032 portID, U008 byte)
{
    U008 i;

    for (i=0;i<8;i++)
    {
	    ResetSCLLine(pDev, Head, portID);
	    tmrDelay(pDev, I2CDELAY/2);
        if (byte & 0x80)
		    SetSDALine(pDev, Head, portID);
	    else
		    ResetSDALine(pDev, Head, portID);
	    tmrDelay(pDev, I2CDELAY/2);
	    SetSCLLine(pDev, Head, portID);
	    tmrDelay(pDev, I2CDELAY);    // clock must be high at least 4us
	    WaitHighSCLLine(pDev, Head, portID);
	    byte <<= 1;
    }
    
    return i2cAck(pDev, Head, portID);
}


RM_STATUS i2cReceiveByte(PHWINFO pDev, U032 Head, U032 portID, U008 *byte, U008 nack)
{
    U008 data=0;
    U008 i;
    RM_STATUS status;

    ResetSCLLine(pDev, Head, portID);
    SetSDALine(pDev, Head, portID); 
    tmrDelay(pDev, 1000);

    for (i=0;i<8;i++)
	{
        ResetSCLLine(pDev, Head, portID);
        ResetSCLLine(pDev, Head, portID);  // 2nd needed?
        tmrDelay(pDev, I2CDELAY);
	    SetSCLLine(pDev, Head, portID); 
        status = WaitHighSCLLine(pDev, Head, portID) ? RM_ERROR : RM_OK;
        if (status != RM_OK)
            goto done;
	    tmrDelay(pDev, I2CDELAY);    // clock must be high at least 4us

	    ReadSDA(pDev, Head, portID, &data);
	    *byte <<= 1;
	    *byte  |= (data == 1);
	}

    ResetSCLLine(pDev, Head, portID);
    if (nack) 
    {
        SetSDALine(pDev, Head, portID);         // send Nack
    }
    else
        ResetSDALine(pDev, Head, portID);       // send Ack

    tmrDelay(pDev, I2CDELAY);
    SetSCLLine(pDev, Head, portID); 
    status = WaitHighSCLLine(pDev, Head, portID) ? RM_ERROR : RM_OK;
	tmrDelay(pDev, I2CDELAY);    // clock must be high at least 4us
    ResetSCLLine(pDev, Head, portID);
    tmrDelay(pDev, I2CDELAY);   // clock width must be 4.7us

done:
    return status;
}


RM_STATUS i2cWrite(PHWINFO pDev, U032 Head, U032 portID, U008 ChipAdr, U016 AdrLen, U008 *Adr, U016 DataLen, U008 *Data)
{
    //
    // Enable writes to the I2C port
    //
    i2cHardwareInit(pDev, Head, portID);

    i2cStart(pDev, Head, portID);
    if ( i2cSendByte(pDev, Head, portID, (U008)(ChipAdr<<1)) ) // send chip adr. with write bit
    {
        i2cStop(pDev, Head, portID);                         // ack failed --> generate stop condition
        return(RM_ERROR); 
    }
    for ( ; AdrLen; AdrLen--)             
    {
        if ( i2cSendByte(pDev, Head, portID, *Adr++) )        // send sub-register byte(s)
        {
            i2cStop(pDev, Head, portID);                    // ack failed --> generate stop condition
            return(RM_ERROR); 
        }
    }
    for ( ; DataLen; DataLen--)            // send data byte(s)
    {
        if ( i2cSendByte(pDev, Head, portID, *Data++) )
        {
            i2cStop(pDev, Head, portID);                     // ack failed --> generate stop condition
            return(RM_ERROR);
        }
    }
    i2cStop(pDev, Head, portID);
    return(RM_OK);
}


RM_STATUS i2cRead(PHWINFO pDev, U032 Head, U032 portID, U008 ChipAdr, U016 AdrLen, U008 *Adr, U016 DataLen, U008 *Data)
{
    U008 dat;
    RM_STATUS status = RM_ERROR;        // pessimist
    
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: ChipAdr ", (U032)ChipAdr);
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: AdrLen ", (U032)AdrLen);
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Adr ", (U032)*Adr);
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: DataLen ", (U032)DataLen);

    //
    // Enable writes to the I2C port
    //
    i2cHardwareInit(pDev, Head, portID);

    i2cStart(pDev, Head, portID);
    i2cSendByte(pDev, Head, portID, (U008)(ChipAdr<<1));        // send chip adr. with write bit

    for ( ; AdrLen; AdrLen--)               // send sub-register address byte(s)
    {
        if ( i2cSendByte(pDev, Head, portID, *Adr++) )
        {
            goto done;
        }
    }
    
    tmrDelay(pDev, I2CDELAY);    // give the device some time to parse the subaddress
    
    i2cStart(pDev, Head, portID);                             // send again chip address for switching to read mode
    if ( i2cSendByte(pDev, Head, portID, (U008)((ChipAdr<<1) | 1)) )  // send chip adr. with read bit
    {
        goto done;
    }

    for (status = RM_OK; DataLen && (status == RM_OK); DataLen--)
    {
        status = i2cReceiveByte(pDev,
                                Head,
                                portID, 
                                (U008 *)&dat, 
                                (U008)((DataLen == 1) ? NACK : ACK));         // receive byte(s)
        *Data++ = dat;
    }

done:
    i2cStop(pDev, Head, portID);
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Data ", (U032)*Data);
    
    return status;
}

RM_STATUS i2cRead_EDDC(PHWINFO pDev, U032 Head, U032 portID, U008 SegmentAddr,U008 ChipAdr, U008 SubByteAddr,U016 DataLen, U008 *Data)
{
    //extended DDC compatibility not confirmed on date modified. No monitors with edids greater than 256 are
    //easily obtainable or locatable.

    U008 dat;
    RM_STATUS status = RM_ERROR;        // pessimist
    
    //
    // Enable writes to the I2C port
    //
    i2cHardwareInit(pDev, Head, portID);

    //if segment!=0, set the segment with this sequence first
    if(SegmentAddr)
    {
        //send start
        i2cStart(pDev, Head, portID);
        
        //send segment register addr
        i2cSendByte(pDev, Head, portID, 0x60);
        
        //send the segment number
        i2cSendByte(pDev, Head, portID, SegmentAddr);
    }

    i2cStart(pDev, Head, portID);
    i2cSendByte(pDev, Head, portID, (U008)(ChipAdr<<1));        // send chip adr. with write bit    
    i2cSendByte(pDev, Head, portID, (U008)SubByteAddr);

    i2cStart(pDev, Head, portID);                             // send again chip address for switching to read mode
    if ( i2cSendByte(pDev, Head, portID, (U008)((ChipAdr<<1) | 1)) )  // send chip adr. with read bit
    {
        goto done;
    }

    for (status = RM_OK; DataLen && (status == RM_OK); DataLen--)
    {
        status = i2cReceiveByte(pDev,
                                Head,
                                portID, 
                                (U008 *)&dat, 
                                (U008)((DataLen == 1) ? NACK : ACK));         // receive byte(s)
        *Data++ = dat;
    }

done:
    i2cStop(pDev, Head, portID);
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Data ", (U032)*Data);
    
    return status;
}

RM_STATUS i2cSend(PHWINFO pDev, U032 Head, U032 portID, U008 ChipAdr, U016 AdrLen, U008 *Adr, U016 DataLen, U008 *Data, U032 NoStopFlag)
{

    if ( ChipAdr ) {
        //
        // Enable writes to the I2C port
        //
        i2cHardwareInit(pDev, Head, portID);

        i2cStart(pDev, Head, portID);
        if ( i2cSendByte(pDev, Head, portID, (U008)(ChipAdr<<1)) ) // send chip adr. with write bit
        {
            i2cStop(pDev, Head, portID);                         // ack failed --> generate stop condition
            return(RM_ERROR); 
        }
    }

    for ( ; AdrLen; AdrLen--)             
    {
        if ( i2cSendByte(pDev, Head, portID, *Adr++) )        // send sub-register byte(s)
        {
            i2cStop(pDev, Head, portID);                    // ack failed --> generate stop condition
            return(RM_ERROR); 
        }
    }

    for ( ; DataLen; DataLen--)            // send data byte(s)
    {
        if ( i2cSendByte(pDev, Head, portID, *Data++) )
        {
            i2cStop(pDev, Head, portID);                     // ack failed --> generate stop condition
            return(RM_ERROR);
        }
    }

    if ( NoStopFlag == 0 )
        i2cStop(pDev, Head, portID);

    return(RM_OK);
}


RM_STATUS i2cWrite_ALT(PHWINFO pDev, U032 Head, U032 portID, U008 ChipAdr, U016 AdrLen, U008 *Adr, U016 DataLen, U008 *Data)
{
    //
    // Enable writes to the I2C port
    //
    i2cHardwareInit(pDev, Head, portID);

    i2cStart(pDev, Head, portID);
    if ( i2cSendByte(pDev, Head, portID, (U008)(ChipAdr<<1)) ) // send chip adr. with write bit
    {
        i2cStop(pDev, Head, portID);                         // ack failed --> generate stop condition
        return(RM_ERROR); 
    }
    for ( ; DataLen; DataLen--)            // send data byte(s)
    {
        if ( i2cSendByte(pDev, Head, portID, *Data++) )
        {
            i2cStop(pDev, Head, portID);                     // ack failed --> generate stop condition
            return(RM_ERROR);
        }
    }
    i2cStop(pDev, Head, portID);
    return(RM_OK);
}


RM_STATUS i2cRead_ALT(PHWINFO pDev, U032 Head, U032 portID, U008 ChipAdr, U016 AdrLen, U008 *Adr, U016 DataLen, U008 *Data)
{
    U008 dat;
    
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: ChipAdr ", (U032)ChipAdr);
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: AdrLen ", (U032)AdrLen);
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Adr ", (U032)*Adr);
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: DataLen ", (U032)DataLen);

    //
    // Enable writes to the I2C port
    //
    i2cHardwareInit(pDev, Head, portID);

    i2cStart(pDev, Head, portID);
    i2cSendByte(pDev, Head, portID, (U008)((ChipAdr<<1) | 1));        // send chip adr. with write bit
    for ( ; DataLen ; DataLen--)
    {
        i2cReceiveByte(pDev, Head, portID, (U008 *)&dat, (U008)((DataLen == 1) ? NACK : ACK));         // receive byte(s)
        *Data++ = dat;
    }
    i2cStop(pDev, Head, portID);
    
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Data ", (U032)*Data);
    
    return(RM_OK);
}


RM_STATUS i2cRead_ALT2(PHWINFO pDev, U032 Head, U032 portID, U008 ChipAdr, U016 AdrLen, U008 *Adr, U016 DataLen, U008 *Data)
{
    U008 dat;
    
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: ChipAdr ", (U032)ChipAdr);
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: AdrLen ", (U032)AdrLen);
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Adr ", (U032)*Adr);
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: DataLen ", (U032)DataLen);

    //
    // Enable writes to the I2C port
    //
    i2cHardwareInit(pDev, Head, portID);

    i2cStart(pDev, Head, portID);
    i2cSendByte(pDev, Head, portID, (U008)(ChipAdr<<1));        // send chip adr. with write bit

    for ( ; AdrLen; AdrLen--)               // send sub-register address byte(s)
    {
        if ( i2cSendByte(pDev, Head, portID, *Adr++) )
        {
            i2cStop(pDev, Head, portID);                      // ack failed --> generate stop condition
            return(RM_ERROR); 
        }
    }
    i2cStop(pDev, Head, portID);
    
    i2cStart(pDev, Head, portID);                             // send again chip address for switching to read mode
    if ( i2cSendByte(pDev, Head, portID, (U008)((ChipAdr<<1) | 1)) )  // send chip adr. with read bit
    {
        i2cStop(pDev, Head, portID);                         // ack failed --> generate stop condition
        return(RM_ERROR); 
    }

    for ( ; DataLen ; DataLen--)
    {
        i2cReceiveByte(pDev, Head, portID, (U008 *)&dat, (U008)((DataLen == 1) ? NACK : ACK));         // receive byte(s)
        *Data++ = dat;
    }

    i2cStop(pDev, Head, portID);
    
    //DBG_PRINT_STRING_VALUE(DEBUGLEVEL_TRACEINFO, "NVRM: Data ", (U032)*Data);
    
    return(RM_OK);
}

RM_STATUS i2cAccess
(
    PHWINFO pDev,
    U032 Head,
    NVRM_I2C_ACCESS_CONTROL    *pCtrl
)
{

    RM_STATUS rmStatus = RM_OK;

    U032	startFlag;
    U032	stopFlag;
    U032	ackFlag;
    U032	port;
    U008	data = 0;
    U008    lock;
    
    /*
     * verify the token
     *
     * NOTE: we allow I2C Access if the token is 0 AND no one
     * else has acquired it.  The idea is that 3rd party vendors
     * will not be able to acquire/lock I2C; however, we can still
     * acquire/lock I2C internally and lock 3rd party access out.
     * The reason is that we don't a 3rd party app to crash while
     * still holding a lock on I2C and thus lock us out from internal
     * access.  We have priority in this scheme and 3rd party
     * apps must deal with contention by checking return codes for
     * errors.
     */
     
    if (pDev->Power.State == MC_POWER_LEVEL_3)
    {
    	DBG_PRINT_STRING(DEBUGLEVEL_ERRORS, "NVRM: Attempted I2C access before waking up. Invalid request denied.\n");
    
    	//if we are at a state of power management, we need to reject this call
    	return NVRM_I2C_ACCESS_STATUS_ERROR;
    }

    // unlock the extended regs
    lock = UnlockCRTC(pDev, Head);

	//point to correct head and enable i2c
	EnableHead(pDev, Head);
	AssocDDC(pDev, Head);

    if ((pCtrl->cmd == NVRM_I2C_ACCESS_CMD_WRITE_BYTE) ||
        (pCtrl->cmd == NVRM_I2C_ACCESS_CMD_READ_BYTE) ||
        (pCtrl->cmd == NVRM_I2C_ACCESS_CMD_NULL) ||
        (pCtrl->cmd == NVRM_I2C_ACCESS_CMD_RESET) ||
        (pCtrl->cmd == NVRM_I2C_ACCESS_CMD_READ_SDA) ||
        (pCtrl->cmd == NVRM_I2C_ACCESS_CMD_READ_SCL) ||
        (pCtrl->cmd == NVRM_I2C_ACCESS_CMD_WRITE_SDA) ||
        (pCtrl->cmd == NVRM_I2C_ACCESS_CMD_WRITE_SCL)) {

        if (pCtrl->token != pDev->I2CAcquired) {
            pCtrl->status = NVRM_I2C_ACCESS_STATUS_DEVICE_BUSY;
            rmStatus = RM_ERROR;
            goto done;
        }
    }

    /*
     * retrieve the flags and port
     */

    startFlag = pCtrl->flags & NVRM_I2C_ACCESS_FLAG_START;
    stopFlag = pCtrl->flags & NVRM_I2C_ACCESS_FLAG_STOP;
    ackFlag = pCtrl->flags & NVRM_I2C_ACCESS_FLAG_ACK;

    if ( pCtrl->port == NVRM_I2C_ACCESS_PORT_PRIMARY ) {
        port = NV_I2C_PORT_PRIMARY;
    }
    else {
        port = NV_I2C_PORT_SECONDARY;
    }

    /*
     * handle the command
     */

    pCtrl->status = NVRM_I2C_ACCESS_STATUS_SUCCESS;

    switch ( pCtrl->cmd ) {

      case NVRM_I2C_ACCESS_CMD_ACQUIRE:

        if ( pDev->I2CAcquired ) {
            pCtrl->status = NVRM_I2C_ACCESS_STATUS_DEVICE_BUSY;
            rmStatus = RM_ERROR;
        }
        else {
            pDev->I2CAcquired = 0xbeeffeed; /* arbtrary */
            pCtrl->token = pDev->I2CAcquired;
            i2cHardwareInit(pDev, Head, port);
        }

        break;

      case NVRM_I2C_ACCESS_CMD_RELEASE:

        if ( pDev->I2CAcquired != pCtrl->token ) {
            pCtrl->status = NVRM_I2C_ACCESS_STATUS_ERROR;
            rmStatus = RM_ERROR;
        }
        else {
            pDev->I2CAcquired = 0;
            pCtrl->token = 0;
            pCtrl->status = NVRM_I2C_ACCESS_STATUS_SUCCESS;
        }

        break;

      case NVRM_I2C_ACCESS_CMD_WRITE_BYTE:

        if ( startFlag ) {
            i2cStart(pDev, Head, port);
        }

        data = (U008)(pCtrl->data);
        if ( i2cSendByte(pDev, Head, port,data) ) {
            i2cStop(pDev, Head, port);
            pCtrl->status = NVRM_I2C_ACCESS_STATUS_PROTOCOL_ERROR;
            rmStatus = RM_ERROR;
        }

        if ( stopFlag ) {
            i2cStop(pDev, Head, port);
        }

      	break;

      case NVRM_I2C_ACCESS_CMD_READ_BYTE:

        if ( startFlag ) {
            i2cStart(pDev, Head, port);
        }

        // check for restart read 
        if ( startFlag ) {
            data = (U008)(pCtrl->data);
            if ( i2cSendByte(pDev, Head, port,data) ) {
                i2cStop(pDev, Head, port);
                pCtrl->status = NVRM_I2C_ACCESS_STATUS_PROTOCOL_ERROR;
                rmStatus = RM_ERROR;
            }
        }
        else {
            i2cReceiveByte(pDev, Head, port, &data, (U008)((ackFlag) ? 0 : 1));
            pCtrl->data = data;
        }

        if ( stopFlag ) {
            i2cStop(pDev, Head, port);
        }

      	break;

      case NVRM_I2C_ACCESS_CMD_NULL:

        if ( startFlag ) {
            i2cStart(pDev, Head, port);
        }

        if ( stopFlag ) {
            i2cStop(pDev, Head, port);
        }

      	break;

      case NVRM_I2C_ACCESS_CMD_RESET:

        i2cStop(pDev, Head, port);

      	break;

      case NVRM_I2C_ACCESS_CMD_READ_SDA:

        ReadSDA(pDev, Head, port, &data);
        pCtrl->data = data;

        break;

      case NVRM_I2C_ACCESS_CMD_READ_SCL:

        ReadSCL(pDev, Head, port, &data);
        pCtrl->data = data;

        break;

      case NVRM_I2C_ACCESS_CMD_WRITE_SDA:

        if ( pCtrl->data ) {
            SetSDALine(pDev, Head, port);
        }
        else {
            ResetSDALine(pDev, Head, port);
        }

      case NVRM_I2C_ACCESS_CMD_WRITE_SCL:

        if ( pCtrl->data ) {
            SetSCLLine(pDev, Head, port);
        }
        else {
            ResetSCLLine(pDev, Head, port);
        }

        break;

      default:

        pCtrl->status = NVRM_I2C_ACCESS_STATUS_ERROR;
        rmStatus = RM_ERROR;
        break;

    }

done:
    RestoreLock(pDev, Head, lock);
    return rmStatus;
}
