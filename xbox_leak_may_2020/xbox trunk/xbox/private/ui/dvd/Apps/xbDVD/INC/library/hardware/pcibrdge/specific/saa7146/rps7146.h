
// FILE:			library\hardware\pcibridge\specific\saa7146\rps7146.h
// AUTHOR:		D. Heidrich, U. Sigmund
// COPYRIGHT:	(c) 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		02.10.96
//
// PURPOSE:		Class for 7146 RPS programs.
//
// HISTORY:

#ifndef RPS7146_H
#define RPS7146_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\lowlevel\dmabuffr.h"



// RPS commands. Don't forget to append the data DWORDs after LDREG_SEQ.
#define RPS_PAUSE(flags)					0x20000000UL | (DWORD)(flags)
#define RPS_UPLOAD(upflags)				0x40000000UL | (DWORD)(upflags)
#define RPS_CHECK_LATE(flags)				0x30000000UL | (DWORD)(flags)
#define RPS_CLR_SIGNAL(flags)				0x00000000UL | (DWORD)(flags)
#define RPS_NOP								0x00000000UL
#define RPS_SET_SIGNAL(flags)				0x10000000UL | (DWORD)(flags)
#define RPS_INTERRUPT(flags)				0x60000000UL | (DWORD)(flags)
#define RPS_STOP(flags)						0x50000000UL | (DWORD)(flags)
#define RPS_JUMP(flags, dest)				0x80000000UL | (DWORD)(flags), (DWORD)(dest)
#define RPS_LDREG(reg, val)				0x90000100UL | (DWORD)(reg>>2), (DWORD)(val)   // no multiple data support
#define RPS_LDREG_SEQ(reg, length)		0x90000000UL | (DWORD)(reg>>2) | (DWORD)(length<<8)
#define RPS_STREG(reg, addr)				0xa0000100UL | (DWORD)(reg>>2), (DWORD)(addr)
#define RPS_MASKLOAD(reg, mask, val)	0xc0000000UL | (DWORD)(reg>>2), (DWORD)(mask), (DWORD)(val)
#define RPS_DONE								0xffffffffUL

// RPS flags
#define RPSB_OAN				27
#define RPSF_OAN				MKFLAG(RPSB_OAN)
#define RPSB_INV				26
#define RPSF_INV				MKFLAG(RPSB_INV)
#define RPSB_SIG4				25
#define RPSF_SIG4				MKFLAG(RPSB_SIG4)
#define RPSB_SIG3				24
#define RPSF_SIG3				MKFLAG(RPSB_SIG3)
#define RPSB_SIG2				23
#define RPSF_SIG2				MKFLAG(RPSB_SIG2)
#define RPSB_SIG1				22
#define RPSF_SIG1				MKFLAG(RPSB_SIG1)
#define RPSB_SIG0				21
#define RPSF_SIG0				MKFLAG(RPSB_SIG0)
#define RPSB_GPIO3			20
#define RPSF_GPIO3			MKFLAG(RPSB_GPIO3)
#define RPSB_GPIO2			19
#define RPSF_GPIO2			MKFLAG(RPSB_GPIO2)
#define RPSB_GPIO1			18
#define RPSF_GPIO1			MKFLAG(RPSB_GPIO1)
#define RPSB_GPIO0			17
#define RPSF_GPIO0			MKFLAG(RPSB_GPIO0)
#define RPSB_HT				16
#define RPSF_HT				MKFLAG(RPSB_HT)
#define RPSB_HS				15
#define RPSF_HS				MKFLAG(RPSB_HS)
#define RPSB_O_FID_B			14
#define RPSF_O_FID_B			MKFLAG(RPSB_O_FID_B)
#define RPSB_E_FID_B			13
#define RPSF_E_FID_B			MKFLAG(RPSB_E_FID_B)
#define RPSB_O_FID_A			12
#define RPSF_O_FID_A			MKFLAG(RPSB_O_FID_A)
#define RPSB_E_FID_A			11
#define RPSF_E_FID_A			MKFLAG(RPSB_E_FID_A)
#define RPSB_VBI_A			10
#define RPSF_VBI_A			MKFLAG(RPSB_VBI_A)
#define RPSB_VBI_B			9
#define RPSF_VBI_B			MKFLAG(RPSB_VBI_B)
#define RPSB_BRS_DONE		8
#define RPSF_BRS_DONE		MKFLAG(RPSB_BRS_DONE)
#define RPSB_HPS_LINE_DONE	6
#define RPSF_HPS_LINE_DONE	MKFLAG(RPSB_HPS_LINE_DONE)
#define RPSB_HPS_DONE		5
#define RPSF_HPS_DONE		MKFLAG(RPSB_HPS_DONE)
#define RPSB_VTD3				4
#define RPSF_VTD3				MKFLAG(RPSB_VTD3)
#define RPSB_VTD2				3
#define RPSF_VTD2				MKFLAG(RPSB_VTD2)
#define RPSB_VTD1				2
#define RPSF_VTD1				MKFLAG(RPSB_VTD1)
#define RPSB_DEBID			1
#define RPSF_DEBID			MKFLAG(RPSB_DEBID)
#define RPSB_IICD				0
#define RPSF_IICD				MKFLAG(RPSB_IICD)

// UPLOAD flags
#define RPSB_UPLD_D1_A			10
#define RPSF_UPLD_D1_A			MKFLAG(RPSB_UPLD_D1_A)
#define RPSB_UPLD_D1_B			9
#define RPSF_UPLD_D1_B			MKFLAG(RPSB_UPLD_D1_B)
#define RPSB_UPLD_BRS			8
#define RPSF_UPLD_BRS			MKFLAG(RPSB_UPLD_BRS)
#define RPSB_UPLD_HPS_CTRL		6
#define RPSF_UPLD_HPS_CTRL		MKFLAG(RPSB_UPLD_HPS_CTRL)
#define RPSB_UPLD_HPS_SCALE	5
#define RPSF_UPLD_HPS_SCALE	MKFLAG(RPSB_UPLD_HPS_SCALE)
#define RPSB_UPLD_DMA3			4
#define RPSF_UPLD_DMA3			MKFLAG(RPSB_UPLD_DMA3)
#define RPSB_UPLD_DMA2			3
#define RPSF_UPLD_DMA2			MKFLAG(RPSB_UPLD_DMA2)
#define RPSB_UPLD_DMA1			2
#define RPSF_UPLD_DMA1			MKFLAG(RPSB_UPLD_DMA1)
#define RPSB_UPLD_DEBI			1
#define RPSF_UPLD_DEBI			MKFLAG(RPSB_UPLD_DEBI)
#define RPSB_UPLD_IIC			0
#define RPSF_UPLD_IIC			MKFLAG(RPSB_UPLD_IIC)



class RPSProgram
	{
	friend class SAA7146;

	private:
		ContinuousDMABuffer *buffer;
		DWORD __huge *dataMem;
		DWORD __huge *codeMem;

		WORD dataSize;
		DWORD codeSize;
		DWORD codeEnd;
		BOOL valid;

		WORD nLabels;
		DWORD __far *labels;

		WORD nLabeledJumps, lastLabeled;
		DWORD __far *labeledJumps;

		Error AppendFix (DWORD __far *com, BOOL patching);

	public:
		// dataSize and codeSize are measured in DWORDs.
		RPSProgram (WORD dataSize, DWORD codeSize, WORD nLabels, WORD nLabeledJumps);
		virtual ~RPSProgram (void);

		Error __cdecl Build (DWORD commands, ...);
		Error __cdecl Append (DWORD commands, ...);
		Error __cdecl Patch (WORD labelNumber, DWORD commands, ...);
		Error CalculateJumps (void);

		Error SetLabel (WORD labelNumber);
		Error GetLabelAddress (WORD labelNumber, DWORD __far &physical);
		Error GetLabelPtr (WORD labelNumber, FPTR __far &fptr);

		Error ReadValue (WORD at, DWORD __far &data) {data = dataMem[at]; GNRAISE_OK;}
		Error WriteValue (WORD at, DWORD data) {dataMem[at] = data; GNRAISE_OK;}
		Error GetDataAddress (WORD at, DWORD __far &physical);

		Error GetPhysicalCodeAddress (DWORD __far &physical);
	};



#endif
