// FILE:      library\hardware\videodma\generic\cm2vppip.h
// AUTHOR:    S. Herr
// COPYRIGHT: (c) 1997 Viona Development GmbH. All Rights Reserved.
// CREATED:   05.11.1997
//
// PURPOSE:   Cinemaster 2 specific Kernel Mode Video Port PIP class
//
// HISTORY:

#ifndef CM2VPPIP_H
#define CM2VPPIP_H

#include "..\generic\krnvppip.h"
#include "library\lowlevel\hardwrio.h"

class Cinemaster2KernelVideoPortPIP : public KernelVideoPortPIP
	{
	private:
		 BitOutputPort				*vbmsel0Port;
		 BitOutputPort				*vbmsel1Port;
		 BitIndexedOutputPort	*vidSyncPolPort;
		 BitIndexedOutputPort	*horizSyncPort;
		 BitOutputPort				*vpTimingPort;

	protected:
		Error StartVideoDisplay(void);
		Error StopVideoDisplay(void);

	public:
		Cinemaster2KernelVideoPortPIP(BOOL							vpAlwaysEnabled,
												DWORD							availConnFeatures,
												DWORD							availProtocols,
												BitOutputPort				*vbmsel0Port,
												BitOutputPort				*vbmsel1Port,
												BitIndexedOutputPort		*vidSyncPolPort,
												BitIndexedOutputPort		*horizSyncPort,
												BitOutputPort				*vpTimingPort);

		~Cinemaster2KernelVideoPortPIP();

		virtual Error Initialize();
	};

#endif


