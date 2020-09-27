
// FILE:      library\hardware\pcibridge\specific\saa7145\scatdma.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   10.05.96
//
// PURPOSE: The scattered DMA classes for the SAA7145.
//
// HISTORY:

#ifndef SCATDMA7145_H
#define SCATDMA7145_H

#include "..\common\prelude.h"
#include "..\lowlevel\dmabuffr.h"
#include "..\i22\fragtab.h"
#include "..\saa7145\saa7145.h"
#include "..\saa7145\rps7145.h"



class ScatteredBuffer7145
	{
	private:
		SAA7145 *saa7145;

		// DEBI port configuration for transfer
		DWORD debiCfg;
		WORD slaveAddress;
		BOOL increment;

		FragmentTableClass *fragTab;
		RPSProgram *program;
		BOOL writeBuffer;
		FPTR bufferPtr;
		DWORD bufferSize;

		HGLOBAL handle;

	public:
		ScatteredBuffer7145 (SAA7145 *saa7145, BOOL writeBuffer, BOOL xirqWait, BIT xirqPol,
		                     int timeOutClks,   // use -1 for no timeout (smart slaves only)
		                     WORD slaveAddress, BOOL smartSlave, BOOL slave16, BOOL increment,
		                     SAA7145::EndianSwap swap, SAA7145::BusStyle busStyle);
		virtual ~ScatteredBuffer7145 (void);

		Error PrepareBuffer (FPTR buffer, DWORD size);
		Error UnprepareBuffer (void);
		HGLOBAL GetHandle (void) {return handle;}

		Error SetBuffer (DWORD size);
		Error GetStartAddress (DWORD __far &start) {return program->GetPhysicalCodeAddress (start);}
		FPTR GetBufferPtr (void) {return bufferPtr;}
	};



#endif
