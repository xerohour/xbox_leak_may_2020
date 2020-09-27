//
// FILE:       library\hardware\mpeg2enc\generic\meminfo.h
// AUTHOR:		Martin Stephan
// COPYRIGHT:	(c) 1999 Viona Development GmbH.  All Rights Reserved.
// CREATED:		15.11.1999
//
// PURPOSE: 	encoder memory info prepare class - interface
//
// HISTORY:
//

#ifndef MEMINFO_H
#define MEMINFO_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\lowlevel\kernlmem.h"

class EncoderMemoryInfo
	{
	public:				
		KernelMemory *kernelMem;
		//FragmentTableClass *frag;

		EncoderMemoryInfo(void);
		~EncoderMemoryInfo(void);

		Error PrepareMemory (FPTR buffer, DWORD size);
		Error AllocMemory (DWORD size, FPTR __far &data);
		Error FreeMemory (void);

		FPTR KernelSpace(void) {return kernelSpace;}

	protected:
		FPTR userSpace, kernelSpace;
	};

#endif // MEMINFO_H
