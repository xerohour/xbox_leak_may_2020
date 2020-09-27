//depot/xbox-aug01-final/private/ui/dvd/driver/softwarecinemaster/Video/VideoDisplay.h#3 - edit change 19203 (text)
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef VIDEODISPLAY_H
#define VIDEODISPLAY_H

#include "driver\SoftwareCinemaster\Common\Prelude.h"
#include "..\common\streamtiming.h"
#include "..\common\ASyncErrorMessages.h"
#include "FrameStore.h"
#include "..\common\TimedFibers.h"
#include "..\SPUDecoder\SPUDisplayBuffer.h"
#include "MacrovisionEncoder.h"
#undef ONLY_EXTERNAL_VISIBLE //get rid of pesky warnings
#define ONLY_EXTERNAL_VISIBLE
#include "library\hardware\videodma\generic\pip.h"

	//
	// State of a PictureDisplayBuffer.  Maintained for client, not
	// used by GenericPictureDisplay.
	//
enum PictureDisplayBufferState
	{
	PDBS_IDLE,						// The display buffer is currently not used
	PDBS_REFERENCED,				// The display buffer is referenced, and can not be reused
	PDBS_DECODING,					// Decoding is currently done into ths display buffer
	PDBS_PENDING,					// The display buffer waits in the display queue
	PDBS_DISPLAYING,				// The display buffer is currently displayed
	PDBS_DISPLAYCOMPLETING		// The display buffer waits for display to complete
	};

enum PictureDisplayPresentationMode
	{
	PDPM_FULLSIZE,
	PDPM_LETTERBOXED,
	PDPM_PANSCAN,
	PDPM_FULLSIZE16by9
	};

enum PictureDisplayAspectRatio
	{
	PDAR_4by3,
	PDAR_16by9
	};

class GenericPictureDisplay;

	//
	// Generic picture display buffer.  Every picture that is in the
	// display queue is associated by a PictureDisplayBuffer
	//
class PictureDisplayBuffer
	{
	protected:
		GenericPictureDisplay			*	display;			// handle to the display handler
	public:
		SPUDisplayBuffer					*	osd;				// associated sub picture buffer, or NULL if none
		FrameStore							*	frame;			// associated frame store, or NULL if none
		int										sheight;			// height of a stripe during StripeAccess
		PictureStructure						mode;				// requested display mode
		PictureStructure						fmode;			// picture structure of the frame buffer
		int										fieldTime;		// duration of a single field
		bool										complete;		// TRUE, if the buffer contains a complete picture
		int										referenceCount;// number of references to this buffer
		PictureDisplayBufferState			state;			// current state of the buffer.
		PictureDisplayAspectRatio			aspectRatio;	// the aspect ratio of this picture
		int										panScanOffset;	// horizontal pan & scan vector

		PictureDisplayBuffer(GenericPictureDisplay * display) {this->display = display; osd = NULL; frame = NULL;}

		virtual ~PictureDisplayBuffer(void) {}

			//
			// Braces the decoding into a display buffer.  A sub picture buffer
			// is given to the buffer, if one is to overlay it.
			//
			// IN  : osd       : Sub picture buffer to overlay.
			//
		virtual void BeginDecoding(SPUDisplayBuffer * osd) {this->osd = osd;}
		virtual void DoneDecoding(void) {}

			//
			// Braces stripe level access to the buffer during decode.  This mode
			// is used during decode to acces vertical subsections of the display
			// buffer.
			//
			// IN  : frame     : Frame buffer that is used as a reference during
			//                   the decode of this frame.
			// IN  : sheight   : Height of a stripe
			// RETURN          : TRUE on success
			//
		virtual bool BeginStripeAccess(FrameStore * frame, int sheight, PictureStructure fmode)
			{
			this->frame = frame;
			this->sheight = sheight;
			return TRUE;
			}
		virtual void DoneStripeAccess(void) {}

			//
			// Braces direct access to a stripe of data during decode.  The data
			// has to be in YUY2 mode for this access to succeed.  The display
			// buffer does also have to have to the correct size.  This function
			// will also fail, if subpicture data is to be overlayed inside the
			// display buffer at this stripe location.
			//
			// IN  : width     : requested decoding width of the picture
			// IN  : height    : requested decoding height of the picture
			// IN  : ss        : vertical stripe start
			//	OUT : p         : pointer to first line of even data to access.
			// OUT : ip        : pointer to first line of odd data to access, or NULL if
			//                   odd and even lines are interleaved.
			// OUT : stride    : distance between to lines of the same field, or two lines
			//                   of the frame if the display is interleaved.
			// RETURN          : TRUE on success
			//
		virtual bool AttemptDirectStripeYUVAccess(int width, int height, int ss, BPTR & p, BPTR & ip, int & stride) {return FALSE;}
		virtual void DoneDirectStripeAccess(PictureStructure fmode) {}

			//
			// Copy a single stripe from the frame buffer to the display buffer.
			// This function is typically used, of the direct access fails.  It
			// has to perform all necessary conversion operations, and subpicture
			// overlay functions.
			//
			// IN  : ss        : vertical stripe start
			// IN  : fmode     : picture structure
			//
		virtual void CopyStripe(int ss, PictureStructure fmode) = 0;

			//
			// Queries whether the display buffer needs the frame buffer data, even
			// if direct access did happen.  This can be the case if deinterlacing
			// is performed.
			//
		virtual bool NeedsFrameDataWithDirectYUVAccess(void) {return FALSE;}

			//
			// Copy a complete frame from the frame buffer into the display buffer
			//
		virtual void CopyFrame(FrameStore * frame)
			{
			if (frame && frame->IsFullFrame() && BeginStripeAccess(frame, frame->Height(), PS_FRAME_PICTURE))
				{
				CopyStripe(0, PS_FRAME_PICTURE);
				DoneStripeAccess();
				}
			}

		virtual void ClearFrame(void) {}

			//
			// Perform deinterlacing after the decoding of a frame.
			//
			// IN  : previous  : Previous display buffer, needed as reference
			// IO  : phint     : Inverse 3/2 pulldown hint, can be utilized by the deinterlace
			//                   algorithm to perform inverse 3/2 pulldown, instead of normal
			//                   deinterlacing, when 3/2 pulldown is likely
			// IN  : hurryUp   : The decoder is low on decoding time, may be used as a hint
			//                   to choose a simpler deinterlacing algorithm.
			//
		virtual bool DeinterlaceFrame(PictureDisplayBuffer * previous, Inverse32PulldownHint & phint, bool hurryUp)
			{
			return FALSE;
			}

			//
			// Prepare a display buffer for deinterlacing during decoding.
			//
			// IN  : previous  : Previous display buffer, needed as reference
			//
		virtual bool PrepareDeinterlaceDuringDecode(PictureDisplayBuffer * previous)
			{
			return FALSE;
			}

			//
			// Inform a display buffer, that it is likely the last display buffer in the
			// sequence, and has to perform deinterlacing with its data.
			//
		virtual bool CompleteDeinterlaceFrame(void)
			{
			return FALSE;
			}

		virtual bool DeinterlaceInitialFrame(void)
			{
			return FALSE;
			}


			//
			// Start the display of this frame, or a single field of the full frame.
			//
			// IN  : mode      : Frame or field to be displayed
			//
		virtual void DisplayFrame(PictureStructure mode) = 0;

			//
			// Check whether display is completed (this display buffer is no longer
			// visible on the screen).
			//
		virtual bool DisplayCompleted(void) = 0;

			//
			// Return the number of milliseconds, that this display buffer will be
			// visible on the screen, before the display is completed.
			//
		virtual int DisplayCompletionDelay(void)
			{
			return 1;
			}

			//
			// The sub picture display has changed during the display of this display
			// buffer.  If this buffer contains enough information to change the sub
			// picture overlay, it should do so.  This function is typically not called
			// during streaming, but for a still frame.
			//
		virtual void UpdateOSDBitmap(SPUDisplayBuffer * map) = 0;

			//
			// Sets the field versus frame display mode of this display buffer
			//
			// IN  : mode      : Either frame mode, or top/bottom first field display
			//                   mode.  This function is called, before the actual
			//                   frame decoding is started.
			// IN  : fieldTime : duration of a single field
			//
		virtual void SetFieldDisplayMode(PictureStructure mode, int fieldTime)
			{
			this->fmode = this->mode = mode;
			this->fieldTime = fieldTime;
			}

		PictureDisplayPresentationMode EffectivePresentationMode(void);

		virtual void SetAspectRatio(PictureDisplayAspectRatio aspectRatio, int panScanOffset = 0)
			{
			this->aspectRatio = aspectRatio;
			this->panScanOffset = panScanOffset;
			}

			//
			// Frame completion routine.  A frame is considered complete, if all
			// data could be decoded correctly, or the error conceilment likely
			// resulted in a usefull picture.  If a frame is not complete, it should
			// not be used for display in a sequence.
			//
		void MarkFrameIncomplete(void) {complete = FALSE;}
		void MarkFrameComplete(void) {complete = TRUE;}
		bool IsFrameComplete(void) {return complete;}

			//
			// Check whether this display buffer (also considered a frame) is
			// likely to be two interlaced fields.
			//
		virtual bool HasInterlaceArtifacts(void) {return FALSE;}

			//
			// Mark the frame buffer associated with this display buffer invalid.
			//
		virtual void InvalidateBuffer(void) {frame = NULL;}

			//
			// Increase or decrease the reference counter for this frame.  Reference
			// counters are used for HW assisted decoders, that use independend direct
			// draw surfaces.
			//
		void ObtainReference(void);
		void ReleaseReference(void);

			//
			// Check whether the rendering of this frame is complete.
			//
		virtual bool IsRenderingComplete(void) {return !frame || frame->IsRenderingComplete();}

		virtual void __cdecl DebugPrint(int x, int y, char * format, ...) {};
	};

#define NUMOSDPREVIEW	8

	//
	// Deinterlacing capabilities
	//
#define DEIF_DEINTERLACE_WEAVE					1
#define DEIF_DEINTERLACE_BOB						2
#define DEIF_CAN_CHANGE_DEINTERLACE_DECODE	4
#define DEIF_CAN_CHANGE_DEINTERLACE_DISPLAY	8

	//
	// Generic display device.  Handles all display timing, and display buffer
	// queueing.  It does also control the subpicture overlay timing.
	//
class GenericPictureDisplay : public TimingSlave,
                              protected TimedFiber
	{
	friend class PictureDisplayBuffer;
	protected:
		TimedFiberMutex			lock;									// mutex for protection of shared variables
		TimedFiberEvent			event;								// event to signal for new frame available
		TimedFiberEvent			queueEvent;							// event to signal for idle frame available
		volatile bool				terminate;							// request for streaming termination
		volatile bool				pause;								// request for pause
		volatile bool				isPaused;							// acknowledge for pause
		volatile bool				completed;							// acknowledge for completion
		volatile bool				step;									// request for single step
		bool							scanning;							// scanning mode indiaction
		bool							streaming;							// streaming mdoe indication
		bool							secondField;						// displaying second field of an interlaced frame
		PictureStructure			displayMode;						// current picture display mode

		PictureDisplayBuffer *	displayBuffers[16];				// all allocated display buffers
		int							numDisplayBuffers;				// number of allocated display buffers

		PictureDisplayBuffer	*	availQueue[16];						// idle display buffers
		PictureDisplayBuffer	*	pendingQueue[8];					// ready to be displayed buffers
		int							timeQueue[8];						// times for the ready to be displayed buffers

		PictureDisplayBuffer	*	display;								// currently displayed buffer
		PictureDisplayBuffer	*	nextAvail;							// previous displayed buffer, while it is still visible

		SPUDisplayBuffer		*	osdMap[NUMOSDPREVIEW];			// pending sub picture buffers
		int							osdStartTime[NUMOSDPREVIEW];	// start times for sub picture buffers
		int							osdStopTime[NUMOSDPREVIEW];	// end times for sub picture buffers

		int							initialDisplayTime;				// display time of first frame in a streaming sequence
		int							noOfDisplayFrames;				// number of frames displayed in current sequence

		volatile	int				firstAvail, lastAvail;			// queue control for idle queue

		MacrovisionEncoder	*	macrovisionEncoder;				// handle to macrovision encoder if available
		DWORD							preferedDeinterlaceMode;		// prefered deinterlace mode

		TimingClient				timingClient;						// timing client, used to wait for display time

		PictureDisplayPresentationMode	presentationMode;		// selects between Pan&Scan, Letterbox and Wide

		struct SPUDeferData												// data for defered sub picture display
			{
			SPUDisplayBuffer		*	map;								// sub picture bitmap
			int							mid;								// index into sub picture display queue
			int							startTime;						// start time of this subpicture
			int							stopTime;						// stop time of this subpicture
			} spuDeferData;

		ASyncErrorMessenger	*	errorMessenger;

			//
			// Check if the final frame is currently displayed
			//
		bool FinalFrameIsDisplayed(void);

			//
			// Execution routine for the display thread
			//
		virtual void FiberRoutine(void);

			//
			// Allocate the display buffers into the displayBuffers array.  If this
			// function fails, it should set numDisplayBuffers to zero.
			//
		virtual void AllocateDisplayBuffers(void) = 0;

			//
			// Free all allocated display buffers
			//
		virtual void FreeDisplayBuffers(void) = 0;

			//
			// Clear all allocated display buffers (fill them with black).
			//
		virtual void ClearDisplayBuffers(void) = 0;

			//
			// Attempt to defer the update of a sub picture bitmap, to prevent flickering
			// caused by frame reordering.
			//
			// IN  : map       : Sub picture display buffer
			// IN  : mid       : Entry in sub picture display queue
			// IN  : startTime : Start time for sub picture display
			// IN  : stopTime  : Stop time for sub picture display
			//
		virtual bool DeferUpdateOSDBitmap(SPUDisplayBuffer * map, int mid, int startTime, int stopTime);

		virtual void BeginFreezeDisplay(void) {}
		virtual void EndFreezeDisplay(void) {}
	public:
		GenericPictureDisplay(void);
		virtual ~GenericPictureDisplay(void);

			//
			// Initialize the display system, after the sequence header has been decoded.
			// This function may also be called inside a sequence, if the picture size
			// changes.
			//
			// IN  : width     : width of decoded frame
			// IN  : height    : height of decoded frame
			// IN  : minStretch: minimum stretch factor from the frame buffer to
			//                   the actual display.  1000 is a 1:1 mapping
			//
		virtual void InitDisplay(int width, int height, int minStretch);

			//
			// Hide the display.
			//
		virtual void HideDisplay(void);

			//
			// Mark all decoding buffers as invalid in the display buffers.
			//
		virtual void InvalidateDecodingBuffer(void);

			//
			// Bracing a playback sequence.  No buffer operation is performed
			// outside of a Begin/EndStreaming pair.
			//
			// IN  : scanning	 : TRUE, if the playback is in scanning mode, which
			//                   means that every buffer is to be displayed as
			//                   soon as it arrives.
			// IN  : fullReset : TRUE, all display buffers should be deleted,
			//                   and the display area hidden.
			//
		virtual void BeginStreaming(bool scanning);
		virtual void EndStreaming(bool fullReset);

			//
			// Start/Stop of a playback sequence.  Several Start/Stop pairs
			// maybe called inside a single pair of Begin/EndStreaming.  These
			// functions should execute immediately, without any delay.
			//
			// IN : playbackSpeed : 0x10000 is normal speed
			//
		virtual void StartStreaming(int playbackSpeed);
		virtual void StopStreaming(void);

			//
			// Inform the display that there is no more buffer to be expected.
			//
		virtual void DoneStreaming(void);

			//
			// Advance a single frame in Stop mode.
			//
		virtual void AdvanceFrame(void);

			//
			// Check whether the display buffers are lost to a different application,
			// and the display should be reinitialized.
			//
//		virtual bool CheckDisplayBuffersLost(void) {return FALSE;}

			//
			// Get an empty IP frame display buffer.  This function will wait
			// until an idle disply buffer is available.  If no display buffer
			// will be available (e.g. display buffers lost), it will return
			// NULL.
			//
			// IN  : approxDisplayTime : approximate display time of this IP frame
			//                           buffer.  The actual time may not be known
			//                           to the decoder at this stage of decoding.
			//									  This time is used to select a sub picture
			//                           display buffer.
			//
		virtual PictureDisplayBuffer * GetIPFrameBuffer(int approxDisplayTime);

			//
			// Put a decoded IP frame display buffer into the queue of pending
			// display buffers.
			//
			// IN  : frame      	 : display buffer to be displayed
			// IN  : displayTime  : actual display time for this picture
			//
		virtual void PostIPFrameBuffer(PictureDisplayBuffer * frame, int displayTime);

			//
			// Display/Frame buffer reference management
			//
		virtual void ObtainIPFrameBufferReference(PictureDisplayBuffer * frame);
		virtual void ReleaseIPFrameBufferReference(PictureDisplayBuffer * frame);
		virtual bool IsIPFrameBufferReferences(PictureDisplayBuffer * frame) {return frame->referenceCount > 0;}

			//
			// Get an empty B frame display buffer.  This function will wait
			// until an idle display buffer is available.  If no display buffer
			// will be available (e.g. display buffers lost), it will return
			// NULL.
			//
			// IN  : displayTime  : actual display time for this picture
			//
		virtual PictureDisplayBuffer * GetBFrameBuffer(int displayTime);

			//
			// Put a decoded B frame display buffer into the queue of pending
			//
			// IN  : frame      	 : display buffer to be displayed
			// IN  : displayTime  : actual display time for this picture
			//
		virtual void PostBFrameBuffer(PictureDisplayBuffer * frame, int displayTime);

			//
			// Return the number of entries in the sub picture display queue.
			//
		virtual int NumOSDBitmapPreview(void) {return NUMOSDPREVIEW;}

			//
			// Find an sub picture display buffer for the given time
			//
		virtual SPUDisplayBuffer * FindOSDBitmap(int time);

			//
			// Update the current sub picture display with this buffer
			//
		virtual void UpdateOSDBitmap(SPUDisplayBuffer * map);

			//
			// Prepare the display of the given sub picture display buffer.
			//
		virtual void PrepareOSDBitmap(SPUDisplayBuffer * map) {}

			//
			// Send a sub picture display buffer from the sub picture display
			// manager.
			//
		virtual void PostOSDBitmap(SPUDisplayBuffer * map, int startTime, int stopTime);

			//
			// Invalidate all sub picture display buffers.
			//
		virtual void InvalidateOSDBitmaps(void);

			//
			// Get dimensions of actual display rectangle in the display window
			//
		virtual void GetDisplayLeft(short & x) {x = 0;}
		virtual void GetDisplayTop(short & y) {y = 0;}
		virtual void GetDisplayWidth(short & w) = 0;
		virtual void GetDisplayHeight(short & h) = 0;

			//
			// Check whether this display is also a decoder
			//
		virtual bool IsDecoder(void) {return FALSE;}

			//
			// Change the macrovision level.  If a macrovisionEncoder is available,
			// this call is simply forwarded.
			//
		virtual void SetMacrovisionLevel(int level) {if (macrovisionEncoder) macrovisionEncoder->SetMacrovisionLevel(level);}

           //
           // Change the CGMS mode
           //

        virtual void SetCGMSMode(VideoCopyMode eCGMSMode) {}


			//
			// Set a MacrovisionEncoder for this display
			//
		virtual void SetMacrovisionEncoder(MacrovisionEncoder * encoder) {macrovisionEncoder = encoder;}

			//
			// Set the cropping rectangle
			//
		virtual void SetCropRectangle(WORD left, WORD top, WORD right, WORD bottom) = 0;

		virtual void SetDisplayPresentationMode(PictureDisplayPresentationMode presentationMode);

			//
			// Force a different video start time, for clips that do not start
			// with a PTS of zero.
			//
		virtual void SetVideoStartTiming(int startTime);

			//
			// Check whether this display device is available and functioning.
			//
		virtual Error CheckHardwareResources(void) {GNRAISE_OK;}

			//
			// Get the available deinterlace features of this display device.
			//
		virtual DWORD GetDeinterlaceFlags(void)
			{
			return DEIF_DEINTERLACE_WEAVE |
				    DEIF_DEINTERLACE_BOB |
				    DEIF_CAN_CHANGE_DEINTERLACE_DECODE |
				    DEIF_CAN_CHANGE_DEINTERLACE_DISPLAY;
			}

			//
			// Set the prefered deinterlace mode
			//
		virtual void SetPreferedDeinterlaceMode(DWORD mode)
			{preferedDeinterlaceMode = mode;}

			//
			// Check whether this display device supports direct stripe access
			//
		virtual bool SupportsStripeAccess(void) {return TRUE;}

			//
			// Check whether this display needs the previous frame for decoding,
			// typically used in deinterlacing.
			//
		virtual bool NeedsPreviousFrame(void) {return FALSE;}

		void SetErrorMessenger(ASyncErrorMessenger * errorMessenger) {this->errorMessenger = errorMessenger;}
	};

inline void PictureDisplayBuffer::ObtainReference(void) {display->ObtainIPFrameBufferReference(this);}
inline void PictureDisplayBuffer::ReleaseReference(void) {display->ReleaseIPFrameBufferReference(this);}
inline PictureDisplayPresentationMode PictureDisplayBuffer::EffectivePresentationMode(void)
	{
	if (/*aspectRatio == PDAR_4by3 ||*/ display->presentationMode == PDPM_FULLSIZE16by9)
		return PDPM_FULLSIZE;
	else
		return display->presentationMode;
	}

#endif
