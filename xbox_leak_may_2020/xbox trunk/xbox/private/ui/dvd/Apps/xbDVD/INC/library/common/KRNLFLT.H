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

//
// FILE:      library\common\krnlflt.h
// AUTHOR:    Viona
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   04.12.96
//
// PURPOSE:
//
// HISTORY:
//
//

#ifndef KRNLFLT_H
#define KRNLFLT_H

#include "prelude.h"
#include "krnlstr.h"

class KernelFloat
	{
	private:
		unsigned long	mantisse;
		short	exponent;
		short sign;

		void Normalize(void);
		KernelFloat(unsigned long mantisse, short exponent, short sign);
	public:
		KernelFloat(void);
		KernelFloat(long val, unsigned long frac = 0, short exp = 0);
		KernelFloat(const KernelString u);

		KernelString ToString(int digits = 0, BOOL exp = FALSE);

		long Trunc(void);

		friend KernelFloat operator+ (const KernelFloat u, const KernelFloat v);
		friend KernelFloat operator* (const KernelFloat u, const KernelFloat v);
		friend KernelFloat operator- (const KernelFloat u, const KernelFloat v);
		friend KernelFloat operator/ (const KernelFloat u, const KernelFloat v);

		KernelFloat & operator+= (const KernelFloat u);
		KernelFloat & operator*= (const KernelFloat u);
		KernelFloat & operator/= (const KernelFloat u);
		KernelFloat & operator-= (const KernelFloat u);

		KernelFloat operator- (void) const;

		int Compare(const KernelFloat u) const;

		friend BOOL operator==(const KernelFloat u, const KernelFloat v);
		friend BOOL operator!=(const KernelFloat u, const KernelFloat v);
		friend BOOL operator<(const KernelFloat u, const KernelFloat v);
		friend BOOL operator>(const KernelFloat u, const KernelFloat v);
		friend BOOL operator<=(const KernelFloat u, const KernelFloat v);
		friend BOOL operator>=(const KernelFloat u, const KernelFloat v);
	};

#endif
