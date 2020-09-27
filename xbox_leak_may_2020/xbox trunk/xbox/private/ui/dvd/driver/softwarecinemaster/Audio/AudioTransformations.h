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

#ifndef AUDIOTRANSFORMATIONS_H
#define AUDIOTRANSFORMATIONS_H

#include <xtl.h>

#define HIGH_PRECISION_FFT	0

#if HIGH_PRECISION_FFT
typedef	double	fftreal;
#else
typedef	float		fftreal;
#endif

struct complex {fftreal r; fftreal i;};

class AudioTransformation
	{
//	protected:

	public:
		AudioTransformation(void) {}
		virtual ~AudioTransformation(void){}
		virtual void AC3TransformChannels(fftreal * chcoeff, fftreal * chpostfft, bool split){}
	};

class NormalTransformChannels : public AudioTransformation
	{
//	protected:

	public:
		NormalTransformChannels(void){}
		~NormalTransformChannels(void){}
		void AC3TransformChannels(fftreal * chcoeff, fftreal * chpostfft, bool split);
	};

class AMD3DNowTransformChannels : public NormalTransformChannels
	{
//	protected:

	public:
		AMD3DNowTransformChannels(void){}
		~AMD3DNowTransformChannels(void){}
		virtual void AC3TransformChannels(fftreal * chcoeff, fftreal * chpostfft, bool split);
	};



#endif
