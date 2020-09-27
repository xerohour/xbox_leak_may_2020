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

#include "MPEGAudioDecoder.h"
#include "MPEGAudioTables.h"

#include <math.h>
#define MEASURE_FILTER_TIME	0

static const double pi =  3.14159265359;

bool MPEGAudioDecoder::ParseHeader(void)
	{
	static const int LayerIBitrate[] = {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448};
	static const int LayerIIBitrate[] = {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384};
	static const int SamplingFrequency[] = {44100, 48000, 32000, 0};

	DWORD w;

	do	{
		if (!strm->RefillBuffer(4000)) return FALSE;

		w = strm->PeekBits(16);
		syncword = (WORD)XTBF(4, 12, w);
		id = XTBF(3, w);
		layer = (BYTE)XTBF(1, 2, w);
		protection = (BYTE)XTBF(0, 1, w);

		while (syncword != 0xfff || id == 0 || layer == 0)
			{
			strm->Advance(8);

			w = strm->PeekBits(16);
			syncword = (WORD)XTBF(4, 12, w);
			id = XTBF(3, w);
			layer = (BYTE)XTBF(1, 2, w);
			protection = (BYTE)XTBF(0, 1, w);
			}

		frameStart = strm->CurrentLocation();

		strm->Advance(16);

		bitrate_index = (BYTE)strm->GetBits(4);
		sampling_frequency = (BYTE)strm->GetBits(2);
		strm->Advance(1);
		private_bit = strm->GetBit();
		mode = (BYTE)strm->GetBits(2);
		mode_extension = (BYTE)strm->GetBits(2);
		copyright = strm->GetBit();
		original = strm->GetBit();
		emphasis = (BYTE)strm->GetBits(2);

		} while (id == 0 || layer == 0 ||
			      bitrate_index == 15 || bitrate_index == 0 ||
					sampling_frequency == 3 || emphasis == 2);

	switch (layer)
		{
		case 3: // Layer I
			frameSize = 384 * LayerIBitrate[bitrate_index] * 1000 / SamplingFrequency[sampling_frequency];
			break;
		case 2: // Layer II
			frameSize = 1152 * LayerIIBitrate[bitrate_index] * 1000 / SamplingFrequency[sampling_frequency];
			break;
		}

	pcmWaveOut->SetSampleRate(SamplingFrequency[sampling_frequency]);

	return TRUE;
	}

void MPEGAudioDecoder::ParseErrorCheck(void)
	{
	if (!protection)
		crc = (WORD)(strm->GetBits(16));
	}

void MPEGAudioDecoder::ParseAudioData(void)
	{
#pragma warning(disable : 4244 4305)
	static const float CT[] = {1.33333333333 / 2, 1.60000000000 / 4, 1.14285714286 / 4, 1.77777777777 / 8,
		                        1.06666666666 / 8, 1.03225806452 / 16, 1.01587301587 / 32, 1.00787401575 / 64,
								      1.00392156863 / 128, 1.00195694716 / 256, 1.00097751711 / 512, 1.00048851979 / 1024,
								      1.00024420024 / 2048, 1.00012208522 / 4096, 1.00006103888 / 8192, 1.00003051851 / 16384,
								      1.00001525902 / 32768};
#pragma warning(default : 4244 4305)

	static const int DT[] = {-1, -2, -3, -4, -7, -15, -31, -63, -127, -255, -511, -1023,
								    -2047, -4095, -8191, -16383, -32767};


	int nblut;
	int channel_bitrate;
	int bap;
	int ch, sb, gr, bound;
	unsigned int group;
	float m, m0, m1;


	switch (layer)
		{
		case 3: // Layer I
			break;
		case 2: // Layer II
			switch (mode)
				{
				case 3: // single chanel
					channel_bitrate = bitrate_index;
					nchannels = 1;
					nblut = LayerIIBalLut[sampling_frequency][channel_bitrate];
					sblimit = (BYTE)LayerIISBLimit[nblut];
					bound = sblimit;
					break;
				case 0:
				case 2:
					channel_bitrate = LayerIIHalfBitrate[bitrate_index];
					nchannels = 2;
					nblut = LayerIIBalLut[sampling_frequency][channel_bitrate];
					sblimit = (BYTE)LayerIISBLimit[nblut];
					bound = sblimit;
					break;
				case 1:
					channel_bitrate = LayerIIHalfBitrate[bitrate_index];
					nchannels = 2;
					nblut = LayerIIBalLut[sampling_frequency][channel_bitrate];
					sblimit = (BYTE)LayerIISBLimit[nblut];
					bound = 4 * mode_extension + 4;
					break;
				}

			for(sb = 0; sb < bound; sb++)
				{
				for(ch = 0; ch < nchannels; ch++)
					{
					baps[ch][sb] = (BYTE)LayerIIBapTab[nblut][sb][strm->GetBits(LayerIINBalTab[nblut][sb])];
					}
				}
			for(sb = bound; sb < sblimit; sb++)
				{
				baps[1][sb] = baps[0][sb] = (BYTE)LayerIIBapTab[nblut][sb][strm->GetBits(LayerIINBalTab[nblut][sb])];
				}

			for(sb = 0; sb < sblimit; sb++)
				{
				for(ch = 0; ch < nchannels; ch++)
					{
					if (baps[ch][sb]) scfsi[ch][sb] = (BYTE)(strm->GetBits(2));
					}
				}

			for(sb = 0; sb < sblimit; sb++)
				{
				for(ch = 0; ch < nchannels; ch++)
					{
					if (baps[ch][sb])
						{
						switch (scfsi[ch][sb])
							{
							case 0:
								scalefactor[ch][sb][0] = (BYTE)(strm->GetBits(6));
								scalefactor[ch][sb][1] = (BYTE)(strm->GetBits(6));
								scalefactor[ch][sb][2] = (BYTE)(strm->GetBits(6));
								break;
							case 1:
								scalefactor[ch][sb][0] =
								scalefactor[ch][sb][1] = (BYTE)(strm->GetBits(6));
								scalefactor[ch][sb][2] = (BYTE)(strm->GetBits(6));
								break;
							case 2:
								scalefactor[ch][sb][0] =
								scalefactor[ch][sb][1] =
								scalefactor[ch][sb][2] = (BYTE)(strm->GetBits(6));
								break;
							case 3:
								scalefactor[ch][sb][0] = (BYTE)(strm->GetBits(6));
								scalefactor[ch][sb][1] =
								scalefactor[ch][sb][2] = (BYTE)(strm->GetBits(6));
								break;
							}
						}
					else
						{
						sample[ch][0][sb] = 0;
						sample[ch][1][sb] = 0;
						sample[ch][2][sb] = 0;
						}
					}
				}

			for(sb = sblimit; sb < 32; sb++)
				{
				for(ch = 0; ch < nchannels; ch++)
					{
					sample[ch][0][sb] = 0;
					sample[ch][1][sb] = 0;
					sample[ch][2][sb] = 0;
					}
				}

			for(gr = 0; gr < 12; gr++)
				{
				for(sb = 0; sb < bound; sb++)
					{
					for(ch = 0; ch < nchannels; ch++)
						{
						if (baps[ch][sb])
							{
							bap = baps[ch][sb];
							m = CT[bap-1] * LayerIIScaleFactors[scalefactor[ch][sb][gr >> 2]];

							switch (bap)
								{
								case 1:
									group = strm->GetBits(5);
									sample[ch][0][sb] = (float)((int)(group     % 3) + DT[0]) * m;
									sample[ch][1][sb] = (float)((int)(group / 3 % 3) + DT[0]) * m;
									sample[ch][2][sb] = (float)((int)(group / 9    ) + DT[0]) * m;
									break;
								case 2:
									group = strm->GetBits(7);
									sample[ch][0][sb] = (float)((int)(group      % 5) + DT[1]) * m;
									sample[ch][1][sb] = (float)((int)(group /  5 % 5) + DT[1]) * m;
									sample[ch][2][sb] = (float)((int)(group / 25    ) + DT[1]) * m;
									break;
								case 3:
									group = strm->GetBits(9);
									sample[ch][0][sb] = (float)((int)( group >> 6     ) + DT[2]) * m;
									sample[ch][1][sb] = (float)((int)((group >> 3) & 7) + DT[2]) * m;
									sample[ch][2][sb] = (float)((int)( group       & 7) + DT[2]) * m;
									break;
								case 4:
									group = strm->GetBits(10);
									sample[ch][0][sb] = (float)((int)(group      % 9) + DT[3]) * m;
									sample[ch][1][sb] = (float)((int)(group /  9 % 9) + DT[3]) * m;
									sample[ch][2][sb] = (float)((int)(group / 81    ) + DT[3]) * m;
									break;
								case 5:
									group = strm->GetBits(12);
									sample[ch][0][sb] = (float)((int)( group >> 8      ) + DT[4]) * m;
									sample[ch][1][sb] = (float)((int)((group >> 4) & 15) + DT[4]) * m;
									sample[ch][2][sb] = (float)((int)( group       & 15) + DT[4]) * m;
									break;
								case 6:
									group = strm->GetBits(15);
									sample[ch][0][sb] = (float)((int)( group >> 10      ) + DT[5]) * m;
									sample[ch][1][sb] = (float)((int)((group >>  5) & 31) + DT[5]) * m;
									sample[ch][2][sb] = (float)((int)( group        & 31) + DT[5]) * m;
									break;
								case 7:
									group = strm->GetBits(18);
									sample[ch][0][sb] = (float)((int)( group >> 12      ) + DT[6]) * m;
									sample[ch][1][sb] = (float)((int)((group >>  6) & 63) + DT[6]) * m;
									sample[ch][2][sb] = (float)((int)( group        & 63) + DT[6]) * m;
									break;
								default:
									sample[ch][0][sb] = (float)((int)(strm->GetBits(bap-1)) + DT[bap-1]) * m;
									sample[ch][1][sb] = (float)((int)(strm->GetBits(bap-1)) + DT[bap-1]) * m;
									sample[ch][2][sb] = (float)((int)(strm->GetBits(bap-1)) + DT[bap-1]) * m;
									break;
								}
							}
						}
					}
				for(sb = bound; sb < sblimit; sb++)
					{
					if (baps[0][sb])
						{
						bap = baps[0][sb];
						m0 = CT[bap-1] * LayerIIScaleFactors[scalefactor[0][sb][gr >> 2]];
						m1 = CT[bap-1] * LayerIIScaleFactors[scalefactor[1][sb][gr >> 2]];

						switch (bap)
							{
							case 1:
								group = strm->GetBits(5);
								m = (float)((int)(group     % 3) + DT[0]);
								sample[0][0][sb] = m * m0; sample[1][0][sb] = m * m1;
								m = (float)((int)(group / 3 % 3) + DT[0]);
								sample[0][1][sb] = m * m0; sample[1][1][sb] = m * m1;
								m = (float)((int)(group / 9    ) + DT[0]);
								sample[0][2][sb] = m * m0; sample[1][2][sb] = m * m1;
								break;
							case 2:
								group = strm->GetBits(7);
								m = (float)((int)(group      % 5) + DT[1]);
								sample[0][0][sb] = m * m0; sample[1][0][sb] = m * m1;
								m = (float)((int)(group /  5 % 5) + DT[1]);
								sample[0][1][sb] = m * m0; sample[1][1][sb] = m * m1;
								m = (float)((int)(group / 25    ) + DT[1]);
								sample[0][2][sb] = m * m0; sample[1][2][sb] = m * m1;
								break;
							case 3:
								group = strm->GetBits(9);
								m = (float)((int)( group >> 6     ) + DT[2]);
								sample[0][0][sb] = m * m0; sample[1][0][sb] = m * m1;
								m = (float)((int)((group >> 3) & 7) + DT[2]);
								sample[0][1][sb] = m * m0; sample[1][1][sb] = m * m1;
								m = (float)((int)( group       & 7) + DT[2]);
								sample[0][2][sb] = m * m0; sample[1][2][sb] = m * m1;
								break;
							case 4:
								group = strm->GetBits(10);
								m = (float)((int)(group      % 9) + DT[3]);
								sample[0][0][sb] = m * m0; sample[1][0][sb] = m * m1;
								m = (float)((int)(group /  9 % 9) + DT[3]);
								sample[0][1][sb] = m * m0; sample[1][1][sb] = m * m1;
								m = (float)((int)(group / 81    ) + DT[3]);
								sample[0][2][sb] = m * m0; sample[1][2][sb] = m * m1;
								break;
							case 5:
								group = strm->GetBits(12);
								m = (float)((int)( group >> 8      ) + DT[4]);
								sample[0][0][sb] = m * m0; sample[1][0][sb] = m * m1;
								m = (float)((int)((group >> 4) & 15) + DT[4]);
								sample[0][1][sb] = m * m0; sample[1][1][sb] = m * m1;
								m = (float)((int)( group       & 15) + DT[4]);
								sample[0][2][sb] = m * m0; sample[1][2][sb] = m * m1;
								break;
							case 6:
								group = strm->GetBits(15);
								m = (float)((int)( group >> 10      ) + DT[5]);
								sample[0][0][sb] = m * m0; sample[1][0][sb] = m * m1;
								m = (float)((int)((group >>  5) & 31) + DT[5]);
								sample[0][1][sb] = m * m0; sample[1][1][sb] = m * m1;
								m = (float)((int)( group        & 31) + DT[5]);
								sample[0][2][sb] = m * m0; sample[1][2][sb] = m * m1;
								break;
							case 7:
								group = strm->GetBits(18);
								m = (float)((int)( group >> 12      ) + DT[6]);
								sample[0][0][sb] = m * m0; sample[1][0][sb] = m * m1;
								m = (float)((int)((group >>  6) & 63) + DT[6]);
								sample[0][1][sb] = m * m0; sample[1][1][sb] = m * m1;
								m = (float)((int)( group        & 63) + DT[6]);
								sample[0][2][sb] = m * m0; sample[1][2][sb] = m * m1;
								break;
							default:
								m = (float)((int)(strm->GetBits(bap-1)) + DT[bap-1]);
								sample[0][0][sb] = m * m0; sample[1][0][sb] = m * m1;
								m = (float)((int)(strm->GetBits(bap-1)) + DT[bap-1]);
								sample[0][1][sb] = m * m0; sample[1][1][sb] = m * m1;
								m = (float)((int)(strm->GetBits(bap-1)) + DT[bap-1]);
								sample[0][2][sb] = m * m0; sample[1][2][sb] = m * m1;
								break;
							}
						}
					}
				SynthesisFilter();
				if (!hurryUp) YieldTimedFiber(0);
				}
			break;
		case 1: // Layer III
			break;
		}
	}

void MPEGAudioDecoder::ParseAncillaryData(void)
	{
	int skip;

	skip = (int)(strm->CurrentLocation() - frameStart);
	if (skip < (frameSize & ~7)) strm->Advance((frameSize & ~7) - skip);

	strm->ByteAllign();
	while (strm->RefillBuffer(4000) && strm->PeekBits(12) != 0xfff)
		{
		strm->Advance(8);
		}
	}

static inline int FF2I(float val)
	{
	int tmp;

	__asm
		{
		fld	[val]
		fistp	[tmp]
		}

	return tmp;
	}

static inline int FF2I2(float m1, float n1, float m2, float n2)
	{
	int tmp;

	__asm
		{
		fld		[m1]
		fmul		[n1]
		fld		[m2]
		fmul		[n2]
		faddp		st(1), st
		fistp		[tmp]
		}

	return tmp;
	}

void MPEGAudioDecoder::SynthesisFilter(void)
	{
	float s, t;
	int i, j, k, gr, ch;
	short samples[3][32][2], msum[2];
	float dif[16], sum[16];

	int tt, ss;

#if MEASURE_FILTER_TIME
	static __int64 lockSumTime;
	static int lockCount;
	__int64 t1, t2;

	ReadPerformanceCounter(t1);
#endif

	for(gr = 0; gr < 3; gr++)
		{
		filterBase = (filterBase - 64) & 1023;

		for(ch = 0; ch < nchannels; ch++)
			{
			for(i = 0; i < 16; i++)
				{
				sum[i] = sample[ch][gr][i] + sample[ch][gr][31-i];
				dif[i] = sample[ch][gr][i] - sample[ch][gr][31-i];
				}

			for(i = 0; i < 16; i+=2)
				{
				s = t = 0;
				for(k = 0; k < 16; k++)
					{
					t += nik2[(17 + i) * (2 * k + 1) & 127] * dif[k];
					s += nik2[(49 + i) * (2 * k + 1) & 127] * dif[k];
					}

				tt = FF2I(t); ss = FF2I(s);

				ivbuffer[ch][filterBase + i+1] = (short)tt;
				ivbuffer[ch][filterBase + 31-i] = -tt;
				ivbuffer[ch][filterBase + 33+i] = (short)ss;
				ivbuffer[ch][filterBase + 63-i] = (short)ss;
				}

			for(i = 0; i < 8; i++)
				{
				dif[i] = sum[i] - sum[15-i];
				sum[i] = sum[i] + sum[15-i];
				}

			for(i = 0; i < 16; i+=4)
				{
				s = t = 0;
				for(k = 0; k < 8; k++)
					{
					s += nik2[(18 + i) * (2 * k + 1) & 127] * dif[k];
					t += nik2[(50 + i) * (2 * k + 1) & 127] * dif[k];
					}

				tt = FF2I(t); ss = FF2I(s);

				ivbuffer[ch][filterBase + i+2] = (short)ss;
				ivbuffer[ch][filterBase + 30-i] = -ss;
				ivbuffer[ch][filterBase + i+34] = (short)tt;
				ivbuffer[ch][filterBase + 62-i] = (short)tt;
				}

			dif[0] = sum[0] - sum[7]; dif[1] = sum[1] - sum[6];
			dif[2] = sum[2] - sum[5]; dif[3] = sum[3] - sum[4];
			sum[0] = sum[0] + sum[7]; sum[1] = sum[1] + sum[6];
			sum[2] = sum[2] + sum[5]; sum[3] = sum[3] + sum[4];

			s = nik2[20 * (2 * 0 + 1) & 127] * dif[0] + nik2[20 * (2 * 1 + 1) & 127] * dif[1] +
				 nik2[20 * (2 * 2 + 1) & 127] * dif[2] + nik2[20 * (2 * 3 + 1) & 127] * dif[3];
			t = nik2[52 * (2 * 0 + 1) & 127] * dif[0] + nik2[52 * (2 * 1 + 1) & 127] * dif[1] +
				 nik2[52 * (2 * 2 + 1) & 127] * dif[2] + nik2[52 * (2 * 3 + 1) & 127] * dif[3];

			tt = FF2I(t); ss = FF2I(s);

			ivbuffer[ch][filterBase + 4] = (short)ss;
			ivbuffer[ch][filterBase + 28] = -ss;
			ivbuffer[ch][filterBase + 36] = (short)tt;
			ivbuffer[ch][filterBase + 60] = (short)tt;

			s = nik2[28 * (2 * 0 + 1) & 127] * dif[0] + nik2[28 * (2 * 1 + 1) & 127] * dif[1] +
				 nik2[28 * (2 * 2 + 1) & 127] * dif[2] + nik2[28 * (2 * 3 + 1) & 127] * dif[3];
			t = nik2[60 * (2 * 0 + 1) & 127] * dif[0] + nik2[60 * (2 * 1 + 1) & 127] * dif[1] +
				 nik2[60 * (2 * 2 + 1) & 127] * dif[2] + nik2[60 * (2 * 3 + 1) & 127] * dif[3];

			tt = FF2I(t); ss = FF2I(s);

			ivbuffer[ch][filterBase + 12] = (short)ss;
			ivbuffer[ch][filterBase + 20] = -ss;
			ivbuffer[ch][filterBase + 44] = (short)tt;
			ivbuffer[ch][filterBase + 52] = (short)tt;

			dif[0] = sum[0] - sum[3]; dif[1] = sum[1] - sum[2];
			sum[0] = sum[0] + sum[3]; sum[1] = sum[1] + sum[2];

//			ss = FF2I2(nik2[24 * (2 * 0 + 1) & 127], dif[0], nik2[24 * (2 * 1 + 1) & 127], dif[1]);
//			tt = FF2I2(nik2[56 * (2 * 0 + 1) & 127], dif[0], nik2[56 * (2 * 1 + 1) & 127], dif[1]);

			s = nik2[24 * (2 * 0 + 1) & 127] * dif[0] + nik2[24 * (2 * 1 + 1) & 127] * dif[1];
			t = nik2[56 * (2 * 0 + 1) & 127] * dif[0] + nik2[56 * (2 * 1 + 1) & 127] * dif[1];

			tt = FF2I(t); ss = FF2I(s);

			ivbuffer[ch][filterBase +  8] =  (short)ss;
			ivbuffer[ch][filterBase + 24] = -ss;
			ivbuffer[ch][filterBase + 40] =  (short)tt;
			ivbuffer[ch][filterBase + 56] =  (short)tt;

			ss = FF2I((sum[0] - sum[1]) * nik2[(16 + 0) & 127]);
			tt = FF2I((sum[0] + sum[1]) * nik2[(16 + 48) & 127]);

			ivbuffer[ch][filterBase +  0] = (short)ss;
			ivbuffer[ch][filterBase + 16] = 0;
			ivbuffer[ch][filterBase + 32] = -ss;
			ivbuffer[ch][filterBase + 48] = (short)tt;


//			ivbuffer[ch][filterBase +  0] = nik2[(16 +  0) & 127] * sum[0] + nik2[(16 +  0) * 3 & 127] * sum[1];
//			ivbuffer[ch][filterBase + 16] = nik2[(16 + 16) & 127] * sum[0] + nik2[(16 + 16) * 3 & 127] * sum[1];
//			ivbuffer[ch][filterBase + 32] = nik2[(16 + 32) & 127] * sum[0] + nik2[(16 + 32) * 3 & 127] * sum[1];
//			ivbuffer[ch][filterBase + 48] = nik2[(16 + 48) & 127] * sum[0] + nik2[(16 + 48) * 3 & 127] * sum[1];

			short * p = ivbuffer[ch];
			static MMXDWORD<2> synFilterRoundVal = {0x00200000, 0x00200000};

			for(j = 0; j < 32; j+=2)
				{
				__asm
					{
					mov			esi, [this]

//					pxor			mm0, mm0
					movq			mm0, [synFilterRoundVal]

					mov			edx, [j]
					shl			edx, 5
					lea			edx, [esi + edx].shortWindowConsts

					mov			eax, [esi].filterBase
					add			eax, [j]
					mov			edi, [p]
					lea			ebx, [eax + 96]
					and			ebx, 1023

//					mov			ecx, 8
//loop1:
					// offset 0
					movd			mm2, [edi + eax*2]
					movd			mm3, [edi + ebx*2]
					movq			mm1, [edx]
					punpcklwd	mm2, mm3
					pmaddwd		mm1, mm2
					paddd			mm0, mm1

					add			eax, 128 * 7
					add			ebx, 128 * 7
					and			eax, 1023
					and			ebx, 1023

					// offset 7
					movd			mm2, [edi + eax*2]
					movd			mm3, [edi + ebx*2]
					movq			mm1, [edx + 7 * 8]
					punpcklwd	mm2, mm3
					pmaddwd		mm1, mm2
					paddd			mm0, mm1

					sub			eax, 128
					sub			ebx, 128
					and			eax, 1023
					and			ebx, 1023

					psrad			mm0, 4

					// offset 6
					movd			mm2, [edi + eax*2]
					movd			mm3, [edi + ebx*2]
					movq			mm1, [edx + 6 * 8]
					punpcklwd	mm2, mm3
					pmaddwd		mm1, mm2
					paddd			mm0, mm1

					sub			eax, 128 * 5
					sub			ebx, 128 * 5
					and			eax, 1023
					and			ebx, 1023

					// offset 1
					movd			mm2, [edi + eax*2]
					movd			mm3, [edi + ebx*2]
					movq			mm1, [edx + 1 * 8]
					punpcklwd	mm2, mm3
					pmaddwd		mm1, mm2
					paddd			mm0, mm1

					add			eax, 128
					add			ebx, 128
					and			eax, 1023
					and			ebx, 1023

					psrad			mm0, 2

					// offset 2
					movd			mm2, [edi + eax*2]
					movd			mm3, [edi + ebx*2]
					movq			mm1, [edx + 2 * 8]
					punpcklwd	mm2, mm3
					pmaddwd		mm1, mm2
					paddd			mm0, mm1

					add			eax, 128 * 3
					add			ebx, 128 * 3
					and			eax, 1023
					and			ebx, 1023

					// offset 5
					movd			mm2, [edi + eax*2]
					movd			mm3, [edi + ebx*2]
					movq			mm1, [edx + 5 * 8]
					punpcklwd	mm2, mm3
					pmaddwd		mm1, mm2
					paddd			mm0, mm1

					sub			eax, 128
					sub			ebx, 128
					and			eax, 1023
					and			ebx, 1023

					psrad			mm0, 3

					// offset 4
					movd			mm2, [edi + eax*2]
					movd			mm3, [edi + ebx*2]
					movq			mm1, [edx + 4 * 8]
					punpcklwd	mm2, mm3
					pmaddwd		mm1, mm2
					paddd			mm0, mm1

					sub			eax, 128
					sub			ebx, 128
					and			eax, 1023
					and			ebx, 1023

					// offset 3
					movd			mm2, [edi + eax*2]
					movd			mm3, [edi + ebx*2]
					movq			mm1, [edx + 3 * 8]
					punpcklwd	mm2, mm3
					pmaddwd		mm1, mm2
					paddd			mm0, mm1


//					add			edx, 8
//					sub			ecx, 1
//					jne			loop1

					psrad			mm0, 13
					packssdw		mm0, mm1

					movd			[msum], mm0

					}

				samples[gr][j][ch] = msum[0];
				samples[gr][j+1][ch] = msum[1];

				}

			__asm {emms}
			}

		for(ch = nchannels; ch < 2; ch ++)
			{
			for(j=0; j<32; j++)
				samples[gr][j][ch] = samples[gr][j][0];
			}
		}

		if (mode == 2)
			{
			// dual channel mode
			short * sp, * dp;

			switch (dualMode)
				{
				case AC3DMDM_CHANNEL1:
					sp = &samples[0][0][0];
					dp = &samples[0][0][1];
					for(j=0; j<32*3; j++)
						{
						*dp = *sp;
						dp += 2;
						sp += 2;
						}
					break;
				case AC3DMDM_CHANNEL2:
					sp = &samples[0][0][1];
					dp = &samples[0][0][0];
					for(j=0; j<32*3; j++)
						{
						*dp = *sp;
						dp += 2;
						sp += 2;
						}
					break;
				case AC3DMDM_MIX:
					sp = &samples[0][0][0];
					dp = &samples[0][0][1];
					for(j=0; j<32*3; j++)
						{
						*dp = (*dp + *sp)/2;
						*sp = *dp;
						dp += 2;
						sp += 2;
						}
					break;
				default:
				case AC3DMDM_STEREO:
					break;
				}
			}

#if MEASURE_FILTER_TIME
		ReadPerformanceCounter(t2);

		lockSumTime += t2 - t1;
		lockCount++;

		if (!(lockCount & 255))
			{
			char buffer[100];
			wsprintf(buffer, "Filter %d : %d\n", lockCount, (int)(lockSumTime / lockCount));
			OutputDebugString(buffer);
			}
#endif

	pcmWaveOut->AddPCMData(samples[0][0], 32 * 3, lastTime);
	}

void MPEGAudioDecoder::ParseFrame(void)
	{

	time = strm->GetCurrentTimeStamp();

	if (!ParseHeader()) return;

	if (!strm->RefillBuffer(frameSize + 4)) return;

	if (strm->AvailBits() >= frameSize - 32)
		{
		if (time != INVALID_TIME_STAMP && time != lastTime)
			{
			lastTime = time;
			time -= pcmWaveOut->GetBufferDelayTime();
			if (time >= 0 && timingSlave)
				{
				timingSlave->UpdateTimer(time);
				}
			}

		ParseErrorCheck();
		ParseAudioData();
		ParseAncillaryData();
		}
	else
		strm->Advance(strm->AvailBits());
	}

#include <stdio.h>
MPEGAudioDecoder::MPEGAudioDecoder(AudioBitStream	*	strm,
			                          PCMWaveOut		*	pcmWaveOut,
					                    TimingSlave		*	timingSlave,
											  GenericProfile	*	globalProfile,
					                    GenericProfile	*	profile)
	{
	int i, j;

	this->strm = strm;
	this->pcmWaveOut = pcmWaveOut;
	this->timingSlave = timingSlave;
	this->globalProfile = globalProfile;
	this->profile = profile;

	dualMode = AC3DMDM_STEREO;

	static int shortWindowShift[] = {9, 5, 3, 0, 0, 3, 5, 9};


	for(i=0; i<128; i++) nik2[i] = (float)(16384.*cos(i * pi / 64.));

	for(j = 0; j < 32; j+=2)
		{
		for(i = 0; i < 8; i++)
			{
			shortWindowConsts[4*j+i][0] = (short)(floor(LayerIIWindowFactors[i*64+   j  ] * 16384 * (1 << shortWindowShift[i])+ 0.5));
			shortWindowConsts[4*j+i][1] = (short)(floor(LayerIIWindowFactors[i*64+32+j  ] * 16384 * (1 << shortWindowShift[i])+ 0.5));
			shortWindowConsts[4*j+i][2] = (short)(floor(LayerIIWindowFactors[i*64+   j+1] * 16384 * (1 << shortWindowShift[i])+ 0.5));
			shortWindowConsts[4*j+i][3] = (short)(floor(LayerIIWindowFactors[i*64+32+j+1] * 16384 * (1 << shortWindowShift[i])+ 0.5));
			}
		}
	}

MPEGAudioDecoder::~MPEGAudioDecoder(void)
	{
	}

void MPEGAudioDecoder::InitLocalsNewStream(void)
	{
	int i, j;
	for(j = 0; j < 2; j++)
		{
		for(i = 0; i < 1024; i++)
			{
			vbuffer[j][i] = 0;
			ivbuffer[j][i] = 0;
			}
		}

	filterBase = 0;

	hurryUp = FALSE;

	time = 0;
	lastTime = -1;
	}

void MPEGAudioDecoder::BeginStreaming(void)
	{
	pcmWaveOut->BeginStreaming();
	InitLocalsNewStream();
	}

void MPEGAudioDecoder::StartStreaming(int playbackSpeed)
	{
	pcmWaveOut->StartStreaming(playbackSpeed);
	}

void MPEGAudioDecoder::StopStreaming(void)
	{
	pcmWaveOut->StopStreaming();
	}

void MPEGAudioDecoder::EndStreaming(bool fullReset)
	{
	pcmWaveOut->EndStreaming(fullReset);
	}

void MPEGAudioDecoder::DetachStreaming(void)
	{
	pcmWaveOut->DetachStreaming();
	}

void MPEGAudioDecoder::AttachStreaming(void)
	{
	pcmWaveOut->AttachStreaming();
	InitLocalsNewStream();
	}

void MPEGAudioDecoder::SetAudioOutConfig(AC3SpeakerConfig spkCfg, AC3DualModeConfig dualModeCfg, AC3KaraokeConfig karaokeCfg)
	{
	dualMode = dualModeCfg;
	}
