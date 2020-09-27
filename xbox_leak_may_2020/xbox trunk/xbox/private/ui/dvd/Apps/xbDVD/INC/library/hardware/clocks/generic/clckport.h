// FILE:			library\hardware\clocks\generic\clckport.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1997 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		04.04.97
//
// PURPOSE:		Port for access to clock PLLs
//
// HISTORY:

#ifndef CLCKPORT_H
#define CLCKPORT_H

#include "library\common\prelude.h"
#include "library\lowlevel\hardwrio.h"

#define T_SU		1 // Setup time
#define T_HD		1 // Hold time
#define T_LDCMD	1 // Load command time


class ClockPort : public DWordIndexedOutputPort {
	private:
		BitOutputPort * dataPort;
		BitOutputPort * clkPort;
	protected:
		Error Unlock();
		Error Start();
		Error Stop();
		Error SetData(BIT data) {return dataPort->OutBit(data);}
		Error SetClock(BIT data) {return clkPort->OutBit(data);}
	public:
		ClockPort(BitOutputPort * dataPort, BitOutputPort * clkPort)
			{this->dataPort = dataPort; this->clkPort = clkPort;}
		Error OutDWord(int at, DWORD data);
	};


// MicroWire (BurrBrown) type of Port to clock

class MWClockPort : public WordOutputPort
	{
	private:
		BitOutputPort	*	ml;
		BitOutputPort	*	mc;
		BitOutputPort	*	md;
	public:
		MWClockPort(BitOutputPort * ml, BitOutputPort * mc, BitOutputPort * md);

		Error OutWord(WORD data);
	};

#endif
