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
// FILE:			library\common\krnlhash.h
// AUTHOR:		Viona
// COPYRIGHT:	(c) 1999 Viona Development.  All Rights Reserved.
// CREATED:		1999-11-09
//
// PURPOSE:		Hashing table for Kernelstring key hashing
//
// HISTORY:		Created 1999-11-09 Ulrich Mohr
//

#ifndef _KRNLHASH_H_
#define _KRNLHASH_H_

#include "library/common/krnlstr.h"
#include "library/general/iterator.h"

#define defaultHashTableSize 32


// Entry in Kernel Hash list....

class KernelHash;

class HashNode
	{
	friend class KernelHash;
	friend class HashIterator;

	protected:
		KernelString key;
		HashNode * next;
		HashNode(KernelString key);
		HashNode(HashNode &);
	};

typedef HashNode * HashNodePtr;


// Kernel Hash class...

class KernelHash : public IteratorHost
	{
	friend class HashIterator;
	private:
		HashNode ** hashTable;
	protected:
		DWORD hashTableSize;
		// hash fuction....
		DWORD Hash(KernelString key);
	public:
		// Constructor ....
		KernelHash(DWORD hashTableSize = defaultHashTableSize);
		// Copy constructor ...
		KernelHash(KernelHash &);

		// Destructor ....
		~KernelHash();

		bool Enter(HashNode * pHash, KernelString key);
		HashNode* Remove(KernelString key);
		HashNode* LookUp(KernelString key);

		Iterator * CreateIterator(void);
	};


// Kernel Hash Iterator class...

class HashIterator : public Iterator
	{
	private:
		HashNode * nextNode;
		KernelHash * hashTable;
		DWORD searchIndex;
	public:
		HashIterator(KernelHash * pHash);
		APTR Proceed(void);
		~HashIterator(void) {}
	};

#endif // _KRNLHASH_H_
