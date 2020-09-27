#ifndef _BLIB_H
#define _BLIB_H

//////////////////////////////////////////////////////////////////////////////
// switches
//

//////////////////////////////////////////////////////////////////////////////
// macros
//
#ifndef NULL
#define NULL        (0)
#endif

//////////////////////////////////////////////////////////////////////////////
// types
//
typedef void* BPosition;

//////////////////////////////////////////////////////////////////////////////
// helpers
//
inline void fillByte  (void *ptr,unsigned data,unsigned count) { unsigned char  *pch = (unsigned char*)ptr; while (count) { *pch = (unsigned char)data; pch++; count--; } }
inline void fillWord  (void *ptr,unsigned data,unsigned count) { unsigned short *pch = (unsigned short*)ptr; while (count) { *pch = (unsigned short)data; pch++; count--; } }
inline void fillDWord (void *ptr,unsigned data,unsigned count) { unsigned long  *pch = (unsigned long*)ptr; while (count) { *pch = (unsigned long)data; pch++; count--; } }

template<class T> T bmin (T a,T b) { return (a < b) ? a : b; }
template<class T> T bmax (T a,T b) { return (a > b) ? a : b; }

//////////////////////////////////////////////////////////////////////////////
// dependencies
//
#include <assert.h>
#include <windows.h>
#include <stdio.h>

#include "BException.h"
#include "BString.h"
#include "BArray.h"
#include "BList.h"
#include "BConsole.h"

#endif