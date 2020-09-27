// FILE:			library\hardware\audio\specific\pcm1717.h
// AUTHOR:		Viona
// COPYRIGHT:	(c) 1995, 1996 VIONA Development GmbH.  All Rights Reserved.
// CREATED:		05.12.1996
//
// PURPOSE:		
//
// HISTORY: 
#ifndef PCM1717_H
#define PCM1717_H

#include "..\generic\audiodac.h"
#include "..\..\..\lowlevel\hardwrio.h"
#include "pcm17xx.h"

//
// Concrete implementation of an audio DAC, Burr Brown 1717
//
class PCM1717 : public PCM17XX
	{
	private:
	protected:
		Error SetInputFormat(DWORD format);
		Error SetOutputFormat(DWORD format);

	public:
		PCM1717(BitOutputPort * md,
		        BitOutputPort * mc,
		        BitOutputPort * ml);
		        
		Error Initialize(void);
	};

typedef VirtualPCM17XX VirtualPCM1717;

#endif
