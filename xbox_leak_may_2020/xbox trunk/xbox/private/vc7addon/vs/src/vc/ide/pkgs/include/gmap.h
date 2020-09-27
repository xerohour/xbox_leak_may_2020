#if _MSC_VER > 1000
#pragma once
#endif

#ifndef INCLUDED_GMAP_H
#define INCLUDED_GMAP_H

#include <afx.h> // CObject
#include <afxtempl.h> // CMap
#include <limits.h> // INT_MAX

/* these are typedef's, not types.

	// GrowingMaps (aka Dictionaries)
	class CGrowingMapWordToOb;         // map from WORD to CObject*
	class CGrowingMapWordToPtr;        // map from WORD to void*
	class CGrowingMapPtrToWord;        // map from void* to WORD
	class CGrowingMapPtrToPtr;         // map from void* to void*

	// Special String variants
	class CGrowingMapStringToPtr;      // map from CString to void*
	class CGrowingMapStringToOb;       // map from CString to CObject*
	class CGrowingMapStringToString;   // map from CString to CString

*/

template<class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CGrowingMap :
	public CMap<KEY, ARG_KEY, VALUE, ARG_VALUE>
{
};

// These typedefs probably cause actual repeated compile time instantiation followed
// by link time dead stripping. They should probably be removed, or changed
// back to declarations with one time instantiation in a .cpp.

//typedef
//	CGrowingMap<CMapWordToOb, WORD, WORD, CObject*, CObject*>
//	CGrowingMapWordToOb;
typedef
	CGrowingMap<WORD, WORD, void*, void*>
	CGrowingMapWordToPtr;
typedef
	CGrowingMap<void*, void*, WORD, WORD>
	CGrowingMapPtrToWord;
typedef
	CGrowingMap<void*, void*, void*, void*>
	CGrowingMapPtrToPtr;
typedef
	CGrowingMap<CString, LPCTSTR, void*, void*>
	CGrowingMapStringToPtr;
//typedef
//	CGrowingMap<CString, LPCTSTR, CObject*, CObject*>
//	CGrowingMapStringToOb;
typedef
	CGrowingMap<CString, LPCTSTR, CString, LPCTSTR>
	CGrowingMapStringToString;


/////////////////////////////////////////////////////////////////////////////
// CTypedPtrMap<BASE_CLASS, KEY, VALUE>

// Some changes compared to CTypedPtrMap
// extra two ARG_ parameters allow CString's to work, cast through LPCTSTR

template<class BASE_CLASS, class KEY, class ARG_KEY, class VALUE, class ARG_VALUE>
class CTypedPtrGrowingMap :
	public BASE_CLASS
{
};

typedef CTypedPtrGrowingMap
<
	CGrowingMapStringToPtr,//CGrowingMap<CString, LPCTSTR, void*, void*>,
	CString, LPCTSTR,
	CObject*, CObject*
>
CGrowingMapStringToOb;

typedef CTypedPtrGrowingMap
<
	CGrowingMapWordToPtr,//CGrowingMap<WORD, WORD, void*, void*>,
	WORD, WORD,
	CObject*, CObject*
>
CGrowingMapWordToOb;

// TODO specialize some maps to not waste 4 bytes to cache a cheaply computed
// hash and recompute it while iterating, like MFC does.

// TODO reintroduce the optimizations of gmap3 and gmap4 that preinstantiated
// common maps in one place and inhibited instantiation otherwise.

#endif // the whole file
