
// FILE:			library\hardware\pcibridge\specific\saa7146\saa7146.h
// AUTHOR:		D. Heidrich, S. Herr
// COPYRIGHT:	(c) 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		02.10.96
//
// PURPOSE:		Philips SAA 7146 management class.
//
// HISTORY:

#ifndef SAA7146_H
#define SAA7146_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\lowlevel\memmpdio.h"
#include "library\lowlevel\i2c.h"  
#include "library\lowlevel\intrctrl.h"
#include "library\lowlevel\dmachanl.h"

#include "saa7146r.h"
#include "rps7146.h"


// Error definitions.

#define	GNR_ILLEGAL_DMA_BUFFER_SIZE	MKERR(ERROR, DMA, PARAMS, 0x010)
	// The size of the dma buffer was not in the supported range

#define GNR_RPS_IN_USE						MKERR(ERROR, PCICONTROLLER, GENERAL, 0x01)
	// An RPS program was already installed


#ifndef ONLY_EXTERNAL_VISIBLE



class SAA7146IntServer : public InterruptServer
	{
	private:
		int intCtrlBit;   // bit in the interrupt control registers
		class SAA7146 *saa7146;

	protected:
		Error EnableInt (void);
		Error DisableInt (void);

	public:
		SAA7146IntServer (SAA7146 *saa7146, int intCtrlBit) {this->saa7146 = saa7146; this->intCtrlBit = intCtrlBit;}
	};



class SAA7146PageTable : public ContinuousDMABuffer
	{
	private:
		ScatteredDMABuffer *scatter;
		FPTR lockedBuffer;
		BOOL allocated;
		DWORD bufferStartOffset;
		DWORD alignedPhysical;
		FPTR alignedLogical;

	public:
		SAA7146PageTable (void);
		~SAA7146PageTable (void);

		Error CreateBufferTable (FPTR buffer, DWORD size);
		Error CreateBufferTableDownRound (FPTR buffer, DWORD size, BOOL wrapEnd);
		Error DeleteBufferTable (void);

		DWORD GetPageTablePhysical (void) {return alignedPhysical;}
		FPTR GetPageTableLogical (void) {return alignedLogical;}

		// This offset is needed as logical base address if the MMU is used.
		DWORD GetBufferStartOffset (void) {return bufferStartOffset;}
	};



// Special mixed indexed Byte/Word In/Out port.

class ByteWordIndexedInOutPort : public ByteIndexedInOutPort, public WordIndexedInOutPort
	{ };



class DEBIQueue;


class SAA7146
	{
	friend class SAA7146IntServer;

	public:
		// Possible RPS timeouts
		enum TimeOutSelect {TO_NONE, TO_PCI, TO_VSYNC, TO_BOTH};

		// RPS task number.
		enum RPSTaskNumber {RPSTASK_0=0, RPSTASK_1=1};

		// Video port selection.
		enum VideoPortSelection {VP_PORT_A, VP_PORT_B};

	private:
		RPSProgram *program[2];

	protected:
		DWORD intEnable;
		DWORD intStatus;

		DWORD initDD1, dataPort;

	public:
		MemoryMappedIO *mem;
		ByteInOutBus *i2c;

		CircularDMAChannel *audioInputDMA;

		// General IO pins
		BitIndexedInOutPort *genPPort;

		// One IRQ server for each IRQ
		InterruptServer *irqServers[32];

		DEBIQueue *debiQueue;

		SAA7146 (MemoryMappedIO *mem);
		~SAA7146(void);

		// Initialize the 7146 and create all the ports.
		virtual Error Initialize (void);

		virtual Error ResetDEBI (void);

		// Configure the general IO pins for input or output and their IRQ function.
		// "monitorBit" valid only for GPIO_MONITOR mode.
		virtual Error ConfigureGPIO (int gpio, GPIOMode7146 mode, int monitorBit);

		// Configure I²C port.
		virtual Error ConfigureI2C (IICClockRate7146 clockRate);

		// Configure IRQs (i.e. for which IRQs to create interrupt servers).
		virtual Error ConfigureIRQs (DWORD mask);   // mask layout like in interrupt registers

		// Configure video port A or B.
		virtual Error ConfigureVideoPort (VideoPortSelection port, VideoPortData7146 dataDirection,
		                                  BOOL outputSAVandEAV, BOOL llcOutputMode, SyncIO7146 syncSel,
		                                  BOOL invertVSPol, BOOL invertHSPol, SyncDetect7146 edgeSel,
		                                  FieldInterrupt7146 fieldInt, BOOL invertFieldDetect);
		virtual Error ConfigureVideoPort (VideoPortSelection port, VideoPortData7146 dataDirection,
		                                  BOOL outputSAVandEAV, BOOL invertFieldDetect);
		BurstLength7146 Burst2Register (int burst);
		FifoThreshold7146 VThreshold2Register (int threshold);

		// Create a new DEBI port.
		virtual Error CreateDEBIPort (WORD slaveBaseAddr,
		                              BOOL slave16,			// 16 bit transfer support
		                              int timeOutClks,		// -1 signals that timer should not be used
		                              ByteSwap7146 swap,		// swapping order of bytes
		                              DEBIBusStyle7146 busStyle,	// bus style (Intel or Motorola)
		                              BOOL asynch,			// port operates in parallel; this might fail
		                              ByteWordIndexedInOutPort * __far &port);		// pointer to new port

		// Create a new DEBI DMA channel.
		virtual Error CreateDEBIDMA (WORD addr, BOOL slave16, int timeOutClks, ByteSwap7146 swap,
		                             DEBIBusStyle7146 busStyle, DMAChannel * __far &channel);		// pointer to new DMA channel


		// Installation of RPS program.
		virtual Error InstallRPSProgram (RPSProgram *program, RPSTaskNumber task, TimeOutSelect toSel,
		                                 VideoPortSelection videoPort, DWORD videoTimeOut, DWORD pciTimeOut);
		virtual Error RemoveRPSProgram (RPSTaskNumber task);
		virtual BOOL RPSIsInstalled (RPSTaskNumber task) {return program[task] != NULL;}


		// To be called in the ISR
		virtual void Interrupt (void);
		BOOL CheckInterrupt (void);
	};



#endif // ONLY_EXTERNAL_VISIBLE

#endif 
