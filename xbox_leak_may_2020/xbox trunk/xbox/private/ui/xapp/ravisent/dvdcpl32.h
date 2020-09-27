#ifndef DVDFPL32_H
#define DVDFPL32_H

#define ONLY_EXTERNAL_VISIBLE


#undef UNICODE
#undef TCHAR
#define TCAHR char

#if   LINUX
////////////////////////////////////////////////////////////////////
//
//	LINUX
//
////////////////////////////////////////////////////////////////////

// #define NULL	0

#define __far
#define __huge
#define __cdecl
#define __pascal
#define __export
#define __loadds
#define FAR
#define cdecl
#define WINAPI

typedef unsigned int HANDLE;
typedef signed long LONG;

typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;


#include <stdlib.h>
#include <malloc.h>
#include <string.h>

enum POOL_TYPE
	{
	NonPagedPool, PagedPool
	};

void * operator new (unsigned int nSize, POOL_TYPE iType);
void * operator new (unsigned int nSize);
void operator delete (void *p);

#define _fstrcpy	strcpy
#define _fstrcat	strcat
#define _fstrlen	strlen
#define _fstrcmp	strcmp
#define _fmemcpy  memcpy
#define _fmalloc	malloc
#define _ffree		free

/* just like below in the #ifdef MMXMEMORY section */
inline void * __cdecl operator new(unsigned int nSize)
	{
	int * p = (int *)malloc(nSize + 16);
	int t = (int)p & 15;
	p += (16 - t) >> 2;
	p[-1] = t;
	return (void *)p;
	}

inline void * __cdecl operator new(unsigned int nSize, POOL_TYPE iType)
	{
	int * p = (int *)malloc(nSize + 16);
	int t = (int)p & 15;
	p += (16 - t) >> 2;
	p[-1] = t;
	return (void *)p;
	}

inline void __cdecl operator delete(void* p, POOL_TYPE iType)
	{
	int * q;

	if (p)
		{
		q = (int *)p;
		int t;
		t = q[-1];
		q -= (16 - t) >> 2;
		free(q);
		}
	}

inline void __cdecl operator delete(void* p)
	{
	int * q;

	if (p)
		{
		q = (int *)p;
		int t;
		t = q[-1];
		q -= (16 - t) >> 2;
		free(q);
		}
	}

inline void __cdecl operator delete[](void* p)
	{
	int * q;

	if (p)
		{
		q = (int *)p;
		int t;
		t = q[-1];
		q -= (16 - t) >> 2;
		free(q);
		}
	}


#elif MACINTOSH
////////////////////////////////////////////////////////////////////
//
//	Apple Macintosh
//
////////////////////////////////////////////////////////////////////

//	You should manually define MACINTOSH and also one of:
//	TARGET_API_MAC_OS8 ||ÊTARGET_API_MAC_OSX ||ÊTARGET_API_MAC_CARBON
//	TARGET_CPU_PPC is default


//	undefine Windows-isms
#define __far
#define __huge
#define __cdecl
#define __pascal
#define __export
#define __loadds


//	Memory management

typedef unsigned long	size_t;

enum POOL_TYPE
	{
	NonPagedPool, PagedPool
	};

void *  operator new(size_t nSize);
void *  operator new(size_t nSize, POOL_TYPE iType);
void	operator delete(void* p);

void *  operator new[](size_t nSize);
void *  operator new[](size_t nSize, POOL_TYPE iType);
void	operator delete[](void* p);

//	core MacOS includes
#include <ConditionalMacros.h>


#else
////////////////////////////////////////////////////////////////////
//
//	The OTHER Operating System
//
////////////////////////////////////////////////////////////////////

//
// Disable warnings for "inline assembler", "lost debugging information"
// and "inline functions".
//
#pragma warning(disable : 4505)
#pragma warning(disable : 4704)
#pragma warning(disable : 4791)


/*



#include <stdlib.h>
#include <malloc.h>

enum POOL_TYPE
	{
	PagedPool = 0,
	NonPagedPool = 1
	};


inline void * __cdecl operator new(unsigned int nSize)
	{
	return malloc(nSize);
	}

inline void * __cdecl operator new(unsigned int nSize, POOL_TYPE iType)
	{
	return malloc(nSize);
	}

inline void __cdecl operator delete(void* p, POOL_TYPE iType)
	{
	if (p)
		{
		free(p);
		}
	}



*/


#endif // not ST20LITE branch


////////////////////////////////////////////////////////////////////
//
//	SECOND PART: Common definitions
//
////////////////////////////////////////////////////////////////////



#include <windows.h>
#include <winnt.h>

// THIS WAS EXPERIMENTALLY REMOVED BECAUSE NOONE KNOWS WHAT IT IS GOOD FOR
//#include "resource.h"

#define __far
#define __huge
#define __loadds

#define _fmemcpy	memcpy
#if UNICODE
#define _fstrcpy	wcscpy
#define _fstrcat	wcscat
#define _fstrlen  wcslen
#define _fstrcmp	wcscmp
#else
#define _fstrcpy	strcpy
#define _fstrcat	strcat
#define _fstrlen	strlen
#define _fstrcmp	strcmp
#endif
#define _fmalloc	malloc
#define _ffree		free
#define _halloc(x,y)	new BYTE[x*y]
#define _hfree(x) delete[] x

typedef int			BOOL;
typedef char	 * LPSTR;

typedef unsigned char		BYTE;
typedef unsigned short		WORD;
typedef unsigned long		DWORD;

typedef unsigned __int64	QWORD;

typedef unsigned int		UINT;
typedef signed long		LONG;


#ifndef LOBYTE
#define LOBYTE(w)	    	((BYTE)(w))
#endif

#ifndef HIBYTE
#define HIBYTE(w)     	((BYTE)(((UINT)(w) >> 8) & 0xFF))
#endif

#ifndef LOWORD
#define LOWORD(l)     	((WORD)(DWORD)(l))
#endif

#ifndef HIWORD
#define HIWORD(l)     	((WORD)((((DWORD)(l)) >> 16) & 0xFFFF))
#endif

#ifndef MAKELONG
#define MAKELONG(low, high) ((DWORD)(WORD)(low) | ((DWORD)(WORD)(high) << 16))
#endif

#ifndef max
#define max(a,b)        (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)        (((a) < (b)) ? (a) : (b))
#endif









//
// Some extended definitions for word/dword arithmetic
//
#ifndef MAKEWORD
#define MAKEWORD(low, high) ((WORD)(BYTE)(low) | ((WORD)(BYTE)(high) << 8))
#endif

#define MAKELONG4(low, lmid, hmid, high) ((DWORD)(BYTE)(low) | ((DWORD)(BYTE)(lmid) << 8) | ((DWORD)(BYTE)(hmid) << 16) | ((DWORD)(BYTE)(high) << 24))
#define LBYTE0(w) ((BYTE)((w) & 0xff))
#define LBYTE1(w) ((BYTE)(((DWORD)(w) >> 8) & 0xff))
#define LBYTE2(w) ((BYTE)(((DWORD)(w) >> 16) & 0xff))
#define LBYTE3(w) ((BYTE)(((DWORD)(w) >> 24) & 0xff))

inline DWORD FLIPENDIAN(DWORD x)
	{
	return MAKELONG4(LBYTE3(x), LBYTE2(x), LBYTE1(x), LBYTE0(x));
	}

//
// Some standard pointer types
//

typedef void 		 	*	APTR;			// local pointer
typedef void __far	*	FPTR;			// far pointer
typedef void __huge	*	HPTR;			//	huge pointer
typedef BYTE __huge 	*	HBPTR;		// huge byte pointer



//
// We need a memory copy for huge memory; will use the windows version
// when available.
//



#define _hmemcpy memcpy





//
// Some more stuff
//
typedef BOOL				BIT;

#define HIGH				TRUE
#define LOW					FALSE

#ifndef FP_OFF
#define FP_OFF(x)			LOWORD(x)
#define FP_SEG(x)			HIWORD(x)
#endif



//
// Flag construction macro
//
#define MKFLAG(x)		(1UL << x)

//
// DWORD Bitfield construction and extraction functions
//
inline DWORD MKBF(int bit, int num, DWORD val) {return (((DWORD)val & ((1UL << num) -1)) << bit);}
inline DWORD MKBF(int bit, BOOL val) {return (val ? (1UL << bit) : 0);}
inline DWORD XTBF(int bit, int num, DWORD bf) {return ((bf >> bit) & ((1UL << num) -1));}
inline BOOL XTBF(int bit, DWORD bf) {return ((bf & (1UL << bit)) != 0);}
inline DWORD WRBF(DWORD bf, int bit, int num, DWORD val)
	{
	DWORD mask = ((1UL << num)-1) << bit;
	return (bf & ~mask) | ((val << bit) & mask);
	}
inline DWORD WRBF(DWORD bf, int bit, BOOL val) {return (val ? (bf | (1UL << bit)) : (bf & ~(1UL << bit)));}

//
// WORD Bitfield construction and extraction functions
//
inline WORD MKBFW(int bit, int num, WORD val) {return (((WORD)val & ((1 << num) -1)) << bit);}
inline WORD MKBFW(int bit, BOOL val) {return (val ? (1 << bit) : 0);}
inline WORD XTBFW(int bit, int num, WORD bf) {return ((bf >> bit) & ((1 << num) -1));}
inline BOOL XTBFW(int bit, WORD bf) {return ((bf & (1 << bit)) != 0);}
inline WORD WRBFW(WORD bf, int bit, int num, WORD val)
	{
	WORD mask = ((1 << num)-1) << bit;
	return (bf & ~mask) | ((val << bit) & mask);
	}
inline WORD WRBFW(WORD bf, int bit, BOOL val) {return (val ? (bf | (1 << bit)) : (bf & ~(1 << bit)));}

//
// BYTE Bitfield construction and extraction functions
//
inline BYTE MKBFB(int bit, int num, BYTE val) {return (((BYTE)val & ((1 << num) -1)) << bit);}
inline BYTE MKBFB(int bit, BOOL val) {return (val ? (1 << bit) : 0);}
inline BYTE XTBFB(int bit, int num, BYTE bf) {return ((bf >> bit) & ((1 << num) -1));}
inline BOOL XTBFB(int bit, BYTE bf) {return ((bf & (1 << bit)) != 0);}
inline BYTE WRBFB(BYTE bf, int bit, int num, BYTE val)
	{
	BYTE mask = ((1 << num)-1) << bit;
	return (bf & ~mask) | ((val << bit) & mask);
	}
inline BYTE WRBFB(BYTE bf, int bit, BOOL val) {return (val ? (bf | (1 << bit)) : (bf & ~(1 << bit)));}

//
// Find the most significate one bit or the least significant one bit in a double word
//
inline int FindMSB(DWORD bf) {int i; for(i=31; i>=0; i--) {if (XTBF(31, bf)) return i; bf <<= 1;} return -1;}
inline int FindLSB(DWORD bf) {int i; for(i=0; i<=31; i++) {if (XTBF( 0, bf)) return i; bf >>= 1;} return 32;}



//
// Scaling of values of WORD range to values of WORD range
//
inline WORD ScaleWord(WORD op, WORD from, WORD to) {return (WORD)((DWORD)op * (DWORD) to / (DWORD) from);}
	DWORD ScaleDWord(DWORD op, DWORD from, DWORD to);
	long ScaleLong(long op, long from, long to);
	void MUL32x32(DWORD op1, DWORD op2, DWORD __far & upper, DWORD __far & lower);
	DWORD DIV64x32(DWORD upper, DWORD lower, DWORD op);
//
// Most values in drivers etc. are scaled in a range from 0 to 10000, this functions transfer from and
// to this range
//
inline WORD ScaleFrom10k(WORD op, WORD to) {return ScaleWord(op, 10000, to);}
inline BYTE ScaleByteFrom10k(WORD op, BYTE to) {return (BYTE)ScaleWord(op, 10000, to);}
inline WORD ScaleTo10k(WORD op, WORD from) {return ScaleWord(op, from, 10000);}







class GenericHook;


extern "C" {
#define DLLCALL __declspec(dllexport)
DLLCALL	void	WINAPI VDR_RegisterCallback(GenericHook * hook);
DLLCALL	void	WINAPI VDR_UnregisterCallback(GenericHook * hook);
#define H20_RegisterCallback	VDR_RegisterCallback
}


#if !VIDEO_MINIPORT_DRIVER
#endif

typedef unsigned long Error;

//
// The last error, that was raised, is not only returned, but preserved in
// this variable.
//
//extern Error LastError;

//
// Error numbers are composed of four parts, an severity level, that
// tells how hard the error hit; a unit id, describing the unit that
// failed; an error type giving some general reasons and an unique
// id (unique for the module, not all error messages).
//
//                  SS-UUUUUUUU-TTTTTTTT-IIIIIIIIIIIIII
//


#define GNR_SEVERITY_BITS	(2)
#define GNR_SEVERITY_SHIFT	(30)
#define GNR_SEVERITY_MASK	(0xC0000000)

#define GNR_UNIT_BITS		(8)
#define GNR_UNIT_SHIFT		(22)
#define GNR_UNIT_MASK		(0x3FC00000)

#define GNR_TYPE_BITS		(8)
#define GNR_TYPE_SHIFT		(14)
#define GNR_TYPE_MASK	   (0x003FC000)

#define GNR_UNIQUE_BITS		(14)
#define GNR_UNIQUE_SHIFT	(0)
#define GNR_UNIQUE_MASK		(0x00003FFF)


//
// The severity level describes how hard this error affected the operation.
//
// As the severity level is the most significant part of an error message,
// it can be tested with a simple compare (not the Error is defined as
// unsigned, so no special sign effects can occur).
//
#define GNR_OK					(0x0L << GNR_SEVERITY_SHIFT)
	// everything did well; The neat side effect of defining the OK level as
	// 0, is that the check for an error can be done with a simple not
	// 0 compare like "if (error) { ... }".

#define GNR_WARNING			(0x1L << GNR_SEVERITY_SHIFT)
	// something not quite perfect happened, but the action was performed

#define GNR_ERROR				(0x2L << GNR_SEVERITY_SHIFT)
	// an error accured, that kept the routine from doing it´s job

#define GNR_DEADLY			(0x3L << GNR_SEVERITY_SHIFT)
	// something happened, that will keep the program from working correct,
	// it would be better to exit gracefully

//
// The error unit specifies the module where this error was defined
//
#define GNR_UNIT_COMMON		(0x0L << GNR_UNIT_SHIFT)
	// defined in gnerrors.H

#define GNR_UNIT_GENERAL	(0x1L << GNR_UNIT_SHIFT)
	// define in ..\general\*.h

#define GNR_UNIT_MEMMPDIO	(0x2L << GNR_UNIT_SHIFT)
	// define in memmpdio.h

#define GNR_UNIT_I2C			(0x3L << GNR_UNIT_SHIFT)
	// defined in viperi2c.h

#define GNR_UNIT_INTERRUPT	(0x4L << GNR_UNIT_SHIFT)
   // defined in intrctrl.h

#define GNR_UNIT_PARSER		(0x5L << GNR_UNIT_SHIFT)
	// defined in mpgparse.h

#define GNR_UNIT_PCI					(0x6L << GNR_UNIT_SHIFT)
	// define in pci.h

#define GNR_UNIT_DMA					(0x7L << GNR_UNIT_SHIFT)
	// define in dma*.h

#define GNR_UNIT_VDECODER			(0x8L << GNR_UNIT_SHIFT)
	// defined in viddec.h

#define GNR_UNIT_VENCODER			(0x9L << GNR_UNIT_SHIFT)
	// defined in videnc.h

#define GNR_UNIT_JPEG				(0xCL << GNR_UNIT_SHIFT)
	// defined in core.h

#define GNR_UNIT_MPEG				(0xDL << GNR_UNIT_SHIFT)
	// defined in library\hardware\mpeg2dec\mp2eldec.h

#define GNR_UNIT_TVTUNER			(0xEL << GNR_UNIT_SHIFT)
	// defined in tvtuner.h

#define GNR_UNIT_VESA20				(0xFL << GNR_UNIT_SHIFT)
	// defined in vesa20.h

#define GNR_UNIT_GFC					(0x10L << GNR_UNIT_SHIFT)
	// defined in gfxconfig.h

#define GNR_UNIT_PIP					(0x11L << GNR_UNIT_SHIFT)
	// defined in *pip*.h

#define GNR_UNIT_DMAMEM				(0x12L << GNR_UNIT_SHIFT)
	//	defined in ctdmamem.h

#define GNR_UNIT_CD					(0x13L << GNR_UNIT_SHIFT)
	// defined in cdifs.h

#define GNR_UNIT_PROFILES			(0x14L << GNR_UNIT_SHIFT)
	// defined in profiles.h

#define GNR_UNIT_AUDIO				(0x15L << GNR_UNIT_SHIFT)
	// defined in audio

#define GNR_UNIT_PCICONTROLLER	(0x16L << GNR_UNIT_SHIFT)

#define GNR_UNIT_AC3					(0x17L << GNR_UNIT_SHIFT)

#define GNR_UNIT_VTX					(0x18L << GNR_UNIT_SHIFT)
	// defined in vtxdll.h

#define GNR_UNIT_MPEG2				(0x19L << GNR_UNIT_SHIFT)

#define GNR_UNIT_MP2PARSER			(0x1AL << GNR_UNIT_SHIFT)

#define GNR_UNIT_VXD					(0x1BL << GNR_UNIT_SHIFT)
	// Error messages from PnP VxD interfaces

#define GNR_UNIT_DISK				(0x1CL << GNR_UNIT_SHIFT)
	// General disk errors (library\hardware\drives\generic\diskerrors.h)

#define GNR_UNIT_NAVIGATION		(0x1DL << GNR_UNIT_SHIFT)
	// Navigation errors (driver\dvdpldrv\win32\naverrors.h)

#define GNR_UNIT_DVD					(0x1EL << GNR_UNIT_SHIFT)
	// DVD specific errors (driver\dvdpldrv\win32\naverrors.h)

#define GNR_UNIT_VCD					(0x1FL << GNR_UNIT_SHIFT)
	// VCD specific errors (driver\dvdpldrv\win32\naverrors.h)

#define GNR_UNIT_CDA					(0x20L << GNR_UNIT_SHIFT)
	// CDA specific errors (driver\dvdpldrv\win32\naverrors.h)

#define GNR_UNIT_DECRYPTION		(0x21L << GNR_UNIT_SHIFT)
	// Decryption errors (library\hardware\mpeg2dec\generic\mp2dcryp.h)

#define GNR_UNIT_FILE				(0x22L << GNR_UNIT_SHIFT)
	// File errors (library\files\fileerrors.h)

#define GNR_UNIT_UNITS				(0x21L << GNR_UNIT_SHIFT)
	// in virtunit.h

#define GNR_UNIT_SUBPICTURE		(0x22L << GNR_UNIT_SHIFT)

#define GNR_UNIT_FRONTPANEL             (0x23L << GNR_UNIT_SHIFT)

//
// The error type gives a general hint, what caused the malfunction
//
#define GNR_TYPE_GENERAL			(0x00L << GNR_TYPE_SHIFT)
	// no special info

#define GNR_TYPE_FILEIO				(0x01L << GNR_TYPE_SHIFT)
	// some file specific error occured

#define GNR_TYPE_HARDWARE			(0x02L << GNR_TYPE_SHIFT)
	// it was a general hardware problem

#define GNR_TYPE_PARAMS				(0x03L << GNR_TYPE_SHIFT)
	// illegal parameters were used

#define GNR_TYPE_TIMEOUT			(0x04L << GNR_TYPE_SHIFT)
	// a timeout happened

#define GNR_TYPE_BUSY				(0x05L << GNR_TYPE_SHIFT)
	// the requested unit is currently busy

#define GNR_TYPE_NOACK				(0x06L << GNR_TYPE_SHIFT)
	// the requested unit did not respond

#define GNR_TYPE_INACTIVE			(0x07L << GNR_TYPE_SHIFT)
	// the requested unit is currently not active, or in an illegal
	// state for the type of the requested operation

#define GNR_TYPE_MEMORY				(0x08L << GNR_TYPE_SHIFT)
	// there was no sufficient free store left

#define GNR_TYPE_UNIMPLEMENTED 	(0x09L << GNR_TYPE_SHIFT)
	// the called function is not yet completely implemented

#define GNR_TYPE_FORMAT				(0x0aL << GNR_TYPE_SHIFT)
	// the examined object was in the wrong format

#define GNR_TYPE_OBJECT				(0x0bL << GNR_TYPE_SHIFT)

#define GNR_TYPE_BOUNDS				(0x0cL << GNR_TYPE_SHIFT)

#define GNR_TYPE_ILLEGALACCESS	(0x0dL << GNR_TYPE_SHIFT)

#define GNR_TYPE_REGION				(0x0eL << GNR_TYPE_SHIFT)

#define GNR_TYPE_PARENTAL			(0x0fL << GNR_TYPE_SHIFT)

#define GNR_TYPE_UOP					(0x10L << GNR_TYPE_SHIFT)

#define GNR_TYPE_OPERATION			(0x11L << GNR_TYPE_SHIFT)

#define GNR_TYPE_INTERNALSTATE	(0x12L << GNR_TYPE_SHIFT)

#define GNR_TYPE_COPYPROTECTION	(0x13L << GNR_TYPE_SHIFT)

//
// Macros to build and decompose error numbers
//
#define MKERR(level, unit, type, unique) ((Error)(GNR_##level | GNR_UNIT_##unit | GNR_TYPE_##type | unique))
	// build an error number from it´s parts

#define GNR_SEVERITY(err) (err & GNR_SEVERITY_MASK)
	// extracts the severity of the error number

#define GNR_UNIT(err) (err & GNR_UNIT_MASK)
	// extracts the unit of the error number

#define GNR_TYPE(err) (err & GNR_TYPE_MASK)
	// extracts the type of the error number

#define GNR_UNIQUE(err) (err & GNR_UNIQUE_MASK)
	// extracts the unique id of the error number

#ifndef IS_ERROR
	//
	// sidenote, the original definition of IS_ERROR in <winerror.h> is
	// #define IS_ERROR(Status) ((unsigned long)(Status) >> 31 == SEVERITY_ERROR)
	// this is functional identical to our definition, so no need to redefine it
	// here.  In any case a new define is added, which is to be used in all future
	// tests.
	//
#define IS_ERROR(err) ((err) >= GNR_ERROR)
#endif
#define IS_GNERROR(err) ((err) >= GNR_ERROR)

#define IS_WARNING(err) (GNR_SEVERITY(err) == GNR_WARNING)

//
// Some default error messages
//
#define GNR_FILE_NOT_FOUND			MKERR(ERROR, COMMON, FILEIO, 0x00)
	// general file not found error

#define GNR_FILE_IN_USE 			MKERR(ERROR, COMMON, FILEIO, 0x01)
	// the file is currently in use

#define GNR_FILE_WRONG_FORMAT 	MKERR(ERROR, COMMON, FILEIO, 0x02)
	// the file is in the wrong file format

#define GNR_END_OF_FILE 			MKERR(ERROR, COMMON, FILEIO, 0x03)
	// attempt to read behind the end of a file

#define GNR_NOT_ENOUGH_MEMORY 	MKERR(ERROR, COMMON, MEMORY, 0x04)
	// general not enough memory left

#define GNR_MEM_NOT_ALLOCATED 	MKERR(WARNING, COMMON, MEMORY, 0x05)
	// the memory that was requested to be freed was not allocated
	// before.

#define GNR_MEM_ALLOCATED_BEFORE MKERR(WARNING, COMMON, MEMORY, 0x06)
	// the memory was allocated before


#define GNR_UNIMPLEMENTED 			MKERR(ERROR, COMMON, UNIMPLEMENTED, 0x07)
	// the function is not yet implemented, and will never be


#define GNR_OBJECT_NOT_FOUND		MKERR(ERROR, COMMON, OBJECT, 0x08)
	// the requested object could not be found

#define GNR_OBJECT_EXISTS			MKERR(ERROR, COMMON, OBJECT, 0x09)
	// the object requested to be created does already exist

#define GNR_OBJECT_IN_USE			MKERR(ERROR, COMMON, OBJECT, 0x0a)
	// an operation that needs exclusive access to an object, found the object
	// already in use

#define GNR_OBJECT_FOUND			MKERR(ERROR, COMMON, OBJECT, 0x0b)
	// an object that was to be inserted in a data structure with unique objects
	// was already in there

#define GNR_RANGE_VIOLATION		MKERR(ERROR, COMMON, BOUNDS, 0x0c)
	// a given parameter was out of bounds


#define GNR_INVALID_CONFIGURE_STATE		MKERR(ERROR, COMMON, INACTIVE, 0x0d)
	// the unit was in an invalid state for configuration

#define GNR_OBJECT_FULL				MKERR(ERROR, COMMON, BOUNDS, 0x0e)

#define GNR_OBJECT_EMPTY			MKERR(ERROR, COMMON, BOUNDS, 0x0f)

#define GNR_OBJECT_NOT_ALLOCATED	MKERR(ERROR, COMMON, INACTIVE, 0x10)

#define GNR_OBJECT_READ_ONLY		MKERR(ERROR, COMMON, OBJECT, 0x11)

#define GNR_OBJECT_WRITE_ONLY		MKERR(ERROR, COMMON, OBJECT, 0x12)

#define GNR_OPERATION_PROHIBITED	MKERR(ERROR, COMMON, ILLEGALACCESS, 0x13)

#define GNR_OBJECT_INVALID			MKERR(ERROR, COMMON, OBJECT, 0x14)

#define GNR_INSUFFICIENT_RIGHTS	MKERR(ERROR, COMMON, ILLEGALACCESS, 0x15)

#define GNR_TIMEOUT					MKERR(ERROR, COMMON, NOACK, 0x16)

#define GNR_FILE_READ_ERROR		MKERR(ERROR, COMMON, FILEIO, 0x17)

#define GNR_FILE_WRITE_ERROR		MKERR(ERROR, COMMON, FILEIO, 0x18)

#define GNR_INVALID_PARAMETERS	MKERR(ERROR, COMMON, PARAMS, 0x19)

#define GNR_CONNECTION_LOST		MKERR(ERROR, COMMON, INACTIVE, 0x1a)

#define GNR_OPERATION_ABORTED		MKERR(ERROR, COMMON, TIMEOUT, 0x1b)

//
// Error invocation macros, to be used to standardize erroneous returns
//
#define GNRAISE(e) return (e)
	// raise an error, store it into the global error variable, and
	// return with the error number.

#define GNRAISE_OK return GNR_OK
	// raise the OK error, all did well

//#define GNRAISE_AGAIN return LastError
	// raise the last error again

#define GNASSERT(cond, except) if (cond) GNRAISE(except); else	0
	// if the condition evaluates to true, the exception is raised

#define GNREASSERT(cond) if (1) {Error e; if (e = (cond)) GNRAISE(e);} else 0
	// if the condition evaluates to not OK, the error is raised again

#define GNREASSERTMAP(cond, ne) if (1) {Error e; if (e = (cond)) GNRAISE(ne);} else 0
	// if the condition evaluates to not OK, the error ne is returned





// In order to be flexible, we use tags for setting and getting data and query
// module capabilities. Tag pairs consist of the tag itself and a data item.
// The tag tells which variable shall be set or retrieved. The data item is
// the value written to the variable or a pointer to the location where the
// retrieved value is to be stored.
// Tags have 32 bits and are built the following way:
//
// iccc uuuu uuuu uuuu rsss ssss ssss ssss
//
// "cc" is the tag command get, set or query. The tagging unit follows. The
// tag specifier itself is contained in the lowest 15 bits.
// Internal tags are marked with an i=1, external tags marked with an i=0.
// The difference between internal and external tagunits is, that external
// units have a type bit each, internal units only have numbers.
// Tags are type checked. Tags are constructed by the MKTAG_* macros and
// terminated by TAGDONE. The inline functions below do the type checking and
// are optimized to NOPs.  The r bit is used for reference tags.  These tags
// do not carry a value, but a reference to a more extendet structure;
//

#define TAG_VAL  0x00000000
#define TAG_GET  0x10000000
#define TAG_SET  0x20000000
#define TAG_QRY  0x30000000

#define TAG_REF  0x00008000

#define TAG_TYPE(x)	(x & 0x30000000)

#define ANYUNIT	0x4fff0000
#define MAIN_UNIT	0x00000000

#define MKTAGUNIT(unit, id)	\
	static const WORD unit##_ID	=	id;	\
	static const DWORD unit =  MKFLAG(16 + id);

#define GETTAGUNITS(tag)   XTBF(16, 12, tag)
#define USESTAGUNIT(tag, id) XTBF(16+id, tag)

#define MKITAGUNIT(unit, id)	\
	static const DWORD unit = 0x80000000L | (id << 16);

struct __far TAG {
	DWORD	id;
	DWORD	data;
	TAG (DWORD _id, DWORD _data) { data = _data; id = _id; };
	TAG() {};
#if !__EDG__ || __EDG_VERSION__<240
	~TAG() {}	// This is mainly used as a workaround for a certain C++ frontend compiler bug.
#endif
	};

inline BOOL __far & QRY_TAG(TAG __far * tag) {return *((BOOL __far *)(tag->data));}

	//
	// TAG terminator
	//
#define TAGDONE TAG(0,0)

DWORD FilterTags(TAG __far * tags, DWORD id, DWORD def);


#define MKTAG(name, unit, val, type)	\
	inline TAG SET_##name(type x) {return TAG(val | unit | TAG_SET, (DWORD)(x));}	\
	inline TAG GET_##name(type __far &x) {return TAG(val | unit | TAG_GET, (DWORD)(&x));}	\
	inline TAG QRY_##name(BOOL __far &x) {return TAG(val | unit | TAG_QRY, (DWORD)(&x));}	\
	inline type VAL_##name(TAG __far * tag) {return (type)(tag->data);}	\
	inline type FVAL_##name(TAG __far * tags, type def) {return (type)(FilterTags(tags, val | unit | TAG_GET, (DWORD)def));}	\
	inline type __far & REF_##name(TAG __far * tag) {return *(type __far *)(tag->data);}	\
	typedef type TTYPE_##name;	\
	static const DWORD CSET_##name = val | unit | TAG_SET;	\
	static const DWORD CGET_##name = val | unit | TAG_GET;	\
	static const DWORD CQRY_##name = val | unit | TAG_QRY;

#define MKRTG(name, unit, val, type)	\
	inline TAG SET_##name(type __far &x) {return TAG(val | unit | TAG_SET | TAG_REF, (DWORD)(FPTR)(&x));}	\
	inline TAG GET_##name(type __far &x) {return TAG(val | unit | TAG_GET | TAG_REF, (DWORD)(FPTR)(&x));}	\
	inline TAG QRY_##name(BOOL __far &x) {return TAG(val | unit | TAG_QRY | TAG_REF, (DWORD)(&x));}	\
	inline type VAL_##name(TAG __far * tag) {return *(type __far *)(tag->data);}	\
	inline type __far & REF_##name(TAG __far * tag) {return *(type __far *)(tag->data);}	\
	typedef type TTYPE_##name;	\
	static const DWORD CSET_##name = val | unit | TAG_SET | TAG_REF;	\
	static const DWORD CGET_##name = val | unit | TAG_GET | TAG_REF;	\
	static const DWORD CQRY_##name = val | unit | TAG_QRY | TAG_REF;


#if UNICODE

struct BoardVersionInfo
	{
	char	boardName[100];
	DWORD	boardVersion;
	};

struct UnicodeBoardVersionInfo
	{
	TCHAR boardName[100];
	DWORD boardVersion;
	};

MKRTG(BOARD_VERSION_UNICODE, MAIN_UNIT, 2, UnicodeBoardVersionInfo)

#else

struct BoardVersionInfo
	{
	char	boardName[100];
	DWORD	boardVersion;
	};

#endif

MKRTG(BOARD_VERSION,						MAIN_UNIT, 1,		BoardVersionInfo)
// 2 is used above!

MKRTG(BOARD_REGION_CODE,				MAIN_UNIT,	0x10,	BYTE)
MKRTG(BOARD_REGION_COUNT,				MAIN_UNIT,	0x11,	WORD)

MKRTG(BOARD_SPLIT_STREAM_SUPPORT,	MAIN_UNIT,	0x20, BOOL)

struct SoftwareVersionInfo
	{
	DWORD softwareVersion, softwareRevision;
	DWORD buildNumber;
	};

MKRTG(SOFTWARE_VERSION,		MAIN_UNIT,	0x30, SoftwareVersionInfo)

//
// Macros for easy access to TAG lists during parsing
//
// GETSET     : Simple get and set of a variable
// GETSETV    : Calls a function when the variable is changed
// GETSETVC   : Like GETSETV, but only calls on real changes
// GETSETC    : Sets a variable "changed" to TRUE when a variable is changed
// GETSETP    : Allows changing only when the virtual unit is passivated
// GETSETCF   : Ors a variable "changed" with a flag when the variable is changed
// GETONLY    : Allows only reading of a value
// GETINQUIRE : Calls an inquire function, the result is to be placed
//              in the pseudo variable "data".
//
// A typical TAG parsing routine will look like this
//
//	Error VirtualMPEGDecoder::Configure(TAG *tags)
//		{
//		PARSE_TAGS_START(tags)
//			GETSETP(MPEG_AUDIO_BITRATE,  params.audioBitRate);
//			GETSETP(MPEG_AUDIO_STREAMID, params.audioStreamID);
//			GETSETP(MPEG_VIDEO_STREAMID, params.videoStreamID);
//			GETSETP(MPEG_VIDEO_WIDTH,    params.videoWidth);
//			GETSETP(MPEG_VIDEO_HEIGHT,   params.videoHeight);
//			GETSETP(MPEG_VIDEO_FPS,      params.videoFPS);
//			GETSETP(MPEG_STREAM_TYPE,    params.streamType);
//
//			GETSET(MPEG_STREAM_HOOK, streamHook);
//
//			GETSETV(MPEG_LEFT_VOLUME,  leftVolume,  GNREASSERT(decoder->SetLeftVolume(leftVolume)));
//			GETSETV(MPEG_RIGHT_VOLUME, rightVolume, GNREASSERT(decoder->SetRightVolume(rightVolume)));
//			GETSETV(MPEG_AUDIO_MUTE,   audioMute,   GNREASSERT(decoder->MuteAudio(audioMute)));
//		PARSE_TAGS_END
//
//		GNRAISE_OK;
//		}
//




MKTAGUNIT(PIP_UNIT,            0)
MKTAGUNIT(VIDEO_ENCODER_UNIT,  1)
MKTAGUNIT(VIDEO_DECODER_UNIT,  2)
MKTAGUNIT(MPEG_DECODER_UNIT,   3)
MKTAGUNIT(TV_TUNER_UNIT,       4)
MKTAGUNIT(AUDIO_MIXER_UNIT,    5)
MKTAGUNIT(AUDIO_DECODER_UNIT,  6)
MKTAGUNIT(AUDIO_ENCODER_UNIT,  7)
MKTAGUNIT(JPEG_CODEC_UNIT,     8)
MKTAGUNIT(OSD_UNIT,				 9)
MKTAGUNIT(PANEL_UNIT,         10)
MKTAGUNIT(COMMUNICATION_UNIT, 11)
// DO NOT USE 12 or 13 due to contention with tag type
MKTAGUNIT(MPEG_ENCODER_UNIT,  14)

#define VIDEO_CHIP_UNIT			(VIDEO_ENCODER_UNIT | VIDEO_DECODER_UNIT)
#define AUDIO_STREAMDEV_UNIT	(AUDIO_ENCODER_UNIT | AUDIO_DECODER_UNIT)

#define NUM_UNITS		12




//
// A hook is a generalized callback.  The receiver of the callback
// gets two parameters, first the hook itself (this may be used for
// caller specific data), and a hook specific data element.
//
// The callback function is _NOT_ a virtual function, but a function
// pointer.  This has been done, to avoid incompatibilities with
// traditional 'C'.
//
// A typical use for a hook would be a refill request for a buffer
// in a buffer based stream:
//
// typedef struct RefillMsgStruct
//    {void __far * data, long num, long actual} RefillMsg;
//
// MKHOOK(Refill, RefillMsg);
//
// The implementor of the hook may then add some additional data,
// to customize the hook:
//
// class FileRefillHook : public RefillHook {
//		private:
//			Error Refill(void __far * data, long num, long &actual);
//			friend Error Refill(RefillHook * me, RefillMsg data);
//			FILE 	*	file;
//    public:
//       FileRefillHook(FILE * file) : RefillHook(Refill);
//		};
//
//	Error Refill(RefillHook * me, RefillMsg data)
//		{
//		return (FileRefillHook *)me->Refill(data.data, data.num, data.actual);
//		};
//
//
// Note that you have to do a MakeProcInstance if your callback function is not
// inside a DLL.
//


class KWorkerQueue;

class GenericHook
	{
	protected:
		KWorkerQueue	*	queue;
	public:
		void Register(KWorkerQueue * queue) {this->queue = queue;}
	};



extern "C" {
__declspec(dllexport) void  WINAPI VDR_RegisterCallback(GenericHook * hook);
}

#define MKHOOK(name, type) \
	class __far name##Hook : public GenericHook { \
		public: \
			typedef Error (WINAPI * CallType)(name##Hook __far * me, type & data); \
		private: \
			CallType	call; \
			class KWorkerQueue	*	queue;	\
		public: \
			name##Hook(CallType call) {this->call = call;VDR_RegisterCallback(this);} \
	};





enum VideoStandard
	{
	VIDEOSTANDARD_MIN,
		VSTD_NTSC	 = 0,
		VSTD_PAL		 = 1,
		VSTD_SECAM	 = 2,
		VSTD_HDTV	 = 3,
		VSTD_UNKNOWN = 4,
	VIDEOSTANDARD_MAX = VSTD_UNKNOWN
	};

enum PALVideoSubStandard
	{
	PALVIDEOSUBSTANDARD_MIN	= 0,
		PALVSSTD_DEFAULT		= 0,
		PALVSSTD_BDGHI			= 1,
		PALVSSTD_N				= 2,
		PALVSSTD_NC				= 3,
	PALVIDEOSUBSTANDARD_MAX	= PALVSSTD_NC
	};

enum NTSCVideoSubStandard
	{
	NTSCVIDEOSUBSTANDARD_MIN	= 0,
		NTSCVSSTD_DEFAULT			= 0,
		NTSCVSSTD_NORMAL			= 1,
		NTSCVSSTD_PALM				= 2,
		NTSCVSSTD_443				= 3,
		NTSCVSSTD_JAPAN			= 4,
		NTSCVSSTD_PAL60			= 5,
	NTSCVIDEOSUBSTANDARD_MAX	= NTSCVSSTD_PAL60
	};

enum VideoSource
	{
	VIDEOSOURCE_MIN			= 0,
		VSRC_COMPOSITE			= 0,
		VSRC_SVIDEO				= 1,
		VSRC_SERIAL_DIGITAL	= 2,
	VIDEOSOURCE_MAX	= VSRC_SVIDEO
	};

enum VideoFormat
	{
	VIDEOFORMAT_MIN = 0,
		VFMT_YUV_422 = 0,
		VFMT_YUV_411 = 1,
		VFMT_YUV_420 = 2,
		VFMT_YUV_444 = 3,
		VFMT_RGB_444 = 4,
	VIDEOFORMAT_MAX = VFMT_RGB_444
	};

enum VideoBusFormat
	{
	VIDEOBUSFORMAT_MIN = 0,
		FBFM_YC16 = 0,
		FBFM_YC8  = 1,
	VIDEOBUSFORMAT_MAX = FBFM_YC8
	};


enum VideoLineMode
	{
	VIDEOLINEMODE_MIN		= 0,
		VLM_PROGRESSIVE	= 0,
		VLM_INTERLACED		= 1,
	VIDEOLINEMODE_MAX		= VLM_INTERLACED
	};

enum VideoMode
	{
	VIDEOMODE_MIN = 0,
		VMOD_RESET    = 0,
		VMOD_PATTERN  = 1,
		VMOD_CAPTURE  = 2,
		VMOD_PLAYBACK = 3,
		VMOD_PATTERNSYNC = 4,
	VIDEOMODE_MAX = VMOD_PATTERNSYNC
//	ModeForceReprogramming   // this one internal use only
	};

enum VideoSampleMode
	{
	VIDEOSAMPLEMODE_MIN = 0,
		VSAMOD_CCIR	= 0,
		VSAMOD_SQP	= 1,
	VIDEOSAMPLEMODE_MAX = VSAMOD_SQP
	};

enum VideoField
	{
	VIDEOFIELD_MIN = 0,
		VFLD_SINGLE = 0,
		VFLD_ODD    = 1,
		VFLD_EVEN   = 2,
	VIDEOFIELD_MAX = VFLD_EVEN
	};

enum VideoMuxSource
	{
	VIDEOMUXSRC_MIN = 0,
		VIDEOMUXSRC_INTERNAL = 0,
		VIDEOMUXSRC_NONE = 1,
		VIDEOMUXSRC_EXTERNAL = 2,
	VIDEOMUXSRC_MAX = VIDEOMUXSRC_EXTERNAL
	};

enum VideoPixClockMode
	{
	VIDEOPIXCLK_MIN = 0,
		VIDEOPIXCLK_INTERNAL = 0,
		VIDEOPIXCLK_EXTERNAL = 1,
	VIDEOPIXCLK_MAX = VIDEOPIXCLK_EXTERNAL
	};


// The values of the following two enums are hardware-independent.

enum GrabFormat
	{
	GRABFORMAT_MIN = 0,
		VGRB_MJPEG    = 0,
		VGRB_RGB_888x = 1,
		VGRB_RGB_888  = 2,
		VGRB_RGB_565  = 3,
		VGRB_RGB_555  = 4,
		VGRB_YUV_422  = 5,
	GRABFORMAT_MAX = VGRB_YUV_422
	};

enum PIPFormat
	{
	PIPFORMAT_MIN = 0,
		PFMT_RGB_888x	= 0,
		PFMT_RGB_888	= 1,
		PFMT_RGB_565	= 2,
		PFMT_RGB_555	= 3,
		PFMT_YUV_422	= 4,
		PFMT_UVY_422	= 5,
		PFMT_PALETTE_8	= 6,
		PFMT_OTHER		= 7,
	PIPFORMAT_MAX = PFMT_OTHER
	};


struct HardVideoParams
	{
	int totalWidth,
	    totalHeight,
	    activeWidth,
	    activeHeight;   // activeHeight must be even (interlace)
	BIT hsPol,          // can be 0 or 1
	    vsPol;          // can be 0 or 1
	int hStart,
	    hEnd,           // hEnd = hStart + activeWidth - 1
	    vStart,
	    vEnd;           // vEnd = vStart + activeHeight/2 - 1
	BIT oddFirst;
	};


// These are defines for TV standard (CCIR) dependent sizes.

const HardVideoParams NTSCParams =
	{
	858, 525,
	720, 480,
	0,   0,
	123, 123+720-1,
	20,  20+480-1,
	TRUE
	};

const HardVideoParams PALParams =
	{
	864, 625,
	720, 576,
	0,   0,
	133, 133+720-1,
	46,  46+576-1,
	FALSE
	};


// These are defines for TV standard (square pixel) dependent sizes.

const HardVideoParams NTSCSquareParams =
	{
	780, 525,
	640, 480,
	1,   1,
	75,  75+640-1,
	20,  20+480-1,
	TRUE
	};

const HardVideoParams PALSquareParams =
	{
	944, 625,
	768, 576,
	1,   1,
	83,  83+768-1,
	34,  34+576-1,
	FALSE
	};


// Video Port specific definitions

// Possible video formats on video port (format field in VideoPortCaps struct)
#define VP_FMT_RGB_555		MKFLAG(0)
#define VP_FMT_RGB_565		MKFLAG(1)
#define VP_FMT_CCIR_422		MKFLAG(2)
#define VP_FMT_YUV_411		MKFLAG(3)
#define VP_FMT_YUV_420		MKFLAG(4)
#define VP_FMT_YONLY_400	MKFLAG(5)
#define VP_FMT_RGB_888_24	MKFLAG(6)
#define VP_FMT_RGB_888_32	MKFLAG(7)
#define VP_FMT_RGB_332		MKFLAG(8)
#define VP_FMT_ACCUPAK		MKFLAG(9)
#define VP_FMT_DYUV			MKFLAG(10)

// Protocols
#define VP_PORT_DI_8A		MKFLAG(0)
#define VP_PORT_DIV_8A		MKFLAG(1)
#define VP_PORT_DE_8A		MKFLAG(2)
#define VP_PORT_SI_8A		MKFLAG(3)
#define VP_PORT_SIV_8A		MKFLAG(4)
#define VP_PORT_SE_8A		MKFLAG(5)
#define VP_PORT_16A			MKFLAG(6)
#define VP_PORT_V_16A		MKFLAG(7)
#define VP_PORT_FIREWIRE	MKFLAG(8)
// Use this when creating a dummy Kernel Video Port PIP:
#define VP_PORT_DUMMY		MKFLAG(9)

// generate from the ones above!
//#define VP_PROT_BROOKTREE
//#define VP_PROT_PHILIPS

// Special connection features
#define VP_FEAT_INVERTPOLARITY	MKFLAG(0)
#define VP_FEAT_INTERLACED			MKFLAG(1)
#define VP_FEAT_TELETEXT			MKFLAG(2)
#define VP_FEAT_CLOSEDCAPTION		MKFLAG(3)
#define VP_FEAT_E_HREFH_VREFH		MKFLAG(4)
#define VP_FEAT_E_HREFH_VREFL		MKFLAG(5)
#define VP_FEAT_E_HREFL_VREFH		MKFLAG(6)
#define VP_FEAT_E_HREFL_VREFL		MKFLAG(7)
#define VP_FEAT_COLORCONTROL		MKFLAG(8)
#define VP_FEAT_BOB					MKFLAG(9)
#define VP_FEAT_WEAVE				MKFLAG(10)
// Use this when creating a dummy Kernel Video Port PIP:
#define VP_FEAT_DUMMY					MKFLAG(11)

struct VideoPortCaps
	{
	DWORD	videoFormat;		// Video format on port connection

	DWORD	protocol;			// Communication protocol(s)
	DWORD	connFeatures;		// Special features for the connection

	DWORD	fieldWidth;			// Maximum total transfer width of one field
	DWORD	vbiWidth;			// Maximum transfer width for VBI information
	DWORD	fieldHeight;		// Maximum total transfer height of one field

	DWORD	usPerField;			// Shortest possible time between two VREFs in microseconds
	DWORD	pixelsPerSecond;	// Number of pixels per second

	DWORD	displayOnly;		// The VGA can only display, but not capture.

	DWORD	clrControls;		// Will contain possible color controls
	};




MKTAG(PIP_VIDEOSTANDARD,	PIP_UNIT,	0x00000001, VideoStandard)
MKTAG(PIP_WINDOW,				PIP_UNIT,	0x00000002, HWND)

	//
	// Source region, relative to active display
	//
MKTAG(PIP_SOURCE_LEFT,		PIP_UNIT,	0x00000003, int)
MKTAG(PIP_SOURCE_TOP,		PIP_UNIT,	0x00000004, int)
MKTAG(PIP_SOURCE_WIDTH,		PIP_UNIT,   0x00000005, int)
MKTAG(PIP_SOURCE_HEIGHT,	PIP_UNIT,   0x00000006, int)

	//
	// Destination region, relative to current window
	//
MKTAG(PIP_DEST_LEFT,			PIP_UNIT,	0x00000007, int)
MKTAG(PIP_DEST_TOP,			PIP_UNIT,	0x00000008, int)
MKTAG(PIP_DEST_WIDTH,		PIP_UNIT,   0x00000009, int)
MKTAG(PIP_DEST_HEIGHT,		PIP_UNIT,   0x0000000A, int)

MKTAG(PIP_ADAPT_CLIENT_SIZE, PIP_UNIT, 0x0000000B, BOOL)
MKTAG(PIP_ADAPT_SOURCE_SIZE, PIP_UNIT, 0x0000000C, BOOL)
MKTAG(PIP_SCREEN_DEST,     PIP_UNIT,   0x0000000D, BOOL)
MKTAG(PIP_SAMPLEMODE,		PIP_UNIT,	0x0000000E, VideoSampleMode)
MKTAG(PIP_HOFFSET,			PIP_UNIT,	0x0000000F, int)
MKTAG(PIP_VOFFSET,			PIP_UNIT,	0x00000010, int)
MKTAG(PIP_ALWAYS_OVERLAY,	PIP_UNIT,	0x00000011, BOOL)
MKTAG(PIP_COLORKEY_COLOR,	PIP_UNIT,	0x00000012, DWORD)
MKTAG(PIP_COLORKEYED,		PIP_UNIT,	0x00000013, BOOL)

	//
	// Display region, relative to the destination rectangle
	//
MKTAG(PIP_DISPLAY_LEFT,		PIP_UNIT,	0x00000014, short)
MKTAG(PIP_DISPLAY_TOP,		PIP_UNIT,	0x00000015, short)
MKTAG(PIP_DISPLAY_WIDTH,	PIP_UNIT,	0x00000016, short)
MKTAG(PIP_DISPLAY_HEIGHT,	PIP_UNIT,	0x00000017, short)

MKTAG(PIP_COLORKEY_INDEX,	PIP_UNIT,	0x00000018, DWORD)

// Make sure to set physical, bytesPerRow and pixelFormat at the same time when enabling this:
MKTAG (PIP_OFFSCREEN_OVERRIDE,		PIP_UNIT, 0x00000019, BOOL)
MKTAG (PIP_OFFSCREEN_BASE,				PIP_UNIT, 0x0000001a, void __far *)
MKTAG (PIP_OFFSCREEN_BASE_LINEAR,	PIP_UNIT, 0x0000001b, DWORD)		// Use a 32bit flat pointer
MKTAG (PIP_OFFSCREEN_BYTES_PER_ROW,	PIP_UNIT, 0x0000001c, WORD)
MKTAG (PIP_OFFSCREEN_PIXEL_FORMAT,	PIP_UNIT, 0x0000001d, PIPFormat)

struct FieldDisplayMsg
	{
	DWORD	fieldCnt;
	};

MKHOOK(FieldDisplay, FieldDisplayMsg);

MKTAG (PIP_FIELD_DISPLAY_HOOK,		PIP_UNIT, 0x0000001e, FieldDisplayHook *)

MKTAG (PIP_IS_POSSIBLE,					PIP_UNIT, 0x0000001f, BOOL)



#define PCCF_BRIGHTNESS					0x00000001l
#define PCCF_CONTRAST					0x00000002l
#define PCCF_HUE							0x00000004l
#define PCCF_SATURATION					0x00000008l
#define PCCF_SHARPNESS					0x00000010l
#define PCCF_GAMMA						0x00000020l
#define PCCF_RED_BRIGHTNESS			0x00001000l
#define PCCF_BLUE_BRIGHTNESS			0x00002000l
#define PCCF_GREEN_BRIGHTNESS			0x00004000l



MKTAG (PIP_COLORCONTROL,				PIP_UNIT, 0x00000020, WORD)			// Get only tag to query color control support
MKTAG (PIP_BRIGHTNESS,					PIP_UNIT, 0x00000021, WORD)
MKTAG (PIP_CONTRAST,						PIP_UNIT, 0x00000022, WORD)
MKTAG (PIP_SATURATION,					PIP_UNIT, 0x00000023, WORD)
MKTAG (PIP_HUE,							PIP_UNIT, 0x00000024, WORD)
MKTAG (PIP_GAMMA,							PIP_UNIT, 0x00000025, WORD)			// Gamma correction
MKTAG (PIP_SHARPNESS,					PIP_UNIT, 0x00000026, WORD)
MKTAG (PIP_COLORENABLE,					PIP_UNIT, 0x00000027, BOOL)			// Used to toggle between B/W and color

// Use these to adjust brightness individually for R/G/B (only works with special VGAs)
MKTAG (PIP_RED_BRIGHTNESS,				PIP_UNIT, 0x00000028, WORD)
MKTAG (PIP_BLUE_BRIGHTNESS,			PIP_UNIT, 0x00000029, WORD)
MKTAG (PIP_GREEN_BRIGHTNESS,			PIP_UNIT, 0x0000002a, WORD)

// Used to crop the source rectangle when ADAPT_SOURCE_SIZE is TRUE
MKTAG (PIP_SOURCE_CROP_LEFT,			PIP_UNIT, 0x0000002b, WORD)
MKTAG (PIP_SOURCE_CROP_TOP,			PIP_UNIT, 0x0000002c, WORD)
MKTAG (PIP_SOURCE_CROP_RIGHT,			PIP_UNIT, 0x0000002d, WORD)
MKTAG (PIP_SOURCE_CROP_BOTTOM,		PIP_UNIT, 0x0000002e, WORD)



// Error definitions.
#define GNR_DMA_PIP_NOT_POSSIBLE			MKERR(ERROR, PIP, GENERAL, 0x01)		// Current screen mode does not allow DMA PIP
#define GNR_VIDEO_STD_NOT_SUPPORTED		MKERR(ERROR, PIP, GENERAL, 0x02)		// Desired video standard (e.g. SECAM) not supported.
#define GNR_PIP_NOT_RUNNING				MKERR(ERROR, PIP, GENERAL, 0x03)		// PIP is not running when trying to execute OptimizeBuffer()
#define GNR_NO_OPTIMAL_MINPIX				MKERR(ERROR, PIP, GENERAL, 0x04)		// No optimal minPix value could be found by OptimizeBuffer()
#define GNR_DEST_RECT_SIZE					MKERR(ERROR, PIP, GENERAL, 0x05)		// Destination rectangle too small or too big.
#define GNR_WRONG_GRAB_SIZE				MKERR(ERROR, PIP, GENERAL, 0x06)		// Dimensions of frame to grab are too big.
#define GNR_NO_VBLANK_IRQ					MKERR(ERROR, PIP, GENERAL, 0x07)		// An expected VBlank IRQ did not occur.
#define GNR_GRAB_TIMEOUT					MKERR(ERROR, PIP, GENERAL, 0x08)		// A timeout occurred during grabbing a frame.
#define GNR_WRONG_GRAB_FORMAT				MKERR(ERROR, PIP, GENERAL, 0x09)		// An unsupported capture format was requested
#define GNR_SAMPLEMODE_NOT_SUPPORTED	MKERR(ERROR, PIP, GENERAL, 0x0a)		// Desired video sample mode not supported
#define GNR_VP_ERROR							MKERR(ERROR, PIP, GENERAL, 0x0b)		// General error with Video Port
#define GNR_VP_NO_PORT_MATCH				MKERR(ERROR, PIP, GENERAL, 0x0c)		// Video source does not match Video Port
#define GNR_VP_HARDWARE_IN_USE			MKERR(ERROR, PIP, GENERAL, 0x0d)		// Video Port Hardware in use
#define GNR_VPM_ERROR						MKERR(ERROR, PIP, GENERAL, 0x0e)		// General error with VPM services
#define GNR_VPE_ERROR						MKERR(ERROR, PIP, GENERAL, 0x0f)		// General error with DirectX5/VPE services
#define GNR_NOT_ENOUGH_VIDEO_MEMORY		MKERR(ERROR, PIP, MEMORY,  0x10)
#define GNR_NO_OVERLAY_HARDWARE			MKERR(ERROR, PIP, GENERAL, 0x11)
#define GNR_OVERLAY_BUSY					MKERR(ERROR, PIP, BUSY,    0x12)
#define GNR_PRIMARY_NOT_AVAILABLE		MKERR(ERROR, PIP, BUSY,		0x13)






typedef struct StreamCaptureMsgStruct
	{
	FPTR bufferInfo;
	FPTR userData;
	DWORD captureTime;
	} StreamCaptureMsg;

MKHOOK(StreamCapture, StreamCaptureMsg);



typedef struct YStreamCaptureMsgStruct
	{
	FPTR bufferInfo;
	FPTR userData;
	WORD yCaptureCounter;
	} YStreamCaptureMsg;

MKHOOK(YStreamCapture, YStreamCaptureMsg);









MKTAG (VID_VIDEOSTANDARD,				VIDEO_CHIP_UNIT, 0x0001, VideoStandard)
MKTAG (VID_MODE,							VIDEO_CHIP_UNIT, 0x0002, VideoMode    )
MKTAG (VID_EXTSYNC,						VIDEO_CHIP_UNIT, 0x0003, BOOL         )
MKTAG (VID_PAL_VIDEOSUBSTANDARD,		VIDEO_CHIP_UNIT, 0x0004, PALVideoSubStandard)
MKTAG (VID_NTSC_VIDEOSUBSTANDARD,	VIDEO_CHIP_UNIT, 0x0005, NTSCVideoSubStandard)














MKTAG (ENC_SAMPLEMODE,  VIDEO_ENCODER_UNIT, 0x0001001, VideoSampleMode)
MKTAG (ENC_HOFFSET,     VIDEO_ENCODER_UNIT, 0x0001002, int)
MKTAG (ENC_VOFFSET,     VIDEO_ENCODER_UNIT, 0x0001003, int)

// The idle screen mapping depends on the specific video encoder. First ask
// for the number of entries via GET_ENC_IDLE_NUMBER, then allocate enough
// space to hold ENC_IDLE_NUMBER times a struct VideoEncoderIdleEntry,
// then call GET_ENC_IDLE_ENTRY to get an array of these structs filled
// with ENC_IDLE_NUMBER entries.

#define VIDENC_IdleNameMaxSize  40

struct __far VideoEncoderIdleEntry
	{
	int number;   // in increasing order
	TCHAR name[VIDENC_IdleNameMaxSize];
	};

MKTAG (ENC_IDLESCREEN,  VIDEO_ENCODER_UNIT, 0x0001004, int)   // global for all units
MKTAG (ENC_IDLE_NUMBER, VIDEO_ENCODER_UNIT, 0x0001005, int)   // global for all units
MKTAG (ENC_IDLE_ENTRY,  VIDEO_ENCODER_UNIT, 0x0001006, VideoEncoderIdleEntry __far *)   // global for all units

MKTAG (ENC_COPY_PROTECTION, VIDEO_ENCODER_UNIT, 0x00001007, int)

MKTAG (ENC_CHROMA_FILTER, VIDEO_ENCODER_UNIT, 0x00001008, DWORD)

MKTAG (ENC_SVIDEO_ACTIVE, VIDEO_ENCODER_UNIT, 0x00001009, BOOL)

MKTAG (ENC_ACTIVE_MUX_SRC, VIDEO_ENCODER_UNIT, 0x0003000, VideoMuxSource)
	// VIDEOMUXSRC_INTERNAL or VIDEOMUXSRC_EXTERNAL
MKTAG (ENC_LINE_MODE, VIDEO_ENCODER_UNIT, 0x0003001, VideoLineMode)
	// VLM_PROGRESSIVE or VLM_INTERLACED
MKTAG (ENC_PIXCLK_MODE, VIDEO_ENCODER_UNIT, 0x0003002, VideoPixClockMode)
	// VIDEOPIXCLK_INTERNAL or VIDEOPIXCLK_EXTERNAL
MKTAG (ENC_EMBEDDED_SYNC, VIDEO_ENCODER_UNIT, 0x0003003, BOOL)
MKTAG (ENC_HSYNC_POLARITY, VIDEO_ENCODER_UNIT, 0x0003005, BOOL)
MKTAG (ENC_VSYNC_POLARITY, VIDEO_ENCODER_UNIT, 0x0003006, BOOL)
MKTAG (ENC_STARTACTIVEPIXEL_DELAY, VIDEO_ENCODER_UNIT, 0x0003007, int)


// Errors from the video encoder.

#define GNR_VENC_PARAMS						MKERR(ERROR, VENCODER, PARAMS,  0x01)
	// illegal parameters
#define GNR_MACROVISION_NOT_SUPPORTED	MKERR(ERROR, VENCODER, GENERAL, 0x02)

#define GNR_CC_NOT_SUPPORTED				MKERR(ERROR, VENCODER, GENERAL, 0x03)




class KernelStringBuffer;

class __far KernelString
	{
	private:
		KernelStringBuffer	*	buffer;
	public:
		KernelString(void);
		KernelString(const TCHAR __far * str);
		KernelString(const TCHAR ch);
		KernelString(const KernelString & str);
		KernelString(DWORD value, int digits = 0, int base = 10, TCHAR fill = __TEXT('0'));
		KernelString(int value, int digits = 0, int base = 10, TCHAR fill = __TEXT('0'));
		~KernelString();

		// returns the length of the string EXCLUDING the succeeding zero...
		int Length() const;

		long ToInt(int base = 10);
		DWORD ToUnsigned(int base = 10);


		BOOL Get(TCHAR __far * str, int len);

		KernelString & operator = (const TCHAR __far * str);
		KernelString & operator = (const KernelString str);

		friend KernelString operator+ (const KernelString u, const KernelString v);
		KernelString & operator+= (const KernelString u);
		friend KernelString operator* (const KernelString u, const int num);
		KernelString & operator*= (const int num);

		int Compare(const KernelString str);

		friend BOOL operator==(const KernelString u, const KernelString v);
		friend BOOL operator!=(const KernelString u, const KernelString v);
		friend BOOL operator<(const KernelString u, const KernelString v);
		friend BOOL operator>(const KernelString u, const KernelString v);
		friend BOOL operator<=(const KernelString u, const KernelString v);
		friend BOOL operator>=(const KernelString u, const KernelString v);

		friend KernelString operator << (const KernelString u, int index);
		friend KernelString operator >> (const KernelString u, int index);
		KernelString & operator <<= (int index);
		KernelString & operator >>= (int index);

		KernelString Seg(int start, int num) const;	// Extract seqment of string
		KernelString Caps(void);
		KernelString Head(int num) const;				// Return the first num characters
		KernelString Tail(int num) const;				// Return the last num characters

		int First(KernelString str) const;				// Find first occurrence of str
		int Next(KernelString str, int pos) const;	// Find next occurrence of str
		int Last(KernelString str) const;				// Find last occurrence of str
		int Prev(KernelString str, int pos) const;	// Find previous occurrence of str

		int First(TCHAR c) const;							// Find first occurrence of c (-1 if not found)
		int Next(TCHAR c, int pos) const;				// Find next occurrence of c (-1 if not found)
		int Last(TCHAR c) const;							// Find last occurrence of c (-1 if not found)
		int Prev(TCHAR c, int pos) const;				// Find prevoius occurrence of c (-1 if not found)
		BOOL Contains(TCHAR c) const;						// Test if character occurs in string

		// deletes spaces (and tabs) at beginning or end of string...
		KernelString Trim();

		TCHAR& operator[] (const int index);
		const TCHAR& operator[] (const int index) const;

//
// Unsafe functions
//
		operator TCHAR * (void);
	};



//
// REMARK:
// This class is passed through DLLs.
// So please:
//        - insert member variables only at the end of the class declaration
//			 - do not append virutal methods
//
// occures in krnlint.h


class __far KernelInt64
	{
	private:
		unsigned long lower;
		signed long upper;
	public:
		KernelInt64(void) {lower = 0; upper = 0;}
		KernelInt64(DWORD val) {lower = val; upper = 0;}
		KernelInt64(long val) {lower = val; upper = val < 0 ? -1 : 0;}
		KernelInt64(int val) {lower = val; upper = val < 0 ? -1 : 0;}
		KernelInt64(unsigned int val) {lower = val; upper =  0;}
      KernelInt64(const KernelInt64 & val) {lower = val.lower; upper = val.upper;}
      KernelInt64 & operator= (const KernelInt64 val) {lower = val.lower; upper = val.upper; return *this;}

		KernelInt64(unsigned long lower, int upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(unsigned int lower, int upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(int lower, unsigned int upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(int lower, int upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(int lower, long upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(unsigned int lower, unsigned int upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(unsigned long lower, unsigned int upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(unsigned long lower, unsigned long upper) {this->lower = lower; this->upper = upper;}
		KernelInt64(unsigned long lower, signed long upper) {this->lower = lower; this->upper = upper;}

		KernelInt64(KernelString str, int base = 10);
		KernelString ToString(int digits = 0, int base = 10, TCHAR fill = __TEXT('0'));

		long ToLong(void)
			{
			if      (upper == 0x00000000 && !(lower & 0x80000000)) return lower;
			else if (upper == 0xffffffff &&  (lower & 0x80000000)) return lower;
			else if (upper < 0) return 0x8000000;
			else return 0x7fffffff;
			}

		DWORD ToDWORD(void)
			{
			if (upper < 0) return 0;
			else if (upper > 0) return 0xffffffff;
			else return lower;
			}

		int ToInt(void)
			{
			if (*this < -32768) return -32768;
			else if (*this > 32767) return 32767;
			else return (int)lower;
			}


		DWORD Lower(void) {return lower;}
		long Upper(void) {return upper;}

		inline int operator! (void) const {return !lower && !upper;}
		inline KernelInt64 operator- (void) const;
		inline KernelInt64 operator~ (void) const {return KernelInt64(~lower, ~upper);}

		inline friend KernelInt64 operator+ (const KernelInt64 u, const KernelInt64 v);
		inline friend KernelInt64 operator- (const KernelInt64 u, const KernelInt64 v);
		inline friend KernelInt64 operator+ (const long u, const KernelInt64 v);
		inline friend KernelInt64 operator- (const long u, const KernelInt64 v);
		inline friend KernelInt64 operator+ (const KernelInt64 u, const long v);
		inline friend KernelInt64 operator- (const KernelInt64 u, const long v);
		friend KernelInt64 operator* (const KernelInt64 u, const KernelInt64 v);
		friend KernelInt64 operator/ (const KernelInt64 u, const KernelInt64 v);
		inline friend KernelInt64 operator% (const KernelInt64 u, const KernelInt64 v);

		inline KernelInt64 & operator+= (const KernelInt64 u);
		inline KernelInt64 & operator-= (const KernelInt64 u);
		inline KernelInt64 & operator+= (const long u);
		inline KernelInt64 & operator-= (const long u);
		inline KernelInt64 & operator*= (const KernelInt64 u);
		inline KernelInt64 & operator/= (const KernelInt64 u);
		inline KernelInt64 & operator%= (const KernelInt64 u);

		inline KernelInt64 & operator++ (void);
		inline KernelInt64 & operator-- (void);

		inline friend KernelInt64 operator << (const KernelInt64 u, const int shl);
		inline friend KernelInt64 operator >> (const KernelInt64 u, const int shl);

		inline KernelInt64 & operator <<= (const int shl);
		inline KernelInt64 & operator >>= (const int shl);

		inline int Compare(const KernelInt64 u) const;

		friend BOOL operator==(const KernelInt64 u, const KernelInt64 v) {return u.Compare(v) == 0;}
		friend BOOL operator!=(const KernelInt64 u, const KernelInt64 v) {return u.Compare(v) != 0;}
		friend BOOL operator<(const KernelInt64 u, const KernelInt64 v)  {return u.Compare(v) < 0;}
		friend BOOL operator>(const KernelInt64 u, const KernelInt64 v)  {return u.Compare(v) > 0;}
		friend BOOL operator<=(const KernelInt64 u, const KernelInt64 v) {return u.Compare(v) <= 0;}
		friend BOOL operator>=(const KernelInt64 u, const KernelInt64 v) {return u.Compare(v) >= 0;}

		friend KernelInt64 operator& (const KernelInt64 u, const KernelInt64 v) {return KernelInt64(u.lower & v.lower, u.upper & v.upper);}
		friend KernelInt64 operator| (const KernelInt64 u, const KernelInt64 v) {return KernelInt64(u.lower | v.lower, u.upper | v.upper);}

		KernelInt64 & operator&= (const KernelInt64 u) {lower &= u.lower; upper &= u.upper; return *this;}
		KernelInt64 & operator|= (const KernelInt64 u) {lower |= u.lower; upper |= u.upper; return *this;}
	};

inline KernelInt64 & KernelInt64::operator+= (const KernelInt64 u)
	{
	lower += u.lower;
	if (lower < u.lower)
		upper += u.upper+1;
	else
		upper += u.upper;
	return *this;
	}

inline KernelInt64 & KernelInt64::operator-= (const KernelInt64 u)
	{
	unsigned long sum = lower - u.lower;
	if (sum > lower)
		upper -= u.upper+1;
	else
		upper -= u.upper;
	lower = sum;
	return *this;
	}

inline KernelInt64 & KernelInt64::operator+= (const long u)
	{
	if (u < 0)
		*this -= -u;
	else
		{
		lower += u;
		if (lower < (DWORD)u)
			upper += 1;
		}
	return *this;
	}

inline KernelInt64 & KernelInt64::operator++ (void)
	{
	lower ++;
	if (!lower)
		upper ++;
	return *this;
	}

inline KernelInt64 & KernelInt64::operator-- (void)
	{
	if (!lower)
		upper --;
	lower --;
	return *this;
	}

inline KernelInt64 & KernelInt64::operator-= (const long u)
	{
	if (u < 0)
		*this += -u;
	else
		{
		unsigned long sum = lower - u;
		if (sum > lower)
			upper -= 1;
		lower = sum;
		}
	return *this;
	}

inline KernelInt64 operator+ (const KernelInt64 u, const KernelInt64 v)
	{
	unsigned long sum = u.lower + v.lower;
	if (sum < u.lower)
		return KernelInt64(sum, u.upper + v.upper + 1);
	else
		return KernelInt64(sum, u.upper + v.upper);
	}

inline KernelInt64 operator- (const KernelInt64 u, const KernelInt64 v)
	{
	unsigned long sum = u.lower - v.lower;
	if (sum > u.lower)
		return KernelInt64(sum, u.upper - v.upper - 1);
	else
		return KernelInt64(sum, u.upper - v.upper);
	}

inline KernelInt64 operator+ (const KernelInt64 u, const long v)
	{
	if (v < 0)
		return u - -v;
	else
		{
		unsigned long sum = u.lower + v;
		if (sum < u.lower)
			return KernelInt64(sum, u.upper + 1);
		else
			return KernelInt64(sum, u.upper);
		}
	}

inline KernelInt64 operator- (const KernelInt64 u, const long v)
	{
	if (v < 0)
		return u + -v;
	else
		{
		unsigned long sum = u.lower - v;
		if (sum > u.lower)
			return KernelInt64(sum, u.upper - 1);
		else
			return KernelInt64(sum, u.upper);
		}
	}


inline KernelInt64 operator+ (const long u, const KernelInt64 v)
	{
	if (u < 0)
		return v - -u;
	else
		{
		unsigned long sum = u + v.lower;
		if (sum < v.lower)
			return KernelInt64(sum, v.upper + 1);
		else
			return KernelInt64(sum, v.upper);
		}
	}

inline KernelInt64 operator- (const long u, const KernelInt64 v)
	{
	return u + -v;
	}

inline KernelInt64 KernelInt64::operator- (void) const
	{
	if (lower == 0)
		return KernelInt64(0, -upper);
	else
		return KernelInt64((DWORD)-(long)lower, ~upper);
	}

inline int KernelInt64::Compare(const KernelInt64 u) const
	{
	if (upper < u.upper) return -1;
	else if (upper > u.upper) return 1;
	else if (lower < u.lower) return -1;
	else if (lower > u.lower) return 1;
	else return 0;
	}

inline KernelInt64 operator<< (const KernelInt64 u, const int shl)
	{
	KernelInt64 v = u;
	v <<= shl;
	return v;
	}

inline KernelInt64 operator>> (const KernelInt64 u, const int shl)
	{
	KernelInt64 v = u;
	v >>= shl;
	return v;
	}

inline KernelInt64 & KernelInt64::operator*= (const KernelInt64 u)
	{
	*this = *this * u;
	return *this;
	}

inline KernelInt64 & KernelInt64::operator/= (const KernelInt64 u)
	{
	*this = *this / u;
	return *this;
	}

inline KernelInt64 & KernelInt64::operator%= (const KernelInt64 u)
	{
	*this = *this % u;
	return *this;
	}

inline KernelInt64 operator % (const KernelInt64 u, const KernelInt64 v)
	{
	return u - (u / v) * v;
	}

inline KernelInt64 & KernelInt64::operator<<= (const int shl)
	{
	int s = shl;

	while (s > 0)
		{
		upper <<= 1;
		if (lower & 0x80000000) upper |= 1;
		lower <<= 1;
		s--;
		}

	return *this;
	}

inline KernelInt64 & KernelInt64::operator>>= (const int shl)
	{
	int s = shl;

	while (s > 0)
		{
		lower >>= 1;
		if (upper & 0x00000001) lower |= 0x80000000;
		upper >>= 1;
		s--;
		}

	return *this;
	}


enum MPEGCommand {mpc_none, 	  		// 0x0
                  mpc_start,    		// 0x1
                  mpc_play,     		// 0x2
                  mpc_seek,     		// 0x3
                  mpc_stop,     		// 0x4
                  mpc_step,     		// 0x5
                  mpc_freeze,   		// 0x6
                  mpc_resync,   		// 0x7
                  mpc_resyncue, 		// 0x8
                  mpc_cue,				// 0x9
                  mpc_end,				// 0xA
                  mpc_params,			// 0xB
                  mpc_stepkey,		// 0xC
                  mpc_scan,			// 0xD
						mpc_reverse,		// 0xE
						mpc_trickplay,		// 0xF
						mpc_seekaudio,		// 0x10
						mpc_resyncaudio,	// 0x11
						mpc_audio_test		// 0x12	Audio tests : pink noise, test tone
						};

enum MPEGState   {mps_reset, mps_preempted, mps_initial,
                  mps_frozen, mps_playing, mps_stepping,
                  mps_seeking, mps_resyncing, mps_stopped,
                  mps_scanning, mps_trickplaying, mps_testing,
						mps_capturing};

enum MPEGElementaryStreamType
	{
	MST_VIDEO,
	MST_AUDIO,
	MST_SUBPICTURE
	};




//
// Commands and parameters:
//
//		none                  : do nothing
//    start                 : start the operation
//    play([1..1000] speed) : play at specific speed (1000 = normal)
//    seek                  : stop playback, go standby
//    stop                  : stop playback
//    step(int frames)      : step some frames
//    freeze                : freeze current frame
//    resync                : resync after seek
//    end                   : end the operation
//
// State transition diagram
//
//	INITIAL:		reset
//
//	SRCSTATE		COMMAND/EVENT		DSTSTATE		ACTION
//
//	reset			start					initial		Init MPEG Decoder, start transfering data
// 				default				reset
//
//	initial		play					playing		Start playing the MPEG Stream
//					...					frozen		Freeze after first I-Frame
//
//	frozen		play					playing		Continue playing from current location
//					seek					seeking		Complete operation, and stop playback
//       		step					stepping		Go to step mode
//					end					reset
//					stop					stopped
//					default				frozen
//
//	playing		play					leaping		Adjust playback speed if required
//											playing
//					seek					seeking		Complete operation and stop playback
//					end					reset
//					stop					stopped
//					default				playing
//
// leaping     default           playing
//
//	stepping		play					playing
//					seek					seeking
//					end					reset
//					stop					stopped
//					default				stepping
//											frozen
//
//	seeking		resync				resyncing
//					resyncue				cued
//					end					reset
//
//	resyncing	play					playing
//					...					frozen
//
//	stopped		play					initial
//					end					reset
//					default				stopped
//

struct MPEGStreamMsg {
	DWORD		size;
	};

struct MPEGSignalMsg {
	DWORD		position;
	};

struct MPEGIdleMsg {
	};

MKHOOK(MPEGStream, MPEGStreamMsg)
MKHOOK(MPEGSignal, MPEGSignalMsg)
MKHOOK(MPEGIdle,   MPEGIdleMsg)

typedef MPEGStreamHook	__far *	MPEGStreamHookPtr;
typedef MPEGSignalHook	__far *	MPEGSignalHookPtr;
typedef MPEGIdleHook		__far *	MPEGIdleHookPtr;

enum MPEGStreamType {audioOnly, videoOnly, multiplexed};

enum MPEGStreamMode {singleStream, dualStream};

enum MPEGAudioLayer
	{
	MPAUDLAYER_1,
	MPAUDLAYER_2,
	MPAUDLAYER_3							// = MP3
	};

enum MPEGStereoMode
	{
	MPSS_NORMAL,							// Select output of first (normal) stereo program
	MPSS_DEFAULT = MPSS_NORMAL,
	MPSS_SECOND_STEREO					// Select output of second stereo program
	};

MKTAG(MPEG_AUDIO_BITRATE,					MPEG_DECODER_UNIT,	0x0001, DWORD)
MKTAG(MPEG_AUDIO_STREAMID,					MPEG_DECODER_UNIT,	0x0002, BYTE)
MKTAG(MPEG_VIDEO_STREAMID,					MPEG_DECODER_UNIT,	0x0003, BYTE)
MKTAG(MPEG_VIDEO_WIDTH,						MPEG_DECODER_UNIT,	0x0004, WORD)
MKTAG(MPEG_VIDEO_HEIGHT,   				MPEG_DECODER_UNIT,   0x0005, WORD)
MKTAG(MPEG_VIDEO_FPS,      				MPEG_DECODER_UNIT,   0x0006, WORD)
MKTAG(MPEG_STREAM_TYPE,						MPEG_DECODER_UNIT,	0x0007, MPEGStreamType)
MKTAG(MPEG_STREAM_HOOK,						MPEG_DECODER_UNIT,	0x0008, MPEGStreamHookPtr)
MKTAG(MPEG_LEFT_VOLUME,    				MPEG_DECODER_UNIT,   0x0009, WORD)
MKTAG(MPEG_RIGHT_VOLUME,   				MPEG_DECODER_UNIT,   0x000A, WORD)
MKTAG(MPEG_AUDIO_MUTE,     				MPEG_DECODER_UNIT,   0x000B, BOOL)
MKTAG(MPEG_SIGNAL_HOOK,						MPEG_DECODER_UNIT,	0x000C, MPEGSignalHookPtr)
MKTAG(MPEG_SIGNAL_POSITION,				MPEG_DECODER_UNIT,	0x000D, DWORD)
MKTAG(MPEG_DONE_HOOK,						MPEG_DECODER_UNIT,	0x000E, MPEGSignalHookPtr)
MKTAG(MPEG_VIDEO_BITRATE,					MPEG_DECODER_UNIT,   0x000F, DWORD)
MKTAG(MPEG_STREAM_BITRATE,					MPEG_DECODER_UNIT,   0x0010, DWORD)
MKTAG(MPEG_INTRA_QUANTIZER_MATRIX,		MPEG_DECODER_UNIT,	0x0011, BYTE __far *)
MKTAG(MPEG_NON_INTRA_QUANTIZER_MATRIX,	MPEG_DECODER_UNIT,	0x0012, BYTE __far *)
MKTAG(MPEG_VIDEOSTANDARD,					MPEG_DECODER_UNIT,	0x0013, VideoStandard)

MKTAG(MPEG_STREAM_MODE,						MPEG_DECODER_UNIT,	0x0014, MPEGStreamMode)

MKTAG(MPEG_DISPLAY_WIDTH,					MPEG_DECODER_UNIT,	0x0020, WORD)
MKTAG(MPEG_DISPLAY_HEIGHT,					MPEG_DECODER_UNIT,	0x0021, WORD)

MKTAG(MPEG_VIDEO_BRIGHTNESS,				MPEG_DECODER_UNIT,	0x0022, WORD)
MKTAG(MPEG_VIDEO_CONTRAST,					MPEG_DECODER_UNIT,	0x0023, WORD)

MKTAG(MPEG_AUDIO_SAMPLERATE,				MPEG_DECODER_UNIT,	0x0024, WORD)
MKTAG(MPEG_IDLE_HOOK,						MPEG_DECODER_UNIT,	0x0025, MPEGIdleHookPtr)

MKTAG(MPEG_ASPECT_RATIO,					MPEG_DECODER_UNIT,	0x0026, WORD)
MKTAG(MPEG_SUPPORTS_HIRES_STILL,			MPEG_DECODER_UNIT,	0x0027, BOOL)

MKTAG(MPEG_CURRENT_STREAM_SEGMENT,		MPEG_DECODER_UNIT,	0x0028, DWORD)
MKTAG(MPEG_FIRST_STREAM_SEGMENT,			MPEG_DECODER_UNIT,	0x0029, DWORD)
MKTAG(MPEG_LAST_STREAM_SEGMENT,			MPEG_DECODER_UNIT,	0x002a, DWORD)

MKTAG(MPEG_DISPLAY_MODE,					MPEG_DECODER_UNIT,   0x0030, VideoMode)

// Determines the audio layer contained in the MPEG data/to be decoded
MKTAG(MPEG_AUDIO_LAYER,						MPEG_DECODER_UNIT,	0x0031, MPEGAudioLayer)

// CRC check enable/disable for MPEG (e.g. MP3) decoding (default: FALSE (off))
MKTAG(MPEG_CRC_CHECK,						MPEG_DECODER_UNIT,	0x0032, BOOL)

// Controls second stereo mode for MPEG audio material
MKTAG(MPEG_AUDIO_STEREO_MODE,				MPEG_DECODER_UNIT,	0x0033, MPEGStereoMode)



enum AC3BassConfig
	{
	AC3BC_NO_REDIRECTION,
	AC3BC_REDIRECT_CENTER_BASS_TO_LEFT_AND_RIGHT,
	AC3BC_REDIRECT_LEFT_RIGHT_CENTER_BASS_TO_SUBWOOFER,
	AC3BC_REDIRECT_CENTER_BASS_TO_SUBWOOFER
	};

enum AC3SpeakerConfig
	{
	AC3SC_20_SURROUND_COMPATIBLE,
	AC3SC_10,
	AC3SC_20_NORMAL,
	AC3SC_30,
	AC3SC_21,
	AC3SC_31,
	AC3SC_22,
	AC3SC_32,
	AC3SC_KARAOKE		// This may only be available for MPEG-2 audio, but it is
							// added to the AC3 settings to avoid introducing new tags
	};

enum AC3DualModeConfig
	{
	AC3DMDM_DEFAULT,	// default is stereo
	AC3DMDM_STEREO = AC3DMDM_DEFAULT,
	AC3DMDM_CHANNEL1,	// channel 1 on both L/R
	AC3DMDM_CHANNEL2, // channel 2 on both L/R
	AC3DMDM_MIX			// mix channel 1 and 2 to mono, output on both L/R
	};

enum AC3KaraokeConfig
	{
	AC3KARA_AWARE,
	AC3KARA_DEFAULT,
	AC3KARA_OFF = AC3KARA_DEFAULT,
	AC3KARA_MULTICHANNEL	= 3,
	AC3KARA_CAPABLE_NO_VOCAL,
	AC3KARA_CAPABLE_V1,
	AC3KARA_CAPABLE_V2,
	AC3KARA_CAPABLE_BOTH_VOCAL
	};

enum DolbyProLogicConfig
	{
	DPLCFG_OFF,
	DPLCFG_DEFAULT = DPLCFG_OFF,
	DPLCFG_3_0 = 3,	// LCR three stereo
	DPLCFG_2_1,			// LRS phantom (= center not used)
	DPLCFG_3_1,			// LCRS
	DPLCFG_2_2,			// LRSS phantom
	DPLCFG_3_2			// LCRSS
	};

enum DolbyProLogicMode
	{
	DPLMODE_AUTOMATIC,	// Decoding depends on input fed into the PL decoder
	DPLMODE_DEFAULT = DPLMODE_AUTOMATIC,
	DPLMODE_ON,				// force it always on
	DPLMODE_OFF				// always off
	};

enum AC3DynamicRange
	{
	AC3DYNRG_DEFAULT,
	AC3DYNRG_COMPRESSED = AC3DYNRG_DEFAULT,	// Dynamic range sclae factor taken from MPEG-2 AC3 stream
	AC3DYNRG_MAXIMUM									// Always use full dynamic range
	};

// AC3 "acmod" property
enum AC3AudioCodingMode
	{
	AC3ACMOD_DUALMONO,
	AC3ACMOD_1_0,
	AC3ACMOD_2_0,
	AC3ACMOD_3_0,
	AC3ACMOD_2_1,
	AC3ACMOD_3_1,
	AC3ACMOD_2_2,
	AC3ACMOD_3_2,
	AC3ACMOD_UNKNOWN
	};

enum KaraokeVoiceEffectType
	{
	KARAOKE_VOICE_EFFECT_NONE = 0,
	KARAOKE_VOICE_EFFECT_ECHO = 1,
	KARAOKE_VOICE_EFFECT_CHORUS = 2,
	KARAOKE_VOICE_EFFECT_REVERB = 3
	};


enum KaraokeWordSizeType
	{
	KARAOKE_WORD_SIZE_16bit = 0,
	KARAOKE_WORD_SIZE_18bit = 1,
	KARAOKE_WORD_SIZE_20bit = 2,
	KARAOKE_WORD_SIZE_24bit = 3
	};

enum KaraokeJustificationType
	{
	KARAOKE_START_JUSTIFIED = 0,
	KARAOKE_END_JUSITIFIED  = 1
	};

enum KaraokePCMOutputModeType
	{
	KARAOKE_PCM_OUTPUT_MODE_2Channel = 0,
	KARAOKE_PCM_OUTPUT_MODE_6Channel = 1
	};

enum KaraokePCMByteOrderType
	{
	KARAOKE_PCM_BYTE_ORDER_LSB_FIRST = 0,
	KARAOKE_PCM_BYTE_ORDER_MSB_FIRST = 1
	};

enum KaraokeVoiceSamplingFrequencyType
	{
	KARAOKE_SAMPLING_FREQUENCY_12kHz = 0,
	KARAOKE_SAMPLING_FREQUENCY_11_025KHz = 1,
	KARAOKE_SAMPLING_FREQUENCY_10kHz = 2
	};

enum KaraokeMusicSamplingFrequencyType
	{
	KARAOKE_SAMPLING_FREQUENCY_48kHz = 0,
	KARAOKE_SAMPLING_FREQUENCY_44_1KHz = 1,
	KARAOKE_SAMPLING_FREQUENCY_32kHz = 2
	};

enum KaraokeSamplingFrequencyType
	{
	KARAOKE_SAMPLING_FREQUENCY_256Fs = 0,
	KARAOKE_SAMPLING_FREQUENCY_384Fs = 1
	};






enum SPUButtonState
	{
	SBS_DISABLED,
	SBS_SELECTED,
	SBS_ACTIVATED
	};

struct MPEG2SPUCommandMsg {
	DWORD		pts;
	int		command;
	};

MKHOOK(MPEG2SPUCommand, MPEG2SPUCommandMsg)

typedef MPEG2SPUCommandHook	__far *	MPEG2SPUCommandHookPtr;

//
//  Error definitions
//

#define GNR_DISPLAY_MODE_NOT_SUPPORTED					MKERR(ERROR, MPEG, FORMAT, 0x00)
// The display mode could not be set

#define GNR_AUDIO_TYPE_NOT_SUPPORTED					MKERR(ERROR, MPEG, FORMAT, 0x01)
// The audio format is not supported

#define GNR_NO_SPDIF_HARDWARE								MKERR(ERROR, MPEG, HARDWARE, 0x02)
// We do not SPDIF hardware

#define GNR_FRAME_ADVANCE_BOUNDARY_REACHED			MKERR(WARNING, MPEG, BOUNDS, 0x03)

enum MPEG2PresentationMode
	{
	MPM_FULLSIZE,
	MPM_LETTERBOXED,
	MPM_PANSCAN,
	MPM_FULLSIZE16by9
	};

enum SPDIFHandling
	{
	SPDIFH_DEFAULT,			// On when playback is running, off in all other cases
	SPDIFH_ON,					// Always physically on
	SPDIFH_OFF					// Always physically off (line idle)
	};

// This can be specified for each audio type
enum SPDIFOutputMode
	{
	SPDIFOM_DEFAULT,			// AC3: compressed, LPCM: decompressed, idle: NULL output
	SPDIFOM_DECOMPRESSED,	// Force decompressed output, even for AC3
	SPDIFOM_MUTE_NULL,		// Force mute with NULL output, if SPDIF_ON
	SPDIFOM_OFF					// Physically off
	};

enum SPDIFCopyMode
	{
	SPDIFCM_DEFAULT,			// Use system's default (transfer rights from source material to output)
	SPDIFCM_NO_COPIES,		// do not allow any copies
	SPDIFCM_ONE_GENERATION,	// allow one generation of copies
	SPDIFCM_UNRESTRICTED		// no copy restrictions
	};

enum MPEG2SplitStreamType
	{
	MP2SST_DVD_ENCRYPTED,
	MP2SST_PROGRAM,
	MP2SST_PES,
	MP2SST_ELEMENTARY
	};

enum MPEG2PESType
	{
	MP2PES_ELEMENTARY,
	MP2PES_DVD,
	MP2PES_MPEG
	};

// Basic Audio Source Type
enum MPEG2AudioType
	{
	MP2AUDTYP_DEFAULT,		// means: use MPEG2_AUDIO_AC3 and MPEG2_AUDIO_LPCM tags to determine audio type
	MP2AUDTYP_MPEG,			// MPEG(-1) all layers
	MP2AUDTYP_MPEG2,
	MP2AUDTYP_AC3,
	MP2AUDTYP_LPCM,			// includes CDDA (set # of bits and sample rate accordingly)
	MP2AUDTYP_DTS,
	MP2AUDTYP_SDDS,
	MP2AUDTYP_DTS_CDDA,		// CDDA with DTS information

	MP2AUDTYP_NUMBER_OF_TYPES
	};

// Virtual Surround ("Spatializer") modes
enum MPEG2AudioSpatializer
	{
	MP2AUDSPAT_DEFAULT,
	MP2AUDSPAT_NONE = MP2AUDSPAT_DEFAULT,
	MP2AUDSPAT_SRS_TS_3D			// SRS True Surround or 3D sound, depending on source material
	};

// PCM Output Configurations (= Bass Redirection Schemes)
enum PCMOutputConfig
	{
	PCMOCFG_DEFAULT,	// ALL, scaled, is default
	PCMOCFG_ALL = PCMOCFG_DEFAULT,
	PCMOCFG_LSW,
	PCMOCFG_LLR,
	PCMOCFG_SLP,
	PCMOCFG_SUM,		// subwoofer = sum of all input channels
	PCMOCFG_BYP			// Bypass
	};

// Downsample mode for 96kHz LPCM
enum PCM96DownsampleMode
	{
	PCM96DWNS_DEFAULT,
	PCM96DWNS_DOWN_48KHZ = PCM96DWNS_DEFAULT,	// sample down to 48kHz by default
	PCM96DWNS_NONE										// no downsampling, play back using 96kHz
	};


// Program Format Flags for tag MPEG2_AUDIO_PROGRAM_FORMAT
#define AUDPF_PROLOGIC			MKFLAG(0)	// Indicates if Pro Logic Decoding is active or not
#define AUDPF_LEFT				MKFLAG(1)	// Shows if program contains
#define AUDPF_RIGHT				MKFLAG(2)
#define AUDPF_CENTER				MKFLAG(3)
#define AUDPF_LFE					MKFLAG(4)
#define AUDPF_LEFT_SURROUND	MKFLAG(5)
#define AUDPF_RIGHT_SURROUND	MKFLAG(6)
#define AUDPF_MONO_SURROUND	MKFLAG(7)


//
//  CDDA Data Format
//  Describes formats supported by decoder
//

#define CDDADF_AUDIO_ONLY			MKFLAG(0)			// Only audio data (corresponds to LPCM, 2 ch, 16 Bit, 44.1 kHz), default
#define CDDADF_AUDIO_SUBCHANNEL	MKFLAG(1)			// For each block first audio data (2352 bytes), then subchannel (98 bytes)
#define CDDADF_SUBCHANNEL_AUDIO	MKFLAG(2)			// For each block first subchannel (98 bytes), then audio data (2352 bytes)
#define CDDADF_DEFAULT				CDDADF_AUDIO_ONLY

struct AudioTypeConfig
	{
	union
		{
		struct
			{
			SPDIFOutputMode	spdifOutMode : 3;
			// ...can be expanded up to size of a DWORD...
			} config;

		DWORD	dummy;
		};

	friend BOOL operator==(const AudioTypeConfig a, const AudioTypeConfig b);
	friend BOOL operator!=(const AudioTypeConfig a, const AudioTypeConfig b);

	operator DWORD (void) {return dummy;}

	AudioTypeConfig(DWORD arg) {dummy = arg;}
	AudioTypeConfig(void) {dummy = 0;}
	AudioTypeConfig(SPDIFOutputMode initMode)
		{
		config.spdifOutMode = initMode;
		// ...
		}
	};

inline BOOL operator==(const AudioTypeConfig a, const AudioTypeConfig b)
	{
	return (a.dummy == b.dummy);
	}

//static inline BOOL operator!=(const AudioTypeConfig a, const AudioTypeConfig b)
inline BOOL operator!=(const AudioTypeConfig a, const AudioTypeConfig b)
	{
	return (a.dummy != b.dummy);
	}

struct MPEGDataSizePair
	{
	HBPTR		data;
	DWORD		size;
	int		timeStamp;
	DWORD		pad0;
	};


enum ForcedAspectRatio
	{
	FORCED_AR_DEFAULT	= 0,
	// 1 left out. Do not change the following two assignments:
	FORCED_AR_4_BY_3	= 2,
	FORCED_AR_16_BY_9	= 3
	};

enum FrameRateValue
	{
	FRV_23976 = 1,
	FRV_24000 = 2,
	FRV_25000 = 3,
	FRV_29970 = 4,
	FRV_30000 = 5,
	FRV_50000 = 6,
	FRV_59940 = 7,
	FRV_60000 = 8
	};

#define MP2SR_SCALE_BITS		0x80000L
#define MP2SR_SCALE_BYTES		0x10000L
#define MP2SR_SCALE_WORDS		0x08000L
#define MP2SR_SCALE_DWORDS		0x04000L
#define MP2SR_SCALE_DVDPES		0x00020L


// Bits for audio speaker test bitfield (MPEG2_AUDIO_TEST_SPEAKER_SELECT)
#define AUDIO_TEST_LEFT_SPEAKER				0x01
#define AUDIO_TEST_RIGHT_SPEAKER				0x02
#define AUDIO_TEST_CENTER_SPEAKER			0x04
#define AUDIO_TEST_SUBWOOFER_SPEAKER		0x08
#define AUDIO_TEST_LEFT_SURROUND_SPEAKER	0x10
#define AUDIO_TEST_RIGHT_SURROUND_SPEAKER	0x20



MKTAG(MPEG2_AUDIO_AC3,						MPEG_DECODER_UNIT,	0x1001, BOOL)
MKTAG(MPEG2_AUDIO_AC3_STREAMID,			MPEG_DECODER_UNIT,	0x1002, BYTE)
MKTAG(MPEG2_CODING_STANDARD,	  		   MPEG_DECODER_UNIT,	0x1003, BOOL)

MKTAG(MPEG2_AC3_BASS_CONFIG,				MPEG_DECODER_UNIT,	0x1004, AC3BassConfig)
MKTAG(MPEG2_AC3_SPEAKER_CONFIG,			MPEG_DECODER_UNIT,	0x1005, AC3SpeakerConfig)
MKTAG(MPEG2_AC3_CENTER_DELAY,				MPEG_DECODER_UNIT,	0x1006, WORD)
MKTAG(MPEG2_AC3_SURROUND_DELAY,			MPEG_DECODER_UNIT,	0x1007, WORD)

MKTAG(MPEG2_AC3_CENTER_VOLUME,			MPEG_DECODER_UNIT,	0x1008, WORD)
MKTAG(MPEG2_AC3_LEFT_SURROUND_VOLUME,  MPEG_DECODER_UNIT,	0x1009, WORD)
MKTAG(MPEG2_AC3_RIGHT_SURROUND_VOLUME,	MPEG_DECODER_UNIT,	0x100a, WORD)
MKTAG(MPEG2_AC3_SUBWOOFER_VOLUME,		MPEG_DECODER_UNIT,	0x100b, WORD)

MKTAG(MPEG2_AC3_DUAL_MODE_CONFIG,		MPEG_DECODER_UNIT,	0x100c, AC3DualModeConfig)
MKTAG(MPEG2_AC3_KARAOKE_CONFIG,			MPEG_DECODER_UNIT,	0x100d, AC3KaraokeConfig)

MKTAG(MPEG2_SPU_STREAMID,					MPEG_DECODER_UNIT,	0x1010, BYTE)
MKTAG(MPEG2_SPU_ENABLE,						MPEG_DECODER_UNIT,	0x1011, BOOL)

MKTAG(MPEG2_SPU_BUTTON_STATE,				MPEG_DECODER_UNIT,	0x1012, SPUButtonState)
MKTAG(MPEG2_SPU_BUTTON_LEFT,				MPEG_DECODER_UNIT,	0x1013, WORD)
MKTAG(MPEG2_SPU_BUTTON_TOP,				MPEG_DECODER_UNIT,	0x1014, WORD)
MKTAG(MPEG2_SPU_BUTTON_WIDTH,				MPEG_DECODER_UNIT,	0x1015, WORD)
MKTAG(MPEG2_SPU_BUTTON_HEIGHT,			MPEG_DECODER_UNIT,	0x1016, WORD)
MKTAG(MPEG2_SPU_BUTTON_SELECT_COLOR,	MPEG_DECODER_UNIT,	0x1017, DWORD)
MKTAG(MPEG2_SPU_BUTTON_ACTIVE_COLOR,	MPEG_DECODER_UNIT,	0x1018, DWORD)
MKTAG(MPEG2_SPU_PALETTE_ENTRY,			MPEG_DECODER_UNIT,	0x1019, DWORD)
	// Palette Bits 0..7 Pen, 8..15 V, 16..23 U, 24..31 Y
MKTAG(MPEG2_SPU_COMMAND_HOOK,				MPEG_DECODER_UNIT,	0x101a, MPEG2SPUCommandHookPtr)
MKTAG(MPEG2_SPU_BUTTON_ID,					MPEG_DECODER_UNIT,	0x101b, WORD)
MKTAG(MPEG2_SPU_BUTTON_STARTTIME,		MPEG_DECODER_UNIT,	0x101c, DWORD)
MKTAG(MPEG2_SPU_BUTTON_ENDTIME,			MPEG_DECODER_UNIT,	0x101d, DWORD)

MKTAG(MPEG2_AUDIO_LPCM,						MPEG_DECODER_UNIT,	0x1021, BOOL)
MKTAG(MPEG2_AUDIO_LPCM_STREAMID,			MPEG_DECODER_UNIT,	0x1022, BYTE)
MKTAG(MPEG2_LPCM_BITSPERSAMPLE,			MPEG_DECODER_UNIT,	0x1023, WORD)
MKTAG(MPEG2_LPCM_CHANNELS,					MPEG_DECODER_UNIT,	0x1024, WORD)

MKTAG(MPEG2_AUDIO_DTS_STREAMID,			MPEG_DECODER_UNIT,	0x1028, BYTE)

MKTAG(MPEG2_PRESENTATION_MODE,			MPEG_DECODER_UNIT,	0x1030, MPEG2PresentationMode)

MKTAG(MPEG2_DVD_STREAM_DEMUX,				MPEG_DECODER_UNIT,	0x1031, BOOL)
MKTAG(MPEG2_DVD_STREAM_ENCRYPTED,		MPEG_DECODER_UNIT,	0x1032, BOOL)


// Use this for overriding the default handling of switching ON/OFF SPDIF output
// (on decoders that can control SPDIF out)
MKTAG(MPEG2_SPDIF_HANDLING,				MPEG_DECODER_UNIT,	0x1034, SPDIFHandling)
MKTAG(MPEG2_SPDIF_OUTPUT_MODE,			MPEG_DECODER_UNIT,	0x1035, SPDIFOutputMode)
MKTAG(MPEG2_SPDIF_COPY_MODE,				MPEG_DECODER_UNIT,	0x1036, SPDIFCopyMode)

// This supercedes tags MPEG2_AUDIO_AC3 and MPEG2_AUDIO_LPCM, if available
MKTAG(MPEG2_AUDIO_TYPE,						MPEG_DECODER_UNIT,	0x103a, MPEG2AudioType)

// Specifies audio spatializing algorithm (e.g. SRS True Surround)
MKTAG(MPEG2_AUDIO_SPATIALIZER,			MPEG_DECODER_UNIT,	0x103b, MPEG2AudioSpatializer)

MKTAG(MPEG2_POSITION_SCALE,				MPEG_DECODER_UNIT,	0x1040, DWORD)

// Tags for downscaling the MPEG image and positioning it at an offset.
MKTAG(MPEG2_DOWNSCALE_FACTOR,				MPEG_DECODER_UNIT,	0x1050, int)
MKTAG(MPEG2_DOWNSCALE_HOFFSET,			MPEG_DECODER_UNIT,	0x1051, int)
MKTAG(MPEG2_DOWNSCALE_VOFFSET,			MPEG_DECODER_UNIT,	0x1052, int)
MKTAG(MPEG2_DOWNSCALE_HOFFSET_LBOXED,	MPEG_DECODER_UNIT,	0x1053, int)
MKTAG(MPEG2_DOWNSCALE_VOFFSET_LBOXED,	MPEG_DECODER_UNIT,	0x1054, int)


MKTAG(MPEG2_AC3_EQUALIZER_FRONT_ENABLE,MPEG_DECODER_UNIT,	0x1100, BOOL)
MKTAG(MPEG2_AC3_EQUALIZER_BACK_ENABLE,	MPEG_DECODER_UNIT,	0x1101, BOOL)
MKTAG(MPEG2_AC3_EQUALIZER_FRONT,			MPEG_DECODER_UNIT,	0x1104, DWORD)
MKTAG(MPEG2_AC3_EQUALIZER_BACK,			MPEG_DECODER_UNIT,	0x1105, DWORD)
MKTAG(MPEG2_STILL_FRAME_SEQUENCE,		MPEG_DECODER_UNIT,	0x1106, BOOL)
MKTAG(MPEG2_CURRENT_PLAYBACK_TIME,		MPEG_DECODER_UNIT,	0x1107, LONG)
MKTAG(MPEG2_AC3_VIRTUAL_3D_AUDIO,		MPEG_DECODER_UNIT,	0x1108, BOOL)

// Tags for split stream playback configuration
MKTAG(MPEG2_VIDEO_SPLIT_STREAM_TYPE,	MPEG_DECODER_UNIT,	0x1109, MPEG2SplitStreamType)
MKTAG(MPEG2_AUDIO_SPLIT_STREAM_TYPE,	MPEG_DECODER_UNIT,	0x1110, MPEG2SplitStreamType)
MKTAG(MPEG2_SPU_SPLIT_STREAM_TYPE,		MPEG_DECODER_UNIT,	0x1111, MPEG2SplitStreamType)

MKTAG(MPEG2_FORCED_SOURCE_ASPECT_RATIO, MPEG_DECODER_UNIT,	0x1120, ForcedAspectRatio)
// Switch to WDM playback (meaning: no navpacks in DVD data streams)
MKTAG(MPEG2_WDM_PLAYBACK,					MPEG_DECODER_UNIT,	0x1122, BOOL)
// Returns current value of the decoder's STC
MKTAG(MPEG2_CURRENT_STC,					MPEG_DECODER_UNIT,	0x1123, LONG)

MKTAG(MPEG2_REQUESTED_PLAYBACK_TIME,   MPEG_DECODER_UNIT,   0x1130, LONG)

// Closed Caption output on/off
MKTAG(MPEG2_CC_ENABLE,						MPEG_DECODER_UNIT,	0x1140, BOOL)

// Additional delay tags
MKTAG(MPEG2_AC3_LEFT_DELAY,				MPEG_DECODER_UNIT,	0x1200, WORD)
MKTAG(MPEG2_AC3_RIGHT_DELAY,				MPEG_DECODER_UNIT,	0x1201, WORD)
MKTAG(MPEG2_AC3_LEFT_SURROUND_DELAY,	MPEG_DECODER_UNIT,	0x1202, WORD)
MKTAG(MPEG2_AC3_RIGHT_SURROUND_DELAY,	MPEG_DECODER_UNIT,	0x1203, WORD)
MKTAG(MPEG2_AC3_SUBWOOFER_DELAY,			MPEG_DECODER_UNIT,	0x1204, WORD)

// Dolby Pro Logic Decoder config and mode
MKTAG(MPEG2_PRO_LOGIC_CONFIG,				MPEG_DECODER_UNIT,	0x1210, DolbyProLogicConfig)
MKTAG(MPEG2_PRO_LOGIC_MODE,				MPEG_DECODER_UNIT,	0x1211, DolbyProLogicMode)

// Low Frequency Enable (LFE). Applicable for AC3, MPEG2-MC and DTS stream playback
MKTAG(MPEG2_DECODE_LFE,						MPEG_DECODER_UNIT,	0x1220, BOOL)

// GET only tag that returns whether a LFE channel is present (and being decoded) or not
MKTAG(MPEG2_LFE_STATUS,						MPEG_DECODER_UNIT,	0x1221, BOOL)

// AC3 Audio Coding Mode (acmod property). GET only.
MKTAG(MPEG2_AC3_AUDIO_CODING_MODE,		MPEG_DECODER_UNIT,	0x1225, AC3AudioCodingMode)

// Returns TRUE if Pro Logic decoding is active
MKTAG(MPEG2_PROLOGIC_STATUS,				MPEG_DECODER_UNIT,	0x1226, BOOL)

// PCM output configuration
MKTAG(MPEG2_PCM_OUTPUT_CONFIG,			MPEG_DECODER_UNIT,	0x1230, PCMOutputConfig)

// Dynamic range
MKTAG(MPEG2_AC3_DYNAMIC_RANGE,			MPEG_DECODER_UNIT,	0x1236, AC3DynamicRange)
MKTAG(MPEG2_AC3_HIGH_DYNAMIC_RANGE,		MPEG_DECODER_UNIT,	0x1237, WORD)
MKTAG(MPEG2_AC3_LOW_DYNAMIC_RANGE,		MPEG_DECODER_UNIT,	0x1238, WORD)

// Audio type specific configurations
MKTAG(MPEG2_AC3_AUDIO_TYPE_CONFIG,		MPEG_DECODER_UNIT,	0x1240, AudioTypeConfig)
MKTAG(MPEG2_LPCM_AUDIO_TYPE_CONFIG,		MPEG_DECODER_UNIT,	0x1241, AudioTypeConfig)
MKTAG(MPEG2_DTS_AUDIO_TYPE_CONFIG,		MPEG_DECODER_UNIT,	0x1242, AudioTypeConfig)
MKTAG(MPEG2_MPEG_AUDIO_TYPE_CONFIG,		MPEG_DECODER_UNIT,	0x1243, AudioTypeConfig)
MKTAG(MPEG2_MPEG2_AUDIO_TYPE_CONFIG,	MPEG_DECODER_UNIT,	0x1244, AudioTypeConfig)
MKTAG(MPEG2_SDDS_AUDIO_TYPE_CONFIG,		MPEG_DECODER_UNIT,	0x1245, AudioTypeConfig)
MKTAG(MPEG2_CDDA_DTS_AUDIO_TYPE_CONFIG,MPEG_DECODER_UNIT,	0x1246, AudioTypeConfig)

MKTAG(MPEG2_PCM96_DOWNSAMPLE_MODE_DAC,		MPEG_DECODER_UNIT,	0x1300, PCM96DownsampleMode)
MKTAG(MPEG2_PCM96_DOWNSAMPLE_MODE_SPDIF,	MPEG_DECODER_UNIT,	0x1301, PCM96DownsampleMode)

MKTAG(MPEG2_VIDEO_MIN_PLAYBACK_SPEED,	MPEG_DECODER_UNIT,	0x1330, WORD)
MKTAG(MPEG2_VIDEO_MAX_PLAYBACK_SPEED,	MPEG_DECODER_UNIT,	0x1331, WORD)

MKTAG(MPEG2_AUDIO_MIN_PLAYBACK_SPEED,	MPEG_DECODER_UNIT,	0x1334, WORD)
MKTAG(MPEG2_AUDIO_MAX_PLAYBACK_SPEED,	MPEG_DECODER_UNIT,	0x1335, WORD)

MKTAG(MPEG2_VIDEO_MIN_SCAN_SPEED,		MPEG_DECODER_UNIT,	0x1340, WORD)
MKTAG(MPEG2_VIDEO_MAX_SCAN_SPEED,		MPEG_DECODER_UNIT,	0x1341, WORD)

MKTAG(MPEG2_AUDIO_MIN_SCAN_SPEED,		MPEG_DECODER_UNIT,	0x1344, WORD)
MKTAG(MPEG2_AUDIO_MAX_SCAN_SPEED,		MPEG_DECODER_UNIT,	0x1345, WORD)

// Speaker selection for audio tests (e.g. pink noise test). Bitfield defined above.
MKTAG(MPEG2_AUDIO_TEST_SPEAKER_SELECT,	MPEG_DECODER_UNIT,	0x1350, WORD)

// Data format for CDDA decoder, should be Get/Set
MKTAG(CDDA_DATA_FORMAT,						MPEG_DECODER_UNIT,	0x1400, DWORD)

MKTAG(UNITS_TIMEOUT_MSG,					MPEG_DECODER_UNIT,	0x1500, WORD)


// Karaoke control tags
#define KARAOKE_TAGVALUE_BASE  0x2000

MKTAG(MPEG2_AUDIO_KARAOKE_ENABLE,								MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  0, BOOL)
MKTAG(MPEG2_AUDIO_KARAOKE_LEFT_CHANNEL_MUSIC_VOLUME,		MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  1, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_RIGHT_CHANNEL_MUSIC_VOLUME,		MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  2, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_ENABLE_MUSIC_CHANNEL_MUTE,		MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  3, BOOL)

MKTAG(MPEG2_AUDIO_KARAOKE_LEFT_CHANNEL_VOICE_VOLUME,		MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  4, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_RIGHT_CHANNEL_VOICE_VOLUME,		MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  5, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_ENABLE_VOICE_CHANNEL_MUTE,		MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  6, BOOL)


MKTAG(MPEG2_AUDIO_KARAOKE_PITCH_SHIFT,							MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  7, int)
MKTAG(MPEG2_AUDIO_KARAOKE_ENABLE_PITCH_SHIFT,				MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  8, BOOL)

MKTAG(MPEG2_AUDIO_KARAOKE_VOICE_CANCELLATION,				MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE +  9, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_ENABLE_VOICE_CANCELLATION,		MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 10, BOOL)

MKTAG(MPEG2_AUDIO_KARAOKE_DUET_THRESHOLD,						MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 11, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_ENABLE_DUET,							MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 12, BOOL)

MKTAG(MPEG2_AUDIO_KARAOKE_VOICE_EFFECT,						MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 13, KaraokeVoiceEffectType )
MKTAG(MPEG2_AUDIO_KARAOKE_VOICE_EFFECT_DELAY,				MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 14, WORD)
MKTAG(MPEG2_AUDIO_KARAOKE_VOICE_EFFECT_BALANCE,				MPEG_DECODER_UNIT,	KARAOKE_TAGVALUE_BASE + 15, WORD)







#define GNR_CSS_NOT_SUPPORTED			MKERR(ERROR, DECRYPTION, OPERATION, 0x00)
#define GNR_AUTHENTICATION_FAILED	MKERR(ERROR, DECRYPTION, OPERATION, 0x00)

enum MPEG2DVDAuthenticationCommand
	{
	M2D_CHECK_DISK_KEY,				// 0
	M2D_CHECK_TITLE_KEY,          // 1
	M2D_START_AUTHENTICATION,     // 2
	M2D_READ_CHALLENGE_KEY,       // 3
	M2D_WRITE_BUS_KEY,            // 4
	M2D_WRITE_CHALLENGE_KEY,      // 5
	M2D_READ_BUS_KEY,             // 6
	M2D_WRITE_DISK_KEY,           // 7
	M2D_WRITE_TITLE_KEY,          // 8
	M2D_COMPLETE_AUTHENTICATION,  // 9
	M2D_CANCEL_AUTHENTICATION     // 10
	};





#if BOARD_HAS_MPEG2_ENCODER


#if LINUX
#include <stdio.h>
#define _export    /* FN: should be corrected in prelude.h.. */
#endif

void __cdecl MDebugPrint(const TCHAR * szFormat, ...);
#define RDP MDebugPrint

	#ifdef _PROPTEST
		extern Error Print(const TCHAR __far * szFormat, ...);
		#define DP Print
	#elif LINUX
		inline void DebugPrintEmpty(const TCHAR * szFormat, ...) {}		// empty function (optimized to nothing)
		#define DP while(0) DebugPrintEmpty
		#define DPF while(0) DebugPrintEmpty
		#define DPR while(0) DebugPrintEmpty
		inline void InitializeDebugRecording (void) {}
		inline void GetDebugRecordingParameters (BYTE * & array, int & size) {array=NULL; size=0;}
#else
		inline void __cdecl DebugPrintEmpty(const TCHAR __far * szFormat, ...) {}		// empty function (optimized to nothing)
		#define DP while(0) DebugPrintEmpty
	#endif


#define BREAKPOINT



#define GNR_CAN_NOT_PASSIVATE_IDLE_UNIT			MKERR(ERROR,	UNITS,	OBJECT,	0x01)

#define GNR_OBJECT_NOT_CURRENT						MKERR(ERROR,	UNITS,	OBJECT,	0x02)

#define GNR_OBJECT_ALREADY_JOINED					MKERR(ERROR,	UNITS,	OBJECT,	0x03)

#define GNR_UNITS_BUSY									MKERR(ERROR,	UNITS,	BUSY,		0x04)

#define GNR_INVALID_UNITS								MKERR(ERROR,	UNITS,	PARAMS,	0x05)




struct MPEGTransferDoneMsg {
	};

struct MPEGEncoderBufferStarvingMsg {
	};


MKHOOK(MPEGTransferDone, MPEGTransferDoneMsg)
MKHOOK(MPEGEncoderBufferStarving, MPEGEncoderBufferStarvingMsg)


typedef MPEGTransferDoneHook	__far *	MPEGTransferDoneHookPtr;
typedef MPEGEncoderBufferStarvingHook __far *  MPEGEncoderBufferStarvingHookPtr;


enum MPEGEncoderStreamType
	{
	MST_UNKNOWN					= 0x00,
	MST_AUDIO_ELEMENTARY		= 0x01,
	MST_VIDEO_ELEMENTARY		= 0x02,
	MST_PROGRAM					= 0x03,
	MST_TRANSPORT				= 0x04,
	MST_PACKET_ELEMENTARY	= 0x05,
	MST_PACKET_VIDEO_ES		= 0x06,
	MST_PACKET_AUDIO_ES		= 0x07,
	MST_MPEG1					= 0x08
	};


MKTAG(MPEG_TRANSFER_DONE_HOOK,		MPEG_ENCODER_UNIT,	0x0001, MPEGTransferDoneHookPtr)
MKTAG(MPEG_ENCODER_BUFFER_STARVING, MPEG_ENCODER_UNIT,   0x0002, MPEGEncoderBufferStarvingHookPtr)
MKTAG(MPEG_ENC_OUTPUT_TYPE,			MPEG_ENCODER_UNIT,   0x0003, MPEGEncoderStreamType)
MKTAG(MPEG_ENC_VIDEO_SOURCE,			MPEG_ENCODER_UNIT,	0x0004, VideoSource)
MKTAG(MPEG_ENC_VIDEOSTANDARD,			MPEG_ENCODER_UNIT,	0x0005, VideoStandard)
//MKTAG(MPEG_ENC_VIDEO_WIDTH,	MPEG_ENCODER_UNIT,	0x0002, WORD)
//MKTAG(MPEG_ENC_VIDEO_HEIGHT,  MPEG_ENCODER_UNIT,   0x0003, WORD)

//
// Previous definition conflicted with decoder definition.
// Should move shared defines to hardware\mpgcodec\generic\mpgcodec.h"
//
//#include "library\hardware\mpegdec\generic\mpegdec.h"








#endif



typedef class __far UnitSetClass __far *UnitSet;
typedef class __far VDRHandleClass __far *VDRHandle;

extern "C" {

DLLCALL void VDR_ServiceProcess(void);

DLLCALL Error WINAPI VDR_OpenDriver(char * name, DWORD boardID, VDRHandle __far & handle);
DLLCALL Error WINAPI VDR_CloseDriver(VDRHandle handle);

DLLCALL DWORD WINAPI VDR_AvailUnits(VDRHandle handle);

DLLCALL Error WINAPI VDR_ReconfigureDriver(VDRHandle handle);

DLLCALL Error WINAPI VDR_OpenUnits(VDRHandle handle, DWORD requnits, UnitSet &units);
DLLCALL Error WINAPI VDR_OpenSubUnits(UnitSet parent, DWORD requnits, UnitSet &units);
DLLCALL Error WINAPI VDR_CloseUnits(UnitSet units);


DLLCALL Error WINAPI VDR_ConfigureUnits(UnitSet units, TAG * tags);
inline Error __cdecl VDR_ConfigureUnitsTags(UnitSet units, TAG tags, ...) {return VDR_ConfigureUnits(units, &tags);}

DLLCALL Error WINAPI VDR_LockUnits(UnitSet units);
DLLCALL Error WINAPI VDR_UnlockUnits(UnitSet units);
DLLCALL Error WINAPI VDR_ActivateUnits(UnitSet units);
DLLCALL Error WINAPI VDR_PassivateUnits(UnitSet units);


DLLCALL Error WINAPI VDR_EnablePIP(UnitSet units, BOOL enable);
DLLCALL Error WINAPI VDR_UpdatePIP(UnitSet units);
DLLCALL Error WINAPI VDR_GrabFrame(UnitSet units, FPTR base,
                                   WORD width, WORD height,
                                   WORD stride,
                                   GrabFormat fmt);

DLLCALL Error WINAPI VDR_OptimizeBuffer(UnitSet units, WORD __far & minPixVal);


//
// MPEG Functions
//
DLLCALL DWORD	WINAPI VDR_SendMPEGData(UnitSet units, HPTR data, DWORD size);
DLLCALL void	WINAPI VDR_CompleteMPEGData(UnitSet units);
DLLCALL DWORD	WINAPI VDR_SendMPEGDataMultiple(UnitSet units, MPEGDataSizePair * data, DWORD size);

DLLCALL DWORD	WINAPI VDR_SendMPEGDataSplit(UnitSet units, MPEGElementaryStreamType streamType, HPTR data, DWORD size, LONG timeStamp);
DLLCALL void	WINAPI VDR_CompleteMPEGDataSplit(UnitSet units, MPEGElementaryStreamType streamType);
DLLCALL DWORD  WINAPI VDR_SendMPEGDataSplitMultiple(UnitSet units, MPEGElementaryStreamType streamType, MPEGDataSizePair * data, DWORD size, int timeStamp);
DLLCALL void	WINAPI VDR_RestartMPEGDataSplit(UnitSet units, MPEGElementaryStreamType streamType);

DLLCALL Error	WINAPI VDR_SendMPEGCommand(UnitSet units, MPEGCommand com, long param, DWORD __far &tag);
DLLCALL Error	WINAPI VDR_DoMPEGCommand(UnitSet units, MPEGCommand com, long param);
DLLCALL Error	WINAPI VDR_CompleteMPEGCommand(UnitSet units, DWORD tag);
DLLCALL BOOL	WINAPI VDR_MPEGCommandPending(UnitSet units, DWORD tag);

DLLCALL DWORD	WINAPI VDR_CurrentMPEGLocation(UnitSet units);
DLLCALL DWORD	WINAPI VDR_CurrentMPEGTransferLocation(UnitSet units);
DLLCALL MPEGState WINAPI VDR_CurrentMPEGState(UnitSet units);

DLLCALL Error	WINAPI VDR_InstallMPEGWinHooks(UnitSet units, HWND hwnd, WORD refillMsg, WORD signalMsg, WORD doneMsg);
DLLCALL Error	WINAPI VDR_RemoveMPEGWinHooks(UnitSet units);
DLLCALL Error	WINAPI VDR_CompleteMPEGRefillMessage(UnitSet units);



#if BOARD_HAS_MPEG2_ENCODER

DLLCALL Error  WINAPI VDR_DoCommand(UnitSet units, MPEGCommand com, long param);
DLLCALL Error	WINAPI VDR_PutBuffer(UnitSet units, HPTR data, DWORD size);
DLLCALL Error  WINAPI VDR_InstallMPEGTransferDoneHook(UnitSet units, HWND hwnd, WORD transferDoneMsg);
DLLCALL Error  WINAPI VDR_RemoveMPEGTransferDoneHook(UnitSet units);
DLLCALL Error  WINAPI VDR_InstallMPEGEncoderBufferStarvingHook(UnitSet units, HWND hwnd, WORD bufferStarvingMsg);
DLLCALL Error  WINAPI VDR_RemoveMPEGEncoderBufferStarvingHook(UnitSet units);

#endif



	}



#define GNR_DRIVE_FAILURE							MKERR(ERROR,	DISK,			HARDWARE,			0x00)
// General drive failure

#define GNR_DRIVE_FATAL_ERROR						MKERR(ERROR,	DISK,			HARDWARE,			0x01)
// Drive firmware is not responding any more

#define GNR_NO_VALID_DISK							MKERR(ERROR,	DISK,			OBJECT,				0x00)
// No valid/known disk was found

#define GNR_NO_DRIVE									MKERR(ERROR,	DISK,			OBJECT,				0x02)
// Drive does not exist/could not be found

#define GNR_DRIVE_DETACHED							MKERR(ERROR,	DISK,			OBJECT,				0x03)
// The drive has been detached -> is not available for playback

#define GNR_NO_DVD_DRIVE							MKERR(ERROR,	DISK,			OBJECT,				0x04)
// The drive is not a DVD drive (used on PCs)

#define GNR_BLOCK_ALREADY_LOCKED					MKERR(ERROR,	DISK,			OBJECT,				0x05)
// A block was already locked in a different mode (read/write)

#define GNR_BLOCK_NOT_LOCKED						MKERR(ERROR,	DISK,			OBJECT,				0x06)
// The block not be unlocked is not locked

#define GNR_INVALID_DRIVE_LETTER					MKERR(ERROR,	DISK,			BOUNDS,				0x01)
// The drive letter specified was not correct/out of range

#define GNR_DISK_READ_ONLY							MKERR(ERROR,	DISK,			FILEIO,				0x00)
// The disk is read-only

#define GNR_DRIVE_LOCK_FAILED						MKERR(ERROR,	DISK,			OPERATION,			0x00)
// Unable to lock/unlock the drive

#define GNR_DRIVE_ALREADY_LOCKED					MKERR(WARNING,	DISK,			OPERATION,			0x01)
// The drive was already locked

#define GNR_DRIVE_LOAD_FAILED						MKERR(ERROR,	DISK,			OPERATION,			0x02)
// Could not load/unload the drive

#define GNR_DRIVE_NOT_LOADABLE					MKERR(ERROR,	DISK,			OPERATION,			0x03)
// Drive does not support loading/unloading the disk

#define GNR_READ_ERROR								MKERR(ERROR,	DISK,			OPERATION,			0x04)
// Some read error occurred

#define GNR_WRITE_ERROR								MKERR(ERROR,	DISK,			OPERATION,			0x05)
// Some write error occurred

#define GNR_COPY_PROTECTION_VIOLATION			MKERR(ERROR,	DISK,			COPYPROTECTION,	0x00)

#define GNR_COPY_PROTECTION_FAILED				MKERR(ERROR,	DISK,			COPYPROTECTION,	0x01)

#define GNR_READ_ERROR_SECTOR_ENCRYPTED		MKERR(ERROR,	DISK,			COPYPROTECTION,	0x02)
// A sector was requested which happened to be encrypted, but the authentication process was not completed


#define GNR_PATH_NOT_FOUND							MKERR(ERROR,	FILE,			OBJECT,			0x00)
// The path specified was not found

#define GNR_INVALID_PATH							MKERR(ERROR,	FILE,			OBJECT,			0x01)
// The path specified was invalid (e.g. contained invalid letters)

#define GNR_NO_FILE_SYSTEM							MKERR(ERROR,	FILE,			OBJECT,			0x02)
// There is no file system to execute operation

#define GNR_NO_VOLUME								MKERR(ERROR,	FILE,			OBJECT,			0x03)
// There is no volume to execute operation

#define GNR_VOLUME_INVALID							MKERR(ERROR,	FILE,			OBJECT,			0x04)
// Volume is invalid for some reason, e.g. not supported

#define GNR_ITEM_NOT_FOUND							MKERR(ERROR,	FILE,			OBJECT,			0x05)
// The item was not found or there is no more file in the directory

#define GNR_NOT_A_DIRECTORY						MKERR(ERROR,	FILE,			OBJECT,			0x06)
// Disk item is not a directory

#define GNR_ITEM_INVALID							MKERR(ERROR,	FILE,			OBJECT,			0x07)
// Item is invalid (e.g. for an operation)

#define GNR_FILE_READ_ONLY							MKERR(ERROR,	FILE,			OPERATION,		0x00)
// The file is read only

//#define GNR_FILE_IN_USE


//  Navigation in general

#define GNR_UNEXPECTED_NAVIGATION_ERROR				MKERR(ERROR,	NAVIGATION,	INTERNALSTATE,		0x00)
// Some internal error happened

#define GNR_INVALID_PLAYER									MKERR(ERROR,	NAVIGATION,	OBJECT,				0x00)
// The player handle is invalid

#define GNR_INVALID_NAV_INFO								MKERR(ERROR,	NAVIGATION,	OBJECT,				0x02)
// The navigation information is invalid (possible causes: wrong authoring, read error from drive, internal error)

#define GNR_LOST_DECODER									MKERR(ERROR,	NAVIGATION,	OBJECT,				0x03)
// The decoder has been preempted

#define GNR_OPERATION_NOT_SUPPORTED						MKERR(ERROR,	NAVIGATION, OPERATION,			0x00)
// This operation is (currently) not supported by the player

#define GNR_TITLE_NOT_FOUND								MKERR(ERROR,	NAVIGATION,	OPERATION,			0x01)
// The title specified does not exist

#define GNR_CHAPTER_NOT_FOUND								MKERR(ERROR,	NAVIGATION,	OPERATION,			0x02)
// The chapter specified does not exist

#define GNR_TIME_OUT_OF_RANGE								MKERR(ERROR,	NAVIGATION,	OPERATION,			0x03)
// The time specified (e.g. for a search operation) is out of range

#define GNR_PROGRAM_LINK_NOT_FOUND						MKERR(ERROR,	NAVIGATION,	OPERATION,			0x04)
// The current program does not have a next/prev/upper/etc. program

#define GNR_MENU_DOES_NOT_EXIST							MKERR(ERROR,	NAVIGATION,	OPERATION,			0x05)
// The requested menu does not exist

#define GNR_STREAM_NOT_FOUND								MKERR(ERROR,	NAVIGATION,	OPERATION,			0x06)
// The stream (audio/subpicture) does not exist

#define GNR_FLAGS_NOT_SUPPORTED							MKERR(ERROR,	NAVIGATION,	OPERATION,			0x07)
// The flags specified for this operation are not supported

#define GNR_BUTTON_NOT_FOUND								MKERR(ERROR,	NAVIGATION,	OPERATION,			0x08)
// The specified button does not exist

#define GNR_UNKNOWN_EVENT_TYPE							MKERR(ERROR,	NAVIGATION,	OPERATION,			0x09)
// The event type specified does not exist

#define GNR_BREAKPOINT_NOT_FOUND							MKERR(ERROR,	NAVIGATION,	OPERATION,			0x0a)
// The breakpoint to be deleted does not exist

#define GNR_INVALID_UNIQUE_KEY							MKERR(ERROR,	NAVIGATION,	OPERATION,			0x0b)
// The unique key could not be created/was invalid

//
//  DVD specific errors
//

#define GNR_UOP_PROHIBITED									MKERR(ERROR,	DVD,			UOP,					0x01)
// The current UOPs do not allow this action

#define GNR_REGIONS_DONT_MATCH							MKERR(ERROR,	DVD,			REGION,				0x00)
// The system and disk regions don't match

#define GNR_REGION_CODE_INVALID							MKERR(ERROR,	DVD,			REGION,				0x01)
// The region code specified was invalid (e.g. multiregion when trying to set region)

#define GNR_NO_MORE_REGION_SETS							MKERR(ERROR,	DVD,			REGION,				0x02)
// The number of region changes is exhausted

#define GNR_REGION_WRITE_ERROR							MKERR(ERROR,	DVD,			REGION,				0x03)
// Unable to write region

#define GNR_REGION_READ_ERROR								MKERR(ERROR,	DVD,			REGION,				0x04)
// Unable to read region

#define GNR_PARENTAL_LEVEL_TOO_LOW						MKERR(ERROR,	DVD,			PARENTAL,			0x03)
// The parental level currently set is too low to play the disc

#define GNR_ILLEGAL_NAVIGATION_COMMAND					MKERR(ERROR,	DVD,			OPERATION,			0x00)
// The DVD contains an illegal navigation command

#define GNR_ILLEGAL_DOMAIN									MKERR(ERROR,	DVD,			OPERATION,			0x01)
// The domain is illegal for this operation

#define GNR_NO_RESUME_INFORMATION						MKERR(ERROR,	DVD,			OPERATION,			0x02)
// No resume information currently available

//
//  VCD specific errors
//

//
//  CDDA specific errors
//


enum DVDPlayerMode
	{
	DPM_STOPPED,
	DPM_PAUSED,
	DPM_STILL,
	DPM_PLAYING,
	DPM_BUSY,
	DPM_SCANNING,
	DPM_INACTIVE,
	DPM_REVERSEPLAY,
	DPM_TRICKPLAY,
	DPM_REVERSESCAN
	};

enum DVDDomain
	{
	FP_DOM,
	VMGM_DOM,
	VTSM_DOM,
	TT_DOM,
	STOP_DOM
	};

enum VTSMenuType
	{
	VMT_TITLE_MENU,
	VMT_ROOT_MENU,
	VMT_SUBPICTURE_MENU,
	VMT_AUDIO_MENU,
	VMT_ANGLE_MENU,
	VMT_PTT_MENU,
	VMT_NO_MENU		// Note that this is used in ExtendedPlayerState as array size
	};

enum DisplayPresentationMode
	{
	DPM_4BY3,
	DPM_16BY9,
	DPM_LETTERBOXED,
	DPM_PANSCAN,
	DPM_NONE
	};

enum DVDAudioCodingMode
	{
	DAM_AC3,
	DAM_MPEG1,
	DAM_MPEG2,
	DAM_LPCM,
	DAM_DTS,
	DAM_SDDS
	};

enum DVDAudioApplicationMode
	{
	DAAM_UNDEFINED		= 0,
	DAAM_KARAOKE		= 1,
	DAAM_SURROUND		= 2
	};

enum MPEG2PrologicStatus		// Status of the decoder (actually played!)
	{
	MP2PS_OFF,
	MP2PS_ON,
	MP2PS_UNKNOWN
	};

enum MPEG2LFEStatus				// Content, not playing
	{
	MP2LFE_OFF,
	MP2LFE_ON,
	MP2LFE_UNKNOWN
	};

enum DVDDiskType
	{
	DDT_NONE,
	DDT_VIDEO_DVD,
	DDT_AUDIO_DVD,
	DDT_VIDEO_CD,
	DDT_AUDIO_CD,
	DDT_SUPER_AUDIO_CD,
	DDT_DTS_AUDIO_CD,
	DDT_SUPER_VIDEO_CD,
	DDT_MULTI_DISK
	};

enum RegionSource
	{
	RGSRC_UNDEFINED	= 0,
	RGSRC_DRIVE			= 1,
	RGSRC_BOARD			= 2
	};

enum VideoCompressionMode
	{
	VCM_UNKNOWN,
	VCM_MPEG1,
	VCM_MPEG2
	};

enum Line21Mode
	{
	L21M_NO_DATA,
	L21M_FIRST_FIELD,
	L21M_SECOND_FIELD,
	L21M_BOTH_FIELDS
	};

//
//  User Operation Bitmasks
//

#define UOP_TIME_PLAY_SEARCH				MKFLAG(0)
#define UOP_PTT_PLAY_SEARCH				MKFLAG(1)
#define UOP_TITLE_PLAY						MKFLAG(2)
#define UOP_STOP								MKFLAG(3)
#define UOP_GO_UP								MKFLAG(4)
#define UOP_TIME_PTT_SEARCH				MKFLAG(5)
#define UOP_PREV_TOP_PG_SEARCH			MKFLAG(6)
#define UOP_NEXT_PG_SEARCH					MKFLAG(7)
#define UOP_FORWARD_SCAN					MKFLAG(8)
#define UOP_BACKWARD_SCAN					MKFLAG(9)
#define UOP_SCAN_OFF							MKFLAG(25)	// additional
#define UOP_MENU_CALL_TITLE				MKFLAG(10)
#define UOP_MENU_CALL_ROOT					MKFLAG(11)
#define UOP_MENU_CALL_SUB_PICTURE		MKFLAG(12)
#define UOP_MENU_CALL_AUDIO				MKFLAG(13)
#define UOP_MENU_CALL_ANGLE				MKFLAG(14)
#define UOP_MENU_CALL_PTT					MKFLAG(15)
#define UOP_RESUME							MKFLAG(16)
#define UOP_BUTTON							MKFLAG(17)
#define UOP_STILL_OFF						MKFLAG(18)
#define UOP_PAUSE_ON							MKFLAG(19)
#define UOP_PAUSE_OFF						MKFLAG(26)	// additional
#define UOP_AUDIO_STREAM_CHANGE			MKFLAG(20)
#define UOP_SUB_PICTURE_STREAM_CHANGE	MKFLAG(21)
#define UOP_ANGLE_CHANGE					MKFLAG(22)
#define UOP_KARAOKE_MODE_CHANGE			MKFLAG(23)
#define UOP_VIDEO_MODE_CHANGE				MKFLAG(24)

//
//  Disc Information Structure
//

class __far DDPDiskInfo
	{
	public:
		DDPDiskInfo(void)			{size = sizeof(DDPDiskInfo);}

		DWORD				size;				// Size of the structure passed, DO NOT CHANGE!!!
		DVDDiskType		type;				// Type of media inserted
		char				uniqueKey[8];	// Unique identifier of disk
		BYTE				systemRegion;	// System region (decoder or drive, only for type == DDT_VIDEO_DVD)
		BYTE				diskRegion;		// Disk region (only for type == DDT_VIDEO_DVD)
		RegionSource	regionSource;	// Region source (decoder or drive, only for type == DDT_VIDEO_DVD)
		BYTE				availSets;		// Number of region sets left (only for type == DDT_VIDEO_DVD)
	};

//
//  DVD Time Class
//

class __far DVDTime
	{
	private:
		DWORD	stamp;  // Format HHHH HHHH MMMM MMMM SSSS SSSS RR FF FFFF
	public:
		DVDTime(DWORD stamp) {this->stamp = stamp;}
		DVDTime(void) {this->stamp = 0;}
		DVDTime(int hours, int minutes, int seconds, int frames, int frameRate);
		DVDTime(int millisecs, int divider, int frameRate);

		int FrameRate(void)	const {return XTBF(7, stamp) ? 30 : 25;}
		int Frames(void)		const {return (int)(XTBF(0, 4, stamp) + 10 * XTBF(4, 2, stamp));}
		int Seconds(void)		const {return (int)(XTBF(8, 4, stamp) + 10 * XTBF(12, 4, stamp));}
		int Minutes(void)		const {return (int)(XTBF(16, 4, stamp) + 10 * XTBF(20, 4, stamp));}
		int Hours(void)		const {return (int)(XTBF(24, 4, stamp) + 10 * XTBF(28, 4, stamp));}

		DWORD FrameTotal(void)	const	{return (Seconds() + Minutes() * 60 + Hours() * 3600) * FrameRate() + Frames();}

		DWORD Millisecs(void) const {return 1000 * Frames() / FrameRate() +
		                              1000 * Seconds() +
		                              60000 * Minutes() +
		                              3600000 * Hours();}

		friend DVDTime operator+ (const DVDTime u, const DVDTime v);
		friend DVDTime operator- (const DVDTime u, const DVDTime v);

		DVDTime & operator+= (const DVDTime u);
		DVDTime & operator-= (const DVDTime u);

		int Compare(const DVDTime u) const;

		friend BOOL operator==(const DVDTime u, const DVDTime v) {return u.Compare(v) == 0;}
		friend BOOL operator!=(const DVDTime u, const DVDTime v) {return u.Compare(v) != 0;}
		friend BOOL operator<(const DVDTime u, const DVDTime v)  {return u.Compare(v) < 0;}
		friend BOOL operator>(const DVDTime u, const DVDTime v)  {return u.Compare(v) > 0;}
		friend BOOL operator<=(const DVDTime u, const DVDTime v) {return u.Compare(v) <= 0;}
		friend BOOL operator>=(const DVDTime u, const DVDTime v) {return u.Compare(v) >= 0;}

		BOOL IsZero(void)		const {return (stamp & 0xffffff3f) == 0;}
		BOOL IsNotZero(void)	const {return (stamp & 0xffffff3f) != 0;}
	};

//
//  DVD Location
//

class __far DVDLocation
	{
	public:
		DVDDomain	domain;
		WORD			videoTitleSet;
		WORD			title;
		WORD			vtsTitle;
		DVDTime		titleTime;
		WORD			partOfTitle;
		WORD			programChain;
		DVDTime		pgcTime;
		WORD			program;
		WORD			cell;
		DVDTime		cellTime;

		DVDLocation(void) {};
		friend BOOL	NotEqual(const DVDLocation u, const DVDLocation v);
		friend BOOL operator !=(const DVDLocation u, const DVDLocation v) { return (BOOL)memcmp(&u, &v, sizeof(DVDLocation)); }
	};

//
//  Audio Stream Format
//

class __far DVDAudioStreamFormat
	{
	public:
		WORD						languageCode;
		WORD						languageExtension;
		DVDAudioCodingMode	codingMode;
		WORD						bitsPerSample;
		DWORD						samplesPerSecond;
		WORD						channels;

		friend BOOL operator==(const DVDAudioStreamFormat u, const DVDAudioStreamFormat v);
		friend BOOL operator!=(const DVDAudioStreamFormat u, const DVDAudioStreamFormat v);
	};

//
//  Video Stream Format
//

class VideoStreamFormat
	{
	public:
		VideoCompressionMode			compressionMode;			// Compression technique used
		VideoStandard					videoStandard;				// Source video standard
		DisplayPresentationMode		sourceAspectRatio;		// Aspect ratio of source picture
		BOOL								panScanOn4By3;				// TRUE if Pan & Scan is allowed on 4 by 3 display
		BOOL								letterboxedOn4By3;		// TRUE if letterboxed is allowed on 4 by 3 display
		Line21Mode						line21Mode;					// Encoded line 21 data
		WORD								sourceWidth;				// Width of source picture
		WORD								sourceHeight;				// Height of source picture
		BOOL								sourceLetterboxed;		// TRUE is source material is letterboxed

		friend BOOL operator==(const VideoStreamFormat & u, const VideoStreamFormat & v);
		friend BOOL operator!=(const VideoStreamFormat & u, const VideoStreamFormat & v);
	};

//
//  Subpicture Stream Format
//

class __far DVDSubPictureStreamFormat
	{
	public:
		WORD						languageCode;
		WORD						languageExtension;

		friend BOOL operator==(const DVDSubPictureStreamFormat u, const DVDSubPictureStreamFormat v);
		friend BOOL operator!=(const DVDSubPictureStreamFormat u, const DVDSubPictureStreamFormat v);
	};

//
//  Button Information
//

#define DBI_MAX_BUTTON_NUMBER 103

class __far DVDButtonInformation
	{
	public:
		WORD	x;
		WORD	y;
		WORD	w;
		WORD	h;
		BOOL	autoAction;
		BYTE	upperButton;
		BYTE	lowerButton;
		BYTE	leftButton;
		BYTE	rightButton;

		friend BOOL operator==(const DVDButtonInformation u, const DVDButtonInformation v);
		friend BOOL operator!=(const DVDButtonInformation u, const DVDButtonInformation v);
	};

//
//  Extended player state
//

class __far ExtendedPlayerState
	{
	public:
		DWORD								size;								// Size of the structure passed

		// Request bitmasks

		DWORD								request;							// Bitmask for requested data
		DWORD								valid;							// Bitmask for delivered data
		DWORD								monitor;							// Bitmask indicating the elements to check for changes
		DWORD								changed;							// Bitmask indicating the elements that have changed

		// Player Information (EPS_PLAYERINFO, additions below)

		DVDPlayerMode					playerMode;						// Current player mode
		WORD								playbackSpeed;					// Current playback speed
		WORD								scanSpeed;						// Current scan speed
		DWORD								forbiddenUOPs;					// Forbidden UOPs

		// Hardware Information (EPS_MISC)

		DVDDiskType						diskType;						// Current disk type
		DisplayPresentationMode		displayMode;					// Current display mode
		VideoStandard					videoStandard;					// Current video standard
		DWORD								currentBitRate;				// Current bitrate

		// Audio Stream Information (EPS_AUDIOSTREAMS, additional info below)

		int								currentAudioStream;			// Current Audio Stream ID
		BYTE								availableAudioStreams;		// Bitmask of audio streams available
		DVDAudioStreamFormat			audioStream[8];				// Audio stream data

		// Sub Picture Stream Information (EPS_SUBPICTURESTREAMS)

		int								currentSubPictureStream;	// Current sub picture Stream ID
		DWORD								availableSubPictureStreams;// Bitmask of sub picture streams available
		DVDSubPictureStreamFormat	subPictureStream[32];		// Subpicture stream data

		// Title Information (EPS_TITLE)

		DVDLocation						location;						// Current location
		DVDLocation						duration;						// Current duration

		// Menu Information (EPS_MENU)

		WORD								menuLanguage;
		BOOL								menuAvailable[VMT_NO_MENU+1];  // Use a VTSMenuType as index

		// Parental Information (EPS_PARENTAL)

		WORD								parentalCountry;
		WORD								parentalLevel;

		// Angle Information (EPS_ANGLE, addition below)

		WORD								numberOfAngles;
		WORD								currentAngle;

		// Button Information (EPS_BUTTON)

		WORD								numberOfButtons;
		WORD								selectedButton;
		WORD								forcedlyActivatedButton;
		WORD								userButtonOffset;
		WORD								numberOfUserButtons;
		DVDButtonInformation			buttonInfo[DBI_MAX_BUTTON_NUMBER];

		// CDDA Information (EPS_PLAYINGGAP)

		BOOL								playingGap;						// CDDA only: TRUE if gap between titles is played

		// Additional Player Information (EPS_PLAYERINFO)

		BOOL								playingForward;

		// Additional Angle Information (EPS_ANGLE)

		BOOL								isMultiAngleScene;
		WORD								playingAngle;

		// Video stream information (EPS_VIDEOSTREAM, additional to info in EPS_MISC)

		VideoStreamFormat				videoStream;

		// Hardware Information (EPS_MISC, additional)

		BOOL								diskIsEncrypted;				// TRUE if disk is encrypted

		// Additional Audio Information

		MPEG2PrologicStatus			mpeg2PrologicStatus;			// Indicating if Prologic decoding is currently done
		MPEG2LFEStatus					mpeg2LFEStatus;				// Indicates if LFE info is in content
		AC3AudioCodingMode			ac3AudioCodingMode;			// Encoding, not actual output

		// Karaoke information (EPS_KARAOKE)

		BYTE								applicationMode[8];			// contains Karaoke information for each audio stream
	};

//
//  Request bitmask defines (00000200 in use)
//

#define EPS_PLAYERINFO				0x00000001
#define EPS_MISC						0x00000002
#define EPS_TITLE						0x00000004
#define EPS_AUDIOSTREAMS			0x00000008
#define EPS_SUBPICTURESTREAMS		0x00000010
#define EPS_ANGLE						0x00000020
#define EPS_MENU						0x00000040
#define EPS_PARENTAL					0x00000080
#define EPS_BUTTON					0x00000100
#define EPS_PLAYINGGAP				0x00000200
#define EPS_VIDEOSTREAM				0x00000400
#define EPS_KARAOKE					0x00000800

//
//  Shortcuts
//

#define EPS_COMMON					EPS_PLAYERINFO | EPS_MISC | EPS_TITLE
#define EPS_DVD						EPS_MENU | EPS_PARENTAL | EPS_BUTTON | EPS_ANGLE | EPS_AUDIOSTREAMS | \
											EPS_SUBPICTURESTREAMS | EPS_VIDEOSTREAM
#define EPS_VCD						EPS_BUTTON | EPS_AUDIOSTREAMS
#define EPS_CDA						EPS_PLAYINGGAP | EPS_AUDIOSTREAMS
#define EPS_ALL						EPS_COMMON | EPS_DVD | EPS_VCD | EPS_CDA

//
//  Disk Navigation Event handling
//

typedef void (WINAPI * DNEEventHandler)(DWORD event, void * userData, DWORD info);

#define DNE_NONE								0
#define DNE_TITLE_CHANGE					1
#define DNE_PART_OF_TITLE_CHANGE			2
#define DNE_VALID_UOP_CHANGE				3
#define DNE_ANGLE_CHANGE					4
#define DNE_AUDIO_STREAM_CHANGE			5
#define DNE_SUBPICTURE_STREAM_CHANGE	6
#define DNE_DOMAIN_CHANGE					7
#define DNE_PARENTAL_LEVEL_CHANGE		8
#define DNE_BITRATE_CHANGE					9
#define DNE_STILL_ON							10
#define DNE_STILL_OFF						11
#define DNE_PLAYBACK_MODE_CHANGE			12
#define DNE_CDA_PLAYING_GAP				13
#define DNE_READ_ERROR						14
#define DNE_DISPLAY_MODE_CHANGE			15
#define DNE_STREAMS_CHANGE					16
#define DNE_SCAN_SPEED_CHANGE				17
#define DNE_PLAYBACK_SPEED_CHANGE		18
#define DNE_VIDEO_STANDARD_CHANGE		19
#define DNE_BREAKPOINT_REACHED			20
#define DNE_DRIVE_DETACHED					21
#define DNE_ERROR								22

#define DNE_EVENT_NUMBER					23

//
//  ERS Breakpoints
//

#define ERS_BREAKPOINT_NONE				0xffffffff	// Defines an invalid breakpoint id

//
//  Flags for DDP_SetBreakpoint()
//

enum ERSBreakpointFlags
	{
	ERS_NONE					=	0x0000,	// No flags, normal breakpoint
	ERS_PAUSE				=	0x0001,	// Go to still mode when reaching breakpoint
	ERS_AUTOCLEAR			=	0x0002,	// Delete breakpoint when it has been reached
	ERS_PAUSEATEND			=	0x0004	// Trigger at end of title
	};

//
//  Flags for DDP_SetBreakpointExt()
//

enum ERSBreakpointExtFlags
	{
	ERS_ATTIME			=	0x0001,	// Trigger when reaching time in title
	ERS_ENDOFTITLE		=	0x0002,	// Trigger at end of title
	ERS_ENDOFPTT		=	0x0004,	// Trigger at and of part of title
	ERS_FORWARD			=	0x0008,	// Trigger when playing forward
	ERS_BACKWARD		=	0x0010,	// Trigger when playing backward
	ERS_TOSTILL			=	0x0020,	// Go to still when reaching breakpoint
	ERS_CLEAR			=	0x0040,	// Automatically delete breakpoint after triggering
	ERS_BEGINOFPTT		=	0x0080	// Trigger at begin of PTT
	};

// flags for ExtendedPlay(DWORD flags, WORD title, WORD ptt, DVDTime time);

#define DDPEPF_PAUSE_AT_START		1
#define DDPEPF_USE_TIME				2
#define DDPEPF_USE_PTT				4
#define DDPEPF_USE_TITLE			8

// flags for DetachDrive(DWORD flags, DVDTime time);

#define DDPDDF_DETACH_IMMEDIATE				1
#define DDPDDF_DETACH_AT_TIME					2
#define DDPDDF_DETACH_AT_END_OF_CELL		4
#define DDPDDF_DETACH_AT_END_OF_PROGRAM	8
#define DDPDDF_DETACH_AT_END_OF_PGC			16

#define DDPDDF_DETACH_ANY						31

#define DDPDDF_STREAM_PREFETCH				32
#define DDPDDF_PREFETCH_BUFFER_SIZE			64

// Flags for DDP_StartPresentationExt and DDP_DefrostExt

#define DDPSPF_NONE		0
#define DDPSPF_TOPAUSE	1


typedef class CDDiskPlayerClass * DVDDiskPlayer;

extern "C" {

#ifndef DLLCALL
#define DLLCALL __declspec(dllexport)
#endif


DLLCALL Error WINAPI  DDP_CheckDrive(char driveLetter, char __far * id);

DLLCALL Error WINAPI  DDP_CheckDriveWithPath(char * drivePath, char __far * id);

DLLCALL Error WINAPI  DDP_GetVolumeName(char driveLetter, char * name);

DLLCALL Error WINAPI  DDP_GetDiskRegionSettings(UnitSet units, char driveLetter, BYTE __far &region, BYTE __far &availSets, RegionSource __far & regionSource, BYTE & diskRegion);

DLLCALL Error WINAPI  DDP_GetRegionSettings(UnitSet units, char driveLetter, BYTE __far &region, BYTE __far &availSets, RegionSource __far &regionSource);

DLLCALL Error WINAPI  DDP_SetSystemRegion(UnitSet units, char driveLetter, BYTE region);

DLLCALL Error WINAPI  DDP_EjectDiskInDrive(char driveLetter);

DLLCALL Error WINAPI  DDP_GetDriveStatus(char driveLetter, BOOL & opened, BOOL & closed);

DLLCALL Error WINAPI  DDP_LoadDiskInDrive(char driveLetter, BOOL wait);

DLLCALL Error WINAPI  DDP_OpenPlayer(UnitSet units, char diskLetter, DVDDiskPlayer __far & player);

DLLCALL Error WINAPI  DDP_OpenPlayerWithPath(UnitSet units, char * drivePath, DVDDiskPlayer __far & player);

DLLCALL Error WINAPI  DDP_OpenPlayerExtended(UnitSet units, char driveLetter, char * drivePath,
															DVDDiskPlayer __far & player, DDPDiskInfo __far & diskInfo);

DLLCALL Error WINAPI  DDP_ClosePlayer(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_StartPresentation(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_StartPresentationExt(DVDDiskPlayer player, DWORD flags);

DLLCALL DVDDiskType	 WINAPI DDP_GetDiskType(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_GetCurrentLocation(DVDDiskPlayer player, DVDLocation __far & location);

DLLCALL Error WINAPI  DDP_GetCurrentDuration(DVDDiskPlayer player, DVDLocation __far & location);

DLLCALL Error WINAPI  DDP_GetTitleDuration(DVDDiskPlayer player, WORD title, DVDTime & duration);

DLLCALL DVDPlayerMode WINAPI DDP_GetPlayerMode(DVDDiskPlayer player);

DLLCALL Error WINAPI	 DDP_GetExtendedPlayerState(DVDDiskPlayer player, ExtendedPlayerState & eps);

DLLCALL DWORD WINAPI  DDP_GetForbiddenUserOperations(DVDDiskPlayer player);

DLLCALL WORD  WINAPI  DDP_GetNumberOfTitles(DVDDiskPlayer player);

DLLCALL WORD  WINAPI  DDP_GetNumberOfPartOfTitle(DVDDiskPlayer player, WORD title);

DLLCALL Error WINAPI  DDP_GetAvailStreams(DVDDiskPlayer player, BYTE __far & audioMask, DWORD __far & subPictureMask);

DLLCALL Error WINAPI  DDP_GetAudioStreamAttributes(DVDDiskPlayer player, WORD stream, DVDAudioStreamFormat __far & format);

DLLCALL Error WINAPI  DDP_GetSubPictureStreamAttributes(DVDDiskPlayer player, WORD stream, DVDSubPictureStreamFormat __far & format);

DLLCALL WORD  WINAPI  DDP_GetCurrentAudioStream(DVDDiskPlayer player);

DLLCALL WORD  WINAPI  DDP_GetCurrentSubPictureStream(DVDDiskPlayer player);

DLLCALL BOOL  WINAPI  DDP_IsCurrentSubPictureEnabled(DVDDiskPlayer player);

DLLCALL WORD  WINAPI  DDP_GetNumberOfAngles(DVDDiskPlayer player, WORD title);

DLLCALL WORD  WINAPI  DDP_GetCurrentAngle(DVDDiskPlayer player);

DLLCALL BOOL  WINAPI  DDP_CheckMenuAvail(DVDDiskPlayer player, VTSMenuType menu);

DLLCALL DisplayPresentationMode WINAPI  DDP_GetCurrentDisplayMode(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_SetDisplayMode(DVDDiskPlayer player, DisplayPresentationMode mode);

DLLCALL Error WINAPI  DDP_TitlePlay(DVDDiskPlayer player, WORD title);

DLLCALL Error WINAPI  DDP_PTTPlay(DVDDiskPlayer player, WORD title, WORD part);

DLLCALL Error WINAPI  DDP_TimePlay(DVDDiskPlayer player, WORD title, DVDTime time);

DLLCALL Error WINAPI  DDP_TimePlayForced(DVDDiskPlayer player, WORD title, DVDTime time);

DLLCALL Error WINAPI  DDP_ExtendedPlay(DVDDiskPlayer player, DWORD flags, WORD title, WORD ptt, DVDTime time);

DLLCALL Error WINAPI  DDP_Stop(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_GoUp(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_TimeSearch(DVDDiskPlayer player, DVDTime time);

DLLCALL Error WINAPI  DDP_TimeSearchForced(DVDDiskPlayer player, DVDTime time);

DLLCALL Error WINAPI  DDP_PTTSearch(DVDDiskPlayer player, WORD part);

DLLCALL Error WINAPI  DDP_PrevPGSearch(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_TopPGSearch(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_NextPGSearch(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_ForwardScan(DVDDiskPlayer player, WORD speed);

DLLCALL Error WINAPI  DDP_BackwardScan(DVDDiskPlayer player, WORD speed);

DLLCALL Error WINAPI  DDP_TrickPlay(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_StopScan(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_ReversePlayback(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_MenuCall(DVDDiskPlayer player, VTSMenuType menu);

DLLCALL Error WINAPI  DDP_Resume(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_UpperButtonSelect(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_LowerButtonSelect(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_LeftButtonSelect(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_RightButtonSelect(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_ButtonSelectAt(DVDDiskPlayer player, WORD x, WORD y);

DLLCALL Error WINAPI  DDP_ButtonActivate(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_ButtonSelectAndActivate(DVDDiskPlayer player, WORD num);

DLLCALL Error WINAPI  DDP_ButtonSelectAndActivateAt(DVDDiskPlayer player, WORD x, WORD y);

DLLCALL BOOL  WINAPI  DDP_IsButtonAt(DVDDiskPlayer player, WORD x, WORD y);

DLLCALL Error WINAPI  DDP_StillOff(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_PauseOn(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_PauseOff(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_AdvanceFrame(DVDDiskPlayer player);

DLLCALL Error WINAPI  DDP_AdvanceFrameBy(DVDDiskPlayer player, int by);

DLLCALL Error WINAPI  DDP_SetPlaybackSpeed(DVDDiskPlayer player, WORD speed);

DLLCALL Error WINAPI  DDP_MenuLanguageSelect(DVDDiskPlayer player, WORD language);

DLLCALL Error WINAPI  DDP_AudioStreamChange(DVDDiskPlayer player, WORD streamID);

DLLCALL Error WINAPI  DDP_SubPictureStreamChange(DVDDiskPlayer player, WORD streamID, BOOL display);

DLLCALL Error WINAPI  DDP_AngleChange(DVDDiskPlayer player, WORD angle);

DLLCALL Error WINAPI  DDP_ParentalLevelSelect(DVDDiskPlayer player, WORD parentalLevel);

DLLCALL Error WINAPI  DDP_ParentalCountrySelect(DVDDiskPlayer player, WORD country);

DLLCALL Error WINAPI  DDP_InitialLanguageSelect(DVDDiskPlayer player,
														      WORD audioLanguage,
														      WORD audioExtension,
														      WORD subPictureLanguage,
														      WORD subPictureExtension);

DLLCALL Error WINAPI  DDP_InquireCurrentBitRate(DVDDiskPlayer player, DWORD & bitsPerSecond);

DLLCALL Error WINAPI  DDP_GetCurrentButtonState(DVDDiskPlayer player, WORD & minButton, WORD & numButtons, WORD & currentButton);

DLLCALL Error WINAPI  DDP_Freeze(DVDDiskPlayer player, BYTE * buffer, DWORD & length);

DLLCALL Error WINAPI  DDP_Defrost(DVDDiskPlayer player, BYTE * buffer, DWORD & length);

DLLCALL Error WINAPI  DDP_DefrostExt(DVDDiskPlayer player, BYTE * buffer, DWORD & length, DWORD flags);

DLLCALL Error WINAPI  DDP_InstallEventHandler(DVDDiskPlayer player, DWORD event, DNEEventHandler handler, void * userData);

DLLCALL Error WINAPI  DDP_RemoveEventHandler(DVDDiskPlayer player, DWORD event);

DLLCALL Error WINAPI  DDP_SetBreakpoint(DVDDiskPlayer player, WORD title, DVDTime time, DWORD flags, DWORD & id);

DLLCALL Error WINAPI  DDP_SetBreakpointExt(DVDDiskPlayer player, WORD title, WORD ptt, DVDTime time, DWORD flags, DWORD & id);

DLLCALL Error WINAPI  DDP_ClearBreakpoint(DVDDiskPlayer player, DWORD id);

DLLCALL Error WINAPI  DDP_DetachDrive(DVDDiskPlayer player, DWORD flags, DVDTime time, DWORD bufferSize);

}


#endif

