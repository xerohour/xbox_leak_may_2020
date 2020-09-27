
// FILE:			library\hardware\clock\specific\h22clock.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		17.01.95
//
// PURPOSE:		H22 specific audio clock generator
//
// HISTORY:

#ifndef H22CLOCK_H
#define H22CLOCK_H

#include "library\hardware\clocks\generic\clocks.h"
#include "library\lowlevel\hardwrio.h"

// Definition of possible base frequencies (for SetBaseFrequency())

#define BASE_FREQ_14_75	14750000
#define BASE_FREQ_12_27 12270000
#define BASE_FREQ_13_50 13500000

// Definition of possible destination frequencies (for SetFrequency())

#define DEST_FREQ_16_9344 16934400
#define DEST_FREQ_24_576  24576000


#ifndef ONLY_EXTERNAL_VISIBLE	


///////////////////////////////////////////////////////////////////////////////
// Audio clock generator
///////////////////////////////////////////////////////////////////////////////

enum H22ClockBaseFrequencies {IDX_BASE_FREQ_14_75,   IDX_BASE_FREQ_12_27, IDX_BASE_FREQ_13_50};
enum H22ClockDestFrequencies {IDX_DEST_FREQ_16_9344, IDX_DEST_FREQ_24_576};

class H22AudioClockGenerator : public PLLClockGenerator { 
	private:
		DWordIndexedOutputPort	*	dataPort;
		BitIndexedOutputPort		*	selPort;

		H22ClockBaseFrequencies	baseFreqIndex;
		H22ClockDestFrequencies destFreqIndex;
		
		DWORD	baseFrequency;
	protected:
		Error ProgramFrequency(DWORD freq);
		virtual Error WriteFrequency();
	public:
		H22AudioClockGenerator(BitIndexedOutputPort	* selPort);

		virtual Error Reset();

		Error SetBaseFrequency(DWORD freq);
	};


#endif // of ONLY_EXTERNAL_VISIBLE	

#endif
