#ifndef __MEMREF_H__
#define __MEMREF_H__

#if defined(_DEBUG)
#define	MRDebug(x)	x
#else
#define	MRDebug(x)
#endif

/////////////////////////////////////////////////////////////////////////////
// CRefObj--handles ref counted garbage collection and dyncasting

class CRefObj
{
private:
	unsigned _cUses;

public:
	CRefObj() { _cUses = 0; }

	// copy ctor, does not copy usage count of object being copied from
	CRefObj(const CRefObj &) { _cUses = 0; }

	// virtual dtor should cause all descendents of CRefObj to have a virtual dtor
	virtual ~CRefObj() { }

	CRefObj &operator= (const CRefObj &) { return *this; }

	unsigned CUses() { return _cUses; }

	void Use() { _cUses++; }

	BOOL FUnUse() { return !(--_cUses); }
};

/////////////////////////////////////////////////////////////////////////////
// CRefMem

class CRefMem : public CRefObj
{
private:
	VOID * _pv;
	size_t _cbAlloc;
	size_t _cbUsed;
	HANDLE _hHeap;

	inline void Uninit();

public:
	CRefMem(HANDLE hHeap = 0)
	{
		_pv = 0;
		_cbAlloc = 0;
		_cbUsed = 0;
		_hHeap = hHeap;
	}

	~CRefMem() { Uninit(); }

	inline BOOL FInit(size_t cb);
	inline BOOL FRealloc(size_t cb);	// If the allocation succeeds, the old
										// data will be copied to the new address;
										// otherwise, the original allocation is
										// left intact.

	operator BYTE *() { return ((BYTE *) _pv); }
	operator VOID *() { return _pv; }
	operator TCHAR *() { return (TCHAR *) _pv; }

	operator const BYTE *() const { return (const BYTE *) _pv; }
	operator const VOID *() const { return (const VOID *) _pv; }
	operator const TCHAR *() const { return (const TCHAR *) _pv; }

	size_t CbAlloc() { return _cbAlloc; }

	size_t CbUsed() { return _cbUsed; }

	void SetCbUsed(size_t cb) 
	{
		if (cb < _cbAlloc)
			_cbUsed = cb;
		else
			_cbUsed = _cbAlloc;
	}
};

/////////////////////////////////////////////////////////////////////////////

#define refNil	0

template <class TARG> class Ref
{
private:
	TARG *_ptarg;

	// unuse pointer and free if last user
	VOID Release()
	{
		if (_ptarg != NULL && _ptarg->FUnUse())
			delete _ptarg;
		// put garbage in pointer
		MRDebug(_ptarg = (TARG *)0xdadadada);
	}

public:
	// constructors
	Ref<TARG>() { _ptarg = NULL; }

	Ref<TARG>(TARG *ptargNew)
	{
		_ptarg = ptargNew;
		if (ptargNew != NULL)
			ptargNew->Use();
	}

	Ref<TARG>(const Ref<TARG> &rtarg)
	{
		_ptarg = rtarg._ptarg;
		if (rtarg._ptarg != NULL)
			rtarg._ptarg->Use();
	}

	// destructor
	~Ref<TARG>() { Release(); }

	// assignment functions (mirror the ctors)
	Ref<TARG>& operator=(TARG *ptargNew)
	{
		if (ptargNew != NULL)
			ptargNew->Use();
		Release();
		_ptarg = ptargNew;
		return *this;
	}

	Ref<TARG>& operator=(const Ref<TARG> &rtarg)
	{
		if (rtarg._ptarg != NULL)
			rtarg._ptarg->Use();
		Release();
		_ptarg = rtarg._ptarg;
		return *this;
	}

	BOOL  operator==(TARG *ptarg) const { return _ptarg == ptarg; }
	BOOL  operator!=(TARG *ptarg) const { return _ptarg != ptarg; }
	TARG &operator*()			  const { return *_ptarg; }
	TARG *operator->()			  const { return _ptarg; }
		  operator TARG *()		  const { return _ptarg; }
};

/////////////////////////////////////////////////////////////////////////////

typedef Ref<CRefMem> RMEM;

inline BOOL MEM_FCreate(RMEM &refMem, size_t cb, HANDLE hHeap = 0)
{
	BOOL f = FALSE;
	CRefMem *pMem = new CRefMem(hHeap);

	if (pMem)
	{
		if (!(f = pMem->FInit(cb)))
		{
			delete pMem;
			pMem = 0;
		}
	}
	refMem = pMem;
	return f;
}

inline BOOL CRefMem::FInit(size_t cb)
{
	BOOL f = FALSE;
	if (_pv && _cbAlloc)
		Uninit();
	if (_hHeap)
		f = (_pv = HeapAlloc(_hHeap, HEAP_ZERO_MEMORY, cb)) != NULL;
	else
		f = (_pv = calloc( 1, cb)) != NULL;
	MRDebug((f ? memset(_pv, 0xcc, cb) : 0));
	if (f)
		_cbAlloc = _cbUsed = cb;
	return f;
}

inline void CRefMem::Uninit()
{
	if (_pv)
	{
		MRDebug(memset(_pv, 0xda, _cbAlloc));
		if (_hHeap)
			HeapFree(_hHeap, 0, _pv);
		else
			free(_pv);
		_pv = 0;
		_cbAlloc = _cbUsed = 0;
	}
}

inline BOOL CRefMem::FRealloc(size_t cb)
{
	BOOL f = FALSE;

	if (_pv)
	{
		VOID *pvT;
		if (_hHeap)
			pvT = HeapAlloc(_hHeap, HEAP_ZERO_MEMORY, cb);
		else
			pvT = calloc(1, cb);
		if (pvT)
		{
			memcpy(pvT, _pv, min(cb, _cbAlloc));
			MRDebug(memset(_pv, 0xda, _cbAlloc));
			if (_hHeap)
				HeapFree(_hHeap, 0, _pv);
			else
				free(_pv);
			MRDebug((cb > _cbAlloc ? memset((BYTE *)pvT + _cbAlloc, 0xcc, cb - _cbAlloc) : 0));
			_cbAlloc = cb;
			if (_cbUsed > cb)
				_cbUsed = cb;
			_pv = pvT;
			f = TRUE;
		}		
	}
	else
		f = FInit(cb);

	return f;
}

#endif	// __MEMREF_H__
