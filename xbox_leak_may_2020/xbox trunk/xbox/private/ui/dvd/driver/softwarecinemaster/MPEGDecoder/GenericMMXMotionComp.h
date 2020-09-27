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

#ifndef GENERICMMXMOTIONCOMP_H
#define GENERICMMXMOTIONCOMP_H

#include "driver\SoftwareCinemaster\Common\Prelude.h"

class GenericMMXMotionComp
	{
	public:
		virtual void BiMotionCompensationUV(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal) = 0;

		virtual void SBiMotionCompensationUV(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal) = 0;

		virtual void BiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal) = 0;

		virtual void SBiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal) = 0;

		virtual void DBiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr) = 0;

		virtual void SDBiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr) = 0;

		virtual void IDBiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist) = 0;

		virtual void ISDBiMotionCompensationY(int yp, int xp, int ys, int xs, BYTE * prevp, BYTE * succp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist) = 0;

		virtual void MotionCompensationUV(int yp, int xp, BYTE * prevp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal) = 0;

		virtual void SMotionCompensationUV(int yp, int xp, BYTE * prevp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal) = 0;

		virtual void MotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal) = 0;

		virtual void SMotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal) = 0;

		virtual void DMotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr) = 0;

		virtual void SDMotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr) = 0;

		virtual void IDMotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist) = 0;

		virtual void ISDMotionCompensationY(int yp, int xp, BYTE * prevp, int sbpr, bool copy, BYTE * destp, short * deltap, int dbpr, short * dfinal, BYTE * uvdp, BYTE * fbp, int fbbpr, int ifbdist) = 0;
	};

#endif
