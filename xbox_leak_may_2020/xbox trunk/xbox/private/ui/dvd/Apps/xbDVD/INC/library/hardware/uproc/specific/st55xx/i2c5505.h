
// FILE:      library\hardware\uproc\specific\st55xx\i2c5505.h
// AUTHOR:    D. Heidrich
// COPYRIGHT: (c) 1998 Viona Development.  All Rights Reserved.
// CREATED:   22.05.98
//
// PURPOSE:   I2C classes for the STi5505 chip.
//
// HISTORY:

#ifndef I2C5505_H
#define I2C5505_H

#include "library\common\prelude.h"
#include "library\general\lists.h"
#include "library\lowlevel\hwbusio.h"
#include "library\lowlevel\memmpdio.h"



class I2CRequest5505;


class I2CBus5505 : public AsyncByteInOutBus
	{
	friend class I2CRequest5505;

	public:
		// Members of ByteInputBus.
		virtual Error InByte (int unit, int reg, BYTE __far &data);
		virtual Error InByte (int unit, BYTE __far &data);
		virtual Error InBytes (int unit, int reg, int inc, BYTE __far *data, int num);
		virtual Error InBytes (int unit, BYTE __far *data, int num);

		// Members of ByteOutputBus.
		virtual Error OutByte (int unit, int reg, BYTE data);
		virtual Error OutByte (int unit, BYTE data);
		virtual Error OutBytes (int unit, int reg, int inc, BYTE __far *data, int num);
		virtual Error OutBytes (int unit, BYTE __far *data, int num);
		virtual Error OutByteSeqA (int unit, int reg, int inc, int __far *data);
		virtual Error OutByteSeqA (int unit, int __far *data);
		virtual Error OutBytePairsA (int unit, int __far *data);

		// Members of AsyncIODevice.
		virtual Error AllocateRequest (AsyncIORequest __far * __far &request);
		virtual Error FreeRequest (AsyncIORequest __far *request);

		// Members of AsyncByteInputBus.
		virtual Error AsyncInByte (int unit, AsyncIORequest __far *request, int reg, BYTE __far &data);
		virtual Error AsyncInByte (int unit, AsyncIORequest __far *request, BYTE __far &data);
		virtual Error AsyncInBytes (int unit, AsyncIORequest __far *request, int reg, int inc, BYTE __far *data, int num);
		virtual Error AsyncInBytes (int unit, AsyncIORequest __far *request, BYTE __far *data, int num);

		// Members of AsyncByteOutputBus.
		virtual Error AsyncOutByte (int unit, AsyncIORequest __far *request, int reg, BYTE data);
		virtual Error AsyncOutByte (int unit, AsyncIORequest __far *request, BYTE data);
		virtual Error AsyncOutBytes (int unit, AsyncIORequest __far *request, int reg, int inc, BYTE __far *data, int num);
		virtual Error AsyncOutBytes (int unit, AsyncIORequest __far *request, BYTE __far *data, int num);
		virtual Error AsyncOutByteSeqA (int unit, AsyncIORequest __far *request, int reg, int inc, int __far *data);
		virtual Error AsyncOutByteSeqA (int unit, AsyncIORequest __far *request, int __far *data);
		virtual Error AsyncOutBytePairsA (int unit, AsyncIORequest __far *request, int __far *data);

		// New members.
		I2CBus5505 (MemoryMappedIO *mem, MemoryMappedIO *pio);
		~I2CBus5505 (void);

		Error Initialize (int taskPriority);

		void HandlerTask (void);

		void Interrupt (void);   // interrupt function

	private:
		BOOL initialized;
		MemoryMappedIO *mem;
		MemoryMappedIO *pio;

		// This list contains the IO requests. Access is controlled with a mutex.
		I2CRequest5505 volatile *currentRequest;   // is not in the list
		semaphore_t mutex;        // grants exclusive list access
		List ioRequestList;
		semaphore_t newRequest;   // signals handler task there's a new request or a request done

		Error ResetBus (void);
		void SendStart (void);
		void SendStop (void);
	};



#endif
