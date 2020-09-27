/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       I3DL2.cpp
 *  Content:    I3DL2 reverb and filter calculations.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  05/30/01    dereks  Created based on Sensaura tables.
 *
 ****************************************************************************/

#include "dsoundi.h"

#define ALIGN_COUNT(a, b) \
    (((a) / (b)) * (b))

#define DWORD_ALIGN(a) \
    (((a) + 3) & ~3)

typedef struct							// Coefficients for a 1 pole IIR filter
{
	int	iA;
	int	iB;
} RVB_IIR_1P;

#define MB_SCALE		2000.0f
#define DS_LEVEL_SILENT	(-10000)		// Attenuation which produces silence
#define DS_GAIN_SILENT	0.00001f		// Linear gain below which is silent

static __inline float ds_alog(const float x)
{
	float y;
	float scale = 3.321928f;			// 1/log10(2)
	float one = 1;
	float dummy;
	float n;

	__asm {
	fld scale;
	fmul x;								// x/log10(2)
	frndint;							// assumes standard rounding mode - should we set it?
	fst n;								// n=int part of x/log10(2)
	fwait;								// wait for n
	fld1;								// 1.0
	fscale;								// generate 2^n
	fxch;								// move n to top of stack
	fstp dummy;							// get rid of it
										// stack now has 2^n
	fld scale;
	fmul x;								// x/log10(2) again
	fsub n;								// remove int part, so in range -1...+1 required by f2xm1
	f2xm1;								// 2^(x/log10(2)-n)-1
	fadd one;							// 2^(x/log10(2)-n) = 2^(x/log10(2)) * 2^(-n)
#ifdef	_MSC_VER
	fmulp st(1),st;						// 2^(x/log10(2))
#else
	fmulp;								// 2^(x/log10(2))
#endif
	fstp y;								// log10(y) = (x/log10(2)) * log10(2) = x
										// y = 10^x QED!
	fwait;								// synchronise
	}
	return y;
}

static inline float mBToLinear(long x) 
	{ return /*x <= DS_LEVEL_SILENT ? 0.f :*/ds_alog((float)x/MB_SCALE); }

static inline long LinearTomB(float x) 
	{ return x <= DS_GAIN_SILENT ? DS_LEVEL_SILENT : (long)FTOI((float)log10(x)*MB_SCALE); }


/****************************************************************************
 *
 *  CI3dl2Listener
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Listener::CI3dl2Listener"

const float CI3dl2Listener::m_flScale23 = 8388608.f;   // 2^23
const float CI3dl2Listener::m_flScale16 = 65536.f;     // 2^16

const float CI3dl2Listener::m_aflReflectionData[RVB_HW_REFLECTIONS][5] =
{
	{ 0.57f, -0.40f, 0.1052f, 0.003651f, 0.2469f },
	{ 0.52f,  0.40f, 0.0673f, 0.003515f, 0.2209f },
	{ 0.54f, -0.40f, 0.1737f, 0.003673f, 0.2311f },
	{ 0.60f, -0.40f, 0.2404f, 0.003946f, 0.2608f }		
};

const float CI3dl2Listener::m_aflShortReverbInputFactor[RVB_HW_SHORTREVERBS][2] =
{
	{ 1.0f, -1.0f },
	{ -1.0f, 1.0f },
	{ 1.0f,  1.0f },
	{ -1.0f,  -1.0f },
};

const float CI3dl2Listener::m_aflShortReverbFeedbackDelay[RVB_HW_SHORTREVERBS] =
{
	0.058005f, 0.070295f, 0.085238f, 0.103288f
};

const float CI3dl2Listener::m_aflLongReverbInputDelay[RVB_HW_LONGREVERBS][4] = 
{
	{ 0.040023f, 0.288186f, 0.603764f, 0.851905f },
	{ 0.021224f, 0.375420f, 0.623583f, 0.759229f },
};

const DWORD CI3dl2Listener::m_dwSamplesPerSec = 24000;

CI3dl2Listener::CI3dl2Listener
(
    REFDSI3DL2LISTENER      ds3dl
)
:   m_I3dl2Params(ds3dl)
{
    DPF_ENTER();
    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  SetSize
 *
 *  Description:
 *      Insert description text here.
 *
 *  Arguments:
 *      TYPE [in/out]: insert argument description here.
 *
 *  Returns:  
 *      RETURNTYPE: insert return description here.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Listener::SetSize"

void
CI3dl2Listener::SetSize
(
    FLOAT                   fSize
)
{
	float	fSizeFactor;
	int		i;
	int		j;
	float	f;
	DWORD	dw;

	if(fSize < RVB_MAXSMALLSIZE)		// Small room?
		fSizeFactor = (fSize-RVB_BASESIZE)/(RVB_MAXSMALLSIZE-RVB_BASESIZE);
	else
		fSizeFactor = 1.0f;

	for(i = 0; i < 4; i++)
		for(j = 0; j < 2; j++)
		{
			f = m_aflLongReverbInputDelay[j][i]*fSizeFactor*m_dwSamplesPerSec;
			dw = ftoi(f);
			ALIGN_COUNT(dw, 4);
				
			DPF_BLAB("Reverb input delay (D4%lx):%d", 2*i+j, dw);
			m_I3dl2Data.dwLongReverbInputDelay[2*i+j]=dw;
		}

	m_I3dl2Data.dwLongReverbFeedbackDelay = ftoi(fSizeFactor*m_dwSamplesPerSec);
	ALIGN_COUNT(m_I3dl2Data.dwLongReverbFeedbackDelay, 4);

	DPF_BLAB("Long reverb feedback delays (D50-D51):%d", m_I3dl2Data.dwLongReverbFeedbackDelay);
}


/****************************************************************************
 *
 *  SetInputFilter
 *
 *  Description:
 *      Insert description text here.
 *
 *  Arguments:
 *      TYPE [in/out]: insert argument description here.
 *
 *  Returns:  
 *      RETURNTYPE: insert return description here.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Listener::SetInputFilter"

void 
CI3dl2Listener::SetInputFilter
(
    LONG                    lGainHF, 
    FLOAT                   fHFReference
)
{
	float	fA, fB;

    //
    // always pass 48k to this function even if dsp reverb is running at 24k
    //

	Get1PoleLoPass(0, lGainHF, fHFReference, (float)max(m_dwSamplesPerSec, MCPX_BASEFREQ), &fA, &fB);
	
	m_I3dl2Data.IIR[InputIIR].dwCoefficients[0]=ftoi(fA*m_flScale23);
	m_I3dl2Data.IIR[InputIIR].dwCoefficients[1]=ftoi(fB*m_flScale23);

	DPF_BLAB("Input IIR(F00):dwCoefficients[0]:%d  dwCoefficients[1]:%d", m_I3dl2Data.IIR[InputIIR].dwCoefficients[0], m_I3dl2Data.IIR[InputIIR].dwCoefficients[1]);
}


/****************************************************************************
 *
 *  SetReflectionsGain
 *
 *  Description:
 *      Insert description text here.
 *
 *  Arguments:
 *      TYPE [in/out]: insert argument description here.
 *
 *  Returns:  
 *      RETURNTYPE: insert return description here.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Listener::SetReflectionsGain"

void 
CI3dl2Listener::SetReflectionsGain
(
    FLOAT                   fGain
)
{
	for(int i = 0; i < RVB_HW_REFLECTIONS; i++)		// G60 to G63
	{
		m_I3dl2Data.dwReflectionsOutputGain[i] = ftoi(m_aflReflectionData[i][0]*fGain*m_flScale16);
		DPF_BLAB("Reflection[%d] output gain(G6%lx):%d", i, i, m_I3dl2Data.dwReflectionsOutputGain[i]);
	}
}


/****************************************************************************
 *
 *  SetReflectionsDelay
 *
 *  Description:
 *      Insert description text here.
 *
 *  Arguments:
 *      TYPE [in/out]: insert argument description here.
 *
 *  Returns:  
 *      RETURNTYPE: insert return description here.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Listener::SetReflectionsDelay"

void 
CI3dl2Listener::SetReflectionsDelay
(
    FLOAT                   fDelay
)
{
	m_I3dl2Data.dwReflectionsInputDelay[0] = ftoi(fDelay*m_dwSamplesPerSec);
	ALIGN_COUNT(m_I3dl2Data.dwReflectionsInputDelay[0], 4);

	DPF_BLAB("Reflections delay(D00):%d", m_I3dl2Data.dwReflectionsInputDelay[0]);
}


/****************************************************************************
 *
 *  SetReverbGain
 *
 *  Description:
 *      Insert description text here.
 *
 *  Arguments:
 *      TYPE [in/out]: insert argument description here.
 *
 *  Returns:  
 *      RETURNTYPE: insert return description here.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Listener::SetReverbGain"

void 
CI3dl2Listener::SetReverbGain
(
    FLOAT                   fGain
)
{
	DPF_BLAB("Reverb output gain(G70-75):%d", ftoi(fGain*m_flScale16));
	m_I3dl2Data.dwShortReverbOutputGain=ftoi(fGain*m_flScale16);
	m_I3dl2Data.dwLongReverbOutputGain=ftoi(fGain*m_flScale16);
}


/****************************************************************************
 *
 *  SetReverbDelay
 *
 *  Description:
 *      Insert description text here.
 *
 *  Arguments:
 *      TYPE [in/out]: insert argument description here.
 *
 *  Returns:  
 *      RETURNTYPE: insert return description here.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Listener::SetReverbDelay"

void 
CI3dl2Listener::SetReverbDelay
(
    FLOAT                   fDelay
)
{
	float f;

	m_I3dl2Data.dwShortReverbInputDelay = ftoi(fDelay*m_dwSamplesPerSec);
	ALIGN_COUNT(m_I3dl2Data.dwShortReverbInputDelay, 4);

	for(int i = 0; i < 4; i++)				// D01 to D04
	{
		f = m_aflReflectionData[i][2]*(float)m_I3dl2Data.dwShortReverbInputDelay ;
		m_I3dl2Data.dwReflectionsInputDelay[1+i] = ftoi(f);
		DPF_BLAB("Reflection[%d] input delay(D0%lx):%d", i, i+1, m_I3dl2Data.dwReflectionsInputDelay[1+i]);
		
	}
	for(i = 0; i < 4; i++)					// D05 to D08
	{
		f = (m_aflReflectionData[i][3]+m_aflReflectionData[i][4]*fDelay)*m_dwSamplesPerSec;
		m_I3dl2Data.dwReflectionsFeedbackDelay[i] = ftoi(f);
		DPF_BLAB("Reflection[%d] feedback delay(D0%lx):%d", i, i+5, m_I3dl2Data.dwReflectionsFeedbackDelay[i]);
	}

	DPF_BLAB("Reverb delay:%d", m_I3dl2Data.dwShortReverbInputDelay);
}


/****************************************************************************
 *
 *  SetDecayTimes
 *
 *  Description:
 *      Insert description text here.
 *
 *  Arguments:
 *      TYPE [in/out]: insert argument description here.
 *
 *  Returns:  
 *      RETURNTYPE: insert return description here.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Listener::SetDecayTimes"

void 
CI3dl2Listener::SetDecayTimes
(
    FLOAT                   fDecayTime, 
    FLOAT                   fDecayHFRatio, 
    FLOAT                   fHFReference
)
{
	int		i;

	DSFX_I3DL2REVERB_IIR ReverbIir; 
	DSFX_I3DL2REVERB_IIR LongReverbFeedbackIir;


	SetDecayFilter(&ReverbIir, m_I3dl2Data.dwLongReverbFeedbackDelay/4, fDecayTime, fDecayHFRatio, fHFReference);
	DPF_BLAB("Reverb IIR(F40-F42):dwCoefficients[0]:%d  dwCoefficients[1]:%d", ReverbIir.dwCoefficients[0], ReverbIir.dwCoefficients[1]);
	for(i=0;i<3;i++)
	{
		m_I3dl2Data.IIR[MainDelayLineLongReverbIIR+i].dwCoefficients[0]=ReverbIir.dwCoefficients[0];
		m_I3dl2Data.IIR[MainDelayLineLongReverbIIR+i].dwCoefficients[1]=ReverbIir.dwCoefficients[1];
	}
	SetDecayFilter(&LongReverbFeedbackIir, m_I3dl2Data.dwLongReverbFeedbackDelay,
					   fDecayTime, fDecayHFRatio, fHFReference);
	DPF_BLAB("Long reverb feedback IIR(F50-F51):dwCoefficients[0]:%d  dwCoefficients[1]:%d", 
		   LongReverbFeedbackIir.dwCoefficients[0], LongReverbFeedbackIir.dwCoefficients[1]);
	for(i=0;i<2;i++)
	{
		m_I3dl2Data.IIR[LongReverbIIR+i].dwCoefficients[0]=LongReverbFeedbackIir.dwCoefficients[0];
		m_I3dl2Data.IIR[LongReverbIIR+i].dwCoefficients[1]=LongReverbFeedbackIir.dwCoefficients[1];
	}

	for(i =0; i < RVB_HW_SHORTREVERBS; i++)
	{
		SetDecayFilter(&m_I3dl2Data.IIR[ShortReverbIIR+i], m_I3dl2Data.DelayLines[i+ShortReverbDelayLineID].dwLength, fDecayTime, fDecayHFRatio, fHFReference);
		DPF_BLAB("Short reverb[%d] feedback IIR(F3%lx):dwCoefficients[0]:%d  dwCoefficients[1]:%d", i, i,
			   m_I3dl2Data.IIR[ShortReverbIIR+i].dwCoefficients[0], m_I3dl2Data.IIR[ShortReverbIIR+i].dwCoefficients[1]);
		
	}
}


/****************************************************************************
 *
 *  SetDiffusion
 *
 *  Description:
 *      Insert description text here.
 *
 *  Arguments:
 *      TYPE [in/out]: insert argument description here.
 *
 *  Returns:  
 *      RETURNTYPE: insert return description here.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Listener::SetDiffusion"

void 
CI3dl2Listener::SetDiffusion
(
    FLOAT                   fDiffusion
)
{
	for(int i = 0; i < RVB_HW_SHORTREVERBS; i++)
	{
		for(int j = 0; j < 2; j++)
		{
			m_I3dl2Data.dwShortReverbInputGain[i*2+j] = ftoi(m_aflShortReverbInputFactor[i][j]*fDiffusion*m_flScale23);

			DPF_BLAB("Short reverb[%d] input gain[%d](G1%lx):%d",
			i, j, i*2+j, m_I3dl2Data.dwShortReverbInputGain[i*2+j]);

		}
	}


	m_I3dl2Data.dwLongReverbCrossfeedGain=ftoi(m_flScale23*fDiffusion/(RVB_HW_LONGREVERBS+RVB_HW_SHORTREVERBS-1));
	m_I3dl2Data.dwLongReverbInputGain=ftoi(m_flScale23*(1.0f-fDiffusion));

	DPF_BLAB("Long reverb crossfeed gains(G50-51):%d", m_I3dl2Data.dwLongReverbCrossfeedGain);
	DPF_BLAB("Long reverb input gains(G52-53):%d", m_I3dl2Data.dwLongReverbInputGain);
}


/****************************************************************************
 *
 *  SetDecayFilter
 *
 *  Description:
 *      Insert description text here.
 *
 *  Arguments:
 *      TYPE [in/out]: insert argument description here.
 *
 *  Returns:  
 *      RETURNTYPE: insert return description here.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Listener::SetDecayFilter"

void
CI3dl2Listener::SetDecayFilter
(
    DSFX_I3DL2REVERB_IIR *  pIir,
    DWORD                   dwDelay,
    FLOAT                   fDecayTime,
    FLOAT                   fDecayHFRatio,
    FLOAT                   fHFReference
)
{
	float	f;
	long	lGain;
	long	lGainHF;
	float	fA, fB;

	ASSERT(pIir != NULL);

	f = (float)dwDelay/(float)m_dwSamplesPerSec;
	f = -6000.0f*(f/fDecayTime);
	lGain = (long)ftoi(f);
	if(fDecayHFRatio < 1.0f)
		f /= fDecayHFRatio;
	lGainHF = (long)ftoi(f);
	lGainHF -= lGain;
	Get1PoleLoPass(lGain, lGainHF, fHFReference, (float)m_dwSamplesPerSec, &fA, &fB);

	pIir->dwCoefficients[0] = ftoi(fA*m_flScale23);
	pIir->dwCoefficients[1] = ftoi(fB*m_flScale23);
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Sets up constant reverb parameters.
 *
 *  Arguments:
 *      LPDSFX_I3dl2REVERB_PARAMS [out]: reverb parameter block.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Listener::Initialize"

void
CI3dl2Listener::Initialize
(
    void
)
{
	int		i;

#ifdef I3DL2_DSP_INIT

    //
    // The DSP program has already initialized part of the structure.  We need
    // to fill in the rest.
    //
    // The DSP specifies the delay line lengths in bytes, but the C code 
    // assumes they're in samples.
    //

    for(i = 0; i < NUMELMS(m_I3dl2Data.DelayLines); i++)
    {
        m_I3dl2Data.DelayLines[i].dwLength--;
        m_I3dl2Data.DelayLines[i].dwLength /= sizeof(DWORD);
        m_I3dl2Data.DelayLines[i].dwLength++;
    }

#else // I3DL2_DSP_INIT

	float	f;
	DWORD	dw;

	for(i = 0; i < 4; i++)
	{
		f = (m_aflReflectionData[i][3]+m_aflReflectionData[i][4]*0.1f)*m_dwSamplesPerSec;
		dw = ftoi(f);
		ALIGN_COUNT(dw , 4)
		m_I3dl2Data.DelayLines[i+ReflectionDelayLineID].dwLength=dw;
	}
	
	for(i = 0; i < RVB_HW_SHORTREVERBS; i++)		
	{									// Calculate reverb delay line parameters
		f = m_aflShortReverbFeedbackDelay[i]*m_dwSamplesPerSec;
		m_I3dl2Data.DelayLines[i+ShortReverbDelayLineID].dwLength = ftoi(f);
		ALIGN_COUNT(m_I3dl2Data.DelayLines[i+ShortReverbDelayLineID].dwLength, 4)
	}
										// Allocate memory for reverb source delay buffer
	for(i=0;i<4;i++)
	{
		m_I3dl2Data.DelayLines[MainDelayLineID+i].dwLength=m_dwSamplesPerSec/4;
	}
	for(i=0;i<2;i++)
	{
		m_I3dl2Data.DelayLines[LongReverbDelayLineID+i].dwLength=m_dwSamplesPerSec;
	}

#endif // I3DL2_DSP_INIT

    //
    // Set the channel count
    //

    m_I3dl2Data.dwChannelCount = 4;

    //
    // Initialize the IIR delay values
    //

    for(i = 0; i < NUMELMS(m_I3dl2Data.IIR); i++)
    {
        m_I3dl2Data.IIR[i].dwDelay = 0;
    }
}


/****************************************************************************
 *
 *  CalculateI3dl2
 *
 *  Description:
 *      Sets all I3DL2 listener properties.
 *
 *  Arguments:
 *      LPDSFX_I3dl2REVERB_PARAMS [out]: reverb parameter block.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Listener::CalculateI3dl2"

void
CI3dl2Listener::CalculateI3dl2
(
    void
)
{
	float fDiffusion;
	float fDensity;
	float fSize;

	fDiffusion = m_I3dl2Params.flDiffusion/100.0f;
	fDensity = m_I3dl2Params.flDensity/100.0f;

	fSize = (m_I3dl2Params.flDecayTime-1.0f)*5.0f;
	fSize *= 2.0f-fDensity;

	if (fSize <1.0f)
		fSize = 1.0f;
	if (fSize > 60.0f)
		fSize = 60.0f;

	int i,j;

	Initialize();
    SetInputFilter(m_I3dl2Params.lRoomHF, m_I3dl2Params.flHFReference);
	SetReflectionsGain(mBToLinear(m_I3dl2Params.lRoom + m_I3dl2Params.lReflections));
	SetReflectionsDelay(m_I3dl2Params.flReflectionsDelay);
	SetReverbGain(mBToLinear(m_I3dl2Params.lRoom+ m_I3dl2Params.lReverb));
	SetReverbDelay(m_I3dl2Params.flReverbDelay);
	SetSize(fSize);
	SetDecayTimes(m_I3dl2Params.flDecayTime, m_I3dl2Params.flDecayHFRatio, m_I3dl2Params.flHFReference);
	SetDiffusion(fDiffusion);




	// Ensure that the lengths of the delay buffers are delays are multiples of 4
	for(i=0;i<NUMELMS(m_I3dl2Data.DelayLines);i++)
	{
		m_I3dl2Data.DelayLines[i].dwLength=(m_I3dl2Data.DelayLines[i].dwLength+1) & (~3);
	}
	
	// Ensure that the positions of the reflection input delay taps are delays are multiples of 4
	for(i=0;i<5;i++)
	{
		m_I3dl2Data.dwReflectionsInputDelay[i]=(m_I3dl2Data.dwReflectionsInputDelay[i]+1) & ~3;
	}
	// Ensure that the positions of the Long Reverb Input Delay taps are delays are multiples of 4
	for(i=0;i<8;i++)
	{
		m_I3dl2Data.dwLongReverbInputDelay[i]=(m_I3dl2Data.dwLongReverbInputDelay[i]+1) & ~3;
	}
	m_I3dl2Data.dwShortReverbInputDelay=(m_I3dl2Data.dwShortReverbInputDelay+1) & ~3;

	// Ensure that the positions of the reflection taps are delays are multiples of 4
	for(i=0;i<4;i++)
	{
		m_I3dl2Data.dwReflectionsFeedbackDelay[i]=(m_I3dl2Data.dwReflectionsFeedbackDelay[i]+1) & ~3;
	}
	m_I3dl2Data.dwLongReverbFeedbackDelay=(m_I3dl2Data.dwLongReverbFeedbackDelay+1) & ~3;


	// There isn't enough memory on the EVM board to implement all the
	// delay buffers. Therefore calculate the length of the long reverb
	// buffers based on the length of the other buffers. (This
	// actually makes the long reverb buffers about 0.6 seconds rather than
	// 1.0 seconds.
	/*DWORD SumMemory=0;								// Add up the lengths of all
	for(i=0;i<NUMELMS(m_I3dl2Data.DelayLines)-2;i++)			// the buffers excluding the
	{												// two long reverb buffers
		SumMemory+=m_I3dl2Data.DelayLines[i].dwLength;
	}
													// Calculate the length of
													// the long reverb buffers
	DWORD MaxLongReverbBufferLength=(128*1024-SumMemory)/2;
													// Clip the length of the
													// reverb buffer
	for(i=0;i<2;i++)
	{
		if(m_I3dl2Data.DelayLines[LongReverbDelayLineID+i].dwLength>MaxLongReverbBufferLength)
		{
			m_I3dl2Data.DelayLines[LongReverbDelayLineID+i].dwLength=MaxLongReverbBufferLength;
		}
	}*/



	
	
	
	
	///////////////////////////////////////////////////////////
	// Write a block representing the host memory delay lines
	///////////////////////////////////////////////////////////
	//int Memory=DSP_StartOfMemory;
	for(i=0;i<NUMELMS(m_I3dl2Data.DelayLines);i++)
	{
		//fprintf( fp , "%d %d",Memory,DelayLines[i].dwLength-1);
		m_I3dl2Data.DelayLines[i].dwLength--;
        //Memory+=m_I3dl2Data.DelayLines[i].dwLength;
	}
	//fprintf( fp , "" );


	///////////////////////////////////////////////////////////
	// Reflections input delay
	///////////////////////////////////////////////////////////
	//fprintf( fp , "%d" , dwReflectionsInputDelay[0]);
	for(i=1;i<5;i++)
	{
		//fprintf( fp , " %d" , dwReflectionsInputDelay[0]+dwReflectionsInputDelay[i]);
        m_I3dl2Data.dwReflectionsInputDelay[i] += m_I3dl2Data.dwReflectionsInputDelay[0];
	}
	//fprintf( fp , "" );

	
	///////////////////////////////////////////////////////////
	// Short reverb input delay
	///////////////////////////////////////////////////////////
	//		Note that the short reverb is additative reflections input delay
	//fprintf( fp , "%d" , dwShortReverbInputDelay+dwReflectionsInputDelay[0]);	
    m_I3dl2Data.dwShortReverbInputDelay += m_I3dl2Data.dwReflectionsInputDelay[0];

	
	///////////////////////////////////////////////////////////
	// Long reverb input delays
	///////////////////////////////////////////////////////////
	// On entry the dwLongReverbInputDelay[] values are measured with
	// respect to the start of the main delay buffer.
	// The long reverb input delays should be relative to the start of 
	// their respective delay buffers.  
	DWORD EndOfLastReverbBuffer=0;
	DWORD EndOfThisReverbBuffer=0;
	for(i=0;i<4;i++)
	{
		EndOfThisReverbBuffer=m_I3dl2Data.dwLongReverbInputDelay[2*i+1];

		int LongestDelayOnThisBuffer=max(m_I3dl2Data.dwLongReverbInputDelay[2*i],m_I3dl2Data.dwLongReverbInputDelay[2*i+1]);

		if(LongestDelayOnThisBuffer-EndOfLastReverbBuffer>m_I3dl2Data.DelayLines[MainDelayLineID+i].dwLength)
		{
			// The second reverb needs to be clipped
			// Scale the reverb delay times so that the long 
			// reverb fits within the buffer
			float fTimeScale=(float)m_I3dl2Data.DelayLines[MainDelayLineID+i].dwLength/(LongestDelayOnThisBuffer-EndOfLastReverbBuffer);
			
			DPF_BLAB("Before clipping  %i - %i %i",i*2,m_I3dl2Data.dwLongReverbInputDelay[2*i],m_I3dl2Data.dwLongReverbInputDelay[2*i+1]);
			m_I3dl2Data.dwLongReverbInputDelay[2*i]=ftoi((m_I3dl2Data.dwLongReverbInputDelay[2*i]-EndOfLastReverbBuffer)*fTimeScale);
			m_I3dl2Data.dwLongReverbInputDelay[2*i+1]=ftoi((m_I3dl2Data.dwLongReverbInputDelay[2*i+1]-EndOfLastReverbBuffer)*fTimeScale);

			// Qudword align the delays
			for(j=0;j<2;j++)
			{
				m_I3dl2Data.dwLongReverbInputDelay[2*i+j]=(m_I3dl2Data.dwLongReverbInputDelay[2*i+j]+1) & ~3;
			}

			DPF_BLAB("Clipped long reverb input delay %i - %i %i",i*2,m_I3dl2Data.dwLongReverbInputDelay[2*i],m_I3dl2Data.dwLongReverbInputDelay[2*i+1]);
		}
		else
		{
			m_I3dl2Data.dwLongReverbInputDelay[2*i]-=EndOfLastReverbBuffer;
			m_I3dl2Data.dwLongReverbInputDelay[2*i+1]-=EndOfLastReverbBuffer;
		}

		EndOfLastReverbBuffer=EndOfThisReverbBuffer;
	}

	for(i=0;i<8;i++)
	{
		//fprintf( fp , "%d" , dwLongReverbInputDelay[i] );
		//if(i!=7) //fprintf( fp , " " );
	}
	//fprintf( fp , "" );


	///////////////////////////////////////////////////////////
	// Reflections feedback delays
	///////////////////////////////////////////////////////////
	for(i=0;i<4;i++)
	{
														// Limit the positions of the
														// reflections delay taps
		if(m_I3dl2Data.dwReflectionsFeedbackDelay[i]>m_I3dl2Data.DelayLines[ReflectionDelayLineID+i].dwLength)
		{
			m_I3dl2Data.dwReflectionsFeedbackDelay[i]=m_I3dl2Data.DelayLines[ReflectionDelayLineID+i].dwLength;
			DPF_BLAB("Clipped reflections feedback delay %i",i);
		}

		//fprintf( fp , "%d" , dwReflectionsFeedbackDelay[i]);
		//if(i!=3) //fprintf( fp , " " );
	}
	//fprintf( fp , "" );



	///////////////////////////////////////////////////////////
	// Long Reverb feedback delay
	///////////////////////////////////////////////////////////
														// Limit the position of the
														// long reverb feedback tap
	if(m_I3dl2Data.dwLongReverbFeedbackDelay>m_I3dl2Data.DelayLines[LongReverbDelayLineID].dwLength)	
	{
		m_I3dl2Data.dwLongReverbFeedbackDelay=m_I3dl2Data.DelayLines[LongReverbDelayLineID].dwLength;
	}
	//fprintf( fp , "%d" , dwLongReverbFeedbackDelay);

	
	///////////////////////////////////////////////////////////
	// Short reverb input gain
	///////////////////////////////////////////////////////////
	for(i=0;i<8;i++)
	{
		if((LONG)m_I3dl2Data.dwShortReverbInputGain[i] > 8388607)
        {
            m_I3dl2Data.dwShortReverbInputGain[i] = 8388607;
        }
        else if((LONG)m_I3dl2Data.dwShortReverbInputGain[i] < -8388607)
        {
            m_I3dl2Data.dwShortReverbInputGain[i] = -8388607;
        }
        /*if(fabs((LONG)m_I3dl2Data.dwShortReverbInputGain[i])>=8388608)
		{
			m_I3dl2Data.dwShortReverbInputGain[i]=ftoi((8388608-1)*(((LONG)m_I3dl2Data.dwShortReverbInputGain[i])>0?1:-1));
		}*/
		//fprintf( fp , "%d" , dwShortReverbInputGain[i]);
		//if(i!=7) //fprintf( fp , " " );
	}
	//fprintf( fp , "" );

	
	///////////////////////////////////////////////////////////
	// Long reverb input gain
	///////////////////////////////////////////////////////////
	//fprintf( fp , "%d" , dwLongReverbInputGain);


	///////////////////////////////////////////////////////////
	// Long Reverb Crossfeed gain
	///////////////////////////////////////////////////////////
	//fprintf( fp , "%d" , dwLongReverbCrossfeedGain);

	
	///////////////////////////////////////////////////////////
	// Reflections output gain
	///////////////////////////////////////////////////////////
	for(i=0;i<4;i++)
	{
		//fprintf( fp , "%d" , ReflectionsOutputGain[i]);
		//if(i!=3) //fprintf( fp , " " );
	}
	//fprintf( fp , "" );


	///////////////////////////////////////////////////////////
	// Short reverb output gain
	///////////////////////////////////////////////////////////
	//fprintf( fp , "%d" , ShortReverbOutputGain);


	///////////////////////////////////////////////////////////
	// Long reverb output gain
	///////////////////////////////////////////////////////////
	//fprintf( fp , "%d" , LongReverbOutputGain);


	///////////////////////////////////////////////////////////
	// Number output channels
	///////////////////////////////////////////////////////////
	//fprintf( fp , "%d" , dwChannelCount);

	///////////////////////////////////////////////////////////
	// IIR outputs
	///////////////////////////////////////////////////////////
	for(i=0;i<10;i++)
	{
		//fprintf( fp , "%d %d" , IIR[i].dwCoefficients[0] , IIR[i].dwCoefficients[1] );
	}
	//fprintf( fp , "" );

	//fclose(fp);}


    //
    // Convert DWORDs to BYTEs
    //

    for(i = 0; i < NUMELMS(m_I3dl2Data.dwReflectionsInputDelay); i++)
    {
        m_I3dl2Data.dwReflectionsInputDelay[i] *= sizeof(DWORD);
    }

    m_I3dl2Data.dwShortReverbInputDelay *= sizeof(DWORD);

    for(i = 0; i < NUMELMS(m_I3dl2Data.dwLongReverbInputDelay); i++)
    {
        m_I3dl2Data.dwLongReverbInputDelay[i] *= sizeof(DWORD);
    }

    for(i = 0; i < NUMELMS(m_I3dl2Data.dwReflectionsFeedbackDelay); i++)
    {
        m_I3dl2Data.dwReflectionsFeedbackDelay[i] *= sizeof(DWORD);
    }

    m_I3dl2Data.dwLongReverbFeedbackDelay *= sizeof(DWORD);
}


/****************************************************************************
 *
 *  Get1PoleLoPass
 *
 *  Description:
 *      Calculates low-pass filter values
 *
 *  Arguments:
 *      long [in]: gain.
 *      long [in]: high-frequency gain.
 *      float [in]: high-frequency reference.
 *      float [in]: sampling rate.
 *      float * [out]: coefficient A.
 *      float * [out]: coefficient B.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Listener::Get1PoleLoPass"

void 
CI3dl2Listener::Get1PoleLoPass
(
    long                    lGain, 
    long                    lGainHF, 
    float                   fHFReference, 
    float                   fSamplesPerSec, 
    float *                 pfA, 
    float *                 pfB
)
{
	static const float      pi          = 3.14159265f;
    double	                fW, fK, fG;
	double	                fCosW, fA;

    DPF_ENTER();

	ASSERT(pfA != NULL);
	ASSERT(pfB != NULL);
	ASSERT(lGainHF <= 0);

	fW = 2.f*pi*fHFReference/fSamplesPerSec;
	ASSERT(fW <= 2*pi);

	if(lGain <= DSBVOLUME_MIN)		// Output will be silent
	{
	    *pfA = 0.f;
	    *pfB = 0.f;
	}
	else if(lGain == 0 && lGainHF == 0)	// Audio is not changed by the filter
	{
	
	    *pfA = 0.f;
		*pfB = 1.f;
	}
	else if(lGainHF == 0)				// The filter is a pure gain
	{
	
	    *pfA = 0.f;
		*pfB = mBToLinear(lGain);
	}
	else
	{
	    // 
        // Filter has the transfer function:
        // 
		//     H(z) = k*	1-a
		// 		       ------
		// 		       1-az(-1)
        // 
	    // If Fh is the reference frequency, x is the attenuation in dB at Fh 
        // and y is the attenuation ratio at 0Hz then:
        // 
	    // k = 10^(x*y/20)
        // 
	    // a = 1 - g*cos(W) - sqrt(2g(1 - cos(W))-g*g*(1-cos^2(W)))
		//     -----------------------------------------------------
		// 					    1 - g
        // 
	    // g = 10^(x(1-y)/10)
        // 

		fK = mBToLinear(lGain);
		fG = mBToLinear(2 * lGainHF);
		fCosW = cos(fW);

		fA = (1.f - fG * fCosW - sqrt(2.f * fG * (1.f - fCosW) - fG * fG * (1.f - fCosW * fCosW))) / (1.f - fG);

		*pfA = (float)fA;
		*pfB = (float)(fK * (1.f - fA));
	}

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Get1PoleLoPass
 *
 *  Description:
 *      Calculates low-pass filter values
 *
 *  Arguments:
 *      long [in]: gain.
 *      long [in]: high-frequency gain.
 *      float [in]: high-frequency reference.
 *      float [in]: sampling rate.
 *      int * [out]: coefficient A.
 *      int * [out]: coefficient B.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Listener::Get1PoleLoPass"

void 
CI3dl2Listener::Get1PoleLoPass
(
    long                    lGain, 
    long                    lGainHF, 
    float                   fHFReference, 
    float                   fSamplesPerSec, 
    int *                   piA,
    int *                   piB
)
{
    FLOAT                   flA, flB;

    DPF_ENTER();

    Get1PoleLoPass(lGain, lGainHF, fHFReference, fSamplesPerSec, &flA, &flB);

    *piA = ftoi(flA * m_flScale23);
    *piB = ftoi(flB * m_flScale23);

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CI3dl2Source
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Source::CI3dl2Source"

const I3DL2SOURCE CI3dl2Source::m_DefaultI3dl2Data =
{
    0,                      // dwChangeMask
    0,                      // lDirect;
    0,                      // lSource;
    0,                      // nDirectIir;
    0                       // nReverbIir;
};

CI3dl2Source::CI3dl2Source
(
    const CI3dl2Listener &  Listener,
    REFDSI3DL2BUFFER        ds3db
)
:   m_Listener(Listener),
    m_I3dl2Params(ds3db)
{
    DPF_ENTER();

    m_I3dl2Data = m_DefaultI3dl2Data;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  CalculateI3dl2
 *
 *  Description:
 *      Sets all I3DL2 source parameters.
 *
 *  Arguments:
 *      LPCDSI3DL2LISTENER [in]: listener properties.
 *      LPCDSI3DL2BUFFER [in]: source properties.
 *      FLOAT [in]: distance between source and m_Listener.m_I3dl2Params.
 *      LPDSFX_I3dl2SOURCE_PARAMS [out]: effect data.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Source::CalculateI3dl2"

void
CI3dl2Source::CalculateI3dl2
(
    FLOAT                   flDistance
)
{
    FLOAT                   flDirectObstructionLevel;
    FLOAT                   flDirectOcclusionLevel;
    FLOAT                   flDirectHFObstructionLevel;
    FLOAT                   flDirectHFOcclusionLevel;
    FLOAT                   flReverbFeed;
    LONG                    lDirect;
    LONG                    lDirectHF;
    LONG                    lSource;
    LONG                    lSourceHF;
    INT                     nDirectIir;
    INT                     nReverbIir;

    DPF_ENTER();

    ASSERT(flDistance >= 0.0f);

    m_I3dl2Data.dwChangeMask = 0;

    flDistance *= m_Listener.m_I3dl2Params.flRoomRolloffFactor;
    flDistance *= m_I3dl2Params.flRoomRolloffFactor;

    //
    // Direct path level (may go above 0 volume)
    //

    flDirectObstructionLevel = (FLOAT)m_I3dl2Params.Obstruction.lHFLevel * m_I3dl2Params.Obstruction.flLFRatio;
    flDirectOcclusionLevel = (FLOAT)m_I3dl2Params.Occlusion.lHFLevel * m_I3dl2Params.Occlusion.flLFRatio;
    
    lDirect = m_I3dl2Params.lDirect + ftoi(flDirectObstructionLevel + flDirectOcclusionLevel);

    if(lDirect != m_I3dl2Data.lDirect)
    {
        m_I3dl2Data.lDirect = lDirect;
        m_I3dl2Data.dwChangeMask |= MCPX_I3DL2APPLY_DIRECT;
    }

    //
    // Direct path HF
    //

    flDirectHFObstructionLevel = (FLOAT)m_I3dl2Params.Obstruction.lHFLevel * (1.f - m_I3dl2Params.Obstruction.flLFRatio);
    flDirectHFOcclusionLevel = (FLOAT)m_I3dl2Params.Occlusion.lHFLevel * (1.f - m_I3dl2Params.Occlusion.flLFRatio);

    lDirectHF = m_I3dl2Params.lDirectHF + ftoi(flDirectHFObstructionLevel + flDirectHFOcclusionLevel);

    CHECKRANGE(lDirectHF, DSBVOLUME_MIN, DSBVOLUME_MAX);

    //
    // Reverb feed level (may go above 0 volume)
    //

    flReverbFeed = flDirectOcclusionLevel;

    if(flDistance > 1.0f)
    {
        flReverbFeed += LinearTomB(flDistance);
    }

    lSource = m_I3dl2Params.lRoom + ftoi(flReverbFeed);

    if(lSource != m_I3dl2Data.lSource)
    {
        m_I3dl2Data.lSource = lSource;
        m_I3dl2Data.dwChangeMask |= MCPX_I3DL2APPLY_SOURCE;
    }

    //
    // Reverb feed HF
    //

    lSourceHF = m_I3dl2Params.lRoom + ftoi(flDirectHFOcclusionLevel);

    CHECKRANGE(lSourceHF, DSBVOLUME_MIN, DSBVOLUME_MAX);

    //
    // Low-pass filters
    //

    nDirectIir = Get1PoleLoPass(0, lDirectHF, m_Listener.m_I3dl2Params.flHFReference, MCPX_BASEFREQ);

    if(nDirectIir != m_I3dl2Data.nDirectIir)
    {
        m_I3dl2Data.nDirectIir = nDirectIir;
        m_I3dl2Data.dwChangeMask |= MCPX_I3DL2APPLY_DIRECTIIR;
    }

    nReverbIir = Get1PoleLoPass(0, lSourceHF, m_Listener.m_I3dl2Params.flHFReference, MCPX_BASEFREQ);

    if(nReverbIir != m_I3dl2Data.nReverbIir)
    {
        m_I3dl2Data.nReverbIir = nReverbIir;
        m_I3dl2Data.dwChangeMask |= MCPX_I3DL2APPLY_REVERBIIR;
    }

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  Get1PoleLoPass
 *
 *  Description:
 *      Calculates low-pass filter values
 *
 *  Arguments:
 *      long [in]: gain.
 *      long [in]: high-frequency gain.
 *      float [in]: high-frequency reference.
 *      float [in]: sampling rate.
 *
 *  Returns:  
 *      int: coefficient A.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CI3dl2Source::Get1PoleLoPass"

int
CI3dl2Source::Get1PoleLoPass
(
    long                    lGain, 
    long                    lGainHF, 
    float                   fHFReference, 
    float                   fSamplesPerSec
)
{
	static const float      pi              = 3.14159265f;
    double                  fG, fW, fCosW;
    float                   fA;
    int                     nA;

    ASSERT(lGainHF <= 0);

    if(lGain <= DSBVOLUME_MIN)
    {
        //
        // Output will be silent.
        //

        nA = 0;
    }
    else if(lGain == 0 && lGainHF == 0)
    {
        //
        // Audio is not changed by the filter.
        //
        
        nA = 0;
    }
    else if(lGainHF == 0)
    {
        //
        // The filter is a pure gain.
        //
        
        nA = 0;
    }
    else
    {
        //
        // If Fh is the reference frequency, x is the attenuation in dB at Fh 
        // and y is the attenuation ratio at 0Hz then:
        //
        // g = 10^(x(1-y)/10)
        //
        // a = 1 - g*cos(W) - sqrt(2g(1 - cos(W))-g*g*(1-cos^2(W)))
        //     -----------------------------------------------------
        //                            1 - g
        //
    
        fW = 2.f * pi * fHFReference / fSamplesPerSec;
        ASSERT(fW <= 2 * pi);

        fCosW = cos(fW);

        fG = mBToLinear(2 * lGainHF);

        fA = (float)((1.f - fG * fCosW - sqrt(2.f * fG * (1.f - fCosW) - fG * fG * (1.f - fCosW * fCosW))) / (1.f - fG));

        if(fA >= 1.f)
        {
            nA = 65535;
        }
        else
        {
            nA = ftoi(fA * 65536.f);
        }
    }

    DPF_LEAVE(nA);

    return nA;
}


