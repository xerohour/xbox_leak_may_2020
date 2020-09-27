/*=======================================================================

    Copyright (C), 1998 Nvidia Coporation

    File:       hwlogdev.h

    Purpose:    This file holds the HWLOGDEV structure and related data.
=========================================================================*/

typedef unsigned char   UCHAR;
typedef unsigned short  USHORT;
typedef unsigned long   ULONG;

// This structure holds dynamic data that is unique for each
// given instance of a PDEVICE (a logical device actually).
typedef struct  _HWLOGDEVCX
{
    // Set to non-0 if we are initting. Cleared on first modeset
    ULONG   dwInitting;

    // The HWBOARDCX attached to this HWLOGDEV
    ULONG   pHWBoardCX;

    // Keep the HWCRTCCX's around
    ULONG   pHWCrtcCX[MAX_ATTACHED_CRTCS];

    // These variables color depth dependent values for objects and
    // objet methods on this logical device.
    ULONG   dwRectAndTextColorFormat;
    ULONG   dwIndexedImageColorFormat;
    ULONG   dwImageFromCpuColorFormat;
    ULONG   dwSolidLinColorFormat;
    ULONG   dwColorAlphaMask;
    ULONG   dwPatternColorFormat;
    ULONG   dwColorPatternAndHeaderSize;
    ULONG   dwColorPatternMethodOffset;
    ULONG   dwPatternAlpha0;
    ULONG   dwPatternAlpha1;

    // Intel Diagnostic's stuff
    USHORT  wDiagInterfaceOpen;
    USHORT  wLastDiagError;

    // This holds the current offset of the logical desktop surface
    ULONG   dwLogicalSurfaceOffset;

    // DIRECTX STUFF
    // If DirectX is active then this variable is non-0; else it is 0.
    ULONG   dwDirectXActive;

    // This is to hold the callbacks into the internals of Direct Draw
    MYDDHALDDRAWFNS sDDHalDDrawFns;

    // Anybody can use this but it was created for the DirectX driver.
    // DirectX should really call every time there is a mode change to
    // get the available mode list, but some of the old DX apps need
    // the mode list before the 32bit driver has initted, so we need
    // something to give them. Note that this sounds CRTC specific
    // since it has to do with modes. It kinda is, but since it was
    // created for DirectX, it is really PDEVICE specific.
    ULONG   pMasterModeList;
    ULONG   lpMasterModeList;       // 16:16 ptr

    // pFlatDirectXSharedData is a flat ptr to the shared data.
    // pDirectXSharedData is a CXSel relative ptr to the shared data.
    ULONG   pDirectXSharedData;
    ULONG   lpDirectXSharedData;    // 16:16 ptr
}   HWLOGDEVCX;

