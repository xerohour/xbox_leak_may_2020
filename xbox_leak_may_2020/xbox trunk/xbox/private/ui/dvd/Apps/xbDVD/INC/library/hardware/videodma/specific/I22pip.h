
// FILE:      library\hardware\videodma\specific\i22pip.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   05.03.96
//
// PURPOSE: The I22 PIP related classes.
//
// HISTORY:

#ifndef I22PIP_H
#define I22PIP_H

#include "i20pip.h"
#include "library\hardware\jpeg\specific\jcod050.h"



#ifndef ONLY_EXTERNAL_VISIBLE



class I22PIP : public I20PIP
	{
	friend class VirtualI22PIP;

	private:
		JCoder050 *jpegCoder;
		int vBlankIntBit;
		BOOL ccirMode;

		ContinuousDMABuffer *bitmapBuffer;
		WORD nStripes, stripeWidth;

	protected:
		Error GrabExpandFrame (FPTR base, WORD sourceWidth, WORD sourceHeight,   // grab a single frame from the running picture
		                       WORD width,  WORD height,
		                       WORD stride, GrabFormat fmt);

	public:
#if NT_KERNEL || VXD_VERSION
		I22PIP (MemoryMappedIO *io, InterruptServer *vblankIntServer,
		        Profile *profile, BOOL ccirMode, BOOL tritonFix, JCoder050 *jpegCoder, int vBlankIntBit,
				  DWORD physicalBase, WORD bytesPerRow, PIPFormat pixelFormat);
#else
		I22PIP (HINSTANCE hinst, MemoryMappedIO *io, InterruptServer *vblankIntServer,
		        Profile *profile, BOOL ccirMode, BOOL tritonFix, JCoder050 *jpegCoder, int vBlankIntBit);
#endif
		~I22PIP (void);

		VirtualUnit *CreateVirtual (void);

		Error Initialize (void);   // Call after the object was constructed
	};



class VirtualI22PIP : public VirtualI20PIP
	{
	friend class I22PIP;

	protected:
		I22PIP *i22pip;

	public:
		VirtualI22PIP (I22PIP *physical);

		Error Configure (TAG __far *tags);
		Error Preempt (VirtualUnit *previous);

		Error VirtualI22PIP::GrabExpandFrame (FPTR base, WORD sourceWidth, WORD sourceHeight, WORD width, WORD height,
		                                      WORD stride, GrabFormat fmt);
	};



#endif // ONLY_EXTERNAL_VISIBLE



#endif
