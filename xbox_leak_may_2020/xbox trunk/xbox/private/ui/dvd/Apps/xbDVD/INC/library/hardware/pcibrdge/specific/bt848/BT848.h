// FILE:      library\hardware\pcibridge\specific\bt848\bt848.h
// AUTHOR:    Viona
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   05.12.95
//
// PURPOSE:   
//
// HISTORY:

#ifndef BT848_H
#define BT848_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"

#ifndef ONLY_EXTERNAL_VISIBLE

#include "library\lowlevel\memmpdio.h"
#include "library\lowlevel\i2c.h"  
#include "library\lowlevel\intrctrl.h"
#include "library\lowlevel\dmachanl.h" 

#endif

//#include "vidde848.h"

#ifndef ONLY_EXTERNAL_VISIBLE
#include "bt848reg.h"
#endif

#define	GNR_ILLEGAL_DMA_BUFFER_SIZE	MKERR(ERROR, DMA, PARAMS, 0x010)
	// The size of the dma buffer was not in the supported range

#ifndef ONLY_EXTERNAL_VISIBLE

class BT848 {
	protected:           
		BitInOutPort			*	sda;
		BitInOutPort			*	scl;
		DWORD						intStatus;        
		int						chipRevision;
	public:
		//
		// Memory area of the ASRs
		//
		MemoryMappedIO			*	mem;
		
		//
		// I2C bus supported by the BT848
		//
		I2CBus					*	i2c;
		
		//
		// General IO pins
		//
		BitIndexedInOutPort	*	genpport;
	
		//
		// Forteen on chip interrupts
		//		
      InterruptServer		*	sync_error;	
      InterruptServer		*  opcode_error;
      InterruptServer	   *	pabort;
      InterruptServer		*  data_par_err;
      InterruptServer    	*	parity_error;
      InterruptServer    	*	fifo_resync;
      InterruptServer    	*	fifo_target;
      InterruptServer    	*	fifo_overrun;
      InterruptServer    	*	irq_in_risc; 
      InterruptServer    	*	gpio_int;
      InterruptServer    	*	i2c_write_read;
      InterruptServer    	*  video_input;
      InterruptServer    	*	hlock_change;
      InterruptServer    	*	lum_chro_overfl;
      InterruptServer    	*	hsync;
      InterruptServer    	*	vsync;
      InterruptServer    	*	videoformat_chg;
        
		//
		// Shadow registers for ASR read problems
		//		
		DWORD							intShadow;
		DWORD							i2cShadow;
		
		BT848(MemoryMappedIO * mem, int chipRevision);
		~BT848(void);

		//
		// Initialize the Bt848
		//		
		virtual Error BuildPorts(void);		
		//
		// Configure the general IO pins for input or output
		//
//		virtual Error ConfigurePPort(DWORD inputs);          
		//
		//
		// To be called in the isr
		//
		virtual void Interrupt(void);
		BOOL CheckInterrupt(void);
	};
      
	//
	// Interrupt server for BT848 interrupt
	//	
class BT848IntServer : public InterruptServer {
	private:
		int		intCtrlBit;	// bit in the interrupt control registers
		BT848	*	bt848;
	protected:
		Error EnableInt(void);
		Error DisableInt(void);		
	public:
		BT848IntServer(BT848 * bt848, int intCtrlBit) {this->bt848 = bt848; this->intCtrlBit = intCtrlBit;}
	};                                                                                          

#endif

#endif 
      