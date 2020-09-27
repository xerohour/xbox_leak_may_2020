// FILE:			library\hardware\audio\specific\pcm1716.h
// AUTHOR:		S. Herr
// COPYRIGHT:	(c) 1999 VIONA Development GmbH & Co. KG.  All Rights Reserved.
// CREATED:		08.09.1999
//
// PURPOSE:		
//
// HISTORY: 

#ifndef PCM1716_H
#define PCM1716_H

#include "..\generic\audiodac.h"
#include "..\..\..\lowlevel\hardwrio.h"
#include "pcm17xx.h"

class PCM1716 : public PCM17XX
	{
	private:
	protected:
		Error SetInputFormat(DWORD format);
		Error SetOutputFormat(DWORD format);

	public:
		PCM1716(BitOutputPort * md,
		        BitOutputPort * mc,
		        BitOutputPort * ml);
		        
		Error Initialize(void);
	};

typedef VirtualPCM17XX VirtualPCM1716;

#endif
