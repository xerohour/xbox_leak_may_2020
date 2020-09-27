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
// FILE:      library\general\iterator.h
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   21.03.95
//
// PURPOSE:   Iterators for abstract data types
//
// HISTORY:
//
//

#ifndef ITERATOR_H
#define ITERATOR_H

class IteratorHost;
class IteratorHandle;

class Iterator {
	friend class IteratorHandle;
	friend class IteratorHost;
	private:
		IteratorHost	*	host;
		Iterator			*	succ;
		APTR					data;
		BOOL					skipped;

		APTR InternalProceed(void) {if (skipped) {skipped = FALSE; return data;} else return (data = Proceed());}
		void CheckRemove(APTR remove);
		virtual APTR Proceed(void) = 0;
	protected:
		inline virtual ~Iterator(void);
	public:
		Iterator(IteratorHost * host);
	};

class __far IteratorHandle {
	private:
		Iterator	*	iter;
	public:
		IteratorHandle(Iterator * iter) {this->iter = iter;}
		~IteratorHandle(void) {delete iter;}
		APTR Proceed(void) {return iter->InternalProceed();}
	};

class IteratorHost {
	friend class Iterator;
	private:
		Iterator		*	head;
	protected:
		void CheckIteratorRemove(APTR data);
	public:
		IteratorHost(void) {head = NULL;}
		virtual Iterator * CreateIterator(void) = 0;
	};

inline Iterator::Iterator(IteratorHost * host)
	{
	this->host = host;
	succ = host->head;
	host->head = this;
	skipped = FALSE;
	data = NULL;
	}

inline void Iterator::CheckRemove(APTR removed)
	{
	if (data == removed)
		{
		data = Proceed();
		skipped = TRUE;
		}
	}

inline void IteratorHost::CheckIteratorRemove(APTR data)
	{
	Iterator	*	p = head;

	while (p)
		{
		p->CheckRemove(data);
		p = p->succ;
		}
	}

inline Iterator::~Iterator(void)
	{
	host->head = succ;
	}


#define ITERATE(element, host) \
	{ IteratorHandle _xcrsr((host)->CreateIterator()); \
	while ((APTR &)element = _xcrsr.Proceed()) {

#define ITERATE_UNTIL(element, host, condition) \
	{ IteratorHandle _xcrsr((host)->CreateIterator()); \
	while (((APTR &)element = _xcrsr.Proceed()) && !(condition)) {

#define ITERATE_FIND(element, host, condition) \
	{ IteratorHandle _xcrsr((host)->CreateIterator()); \
	while (((APTR &)element = _xcrsr.Proceed()) && !(condition)) ; }

#define ITERATE_WHILE(element, host, condition) \
	{ IteratorHandle _xcrsr((host)->CreateIterator()); \
	while (((APTR &)element = _xcrsr.Proceed()) && (condition)) {

#define ITERATE_IF(element, host, condition) \
	{ IteratorHandle _xcrsr((host)->CreateIterator()); \
	while ((APTR &)element = _xcrsr.Proceed()) if (condition) {

#define ITERATE_END } }

#endif
