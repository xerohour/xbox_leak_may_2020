// FILE:			library\hardware\clock\specific\pll1700.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1999 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		29.07.1999
//
// PURPOSE:		BurrBrown PLL1700 Multi-Clock Generator class
//
// HISTORY:

#ifndef PLL1700_H
#define PLL1700_H

#include "..\generic\clocks.h"
#include "..\generic\clckport.h"
#include "library\lowlevel\hardwrio.h"

// Programmable clock generator (output frequency programmable)
class PLL1700ClockGenerator : public ProgrammableClockGenerator {
	private:
		WordOutputPort		*	port;
		WORD						outputEnableMask;
		BOOL						mClockEnable;
		BOOL						invMClockEnable;
		WORD						controlWord;
	protected:
		Error ProgramFrequency(DWORD freq);
	public:
		PLL1700ClockGenerator(WordOutputPort * port, WORD outputEnableMask);

		Error SetMasterClock(BOOL enable, BOOL enableInverse);
		Error Reset();
	};

#endif
