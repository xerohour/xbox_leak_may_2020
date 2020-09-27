
// FILE:      library\hardware\uproc\specific\st55xx\cpu5505.h
// AUTHOR:    D. Heidrich
// COPYRIGHT: (c) 1998 Viona Development.  All Rights Reserved.
// CREATED:   22.05.98
//
// PURPOSE:   Classes for the STi5505 chip.
//
// HISTORY:

#ifndef CPU5505_H
#define CPU5505_H

#include "cpu55xx.h"
#include "library\general\asncstrm.h"
#include "library\hardware\clocks\generic\clocks.h"


class STi5505ClockGenerator;


class STi5505 : public STi55xx
	{
	friend class STi5505ClockGenerator;
	protected:
		DWORD	fvcoFreq;

		Error ProgramClockGenerator(STi5505Clock clock, BOOL external, DWORD freq);
		Error StopClockGenerator (STi5505Clock clock);

	public:
		STi5505ClockGenerator	*	memClock;
		STi5505ClockGenerator	*	audClock;
		STi5505ClockGenerator	*	auxClock;
		STi5505ClockGenerator	*	lnkClock;

		STi5505 (void);

		virtual Error Initialize (int i2cTaskPriority, DWORD serialPortNum);
		Error InitClockGenerator(STi5505ClockSource source, WORD nominator, WORD denominator, DWORD freq);

		virtual void InterruptI2C (void);
		virtual void InterruptFrontDMA (void);

		BOOL CheckInterrupt (void);
	};



class DMAOutStream5505 : public ASyncOutStream
	{
	private:
		MemoryMappedIO *mem;
		int channelNum;
		DWORD location;
		BOOL transferAllowed;

		DWORD ReadWordHL (WORD offset);

	public:
		DMAOutStream5505 (MemoryMappedIO *mem, int channelNumber)
			{this->mem = mem; channelNum = channelNumber; location = 0; transferAllowed = FALSE;}

      virtual Error Initialize (void);
		virtual Error WriteData (HPTR data, DWORD size, DWORD __far &done);
		virtual DWORD AvailSpace (void);
		virtual DWORD AvailData (void);
		virtual DWORD GetTransferLocation (void);
		virtual void SetTransferLocation (DWORD pos);
		virtual Error StartTransfer (void);
		virtual Error StopTransfer (void);
		virtual Error SyncTransfer (void);
		virtual Error CompleteTransfer(void);
		virtual Error FlushBuffer (void);
	};


///////////////////////////////////////////////////////////////////////////////
// ST5505 Internal Programmable Clock Generator (PLL) class
///////////////////////////////////////////////////////////////////////////////

class STi5505ClockGenerator : public ProgrammableClockGenerator
	{
	friend class STi5505;
	protected:
		STi5505		*	cpu;
		STi5505Clock	clock;
	public:
		STi5505ClockGenerator (STi5505 * cpu, STi5505Clock clock)
			{
			this->cpu = cpu;
			this->clock = clock;
			}

		Error ProgramFrequency (DWORD freq);
		Error StopClock (void) {return cpu->StopClockGenerator (clock);}
	};

#endif
