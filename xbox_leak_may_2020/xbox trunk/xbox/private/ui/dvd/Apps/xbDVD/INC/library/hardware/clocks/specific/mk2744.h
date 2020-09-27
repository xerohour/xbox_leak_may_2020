// FILE:			library\hardware\clock\specific\mk2744.h
// AUTHOR:		Viona
// COPYRIGHT:	(c) 1995 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		05.12.95
//
// PURPOSE:		
//
// HISTORY:
#ifndef MK2744_H
#define MK2744_H

#include "library\common\prelude.h"
#include "..\generic\clocks.h"          
#include "library\lowlevel\hardwrio.h"

class ClockGeneratorMK2744 : public ProgrammableClockGenerator
	{
	private:
		BitOutputPort	*	as0;
		BitOutputPort	*	as1;
		BitOutputPort	*	as2;
	protected:
		Error ProgramFrequency(DWORD freq);
	public:
		ClockGeneratorMK2744(BitOutputPort * as0, BitOutputPort * as1, BitOutputPort * as2);				
	};

#endif
