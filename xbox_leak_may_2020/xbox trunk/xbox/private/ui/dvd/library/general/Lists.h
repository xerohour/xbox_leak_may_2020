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
// FILE:      library\general\lists.h
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   21.03.95
//
// PURPOSE:   Generic linear list classes
//
// HISTORY:
//
//


#ifndef LISTS_H
#define LISTS_H

#include "library/common/prelude.h"
#include "Iterator.h"

class List;

class Node
	{
	friend class List;
	friend class ListIterator;

	private:
		Node	*	succ;
		Node	*	pred;
	protected:
		virtual BOOL HigherPriorityThan(Node * n) {return FALSE;}
	public:
		Node(void) {succ = NULL;pred = NULL;};
		virtual ~Node(void) {;};

		Node * Succ(void) {return succ;};
		Node * Pred(void) {return pred;};

		BOOL IsFirst(void) {return pred == NULL;};
		BOOL IsLast(void) {return succ == NULL;};
	};

class List : public IteratorHost
	{
	private:
		Node				*	first;
		Node				*	last;
		int					num;
	public:
		List(void) {first = NULL; last = NULL; num = 0;};
		virtual ~List(void) {;};

		Node * First(void) {return first;};
		Node * Last(void) {return last;};

		BOOL IsEmpty(void) {return first == NULL;};
		BOOL Contains(Node * n);
		int Num(void) {return num;};

		void InsertFirst(Node * n);
		void InsertLast(Node * n);
		void InsertBefore(Node * n, Node * before);
		void InsertAfter(Node * n, Node * after);

		void InsertByPriority(Node * n);
		void SortByPriority(Node * n);

		void RemoveFirst(void);
		void RemoveLast(void);
		void Remove(Node * n);

		void Enqueue(Node * n) {InsertLast(n);}
		Node * Dequeue(void) {Node * n = first; if (n) RemoveFirst(); return n;}

		void Push(Node * n) {InsertFirst(n);}
		Node * Pop(void) {return Dequeue();}

		Iterator * CreateIterator(void);
	};

#endif
