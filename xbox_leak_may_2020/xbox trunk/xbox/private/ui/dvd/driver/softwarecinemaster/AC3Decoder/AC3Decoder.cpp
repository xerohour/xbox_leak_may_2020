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

#include "AC3Decoder.h"
#include "math.h"
#include <memory.h>
#include "library\common\vddebug.h"
#include "..\common\TimedFibers.h"

#define BITSTREAM_CHECK		1
#define PCM_CLIPPING		1
#define DUMP_FFT_DATA		0
#define USE_SIMPLE_DOWNMIX	0
#define USE_DITHERING		1
#define DUMP_ERRORS			0
#define USE_PCM_DITHERING	0
#define DUMP_COMPRESSION	0
#define USE_TRIANGLE_DITHER	1
#define USE_SIMD_WINDOWING	1

#include "AC3Tables.h"

#define chcoeff	chpostfft
//#define lfecoeff	lfepostfft

#if DUMP_FFT_DATA
#include <stdio.h>
FILE * dfile;
#endif

int FrameCnt;

//#define UFDB(x) ((fftreal)(pow(1.258925412, (fftreal)x / 2)))
#define UFDB(x) ((fftreal)(pow(1.258925412, (fftreal)x * 1.00343331887 / 2)))

static fftreal NegativeUDB[32] =
	{
	UFDB(-31), UFDB(-30), UFDB(-29), UFDB(-28), UFDB(-27), UFDB(-26), UFDB(-25), UFDB(-24),
	UFDB(-23), UFDB(-22), UFDB(-21), UFDB(-20), UFDB(-19), UFDB(-18), UFDB(-17), UFDB(-16),
	UFDB(-15), UFDB(-14), UFDB(-13), UFDB(-12), UFDB(-11), UFDB(-10), UFDB(- 9), UFDB(- 8),
	UFDB(- 7), UFDB(- 6), UFDB(- 5), UFDB(- 4), UFDB(- 3), UFDB(- 2), UFDB(- 1), 1.0
	};

static const int Plus11DBCompr = 7509;

complex cossintab64[64];
complex xcossin512[128];
complex xcossin256[64];

static const double pi =  3.14159265359;

double xcos1(int k, int n)
	{
	return -cos(2*pi*(8*k+1)/(8*n));
	}

double xsin1(int k, int n)
	{
	return -sin(2*pi*(8*k+1)/(8*n));
	}

double xcos2(int k, int n)
	{
	return -cos(2*pi*(8*k+1)/(4*n));
	}

double xsin2(int k, int n)
	{
	return -sin(2*pi*(8*k+1)/(4*n));
	}

static void InitIFFT(void)
	{
	int i;
	double w;

	for(i=0; i<64; i++)
		{
		w = 2 * pi * i / 128;
		cossintab64[i].r = (fftreal)cos(w);
		cossintab64[i].i = (fftreal)sin(w);
		}

	for(i=0; i<128; i++)
		{
		xcossin512[i].r = (fftreal)xcos1(i, 512);
		xcossin512[i].i = (fftreal)xsin1(i, 512);
		}

	for(i=0; i<64; i++)
		{
		xcossin256[i].r = (fftreal)xcos2(i, 512);
		xcossin256[i].i = (fftreal)xsin2(i, 512);
		}
	}


static inline short MKShort(unsigned char high, unsigned char low)
	{
	return ((short)high << 8) | (short)low;
	}


AC3Decoder::AC3Decoder(AudioBitStream * strm,
							  PCMWaveOut * pcmWaveOut,
							  TimingSlave * timingSlave,
							  GenericProfile			*	globalProfile,
							  GenericProfile			*	profile)
	{
#pragma warning(disable : 4244 4305)
	static fftreal valtab1[] = {-2./3., 0., 2./3.};
	static fftreal valtab2[] = {-4./5., -2./5., 0., 2./5., 4./5.};
	static fftreal valtab4[] = {-10./11., -8./11., -6./11., -4./11., -2./11., 0., 2./11., 4./11., 6./11., 8./11., 10./11.};
#pragma warning(default : 4244 4305)

	this->strm = strm;
	this->pcmWaveOut = pcmWaveOut;
	this->timingSlave = timingSlave;
	this->globalProfile = globalProfile;
	this->profile = profile;

	int i;

	for(i=0; i<27; i++)
		{
		mant1lt[i][0] = valtab1[i / 9];
		mant1lt[i][1] = valtab1[i / 3 % 3];
		mant1lt[i][2] = valtab1[i % 3];
		}

	for(i=28; i<32; i++)
		{
		mant1lt[i][0] = 0;
		mant1lt[i][1] = 0;
		mant1lt[i][2] = 0;
		}

	for(i=0; i<128; i++)
		{
		if (i < 125)
			{
			mant2lt[i][0] = valtab2[i / 25];
			mant2lt[i][1] = valtab2[i / 5 % 5];
			mant2lt[i][2] = valtab2[i % 5];
			}
		else
			{
			mant2lt[i][0] = 0;
			mant2lt[i][1] = 0;
			mant2lt[i][2] = 0;
			}
		if (i < 121)
			{
			mant4lt[i][0] = valtab4[i / 11];
			mant4lt[i][1] = valtab4[i % 11];
			}
		else
			{
			mant4lt[i][0] = 0;
			mant4lt[i][1] = 0;
			}
		if (i < 125)
			{
			exp5lt[i][0] = i / 25 - 2;
			exp5lt[i][1] = i / 5 % 5 - 2;
			exp5lt[i][2] = i % 5 - 2;
			}
		else
			{
			exp5lt[i][0] = 0;
			exp5lt[i][1] = 0;
			exp5lt[i][2] = 0;
			}
		}

	InitIFFT();
	InitWindow();

	lastfscod = 0xff;

	ac3DigitalOutput = false;

	globalProfile->Read("AudioDecoder", "UseLFE", enableLFE, FALSE);

	SetAC3Config(AC3DYNRG_COMPRESSED, enableLFE);
	SetAudioOutConfig(AC3SC_20_SURROUND_COMPATIBLE, AC3DMDM_STEREO, AC3KARA_AWARE);

	frameSize = 0;

#if DUMP_FFT_DATA
//	dfile = fopen("f:\\ac3\\dmp.txt", "w");
	dfile = fopen("D:\\dmp.txt", "w");
#endif
	}

static inline fftreal ShiftFP(fftreal f, int n)
	{
	static fftreal mulh[] =
		{1. / (1 <<  0), 1. / (1 <<  1), 1. / (1 <<  2), 1. / (1 <<  3),
		 1. / (1 <<  4), 1. / (1 <<  5), 1. / (1 <<  6), 1. / (1 <<  7),
		 1. / (1 <<  8), 1. / (1 <<  9), 1. / (1 << 10), 1. / (1 << 11),
		 1. / (1 << 12), 1. / (1 << 13), 1. / (1 << 14), 1. / (1 << 15),
		 1. / (1 << 16), 1. / (1 << 17), 1. / (1 << 18), 1. / (1 << 19),
		 1. / (1 << 20), 1. / (1 << 21), 1. / (1 << 22), 1. / (1 << 23),
		 1. / (1 << 24), 1. / (1 << 25), 1. / (1 << 26), 1. / (1 << 27),
		 1. / (1 << 28), 1. / (1 << 29), 1. / (1 << 30), 1. / (1 << 16) / (1 << 15),
		 1. / (1 << 16) / (1 << 16),
		 1. / (1 << 16) / (1 << 17),
		 1. / (1 << 16) / (1 << 18),
		 1. / (1 << 16) / (1 << 19),
		 1. / (1 << 16) / (1 << 20),
		 1. / (1 << 16) / (1 << 21),
		 1. / (1 << 16) / (1 << 22),
		 1. / (1 << 16) / (1 << 23),
		 1. / (1 << 16) / (1 << 24),
		 1. / (1 << 16) / (1 << 25),
		 1. / (1 << 16) / (1 << 26),
		 1. / (1 << 16) / (1 << 27),
		 1. / (1 << 16) / (1 << 28),
		 1. / (1 << 16) / (1 << 29),
		 1. / (1 << 16) / (1 << 30),
		 1. / (1 << 16) / (1 << 16) / (1 << 15),

		 1. / (1 << 16) / (1 << 16) / (1 << 16),
		 1. / (1 << 16) / (1 << 16) / (1 << 17),
		 1. / (1 << 16) / (1 << 16) / (1 << 18),
		 1. / (1 << 16) / (1 << 16) / (1 << 19),
		 1. / (1 << 16) / (1 << 16) / (1 << 20),
		 1. / (1 << 16) / (1 << 16) / (1 << 21),
		 1. / (1 << 16) / (1 << 16) / (1 << 22),
		 1. / (1 << 16) / (1 << 16) / (1 << 23),
		 1. / (1 << 16) / (1 << 16) / (1 << 24),
		 1. / (1 << 16) / (1 << 16) / (1 << 25),
		 1. / (1 << 16) / (1 << 16) / (1 << 26),
		 1. / (1 << 16) / (1 << 16) / (1 << 27),
		 1. / (1 << 16) / (1 << 16) / (1 << 28),
		 1. / (1 << 16) / (1 << 16) / (1 << 29),
		 1. / (1 << 16) / (1 << 16) / (1 << 30),
		 1. / (1 << 16) / (1 << 16) / (1 << 16) / (1 << 15)
		};

	return f * mulh[n];
	}


void AC3Decoder::StartParse(void)
	{
	int i, ch;

//#define START_CLEAR	syncword
//#define END_CLEAR		skiple

//	memset(&(this->START_CLEAR), 0, ((BYTE *)&(this->END_CLEAR)) - (BYTE *)&(this->START_CLEAR));

	for(ch = 0; ch< MAX_AC3_CHANNELS; ch++)
		for(i=0; i<256; i++)
			{
			overlap[ch][i] = 0;
			}


	for(ch = 0; ch < MAX_AC3_CHANNELS; ch++)
		for(i=0; i<256; i++)
			chcoeff[ch][i] = 0;

	for(i=0; i<256; i++)
		lfecoeff[i] = 0;

	dithReg = 1;
	}

static inline void MixChannels(fftreal * dst, fftreal * src, int num)
	{
	int i;

	for(i=0; i<num; i++)
		{
		*dst++ += *src++ * (fftreal)M3DB;
		}
	}

static inline void FMixChannels(fftreal * dst, fftreal * src, int num, fftreal f)
	{
	int i;

	for(i=0; i<num; i++)
		{
		*dst++ += *src++ * f;
		}
	}

static inline void AddChannels(fftreal * dst, fftreal * src, int num)
	{
	int i;

	for(i=0; i<num; i++)
		{
		*dst++ += *src++;
		}
	}

static inline void MulChannel(fftreal * dst, fftreal val, int num)
	{
	int i;

	for(i=0; i<num; i++)
		{
		*dst++ *= val;
		}
	}

static inline void MulChannels(fftreal * dst, fftreal * src, int num)
	{
	int i;

	for(i=0; i<num; i++)
		{
		*dst++ *= *src++;
		}
	}

static inline void IMixChannels(fftreal * dst, fftreal * src, int num)
	{
	int i;

	for(i=0; i<num; i++)
		{
		*dst++ -= *src++ * (fftreal)M3DB;
		}
	}

static const bool chp = true;

static const fftreal cmixs[] = {(fftreal)M3DB, (fftreal)0.594603557, (fftreal)0.500, (fftreal)0.594603557};
static const fftreal smixs[] = {(fftreal)M3DB, (fftreal)0.500, (fftreal)0.000, (fftreal)0.500};

void AC3Decoder::PreDownmixChannels(void)
	{
		{
		switch (acmod)
			{
			case 0:
				switch (dualMode)
					{
					case AC3DMDM_CHANNEL1:
						leftChannelIndex = 0;
						rightChannelIndex = 0;
						leftMultiplier = (fftreal)M3DB;
						rightMultiplier = (fftreal)M3DB;
						break;
					case AC3DMDM_CHANNEL2:
						leftChannelIndex = 1;
						rightChannelIndex = 1;
						leftMultiplier = (fftreal)M3DB;
						rightMultiplier = (fftreal)M3DB;
						break;
					case AC3DMDM_MIX:
						leftChannelIndex = 0;
						rightChannelIndex = 1;
						leftMultiplier = 0.5;
						rightMultiplier = 0.5;
						break;
					default:
					case AC3DMDM_STEREO:
						leftChannelIndex = 0;
						rightChannelIndex = 1;
						leftMultiplier = 1.0;
						rightMultiplier = 1.0;
						break;
					}
				break;
			case 2:
				leftChannelIndex = 0;
				rightChannelIndex = 1;
				leftMultiplier = 1.0;
				rightMultiplier = 1.0;
				break;
			case 1:
				leftChannelIndex = 0;
				rightChannelIndex = 0;
				leftMultiplier = (fftreal)M3DB;
				rightMultiplier = (fftreal)M3DB;
				break;
			case 3:
				channelPremixed[1] = TRUE;
				leftChannelIndex = 0;
				rightChannelIndex = 2;

				if (surroundDownmix)
					{
					leftMultiplier = (fftreal)(1./(1. + M3DB));
					rightMultiplier = (fftreal)(1./(1. + M3DB));
					if (blksw[0] == blksw[1])
						MixChannels(chcoeff[0], chcoeff[1], 256);
					else
						channelPremixed[1] = FALSE;
					if (chp && blksw[1] == blksw[2])
						MixChannels(chcoeff[2], chcoeff[1], 256);
					else
						channelPremixed[1] = FALSE;
					}
				else
					{
					cmix = cmixs[cmixlev];
					leftMultiplier = rightMultiplier = (fftreal)1./ ((fftreal)1. + cmix);

					if (chp && blksw[0] == blksw[1])
						FMixChannels(chcoeff[0], chcoeff[1], 256, cmix);
					else
						channelPremixed[1] = FALSE;

					if (chp && blksw[1] == blksw[2])
						FMixChannels(chcoeff[2], chcoeff[1], 256, cmix);
					else
						channelPremixed[1] = FALSE;
					}

	#if USE_SIMPLE_DOWNMIX
				leftMultiplier = rightMultiplier = 0.291703308;
	#endif
				break;
			case 4:
				leftChannelIndex = 0;
				rightChannelIndex = 1;

				if (surroundDownmix)
					{
					leftMultiplier = (fftreal)(1./(1. + M3DB));
					rightMultiplier = (fftreal)(1./(1. + M3DB));

					if (chp && blksw[0] == blksw[2] && blksw[1] == blksw[2])
						{
						IMixChannels(chcoeff[0], chcoeff[2], 256);
						MixChannels(chcoeff[1], chcoeff[2], 256);
						channelPremixed[2] = TRUE;
						}
					else
						channelPremixed[2] = FALSE;

					}
				else
					{
					smix = smixs[surmixlev] * (fftreal)M3DB;
					leftMultiplier = rightMultiplier = (fftreal)1./ ((fftreal)1. + smix);

					if (chp && blksw[0] == blksw[2] && blksw[1] == blksw[2])
						{
						FMixChannels(chcoeff[0], chcoeff[2], 256, smix);
						FMixChannels(chcoeff[1], chcoeff[2], 256, smix);
						channelPremixed[2] = TRUE;
						}
					else
						channelPremixed[2] = FALSE;
					}

	#if USE_SIMPLE_DOWNMIX
				leftMultiplier = rightMultiplier = 0.291703308;
	#endif
				break;
			case 5:
				leftChannelIndex = 0;
				rightChannelIndex = 2;

				if (surroundDownmix)
					{
					leftMultiplier = (fftreal)(1./(1. + 2 * M3DB));
					rightMultiplier = (fftreal)(1./(1. + 2 * M3DB));

					if (chp && blksw[0] == blksw[1] && blksw[2] == blksw[1])
						{
						MixChannels(chcoeff[0], chcoeff[1], 256);
						MixChannels(chcoeff[2], chcoeff[1], 256);
						channelPremixed[1] = TRUE;
						}
					else
						channelPremixed[1] = FALSE;

					if (chp && blksw[0] == blksw[3] && blksw[2] == blksw[3])
						{
						IMixChannels(chcoeff[0], chcoeff[3], 256);
						MixChannels(chcoeff[2], chcoeff[3], 256);
						channelPremixed[3] = TRUE;
						}
					else
						channelPremixed[3] = FALSE;
					}
				else
					{
					cmix = cmixs[cmixlev];
					smix = smixs[surmixlev] * (fftreal)M3DB;
					leftMultiplier = rightMultiplier = (fftreal)1./ ((fftreal)1. + cmix + smix);

					if (chp && blksw[0] == blksw[1] && blksw[1] == blksw[2])
						{
						FMixChannels(chcoeff[0], chcoeff[1], 256, cmix);
						FMixChannels(chcoeff[2], chcoeff[1], 256, cmix);
						channelPremixed[1] = TRUE;
						}
					else
						channelPremixed[1] = FALSE;

					if (chp && blksw[0] == blksw[3] && blksw[3] == blksw[2])
						{
						FMixChannels(chcoeff[0], chcoeff[3], 256, smix);
						FMixChannels(chcoeff[2], chcoeff[3], 256, smix);
						channelPremixed[3] = TRUE;
						}
					else
						channelPremixed[3] = FALSE;
					}

	#if USE_SIMPLE_DOWNMIX
				leftMultiplier = rightMultiplier = 0.291703308;
	#endif
				break;
			case 6:
				leftChannelIndex = 0;
				rightChannelIndex = 1;

				if (surroundDownmix)
					{
					leftMultiplier = (fftreal)(1./(1. + 2 * M3DB));
					rightMultiplier = (fftreal)(1./(1. + 2 * M3DB));

					if (chp && blksw[2] == blksw[3])
						{
						AddChannels(chcoeff[2], chcoeff[3], 256);
						channelPremixed[3] = TRUE;

						if (blksw[0] == blksw[2] && blksw[1] == blksw[2])
							{
							IMixChannels(chcoeff[0], chcoeff[2], 256);
							MixChannels(chcoeff[1], chcoeff[2], 256);
							channelPremixed[2] = TRUE;
							}
						else
							channelPremixed[2] = FALSE;
						}
					else
						{
						channelPremixed[2] = FALSE;
						channelPremixed[3] = FALSE;
						}

					}
				else
					{
					smix = smixs[surmixlev];
					leftMultiplier = rightMultiplier = (fftreal)1./ ((fftreal)1. + smix);

					if (chp && blksw[0] == blksw[2])
						{
						FMixChannels(chcoeff[0], chcoeff[2], 256, smix);
						channelPremixed[2] = TRUE;
						}
					else
						channelPremixed[2] = FALSE;

					if (chp && blksw[1] == blksw[3])
						{
						FMixChannels(chcoeff[1], chcoeff[3], 256, smix);
						channelPremixed[3] = TRUE;
						}
					else
						channelPremixed[3] = FALSE;

					}

	#if USE_SIMPLE_DOWNMIX
				leftMultiplier = rightMultiplier = 0.291703308;
	#endif
				break;
			case 7:
				channelPremixed[1] = TRUE;
				channelPremixed[3] = TRUE;
				channelPremixed[4] = TRUE;

				leftChannelIndex = 0;
				rightChannelIndex = 2;

				if (surroundDownmix)
					{
					leftMultiplier = (fftreal)(1./(1. + 3. * M3DB)); // / 1.09835964;
					rightMultiplier = (fftreal)(1./(1. + 3. * M3DB)); // / 1.09835964;

					if (chp && blksw[0] == blksw[1] &&
						 blksw[0] == blksw[2] &&
						 blksw[0] == blksw[3] &&
						 blksw[0] == blksw[4])
						{
						fftreal c, s;
						int i;
						for(i=0; i<256; i++)
							{
							c = chcoeff[1][i];
							s = chcoeff[3][i] + chcoeff[4][i];
							chcoeff[0][i] += (fftreal)M3DB * (c - s);
							chcoeff[2][i] += (fftreal)M3DB * (c + s);
							}
						}
					else
						{
						if (chp && blksw[0] == blksw[1])
							MixChannels(chcoeff[0], chcoeff[1], 256);
						else
							channelPremixed[1] = FALSE;

						if (chp && blksw[1] == blksw[2])
							MixChannels(chcoeff[2], chcoeff[1], 256);
						else
							channelPremixed[1] = FALSE;

						if (chp && blksw[3] == blksw[4])
							{
							AddChannels(chcoeff[3], chcoeff[4], 256);

							if (blksw[0] == blksw[3])
								IMixChannels(chcoeff[0], chcoeff[3], 256);
							else
								channelPremixed[3] = FALSE;

							if (blksw[2] == blksw[3])
								MixChannels(chcoeff[2], chcoeff[3], 256);
							else
								channelPremixed[3] = FALSE;
							}
						else
							{
							channelPremixed[3] = FALSE;
							channelPremixed[4] = FALSE;
							}
						}
					}
				else
					{
					cmix = cmixs[cmixlev];
					smix = smixs[surmixlev];
					leftMultiplier = rightMultiplier = (fftreal)1./ ((fftreal)1. + cmix + smix);

					if (chp && blksw[0] == blksw[1])
						FMixChannels(chcoeff[0], chcoeff[1], 256, cmix);
					else
						channelPremixed[1] = FALSE;

					if (chp && blksw[1] == blksw[2])
						FMixChannels(chcoeff[2], chcoeff[1], 256, cmix);
					else
						channelPremixed[1] = FALSE;

					if (chp && blksw[0] == blksw[3])
						FMixChannels(chcoeff[0], chcoeff[3], 256, smix);
					else
						channelPremixed[3] = FALSE;

					if (chp && blksw[2] == blksw[4])
						FMixChannels(chcoeff[2], chcoeff[4], 256, smix);
					else
						channelPremixed[4] = FALSE;
					}

	#if USE_SIMPLE_DOWNMIX
				leftMultiplier = rightMultiplier = 0.291703308;
	#endif
				break;
			}

		channelPremixed[leftChannelIndex] = FALSE;
		channelPremixed[rightChannelIndex] = FALSE;

		downmixActive = leftMultiplier < 1.0 && acmod > 2 ;

		if (acmod > 2)
			{
			leftMultiplier = 1.0;
			rightMultiplier = 1.0;
			}
		}
	}

fftreal AC3Decoder::CalcCompression(int cVal, int hscl, int lscl)
	{
	fftreal mul, dmul;

	dmul = NegativeUDB[dialnorm];

	if (cVal < 0)
		{
		cVal = (cVal * hscl) >> 8;
		mul = (fftreal)(0x1000 | (cVal & 0xfff)) / 0x2000;
		if (cVal < 0)
			mul /= (fftreal)(1 << (15 - ((WORD)cVal >> 12)));
		else
			mul *= (fftreal)(2 << (cVal >> 12));
		}
	else
		{
		cVal = (cVal * lscl) >> 8;
		mul = (fftreal)(0x1000 | (cVal & 0xfff)) / 0x2000;
		mul *= (fftreal)(2 << (cVal >> 12));
		}

	if (useDialNorm) mul *= dmul;

	return mul;
	}


void AC3Decoder::DynamicRangeCompression(void)
	{
	fftreal mul;

	if (dynrng || dialnorm != 31 && useDialNorm ||  compressionMode != COMP_MODE_LINE)
		{
		int iCompr = (short)(dynrng << 8) >> 1;
		int hscl = 256;
		int lscl = 256;

		switch (compressionMode)
			{
			case COMP_MODE_CUSTOM_A:
			case COMP_MODE_CUSTOM_B:
				// must apply scaling before -11db
				if (iCompr<0)
					iCompr = (iCompr * hdr) >> 8;
				else
					iCompr = (iCompr * ldr) >> 8;

				if (downmixActive || (bsmod == 7 && acmod >= 2))
					iCompr -= Plus11DBCompr;
				break;

			case COMP_MODE_LINE:
				lscl = ldr;
				if (!downmixActive)
					hscl = hdr;
				break;

			case COMP_MODE_RF:
				if (compre)
					{
					if (compr)
						iCompr = (short)(compr << 8);

					if (drc != AC3DYNRG_MINIMUM)
						iCompr += Plus11DBCompr;
					}
				else
					if (downmixActive || (bsmod == 7 && acmod >= 2))
						iCompr -= Plus11DBCompr;
				break;

			}

			mul = CalcCompression(iCompr, hscl, lscl);

#if DUMP_COMPRESSION
			{
			char buffer[100];
			wsprintf(buffer, "CE %d CP %2x DR %2x DN %2x M %d.%2d ", compre, compr, dynrng, dialnorm, (int)(mul), (int)(mul * 100) % 100);
			OutputDebugString(buffer);
			}
#endif

		leftMultiplier *= mul;
		rightMultiplier *= mul;
		}
	}


void AC3Decoder::PostDownmixChannels(void)
	{
	switch (acmod)
		{
		case 0:
			switch (dualMode)
				{
				case AC3DMDM_MIX:
					AddChannels(chpostfft[0], chpostfft[1], 512);
					memcpy(chpostfft[1], chpostfft[0], 512*sizeof(fftreal));
					break;
				default:
				case AC3DMDM_CHANNEL1:
				case AC3DMDM_CHANNEL2:
				case AC3DMDM_STEREO:
					break;
				}
			break;
		case 1:
		case 2:
			break;
		case 3:
			if (surroundDownmix)
				{
				if (blksw[0] != blksw[1] || !chp)
					MixChannels(chpostfft[0], chpostfft[1], 512);
				if (blksw[1] != blksw[2] || !chp)
					MixChannels(chpostfft[2], chpostfft[1], 512);
				}
			else
				{
				if (blksw[0] != blksw[1] || !chp)
					FMixChannels(chpostfft[0], chpostfft[1], 512, cmix);
				if (blksw[1] != blksw[2] || !chp)
					FMixChannels(chpostfft[2], chpostfft[1], 512, cmix);
				}
			break;
		case 4:
			if (surroundDownmix)
				{
				if (blksw[0] != blksw[2] || blksw[1] != blksw[2] || !chp)
					{
					IMixChannels(chpostfft[0], chpostfft[2], 512);
					MixChannels(chpostfft[1], chpostfft[2], 512);
					}
				}
			else
				{
				if (blksw[0] != blksw[2] || blksw[1] != blksw[2] || !chp)
					{
					FMixChannels(chpostfft[0], chpostfft[2], 512, smix);
					FMixChannels(chpostfft[1], chpostfft[2], 512, smix);
					}
				}
			break;
		case 5:
			if (surroundDownmix)
				{
				if (blksw[0] != blksw[1] || blksw[2] != blksw[1] || !chp)
					{
					MixChannels(chpostfft[0], chpostfft[1], 512);
					MixChannels(chpostfft[2], chpostfft[1], 512);
					}

				if (blksw[0] != blksw[3] || blksw[2] != blksw[3] || !chp)
					{
					IMixChannels(chpostfft[0], chpostfft[3], 512);
					MixChannels(chpostfft[2], chpostfft[3], 512);
					}
				}
			else
				{
				if (blksw[0] != blksw[1] || blksw[1] != blksw[2] || !chp)
					{
					FMixChannels(chpostfft[0], chpostfft[1], 512, cmix);
					FMixChannels(chpostfft[2], chpostfft[1], 512, cmix);
					}

				if (blksw[0] != blksw[3] || blksw[3] != blksw[2] || !chp)
					{
					FMixChannels(chpostfft[0], chpostfft[3], 512, smix);
					FMixChannels(chpostfft[2], chpostfft[3], 512, smix);
					}
				}
			break;
		case 6:
			if (surroundDownmix)
				{
				if (blksw[2] != blksw[3] || !chp)
					{
					AddChannels(chpostfft[2], chpostfft[3], 512);
					}

				if (blksw[0] != blksw[2] || blksw[1] != blksw[2] || !chp)
					{
					IMixChannels(chpostfft[0], chpostfft[2], 512);
					MixChannels(chpostfft[1], chpostfft[2], 512);
					}
				}
			else
				{
				if (blksw[0] != blksw[2] || !chp)
					{
					FMixChannels(chpostfft[0], chpostfft[2], 512, smix);
					}

				if (blksw[1] != blksw[3] || !chp)
					{
					FMixChannels(chpostfft[1], chpostfft[3], 512, smix);
					}
				}
			break;
		case 7:
			if (surroundDownmix)
				{
				if (!chp || blksw[0] != blksw[1] || !chp)
					MixChannels(chpostfft[0], chpostfft[1], 512);

				if (!chp || blksw[1] != blksw[2] || !chp)
					MixChannels(chpostfft[2], chpostfft[1], 512);

				if (!chp || blksw[3] != blksw[4] || !chp)
					{
					AddChannels(chpostfft[3], chpostfft[4], 512);
					IMixChannels(chpostfft[0], chpostfft[3], 512);
					MixChannels(chpostfft[2], chpostfft[3], 512);
					}
				else
					{
					if (blksw[0] != blksw[3] || !chp)
						IMixChannels(chpostfft[0], chpostfft[3], 512);

					if (blksw[2] != blksw[3] || !chp)
						MixChannels(chpostfft[2], chpostfft[3], 512);
					}
				}
			else
				{
				if (!chp || blksw[0] != blksw[1] || !chp)
					FMixChannels(chpostfft[0], chpostfft[1], 512, cmix);

				if (!chp || blksw[1] != blksw[2] || !chp)
					FMixChannels(chpostfft[2], chpostfft[1], 512, cmix);

				if (!chp || blksw[0] != blksw[3] || !chp)
					FMixChannels(chpostfft[0], chpostfft[3], 512, smix);

				if (!chp || blksw[2] != blksw[4] || !chp)
					FMixChannels(chpostfft[2], chpostfft[4], 512, smix);
				}
			break;
		}
	if (lfeon && enableLFE)
		{
		FMixChannels(chpostfft[0], lfepostfft, 512, (fftreal)3.16);
		FMixChannels(chpostfft[2], lfepostfft, 512, (fftreal)3.16);
		}
	}



static inline int ScaleToInt(fftreal val, fftreal smul)
	{
	int tmp;
	fftreal ftmp = (fftreal)((val * smul) + 0.5);

	if (ftmp < -32767.0) tmp = -32768;
	else if (ftmp > 32768.0) tmp = 32768;
	else
		{
		tmp = (int)ftmp;
		if ((fftreal)tmp > ftmp) tmp--;
		}

	if (tmp < -32768)
		return -32768;
	else if (tmp > 32767)
		return 32767;
	else
		return tmp;
	}


void AC3Decoder::InitWindow(void)
	{
	int i;

	// added scaling from fftreal to word into window (*32768.0)
	for(i=0; i<256; i++)
		{
		ac3WindowTable2[i] = (fftreal) (2.0 * AC3WindowTable[i] * 32768.0);
		ac3WindowTable2[511-i] = (fftreal) (2.0 * AC3WindowTable[i] * 32768.0);
		}
	}

void AC3Decoder::WindowChannels(short * leftRightBuffer)
	{
	fftreal lm, rm;

	lm = leftMultiplier;
	rm = rightMultiplier;


#if USE_SIMD_WINDOWING
	fftreal * op = &(overlap[0][0]);
	fftreal * lp = &(chpostfft[leftChannelIndex][0]);
	fftreal * rp = &(chpostfft[rightChannelIndex][0]);
	fftreal * wp = &(ac3WindowTable2[0]);
	short * lrp = leftRightBuffer;

	_asm
		{
		mov	esi, [lp]
		mov	edi, [rp]
		mov	edx, [lrp]
		mov	ebx, [wp]
		mov	ecx, [op]
		mov	eax, -256

		movss		xmm7, [lm]
		movss		xmm6, [rm]
		shufps		xmm7, xmm7, 00000000b				// lm, lm		lm, lm
		shufps		xmm6, xmm6, 00000000b				// rm, rm		rm, rm

loop1:
		movaps		xmm0, [esi + eax*4 + 4*256]			// chpostfft[leftChannelIndex][0,1,2,3
		movaps		xmm1, [edi + eax*4 + 4*256]			// chpostfft[rightChannelIndex][0....
		mulps		xmm0, xmm7							// leftIndex][0...] * lm
		movaps		xmm4, [ebx + eax*4 + 4*256]			// ac3WindowTable2[0]
		mulps		xmm1, xmm6							// rightIndex][0...] * rm

		mulps		xmm0, xmm4							// ac3WindowTable2[0] * leftIndex[0]*lm
		mulps		xmm1, xmm4							// ac3WindowTable2[0] * rightIndex[0]*lm

		addps		xmm0, [ecx + eax*4 + 4*256]			// overlap[0][0] + ac3WindowTable2[0] * leftIndex[0]*lm
		addps		xmm1, [ecx + eax*4 + 4*256 + 4*256] // overlap[1][0] + ac3WindowTable2[0] * rightIndex[0]*lm

		movaps		xmm2, [esi + eax*4 + 4*256 + 4*256] // chpostfft[leftChannelIndex][256,,,,
		movaps		xmm3, [edi + eax*4 + 4*256 + 4*256] // chpostfft[rightChannelIndex][256,,,,
		mulps		xmm2, xmm7							// leftIndex][256] * lm
		movaps		xmm4, [ebx + eax*4 + 4*256 + 4*256]	// ac3WindowTable2[256]
		mulps		xmm3, xmm6							// rightIndex][256] * rm

		mulps		xmm2, xmm4							// ac3WindowTable2[256] * leftIndex][256] * lm
		mulps		xmm3, xmm4							// ac3WindowTable2[256] * leftIndex][256] * lm

		movaps		[ecx + eax*4 + 4*256], xmm2			// save  overlap[0][0]
		movaps		[ecx + eax*4 + 4*256 + 4*256], xmm3	// save  overlap[1][0]

		movaps		xmm3, xmm0							// l3, l2, l1, l0
		unpcklps	xmm0, xmm1							// r1, l1, r0, l0
		unpckhps	xmm3, xmm1							// r3, l3, r2, l2

		cvtps2pi	mm0, xmm0							// r0		l0
		shufps		xmm0, xmm0, 00001110b				// xx,xx	r1, l1
		cvtps2pi	mm2, xmm3							// r2		l2
		shufps		xmm3, xmm3, 00001110b				// xx,xx	r3, l3
		cvtps2pi	mm1, xmm0							// r1		l1
		cvtps2pi	mm3, xmm3							// r3		l3

		add			eax, 4
		packssdw	mm0, mm1							// r1, l1, r0 , l0
		packssdw	mm2, mm3							// r3, l3, r2 , l2

		movq		[edx + eax*4 + 0 - 4*4 + 4*256], mm0// save l0, r0, l1, r1
		movq		[edx + eax*4 + 8 - 4*4 + 4*256], mm2// save l2, r2, l3, r3

		jl			loop1
		emms
		}

#else
	int i;

	for(i=0; i<256; i++)
		{
		leftRightBuffer[2*i+0] =(short) ScaleToInt(chpostfft[leftChannelIndex][i] *  ac3WindowTable2[i] * lm + overlap[0][i], 1.0);
		leftRightBuffer[2*i+1] =(short) ScaleToInt(chpostfft[rightChannelIndex][i] *  ac3WindowTable2[i] * rm + overlap[1][i], 1.0);
		overlap[0][i] = chpostfft[leftChannelIndex][256+i] * ac3WindowTable2[256+i] * lm;
		overlap[1][i] = chpostfft[rightChannelIndex][256+i] * ac3WindowTable2[256+i] * rm;
		}

#endif
	}


void AC3Decoder::UncoupleChannel(int ch)
	{
	int bnd, sbnd, bin;
	fftreal temp;

	if (cplinu)
		{
		if (chincpl[ch])
			{
			for(sbnd = cplbegf; sbnd < 3 + cplendf; sbnd++)
				{
				for(bin = 0; bin < 12; bin++)
					{
					if (dithflag[ch] && cplbap[sbnd * 12 + bin + 37] == 0)
						{
						chcoeff[ch][sbnd * 12 + bin + 37] = GetDitheredMantissa(cplexps[sbnd * 12 + bin + 37]) * cplco[ch][sbnd] * 8;
						}
					else
						{
						chcoeff[ch][sbnd * 12 + bin + 37] = cplcoeff[sbnd * 12 + bin + 37] * cplco[ch][sbnd] * 8;
						}
					}
				}
			}
		}
	}

static void RematrixBand(int start, int end, fftreal * ch0, fftreal * ch1)
	{
	int i;
	fftreal a0, a1;

	for(i=start; i<end; i++)
		{
		a0 = ch0[i] + ch1[i];
		a1 = ch0[i] - ch1[i];
		ch0[i] = a0; ch1[i] = a1;
		}
	}



void AC3Decoder::RematrixChannels(void)
{
	int maxbnd;

	if (acmod == 0x02)
	{
		if (!cplinu)
			{
			maxbnd = min(endmant[0], endmant[1]);
			if (rematflg[0]) RematrixBand(13, min( 25, maxbnd), chcoeff[0],chcoeff[1]);
			if (rematflg[1]) RematrixBand(25, min( 37, maxbnd), chcoeff[0],chcoeff[1]);
			if (rematflg[2]) RematrixBand(37, min( 61, maxbnd), chcoeff[0],chcoeff[1]);
			if (rematflg[3]) RematrixBand(61, min(253, maxbnd), chcoeff[0],chcoeff[1]);
			}
		else if (cplbegf > 2)
			{
			if (rematflg[0]) RematrixBand(13,  25, chcoeff[0], chcoeff[1]);
			if (rematflg[1]) RematrixBand(25,  37, chcoeff[0], chcoeff[1]);
			if (rematflg[2]) RematrixBand(37,  61, chcoeff[0], chcoeff[1]);
			if (rematflg[3]) RematrixBand(61, 37 + cplbegf * 12, chcoeff[0], chcoeff[1]);
			}
		else if (cplbegf > 0)
			{
			if (rematflg[0]) RematrixBand(13,  25, chcoeff[0], chcoeff[1]);
			if (rematflg[1]) RematrixBand(25,  37, chcoeff[0], chcoeff[1]);
			if (rematflg[2]) RematrixBand(37, 37 + cplbegf * 12, chcoeff[0], chcoeff[1]);
			}
		else
			{
			if (rematflg[0]) RematrixBand(13,  25, chcoeff[0], chcoeff[1]);
			if (rematflg[1]) RematrixBand(25,  37, chcoeff[0], chcoeff[1]);
			}
		}
	}


void AC3Decoder::InitParseMantissa(void)
	{
	mant1Num = mant2Num = mant4Num = 0;
	}



#define		DITHMULT	47989

inline fftreal AC3Decoder::GetDitherValue(void)
	{
#if USE_DITHERING
	int rnd0;

	dithReg = (short)((DITHMULT * dithReg) & 0xffff);
	rnd0 = dithReg;

#if USE_TRIANGLE_DITHER
	dithReg = (short)((DITHMULT * dithReg) & 0xffff);
	rnd0 = ((rnd0 + dithReg)/2);
#endif

#if DUMP_FFT_DATA
	fprintf(dfile, "Dither %f\n", (double)((fftreal)rnd0 * (fftreal)M3DB/32768.0));
#endif

	return ((fftreal)(rnd0<<16) * (fftreal)M3DB);

#else
	return 0.;
#endif //USE_DITHERING
	}

inline fftreal AC3Decoder::GetDitheredMantissa(int exp)
	{
	return ShiftFP(GetDitherValue(), 31+exp);
	}


inline fftreal AC3Decoder::GetMantissa(int size, int exp, bool dither)
	{
	int val, bits;
	static int rnd0, rnd1, rnd2;

#pragma warning(disable : 4244 4305)
	static int bittab[]   = {0, 0, 0, 3,  0,  4,  5,  6,   7,   8,   9,   10,   11,   12,    14,    16};
	static fftreal valtab3[] = {-6./7., -4./7., -2./7., 0., 2./7., 4./7., 6./7.};
	static fftreal valtab5[] = {-14./15., -12./15., -10./15., -8./15., -6./15., -4./15., -2./15., 0.,
		                         2./15., 4./15., 6./15., 8./15., 10./15., 12./15., 14./15.};
	switch (size)
		{
		case 0:
			if (dither)
				return GetDitheredMantissa(exp);
			else
				return 0;
			break;
		case 1:
			if (!mant1Num)
				{
				val = strm->Get5Bits();
				mant1Val[0] = mant1lt[val][0];
				mant1Val[1] = mant1lt[val][1];
				mant1Val[2] = mant1lt[val][2];

				mant1Num = 2;

				return ShiftFP(mant1Val[0], exp);
				}
			else
				{
				mant1Num--;
				return ShiftFP(mant1Val[2 - mant1Num], exp);
				}
			break;
		case 2:
			if (!mant2Num)
				{
				val = strm->Get7Bits();
				mant2Val[0] = mant2lt[val][0];
				mant2Val[1] = mant2lt[val][1];
				mant2Val[2] = mant2lt[val][2];

				mant2Num = 2;

				return ShiftFP(mant2Val[0], exp);
				}
			else
				{
				mant2Num--;
				return ShiftFP(mant2Val[2 - mant2Num], exp);
				}
			break;
		case 3:
			val = strm->Get3Bits();
			return ShiftFP(valtab3[val], exp);
			break;
		case 4:
			if (!mant4Num)
				{
				val = strm->Get7Bits();
				mant4Val[0] = mant4lt[val][0];
				mant4Val[1] = mant4lt[val][1];

				mant4Num = 1;

				return ShiftFP(mant4Val[0], exp);
				}
			else
				{
				mant4Num = 0;

				return ShiftFP(mant4Val[1], exp);
				}
			break;
		case 5:
			val = strm->Get4Bits();
			return ShiftFP(valtab5[val], exp);
			break;
		default:
			bits = bittab[size];
			val = strm->GetHBits(bits);

			return ShiftFP((fftreal)val, 31+exp);
		}
	}

void AC3Decoder::GetMantissas(int num, fftreal * dst, BYTE * bap, BYTE * exps, bool dither)
	{
	int i;

#if DUMP_FFT_DATA
	fprintf(dfile, "At Get Mantissa %08x\n", (int)(strm->CurrentLocation()));
	for(i=0; i<num; i++)
		fprintf(dfile, "%3d : BA %x EX %d\n", i, bap[i], exps[i]);
	fprintf(dfile, "\n");
#endif

	for(i=0; i<num; i++)
		dst[i] = GetMantissa(bap[i], exps[i], dither);
	}



static inline int logadd(int a, int b)
	{
	static BYTE latab[] = {0x40, 0x3f, 0x3e, 0x3d, 0x3c, 0x3b, 0x3a, 0x39, 0x38, 0x37,
		                    0x36, 0x35, 0x34, 0x34, 0x33, 0x32, 0x31, 0x30, 0x2f, 0x2f,
								  0x2e, 0x2d, 0x2c, 0x2c, 0x2b, 0x2a, 0x29, 0x29, 0x28, 0x27,
								  0x26, 0x26, 0x25, 0x24, 0x24, 0x23, 0x23, 0x22, 0x21, 0x21,
								  0x20, 0x20, 0x1f, 0x1e, 0x1e, 0x1d, 0x1d, 0x1c, 0x1c, 0x1b,
								  0x1b, 0x1a, 0x1a, 0x19, 0x19, 0x18, 0x18, 0x17, 0x17, 0x16,
								  0x16, 0x15, 0x15, 0x15, 0x14, 0x14, 0x13, 0x13, 0x13, 0x12,
								  0x12, 0x12, 0x11, 0x11, 0x11, 0x10, 0x10, 0x10, 0x0f, 0x0f,
								  0x0f, 0x0e, 0x0e, 0x0e, 0x0d, 0x0d, 0x0d, 0x0d, 0x0c, 0x0c,
								  0x0c, 0x0c, 0x0b, 0x0b, 0x0b, 0x0b, 0x0a, 0x0a, 0x0a, 0x0a,
								  0x0a, 0x09, 0x09, 0x09, 0x09, 0x09, 0x08, 0x08, 0x08, 0x08,
								  0x08, 0x08, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x06, 0x06,
								  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x05, 0x05, 0x05, 0x05,
								  0x05, 0x05, 0x05, 0x05, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04,
								  0x04, 0x04, 0x04, 0x04, 0x04, 0x03, 0x03, 0x03, 0x03, 0x03,
								  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x02,
								  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
								  0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x01, 0x01,
								  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
								  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
								  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
								  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	int c;

	if (a >= b)
		{
		c = (a - b) >> 1;
		if (c > 255) c = 255;

		return a + latab[c];
		}
	else
		{
		c = (b - a) >> 1;
		if (c > 255) c = 255;

		return b + latab[c];
		}
	}

#define SUBS(x, y) ((x) > (y) ? (x) - (y) : 0)

static inline int calc_lowcomp(int a, int b0, int b1, int bin)
	{
	if (bin < 7)
		{
		if ((b0 + 256) == b1)
			a = 384;
		else if (b0 > b1)
			a = SUBS(a, 64);
		}
	else if (bin < 20)
		{
		if ((b0 + 256) == b1)
			a = 320;
		else if (b0 > b1)
			a = SUBS(a, 64);
		}
	else
		{
		a = SUBS(a, 128);
		}

	return a;
	}

void AC3Decoder::CalculateBitAllocation(
										int start,
										int end,
										int fgaincod,
										int fsnroffst,
										BYTE * exp,
										int fastleak,
										int slowleak,
										BYTE deltbae,
										WORD deltnseg,
										BYTE * deltoffst,
										BYTE * deltba,
										BYTE * deltlen,
										BYTE * bap,
										bool lfe)
	{
	static BYTE slowdec[] = {0x0f, 0x11, 0x13, 0x15};
	static BYTE fastdec[] = {0x3f, 0x53, 0x67, 0x7b};
	static WORD slowgain[] = {0x540, 0x4d8, 0x478, 0x410};
	static WORD dbpbtab[] = {0x000, 0x700, 0x900, 0xb00};
	static short floortab[] = {0x2f0, 0x2b0, 0x270, 0x230, 0x1f0, 0x170, 0x0f0, (short)0xf800};
	static WORD fastgain[] = {0x080, 0x100, 0x180, 0x200, 0x280, 0x300, 0x380, 0x400};
	static BYTE bndtab[] = {   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,
									  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,
									  20,  21,  22,  23,  24,  25,  26,  27,  28,  31,
									  34,  37,  40,  43,  46,  49,  55,  61,  67,  73,
									  79,  85,  97, 109, 121, 133, 157, 181, 205, 229, 255};
	static BYTE bndsz[]  = {   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
		                        1,   1,   1,   1,   1,   1,   1,   1,   1,   1,
										1,   1,   1,   1,   1,   1,   1,   1,   3,   3,
										3,   3,   3,   3,   3,   6,   6,   6,   6,   6,
										6,  12,  12,  12,  12,  24,  24,  24,  24,  24, 0};
	static BYTE masktab[] = {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
		                       10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
									  20, 21, 22, 23, 24, 25, 26, 27, 28, 28,
									  28, 29, 29, 29, 30, 30, 30, 31, 31, 31,
									  32, 32, 32, 33, 33, 33, 34, 34, 34, 35,
									  35, 35, 35, 35, 35, 36, 36, 36, 36, 36,
									  36, 37, 37, 37, 37, 37, 37, 38, 38, 38,
									  38, 38, 38, 39, 39, 39, 39, 39, 39, 40,
									  40, 40, 40, 40, 40, 41, 41, 41, 41, 41,
									  41, 41, 41, 41, 41, 41, 41, 42, 42, 42,
									  42, 42, 42, 42, 42, 42, 42, 42, 42, 43,
									  43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
									  43, 44, 44, 44, 44, 44, 44, 44, 44, 44,
									  44, 44, 44, 45, 45, 45, 45, 45, 45, 45,
									  45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
									  45, 45, 45, 45, 45, 45, 45, 46, 46, 46,
									  46, 46, 46, 46, 46, 46, 46, 46, 46, 46,
									  46, 46, 46, 46, 46, 46, 46, 46, 46, 46,
									  46, 47, 47, 47, 47, 47, 47, 47, 47, 47,
									  47, 47, 47, 47, 47, 47, 47, 47, 47, 47,
									  47, 47, 47, 47, 47, 48, 48, 48, 48, 48,
									  48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
									  48, 48, 48, 48, 48, 48, 48, 48, 48, 49,
									  49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
									  49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
									  49, 49, 49,  0,  0,  0};
	static WORD hth[3][50] = {{0x4d0, 0x4d0, 0x440, 0x400, 0x3e0, 0x3c0, 0x3b0, 0x3b0, 0x3a0, 0x3a0,
		                        0x3a0, 0x3a0, 0x3a0, 0x390, 0x390, 0x390, 0x380, 0x380, 0x370, 0x370,
										0x360, 0x360, 0x350, 0x350, 0x340, 0x340, 0x330, 0x320, 0x310, 0x300,
										0x2f0, 0x2f0, 0x2f0, 0x2f0, 0x300, 0x310, 0x340, 0x390, 0x3e0, 0x420,
										0x460, 0x490, 0x4a0, 0x460, 0x440, 0x440, 0x520, 0x800, 0x840, 0x840},
		                       {0x4f0, 0x4f0, 0x460, 0x410, 0x3e0, 0x3d0, 0x3c0, 0x3b0, 0x3b0, 0x3a0,
									   0x3a0, 0x3a0, 0x3a0, 0x3a0, 0x390, 0x390, 0x390, 0x380, 0x380, 0x380,
										0x370, 0x370, 0x360, 0x360, 0x350, 0x350, 0x340, 0x340, 0x320, 0x310,
										0x300, 0x2f0, 0x2f0, 0x2f0, 0x2f0, 0x300, 0x320, 0x350, 0x390, 0x3e0,
										0x420, 0x450, 0x4a0, 0x490, 0x460, 0x440, 0x480, 0x630, 0x840, 0x840},
		                       {0x580, 0x580, 0x4b0, 0x450, 0x420, 0x3f0, 0x3e0, 0x3d0, 0x3c0, 0x3b0,
									   0x3b0, 0x3b0, 0x3a0, 0x3a0, 0x3a0, 0x3a0, 0x3a0, 0x3a0, 0x3a0, 0x3a0,
										0x390, 0x390, 0x390, 0x390, 0x380, 0x380, 0x380, 0x370, 0x360, 0x350,
										0x340, 0x330, 0x320, 0x310, 0x300, 0x2f0, 0x2f0, 0x2f0, 0x300, 0x310,
										0x330, 0x350, 0x3c0, 0x410, 0x470, 0x4a0, 0x460, 0x440, 0x450, 0x4e0}};
	static BYTE baptab[] = { 0,  1,  1,  1,  1,  1,  2,  2,  3,  3,
		                      3,  4,  4,  5,  5,  6,  6,  6,  6,  7,
									 7,  7,  7,  8,  8,  8,  8,  9,  9,  9,
									 9, 10, 10, 10, 10, 11, 11, 11, 11, 12,
									12, 12, 12, 13, 13, 13, 13, 14, 14, 14,
									14, 14, 14, 14, 14, 15, 15, 15, 15, 15,
									15, 15, 15, 15};

	int sdecay = slowdec[sdcycod];
	int fdecay = fastdec[fdcycod];
	int sgain = slowgain[sgaincod];
	int dbknee = dbpbtab[dbpbcod];
	int floor = floortab[floorcod];

	int fgain = fastgain[fgaincod];
	int lowcomp = 0;

	int snroffset = (((csnroffst - 15) << 4) + fsnroffst) << 2;

	int psd[256], bndpsd[256], excite[256], mask[256];

	int bin, i, j, k, n, t;
	int bndstrt, bndend, begin;

	for(bin = start; bin < end; bin++)
		{
		psd[bin] = 3072 - (exp[bin] << 7);
		}

	j = start;
	k = masktab[start];
	do {
		bndpsd[k] = psd[j];
		j++;
		n = min(bndtab[k+1], end);
		for(i = j; i < n; i++)
			{
			bndpsd[k] = logadd(bndpsd[k], psd[j]);
			j++;
			}
		k++;
		} while (end > bndtab[k]);

	bndstrt = masktab[start];
	bndend = masktab[end-1] + 1;
	if (bndstrt == 0)
		{
		lowcomp = calc_lowcomp(lowcomp, bndpsd[0], bndpsd[1], 0);
		excite[0] = bndpsd[0] - fgain - lowcomp;
		lowcomp = calc_lowcomp(lowcomp, bndpsd[1], bndpsd[2], 1);
		excite[1] = bndpsd[1] - fgain - lowcomp;
		begin = 7;
		for(bin = 2; bin < 7; bin++)
			{
			if (bin < 6 || !lfe)
				lowcomp = calc_lowcomp(lowcomp, bndpsd[bin], bndpsd[bin+1], bin);
			fastleak = bndpsd[bin] - fgain;
			slowleak = bndpsd[bin] - sgain;
			excite[bin] = fastleak - lowcomp;
			if (bin < 6 && bndpsd[bin] <= bndpsd[bin+1])
				{
				begin = bin + 1;
				break;
				}
			}
		n = min(bndend, 22);
		for(bin = begin; bin < n; bin++)
			{
			if (bin < 6 || !lfe)
				lowcomp = calc_lowcomp(lowcomp, bndpsd[bin], bndpsd[bin+1], bin);
			fastleak -= fdecay;
			fastleak = max(fastleak, bndpsd[bin] - fgain);
			slowleak -= sdecay;
			slowleak = max(slowleak, bndpsd[bin] - sgain);
			excite[bin] = max(fastleak - lowcomp, slowleak);
			}
		begin = 22;
		}
	else
		{
		begin = bndstrt;
		}

	for(bin = begin; bin < bndend; bin++)
		{
		fastleak -= fdecay;
		fastleak = max(fastleak, bndpsd[bin] - fgain);
		slowleak -= sdecay;
		slowleak = max(slowleak, bndpsd[bin] - sgain);
		excite[bin] = max(fastleak, slowleak);
		}

	for(bin = bndstrt; bin < bndend; bin++)
		{
#if DUMP_FFT_DATA
		fprintf(dfile, "Excite %3d %04x\n", bin, excite[bin]);
#endif
		if (bndpsd[bin] < dbknee)
			{
			excite[bin] += (dbknee - bndpsd[bin]) >> 2;
			}
		mask[bin] = max(excite[bin], hth[fscod][bin]);
		}

	int delta, seg;
	WORD band;

	if (deltbae == 0 || deltbae == 1)
		{
		band = masktab[start];
		for(seg = 0; seg < deltnseg; seg++)
			{
			band += deltoffst[seg];
			if (deltba[seg] >= 4)
				delta = ((int)deltba[seg] - 3) << 7;
			else
				delta = ((int)deltba[seg] - 4) << 7;
			n = deltlen[seg];
#if DUMP_FFT_DATA
			fprintf(dfile, "Deltba Deltoffst %d Band %d Len %d Delta %d Deltba %d\n", deltoffst[seg], band, n, delta, deltba[seg]);
#endif
			for(k = 0; k < n; k++)
				{
#if DUMP_FFT_DATA
			fprintf(dfile, "at %d mask %04x", band, (int)(mask[band]));
#endif
				mask[band] += delta;
#if DUMP_FFT_DATA
			fprintf(dfile, " to %04x\n", (int)(mask[band]));
#endif
				band++;
				}
			}
		}

	int address;

	i = start;
	j = masktab[start];
	do {
		t = mask[j];
		t -= snroffset;
		t -= floor;
		if (t < 0) t = 0;
		t &= 0x1fe0;
		t += floor;

		if (nosnroffst) t = 0x1fe0;

#if DUMP_FFT_DATA
		fprintf(dfile, "Mask %3d %04x\n", j, t);
#endif
		n = bndtab[j] + bndsz[j];
		if (n > end) n = end;
		for(k= i; k < n; k++)
			{
			address = (psd[i] - t) >> 5;
			if (address < 0)
				bap[i] = baptab[0];
			else if (address > 63)
				bap[i] = baptab[63];
			else
				bap[i] = baptab[address];

			i++;
			}
		j++;
		} while (end > n);
	}

bool AC3Decoder::UnpackExponents(int ngrps, BYTE * gexp, BYTE expstr, BYTE absexp, BYTE * exp)
	{
	int grp, i, j, grpsize;
	BYTE expacc;
	BYTE prevexp, prevexp1, prevexp2;

	grpsize = 1 << (expstr - 1);
	prevexp = absexp;
	exp[0] = absexp;
	i = 0;
	for(grp = 0; grp < ngrps; grp++)
		{
		expacc = gexp[grp];
		prevexp1 = prevexp  + exp5lt[expacc][0];
		prevexp2 = prevexp1 + exp5lt[expacc][1];
		prevexp  = prevexp2 + exp5lt[expacc][2];

#if BITSTREAM_CHECK
		if ((prevexp1 | prevexp2 | prevexp) & ~31)
			{
#if DUMP_ERRORS
			OutputDebugString("UnpackExponent failed\n");
#endif
					//
			return FALSE;
			}
#endif

		for(j = 1; j <= grpsize; j++)
			{
			exp[i +               j] = prevexp1;
			exp[i +     grpsize + j] = prevexp2;
			exp[i + 2 * grpsize + j] = prevexp;
			}
		i += 3 * grpsize;
		}

	return TRUE;
	}

bool AC3Decoder::ParseSyncInfo(void)
	{
	int newFrameSize;

	static int frameSizes[4][40] = {

		{  64 * 16,   64 * 16,   80 * 16,   80 * 16,	  96 * 16,   96 * 16,  112 * 16,  112 * 16,
		  128 * 16,  128 * 16,  160 * 16,  160 * 16,	 192 * 16,  192 * 16,  224 * 16,  224 * 16,
		  256 * 16,  256 * 16,  320 * 16,  320 * 16,	 384 * 16,  384 * 16,  448 * 16,  448 * 16,
		  512 * 16,  512 * 16,  640 * 16,  640 * 16,  768 * 16,  768 * 16,  896 * 16,  896 * 16,
		 1024 * 16, 1024 * 16, 1152 * 16, 1152 * 16, 1280 * 16, 1280 * 16,         0,         0},

		{  69 * 16,   70 * 16,   87 * 16,   88 * 16,  104 * 16,  105 * 16,  121 * 16,  122 * 16,
		  139 * 16,  140 * 16,  174 * 16,  175 * 16,  208 * 16,  209 * 16,  243 * 16,  244 * 16,
		  278 * 16,  279 * 16,  348 * 16,  349 * 16,  417 * 16,  418 * 16,  487 * 16,  488 * 16,
		  557 * 16,  558 * 16,  696 * 16,  697 * 16,  835 * 16,  836 * 16,  975 * 16,  976 * 16,
       1114 * 16, 1115 * 16, 1253 * 16, 1254 * 16, 1393 * 16, 1394 * 16,         0,         0},

		{  96 * 16,   96 * 16,  120 * 16,  120 * 16,  144 * 16,  144 * 16,  168 * 16,  168 * 16,
		  192 * 16,  192 * 16,  240 * 16,  240 * 16,  288 * 16,  288 * 16,  336 * 16,  336 * 16,
		  384 * 16,  384 * 16,  480 * 16,  480 * 16,  576 * 16,  576 * 16,  672 * 16,  672 * 16,
		  768 * 16,  768 * 16,  960 * 16,  960 * 16, 1152 * 16, 1152 * 16, 1344 * 16, 1344 * 16,
       1536 * 16, 1536 * 16, 1728 * 16, 1728 * 16, 1920 * 16, 1920 * 16,         0,         0}

		};

	strm->ByteAllign();
	if (!strm->RefillBuffer(64 * 16)) return FALSE;

	while (strm->AvailBits() >= 64 * 16 && (syncword = (WORD)(strm->PeekBits(16))) != 0x0b77)
		{
		strm->Advance(8);
		if (!strm->RefillBuffer(64 * 16)) return FALSE;
		}

	frameStart = strm->CurrentLocation();

	if (syncword != 0x0b77)
		{
		strm->Advance(strm->AvailBits());

		return FALSE;
		}

	strm->Advance(16);

	crc1 = (WORD)(strm->GetBits(16));
#if DUMP_FFT_DATA
	fprintf(dfile, "CRC1 : %04x\n", crc1);
#endif
	fscod = (BYTE)(strm->GetBits(2));
	frmsizecod = (BYTE)(strm->GetBits(6));

	frmsizecod = min(39, frmsizecod);
	newFrameSize = frameSizes[fscod][frmsizecod];
	if (!newFrameSize) return FALSE;

	if (!strm->RefillBuffer(newFrameSize - 40)) return FALSE;

	if (newFrameSize - 40 <= strm->AvailBits())
		{
		frameSize = newFrameSize;

		return TRUE;
		}
	else
		{
		return FALSE;
		}
	}

bool AC3Decoder::ParseBSI(void)
	{
	static BYTE NFCHANS[] = {2, 1, 2, 3, 3, 4, 4, 5};

	bsid = (BYTE)(strm->GetBits(5));
	if (bsid > 8)
		return FALSE;
	bsmod = (BYTE)(strm->GetBits(3));
	acmod = (BYTE)(strm->GetBits(3));

	nfchans = NFCHANS[acmod];

	if ((acmod & 0x1) && (acmod != 0x1)) cmixlev = (BYTE)(strm->GetBits(2));
	if (acmod & 0x4) surmixlev = (BYTE)(strm->GetBits(2));
	if (acmod == 0x02) dsurmod = (BYTE)(strm->GetBits(2));

	lfeon = strm->GetBool();
	dialnorm = (BYTE)(strm->GetBits(5));
	if (dialnorm == 0) dialnorm = 31;
	compre = strm->GetBool();
	if (compre) compr = (BYTE)(strm->GetBits(8));
	langcode = strm->GetBool();
	if (langcode) langcod = (BYTE)(strm->GetBits(8));
	audprodie = strm->GetBool();
	if (audprodie)
		{
		mixlevel = (BYTE)(strm->GetBits(5));
		roomtyp = (BYTE)(strm->GetBits(2));
		}

	if (acmod == 0)
		{
		dialnorm2 = (BYTE)(strm->GetBits(5));
		if (dialnorm != dialnorm2)
			DP("dialnorm %d != dialnorm2 %d", dialnorm, dialnorm2);
		compr2e = strm->GetBool();
		if (compre != compr2e)
			DP("compre %d != compr2e %d", compre, compr2e);
		if (compr2e)
			{
			compr2 = (BYTE)(strm->GetBits(8));
			if (compr != compr2)
				DP("compr %d != compr2 %d", compr, compr2);
			}

		lngcod2e = strm->GetBool();
		if (lngcod2e) langcod2 = (BYTE)(strm->GetBits(8));
		audprodi2e = strm->GetBool();
		if (audprodi2e)
			{
			mixlevel2 = (BYTE)(strm->GetBits(5));
			roomtyp2 = (BYTE)(strm->GetBits(2));
			}
		}

	copyrightb = strm->GetBool();
	origbs = strm->GetBool();
	timecod1e = strm->GetBool();
	if (timecod1e) timecod1 = (WORD)(strm->GetBits(14));
	timecod2e = strm->GetBool();
	if (timecod2e) timecod2 = (WORD)(strm->GetBits(14));
	addbsie = strm->GetBool();
	if (addbsie)
		{
		addbsil = (BYTE)(strm->GetBits(6));
		strm->Advance((addbsil + 1) * 8);
		}

//	DP("bsmod %d, acmod %d, cmixlev %d, surmixlev %d", bsmod, acmod, cmixlev, surmixlev);
	surroundDownmix = surroundDownmixProfile;

	if (bsmod == 7 && acmod > 1 && karaoke != AC3KARA_OFF)
		{
		// karaoke mode && aware or capable
		// force karoke aware decoding (Lt/Rt)
		surroundDownmix =FALSE;

		switch (karaoke)
			{
			case AC3KARA_CAPABLE_BOTH_VOCAL:
				surmixlev = 0;
				break;

			case AC3KARA_CAPABLE_NO_VOCAL:
				surmixlev = 2;
				break;

			default:
				break;
			}

		}

	return TRUE;
	}

bool AC3Decoder::ParseAudblk(int blknum)
	{
#if DUMP_FFT_DATA
	int	i;
#endif
	int ch, bnd, grp, seg, bin, num;
	BYTE b;
	bool prevcplinu;
	BYTE prevcplbegf, prevcplendf;

#if DUMP_FFT_DATA
		fprintf(dfile, "At block switch %08x\n", (int)(strm->CurrentLocation()));
#endif

	for(ch = 0; ch < nfchans; ch++) blksw[ch]    = strm->GetBool();
	for(ch = 0; ch < nfchans; ch++) dithflag[ch] = strm->GetBool();

	dynrnge = strm->GetBool();
	if (dynrnge) dynrng = (BYTE)(strm->GetBits(8));

	if (acmod == 0)
		{
		dynrng2e = strm->GetBool();
		if (dynrng2e) dynrng2 = (BYTE)(strm->GetBits(8));
		}

	prevcplbegf = cplbegf;
	prevcplendf = cplendf;
	prevcplinu = cplinu;

	cplstre = strm->GetBool();
	if (cplstre)
		{
		cplinu = strm->GetBool();
#if DUMP_FFT_DATA
		fprintf(dfile, "CLPINU %d\n", cplinu);
#endif
		if (cplinu)
			{
			for(ch = 0; ch < nfchans; ch++) chincpl[ch] = strm->GetBool();
			if (acmod == 0x2) phsflginu = strm->GetBool();
			cplbegf = (BYTE)(strm->GetBits(4));
			cplendf = (BYTE)(strm->GetBits(4));

			ncplsubnd = 3 + cplendf - cplbegf;
			for(bnd = 1; bnd < ncplsubnd; bnd++) cplbndstrc[bnd] = strm->GetBool();
#if DUMP_FFT_DATA
			for(ch = 0; ch < nfchans; ch++)
				fprintf(dfile, "CHINCPL (%d) %d\n", ch, chincpl[ch]);
			fprintf(dfile, "CPLBEGF %d CPLENDF %d\n", cplbegf, cplendf);
			for(bnd = 1; bnd < ncplsubnd; bnd++)
				fprintf(dfile, "cplbndstrc (%d) %d\n", bnd, cplbndstrc[bnd]);
#endif
			}
		}
#if BITSTREAM_CHECK
	else if (!blknum)
		{
		// BITSTREAM ERROR :
		//
#if DUMP_ERRORS
		OutputDebugString("(blknum == 0) && (cplstre == 0)\n");
#endif
		//
		return FALSE;
		}
#endif

	if (cplinu)
		{
#if BITSTREAM_CHECK
		if (cplbegf > cplendf + 2)
			{
			// BITSTREAM ERROR :
			//
#if DUMP_ERRORS
			OutputDebugString("(cplinu == 1) && (cplbegf > cplendf + 2)\n");
#endif
			//
			return FALSE;
			}
#endif

		ncplbnd = 1;
		for(bnd = 1; bnd < ncplsubnd; bnd++) if (cplbndstrc[bnd] == 0) ncplbnd++;
		num = 0;

		for(ch = 0; ch < nfchans; ch++)
			{
			if (chincpl[ch])
				{
				num++;
				cplcoe[ch] = strm->GetBool();
#if DUMP_FFT_DATA
				fprintf(dfile, "cplcoe (%d) %d\n", ch, cplcoe[ch]);
#endif
				if (cplcoe[ch])
					{
					mstrcplco[ch] = (BYTE)(strm->GetBits(2));
#if DUMP_FFT_DATA
					fprintf(dfile, "mstrcplco (%d) %d\n", ch, mstrcplco[ch]);
#endif
					for(bnd = 0; bnd < ncplbnd; bnd++)
						{
						cplcoexp[ch][bnd] = (BYTE)(strm->GetBits(4));
						cplcomant[ch][bnd] = (BYTE)(strm->GetBits(4));
#if DUMP_FFT_DATA
						fprintf(dfile, "cplcoexp, mant (%d.%d) %x %x\n", ch, bnd, cplcoexp[ch][bnd], cplcomant[ch][bnd]);
#endif
						}

					int sbnd;
					fftreal temp;

					bnd = 0;
					for(sbnd = cplbegf; sbnd < 3 + cplendf; sbnd++)
						{
						if (cplcoexp[ch][bnd] == 15)
							temp = (fftreal)cplcomant[ch][bnd] * (fftreal)(1. / 16.);
						else
							temp = (fftreal)(cplcomant[ch][bnd] + 16) * (fftreal)(1. / 32);

						cplco[ch][sbnd] = temp / (1 << (cplcoexp[ch][bnd] + 3 * mstrcplco[ch]));

						if (sbnd < 2 + cplendf && !cplbndstrc[sbnd - cplbegf + 1]) bnd++;
						}
					}
#if BITSTREAM_CHECK
				else if (blknum == 0 || prevcplinu == 0)
					{
					// BITSTREAM ERROR :
					//
#if DUMP_ERRORS
					OutputDebugString("(cplinu == 1) &&\n");
					OutputDebugString("(blknum == 0 || prevcplinu == 0) &&\n");
					OutputDebugString("(chincpl[n] == 1) && (cplcoe[n] == 0)\n");
#endif
					//
					return FALSE;
					}
#endif
				}
			}
#if BITSTREAM_CHECK
		if (!num)
			{
			// BITSTREAM ERROR :
			//
#if DUMP_ERRORS
			OutputDebugString("(cplinu == 1) && (no channels in coupling)\n");
#endif
			//
			return FALSE;
			}
#endif
		if (acmod == 0x2 && phsflginu && (cplcoe[0] || cplcoe[1]))
			{
			for(bnd = 0; bnd < ncplbnd; bnd++)
				{
				phsflg[bnd] = strm->GetBool();
				}

			int sbnd;

			bnd = 0;
			for(sbnd = cplbegf; sbnd < 3 + cplendf; sbnd++)
				{
				if (phsflg[bnd])
				cplco[1][sbnd] = -cplco[1][sbnd];

				if (sbnd < 2 + cplendf && !cplbndstrc[sbnd - cplbegf + 1]) bnd++;
				}
			}
		}

	if (acmod == 0x2)
		{
		rematstr = strm->GetBool();
		if (rematstr)
			{
			if (cplbegf > 2 || cplinu == 0)
				{
				for(bnd = 0; bnd < 4; bnd++) rematflg[bnd] = strm->GetBool();
				}
			else if (cplbegf > 0 && cplbegf <= 2 && cplinu)
				{
				for(bnd = 0; bnd < 3; bnd++) rematflg[bnd] = strm->GetBool();
				}
			else if (cplbegf == 0 && cplinu)
				{
				for(bnd = 0; bnd < 2; bnd++) rematflg[bnd] = strm->GetBool();
				}
			}
#if BITSTREAM_CHECK
		else if (blknum == 0)
			{
			// BITSTREAM ERROR :
			//
#if DUMP_ERRORS
			OutputDebugString("(blknum == 0) && (acmod == 2) && (rematstr == 0)\n");
#endif
			//
			return FALSE;
			}
#endif
		}

	if (cplinu)
		{
		b = (BYTE)(strm->GetBits(2));
#if DUMP_FFT_DATA
		fprintf(dfile, "cplexps %d\n", b);
#endif
		newcplexps = b != 0;
		if (newcplexps)
			cplexpstr = b;
#if BITSTREAM_CHECK
		else if (blknum == 0 || prevcplinu == 0)
			{
			// BITSTREAM ERROR :
			//
#if DUMP_ERRORS
			OutputDebugString("(cplinu == 1) &&\n");
			OutputDebugString("(blknum == 0 || prevcplinu == 0) &&\n");
			OutputDebugString("(cplexpstr == 0)\n");
#endif
			//
			return FALSE;
			}
#endif
		}
	for(ch = 0; ch < nfchans; ch++)
		{
		b = (BYTE)(strm->GetBits(2));
#if DUMP_FFT_DATA
		fprintf(dfile, "CHEXPS %d\n", b);
#endif
		newchexps[ch] = b != 0;
		if (newchexps[ch]) chexpstr[ch] = b;
		}
	if (lfeon)
		{
		b = (BYTE)(strm->GetBit());
		newlfeexps = b != 0;
		if (newlfeexps) lfeexpstr = b;
		}
	for(ch = 0; ch < nfchans; ch++)
		{
		if (newchexps[ch] != 0)
			{
			if (!(cplinu && chincpl[ch]))
				{
				chbwcod[ch] = (BYTE)(strm->GetBits(6));
#if DUMP_FFT_DATA
				fprintf(dfile, "CHBWCOD (%d) %02x\n", ch, chbwcod[ch]);
#endif
				}
			}
#if BITSTREAM_CHECK
		if ((cplinu == 0 || chincpl[ch] == 0) && chbwcod[ch] > 60)
			{
			// BITSTREAM ERROR :
			//
#if DUMP_ERRORS
			OutputDebugString("(chincpl[n] == 0) && (chbwcod[n] > 60)\n");
#endif
			//
			return FALSE;
			}
#endif
		}

	if (cplinu)
		{
		cplstrtmant = (cplbegf * 12) + 37;
		cplendmant = (cplendf + 3) * 12 + 37;
		ncplgrps = (cplendmant - cplstrtmant) / (3 << (cplexpstr - 1));

		if (newcplexps)
			{
			cplabsexp = (BYTE)(strm->GetBits(4));
			for(grp = 0; grp < ncplgrps; grp++)
				{
				cplgexps[grp] = (BYTE)(strm->GetBits(7));
				if (cplgexps[grp] >= 125)
					{
#if BITSTREAM_CHECK
					// BITSTREAM ERROR :
					//
#if DUMP_ERRORS
					OutputDebugString("(cplgexps[grp] >= 125)\n");
#endif
					//
					return FALSE;
#endif
					}
				}
			}
#if BITSTREAM_CHECK
		else if (cplbegf != prevcplbegf || cplendf != prevcplendf)
			{
			// BITSTREAM ERROR :
			//
#if DUMP_ERRORS
			OutputDebugString("(cplinu == 1) &&\n");
			OutputDebugString("(cplbegf != previous cplbegf || cplendf != previous cplendf) &&\n");
			OutputDebugString("(cplexpstr == 0)\n");
#endif
			//
			return FALSE;
			}
#endif
		if (newcplexps || cplstre)
			{
			if (!UnpackExponents(ncplgrps, cplgexps, cplexpstr, cplabsexp << 1, cplexps + cplstrtmant - 1)) return FALSE;
			}
		}

	for(ch = 0; ch < nfchans; ch++)
		{
		strtmant[ch] = 0;
		endmant[ch] = (cplinu && chincpl[ch]) ? cplstrtmant : (chbwcod[ch] + 12) * 3 + 37;

		switch (chexpstr[ch])
			{
			case 1:
				nchgrps[ch] = (endmant[ch] - 1) / 3;
				break;
			case 2:
				nchgrps[ch] = (endmant[ch] - 1 + 3) / 6;
				break;
			case 3:
				nchgrps[ch] = (endmant[ch] - 1 + 9) / 12;
				break;
			}

		if (newchexps[ch])
			{
			gexps[ch][0] = (BYTE)(strm->GetBits(4));

			for(grp = 1; grp <= nchgrps[ch]; grp++) gexps[ch][grp] = (BYTE)(strm->GetBits(7));
			gainrng[ch] = (BYTE)(strm->GetBits(2));
#if DUMP_FFT_DATA
			for(grp = 0; grp <= nchgrps[ch]; grp++)
				fprintf(dfile, "CHGRPS (%d, %d) %02x\n", ch, grp, (int)(gexps[ch][grp]));
#endif
			}
#if BITSTREAM_CHECK
		else if (blknum == 0)
			{
			// BITSTREAM ERROR :
			//
			// (blknum == 0) && (chexpstr[n] == 0)
			//
#if DUMP_ERRORS
			OutputDebugString("(blknum == 0) && (chexpstr[n] == 0)\n");
#endif
			return FALSE;
			}
		else if (cplinu && cplbegf != prevcplbegf && chincpl[ch])
			{
			// BITSTREAM ERROR :
			//
			// (cplinu == 1) && (cplbegf != previous cplbegf) &&
			// (chincpl[n] == 1) && (chexpstr[n] == 0)
			//
#if DUMP_ERRORS
			OutputDebugString("(cplinu ==1 ) && (cplbegf != previous cplbegf) &&\n");
			OutputDebugString("(chincpl[n] ==1) && (chexpstr[n] == 0) &&\n");
#endif
			//
			return FALSE;
			}
#endif

		if (newchexps[ch] || cplstre)
			{
			if (!UnpackExponents(nchgrps[ch], gexps[ch] + 1, chexpstr[ch], gexps[ch][0], exps[ch])) return FALSE;
#if DUMP_FFT_DATA
			fprintf(dfile, "Unpack exponents %d\n", ch);
			for(i=0; i<255; i++)
				{
				fprintf(dfile, "%3d : %02x\n", i, exps[ch][i]);
				}
			fprintf(dfile, "\n");
#endif
			}
		}

	if (lfeon)
		{
		lfestrtmant = 0; lfeendmant = 7;
		nlfegrps = 2;
		if (newlfeexps)
			{
			lfegexps[0] = (BYTE)(strm->GetBits(4));
			for(grp = 1; grp <= nlfegrps; grp++) lfegexps[grp] = (BYTE)(strm->GetBits(7));
			if (!UnpackExponents(nlfegrps, lfegexps + 1, 1, lfegexps[0], lfeexps)) return FALSE;
			}
#if BITSTREAM_CHECK
		else if (blknum == 0)
			{
			// BITSTREAM ERROR :
			//
			// (blknum == 0) && (lfeon == 1) && (lfeexpstr == 0)
#if DUMP_ERRORS
			OutputDebugString("(blknum == 0) && (lfeon == 1) && (lfeexpstr == 0)\n");
#endif
			//
			return FALSE;
			}
#endif
		}

	baie = strm->GetBool();
	if (baie)
		{
		sdcycod = (BYTE)(strm->GetBits(2));
		fdcycod = (BYTE)(strm->GetBits(2));
		sgaincod = (BYTE)(strm->GetBits(2));
		dbpbcod = (BYTE)(strm->GetBits(2));
		floorcod = (BYTE)(strm->GetBits(3));
		}
#if BITSTREAM_CHECK
	else if (blknum == 0)
		{
		// BITSTREAM ERROR :
		//
		// (blknum == 0) && (baie == 0)
#if DUMP_ERRORS
			OutputDebugString("(blknum == 0) && (baie == 0)\n");
#endif
		//
		return FALSE;
		}
#endif

	snroffste = strm->GetBool();
	if (snroffste)
		{
		csnroffst = (BYTE)(strm->GetBits(6));
		nosnroffst = csnroffst == 0;

		if (cplinu)
			{
			cplfsnroffst = (BYTE)(strm->GetBits(4));
			cplfgaincod = (BYTE)(strm->GetBits(3));
			if (cplfsnroffst != 0) nosnroffst = false;
			}
		for(ch = 0; ch < nfchans; ch++)
			{
			fsnroffst[ch] = (BYTE)(strm->GetBits(4));
			fgaincod[ch] = (BYTE)(strm->GetBits(3));
			if (fsnroffst[ch] != 0) nosnroffst = false;
			}
		if (lfeon)
			{
			lfefsnroffst = (BYTE)(strm->GetBits(4));
			lfefgaincod = (BYTE)(strm->GetBits(3));
			if (lfefsnroffst != 0) nosnroffst = false;
			}
		}
#if BITSTREAM_CHECK
	else if (blknum == 0)
		{
		// BITSTREAM ERROR :
		//
		// (blknum == 0) && (snroffste == 0)
		//
#if DUMP_ERRORS
			OutputDebugString("(blknum == 0) && (snroffste == 0)\n");
#endif
		return FALSE;
		}
#endif

	if (cplinu)
		{
		cplleake = strm->GetBool();
		if (cplleake)
			{
			cplfleak = (BYTE)(strm->GetBits(3));
			cplsleak = (BYTE)(strm->GetBits(3));
			}
#if BITSTREAM_CHECK
		else if (blknum == 0)
			{
			// BITSTREAM ERROR :
			//
			// (blknum == 0) && (baie == 0)
#if DUMP_ERRORS
			OutputDebugString("(blknum == 0) && (baie == 0)\n");
#endif
			//
			return FALSE;
			}
#endif
		}

	deltbaie = strm->GetBool();
	if (deltbaie)
		{
		if (cplinu) cpldeltbae = (BYTE)(strm->GetBits(2));
		for(ch = 0; ch < nfchans; ch++) deltbae[ch] = (BYTE)(strm->GetBits(2));
		if (cplinu)
			{
			if (cpldeltbae == 1)
				{
				num = 0;
				cpldeltnseg = (BYTE)(strm->GetBits(3)) + 1;
				for(seg = 0; seg < cpldeltnseg; seg++)
					{
					num += cpldeltoffst[seg] = (BYTE)(strm->GetBits(5));
					num += cpldeltlen[seg] = (BYTE)(strm->GetBits(4));
					cpldeltba[seg] = (BYTE)(strm->GetBits(3));
					}
#if BITSTREAM_CHECK
				if (num > 50)
					{
					// BITSTREAM ERROR :
					//
#if DUMP_ERRORS
					OutputDebugString("(sum of cpldeltoffst > 50)\n");
#endif
					return FALSE;
					}
#endif
				}
			}
		for(ch = 0; ch < nfchans; ch++)
			{
			if (deltbae[ch] == 1)
				{
				num = 0;
				deltnseg[ch] = (BYTE)(strm->GetBits(3)) + 1;
				for(seg = 0; seg < deltnseg[ch]; seg++)
					{
					num += deltoffst[ch][seg] = (BYTE)(strm->GetBits(5));
					num += deltlen[ch][seg] = (BYTE)(strm->GetBits(4));
					deltba[ch][seg] = (BYTE)(strm->GetBits(3));
					}
#if BITSTREAM_CHECK
				if (num > 50)
					{
					// BITSTREAM ERROR :
					//
#if DUMP_ERRORS
					OutputDebugString("(sum of deltoffst > 50)\n");
#endif
					return FALSE;
					}
#endif
				}
			}
		}

	skiple = strm->GetBool();
	if (skiple)
		{
		skipl = (WORD)(strm->GetBits(9));
		strm->Advance(skipl * 8);
#if DUMP_FFT_DATA
		fprintf(dfile, "Skipl %d\n", skipl);
		fprintf(dfile, "Fixed data boundary %08x\n", (int)(strm->CurrentLocation()));
#endif
		}

	if (cplinu)
		{
		ncplmant = cplendmant - cplstrtmant;
		if (newcplexps || cplstre || cplleake || (deltbaie && cpldeltbae) || baie || snroffste)
			CalculateBitAllocation(cplstrtmant, cplendmant, cplfgaincod, cplfsnroffst, cplexps,
										  ((WORD)cplfleak << 8) + 768, ((WORD)cplsleak << 8) + 768,
										  cpldeltbae, cpldeltnseg, cpldeltoffst, cpldeltba, cpldeltlen, cplbap, FALSE);
		}
	for(ch = 0; ch < nfchans; ch++)
		{
		nchmant[ch] = endmant[ch] - strtmant[ch];
		if (newchexps[ch] || cplstre || (deltbaie && deltbae[ch]) || baie || snroffste)
			CalculateBitAllocation(strtmant[ch], endmant[ch], fgaincod[ch], fsnroffst[ch], exps[ch],
										  0, 0,
										  deltbae[ch], deltnseg[ch], deltoffst[ch], deltba[ch], deltlen[ch], chbap[ch], FALSE);
		}
	if (lfeon)
		{
		nlfemant = lfeendmant - lfestrtmant;
		if (newlfeexps || baie || snroffste)
			CalculateBitAllocation(lfestrtmant, lfeendmant, lfefgaincod, lfefsnroffst, lfeexps,
										  0, 0,
										  2, 0, NULL, NULL, NULL, lfebap, TRUE);
		}

	InitParseMantissa();

	ch = 0;
	do {
		GetMantissas(nchmant[ch], chcoeff[ch], chbap[ch], exps[ch], dithflag[ch]);
		ch++;
		} while (!(cplinu && chincpl[ch-1]) && ch < nfchans);
	if (cplinu)
		{
		GetMantissas(ncplmant, cplcoeff+cplstrtmant, cplbap+cplstrtmant, cplexps+cplstrtmant, FALSE);
		if (chincpl[ch-1]) UncoupleChannel(ch-1);

//		for(bin = cplstrtmant; bin < cplendmant; bin++) cplcoeff[bin] = ShiftFP(GetMantissa(cplbap[bin]), cplexps[bin]);
		}
	while (ch < nfchans)
		{
		GetMantissas(nchmant[ch], chcoeff[ch], chbap[ch], exps[ch], dithflag[ch]);
		if (cplinu && chincpl[ch])
			UncoupleChannel(ch);
		ch++;
		}
	if (lfeon)
		{
		GetMantissas(nlfemant, lfecoeff, lfebap, lfeexps, FALSE);
		}

	for (ch = 0; ch < nfchans; ch ++)
		{
		if (cplinu && chincpl[ch])
			{
			for(bin = cplendmant; bin < 256; bin++) chcoeff[ch][bin] = 0.0;
			}
		else
			{
			for(bin = endmant[ch]; bin < 256; bin++) chcoeff[ch][bin] = 0.0;
			}
		}

	return TRUE;
	}

static inline void IFFT(complex * x, complex * y, int n, complex * fm)
	{
	int m, i, j, r, nm, is, n2, n4, n6, n8;
	fftreal wr, wi;
	fftreal sr, si;
	fftreal yr0, yi0, yr1, yi1, yr2, yi2, yr3, yi3;

	n2 = n >> 1;
	n4 = n >> 2;
	n8 = n >> 3;
	n6 = n2 + n4;

	j = 0;
	for(i=0; i<n; i+=4)
		{
		yr0 = x[j+0].r + x[j+n2].r;
		yi0 = x[j+0].i + x[j+n2].i;
		yr1 = x[j+0].r - x[j+n2].r;
		yi1 = x[j+0].i - x[j+n2].i;
		yr2 = x[j+n4].r + x[j+n6].r;
		yi2 = x[j+n4].i + x[j+n6].i;
		yr3 = x[j+n4].r - x[j+n6].r;
		yi3 = x[j+n4].i - x[j+n6].i;

		y[i+0].r = yr0 + yr2;
		y[i+0].i = yi0 + yi2;
		y[i+1].r = yr1 - yi3;
		y[i+1].i = yi1 + yr3;
		y[i+2].r = yr0 - yr2;
		y[i+2].i = yi0 - yi2;
		y[i+3].r = yr1 + yi3;
		y[i+3].i = yi1 - yr3;

		m = n8;
		while (m && (j & m))
			{
			j -= m;
			m >>= 1;
			}
		j += m;
		}

	nm = 8;
	r = 16;
	while (n > nm)
		{
		m = 0;
		is = nm >> 1;
		for(i=0; i <is; i++)
			{
			wr = cossintab64[m].r; wi = cossintab64[m].i;
			m += r;

			for(j=i; j<n; j+=nm)
				{
				sr = y[j+is].r * wr - y[j+is].i * wi;
				si = y[j+is].r * wi + y[j+is].i * wr;
				y[j+is].r = y[j].r - sr;
				y[j+is].i = y[j].i - si;
				y[j].r += sr;
				y[j].i += si;
				}
			}
		nm <<= 1;
		r >>= 1;
		}

	m = 0;
	is = nm >> 1;
	for(i=0; i <is; i++)
		{
		wr = cossintab64[m].r; wi = cossintab64[m].i;
		m += r;

		sr = y[i+is].r * wr - y[i+is].i * wi;
		si = y[i+is].r * wi + y[i+is].i * wr;

		yr0 = y[i].r - sr;
		yi0 = y[i].i - si;
		yr1 = y[i].r + sr;
		yi1 = y[i].i + si;

		y[i+is].r = yi0 * fm[i+is].i - yr0 * fm[i+is].r;
		y[i+is].i = yi0 * fm[i+is].r + yr0 * fm[i+is].i;

		y[i   ].r = yi1 * fm[i   ].i - yr1 * fm[i   ].r;
		y[i   ].i = yi1 * fm[i   ].r + yr1 * fm[i   ].i;
		}

	}


#define abs(x) ((x) > 0 ? (x) : -(x))

static inline void InverseFFT512(fftreal * x, fftreal * y)
	{
	static const int n = 512;


	complex z[n/4];
	complex zz[n/4];

	int k, l;

	for(k=0; k<n/4; k++)
		{
		l = 2 * k;

		z[k].r = x[n/2-l-1] * xcossin512[k].r - x[    l  ] * xcossin512[k].i;
		z[k].i = x[    l  ] * xcossin512[k].r + x[n/2-l-1] * xcossin512[k].i;
		}

	IFFT(z, zz, n / 4, xcossin512);


	for(l=0; l<n/8; l++)
		{
		k = l * 2;

		// tzr = zz[l].r * xcossin512[l].r - zz[l].i * xcossin512[l].i;
		// tzi = zz[l].i * xcossin512[l].r + zz[l].r * xcossin512[l].i;

		y[    n/4+k  ] =  zz[l].r;;
		y[    n/4-k-1] = -zz[l].r;;
		y[n/2+n/4+k  ] =  zz[l].i;
		y[n/2+n/4-k-1] =  zz[l].i;

		// tzr = zz[n/8+l].r * xcossin512[n/8+l].r - zz[n/8+l].i * xcossin512[n/8+l].i;
		// tzi = zz[n/8+l].i * xcossin512[n/8+l].r + zz[n/8+l].r * xcossin512[n/8+l].i;

		y[    k  ] = -zz[n/8+l].i;
		y[n/2-k-1] =  zz[n/8+l].i;
		y[n/2+k  ] =  zz[n/8+l].r;
		y[n  -k-1] =  zz[n/8+l].r;
		}


	}

static inline void InverseFFT256(fftreal * x, fftreal * y)
	{

	static const int n = 512;

	int k, l;

	complex z[2][n/8];
	complex zz[2][n/8];
	for(k=0; k<n/8; k++)
		{
		l = 2 * k;

		z[0][k].r = x[2*(n/4-l-1)]   * xcossin256[k].r - x[2*(    l  )]   * xcossin256[k].i;
		z[0][k].i = x[2*(    l  )]   * xcossin256[k].r + x[2*(n/4-l-1)]   * xcossin256[k].i;

		z[1][k].r = x[2*(n/4-l-1)+1] * xcossin256[k].r - x[2*(    l  )+1] * xcossin256[k].i;
		z[1][k].i = x[2*(    l  )+1] * xcossin256[k].r + x[2*(n/4-l-1)+1] * xcossin256[k].i;
		}
	IFFT(z[0], zz[0], n / 8, xcossin256);
	IFFT(z[1], zz[1], n / 8, xcossin256);
	for(l=0; l<n/8; l++)
		{
		k = l * 2;

		y[k]         = -zz[0][l].i;
		y[k+1]       = -zz[0][n/8-l-1].r;
		y[n/4+k]     =  zz[0][l].r;
		y[n/4+k+1]   =  zz[0][n/8-l-1].i;
		y[n/2+k]     =  zz[1][l].r;
		y[n/2+k+1]   =  zz[1][n/8-l-1].i;
		y[3*n/4+k]   =  zz[1][l].i;
		y[3*n/4+k+1] =  zz[1][n/8-l-1].r;

		}
	}

void AC3Decoder::TransformChannels(void)
	{
	int ch;
	for(ch = 0; ch < nfchans; ch++)
		{
		if (!channelPremixed[ch])
			{
			if (blksw[ch])
				InverseFFT256(chcoeff[ch], chpostfft[ch]);
			else
				InverseFFT512(chcoeff[ch], chpostfft[ch]);
			}
		}
	if (lfeon && enableLFE)
		{
		InverseFFT512(lfecoeff, lfepostfft);
		}
	}

void AC3Decoder::MuteAudioBuffer(void)
	{
	int i;

	for(i=0; i<256 * 6; i++)
		{
		leftRightBuffer[2*i+0] = lastLeft * (256 * 6 - i) / (256 * 6);
		leftRightBuffer[2*i+1] = lastRight * (255  * 6 - i) / (256 * 6);
		}
	for(i=0; i<256; i++)
		{
		overlap[0][i] = 0;
		overlap[1][i] = 0;
		}
	}

void AC3Decoder::UnmuteAudioBuffer(void)
	{
	int i;

	lastLeft = leftRightBuffer[2 * 256 * 6 - 2];
	lastRight = leftRightBuffer[2 * 256 * 6 - 1];

	for(i=0; i<256 * 6; i++)
		{
		leftRightBuffer[2*i+0] = lastLeft * i / (256 * 6);
		leftRightBuffer[2*i+1] = lastRight * i / (256 * 6);
		}
	}


bool AC3Decoder::CheckCRCAndFrameAC3DigitalOutput(void)
	{
	static WORD crctab[256] =
	{	0x0000, 0x8005, 0x800f, 0x000a, 0x801b, 0x001e, 0x0014, 0x8011,
		0x8033, 0x0036, 0x003c, 0x8039, 0x0028, 0x802d, 0x8027, 0x0022,
		0x8063, 0x0066, 0x006c, 0x8069, 0x0078, 0x807d, 0x8077, 0x0072,
		0x0050, 0x8055, 0x805f, 0x005a, 0x804b, 0x004e, 0x0044, 0x8041,
		0x80c3, 0x00c6, 0x00cc, 0x80c9, 0x00d8, 0x80dd, 0x80d7, 0x00d2,
		0x00f0, 0x80f5, 0x80ff, 0x00fa, 0x80eb, 0x00ee, 0x00e4, 0x80e1,
		0x00a0, 0x80a5, 0x80af, 0x00aa, 0x80bb, 0x00be, 0x00b4, 0x80b1,
		0x8093, 0x0096, 0x009c, 0x8099, 0x0088, 0x808d, 0x8087, 0x0082,
		0x8183, 0x0186, 0x018c, 0x8189, 0x0198, 0x819d, 0x8197, 0x0192,
		0x01b0, 0x81b5, 0x81bf, 0x01ba, 0x81ab, 0x01ae, 0x01a4, 0x81a1,
		0x01e0, 0x81e5, 0x81ef, 0x01ea, 0x81fb, 0x01fe, 0x01f4, 0x81f1,
		0x81d3, 0x01d6, 0x01dc, 0x81d9, 0x01c8, 0x81cd, 0x81c7, 0x01c2,
		0x0140, 0x8145, 0x814f, 0x014a, 0x815b, 0x015e, 0x0154, 0x8151,
		0x8173, 0x0176, 0x017c, 0x8179, 0x0168, 0x816d, 0x8167, 0x0162,
		0x8123, 0x0126, 0x012c, 0x8129, 0x0138, 0x813d, 0x8137, 0x0132,
		0x0110, 0x8115, 0x811f, 0x011a, 0x810b, 0x010e, 0x0104, 0x8101,
		0x8303, 0x0306, 0x030c, 0x8309, 0x0318, 0x831d, 0x8317, 0x0312,
		0x0330, 0x8335, 0x833f, 0x033a, 0x832b, 0x032e, 0x0324, 0x8321,
		0x0360, 0x8365, 0x836f, 0x036a, 0x837b, 0x037e, 0x0374, 0x8371,
		0x8353, 0x0356, 0x035c, 0x8359, 0x0348, 0x834d, 0x8347, 0x0342,
		0x03c0, 0x83c5, 0x83cf, 0x03ca, 0x83db, 0x03de, 0x03d4, 0x83d1,
		0x83f3, 0x03f6, 0x03fc, 0x83f9, 0x03e8, 0x83ed, 0x83e7, 0x03e2,
		0x83a3, 0x03a6, 0x03ac, 0x83a9, 0x03b8, 0x83bd, 0x83b7, 0x03b2,
		0x0390, 0x8395, 0x839f, 0x039a, 0x838b, 0x038e, 0x0384, 0x8381,
		0x0280, 0x8285, 0x828f, 0x028a, 0x829b, 0x029e, 0x0294, 0x8291,
		0x82b3, 0x02b6, 0x02bc, 0x82b9, 0x02a8, 0x82ad, 0x82a7, 0x02a2,
		0x82e3, 0x02e6, 0x02ec, 0x82e9, 0x02f8, 0x82fd, 0x82f7, 0x02f2,
		0x02d0, 0x82d5, 0x82df, 0x02da, 0x82cb, 0x02ce, 0x02c4, 0x82c1,
		0x8243, 0x0246, 0x024c, 0x8249, 0x0258, 0x825d, 0x8257, 0x0252,
		0x0270, 0x8275, 0x827f, 0x027a, 0x826b, 0x026e, 0x0264, 0x8261,
		0x0220, 0x8225, 0x822f, 0x022a, 0x823b, 0x023e, 0x0234, 0x8231,
		0x8213, 0x0216, 0x021c, 0x8219, 0x0208, 0x820d, 0x8207, 0x0202 };


	WORD w;
	int i;
	__int64 loc;
	BYTE * dptr = (BYTE *)&encodedBuffer;

	loc = strm->CurrentLocation();
	strm->SetCurrentLocation(frameStart);

	*dptr++ = 0x72;
	*dptr++ = 0xf8;
	*dptr++ = 0x1f;
	*dptr++ = 0x4e;
	*dptr++ = 0x01;
	*dptr++ = 0x00;
	*dptr++ = frameSize & 0xff;
	*dptr++ = frameSize >> 8;

	WORD crcVerify1 = 0;
	WORD crcVerify2 = 0;
	int crcLength = (frameSize >> 5) + (frameSize >> 7);

	// sync word
	w = (WORD)(strm->GetBits(16));
	*dptr++ = w & 0xff;
	*dptr++ = w >> 8;

	// first 5/8
	for (i=1; i<crcLength; i++)
		{
		w = (WORD)(strm->GetBits(16));
		*dptr++ = w & 0xff;
		*dptr++ = w >> 8;

		crcVerify1 = (WORD)(((crcVerify1 << 8) & 0xff00) ^ (BYTE)( w >> 8 ) ^ crctab[(crcVerify1 >> 8) & 0xff]);
		crcVerify1 = (WORD)(((crcVerify1 << 8) & 0xff00) ^ (BYTE)(w & 0xff) ^ crctab[(crcVerify1 >> 8) & 0xff]);
		}

	// last 3/8
	for (; i<frameSize/16; i++)
		{
		w = (WORD)(strm->GetBits(16));
		*dptr++ = w & 0xff;
		*dptr++ = w >> 8;

		crcVerify2 = (WORD)(((crcVerify2 << 8) & 0xff00) ^ (BYTE)( w >> 8 ) ^ crctab[(crcVerify2 >> 8) & 0xff]);
		crcVerify2 = (WORD)(((crcVerify2 << 8) & 0xff00) ^ (BYTE)(w & 0xff) ^ crctab[(crcVerify2 >> 8) & 0xff]);
		}

	i = 6144 - 8 - (frameSize>>3);
	ZeroMemory(dptr, i);
	strm->SetCurrentLocation(loc);

	if(crcVerify1 || crcVerify2)
		{
		DP("CRC error in first 5/8s of frame crc val 0x%04x\n", crcVerify1);
		DP("CRC error in remaining 3/8s of frame crc val 0x%04x\n", crcVerify2);
		return false;
		}

	return true;
	}



void AC3Decoder::ParseSyncFrame(void)
	{
	bool frameOk;
	int blk, ch, time;
	__int64 frameEnd;

	do {
		time = strm->GetCurrentTimeStamp();

		if (!ParseSyncInfo()) return;

		if (!strm->RefillBuffer(frameSize + 64)) return;

		frameEnd = frameStart + frameSize;

		frameOk = PeekWordAt((int)(frameEnd - strm->CurrentLocation())) == 0x0b77;

		if (frameOk)
			{
			frameOk = CheckCRCAndFrameAC3DigitalOutput();
			}
		else
			{
			DP("Next AC3 sync frame not found frameStart %d frameEnd %d",frameStart, frameEnd);
			}

		} while (!frameOk);


	if (time != INVALID_TIME_STAMP && time != lastTime)
		{
		lastTime = time;
		currentDataTime = time;
		time -= pcmWaveOut->GetBufferDelayTime();
		if (time >= 0 && timingSlave)
			{
			timingSlave->UpdateTimer(time);
			}
		}

	if (lastfscod != fscod)
		{
		switch (fscod)
			{
			default:
				DP("ERROR: fscod reversed ???");
			case 0:
				sampleRate = 48000;
				break;
			case 1:
				sampleRate = 44100;
				break;
			case 2:
				sampleRate = 32000;
				break;
			}
		// does nothing on Xbox
		pcmWaveOut->SetSampleRate(sampleRate);
		lastfscod = fscod;
		}


#if DUMP_FFT_DATA
	fprintf(dfile, "FRAME AT %08lx to %08lx\n", (int)frameStart / 8, (int)frameEnd / 8);
#endif

	dynrng = 0;
	compr = 0;

	if (ParseBSI())
		{
		cpldeltbae = 2;
		cpldeltnseg = 0;
		for(ch = 0; ch < nfchans; ch++)
			{
			deltbae[ch] = 2;
			deltnseg[ch] = 0;
			}

		outNum = 0;

		for(blk = 0; blk < 6; blk++)
			{
#if DUMP_FFT_DATA
			fprintf(dfile, "BLOCK NO %d \n\n", blk);
#endif


			if (ParseAudblk(blk))
				{
#if DUMP_COMPRESSION
					{
					char buffer[100];
					int time = (blk + 6 * FrameCnt) * 16 / 3;
					wsprintf(buffer, "At %02d:%02d:%02d,%04d ",
						time / 3600000, time / 60000 % 60, time / 1000 % 60, time % 1000);
					OutputDebugString(buffer);
					}
#endif
				RematrixChannels();
				PreDownmixChannels();
				DynamicRangeCompression();
				TransformChannels();
				PostDownmixChannels();
				WindowChannels(leftRightBuffer+outNum);
				outNum += 512;

#if DUMP_COMPRESSION
					{
					int i;
					int max = 0, min = 0;
					for(i=0; i<512; i++)
						{
						if (leftRightBuffer[512*blk+i] > max) max = leftRightBuffer[512*blk+i];
						if (leftRightBuffer[512*blk+i] < min) min = leftRightBuffer[512*blk+i];
						}
					if (-min > max) max = -min;

					char buffer[100];
					wsprintf(buffer, "%5d\n", max);
					OutputDebugString(buffer);
					}
#endif

				}
			else
				{
#if DUMP_ERRORS
					{
					char buffer[100];
					wsprintf(buffer, "AC3 error in Frame %d Block %d At %08lx\n", FrameCnt, blk, (int)(frameStart / 8));
					OutputDebugString(buffer);
					}
#endif
				recoverPhase = 8;

				break;
				}

			if (!hurryUp) YieldTimedFiber(0);
			}

		}
	else
		{
		recoverPhase = 8;
		}

	if (strm->CurrentLocation() > frameEnd)
		{
		recoverPhase = 8;
		}

	if (recoverPhase > 1)
		{
		MuteAudioBuffer();
		recoverPhase--;
		}
	else if (recoverPhase == 1)
		{
		UnmuteAudioBuffer();
		recoverPhase--;
		}

#if DUMP_ERRORS
		{
		int i;

		for(i=2; i<256*6*2-2; i++)
			{
			if (leftRightBuffer[i] == -32768)
				{
				char buffer[100];
				wsprintf(buffer, "Missed AC3 error in Frame %d Block %d At %08lx (%08lx)\n", FrameCnt, i / 512, (int)(frameStart / 8), (int)((strm->CurrentLocation() / 8)));
				OutputDebugString(buffer);
				}
			}
		}
#endif

	if (ac3DigitalOutput)
		{
		if (zeroBlocks)
			{
			ZeroMemory(encodedBuffer, sizeof(encodedBuffer));
			zeroBlocks--;
			}
		pcmWaveOut->AddPCMDigData(leftRightBuffer, encodedBuffer, outNum>>1, currentDataTime);
		}
	else
		{
		pcmWaveOut->AddPCMData(leftRightBuffer, outNum>>1, currentDataTime);
		}
	currentDataTime += (1000 * (outNum>>1)) / sampleRate;
	lastLeft = leftRightBuffer[outNum - 2];
	lastRight = leftRightBuffer[outNum - 1];

	if (!hurryUp) YieldTimedFiber(0);


	strm->Advance((int)(frameEnd - strm->CurrentLocation()));

	FrameCnt++;
	}

void AC3Decoder::SkipSyncFrame(void)
	{
	if (!strm->RefillBuffer()) return;

	ParseSyncInfo();
	ParseBSI();
	}

AC3Decoder::~AC3Decoder(void)
	{
#if DUMP_FFT_DATA
	fclose(dfile);
#endif
	}

void AC3Decoder::InitLocalsNewStream(void)
	{
	StartParse();
	hurryUp = FALSE;
	FrameCnt = 0;
	recoverPhase = 0;

	lastLeft = 0;
	lastRight = 0;
	lastTime = -1;
	zeroBlocks = 0;
	}

void AC3Decoder::BeginStreaming(void)
	{
	pcmWaveOut->BeginStreaming();
	InitLocalsNewStream();
	}

void AC3Decoder::StartStreaming(int playbackSpeed)
	{
	pcmWaveOut->StartStreaming(playbackSpeed);
	}

void AC3Decoder::StopStreaming(void)
	{
	pcmWaveOut->StopStreaming();
	}

void AC3Decoder::EndStreaming(bool fullReset)
	{
	pcmWaveOut->EndStreaming(fullReset);
	}

void AC3Decoder::DetachStreaming(void)
	{
	pcmWaveOut->DetachStreaming();
	}

void AC3Decoder::AttachStreaming(void)
	{
	pcmWaveOut->AttachStreaming();
	InitLocalsNewStream();

	// audio trash avoidance; zero the first "few" blocks so they don't get decoded as pcm
	// this number is dependent on the AudioOuputDevice delay in setting spdifDatamode
	zeroBlocks = AC3_BLOCK_TO_ZERO_ON_SWITCH;
	}


void AC3Decoder::SetDigitalOut(AudioTypeConfig ac3AudioType)
	{
	if (pcmWaveOut->SupportsSPDIFDataOut())
		{
		ac3DigitalOutput = ac3AudioType.config.spdifOutMode == SPDIFOM_DEFAULT;
		if(ac3DigitalOutput)
			{
			zeroBlocks = AC3_BLOCK_TO_ZERO_ON_SWITCH;
			}
		}
	else
		{
		ac3DigitalOutput = false;
		}
	}

void AC3Decoder::GetDigitalOut(AudioTypeConfig & ac3AudioType)
	{
	ac3AudioType.config.spdifOutMode = ac3DigitalOutput ? SPDIFOM_DEFAULT : SPDIFOM_DECOMPRESSED;
	}

void AC3Decoder::SetAudioOutConfig(AC3SpeakerConfig spkCfg, AC3DualModeConfig dualModeCfg, AC3KaraokeConfig karaokeCfg)
	{
	// Use Lo/Ro for mono and stereo
	surroundDownmixProfile = (spkCfg == AC3SC_20_SURROUND_COMPATIBLE) ? TRUE : FALSE;
	dualMode = dualModeCfg;
	karaoke = karaokeCfg;
	}

void AC3Decoder::SetDolbyTestMode(AC3OperationalMode op, WORD hf, WORD lf, AC3DialogNorm dn)
	{
	hf = hf > 10000 ? 10000 : hf;
	lf = lf > 10000 ? 10000 : lf;

	ldr = (int)lf * 256/10000;
	hdr = (int)hf * 256/10000;

	useDialNorm = (dn == AC3DNORM_ON);

	switch (op)
		{
		default:
			DP("SetDolbyTestMode compression mode Unknown %d", op);
		case AC3OPMODE_LINE:
			compressionMode = COMP_MODE_LINE;
			if (ldr == 256 && hdr == 256)
				drc = AC3DYNRG_COMPRESSED;
			else if (ldr == 0 && hdr == 0)
				drc = AC3DYNRG_MAXIMUM;
			else
				drc = (AC3DynamicRange)-1;
			break;

		case AC3OPMODE_RF:
			compressionMode = COMP_MODE_RF;
			if (ldr == 256 && hdr == 256)
				drc = AC3DYNRG_TV;
			else
				drc = (AC3DynamicRange)-1;
			break;

		case AC3OPMODE_CUSTOM_0:
			compressionMode = COMP_MODE_CUSTOM_A;
			useDialNorm = FALSE;
			break;

		case AC3OPMODE_CUSTOM_1:
			compressionMode = COMP_MODE_CUSTOM_B;
			drc = (AC3DynamicRange)-1;
			useDialNorm = TRUE;
			break;
		}

	if (!useDialNorm)
		drc = (AC3DynamicRange)-1;

	}


void AC3Decoder::GetDolbyTestMode(AC3OperationalMode &op, WORD &hf, WORD &lf, AC3DialogNorm &dn)
	{
	hf = (WORD)(hdr * 10000/256);
	lf = (WORD)(ldr * 10000/256);

	dn = (useDialNorm ? AC3DNORM_ON: AC3DNORM_OFF);

	switch (compressionMode)
		{
		default:
			DP("GetDolbyTestMode compression mode Unknown %d", compressionMode);
		case COMP_MODE_LINE:
			op = AC3OPMODE_LINE;
			break;

		case COMP_MODE_RF:
			op = AC3OPMODE_RF;
			break;

		case COMP_MODE_CUSTOM_A:
			op = AC3OPMODE_CUSTOM_0;
			break;

		case COMP_MODE_CUSTOM_B:
			op = AC3OPMODE_CUSTOM_1;
			break;
		}
	}


void AC3Decoder::SetAC3Config(AC3DynamicRange dr, BOOL enableLFE)
	{
	this->enableLFE = enableLFE == TRUE;
	drc = dr;
	useDialNorm = TRUE;

	switch (dr)
		{
		default:
			DP("SetAC3Config AC3DynamicRange Unknown %d", dr);
			drc = AC3DYNRG_COMPRESSED ;
		case AC3DYNRG_COMPRESSED:
			compressionMode = COMP_MODE_LINE;
			ldr = 256;
			hdr = 256;
			break;

		case AC3DYNRG_MAXIMUM:
			compressionMode = COMP_MODE_LINE;
			ldr = 0;
			hdr = 0;
			break;

		// the difference is drc is checked in RF mode
		case AC3DYNRG_MINIMUM:
		case AC3DYNRG_TV:
			compressionMode = COMP_MODE_RF;
			ldr = 256;
			hdr = 256;
			break;
		}


	}


void AC3Decoder::GetAC3Config(AC3DynamicRange &dr, BOOL &enableLFE)
	{
	enableLFE = (BOOL)this->enableLFE;
	dr = drc;
	}
