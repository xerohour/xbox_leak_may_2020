
// FILE:      library\hardware\jpeg\generic\jcore.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   15.01.96
//
// PURPOSE: The virtualized JPEG core.
//
// HISTORY:

#ifndef JPEGCORE_H
#define JPEGCORE_H

#include "library\common\prelude.h"
#include "library\common\gnerrors.h"
#include "library\common\tags.h"
#include "library\common\hooks.h"
#include "library\common\virtunit.h"
#include "jpegdefs.h"
#include "jcoder.h"



#ifndef ONLY_EXTERNAL_VISIBLE



class JPEGManager : public PhysicalUnit
	{
	public:
		virtual void Start (void) = 0;
		virtual void Stop  (void) = 0;
	};



class VirtualJPEGUnit : public VirtualUnit
	{
	public:
		VirtualJPEGUnit (JPEGManager *physical, VirtualJCoder *coder);
		virtual ~VirtualJPEGUnit (void);

		virtual Error Configure (TAG __far *tags);

		virtual Error PrepareBuffer (FPTR buffer, DWORD size, FPTR __far &info) = 0;
		virtual Error UnprepareBuffer (FPTR info) = 0;

#if VIONA_VERSION
		virtual Error ExpandToRGB (FPTR src, DWORD size, FPTR dst, WORD dstWidth, WORD dstHeight, GrabFormat bitFormat) = 0;
		virtual BOOL IsValidExpansionScaling (WORD srcWidth, WORD srcHeight, WORD dstWidth, WORD dstHeight, GrabFormat bitFormat) = 0;
		virtual Error CompressFromRGB (FPTR src, FPTR dst, DWORD maxSize, DWORD __far &size, BOOL bits32) = 0;
		virtual BOOL IsValidCompression (WORD width, WORD height, BOOL bits32) = 0;
#else
		virtual Error ExpandToRGB (FPTR src, DWORD size, FPTR dst) = 0;
		virtual Error CompressFromRGB (FPTR src, FPTR dst, DWORD maxSize, DWORD __far &size) = 0;
#endif

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

		VirtualJCoder *coder;

		BOOL changed;      // TRUE if Configure() changed a variable
		BOOL compress;
		BOOL stillMode;
		int top, left;
		WORD width, height;

		BOOL ccirLevels;

#if LOW_PIPELINE

#define NUM_BUFFERS     17
#define MAX_BUFFERS     16   // must be power of two !
#define WANTED_BUFFERS  7

		struct PlayingBuffer
			{
			FPTR data;
			DWORD size;
			LONG sampleNumber;
			};

		struct PlayPipe
			{
			DWORD maxBufferSize;
			DWORD rate, scale;
			PlayingBuffer __far *lastBufferPtr;
			BOOL buffersAllocated;
			BOOL identicalFrameNumberPassed;
			int volatile nBuffers;
			int volatile firstBuffer;
			int volatile lastBuffer;
			FPTR bufferInfo[NUM_BUFFERS];
			FPTR bufferData[NUM_BUFFERS];
			DWORD bufferSize[NUM_BUFFERS];
			PlayingBuffer buffers[NUM_BUFFERS];
			long startTime;
			int initBuffersAdded;
			};
		PlayPipe playPipe;

	public:
		Error AllocPlayPipe (DWORD maxBufferSize);
		Error FreePlayPipe (void);
		Error FlushPlayPipe (void);

		BOOL InsertPlayBuffer (PlayingBuffer __far *buffer);
		void RemoveBuffer (void);
		FPTR GetPlayBufferInfo (void) {return playPipe.bufferInfo[playPipe.firstBuffer];}
		PlayingBuffer __far *GetPlayBuffer (void)
			{
			if (playPipe.nBuffers > 0)
				return &playPipe.buffers[playPipe.firstBuffer];
			else
				return NULL;
			}
#endif // LOW_PIPELINE
	};



#endif // ONLY_EXTERNAL_VISIBLE



#endif
