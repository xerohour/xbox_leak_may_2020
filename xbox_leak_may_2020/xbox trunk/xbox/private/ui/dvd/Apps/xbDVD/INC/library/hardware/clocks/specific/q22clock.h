
// FILE:			library\hardware\clock\specific\h22clock.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1997 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		04.04.97
//
// PURPOSE:		Q22 specific audio clock generator
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

#define DEST_FREQ_16_9344	16934400
#define DEST_FREQ_24_576	24576000
#define DEST_FREQ_25  		25000000


#ifndef ONLY_EXTERNAL_VISIBLE	


///////////////////////////////////////////////////////////////////////////////
// Audio clock generator
///////////////////////////////////////////////////////////////////////////////

enum H22ClockBaseFrequencies {IDX_BASE_FREQ_14_75,   IDX_BASE_FREQ_12_27, IDX_BASE_FREQ_13_50};
enum H22ClockDestFrequencies {IDX_DEST_FREQ_16_9344, IDX_DEST_FREQ_24_576, IDX_DEST_FREQ_25};

class Q22AudioClockGenerator : public ProgrammableClockGenerator { 
	private:
		BitOutputPort				*	opModeSelPort;
		BitOutputPort				*	regSelPort;
		BitOutputPort				*	sel1Port;
		BitOutputPort				*	sel0Port;
		BitInputPort				*	pllErrorPort;

		DWordIndexedOutputPort	*	dataPort;
		
		H22ClockBaseFrequencies	baseFreqIndex[4];
		H22ClockDestFrequencies destFreqIndex;
		
		DWORD	baseFrequency[4];
		
		BOOL	use2061;
		BOOL	mclkRef;
		BOOL	ccir;
	protected:
		Error ProgramFrequency(DWORD freq);
		virtual Error WriteFrequency();
	public:
		Q22AudioClockGenerator(BitOutputPort * opModeSelPort,	// Port to select operation mode
									  BitOutputPort * regSelPort,		// Port to select which register set to use
									  BitOutputPort * sel1Port,		// Port connected to SEL1 of PLL
									  BitOutputPort * sel0Port,		// Port connected to SEL0 of PLL
									  BitInputPort  * pllErrorPort,  // if TRUE then PLL error
									  BOOL				use2061 = FALSE
									  );

		virtual Error Reset();
		
		virtual Error SetBaseFrequency(int index, DWORD freq);
		
		virtual Error SetCCIR(BOOL ccir);
	};


#endif // of ONLY_EXTERNAL_VISIBLE	

#endif
