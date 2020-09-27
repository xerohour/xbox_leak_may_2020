// FILE:			library\hardware\pcibridge\specific\saa7145\rps7145.h
// AUTHOR:		U. Sigmund
// COPYRIGHT:	(c) 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		19.03.96
//
// PURPOSE:		Class for Register Programming Sequencer (RPS) programs.
//
// HISTORY:

#ifndef RPS7145_H
#define RPS7145_H

#include "..\common\prelude.h"
#include "..\common\gnerrors.h"
#include "..\lowlevel\dmabuffr.h"

#define RPS_LDREG(reg, val)		0x50000100UL | (DWORD)(reg), (DWORD)(val)
#define RPS_STREG(reg, addr)		0x60000100UL | (DWORD)(reg), (DWORD)(addr)
#define RPS_STOP						0x30000000UL
#define RPS_JUMP(flags, dest)		0x40000000UL | (DWORD)(flags), (DWORD)(dest)
#define RPS_IRQ						0x20000000UL
#define RPS_NOP						0x00000000UL
#define RPS_WAIT(flags)				0x90000000UL | (DWORD)(flags)
#define RPS_CHECK(flags)			0x80000000UL | (DWORD)(flags)
#define RPS_PAUSE(flags)			0xa0000000UL | (DWORD)(flags)
#define RPS_SET(flags)				0x10000000UL | (DWORD)(flags)
#define RPS_CLR(flags)				0x00000000UL | (DWORD)(flags)
#define RPS_DONE						0xffffffffUL

#define RPSB_ANY		15
#define RPSF_ANY		(1<<RPSB_ANY)
#define RPSB_INV		14
#define RPSF_INV		(1<<RPSB_INV)
#define RPSB_RPS_S	13
#define RPSF_RPS_S	(1<<RPSB_RPS_S)
#define RPSB_VFE		12
#define RPSF_VFE		(1<<RPSB_VFE)
#define RPSB_GPIO3	11
#define RPSF_GPIO3	(1<<RPSB_GPIO3)
#define RPSB_GPIO2	10
#define RPSF_GPIO2	(1<<RPSB_GPIO2)
#define RPSB_GPIO1	9
#define RPSF_GPIO1	(1<<RPSB_GPIO1)
#define RPSB_GPIO0	8
#define RPSF_GPIO0	(1<<RPSB_GPIO0)
#define RPSB_EAW		7
#define RPSF_EAW		(1<<RPSB_EAW)
#define RPSB_EOL		6
#define RPSF_EOL		(1<<RPSB_EOL)
#define RPSB_HS		5
#define RPSF_HS		(1<<RPSB_HS)
#define RPSB_HT		4
#define RPSF_HT		(1<<RPSB_HT)
#define RPSB_ODD		3
#define RPSF_ODD		(1<<RPSB_ODD)
#define RPSB_EVEN		2
#define RPSF_EVEN		(1<<RPSB_EVEN)
#define RPSB_DEBID	1
#define RPSF_DEBID	(1<<RPSB_DEBID)
#define RPSB_I2CD		0
#define RPSF_I2CD		(1<<RPSB_I2CD)



class RPSProgram
	{
	friend class SAA7145;

	private:
		ContinuousDMABuffer *buffer;
		DWORD __far *dataMem;
		DWORD __far *codeMem;

		WORD dataSize;
		WORD codeSize;
		WORD codeEnd;
		BOOL valid;

		WORD nLabels;
		WORD __far *labels;

		WORD nLabeledJumps, lastLabeled;
		WORD __far *labeledJumps;

		Error AppendFix (DWORD __far *com, BOOL patching);

	public:
		RPSProgram (WORD dataSize, WORD codeSize, WORD nLabels, WORD nLabeledJumps);
		virtual ~RPSProgram (void);

		Error __cdecl Build (DWORD commands, ...);
		Error __cdecl Append (DWORD commands, ...);
		Error __cdecl Patch (WORD labelNumber, DWORD commands, ...);
		Error CalculateJumps (void);

		Error SetLabel (WORD labelNumber);
		Error GetLabelAddress (WORD labelNumber, DWORD __far &physical);

		Error ReadValue (WORD at, DWORD __far &data) {data = dataMem[at]; GNRAISE_OK;}
		Error WriteValue (WORD at, DWORD data) {dataMem[at] = data; GNRAISE_OK;}
		Error GetDataAddress (WORD at, DWORD __far &physical);

		Error GetPhysicalCodeAddress (DWORD __far &physical);
	};



#endif
