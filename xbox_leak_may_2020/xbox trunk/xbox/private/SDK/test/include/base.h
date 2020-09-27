// -*- Mode:C++ -*-
//
//      Copyright (c) 1996-1998, Microsoft Corp. All rights reserved. 
//
// $Header: /ast/sdk/lib/base.h 4     5/09/96 1:25p Erikruf $
// $NoKeywords: $
//

// Our virtual machine 

#ifndef INC_BASE_H 
#define INC_BASE_H

///////////////////////////////////////////////////////////////////////////////
// Types

// Ideally, we want to use C++'s built-in "bool" type.  Unfortunately
// for us, VC (4.1 anyway) reserves the "bool" keyword, but doesn't
// give it a meaning!  The same goes for "true" and "false."

// This looks nice, but has no default coercion from relational!
//enum Bool {FALSE=0, TRUE=1};

// Ok, ok, we'll do it the old-fashioned way
typedef unsigned int Bool;
#ifndef TRUE
 // client might already have included a .h file defining these
 #define FALSE ((unsigned int)0)
 #define TRUE  ((unsigned int)1)
#endif // TRUE

// Ha!  We can fake out the stinkin' compiler with macro tricks.
#undef bool
#undef true
#undef false
#define bool Bool
#define true TRUE
#define false FALSE

typedef unsigned char Byte;
typedef __int8 Int8;
typedef __int16 Int16;
typedef __int32 Int32;
typedef __int64 Int64;
typedef unsigned __int8 UInt8;
typedef unsigned __int16 UInt16;
typedef unsigned __int32 UInt32;
typedef unsigned __int64 UInt64;

///////////////////////////////////////////////////////////////////////////////
// Useful constants

#define MAX_ID_LENGTH 512

///////////////////////////////////////////////////////////////////////////////
// Forward declarations

typedef unsigned long MapIndex;
typedef MapIndex AttribKey;
typedef unsigned long TypeIndex;


// Language type
enum FrontEndType {frontC, frontCXX};

#endif // INC_BASE_H

