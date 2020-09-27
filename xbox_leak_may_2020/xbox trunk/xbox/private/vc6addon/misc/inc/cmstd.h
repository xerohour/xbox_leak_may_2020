/*************************************************************************
	msostd.h

	Owner: rickp
	Copyright (c) 1994 Microsoft Corporation

	Standard common definitions shared by all office stuff
*************************************************************************/

#if !defined(CMSTD_H)
#define CMSTD_H

/*************************************************************************
	make sure we have our processor type set up right - note that we
	now have three - count 'em, three - different symbols defined for
	each processor we support (e.g., X86, _X86_, and _M_IX386)
*************************************************************************/

#if !defined(PPCMAC) && !defined(PPCLIB) && !defined(X86) && !defined(M68K)

	#if defined(_M_IX86)
		#define X86 1
	#elif defined(_M_MPPC)
		#define PPCMAC 1
	#endif

#endif

/*************************************************************************
	Pull in standard Windows and C definitions.
*************************************************************************/

/*	make sure the compiler generates intrinsic calls of all crt functions,
	or else we'll pull in a ton of crt stuff we probably don't want. */
#ifndef RC_INVOKED
	#include <string.h>
	#pragma intrinsic(strcpy, strcat, strlen, memcpy, memset, memcmp)
#endif

#define OEMRESOURCE

#include <windows.h>
#if !defined(RC_INVOKED)
	#include <ole2.h>
#endif

#define MsoMemcpy memcpy
#define MsoMemset memset
#define MsoMemmove memmove

/*************************************************************************
	Pre-processor magic to simplify Mac vs. Windows expressions.
*************************************************************************/

#if MAC
	#define Mac(foo) foo
	#define MacElse(mac, win) mac
	#define NotMac(foo)
	#define Win(foo)
	#define WinMac(win,mac) mac
#else
	#define Mac(foo)
	#define MacElse(mac, win) win
	#define NotMac(foo) foo
	#define Win(foo) foo
	#define WinMac(win,mac) win
#endif


/*************************************************************************
	Calling conventions 

	If you futz with these, check the cloned copies in inc\msosdm.h
	
*************************************************************************/

// define these as nothing since we are not a dll anymore
#define MSOPUB
#define MSOPUBDATA

/* used for interface that rely on using the OS (stdcall) convention */
#define MSOSTDAPICALLTYPE __stdcall

/* used for interfaces that don't depend on using the OS (stdcall) convention */
#define MSOAPICALLTYPE __stdcall

#if defined(__cplusplus)
	#define MSOEXTERN_C extern "C"
#else
	#define MSOEXTERN_C 
#endif

#define MSOAPI_(t) MSOEXTERN_C MSOPUB t MSOAPICALLTYPE 
#define MSOSTDAPI_(t) MSOEXTERN_C MSOPUB t MSOSTDAPICALLTYPE 
#define MSOAPIX_(t) MSOEXTERN_C MSOPUBX t MSOAPICALLTYPE 
#define MSOSTDAPIX_(t) MSOEXTERN_C MSOPUBX t MSOSTDAPICALLTYPE 

#if MAC
	#define MSOPUBXX	
	#define MSOAPIMX_(t) MSOAPI_(t)
	#define MSOAPIXX_(t) MSOAPIX_(t) 
#else
	#define MSOPUBXX MSOPUB
	#define MSOAPIMX_(t) MSOAPIX_(t)
	#define MSOAPIXX_(t) MSOAPI_(t)
#endif

#define MSOMETHOD(m)      STDMETHOD(m)
#define MSOMETHOD_(t,m)   STDMETHOD_(t,m)
#define MSOMETHODIMP      STDMETHODIMP
#define MSOMETHODIMP_(t)  STDMETHODIMP_(t)

/* Interfaces derived from IUnknown behave in funny ways on the Mac */
#if MAC && MSO_NATIVE_MACOLE
#define BEGIN_MSOINTERFACE BEGIN_INTERFACE
#else
#define BEGIN_MSOINTERFACE
#endif


// Deal with "split" DLLs for the Mac PPC Build
#if MAC &&      MACDLLSPLIT
	#define MSOMACPUB MSOPUB
	#define MSOMACPUBDATA  MSOPUBDATA
	#define MSOMACAPI_(t)  MSOAPI_(t)
	#define MSOMACAPIX_(t) MSOAPIX_(t)
#else
	#define MSOMACPUB 
	#define MSOMACPUBDATA
	#define MSOMACAPI_(t) t
	#define MSOMACAPIX_(t) t 
#endif
	
#if X86 && !DEBUG
	#define MSOPRIVCALLTYPE __fastcall
#else
	#define MSOPRIVCALLTYPE __cdecl
#endif

#if MAC
#define MSOCONSTFIXUP(t) t 
#else
#define MSOCONSTFIXUP(t) const t 
#endif



/*************************************************************************
	Common #define section
*************************************************************************/

/* All Microsoft Office specific Apple events should use MSO_EVENTCLASS 
	as the EventClass of their Apple Events */
	
// TODO: This value needs to be okay'd by Word, Excel, PowerPoint, Access and
//              possibly Apple
	
#if MAC
#define MSO_EVENTCLASS '_mso'
#define MSO_WPARAM 'wprm'
#define MSO_LPARAM 'lprm'
#define MSO_NSTI 'nsti'
#endif

// NA means not applicable. Use NA to help document parameters to functions.
#undef  NA
#define NA 0L

/* End of common #define section */


#endif // CMSTD_H
