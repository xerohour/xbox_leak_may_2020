
// FILE:      library\hardware\pcibridge\specific\saa7146\scatdma.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   02.10.96
//
// PURPOSE: The scattered DMA classes for the SAA7146.
//
// HISTORY:

#ifndef SCATDMA7146_H
#define SCATDMA7146_H

#include "library\common\prelude.h"
#include "library\lowlevel\dmabuffr.h"
#include "library\hardware\pcibrdge\specific\saa7146\saa7146.h"
#include "library\hardware\pcibrdge\specific\saa7146\rps7146.h"



class ScatteredBuffer7146
	{
	private:
		SAA7146 *saa7146;

		// DEBI port configuration for transfer.
		DWORD debiCfg;
		WORD slaveAddress;
		BOOL increment;

		SAA7146PageTable *pageTable;
		RPSProgram *program;
		BOOL writeBuffer;
		FPTR bufferPtr;
		DWORD bufferSize;

		HGLOBAL handle;

	public:
		ScatteredBuffer7146 (SAA7146 *saa7146, BOOL writeBuffer,
		                     int timeOutClks,   // use -1 for no timeout
		                     WORD slaveAddress, BOOL slave16, BOOL increment,
		                     ByteSwap7146 swap, DEBIBusStyle7146 busStyle);
		virtual ~ScatteredBuffer7146 (void);

		Error PrepareBuffer (FPTR buffer, DWORD size, BOOL playback);
		Error UnprepareBuffer (void);
		HGLOBAL GetHandle (void) {return handle;}

		struct PlayParametersRPS
			{
			DWORD start;        // physical start of block transfer RPS program
			DWORD mmu1, mmu2;   // physical patch addresses for MMU page table entries
			DWORD returnStart;  // physical start of second block
			DWORD blockSizeRPS;   // byte size of RPS program to transfer one block
			DWORD sizeEntry;    // back offset from return patch address to last size entry
			};

		Error SetBuffer (PlayParametersRPS parameters, DWORD size);
		Error GetStartAddress (DWORD __far &start) {return program->GetPhysicalCodeAddress (start);}
		DWORD GetBufferStartOffset (void) {return pageTable->GetBufferStartOffset ();}
		FPTR GetBufferPtr (void) {return (FPTR)((BYTE __huge *)bufferPtr + GetBufferStartOffset ());}

		Error GetPageTablePhysical (DWORD __far &physical);
	};



#endif
