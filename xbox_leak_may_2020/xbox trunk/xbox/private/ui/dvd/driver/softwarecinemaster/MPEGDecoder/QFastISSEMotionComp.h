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

#ifndef QFASTISSEMOTIONCOMP_H
#define QFASTISSEMOTIONCOMP_H

#include "GenericMMXMotionComp.h"

class FastISSEMotionComp : public GenericMMXMotionComp
	{
	public:
		void BiMotionCompensationUV(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal);

		void SBiMotionCompensationUV(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal);

		void BiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal);

		void SBiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal);

		void DBiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr);

		void SDBiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr);

		void IDBiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist);

		void ISDBiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist);

		void MotionCompensationUV(int yp, int xp, BYTE * prevp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal);

		void SMotionCompensationUV(int yp, int xp, BYTE * prevp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal);

		void MotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal);

		void SMotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal);

		void DMotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr);

		void SDMotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr);

		void IDMotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist);

		void ISDMotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist);

	};

#endif

