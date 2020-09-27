#ifndef PCM1721_H
#define PCM1721_H


#include "pcm17xx.h"

#include "library\lowlevel\hardwrio.h"
#include "library\hardware\audio\generic\audiodac.h"
#include "library\hardware\clocks\generic\clocks.h"
#include "pcm17xx.h"

//
// Concrete implementation of an audio DAC, Burr Brown 1717
//
class PCM1721 : public PCM17XX, public ProgrammableClockGenerator
	{
	private:
	protected:
		Error SetInputFormat(DWORD format);
		Error SetOutputFormat(DWORD format);

		Error ProgramFrequency(DWORD freq);
	public:
		PCM1721(BitOutputPort * md,
		        BitOutputPort * mc,
		        BitOutputPort * ml);
	};

typedef VirtualPCM17XX VirtualPCM1721;

#endif
