
// FILE:      library\hardware\video\generic\videvice.h
// AUTHOR:    Dietmar Heidrich
// COPYRIGHT: (c) 1996 Viona Development.  All Rights Reserved.
// CREATED:   09.10.96
//
// PURPOSE: Video device tags define which attributes are available on the video bus.
//
// HISTORY:

#ifndef VIDEVICE_H
#define VIDEVICE_H

#include "..\..\..\common\prelude.h"
#include "..\..\..\common\tags.h"
#include "..\..\..\common\tagunits.h"
#include "..\..\..\common\virtunit.h"
#include "vidtypes.h"

// Video Content Types

// Interlaced material (two fields of a frame independent)
#define	VCT_INTERLACED			MKFLAG(0)
// Progressive material (two fields of a frame identical)
#define	VCT_PROGRESSIVE		MKFLAG(1)
// Material for which 3:2 pulldown may be applied:
#define	VCT_MOVIE				MKFLAG(2)

// Content is a still sequence (stopped/paused)
#define	VCT_STILL_SEQUENCE	MKFLAG(3)
#define	VCT_STOPPED				MKFLAG(4)
#define	VCT_PAUSED				MKFLAG(5)

#define	VCT_HALFWIDTH			MKFLAG(6)
#define	VCT_HALFHEIGHT			MKFLAG(7)


#ifndef ONLY_EXTERNAL_VISIBLE

enum VideoHorizontalReference
	{
	VHR_HSYNC,
	VHR_HREF,
	VHR_HBLANK,
	VHR_CBLANK,
	VHR_CCIR656
	};

enum VideoVerticalReference
	{
	VVR_VSYNC,
	VVR_VREF,
	VVR_VBLANK,
	VVR_ODD_EVEN,
	VVR_CBLANK,
	VVR_CCIR656
	};
	
enum VideoFieldIndication
	{
	VFID_INTERNAL,
	VFID_EXTERNAL_ODD,
	VFID_EXTERNAL_EVEN,
	VFID_INTERNAL_REVERSE_POL   // reverse polarity
	};

MKTAG (VDV_VIDEOSTANDARD,			VIDEO_DEVICE_UNIT, 0x0000001, VideoStandard)
//MKTAG (VDV_COLORSTANDARD,	VIDEO_DEVICE_UNIT, 0x0000002, yet undefined enum)
MKTAG (VDV_SAMPLEMODE,				VIDEO_DEVICE_UNIT, 0x0000003, VideoSampleMode)
MKTAG (VDV_VIDEOFORMAT,				VIDEO_DEVICE_UNIT, 0x0000004, VideoFormat)
MKTAG (VDV_HORIZONTAL_REFERENCE,	VIDEO_DEVICE_UNIT, 0x0000005, VideoHorizontalReference)
MKTAG (VDV_VERTICAL_REFERENCE,	VIDEO_DEVICE_UNIT, 0x0000006, VideoVerticalReference)
MKTAG (VDV_SYNC_MASTER,				VIDEO_DEVICE_UNIT, 0x0000007, BOOL)
MKTAG (VDV_FIELD_INDICATION,		VIDEO_DEVICE_UNIT, 0x0000008, VideoFieldIndication)
MKTAG (VDV_PIXELCLOCK_EDGE,		VIDEO_DEVICE_UNIT, 0x0000009, BOOL)
MKTAG (VDV_VIDEOBUSFORMAT,			VIDEO_DEVICE_UNIT, 0x000000a, VideoBusFormat)
MKTAG (VDV_MODE_OVERRIDE,			VIDEO_DEVICE_UNIT, 0x000000b, BOOL)


	//
	// Offset of the actual video frame relative to an ideal
	// video frame.  This includes black borders around the
	// actual video image.  This is in units of single pixels.
	//
MKTAG (VDV_HOFFSET,					VIDEO_DEVICE_UNIT, 0x0000010, int)
MKTAG (VDV_VOFFSET,					VIDEO_DEVICE_UNIT, 0x0000011, int)

	//
	// In 4 times pixel clk units.
	//
MKTAG (VDV_HTRIGGER,					VIDEO_DEVICE_UNIT, 0x0000020, int)

	//
	// HIGH	: rising edge
	// LOW	: falling edge
	//
MKTAG (VDV_HSYNC_POL,				VIDEO_DEVICE_UNIT, 0x0000021, BIT)
MKTAG (VDV_VSYNC_POL,				VIDEO_DEVICE_UNIT, 0x0000022, BIT)

	//
	// Active display area (esp. not blank) inside the acutal video
	// rectangle.
	//
MKTAG (VDV_ACTIVE_LEFT,				VIDEO_DEVICE_UNIT, 0x0000030, WORD)
MKTAG (VDV_ACTIVE_TOP,				VIDEO_DEVICE_UNIT, 0x0000031, WORD)
MKTAG (VDV_ACTIVE_WIDTH,			VIDEO_DEVICE_UNIT, 0x0000032, WORD)
MKTAG (VDV_ACTIVE_HEIGHT,			VIDEO_DEVICE_UNIT, 0x0000033, WORD)

	//
	// Video out options.
	//
MKTAG (VDV_VOUT_NTSC_SETUP,		VIDEO_DEVICE_UNIT, 0x0000040, BOOL)   // NTSC 7.5 IRE setup
MKTAG (VDV_VOUT_IDLE_VSTANDARD,	VIDEO_DEVICE_UNIT, 0x0000041, VideoStandard)   // idle video standard

	//
	// Information on video content
	//
MKTAG (VDV_STILL_SEQUENCE,			VIDEO_DEVICE_UNIT, 0x0000050, BOOL)

// More than one flag may be set at one time
MKTAG (VDV_CONTENT_TYPE,			VIDEO_DEVICE_UNIT, 0x0000051, DWORD)


#define VDVC_VIDEOSTANDARD		MKFLAG(0)
#define VDVC_SAMPLEMODE			MKFLAG(1)
#define VDVC_VIDEOFORMAT		MKFLAG(2)
#define VDVC_SYNCREF				MKFLAG(3)
#define VDVC_SYNC_MASTER		MKFLAG(4)
#define VDVC_OFFSET				MKFLAG(5)
#define VDVC_ACTIVE_RECT		MKFLAG(6)
#define VDVC_FIELD_INDICATION MKFLAG(7)

class VideoMux : public PhysicalUnit
	{
	protected:
		virtual Error SetVideoStandard(VideoStandard videoStandard) {GNRAISE_OK;}
		virtual Error SetSampleMode(VideoSampleMode videoSampleMode) {GNRAISE_OK;}
		virtual Error SetVideoFormat(VideoFormat videoFormat) {GNRAISE_OK;}
		virtual Error SetHorizontalReference(VideoHorizontalReference horizontalReference) {GNRAISE_OK;}
		virtual Error SetVerticalReference(VideoVerticalReference verticalReference) {GNRAISE_OK;}
		virtual Error SetSyncMaster(BOOL syncMaster) {GNRAISE_OK;}
		virtual Error SetFieldIndication(VideoFieldIndication fieldIndication) {GNRAISE_OK;}
		
		virtual Error SetHOffset(int hoffset) {GNRAISE_OK;}
		virtual Error SetVOffset(int voffset) {GNRAISE_OK;}
		virtual Error SetHTrigger(int htrigger) {GNRAISE_OK;}
		virtual Error SetHSyncPolarity(BIT hsyncPol) {GNRAISE_OK;}
		virtual Error SetVSyncPolarity(BIT vsyncPol) {GNRAISE_OK;}
				
		virtual Error SetActiveLeft(WORD activeLeft) {GNRAISE_OK;}
		virtual Error SetActiveTop(WORD activeTop) {GNRAISE_OK;}
		virtual Error SetActiveWidth(WORD activeWidth) {GNRAISE_OK;}
		virtual Error SetActiveHeight(WORD activeHeight) {GNRAISE_OK;}
	public:
		virtual Error Update(void) {GNRAISE_OK;}
	};
	
class VirtualVideoMux : public VirtualUnit
	{
	private:
		VideoMux	*	mux;
	protected:
		struct {
			VideoStandard					videoStandard;
			VideoSampleMode				videoSampleMode;
			VideoFormat						videoFormat;
			VideoHorizontalReference   horizontalReference;
			VideoVerticalReference		verticalReference;
			BOOL								syncMaster;
			VideoFieldIndication			fieldIndication;
			int								hoffset, voffset;
			int								htrigger;
			BIT								hsyncPol;
			BIT								vsyncPol;
			WORD								activeLeft;
			WORD								activeTop;
			WORD								activeWidth;
			WORD								activeHeight;
			} actual, updated;
		
		DWORD								changed;

		virtual Error UpdateParameters(void);
	public:
		VirtualVideoMux(VideoMux * mux);
		
		Error Configure(TAG __far * tags);
		Error Preempt(VirtualUnit * previous);		
	};

#endif // external visible

#endif
