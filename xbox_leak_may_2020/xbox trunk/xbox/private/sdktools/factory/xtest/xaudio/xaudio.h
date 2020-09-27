// (c) Copyright 2000-2001 Intel Corp. All rights reserved.
// Intel Confidential
// WinAudio.h : main header file for the WINAUDIO WinMTA module
//
#if !defined(_XAUDIO_H_)
#define _XAUDIO_H_

// supress futile warnings about assignment operators
#pragma warning (disable:4512)

#include "..\parameter.h"
#include <dsound.h>

#ifndef _XBOX // Win32 libraries
#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "dsound.lib")
#pragma comment (lib, "dxguid.lib")
#endif

// We assume that short will always be signed 16-bit, and char will always be signed 8-bit
typedef long SLONG;  // Define Signed LONG (signed 32-bit value)
#ifndef PASS
#define PASS 0
#endif
#ifndef FAIL
#define FAIL 0xFFFFFFFF
#endif

#define XAUDIO_MODNUM   2

#define XMTA_AUDIO_MUTEX_NAME  "XMTA_AUDIO"

#define AUDIO_HOST_TIMEOUT 180 // Wait for 3 minutes for the host to respond to any communications
#define PARAMETER_REV_FOR_AUDIO_RECORD_ANALOG  1
#define PARAMETER_REV_FOR_AUDIO_RECORD_DIGITAL 1

// Naming of class test and module classes.
// 
// module class: CXmta<module>
// test class:   CXmta<module>Test<test>
//
// The names are important because the modules and exective are 
// all one project.  Since the classes are presented alphebetically, 
// using this naming convention makes the code easier to navigate.
//

typedef struct {
	DWORD dwNumberOfFrequencies;
	double *pdblFrequency;
	double *pdblSamplesPerPeriod;
} WAVE_PLAYBACK_CHANNEL_FREQ_DATA;

typedef struct {
	LPCTSTR pwszAudioOutDevName;
	UINT  uiAudioOutDevToUse;
	LPGUID pAudioOutDevToUseGUID;
	DWORD dwNumberOfChannels;
	DWORD dwBitsPerSample;
	DWORD dwSampleRate;
	DWORD dwDigitalAttenuation;
	WAVE_PLAYBACK_CHANNEL_FREQ_DATA *WaveChannelFreqData;
} WAVE_PLAYBACK_CONFIGURATION_DATA;

typedef struct {
	char *name;
	WAVE_PLAYBACK_CONFIGURATION_DATA *data;
} PLAYBACK_TEST_CONFIGURATIONS;

typedef struct {
	DWORD dwNumberOfFrequencies;
	double *pdblFrequency;  // In Hz
	double *pdblAmplitude;  // In dB (normally negative)
} WAVE_RECORD_CHANNEL_FREQ_DATA;

// An array of these structures will contain the noise level ranges that the test will compare input data against.  The ranges are lines between the startlevel and the stoplevel
typedef struct {
	double dblNoiseLimitStartFreq;  // In Hz
	double dblNoiseLimitStopFreq;   // In Hz
	double dblNoiseLimitStartLevel; // In dB relative to full-scale (so a negative number)
	double dblNoiseLimitStopLevel;  // In dB relative to full-scale (so a negative number)
} WAVE_RECORD_NOISE_LIMIT;

typedef struct {
	LPCTSTR pwszAudioInDevName;
	UINT  uiAudioInDevToUse;
	DWORD dwNumberOfChannels;
	wchar_t **pchChannelLabel;
	DWORD dwBitsPerSample;
	DWORD dwSampleRate;
	DWORD dwCRC;                    // CRC used for digital loopback testing
	DWORD dwFFTWindowFunction;
	double dblAmplitudeTolerance;   // In dB (positive)
	double dblMaxDCOffsetPercent;   // The default for this should be 100; in which case we shouldn't even bother computing the DC offset (save some time)
	double dblSignalToNoiseRatio;   // In dB (positive)
	double dblSignalToNoiseRatioIgnoringCrosstalk;   // In dB (positive)
	double dblChannelSeparation;    // In dB (positive)
	double dblFrequencyResponseTolerance;            // In dB (positive)
	double dblFrequencyResponseLevel;                // In dB (negative)
	double dblFrequencyShiftPercentAllowed;               // 60 is great for 44100 sampling with COS_SQUARED, 35 is great for 22050 sampling with COS_SQUARED, 20 is great for 11025 sampling with COS_SQUARED
	double dblFrequencyShiftHzAllowed;               // 60 is great for 44100 sampling with COS_SQUARED, 35 is great for 22050 sampling with COS_SQUARED, 20 is great for 11025 sampling with COS_SQUARED
	DWORD dwNumberOfNoiseRanges;    // Calculated, not read from a config parameter
	WAVE_RECORD_NOISE_LIMIT *WaveRecordNoiseLimit;
	WAVE_RECORD_CHANNEL_FREQ_DATA *WaveChannelFreqData;
} WAVE_RECORD_CONFIGURATION_DATA;

typedef struct {
	char *name;
	WAVE_RECORD_CONFIGURATION_DATA *data;
} RECORD_TEST_CONFIGURATIONS;

typedef struct {
	DWORD dwMuxControlLowDWORD;
	DWORD dwMuxControlHighDWORD;
} AUDIO_MUX_CONFIGURATION_INFORMATION;


class CXModuleAudio : public CTestObj
{
public:
	DECLARE_XMTAMODULE (CXModuleAudio, "audio", XAUDIO_MODNUM);  //(module name, module number)

protected:
	// Error messages declared here ...
	//(Note: 0x000 - 0x00f reserved for XMTA)
	//

	// Error routines return true if the calling routine should abort, and false otherwise
	bool err_BADPARAMETER (LPCTSTR s1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x010, L"Can't find configuration parameter \"%s\"\nThis configuration parameter doesn't exist or is invalid", s1); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateWAVEFORMATEX () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x011,L"Could not allocate a WAVEFORMATEX data structure"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateRecordBuffer (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x012,L"Could not allocate memory for a %lu byte audio record buffer", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_AllocWavePlayConfigDataL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x013,L"Could not allocate a %lu byte WAVE_PLAYBACK_CONFIGURATION_DATA structure to hold WAVE playback configuration information", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_AllocWavePlayChannelFreqDataL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x014,L"Could not allocate a %lu byte array of WAVE_PLAYBACK_CHANNEL_FREQ_DATA structures to hold WAVE playback frequency information", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_AllocDoublesForPlaybackFreqL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x015,L"Could not allocate a %lu byte array of doubles to hold playback frequency information", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_AllocDoublesForPlaybackSPPL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x016,L"Could not allocate a %lu byte array of doubles to hold playback samples-per-period information", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_CantReadPlaybackFreqData () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x017,L"Could not read playback frequency configuration parameters"); return (CheckAbort (HERE)); } return false;
	}
	bool err_AllocWaveRecordConfigDataL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x018,L"Could not allocate a %lu byte WAVE_RECORD_CONFIGURATION_DATA structure to hold WAVE record configuration information", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_AllocWaveRecordNoiseLimitL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x019,L"Could not allocate a %lu byte array of WAVE_RECORD_NOISE_LIMIT structures to hold WAVE record noise limits information", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_AllocWaveRecordChannelFreqDataL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x01A,L"Could not allocate a %lu byte array of WAVE_RECORD_CHANNEL_FREQ_DATA structures to hold WAVE record frequency information", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_AllocDoublesForRecordFreqL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x01B,L"Could not allocate a %lu byte array of doubles to hold record frequency information", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_AllocDoublesForRecordAmplitudeL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x01C,L"Could not allocate a %lu byte array of doubles to hold record amplitude information", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_CantReadRecordNoiseLimit () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x01D,L"Could not read record noise limit configuration parameters"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CantReadRecordFreqData () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x01E,L"Could not read record frequency configuration parameters"); return (CheckAbort (HERE)); } return false;
	}
	bool err_AllocAudioMuxConfigInfoL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x01F,L"Could not allocate a %lu byte AUDIO_MUX_CONFIGURATION_INFORMATION structure to hold audio mux configuration information", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocatePlaybackBuffer (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x020,L"Could not allocate memory for a %lu byte audio playback buffer", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_PlaybackAudioFormatNotSupported () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x021,L"The specified audio playback format is not supported by this device"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateFFTInputBufferL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x022,L"Could not allocate a %lu byte FFT input data buffer", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateDCOffsetPercentsL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x023,L"Could not allocate a %lu byte buffer to contain DC offset percents", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_DCOffsetsOutOfRangeLSDD (DWORD dw1, LPCTSTR s1, double d1, double d2) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x024,L"Channel %lu %s DC offset of %5.2lf percent detected!  This is greater than the expected %5.2lf percent", dw1, s1, d1, d2); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateNoiseLimitsL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x025,L"Could not allocate a %lu byte buffer to contain noise limit values", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateFFTMagnitudeL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x026,L"Could not allocate a %lu byte FFT magnitude data buffer", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_SignalToNoiseRatioTooSmallLSDD (DWORD dw1, LPCTSTR s1, double d1, double d2) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x027,L"Signal-to-Noise ratio is too small on channel %lu %s.  Expected %6.2lf dB, Actual %6.2lf dB", dw1, s1, d1, d2); return (CheckAbort (HERE)); } return false;
	}
	bool err_SignalToNoiseNoCrossTooSmallLSDD (DWORD dw1, LPCTSTR s1, double d1, double d2) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x028,L"Signal-to-Noise ratio ignoring crosstalk is too small on channel %lu %s.  Expected %6.2lf dB, Actual %6.2lf dB", dw1, s1, d1, d2); return (CheckAbort (HERE)); } return false;
	}
	bool err_ChannelSeperationTooSmallLSDD (DWORD dw1, LPCTSTR s1, double d1, double d2) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x029,L"Channel separation value is too small on channel %lu %s.  Expected %6.2lf dB, Actual %6.2lf dB", dw1, s1, d1, d2); return (CheckAbort (HERE)); } return false;
	}
	bool err_FrequencyResponseBadLSDD (DWORD dw1, LPCTSTR s1, double d1, double d2) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x02A,L"Frequency response is not within tolerance on channel %lu %s.  Expected %6.2lf dB, Actual %6.2lf dB", dw1, s1, d1, d2); return (CheckAbort (HERE)); } return false;
	}
	bool err_DidNotFindExpectedSineFreqLSDDDD (DWORD dw1, LPCTSTR s1, double d1, double d2, double d3, double d4) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x02B,L"Did not find the expected channel %lu %s sine wave frequency of %8.2lf Hz\n"
				L"Expected amplitude between %7.2lf and %7.2lf dB, Actual %7.2lf dB", dw1, s1, d1, d2, d3, d4); return (CheckAbort (HERE)); } return false;
	}
	bool err_DetectedUnexpectedSineFreqLSDDD (DWORD dw1, LPCTSTR s1, double d1, double d2, double d3) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x02C,L"Detected an unexpected channel %lu %s sine wave frequency of %8.2lf Hz\n"
				L"Expected amplitude below %7.2lf dB, Actual %7.2lf dB", dw1, s1, d1, d2, d3); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateFFTPiBufferL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x02D,L"Could not allocate a %lu byte buffer to hold PI function data for FFT", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateFFTRTFBufferL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x02E,L"Could not allocate a %lu byte buffer to hold Real Transform function data for FFT", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateFFTITFBufferL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x02F,L"Could not allocate a %lu byte buffer to hold Imaginary Transform function data for FFT", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateFFTROFBufferL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x030,L"Could not allocate a %lu byte buffer to hold Real Omega function data for FFT", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateFFTIOFBufferL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x031,L"Could not allocate a %lu byte buffer to hold Imaginary Omega function data for FFT", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateSineWaveCreateL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x032,L"Could not allocate a %lu byte buffer for sine wave data creation", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_MutexTimeout (LPCTSTR s1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x033,L"Resource confict, timed out waiting for %s mutex", s1); return (CheckAbort (HERE)); } return false;
	}
	bool err_MutexHandle (LPCTSTR s1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x034,L"Could not get a handle for %s ", s1); return (CheckAbort (HERE)); } return false;
	}
	bool err_CRCMiscompareLL (DWORD dw1, DWORD dw2) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x035,L"CRC Value is incorrect\n"
				L"Expected value is 0x%8.8lx, Actual value is 0x%8.8lx", dw1, dw2); return (CheckAbort (HERE)); } return false;
	}
	bool err_DirectSoundEnumerateFailed () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x036,L"Can not determine which Direct Sound drivers are installed"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotCreateIDirectSound () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x037,L"Could not create an IDirectSound interface to play DirectSound audio"); return (CheckAbort (HERE)); } return false;
	}
	bool err_DirectSoundFindFailed () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x038,L"Could not find a DirectSound device"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotSetCooperativeLevel () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x039,L"Could not set the cooperative level to the DirectSound device"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotGetAccessToPrimaryBuffer () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x03A,L"Could not get access to the primary DirectSound playback buffer"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotGetAccessToSecondaryBuffer () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x03B,L"Could not get access to the secondary DirectSound playback buffer"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotSetPrimaryBufferFormat () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x03C,L"Could not set the format of the primary DirectSound playback buffer"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotReadPrimaryBufferFormat () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x03D,L"Could not read the format of the primary DirectSound playback buffer"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotSetSecondaryBufferVolume () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x03E,L"Could not set the volume of the secondary DirectSound playback buffer"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotSetSecondaryBufferPanning () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x03F,L"Could not set the panning characteristics of the secondary DirectSound playback buffer"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotLockSecondaryBuffer () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x040,L"Could not lock the secondary DirectSound playback buffer"); return (CheckAbort (HERE)); } return false;
	}
	bool err_LockedSecondaryBufferSegmentWrongSize (DWORD dw1, DWORD dw2) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x041,L"The locked secondary buffer is not the correct size\n"
				L"Requested buffer size is %lu bytes; actual buffer size is %lu bytes", dw1, dw2); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotUnlockSecondaryBuffer () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x042,L"Could not unlock the secondary DirectSound playback buffer"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotPlaySecondaryBuffer () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x043,L"Could not play the secondary DirectSound playback buffer"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotStopPlayingSecondaryBuffer () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x044,L"Could not stop playing the secondary DirectSound playback buffer"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateLPDIRECTSOUNDBUFFERArray () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x045,L"Could not allocate an array of Direct Sound buffer pointers"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateLPDIRECTSOUNDBUFFER8Array () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x046,L"Could not allocate an array of Direct Sound 8 buffer pointers"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotGetAccessToPrimaryIDirectSoundBuffer8Interface () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x047,L"Could not get access to the primary buffer's IDirectSoundBuffer8 interface"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotGetAccessToSecondaryIDirectSoundBuffer8Interface () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x048,L"Could not get access to the secondary buffer's IDirectSoundBuffer8 interface"); return (CheckAbort (HERE)); } return false;
	}
	bool err_HostCommunicationError (int i1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x049, L"iSendHost communication routine returned an error code of 0x%x", i1); return (CheckAbort (HERE)); } return false;
	}
	bool err_RecordBufferIsEmpty () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x04A, L"The recorded audio buffer from the host is empty"); return (CheckAbort (HERE)); } return false;
	}
	bool err_RecordBufferIsNotCorrectLength (DWORD dw1, DWORD dw2) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x04B, L"The length of the recorded audio buffer from the host is not correct.\nExpected %ld bytes, received %ld bytes", dw1, dw2); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateWaveOutBufferArray () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x04C,L"Could not allocate an array of audio data output buffer pointers"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotSetSecondaryBufferData () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x04D,L"Could not point the secondary buffer to the local data buffer"); return (CheckAbort (HERE)); } return false;
	}
	bool err_AllocMixerStorage (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x04E,L"Could not allocate a buffer of size %lu bytes to store mixer parameters", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotFindDigitalSignature () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x04F,L"Could not find the signature at the start of the recorded digital buffer"); return (CheckAbort (HERE)); } return false;
	}
	bool err_CouldNotAllocateCRCBufferL (DWORD dw1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x050,L"Could not allocate a %lu byte array to contain data for the CRC calculation", dw1); return (CheckAbort (HERE)); } return false;
	}
	bool err_HostResponseError (DWORD dwErrorCodeFromHost, LPCTSTR s1) { if (gdwIgnoreError != TRUE)
	{
		ReportError ((unsigned short)dwErrorCodeFromHost, L"The host responded with the following error message:\n%s", s1); return (CheckAbort (HERE)); } return false;
	}


	// Global variables
public:
	// Parameters defined globally (initialized only once)
	DWORD gdwMaxTestTrys;
	DWORD gdwIgnoreError;
	DWORD gdwDelayBetweenPlaybackStartAndRecordStart;     // Contains the delay in ms that the test should pause after starting a new playback stream before starting recording
	DWORD gdwMutexTimeout;     // Contains the delay in ms that the test will wiat for a mutex to become available
	DWORD gdwAudioMuxComPort;        // The COM port that the audio mux is attached to.  0 indicates no audio mux is attached
	WAVE_PLAYBACK_CONFIGURATION_DATA *gpWavePlaybackConfigurationData;  // Pointer to playback configuration data structure
	WAVE_RECORD_CONFIGURATION_DATA *gpWaveRecordConfigurationData;  // Pointer to record configuration data structure
	FILE *gpF;
	DWORD *gpdwPI; // Pointer to a buffer that will contain the PI data for the FFT function
	AUDIO_MUX_CONFIGURATION_INFORMATION *gpAudioMuxConfigurationInformation;
	LPCTSTR gpwszAudioMixerName;
	DWORD gdwNumberOfMixerControls;
	CStore *pCSMixer;  // The list of mixer parameters could get very long

	static const wchar_t * pwchFFTWindowFunctionNames[];
	static const wchar_t * gpchMonoLabel[];
	static const wchar_t * gpchStereoLabel[];
	static const double gdblStereoTwelveFrequencyQuicktestFreqCh0[];
	static const double gdblStereoTwelveFrequencyQuicktestAmp[];
	static const double gdblStereoTwelveFrequencyQuicktestFreqCh1[]; 
	static const WAVE_PLAYBACK_CHANNEL_FREQ_DATA gwpcfdStereoTwelveFrequencyQuicktest[];
	static const WAVE_RECORD_CHANNEL_FREQ_DATA gwrcfdStereoTwelveFrequencyQuicktest[];
	static const WAVE_RECORD_NOISE_LIMIT gwrnlStereoTwelveFrequencyQuicktest[];
	static const WAVE_PLAYBACK_CONFIGURATION_DATA gwpcdStereoTwelveFrequencyQuicktest;
	static const WAVE_RECORD_CONFIGURATION_DATA gwrcdStereoTwelveFrequencyQuicktest;
	static const PLAYBACK_TEST_CONFIGURATIONS gPlaybackTestConfigurations[];
	static const RECORD_TEST_CONFIGURATIONS gRecordTestConfigurations[];

	// Parameters used within the retry loop (re-initialized with each loop)
	LPDIRECTSOUND8 pDSoundOut;
	LPDIRECTSOUNDBUFFER pPrimaryBuffer, *pSecondaryBufferPre8;
	LPDIRECTSOUNDBUFFER8 *pSecondaryBuffer;
	DWORD **ppdwWaveOutBuffer;
	DWORD *pdwWaveInBuffer;
//	WAVEHDR *pWaveOutHdr;
//	WAVEHDR *pWaveInHdr;
//	HWAVEOUT hWaveOut; 
//	HWAVEIN hWaveIn;
	WAVEFORMATEX *WaveFormatex;
	SLONG *pslFFTInputBuffer;
	double *pdblDCOffsetPercents;
	double *pdblNoiseLimitValues;
	DWORD *pdwMagnitudeValues;
	DWORD dwNumberOfFrequencies;
	WORD wTestNumber;

	HANDLE hXMTAAudioMutex;
//	HANDLE ghWaveRecordDone;
	HWND hDSWnd; // This is the handle to the current Window, used for DirectSound functions.  This will probably go away on XBox.
	DWORD dwLoopCounter;



	// Configuration parameters declared here ...
	//
//	int m_cfgInt;
//	UINT m_cfgUint;
//	LPCTSTR m_cfgString;
//	int m_cfgChoice;

virtual bool InitializeParameters ();

public:
	// Common functions go here.  Put any functions that need 
	// to be global to the tests in the module class
	//
	virtual ~CXModuleAudio () // Destructor
	{
		DWORD j;

		if (gpdwPI != NULL)
		{
			delete [] gpdwPI;
			gpdwPI = NULL;
		}
		// Let's clean up the WAVE_PLAYBACK_CONFIGURATION_DATA structure if it needs some attention (delete things in the opposite order than they were created)
		if(gpWavePlaybackConfigurationData != NULL)
		{
			if (gpWavePlaybackConfigurationData->WaveChannelFreqData != NULL)
			{
				for (j = 0; j < gpWavePlaybackConfigurationData->dwNumberOfChannels; j++)
				{
					if (gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency != NULL)
					{
						delete [] gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency;
						gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency = NULL;
					}
					if (gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblSamplesPerPeriod != NULL)
					{
						delete [] gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblSamplesPerPeriod;
						gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblSamplesPerPeriod = NULL;
					}
				}
				delete [] gpWavePlaybackConfigurationData->WaveChannelFreqData;
				gpWavePlaybackConfigurationData->WaveChannelFreqData = NULL;
			}
			delete gpWavePlaybackConfigurationData;
			gpWavePlaybackConfigurationData = NULL;
		}
		// Let's clean up the WAVE_RECORD_CONFIGURATION_DATA structure if it needs some attention (delete things in the opposite order than they were created)
		if(gpWaveRecordConfigurationData != NULL)
		{
			if (gpWaveRecordConfigurationData->WaveRecordNoiseLimit != NULL)
			{
				delete [] gpWaveRecordConfigurationData->WaveRecordNoiseLimit;
				gpWaveRecordConfigurationData->WaveRecordNoiseLimit = NULL;
			}
			if (gpWaveRecordConfigurationData->WaveChannelFreqData != NULL)
			{
				for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfChannels; j++)
				{
					if (gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblFrequency != NULL)
					{
						delete [] gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblFrequency;
						gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblFrequency = NULL;
					}
					if (gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblAmplitude != NULL)
					{
						delete [] gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblAmplitude;
						gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblAmplitude = NULL;
					}
				}
				delete [] gpWaveRecordConfigurationData->WaveChannelFreqData;
				gpWaveRecordConfigurationData->WaveChannelFreqData = NULL;
			}
			delete gpWaveRecordConfigurationData;
			gpWaveRecordConfigurationData = NULL;
		}
		if (gpAudioMuxConfigurationInformation != NULL)
		{
			delete gpAudioMuxConfigurationInformation;
			gpAudioMuxConfigurationInformation = NULL;
		}

		if (pCSMixer != NULL)
		{
			delete pCSMixer;
			pCSMixer = NULL;
		}
	};

	DWORD dwAnalogPlayback(void);
	DWORD dwDigitalAccuracy(void);
	void vInitializeTestVariables(void);
	void vFixUpPlaybackFrequencies(void);
	void vCalculateSamplesPerPeriod(void);
	DWORD dwGenSineWave(DWORD *pdwDataBuffer, DWORD dwBitsPerSample, DWORD dwNumberOfSamples, WAVE_PLAYBACK_CHANNEL_FREQ_DATA *pWaveChannelFreqData, DWORD dwNumberOfChannels, DWORD dwAttenuationDB);
	DWORD dwGenSingleSineWave(DWORD *pdwDataBuffer, DWORD dwBitsPerSample, DWORD dwNumberOfSamples, WAVE_PLAYBACK_CHANNEL_FREQ_DATA *pWaveChannelFreqData, DWORD dwNumberOfChannels, DWORD dwAttenuationDB, DWORD dwFrequencyNumber);
	DWORD dwGetPossessionOfAudioResources(DWORD dwTimeout);
	void vReleasePossessionOfAudioResources (void);
	void vStraightenUpTestVariables(void);
//	static void CALLBACK vWaveInCallback(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
	DWORD dwTotalDCOffset(DWORD *pdwSourceBuffer, SLONG *pslDestBuffer, DWORD dwBitsPerSample, DWORD dwFFTSize, DWORD dwNumberOfChannels, double *pdblDCOffsetPercents);
	void vCalculateRecordNoiseLimit(double *pdblNoiseLimitValues, DWORD dwFFTSize, DWORD dwBitsPerSample, DWORD dwSampleRate, WAVE_RECORD_NOISE_LIMIT* pwrnlWaveRecordNoiseLimit, DWORD dwNumberOfNoiseRanges);
	DWORD dwComputeFFT(SLONG *pslInputData, DWORD *pdwMagnitude, DWORD dwFFTBits, DWORD dwFFTSize, double dblTwoPI, double dblPIOverTwo, DWORD dwWindowType);
	DWORD dwCompareFrequenciesAndNoise(DWORD *pdwMagnitude, DWORD dwFFTSize, WAVE_RECORD_CONFIGURATION_DATA wrcdWaveRecordInfo, DWORD dwChannel, double *pdblNoiseLimitValues);
	DWORD dwComputeSignalToNoise(DWORD *pdwMagnitude, DWORD dwFFTSize, double dblFrequencyShiftPercentAllowed, double *pdblFrequencies, DWORD dwNumberOfFrequencies, DWORD dwSampleRate, double *pdblNoiseLimitValues, double dblFrequencyShiftHzAllowed, double *pdblSignalToNoiseRatioToReturn);
	DWORD dwComputeSignalToNoiseIgnoringCrosstalk(DWORD *pdwMagnitude, DWORD dwFFTSize, double dblFrequencyShiftPercentAllowed, WAVE_RECORD_CHANNEL_FREQ_DATA *WaveRecordChannelFreqData, DWORD dwRecordNumberOfChannels, WAVE_PLAYBACK_CHANNEL_FREQ_DATA *WavePlaybackChannelFreqData, DWORD dwPlaybackNumberOfChannels, DWORD dwChannelNumber, DWORD dwSampleRate, double *pdblNoiseLimitValues, double dblFrequencyShiftHzAllowed, double *pdblSignalToNoiseRatioToReturn);
	DWORD dwComputeChannelSeparation(DWORD *pdwMagnitude, DWORD dwFFTSize, double dblFrequencyShiftPercentAllowed, WAVE_RECORD_CHANNEL_FREQ_DATA *WaveRecordChannelFreqData, DWORD dwRecordNumberOfChannels, WAVE_PLAYBACK_CHANNEL_FREQ_DATA *WavePlaybackChannelFreqData, DWORD dwPlaybackNumberOfChannels, DWORD dwChannelNumber, DWORD dwSampleRate, double dblFrequencyShiftHzAllowed, double *pdblChannelSeparationToReturn, double *pdblNoiseLimitValues);
	DWORD dwComputeFrequencyResponse(DWORD *pdwMagnitude, DWORD dwFFTSize, double dblFrequencyShiftPercentAllowed, double *pdblFrequencies, DWORD dwNumberOfFrequencies, double dblBaseMagnitude, DWORD dwBitsPerSample, DWORD dwSampleRate, double dblFrequencyShiftHzAllowed, double *pdblFrequencyResponseToReturn, double *pdblNoiseLimitValues);
	DWORD dwComputeCRC(unsigned char *pucBuffer, DWORD dwNumBytes, DWORD dwCRC);
	static BOOL CALLBACK bDSEnumCallback(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext);
};

#endif // _WINAUDIO_H_
