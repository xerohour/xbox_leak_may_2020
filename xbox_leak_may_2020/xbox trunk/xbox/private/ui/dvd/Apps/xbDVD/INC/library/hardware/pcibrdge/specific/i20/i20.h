// FILE:      library\hardware\pcibridge\specific\i20\i20.h
// AUTHOR:    Viona
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   05.12.95
//
// PURPOSE:   
//
// HISTORY:

#ifndef I20_H
#define I20_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\lowlevel\memmpdio.h"
#include "library\lowlevel\i2c.h"  
#include "library\lowlevel\intrctrl.h"
#include "library\lowlevel\dmachanl.h"
#include "library\common\krnlsync.h"

#include "i20regs.h"

#define	GNR_ILLEGAL_DMA_BUFFER_SIZE	MKERR(ERROR, DMA, PARAMS, 0x010)
	// The size of the dma buffer was not in the supported range

class I20 : public VDSpinLock {
	protected:           
		BitInOutPort			*	sda;
		BitInOutPort			*	scl;
		DWORD							intStatus;        
		int							chipRevision;
	public:
		//
		// Memory area of the ASRs
		//
		MemoryMappedIO			*	mem;
		
		//
		// I2C bus supported by the I20
		//
		I2CBus					*	i2c;
		
		//
		// Guest bus
		//
		ByteInOutBus			*	guestBus;

		ByteIndexedInOutPort * BuildHandshakePort(int guestID, int gpio, BOOL level);
				
		//
		// General IO pins
		//
		BitIndexedInOutPort	*	genPPort;
	
		//
		// Three on chip interrupts
		//		
		InterruptServer		*	guestIRQ0;
		InterruptServer		*	guestIRQ1;
		InterruptServer		*	codRepIRQ;

		//
		// Code DMA channel		
		//
		CircularDMAChannel	*	codeDMA;

		//
		// Shadow registers for ASR read problems
		//		
		DWORD							intShadow;
		DWORD							i2cShadow;
		
		I20(MemoryMappedIO * mem, int chipRevision);
		~I20(void);

		//
		// Initialize the I20
		//		
		virtual Error BuildPorts(void);		

		// Reinitialization function (e.g. after power down)
		virtual Error ReInitialize(void);

		//
		// Configure the general IO pins for input or output
		//
		virtual Error ConfigurePPort(BYTE inputs);          
		//
		// Configure guest timing
		//
		virtual Error ConfigureGuest(int no, int dur, int rec);
		//
		// Configure code dma
		//
		virtual Error ConfigureCodeDMA(int no, int reg, int trshld);
		
		//
		// To be called in the isr
		//
		virtual void Interrupt(void);
		InterruptServeType CheckInterrupt(void);
	};
      
	//
	// Interrupt server for I20 interrupt
	//	
class I20IntServer : public InterruptServer {
	private:
		int		intCtrlBit;	// bit in the interrupt control registers
		I20	*	i20;
	protected:
		Error EnableInt(void);
		Error DisableInt(void);		
	public:
		I20IntServer(I20 * i20, int intCtrlBit) {this->i20 = i20; this->intCtrlBit = intCtrlBit;}
	};                                                                                          

BOOL CheckI20BoardAccess(DWORD physical);

#endif 
      