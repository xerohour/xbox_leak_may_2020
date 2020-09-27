// FILE:			library\hardware\clock\generic\clocks.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		15.01.95
//
// PURPOSE:		Base classes for clock generators.
//
// HISTORY:

#ifndef CLOCKS_H
#define CLOCKS_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"

#ifndef ONLY_EXTERNAL_VISIBLE


// Elementary clock generator
class ClockGenerator {
	public:
		virtual Error GetFrequency(DWORD __far & freq) = 0;				// freq unit is Hz
	};
	
// Programmable clock generator (output frequency programmable)
class ProgrammableClockGenerator : public ClockGenerator {
	protected:
		DWORD	desiredFrequency;
		DWORD	actualFrequency;
		
		virtual Error ProgramFrequency(DWORD freq) = 0;
	public:
		ProgrammableClockGenerator() {desiredFrequency = 0; actualFrequency = 0;}
		
		virtual Error GetFrequency(DWORD __far & freq) {freq = actualFrequency; GNRAISE_OK;}
		virtual Error SetFrequency(DWORD freq);
		virtual Error GetError(DWORD __far & error);		// Get error of frequency.
	};

// Programmable clock generator with variable base frequency
class PLLClockGenerator : public ProgrammableClockGenerator {
	public:
		virtual Error SetBaseFrequency(DWORD freq) = 0;
	};

inline Error ProgrammableClockGenerator::GetError(DWORD __far & error)
	{
	long diff;
	
	diff = desiredFrequency - actualFrequency; 
	error = (DWORD) max(diff, -diff);
	
	GNRAISE_OK;	
	}

inline Error ProgrammableClockGenerator::SetFrequency(DWORD freq)
	{
	desiredFrequency = freq;
	GNREASSERT(ProgramFrequency(freq));
	
	GNRAISE_OK;
	}

#endif // of ONLY_EXTERNAL_VISIBLE

#endif

