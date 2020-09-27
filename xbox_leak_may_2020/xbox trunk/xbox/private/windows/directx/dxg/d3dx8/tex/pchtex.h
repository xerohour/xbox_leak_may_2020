//
// D3DX texture precompiled header
//

#ifndef __PCHTEX_H__
#define __PCHTEX_H__

#define D3DFMT_R8G8B8 (D3DFORMAT)0x999

#include <d3dx8seg.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include <xtl.h>
#include <xgraphics.h>

#include "png.h"
#include "jpeglib.h"
#include "s3tc.h"
#include "s3tchelp.h"

#include "d3dx8.h"
#include "d3dx8dbg.h"

typedef signed char  INT8;
typedef short INT16;

typedef unsigned char  UINT8;
typedef unsigned short UINT16;

#include "CD3DXFile.h"
#include "CD3DXImage.h"
#include "CD3DXCodec.h"
#include "CD3DXBlt.h"

#define RELEASE(x) \
    do { if(x) { x->Release(); x = NULL; } } while(0)



//----------------------------------------------------------------------------
//
// F2IBegin, F2I, F2IEnd
//
// Fast FLOAT->INT conversion.  F2IBegin sets and F2IEnd restores the FPU
// rounding mode.  F2I converts a float to an int.  You need to be careful of
// what other floating point code resides between F2IBegin and F2IEnd:  If
// something messes with the rounding mode, you could get unpredicted results.
//
//----------------------------------------------------------------------------

#ifdef _X86_

static UINT32 g_ulFPU;

static VOID
F2IBegin() {
    UINT32 ulFPU, ul;

    __asm {
        fnstcw WORD PTR [ulFPU]        // Get FPU control word
        mov    eax, DWORD PTR [ulFPU]
        or     eax, 0x0C00             // Rounding mode = CLAMP
        mov    DWORD PTR [ul], eax
        fldcw  WORD PTR [ul]           // Set FPU control word
    }

    // Save old FPU control word in thread-local storage
    g_ulFPU = ulFPU;
}


static inline INT
F2I(FLOAT f)
{
    volatile INT n;

    __asm {
        fld   f   // Load fload
        fistp n   // Store integer (and pop)
    }

    return n;
}

static VOID
F2IEnd() {
    // Get old FPU control word from thread-local storage
    UINT32 ulFPU = g_ulFPU;

    __asm {
        fldcw WORD PTR [ulFPU]    // Set FPU control word
    }
}

#else // !_X86_

static VOID
F2IBegin()
{
    // Do nothing
}

static inline INT
F2I(float f)
{
    return (INT) f;
}

static VOID
F2IEnd()
{
    // Do nothing
}

#endif // !_X86_

#endif //__PCHTEX_H__
