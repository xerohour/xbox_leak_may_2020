////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000 Ravisent Technologies, Inc.  All Rights Reserved.           //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of Ravisent Technologies, Inc.    //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between Ravisent and Microsoft.  This            //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by Ravisent.  THE ONLY PERSONS   //
// WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS                     //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO RAVISENT, IN THE FORM                 //
// PRESCRIBED BY RAVISENT.                                                    //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "AudioTransformations.h"
//#include "..\common\amd3dx.h"
#include "driver\SoftwareCinemaster\Common\Prelude.h"


static MMXDWORD<2>	xorLo1HiMinus1 = {0x00000000, 0x80000000};
static MMXDWORD<2>	xorLoMinus1Hi1 = {0x80000000, 0x00000000};

extern complex cossintab64[64];
extern complex xcossin512[128];
extern complex xcossin256[64];
extern fftreal ac3WindowTable2[256];

#pragma warning(disable : 4799)

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



void NormalTransformChannels::AC3TransformChannels(fftreal * chcoeff, fftreal * chpostfft, bool split)
	{
	if(split)
		InverseFFT256(chcoeff, chpostfft);
	else
		InverseFFT512(chcoeff, chpostfft);
	}



#pragma warning(default : 4799)
