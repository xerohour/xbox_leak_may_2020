
// FILE:      library\hardware\uproc\specific\st55xx\cpu55xx.h
// AUTHOR:    D. Heidrich, S. Herr
// COPYRIGHT: (c) 2000 VIONA Development GmbH & Co. KG.  All Rights Reserved.
// CREATED:   02.05.2000
//
// PURPOSE:   Classes for STi55xx chips.
//
// HISTORY:

#ifndef CPU55XX_H
#define CPU55XX_H

#include "library\lowlevel\hwbusio.h"
#include "library\lowlevel\memmpdio.h"
#include "library\lowlevel\intrctrl.h"


enum STi5505ClockSource   // for CKG_PLL register
	{
	S55CS_PIXCLK		= 0,
	S55CS_POWERDOWN	= 2,
	S55CS_PCMCLK		= 1
	};

enum STi5505Clock
	{
	S55CL_AUDCLK,
	S55CL_AUXCLK,
	S55CL_SMCCLK,
	S55CL_LNKCLK,
	S55CL_MEMCLK
	};



// This is a rather dummy implementation of an Interrupt Server

class Int55xxServer : public InterruptServer
	{
	protected:
		virtual Error EnableInt (void);
		virtual Error DisableInt (void);

	public:
		Int55xxServer (void)
			{
			}
	};

inline Error Int55xxServer::EnableInt (void)
	{
	GNRAISE_OK;
	}

inline Error Int55xxServer::DisableInt (void)
	{
	GNRAISE_OK;
	}


class STi55xx
	{
	public:
		// The following objects are not initialized by construction of an object
		// of this class! They are here to describe the common memory ranges of ST55xx
		// chips.

		AsyncByteInOutBus		*i2c;

		InterruptServer		*mpegVideoIRQ;
		InterruptServer		*mpegAudioIRQ;

		MemoryMappedIO			*dmaCtrl0;
		MemoryMappedIO			*dmaCtrl1;

		MemoryMappedIO		*mpegMem;
		MemoryMappedIO		*spuMem;
		MemoryMappedIO		*encMem;
		MemoryMappedIO		*mpegAudioMem;
		MemoryMappedIO		*framLinkMem;
		MemoryMappedIO		*commMem;
		MemoryMappedIO		*pio0Mem;
		MemoryMappedIO		*pio1Mem;
		MemoryMappedIO		*pio2Mem;
		MemoryMappedIO		*pio3Mem;
		MemoryMappedIO		*pio4Mem;

		STi55xx (void);

		virtual Error Initialize (int i2cTaskPriority, DWORD serialPortNum) = 0;

		virtual void InterruptI2C (void) = 0;

		virtual void InterruptMPEGVideo (void);
		virtual void InterruptMPEGAudio (void);

		virtual BOOL CheckInterrupt (void);
	};


///////////////////////////////////////////////////////////////////////////////
// ST55xx GPIO based bit output ports
///////////////////////////////////////////////////////////////////////////////

class GPIOBitOutputPort : public BitOutputPort
	{
	private:
		MemoryMappedIO *	ioMem;
		int					at;
		DWORD					mask;
		BOOL					invertedLogic;
	public :
		Error OutBit(BIT data);

		GPIOBitOutputPort(MemoryMappedIO * ioMem, int at, BOOL invertedLogic = FALSE);
	};

///////////////////////////////////////////////////////////////////////////////
// ST55xx GPIO based bit input ports
///////////////////////////////////////////////////////////////////////////////

class GPIOBitInputPort : public BitInputPort
	{
	private:
		MemoryMappedIO *	ioMem;
		int					at;
	public :
		Error InBit(BIT __far &data);

		GPIOBitInputPort(MemoryMappedIO * ioMem, int at);
	};

///////////////////////////////////////////////////////////////////////////////
// ST55xx GPIO based bit inout ports (bidirectional)
///////////////////////////////////////////////////////////////////////////////

class GPIOBitInOutPort : public BitInOutPort
	{
	private:
		MemoryMappedIO *	ioMem;
		int					at;
		DWORD					mask;
	public :
		Error OutBit(BIT data);
		Error OutBitStroke(BIT data, DWORD micros);
		Error InBit(BIT __far &data);

		GPIOBitInOutPort(MemoryMappedIO * ioMem, int at);
	};

#endif
