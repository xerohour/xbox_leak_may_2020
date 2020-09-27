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

//////////////////////////////////////////////////////////////////////////////
//
//  $Workfile: qproperties.h $
//
//  Copyright (c) 1999  Quadrant International, Inc.
//  All Rights Reserved.
//
//////////////////////////////////////////////////////////////////////////////
//
// $Archive: /Tproj/Apps/xbDVD/INC/qproperties.h $
// $Author: Thermanson $
// $Modtime: 1/22/01 10:46a $
// $Date: 1/22/01 3:56p $
// $Archive: /Tproj/Apps/xbDVD/INC/qproperties.h $
// $Revision: 1 $
//
////////////////////////////////////$NoKeywords: $////////////////////////////


#ifndef QIProperties_H
#define QIProperties_H

enum {
	QI_MEDIACONTROL,
	QI_MEDIACONTROL_CDA,
	QI_MEDIACONTROL_DVD,
	QI_MEDIACONTROL_VIDEOCD,
	QI_MEDIACONTROL_FILE,
	QI_MEDIACONTROL_ANALOG_TV,
	QI_MEDIACONTROL_DIGITAL_TV,
	QI_MEDIACONTROL_CAMVCR,
	// add new type objects above this comment...
	QI_MEDIACONTROL_COUNT
	};


// Available Properties for use with IQMedia, IQMediaManager, and all
// derived classes.
//
// All definitions are comprised as follows:
//
//		QPROP_TYPE_NAME
//
// where QPROP indicates it's a property, TYPE is replaced by the data type of
// the property (such as BOOL, INT, etc.), and NAME is simply the name of the
// property.
//
// The appropriate type item in the QIPROP structure will be construed from
// the LONG type passed to set or get by typecasting.  For example, if a BOOL
// type is expected, the LONG should simply be 0 or 1, or an error is produced.
//
enum {
	QPROP_START,
//	QPROP_BOOL_TEST,			// set classes to test mode only
	QPROP_INT_MODE,			// current mode, read only by app
	QPROP_BOOL_CLOSEDCAPTIONS,	// turn on/off closed captioning

	QPROP_SHORT_VIDEOMODE,		// preferred video mode

	QPROP_SHORT_PARENTLVL,		// parental level of player --
							//		it is up to the app to save and
							//		maintain this setting -- this simply
							//		passes it along to any devices that
							//		care
	QPROP_WORD_PARENTCOUNTRY,	// parental level country, see description for
							//		parental level...

	QPROP_BOOL_STOPTOMENU,		// when playing, hit stop, go to menu or stop
							// completely?  TRUE to menu, FALSE to stop

	QPROP_HWND_EVENTWINDOW,		// window to send messages and events to

	QPROP_BOOL_USEDIVICORE,		// force use of divicore DShow filter(s)

	QPROP_BOOL_PREFERHW,		// prefer use of hardware decoding (DirectShow)

	QPROP_DWORD_SOURCESIZE,		// read only, retrieve the size of the video source as
							// HIGHWORD x LOWORD of the DWORD.


	QPROP_BOOL_ASPECTFIXED,		// should the video aspect ratio be fixed (TRUE), or should the
							// video fill the given window (FALSE)?

	QPROP_POINTER_DDCOLORCONTROL,	// get/set color controls (brightness, contrast, etc.) using a
							// pointer to the DirectDraw defined DDCOLORCONTROL structure
							// declared in ddraw.h

	QPROP_LONG_SRCLETTERBOXING,	// is the source of the video letterboxed already? see enum
							// SrcLetterBoxing

	QPROP_BOOL_PAUSEHACK,		// uses IMediaControl->Pause instead of IDvdControl->PauseOn

	QPROP_LCID_AUDIOCOUNTRY,		// place the number of the audio stream in the parameter; what is
							// returned will be the code.  READ ONLY

	QPROP_LCID_SUBPICCOUNTRY,	// see QPROP_WORD_AUDIOCOUNTRY

	QPROP_BOOL_CABLEORANTENNA,	// indication of whether cable is being used, or an antenna
							// is being used for coaxial input of a TV stream (TRUE = CABLE)

	QPROP_BOOL_TIMESHIFTABLE,	// recording device can be time-shifted.  READ ONLY

	QPROP_POINTER_ZOOMRECT,		// pass a pointer to a RECT, get back the dimensions of the video
								// section that you have zoomed using IQMediaTricks.  READ ONLY
	QPROP_BOOL_AC3_DIGITALAUDIO_PASSTHROUGH_ENABLED,		// Used for setting or obtaining the ACS digital audio passthrough
															// setting as seen in Software Cinemaster property sheet.  TRUE means
															// enabled, FALSE means disabled.
	QPROP_BOOL_AC3_DIGITALAUDIO_PASSTHROUGH_AVAILABLE,		// Determines whether or not AC-3 digital audio passthrough is available
															// on this soundcard. READ ONLY
	QPROP_END
	};

enum {
	SRCLTX_NONE,
	SRCLTX_4x3_LETTERBOX,
	SRCLTX_COUNT
	};

enum {						// Modes for QPROP_INT_MODE above
	QMODE_NONE,				// no applicable state (no usable media)
	QMODE_STILL,				// media is in still frame
	QMODE_PLAYING,				// media is playing
	QMODE_STOPPED,				// media is stopped
	QMODE_PAUSED,				// media is paused
	QMODE_RECORDING,			// media is recording
	QMODE_SEEKING,				// seeking the current media
	QMODE_BUSY,
	QMODE_TRICKREVERSE,			// trick mode reverse play
	QMODE_TRICKFRAME,			// trick mode frame advance
	QMODE_COUNT
	};

enum {
	QMENU_NONE,				// menu choices for DVD
	QMENU_TITLE,
	QMENU_ROOT,
	QMENU_SUBPIC,
	QMENU_AUDIO,
	QMENU_ANGLE,
	QMENU_CHAPTER,				// chapter menu, or part of title (PTT)
	QMENU_COUNT
	};

enum {
	QDOMAIN_NONE,				// DVD Domains
	QDOMAIN_FIRSTPLAY,
	QDOMAIN_TITLE,
	QDOMAIN_VIDEOTITLESET,
	QDOMAIN_VIDEOMANAGER,
	QDOMAIN_STOP,
	QDOMAIN_COUNT
	};

enum {
	QVIDEO_NONE,				// preferred video playback modes
	QVIDEO_DEFAULT,			// default content type
	QVIDEO_16x9,				// enhanced for 16x9 TVs
	QVIDEO_PANSCAN,			// pan and scan (TV) format
	QVIDEO_LETTERBOX,			// widescreen for normal TVs
	QVIDEO_COUNT
	};

enum {								// AnalogTV constants
	QPROP_BOOL_TIMESHIFT= 100,
	QPROP_BOOL_BLINDRECORD,
	QPROP_LONG_BITRATE,
	QPROP_INT_PICTUREQUALITY,
	QPROP_BOOL_AUTOSCANNED,
	QPROP_LONG_BUFFERLEFTMB,
	QPROP_LONG_RECORDTIMESEC,
	QPROP_LONG_DELAYTIMESEC
	};

enum {								// AnalogTV Recording states
	RECORDSTATE_STOPPED,
	RECORDSTATE_VIEWING,
	RECORDSTATE_PLAYING,
	RECORDSTATE_RECORDING,
	RECORDSTATE_TIMESHIFTING,
	RECORDSTATE_PAUSED
	};

// define a GUID for QI DVD filter
// {595E37C1-A479-11d1-BC3C-006097B18ABC}
//wmpx DEFINE_GUID(CLSID_QIDVDFilter, 0x595e37c1, 0xa479, 0x11d1, 0xbc, 0x3c, 0x00, 0x60, 0x97, 0xb1, 0x8a, 0xbc);

#define FLAG(x)	(1UL << x)

#define QFLAG_DIRECTSHOW		FLAG(0)
#define QFLAG_FILEBASED		FLAG(1)
#define QFLAG_DISCBASED		FLAG(2)
#define QFLAG_HARDWAREDECODE	FLAG(3)
#define QFLAG_SOFTWAREDECODE	FLAG(4)
#define QFLAG_CONTAINSVIDEO	FLAG(5)
#define QFLAG_STREAMING		FLAG(6)
#define QFLAG_DIGITALSTR		FLAG(7)		// digital streaming data, or analog
#define QFLAG_RECORDABLE		FLAG(8)


#endif
