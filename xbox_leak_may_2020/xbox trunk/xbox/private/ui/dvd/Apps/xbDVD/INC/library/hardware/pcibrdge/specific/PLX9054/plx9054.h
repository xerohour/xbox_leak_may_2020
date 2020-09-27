//
// FILE:       library\hardware\pcibridge\specific\PLX9054\plx9054.h
// AUTHOR:		Martin Stephan
// COPYRIGHT:	(c) 1999 Viona Development GmbH.  All Rights Reserved.
// CREATED:		19.11.1999
//
// PURPOSE: 	PCI bridge class for CineMaster II encoder --- Header file
//
// HISTORY:		
//

#ifndef PLX9054_H
#define PLX9054_H

#include "library\common\gnerrors.h"
#include "library\lowlevel\hwbusio.h"
#include "library\lowlevel\intrctrl.h"
#include "library\common\krnlsync.h"

//
// PLX 9054 PCI bridge class
//
class PLX9054 : public VDSpinLock
	{
	public:
		PLX9054(MemoryMappedIO * plxMem, MemoryMappedIO * localMem, MemoryMappedIO * msgMem, IndexedInOutPort * configPort);
		~PLX9054(void);

		//
		// Initialize the PLX 9054 PCI bridge chip
		//		
		virtual Error BuildPorts(void);

		//
		// To be called in the isr
		//
		virtual void Interrupt(void);				
		InterruptServeType CheckInterrupt(void);

		Error Initialize(void);

		//
		// configure local bus
		//
		virtual Error ConfigureLocal(void);
		
		//
		// Interrupts
		//		
		InterruptServer		*	encoderIRQ;

		//
		// Memory areas
		//
		MemoryMappedIO			*	plxMem;
		MemoryMappedIO			*  localMem;
		MemoryMappedIO			*	controlMem;

		//
		// Configuration space port
		//
		IndexedInOutPort		*	configPort;

		//
		// Local bus
		//
		DWordInOutBus			*	localBus;
		//
		// Control bus
		//
		DWordInOutBus			*	controlBus;

		DWORD						intShadow;
		
	protected:
		DWORD						intStatus;        

	private:

	};


//
// Interrupt server for PLX interrupt
//	
class PLX9054IntServer : public InterruptServer 
	{
	private:
		PLX9054	*	plx9054;

	protected:
		Error EnableInt(void);
		Error DisableInt(void);

	public:
		PLX9054IntServer(PLX9054 * plx9054) : InterruptServer()
			{ this->plx9054 = plx9054; }
	};                                                                                          


#endif // PLX9054_H

