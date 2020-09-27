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

#include "GenericMMXYUVConverter.h"
#include "..\common\TimedFibers.h"

#define MEASURE_SCALE_TIME				0
#define MEASURE_XSCALE_TIME			0
#define MEASURE_DEINTERLACE_TIME		0

MMXShort<4> sinctab[128];
MMXShort<4> sinctabxSet[16][128];
MMXShort<4> sinctaby[128];
MMXShort<4> sinctabySet[16][128];

#pragma warning (disable : 4799 4731)


#include <math.h>

static inline double sinc(double x)
	{
	if (x != 0.0)
		return sin(x) / x;
	else
		return 1.0;
	}

static const double pi = 3.14159265359;

double bessi0(double x)
//Returns the modified Bessel function I0(x) for any real x.
	{
	double ax,ans;
	double y;						// Accumulate polynomials in double precision.

	if ((ax=fabs(x)) < 3.75)
		{								// Polynomial fit
		y=x/3.75;
		y*=y;
		ans=1.0+y*(3.5156229+y*(3.0899424+y*(1.2067492
				+y*(0.2659732+y*(0.360768e-1+y*0.45813e-2)))));
		}
	else
		{
		y=3.75/ax;
		ans=(exp(ax)/sqrt(ax))*(0.39894228+y*(0.1328592e-1
				+y*(0.225319e-2+y*(-0.157565e-2+y*(0.916281e-2
				+y*(-0.2057706e-1+y*(0.2635537e-1+y*(-0.1647633e-1
				+y*0.392377e-2))))))));
		}

	return ans;
	}


int RoundFloat2Int(double f)
	{
	return ((int)(f - floor(f)<0.5 ? floor(f) : ceil(f)));
	}

void LPFCoefficients(double stop_freq_ratio, int numOfTaps,int	beta,int fractN, int fractD, double * coefsW)
	{
	double coefs[100];
	double window[100];
	double sumCoefs = 0;
	double sumCoefsW = 0;
	int n;
	double currFrac = (double)fractN/(double)fractD;
	double fbeta = beta*1.23;	// emperically found to counter beta being spread over numOfTaps*fractD
	double xn, x;

	for (n=0; n<numOfTaps; n++)
		{
		// do variable width sinc
//		double x = 2.0*pi*stop_freq_ratio*(n-((numOfTaps-1.0+currFrac-0.5)/2.0));
		xn = currFrac - (n-floor((numOfTaps-1.0)/2.0));
		x = 2.0*pi*stop_freq_ratio*xn;

		if (x==0)
			{
			coefs[n] = 2.0*stop_freq_ratio*1.0;
			}
		else
			{
			coefs[n] = 2.0*stop_freq_ratio*sin(x)/x;
			}
		sumCoefs += coefs[n];
		window[n] = bessi0(fbeta * sqrt(1 - (2 * xn / numOfTaps) * (2 * xn / numOfTaps))) / bessi0(fbeta);
//			bessi0(fbeta* sqrt(fabs(1.0 - pow(1.0-(2.0*((n*fractD)-fractN+fractD-1)/(numOfTaps*fractD-1.0)),2))))/bessi0(fbeta);
		coefsW[n] = coefs[n]*window[n];
		sumCoefsW += coefsW[n];
		}

	// normallize the gain of the coefs to 1.0
	for (n=0; n<numOfTaps; n++)
		{
		coefs[n] = coefs[n]*1.0/sumCoefs;
		coefsW[n] = coefsW[n]*1.0/sumCoefsW;
		}

	} /*end of routine */

void InitFrameStoreScalerTables(void)
	{
	double c0, c1, c2, c3;
	int i0, i1, i2, i3;
	double cs;
	int i, j;

	for(i=0; i<128; i++)
		{
		c0 = c1 = c2 = c3 = 0;

		c0 += sinc(pi * ((double)i  / 128  + 1));
		c1 += sinc(pi * ((double)i  / 128     ));
		c2 += sinc(pi * ((double)i  / 128  - 1));
		c3 += sinc(pi * ((double)i  / 128  - 2));

		cs = c0 + c1 + c2 + c3;

		c0 = c0 * 16384 / cs;
		c1 = c1 * 16384 / cs;
		c2 = c2 * 16384 / cs;
		c3 = c3 * 16384 / cs;

		sinctab[i][0] = (short)floor(c0 + 0.5);
		sinctab[i][1] = (short)floor(c1 + 0.5);
		sinctab[i][2] = (short)floor(c2 + 0.5);
		sinctab[i][3] = (short)floor(c3 + 0.5);

		c0 = c0 / 256;
		c1 = c1 / 256;
		c2 = c2 / 256;
		c3 = c3 / 256;

		double t = 0.5;


		do {
			i0 = (int)floor(c0 + t);
			i1 = (int)floor(c1 + t);
			i2 = (int)floor(c2 + t);
			i3 = (int)floor(c3 + t);

			t += 0.01;
			} while (i0 + i1 + i2 + i3 < 64);

		while (i0 + i1 + i2 + i3 > 64)
			{
			t -= 0.01;

			i0 = (int)floor(c0 + t);
			i1 = (int)floor(c1 + t);
			i2 = (int)floor(c2 + t);
			i3 = (int)floor(c3 + t);
			}

		sinctaby[i][0] = (short)i0;
		sinctaby[i][1] = (short)i1;
		sinctaby[i][2] = (short)i2;
		sinctaby[i][3] = (short)i3;
		}

	double fw[4];

	for(j=0; j<16; j++)
		{
		for(i=0; i<128; i++)
			{
			LPFCoefficients((j + 1) / 32., 4, 2, i, 128, fw);

			double t = 0.5;


			do {
				i0 = (int)floor(fw[0] * 64 + t);
				i1 = (int)floor(fw[1] * 64 + t);
				i2 = (int)floor(fw[2] * 64 + t);
				i3 = (int)floor(fw[3] * 64 + t);

				t += 0.01;
				} while (i0 + i1 + i2 + i3 < 64);

			while (i0 + i1 + i2 + i3 > 64)
				{
				t -= 0.01;

				i0 = (int)floor(fw[0] * 64 + t);
				i1 = (int)floor(fw[1] * 64 + t);
				i2 = (int)floor(fw[2] * 64 + t);
				i3 = (int)floor(fw[3] * 64 + t);
				}

			sinctabySet[j][i][0] = (short)i0;
			sinctabySet[j][i][1] = (short)i1;
			sinctabySet[j][i][2] = (short)i2;
			sinctabySet[j][i][3] = (short)i3;

			i0 = (int)floor(fw[0] * 16384 + 0.5);
			i1 = (int)floor(fw[1] * 16384 + 0.5);
			i2 = (int)floor(fw[2] * 16384 + 0.5);
			i3 = (int)floor(fw[3] * 16384 + 0.5);

			sinctabxSet[j][i][0] = (short)i0;
			sinctabxSet[j][i][1] = (short)i1;
			sinctabxSet[j][i][2] = (short)i2;
			sinctabxSet[j][i][3] = (short)i3;
			}
		}
	}

GenericMMXYUVVideoConverter::GenericMMXYUVVideoConverter(void)
	{
	int x, y;

	for(x = VSQL_BINARY; x <= VSQL_EIGHT_TAP; x++)
		{
		for(y = VSQL_BINARY; y <= VSQL_EIGHT_TAP; y++)
			{
			convertStripes[0][0][x][y] = ConvertStripeToYUV_P7DS;
			convertStripes[0][1][x][y] = ConvertStripeToYUVThroughOSD_P7DS;

			switch (y)
				{
				case VSQL_BINARY:
				case VSQL_DROP:
					switch (x)
						{
						case VSQL_BINARY:
							convertStripes[1][0][x][y] = ConvertStripeToYUV_P8DS;
							convertStripes[1][1][x][y] = ConvertStripeToYUVThroughOSD_P8DS;
							break;
						case VSQL_DROP:
						case VSQL_LINEAR:
							convertStripes[1][0][x][y] = ConvertStripeToYUV_P8F2;
							convertStripes[1][1][x][y] = ConvertStripeToYUVThroughOSD_P8F2;
							break;
						case VSQL_FOUR_TAP:
						case VSQL_EIGHT_TAP:
							convertStripes[1][0][x][y] = ConvertStripeToYUV_P8F4;
							convertStripes[1][1][x][y] = ConvertStripeToYUVThroughOSD_P8F4;
							break;
						}
					break;
				case VSQL_LINEAR:
					switch (x)
						{
						case VSQL_BINARY:
							convertStripes[1][0][x][y] = ConvertStripeToYUV_P8DSY2;
							convertStripes[1][1][x][y] = ConvertStripeToYUVThroughOSD_P8DSY2;
							break;
						case VSQL_DROP:
						case VSQL_LINEAR:
							convertStripes[1][0][x][y] = ConvertStripeToYUV_P8F2Y2;
							convertStripes[1][1][x][y] = ConvertStripeToYUVThroughOSD_P8F2Y2;
							break;
						case VSQL_FOUR_TAP:
						case VSQL_EIGHT_TAP:
							convertStripes[1][0][x][y] = ConvertStripeToYUV_P8F4Y2;
							convertStripes[1][1][x][y] = ConvertStripeToYUVThroughOSD_P8F4Y2;
							break;
						}
					break;
				case VSQL_FOUR_TAP:
				case VSQL_EIGHT_TAP:
					switch (x)
						{
						case VSQL_BINARY:
						case VSQL_DROP:
						case VSQL_LINEAR:
							convertStripes[1][0][x][y] = ConvertStripeToYUV_P8F2Y4;
							convertStripes[1][1][x][y] = ConvertStripeToYUVThroughOSD_P8F2Y4;
							break;
						case VSQL_FOUR_TAP:
						case VSQL_EIGHT_TAP:
							convertStripes[1][0][x][y] = ConvertStripeToYUV_P8F4Y4;
							convertStripes[1][1][x][y] = ConvertStripeToYUVThroughOSD_P8F4Y4;
							break;
						}
					break;
				}

			}
		}
	}


#pragma warning (default : 4799 4731)
