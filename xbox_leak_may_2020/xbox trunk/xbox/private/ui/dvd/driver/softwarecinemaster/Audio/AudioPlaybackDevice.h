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

#ifndef AUDIOPLAYBACKDEVICE_H
#define AUDIOPLAYBACKDEVICE_H

#include "driver\SoftwareCinemaster\Common\Prelude.h"
#include "..\common\TimedFibers.h"
#include "library\common\profiles.h"
#define VIONA_VERSION	1
#define ONLY_EXTERNAL_VISIBLE
#include "library\hardware\mpeg2dec\generic\mp2eldec.h"
#include "library\hardware\audio\generic\aucodecs.h"

	//
	// Generic callback for buffer completion information
	//
class GenericAudioPlaybackCallback
	{
	public:
		virtual void WaveDataRequest(void) = 0;
	};

	//
	// Generic definition of a playback device for Audio data.
	//
class GenericAudioPlaybackDevice
	{
	public:
		virtual ~GenericAudioPlaybackDevice(void) {}

			//
			// Begin a session with audio streaming.  Has to be called prior
			// to sending any data to the streaming device.
			//
			// IN  : callback      : This callback will be called, whenever a
			//                       buffer finishes playback.
			//
		virtual void BeginStreaming(GenericAudioPlaybackCallback * callback) = 0;

			//
			// Start actual playback.  When this function is called, data is
			// typically already added to the buffer.  The playback should start
			// immediately with this call.  All long time operations have to
			// be done by BeginStreaming.
			//
		virtual void StartStreaming(void) = 0;

			//
			// Stop playback.  Playback may be resumed with StartPlayback at the
			// same location where it was stopped.  This function is called for
			// pause, or before EndStreaming.
			//
		virtual void StopStreaming(void) = 0;

			//
			// End playback.  All pending data is to be discarded.
			//
		virtual void EndStreaming(bool fullReset) = 0;

			//
			// Inform the playback device that very likely no more data is to
			// be expected, and all pending data in not yet full buffers, shall
			// be sent off to the audio hardware for playback.
			//
			// IN  : fullReset : TRUE, if there is likely a pause before the next
			//                   BeginStreaming
			//
		virtual void DoneStreaming(void) = 0;

			//
			// This function will not return before all pending data is played,
			// or EndStreaming is called.
			// It can be implemented as polling loop with calling Yield.
			//
		virtual void WaitForCompletion(void) = 0;

			//
			// Check whether the playback is completed (all data is sent to
			// the hardware, and the hardware is finished playing).
			//
		virtual bool IsCompleted(void) = 0;


			//
			// Request buffer space to fill with new audio data.
			//
			// IN  : requestSize : Requested size of buffer space to fill in
			//                     bytes.  This parameter may be ignored by
			//                     the playback device.
			// OUT : buffer      : Pointer to buffer space for filling with
			//                     audio data by decoder.
			// OUT : size        : Actual size of buffer in bytes.
			// RETURN            : TRUE, if buffer space was available.
			//
		virtual bool GetBuffer(int requestSize, short * & buffer, int & size) = 0;

			//
			// Request buffer space to fill with new audio data.
			//
			// IN  : requestSize : Requested size of buffer space to fill in
			//                     bytes.  This parameter may be ignored by
			//                     the playback device.
			// OUT : bufA		 : Pointer to buffer space for filling with analog
			//                     audio data by decoder.
			// OUT : bufA		 : Pointer to buffer space for filling with digital
			//                     audio data by decoder.
			// OUT : size        : Actual size of buffer in bytes.
			// RETURN            : TRUE, if buffer space was available.
			//
		virtual bool GetBuffers(int requestSize, short * & bufA, short * & bufD, int & size) = 0;

			//
			// Return buffer space that was requested with GetBuffer back to
			// the playback device.
			//
			// IN  : usedSize    : Actually used size of buffer in bytes.
			//
		virtual void PostBuffer(int usedSize, int time) = 0;

			//
			// Returns the current playback location since the last call to
			// BeginStreaming in stereo samples
			//
		virtual int CurrentLocation(void) = 0;

			//
			// Returns the available buffer space in stereo samples.
			//
		virtual int AvailBufferSpace(void) = 0;

			//
			// Returns the current sampling rate
			//
		virtual int GetSamplingFrequency(void) = 0;

			//
			// Returns the current volume, HIWORD left, LOWORD right
			//
		virtual DWORD GetVolume(void) = 0;

			//
			// Sets the current volume
			//
		virtual void SetVolume(DWORD volume) = 0;

			//
			// Attempts to change the playback rate on the fly.
			//
			// IN  : rate      : Requested playback rate, 0x10000 is normal
			//                   playback speed.
			// RETURN          : Returns TRUE on success
			//
		virtual bool SetPlaybackRate(DWORD rate) = 0;

			//
			// Returns TRUE, if this playback device is functional
			//
		virtual Error CheckHardwareResources(void) = 0;

			//
			// Sets the Sample Rate
			//
		virtual Error SetSampleRate(int newSampleRate) {GNRAISE(GNR_AUDIO_TYPE_NOT_SUPPORTED);}

			//
			// Sets the copy protection mode for SPDIF
			//
		virtual Error SetCopyProtectionLevel(SPDIFCopyMode sCopyMode) {GNRAISE(GNR_NO_SPDIF_HARDWARE);}

			//
			// Checks whether this playback device supports SPDIF data output
			//
		virtual bool SupportsSPDIFDataOut(void) {return FALSE;}

			//
			// Sets the playback device to SPDIF data output mode
			//
		virtual Error SetSPDIFDataOut(bool enable) {if (enable) GNRAISE(GNR_NO_SPDIF_HARDWARE); else GNRAISE_OK;}

			//
			// Checks whether this device supports volume control
			//
		virtual bool SupportsVolumeControl(void) {return TRUE;}

			//
			// Requests the device to enter DropDataMode.  In this mode, no
			// playback is happening.  This function is called only in stop
			// state.  Instead of playing the data, the data is to be dropped
			// by the device on calls of DropData.  This function is used for
			// single step and slow motion playback.
			//
		virtual bool EnterDropDataMode(void) {return FALSE;}

			//
			// Requests the device to return to normal playback.
			//
		virtual void LeaveDropDataMode(void) {}

			//
			// Requests the device to drop a number of bytes out of the decoder
			// buffer.
			//
		virtual void DropData(int bytes) {}

			//
			// Requests the device to enter ResampleMode.  In this mode, the audio
			// data is played with a different speed.  This function is only called
			// in stop state, and used for smooth slow motion or fast forward.
			//
			// IN  : speed     : Requested playback speed, 0x10000 is normal
			//                   playback speed.
			// RETURN          : Returns TRUE on success
			//
		virtual bool EnterResampleMode(DWORD speed) {return FALSE;}

			//
			// Request the device to leave the ResampleMode, and return to normal
			// playback mode.
			//
		virtual void LeaveResampleMode(void) {}

	};


#endif
