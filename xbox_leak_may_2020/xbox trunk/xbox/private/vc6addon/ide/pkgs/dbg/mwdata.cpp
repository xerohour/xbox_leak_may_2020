/////////////////////////////////////////////////////////////////////////
//
//	MWDATA.C - Memory Window Handler
//
//	Copyright (C)1990, Microsoft Corporation
//
//	Purpose: To display and update the memory windows.
//
//  Revision History:
//       Written by: Mark A. Brodsky
//
// 	Notes: Private global data for the memory window
//
//////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// Data for the memory window

#ifdef _WIN32
const MWD rgmwd[] =
#else
MWD _based(_segname("MW_CODE")) rgmwd[] =
#endif
{
	{ sizeof( char ),    1, "%c"	},
#ifdef _WIN32
    { sizeof( wchar_t ), 2, "%wc"	},
#endif	// _WIN32
	{ sizeof( BYTE ),    2, "%2x"	},
	{ sizeof( USHORT ),  6, "%d"	},
	{ sizeof( USHORT ),  4, "%x"	},
	{ sizeof( USHORT ),  5, "%u"	},
	{ sizeof( long ),   11, "%ld"	},
	{ sizeof( long ),    8, "%lx"	},
	{ sizeof( long ),   10, "%lu"	},
	{ sizeof( float ),  13, "%e"	},
	{ sizeof( double ), 22, "%le"	},
#ifndef _WIN32
	{ 10,               28, "%Le"	}
#else	// !_WIN32
#if _INTEGRAL_MAX_BITS >= 64
	{ sizeof( __int64 ),20, "%I64d" },
	{ sizeof( __int64 ),16, "%I64x" },
	{ sizeof( __int64 ),20, "%I64u" }
#endif	// _INTEGRAL_MAX_BITS >= 64
#endif	// !_WIN32
};																  	

const char rgchType[] = {
    'A',
#ifdef _WIN32
    'C',
#endif	// _WIN32
    'B', 'I', 'W', 'U',
    'Z', 'D', 'X', 'S', 'L',
#ifndef _WIN32
    'T'
#else	// !_WIN32
#if _INTEGRAL_MAX_BITS >= 64
	 'M', 'N', 'P'
#endif	// _INTEGRAL_MAX_BITS >= 64
#endif	// !_WIN32
};

// mpbHex is for converting bytes to ascii hex.  The lowercase and
// zero terminator are for quick scans for editing addresses.
const char mpbHex[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
                  'A', 'B', 'C', 'D', 'E', 'F', 'a', 'b', 'c', 'd',
				  'e', 'f', '\0' };

BOOL	fDumping = (BOOL)FALSE;
