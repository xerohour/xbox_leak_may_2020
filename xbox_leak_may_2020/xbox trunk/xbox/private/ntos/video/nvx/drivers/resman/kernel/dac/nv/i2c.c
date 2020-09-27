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

// Constants used by I2C Protocoll:

#define SDA_REG         0x02
#define SCL_REG         0x01


#define ACK             0   // I2C Acknowledge
#define NACK            1   // I2C No Acknowledge

	
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
    // save regs (especially ebx) that might get stomped by 'cpuid'
    __asm__("push %esi");
    __asm__("push %edi");
    __asm__("push %ebx");
        __asm__("xorl %eax, %eax");
        __asm__("cpuid");
    __asm__("pop %ebx");
    __asm__("pop %edi");
    __asm__("pop %esi");
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
        tmrDelay(pDev, 5000);         // 5us delay    
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
        tmrDelay(pDev, 1000);              // 1.0 us delay    
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
    tmrDelay(pDev, 5000); SetSCLLine(pDev, Head, portID); WaitHighSCLLine(pDev, Head, portID);
    ResetSDALine(pDev, Head, portID);
    tmrDelay(pDev, 5000);
    ResetSCLLine(pDev, Head, portID);
}


VOID i2cStop(PHWINFO pDev, U032 Head, U032 portID)
{
    tmrDelay(pDev, 200000);
    ResetSCLLine(pDev, Head, portID);
    ResetSDALine(pDev, Head, portID);
    tmrDelay(pDev, 5000); SetSCLLine(pDev, Head, portID); WaitHighSCLLine(pDev, Head, portID); 
    SetSDALine(pDev, Head, portID); 
    tmrDelay(pDev, 5000);
}


/*
 * I2cAck() returns 1: fail
 *                  0: acknolege
 */

U008 i2cAck(PHWINFO pDev, U032 Head, U032 portID)
{
    U008 ack;

    ResetSCLLine(pDev, Head, portID); tmrDelay(pDev, 5000);
    SetSDALine(pDev, Head, portID); tmrDelay(pDev, 5000);
    SetSCLLine(pDev, Head, portID); WaitHighSCLLine(pDev, Head, portID);
    ReadSDA(pDev, Head, portID, &ack);
    ResetSCLLine(pDev, Head, portID);
    return (ack);
}


VOID i2cInit(PHWINFO pDev, U032 Head, U032 portID)
{
    SetSCLLine(pDev, Head, portID); WaitHighSCLLine(pDev, Head, portID);
    SetSDALine(pDev, Head, portID); 
}


U008 i2cSendByte(PHWINFO pDev, U032 Head, U032 portID, U008 byte)
{
    U008 i;

    for (i=0;i<8;i++)
    {
	    ResetSCLLine(pDev, Head, portID);
	    tmrDelay(pDev, 2500);
        if (byte & 0x80)
		    SetSDALine(pDev, Head, portID);
	    else
		    ResetSDALine(pDev, Head, portID);
	    tmrDelay(pDev, 2500);
	    SetSCLLine(pDev, Head, portID); WaitHighSCLLine(pDev, Head, portID);
	    byte <<= 1;
    }
    
    return (i2cAck(pDev, Head, portID));
}


RM_STATUS i2cReceiveByte(PHWINFO pDev, U032 Head, U032 portID, U008 *byte, U008 ack)
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
	    ResetSCLLine(pDev, Head, portID);	/* this second reset seems to be needed, why? (jsun) */
	    SetSCLLine(pDev, Head, portID); 
        status = WaitHighSCLLine(pDev, Head, portID) ? RM_ERROR : RM_OK;
        if (status != RM_OK)
            goto done;

	    ReadSDA(pDev, Head, portID, &data);
	    *byte <<= 1;
	    *byte  |= (data == 1);
	}

    ResetSCLLine(pDev, Head, portID);
    if (ack) 
    {
        SetSDALine(pDev, Head, portID);         // send Nack
	tmrDelay(pDev, 1000);
    }
    else
        ResetSDALine(pDev, Head, portID);       // send Ack
    
    SetSCLLine(pDev, Head, portID); 
    status = WaitHighSCLLine(pDev, Head, portID) ? RM_ERROR : RM_OK;
    ResetSCLLine(pDev, Head, portID);

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
     
    if(pDev->Power.State == MC_POWER_LEVEL_3)
    {
    	DBG_PRINT_STRING(DEBUGLEVEL_ERRORS,"Attempted I2C access before waking up. Invalid request denied.\n");
    
    	//if we are at a state of power management, we need to reject this call
    	return NVRM_I2C_ACCESS_STATUS_ERROR;
    }
     
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
            return RM_ERROR;
        }

    }

    /*
     * Unlock CRTC extended regs
     */

    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE, Head);

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

    /* Lock CRTC extended regs */
    CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE, Head);

    return rmStatus;

}

#if 0

///////////////////////////////////////////////////////////////////////////////////////
//
// The following routines were rewritten to not use the NV timer for 
// self-timing.  Don't use them, but keep them around in case we do need them 
// in the future.
//

//
//  delay
//
i2cdelay(int count)
{
	while(count) 
    {
        _asm {
            push dx
            mov  dx,0x80
            in   al,dx
            pop  dx
        }
		count--;
	}
}

//****************************************************************************************
//
//  Send a byte out the I2C interface
int sendbyte(int serial_data) {
	int i, rval=0;
	int readback=0 ; // clear the readback byte
	int r;

    r=0;

// send the bits
	for (i=128; i>=1; i=i/2)
    {

//set the data with clock low
		if (serial_data & i) 
			DDCout(clock0_hi);
		else
			DDCout(clock0_lo);
		i2cdelay(I2C_DELAY/2);

// set clock hi
		if (serial_data & i) 
			DDCout(clock1_hi);
		else
			DDCout(clock1_lo);
		i2cdelay(I2C_DELAY);

// read the data port
#if DEBUG4
		r=input(read_port) ; printf ("port= %x",r);
		r=~r ; printf (", inv, %x",r);
		r=r & SDATA ; printf (", \&, %x",r);
		r=r/2 ; printf (", div, %x",r);
		r=r*i ; printf (", bit #, %x",r);
		readback=readback+r ; printf (", readback=%x \n",readback);
#else
		readback+=i * ( ( (~DDCin() ) & SDATA) >> SDATA_BIT ) ;  // read data in case this is a read
#endif

//return the clock low
		if (serial_data & i) 
			DDCout(clock0_hi);
		else
			DDCout(clock0_lo);
		i2cdelay(I2C_DELAY/2);
	}

// ack bit
	DDCout(ack_state0);
	i2cdelay(I2C_DELAY/2);
	DDCout(ack_state1);
	i2cdelay(I2C_DELAY);

#if DEBUG2
	if ((input(read_port) & 0x02)) printf(" ack\n");
	else printf(" no ack\n");
#endif

	if ((DDCin() & data_in)) rval=ERR_I2C_COMM;	// return a fail.

//	printf (" readback=%x \n", readback );
	DDCout(ack_state2);
	i2cdelay(I2C_DELAY/2);
//   DDCout(i2c_state0);                 // bring both high
	return rval;
}

//****************************************************************************************
//
//  read a byte from the I2C interface
//  this is almost identical to the send byte function
int readbyte(int *read_data) {
	int i, rval=0;
	int readback=0 ; // clear the readback byte
    int serial_data = 0xff;
	int r;

    r=0;

// send the bits
	for (i=128; i>=1; i=i/2)
    {

// send all 1's, which tristates the line, and read back what the encoder is driving
//set the data with clock low
		if (serial_data & i) 
			DDCout(clock0_hi);
		else
			DDCout(clock0_lo);
		i2cdelay(I2C_DELAY/2);

// set clock hi
		if (serial_data & i) 
			DDCout(clock1_hi);
		else
			DDCout(clock1_lo);
		i2cdelay(I2C_DELAY);

//return the clock low
		if (serial_data & i) 
			DDCout(clock0_hi);
		else
			DDCout(clock0_lo);
		i2cdelay(I2C_DELAY/2);

// read the data port
		readback+=i * ( ( (DDCin() ) & SDATA) >> SDATA_BIT) ;  
	}

// ack bit
	DDCout(ack_state0);
	i2cdelay(I2C_DELAY/2);
	DDCout(ack_state1);
	i2cdelay(I2C_DELAY);

	if ((DDCin() & data_in)) rval=ERR_I2C_COMM;	// return a fail.

//	printf (" readback=%x \n", readback );
	DDCout(ack_state2);
	i2cdelay(I2C_DELAY/2);
//   DDCout(i2c_state0);                 // bring both high
    *read_data = readback;
	return rval;
}

//****************************************************************************************
//
void clockStart() {	// creates the start condition

#if DEBUG3
      output(write_port,0x0C); // create a strobe on pin 16 & 17
	i2cdelay(I2C_DELAY);
#endif
	DDCout(i2c_state0);
	i2cdelay(I2C_DELAY/2);
	DDCout(i2c_state1);
	i2cdelay(I2C_DELAY/2);
	DDCout(i2c_state2);
	i2cdelay(I2C_DELAY/2);

}

//****************************************************************************************
//
void clockEnd() {	// creates the end condition

	DDCout(i2c_state2);
	i2cdelay(I2C_DELAY/2);
	DDCout(i2c_state1);
	i2cdelay(I2C_DELAY/2);
	DDCout(i2c_state0);
	i2cdelay(I2C_DELAY/2);
}

//****************************************************************************************
//
VOID DDCout(U008 data)
{
    CRTC_WR(NV_CIO_CRE_DDC_WR__INDEX, data);
}

//****************************************************************************************
//
U008 DDCin()
{
U008 data;

    CRTC_RD(NV_CIO_CRE_DDC_STATUS__INDEX, data);
    return data;
}

/////////////////////////////////////////////////////////////////////

#endif /* 0 */

