
// FILE:      library\hardware\pcibrdge\specific\amcc\s5933.h
// AUTHOR:    D. Heidrich
// COPYRIGHT: (c) 1996 VIONA Development GmbH. All Rights Reserved.
// CREATED:   29.01.96
//
// PURPOSE:   Abstraction for AMCC S5933 PCI controller.
//
// HISTORY:

#ifndef S5933_H
#define S5933_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\lowlevel\intrctrl.h"
#include "library\lowlevel\dmachanl.h"



class S5933;



class S5933DMAChannel : public DMAChannel
	{
	friend class S5933;

	protected:
		S5933 *amcc;
		BOOL writeChannel;
		DWORD physicalAddress;
		DWORD start, transferSize;

		virtual Error InitTransfer (DMABuffer *buffer, DWORD start, DWORD size);
		void FinishTransfer (void);

		virtual Error StartTransfer (void);
		virtual Error StopTransfer (void);

		virtual Error EndTransfer (void);

	public:
		S5933DMAChannel (S5933 *amcc, BOOL writeChannel);
		virtual ~S5933DMAChannel (void);

		virtual long GetTransferLocation (void);
		virtual void SetTransferLocation (long pos);
	};



// Interrupt server for AMCC interrupts.

class S5933IntServer : public InterruptServer
	{
	private:
		BYTE enableMask;   // bit in the interrupt control register
		BYTE checkMask;
		S5933 *amcc;

	protected:
		Error EnableInt (void);
		Error DisableInt (void);

	public:
		S5933IntServer (S5933 *amcc, BYTE enableMask, BYTE checkMask)
			{this->amcc = amcc; this->enableMask = enableMask; this->checkMask = checkMask;}

		Error SwitchOn (void) {return EnableInt ();}
		Error SwitchOff (void) {return DisableInt ();}

		BOOL IsActive (BYTE intReg) {return (intReg & checkMask) != 0;}
	};



class S5933
	{
	friend class S5933IntServer;
	friend class S5933DMAChannel;

	protected:
		WORD base;
		WORD cntBase;
		BOOL writeBurst, readBurst;

		BYTE intReg;
		BOOL readTransferRunning, writeTransferRunning;

		S5933IntServer *dmaReadIRQ;
		S5933IntServer *dmaWriteIRQ;

		Error StartWriteDMA (DWORD physicalAddress, DWORD size);
		Error StartReadDMA (DWORD physicalAddress, DWORD size);

		Error FinishWriteDMA (void);
		Error FinishReadDMA (void);

	public:
		S5933IntServer *jpegIRQ;

		S5933DMAChannel *dmaReadChannel;
		S5933DMAChannel *dmaWriteChannel;

		S5933 (WORD basePort, WORD counterBasePort, BOOL writeDMABurst, BOOL readDMABurst)
			{
			base = basePort;
			cntBase = counterBasePort;
			writeBurst = writeDMABurst;
			readBurst  = readDMABurst;
			writeTransferRunning = FALSE;
			readTransferRunning = FALSE;
			}

		virtual Error Build (void);

		BOOL CheckInterrupt (void);
		virtual void Interrupt (void);   // called in ISR

		Error WriteData (FPTR data, DWORD size, WORD counterOffset);
		Error ReadData (FPTR data, DWORD size, WORD counterOffset);   // size must be multiple of four
	};



#endif
