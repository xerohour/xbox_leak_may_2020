// FILE:			library\hardware\pcibridge\specific\saa7145\saa7145.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		19.03.96
//
// PURPOSE:		Philips SAA 7145 management class.
//
// HISTORY:

#ifndef SAA7145_H
#define SAA7145_H

#include "..\common\prelude.h"
#include "..\common\gnerrors.h"
#include "..\lowlevel\memmpdio.h"
#include "..\lowlevel\i2c.h"  
#include "..\lowlevel\intrctrl.h"
#include "..\lowlevel\dmachanl.h"

#include "saa7145r.h"
#include "rps7145.h"

///////////////////////////////////////////////////////////////////////////////
// Error definitions.
///////////////////////////////////////////////////////////////////////////////

#define	GNR_ILLEGAL_DMA_BUFFER_SIZE	MKERR(ERROR, DMA, PARAMS, 0x010)
	// The size of the dma buffer was not in the supported range

#define GNR_RPS_IN_USE						MKERR(ERROR, PCICONTROLLER, GENERAL, 0x01)
	// An RPS program was already installed.


#ifndef ONLY_EXTERNAL_VISIBLE

///////////////////////////////////////////////////////////////////////////////
// Interrupt server for 7145 interrupts
///////////////////////////////////////////////////////////////////////////////

class SAA7145IntServer : public InterruptServer
	{
	private:
		int		intCtrlBit;	// bit in the interrupt control registers
		class SAA7145	*	saa7145;
	protected:
		Error EnableInt(void);
		Error DisableInt(void);
	public:
		SAA7145IntServer(SAA7145 * saa7145, int intCtrlBit) {this->saa7145 = saa7145; this->intCtrlBit = intCtrlBit;}
	};                                                                                          

///////////////////////////////////////////////////////////////////////////////
// Special mixed indexed Byte/Word In/Out Port
///////////////////////////////////////////////////////////////////////////////

class ByteWordIndexedInOutPort : public ByteIndexedInOutPort, public WordIndexedInOutPort
	{ };


///////////////////////////////////////////////////////////////////////////////
// The SAA 7145 management class.
///////////////////////////////////////////////////////////////////////////////

class DEBIQueue;

class SAA7145
	{
	friend class SAA7145IntServer;

	public:
		// Endian swapping control
		enum EndianSwap {ES_NO_SWAP, ES_TWO_BYTE_SWAP, ES_FOUR_BYTE_SWAP, ES_RESERVED};
	
		// Bus interface style for DEBI port.
		enum BusStyle {BS_INTEL, BS_MOTOROLA};
	
		// Possible I2C clock rates (PCI clock divisors).
		enum I2CClockRate {I2C_CLK_120, I2C_CLK_3200, I2C_CLK_80, I2C_CLK_RESERVED1,
		                   I2C_CLK_480, I2C_CLK_6400, I2C_CLK_320, I2C_CLK_RESERVED2};
	
		// Possible RPS timeouts.
		enum TimeOutSelect {TO_NONE, TO_PCI, TO_VSYNC, TO_BOTH};

		// General IO pin input/output mode.
		enum GIOPinMode {GIO_INPUT=0, GIO_OUTPUT=1};

	private:
		RPSProgram 				*	program;

	protected:           
		DWORD intEnable;
		DWORD intStatus;

	public:
		// Memory area of the register set
		MemoryMappedIO			*	mem;

		// I2C bus supported by the 7145
		ByteInOutBus			*	i2c;
		
		// Audio Input Port DMA
		CircularDMAChannel	*	audioInputDMA;

		// General IO pins
		BitIndexedInOutPort	*	genPPort;

		// One IRQ server for every IRQ
		InterruptServer*	irqServers[32];

		DEBIQueue				*	debiQueue;

		SAA7145(MemoryMappedIO * mem);
		~SAA7145(void);

		// Initialize the 7145 and create all the ports.
		virtual Error Initialize(void);

		virtual Error ResetDEBI (void);

		// Configure the general IO pins for input or output and their IRQ function
		// N.B.: "mode" defines transition edge if monitor = FALSE, else it is the IRQ source.
		virtual Error ConfigurePPort(int at, GIOPinMode output, BOOL monitor, int mode);

		// Configure I²C port.
		virtual Error ConfigureI2C(I2CClockRate clockRate);

		// Configure IRQs (i.e. for which IRQs to create interrupt servers).
		virtual Error ConfigureIRQs(DWORD mask);										// Mask is built like in the IMR register.

		// Create a new DEBI port.
		virtual Error CreateDEBIPort(WORD			addr,				// slave base address
											  BOOL			xirqWait,
											  BIT				xirqPol,
											  BOOL 			smart,			// smart or dumb slave
											  BOOL			slave16,			// 16 bit transfer support
											  int 			timeOutClks,	// -1 signals that timer should not be used
											  EndianSwap	swap,				// swapping order of bytes
											  BusStyle		busStyle,		// bus style (Intel or Motorola)
											  BOOL			asynch,			// port operates in parallel, not neccessarily
											                              // successfull
											  ByteWordIndexedInOutPort * __far &port);		// pointer to new port

		// Create a new DEBI DMA channel.
		virtual Error CreateDEBIDMA(WORD				addr,
											 BOOL				xirqWait,
											 BIT				xirqPol,
											 BOOL 			smart,
											 BOOL				slave16,
											 int 				timeOutClks,
											 EndianSwap 	swap,
											 BusStyle		busStyle,
											 DMAChannel		*	__far &channel);		// pointer to new DMA channel


		// Installation of a RPS program.
		virtual Error InstallRPSProgram(RPSProgram * program, TimeOutSelect toSel, DWORD videoTimeOut, DWORD pciTimeOut);

		// Removing of current RPS program (if there is any).
		virtual Error RemoveRPSProgram();


		// To be called in the ISR
		virtual void Interrupt(void);
		BOOL CheckInterrupt(void);
	};


#endif	// of ONLY_EXTERNAL_VISIBLE

#endif 

      