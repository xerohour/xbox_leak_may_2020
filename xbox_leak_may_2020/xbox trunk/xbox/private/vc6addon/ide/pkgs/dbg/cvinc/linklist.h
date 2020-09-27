#ifndef _LINKLIST_
#define _LINKLIST_

#if !(defined (OSDEBUG4))
//
//	Return values from lpfnCmpNode functions
//
#define	fCmpLT		-1
#define	fCmpEQ		0
#define	fCmpGT		1
//
// Magical linklist types
//
#define	HLLI	HMEM
#define	HLLE	HMEM
//
//	Prototypes for callbacks
//
typedef void	(FAR PASCAL * LPFNKILLNODE)( LPV );
typedef int		(FAR PASCAL * LPFNFCMPNODE)( LPV, LPV, LONG );
//
// List flags
typedef WORD	LLF;
#define	llfNull				(LLF)0x0
#define	llfAscending		(LLF)0x1
#define	llfDescending		(LLF)0x2
#endif	// OSDEBUG4
//
//
// -- Here's the APIs
//
extern	HLLI	PASCAL	LLHlliInit( UINT, LLF, LPFNKILLNODE, LPFNFCMPNODE );
extern	HLLE	PASCAL	LLHlleCreate( HLLI );
extern	void	PASCAL	LLAcquire( HLLI );
extern	void	PASCAL	LLRelease( HLLI );
extern	void	PASCAL	LLAddHlleToLl( HLLI, HLLE );
extern	void	PASCAL	LLInsertHlleInLl( HLLI, HLLE, DWORD );
extern	BOOL	PASCAL	LLFDeleteHlleIndexed( HLLI, DWORD );
extern	BOOL	PASCAL	LLFDeleteLpvFromLl( HLLI, HLLE, LPV, DWORD );
extern	BOOL	PASCAL	LLFDeleteHlleFromLl( HLLI, HLLE );
extern	HLLE	PASCAL	LLHlleFindNext( HLLI, HLLE );

#ifndef BMHANDLES
#define LLHlleFindNextFast(hlli, hlle) \
	(HLLE)((hlle) ? \
	       ((LPLLE)(hlle))->hlleNext : \
			LLHlleFindNext(hlli, hlle))
#else
#define LLHlleFindNextFast(HLLI, HLLE) LLHlleFindNext(HLLI, HLLE)
#endif

#ifdef DBLLINK
extern	HLLE	PASCAL	LLHlleFindPrev( HLLI, HLLE );
#endif	// DBLLINK
extern	DWORD	PASCAL	LLChlleDestroyLl( HLLI );
extern	HLLE	PASCAL	LLHlleFindLpv( HLLI, HLLE, LPV, DWORD );
extern	DWORD	PASCAL	LLChlleInLl( HLLI );
extern	LPV		PASCAL	LLLpvFromHlle( HLLE );
extern  HLLE    PASCAL  LLHlleGetLast( HLLI );
extern	void	PASCAL	LLHlleAddToHeadOfLI( HLLI, HLLE );
extern	BOOL	PASCAL	LLFRemoveHlleFromLl( HLLI, HLLE );

//
// FCheckHlli is for debug versions ONLY as an integrity check
//
#ifdef _DEBUG
extern	BOOL  PASCAL  LLFCheckHlli( HLLI );
#else	// _DEBUG
#define LLFCheckHlli(hlli)	1
#endif	// _DEBUG
//
// Map memory manager to our source versions
//
#ifdef CODEVIEW
    #define AllocHmem(cb)   BMAlloc(cb) // _fmalloc(cb)
    #define FreeHmem(h)     BMFree(h)          // _ffree(h)
    #define LockHmem(h)     BMLock(h)          // (h)
    #define UnlockHmem(h)   BMUnlock(h)          //
#else	// !CODEVIEW
	HMEM		LOADDS PASCAL MHMemAllocate(unsigned int);
	void		LOADDS PASCAL MHMemFree(HMEM);
	void FAR*	LOADDS PASCAL MHMemLock(HMEM);
	void		LOADDS PASCAL MHMemUnLock(HMEM);
	#define AllocHmem(cb)	MHMemAllocate(cb)
	#define FreeHmem(h)		MHMemFree(h)
	#define LockHmem(h)		MHMemLock(h)
	#define UnlockHmem(h)	MHMemUnLock(h)
#endif	// !CODEVIEW
//
//	This helps the codes appearance!
//
#define	UnlockHlle(hlle)	UnlockHmem(hlle)
#define	UnlockHlli(hlli)	UnlockHmem(hlli)

#ifndef hlleNull
#define hlleNull	(HLLE)NULL
#endif	// !hlleNull

#endif	// _LINKLIST_

