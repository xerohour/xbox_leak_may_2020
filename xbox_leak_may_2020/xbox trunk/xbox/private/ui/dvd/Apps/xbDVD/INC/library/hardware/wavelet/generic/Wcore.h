
// FILE:      library\hardware\wavelet\generic\wcore.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   07.05.96
//
// PURPOSE: The virtualized Wavelet core.
//
// HISTORY:

#ifndef WAVELETCORE_H
#define WAVELETCORE_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\common\tags.h"
#include "library\common\hooks.h"
#include "library\common\virtunit.h"
#include "library\hardware\jpeg\generic\jpegdefs.h"



#ifndef ONLY_EXTERNAL_VISIBLE



class WaveletManager : public PhysicalUnit
	{
	public:
		virtual void Start (void) = 0;
		virtual void Stop  (void) = 0;
	};



class VirtualWaveletUnit : public VirtualUnit
	{
	public:
		VirtualWaveletUnit (WaveletManager *physical);
		virtual ~VirtualWaveletUnit (void);

		virtual Error Configure (TAG __far *tags);

		virtual Error PrepareBuffer (FPTR buffer, DWORD size, FPTR __far &info) = 0;
		virtual Error UnprepareBuffer (FPTR info) = 0;

		virtual Error ExpandToRGB (FPTR src, DWORD size, FPTR dst, WORD dstWidth, WORD dstHeight, GrabFormat bitFormat) = 0;
		virtual BOOL IsValidExpansionScaling (WORD srcWidth, WORD srcHeight, WORD dstWidth, WORD dstHeight, GrabFormat bitFormat) = 0;

		virtual Error CompressFromRGB (FPTR src, FPTR dst, DWORD maxSize, DWORD __far &size, BOOL bits32) = 0;
		virtual BOOL IsValidCompression (WORD width, WORD height, BOOL bits32) = 0;

		// These are allowed only after locking and activating the unit.
		virtual Error InitCapture (JPEGCaptureHook __far *hook) = 0;
		virtual Error FinishCapture (void) = 0;
		virtual Error AddVideoBuffer (FPTR info, FPTR userData, DWORD size) = 0;
		virtual Error Start (void);
		virtual Error Stop  (void);
		virtual Error AllocateVideoBuffer (DWORD size, FPTR __far &info, FPTR __far &data) = 0;
		virtual Error FreeVideoBuffer (FPTR info) = 0;
		virtual Error InitPlayback (JPEGPlaybackHook __far *hook) = 0;
		virtual Error FinishPlayback (void) = 0;
		virtual Error FlushPlayback (DWORD bufferSize) = 0;
		virtual Error PlayBuffer (FPTR info, DWORD size) = 0;

	protected:
		virtual Error Preempt (VirtualUnit *previous) = 0;

		BOOL changed;      // TRUE if Configure() changed a variable
		BOOL compress;
		BOOL stillMode;
		int top, left;
		WORD width, height;
	};



#endif // ONLY_EXTERNAL_VISIBLE



#endif
