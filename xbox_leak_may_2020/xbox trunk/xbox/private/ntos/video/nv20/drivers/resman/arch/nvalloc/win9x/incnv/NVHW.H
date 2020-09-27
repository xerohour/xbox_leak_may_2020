#ifndef _NVHW_H_
#define _NVHW_H_
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

/**************** Resource Manager Defines and Structures ******************\
*                                                                           *
* Module: NVHW.H                                                            *
*       Resource Manager hardware access macros (OS specific).              *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/


//
// HW register access macros.
//

#define REG_WR08(a,d)   nvAddr->Reg008[(a)]  =(U008)(d)
#define REG_WR16(a,d)   nvAddr->Reg016[(a)/2]=(U016)(d)
#define REG_WR32(a,d)   nvAddr->Reg032[(a)/4]=(U032)(d)
#define REG_RD08(a)     nvAddr->Reg008[(a)]
#define REG_RD16(a)     nvAddr->Reg016[(a)/2]
#define REG_RD32(a)     nvAddr->Reg032[(a)/4]
#define WriteIndexed(addr, datum) REG_WR08(addr, datum & 0xFF); REG_WR08(addr+1, (datum >> 8) & 0xFF);
#define ReadIndexed(addr, reg) ReadIndexedpDev(pDev,addr,reg)

/*
	The REG_WR..DIRECT() and REG_RD..DIRECT() macros are used to access the NV 
    registers given as system addresses.  System addresses of the registers
    are passed in.  A read is made directly from that address, without manipulation. 
*/
#define REG_WR08_DIRECT(a,d)   (*a)=(U008)(d)
#define REG_WR16_DIRECT(a,d)   (*a)=(U016)(d)
#define REG_WR32_DIRECT(a,d)   (*a)=(U032)(d)
#define REG_RD08_DIRECT(a)     (*a)
#define REG_RD16_DIRECT(a)     (*a)
#define REG_RD32_DIRECT(a)     (*a)


#define CFG_WR08(a,d)
#define CFG_WR16(a,d)
#define CFG_WR32(a,d)
#define CFG_RD08(a)
#define CFG_RD16(a)
#define CFG_RD32(a)

#define IO_WR08(a,d)    outp((a),(d))
#define IO_WR16(a,d)    outpw((a),(d))
#define IO_WR32(a,d)    outpdw((a),(d))
#define IO_RD08(a)      inp(a)
#define IO_RD16(a)      inpw(a)
#define IO_RD32(a)      inpdw(a)

#define ROM_RD08(a)     (romAddr->Reg008[(a)])
#define ROM_RD16(a)     (romAddr->Reg008[(a)]          \
                        |(romAddr->Reg008[(a)+1]<<8))
#define ROM_RD32(a)     (romAddr->Reg008[(a)]          \
                        |(romAddr->Reg008[(a)+1]<<8)   \
                        |(romAddr->Reg008[(a)+2]<<16)  \
                        |(romAddr->Reg008[(a)+3]<<24))
                        
#define BIOS_RD08(a)     (biosAddr->Reg008[(a)])
#define BIOS_RD16(a)     (biosAddr->Reg008[(a)]          \
                        |(biosAddr->Reg008[(a)+1]<<8))
#define BIOS_RD32(a)     (biosAddr->Reg008[(a)]          \
                        |(biosAddr->Reg008[(a)+1]<<8)   \
                        |(biosAddr->Reg008[(a)+2]<<16)  \
                        |(biosAddr->Reg008[(a)+3]<<24))

#if 0                        
//
// HW register access macros, based on dumb frame buffer address.
//

#define MEM_BASE        (0?NV_PDFB)
#define MEM_WR08(a,d)   fbAddr->Reg008[(a-MEM_BASE)]  =(U008)(d)
#define MEM_WR16(a,d)   fbAddr->Reg016[(a-MEM_BASE)/2]=(U016)(d)
#define MEM_WR32(a,d)   fbAddr->Reg032[(a-MEM_BASE)/4]=(U032)(d)
#define MEM_RD08(a)     fbAddr->Reg008[(a-MEM_BASE)]
#define MEM_RD16(a)     fbAddr->Reg016[(a-MEM_BASE)/2]
#define MEM_RD32(a)     fbAddr->Reg032[(a-MEM_BASE)/4]
#else
#define FB_WR08(a,d)   fbAddr->Reg008[(a)  ] = (U008)(d)
#define FB_WR16(a,d)   fbAddr->Reg016[(a)/2] = (U016)(d)
#define FB_WR32(a,d)   fbAddr->Reg032[(a)/4] = (U032)(d)
#define FB_RD08(a)     fbAddr->Reg008[(a)  ]
#define FB_RD16(a)     fbAddr->Reg016[(a)/2]
#define FB_RD32(a)     fbAddr->Reg032[(a)/4]
#endif

#define INST_DIRECT_WR32(a,d)    nvAddr->Reg032[(pDev->Pram.HalInfo.PraminOffset+a)/4]=(U032)(d)
#define INST_DIRECT_RD32(a)      nvAddr->Reg032[(pDev->Pram.HalInfo.PraminOffset+a)/4]

#define channelInfo clientInfo[ClientInstance].CliDeviceList[DeviceInstance].DevFifoList

#endif // _NVHW_H_

