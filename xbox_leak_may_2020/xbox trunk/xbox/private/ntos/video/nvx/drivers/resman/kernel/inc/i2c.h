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

/***************************** I2C includes ********************************\
*                                                                           *
* Module: I2C.H                                                             *
*   This module contains structure and prototype definitions used in the    *
*   I2C interface routines.                                                 *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/
#ifndef _I2C_H_
#define _I2C_H_

#define	NV_I2C_PORT_PRIMARY	    0
#define	NV_I2C_PORT_SECONDARY   1
#define	NV_I2C_PORT_TERTIARY    2

// This is the number of times we try sending out an I2C addresss byte without
// getting an acknowledge back before giving up.
#define I2C_ADDRESS_RETRIES     8

RM_STATUS i2cRead(PHWINFO, U032, U032, U008, U016, U008 *, U016, U008 *);
RM_STATUS i2cWrite(PHWINFO, U032, U032, U008, U016, U008 *, U016, U008 *);
RM_STATUS i2cSend(PHWINFO, U032, U032, U008, U016, U008 *, U016 , U008 *, U032);

RM_STATUS i2cRead_ALT(PHWINFO, U032, U032, U008, U016, U008 *, U016, U008 *);
RM_STATUS i2cWrite_ALT(PHWINFO, U032, U032, U008, U016, U008 *, U016, U008 *);

RM_STATUS i2cRead_ALT2(PHWINFO, U032, U032, U008, U016, U008 *, U016, U008 *);

//
// lower level routines
//
VOID i2cInit(PHWINFO, U032, U032 );
U008 i2cSendByte(PHWINFO, U032, U032, U008);
RM_STATUS i2cReceiveByte(PHWINFO, U032, U032, U008 *, U008);
VOID i2cStart(PHWINFO, U032, U032 );
VOID i2cStop(PHWINFO, U032, U032 );

U008 i2cHardwareInit(PHWINFO, U032, U032);
VOID ResetSCLLine(PHWINFO, U032, U032);
VOID SetSCLLine(PHWINFO, U032, U032);
VOID ResetSDALine(PHWINFO, U032, U032);
VOID SetSDALine(PHWINFO, U032, U032);
VOID ReadSDA(PHWINFO, U032, U032, U008 *);
VOID ReadSCL(PHWINFO, U032, U032, U008 *);
#endif // _I2C_H_
