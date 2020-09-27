/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		agcva1.cpp
 *  Content:	Concrete class that implements CAutoGainControl
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *  12/01/99	pnewson Created it
 *  01/14/2000	rodtoll	Plugged memory leak
 *  01/21/2000	pnewson	Fixed false detection at start of audio stream
 *  					Raised VA_LOW_ENVELOPE from (2<<8) to (3<<8)
 *  01/24/2000	pnewson	Fixed return code on Deinit
 *  01/31/2000	pnewson re-add support for absence of DVCLIENTCONFIG_AUTOSENSITIVITY flag
 *  02/08/2000	rodtoll	Bug #131496 - Selecting DVSENSITIVITY_DEFAULT results in voice
 *						never being detected 
 *  03/03/2000	rodtoll	Updated to handle alternative gamevoice build.   
 *  04/20/2000  rodtoll Bug #32889 - Unable to run on non-admin accounts on Win2k
 *  04/20/2000	pnewson Tune AGC algorithm to make it more agressive at 
 *						raising the recording volume.
 *  04/25/2000  pnewson Fix to improve responsiveness of AGC when volume level too low
 *  07/12/2000	rodtoll	Analyze Data needs optimization
 *
 ***************************************************************************/

/*

How this voice activation code works:

The idea is this. The power of the noise signal is pretty much constant over
time. The power of a voice signal varies considerably over time. The power of
a voice signal is not always high however. Weak frictive noises and such do not
generate much power, but since they are part of a stream of speech, they represent
a dip in the power, not a constant low power like the noise signal. We therefore 
associate changes in power with the presence of a voice signal.

If it works as expected, this will allow us to detect voice activity even
when the input volume, and therefore the total power of the signal, is very
low. This in turn will allow the auto gain control code to be more effective.

To estimate the power of the signal, we run the absolute value of the input signal
through a recursive digital low pass filter. This gives us the "envelope" signal.
[An alternative way to view this is a low frequency envelope signal modulated by a 
higher frequency carrier signal. We're extracting the low frequency envelope signal.]

*/


#include "dvntos.h"
#include "in_core.h"
#include <dvoicep.h>

#include "va.h"


// The following VA parameters were optimized for what I believe to be
// the hardest configuration: A cheap open stick mic with external speakers,
// with Echo Suppression turned on. Echo suppression penalizes false positives
// harshly, since the receiver cannot send which receiving the "noise". If 
// the VA parameters work for this case, then they should be fine for the 
// much better signal to noise ratio provided by a headset or collar mic.
// (As long as the user does not breathe directly on the headset mic.)
//
// Two source-to-mic distances were tested during tuning.
//
// 1) Across an enclosed office (approx 8 to 10 feet)
// 2) Seated at the workstation (approx 16 to 20 inches)
//
// At distance 1, the AGC was never invoked, gain was at 100%
// At distance 2, the AGC would take the mic down a few ticks.
//
// The office enviroment had the background noise from 3 computers,
// a ceiling vent, and a surprisingly noisy fan from the ethernet
// hub. There is no background talking, cars, trains, or things of
// that nature.
//
// Each parameter was tuned separately to reject 100% of the 
// background noise for case 1 (gain at 100%).
//
// Then they were tested together to see if they could detect
// across the room speech.
//
// Individually, none of the detection criteria could reliably
// detect all of the across the room speech. Together, they did
// not do much better. They even missed some speech while seated.
// Not very satifactory.
//
// Therefore, I decided to abandon the attempt to detect across
// the room speech. I retuned the parameters to reject noise 
// after speaking while seated (which allowed AGC to reduce
// the volume a couple of ticks, thereby increasing the signal
// to noise ratio) and to reliably detect seated speech.
//
// I also found that the "fast" envelope signal was better at
// detecting speech than the "slow" one in a straight threshold
// comparison, so it is used in the VA tests.
//

/// rodtoll
// Changing range so instead of 0 - 100 we have 0 - 128
// 
// The factor is the value we need to shift right by to get this division
// 
#define VA_MEASUREMENT_RANGE		128
#define VA_MEASUREMENT_RANGE_FACTOR	7

// VA_HIGH_PERCENT
//
// If the fast envelope signal is more than this percentage
// higher than the slow envelope signal, speech is detected.
//
// rodtoll : Modifying so that high and low values are factor of 2 values
//			 so that divisions can be removed.  Used 170 as reference point
//			 128 as the range.
//
#define VA_HIGH_PERCENT					218	

// Cannot have a factor because 218 cannot be factored by 2.
//
// #define VA_HIGH_PERCENT_FACTOR		

// Macro for doing x * VA_HIGH_PERCENT / VA_MEASUREMENT_RANGE
#define CALC_HIGH_PERCENT_RANGE(x)		((x*VA_HIGH_PERCENT)>>VA_MEASUREMENT_RANGE_FACTOR)
// #define VA_HIGH_PERCENT 170 // rejects most noise, still catches some.
							// decent voice detection. Catches the beginning
							// of speech a majority of the time, but does miss
							// once in a while. Will often drop out partway 
							// into a phrase when used alone. Must test in 
							// conjunction with VA_LOW_PERCENT.
							//
							// After testing in conjunction with VA_LOW_PERCENT,
							// the performance is reasonable. Low input volume
							// signals are usually detected ok, but dropouts are
							// a bit common. However, noise is sometimes still
							// detected, so making these parameters more sensitive
							// would not be useful.
//#define VA_HIGH_PERCENT 165 // catches occational noise
//#define VA_HIGH_PERCENT 160 // catches too much noise
//#define VA_HIGH_PERCENT 150 // catches most noise
//#define VA_HIGH_PERCENT 140 // catches almost all noise
//#define VA_HIGH_PERCENT 0x00007fff // select this to factor out this VA parameter

// VA_LOW_PERCENT
//
// If the fast envelope signal is more than this percentage
// lower than the slow envelope signal, speech is detected.
//
#define VA_LOW_PERCENT					64

// rodtoll -- Factor for shifting left instead of multiplying
#define VA_LOW_PERCENT_FACTOR			6

// Combination of shift left by low then right by VA_MEASUREMENT_RANGE_FACTOR, 
#define VA_LOW_PERCENT_RANGE_FACTOR		1

// Macro for doing x * VA_LOW_PERCENT / VA_MEASUREMENT_RANGE
#define CALC_LOW_PERCENT_RANGE(x)		(x>>VA_LOW_PERCENT_RANGE_FACTOR)

// Shift Right by 7

//#define VA_LOW_PERCENT 50 // excellent noise rejection. poor detection of speech.
						  // when used alone, could miss entire phrases. Must evaluate
						  // in conjunction with tuned VA_HIGH_PERCENT
						  //
						  // See note above re: testing in conjunction with VA_HIGH_PERCENT
//#define VA_LOW_PERCENT 55 // still catches too much noise
//#define VA_LOW_PERCENT 60 // catches most noise
//#define VA_LOW_PERCENT 65 // catches most noise
//#define VA_LOW_PERCENT 70 // still catches almost all noise
//#define VA_LOW_PERCENT 75 // catches almost all noise
//#define VA_LOW_PERCENT 80 // catches all noise
//#define VA_LOW_PERCENT 0 // select this to factor out this VA parameter

// VA_HIGH_ENVELOPE
//
// If the 16 bit normalized value of the envelope exceeds
// this number, the signal is considered voice.
//
//#define VA_HIGH_ENVELOPE (15 << 8) // still catches high gain noise, starting to get 
								   // speech dropouts, when "p" sounds lower the gain
//#define VA_HIGH_ENVELOPE (14 << 8) // Noise immunity good at "seated" S/N ratio. No speech
								   // dropouts encountered. Still catches noise at full gain.
//#define VA_HIGH_ENVELOPE (13 << 8) // Noise immunity not as good as expected (new day).
//#define VA_HIGH_ENVELOPE (12 << 8) // Good noise immunity. Speech recognition excellent.
								   // Only one dropout occured in the test with a 250ms
								   // hangover. I think the hangover time should be increased
								   // above 250 however, because a comma (properly read) tends 
								   // to cause a dropout. I'm going to tune the hangover time, 
								   // and return to this test.
								   //
								   // Hangover time is now 400ms. No dropouts occur with
								   // "seated" speech.
//#define VA_HIGH_ENVELOPE (11 << 8) // Catches almost no noise at "seated" gain
								   // however, if the gain creeped up a bit, noise would
								   // be detected. I therefore think a slightly higher 
								   // threshold would be a good idea. The speech recognition
								   // based on only this parameter at this level was flawless.
								   // No dropouts at all with a 250 ms hangover time. (commas
								   // excepted).
#define VA_HIGH_ENVELOPE (10 << 8) // catches some noise at "seated" gain - getting very close
//#define VA_HIGH_ENVELOPE (9 << 8) // catches some noise at "seated" gain - getting close
//#define VA_HIGH_ENVELOPE (8 << 8) // catches noise at "seated" gain
//#define VA_HIGH_ENVELOPE (7 << 8) // catches noise at "seated" gain
//#define VA_HIGH_ENVELOPE (0x7fffffff) // select this to factor out this VA parameter

// VA_LOW_ENVELOPE
//
// If the 16 bit normalized value of the envelope is below
// this number, the signal will never be considered voice.
// This reduces some false positives on the delta checks
// at very low signal levels
//#define VA_LOW_ENVELOPE (3 << 8)
//#define VA_LOW_ENVELOPE (2 << 8) // causes false VA at low input volumes
#define VA_LOW_ENVELOPE (1 << 8) // causes false VA at low input volumes

// VA_HANGOVER_TIME
//
// The time, in milliseconds, that voice activation sticks in
// the ON position following a voice detection. E.g. a value of 500
// means that voice will always be transmitted in at least 1/2 second
// bursts.
//
// I am trying to tune this so that a properly read comma will not cause
// a dropout. This will give the user a bit of leeway to pause in the
// speech stream without losing the floor when in Echo Suppression mode.
// It will also prevent dropouts even when not in Echo Suppression mode

#define VA_HANGOVER_TIME 500 // more forgiving

//#define VA_HANGOVER_TIME 400 // this gives satisfying performance
//#define VA_HANGOVER_TIME 375 // almost there, longest commas still goners
//#define VA_HANGOVER_TIME 350 // still drops long commas
//#define VA_HANGOVER_TIME 325 // does not drop fast commas, drops long ones
//#define VA_HANGOVER_TIME 300 // drops almost no commas, quite good
//#define VA_HANGOVER_TIME 275 // drops about half of the commas
//#define VA_HANGOVER_TIME 250 // commas are always dropped

// macros to avoid clib dependencies
#define DV_ABS(a) ((a) < 0 ? -(a) : (a))
#define DV_MAX(a, b) ((a) > (b) ? (a) : (b))
#define DV_MIN(a, b) ((a) < (b) ? (a) : (b))

// A function to lookup the log of n base 1.354 (sort of)
// where 0 <= n <= 127
//
// Why the heck do we care about log n base 1.354???
//
// What we need is a function that maps 0 to 127 down to 0 to 15
// in a nice, smooth non-linear fashion that has more fidelity at
// the low end than at the high end.
//
// The function is actually floor(log(n, 1.354), 1) to keep things
// in the integer realm.
//
// Why 1.354? Because log(128, 1.354) = 16, so we are using the full
// range from 0 to 15.
// 
// This function also cheats and just defines fn(0) = 0 and fn(1) = 1
// for convenience.
BYTE DV_LOG_1_354_lookup_table[95] = 
{
	 0,  1,  2,  3,  4,  5,  5,  6,	//   0..  7
	 6,  7,  7,  7,  8,  8,  8,  8, //   8.. 15
	 9,  9,  9,  9,  9, 10, 10, 10, //  16.. 23
	10, 10, 10, 10, 10, 11, 11, 11,	//  24.. 31
	11, 11, 11, 11, 11, 11, 12, 12, //  32.. 39
	12, 12, 12, 12, 12, 12, 12, 12, //  40.. 47
	12, 12, 12, 12, 13, 13, 13, 13, //  48.. 55
	13, 13, 13, 13, 13, 13, 13, 13, //  56.. 63
	13, 13, 13, 13, 13, 13, 14, 14, //  64.. 71
	14, 14, 14, 14, 14, 14, 14, 14, //  72.. 79
	14, 14, 14, 14, 14, 14, 14, 14, //  80.. 87
	14, 14, 14, 14, 14, 14, 14		//  88.. 94 - stop table at 94 here, everything above is 15
};

BYTE DV_log_1_354(BYTE n)
{
	if (n > 94) return 15;
	return DV_LOG_1_354_lookup_table[n];
}

// function to lookup the base 2 log of (n) where n is 16 bits unsigned
// except that we cheat and say that log_2 of zero is zero
// and we chop of any decimals.
BYTE DV_log_2(WORD n)
{
	if (n & 0x8000)
	{
		return 0x0f;
	}
	if (n & 0x4000)
	{
		return 0x0e;
	}
	if (n & 0x2000)
	{
		return 0x0d;
	}
	if (n & 0x1000)
	{
		return 0x0c;
	}
	if (n & 0x0800)
	{
		return 0x0b;
	}
	if (n & 0x0400)
	{
		return 0x0a;
	}
	if (n & 0x0200)
	{
		return 0x09;
	}
	if (n & 0x0100)
	{
		return 0x08;
	}
	if (n & 0x0080)
	{
		return 0x07;
	}
	if (n & 0x0040)
	{
		return 0x06;
	}
	if (n & 0x0020)
	{
		return 0x05;
	}
	if (n & 0x0010)
	{
		return 0x04;
	}
	if (n & 0x0008)
	{
		return 0x03;
	}
	if (n & 0x0004)
	{
		return 0x02;
	}
	if (n & 0x0002)
	{
		return 0x01;
	}
	return 0x00;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CVoiceActivation1::Init"
//
// Init - initializes the AGC and VA algorithms, including loading saved
// values from registry.
//
// dwFlags - the dwFlags from the dvClientConfig structure
// guidCaptureDevice - the capture device we're performing AGC for
// plInitVolume - the initial volume level is written here
//
HRESULT CVoiceActivation1::Init(
	DWORD dwFlags, 
	int iSampleRate,
	DWORD dwSensitivity)
{

	// Remember the flags
	m_dwFlags = dwFlags;

	// Remember the sensitivity
	m_dwSensitivity = dwSensitivity;

	// Figure out the shift constants for this sample rate
	m_iShiftConstantFast = (DV_log_2((iSampleRate * 2) / 1000) + 1);

	// This gives the slow filter a cutoff frequency 1/4 of 
	// the fast filter
	m_iShiftConstantSlow = m_iShiftConstantFast + 2;

	// Start the envelope signal at zero
	m_iCurEnvelopeValueFast = 0;
	m_iCurEnvelopeValueSlow = 0;
	m_iPrevEnvelopeSample = 0;
	m_iCurSampleNum = 0;

	// We're not clipping now
	//m_fClipping = 0;
	//m_iClippingCount = 0;

	DPVF(DPVF_INFOLEVEL, "AGCVA1:INIT:%i,%i,%i,%i", 
		iSampleRate,
		m_iShiftConstantFast,
		m_iShiftConstantSlow);
	
	return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CVoiceActivation1::Deinit"
//
// Deinit - saves the current AGC and VA state to the registry for use next session
//
HRESULT CVoiceActivation1::Deinit()
{
	HRESULT hr = DV_OK;

	return hr;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CVoiceActivation1::SetSensitivity"
HRESULT CVoiceActivation1::SetSensitivity(DWORD dwFlags, DWORD dwSensitivity)
{
	if (dwFlags & DVCLIENTCONFIG_AUTOVOICEACTIVATED)
	{
		m_dwFlags |= DVCLIENTCONFIG_AUTOVOICEACTIVATED;
	}
	else
	{
		m_dwFlags &= ~DVCLIENTCONFIG_AUTOVOICEACTIVATED;
	}
	m_dwSensitivity = dwSensitivity;
	return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CVoiceActivation1::GetSensitivity"
HRESULT CVoiceActivation1::GetSensitivity(DWORD* pdwFlags, DWORD* pdwSensitivity)
{
	*pdwSensitivity = m_dwSensitivity;
	return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CVoiceActivation1::AnalyzeData"
//
// AnaylzeData - performs the AGC & VA calculations on one frame of audio
//
// pbAudioData - pointer to a buffer containing the audio data
// dwAudioDataSize - size, in bytes, of the audio data
//
HRESULT CVoiceActivation1::AnalyzeData(BYTE* pbAudioData, DWORD dwAudioDataSize /*, DWORD dwFrameTime*/)
{
	int iMaxValue;
	//int iValue;
	int iValueAbs;
	//int iZeroCrossings;
	int iIndex;
	int iMaxPossiblePeak;
	int iNumberOfSamples;
	//BYTE bPeak255;

	//m_dwFrameTime = dwFrameTime;

	if (dwAudioDataSize < 1)
	{
		DPVF(DPVF_ERRORLEVEL, "Error: Audio Data Size < 1");
		return DVERR_INVALIDPARAM;
	}

	// new algorithm...

	// cast the audio data to signed 16 bit integers
	signed short* psiAudioData = (signed short *)pbAudioData;

    // 16 bits per sample assumed!!
	iNumberOfSamples = dwAudioDataSize / 2;

    //
    // BUGBUG the hawk certainly regulates the volume so the max value is never above 600
    // we got to figure this out tho
    //

	iMaxPossiblePeak = 500;
	

	m_fVoiceDetectedThisFrame = FALSE;
	iMaxValue = 0;
	for (iIndex = 0; iIndex < (int)iNumberOfSamples; ++iIndex)
	{
		++m_iCurSampleNum;

        iValueAbs = DV_ABS((int)psiAudioData[iIndex]);

		// see if it is the new peak value
		iMaxValue = DV_MAX(iValueAbs, iMaxValue);

		// do the low pass filtering, but only if we are in autosensitivity mode
		int iNormalizedCurEnvelopeValueFast;
		int iNormalizedCurEnvelopeValueSlow;
		if (m_dwFlags & DVCLIENTCONFIG_AUTOVOICEACTIVATED)
		{
			m_iCurEnvelopeValueFast = 
				iValueAbs + 
				(m_iCurEnvelopeValueFast - (m_iCurEnvelopeValueFast >> m_iShiftConstantFast));
			iNormalizedCurEnvelopeValueFast = m_iCurEnvelopeValueFast >> m_iShiftConstantFast;

			m_iCurEnvelopeValueSlow = 
				iValueAbs + 
				(m_iCurEnvelopeValueSlow - (m_iCurEnvelopeValueSlow >> m_iShiftConstantSlow));
			iNormalizedCurEnvelopeValueSlow = m_iCurEnvelopeValueSlow >> m_iShiftConstantSlow;

			// check to see if we consider this voice
			if (iNormalizedCurEnvelopeValueFast > VA_LOW_ENVELOPE &&
				(iNormalizedCurEnvelopeValueFast > VA_HIGH_ENVELOPE ||
				CALC_HIGH_PERCENT_RANGE( iNormalizedCurEnvelopeValueSlow ) ||
				CALC_LOW_PERCENT_RANGE( iNormalizedCurEnvelopeValueSlow ) ) )
			{
				m_fVoiceDetectedNow = TRUE;
				m_fVoiceDetectedThisFrame = TRUE;
				m_fVoiceHangoverActive = TRUE;
				m_iCurHangoverSamples = 0;
			}
			else
			{
				m_fVoiceDetectedNow = FALSE;
				++m_iCurHangoverSamples;
				if (m_iCurHangoverSamples > m_iHangoverSamples)
				{
					m_fVoiceHangoverActive = FALSE;
				}
				else
				{
					m_fVoiceHangoverActive = TRUE;
					m_fVoiceDetectedThisFrame = TRUE;

                    DPVF(DPVF_INFOLEVEL, "VA1:ANA, VOiceDetected %i,%i", 
                         m_bPeak,
                         iMaxValue);

				}
			}
		}
	}

	// Normalize the peak value to the range DVINPUTLEVEL_MIN to DVINPUTLEVEL_MAX
	// This is what is returned for caller's peak meters...
	m_bPeak = (BYTE)(DVINPUTLEVEL_MIN + 
		((iMaxValue * (DVINPUTLEVEL_MAX - DVINPUTLEVEL_MIN)) / iMaxPossiblePeak));

	// if we are in manual VA mode (not autovolume) check the peak against
	// the sensitivity threshold
	if (!(m_dwFlags & DVCLIENTCONFIG_AUTOVOICEACTIVATED))
	{
		if (m_bPeak > m_dwSensitivity)
		{
			m_fVoiceDetectedThisFrame = TRUE;
            DPVF(DPVF_INFOLEVEL, "VA1:ANA, VOiceDetetced %i,%i", 
                 m_bPeak,
                 iMaxValue);

		}
	}

    //
    // BUGBUG add a ahck that will not allow buffers with saturated volumes
    // to be send as voice. Until we figure why hawk does this..
    //

    if (m_bPeak == 240) {

        m_fVoiceDetectedThisFrame = FALSE;
        m_fVoiceDetectedNow = FALSE;
        m_fVoiceHangoverActive = FALSE;

    }

	DPVF(DPVF_SPAMLEVEL, "VA1:ANA,%i,%i,%i", 
		m_bPeak,
		iMaxValue,
		m_fVoiceDetectedThisFrame);
	
	return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CVoiceActivation1::VAResults"
//
// VAResults - returns the VA results from the previous AnalyzeFrame call
//
// pfVoiceDetected - stuffed with TRUE if voice was detected in the data, FALSE otherwise
//
HRESULT CVoiceActivation1::VAResults(BOOL* pfVoiceDetected)
{
	if (pfVoiceDetected != NULL)
	{
		*pfVoiceDetected = m_fVoiceDetectedThisFrame;
	}
	return DV_OK;
}

#undef DPF_MODNAME
#define DPF_MODNAME "CVoiceActivation1::PeakResults"
//
// PeakResults - returns the peak sample value from the previous AnalyzeFrame call,
// 				 normalized to the range 0 to 99
//
// pfPeakValue - pointer to a byte where the peak value is written
//
HRESULT CVoiceActivation1::PeakResults(BYTE* pbPeakValue)
{
	DPVF(DPVF_SPAMLEVEL, "VA1: peak value: %i" , m_bPeak);
	*pbPeakValue = m_bPeak;
	return DV_OK;
}

