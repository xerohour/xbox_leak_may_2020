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
// FILE:      library\common\krnlfile.h
// AUTHOR:    Viona
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   04.12.96
//
// PURPOSE:
//
// HISTORY:
//
#ifndef KRNLFILE_H
#define KRNLFILE_H

#include "prelude.h"
#include "gnerrors.h"
#include "krnlint.h"
#include "..\general\lists.h"
#include "..\general\refcnt.h"

class KernelFileHandle;
class GenericKernelInputFile;
class GenericKernelOutputFile;

class GenericKernelFileHandle : public Node, public ReferenceCounter
	{
	friend class GenericKernelFile;
	friend class GenericKernelInputFile;
	friend class GenericKernelOutputFile;

	public:
		KernelInt64	pos;
		KernelInt64 size;

		virtual Error RawWrite(HPTR buffer, DWORD size, DWORD __far & done) = 0;
		virtual Error RawRead(HPTR buffer, DWORD size, DWORD __far & done) = 0;

		virtual ~GenericKernelFileHandle(void) {}

		virtual Error Seek(KernelInt64 at) = 0;
	};

class GenericKernelFile
	{
	protected:
		GenericKernelFileHandle	*	handle;
	public:
		GenericKernelFile(void);
		virtual ~GenericKernelFile(void);

		GenericKernelInputFile * CastToInputFile(void) {return NULL;}
		GenericKernelOutputFile * CastToOutputFile(void) {return NULL;}

		Error Open(GenericKernelFileHandle * handle);
		Error Close(void);
		Error Seek(KernelInt64 at);

		KernelInt64 FilePos(void) {return handle ? handle->pos : 0;}
		KernelInt64 FileSize(void) {return handle ? handle->size : 0;}
	};

class GenericKernelInputFile : virtual public GenericKernelFile
	{
	public:
		GenericKernelInputFile * CastToInputFile(void) {return this;}

		Error Read(HPTR buffer, DWORD size, DWORD __far & done);
		Error Read(BYTE __far & data);
		Error Read(WORD __far & data);
		Error Read(DWORD __far & data);
	};

class GenericKernelOutputFile : virtual public GenericKernelFile
	{
	public:
		GenericKernelOutputFile * CastToOutputFile(void) {return this;}

		Error Write(HPTR buffer, DWORD size, DWORD __far & done);
		Error Write(BYTE data);
		Error Write(WORD data);
		Error Write(DWORD data);
	};

class GenericKernelInOutFile : public GenericKernelInputFile, public GenericKernelOutputFile
	{
	};

class KernelInputFile : public GenericKernelInputFile
	{
	friend class KernelInputSubFile;
	friend class KernelInputChainFile;
	public:
		virtual Error Open(KernelString fname);
	};

class KernelOutputFile : public GenericKernelOutputFile
	{
	public:
		virtual Error Open(KernelString fname);
		virtual Error Create(KernelString fname);

	};

class KernelInOutFile : public KernelInputFile,
                        public KernelOutputFile,
                        public GenericKernelInOutFile
	{
	public:
		virtual Error Open(KernelString fname);
		virtual Error Create(KernelString fname);
	};


class KernelInputSubFile : public GenericKernelInputFile
	{
	public:
		virtual Error Open(KernelInputFile * file, KernelInt64 start, KernelInt64 size);
	};

class KernelInputChainFile : public KernelInputFile
	{
	protected:
		class KernelChainFileHandle * handle;
	public:
		KernelInputChainFile(void) {handle = NULL;}
		virtual Error Open(void);

		virtual Error AppendFile(KernelInputFile * file);
		virtual Error PrependFile(KernelInputFile * file);
		virtual Error RemoveFile(KernelInputFile * file);
	};

#endif
