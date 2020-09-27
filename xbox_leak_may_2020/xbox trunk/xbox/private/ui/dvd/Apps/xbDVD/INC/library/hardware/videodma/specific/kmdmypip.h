// FILE:      library\hardware\videodma\specific\kmdmypip.h
// AUTHOR:    Viona
// COPYRIGHT: (c) 1998 Viona Development.  All Rights Reserved.
// CREATED:   27.11.98
//
// PURPOSE:   
//
// HISTORY:
#ifndef KMDMYPIP_H
#define KMDMYPIP_H

#include <library\hardware\videodma\generic\krnvppip.h>


class KernelDummyPIP : public KernelVideoPortPIP
	{	

	protected:
		Error StartVideoDisplay(void);
		Error StopVideoDisplay(void);

	public:
		KernelDummyPIP(void) : KernelVideoPortPIP(FALSE,VP_FEAT_DUMMY,VP_PORT_DUMMY,NULL) {;}
		~KernelDummyPIP(void);	
	};
	

#endif
