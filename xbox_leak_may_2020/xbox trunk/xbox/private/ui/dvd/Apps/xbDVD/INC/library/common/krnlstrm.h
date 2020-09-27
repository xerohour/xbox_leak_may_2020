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
// FILE:      library\common\krnlstrm.h
// AUTHOR:    Viona
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   04.12.96
//
// PURPOSE:
//
// HISTORY:
//
#ifndef KRNLSTRM
#define KRNLSTRM

#include "prelude.h"
#include "gnerrors.h"
#include "krnlstr.h"

#if !NT_KERNEL
#include <mmsystem.h>
#endif

class KernelInputStream
	{
	private:
#if NT_KERNEL
		HANDLE		file;
#else
		HMMIO			file;
#endif
		DWORD			pos;		// position of _next_ buffer in file
		DWORD			size;		// size of file
		BYTE		*	buffer;
		WORD			bsize;	// size of buffer
		WORD			bpos;		// current byte in buffer
		WORD			bend;		// end of buffer

		Error DirectRead(FPTR buffer, DWORD size, DWORD __far & done);

	public:
		Error Open(KernelString name, WORD bufferSize = 512);
		Error Close(void);

		Error Read(FPTR buffer, DWORD size, DWORD __far & done);
		Error Read(char __far & c);
		Error Peek(char __far & c);

		Error ReadDec(BYTE __far & b);
		Error ReadDec(WORD __far & w);
		Error ReadDec(DWORD __far & d);

		Error ReadHex(BYTE __far & b);
		Error ReadHex(WORD __far & w);
		Error ReadHex(DWORD __far & d);

		BOOL EOF(void);
	};

class KernelOutputStream
	{
	private:
#if NT_KERNEL
		HANDLE		file;
#else
		HMMIO			file;
#endif
		DWORD			pos;
		BYTE		*	buffer;
		WORD			bsize, bend;

		Error DirectWrite(FPTR buffer, DWORD size, DWORD __far & done);

	public:
		Error Open(KernelString name, WORD bufferSize = 512);
		Error Close(void);

		Error Write(FPTR buffer, DWORD size, DWORD __far & done);
		Error Write(char c);

		Error WriteHex(BYTE  b);
		Error WriteHex(WORD  w);
		Error WriteHex(DWORD  d);

		Error WriteDec(DWORD d, int num);
	};

#endif
