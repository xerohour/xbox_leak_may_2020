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
// FILE:      library\general\lists.cpp
// AUTHOR:    U. Sigmund
// COPYRIGHT: (c) 1995 Viona Development.  All Rights Reserved.
// CREATED:   21.03.95
//
// PURPOSE:
//
// HISTORY:
//
//

#include "Lists.h"

BOOL List::Contains(Node * n)
	{
	Node * p = first;

	while (p && (p!=n)) p=p->succ;

	return (p == n);
	}

void List::InsertFirst(Node * n)
	{
	n->succ = first;
	n->pred = NULL;
	if (first)
		first->pred = n;
	else
		last = n;
	first = n;
	num++;
	}

void List::InsertLast(Node * n)
	{
	n->succ = NULL;
	n->pred = last;
	if (last)
		last->succ = n;
	else
		first = n;
	last = n;
	num++;
	}

void List::InsertBefore(Node * n, Node * before)
	{
	if (before)
		{
		n->succ = before;
		n->pred = before->pred;
		if (before->pred)
			before->pred->succ = n;
		else
			first = n;
		before->pred = n;
		num++;
		}
	else
		InsertLast(n);
	}

void List::InsertAfter(Node * n, Node * after)
	{
	if (after)
		{
		n->succ = after->succ;
		n->pred = after;
		if (after->succ)
			after->succ->pred = n;
		else
			last = n;

      after->succ = n;
		num++;
		}
	else
		InsertFirst(n);
	}

void List::InsertByPriority(Node * n)
	{
	Node * p = first;

	while (p && p->HigherPriorityThan(n)) p = p->succ;

	InsertBefore(n, p);
	}

void List::SortByPriority(Node * n)
	{
	}

void List::RemoveFirst(void)
	{
	CheckIteratorRemove(first);

	first = first->succ;
	if (first)
		first->pred = NULL;
	else
		last = NULL;
	num--;
	}

void List::RemoveLast(void)
	{
	CheckIteratorRemove(last);

	last = last->pred;
	if (last)
		last->succ = NULL;
	else
		first = NULL;
	num--;
	}

void List::Remove(Node * n)
	{
	CheckIteratorRemove(n);

	if (n->pred)
		n->pred->succ = n->succ;
	else
		first = n->succ;
	if (n->succ)
		n->succ->pred = n->pred;
	else
		last = n->pred;
	num--;
	}

class ListIterator : public Iterator
	{
	private:
		List	*	list;
		Node	*	node;
	public:
		ListIterator(List * list) : Iterator(list) {this->list = list; node = NULL;}
		APTR Proceed(void);
		~ListIterator(void) {}
	};

APTR ListIterator::Proceed(void)
	{
	if (node)
		return (node = node->Succ());
	else
		return (node = list->First());
	}

Iterator * List::CreateIterator(void)
	{
	return new ListIterator(this);
	}

