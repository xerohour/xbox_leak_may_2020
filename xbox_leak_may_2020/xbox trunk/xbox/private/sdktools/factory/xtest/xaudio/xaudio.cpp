// (c) Copyright 2000-2001 Intel Corp. All rights reserved.
//
// Title: xAudio
//
// History:
//
// 10/XX/00 V1.00 BLI Release V1.00.

#include "..\stdafx.h"
#include "..\testobj.h"
#include "..\statkeys.h"
#include "xaudio.h"
#include <float.h>
#include <math.h>

IMPLEMENT_MODULEUNLOCKED (CXModuleAudio);

// Define the initial value to be passed into the CRC calculation routine.
#define INITIAL_CRC    0x55555555UL

// Audio test number definitions
#define ANALOG_PLAYBACK_TEST  1
#define DIGITAL_PLAYBACK_TEST 2
#define ANALOG_SILENCE_TEST 3
#define DIGITAL_ACCURACY_TEST 4

// Configuration parameter default values
#define DEFAULT_MUTEX_TIMEOUT                       180000      // The number of ms to wait for a mutex
#define DEFAULT_AUDIO_MUX_COM_PORT                  0
#define DEFAULT_AUDIO_MUX_LOW_DWORD                 0x88888888  // Disable all channels
#define DEFAULT_AUDIO_MUX_HIGH_DWORD                0x88888888  // Disable all channels

#define	DEFAULT_FREQUENCY_SHIFT_PERCENT_ALLOWED 3
#define	DEFAULT_DELAY_BETWEEN_PLAYBACK_START_AND_RECORD_START  700 // ms

#define DEFAULT_PLAYBACK_TEST_CONFIGURATION     1  // The first non-custom entry in gPlaybackTestConfigurations[] is the default
#define DEFAULT_RECORD_TEST_CONFIGURATION       1  // The first non-custom entry in gRecordTestConfigurations[] is the default

// Some more general constants
#define TWO_PI         (6.28318530718)
#define PI_OVER_TWO    (1.570796326795)
#define PIT_FFT_BITS    9         /* Make FFT_SIZE 512 values */
#define PIT_FFT_SIZE   (1 << PIT_FFT_BITS)
#define FFT_BITS       12         /* Make FFT_SIZE 4096 values */
#define FFT_SIZE       (1 << FFT_BITS)
#define WAVE_OUT_NUMBER_OF_SAMPLES  FFT_SIZE  // The number of samples in a single buffer; we will probably use multiple buffers of this size; this must be a multiple of FFT_SIZE
#define WAVE_IN_NUMBER_OF_SAMPLES   FFT_SIZE*2
#define IGNORE_BIN_VALUE  DBL_MAX
#define AVERAGE_FREQUENCY_RESPONSE_LEVEL  -1000000

#define NO_WINDOW          0    /* Represents no window function being applied to FFT data */
#define COS_SQUARED_WINDOW 1    /* Represents a cosine squared window function being applied to FFT data */

const wchar_t * CXModuleAudio::pwchFFTWindowFunctionNames[] = {
	L"NONE",
	L"COS_SQUARED",
	NULL
};

const wchar_t * CXModuleAudio::gpchMonoLabel[] = {L"(mono)"};
const wchar_t * CXModuleAudio::gpchStereoLabel[] = {L"(stereo left)", L"(stereo right)"};

const double CXModuleAudio::gdblStereoTwelveFrequencyQuicktestFreqCh0[] = {43, 75, 129, 215, 366, 624, 1055, 1809, 3079, 5243, 8947, 15256};
const double CXModuleAudio::gdblStereoTwelveFrequencyQuicktestFreqCh1[] = {54, 97, 161, 280, 474, 807, 1378, 2358, 4016, 6848, 11682, 19918}; 
const double CXModuleAudio::gdblStereoTwelveFrequencyQuicktestAmp[] = {-21.58, -21.58, -21.58, -21.58, -21.58, -21.58, -21.58, -21.58, -21.58, -21.58, -21.58, -21.58};  // Full amplitude for a 12 frequency signal

const WAVE_PLAYBACK_CHANNEL_FREQ_DATA CXModuleAudio::gwpcfdStereoTwelveFrequencyQuicktest[2] = {
	{sizeof(gdblStereoTwelveFrequencyQuicktestFreqCh0)/sizeof(double), (double *)gdblStereoTwelveFrequencyQuicktestFreqCh0},
	{sizeof(gdblStereoTwelveFrequencyQuicktestFreqCh1)/sizeof(double), (double *)gdblStereoTwelveFrequencyQuicktestFreqCh1}
};

const WAVE_RECORD_CHANNEL_FREQ_DATA CXModuleAudio::gwrcfdStereoTwelveFrequencyQuicktest[2] = {
	{sizeof(gdblStereoTwelveFrequencyQuicktestFreqCh0)/sizeof(double), (double *)gdblStereoTwelveFrequencyQuicktestFreqCh0, (double *)gdblStereoTwelveFrequencyQuicktestAmp},
	{sizeof(gdblStereoTwelveFrequencyQuicktestFreqCh1)/sizeof(double), (double *)gdblStereoTwelveFrequencyQuicktestFreqCh1, (double *)gdblStereoTwelveFrequencyQuicktestAmp}
};

const WAVE_RECORD_NOISE_LIMIT CXModuleAudio::gwrnlStereoTwelveFrequencyQuicktest[] = {
	{20, 20000, -50, -50}
};

const WAVE_PLAYBACK_CONFIGURATION_DATA CXModuleAudio::gwpcdStereoTwelveFrequencyQuicktest = {
	NULL, 0, NULL, 2, 16, 48000, 0, (WAVE_PLAYBACK_CHANNEL_FREQ_DATA *)gwpcfdStereoTwelveFrequencyQuicktest
};

const WAVE_RECORD_CONFIGURATION_DATA CXModuleAudio::gwrcdStereoTwelveFrequencyQuicktest = {
	0, 0, 2, (wchar_t **)gpchStereoLabel, 16, 44100, 0, NO_WINDOW, 40, 15, 0, 0, 0, 0, AVERAGE_FREQUENCY_RESPONSE_LEVEL, 3, 1, sizeof(gwrnlStereoTwelveFrequencyQuicktest)/sizeof(WAVE_RECORD_NOISE_LIMIT), (WAVE_RECORD_NOISE_LIMIT *)gwrnlStereoTwelveFrequencyQuicktest, (WAVE_RECORD_CHANNEL_FREQ_DATA *)gwrcfdStereoTwelveFrequencyQuicktest
};

const PLAYBACK_TEST_CONFIGURATIONS CXModuleAudio::gPlaybackTestConfigurations[] = {
	{"CUSTOM", (WAVE_PLAYBACK_CONFIGURATION_DATA *)&gwpcdStereoTwelveFrequencyQuicktest},
	{"STEREO 12 FREQUENCY QUICKTEST", (WAVE_PLAYBACK_CONFIGURATION_DATA *)&gwpcdStereoTwelveFrequencyQuicktest},
	{"", NULL}
};

const RECORD_TEST_CONFIGURATIONS CXModuleAudio::gRecordTestConfigurations[] = {
	{"CUSTOM", (WAVE_RECORD_CONFIGURATION_DATA *)&gwrcdStereoTwelveFrequencyQuicktest},
	{"STEREO 12 FREQUENCY QUICKTEST", (WAVE_RECORD_CONFIGURATION_DATA *)&gwrcdStereoTwelveFrequencyQuicktest},
	{"", NULL}
};

// CRC look-up table
static const unsigned long crctab[256] = {
	0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
	0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988, 0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
	0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
	0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
	0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172, 0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
	0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
	0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
	0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924, 0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
	0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
	0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
	0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e, 0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
	0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
	0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
	0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0, 0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
	0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
	0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
	0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a, 0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
	0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
	0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
	0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc, 0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
	0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
	0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
	0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236, 0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
	0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
	0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
	0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38, 0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
	0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
	0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
	0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2, 0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
	0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
	0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
	0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94, 0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
};

/*
Parameters fetched by this routine

testname.max_test_retries - The number of times to retry tests that fail
testname.delay_between_playback_start_and_record_start - The amount of time, in ms, to delay before recording audio after playback has started
testname.mutex_timeout - The amount of time, in ms, to wait for the audio resources to become free for use before aborting
testname.audio_mux_com_port - the serial port to which the audio mux is attached, if applicable
testname.playback.wave_output_device_name - the wave output device name assigned by the driver
testname.playback.number_of_channels - number of channels to play for a specific test
testname.playback.sample_rate - the playback rate for the test in KHz
testname.playback.bits_per_sample - the resolution of the playback for the test (8 and 16 are common)
testname.playback.digital_attenuation - the amount of attenuation, in dB, to apply to the digital output when generating the output waveform data
testname.playback.ch[kk]_freq[mm] - the (mm + 1) playback frequencies for the channel of this test to generate

testname.record.wave_input_device_name - the wave input device name assigned by the driver
testname.record.number_of_channels - number of channels to record for a specific test
testname.record.sample_rate - the record rate for the test in KHz
testname.record.bits_per_sample - the resolution of the record for the test (8 and 16 are common)
testname.record.fft_window_function - the FFT windowing function to apply to recorded FFT information.  "NONE" and COS_SQUARED" are common.
testname.record.amplitude_tolerance - the number of dB above or below the chkk_amplitudemm values a frequency can reside
testname.record.max_dc_offset_percent - the amount of DC offset vs half of full scale that a recorded sample can exhibit. 100% effectively means don't bother with DC offset comparisons
testname.record.signal_to_noise_ratio - the allowed S/N ratio.  One value for an entire test instead of one per channel
testname.record.signal_to_noise_ratio_ignoring_crosstalk - the allowed S/N ratio when crosstalk from other channels is ignored.  One value for an entire test instead of one per channel
testname.record.channel_separation - the minimum allowed channel separation between all channels in dB.
testname.record.frequency_response_tolerance - the number of dB above or below the frequency_response_level (or average of all fundamental frequencies) that all fundamentals should fall within
testname.record.frequency_response_level - the absolute dB level that all fundamentals should be recorded at, + or - the frequency_response_tolerance.  If this is not specified or is set to AVERAGE, the average of all fundamentals will be used for this value
testname.record.frequency_shift_percent_allowed - the percent a frequency may be off from its expected value
testname.record.frequency_shift_hz_allowed - the number of hertz any particular recorded frequency is permitted to exist above or below the expected value.  This is helpful when using FFT window functions.  This works along with the testname.record.frequency_shift_percent_allowed parameter
testname.record.crc - the CRC value to expect from an audio input buffer during digital audio testing.
// The following 4 parameters specify lines that describe where noise cutoff limits should be when analyzing recorded data
testname.record.noise_limit_start_freq[nn] - the starting frequency of range (nn) for maximum noise allowed parameters
testname.record.noise_limit_stop_freq[nn] - the ending frequency of range (nn) for maximum noise allowed parameters
testname.record.noise_limit_start_level[nn] - the starting level (in dB relative to ideal 0 dB) of range (nn) for maximum noise allowed parameters
testname.record.noise_limit_stop_level[nn] - the ending level (in dB relative to ideal 0 dB) of range (nn) for maximum noise allowed parameters
testname.record.ch[kk]_amplitude[mm] - the (mm + 1) recorded frequencies' amplitudes for the channel of this test to expect
testname.record.ch[kk]_freq[mm] - the (mm + 1) recorded frequencies for the channel of this test to expect

testname.mixer.device_name - the name assigned to the mixer
testname.mixer.mixercontrolname_ID_ch[kk]_item[mm] - channel kk item mm mixer controls are listed in this format
testname.mixer.mux_control_low_dword - the low 32 bits of the audio mux control QWORD for a particular test (controls the low 8 audio mux ports)
testname.mixer.mux_control_high_dword -  the high 32 bits of the audio mux control QWORD for a particular test (controls the high 16 digital output lines and 16 future expansion signals)

All of the stuff allocated in this routine is cleaned up in the module's destructor
This routine will only execute once per test invocation so it is not re-enterent.

*/
bool CXModuleAudio::InitializeParameters ()
{
	#define SUBTEST_NAME_SIZE   80
	#define GENERAL_RECEIVE_STRING_SIZE   81
	#define PARAMETER_TEMP_STRING_SIZE     256
	#define MIXER_PARAMETER_STORAGE_SIZE     32768

	wchar_t wszTempString[PARAMETER_TEMP_STRING_SIZE];
	wchar_t wszSubtestName[SUBTEST_NAME_SIZE];
//	wchar_t wszReceiveString[GENERAL_RECEIVE_STRING_SIZE];
	LPCTSTR pwszValue = NULL;
	LPCTSTR pwszStringParam;
	DWORD j, k;
	double *pdblDtaPlaybackChannelFrequencies;

	if (!CTestObj::InitializeParameters ())
		return false;

	dwLoopCounter = 0;
	gpdwPI = NULL; // This parameter is not re-initialized with each retry loop, so initialize it here
	gdwMaxTestTrys = 1; // This is actually the number of retries + 1
	gdwIgnoreError = FALSE;
	gpWavePlaybackConfigurationData = NULL;
	gpWaveRecordConfigurationData = NULL;
	gpAudioMuxConfigurationInformation = NULL;
	gpwszAudioMixerName = NULL;
	gdwNumberOfMixerControls = 0;
	if ((pCSMixer = new CStore(MIXER_PARAMETER_STORAGE_SIZE)) == NULL)
	{
		err_AllocMixerStorage(MIXER_PARAMETER_STORAGE_SIZE);
		return false;
	}

	swprintf(wszSubtestName, L"%s", GetTestName());

	// Get dwMaxTestRetries
	swprintf(wszTempString, L"%s.%s", wszSubtestName, L"max_test_retries");
	gdwMaxTestTrys = (DWORD)GetCfgUint (wszTempString, 0);
	if (gdwMaxTestTrys > 10)
		gdwMaxTestTrys = 0;
	gdwMaxTestTrys++; // Add 1 because this represents trys, not retries

	// Get gdwDelayBetweenPlaybackStartAndRecordStart
	swprintf(wszTempString, L"%s.%s", wszSubtestName, L"delay_between_playback_start_and_record_start");
	gdwDelayBetweenPlaybackStartAndRecordStart = (DWORD)GetCfgUint (wszTempString, DEFAULT_DELAY_BETWEEN_PLAYBACK_START_AND_RECORD_START);

	// Get gdwMutexTimeout
	swprintf(wszTempString, L"%s.%s", wszSubtestName, L"mutex_timeout");
	gdwMutexTimeout = (DWORD)GetCfgUint (wszTempString, DEFAULT_MUTEX_TIMEOUT);

	// Get gdwAudioMuxComPort
	swprintf(wszTempString, L"%s.%s", wszSubtestName, L"audio_mux_com_port");
	gdwAudioMuxComPort = (DWORD)GetCfgUint (wszTempString, DEFAULT_AUDIO_MUX_COM_PORT);


	// ************************* Now get playback parameters ************************
	// Allocate a WAVE_PLAYBACK_CONFIGURATION_DATA structure
	if ((gpWavePlaybackConfigurationData = new WAVE_PLAYBACK_CONFIGURATION_DATA) == NULL)  // Create a structure to contain playback parameters
	{
		err_AllocWavePlayConfigDataL(sizeof(WAVE_PLAYBACK_CONFIGURATION_DATA));
		return false;
	}
	gpWavePlaybackConfigurationData->WaveChannelFreqData = NULL;  // Do this for safety

	// Get the name of the audio output device to use
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"playback", L"wave_output_device_name");
	gpWavePlaybackConfigurationData->pwszAudioOutDevName = GetCfgString (wszTempString, L"none");

	// All test cases will be custom with no built-in configurations.

	// Get dwSampleRate
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"playback", L"sample_rate");
	gpWavePlaybackConfigurationData->dwSampleRate = (DWORD)GetCfgUint (wszTempString, (UINT)gPlaybackTestConfigurations[DEFAULT_PLAYBACK_TEST_CONFIGURATION].data->dwSampleRate);
	
	// Get dwBitsPerSample
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"playback", L"bits_per_sample");
	gpWavePlaybackConfigurationData->dwBitsPerSample = (DWORD)GetCfgUint (wszTempString, (UINT)gPlaybackTestConfigurations[DEFAULT_PLAYBACK_TEST_CONFIGURATION].data->dwBitsPerSample);
	
	// Get dwDigitalAttenuation
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"playback", L"digital_attenuation");
	gpWavePlaybackConfigurationData->dwDigitalAttenuation = (DWORD)GetCfgUint (wszTempString, (UINT)gPlaybackTestConfigurations[DEFAULT_PLAYBACK_TEST_CONFIGURATION].data->dwDigitalAttenuation);
	
	// Get dwNumberOfChannels
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"playback", L"number_of_channels");
	gpWavePlaybackConfigurationData->dwNumberOfChannels = (DWORD)GetCfgUint (wszTempString, (UINT)gPlaybackTestConfigurations[DEFAULT_PLAYBACK_TEST_CONFIGURATION].data->dwNumberOfChannels);

	// Allocate an array of structures to contain playback frequency information
	if ((gpWavePlaybackConfigurationData->WaveChannelFreqData = new WAVE_PLAYBACK_CHANNEL_FREQ_DATA[gpWavePlaybackConfigurationData->dwNumberOfChannels]) == NULL)  // Create an array of WAVE_PLAYBACK_CHANNEL_FREQ_DATA structures; one per channel
	{
		err_AllocWavePlayChannelFreqDataL(sizeof(WAVE_PLAYBACK_CHANNEL_FREQ_DATA)*gpWavePlaybackConfigurationData->dwNumberOfChannels);
		return false;
	}
	for (j = 0; j < gpWavePlaybackConfigurationData->dwNumberOfChannels; j++) // Set all of the double array pointers to NULL for safety
	{
		gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency = NULL;
		gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblSamplesPerPeriod = NULL;
	}
	// Read in playback configuration parameters for all channels of this test
	for (j = 0; j < gpWavePlaybackConfigurationData->dwNumberOfChannels; j++)
	{
		swprintf(wszTempString, L"%s.playback.ch[%2.2lu]_freq[%2.2lu]", wszSubtestName, j, 0);

		// Need to check for the presence of the parameter, and then load with defaults if it does not exist
		if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL)  // No frequencies are specified for this channel, so set default frequencies
		{
			if ((j & 0x1) == 0)  // Channels 0,2,4,6,8,... get this set of frequencies by default
			{
				pdblDtaPlaybackChannelFrequencies = gPlaybackTestConfigurations[DEFAULT_PLAYBACK_TEST_CONFIGURATION].data->WaveChannelFreqData[0].pdblFrequency;
				gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies = gPlaybackTestConfigurations[DEFAULT_PLAYBACK_TEST_CONFIGURATION].data->WaveChannelFreqData[0].dwNumberOfFrequencies;
			}
			else  // Channels 1,3,5,7,9,... get this set of frequencies by default
			{
				pdblDtaPlaybackChannelFrequencies = gPlaybackTestConfigurations[DEFAULT_PLAYBACK_TEST_CONFIGURATION].data->WaveChannelFreqData[1].pdblFrequency;
				gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies = gPlaybackTestConfigurations[DEFAULT_PLAYBACK_TEST_CONFIGURATION].data->WaveChannelFreqData[1].dwNumberOfFrequencies;
			}
			if ((gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency = new double[gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies]) == NULL)  // Create an array of double to contain frequencies
			{
				err_AllocDoublesForPlaybackFreqL(sizeof(double)*gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies);
				return false;
			}
			if ((gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblSamplesPerPeriod = new double[gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies]) == NULL)  // Create an array of double to contain SamplesPerPeriod
			{
				err_AllocDoublesForPlaybackSPPL(sizeof(double)*gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies);
				return false;
			}
			for (k = 0; k < gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies; k++)
				gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency[k] = pdblDtaPlaybackChannelFrequencies[k];
		}
		else  // Frequencies have been specified for this channel, so read them in
		{
			for (k = 1; k > 0; k++) // Count the number of frequencies on this channel; terminate only with the break inside the loop
			{
				swprintf(wszTempString, L"%s.playback.ch[%2.2lu]_freq[%2.2lu]", wszSubtestName, j, k);
				if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL)
						break;
			}
			gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies = k;
			if ((gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency = new double[k]) == NULL)  // Create an array of double to contain frequencies
			{
				err_AllocDoublesForPlaybackFreqL(sizeof(double)*k);
				return false;
			}
			if ((gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblSamplesPerPeriod = new double[k]) == NULL)  // Create an array of double to contain SamplesPerPeriod
			{
				err_AllocDoublesForPlaybackSPPL(sizeof(double)*k);
				return false;
			}
			// Now that we know how many frequencies there are, let's read them in
			for (k = 0; k < gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies; k++)
			{
				swprintf(wszTempString, L"%s.playback.ch[%2.2lu]_freq[%2.2lu]", wszSubtestName, j, k);
				if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL)
				{
					// We read this parameter successfully before.  Something is wrong if we can't read it now
					err_CantReadPlaybackFreqData();
					return false;
				}
				if ((gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency[k] = wcstod(pwszStringParam, NULL)) == 0)
					gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency[k] = FLT_MIN;
			}
		}
	}
	
	// ************************* Now get record parameters ************************
	// Allocate a WAVE_RECORD_CONFIGURATION_DATA structure
	if ((gpWaveRecordConfigurationData = new WAVE_RECORD_CONFIGURATION_DATA) == NULL)  // Create a structure to contain record parameters
	{
		err_AllocWaveRecordConfigDataL(sizeof(WAVE_RECORD_CONFIGURATION_DATA));
		return false;
	}
	gpWaveRecordConfigurationData->pchChannelLabel = NULL;  // Do this for safety
	gpWaveRecordConfigurationData->WaveRecordNoiseLimit = NULL;  // Do this for safety
	gpWaveRecordConfigurationData->WaveChannelFreqData = NULL;  // Do this for safety
	
	// Get the name of the audio input device to use
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"wave_input_device_name");
	gpWaveRecordConfigurationData->pwszAudioInDevName = GetCfgString (wszTempString, L"none");

	// All test cases will be custom with no built-in configurations.

	// Get dwSampleRate
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"sample_rate");
	gpWaveRecordConfigurationData->dwSampleRate = (DWORD)GetCfgUint (wszTempString, (UINT)gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dwSampleRate);

	// Get dwBitsPerSample
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"bits_per_sample");
	gpWaveRecordConfigurationData->dwBitsPerSample = (DWORD)GetCfgUint (wszTempString, (UINT)gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dwBitsPerSample);
	
	// Get dwNumberOfChannels
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"number_of_channels");
	gpWaveRecordConfigurationData->dwNumberOfChannels = (DWORD)GetCfgUint (wszTempString, (UINT)gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dwNumberOfChannels);
	// Set up the string channel label
	switch (gpWaveRecordConfigurationData->dwNumberOfChannels)
	{
		case 1:
			gpWaveRecordConfigurationData->pchChannelLabel = (wchar_t **)gpchMonoLabel;
			break;
		case 2:
			gpWaveRecordConfigurationData->pchChannelLabel = (wchar_t **)gpchStereoLabel;
			break;
		default:
			gpWaveRecordConfigurationData->pchChannelLabel = NULL;
			break;
	}

	// Get dwCRC
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"crc");
	gpWaveRecordConfigurationData->dwCRC = (DWORD)GetCfgUint (wszTempString, (UINT)gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dwCRC);

	// Get dwFFTWindowFunction
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"fft_window_function");
	if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL) // Parameter did not exist
		gpWaveRecordConfigurationData->dwFFTWindowFunction = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dwFFTWindowFunction;
	else // The parameter was defined, what is the numerical equivalent?
	{
		j = 0;
		while (pwchFFTWindowFunctionNames[j] != NULL)
		{
			if (wcsicmp(pwchFFTWindowFunctionNames[j], pwszStringParam) == 0)  // We found a match
			{
				gpWaveRecordConfigurationData->dwFFTWindowFunction = j;
				break;
			}
			j++;
		}
		if (pwchFFTWindowFunctionNames[j] == NULL) // We didn't read in a valid FFT window type
			gpWaveRecordConfigurationData->dwFFTWindowFunction = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dwFFTWindowFunction;
	}

	// Get dblAmplitudeTolerance
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"amplitude_tolerance");
	if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL) // Parameter did not exist
		gpWaveRecordConfigurationData->dblAmplitudeTolerance = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dblAmplitudeTolerance;
	else
	{
		if ((gpWaveRecordConfigurationData->dblAmplitudeTolerance = wcstod(pwszStringParam, NULL)) == 0)
			gpWaveRecordConfigurationData->dblAmplitudeTolerance = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dblAmplitudeTolerance;
	}
	if (gpWaveRecordConfigurationData->dblAmplitudeTolerance < 0)  // Make this value positive
		gpWaveRecordConfigurationData->dblAmplitudeTolerance = 0 - gpWaveRecordConfigurationData->dblAmplitudeTolerance;

	// Get dblMaxDCOffsetPercent
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"max_dc_offset_percent");
	if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL) // Parameter did not exist
		gpWaveRecordConfigurationData->dblMaxDCOffsetPercent = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dblMaxDCOffsetPercent;
	else
	{
		if ((gpWaveRecordConfigurationData->dblMaxDCOffsetPercent = wcstod(pwszStringParam, NULL)) == 0)  // Offset percent allowed can't be 0
			gpWaveRecordConfigurationData->dblMaxDCOffsetPercent = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dblMaxDCOffsetPercent;
	}
	if (gpWaveRecordConfigurationData->dblMaxDCOffsetPercent < 0)  // Convert to a positive number
		gpWaveRecordConfigurationData->dblMaxDCOffsetPercent = 0 - gpWaveRecordConfigurationData->dblMaxDCOffsetPercent;

	// Get dblSignalToNoiseRatio
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"signal_to_noise_ratio");
	if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL) // Parameter did not exist
		gpWaveRecordConfigurationData->dblSignalToNoiseRatio = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dblSignalToNoiseRatio;
	else
	{
		gpWaveRecordConfigurationData->dblSignalToNoiseRatio = wcstod(pwszStringParam, NULL);
	}
	if (gpWaveRecordConfigurationData->dblSignalToNoiseRatio < 0)  // Make the value positive
		gpWaveRecordConfigurationData->dblSignalToNoiseRatio = 0 - gpWaveRecordConfigurationData->dblSignalToNoiseRatio;
	// Get dblSignalToNoiseRatioIgnoringCrosstalk
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"signal_to_noise_ratio_ignoring_crosstalk");
	if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL) // Parameter did not exist
		gpWaveRecordConfigurationData->dblSignalToNoiseRatioIgnoringCrosstalk = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dblSignalToNoiseRatioIgnoringCrosstalk;
	else
	{
		gpWaveRecordConfigurationData->dblSignalToNoiseRatioIgnoringCrosstalk = wcstod(pwszStringParam, NULL);
	}
	if (gpWaveRecordConfigurationData->dblSignalToNoiseRatioIgnoringCrosstalk < 0)  // Make the value positive
		gpWaveRecordConfigurationData->dblSignalToNoiseRatioIgnoringCrosstalk = 0 - gpWaveRecordConfigurationData->dblSignalToNoiseRatioIgnoringCrosstalk;

	// Get dblChannelSeparation
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"channel_separation");
	if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL) // Parameter did not exist
		gpWaveRecordConfigurationData->dblChannelSeparation = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dblChannelSeparation;
	else
	{
		gpWaveRecordConfigurationData->dblChannelSeparation = wcstod(pwszStringParam, NULL);
	}
	if (gpWaveRecordConfigurationData->dblChannelSeparation < 0)  // Make the value positive
		gpWaveRecordConfigurationData->dblChannelSeparation = 0 - gpWaveRecordConfigurationData->dblChannelSeparation;

	// Get dblFrequencyResponseTolerance
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"frequency_response_tolerance");
	if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL) // Parameter did not exist
		gpWaveRecordConfigurationData->dblFrequencyResponseTolerance = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dblFrequencyResponseTolerance;
	else
	{
		gpWaveRecordConfigurationData->dblFrequencyResponseTolerance = wcstod(pwszStringParam, NULL);
	}
	if (gpWaveRecordConfigurationData->dblFrequencyResponseTolerance < 0)  // Make the value positive
		gpWaveRecordConfigurationData->dblFrequencyResponseTolerance = 0 - gpWaveRecordConfigurationData->dblFrequencyResponseTolerance;

	// Get dblFrequencyResponseLevel
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"frequency_response_level");
	if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL) // Parameter did not exist
		gpWaveRecordConfigurationData->dblFrequencyResponseLevel = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dblFrequencyResponseLevel;
	else
	{
		if (wcsicmp(L"average", pwszStringParam) == 0)  // Average the frequencies
			gpWaveRecordConfigurationData->dblFrequencyResponseLevel = AVERAGE_FREQUENCY_RESPONSE_LEVEL;
		else
			gpWaveRecordConfigurationData->dblFrequencyResponseLevel = wcstod(pwszStringParam, NULL);
	}
	if (gpWaveRecordConfigurationData->dblFrequencyResponseLevel > 0)  // Make the value negative
		gpWaveRecordConfigurationData->dblFrequencyResponseLevel = 0 - gpWaveRecordConfigurationData->dblFrequencyResponseLevel;

	// Get dblFrequencyShiftPercentAllowed
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"frequency_shift_percent_allowed");
	if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL)
		gpWaveRecordConfigurationData->dblFrequencyShiftPercentAllowed = DEFAULT_FREQUENCY_SHIFT_PERCENT_ALLOWED;
	else
		gpWaveRecordConfigurationData->dblFrequencyShiftPercentAllowed = wcstod(pwszStringParam, NULL);
	if (gpWaveRecordConfigurationData->dblFrequencyShiftPercentAllowed == 0)  // 0 percent is not an acceptable value
		gpWaveRecordConfigurationData->dblFrequencyShiftPercentAllowed = DEFAULT_FREQUENCY_SHIFT_PERCENT_ALLOWED;

	// Get dblFrequencyShiftHzAllowed
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"frequency_shift_hz_allowed");
	if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL) // Parameter did not exist
		gpWaveRecordConfigurationData->dblFrequencyShiftHzAllowed = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dblFrequencyShiftHzAllowed;
	else
	{
		gpWaveRecordConfigurationData->dblFrequencyShiftHzAllowed = wcstod(pwszStringParam, NULL);
	}
	if (gpWaveRecordConfigurationData->dblFrequencyShiftHzAllowed < 0)  // Make the value positive
		gpWaveRecordConfigurationData->dblFrequencyShiftHzAllowed = 0 - gpWaveRecordConfigurationData->dblFrequencyShiftHzAllowed;

	// Get noise limit frequencies and levels
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"record", L"noise_limit_start_freq[00]");
	if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL) // Parameter did not exist
	{
		gpWaveRecordConfigurationData->dwNumberOfNoiseRanges = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->dwNumberOfNoiseRanges;
		if ((gpWaveRecordConfigurationData->WaveRecordNoiseLimit = new WAVE_RECORD_NOISE_LIMIT[gpWaveRecordConfigurationData->dwNumberOfNoiseRanges]) == NULL)  // Create an array of WAVE_RECORD_NOISE_LIMIT to contain noise limit info
		{
			err_AllocWaveRecordNoiseLimitL(sizeof(WAVE_RECORD_NOISE_LIMIT)*gpWaveRecordConfigurationData->dwNumberOfNoiseRanges);
			return false;
		}
		for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfNoiseRanges; j++)  // Set all noise range parameters to their default values
		{
			gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStartFreq = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->WaveRecordNoiseLimit[j].dblNoiseLimitStartFreq;
			gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStopFreq = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->WaveRecordNoiseLimit[j].dblNoiseLimitStopFreq;
			gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStartLevel = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->WaveRecordNoiseLimit[j].dblNoiseLimitStartLevel;
			gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStopLevel = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->WaveRecordNoiseLimit[j].dblNoiseLimitStopLevel;
		}
	}
	else // Noise ranges have been specified, so read them in
	{
		for (j = 1; j > 0; j++) // Loop until the break is reached; count the number of noise ranges
		{
			swprintf(wszTempString, L"%s.record.noise_limit_start_freq[%2.2lu]", wszSubtestName, j);
			if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL) // Parameter did not exist
				break;
		}
		gpWaveRecordConfigurationData->dwNumberOfNoiseRanges = j;  // Now we know how many noise ranges there are
		if ((gpWaveRecordConfigurationData->WaveRecordNoiseLimit = new WAVE_RECORD_NOISE_LIMIT[j]) == NULL)  // Create an array of WAVE_RECORD_NOISE_LIMIT to contain noise limit info
		{
			err_AllocWaveRecordNoiseLimitL(sizeof(WAVE_RECORD_NOISE_LIMIT)*j);
			return false;
		}
		for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfNoiseRanges; j++)  // Read in all noise range parameters
		{
			swprintf(wszTempString, L"%s.record.noise_limit_start_freq[%2.2lu]", wszSubtestName, j);
			if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL) // Parameter did not exist
			{
				// We read this parameter successfully before.  Something is wrong if we can't read it now
				err_CantReadRecordNoiseLimit();
				return false;
			}
			gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStartFreq = wcstod(pwszStringParam, NULL);
			swprintf(wszTempString, L"%s.record.noise_limit_stop_freq[%2.2lu]", wszSubtestName, j);
			if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL) // Parameter did not exist
				gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStopFreq = gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStartFreq;  // The end frequency of this range is not specified, so effectively make the range 0 Hz
			else
				gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStopFreq = wcstod(pwszStringParam, NULL);
			swprintf(wszTempString, L"%s.record.noise_limit_start_level[%2.2lu]", wszSubtestName, j);
			if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL) // Parameter did not exist
				gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStartLevel = 100;  // 100 dB above the effective 0 dB level makes the acceptable noise range almost infinite (so we won't get any failures)
			else
				gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStartLevel = wcstod(pwszStringParam, NULL);
			swprintf(wszTempString, L"%s.record.noise_limit_stop_level[%2.2lu]", wszSubtestName, j);
			if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL) // Parameter did not exist
				gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStopLevel = gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStartLevel;  // The end level will default to the same amplitude as the start level
			else
				gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStopLevel = wcstod(pwszStringParam, NULL);
		}
	}

	if ((gpWaveRecordConfigurationData->WaveChannelFreqData = new WAVE_RECORD_CHANNEL_FREQ_DATA[gpWaveRecordConfigurationData->dwNumberOfChannels]) == NULL)  // Create an array of WAVE_RECORD_CHANNEL_FREQ_DATA structures; one per channel
	{
		err_AllocWaveRecordChannelFreqDataL(sizeof(WAVE_RECORD_CHANNEL_FREQ_DATA)*gpWaveRecordConfigurationData->dwNumberOfChannels);
		return false;
	}
	for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfChannels; j++) // Set all of the double array pointers to NULL for safety
	{
		gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblFrequency = NULL;
		gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblAmplitude = NULL;
	}

	// Read in record configuration parameters for all channels of this test
	for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfChannels; j++)
	{
		swprintf(wszTempString, L"%s.record.ch[%2.2lu]_freq[%2.2lu]", wszSubtestName, j, 0);

		// Need to check for the presence of the parameter, and then load with defaults if it does not exist
		if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL)  // No frequencies are specified for this channel
		{
			gpWaveRecordConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies = 0;
		}
/*
		{
			if ((j & 0x1) == 0)  // Channels 0,2,4,6,8,... get this set of frequencies by default
			{
				pdblDtaRecordChannelFrequencies = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->WaveChannelFreqData[0].pdblFrequency;
				pdblDtaRecordChannelAmplitudes = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->WaveChannelFreqData[0].pdblAmplitude;
				gpWaveRecordConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->WaveChannelFreqData[0].dwNumberOfFrequencies;
			}
			else  // Channels 1,3,5,7,9,... get this set of frequencies by default
			{
				pdblDtaRecordChannelFrequencies = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->WaveChannelFreqData[1].pdblFrequency;
				pdblDtaRecordChannelAmplitudes = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->WaveChannelFreqData[1].pdblAmplitude;
				gpWaveRecordConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->WaveChannelFreqData[1].dwNumberOfFrequencies;
			}
			if ((gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblFrequency = new double[gpWaveRecordConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies]) == NULL)  // Create an array of double to contain frequencies
			{
				err_AllocDoublesForRecordFreqL(sizeof(double)*gpWaveRecordConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies);
				return false;
			}
			if ((gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblAmplitude = new double[gpWaveRecordConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies]) == NULL)  // Create an array of double to contain Amplitude
			{
				err_AllocDoublesForRecordAmplitudeL(sizeof(double)*gpWaveRecordConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies);
				return false;
			}
			for (k = 0; k < gpWaveRecordConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies; k++)
			{
				gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblFrequency[k] = pdblDtaRecordChannelFrequencies[k];
				gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblAmplitude[k] = pdblDtaRecordChannelAmplitudes[k];
			}
		}
*/
		else  // Frequencies have been specified for this channel, so read them in
		{
			for (k = 1; k > 0; k++) // Count the number of frequencies on this channel; terminate only with the break inside the loop
			{
				swprintf(wszTempString, L"%s.record.ch[%2.2lu]_freq[%2.2lu]", wszSubtestName, j, k);
				if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL)
						break;
			}
			gpWaveRecordConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies = k;
			if ((gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblFrequency = new double[k]) == NULL)  // Create an array of double to contain frequencies
			{
				err_AllocDoublesForRecordFreqL(sizeof(double)*k);
				return false;
			}
			if ((gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblAmplitude = new double[k]) == NULL)  // Create an array of double to contain Amplitude
			{
				err_AllocDoublesForRecordAmplitudeL(sizeof(double)*k);
				return false;
			}
			// Now that we know how many frequencies there are, let's read them in
			for (k = 0; k < gpWaveRecordConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies; k++)
			{
				swprintf(wszTempString, L"%s.record.ch[%2.2lu]_freq[%2.2lu]", wszSubtestName, j, k);
				if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL)
				{
					// We read this parameter successfully before.  Something is wrong if we can't read it now
					err_CantReadRecordFreqData();
					return false;
				}
				if ((gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblFrequency[k] = wcstod(pwszStringParam, NULL)) == 0)
					gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblFrequency[k] = FLT_MIN;
				swprintf(wszTempString, L"%s.record.ch[%2.2lu]_amplitude[%2.2lu]", wszSubtestName, j, k);
				if ((pwszStringParam = GetCfgString (wszTempString, NULL)) == NULL)
				{
					// If there is no matching amplitude for this frequency, just pull the default amplitude from the first frequency of the first channel of the default configuration
					gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblAmplitude[k] = gRecordTestConfigurations[DEFAULT_RECORD_TEST_CONFIGURATION].data->WaveChannelFreqData[0].pdblAmplitude[0];
				}
				else
					gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblAmplitude[k] = wcstod(pwszStringParam, NULL);
			}
		}
	}

	// ************************* Now get mixer parameters ************************

	// Let's do Audio Mux parameters before the mixer parameters because they are easier.
	// Allocate an AUDIO_MUX_CONFIGURATION_INFORMATION structure
	if ((gpAudioMuxConfigurationInformation = new AUDIO_MUX_CONFIGURATION_INFORMATION) == NULL)
	{
		err_AllocAudioMuxConfigInfoL(sizeof(AUDIO_MUX_CONFIGURATION_INFORMATION));
		return false;
	}

	// Get gpAudioMuxConfigurationInformation parameters
	swprintf(wszTempString, L"%s.mixer.mux_control_low_dword", wszSubtestName);
	gpAudioMuxConfigurationInformation->dwMuxControlLowDWORD = (DWORD)GetCfgUint (wszTempString, DEFAULT_AUDIO_MUX_LOW_DWORD);

	swprintf(wszTempString, L"%s.mixer.mux_control_high_dword", wszSubtestName);
	gpAudioMuxConfigurationInformation->dwMuxControlHighDWORD = (DWORD)GetCfgUint (wszTempString, DEFAULT_AUDIO_MUX_HIGH_DWORD);

	// Get the mixer name
	swprintf(wszTempString, L"%s.%s.%s", wszSubtestName, L"mixer", L"device_name");
	gpwszAudioMixerName = (unsigned short *)GetCfgString (wszTempString, L"none");

	if (wcsicmp(gpwszAudioMixerName, L"none") != 0)  // The mixer name was defined
	{
		// Read in all mixer configuration parameters in list form
		for (gdwNumberOfMixerControls = 0;; gdwNumberOfMixerControls++)
		{
			swprintf(wszTempString, L"%s.mixer.m[%3.3lu]", wszSubtestName, gdwNumberOfMixerControls);
			if ((pwszValue = (unsigned short *)GetCfgString (wszTempString, NULL)) == NULL)
				break; // We reached the end of the list
			// Put the value into the parameter buffer
			*pCSMixer << (LPCTSTR)pwszValue;
		}
	}

	return true;
}


/*
void vAnalogPlayback(void)
{
	DWORD dwStatus = FAIL;

	while ((gdwMaxTestRetries != 0) && (dwStatus == FAIL))
	{
		if (gdwMaxTestRetries == 1)
			gdwIgnoreError = FALSE;
		else
			gdwIgnoreError = TRUE;
		dwStatus = dwAnalogPlayback();
		gdwMaxTestRetries--;
	}
	return;
}
*/


DWORD CXModuleAudio::dwAnalogPlayback(void)
{
	DWORD j, k;
	DWORD dwLocalStatus = PASS;
	WORD wSampleFormat = WAVE_FORMAT_PCM;
	DWORD dwPlaybackBytesPerSample, dwRecordBytesPerSample;
	DWORD dwWaveOutBufferSize, dwWaveInBufferSize;
	double dblActualSignalToNoiseRatio;
	double dblActualChannelSeparation;
	double dblActualFrequencyResponse;
	DWORD dwAmountOfLeadingRecordedAudioToThrowAway;
	DSBUFFERDESC dsBufferDesc;
	DWORD dwDelayBetweenPlaybackStartAndRecordStart;
	CHostResponse CHR;
	DWORD dwActualBytesReceived, dwErrorCodeFromHost;
	int iError;
	LPBYTE pBMixerControls = NULL;
	int iMixerControlBufferLength = 0;

#ifndef _XBOX // Win32
	char *pcTemp = NULL;
	DWORD dwWaveFormatExSize, dwWaveFormatExSize2;
	void *pFirstDSBufferLoc, *pSecondDSBufferLoc;
	DWORD dwFirstDSBufferSize, dwSecondDSBufferSize;
	DWORD *pdwBufferBase;

	gpWavePlaybackConfigurationData->pAudioOutDevToUseGUID = NULL;  // Initialize the playback GUID to NULL
	// Check to be sure the specified playback device is present.  If it is not, silently use the first installed playback device
	if(DirectSoundEnumerate((LPDSENUMCALLBACK)bDSEnumCallback, this) != DS_OK)
	{
		err_DirectSoundEnumerateFailed();
		return(FAIL);
	}
	if(gpWavePlaybackConfigurationData->pAudioOutDevToUseGUID == NULL)
	{
		err_DirectSoundFindFailed();
		return(FAIL);
	}
	// The playback device GUID is now correct
#endif
	// This will simply print out the values of the configuration parameters (in Win32)
/*
	wprintf(L"Trys = %lu\n", gdwMaxTestTrys);
	wprintf(L"Delay between playback start and record start = %lu\n", gdwDelayBetweenPlaybackStartAndRecordStart);
	wprintf(L"Mutex Timeout = %lu\n", gdwMutexTimeout);
	wprintf(L"Playback device number = %lu\n", (DWORD)gpWavePlaybackConfigurationData->uiAudioOutDevToUse);
	wprintf(L"Playback number of channels = %lu\n", gpWavePlaybackConfigurationData->dwNumberOfChannels);
	wprintf(L"Playback sample rate = %lu\n", gpWavePlaybackConfigurationData->dwSampleRate);
	wprintf(L"Playback bits per sample = %lu\n", gpWavePlaybackConfigurationData->dwBitsPerSample);
	wprintf(L"Playback digital attenuation = %lu\n", gpWavePlaybackConfigurationData->dwDigitalAttenuation);
	for (j = 0; j < gpWavePlaybackConfigurationData->dwNumberOfChannels; j++)
	{
		for (k = 0; k < gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies; k++)
		{
			wprintf(L"Channel %lu frequency %lu value is %lf\n", j, k, gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency[k]);
		}
	}
	wprintf(L"Record device number = %lu\n", (DWORD)gpWaveRecordConfigurationData->uiAudioInDevToUse);
	wprintf(L"Record number of channels = %lu\n", gpWaveRecordConfigurationData->dwNumberOfChannels);
	wprintf(L"Record sample rate = %lu\n", gpWaveRecordConfigurationData->dwSampleRate);
	wprintf(L"Record bits per sample = %lu\n", gpWaveRecordConfigurationData->dwBitsPerSample);
	wprintf(L"Record FFT Window function = %lu\n", gpWaveRecordConfigurationData->dwFFTWindowFunction);
	wprintf(L"Record amplitude tolerance = %lf\n", gpWaveRecordConfigurationData->dblAmplitudeTolerance);
	wprintf(L"Record max DC offset percent = %lf\n", gpWaveRecordConfigurationData->dblMaxDCOffsetPercent);
	wprintf(L"Record signal to noise ratio = %lf\n", gpWaveRecordConfigurationData->dblSignalToNoiseRatio);
	wprintf(L"Record S/N ratio ignoring crosstalk = %lf\n", gpWaveRecordConfigurationData->dblSignalToNoiseRatioIgnoringCrosstalk);
	wprintf(L"Record channel separation = %lf\n", gpWaveRecordConfigurationData->dblChannelSeparation);
	wprintf(L"Record frequency response tolerance = %lf\n", gpWaveRecordConfigurationData->dblFrequencyResponseTolerance);
	wprintf(L"Record frequency response level = %lf\n", gpWaveRecordConfigurationData->dblFrequencyResponseLevel);
	wprintf(L"Record frequency shift percent allowed = %lf\n", gpWaveRecordConfigurationData->dblFrequencyShiftPercentAllowed);
	wprintf(L"Record frequency shift hz allowed = %lf\n", gpWaveRecordConfigurationData->dblFrequencyShiftHzAllowed);
	wprintf(L"Record CRC = %8.8lx\n", gpWaveRecordConfigurationData->dwCRC);
	for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfNoiseRanges; j++)
	{
		wprintf(L"Noise range %lu start frequency value is %lf\n", j, gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStartFreq);
		wprintf(L"Noise range %lu stop frequency value is %lf\n", j, gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStopFreq);
		wprintf(L"Noise range %lu start level value is %lf\n", j, gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStartLevel);
		wprintf(L"Noise range %lu stop level value is %lf\n", j, gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStopLevel);
	}
	for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfChannels; j++)
	{
		for (k = 0; k < gpWaveRecordConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies; k++)
		{
			wprintf(L"Channel %lu freq %lu value is %lf, amp = %lf\n", j, k, gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblFrequency[k], gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblAmplitude[k]);
		}
	}

*/

	// Now fix up the frequencies to fit in the playback buffer 
	// and calculate the SamplesPerPeriod from the Frequency values 
	vFixUpPlaybackFrequencies();
	vCalculateSamplesPerPeriod();

	// ***** Setup for playback
    // This assumes that 17-24 bit samples will use 3 bytes per sample
	dwPlaybackBytesPerSample = (gpWavePlaybackConfigurationData->dwBitsPerSample + 7) / 8;
	dwWaveOutBufferSize = dwPlaybackBytesPerSample * 
						  gpWavePlaybackConfigurationData->dwNumberOfChannels * 
						  WAVE_OUT_NUMBER_OF_SAMPLES;

	// *** At this point a mutex should indicate that we are using audio resources
	if (dwGetPossessionOfAudioResources(gdwMutexTimeout) != PASS)
			return(FAIL);

	// Create an IDirectSound interface for playing audio
#ifdef _XBOX

	if (DirectSoundCreate(NULL, &pDSoundOut, NULL) != DS_OK)
	{
		err_CouldNotCreateIDirectSound();
		return(FAIL);
	}
#else // not _XBOX (Win32)
	if (DirectSoundCreate8(gpWavePlaybackConfigurationData->pAudioOutDevToUseGUID, &pDSoundOut, NULL) != DS_OK)
	{
		err_CouldNotCreateIDirectSound();
		return(FAIL);
	}
	// Get a window handle; this will change in the future (XBox will probably not require a Window handle)
	hDSWnd = GetForegroundWindow();
	if (hDSWnd == NULL)
	{
		hDSWnd = GetDesktopWindow();
	}

	if (pDSoundOut->SetCooperativeLevel(hDSWnd, DSSCL_EXCLUSIVE) != DS_OK)
	{
		err_CouldNotSetCooperativeLevel();
		return(FAIL);
	}

	// Now set up the primary sound buffer characteristics
	memset(&dsBufferDesc, 0, sizeof(DSBUFFERDESC));
	dsBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	dsBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsBufferDesc.dwReserved = 0;
	dsBufferDesc.dwBufferBytes = 0; // Set to 0 for a primary buffer
	dsBufferDesc.lpwfxFormat = NULL; // NULL for a primary buffer
	dsBufferDesc.guid3DAlgorithm = GUID_NULL;

	if (pDSoundOut->CreateSoundBuffer(&dsBufferDesc, &pPrimaryBuffer, NULL)  != DS_OK)
	{
		err_CouldNotGetAccessToPrimaryBuffer();
		return(FAIL);
	}
/*
	// This is here just in case we need to change to IDirectSoundBuffer8 access of primary buffers
	if (pPrimaryBufferPre8->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID *)&pPrimaryBuffer)  != S_OK) // Get the IDirectSoundBuffer8 interface
	{
		err_CouldNotGetAccessToPrimaryIDirectSoundBuffer8Interface();
		return(FAIL);
	}
*/
	if ((WaveFormatex = new WAVEFORMATEX) == NULL)  // Create a WAVEFORMATX structure
	{
		err_CouldNotAllocateWAVEFORMATEX();
		return FAIL;
	}
	memset(WaveFormatex, 0, sizeof(WAVEFORMATEX));
	WaveFormatex->wFormatTag = wSampleFormat;
	WaveFormatex->nChannels = (WORD)gpWavePlaybackConfigurationData->dwNumberOfChannels;
	WaveFormatex->nSamplesPerSec = gpWavePlaybackConfigurationData->dwSampleRate;
	WaveFormatex->nBlockAlign = (WORD)(gpWavePlaybackConfigurationData->dwNumberOfChannels * dwPlaybackBytesPerSample);
	WaveFormatex->nAvgBytesPerSec = gpWavePlaybackConfigurationData->dwSampleRate * (DWORD)WaveFormatex->nBlockAlign;
	WaveFormatex->wBitsPerSample = (WORD)gpWavePlaybackConfigurationData->dwBitsPerSample;
	WaveFormatex->cbSize = 0;

	if (pPrimaryBuffer->SetFormat(WaveFormatex) != DS_OK)
	{
		err_CouldNotSetPrimaryBufferFormat();
		return(FAIL);
	}
	delete WaveFormatex;
	WaveFormatex = NULL;

	// Read back the format to make sure the hardware really supports it
	if (pPrimaryBuffer->GetFormat(NULL, 0, &dwWaveFormatExSize) != DS_OK)
	{
		err_CouldNotReadPrimaryBufferFormat();
		return(FAIL);
	}
	if ((pcTemp = new char [dwWaveFormatExSize]) == NULL)  // Create a WAVEFORMATEX structure
	{
		err_CouldNotAllocateWAVEFORMATEX();
		return FAIL;
	}
	WaveFormatex = (WAVEFORMATEX *)pcTemp;
	memset(WaveFormatex, 0, dwWaveFormatExSize);
	if (pPrimaryBuffer->GetFormat(WaveFormatex, dwWaveFormatExSize, &dwWaveFormatExSize2) != DS_OK) // Get the actual WAVEFORMATEX data
	{
		delete [] pcTemp; // Clean up this stuff
		pcTemp = NULL;
		WaveFormatex = NULL;
		err_CouldNotReadPrimaryBufferFormat();
		return(FAIL);
	}
	if (dwWaveFormatExSize != dwWaveFormatExSize2) // The whole structure had better be filled in
	{
		delete [] pcTemp; // Clean up this stuff
		pcTemp = NULL;
		WaveFormatex = NULL;
		err_CouldNotReadPrimaryBufferFormat();
		return(FAIL);
	}
	// Compare what we set to what the hardware supports
	if ((WaveFormatex->wFormatTag != wSampleFormat) ||
	    (WaveFormatex->nChannels != (WORD)gpWavePlaybackConfigurationData->dwNumberOfChannels) ||
	    (WaveFormatex->nSamplesPerSec != gpWavePlaybackConfigurationData->dwSampleRate) ||
	    (WaveFormatex->nBlockAlign != (WORD)(gpWavePlaybackConfigurationData->dwNumberOfChannels * dwPlaybackBytesPerSample)) ||
	    (WaveFormatex->nAvgBytesPerSec != gpWavePlaybackConfigurationData->dwSampleRate * (DWORD)WaveFormatex->nBlockAlign) ||
	    (WaveFormatex->wBitsPerSample != (WORD)gpWavePlaybackConfigurationData->dwBitsPerSample))
	{
		delete [] pcTemp; // Clean up this stuff
		pcTemp = NULL;
		WaveFormatex = NULL;
		err_PlaybackAudioFormatNotSupported();
		return(FAIL);
	}
	delete [] pcTemp; // Clean up this stuff
	pcTemp = NULL;
	WaveFormatex = NULL;
#endif

	// Now set up the secondary sound buffer characteristics
	if ((WaveFormatex = new WAVEFORMATEX) == NULL)  // Create a WAVEFORMATX structure
	{
		err_CouldNotAllocateWAVEFORMATEX();
		return FAIL;
	}

	// Find the largest number of frequencies in a channel
	dwNumberOfFrequencies = 0;
	for (j = 0; j < gpWavePlaybackConfigurationData->dwNumberOfChannels; j++)
	{
		if (gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies > dwNumberOfFrequencies)
			dwNumberOfFrequencies = gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies;
	}
	if ((pSecondaryBufferPre8 = new LPDIRECTSOUNDBUFFER[dwNumberOfFrequencies]) == NULL)  // Create an array of pointers to secondary DS sound buffers
	{
		err_CouldNotAllocateLPDIRECTSOUNDBUFFERArray();
		return FAIL;
	}
	for (j = 0; j < dwNumberOfFrequencies; j++) // Initialize these to NULL for safety
		pSecondaryBufferPre8[j] = NULL;
	if ((pSecondaryBuffer = new LPDIRECTSOUNDBUFFER8[dwNumberOfFrequencies]) == NULL)  // Create an array of pointers to secondary DS8 sound buffers
	{
		err_CouldNotAllocateLPDIRECTSOUNDBUFFER8Array();
		return FAIL;
	}
	for (j = 0; j < dwNumberOfFrequencies; j++) // Initialize these to NULL for safety
		pSecondaryBuffer[j] = NULL;

	if ((ppdwWaveOutBuffer = new DWORD *[dwNumberOfFrequencies]) == NULL)  // Create an array of pointers to data buffers
	{
		err_CouldNotAllocateWaveOutBufferArray();
		return FAIL;
	}
	for (j = 0; j < dwNumberOfFrequencies; j++) // Initialize these to NULL for safety
		ppdwWaveOutBuffer[j] = NULL;

	// Create a secondary buffer for each frequency
	for (j = 0; j < dwNumberOfFrequencies; j++)
	{
		// Allocate a buffer for the audio for this frequency, and put the frequency data in it
		if ((ppdwWaveOutBuffer[j] = (DWORD *)VirtualAlloc(NULL, dwWaveOutBufferSize, MEM_COMMIT, PAGE_READWRITE | PAGE_NOCACHE)) == NULL)
		{
			err_CouldNotAllocatePlaybackBuffer(dwWaveOutBufferSize);
			return FAIL;
		}

		// Generate a sine wave in each channel of this buffer
		if (dwGenSingleSineWave(ppdwWaveOutBuffer[j], 
						  gpWavePlaybackConfigurationData->dwBitsPerSample, 
						  FFT_SIZE, 
						  gpWavePlaybackConfigurationData->WaveChannelFreqData, 
						  gpWavePlaybackConfigurationData->dwNumberOfChannels, 
						  gpWavePlaybackConfigurationData->dwDigitalAttenuation,
						  j) != PASS)
			return(FAIL);  // We failed for some reason
		for (k = 1; k < (WAVE_OUT_NUMBER_OF_SAMPLES/FFT_SIZE); k++) // Duplicate generated data throughout the rest of the playback buffer
			memcpy((char *)ppdwWaveOutBuffer[j]+(k*(dwWaveOutBufferSize/(WAVE_OUT_NUMBER_OF_SAMPLES/FFT_SIZE))), ppdwWaveOutBuffer[j], (dwWaveOutBufferSize/(WAVE_OUT_NUMBER_OF_SAMPLES/FFT_SIZE)));

		memset(WaveFormatex, 0, sizeof(WAVEFORMATEX));
		WaveFormatex->wFormatTag = wSampleFormat;
		WaveFormatex->nChannels = (WORD)gpWavePlaybackConfigurationData->dwNumberOfChannels;
		WaveFormatex->nSamplesPerSec = gpWavePlaybackConfigurationData->dwSampleRate;
		WaveFormatex->nBlockAlign = (WORD)(gpWavePlaybackConfigurationData->dwNumberOfChannels * dwPlaybackBytesPerSample);
		WaveFormatex->nAvgBytesPerSec = gpWavePlaybackConfigurationData->dwSampleRate * (DWORD)WaveFormatex->nBlockAlign;
		WaveFormatex->wBitsPerSample = (WORD)gpWavePlaybackConfigurationData->dwBitsPerSample;
		WaveFormatex->cbSize = 0;

#ifdef _XBOX
		memset(&dsBufferDesc, 0, sizeof(DSBUFFERDESC));
		dsBufferDesc.dwSize = sizeof(DSBUFFERDESC);
		dsBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
		dsBufferDesc.dwBufferBytes = dwWaveOutBufferSize;
		dsBufferDesc.lpwfxFormat = WaveFormatex;

		if (pDSoundOut->CreateSoundBuffer(&dsBufferDesc, &pSecondaryBuffer[j], NULL) != DS_OK)
		{
			err_CouldNotGetAccessToSecondaryBuffer();
			return(FAIL);
		}

		// Set the headroom to 0 so that samples are not digitally attenuated.
		if (pSecondaryBuffer[j]->SetHeadroom(0) != DS_OK)
		{
			err_CouldNotSetSecondaryBufferVolume();
			return(FAIL);
		}

		// Set the volume to maximum (no attenuation of digital buffer contents)
		if (pSecondaryBuffer[j]->SetVolume(DSBVOLUME_MAX) != DS_OK)
		{
			err_CouldNotSetSecondaryBufferVolume();
			return(FAIL);
		}

		// Point to the data buffer.
		if (pSecondaryBuffer[j]->SetBufferData(ppdwWaveOutBuffer[j], dwWaveOutBufferSize) != DS_OK)
		{
			err_CouldNotSetSecondaryBufferData();
			return(FAIL);
		}

#else // Win32
		memset(&dsBufferDesc, 0, sizeof(DSBUFFERDESC));
		dsBufferDesc.dwSize = sizeof(DSBUFFERDESC);
		dsBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME |
							   DSBCAPS_CTRLPAN |
							   DSBCAPS_GLOBALFOCUS |
							   DSBCAPS_STATIC;
		dsBufferDesc.dwReserved = 0;
		dsBufferDesc.dwBufferBytes = dwWaveOutBufferSize;
		dsBufferDesc.lpwfxFormat = WaveFormatex;
		dsBufferDesc.guid3DAlgorithm = GUID_NULL;

		if (pDSoundOut->CreateSoundBuffer(&dsBufferDesc, &pSecondaryBufferPre8[j], NULL) != DS_OK)
		{
			err_CouldNotGetAccessToSecondaryBuffer();
			return(FAIL);
		}

		if (pSecondaryBufferPre8[j]->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID *)&pSecondaryBuffer[j])  != S_OK) // Get the IDirectSoundBuffer8 interface
		{
			err_CouldNotGetAccessToSecondaryIDirectSoundBuffer8Interface();
			return(FAIL);
		}

		// Set the volume to maximum (no attenuation of digital buffer contents)
		if (pSecondaryBuffer[j]->SetVolume(DSBVOLUME_MAX) != DS_OK)
		{
			err_CouldNotSetSecondaryBufferVolume();
			return(FAIL);
		}

		// Set the panning to center
		if (pSecondaryBuffer[j]->SetPan(DSBPAN_CENTER) != DS_OK)
		{
			err_CouldNotSetSecondaryBufferPanning();
			return(FAIL);
		}
		// Lock the entire secondary sound buffer so that we can copy data into it
		if (pSecondaryBuffer[j]->Lock(0, dwWaveOutBufferSize, &pFirstDSBufferLoc, &dwFirstDSBufferSize, &pSecondDSBufferLoc, &dwSecondDSBufferSize, DSBLOCK_ENTIREBUFFER) != DS_OK)
		{
			err_CouldNotLockSecondaryBuffer();
			return(FAIL);
		}
		if ((dwFirstDSBufferSize + dwSecondDSBufferSize) != dwWaveOutBufferSize) // Make sure that the whole buffer was locked
		{
			err_LockedSecondaryBufferSegmentWrongSize(dwWaveOutBufferSize, dwFirstDSBufferSize + dwSecondDSBufferSize);
			return(FAIL);
		}
		// Point pdwBufferBase to the real base of the buffer
		if (pFirstDSBufferLoc == NULL)
			pdwBufferBase = (DWORD *)pSecondDSBufferLoc;
		else if (pSecondDSBufferLoc == NULL)
			pdwBufferBase = (DWORD *)pFirstDSBufferLoc;
		else if (pSecondDSBufferLoc > pFirstDSBufferLoc)
			pdwBufferBase = (DWORD *)pFirstDSBufferLoc;
		else
			pdwBufferBase = (DWORD *)pSecondDSBufferLoc;

		// Copy the contents of the data buffer to the buffer that CreateSoundBuffer gave us.
		memcpy(pdwBufferBase, (char *)ppdwWaveOutBuffer[j], dwWaveOutBufferSize);

		// Now, let's unlock the secondary buffer
		if (pSecondaryBuffer[j]->Unlock(pFirstDSBufferLoc, dwFirstDSBufferSize, pSecondDSBufferLoc, dwSecondDSBufferSize) != DS_OK)
		{
			err_CouldNotUnlockSecondaryBuffer();
			return(FAIL);
		}
#endif
	}
	delete WaveFormatex;
	WaveFormatex = NULL;

	dwRecordBytesPerSample = (gpWaveRecordConfigurationData->dwBitsPerSample+7)/8;  // This assumes that 17-24 but samples will use 3 bytes per sample
	dwWaveInBufferSize = dwRecordBytesPerSample * 
						 gpWaveRecordConfigurationData->dwNumberOfChannels * 
						 WAVE_IN_NUMBER_OF_SAMPLES;
	// Allocate memory for the waveform data.
	// This should be wBytesPerSample*wNumberOfChannels*WAVE_IN_NUMBER_OF_SAMPLES bytes long
	if ((pdwWaveInBuffer = new DWORD [dwWaveInBufferSize/(sizeof(DWORD))]) == NULL)  // Divide by 4 to correct for the fact that this a DWORD pointer
	{
		err_CouldNotAllocateRecordBuffer(dwWaveInBufferSize);
		return(FAIL);
	}
	// Now, let's play the secondary buffer in a continuous loop
	for (j = 0; j < dwNumberOfFrequencies; j++)
	{
		if (pSecondaryBuffer[j]->Play(0, 0, DSBPLAY_LOOPING) != DS_OK)
		{
			err_CouldNotPlaySecondaryBuffer();
			return(FAIL);
		}
	}

	// Figure out how many milliseconds of audio we will record before getting to data we will actually analyze
	dwAmountOfLeadingRecordedAudioToThrowAway = (((WAVE_IN_NUMBER_OF_SAMPLES/2)-16)*1000)/gpWaveRecordConfigurationData->dwSampleRate;

	if (gdwDelayBetweenPlaybackStartAndRecordStart > dwAmountOfLeadingRecordedAudioToThrowAway)
		dwDelayBetweenPlaybackStartAndRecordStart = gdwDelayBetweenPlaybackStartAndRecordStart - dwAmountOfLeadingRecordedAudioToThrowAway;
	else
		dwDelayBetweenPlaybackStartAndRecordStart = 0;

	// Create a parameter buffer and stuff it with parameters to send to the host
	// revision - DWORD revision of the data structure.  Rev 1 is listed here
	// audio_mux_com_port - DWORD indicating the COM port that the audio mux is attached to
	// mux_control_low_dword - DWORD indicating the low 32 bits of the audio mux setting
	// mux_control_high_dword - DWORD indicating the high 32 bits of the audio mux setting
	// mixer_name - string.  The name of the mixer to adjust for recording.  "none" indicates no mixer is to be used
	// number_of_mixer_controls - DWORD indicating how many mixer control strings are in the following list
	// mixer_controls - list of strings indicating the names and settings of all mixer controls to be adjusted
	// wave_input_device_name - string.  The name "none" implies that the default analog record device should be used
	// number_of_channels - DWORD indicating how many channels to record
	// sample_rate - DWORD indicating the record rate in KHz
	// bits_per_sample - DWORD indicating the resolution of the recording (8 and 16 are common)
	// wave_input_number_of_samples - DWORD indicating the number of samples to capture to the returned buffer
	// delay_between_playback_start_and_record_start - DWORD inidcating the delay between playback starting and record starting

	CStore CSParams(32768);  // The list of mixer parameters could get very long
	CSParams << (DWORD)(PARAMETER_REV_FOR_AUDIO_RECORD_ANALOG);
	CSParams << gdwAudioMuxComPort;
	CSParams << gpAudioMuxConfigurationInformation->dwMuxControlLowDWORD;
	CSParams << gpAudioMuxConfigurationInformation->dwMuxControlHighDWORD;
	CSParams << (LPCTSTR)gpwszAudioMixerName;
	CSParams << gdwNumberOfMixerControls;
	pBMixerControls = pCSMixer->GetBuffer();
	iMixerControlBufferLength = pCSMixer->GetLength();
	for (j = 0; j < (DWORD)iMixerControlBufferLength; j++) // Copy the mixer controls to the buffer we will send to the host
	{
		CSParams << pBMixerControls[j];
	}
	CSParams << (LPCTSTR)gpWaveRecordConfigurationData->pwszAudioInDevName;
	CSParams << gpWaveRecordConfigurationData->dwNumberOfChannels;
	CSParams << gpWaveRecordConfigurationData->dwSampleRate;
	CSParams << gpWaveRecordConfigurationData->dwBitsPerSample;
	CSParams << (DWORD)(WAVE_IN_NUMBER_OF_SAMPLES);
	CSParams << dwDelayBetweenPlaybackStartAndRecordStart;
	// Send message to record audio here
	// Use the full version of iSendHost because a response is expected
	if ((iError = g_host.iSendHost(MID_AUDIO_RECORD_ANALOG, (char *)CSParams.GetBuffer(), CSParams.GetLength(), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, AUDIO_HOST_TIMEOUT)) != EVERYTHINGS_OK)
	{
		if (dwErrorCodeFromHost != 0)
		{
			// Process error from host
			err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost);
			return(FAIL);
		}
		else
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			err_HostCommunicationError(iError);
			return(FAIL);
		}
	}
	else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
	{
		// The response buffer is empty, so take appropriate action (like an error message)
		err_RecordBufferIsEmpty();
		return(FAIL);
	}
	else // The response is not NULL, and is not an error message, so process it
	{
		if (dwActualBytesReceived < dwWaveInBufferSize)
		{
			err_RecordBufferIsNotCorrectLength(dwWaveInBufferSize, dwActualBytesReceived);
			return(FAIL);
		}
		// We received the record audio buffer the size that we wanted
	}

	memcpy(pdwWaveInBuffer, CHR.pvBufferFromHost, dwWaveInBufferSize);  // Copy the record data to a local buffer for processing

	// Now, let's stop the secondary buffer
	for (j = 0; j < dwNumberOfFrequencies; j++)
	{
		if (pSecondaryBuffer[j]->Stop() != DS_OK)
		{
			err_CouldNotStopPlayingSecondaryBuffer();
			return(FAIL);
		}
	}

	// *** At this point a mutex should indicate that we are done using audio resources
	vReleasePossessionOfAudioResources();

	// Uncomment the 2 lines below to feed the generated sine wave into the input analysis routines directly.  Make sure number of channels, bits per sample, and sample rate are identical between playback and record
	//memcpy(pdwWaveInBuffer, pdwWaveOutBuffer, dwWaveOutBufferSize);  // Copy the generated waveforms to the input buffer to test our analysis algorithms
	//memcpy(pdwWaveInBuffer + dwWaveOutBufferSize, pdwWaveOutBuffer, dwWaveOutBufferSize);  // Copy the generated waveforms to the input buffer to test our analysis algorithms
	if (m_debugmask & BIT0) // Do very verbose messaging at debug bit level 0
	{
		gpF = NULL;
		// The following 4 lines write the raw recorded data to a file
#ifdef _XBOX
		gpF = fopen("Z:\\BUFFER.IN", "wb");
#else // Win32
		gpF = fopen("BUFFER.IN", "wb");
#endif
		if (gpF != NULL)
		{
			fwrite(pdwWaveInBuffer, gpWaveRecordConfigurationData->dwNumberOfChannels*dwRecordBytesPerSample, WAVE_IN_NUMBER_OF_SAMPLES, gpF);
			fclose(gpF);
			gpF = NULL;
		}
		else
		{
			ReportDebug(BIT0, L"Could not open file %s", L"BUFFER.IN");
		}
	}
	// The following 3 lines read in data to the FFT input buffer from a file
	//gpF = fopen("INPUT.raw", "rb");  // Opening info file
	//fread(pdwWaveInBuffer, 1, dwWaveInBufferSize, gpF);
	//fclose(gpF);
	//gpF = NULL;

	// Allocate a buffer to contain the raw input data reorganized into seperate channel blocks
	// No matter how many bits the recorded data was, these buffers will be filled with signed DWORD data
	if ((pslFFTInputBuffer = new SLONG [FFT_SIZE*gpWaveRecordConfigurationData->dwNumberOfChannels]) == NULL)
	{
		err_CouldNotAllocateFFTInputBufferL((unsigned long)(sizeof(SLONG)*FFT_SIZE*gpWaveRecordConfigurationData->dwNumberOfChannels));
		return(FAIL);
	}
	if ((pdblDCOffsetPercents = new double [gpWaveRecordConfigurationData->dwNumberOfChannels]) == NULL)
	{
		err_CouldNotAllocateDCOffsetPercentsL((unsigned long)(sizeof(double)*gpWaveRecordConfigurationData->dwNumberOfChannels));
		return(FAIL);
	}
	// Now let's analyze the data we got back
	// This routine computes DC offset percents for all channels and resorts the data into seperate FFT buffers sections (one per channel); each value is a signed DWORD regardless of how big the recorded values were
	// The pdwWaveInBuffer + XXX term moves the starting point of the resort from sample 0 to sample 4080, 
	// past the end of garbage data left by the driver.
	if (dwTotalDCOffset((pdwWaveInBuffer + (dwRecordBytesPerSample*gpWaveRecordConfigurationData->dwNumberOfChannels*((WAVE_IN_NUMBER_OF_SAMPLES/2)-16))/sizeof(DWORD)), pslFFTInputBuffer,
						gpWaveRecordConfigurationData->dwBitsPerSample, 	FFT_SIZE, 
						gpWaveRecordConfigurationData->dwNumberOfChannels,	pdblDCOffsetPercents) != PASS)
		return(FAIL);  // We failed for some reason
		

	
	if (gpWaveRecordConfigurationData->dblMaxDCOffsetPercent < 100.0)  // See whether any of the channels' DC offsets are out of range
	{
		for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfChannels; j++)  // This might be included in a larger loop that checks out each channel independently for DC offset, frequency response, THD+N, etc.
		{
			if (wTestNumber == ANALOG_PLAYBACK_TEST)
			{
				if (j == 0)
					ReportStatistic(XAUDIO_ANALOG_DC_OFFSET_PERCENT_LEFT_CHANNEL, _T("%.3lf"), pdblDCOffsetPercents[j]);
				else
					ReportStatistic(XAUDIO_ANALOG_DC_OFFSET_PERCENT_RIGHT_CHANNEL, _T("%.3lf"), pdblDCOffsetPercents[j]);
			}
			else if(wTestNumber == ANALOG_SILENCE_TEST)
			{
				if (j == 0)
					ReportStatistic(XAUDIO_SILENCE_DC_OFFSET_PERCENT_LEFT_CHANNEL, _T("%.3lf"), pdblDCOffsetPercents[j]);
				else
					ReportStatistic(XAUDIO_SILENCE_DC_OFFSET_PERCENT_RIGHT_CHANNEL, _T("%.3lf"), pdblDCOffsetPercents[j]);
			}
			else // Assume DIGITAL_PLAYBACK_TEST
			{
				if (j == 0)
					ReportStatistic(XAUDIO_DIGITAL_DC_OFFSET_PERCENT_LEFT_CHANNEL, _T("%.3lf"), pdblDCOffsetPercents[j]);
				else
					ReportStatistic(XAUDIO_DIGITAL_DC_OFFSET_PERCENT_RIGHT_CHANNEL, _T("%.3lf"), pdblDCOffsetPercents[j]);
			}
//			ReportDebug(BIT0, L"Channel %lu DC offset = %5.2lf%%\n", j, pdblDCOffsetPercents[j]);
			if (pdblDCOffsetPercents[j] > gpWaveRecordConfigurationData->dblMaxDCOffsetPercent)
			{
				err_DCOffsetsOutOfRangeLSDD(j, (gpWaveRecordConfigurationData->pchChannelLabel != NULL) ? gpWaveRecordConfigurationData->pchChannelLabel[j] : L"", pdblDCOffsetPercents[j], gpWaveRecordConfigurationData->dblMaxDCOffsetPercent);
				dwLocalStatus = FAIL;
			}
		}
	}
	if ((pdblNoiseLimitValues = new double [FFT_SIZE/2]) == NULL)  // This buffer will contain the noise limit value for each FFT bin
	{
		err_CouldNotAllocateNoiseLimitsL((unsigned long)(sizeof(double)*(FFT_SIZE/2)));
		return(FAIL);
	}
	vCalculateRecordNoiseLimit(pdblNoiseLimitValues, FFT_SIZE, gpWaveRecordConfigurationData->dwBitsPerSample, gpWaveRecordConfigurationData->dwSampleRate, gpWaveRecordConfigurationData->WaveRecordNoiseLimit, gpWaveRecordConfigurationData->dwNumberOfNoiseRanges);
	// Allocate a buffer large enough to store the FFT magnitude results for all channels
	if ((pdwMagnitudeValues = new DWORD [(FFT_SIZE/2) * gpWaveRecordConfigurationData->dwNumberOfChannels]) == NULL)  // This buffer will contain the magnitude value for each FFT bin
	{
		err_CouldNotAllocateFFTMagnitudeL((unsigned long)(sizeof(DWORD)*(FFT_SIZE/2) * gpWaveRecordConfigurationData->dwNumberOfChannels));
		return(FAIL);
	}

	for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfChannels; j++)  // Compute the FFT of all channels
	{
		if (dwComputeFFT(&(pslFFTInputBuffer[FFT_SIZE*j]), &(pdwMagnitudeValues[(FFT_SIZE/2)*j]), FFT_BITS, FFT_SIZE, (double)TWO_PI, (double)PI_OVER_TWO, gpWaveRecordConfigurationData->dwFFTWindowFunction) != PASS)
			return(FAIL);
	}

	if (m_debugmask & BIT0) // Do very verbose messaging at debug bit level 0
	{
		gpF = NULL;
		// Write FFT info from all channels to a file
#ifdef _XBOX
		gpF = fopen("Z:\\FFT.IN", "wb");
#else // Win32
		gpF = fopen("FFT.IN", "wb");
#endif
		if (gpF != NULL)
		{
			for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfChannels; j++)
			{
				for (k = 0; k < FFT_SIZE/2; k++)
				{
					if (pdwMagnitudeValues[((FFT_SIZE/2)*j)+k] != 0)
						fprintf(gpF, "channel %lu fft bin %4.1lu = %5.1lu Hz  magnitude = %10.1lu, %07.2lf dB\n", j, k, ((gpWaveRecordConfigurationData->dwSampleRate * k)/FFT_SIZE), pdwMagnitudeValues[((FFT_SIZE/2)*j)+k], 20*log10((double)pdwMagnitudeValues[((FFT_SIZE/2)*j)+k]/(double)(((double)((DWORD)1 << (gpWaveRecordConfigurationData->dwBitsPerSample-2)))*(double)FFT_SIZE)));
					else
						fprintf(gpF, "channel %lu fft bin %4.1lu = %5.1lu Hz  magnitude = %10.1lu, -XXX.XX dB\n", j, k, ((gpWaveRecordConfigurationData->dwSampleRate * k)/FFT_SIZE), pdwMagnitudeValues[((FFT_SIZE/2)*j)+k]);
				}
			}
			fclose(gpF);
			gpF = NULL;
		}
		else
		{
			ReportDebug(BIT0, L"Could not open file %s", L"FFT.IN");
		}
	}

	// Now compare the received FFT to the expected frequencies and the noise limits
	for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfChannels; j++)  // Compute the S/N ratio of all channels
	{
		if (dwCompareFrequenciesAndNoise(&(pdwMagnitudeValues[(FFT_SIZE/2)*j]), FFT_SIZE, *gpWaveRecordConfigurationData, j, pdblNoiseLimitValues) != PASS)
			dwLocalStatus = FAIL;
	}
	// Now we can perform other measurements on the FFT data.
	// Let's compare the S/N ratio
	if (gpWaveRecordConfigurationData->dblSignalToNoiseRatio != 0)  // Check S/N ratio if it is anything other than 0 dB
	{
		for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfChannels; j++)  // Compute the S/N ratio of all channels
		{
			if (dwComputeSignalToNoise(&(pdwMagnitudeValues[(FFT_SIZE/2)*j]), FFT_SIZE, gpWaveRecordConfigurationData->dblFrequencyShiftPercentAllowed, gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblFrequency, gpWaveRecordConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies, gpWaveRecordConfigurationData->dwSampleRate, pdblNoiseLimitValues, gpWaveRecordConfigurationData->dblFrequencyShiftHzAllowed, &dblActualSignalToNoiseRatio) != PASS)
				return(FAIL);
			if (wTestNumber == ANALOG_PLAYBACK_TEST)
			{
				if (j == 0)
					ReportStatistic(XAUDIO_ANALOG_SIGNAL_TO_NOISE_RATIO_LEFT_CHANNEL, _T("%.2lf"), dblActualSignalToNoiseRatio);
				else
					ReportStatistic(XAUDIO_ANALOG_SIGNAL_TO_NOISE_RATIO_RIGHT_CHANNEL, _T("%.2lf"), dblActualSignalToNoiseRatio);
			}
			else if (wTestNumber == ANALOG_SILENCE_TEST)
			{
				if (j == 0)
					ReportStatistic(XAUDIO_SILENCE_SIGNAL_TO_NOISE_RATIO_LEFT_CHANNEL, _T("%.2lf"), dblActualSignalToNoiseRatio);
				else
					ReportStatistic(XAUDIO_SILENCE_SIGNAL_TO_NOISE_RATIO_RIGHT_CHANNEL, _T("%.2lf"), dblActualSignalToNoiseRatio);
			}
			else // Assume DIGITAL_PLAYBACK_TEST
			{
				if (j == 0)
					ReportStatistic(XAUDIO_DIGITAL_SIGNAL_TO_NOISE_RATIO_LEFT_CHANNEL, _T("%.2lf"), dblActualSignalToNoiseRatio);
				else
					ReportStatistic(XAUDIO_DIGITAL_SIGNAL_TO_NOISE_RATIO_RIGHT_CHANNEL, _T("%.2lf"), dblActualSignalToNoiseRatio);
			}
			if (gpWaveRecordConfigurationData->dblSignalToNoiseRatio > dblActualSignalToNoiseRatio)
			{
				err_SignalToNoiseRatioTooSmallLSDD(j, (gpWaveRecordConfigurationData->pchChannelLabel != NULL) ? gpWaveRecordConfigurationData->pchChannelLabel[j] : L"", gpWaveRecordConfigurationData->dblSignalToNoiseRatio, dblActualSignalToNoiseRatio);
				dwLocalStatus = FAIL;
			}
		}
	}

	// Let's compare the S/N ratio ignoring cross-talk frequencies
	if (gpWaveRecordConfigurationData->dblSignalToNoiseRatioIgnoringCrosstalk != 0)  // Check S/N ratio ignoring crosstalk frequencies to see if it is anything other than 0 dB
	{
		for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfChannels; j++)  // Compute the S/N ratio of all channels, ignoring crosstalk frequencies from other channels
		{
			if (dwComputeSignalToNoiseIgnoringCrosstalk(&(pdwMagnitudeValues[(FFT_SIZE/2)*j]), FFT_SIZE, gpWaveRecordConfigurationData->dblFrequencyShiftPercentAllowed, gpWaveRecordConfigurationData->WaveChannelFreqData, gpWaveRecordConfigurationData->dwNumberOfChannels, gpWavePlaybackConfigurationData->WaveChannelFreqData, gpWavePlaybackConfigurationData->dwNumberOfChannels, j, gpWaveRecordConfigurationData->dwSampleRate, pdblNoiseLimitValues, gpWaveRecordConfigurationData->dblFrequencyShiftHzAllowed, &dblActualSignalToNoiseRatio) != PASS)
				return(FAIL);
			if (wTestNumber == ANALOG_PLAYBACK_TEST)
			{
				if (j == 0)
					ReportStatistic(XAUDIO_ANALOG_SIGNAL_TO_NOISE_RATIO_IGNORING_CROSSTALK_LEFT_CHANNEL, _T("%.2lf"), dblActualSignalToNoiseRatio);
				else
					ReportStatistic(XAUDIO_ANALOG_SIGNAL_TO_NOISE_RATIO_IGNORING_CROSSTALK_RIGHT_CHANNEL, _T("%.2lf"), dblActualSignalToNoiseRatio);
			}
			else if (wTestNumber == ANALOG_SILENCE_TEST)
			{
				if (j == 0)
					ReportStatistic(XAUDIO_SILENCE_SIGNAL_TO_NOISE_RATIO_IGNORING_CROSSTALK_LEFT_CHANNEL, _T("%.2lf"), dblActualSignalToNoiseRatio);
				else
					ReportStatistic(XAUDIO_SILENCE_SIGNAL_TO_NOISE_RATIO_IGNORING_CROSSTALK_RIGHT_CHANNEL, _T("%.2lf"), dblActualSignalToNoiseRatio);
			}
			else // Assume DIGITAL_PLAYBACK_TEST
			{
				if (j == 0)
					ReportStatistic(XAUDIO_DIGITAL_SIGNAL_TO_NOISE_RATIO_IGNORING_CROSSTALK_LEFT_CHANNEL, _T("%.2lf"), dblActualSignalToNoiseRatio);
				else
					ReportStatistic(XAUDIO_DIGITAL_SIGNAL_TO_NOISE_RATIO_IGNORING_CROSSTALK_RIGHT_CHANNEL, _T("%.2lf"), dblActualSignalToNoiseRatio);
			}
			if (gpWaveRecordConfigurationData->dblSignalToNoiseRatioIgnoringCrosstalk > dblActualSignalToNoiseRatio)
			{
				err_SignalToNoiseNoCrossTooSmallLSDD(j, (gpWaveRecordConfigurationData->pchChannelLabel != NULL) ? gpWaveRecordConfigurationData->pchChannelLabel[j] : L"", gpWaveRecordConfigurationData->dblSignalToNoiseRatioIgnoringCrosstalk, dblActualSignalToNoiseRatio);
				dwLocalStatus = FAIL;
			}
		}
	}

	// Let's figure out what the channel separation is
	if (gpWaveRecordConfigurationData->dblChannelSeparation != 0)  // Check channel separation
	{
		for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfChannels; j++)  // Compute the channel separation of all channels
		{
			if (dwComputeChannelSeparation(&(pdwMagnitudeValues[(FFT_SIZE/2)*j]), FFT_SIZE, gpWaveRecordConfigurationData->dblFrequencyShiftPercentAllowed, gpWaveRecordConfigurationData->WaveChannelFreqData, gpWaveRecordConfigurationData->dwNumberOfChannels, gpWavePlaybackConfigurationData->WaveChannelFreqData, gpWavePlaybackConfigurationData->dwNumberOfChannels, j, gpWaveRecordConfigurationData->dwSampleRate, gpWaveRecordConfigurationData->dblFrequencyShiftHzAllowed, &dblActualChannelSeparation, pdblNoiseLimitValues) != PASS)
				return(FAIL);
			if (wTestNumber == ANALOG_PLAYBACK_TEST)
			{
				if (j == 0)
					ReportStatistic(XAUDIO_ANALOG_CHANNEL_SEPERATION_LEFT_CHANNEL, _T("%.3lf"), dblActualChannelSeparation);
				else
					ReportStatistic(XAUDIO_ANALOG_CHANNEL_SEPERATION_RIGHT_CHANNEL, _T("%.3lf"), dblActualChannelSeparation);
			}
			else if (wTestNumber == ANALOG_SILENCE_TEST)
			{
				if (j == 0)
					ReportStatistic(XAUDIO_SILENCE_CHANNEL_SEPERATION_LEFT_CHANNEL, _T("%.3lf"), dblActualChannelSeparation);
				else
					ReportStatistic(XAUDIO_SILENCE_CHANNEL_SEPERATION_RIGHT_CHANNEL, _T("%.3lf"), dblActualChannelSeparation);
			}
			else // Assume DIGITAL_PLAYBACK_TEST
			{
				if (j == 0)
					ReportStatistic(XAUDIO_DIGITAL_CHANNEL_SEPERATION_LEFT_CHANNEL, _T("%.3lf"), dblActualChannelSeparation);
				else
					ReportStatistic(XAUDIO_DIGITAL_CHANNEL_SEPERATION_RIGHT_CHANNEL, _T("%.3lf"), dblActualChannelSeparation);
			}
			if (gpWaveRecordConfigurationData->dblChannelSeparation > dblActualChannelSeparation)
			{
				err_ChannelSeperationTooSmallLSDD(j, (gpWaveRecordConfigurationData->pchChannelLabel != NULL) ? gpWaveRecordConfigurationData->pchChannelLabel[j] : L"", gpWaveRecordConfigurationData->dblChannelSeparation, dblActualChannelSeparation);
				dwLocalStatus = FAIL;
			}
		}
	}

	// Let's figure out what the frequency response is
	if (gpWaveRecordConfigurationData->dblFrequencyResponseTolerance != 0)
	{
		for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfChannels; j++)  // Compute the frequency response of all channels
		{
			if (dwComputeFrequencyResponse(&(pdwMagnitudeValues[(FFT_SIZE/2)*j]), FFT_SIZE, gpWaveRecordConfigurationData->dblFrequencyShiftPercentAllowed, gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblFrequency, gpWaveRecordConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies, gpWaveRecordConfigurationData->dblFrequencyResponseLevel, gpWaveRecordConfigurationData->dwBitsPerSample, gpWaveRecordConfigurationData->dwSampleRate, gpWaveRecordConfigurationData->dblFrequencyShiftHzAllowed, &dblActualFrequencyResponse, pdblNoiseLimitValues) != PASS)
				return(FAIL);
			if (wTestNumber == ANALOG_PLAYBACK_TEST)
			{
				if (j == 0)
					ReportStatistic(XAUDIO_ANALOG_FREQUENCY_RESPONSE_LEFT_CHANNEL, _T("%.3lf"), dblActualFrequencyResponse);
				else
					ReportStatistic(XAUDIO_ANALOG_FREQUENCY_RESPONSE_RIGHT_CHANNEL, _T("%.3lf"), dblActualFrequencyResponse);
			}
/*
			else if (wTestNumber == ANALOG_SILENCE_TEST)
			{
				if (j == 0)
					ReportStatistic(XAUDIO_SILENCE_FREQUENCY_RESPONSE_LEFT_CHANNEL, _T("%.3lf"), dblActualFrequencyResponse);
				else
					ReportStatistic(XAUDIO_SILENCE_FREQUENCY_RESPONSE_RIGHT_CHANNEL, _T("%.3lf"), dblActualFrequencyResponse);
			}
*/
			else // Assume DIGITAL_PLAYBACK_TEST
			{
				if (j == 0)
					ReportStatistic(XAUDIO_DIGITAL_FREQUENCY_RESPONSE_LEFT_CHANNEL, _T("%.3lf"), dblActualFrequencyResponse);
				else
					ReportStatistic(XAUDIO_DIGITAL_FREQUENCY_RESPONSE_RIGHT_CHANNEL, _T("%.3lf"), dblActualFrequencyResponse);
			}
			if (gpWaveRecordConfigurationData->dblFrequencyResponseTolerance < dblActualFrequencyResponse)
			{
				err_FrequencyResponseBadLSDD(j, (gpWaveRecordConfigurationData->pchChannelLabel != NULL) ? gpWaveRecordConfigurationData->pchChannelLabel[j] : L"", gpWaveRecordConfigurationData->dblFrequencyResponseTolerance, dblActualFrequencyResponse);
				dwLocalStatus = FAIL;
			}
		}
	}

/*
	if (dwLocalStatus == FAIL)
	{
		gpF = NULL;
		// Write FFT info from all channels to a file

		gpF = fopen("FFT.IN", "wb");  // Opening info file

		if( gpF != NULL)
		{
			for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfChannels; j++)
			{
				for (k = 0; k < FFT_SIZE/2; k++)
				{
					if (pdwMagnitudeValues[((FFT_SIZE/2)*j)+k] != 0)
						fprintf(gpF, "channel %lu fft bin %4.1lu = %5.1lu Hz  magnitude = %10.1lu, %07.2lf dB\n", j, k, ((gpWaveRecordConfigurationData->dwSampleRate * k)/FFT_SIZE), pdwMagnitudeValues[((FFT_SIZE/2)*j)+k], 20*log10((double)pdwMagnitudeValues[((FFT_SIZE/2)*j)+k]/(double)(((double)((DWORD)1 << (gpWaveRecordConfigurationData->dwBitsPerSample-2)))*(double)FFT_SIZE)));
					else
						fprintf(gpF, "channel %lu fft bin %4.1lu = %5.1lu Hz  magnitude = %10.1lu, -XXX.XX dB\n", j, k, ((gpWaveRecordConfigurationData->dwSampleRate * k)/FFT_SIZE), pdwMagnitudeValues[((FFT_SIZE/2)*j)+k]);
				}
			}
			fclose(gpF);
		}
		else
		{
			wt_status("Could not open file %s", "FFT.IN");
		}

		gpF = NULL;
		// The following 4 lines write the raw recorded data to a file
		gpF = fopen("BUFFER.IN", "wb");  // Opening info file

		if( gpF != NULL)
		{

			fwrite(pdwWaveInBuffer, gpWaveRecordConfigurationData->dwNumberOfChannels*dwRecordBytesPerSample, WAVE_IN_NUMBER_OF_SAMPLES, gpF);
			fclose(gpF);
			gpF = NULL;
		}
		else
		{
			wt_status("Could not open file %s", "BUFFER.IN");
		}
		return(FAIL);
	}
*/
	return (dwLocalStatus);
}

IMPLEMENT_TESTUNLOCKED (Audio, analog_playback, ANALOG_PLAYBACK_TEST)
{
	#ifdef _DEBUG
		static int AnalogPlay;
		AnalogPlay++;
		ReportDebug(BIT0, _T("Analog Playback Test - Loop %d"), AnalogPlay);
	#endif

	wTestNumber = GetTestNumber(); // This tells us which test we are fetching parameters for

	DWORD dwStatus = FAIL;
	DWORD dwMaxTestTrys = gdwMaxTestTrys;

#ifdef _XBOX
// Make sure the audio circuit is unclamped
#endif

	while ((dwMaxTestTrys != 0) && (dwStatus == FAIL))
	{
		if (dwMaxTestTrys == 1)
		{
			gdwIgnoreError = FALSE;
		}
		else
			gdwIgnoreError = TRUE;
		vInitializeTestVariables();
		dwStatus = dwAnalogPlayback();
		vStraightenUpTestVariables();
		if (CheckAbort (HERE)) // Make sure that we don't keep looping if we are supposed to abort
			return;
		dwMaxTestTrys--;
	}
}

IMPLEMENT_TESTUNLOCKED (Audio, analog_silence, ANALOG_SILENCE_TEST)
{
	#ifdef _DEBUG
		static int AnalogSilence;
		AnalogSilence++;
		ReportDebug(BIT0, _T("Analog Playback Test - Loop %d"), AnalogSilence);
	#endif

	wTestNumber = GetTestNumber(); // This tells us which test we are fetching parameters for

	DWORD dwStatus = FAIL;
	DWORD dwMaxTestTrys = gdwMaxTestTrys;

#ifdef _XBOX
// Make sure the audio circuit is clamped
#endif

	while ((dwMaxTestTrys != 0) && (dwStatus == FAIL))
	{
		if (dwMaxTestTrys == 1)
		{
			gdwIgnoreError = FALSE;
		}
		else
			gdwIgnoreError = TRUE;
		vInitializeTestVariables();
		dwStatus = dwAnalogPlayback();
		vStraightenUpTestVariables();
		if (CheckAbort (HERE)) // Make sure that we don't keep looping if we are supposed to abort
			return;
		dwMaxTestTrys--;
	}
}

DWORD CXModuleAudio::dwDigitalAccuracy(void)
{
	DWORD j, k;
	DWORD dwLocalStatus = PASS;
	WORD wSampleFormat = WAVE_FORMAT_PCM;
	DWORD dwPlaybackBytesPerSample, dwRecordBytesPerSample;
	DWORD dwWaveOutBufferSize, dwWaveInBufferSize;
	DWORD dwAmountOfLeadingRecordedAudioToThrowAway;
	DSBUFFERDESC dsBufferDesc;
	DWORD dwDelayBetweenPlaybackStartAndRecordStart;
	CHostResponse CHR;
	DWORD dwActualBytesReceived, dwErrorCodeFromHost;
	int iError;
	LPBYTE pBMixerControls = NULL;
	int iMixerControlBufferLength = 0;
	DWORD dwCRCValue;

#ifndef _XBOX // Win32
	char *pcTemp = NULL;
	DWORD dwWaveFormatExSize, dwWaveFormatExSize2;
	void *pFirstDSBufferLoc, *pSecondDSBufferLoc;
	DWORD dwFirstDSBufferSize, dwSecondDSBufferSize;
	DWORD *pdwBufferBase;

	gpWavePlaybackConfigurationData->pAudioOutDevToUseGUID = NULL;  // Initialize the playback GUID to NULL
	// Check to be sure the specified playback device is present.  If it is not, silently use the first installed playback device
	if(DirectSoundEnumerate((LPDSENUMCALLBACK)bDSEnumCallback, this) != DS_OK)
	{
		err_DirectSoundEnumerateFailed();
		return(FAIL);
	}
	if(gpWavePlaybackConfigurationData->pAudioOutDevToUseGUID == NULL)
	{
		err_DirectSoundFindFailed();
		return(FAIL);
	}
	// The playback device GUID is now correct
#endif
	// This will simply print out the values of the configuration parameters (in Win32)
/*
	wprintf(L"Trys = %lu\n", gdwMaxTestTrys);
	wprintf(L"Delay between playback start and record start = %lu\n", gdwDelayBetweenPlaybackStartAndRecordStart);
	wprintf(L"Mutex Timeout = %lu\n", gdwMutexTimeout);
	wprintf(L"Playback device number = %lu\n", (DWORD)gpWavePlaybackConfigurationData->uiAudioOutDevToUse);
	wprintf(L"Playback number of channels = %lu\n", gpWavePlaybackConfigurationData->dwNumberOfChannels);
	wprintf(L"Playback sample rate = %lu\n", gpWavePlaybackConfigurationData->dwSampleRate);
	wprintf(L"Playback bits per sample = %lu\n", gpWavePlaybackConfigurationData->dwBitsPerSample);
	wprintf(L"Playback digital attenuation = %lu\n", gpWavePlaybackConfigurationData->dwDigitalAttenuation);
	for (j = 0; j < gpWavePlaybackConfigurationData->dwNumberOfChannels; j++)
	{
		for (k = 0; k < gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies; k++)
		{
			wprintf(L"Channel %lu frequency %lu value is %lf\n", j, k, gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency[k]);
		}
	}
	wprintf(L"Record device number = %lu\n", (DWORD)gpWaveRecordConfigurationData->uiAudioInDevToUse);
	wprintf(L"Record number of channels = %lu\n", gpWaveRecordConfigurationData->dwNumberOfChannels);
	wprintf(L"Record sample rate = %lu\n", gpWaveRecordConfigurationData->dwSampleRate);
	wprintf(L"Record bits per sample = %lu\n", gpWaveRecordConfigurationData->dwBitsPerSample);
	wprintf(L"Record FFT Window function = %lu\n", gpWaveRecordConfigurationData->dwFFTWindowFunction);
	wprintf(L"Record amplitude tolerance = %lf\n", gpWaveRecordConfigurationData->dblAmplitudeTolerance);
	wprintf(L"Record max DC offset percent = %lf\n", gpWaveRecordConfigurationData->dblMaxDCOffsetPercent);
	wprintf(L"Record signal to noise ratio = %lf\n", gpWaveRecordConfigurationData->dblSignalToNoiseRatio);
	wprintf(L"Record S/N ratio ignoring crosstalk = %lf\n", gpWaveRecordConfigurationData->dblSignalToNoiseRatioIgnoringCrosstalk);
	wprintf(L"Record channel separation = %lf\n", gpWaveRecordConfigurationData->dblChannelSeparation);
	wprintf(L"Record frequency response tolerance = %lf\n", gpWaveRecordConfigurationData->dblFrequencyResponseTolerance);
	wprintf(L"Record frequency response level = %lf\n", gpWaveRecordConfigurationData->dblFrequencyResponseLevel);
	wprintf(L"Record frequency shift percent allowed = %lf\n", gpWaveRecordConfigurationData->dblFrequencyShiftPercentAllowed);
	wprintf(L"Record frequency shift hz allowed = %lf\n", gpWaveRecordConfigurationData->dblFrequencyShiftHzAllowed);
	wprintf(L"Record CRC = %8.8lx\n", gpWaveRecordConfigurationData->dwCRC);
	for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfNoiseRanges; j++)
	{
		wprintf(L"Noise range %lu start frequency value is %lf\n", j, gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStartFreq);
		wprintf(L"Noise range %lu stop frequency value is %lf\n", j, gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStopFreq);
		wprintf(L"Noise range %lu start level value is %lf\n", j, gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStartLevel);
		wprintf(L"Noise range %lu stop level value is %lf\n", j, gpWaveRecordConfigurationData->WaveRecordNoiseLimit[j].dblNoiseLimitStopLevel);
	}
	for (j = 0; j < gpWaveRecordConfigurationData->dwNumberOfChannels; j++)
	{
		for (k = 0; k < gpWaveRecordConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies; k++)
		{
			wprintf(L"Channel %lu freq %lu value is %lf, amp = %lf\n", j, k, gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblFrequency[k], gpWaveRecordConfigurationData->WaveChannelFreqData[j].pdblAmplitude[k]);
		}
	}

*/

	// Now fix up the frequencies to fit in the playback buffer 
	// and calculate the SamplesPerPeriod from the Frequency values 
	vFixUpPlaybackFrequencies();
	vCalculateSamplesPerPeriod();

	// ***** Setup for playback
    // This assumes that 17-24 bit samples will use 3 bytes per sample
	dwPlaybackBytesPerSample = (gpWavePlaybackConfigurationData->dwBitsPerSample + 7) / 8;
	dwWaveOutBufferSize = dwPlaybackBytesPerSample * 
						  gpWavePlaybackConfigurationData->dwNumberOfChannels * 
						  WAVE_OUT_NUMBER_OF_SAMPLES;

	// *** At this point a mutex should indicate that we are using audio resources
	if (dwGetPossessionOfAudioResources(gdwMutexTimeout) != PASS)
			return(FAIL);

	// Create an IDirectSound interface for playing audio
#ifdef _XBOX

	if (DirectSoundCreate(NULL, &pDSoundOut, NULL) != DS_OK)
	{
		err_CouldNotCreateIDirectSound();
		return(FAIL);
	}
#else // not _XBOX (Win32)
	if (DirectSoundCreate8(gpWavePlaybackConfigurationData->pAudioOutDevToUseGUID, &pDSoundOut, NULL) != DS_OK)
	{
		err_CouldNotCreateIDirectSound();
		return(FAIL);
	}
	// Get a window handle; this will change in the future (XBox will probably not require a Window handle)
	hDSWnd = GetForegroundWindow();
	if (hDSWnd == NULL)
	{
		hDSWnd = GetDesktopWindow();
	}

	if (pDSoundOut->SetCooperativeLevel(hDSWnd, DSSCL_EXCLUSIVE) != DS_OK)
	{
		err_CouldNotSetCooperativeLevel();
		return(FAIL);
	}

	// Now set up the primary sound buffer characteristics
	memset(&dsBufferDesc, 0, sizeof(DSBUFFERDESC));
	dsBufferDesc.dwSize = sizeof(DSBUFFERDESC);
	dsBufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER;
	dsBufferDesc.dwReserved = 0;
	dsBufferDesc.dwBufferBytes = 0; // Set to 0 for a primary buffer
	dsBufferDesc.lpwfxFormat = NULL; // NULL for a primary buffer
	dsBufferDesc.guid3DAlgorithm = GUID_NULL;

	if (pDSoundOut->CreateSoundBuffer(&dsBufferDesc, &pPrimaryBuffer, NULL)  != DS_OK)
	{
		err_CouldNotGetAccessToPrimaryBuffer();
		return(FAIL);
	}
/*
	// This is here just in case we need to change to IDirectSoundBuffer8 access of primary buffers
	if (pPrimaryBufferPre8->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID *)&pPrimaryBuffer)  != S_OK) // Get the IDirectSoundBuffer8 interface
	{
		err_CouldNotGetAccessToPrimaryIDirectSoundBuffer8Interface();
		return(FAIL);
	}
*/
	if ((WaveFormatex = new WAVEFORMATEX) == NULL)  // Create a WAVEFORMATX structure
	{
		err_CouldNotAllocateWAVEFORMATEX();
		return FAIL;
	}
	memset(WaveFormatex, 0, sizeof(WAVEFORMATEX));
	WaveFormatex->wFormatTag = wSampleFormat;
	WaveFormatex->nChannels = (WORD)gpWavePlaybackConfigurationData->dwNumberOfChannels;
	WaveFormatex->nSamplesPerSec = gpWavePlaybackConfigurationData->dwSampleRate;
	WaveFormatex->nBlockAlign = (WORD)(gpWavePlaybackConfigurationData->dwNumberOfChannels * dwPlaybackBytesPerSample);
	WaveFormatex->nAvgBytesPerSec = gpWavePlaybackConfigurationData->dwSampleRate * (DWORD)WaveFormatex->nBlockAlign;
	WaveFormatex->wBitsPerSample = (WORD)gpWavePlaybackConfigurationData->dwBitsPerSample;
	WaveFormatex->cbSize = 0;

	if (pPrimaryBuffer->SetFormat(WaveFormatex) != DS_OK)
	{
		err_CouldNotSetPrimaryBufferFormat();
		return(FAIL);
	}
	delete WaveFormatex;
	WaveFormatex = NULL;

	// Read back the format to make sure the hardware really supports it
	if (pPrimaryBuffer->GetFormat(NULL, 0, &dwWaveFormatExSize) != DS_OK)
	{
		err_CouldNotReadPrimaryBufferFormat();
		return(FAIL);
	}
	if ((pcTemp = new char [dwWaveFormatExSize]) == NULL)  // Create a WAVEFORMATEX structure
	{
		err_CouldNotAllocateWAVEFORMATEX();
		return FAIL;
	}
	WaveFormatex = (WAVEFORMATEX *)pcTemp;
	memset(WaveFormatex, 0, dwWaveFormatExSize);
	if (pPrimaryBuffer->GetFormat(WaveFormatex, dwWaveFormatExSize, &dwWaveFormatExSize2) != DS_OK) // Get the actual WAVEFORMATEX data
	{
		delete [] pcTemp; // Clean up this stuff
		pcTemp = NULL;
		WaveFormatex = NULL;
		err_CouldNotReadPrimaryBufferFormat();
		return(FAIL);
	}
	if (dwWaveFormatExSize != dwWaveFormatExSize2) // The whole structure had better be filled in
	{
		delete [] pcTemp; // Clean up this stuff
		pcTemp = NULL;
		WaveFormatex = NULL;
		err_CouldNotReadPrimaryBufferFormat();
		return(FAIL);
	}
	// Compare what we set to what the hardware supports
	if ((WaveFormatex->wFormatTag != wSampleFormat) ||
	    (WaveFormatex->nChannels != (WORD)gpWavePlaybackConfigurationData->dwNumberOfChannels) ||
	    (WaveFormatex->nSamplesPerSec != gpWavePlaybackConfigurationData->dwSampleRate) ||
	    (WaveFormatex->nBlockAlign != (WORD)(gpWavePlaybackConfigurationData->dwNumberOfChannels * dwPlaybackBytesPerSample)) ||
	    (WaveFormatex->nAvgBytesPerSec != gpWavePlaybackConfigurationData->dwSampleRate * (DWORD)WaveFormatex->nBlockAlign) ||
	    (WaveFormatex->wBitsPerSample != (WORD)gpWavePlaybackConfigurationData->dwBitsPerSample))
	{
		delete [] pcTemp; // Clean up this stuff
		pcTemp = NULL;
		WaveFormatex = NULL;
		err_PlaybackAudioFormatNotSupported();
		return(FAIL);
	}
	delete [] pcTemp; // Clean up this stuff
	pcTemp = NULL;
	WaveFormatex = NULL;
#endif

	// Now set up the secondary sound buffer characteristics
	if ((WaveFormatex = new WAVEFORMATEX) == NULL)  // Create a WAVEFORMATX structure
	{
		err_CouldNotAllocateWAVEFORMATEX();
		return FAIL;
	}

	// Find the largest number of frequencies in a channel
	dwNumberOfFrequencies = 1;
//	dwNumberOfFrequencies = 0;
/*
	for (j = 0; j < gpWavePlaybackConfigurationData->dwNumberOfChannels; j++)
	{
		if (gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies > dwNumberOfFrequencies)
			dwNumberOfFrequencies = gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies;
	}
*/
	if ((pSecondaryBufferPre8 = new LPDIRECTSOUNDBUFFER[dwNumberOfFrequencies]) == NULL)  // Create an array of pointers to secondary DS sound buffers
	{
		err_CouldNotAllocateLPDIRECTSOUNDBUFFERArray();
		return FAIL;
	}
	for (j = 0; j < dwNumberOfFrequencies; j++) // Initialize these to NULL for safety
		pSecondaryBufferPre8[j] = NULL;
	if ((pSecondaryBuffer = new LPDIRECTSOUNDBUFFER8[dwNumberOfFrequencies]) == NULL)  // Create an array of pointers to secondary DS8 sound buffers
	{
		err_CouldNotAllocateLPDIRECTSOUNDBUFFER8Array();
		return FAIL;
	}
	for (j = 0; j < dwNumberOfFrequencies; j++) // Initialize these to NULL for safety
		pSecondaryBuffer[j] = NULL;

	if ((ppdwWaveOutBuffer = new DWORD *[dwNumberOfFrequencies]) == NULL)  // Create an array of pointers to data buffers
	{
		err_CouldNotAllocateWaveOutBufferArray();
		return FAIL;
	}
	for (j = 0; j < dwNumberOfFrequencies; j++) // Initialize these to NULL for safety
		ppdwWaveOutBuffer[j] = NULL;



	// Create a secondary buffer for each frequency
	for (j = 0; j < dwNumberOfFrequencies; j++)
	{
		// Allocate a buffer for the audio for this frequency, and put the frequency data in it
		if ((ppdwWaveOutBuffer[j] = (DWORD *)VirtualAlloc(NULL, dwWaveOutBufferSize, MEM_COMMIT, PAGE_READWRITE | PAGE_NOCACHE)) == NULL)
		{
			err_CouldNotAllocatePlaybackBuffer(dwWaveOutBufferSize);
			return FAIL;
		}

		// Generate a sine wave in each channel of this buffer
/*
		if (dwGenSingleSineWave(ppdwWaveOutBuffer[j], 
						  gpWavePlaybackConfigurationData->dwBitsPerSample, 
						  FFT_SIZE, 
						  gpWavePlaybackConfigurationData->WaveChannelFreqData, 
						  gpWavePlaybackConfigurationData->dwNumberOfChannels, 
						  gpWavePlaybackConfigurationData->dwDigitalAttenuation,
						  j) != PASS)
			return(FAIL);  // We failed for some reason
*/
///*
		if (dwGenSineWave(ppdwWaveOutBuffer[j],
						  gpWavePlaybackConfigurationData->dwBitsPerSample,
						  FFT_SIZE,
						  gpWavePlaybackConfigurationData->WaveChannelFreqData,
						  gpWavePlaybackConfigurationData->dwNumberOfChannels,
						  gpWavePlaybackConfigurationData->dwDigitalAttenuation) != PASS)
			return(FAIL);  // We failed for some reason
//*/
		for (k = 1; k < (WAVE_OUT_NUMBER_OF_SAMPLES/FFT_SIZE); k++) // Duplicate generated data throughout the rest of the playback buffer
			memcpy((char *)ppdwWaveOutBuffer[j]+(k*(dwWaveOutBufferSize/(WAVE_OUT_NUMBER_OF_SAMPLES/FFT_SIZE))), ppdwWaveOutBuffer[j], (dwWaveOutBufferSize/(WAVE_OUT_NUMBER_OF_SAMPLES/FFT_SIZE)));

		// Add a footprint to the begining of the sample
		memset((((DWORD *)ppdwWaveOutBuffer[j])+0), 0, 32); // A string of 32 0s ought to be a unique footprint
/*
		memset((((DWORD *)ppdwWaveOutBuffer[j])+2), 0x55, 4);
		memset((((DWORD *)ppdwWaveOutBuffer[j])+3), 0xAA, 4);
		memset((((DWORD *)ppdwWaveOutBuffer[j])+4), 0, 1024);

		for (k = 0; k < dwWaveOutBufferSize/2; k++)
			*(((WORD *)ppdwWaveOutBuffer[j]) + k) = k + 0x7000;
*/

		memset(WaveFormatex, 0, sizeof(WAVEFORMATEX));
		WaveFormatex->wFormatTag = wSampleFormat;
		WaveFormatex->nChannels = (WORD)gpWavePlaybackConfigurationData->dwNumberOfChannels;
		WaveFormatex->nSamplesPerSec = gpWavePlaybackConfigurationData->dwSampleRate;
		WaveFormatex->nBlockAlign = (WORD)(gpWavePlaybackConfigurationData->dwNumberOfChannels * dwPlaybackBytesPerSample);
		WaveFormatex->nAvgBytesPerSec = gpWavePlaybackConfigurationData->dwSampleRate * (DWORD)WaveFormatex->nBlockAlign;
		WaveFormatex->wBitsPerSample = (WORD)gpWavePlaybackConfigurationData->dwBitsPerSample;
		WaveFormatex->cbSize = 0;

#ifdef _XBOX
		memset(&dsBufferDesc, 0, sizeof(DSBUFFERDESC));
		dsBufferDesc.dwSize = sizeof(DSBUFFERDESC);
		dsBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
		dsBufferDesc.dwBufferBytes = dwWaveOutBufferSize;
		dsBufferDesc.lpwfxFormat = WaveFormatex;

		if (pDSoundOut->CreateSoundBuffer(&dsBufferDesc, &pSecondaryBuffer[j], NULL) != DS_OK)
		{
			err_CouldNotSetSecondaryBufferVolume();
			return(FAIL);
		}

		if (pDSoundOut->SetMixBinHeadroom(0xffffffff, 0) != DS_OK) // Set all mix bins to 0 headroom
		{
			err_CouldNotSetSecondaryBufferVolume();
			return(FAIL);
		}

		// Set the headroom to 0 so that samples are not digitally attenuated.
		if (pSecondaryBuffer[j]->SetHeadroom(0) != DS_OK)
		{
			err_CouldNotSetSecondaryBufferVolume();
			return(FAIL);
		}

		// Set the volume to maximum (no attenuation of digital buffer contents)
		if (pSecondaryBuffer[j]->SetVolume(DSBVOLUME_MAX) != DS_OK)
		{
			err_CouldNotSetSecondaryBufferVolume();
			return(FAIL);
		}



		// Point to the data buffer.
		if (pSecondaryBuffer[j]->SetBufferData(ppdwWaveOutBuffer[j], dwWaveOutBufferSize) != DS_OK)
		{
			err_CouldNotSetSecondaryBufferData();
			return(FAIL);
		}

#else // Win32
		memset(&dsBufferDesc, 0, sizeof(DSBUFFERDESC));
		dsBufferDesc.dwSize = sizeof(DSBUFFERDESC);
		dsBufferDesc.dwFlags = DSBCAPS_CTRLVOLUME |
							   DSBCAPS_CTRLPAN |
							   DSBCAPS_GLOBALFOCUS |
							   DSBCAPS_STATIC;
		dsBufferDesc.dwReserved = 0;
		dsBufferDesc.dwBufferBytes = dwWaveOutBufferSize;
		dsBufferDesc.lpwfxFormat = WaveFormatex;
		dsBufferDesc.guid3DAlgorithm = GUID_NULL;

		if (pDSoundOut->CreateSoundBuffer(&dsBufferDesc, &pSecondaryBufferPre8[j], NULL) != DS_OK)
		{
			err_CouldNotGetAccessToSecondaryBuffer();
			return(FAIL);
		}

		if (pSecondaryBufferPre8[j]->QueryInterface(IID_IDirectSoundBuffer8, (LPVOID *)&pSecondaryBuffer[j])  != S_OK) // Get the IDirectSoundBuffer8 interface
		{
			err_CouldNotGetAccessToSecondaryIDirectSoundBuffer8Interface();
			return(FAIL);
		}

		// Set the volume to maximum (no attenuation of digital buffer contents)
		if (pSecondaryBuffer[j]->SetVolume(DSBVOLUME_MAX) != DS_OK)
		{
			err_CouldNotSetSecondaryBufferVolume();
			return(FAIL);
		}

		// Set the panning to center
		if (pSecondaryBuffer[j]->SetPan(DSBPAN_CENTER) != DS_OK)
		{
			err_CouldNotSetSecondaryBufferPanning();
			return(FAIL);
		}
		// Lock the entire secondary sound buffer so that we can copy data into it
		if (pSecondaryBuffer[j]->Lock(0, dwWaveOutBufferSize, &pFirstDSBufferLoc, &dwFirstDSBufferSize, &pSecondDSBufferLoc, &dwSecondDSBufferSize, DSBLOCK_ENTIREBUFFER) != DS_OK)
		{
			err_CouldNotLockSecondaryBuffer();
			return(FAIL);
		}
		if ((dwFirstDSBufferSize + dwSecondDSBufferSize) != dwWaveOutBufferSize) // Make sure that the whole buffer was locked
		{
			err_LockedSecondaryBufferSegmentWrongSize(dwWaveOutBufferSize, dwFirstDSBufferSize + dwSecondDSBufferSize);
			return(FAIL);
		}
		// Point pdwBufferBase to the real base of the buffer
		if (pFirstDSBufferLoc == NULL)
			pdwBufferBase = (DWORD *)pSecondDSBufferLoc;
		else if (pSecondDSBufferLoc == NULL)
			pdwBufferBase = (DWORD *)pFirstDSBufferLoc;
		else if (pSecondDSBufferLoc > pFirstDSBufferLoc)
			pdwBufferBase = (DWORD *)pFirstDSBufferLoc;
		else
			pdwBufferBase = (DWORD *)pSecondDSBufferLoc;

		// Copy the contents of the data buffer to the buffer that CreateSoundBuffer gave us.
		memcpy(pdwBufferBase, (char *)ppdwWaveOutBuffer[j], dwWaveOutBufferSize);

/*
	if (m_debugmask & BIT0) // Do very verbose messaging at debug bit level 0
	{
		// The following 4 lines write the digitally generated playback buffer to a file
		gpF = fopen("BUFFER.OUT", "wb");  // Opening info file

		if(gpF != NULL)
		{
			//for (j = 0; j < 22; j++)
			fwrite(pdwBufferBase, 4, FFT_SIZE, gpF);
			fclose(gpF);
			gpF = NULL;
		}

		gpF = fopen("BUFFER.OU1", "w");  // Opening info file	
		if(gpF != NULL)
		{
			for (j = 0; j < FFT_SIZE; j++)
			{
				fprintf(gpF, "%5.5lu, %5.5x, %5.5x\n", j, (short)(pdwBufferBase[j] & 0xFFFF), (short)((pdwBufferBase[j] >> 16) & 0xFFFF));
			}
			fclose(gpF);
			gpF = NULL;
		}

	}

*/




		// Now, let's unlock the secondary buffer
		if (pSecondaryBuffer[j]->Unlock(pFirstDSBufferLoc, dwFirstDSBufferSize, pSecondDSBufferLoc, dwSecondDSBufferSize) != DS_OK)
		{
			err_CouldNotUnlockSecondaryBuffer();
			return(FAIL);
		}
#endif
	}
	delete WaveFormatex;
	WaveFormatex = NULL;

	dwRecordBytesPerSample = (gpWaveRecordConfigurationData->dwBitsPerSample+7)/8;  // This assumes that 17-24 but samples will use 3 bytes per sample
	dwWaveInBufferSize = dwRecordBytesPerSample * 
						 gpWaveRecordConfigurationData->dwNumberOfChannels * 
						 WAVE_IN_NUMBER_OF_SAMPLES;
	// Allocate memory for the waveform data.
	// This should be wBytesPerSample*wNumberOfChannels*WAVE_IN_NUMBER_OF_SAMPLES bytes long
	if ((pdwWaveInBuffer = new DWORD [dwWaveInBufferSize/(sizeof(DWORD))]) == NULL)  // Divide by 4 to correct for the fact that this a DWORD pointer
	{
		err_CouldNotAllocateRecordBuffer(dwWaveInBufferSize);
		return(FAIL);
	}
	// Now, let's play the secondary buffer in a continuous loop
	for (j = 0; j < dwNumberOfFrequencies; j++)
	{
		if (pSecondaryBuffer[j]->Play(0, 0, DSBPLAY_LOOPING) != DS_OK)
		{
			err_CouldNotPlaySecondaryBuffer();
			return(FAIL);
		}
	}

	// Figure out how many milliseconds of audio we will record before getting to data we will actually analyze
	dwAmountOfLeadingRecordedAudioToThrowAway = (((WAVE_IN_NUMBER_OF_SAMPLES/2)-16)*1000)/gpWaveRecordConfigurationData->dwSampleRate;

	if (gdwDelayBetweenPlaybackStartAndRecordStart > dwAmountOfLeadingRecordedAudioToThrowAway)
		dwDelayBetweenPlaybackStartAndRecordStart = gdwDelayBetweenPlaybackStartAndRecordStart - dwAmountOfLeadingRecordedAudioToThrowAway;
	else
		dwDelayBetweenPlaybackStartAndRecordStart = 0;

	// Create a parameter buffer and stuff it with parameters to send to the host
	// revision - DWORD revision of the data structure.  Rev 1 is listed here
	// audio_mux_com_port - DWORD indicating the COM port that the audio mux is attached to
	// mux_control_low_dword - DWORD indicating the low 32 bits of the audio mux setting
	// mux_control_high_dword - DWORD indicating the high 32 bits of the audio mux setting
	// mixer_name - string.  The name of the mixer to adjust for recording.  "none" indicates no mixer is to be used
	// number_of_mixer_controls - DWORD indicating how many mixer control strings are in the following list
	// mixer_controls - list of strings indicating the names and settings of all mixer controls to be adjusted
	// wave_input_device_name - string.  The name "none" implies that the default analog record device should be used
	// number_of_channels - DWORD indicating how many channels to record
	// sample_rate - DWORD indicating the record rate in KHz
	// bits_per_sample - DWORD indicating the resolution of the recording (8 and 16 are common)
	// wave_input_number_of_samples - DWORD indicating the number of samples to capture to the returned buffer
	// delay_between_playback_start_and_record_start - DWORD inidcating the delay between playback starting and record starting

	CStore CSParams(32768);  // The list of mixer parameters could get very long
	CSParams << (DWORD)(PARAMETER_REV_FOR_AUDIO_RECORD_ANALOG);
	CSParams << gdwAudioMuxComPort;
	CSParams << gpAudioMuxConfigurationInformation->dwMuxControlLowDWORD;
	CSParams << gpAudioMuxConfigurationInformation->dwMuxControlHighDWORD;
	CSParams << (LPCTSTR)gpwszAudioMixerName;
	CSParams << gdwNumberOfMixerControls;
	pBMixerControls = pCSMixer->GetBuffer();
	iMixerControlBufferLength = pCSMixer->GetLength();
	for (j = 0; j < (DWORD)iMixerControlBufferLength; j++) // Copy the mixer controls to the buffer we will send to the host
	{
		CSParams << pBMixerControls[j];
	}
	CSParams << (LPCTSTR)gpWaveRecordConfigurationData->pwszAudioInDevName;
	CSParams << gpWaveRecordConfigurationData->dwNumberOfChannels;
	CSParams << gpWaveRecordConfigurationData->dwSampleRate;
	CSParams << gpWaveRecordConfigurationData->dwBitsPerSample;
	CSParams << (DWORD)(WAVE_IN_NUMBER_OF_SAMPLES);
	CSParams << dwDelayBetweenPlaybackStartAndRecordStart;
	// Send message to record audio here
	// Use the full version of iSendHost because a response is expected
	if ((iError = g_host.iSendHost(MID_AUDIO_RECORD_ANALOG, (char *)CSParams.GetBuffer(), CSParams.GetLength(), CHR, &dwActualBytesReceived, &dwErrorCodeFromHost, AUDIO_HOST_TIMEOUT)) != EVERYTHINGS_OK)
	{
		if (dwErrorCodeFromHost != 0)
		{
			// Process error from host
			err_HostResponseError(dwErrorCodeFromHost, (LPCTSTR)CHR.pvBufferFromHost);
			return(FAIL);
		}
		else
		{
			// Process a communication error
			// This test module just reports the number of the communication error that was received
			err_HostCommunicationError(iError);
			return(FAIL);
		}
	}
	else if ((CHR.pvBufferFromHost == NULL) || (dwActualBytesReceived == 0)) // Don't access the response buffer if it is NULL
	{
		// The response buffer is empty, so take appropriate action (like an error message)
		err_RecordBufferIsEmpty();
		return(FAIL);
	}
	else // The response is not NULL, and is not an error message, so process it
	{
		if (dwActualBytesReceived < dwWaveInBufferSize)
		{
			err_RecordBufferIsNotCorrectLength(dwWaveInBufferSize, dwActualBytesReceived);
			return(FAIL);
		}
		// We received the record audio buffer the size that we wanted
	}

	memcpy(pdwWaveInBuffer, CHR.pvBufferFromHost, dwWaveInBufferSize);  // Copy the record data to a local buffer for processing

	// Now, let's stop the secondary buffer
	for (j = 0; j < dwNumberOfFrequencies; j++)
	{
		if (pSecondaryBuffer[j]->Stop() != DS_OK)
		{
			err_CouldNotStopPlayingSecondaryBuffer();
			return(FAIL);
		}
	}

	// *** At this point a mutex should indicate that we are done using audio resources
	vReleasePossessionOfAudioResources();

	// Uncomment the 2 lines below to feed the generated sine wave into the input analysis routines directly.  Make sure number of channels, bits per sample, and sample rate are identical between playback and record
	//memcpy(pdwWaveInBuffer, pdwWaveOutBuffer, dwWaveOutBufferSize);  // Copy the generated waveforms to the input buffer to test our analysis algorithms
	//memcpy(pdwWaveInBuffer + dwWaveOutBufferSize, pdwWaveOutBuffer, dwWaveOutBufferSize);  // Copy the generated waveforms to the input buffer to test our analysis algorithms
	if (m_debugmask & BIT0) // Do very verbose messaging at debug bit level 0
	{
		gpF = NULL;
		// The following 4 lines write the raw recorded data to a file
#ifdef _XBOX
		gpF = fopen("Z:\\BUFFER.IN", "wb");
#else // Win32
		gpF = fopen("BUFFER.IN", "wb");
#endif
		if (gpF != NULL)
		{
			fwrite(pdwWaveInBuffer, gpWaveRecordConfigurationData->dwNumberOfChannels*dwRecordBytesPerSample, WAVE_IN_NUMBER_OF_SAMPLES, gpF);
			fclose(gpF);
			gpF = NULL;
		}
		else
		{
			ReportDebug(BIT0, L"Could not open file %s", L"BUFFER.IN");
		}
	}
	// The following 3 lines read in data to the FFT input buffer from a file
	//gpF = fopen("INPUT.raw", "rb");  // Opening info file
	//fread(pdwWaveInBuffer, 1, dwWaveInBufferSize, gpF);
	//fclose(gpF);
	//gpF = NULL;

	// Look for the signature.  Start looking a ways into the buffer so that we are not
	// tricked by any partial signature of leading zeros that might exist at the start
	// of the buffer
	DWORD dwStartSample = 200;
	for (k = 0; k < 2; k++)
	{
		for (j = dwStartSample; j < (dwWaveInBufferSize/sizeof(DWORD)) - 3; j++)
		{
			if (pdwWaveInBuffer[j] == 0x00000000)
			{
				if (pdwWaveInBuffer[j+1] == 0x00000000)
				{
					if (pdwWaveInBuffer[j+2] == 0x00000000)
					{
						if (pdwWaveInBuffer[j+3] == 0x00000000)  // Found the signature
						{
							break;
						}
					}
				}
			}
		}
		if (j == dwStartSample) // This may only be a partial signature, so back up in the buffer and look for the signature again
			dwStartSample = dwStartSample - 100;
		else
			break;
	}
	if (j == ((dwWaveInBufferSize/sizeof(DWORD)) - 3)) // Did not find the signature
	{
		err_CouldNotFindDigitalSignature();
		return(FAIL);
	}

	if ((pdwMagnitudeValues = new DWORD [dwWaveOutBufferSize/(sizeof(DWORD))]) == NULL)
	{
		err_CouldNotAllocateCRCBufferL((unsigned long)(sizeof(DWORD)*dwWaveOutBufferSize));
		return(FAIL);
	}

	// Copy the recorded buffer contents into the pdwMagnitudeValues with the proper alignment before CRCing it
	DWORD dwBytesToCopy = dwWaveInBufferSize-(j*sizeof(DWORD));
	if (dwBytesToCopy > dwWaveOutBufferSize)
		dwBytesToCopy = dwWaveOutBufferSize;
	memcpy(pdwMagnitudeValues, &pdwWaveInBuffer[j], dwBytesToCopy);
	memcpy(&pdwMagnitudeValues[dwBytesToCopy/sizeof(DWORD)], &pdwWaveInBuffer[0], dwWaveOutBufferSize-dwBytesToCopy);

	// Compute the CRC based on the FFT magnitudes from all channels
	dwCRCValue = dwComputeCRC((unsigned char *)pdwMagnitudeValues, dwWaveOutBufferSize, INITIAL_CRC);

	// Compare the CRC value here
	if (gpWaveRecordConfigurationData->dwCRC != 0) // Don't compare the CRC if the config parameter is set to 0
	{
		if (gpWaveRecordConfigurationData->dwCRC != dwCRCValue)
		{
			err_CRCMiscompareLL(gpWaveRecordConfigurationData->dwCRC, dwCRCValue);
			return(FAIL);
		}
	}

	return (dwLocalStatus);
}


IMPLEMENT_TESTUNLOCKED (Audio, digital_playback, DIGITAL_PLAYBACK_TEST)
{
	#ifdef _DEBUG
		static int DigitalPlay;
		DigitalPlay++;
		ReportDebug(BIT0, _T("Digital Playback Test - Loop %d"), DigitalPlay);
	#endif

	wTestNumber = GetTestNumber(); // This tells us which test we are fetching parameters for

	DWORD dwStatus = FAIL;
	DWORD dwMaxTestTrys = gdwMaxTestTrys;

#ifdef _XBOX
// Make sure the audio circuit is unclamped
#endif

	while ((dwMaxTestTrys != 0) && (dwStatus == FAIL))
	{
		if (dwMaxTestTrys == 1)
		{
			gdwIgnoreError = FALSE;
		}
		else
			gdwIgnoreError = TRUE;
		vInitializeTestVariables();
		dwStatus = dwAnalogPlayback();
		vStraightenUpTestVariables();
		if (CheckAbort (HERE)) // Make sure that we don't keep looping if we are supposed to abort
			return;
		dwMaxTestTrys--;
	}
}

IMPLEMENT_TESTUNLOCKED (Audio, digital_accuracy, DIGITAL_ACCURACY_TEST)
{
	#ifdef _DEBUG
		static int DigitalPlay;
		DigitalPlay++;
		ReportDebug(BIT0, _T("Digital Playback Test - Loop %d"), DigitalPlay);
	#endif

	wTestNumber = GetTestNumber(); // This tells us which test we are fetching parameters for

	DWORD dwStatus = FAIL;
	DWORD dwMaxTestTrys = gdwMaxTestTrys;

#ifdef _XBOX
// Make sure the audio circuit is unclamped
#endif

	while ((dwMaxTestTrys != 0) && (dwStatus == FAIL))
	{
		if (dwMaxTestTrys == 1)
		{
			gdwIgnoreError = FALSE;
		}
		else
			gdwIgnoreError = TRUE;
		vInitializeTestVariables();
		dwStatus = dwDigitalAccuracy();
		vStraightenUpTestVariables();
		if (CheckAbort (HERE)) // Make sure that we don't keep looping if we are supposed to abort
			return;
		dwMaxTestTrys--;
	}
}


// This routine initializes a set of global variables for every test retry loop
void CXModuleAudio::vInitializeTestVariables(void)
{
	gpF = NULL;
	pDSoundOut = NULL;
	hDSWnd = NULL;
	pPrimaryBuffer = NULL;
	pSecondaryBuffer = NULL;
	pSecondaryBufferPre8 = NULL;
	dwNumberOfFrequencies = 0;
	ppdwWaveOutBuffer = NULL;
	pdwWaveInBuffer = NULL;
	WaveFormatex = NULL;
	pslFFTInputBuffer = NULL;
	pdblDCOffsetPercents = NULL;
	pdblNoiseLimitValues = NULL;
	pdwMagnitudeValues = NULL;
	hXMTAAudioMutex = (HANDLE) -1;
}



/*
vFixUpPlaybackFrequencies uses gpWavePlaybackConfigurationData as its input.

The playback frequencies are scaled to the nearest playback bin (which is currently equivalent to an
FFT bin and always should be a multiple of an FFT bin) so that no partial sine waves hang off the end
of the playback buffer.  dwSampleRate is the sample rate to which the frequencies should be scaled.

If we record at a different rate than we play back, we must use an FFT window (unless playback and record
rates are integer multiples of one another)

*/
void CXModuleAudio::vFixUpPlaybackFrequencies()
{
	DWORD j, k;

	for (j = 0; j < gpWavePlaybackConfigurationData->dwNumberOfChannels; j++)
	{
		for (k = 0; k < gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies; k++)
		{
			gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency[k] = ((double)((DWORD)((gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency[k]*((double)FFT_SIZE/(double)gpWavePlaybackConfigurationData->dwSampleRate))+(double)0.5))) * ((double)gpWavePlaybackConfigurationData->dwSampleRate/(double)FFT_SIZE);
			if (gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency[k] == 0)
				gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency[k] = FLT_MIN;  // This should avoid divide by zero errors; use FLT_MIN instead of DBL_MIN to keep from underflowing double representation during future computations.
		}
	}
}

/*
vCalculateSamplesPerPeriod uses gpWavePlaybackConfigurationData as its input.
*/
void CXModuleAudio::vCalculateSamplesPerPeriod()
{
	DWORD j, k;

	for (j = 0; j < gpWavePlaybackConfigurationData->dwNumberOfChannels; j++)
	{
		for (k = 0; k < gpWavePlaybackConfigurationData->WaveChannelFreqData[j].dwNumberOfFrequencies; k++)
		{
			gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblSamplesPerPeriod[k] = (double)gpWavePlaybackConfigurationData->dwSampleRate/gpWavePlaybackConfigurationData->WaveChannelFreqData[j].pdblFrequency[k];
		}
	}
}


/*****************************************************************
    dwGenSineWave  -  generate sound waveform data
  
    This routine creates a buffer full of sine wave patterns.  It should be able
	to handle buffers of any number of channels, with the number of sample bits anywhere
	between 1 and 32 bits.  This routine places all defined sine waves into their appropriate channels

	Accepts:
	pdwDataBuffer - Pointer to the buffer that will contain the generated sine wave data
	dwBitsPerSample - The number of bits per sample (for a single channel)
	dwNumberOfSamples - The number of sample points per channel (is 4096 for the first rev of this test module)
	pWaveChannelFreqData - Pointer to an array of WAVE_PLAYBACK_CHANNEL_FREQ_DATA structures
	dwNumberOfChannels - The number of audio channels (normally 1 or 2)
	dwAttenuationDB - Attenuation, in dB, to apply to the data (decreases the amplitude of the sine waves)

	Returns:
	PASS - When the pdwDataBuffer is successfully filled
	FAIL - When anything goes wrong

*****************************************************************/
DWORD CXModuleAudio::dwGenSineWave(DWORD *pdwDataBuffer, DWORD dwBitsPerSample, DWORD dwNumberOfSamples, WAVE_PLAYBACK_CHANNEL_FREQ_DATA *pWaveChannelFreqData, DWORD dwNumberOfChannels, DWORD dwAttenuationDB)
{
	BYTE *pbDataBuffer;
	WORD *pwDataBuffer;
	SLONG *pslWorkingDataBuffer;
	DWORD dwBytesPerSample;
	DWORD dwMaxAmplitudedAllowed;
	DWORD dwNumberOfChannelsTimesBytesPerSample;
	DWORD h, i, j, k;
	double dblSamplesPerPeriod;
	double dblRadiansPerSample, dblRadians;
	double dblScaleFactor;
	DWORD dwSampleOffset;
	DWORD dwTempDWORD;


	pbDataBuffer = (BYTE *)pdwDataBuffer;
	pwDataBuffer = (WORD *)pdwDataBuffer;
	dwBytesPerSample = (dwBitsPerSample+7)/8;  // This assumes that 17-24 but samples will use 3 bytes per sample
	dwMaxAmplitudedAllowed = ((DWORD)1 << (dwBitsPerSample-1)) - (DWORD)1;
	dwNumberOfChannelsTimesBytesPerSample = dwNumberOfChannels*dwBytesPerSample;
	// Create and lock a working buffer large enough to contain the number of samples in longs for a single channel
	if ((pslWorkingDataBuffer = new SLONG [dwNumberOfSamples]) == NULL)
	{
		err_CouldNotAllocateSineWaveCreateL((unsigned long)(sizeof(SLONG)*dwNumberOfSamples));
		return FAIL;
	}

	// First generate the left channel output data
	
	for (h = 0; h < dwNumberOfChannels; h++)
	{
		memset(pslWorkingDataBuffer, 0, dwNumberOfSamples*sizeof(SLONG));  // Clear the memory buffer
		for (j = 0; j < pWaveChannelFreqData[h].dwNumberOfFrequencies; j++)  // Create sine waves for all of the frequencies passed in for this channel
		{
			dblSamplesPerPeriod = pWaveChannelFreqData[h].pdblSamplesPerPeriod[j];
			dblRadiansPerSample = TWO_PI / dblSamplesPerPeriod;
			i = 0;
			k = 0;
			while (k < dwNumberOfSamples)
			{
				dblRadians = dblRadiansPerSample * (double)i;
				pslWorkingDataBuffer[k] = pslWorkingDataBuffer[k] + (SLONG)((double)dwMaxAmplitudedAllowed * sin(dblRadians));
				k++;
				i++;
			}
		}
		// Now the working data buffer contains all of the frequencies added together at their maximum amplitudes.  Now we need to scale them down and put them in their final buffer

		dblScaleFactor = (double)pWaveChannelFreqData[h].dwNumberOfFrequencies  * pow((double)10, ((double)dwAttenuationDB/(double)20));
		dwSampleOffset = h*dwBytesPerSample;
		if (dwBytesPerSample == 1)  // Generate 8 bit output
		{
			for (i = 0; i < dwNumberOfSamples; i++)
			{
				dwTempDWORD = (DWORD)(pslWorkingDataBuffer[i]/dblScaleFactor);  // Keep this line and the one below it seperate, or the optimizing compiler will goof up
				pbDataBuffer[i*dwNumberOfChannels + h] = (BYTE)((dwTempDWORD+128) & 0xff);  // 8-bit PCM is unsigned instead of signed
			}
		}
		else if (dwBytesPerSample == 2)  // Generate 16 bit output
		{
			for (i = 0; i < dwNumberOfSamples; i++)
			{
				dwTempDWORD = (DWORD)(pslWorkingDataBuffer[i]/dblScaleFactor);  // Keep this line and the one below it seperate, or the optimizing compiler will goof up
				pwDataBuffer[i*dwNumberOfChannels + h] = (WORD)(dwTempDWORD & 0xffff);
			}
		}
		else if (dwBytesPerSample == 3)  // Generate 24 bit output
		{
			for (i = 0; i < dwNumberOfSamples; i++)
			{
				dwTempDWORD = (DWORD)(pslWorkingDataBuffer[i]/dblScaleFactor);  // Keep this line and the one below it seperate, or the optimizing compiler will goof up
				pbDataBuffer[i*dwNumberOfChannelsTimesBytesPerSample + dwSampleOffset] = (BYTE)(dwTempDWORD & 0xff);
				pbDataBuffer[i*dwNumberOfChannelsTimesBytesPerSample + dwSampleOffset + 1] = (BYTE)((dwTempDWORD >> 8) & 0xff);
				pbDataBuffer[i*dwNumberOfChannelsTimesBytesPerSample + dwSampleOffset + 2] = (BYTE)((dwTempDWORD >> 16) & 0xff);
			}
		}
		else  // Assume 32 bit output
		{
			for (i = 0; i < dwNumberOfSamples; i++)
			{
				dwTempDWORD = (DWORD)(pslWorkingDataBuffer[i]/dblScaleFactor);  // Keep this line and the one below it seperate, or the optimizing compiler will goof up
				pdwDataBuffer[i*dwNumberOfChannels + h] = dwTempDWORD;
			}
		}
	}
	delete [] pslWorkingDataBuffer;
	pslWorkingDataBuffer = NULL;
	return(PASS);
}

/*****************************************************************
    dwGenSingleSineWave  -  generate sound waveform data
  
    This routine creates a buffer full of sine wave patterns.  It should be able
	to handle buffers of any number of channels, with the number of sample bits anywhere
	between 1 and 32 bits.  This routine only places a single sine wave in every channel.

	Accepts:
	pdwDataBuffer - Pointer to the buffer that will contain the generated sine wave data
	dwBitsPerSample - The number of bits per sample (for a single channel)
	dwNumberOfSamples - The number of sample points per channel (is 4096 for the first rev of this test module)
	pWaveChannelFreqData - Pointer to an array of WAVE_PLAYBACK_CHANNEL_FREQ_DATA structures
	dwNumberOfChannels - The number of audio channels (normally 1 or 2)
	dwAttenuationDB - Attenuation, in dB, to apply to the data (decreases the amplitude of the sine waves)

	Returns:
	PASS - When the pdwDataBuffer is successfully filled
	FAIL - When anything goes wrong

*****************************************************************/
DWORD CXModuleAudio::dwGenSingleSineWave(DWORD *pdwDataBuffer, DWORD dwBitsPerSample, DWORD dwNumberOfSamples, WAVE_PLAYBACK_CHANNEL_FREQ_DATA *pWaveChannelFreqData, DWORD dwNumberOfChannels, DWORD dwAttenuationDB, DWORD dwFrequencyNumber)
{
	BYTE *pbDataBuffer;
	WORD *pwDataBuffer;
	SLONG *pslWorkingDataBuffer;
	DWORD dwBytesPerSample;
	DWORD dwMaxAmplitudedAllowed;
	DWORD dwNumberOfChannelsTimesBytesPerSample;
	DWORD h, i;
	double dblSamplesPerPeriod;
	double dblRadiansPerSample, dblRadians;
	double dblScaleFactor;
	DWORD dwSampleOffset;
	DWORD dwTempDWORD;


	pbDataBuffer = (BYTE *)pdwDataBuffer;
	pwDataBuffer = (WORD *)pdwDataBuffer;
	dwBytesPerSample = (dwBitsPerSample+7)/8;  // This assumes that 17-24 but samples will use 3 bytes per sample
	dwMaxAmplitudedAllowed = ((DWORD)1 << (dwBitsPerSample-1)) - (DWORD)1;
	dwNumberOfChannelsTimesBytesPerSample = dwNumberOfChannels*dwBytesPerSample;
	// Create and lock a working buffer large enough to contain the number of samples in longs for a single channel
	if ((pslWorkingDataBuffer = new SLONG [dwNumberOfSamples]) == NULL)
	{
		err_CouldNotAllocateSineWaveCreateL((unsigned long)(sizeof(SLONG)*dwNumberOfSamples));
		return FAIL;
	}

	// Generate a sine wave in each channel that has a defined frequency configuration parameter
	for (h = 0; h < dwNumberOfChannels; h++)
	{
		memset(pslWorkingDataBuffer, 0, dwNumberOfSamples*sizeof(SLONG));  // Clear the memory buffer
		if (dwFrequencyNumber < pWaveChannelFreqData[h].dwNumberOfFrequencies) // Only generate a sine wave for this channel if a frequency was specified
		{
			dblSamplesPerPeriod = pWaveChannelFreqData[h].pdblSamplesPerPeriod[dwFrequencyNumber];
			dblRadiansPerSample = TWO_PI / dblSamplesPerPeriod;
			i = 0;
			while (i < dwNumberOfSamples)
			{
				dblRadians = dblRadiansPerSample * (double)i;
				pslWorkingDataBuffer[i] = (SLONG)((double)dwMaxAmplitudedAllowed * sin(dblRadians));
				i++;
			}
		}
		// Now the working data buffer contains a frequency's data for each channel.  Now we need to scale them down and put them in their final buffer

		dblScaleFactor = (double)pWaveChannelFreqData[h].dwNumberOfFrequencies  * pow((double)10, ((double)dwAttenuationDB/(double)20));
		dwSampleOffset = h*dwBytesPerSample;
		if (dwBytesPerSample == 1)  // Generate 8 bit output
		{
			for (i = 0; i < dwNumberOfSamples; i++)
			{
				dwTempDWORD = (DWORD)(pslWorkingDataBuffer[i]/dblScaleFactor);  // Keep this line and the one below it seperate, or the optimizing compiler will goof up
				pbDataBuffer[i*dwNumberOfChannels + h] = (BYTE)((dwTempDWORD+128) & 0xff);  // 8-bit PCM is unsigned instead of signed
			}
		}
		else if (dwBytesPerSample == 2)  // Generate 16 bit output
		{
			for (i = 0; i < dwNumberOfSamples; i++)
			{
				dwTempDWORD = (DWORD)(pslWorkingDataBuffer[i]/dblScaleFactor);  // Keep this line and the one below it seperate, or the optimizing compiler will goof up
				pwDataBuffer[i*dwNumberOfChannels + h] = (WORD)(dwTempDWORD & 0xffff);
			}
		}
		else if (dwBytesPerSample == 3)  // Generate 24 bit output
		{
			for (i = 0; i < dwNumberOfSamples; i++)
			{
				dwTempDWORD = (DWORD)(pslWorkingDataBuffer[i]/dblScaleFactor);  // Keep this line and the one below it seperate, or the optimizing compiler will goof up
				pbDataBuffer[i*dwNumberOfChannelsTimesBytesPerSample + dwSampleOffset] = (BYTE)(dwTempDWORD & 0xff);
				pbDataBuffer[i*dwNumberOfChannelsTimesBytesPerSample + dwSampleOffset + 1] = (BYTE)((dwTempDWORD >> 8) & 0xff);
				pbDataBuffer[i*dwNumberOfChannelsTimesBytesPerSample + dwSampleOffset + 2] = (BYTE)((dwTempDWORD >> 16) & 0xff);
			}
		}
		else  // Assume 32 bit output
		{
			for (i = 0; i < dwNumberOfSamples; i++)
			{
				dwTempDWORD = (DWORD)(pslWorkingDataBuffer[i]/dblScaleFactor);  // Keep this line and the one below it seperate, or the optimizing compiler will goof up
				pdwDataBuffer[i*dwNumberOfChannels + h] = dwTempDWORD;
			}
		}
	}
	delete [] pslWorkingDataBuffer;
	pslWorkingDataBuffer = NULL;
	return(PASS);
}

DWORD CXModuleAudio::dwGetPossessionOfAudioResources(DWORD dwTimeout)
{
	DWORD x;
	int onesecond = 1000;

	// Use a mutex to avoid resource conflicts
	//
	// First, try creating the mutex. If it exists, try to open it.
	//
	hXMTAAudioMutex = CreateMutexA (NULL, FALSE, XMTA_AUDIO_MUTEX_NAME);
//	if ((hXMTAAudioMutex == NULL) && (GetLastError() == ERROR_ALREADY_EXISTS))
//		hXMTAAudioMutex = OpenMutexA (NULL, FALSE, XMTA_AUDIO_MUTEX_NAME);
	if (hXMTAAudioMutex == NULL) // still NULL?, don't even try accessing
	{
		err_MutexHandle(_T(XMTA_AUDIO_MUTEX_NAME));
		return FAIL;
	}

	// Now try to get possession of the mutex
	//
	for (x = 0; x < (dwTimeout/(DWORD)onesecond); x++)
	{
		if (WaitForSingleObject(hXMTAAudioMutex, onesecond) == WAIT_OBJECT_0)
			return (PASS);
		if (CheckAbort(HERE))
			return (FAIL);
	}
	err_MutexTimeout(_T(XMTA_AUDIO_MUTEX_NAME));
	return (FAIL);
}

void CXModuleAudio::vReleasePossessionOfAudioResources ()
{
	if (hXMTAAudioMutex != (HANDLE) -1)
	{
		ReleaseMutex (hXMTAAudioMutex);
		CloseHandle (hXMTAAudioMutex);
		hXMTAAudioMutex = (HANDLE) -1;
	}
}

// This routine computes total DC offset values for all channels and resorts the data into seperate FFT buffers sections (one per channel); each value is a signed DWORD regardless of how big the recorded values were
DWORD CXModuleAudio::dwTotalDCOffset(DWORD *pdwSourceBuffer, SLONG *pslDestBuffer, DWORD dwBitsPerSample, DWORD dwFFTSize, DWORD dwNumberOfChannels, double *pdblDCOffsetPercents)
{
	DWORD i, j;
	DWORD dwBytesPerSample;
	SLONG *pslTempPointer;
	unsigned char *pucTempPointer;
	short *pshTempPointer;
	SLONG slTotalAmplitudeTemp;
	DWORD dwTempDWORD;

	pucTempPointer = (unsigned char *)pdwSourceBuffer;  // A character pointer instead of a DWORD pointer
	pshTempPointer = (short *)pdwSourceBuffer;  // A signed 16-bit pointer instead of a DWORD pointer
	pslTempPointer = (SLONG *)pdwSourceBuffer;  // A signed 32-bit pointer instead of DWORD pointer
	dwBytesPerSample = (dwBitsPerSample+7)/8;  // This assumes that 17-24 but samples will use 3 bytes per sample
	for (i = 0; i < dwNumberOfChannels; i++)   // reconfigure for all channels
	{
		slTotalAmplitudeTemp = 0;   // Start the amplitude total at 0
		if (dwBytesPerSample == 1)  // Convert from unsigned char values to signed 32-bit values
		{
			for (j = 0; j < dwFFTSize; j++)
			{
				pslDestBuffer[j+(i*dwFFTSize)] = (SLONG)(pucTempPointer[(j*dwNumberOfChannels)+i]) - (SLONG)128;   // Fill dwDestBuffer with the channel sorted contents of dwSourceBuffer
				slTotalAmplitudeTemp = slTotalAmplitudeTemp + pslDestBuffer[j+(i*dwFFTSize)];
			}
		}
		else if (dwBytesPerSample == 2)  // Convert from signed 16-bit values to signed 32-bit values
		{
			for (j = 0; j < dwFFTSize; j++)
			{
				pslDestBuffer[j+(i*dwFFTSize)] = (SLONG)pshTempPointer[(j*dwNumberOfChannels)+i];   // Fill dwDestBuffer with the channel sorted contents of dwSourceBuffer
				slTotalAmplitudeTemp = slTotalAmplitudeTemp + pslDestBuffer[j+(i*dwFFTSize)];
			}
		}
		else if (dwBytesPerSample == 3)  // Convert from signed 24-bit values to signed 32-bit values
		{
			for (j = 0; j < dwFFTSize; j++)
			{
				dwTempDWORD = (DWORD)(pucTempPointer[(j*dwNumberOfChannels*3)+(i*3)]);
				dwTempDWORD |= ((DWORD)(pucTempPointer[(j*dwNumberOfChannels*3)+(i*3)+1]) << 8);
				dwTempDWORD |= ((DWORD)((SLONG)((char)pucTempPointer[(j*dwNumberOfChannels*3)+(i*3)+2])) << 16);  // Convert to a char, then an SLONG, and then a DWORD so that the sign is extended to fill the upper 16 bits of the DWORD
				pslDestBuffer[j+(i*dwFFTSize)] = (SLONG)dwTempDWORD;
				slTotalAmplitudeTemp = slTotalAmplitudeTemp + pslDestBuffer[j+(i*dwFFTSize)];
			}
		}
		else  // Convert from signed 32-bit values to signed 32-bit values
		{
			for (j = 0; j < dwFFTSize; j++)
			{
				pslDestBuffer[j+(i*dwFFTSize)] = pslTempPointer[(j*dwNumberOfChannels)+i];   // Fill dwDestBuffer with the channel sorted contents of dwSourceBuffer
				slTotalAmplitudeTemp = slTotalAmplitudeTemp + pslDestBuffer[j+(i*dwFFTSize)];
			}
		}
		if (slTotalAmplitudeTemp < 0)  // Make sure the offset is positive
			slTotalAmplitudeTemp = 0 - slTotalAmplitudeTemp;
		pdblDCOffsetPercents[i] = (double)slTotalAmplitudeTemp/(double)FFT_SIZE; // What is the average (the DC Offset of a single bin)?
		pdblDCOffsetPercents[i] = (pdblDCOffsetPercents[i] * (double)100)/pow((double)2, (double)(dwBitsPerSample-1)); // Now compute the percent vs. full scale
	}
	return(PASS);
}

// This routine computes the noise limit of each FFT bin based on configuration parameter settings for the noise limits
void CXModuleAudio::vCalculateRecordNoiseLimit(double *pdblNoiseLimitValues, DWORD dwFFTSize, DWORD dwBitsPerSample, DWORD dwSampleRate, WAVE_RECORD_NOISE_LIMIT* pwrnlWaveRecordNoiseLimit, DWORD dwNumberOfNoiseRanges)
{
	DWORD i, j;
	double dblZeroDBValue, dblSlope, dblLevelStart;
	DWORD dwStartBin, dwStopBin;

	dblZeroDBValue = (double)((double)((DWORD)1 << (dwBitsPerSample-2))*(double)dwFFTSize);  // This is the 0 dB value for this recorded sample
	for (i = 0; i < dwFFTSize/2; i++)
		pdblNoiseLimitValues[i] = IGNORE_BIN_VALUE;  // Fill the buffer initially with values so large that no FFT bin could reach this high; this is an indication to the test to ignore these bins when evaluating results
	for (i = 0; i < dwNumberOfNoiseRanges; i++)  // Now apply all noise ranges, one at a time
	{
		if (pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStartFreq < 1)
			pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStartFreq = 1;  // 1 Hz is the minimum frequency.  Otherwise, things get really messy with some of the computations below
		if (pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStopFreq < 1)
			pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStopFreq = 1;  // 1 Hz is the minimum frequency.  Otherwise, things get really messy with some of the computations below
		dwStartBin = (DWORD)((pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStartFreq * (double)dwFFTSize)/(double)dwSampleRate + (double)1.0);
		// If the value was EXACTLY aligned on an FFT bin, correct for the mathematical error
		if ((double)dwStartBin == (pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStartFreq * (double)dwFFTSize)/(double)dwSampleRate + (double)1.0)
			dwStartBin = dwStartBin - 1;
		dwStopBin = (DWORD)((pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStopFreq * (double)dwFFTSize)/(double)dwSampleRate);
		dwStopBin = min(dwStopBin, (dwFFTSize/2) - 1);  // Make sure we don't run off the end of the buffer
		if (dwStartBin < dwStopBin)  // If the start and stop freqs are the same, then don't pay any attention to this range
		{
//			dblSlope = (pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStopLevel - pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStartLevel)/(pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStopFreq - pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStartFreq);  // This would compute a linear slope across the frequency range
			dblSlope = (pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStopLevel - pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStartLevel)/(log10(pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStopFreq) - log10(pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStartFreq));  // This computes a logarithmic slope across the frequency range
			dblLevelStart = pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStartLevel;
			for (j = dwStartBin; j <= dwStopBin; j++)
			{
//				pdblNoiseLimitValues[j] = dblZeroDBValue*pow(10.0, (dblSlope*((((double)j*(double)dwSampleRate)/(double)dwFFTSize) - (pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStartFreq)) + dblLevelStart)/20.0);  // This computes the noise level using a linear frequency range
				pdblNoiseLimitValues[j] = dblZeroDBValue*pow(10.0, (dblSlope*(log10(((double)j*(double)dwSampleRate)/(double)dwFFTSize) - log10(pwrnlWaveRecordNoiseLimit[i].dblNoiseLimitStartFreq)) + dblLevelStart)/20.0);  // This computes the noise level using a logarithmic frequency range
			}
		}
	}
}

// This routine performs an FFT on a buffer of data
// pdwMagnitude is a pre-allocated buffer of DWORDs length FFTSize/2
DWORD CXModuleAudio::dwComputeFFT(SLONG *pslInputData, DWORD *pdwMagnitude, DWORD dwFFTBits, DWORD dwFFTSize, double dblTwoPI, double dblPIOverTwo, DWORD dwWindowType)
{
	DWORD i, j, t, m, dwNum;
	DWORD dwTemp;
	double dblOmegaTemp;  /* Contains the polar values temporarily while computing the cartesian omega[] */
	double dblTempr, dblTempi;  /* A temporary real and imaginary variable set */
	int l;
	double *pdblTransformr, *pdblTransformi;
	double *pdblOmegar, *pdblOmegai;

	if (gpdwPI == NULL)  // Only spend the time computing PI if we have not done so already
	{
		if ((gpdwPI = new DWORD[dwFFTSize]) == NULL)
		{
			err_CouldNotAllocateFFTPiBufferL((unsigned long)(sizeof(DWORD)*dwFFTSize));
			return(FAIL);
		}
		for (i = 0; i < dwFFTSize; i++)  /* Generate the pi function for FFT; just binary reverse all values of i */
		{
			dwTemp = i;
			gpdwPI[i] = 0;
			for (j = 0; j < dwFFTBits; j++)
			{
				gpdwPI[i] = ((dwTemp & (DWORD)1) | (gpdwPI[i] << (DWORD)1));
				dwTemp = dwTemp >> (DWORD)1;
			}
		}
	}
	if ((pdblTransformr = new double[dwFFTSize]) == NULL)
	{
		err_CouldNotAllocateFFTRTFBufferL((unsigned long)(sizeof(double)*dwFFTSize));
		return(FAIL);
	}
	if ((pdblTransformi = new double[dwFFTSize]) == NULL)
	{
		delete [] pdblTransformr;
		pdblTransformr = NULL;
		err_CouldNotAllocateFFTITFBufferL((unsigned long)(sizeof(double)*dwFFTSize));
		return(FAIL);
	}
	if ((pdblOmegar = new double[dwFFTSize]) == NULL)
	{
		delete [] pdblTransformi;
		pdblTransformi = NULL;
		delete [] pdblTransformr;
		pdblTransformr = NULL;
		err_CouldNotAllocateFFTROFBufferL((unsigned long)(sizeof(double)*dwFFTSize));
		return(FAIL);
	}
	if ((pdblOmegai = new double[dwFFTSize]) == NULL)
	{
		delete [] pdblOmegar;
		pdblOmegar = NULL;
		delete [] pdblTransformi;
		pdblTransformi = NULL;
		delete [] pdblTransformr;
		pdblTransformr = NULL;
		err_CouldNotAllocateFFTIOFBufferL((unsigned long)(sizeof(double)*dwFFTSize));
		return(FAIL);
	}
	if (dwWindowType == COS_SQUARED_WINDOW)
	{
		for (i = 0; i < dwFFTSize; i+=2)  /* Calculate the starting point for transform[] */
		{
			pdblTransformr[i] = (double)pslInputData[gpdwPI[i]]*pow(cos((((double)gpdwPI[i]-(double)(dwFFTSize/2))/(double)(dwFFTSize/2))*dblPIOverTwo), 2) + (double)pslInputData[gpdwPI[i+1]]*pow(cos((((double)gpdwPI[i+1]-(double)(dwFFTSize/2))/(double)(dwFFTSize/2))*dblPIOverTwo), 2);
			pdblTransformi[i] = 0;
			pdblTransformr[i+1] = (double)pslInputData[gpdwPI[i]]*pow(cos((((double)gpdwPI[i]-(double)(dwFFTSize/2))/(double)(dwFFTSize/2))*dblPIOverTwo), 2) - (double)pslInputData[gpdwPI[i+1]]*pow(cos((((double)gpdwPI[i+1]-(double)(dwFFTSize/2))/(double)(dwFFTSize/2))*dblPIOverTwo), 2);
			pdblTransformi[i+1] = 0;
		}
	}
	else
	{
		for (i = 0; i < dwFFTSize; i+=2)  /* Calculate the starting point for transform[] */
		{
			pdblTransformr[i] = (double)(pslInputData[gpdwPI[i]] + pslInputData[gpdwPI[i+1]]);
			pdblTransformi[i] = 0;
			pdblTransformr[i+1] = (double)(pslInputData[gpdwPI[i]] - pslInputData[gpdwPI[i+1]]);
			pdblTransformi[i+1] = 0;
		}
	}
	for (i = 0; i < dwFFTSize; i++)  /* Calculate omega[], the dwFFTSize roots of unity for this transform */
	{
		dblOmegaTemp = ((dblTwoPI*(double)i)/(double)dwFFTSize);
		pdblOmegar[i] = cos(dblOmegaTemp);
		pdblOmegai[i] = sin(dblOmegaTemp);
	}
	m = dwFFTSize/2;
	dwNum = 2;
	for (l = (dwFFTBits-2); l >= 0; l--)
	{
		m = m/2;
		dwNum = dwNum*2;
		for (t = 0; t <= (DWORD)(((1 << l)-1)*dwNum); t += dwNum)
		{
			for (j = 0; j <= ((dwNum/2) - 1); j++)
			{
				/* First multiply omega[m*j] with transform[t+(dwNum/2)+j] */
				dblTempr = ((pdblOmegar[m*j]*pdblTransformr[t+(dwNum/2)+j]) - (pdblOmegai[m*j]*pdblTransformi[t+(dwNum/2)+j]));
				dblTempi = ((pdblOmegar[m*j]*pdblTransformi[t+(dwNum/2)+j]) + (pdblOmegai[m*j]*pdblTransformr[t+(dwNum/2)+j]));
				/* Second, compute transform[t+(dwNum/2)+j] */
				pdblTransformr[t+(dwNum/2)+j] = pdblTransformr[t+j] - dblTempr;
				pdblTransformi[t+(dwNum/2)+j] = pdblTransformi[t+j] - dblTempi;
				/* Finally, compute transform[t+j] */
				pdblTransformr[t+j] = pdblTransformr[t+j] + dblTempr;
				pdblTransformi[t+j] = pdblTransformi[t+j] + dblTempi;
			}
		}
	}
	for (i = 0; i < (dwFFTSize/2); i++)
	{
		pdwMagnitude[i] = (DWORD)sqrt(pdblTransformr[i]*pdblTransformr[i] + pdblTransformi[i]*pdblTransformi[i]);
		// We currently aren't returning dwMaxValue.  If we do in the future, the routine
		// below should be modified to ignore all bins below a certain frequency (like 20 Hz)
		// instead of arbitrarily ignoring the first 2 bins always.
		//if ((*dwMaxValue < dwMagnitude[i]) && (i >= 2))
		//	*dwMaxValue = dwMagnitude[i];
	}
	
	delete [] pdblOmegai;
	pdblOmegai = NULL;
	delete [] pdblOmegar;
	pdblOmegar = NULL;
	delete [] pdblTransformi;
	pdblTransformi = NULL;
	delete [] pdblTransformr;
	pdblTransformr = NULL;
	return(PASS);
}

// This routine makes sure all expected frequencies are within their amplitude tolerances, and 
DWORD CXModuleAudio::dwCompareFrequenciesAndNoise(DWORD *pdwMagnitude, DWORD dwFFTSize, WAVE_RECORD_CONFIGURATION_DATA wrcdWaveRecordInfo, DWORD dwChannel, double *pdblNoiseLimitValues)
{
	DWORD i, j;
	DWORD *pdwCopyOfMagnitudeBuffer = NULL;
	double dblFrequencyDB;
	double dblZeroDBValue;
	DWORD dwTempMagnitude;
	DWORD dwStatus = PASS;
	DWORD dwStartBin, dwStopBin;
	double dblMaxFrequencyDB = -1000.0, dblMaxFrequency = 1, dblMinFrequencyDB = 1000.0, dblMinFrequency = 1;
	DWORD dwMaxNoiseLevel = 1, dwMaxNoiseBin = 1;
	DWORD dwUnexpectedFrequencyErrors = 0;

	if ((pdwCopyOfMagnitudeBuffer = new DWORD [dwFFTSize/2]) == NULL)  // Allocate a buffer to work with
	{
		err_CouldNotAllocateFFTMagnitudeL((dwFFTSize/2) * sizeof(DWORD));
		return(FAIL);
	}
	memcpy(pdwCopyOfMagnitudeBuffer, pdwMagnitude, (dwFFTSize/2)*sizeof(DWORD));
	dblZeroDBValue = (double)((double)((DWORD)1 << (wrcdWaveRecordInfo.dwBitsPerSample-2))*(double)dwFFTSize);  // This is the 0 dB value for this recorded sample

	// Compare all expected frequencies to their acceptable magnitude ranges and report any errors.  Also zero out the magnitudes of those freuqencies for the noise level comparison below
	for (i = 0; i < wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].dwNumberOfFrequencies; i++)
	{
		dwTempMagnitude = 0;
		dwStartBin = min((DWORD)((((wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblFrequency[i]-wrcdWaveRecordInfo.dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)wrcdWaveRecordInfo.dwSampleRate) + (double)0.5), (DWORD)(((wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblFrequency[i]*(double)dwFFTSize)/(double)wrcdWaveRecordInfo.dwSampleRate) * ((double)1.0 - (wrcdWaveRecordInfo.dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
		dwStopBin = max((DWORD)((((wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblFrequency[i]+wrcdWaveRecordInfo.dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)wrcdWaveRecordInfo.dwSampleRate) + (double)0.5), (DWORD)(((wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblFrequency[i]*(double)dwFFTSize)/(double)wrcdWaveRecordInfo.dwSampleRate) * ((double)1.0 + (wrcdWaveRecordInfo.dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
		for (j = dwStartBin; j <= dwStopBin; j++)
		{
			if ((j == 0) || (j > dwFFTSize/2))  // Don't try checking FFT bins that are out of range
				continue;
			if (pdwMagnitude[j] > dwTempMagnitude)
				dwTempMagnitude = pdwMagnitude[j];
		}
		if (dwTempMagnitude == 0) // This avoids a log(0) function, which returns a strange value
			dblFrequencyDB = 20.0*log10(1/dblZeroDBValue);
		else
			dblFrequencyDB = 20.0*log10(dwTempMagnitude/dblZeroDBValue);
		if (dblMaxFrequencyDB < dblFrequencyDB) // Keep track of the maximum frequency amplitude on this channel
		{
			dblMaxFrequencyDB = dblFrequencyDB;
			dblMaxFrequency = wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblFrequency[i];
		}
		if (dblMinFrequencyDB > dblFrequencyDB) // Keep track of the minimum frequency amplitude on this channel
		{
			dblMinFrequencyDB = dblFrequencyDB;
			dblMinFrequency = wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblFrequency[i];
		}
		if ((dblFrequencyDB < (wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblAmplitude[i] - wrcdWaveRecordInfo.dblAmplitudeTolerance)) || (dblFrequencyDB > (wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblAmplitude[i] + wrcdWaveRecordInfo.dblAmplitudeTolerance)))  // The received frequency magnitude is outside of the acceptable range
		{
			err_DidNotFindExpectedSineFreqLSDDDD(dwChannel, (wrcdWaveRecordInfo.pchChannelLabel != NULL) ? wrcdWaveRecordInfo.pchChannelLabel[dwChannel] : L"", wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblFrequency[i], wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblAmplitude[i] + wrcdWaveRecordInfo.dblAmplitudeTolerance, wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblAmplitude[i] - wrcdWaveRecordInfo.dblAmplitudeTolerance, dblFrequencyDB);
			dwStatus = FAIL;
		}
		pdwCopyOfMagnitudeBuffer[(DWORD)(((wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblFrequency[i]*(double)dwFFTSize)/(double)wrcdWaveRecordInfo.dwSampleRate) + (double)0.5)] = 0;
	}
	
	// Set all of the bins within the dblFrequencyShiftPercentAllowed to 0 so that they aren't compared against noise
	for (i = 0; i < wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].dwNumberOfFrequencies; i++)
	{
		dwStartBin = min((DWORD)((((wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblFrequency[i]-wrcdWaveRecordInfo.dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)wrcdWaveRecordInfo.dwSampleRate) + (double)0.5), (DWORD)(((wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblFrequency[i]*(double)dwFFTSize)/(double)wrcdWaveRecordInfo.dwSampleRate) * ((double)1.0 - (wrcdWaveRecordInfo.dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
		dwStopBin = max((DWORD)((((wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblFrequency[i]+wrcdWaveRecordInfo.dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)wrcdWaveRecordInfo.dwSampleRate) + (double)0.5), (DWORD)(((wrcdWaveRecordInfo.WaveChannelFreqData[dwChannel].pdblFrequency[i]*(double)dwFFTSize)/(double)wrcdWaveRecordInfo.dwSampleRate) * ((double)1.0 + (wrcdWaveRecordInfo.dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
		for (j = dwStartBin; j <= dwStopBin; j++)
		{
			if ((j == 0) || (j > dwFFTSize/2))  // Don't try checking FFT bins that are out of range
				continue;
			pdwCopyOfMagnitudeBuffer[j] = 0;
		}
	}

	// Compare all bins to their permitted noise levels
	for (i = 1; i < dwFFTSize/2; i++)  // Don't ever compare the DC bin (bin 0)
	{
		if (pdblNoiseLimitValues[i] != IGNORE_BIN_VALUE)
		{
			if (dwMaxNoiseLevel < pdwCopyOfMagnitudeBuffer[i]) // Keep track of the maximum noise on this channel
			{
				dwMaxNoiseLevel = pdwCopyOfMagnitudeBuffer[i];
				dwMaxNoiseBin = i;
			}
			if (pdwCopyOfMagnitudeBuffer[i] > pdblNoiseLimitValues[i])  // This bin's noise is too high
			{
				err_DetectedUnexpectedSineFreqLSDDD(dwChannel, (wrcdWaveRecordInfo.pchChannelLabel != NULL) ? wrcdWaveRecordInfo.pchChannelLabel[dwChannel] : L"", (double)(i*wrcdWaveRecordInfo.dwSampleRate)/(double)dwFFTSize, 20.0*log10(pdblNoiseLimitValues[i]/dblZeroDBValue), 20.0*log10(pdwCopyOfMagnitudeBuffer[i]/dblZeroDBValue));
				dwStatus = FAIL;
				if (dwUnexpectedFrequencyErrors < 6)
				{
					dwUnexpectedFrequencyErrors++;
				}
				else
				{
					if (CheckAbort(HERE))
						break;
				}
			}
		}
	}


	if (wTestNumber == ANALOG_PLAYBACK_TEST)
	{
		if (dwChannel == 0)
		{
			ReportStatistic(XAUDIO_ANALOG_MAX_FREQUENCY_AMPLITUDE_LEFT_CHANNEL, _T("%.3lf,%.1lf"), dblMaxFrequencyDB, dblMaxFrequency);
			ReportStatistic(XAUDIO_ANALOG_MIN_FREQUENCY_AMPLITUDE_LEFT_CHANNEL, _T("%.3lf,%.1lf"), dblMinFrequencyDB, dblMinFrequency);
			ReportStatistic(XAUDIO_ANALOG_MAX_NOISE_LEVEL_LEFT_CHANNEL, _T("%.3lf,%.1lf"), 20.0*log10(dwMaxNoiseLevel/dblZeroDBValue), (double)(dwMaxNoiseBin*wrcdWaveRecordInfo.dwSampleRate)/(double)dwFFTSize);
		}
		else
		{
			ReportStatistic(XAUDIO_ANALOG_MAX_FREQUENCY_AMPLITUDE_RIGHT_CHANNEL, _T("%.3lf,%.1lf"), dblMaxFrequencyDB, dblMaxFrequency);
			ReportStatistic(XAUDIO_ANALOG_MIN_FREQUENCY_AMPLITUDE_RIGHT_CHANNEL, _T("%.3lf,%.1lf"), dblMinFrequencyDB, dblMinFrequency);
			ReportStatistic(XAUDIO_ANALOG_MAX_NOISE_LEVEL_RIGHT_CHANNEL, _T("%.3lf,%.1lf"), 20.0*log10(dwMaxNoiseLevel/dblZeroDBValue), (double)(dwMaxNoiseBin*wrcdWaveRecordInfo.dwSampleRate)/(double)dwFFTSize);
		}
	}
	else if (wTestNumber == ANALOG_SILENCE_TEST)
	{
		if (dwChannel == 0)
		{
//			ReportStatistic(XAUDIO_SILENCE_MAX_FREQUENCY_AMPLITUDE_LEFT_CHANNEL, _T("%.3lf,%.1lf"), dblMaxFrequencyDB, dblMaxFrequency);
//			ReportStatistic(XAUDIO_SILENCE_MIN_FREQUENCY_AMPLITUDE_LEFT_CHANNEL, _T("%.3lf,%.1lf"), dblMinFrequencyDB, dblMinFrequency);
			ReportStatistic(XAUDIO_SILENCE_MAX_NOISE_LEVEL_LEFT_CHANNEL, _T("%.3lf,%.1lf"), 20.0*log10(dwMaxNoiseLevel/dblZeroDBValue), (double)(dwMaxNoiseBin*wrcdWaveRecordInfo.dwSampleRate)/(double)dwFFTSize);
		}
		else
		{
//			ReportStatistic(XAUDIO_SILENCE_MAX_FREQUENCY_AMPLITUDE_RIGHT_CHANNEL, _T("%.3lf,%.1lf"), dblMaxFrequencyDB, dblMaxFrequency);
//			ReportStatistic(XAUDIO_SILENCE_MIN_FREQUENCY_AMPLITUDE_RIGHT_CHANNEL, _T("%.3lf,%.1lf"), dblMinFrequencyDB, dblMinFrequency);
			ReportStatistic(XAUDIO_SILENCE_MAX_NOISE_LEVEL_RIGHT_CHANNEL, _T("%.3lf,%.1lf"), 20.0*log10(dwMaxNoiseLevel/dblZeroDBValue), (double)(dwMaxNoiseBin*wrcdWaveRecordInfo.dwSampleRate)/(double)dwFFTSize);
		}
	}
	else // assume DIGITAL_PLAYBACK_TEST
	{
		if (dwChannel == 0)
		{
			ReportStatistic(XAUDIO_DIGITAL_MAX_FREQUENCY_AMPLITUDE_LEFT_CHANNEL, _T("%.3lf,%.1lf"), dblMaxFrequencyDB, dblMaxFrequency);
			ReportStatistic(XAUDIO_DIGITAL_MIN_FREQUENCY_AMPLITUDE_LEFT_CHANNEL, _T("%.3lf,%.1lf"), dblMinFrequencyDB, dblMinFrequency);
			ReportStatistic(XAUDIO_DIGITAL_MAX_NOISE_LEVEL_LEFT_CHANNEL, _T("%.3lf,%.1lf"), 20.0*log10(dwMaxNoiseLevel/dblZeroDBValue), (double)(dwMaxNoiseBin*wrcdWaveRecordInfo.dwSampleRate)/(double)dwFFTSize);
		}
		else
		{
			ReportStatistic(XAUDIO_DIGITAL_MAX_FREQUENCY_AMPLITUDE_RIGHT_CHANNEL, _T("%.3lf,%.1lf"), dblMaxFrequencyDB, dblMaxFrequency);
			ReportStatistic(XAUDIO_DIGITAL_MIN_FREQUENCY_AMPLITUDE_RIGHT_CHANNEL, _T("%.3lf,%.1lf"), dblMinFrequencyDB, dblMinFrequency);
			ReportStatistic(XAUDIO_DIGITAL_MAX_NOISE_LEVEL_RIGHT_CHANNEL, _T("%.3lf,%.1lf"), 20.0*log10(dwMaxNoiseLevel/dblZeroDBValue), (double)(dwMaxNoiseBin*wrcdWaveRecordInfo.dwSampleRate)/(double)dwFFTSize);
		}
	}

	delete [] pdwCopyOfMagnitudeBuffer;
	pdwCopyOfMagnitudeBuffer = NULL;
	return(dwStatus);
}

// Traditional audio measurements:
//
// Total Harmonic Distortion plus noise (THD+N) is the ratio of the RMS sum of
// the noise and distortion components (but not the test signal) to 0 dB FS in the
// presence of a test signal close to FS.  To meaure this, send in a single sine wave
// of around 1 kHz between 0 and -3 dB FS.  After the FFT, RMS all bins except for the
// fundamental frequency, and compare this value to 0 dB FS.
//
// Dynamic Range (DR) is similar to THD+N, except that the test signal is -60 dB FS.
// All measurements are done the same way as with THD+N.
//
// Signal-to-Noise ratio (S/N) is not well defined.  The most common definition is the
// same as for THD+N.
//
// To compute S/N ratio in our test, we add up the amplitudes of the fundamental frequencies
// and then ratio that value to the RMS of everything else (noise, harmonics, etc).  This
// is a "real" THD+N calculation if a single near 0 dB FS sine wave of near 1000 Hz is
// applied, but will also yield satisfactory results is multiple frequencies are used
// simultaneously.
DWORD CXModuleAudio::dwComputeSignalToNoise(DWORD *pdwMagnitude, DWORD dwFFTSize, double dblFrequencyShiftPercentAllowed, double *pdblFrequencies, DWORD dwNumberOfFrequencies, DWORD dwSampleRate, double *pdblNoiseLimitValues, double dblFrequencyShiftHzAllowed, double *pdblSignalToNoiseRatioToReturn)
{
	DWORD i, j;
	double dblTotalFreqAmplitude = 0;
	double *pdblCopyOfNoiseLimitBuffer = NULL;
	DWORD dwTempMagnitude;
	DWORD dwStartBin, dwStopBin;
	DWORD dwNumberOfBinsSummed = 0;
	double dblTotalNoiseSquared = 0, dblMeanNoiseSquared = 0, dblRootMeanSquared = 0;

	if ((pdblCopyOfNoiseLimitBuffer = new double [dwFFTSize/2]) == NULL)  // Allocate a buffer to work with
	{
		err_CouldNotAllocateNoiseLimitsL((unsigned long)(sizeof(double)*(dwFFTSize/2)));
		return(FAIL);
	}
	memcpy(pdblCopyOfNoiseLimitBuffer, pdblNoiseLimitValues, (dwFFTSize/2)*sizeof(double));
	for (i = 0; i < dwNumberOfFrequencies; i++)  // Add up the magnitudes of the received frequencies
	{
		dwTempMagnitude = 0;
		dwStartBin = min((DWORD)((((pdblFrequencies[i]-dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((pdblFrequencies[i]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 - (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
		dwStopBin = max((DWORD)((((pdblFrequencies[i]+dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((pdblFrequencies[i]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 + (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
		for (j = dwStartBin; j <= dwStopBin; j++)
		{
			if ((j == 0) || (j > dwFFTSize/2))  // Don't try checking FFT bins that are out of range
				continue;
			if (pdwMagnitude[j] > dwTempMagnitude)
				dwTempMagnitude = pdwMagnitude[j];
			pdblCopyOfNoiseLimitBuffer[j] = IGNORE_BIN_VALUE;
		}
		dblTotalFreqAmplitude = dblTotalFreqAmplitude + (double)dwTempMagnitude;
	}
	for (i = 0; i < dwFFTSize/2; i++)  // Now look at every bin that is not to be ignored to see which one has the largest magnitude
	{
		if (pdblCopyOfNoiseLimitBuffer[i] != IGNORE_BIN_VALUE)
		{
			dwNumberOfBinsSummed += 1;
			dblTotalNoiseSquared = dblTotalNoiseSquared + ((double)pdwMagnitude[i] * (double)pdwMagnitude[i]);
		}
	}
	dblMeanNoiseSquared = dblTotalNoiseSquared/(double)dwNumberOfBinsSummed;
	if (dblMeanNoiseSquared < 1)
		dblMeanNoiseSquared = 1; // Just for safety with calculations below
	dblRootMeanSquared = (double)sqrt(dblMeanNoiseSquared);
//ReportDebug (BIT0, L"NumberOfBinsSummed = %lu\n", dwNumberOfBinsSummed);
//ReportDebug (BIT0, L"RMS = %lf\n", dblRootMeanSquared);

	*pdblSignalToNoiseRatioToReturn = 20*log10(dblTotalFreqAmplitude/dblRootMeanSquared); // Compute the found S/N ratio
//ReportDebug (BIT0, L"S/N ratio = %lf\n", *pdblSignalToNoiseRatioToReturn);
	delete [] pdblCopyOfNoiseLimitBuffer;
	pdblCopyOfNoiseLimitBuffer = NULL;

	return(PASS);
}

// This routine computes the dB value between the sum of the fundamental frequencies received on this
// channel and the RMS of the values obtained from all other bins excluding those corresponding to playback and record
// frequencies on other channels.
DWORD CXModuleAudio::dwComputeSignalToNoiseIgnoringCrosstalk(DWORD *pdwMagnitude, DWORD dwFFTSize, double dblFrequencyShiftPercentAllowed, WAVE_RECORD_CHANNEL_FREQ_DATA *WaveRecordChannelFreqData, DWORD dwRecordNumberOfChannels, WAVE_PLAYBACK_CHANNEL_FREQ_DATA *WavePlaybackChannelFreqData, DWORD dwPlaybackNumberOfChannels, DWORD dwChannelNumber, DWORD dwSampleRate, double *pdblNoiseLimitValues, double dblFrequencyShiftHzAllowed, double *pdblSignalToNoiseRatioToReturn)
{
	DWORD i, j, k;
	double dblTotalFreqAmplitude = 0;
	double *pdblCopyOfNoiseLimitBuffer = NULL;
	DWORD dwTempMagnitude;
	DWORD dwStartBin, dwStopBin;
	DWORD dwNumberOfBinsSummed = 0;
	double dblTotalNoiseSquared = 0, dblMeanNoiseSquared = 0, dblRootMeanSquared = 0;

	if ((pdblCopyOfNoiseLimitBuffer = new double [dwFFTSize/2]) == NULL)  // Allocate a buffer to work with
	{
		err_CouldNotAllocateNoiseLimitsL((unsigned long)(sizeof(double)*(dwFFTSize/2)));
		return(FAIL);
	}
	memcpy(pdblCopyOfNoiseLimitBuffer, pdblNoiseLimitValues, (dwFFTSize/2)*sizeof(double));
	for (i = 0; i < WaveRecordChannelFreqData[dwChannelNumber].dwNumberOfFrequencies; i++)  // Add up the magnitudes of the received frequencies
	{
		dwTempMagnitude = 0;
		dwStartBin = min((DWORD)((((WaveRecordChannelFreqData[dwChannelNumber].pdblFrequency[i]-dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((WaveRecordChannelFreqData[dwChannelNumber].pdblFrequency[i]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 - (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
		dwStopBin = max((DWORD)((((WaveRecordChannelFreqData[dwChannelNumber].pdblFrequency[i]+dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((WaveRecordChannelFreqData[dwChannelNumber].pdblFrequency[i]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 + (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
		for (j = dwStartBin; j <= dwStopBin; j++)
		{
			if ((j == 0) || (j > dwFFTSize/2))  // Don't try checking FFT bins that are out of range
				continue;
			if (pdwMagnitude[j] > dwTempMagnitude)
				dwTempMagnitude = pdwMagnitude[j];
			pdblCopyOfNoiseLimitBuffer[j] = IGNORE_BIN_VALUE;
		}
		dblTotalFreqAmplitude = dblTotalFreqAmplitude + (double)dwTempMagnitude;
	}

	for (i = 0; i < dwRecordNumberOfChannels; i++)  // Now force all bins associated with a fundamental freq on any record channel to 0
	{
		for (j = 0; j < WaveRecordChannelFreqData[i].dwNumberOfFrequencies; j++)
		{
			dwStartBin = min((DWORD)((((WaveRecordChannelFreqData[i].pdblFrequency[j]-dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((WaveRecordChannelFreqData[i].pdblFrequency[j]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 - (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
			dwStopBin = max((DWORD)((((WaveRecordChannelFreqData[i].pdblFrequency[j]+dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((WaveRecordChannelFreqData[i].pdblFrequency[j]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 + (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
			for (k = dwStartBin; k <= dwStopBin; k++)
			{
				if ((k == 0) || (k > dwFFTSize/2))  // Don't try checking FFT bins that are out of range
					continue;
				pdblCopyOfNoiseLimitBuffer[k] = IGNORE_BIN_VALUE;
			}
		}
	}
	for (i = 0; i < dwPlaybackNumberOfChannels; i++)  // Now force all bins associated with a fundamental freq on any playback channel to 0
	{
		for (j = 0; j < WavePlaybackChannelFreqData[i].dwNumberOfFrequencies; j++)
		{
			dwStartBin = min((DWORD)((((WavePlaybackChannelFreqData[i].pdblFrequency[j]-dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((WavePlaybackChannelFreqData[i].pdblFrequency[j]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 - (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
			dwStopBin = max((DWORD)((((WavePlaybackChannelFreqData[i].pdblFrequency[j]+dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((WavePlaybackChannelFreqData[i].pdblFrequency[j]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 + (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
			for (k = dwStartBin; k <= dwStopBin; k++)
			{
				if ((k == 0) || (k > dwFFTSize/2))  // Don't try checking FFT bins that are out of range
					continue;
				pdblCopyOfNoiseLimitBuffer[k] = IGNORE_BIN_VALUE;
			}
		}
	}
	for (i = 0; i < dwFFTSize/2; i++)  // Now look at every bin that is not to be ignored to see which one has the largest magnitude
	{
		if (pdblCopyOfNoiseLimitBuffer[i] != IGNORE_BIN_VALUE)
		{
			dwNumberOfBinsSummed += 1;
			dblTotalNoiseSquared = dblTotalNoiseSquared + ((double)pdwMagnitude[i] * (double)pdwMagnitude[i]);
		}
	}

	dblMeanNoiseSquared = dblTotalNoiseSquared/(double)dwNumberOfBinsSummed;
	if (dblMeanNoiseSquared < 1)
		dblMeanNoiseSquared = 1; // Just for safety with calculations below
	dblRootMeanSquared = (double)sqrt(dblMeanNoiseSquared);
//ReportDebug (BIT0, L"NumberOfBinsSummed = %lu\n", dwNumberOfBinsSummed);
//ReportDebug (BIT0, L"RMS = %lf\n", dblRootMeanSquared);

	*pdblSignalToNoiseRatioToReturn = 20*log10(dblTotalFreqAmplitude/dblRootMeanSquared); // Compute the found S/N ratio
//ReportDebug (BIT0, L"S/N ratio = %lf\n", *pdblSignalToNoiseRatioToReturn);
	delete [] pdblCopyOfNoiseLimitBuffer;
	pdblCopyOfNoiseLimitBuffer = NULL;

	return(PASS);
}

// This routine computes the dB value between the average of the fundamental frequencies received on this
// channel and the largest value obtained from all bins corresponding to playback and record frequencies on other channels.
DWORD CXModuleAudio::dwComputeChannelSeparation(DWORD *pdwMagnitude, DWORD dwFFTSize, double dblFrequencyShiftPercentAllowed, WAVE_RECORD_CHANNEL_FREQ_DATA *WaveRecordChannelFreqData, DWORD dwRecordNumberOfChannels, WAVE_PLAYBACK_CHANNEL_FREQ_DATA *WavePlaybackChannelFreqData, DWORD dwPlaybackNumberOfChannels, DWORD dwChannelNumber, DWORD dwSampleRate, double dblFrequencyShiftHzAllowed, double *pdblChannelSeparationToReturn, double *pdblNoiseLimitValues)
{
	DWORD i, j, k;
	double dblTotalFreqAmplitude = 0;
	double dblAverageFreqAmplitude;
	DWORD *pdwCopyOfMagnitudeBuffer = NULL;
	DWORD dwMaxCrosstalkValue = 0;
	DWORD dwTempMagnitude;
	DWORD dwStartBin, dwStopBin;

	if ((pdwCopyOfMagnitudeBuffer = new DWORD [dwFFTSize/2]) == NULL)  // Allocate a buffer to work with
	{
		err_CouldNotAllocateFFTMagnitudeL((dwFFTSize/2) * sizeof(DWORD));
		return(FAIL);
	}
	memcpy(pdwCopyOfMagnitudeBuffer, pdwMagnitude, (dwFFTSize/2)*sizeof(DWORD));
	for (i = 0; i < WaveRecordChannelFreqData[dwChannelNumber].dwNumberOfFrequencies; i++)  // Add up the magnitudes of the received frequencies
	{
		dwTempMagnitude = 0;
		dwStartBin = min((DWORD)((((WaveRecordChannelFreqData[dwChannelNumber].pdblFrequency[i]-dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((WaveRecordChannelFreqData[dwChannelNumber].pdblFrequency[i]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 - (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
		dwStopBin = max((DWORD)((((WaveRecordChannelFreqData[dwChannelNumber].pdblFrequency[i]+dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((WaveRecordChannelFreqData[dwChannelNumber].pdblFrequency[i]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 + (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
		for (j = dwStartBin; j <= dwStopBin; j++)
		{
			if ((j == 0) || (j > dwFFTSize/2) || (pdblNoiseLimitValues[j] == IGNORE_BIN_VALUE))  // Don't try checking FFT bins that are out of range
				continue;
			if (pdwMagnitude[j] > dwTempMagnitude)
				dwTempMagnitude = pdwMagnitude[j];
			pdwCopyOfMagnitudeBuffer[j] = 0;  // Zero this out so that it does not come into play when we scan this buffer for crosstalk magnitudes
		}
		dblTotalFreqAmplitude = dblTotalFreqAmplitude + (double)dwTempMagnitude;
	}
	dblAverageFreqAmplitude = dblTotalFreqAmplitude/(double)WaveRecordChannelFreqData[dwChannelNumber].dwNumberOfFrequencies;
	// Look at the record frequency bins
	for (i = 0; i < dwRecordNumberOfChannels; i++)  // Look at frequencies from all channels
	{
		for (j = 0; j < WaveRecordChannelFreqData[i].dwNumberOfFrequencies; j++)
		{
			dwTempMagnitude = 0;
			dwStartBin = min((DWORD)((((WaveRecordChannelFreqData[i].pdblFrequency[j]-dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((WaveRecordChannelFreqData[i].pdblFrequency[j]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 - (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
			dwStopBin = max((DWORD)((((WaveRecordChannelFreqData[i].pdblFrequency[j]+dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((WaveRecordChannelFreqData[i].pdblFrequency[j]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 + (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
			for (k = dwStartBin; k <= dwStopBin; k++)
			{
				if ((k == 0) || (k > dwFFTSize/2) || (pdblNoiseLimitValues[k] == IGNORE_BIN_VALUE))  // Don't try checking FFT bins that are out of range
					continue;
				if (pdwCopyOfMagnitudeBuffer[k] > dwTempMagnitude)
					dwTempMagnitude = pdwCopyOfMagnitudeBuffer[k];
			}
			if (dwMaxCrosstalkValue < dwTempMagnitude)
			{
				dwMaxCrosstalkValue = dwTempMagnitude;
/*
wchar_t wszLTemp[200];
wsprintf(wszLTemp, L"A Bin %lu val %lu\n", k, dwMaxCrosstalkValue);
OutputDebugString(wszLTemp);
*/
			}
		}
	}
	// Look at the playback frequency bins
	for (i = 0; i < dwPlaybackNumberOfChannels; i++)  // Look at frequencies from all channels
	{
		for (j = 0; j < WavePlaybackChannelFreqData[i].dwNumberOfFrequencies; j++)
		{
			dwTempMagnitude = 0;
			dwStartBin = min((DWORD)((((WavePlaybackChannelFreqData[i].pdblFrequency[j]-dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((WavePlaybackChannelFreqData[i].pdblFrequency[j]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 - (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
			dwStopBin = max((DWORD)((((WavePlaybackChannelFreqData[i].pdblFrequency[j]+dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((WavePlaybackChannelFreqData[i].pdblFrequency[j]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 + (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
			for (k = dwStartBin; k <= dwStopBin; k++)
			{
				if ((k == 0) || (k > dwFFTSize/2) || (pdblNoiseLimitValues[k] == IGNORE_BIN_VALUE))  // Don't try checking FFT bins that are out of range
					continue;
				if (pdwCopyOfMagnitudeBuffer[k] > dwTempMagnitude)
					dwTempMagnitude = pdwCopyOfMagnitudeBuffer[k];
			}
			if (dwMaxCrosstalkValue < dwTempMagnitude)
			{
				dwMaxCrosstalkValue = dwTempMagnitude;
/*
wchar_t wszLTemp[200];
wsprintf(wszLTemp, L"B Bin %lu val %lu\n", k, dwMaxCrosstalkValue);
OutputDebugString(wszLTemp);
*/
			}
		}
	}

	if (dwMaxCrosstalkValue == 0)
		dwMaxCrosstalkValue = 1;  // Avoid a divide by zero, though this is extremely unlikely
	*pdblChannelSeparationToReturn = 20*log10(dblAverageFreqAmplitude/(double)dwMaxCrosstalkValue); // Compute the found channel separation value
	delete [] pdwCopyOfMagnitudeBuffer;
	pdwCopyOfMagnitudeBuffer = NULL;
	return(PASS);
}

// This routine computes the dB value of the difference between the levels of all of the fundamental frequencies recorded.
#pragma optimize("g", off)
DWORD CXModuleAudio::dwComputeFrequencyResponse(DWORD *pdwMagnitude, DWORD dwFFTSize, double dblFrequencyShiftPercentAllowed, double *pdblFrequencies, DWORD dwNumberOfFrequencies, double dblBaseMagnitude, DWORD dwBitsPerSample, DWORD dwSampleRate, double dblFrequencyShiftHzAllowed, double *pdblFrequencyResponseToReturn, double *pdblNoiseLimitValues)
{
	DWORD i, j;
	double dblTotalFreqAmplitude = 0;
	double dblAverageFreqAmplitude;
	double dblZeroDBValue;
	double dblTempDBDifference;
	DWORD dwTempMagnitude;
	DWORD dwStartBin, dwStopBin;

	*pdblFrequencyResponseToReturn = 0;
	dblZeroDBValue = (double)((double)((DWORD)1 << (dwBitsPerSample-2))*(double)dwFFTSize);  // This is the 0 dB value for this recorded sample
	if (dblBaseMagnitude == AVERAGE_FREQUENCY_RESPONSE_LEVEL)  // Average the BaseMagnitude value
	{
		for (i = 0; i < dwNumberOfFrequencies; i++)  // Add up the magnitudes of the received frequencies
		{
			dwTempMagnitude = 0;
			dwStartBin = min((DWORD)((((pdblFrequencies[i]-dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((pdblFrequencies[i]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 - (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
			dwStopBin = max((DWORD)((((pdblFrequencies[i]+dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((pdblFrequencies[i]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 + (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
			for (j = dwStartBin; j <= dwStopBin; j++)
			{
				if ((j == 0) || (j > dwFFTSize/2) || (pdblNoiseLimitValues[j] == IGNORE_BIN_VALUE))  // Don't try checking FFT bins that are out of range
					continue;
				if (pdwMagnitude[j] > dwTempMagnitude)
					dwTempMagnitude = pdwMagnitude[j];
			}
			dblTotalFreqAmplitude = dblTotalFreqAmplitude + (double)dwTempMagnitude;
		}
		dblAverageFreqAmplitude = dblTotalFreqAmplitude/(double)dwNumberOfFrequencies;
	}
	else
		dblAverageFreqAmplitude = dblZeroDBValue * pow(10.0, (dblBaseMagnitude/20.0));

	for (i = 0; i < dwNumberOfFrequencies; i++)  // Find the largest difference between a fundamental and the base magnitude
	{
		dwTempMagnitude = 1;  // Start at 1 to prevent a log10(0) below
		dwStartBin = min((DWORD)((((pdblFrequencies[i]-dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((pdblFrequencies[i]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 - (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
		dwStopBin = max((DWORD)((((pdblFrequencies[i]+dblFrequencyShiftHzAllowed)*(double)dwFFTSize)/(double)dwSampleRate) + (double)0.5), (DWORD)(((pdblFrequencies[i]*(double)dwFFTSize)/(double)dwSampleRate) * ((double)1.0 + (dblFrequencyShiftPercentAllowed/(double)100.0)) + (double)0.5));
		for (j = dwStartBin; j <= dwStopBin; j++)
		{
			if ((j == 0) || (j > dwFFTSize/2) || (pdblNoiseLimitValues[j] == IGNORE_BIN_VALUE))  // Don't try checking FFT bins that are out of range
				continue;
			if (pdwMagnitude[j] > dwTempMagnitude)
				dwTempMagnitude = pdwMagnitude[j];
		}

		if (dblAverageFreqAmplitude != 0)
			dblTempDBDifference = 20.0*log10(dwTempMagnitude/dblAverageFreqAmplitude);
		else
			dblTempDBDifference = 0;
		if (dblTempDBDifference < 0)
			dblTempDBDifference = 0 - dblTempDBDifference;
		if (*pdblFrequencyResponseToReturn < dblTempDBDifference)
			*pdblFrequencyResponseToReturn = dblTempDBDifference;
	}
	return(PASS);
}
#pragma optimize("", on)

// This routine computes the CRC of a buffer of Bytes
DWORD CXModuleAudio::dwComputeCRC(unsigned char *pucBuffer, DWORD dwNumBytes, DWORD dwCRC)
{
	DWORD i;

	for (i = 0; i < dwNumBytes; i++)
		dwCRC = crctab[(dwCRC ^ pucBuffer[i]) & 0x0ff] ^ (dwCRC >> 8);
	return dwCRC;
}

// This routine releases a set of global variables for every test retry loop
void CXModuleAudio::vStraightenUpTestVariables(void)
{
	DWORD j;

	if (pdwWaveInBuffer != NULL) {delete [] pdwWaveInBuffer; pdwWaveInBuffer = NULL;}
	if (WaveFormatex != NULL) {delete WaveFormatex; WaveFormatex = NULL;}
	if (pslFFTInputBuffer != NULL) {delete [] pslFFTInputBuffer; pslFFTInputBuffer = NULL;}
	if (pdblDCOffsetPercents != NULL) {delete [] pdblDCOffsetPercents; pdblDCOffsetPercents = NULL;}
	if (pdblNoiseLimitValues != NULL) {delete [] pdblNoiseLimitValues; pdblNoiseLimitValues = NULL;}
	if (pdwMagnitudeValues != NULL) {delete [] pdwMagnitudeValues; pdwMagnitudeValues = NULL;}

	// Release these in the order allocated because they lock the buffers that are set to them and will
	// not allow the buffers to be freed until after release.
	if (pSecondaryBufferPre8 != NULL)
	{
		for (j = 0; j < dwNumberOfFrequencies; j++)
		{
			if (pSecondaryBufferPre8[j] != NULL)
			{
				pSecondaryBufferPre8[j]->Release();
				pSecondaryBufferPre8[j] = NULL;
			}
		}
		delete [] pSecondaryBufferPre8;
		pSecondaryBufferPre8 = NULL;
	}
	if (pSecondaryBuffer != NULL)
	{
		for (j = 0; j < dwNumberOfFrequencies; j++)
		{
			if (pSecondaryBuffer[j] != NULL)
			{
				pSecondaryBuffer[j]->Release();
				pSecondaryBuffer[j] = NULL;
			}
		}
		delete [] pSecondaryBuffer;
		pSecondaryBuffer = NULL;
	}
	if (ppdwWaveOutBuffer != NULL)
	{
		for (j = 0; j < dwNumberOfFrequencies; j++)
		{
			if (ppdwWaveOutBuffer[j] != NULL)
			{
				VirtualFree(ppdwWaveOutBuffer[j], 0, MEM_RELEASE);
				ppdwWaveOutBuffer[j] = NULL;
			}
		}
		delete [] ppdwWaveOutBuffer;
		ppdwWaveOutBuffer = NULL;
	}

	if (pPrimaryBuffer != NULL)
	{
		pPrimaryBuffer->Release();
		pPrimaryBuffer = NULL;
	}
	if (hDSWnd != NULL)
	{
		CloseHandle(hDSWnd);
		hDSWnd = NULL;
	}
	if (pDSoundOut != NULL)
	{
		pDSoundOut->Release();
		pDSoundOut = NULL;
	}
	vReleasePossessionOfAudioResources();
	if (gpF != NULL)
	{
		fclose(gpF);
		gpF = NULL;
	}
}

#ifndef _XBOX
BOOL CALLBACK CXModuleAudio::bDSEnumCallback(LPGUID lpGuid, LPCSTR lpcstrDescription, LPCSTR lpcstrModule, LPVOID lpContext)
{
	UNREFERENCED_PARAMETER (lpcstrModule);
	CXModuleAudio *pthis = (CXModuleAudio *)lpContext;

	if (lpGuid != 0) // The system's primary sound driver may return a NULL LPGUID
	{
		if (wcsicmp((LPCTSTR)lpcstrDescription, pthis->gpWavePlaybackConfigurationData->pwszAudioOutDevName) == 0)  // We found a match
		{
			pthis->gpWavePlaybackConfigurationData->pAudioOutDevToUseGUID = lpGuid;  // Use this device
			pthis->ReportDebug(0x01, L"Found a match: %s\n", lpcstrDescription);
		}
		// If pAudioOutDevToUseGUID is NULL, then this is the first device.  Use it as a default.  If a real match exists, this will be overwritten by the match in a later CALLBACK to this function
		if (pthis->gpWavePlaybackConfigurationData->pAudioOutDevToUseGUID == NULL)
			pthis->gpWavePlaybackConfigurationData->pAudioOutDevToUseGUID = lpGuid;  // Use this device as default
	}
	if (lpcstrDescription != NULL)
		pthis->ReportDebug(0x01, L"Found DirectSound device: %s\n", lpcstrDescription);
	return TRUE;
}
#endif


// Need to add proper CheckAbort(HERE) calls.  Especially need them when waiting on other events
// Are the S/N and S/N/C calculations correct?