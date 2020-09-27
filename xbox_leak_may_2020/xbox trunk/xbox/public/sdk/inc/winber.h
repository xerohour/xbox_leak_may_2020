/*++

Copyright (c) 1996-1999  Microsoft Corporation

Module Name:

    winber.h   Basic Encoding Rules (BER) API header file

Abstract:

   This module is the header file for the 32 bit BER library on
   Windows NT and Windows 95.

Updates :

Environments :

    Win32 user mode

--*/

//
// Only pull in this header file once.
//

#ifndef _WINBER_DEFINED_
#define _WINBER_DEFINED_

#if _MSC_VER > 1000
#pragma once
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(_WINBER_)
#define WINBERAPI DECLSPEC_IMPORT
#else
//#define WINBERAPI __declspec(dllexport)
#define WINBERAPI
#endif

#ifndef BERAPI
#define BERAPI __cdecl
#endif

#define LBER_ERROR   0xffffffffL
#define LBER_DEFAULT 0xffffffffL

//
// This constructs a new BerElement structure containing a copy of the
// data in the supplied berval structure.
//

WINBERAPI BerElement * BERAPI ber_init( BERVAL *pBerVal );

//
// This frees a BerElement which is returned from ber_alloc_t()
// or ber_init(). The second argument - fbuf should always be set
// to 1.
//
//

WINBERAPI VOID BERAPI ber_free( BerElement *pBerElement, INT fbuf );

//
// Frees a BERVAL structure. Applications should not call
// this API to free BERVAL structures which they themselves
// have allocated
//

WINBERAPI VOID BERAPI ber_bvfree( BERVAL *pBerVal );


//
// Frees an array of BERVAL structures.
//

WINBERAPI VOID BERAPI ber_bvecfree( PBERVAL *pBerVal );

//
// Returns a copy of a the supplied berval structure
//

WINBERAPI BERVAL * BERAPI ber_bvdup( BERVAL *pBerVal );


//
// Constructs and returns a BerElement structure. The options field
// contains a bitwise-or of options which are to be used when generating
// the encoding of the BerElement
//
// The LBER_USE_DER options should always be specified.
//

WINBERAPI BerElement * BERAPI ber_alloc_t( INT options );


//
// This skips over the current tag and returns the tag of the next
// element in the supplied BerElement. The lenght of this element is
// stored in the pLen argument.
//
// LBER_DEFAULT is returned if there is no further data to be read
// else the tag of the next element is returned.
//
// The difference between ber_skip_tag() and ber_peek_tag() is that the
// state pointer is advanced past the first tag+lenght and is pointed to
// the value part of the next element
//

WINBERAPI ULONG BERAPI ber_skip_tag( BerElement *pBerElement, ULONG *pLen );

//
// This returns the tag of the next element to be parsed in the
// supplied BerElement. The length of this element is stored in the
// pLen argument.
//
// LBER_DEFAULT is returned if there is no further data to be read
// else the tag of the next element is returned.
//

WINBERAPI ULONG BERAPI ber_peek_tag( BerElement *pBerElement, ULONG *pLen);

//
// This returns the tag and length of the first element in a SET, SET OF
// or SEQUENCE OF data value.
//
// LBER_DEFAULT is returned if the constructed value is empty else, the tag
// is returned. It also returns an opaque cookie which has to be passed to
// subsequent invocations of ber_next_element().
//

WINBERAPI ULONG BERAPI ber_first_element( BerElement *pBerElement, ULONG *pLen, CHAR **ppOpaque );

//
// This positions the state at the start of the next element in the
// constructed type.
//
// LBER_DEFAULT is returned if the constructed value is empty else, the tag
// is returned.
//

WINBERAPI ULONG BERAPI ber_next_element( BerElement *pBerElement, ULONG *pLen, CHAR *opaque );

//
// This allocates a BerVal structure whose contents are taken from the
// supplied BerElement structure.
//
// The return values are 0 on success and -1 on error.
//

WINBERAPI INT BERAPI ber_flatten( BerElement *pBerElement, PBERVAL *pBerVal );

//
// This is similar to sprintf(). One important difference
// is that state information is maintained in the BerElement so that multiple
// calls can be made to ber_printf() to append to the end of the BerElement.
//
// The function returns -1 if there is an error during encoding.
//

WINBERAPI INT BERAPI ber_printf( BerElement *pBerElement, PCHAR fmt, ... );


WINBERAPI ULONG BERAPI ber_scanf( BerElement *pBerElement, PCHAR fmt, ... );


#ifdef __cplusplus
}
#endif


#endif  // _WINBER_DEFINED_
