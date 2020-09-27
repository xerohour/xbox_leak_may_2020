// FILE:      library\hardware\videodma\generic\clcvppip.h
// AUTHOR:    S. Herr
// COPYRIGHT: (c) 1998 Viona Development GmbH. All Rights Reserved.
// CREATED:   08.04.1998
//
// PURPOSE:   Cinemaster C LC specific Kernel Mode Video Port PIP class
//
// HISTORY:

#ifndef CLCVPPIP_H
#define CLCVPPIP_H

#include "..\generic\krnvppip.h"
#include "library\lowlevel\hardwrio.h"

class CinemasterCLCKernelVideoPortPIP : public KernelVideoPortPIP
	{
	private:
		ByteIndexedInOutPort	*	hostPort;
		ByteIndexedInOutPort	*	configPort;

	protected:
		Error StartVideoDisplay(void);
		Error StopVideoDisplay(void);

	public:
		CinemasterCLCKernelVideoPortPIP(	BOOL							vpAlwaysEnabled,
													DWORD							availConnFeatures,
													DWORD							availProtocols,
													ByteIndexedInOutPort	*	hostPort,
													ByteIndexedInOutPort	*	configPort,
													ColorController * colorController = NULL);

		~CinemasterCLCKernelVideoPortPIP(void);

		virtual Error Initialize(void);

		Error	ReInitialize(void);
	};

#endif


