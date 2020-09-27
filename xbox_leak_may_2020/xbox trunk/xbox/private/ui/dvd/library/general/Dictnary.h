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

//
// FILE:      library\general\dictnary.h
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   21.03.95
//
// PURPOSE:   Dictionaries for sparse integer arrays, and string
//				  to pointer association
//
// HISTORY:
//
//


#ifndef DICTNARY_H
#define DICTNARY_H

#include "library/common/prelude.h"
#include "library/common/gnerrors.h"
#include "Iterator.h"

class IntDictIterator;

class IntDictionary : public IteratorHost {
	friend class IntDictIterator;
	private:
		typedef struct DictNode {
			struct DictNode * succ;
			DWORD 				key;
			APTR					data;
		} * DictNodePtr;
		DictNodePtr	hashTable[47];
		int size;
	public:
		IntDictionary(void);
		~IntDictionary(void);

		// Clear the dictionary from all contents.
		void Clear (void);

		//
		// Insert an element into a dictionary, fails with GNR_OBJECT_FOUND, when the
		// given key does already exist.
		//
		Error Insert(DWORD key, APTR data);

		//
		// Change an element in a dictionary, fails with GNR_OBJECT_NOT_FOUND, when the
		// given key does not exist.
		//
		Error Change(DWORD key, APTR data);

		//
		// Remove an element from a dictionary, fails with GNR_OBJECT_NOT_FOUND, when the
		// given key does not exist.
		//
		Error Remove(DWORD key);

		//
		// Remove an element from a dictionary, fails with GNR_OBJECT_NOT_FOUND, when the
		// given key does not exist.
		//
		Error Lookup(DWORD key, APTR __far &data);

		//
		// Checks whether an key is included in a dictionary
		//
		BOOL Contains(DWORD key);
		int Size(void) {return size;}

		Iterator * CreateIterator(void);
	};

class StringDictIterator;

class StringDictionary : public IteratorHost {
friend class StringDictIterator;
	private:
		typedef struct DictNode {
			struct DictNode * succ;
			char 				 *	key;
			APTR					data;
		} * DictNodePtr;
		DictNodePtr	hashTable[47];
		int size;
	public:
		StringDictionary(void);
		~StringDictionary(void);

		// Clear the dictionary from all contents.
		void Clear (void);

		//
		// Insert an element into a dictionary, fails with GNR_OBJECT_FOUND, when the
		// given key does already exist.
		//
		Error Insert(const char * key, APTR data);

		//
		// Change an element in a dictionary, fails with GNR_OBJECT_NOT_FOUND, when the
		// given key does not exist.
		//
		Error Change(const char * key, APTR data);

		//
		// Remove an element from a dictionary, fails with GNR_OBJECT_NOT_FOUND, when the
		// given key does not exist.
		//
		Error Remove(const char * key);

		//
		// Remove an element from a dictionary, fails with GNR_OBJECT_NOT_FOUND, when the
		// given key does not exist.
		//
		Error Lookup(const char * key, APTR __far &data);

		//
		// Checks whether an key is included in a dictionary
		//
		BOOL Contains(const char * key);
		int Size(void) {return size;}

		Iterator * CreateIterator(void);
	};


#endif
