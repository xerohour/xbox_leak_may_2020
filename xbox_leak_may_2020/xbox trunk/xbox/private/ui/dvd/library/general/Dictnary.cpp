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
// FILE:      library\general\dictnary.cpp
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   21.03.95
//
// PURPOSE:
//
// HISTORY:
//
//

#include "Dictnary.h"
#include <string.h>

IntDictionary::IntDictionary(void)
	{
	int i;

	for (i=0; i<47; i++) hashTable[i]=NULL;
	size = 0;
	}

IntDictionary::~IntDictionary(void)
	{
	Clear ();
	}

void IntDictionary::Clear (void)
	{
	int i;
	DictNodePtr p, next;

	for (i = 0;  i < 47;  i++)
		{
		p = hashTable[i];
		while (p != NULL)
			{
			next = p->succ;
			delete p;
			p = next;
			}
		hashTable[i] = NULL;
		}
	size = 0;
	}

Error IntDictionary::Insert(DWORD key, APTR data)
	{
	int hash;
	DictNodePtr	n;

	hash = (int)(key % 47);
	n = hashTable[hash];

	while (n && (n->key!=key)) n=n->succ;

	if (n)
		GNRAISE(GNR_OBJECT_FOUND);
	else
		{
		n = new DictNode;
		n->succ = hashTable[hash];
		hashTable[hash] = n;
		n->key = key;
		n->data = data;
		size++;

		GNRAISE_OK;
		}

	}

Error IntDictionary::Change(DWORD key, APTR data)
	{
	int hash;
	DictNodePtr	n;

	hash = (int)(key % 47);
	n = hashTable[hash];

	while (n && (n->key!=key)) n=n->succ;

	if (!n)
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	else
		{
		n->data = data;

		GNRAISE_OK;
		}
	}

Error IntDictionary::Lookup(DWORD key, APTR __far &data)
	{
	int hash;
	DictNodePtr	n;

	hash = (int)(key % 47);
	n = hashTable[hash];

	while (n && (n->key!=key)) n=n->succ;

	if (!n)
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	else
		{
		data = n->data;

		GNRAISE_OK;
		}
	}

Error IntDictionary::Remove(DWORD key)
	{
	int hash;
	DictNodePtr	n, p;

	hash = (int)(key % 47);
	n = hashTable[hash];
	p = NULL;

	while (n && (n->key!=key)) {p=n;n=n->succ;}

	if (!n)
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	else
		{
		if (p)
			p->succ = n->succ;
		else
			hashTable[hash] = n->succ;

		delete n;
		size--;

		GNRAISE_OK;
		}

	}

BOOL IntDictionary::Contains(DWORD key)
	{
	int hash;
	DictNodePtr	n;

	hash = (int)(key % 47);
	n = hashTable[hash];

	while (n && (n->key!=key)) n=n->succ;

	return n != NULL;
	}

class IntDictIterator : public Iterator {
	private:
		IntDictionary				*	dict;
		IntDictionary::DictNode *	node;
		int								num;
	public:
		IntDictIterator(IntDictionary * dict) : Iterator(dict) {this->dict = dict; node = NULL; num = 0;}
		APTR Proceed(void);
		~IntDictIterator(void) {}
	};

APTR IntDictIterator::Proceed(void)
	{
	if (node) node = node->succ;

	while (!node && (num < 47))
		{
		node = dict->hashTable[num++];
		}

	if (node) return node->data; else return NULL;
	}

Iterator * IntDictionary::CreateIterator(void)
	{
	return new IntDictIterator(this);
	}


int StringHash(const char * str)
	{
	DWORD val;

	val = 0;
	while (*str) val = val*2+*str++;

	return (int)(val % 47);
	}

StringDictionary::StringDictionary(void)
	{
	int i;

	for (i=0; i<47; i++) hashTable[i]=NULL;
	size = 0;
	}

StringDictionary::~StringDictionary(void)
	{
	Clear ();
	}


void StringDictionary::Clear (void)
	{
	int i;
	DictNodePtr p, next;

	for (i = 0;  i < 47;  i++)
		{
		p = hashTable[i];
		while (p != NULL)
			{
			next = p->succ;
			delete[] p->key;
			delete p;
			p = next;
			}
		hashTable[i] = NULL;
		}
	size = 0;
	}


Error StringDictionary::Insert(const char * key, APTR data)
	{
	int hash;
	DictNodePtr	n;

	hash = StringHash(key);
	n = hashTable[hash];

	while (n && (strcmp(n->key,key))) n=n->succ;

	if (n)
		GNRAISE(GNR_OBJECT_FOUND);
	else
		{
		n = new DictNode;
		n->succ = hashTable[hash];
		hashTable[hash] = n;
		n->key = new char[strlen(key)+1];
		strcpy(n->key,key);
		n->data = data;
		size++;

		GNRAISE_OK;
		}

	}

Error StringDictionary::Change(const char * key, APTR data)
	{
	int hash;
	DictNodePtr	n;

	hash = StringHash(key);
	n = hashTable[hash];

	while (n && (strcmp(n->key,key))) n=n->succ;

	if (!n)
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	else
		{
		n->data = data;

		GNRAISE_OK;
		}
	}

Error StringDictionary::Lookup(const char * key, APTR __far &data)
	{
	int hash;
	DictNodePtr	n;

	hash = StringHash(key);
	n = hashTable[hash];

	while (n && (strcmp(n->key,key))) n=n->succ;

	if (!n)
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	else
		{
		data = n->data;

		GNRAISE_OK;
		}
	}

Error StringDictionary::Remove(const char * key)
	{
	int hash;
	DictNodePtr	n, p;

	hash = StringHash(key);
	n = hashTable[hash];
	p = NULL;

	while (n && (strcmp(n->key,key))) {p=n;n=n->succ;}

	if (!n)
		GNRAISE(GNR_OBJECT_NOT_FOUND);
	else
		{
		if (p)
			p->succ = n->succ;
		else
			hashTable[hash] = n->succ;

		delete[] n->key;
		delete n;
		size--;

		GNRAISE_OK;
		}

	}

BOOL StringDictionary::Contains(const char * key)
	{
	int hash;
	DictNodePtr	n;

	hash = StringHash(key);
	n = hashTable[hash];

	while (n && (strcmp(n->key,key))) n=n->succ;

	return n != NULL;
	}

class StringDictIterator : public Iterator {
	private:
		StringDictionary				*	dict;
		StringDictionary::DictNode *	node;
		int									num;
	public:
		StringDictIterator(StringDictionary * dict) : Iterator(dict) {this->dict = dict; node = NULL; num = 0;}
		APTR Proceed(void);
		~StringDictIterator(void) {}
	};

APTR StringDictIterator::Proceed(void)
	{
	if (node) node = node->succ;

	while (!node && (num < 47))
		{
		node = dict->hashTable[num++];
		}

	if (node) return node->data; else return NULL;
	}

Iterator * StringDictionary::CreateIterator(void)
	{
	return new StringDictIterator(this);
	}

