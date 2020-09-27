/*************************************************************************
 	msoalloc.h

 	Owner: rickp
 	Copyright (c) 1994 Microsoft Corporation

	Standard memory manager routines for the Office world.
*************************************************************************/

#if !defined(CMALLOC_H)
#define CMALLOC_H

/*************************************************************************
	The Plex data structure.  A plex is a low-overhead implementation
	of a varible-sized array.
*************************************************************************/

/* The generic plex structure itself */
typedef struct MSOPX
{
	/* WARNING: the following must line up with the MSOTPX template and
		the MsoPxStruct macro */
	WORD iMac, iMax;	/* used size, and total allocated size */
	unsigned cbItem:16,	/* size of each data item */
		dAlloc:15,	/* amount to grow by when reallocating larger */
		fUseCount:1;	/* if items in the plex should be use-counted */
	int dg;	/* data group to allocate out of */
	BYTE* rg;	/* the data */
} MSOPX;


/*	Handy macro for declaring a named Plex structure - must line up with
	the MSOPX structure */
#define MsoPxStruct(TYP,typ) \
		struct \
		{ \
		WORD i##typ##Mac, i##typ##Max; \
		unsigned cbItem:16, \
			dAlloc:15, \
			fUseCount:1; \
		int dg; \
		TYP *rg##typ; \
		}

/* Handy macro for enumerating over all the items in a plex ppl, using loop
	variables p and pMac */
#define FORPX(p, pMac, ppl, T) \
		for ((pMac) = ((p) = (T*)((MSOPX*)(ppl))->rg) + ((MSOPX*)(ppl))->iMac; \
			 (p) < (pMac); (p)++)

/* Handy macro for enumerating over all the items in a plex ppl backwards,
	using loop variables p and pMac */
#define FORPX2(p, pMac, ppl, T) \
		for ((p) = ((pMac) = (T*)((MSOPX*)(ppl))->rg) + ((MSOPX*)(ppl))->iMac - 1; \
			 (p) >= (pMac); (p)--)


/*************************************************************************
	Creation and destruction
*************************************************************************/

MSOAPI_(BOOL) MsoFInitPx(void* pvPx, int dAlloc, int iMax);

/*************************************************************************
	Lookups
*************************************************************************/

typedef int (MSOPRIVCALLTYPE* MSOPFNSGNPX)(const void*, const void*);

MSOAPIXX_(void*) MsoPLookupPx(void* pvPx, const void* pvItem, MSOPFNSGNPX pfnSgn);
MSOAPI_(BOOL) MsoFLookupPx(void* pvPx, const void* pvItem, int* pi, MSOPFNSGNPX pfnSgn);

MSOAPI_(int) MsoFLookupSortPx(void* pvPx, const void* pvItem, int* pi, MSOPFNSGNPX pfnSgn);


/*************************************************************************
	Adding items
*************************************************************************/

#define MsoFAppendPx(pvPx, pv) (MsoIAppendPx(pvPx, pv) >= 0)
MSOAPI_(int) MsoIAppendPx(void* pvPx, const void* pv);
MSOAPI_(int) MsoIAppendPx(void* pvPx, const void* pv);
MSOAPI_(int) MsoIAppendUniquePx(void* pvPx, const void* pv, MSOPFNSGNPX pfnSgn);

MSOAPI_(int) MsoIInsertSortPx(void* pvPx, const void* pv, MSOPFNSGNPX pfnSgn);
MSOAPI_(BOOL) MsoFInsertPx(void* pvPx, const void* pv, int i);
MSOAPIXX_(BOOL) MsoFInsertExPx(void* pvPx, const void* pv, int i);

/*************************************************************************
	Removing items
*************************************************************************/

MSOAPI_(int) MsoFRemovePx(void* pvPx, int i, int c);
MSOAPI_(void) MsoDeletePx(void* pvPx, int i, int c);


/*************************************************************************
	Miscellaneous shuffling around
*************************************************************************/

MSOAPIXX_(void) MsoMovePx(void* pvPx, int iFrom, int iTo);
MSOAPI_(BOOL) MsoFCompactPx(void* pvPx, BOOL fFull);
MSOAPI_(BOOL) MsoFResizePx(void* pvPx, int iMac, int iIns);
MSOAPI_(BOOL) MsoFGrowPx(void* pvPx, int iMac);
MSOAPI_(void) MsoStealPx(void *pvPxSrc, void *pvPxDest);
MSOMACAPI_(void) MsoEmptyPx(void *pvPx);
MSOMACAPI_(BOOL) MsoFClonePx(void *pvPxSrc, void *pvPxDest, int dg);


/*************************************************************************
	Plex with use count items utilities
*************************************************************************/

MSOAPI_(int) MsoIIncUsePx(void* pvPx, int i);
MSOAPI_(int) MsoIDecUsePx(void* pvPx, int i);

/*************************************************************************
	Debug stuff
*************************************************************************/

#if DEBUG
	MSOAPI_(BOOL) MsoFValidPx(const void* pvPx);
	MSOAPI_(BOOL) MsoFWritePxBe2(void* pvPx, LPARAM lParam, BOOL fSaveObj, 
											BOOL fAllocHasSize);
#else
	#define MsoFValidPx(pvPx) (TRUE)
#endif



/*************************************************************************
	Plex class template - this is basically a big inline class wrapper 
	around the C plex interface.
*************************************************************************/

#ifdef __cplusplus

template <class S> class MSOTPX
{
public:
	/* WARNING: the following must line up exactly with the MSOPX structure */
	WORD iMac, iMax;	/* used size, and total allocated size */
	unsigned cbItem:16,	/* size of each data item */
		dAlloc:15,	/* amount to grow by when reallocating larger */
		fUseCount:1;	/* if items in the plex should be use-counted */
	int dg;	/* data group to allocate out of */
	S* rg;	/* the data */

	/* Unexciting constructor. */
	inline MSOTPX<S>(void) 
	{ iMax = iMac = 0; cbItem = sizeof(S); rg = NULL; }

	/* Destructor to deallocate memory */
	inline ~MSOTPX<S>(void) 
	{ if (rg) free(rg); }

	inline BOOL FValid(void) const
	{ return MsoFValidPx(this); }

	inline BOOL FInit(int dAlloc, int iMax)
	{ return MsoFInitPx(this, dAlloc, iMax); }

	inline int IMax(void) const
	{ return iMax; }

	inline int IMac(void) const
	{ return iMac; }

	inline S* PGet(int i) const
	{  return &rg[i]; }

	inline void Get(S* p, int i) const
	{	AssertMsgTemplate(i >=0 && i < iMac, NULL); *p = rg[i]; }

	inline void Put(S* p, int i)
	{	AssertMsgTemplate(i >=0 && i < iMac, NULL); rg[i] = *p; }

	// plex[i] has exactly the same semantics and performance as plex.rg[i]
	inline S& operator[](int i)
	{	AssertMsgTemplate(i >=0 && i < iMac, NULL); return rg[i]; }

	inline S* PLookup(S* pItem, MSOPFNSGNPX pfnSgn)
	{ return (S*)MsoPLookupPx(this, pItem, pfnSgn); }

	inline BOOL FLookup(S* pItem, int* pi, MSOPFNSGNPX pfnSgn)
	{ return MsoFLookupPx(this, pItem, pi, pfnSgn); }

	inline BOOL FLookupSort(S* pItem, int* pi, MSOPFNSGNPX pfnSgn)
	{ return MsoFLookupSortPx(this, pItem, pi, pfnSgn); }

	inline int FAppend(S* p)
	{ return MsoIAppendPx(this, p) != -1; }

	inline int IAppend(S* p)
	{ return MsoIAppendPx(this, p); }

	inline int IAppendUnique(S* p, MSOPFNSGNPX pfnSgn)
	{ return MsoIAppendUniquePx(this, p, pfnSgn); }

	inline BOOL FInsert(S* p, int i)
	{ return MsoFInsertPx(this, p, i); }

	inline int FInsertEx(S* p, int i)
	{ return MsoFInsertExPx(this, p, i); }

	inline int IInsertSort(S* p, MSOPFNSGNPX pfnSgn)
	{ return MsoIInsertSortPx(this, p, pfnSgn); }

	inline int FRemove(int i, int c)
	{ return MsoFRemovePx(this, i, c); }

	inline void Delete(int i, int c)
	{ MsoDeletePx(this, i, c); }

	inline void Move(int iFrom, int iTo)
	{ MsoMovePx(this, iFrom, iTo); }

	inline BOOL FCompact(BOOL fFull)
	{ return MsoFCompactPx(this, fFull); }

	inline BOOL FResize(int iMac, int iIns)
	{ return MsoFResizePx(this, iMac, iIns); }

	inline BOOL FReplace(S* p, int i)
	{ 
		if (i >= iMac && !FSetIMac(i+1))
			return FALSE; 
		rg[i] = *p;
		return TRUE;
	}

	inline BOOL FSetIMac(int iMac)
	{ return MsoFResizePx(this, iMac, -1); }

	inline BOOL FSetIMax(int iMax)
	{ return MsoFGrowPx(this, iMax); }

	inline int IIncUse(int i)
	{ return MsoIIncUsePx(this, i); }

	inline int IDecUse(int i)
	{ return MsoIDecUsePx(this, i); }

	inline void Steal(void *pvPxSrc)
	{ MsoStealPx(pvPxSrc, this); }

	inline void Empty()
	{ MsoEmptyPx(this); }

	inline BOOL FClone(void *pvPxDest, int dg)
	{ return MsoFClonePx(this, pvPxDest, dg); }

#if DEBUG
	inline BOOL FWriteBe(LPARAM lParam, BOOL fSaveObj)
	{ return MsoFWritePxBe2(this, lParam, fSaveObj, TRUE); }
#endif

};

#endif /* __cplusplus */


#endif /* CMALLOC_H */
