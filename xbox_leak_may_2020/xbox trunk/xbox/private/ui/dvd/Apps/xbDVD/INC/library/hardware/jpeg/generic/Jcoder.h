
// FILE:      library\hardware\jpeg\generic\jcoder.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   12.01.96
//
// PURPOSE: JPEG coder class.
//
// HISTORY:

#ifndef JCODER_H
#define JCODER_H

#include "library\common\prelude.h"
#include "library\common\virtunit.h"
#include "library\hardware\video\generic\vidtypes.h"
#include "library\hardware\jpeg\generic\jpegdefs.h"



#ifndef ONLY_EXTERNAL_VISIBLE



class JCoder : public PhysicalUnit
	{
	public:
		// These must be BYTEs, even for Unicode !
		static BYTE DefaultAPPStringSingle[];
		static BYTE DefaultAPPStringOdd[];
		static BYTE DefaultAPPStringEven[];
	};



class VirtualJCoder : public VirtualUnit
	{
	public:
		VirtualJCoder (JCoder *physical);

		virtual Error Configure (TAG __far *tags);

		// These are allowed only after locking and activating the unit.
		virtual Error InitOperation (void) = 0;
		virtual void Start (void) = 0;
		virtual void Stop  (void) = 0;

		Error GetFieldOrder (FPTR field, DWORD size, VideoField __far &fieldOrder);
		Error GetSizes (FPTR frame, DWORD size, WORD __far &width, WORD __far &height);
		Error MarkFirstField (FPTR frame, DWORD size, VideoField fieldOrder);

	protected:
		virtual Error Preempt (VirtualUnit *previous);

		// Find a marker and return its position, zero for not found.
		DWORD FindMarker (FPTR buffer, DWORD bufferSize, BYTE marker);

		BOOL compress;             // compression or expansion ?
		BOOL stillMode;            // still or motion mode ?
		WORD width, height;
		VideoField fieldOrder;
	};



#endif // ONLY_EXTERNAL_VISIBILE



#endif
