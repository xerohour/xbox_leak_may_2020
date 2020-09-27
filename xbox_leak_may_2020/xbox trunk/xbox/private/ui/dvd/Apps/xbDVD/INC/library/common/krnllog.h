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
// FILE:      library\common\krnllog.h
// AUTHOR:    Viona
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   04.12.96
//
// PURPOSE:
//
// HISTORY:
//
#ifndef KRNLLOG_H
#define KRNLLOG_H

#include "prelude.h"
#include "krnlstr.h"

class KernelLogger
	{
	protected:
#if NT_KERNEL
		PDRIVER_OBJECT	driver;
#endif
	public:
#if NT_KERNEL
		KernelLogger(PDRIVER_OBJECT driver);
#else
#endif
		Error LogEvent(DWORD msgID);
		Error LogEvent(DWORD msgID, KernelString ext1);
		Error LogEvent(DWORD msgID, KernelString ext1, KernelString ext2);
		Error LogEvent(DWORD msgID, KernelString ext1, KernelString ext2, KernelString ext3);
	};

#endif

