#if DBG

#define MY_ASSERT(x) if( !(x) ) _asm int 3;

extern "C"
{
	ULONG
	DebugPrint(
		PCHAR Format,
		...
		);
}

#define DEBUG_LEVEL_ERROR 0
#define DEBUG_LEVEL_WARNING 1
#define DEBUG_LEVEL_INFO 2
#define DEBUG_LEVEL_SPAM 5

extern ULONG g_VaXmoDebugLevel;
#define DBG_SPAM(_exp_) {if (g_VeXmoDebugLevel >= DEBUG_LEVEL_SPAM) DebugPrint _exp_;}
#define DBG_INFO(_exp) {if (g_VeXmoDebugLevel >= DEBUG_LEVEL_INFO) DebugPrint _exp;}
#define DBG_ERROR(_exp) {if (g_VeXmoDebugLevel >= DEBUG_LEVEL_ERROR) DebugPrint _exp;}
#define DBG_WARN(_exp) {if (g_VeXmoDebugLevel >= DEBUG_LEVEL_WARNING) DebugPrint _exp;}

#else

#define MY_ASSERT(x)
#define DBG_SPAM(_exp_)
#define DBG_INFO(_exp_)
#define DBG_ERROR(_exp_)
#define DBG_WARN(_exp_)

#endif





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

#define VE_INPUTLEVEL_MIN                    0x00000000
#define VE_INPUTLEVEL_MAX                    0x00000063  // 99 decimal

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

// maximum msg time
#define MAX_MSG_TIME    3 * 60 * 1000 //3 minutes = 180000 ms

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

class CVoiceEncoderXmo: public XMediaObject
{
protected:
	DWORD m_fAutoMode;
	DWORD m_dwSensitivity;
	int m_dwSampleRate;

	int m_iEnvelopeSampleRate;
	int m_iCurEnvelopeValueFast;
	int m_iCurEnvelopeValueSlow;
	int m_iHangoverSamples;
	int m_iCurHangoverSamples;
	int m_iShiftConstantFast;
	int m_iShiftConstantSlow;

	BYTE m_bPeak;
    BYTE m_bMsgNum;
    WORD m_wSeqNum;
	WORD m_wCurrentMsgSeqCount;
	WORD m_wMaxSeqInMsg;
    DWORD m_dwMinOutputSize;

    LPXMEDIAOBJECT m_pEncoderXmo;

	BOOL m_fVoiceDetectedThisFrame;
    DWORD m_cRef;

public:

	CVoiceEncoderXmo() 
		: m_bPeak(0)
		, m_fVoiceDetectedThisFrame(FALSE)
        , m_dwSampleRate(0)
        , m_iCurEnvelopeValueFast(0)
        , m_iCurEnvelopeValueSlow(0)
        , m_iCurHangoverSamples(0)
        , m_bMsgNum(0)
        , m_wSeqNum(0)
		, m_wCurrentMsgSeqCount(0)
		, m_wMaxSeqInMsg(0)
        , m_pEncoderXmo(NULL)
        , m_fAutoMode(FALSE)
        , m_dwSensitivity(0)
        , m_iHangoverSamples(0) // number of samples we still consider trailing voice
        , m_dwMinOutputSize(sizeof(VOICE_ENCODER_HEADER))
        , m_cRef(0)
		{};

	~CVoiceEncoderXmo();
	
    STDMETHODIMP_(ULONG) AddRef() {
       return InterlockedIncrement((long*)&m_cRef);
    }

    STDMETHODIMP_(ULONG) Release() {
       long l = InterlockedDecrement((long*)&m_cRef);
       if (l == 0)
          delete this;
       return l;
    }

    HRESULT STDMETHODCALLTYPE Process( 
        LPCXMEDIAPACKET pSrcBuffer,
        LPCXMEDIAPACKET pDstBuffer
        );
    
	HRESULT STDMETHODCALLTYPE GetInfo(
        LPXMEDIAINFO pInfo
		);

    HRESULT STDMETHODCALLTYPE Flush();
    HRESULT STDMETHODCALLTYPE Discontinuity(void);

    HRESULT STDMETHODCALLTYPE GetStatus( 
        LPDWORD pdwStatus 
		);


	HRESULT Init(
		DWORD fAutoMode, 
        DWORD dwCodecTag,
        PWAVEFORMATEX pwfx,
		DWORD dwSensitivity);

private:

};

