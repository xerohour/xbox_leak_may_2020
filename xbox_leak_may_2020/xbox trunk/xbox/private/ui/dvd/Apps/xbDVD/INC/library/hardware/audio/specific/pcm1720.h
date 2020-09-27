#ifndef PCM1720_H
#define PCM1720_H

#include "library\lowlevel\hardwrio.h"
#include "library\hardware\audio\generic\audiodac.h"
#include "pcm17xx.h"

//
// Concrete implementation of an audio DAC, Burr Brown 1717
//
class PCM1720 : public PCM17XX
	{
	private:
	protected:
		Error SetInputFormat(DWORD format);
		Error SetOutputFormat(DWORD format);
	public:
		PCM1720(BitOutputPort * md,
		        BitOutputPort * mc,
		        BitOutputPort * ml);
	};

typedef VirtualPCM17XX VirtualPCM1720;

#endif
