//codecs/scrunchcore/type/basic.hpp#5 - edit change 20855 (text)
/*************************************************************************

Copyright (c) Microsoft Corporation 1996 - 1999

Module Name:

	basic.hpp

Abstract:

    Basic types:
		Data, CSite, CVector2D, CVector3D, CRct, CPixel, CMotionVector, CMatrix3x3D

Author:

	Ming-Chieh Lee (mingcl@microsoft.com) 20-March-1996
	Wei-ge Chen (wchen@microsoft.com) 20-March-1996
	Bruce Lin	(blin@microsoft.com) 14-April-1996
	Chuang Gu (chuanggu@microsoft.com) 5-June-1996

Revision History:


*************************************************************************/


#ifndef __BASIC_HPP_
#define __BASIC_HPP_

// briaking - replaced the previously convoluted stuff involving ndis.h with this:
#ifdef UNDER_CE
// find ASSERT in windows CE include files
#include <windows.h>
#include <dbgapi.h>
#define assert ASSERT
#else
#include "assert.h"
#endif

#include <string.h>

#ifdef __MAKE_DLL_
#define API __declspec (dllexport)
#define Class        class API
#else
#define API
#define Class		class
#endif // __MAKE_DLL_

#ifdef __MFC_
#include <afx.h>
#include <afxtempl.h>
#include <windowsx.h>
#endif // __MFC_

#ifdef DO_BOE
#define BOE DebugBreak()
#else
#define BOE	
#endif

#ifndef ICERR_OK
#define ICERR_OK		0L
#define ICERR_ERROR		-100L
#define	ICERR_MEMORY		-3L
#define ICERR_BADFORMAT        	-2L
#endif

#if defined(__MACVIDEO__) || defined(__UNIXVIDEO__)
#undef NOERROR
#define NOERROR	0
#undef far
#define far                 
typedef void far            *LPVOID;
#endif
typedef void *LPVoid;

#ifdef __MACAUDIO__

#define NO_WINDOWS
#define WORD_ALIGN
#define DWORD_ALIGN

#include <MacTypes.h>
//#include <Errors.h>
//#include <Endian.h>
//#include <MacMemory.h>
//#include <Resources.h>
//#include <Components.h>
//#include <Sound.h>
//#include <MoviesFormat.h>

typedef SInt32 __int32;
typedef SInt64 __int64;
#define __huge
#define E_FAIL			0x80004005L
#define E_OUTOFMEMORY	0x8007000EL
#define S_OK			0x0L
#define S_FALSE			0x1L
#define ZeroMemory(a,b)	memset(a,0,b)

typedef unsigned char BYTE;
typedef UInt32 DWORD;
typedef UInt16 WORD;
#ifndef _HRESULT
#define _HRESULT
typedef long HRESULT;
#endif
typedef long HMODULE;

#else
#ifndef _MAC
#ifndef Linux
#ifndef HITACHI
#ifndef NULL
#define NULL 0
#endif
#endif
#endif//Linux   
#define signbit(x) (((x) > 0) ? 1 : 0)
typedef char Byte;
#else
#define __huge
#endif

#endif

#define own // used as "reserved word" to indicate ownership or transfer to distinguish from const
#define TRUE 1
#define FALSE 0
#define transpValue 0
#define opaqueValue 255
#define transpPixel CPixel(0,0,0,0)
#define opaquePixel CPixel(255,255,255,255)

#define max(a,b) (((a) > (b)) ? (a) : (b))
#ifndef _MIN
#define _MIN
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#endif
#define checkRange(x, a, b) (((x) < (a)) ? (a) : ((x) > (b)) ? (b) : (x))
#define DELETE_ARRAY(ptr) if (ptr) {delete [] ptr; ptr = NULL;};
#define DELETE_PTR(ptr) if (ptr) {delete ptr; ptr = NULL;};



//NQF, for Linux, define UNIX, then, undefine UNIX at very end
#if defined(Linux) || defined(_Embedded_x86)
#define  UNIX
#undef   NULL 
#include <stddef.h>
#endif  
//NQF  


/////////////////////////////////////////////
//
//      Forward declarations for classes
//
/////////////////////////////////////////////
#ifdef __cplusplus
Class CSite;
Class CVector2D;
Class CRct;
Class CPixel; // 32 bit pixel, various interpretations depending on PixelType
Class CMotionVector;
#endif //__cplusplus

// Unix defines

#ifdef UNIX

#define NO_WINDOWS
#define WORD_ALIGN
#define DWORD_ALIGN

#ifndef _XBOX
typedef signed long __int32;
#ifndef HITACHI
typedef signed long long __int64;
typedef unsigned long long U64;
#endif
#else
typedef unsigned __int64 U64;
#endif // _XBOX
#define __huge
#define _cdecl
#define __fastcall
#define FAR
#define E_FAIL		     0x80004005L
#define E_OUTOFMEMORY	     0x8007000EL
#define S_OK		     0x0L
#define S_FALSE		     0x1L
#ifndef _XBOX
#define ZeroMemory(a,b)	memset(a,0,b)
#endif

typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef char TCHAR;
typedef long HRESULT;
typedef long HMODULE;
#ifdef HITACHI

typedef struct tQWORD
{
    DWORD   dwLo;
    DWORD   dwHi;

}   QWORD;

typedef QWORD U64;

#endif


#if !(defined(macintosh) || defined(_Embedded_x86))
	#ifndef SUNCC50
#ifndef Linux
#ifndef HITACHI
    typedef enum { false , true } bool;  
#endif
#endif
#endif
#endif  // _MAC

#endif


/////////////////////////////////////////////
//
//  Typedefs for basic types
//
/////////////////////////////////////////////

// HongCho: some platforms might not support inline assembly
#ifdef _M_IX86
#ifdef _WIN32
#define _SCRUNCH_INLINEASM
#else
// default is not supporting it...
#undef _SCRUNCH_INLINEASM
#endif // _WIN32
#endif // _M_IX86
#ifdef _ALPHA_
#define _SCRUNCH_INLINEASM
#endif // _ALPHA_
#if defined(_Embedded_x86)
#define _SCRUNCH_INLINEASM
#endif // _Embedded_x86

#if 1 // in case future compilers redefine longs and shorts
typedef unsigned long U32;

#if defined(_WIN32) || defined (_XBOX)
typedef int I32;
#else
//#if macintosh
//typedef SInt32 I32;
//#else
typedef long int I32;
//#endif
#endif

typedef unsigned short U16;
typedef short I16;
#endif // in case future compilers redefine longs and shorts

#define Sizeof(x) ((U32) sizeof (x))


#ifdef _ALPHA_ //Wide type is the widest integer register supported by the machine
typedef __int64 Wide;
typedef unsigned __int64 UWide;
#else
#ifndef UNDER_CE
#define __unaligned
#endif
//typedef __int32 Wide;
//#if macintosh
//typedef UInt32 UWide;
//#else
//typedef unsigned __int32 UWide;
typedef I32 Wide;
typedef U32 UWide;
//#endif
#endif
#ifdef macintosh
typedef long long Int64;
typedef long long I64;
#else
#ifndef HITACHI
typedef __int64 Int64;
typedef __int64 I64;
#endif
#endif

#ifdef __MACAUDIO__
typedef UInt64 U64;
#else
#ifndef UNIX
#ifdef macintosh
typedef unsigned long long U64;
#else
typedef unsigned __int64 U64;
#endif // UNIX
#endif // macintosh
#endif  //_MACAUDIO
typedef U32 UInt;
typedef I32 Int;
typedef double Double;
typedef float Float;
typedef float Float32;
typedef float F32;
#ifndef _Embedded_x86
typedef unsigned char U8;
#else
#define U8 unsigned char
#endif
typedef signed char I8;
typedef I32 Long;
typedef U32 ULong;
typedef void Void;
typedef I32 Bool;
typedef I32 CoordI;
typedef double CoordD;
typedef char Char;
typedef enum AlphaUsage {RECTANGLE = 0, ONE_BIT = 1, EIGHT_BIT = 2} AlphaUsage;
typedef enum {RGBT_ILLEGAL, RGBT_8, RGBT_555, RGBT_565, RGBT_888} RGBType;
typedef enum {red, green, blue, alpha} RGBA; // define pixel component
typedef enum BlockNum {
	ALL_Y_BLOCKS	= 0,
	Y_BLOCK1		= 1,
	Y_BLOCK2		= 2,
	Y_BLOCK3		= 3,
	Y_BLOCK4		= 4,
	U_BLOCK			= 5,
	V_BLOCK			= 6,
	A_BLOCK1		= 7,
	A_BLOCK2		= 8,
	A_BLOCK3		= 9,
	A_BLOCK4		= 10,
	ALL_A_BLOCKS	= 11
} BlockNum;
typedef enum PlaneType {Y_PLANE, U_PLANE, V_PLANE, A_PLANE, BY_PLANE, BUV_PLANE} PlaneType;
//typedef enum MotEstMode {MOTEST_VIDEO_CONFERENCE, MOTEST_PLATEAU} MotEstMode;
#define MOTEST_VIDEO_CONFERENCE 1
#define MOTEST_PLATEAU 2

//typedef Int Time;
#ifdef macintosh
typedef long long Time;
#else
typedef __int64 Time;
#endif

#ifndef __huge
#if !defined(_WIN32)
#define __huge _huge
#else
#define __huge
#endif
#endif

#if defined(_WIN16)

#define memcpy _fmemcpy
#define memset _fmemset

Void HugeCopyMemory(Void __huge *dst, const Void __huge *src, UInt count);
Void HugeSetMemory(Void __huge *dst, Int c, UInt count);

#else

#define HugeCopyMemory memcpy
#define HugeSetMemory memset

#endif


/////////////////////////////////////////////
//
//  Space
//
/////////////////////////////////////////////

#ifdef __cplusplus
Class CSite
{
public:  
	CoordI x; 
	CoordI y; 

	// Constructors
	CSite () {}
	CSite (const CSite& s) {x = s.x; y = s.y;}
	CSite (CoordI xx, CoordI yy) {x = xx; y = yy;}

	// Properties
	CoordI xCoord () const {return x;}
	CoordI yCoord () const {return y;}

	// Operators
	Void set (CoordI xx, CoordI yy) {x = xx; y = yy;}
	CSite operator + (const CSite& st) const; // Coornidate-wise +
	CSite operator - (const CSite& st) const; // Coornidate-wise -
	CSite operator * (const CSite& st) const; // Coornidate-wise *
	CSite operator * (Int scale) const; // Coornidate-wise scaling
	CSite operator / (const CSite& st) const; // Coornidate-wise /
	CSite operator / (Int scale) const; // Coornidate-wise scaling
	CSite operator % (const CSite& st) const; // Coornidate-wise %
	Void operator = (const CSite& st);

	// Synonyms
	Bool operator == (const CSite& s) const {return x == s.x && y == s.y;}
	Bool operator != (const CSite& s) const {return x != s.x || y != s.y;}
}; 

Class CVector
{
public:
	I8 x;
	I8 y;

    // constructor
    CVector () {x = y = 0;}
	CVector (I8 xx, I8 yy) {x = xx; y = yy;}
};


Class CRct
{
public:
    CoordI left, top, right, bottom;

    // Constructors
    CRct ()
	    {left = top = 0; right = bottom = -1;}
    CRct (CoordI l, CoordI t, CoordI r, CoordI b)
	    {left = l; top = t; right = r; bottom = b;}

    // Attributes
    Bool valid () const
	    {return left < right && top < bottom;} 
    Bool empty () const
	    {return left >= right || top >= bottom;}
    CoordI width () const
	    {return (right - left);}
    CoordI height () const
	    {return (bottom - top);}
    UInt area () const
	    {return (UInt) width () * height ();}
    UInt offset (CoordI x, CoordI y) const
        {return !valid () ? 0 : width () * (y - top) + (x - left);} 
    Bool operator == (const CRct& rc) const; 
    Bool operator != (const CRct& rc) const; 

    // Operations
    Void operator = (const CRct& rc);
};

/////////////////////////////////////////////
//
//  Motion Vectors
//
/////////////////////////////////////////////

Class CMotionVector
{
public:
	I8 m_iMVX; // x direction motion
	I8 m_iMVY; // y direction motion
	I8 m_iHalfX; // x direction half pixel. 3 values: -1, 0, 1
	I8 m_iHalfY; // x direction half pixel. 3 values: -1, 0, 1
    CVector m_vctTrueHalfPel; // true motion vector (double)
#ifdef _4MV_
    I8 m_i8x8MVX [4]; // x direction motion
	I8 m_i8x8MVY [4]; // y direction motion
	I8 m_i8x8HalfX [4]; // x direction half pixel. 3 values: -1, 0, 1
	I8 m_i8x8HalfY [4]; // x direction half pixel. 3 values: -1, 0, 1
	CVector m_vct8x8TrueHalfPel [4]; // true8x8 motion vector (double)
	CVector m_vct8x8PredMVHalfPel [4]; // pred8x8 motion vector (double)
#endif
	CVector m_vctPredMVHalfPel; // pred motion vector (double)

	// Constructor
	CMotionVector (const CVector& vctHalfPel);
	CMotionVector () {setToZero ();}
	// Attributes
	const CVector& trueMVHalfPel () const {return m_vctTrueHalfPel;}

	// Operations
	Void operator = (const CMotionVector& mv);
	Void operator = (const CVector& vctHalfPel);
	Void setToZero ();
	Void computeTrueHalfPelMV (); // compute trueMV (CVector2D) from MV (CMotionVector)
    Void computeTrue8x8HalfPelMV (Int iBlkNum);
	Void computeMV (); // compute MV (CMotionVector) from trueMV (CVector2D)

};

#define NUM_BYTES_MV	(sizeof(CMotionVector))
#define NUM_BYTES_5MV	(sizeof(CMotionVector[5]))
#ifdef _4MV_
#define NUM_BYTES_MV_NOPREDMV	(sizeof(CMotionVector) - sizeof(CVector[5]))
#else
#define NUM_BYTES_MV_NOPREDMV	(sizeof(CMotionVector) - sizeof(CVector))
#endif
#endif //__cplusplus

#if defined (_M_IX86) && defined (__cplusplus)
#pragma warning (disable:4035)
inline Int _cdecl ROUND( float f) {
    __int64 intval;
    _asm {
        fld         f
        fistp       qword ptr [intval]
        mov         eax,dword ptr [intval]
    }
}

inline Int _cdecl ROUND( double f) {
    _asm {
        fld         f
        fistp       qword ptr [f]
        mov         eax,dword ptr [f]
    }
}
#pragma warning (default:4035)
#else
#define ROUND(x) ((x) > 0 ? (Int)(x + 0.5) : (Int)(x - 0.5))
#endif

union Buffer {
    I16 i16[64];
    Int i32[64];
};

#if !defined( NO_WINDOWS ) && defined( _M_IX86 ) && defined (__cplusplus)

#include <windows.h>
class CMyCriticalSection
{
public:
    CMyCriticalSection() {InitializeCriticalSection(&m_cs); m_bInitialized = TRUE;};
    ~CMyCriticalSection() {DeleteCriticalSection(&m_cs);};
    void Lock() {EnterCriticalSection(&m_cs);};
    void Unlock() {LeaveCriticalSection(&m_cs);};
    BOOL CheckInitialization() {return m_bInitialized;}; 
private:
    CRITICAL_SECTION m_cs;
    BOOL m_bInitialized;
};
#endif


//NQF, for Linux, define UNIX, then, undefine UNIX at very end
#ifdef   Linux
#undef  UNIX
#endif  
//NQF  


#endif // __BASIC_HPP_
