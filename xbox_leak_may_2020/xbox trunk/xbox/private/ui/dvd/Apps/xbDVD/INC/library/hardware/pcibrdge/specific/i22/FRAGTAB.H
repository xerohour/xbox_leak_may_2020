
// FILE:      library\hardware\pcibridge\specific\i22\fragtab.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   12.05.95
//
// PURPOSE: The fragment table class.
//
// HISTORY:

#ifndef FRAGTAB_H
#define FRAGTAB_H

#include "..\..\..\..\common\prelude.h"
#include "..\..\..\..\lowlevel\dmabuffr.h"



class FragmentTableClass : public ContinuousDMABuffer
	{
	private:
		ScatteredDMABuffer *scatter;
		FPTR lockedBuffer;
		BOOL allocated;

	public:
		FragmentTableClass (void);
		~FragmentTableClass (void);

		Error CreateFragmentBuffer (FPTR buffer, DWORD size);
		Error DeleteFragmentBuffer (void);

		Error GetBufferPointer (FPTR __far &buffer, DWORD __far &size)
			{ return scatter->GetBuffer (buffer, size); }

		Error GetFragmentTablePtr (DWORD __far * __far &fragTable)
			{
			FPTR fptr;
			DWORD size;
			Error err = GetBuffer (fptr, size);
			fragTable = (DWORD __far *)fptr;
			return err;
			}

		void GetFragmentInfo (DWORD __far * __far &fragTable, DWORD __far &physical,
		                      DWORD __far &byteSize, BOOL __far &finalFragment)
			{
			physical = *fragTable++;
			byteSize = *fragTable++;
			finalFragment = (byteSize & 1) != 0;
			byteSize = (byteSize << 1) & ~3;
			}
	};



#endif
