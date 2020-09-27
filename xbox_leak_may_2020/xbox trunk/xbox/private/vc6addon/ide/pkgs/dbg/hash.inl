
/*++

Copyright(c) 1997 Microsoft Corporation

Module Name:

	hash.inl

Abstract:

	Inline definitions of templated hash functions.  See hash.h for more
	information.

Author:

    Matthew D Hendel (math) 13-May-1997

--*/

#ifndef _Hash_inl_
#define _Hash_inl_

#include "primes.h"

//
// Implementation of THashedMapOf
//

template <class _KeyType, class _DataType>
THashedMapOf <_KeyType, _DataType>::THashedMapOf(
	ULONG		nInitialSize,	// = 512
	float		highWaterMark,	// = 2.0
	float		lowWaterMark	// = 0.0
	)
  : m_nArraySize (0),
	m_nElements (0),
	m_HighWaterMark (highWaterMark),
	m_LowWaterMark (lowWaterMark)
{
	m_nArraySize = FindPrimeSuccessor (nInitialSize);
	m_HashArray = new Node* [m_nArraySize];
	memset (m_HashArray, 0, m_nArraySize * sizeof (m_HashArray [0]));
	InitializeCriticalSection (&m_cs);
}


template <class _KeyType, class _DataType>
THashedMapOf <_KeyType, _DataType>::~THashedMapOf(
	)
{
	int		i;
	Node*	p = NULL;
	Node*	q = NULL;
	
	for (i = 0; i < m_nArraySize; i++) {
		p = m_HashArray [i];

		while (p) {
			q = p->GetNext ();
			delete p;
			p = q;
		}
	}
	
	delete [] m_HashArray;
	m_HashArray = NULL;
	DeleteCriticalSection (&m_cs);
}

template <class _KeyType, class _DataType>
void
THashedMapOf <_KeyType, _DataType>::Lock(
	)
{
	EnterCriticalSection (&m_cs);
}

template <class _KeyType, class _DataType>
void
THashedMapOf <_KeyType, _DataType>::UnLock(
	)
{
	LeaveCriticalSection (&m_cs);
}

template <class _KeyType, class _DataType>
BOOL
THashedMapOf <_KeyType, _DataType>::Insert(
	_KeyTypeRef	key,
	_DataType	data
	)
{
	Node*	node = NULL;
	Node*	pCur = NULL;
	Node*	pNext= NULL;
	BOOL	fSuccess = FALSE;

	Lock ();
	
	MaybeRehash ();

	node = new Node (key, data);

	fSuccess = InsertNode (m_HashArray, node, m_nArraySize);

	node = NULL;
	
	if (!fSuccess) {
		delete node;
	} else {
		m_nElements++;
	}

	UnLock ();

	return fSuccess;
}


template <class _KeyType, class _DataType>
BOOL
THashedMapOf <_KeyType, _DataType>::InsertNode(
	Node*	hashArray [],
	Node*	pNode,
	int		nArraySize
	)
{
	int		cmp;
	ULONG	n;
	Node*	pCur  = NULL;
	Node*	pPrev = NULL;
	BOOL	fSucc = FALSE;

	Lock ();
	
	n = Hash (pNode->GetKey ()) % nArraySize;

	if (hashArray [n] == NULL) {
		hashArray [n] = pNode;
		fSucc = TRUE;
	} else {
		pCur = hashArray [n];
		pPrev = NULL;
		
		while (pCur->GetNext () &&
			   Compare (pCur->GetKey (), pNode->GetKey ()) > 0) {
			   
			pPrev = pCur;
			pCur = pCur->GetNext ();
		}

		cmp = Compare (pCur->GetKey (), pNode->GetKey ());

		if (cmp == 0) {
			fSucc = FALSE;
		} else if (cmp < 0) {

			fSucc = ReferenceHook (INSERT_HOOK, pCur->GetKey (), pNode->GetData ());

			if (fSucc) {
				
				pNode->SetNext (pCur);

				if (pPrev) {
					pPrev->SetNext (pNode);
				} else {
					hashArray [n] = pNode;
				}

				fSucc = TRUE;
			}
		}
	}

	UnLock ();

	return fSucc;
}

template <class _KeyType, class _DataType>
BOOL
THashedMapOf <_KeyType, _DataType>::Find(
	IN	_KeyTypeRef	key,
	OUT	_DataType*	data
	)
{
	ULONG		n;
	Node*		pCur  = NULL;

	Lock ();
	
	n = Hash (key) % m_nArraySize;

	pCur = m_HashArray [n];

	while (pCur && Compare (pCur->GetKey (), key) > 0) {
		pCur = pCur->GetNext ();
	}

	if (pCur && Compare (pCur->GetKey (), key) != 0) {
		pCur = NULL;
	}
	
	if (pCur) {

		if (ReferenceHook (FIND_HOOK, key, pCur->GetData ())) {
			*data = pCur->GetData ();
		} else {
			pCur = NULL;
		}
	}

	UnLock ();
	
	return pCur ? TRUE : FALSE;
}


template <class _KeyType, class _DataType>
BOOL
THashedMapOf <_KeyType, _DataType>::Remove(
	IN	_KeyTypeRef	key,
	OUT	_DataType*	data	OPTIONAL
	)
{
	int			cmp;
	ULONG		n;
	BOOL		fSucc = FALSE;
	Node*		pCur = NULL;
	Node*		pPrev= NULL;

	Lock ();

	n = Hash (key) % m_nArraySize;

	if (m_HashArray [n]) {
		pCur = m_HashArray [n];
		pPrev = NULL;
		
		while (pCur->GetNext () && Compare (pCur->GetKey (), key) > 0)	{
			pPrev = pCur;
			pCur = pCur->GetNext ();
		}

		cmp = Compare (pCur->GetKey (), key);

		if (cmp == 0) {

			if (ReferenceHook (REMOVE_HOOK, pCur->GetKey (), pCur->GetData ())) {

				if (pPrev == NULL) {
					m_HashArray [n] = m_HashArray [n]->GetNext ();
				} else {
					pPrev->SetNext (pCur->GetNext ());
				}
			}
		}
	}

	if (pCur) {
		fSucc = TRUE;
		if (data) {
			*data = pCur->GetData ();
		}
		delete pCur;
		pCur = NULL;
		m_nElements--;
	} else {
		fSucc = FALSE;
	}

	MaybeRehash ();
	UnLock ();
	
	return fSucc;
}

	

template <class _KeyType, class _DataType>
void
THashedMapOf <_KeyType, _DataType>::MaybeRehash(
	)
{
	Node**	hashArray = NULL;
	Node*	pNode = NULL;
	Node*	pNext = NULL;
	
	int		newArraySize;
	float	factor;
	int		i;

	Lock ();
	
	factor = ((float) m_nElements) / ((float) m_nArraySize);
	
	if ( factor > m_HighWaterMark || factor < m_LowWaterMark ) {	
		newArraySize = FindPrimeSuccessor (m_nElements);
	} else {
		newArraySize = m_nArraySize;
	}

	if (newArraySize != m_nArraySize) {

		hashArray = new Node* [newArraySize];
		memset (hashArray, 0, newArraySize * sizeof (hashArray [0]));

		for (i = 0; i < m_nArraySize; i++) {
			pNode = m_HashArray [i];

			while (pNode) {
				pNext = pNode->GetNext ();
				pNode->SetNext (NULL);
				Verify (InsertNode (hashArray, pNode, newArraySize));
				pNode = pNext;
			}
		}

		delete [] m_HashArray;
		m_HashArray = hashArray;
		hashArray = NULL;
		m_nArraySize = newArraySize;
	}

	UnLock ();
}


template <class _Type>
int
Bsearch(
	_Type	array [],
	int		nElements,
	_Type	n
	)
{
	BOOL	fDone = FALSE;
	int		low;
	int		high;
	int		i;

	//
	// return -1 for out of bounds
	//
	
	if (n < array [0] || n > array [nElements - 1]) {
		return -1;
	}
	
	low = 0;
	high = nElements;

	do {

		i = (high + low) / 2;

		if (i + 1 < nElements && array [i] < n && array [i + 1] > n) {
			fDone = TRUE;
		} else if (array [i] < n) {
			low = i;
		} else if (array [i] > n) {
			high = i;
		} else if (array [i] == n) {
			fDone = TRUE;
		}

	} while (!fDone);

	return i;
}


template <class _KeyType, class _DataType>
ULONG
THashedMapOf <_KeyType, _DataType>::FindPrimeSuccessor(
	ULONG	n
	)
/*++

Routine Description:

	This routine returns a prime number following the number n.  The return
	value is guarenteed not to exceed 2 * n.

--*/
{
	int		i;
	int		nEle = nPrimes;
	ULONG	q = 0;
	

	if (n < Primes [0]) {
		q = Primes [0];
	} else if (n > Primes [nEle - 1]) {
		q = Primes [nEle - 1];
	} else {
		i = Bsearch (Primes, nEle, n);
		q = Primes [i + 1];
	}

	return q;
}



//
// THashIterator template
//

template <class _KeyType, class _DataType>
THashIterator <_KeyType, _DataType>::THashIterator(
	Map*	map
	)
  : m_map (map)
{
	Create ();
}

template <class _KeyType, class _DataType>
THashIterator <_KeyType, _DataType>::THashIterator(
	Map&	map
	)
  : m_map (&map)
{
	Create ();
}


template <class _KeyType, class _DataType>
void
THashIterator <_KeyType, _DataType>::Create(
	)
{
	Assert (m_map);
//		Verify (m_map->IteratorLock ());
	ResetToHead ();
}
	


template <class _KeyType, class _DataType>
BOOL
THashIterator <_KeyType, _DataType>::MoveNext(
	)
{
	BOOL	ret;
	
	m_map->Lock ();
	
	if (m_pNode) {

		m_pNode = m_pNode->GetNext ();

	}

	if (!m_pNode) {

		m_bucket++;
		
		while ( m_map->m_HashArray [m_bucket] == NULL &&
				m_bucket < m_map->m_nArraySize) {

			m_bucket++;
		}

		//
		// No more entries.
		//
		
		if (m_bucket == m_map->m_nArraySize) {
			ret = FALSE;
		} else {
			m_pNode = m_map->m_HashArray [m_bucket];
			ret = TRUE;
		}
	}

	if (m_pNode) {
		ret = TRUE;
	}
	
	m_map->UnLock ();
	return ret;
}


template <class _KeyType, class _DataType>
BOOL
THashIterator <_KeyType, _DataType>::GetValue(
	OUT _DataType*	data
	)
{
	BOOL	ret;
	
	m_map->Lock ();

	if (m_pNode) {
		*data = m_pNode->GetData ();
		ret = TRUE;
	} else {
		ret = FALSE;
	}

	return ret;
}


template <class _KeyType, class _DataType>
BOOL
THashIterator <_KeyType, _DataType>::ResetToHead(
	)
{
	m_bucket = -1;	// this is kindof weird, but correct.
	m_pNode = NULL;
	return MoveNext ();
}


#if 0
#include <windows.h>
#include <stdio.h>

void
THashedMapOf::Dump(
	)
{
	THashNode*	p = NULL;
	int			i = 0;
	char		buffer [512];

	__try {
	
		OutputDebugString ("<< Dump of hash table >>\n\n");
		for (i = 0; i < m_nArraySize; i++) {
			p = m_HashArray [i];

			
			sprintf (buffer, "bucket %d\n", i);
			OutputDebugString (buffer);
			while (p) {
				sprintf (buffer, "  p %#x key %d data %#x\n",
						p,
						p->GetKey (),
						p->GetData ()
						);

				OutputDebugString (buffer);
				p = p->GetNext ();
			}
		}
	}

	__except (1) {
		sprintf (buffer, "exception dumping bucket %d, p = %#x\n", i, p);
		OutputDebugString (buffer);
	}
		
		
}

#endif


		
#endif // _Hash_inl_
