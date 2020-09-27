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

/***************************** Gray Code Support *****************************
 *                                                                           *
 * Module: grayclk.c                                                         *
 *                                                                           *
 *****************************************************************************/

#include <nvrm.h>
#include <nvhw.h>
#include <nvos.h>
#include <bios.h>            // need BIOS_ROM_CODE_SIZE
#include <nv10_ref.h>

#define __KERNEL__
#include <nv.h>
#include <Nvcm.h>

//doubled table length to avoid wasted effort of boundary checking for something so small
static const U032 GrayCodeLookupTable[16]={0,1,3,2,6,7,5,4,0,1,3,2,6,7,5,4};
static const U032 GrayCodeLookupTableLength=16;

static U032 GrayCodeLookup(U032 Value,U032 starting_position)
{
	U032 x;
	
	Value=Value&0x7;
	
	//looks up 3 bit value in lookup table and returns the position
	for(x=starting_position;x<GrayCodeLookupTableLength;x++)
	{
		if(GrayCodeLookupTable[x]==Value)
		{
			return x;
		}
	}
	
	return 0;
}

void RmProperClockPdivProgrammer(PHWINFO pDev,U032 ClockAddress,U032 NewValue)
{
	U032 OldValue;
	U032 OldPdiv;
	U032 NewPdiv;
	U032 PllValLessPdiv;
	U032 ListPosition;
	U032 TargetPosition;
	U032 CurrentPosition;
	
	//read old value
	OldValue=REG_RD32(ClockAddress);
	
	//obtain old pdiv (16 though 18)
	OldPdiv=(OldValue>>16)&0x7;
	
	//obtain the desired Pll value less the Pdiv
	PllValLessPdiv=NewValue&0xffff;
	
	//obtain the desired Pdiv
	NewPdiv=(NewValue>>16)&0x7;
	
	//lookup position of the items we have and want
	ListPosition=GrayCodeLookup(OldPdiv,0);
	TargetPosition=GrayCodeLookup(NewPdiv,ListPosition);
	
	//transition via gray code to desired setting
	for(CurrentPosition=ListPosition;CurrentPosition<=TargetPosition;CurrentPosition++)
	{
		REG_WR32(ClockAddress,(GrayCodeLookupTable[CurrentPosition]<<16)|PllValLessPdiv);
		
		//wait to allow settle before transitioning again
		osDelay(10);
	}
}
