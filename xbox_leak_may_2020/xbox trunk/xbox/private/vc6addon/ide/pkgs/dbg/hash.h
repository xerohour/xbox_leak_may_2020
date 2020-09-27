/*++

Copyright (c) 1997 Microsoft Corporation

Module Name:

	hash.h
	
Abstract:

	This is a self-adjusting hash table package.  It provides the following
	interfaces and templates.

	  o interface IMappingOf <_KeyType, _DataType>.

		This is an interface that describes any type of mapping which
		supports Insert, Find and Remove operators (like the hash table).

	  o template THashedMapOf <_KeyType, _DataType>

		This template maps an object of type KeyType into an object of type
		DataType.  The following things should be considered when using this
		type: the key is constructed in the node data structure internal to
		the hash node.  Because of this, the key should not be too heavy
		weight of an object.  The key object must support the operations:
		copy constructor and destructor.  Type DataType object must support
		copy constructor, destructor and assignment operator.  In general it
		may be advisable limit both Key and Data to fairly simple types that
		trivially implement the above operators (simple C data types plus
		pointers of all kinds are good), or to objects where the user knows
		the cost of constructing, destructing and copying the object and that
		cost is low -- a refcounted string class like MFCs CString class is
		a good example.


Author:

	Matthew D Hendel (math) 13-May-1997


Example Usage:

	ULONG
	THashedMapOf<PID, CProcess*>::Hash(
		const PID&	pid
		)
	{
		return (ULONG) pid;
	}

	int
	THashedMapOf<PID, CProcess*>::Compare(
		const PID&	pid1,
		const PID&	pid2
		)
	{
		return  ((int) pid1) - ((int) pid2) ;
	}
		


	THashedMapOf<PID, CProcess*>	ProcessTable;

	CProcess*	proc;

	proc = new CProcess;
	ProcessTable.Insert (pid, proc);

	ProcessTable.Find (pid, &proc);
	ProcessTable.Remove (pid, &proc);

--*/


#ifndef _Hash_h_
#define _Hash_h_

//#include "defs.h"

#ifndef Verify
#define Verify VERIFY
#endif

#ifndef Assert
#define Assert ASSERT
#endif


template <class _KeyType, class _DataType> class THashedMapOf;
template <class _KeyType, class _DataType> class THashNode;
template <class _KeyType, class _DataType> class THashIterator;

template <class _KeyType, class _DataType>
interface IMappingOf
{
	typedef const _KeyType& _KeyTypeRef;
	
	virtual BOOL
	Insert(
		IN	_KeyTypeRef		key,
		IN	_DataType		data
		) = 0;

	virtual BOOL
	Remove(
		IN	_KeyTypeRef		key,
		OUT _DataType*		data    OPTIONAL
		) = 0;

	virtual BOOL
	Find(
		IN	_KeyTypeRef		key,
		OUT	_DataType*		data
		) = 0;
};


//
// This is a private type of the THashMapping.  But due to the use of
// templates it needs to be declared here in the global namespace.
//

template <class _KeyType, class _DataType>
class THashNode
{
  private: // types

	typedef const _KeyType&	_KeyTypeRef;
	typedef const _DataType& _DataTypeRef;
	
  private:
  
	_KeyType	m_key;
	_DataType	m_data;
	THashNode*	m_next;

  public:

	THashNode(
		_KeyTypeRef		key,
		_DataTypeRef	data
		)
	  : m_key (key),
	    m_data (data),
		m_next (NULL)
	{
	}
		
	_DataTypeRef
	GetData(
		)
	{
		return m_data;
	}

	_KeyTypeRef
	GetKey(
		)
	{
		return m_key;
	}

	THashNode*
	GetNext(
		)
	{
		return m_next;
	}
	
	THashNode*
	SetNext(
		THashNode*	node
		)
	{
		THashNode*	t = m_next;
		m_next = node;
		return t;
	}
};


  

template <class _KeyType, class _DataType>
class THashedMapOf : public IMappingOf <_KeyType, _DataType>
{
	friend THashIterator <_KeyType, _DataType>;

  private:  // local types

	typedef THashNode <_KeyType, _DataType>	Node;
	
  private:  // local data
  
	int			m_nArraySize;
	int			m_nElements;
	Node**		m_HashArray;
	float		m_HighWaterMark;
	float		m_LowWaterMark;

	CRITICAL_SECTION	m_cs;

  public: // types
  
	enum  REFERENCE_TYPE
	{
		INSERT_HOOK,		// just before an element is added to the list
		REMOVE_HOOK,		// just before an element is removed from the list
		FIND_HOOK			// just before a found object is returned from
	};						// a find operation
	
  public:  // construction & destruction

	THashedMapOf(
		ULONG		nInitialSize = 512,
		float		highWaterMark = 2.0, 
		float		lowWaterMark = 0.0
		);

	~THashedMapOf(
		);


  public:  // public interface	

	BOOL
	Insert(
		IN _KeyTypeRef	key,
		IN _DataType	data
		);

	BOOL
	Remove(
		IN	_KeyTypeRef		key,
		OUT _DataType*		data		OPTIONAL
		);

	BOOL
	Find(
		IN	_KeyTypeRef		key,
		OUT _DataType*		data
		);

	void
	Lock(
		);

	void
	UnLock(
		);

	ULONG
	GetCount(
		)
	{
		return m_nElements;
	}

  public:

	virtual ULONG
	Hash(
		_KeyTypeRef	key
		);

	virtual int
	Compare(
		_KeyTypeRef	key1,
		_KeyTypeRef	key2
		);

	virtual BOOL
	ReferenceHook(
		REFERENCE_TYPE		rtype,
		_KeyTypeRef			key,
		_DataType			data
		)
	{
		return TRUE;
	}

  public: // debugging functions
		
	void
	Dump(
		);
		

  private:	// local methods


	void
	MaybeRehash(
		);

	BOOL
	InsertNode(
		Node*	hashArray [],
		Node*	pNode,
		int		nArraySize
		);

	ULONG
	FindPrimeSuccessor(
		ULONG	n
		);
	
};


//
// Allow you to iterate through the Hash table.  Problems: the hash-table
// iterator does not support multiple iterators on the table at one time
// (you will crash and burn); also, the iteration of the hash table is slow
// and should considered only for functions where speed is not of the
// essence.
//

template <class _KeyType, class _DataType>
class THashIterator
{

  private: // types

  typedef THashedMapOf <_KeyType, _DataType> Map;
  typedef THashNode <_KeyType, _DataType>	Node;

	Map*	m_map;
	Node*	m_pNode;
	int		m_bucket;

  public: // public construction + destruction

	THashIterator(
		Map*	map
		);

	THashIterator(
		Map&	map
		);

	void
	Create(
		);
		
  public: // operators

	BOOL
	MoveNext(
		);

	BOOL
	GetValue(
		OUT _DataType*	data
		);

	BOOL
	ResetToHead(
		);
};


ULONG
HashPjw(
	LPCSTR	sz
	);


#include "hash.inl"

#endif // _Hash_h_
