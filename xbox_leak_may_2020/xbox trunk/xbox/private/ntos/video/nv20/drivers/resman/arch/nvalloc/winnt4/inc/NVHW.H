#ifndef _NVHW_H_
#define _NVHW_H_
/**************************************************************************************************************
*
*	Module:  nvhw.h
*
*	Description:
*		Hardware access macros for the resource manager.
*
*
*	Copyright (c) 1993-1997 NVIDIA, Corp.  All rights reserved.   
*	                                                                 
*	NOTICE TO USER:   The source code  is copyrighted under  U.S. and
*	international laws.   NVIDIA, Corp. of Sunnyvale, California owns
*	the copyright  and as design patents  pending  on the design  and
*	interface  of the NV chips.   Users and possessors of this source
*	code are hereby granted  a nonexclusive,  royalty-free  copyright
*	and  design  patent license  to use this code  in individual  and
*	commercial software.                                             
*	                                                                 
*	Any use of this source code must include,  in the user documenta-
*	tion and  internal comments to the code,  notices to the end user
*	as follows:                                                      
*	                                                                 
*	Copyright (c) 1993-1997  NVIDIA, Corp.    NVIDIA  design  patents
*	pending in the U.S. and foreign countries.                       
*	                                                                 
*	NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF
*	THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT
*	EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS
*	ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL
*	IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A
*	PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE
*	FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,
*	OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR
*	PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER
*	TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR
*	PERFORMANCE OF THIS SOURCE CODE.                                 
*                                                                      
*************************************************************************************************************/

/*
	The REG_WR..() and REG_RD..() macros are used to access the NV registers.
	Actual address are passed in then scaled to the correct size of access,
	so no prescaling is necessary.  Just use the register address defined
	in the reference manuals, e.g. nv.._ref.h.
*/
#define REG_WR08(a,d)   (osWriteReg008((U008*)&nvAddr->Reg008[(a)  ], (U008)(d)))
#define REG_WR16(a,d)   (osWriteReg016((U016*)&nvAddr->Reg016[(a)/2], (U016)(d)))
#define REG_WR32(a,d)   (osWriteReg032((U032*)&nvAddr->Reg032[(a)/4], (U032)(d)))
#define REG_RD08(a)     (osReadReg008 ((U008*)&nvAddr->Reg008[(a)  ])			)
#define REG_RD16(a)     (osReadReg016 ((U016*)&nvAddr->Reg016[(a)/2])			)
#define REG_RD32(a)     (osReadReg032 ((U032*)&nvAddr->Reg032[(a)/4])			)

/*
	The REG_WR..DIRECT() and REG_RD..DIRECT() macros are used to access the NV 
    registers given as system addresses.  System addresses of the registers
    are passed in.  A read is made directly from that address, without manipulation. 
*/
#define REG_WR08_DIRECT(a,d)   (osWriteReg008((a), (d)))
#define REG_WR16_DIRECT(a,d)   (osWriteReg016((a), (d)))
#define REG_WR32_DIRECT(a,d)   (osWriteReg032((a), (d)))
#define REG_RD08_DIRECT(a)     (osReadReg008 ((a)     ))
#define REG_RD16_DIRECT(a)     (osReadReg016 ((a)     ))
#define REG_RD32_DIRECT(a)     (osReadReg032 ((a)     ))

/*
	The MEM_WR..() and MEM_RD..() macros are used to access the frame buffer
	memory.  Actual addresses are passed in then scaled to the correct size of
	access, so no prescaling is necessary.  Just use the actual byte address
	of the frame buffer that needs to be accessed.  

	Note:  For NV3, these macros are used to access instance memory directly.
	So, passing in a 0 would access the first location of instance memory, 
	which is at the end of the frame buffer, i.e. at an offset of NV_PRAMIN
	from the beginning of the frame buffer.
*/

// frame buffer access macros
// for NV4 and above, fbAddr actually points to the frame buffer
#define MEM_BASE		(0)
#define MEM_WR08(a,d)   (osWriteReg008((U008*)&fbAddr->Reg008[((a)-MEM_BASE)  ], (U008)(d)))
#define MEM_WR16(a,d)   (osWriteReg016((U016*)&fbAddr->Reg016[((a)-MEM_BASE)/2], (U016)(d)))
#define MEM_WR32(a,d)   (osWriteReg032((U032*)&fbAddr->Reg032[((a)-MEM_BASE)/4], (U032)(d)))
#define MEM_RD08(a)     (osReadReg008 ((U008*)&fbAddr->Reg008[((a)-MEM_BASE)  ])		   )
#define MEM_RD16(a)     (osReadReg016 ((U016*)&fbAddr->Reg016[((a)-MEM_BASE)/2])		   )
#define MEM_RD32(a)     (osReadReg032 ((U032*)&fbAddr->Reg032[((a)-MEM_BASE)/4])		   )

#define FB_WR08(a,d)   (osWriteReg008((U008*)&fbAddr->Reg008[(a)  ], (U008)(d)))
#define FB_WR16(a,d)   (osWriteReg016((U016*)&fbAddr->Reg016[(a)/2], (U016)(d)))
#define FB_WR32(a,d)   (osWriteReg032((U032*)&fbAddr->Reg032[(a)/4], (U032)(d)))
#define FB_RD08(a)     (osReadReg008 ((U008*)&fbAddr->Reg008[(a)  ])		   )
#define FB_RD16(a)     (osReadReg016 ((U016*)&fbAddr->Reg016[(a)/2])		   )
#define FB_RD32(a)     (osReadReg032 ((U032*)&fbAddr->Reg032[(a)/4])		   )

/*
	The ROM_RD..() and BIOS_RD..() macros are used to read data from the BIOS, an image of 
	which resides in the first 64K of instance memory.
*/

// ROM access macros
#define ROM_RD08(a)     ((osReadReg008((U008*)&romAddr->Reg008[(a)  ])    ))
#define ROM_RD16(a)     ((osReadReg008((U008*)&romAddr->Reg008[(a)  ])	  )\
                        |(osReadReg008((U008*)&romAddr->Reg008[(a)+1])<<8 ))
#define ROM_RD32(a)     ((osReadReg008((U008*)&romAddr->Reg008[(a)  ])    )\
                        |(osReadReg008((U008*)&romAddr->Reg008[(a)+1])<<8 )\
                        |(osReadReg008((U008*)&romAddr->Reg008[(a)+2])<<16)\
                        |(osReadReg008((U008*)&romAddr->Reg008[(a)+3])<<24))

// BIOS access macros
#define BIOS_RD08(a)    ((osReadReg008((U008*)&biosAddr->Reg008[(a)  ])    ))
#define BIOS_RD16(a)    ((osReadReg008((U008*)&biosAddr->Reg008[(a)  ])	  )\
                        |(osReadReg008((U008*)&biosAddr->Reg008[(a)+1])<<8 ))
#define BIOS_RD32(a)    ((osReadReg008((U008*)&biosAddr->Reg008[(a)  ])    )\
                        |(osReadReg008((U008*)&biosAddr->Reg008[(a)+1])<<8 )\
                        |(osReadReg008((U008*)&biosAddr->Reg008[(a)+2])<<16)\
                        |(osReadReg008((U008*)&biosAddr->Reg008[(a)+3])<<24))

#endif // _NVHW_H_

