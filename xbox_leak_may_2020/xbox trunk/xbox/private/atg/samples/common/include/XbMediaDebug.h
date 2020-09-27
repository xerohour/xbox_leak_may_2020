//-----------------------------------------------------------------------------
// File: XbMediaDebug.h
//
// Desc: Miscellaneous functions to aid debugging of media/graphics/etc.
//
// Hist: 11.01.01 - New for November 2001 XDK release
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

// NOTES:
//
// * Most of these functions are designed so that they can be called
//   via the the debugger watch window, which is often the most convenient
//   way to use them.
//
// * However, these routines might cause strange behavior if you're using a
//   D3D pure device, because many of them save/modify/restore device state.

#ifndef XBMEDIADEBUG_H
#define XBMEDIADEBUG_H

#include <xtl.h>
#include <xgraphics.h>


// Uncomment these lines and #include this header in your code (or just add
// these lines directly to your code) to force the linker to include the
// specified symbol(s).
//
// This is useful for making sure functions don't get optimized-out when
// linking.  It ensures you'll be able to call the functions via the debugger
// watch window, even if you never called the routines in your code.
///#pragma comment( linker, "/include:_DebugPixels" )


#ifdef __cplusplus
extern "C" {
#endif


///////////////////////////////////////////////////////////////////////////////
// BUFFER CLEARING
///////////////////////////////////////////////////////////////////////////////
extern D3DXCOLOR g_DebugClearColor; // bkgrnd color when this lib clears screen


///////////////////////////////////////////////////////////////////////////////
// TEXTURE/SURFACE DEBUGGING
///////////////////////////////////////////////////////////////////////////////
extern bool g_bDebugShrinkToFit;  // decrease size (if necess) to fit screen
extern bool g_bDebugExpandToFit;  // increase size (if necess) to fill screen
extern bool g_bDebugAlpha;  // view the alpha channel when rendering

//-----------------------------------------------------------------------------
// Name: DebugPixels
// Desc: Displays a pTexture or pSurface (you can pass-in either one here).
//       For convenience, you can also pass-in special pre-defined values for
//       convenient access to common surfaces:
//          1 == 1st back-color-buffer (note: differs from GetBackBuffer param)
//          2 == 2nd back-color-buffer
//          N == Nth back-color-buffer
//         -1 == depth-stencil-buffer
// Note: This function can be called via the debugger watch window, in which
//       case it will be executed with each step through the code.
//-----------------------------------------------------------------------------
HRESULT __cdecl DebugPixels( void* pTextureOrSurface, UINT iLevel = 0 );



#ifdef __cplusplus
};
#endif

#endif // XBMEDIADEBUG_H

