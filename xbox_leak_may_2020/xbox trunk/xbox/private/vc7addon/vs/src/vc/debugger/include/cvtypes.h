//  CVTYPES.H
//
//  This file contains a common set of base type declarations
//  between multiple CodeView projects.  If you touch this in one
//  project be sure to copy it to all other projects as well.

#ifndef _VC_VER_INC
#include "..\include\vcver.h"
#endif

#ifndef CV_PROJECT_BASE_TYPES
#define CV_PROJECT_BASE_TYPES

//#if !defined(DOLPHIN)
//#include "dbapiver.h"
//#endif

#pragma once

#include "types.h"
#include "util\debug.h"

typedef HANDLE HDEP;				// handle on host platform
typedef DWORDLONG HIND;				// type that works on all platforms

typedef HDEP FAR *  LPHDEP;
typedef HIND FAR *  LPHIND;

// HMEM should be avoided (HDEP should be used instead), but for now we'll
// define it for backwards compatibility.

typedef HDEP    HMEM;
typedef HMEM FAR *  LPHMEM;

typedef HIND HPID;
typedef HIND HTID;

DECLARE_HANDLE(HEMI);

typedef HPID FAR *LPHPID;
typedef HTID FAR *LPHTID;

// These values are used in the SegType field of the Expression Evaluator's
// TI structure, and as the third parameter to the Symbol Handler's
// SHGetNearestHsym function.

#define EECODE      0x01
#define EEDATA      0x02
#define EECLABEL    0x04
#define EEANYSEG    0xFFFF


typedef ULONG   SEGMENT;    // 32-bit compiler doesn't like "_segment"
typedef USHORT  SEG16;
typedef ULONG   SEG32;
typedef SEGMENT FAR * LPSEGMENT;
typedef ULONG   UOFF32;
typedef UOFF32 FAR * LPUOFF32;
typedef USHORT  UOFF16;
typedef UOFF16 FAR * LPUOFF16;
typedef LONG    OFF32;
typedef OFF32 FAR * LPOFF32;
typedef SHORT   OFF16;
typedef OFF16 FAR * LPOFF16;
typedef LONG64 OFF64;
typedef ULONG64 UOFF64;
typedef UOFF64 *LPUOFF64;

typedef UOFF64 UOFFSET;
typedef OFF64 OFFSET;
typedef UOFFSET FAR * LPUOFFSET;
typedef OFFSET FAR * LPOFFSET;

//  address definitions
//  the address packet is always a 16:32 address.

typedef struct {
	union {
		UOFF64		off64;
		UOFF32		off32;
	};
    SEGMENT     seg;
} address_t;

typedef struct {
    BYTE    fFlat   :1;         // true if address is flat
    BYTE    fOff32  :1;         // true if offset is 32 bits
    BYTE    fIsLI   :1;         // true if segment is linker index
    BYTE    fReal   :1;         // x86: is segment a real mode address
    BYTE    fOff64  :1;         // true if segment is 64 bits
    BYTE    unused  :3;         // unused
// OMNI
    BYTE    omnioff;            //used for omnibyte<->x86 addr conversion
// end OMNI
} memmode_t;

typedef struct ADDR {
    address_t   addr;
    union {
        DWORDLONG Align;
        HEMI        emi;
    };
    memmode_t   mode;
} ADDR, *PADDR, *LPADDR;                     // An address specifier

typedef const ADDR* LPCADDR;

#define addrAddr(a)         ((a).addr)
#define emiAddr(a)          ((a).emi)
#define modeAddr(a)         ((a).mode)

#define MODE_IS_FLAT(m)     ((m).fFlat)
#define MODE_IS_OFF32(m)    ((m).fOff32)
#define MODE_IS_LI(m)       ((m).fIsLI)
#define MODE_IS_REAL(m)     ((m).fReal)
#define MODE_IS_OFF64(m)    ((m).fOff64)

#define ADDR_IS_FLAT(a)     (MODE_IS_FLAT(modeAddr(a)))
#define ADDR_IS_OFF32(a)    (MODE_IS_OFF32(modeAddr(a)))
#define ADDR_IS_LI(a)       (MODE_IS_LI(modeAddr(a)))
#define ADDR_IS_REAL(a)     (MODE_IS_REAL(modeAddr(a)))
#define ADDR_IS_OFF64(a)    (MODE_IS_OFF64(modeAddr(a)))

#define ADDRSEG16(a)        {ADDR_IS_FLAT(a) = FALSE; ADDR_IS_OFF32(a) = FALSE; ADDR_IS_OFF64(a) = FALSE;}
#define ADDRSEG32(a)        {ADDR_IS_FLAT(a) = FALSE; ADDR_IS_OFF32(a) = TRUE; ADDR_IS_OFF64(a) = FALSE;}

#define ADDRLIN(a)			{ADDR_IS_FLAT(a) = TRUE;  ADDR_IS_REAL(a) = FALSE;}
#define ADDRLIN32(a)        {ADDRLIN(a);  ADDR_IS_OFF32(a) = TRUE;  ADDR_IS_OFF64(a) = FALSE;}
#define ADDRLIN64(a)        {ADDRLIN(a);  ADDR_IS_OFF32(a) = FALSE; ADDR_IS_OFF64(a) = TRUE;}

#define GetAddrSeg(a)       ((a).addr.seg)
#define SetAddrSeg(pa,s)    ((pa)->addr.seg = s)

#define GetAddrOff32(a)     ((a).addr.off32)
//
// Masked to not have RTC fires.
//
#define SetAddrOff32(pa,o)  ((pa)->addr.off32 = (UOFF32) (o & 0xFFFFFFFF))

#define GetAddrOff64(a)     ((a).addr.off64)
#define SetAddrOff64(pa,o)  ((pa)->addr.off64 = (UOFF64)(o))


//#define GetAddrOff(a)		(GetAddrOff64(a))
//#define SetAddrOff(pa,o)    { assert(                                             \
//		                       ADDR_IS_OFF64(*pa) + ADDR_IS_OFF32(*pa) == 1 || \
//							   GetAddrOff(*pa) == 0x0);                        \
//							  (SetAddrOff64(pa,o));                            \
//							  if (ADDR_IS_OFF32(*pa)) {\
//								  UOFFSET value = GetAddrOff64(*pa); \
//								  /* sign extension is OK */ \
//								  if (((value & 0xFFFFFFFF00000000) !=0) && \
//								      ((value & 0xFFFFFFFF00000000) != 0xFFFFFFFF00000000) \
//										  ) { \
//									  assert (!"32bit addr has non empty upper dword- fatal");\
//								  }\
//							  }\
//							}\



__inline
UOFFSET
GetAddrOff(
    ADDR addr
	)
{

	UOFFSET ret =  GetAddrOff64(addr);

#ifdef _DEBUG
	UOFFSET value =ret;

	// this shoudn't happen since upperDWORD is cleared out in SetAddrOff
	if (ADDR_IS_OFF32(addr) && (value & 0xFFFFFFFF00000000) !=0) { 
		Assert (!"32bit addr has non empty upper dword- fatal");
	}
#endif

	return ret;
}

__inline
void
SetAddrOff(
	LPADDR lpaddr,
	UOFFSET offset
	)
{
	Assert (ADDR_IS_OFF64(*lpaddr) + ADDR_IS_OFF32(*lpaddr) == 1 || 
			GetAddrOff(*lpaddr) == 0x0 || 
			!"Address cannot be 32 and 64 bit");


	//
	// clear upper DWROD for 32 bit values
	//
	if (ADDR_IS_OFF32(*lpaddr))  {
		offset &=0xFFFFFFFF;
	}
	SetAddrOff64(lpaddr,offset);
}



__inline
void
AddrInitEx(
    LPADDR paddr,
    HEMI emiSet,
    UOFFSET offset,
	BOOL f64,
    BOOL fLi
    )
{
	paddr->addr.seg = 0;
	paddr->emi = emiSet;
	paddr->mode.fFlat = TRUE;
	paddr->mode.fIsLI = (BYTE)fLi;
	paddr->mode.fOff64 = (BYTE)f64;
	paddr->mode.fOff32 = (BYTE)!f64;
	paddr->mode.fReal = FALSE;
	paddr->mode.omnioff = 0;
	paddr->mode.unused = 0;
	SetAddrOff(paddr,offset);

#if (defined(HOST_i386) && !defined(ENABLE_IA64_SUPPORT)) || defined (HOST_ALPHA) 
	assert (paddr->mode.fOff32 && !paddr->mode.fOff64 ||
			!"Incorrect address format");
#endif

#if defined(HOST_IA64)  || defined (HOST_AXP64) 
	//
	// (It is possible for 32 bit address to be present in 64 bit
	// eg UnFixedup Address? ADDR_IS_LI(addr) == 0) vvadimp - yes, addresses could be init'ed to 32-bit as LIs
	//
	assert (paddr->mode.fOff64 && !paddr->mode.fOff32  || paddr->mode.fIsLI ||
			!"Possibly incorrect address format");
#endif

}
__inline
void
AddrInit(
    LPADDR paddr,
    HEMI emiSet,
    SEGMENT segSet,
    UOFF64 offSet,
    BOOL fFlat,
    BOOL fOff32,
    BOOL fLi,
    BOOL fRealSet
    )
{
    assert(fRealSet == 0);
    AddrInitEx(paddr, emiSet, offSet, !fOff32, fLi);

    SetAddrSeg(paddr, segSet);
	paddr->mode.fFlat = (BYTE)fFlat;
	paddr->mode.fReal = (BYTE)fRealSet;
}

typedef struct FRAME {
    SEG16       SS;
    address_t   BP;
    SEG16       DS;
    memmode_t   mode;
    address_t   SLP;        // Static link pointer
    address_t   BSP;        // IA64 Back Store Pointer
} FRAME;
typedef FRAME *PFRAME;

#define addrFrameSS(a)     ((a).SS)
#define addrFrameBP(a)     ((a).BP)
#define addrFrameSLP(a)    ((a).SLP)
#define addrFrameBSP(a)	   ((a).BSP)

//#define GetFrameBPOff(a)   (MODE_IS_OFF64(FRAMEMODE(a))? addrFrameBP(a).off64: addrFrameBP(a). off32)
#define GetFrameBPOff(a)   (addrFrameBP(a).off64)
#define GetFrameBPSeg(a)   ((a).BP.seg)

//#define SetFrameBPOff(a,o) (MODE_IS_OFF64(FRAMEMODE(a))? addrFrameBP(a).off64 = (UOFF64) o : addrFrameBP(a). off32 = (UOFF32) o)
#define SetFrameBPOff(a,o) (addrFrameBP(a).off64 = (UOFF64)(o))
#define SetFrameBPSeg(a,s) ((a).BP.seg = s)

//#define GetFrameSLPOff(a) (MODE_IS_OFF64(FRAMEMODE(a))? addrFrameSLP(a).off64: addrFrameSLP(a).off32)
#define GetFrameSLPOff(a) (addrFrameSLP(a).off64)
#define GetFrameSLPSeg(a)  ((a).SLP.seg)

//#define SetFrameSLPOff(a,o) (MODE_IS_OFF64(FRAMEMODE(a))? addrFrameSLP(a).off64 = (UOFF64) o : addrFrameSLP(a). off32 = (UOFF32) o)
#define SetFrameSLPOff(a,o) (addrFrameSLP(a).off64 = (UOFF64)(o))
#define SetFrameSLPSeg(a,s) ((a).SLP.seg = s)

//#define GetFrameBSPOff(a) (MODE_IS_OFF64(FRAMEMODE(a))? addrFrameBSP(a).off64: addrFrameBSP(a).off32)
#define GetFrameBSPOff(a) (addrFrameBSP(a).off64)
#define GetFrameBSPSeg(a)  ((a).BSP.seg)

#define FRAMEMODE(a)       ((a).mode)
#define FrameLI(a)         MODE_IS_LI(FRAMEMODE(a))
#define FrameFlat(a)       MODE_IS_FLAT(FRAMEMODE(a))
#define FrameOff32(a)      MODE_IS_OFF32(FRAMEMODE(a))
#define FrameOff64(a)      MODE_IS_OFF64(FRAMEMODE(a))
#define FrameReal(a)       MODE_IS_REAL((FRAMEMODE(a))

//  A few public types related to the linked list manager

typedef HDEP    HLLI;       // A handle to a linked list
typedef HDEP    HLLE;       // A handle to a linked list entry

typedef void (FAR PASCAL * LPFNKILLNODE)(LPVOID);
typedef int  (FAR PASCAL * LPFNFCMPNODE)(LPVOID, LPVOID, LONG );

typedef DWORD          LLF;    // Linked List Flags
#define llfNull         (LLF)0x0
#define llfAscending    (LLF)0x1
#define llfDescending   (LLF)0x2
#define fCmpLT              (-1)
#define fCmpEQ              (0)
#define fCmpGT              (1)

//  EXPCALL indicates that a function should use whatever calling
//      convention is preferable for exported functions.

#define EXPCALL         __stdcall

typedef struct _MEMINFO {
    ADDR    addr;
    ADDR    addrAllocBase;
    UOFFSET uRegionSize;
    DWORD   dwProtect;
	DWORD   dwAllocationProtect;
    DWORD   dwState;
    DWORD   dwType;
} MEMINFO;
typedef MEMINFO FAR * LPMEMINFO;

//  Return values for mtrcProcessorType
typedef enum _MPT {
    mptUnknown = 0,
    mptix86  = 1,   // Intel X86
    mptm68k  = 2,   // Mac 68K
    mptdaxp  = 3,   // Alpha AXP
    mptmips  = 4,   // MIPS
    mptmppc  = 5,   // Mac PPC
    mptntppc = 6,   // NT PPC
	mptpegsh = 8,	// PEG SH
	mptpegmips = 9,	// PEG MIPS
	mptpegppc = 10,	// PEG PPC
	mptpegarm = 11,	// PEG ARM
	mptwin32nkpc = 12,	// PEG nkpc
	mptpegsh4 = 13,		// PEG SH4
// leave room for more WinCE types here
	mptia64 = 20,		// Intel Merced
	mptaxp64 = 21,		// Alpha 64-bit
} MPT;

#define CLEAR_UPPER_DWORD(x)                                   \
         Assert ((sizeof (x) >= 4  && sizeof(x)% 4 == 0) ||  \
				  !"Incorrect Usage");                       \
		 x = x & 0xFFFFFFFF


#include <dbgver.h>     // For AVS definition and support functions

#endif  // CV_PROJECT_BASE_TYPES
