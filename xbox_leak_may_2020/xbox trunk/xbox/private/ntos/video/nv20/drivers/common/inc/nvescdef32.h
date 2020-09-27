/*
 * nvEscDef32.h
 *
 * Provides definitions for 32 bit views for nVidia ExtEscape strucures into
 * the 64 bit display driver.  Eventually we'll have a single source file.
 *
 * Copyright (c) 2000, Nvidia Corporation.  All rights reserved.
 * Portions Copyright (c) 1996, 1997, Silicon Graphics Inc.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */

#ifndef NV_ESC_DEF32_INCLUDED
#define NV_ESC_DEF32_INCLUDED

typedef struct _NV_PRIMARY_INFO32
{
    ULONG          ulNVSignature;    // IN
    ULONG          ulNumDevices;     // IN
    ULONG          ulHeadNum;        // IN
    ULONG          ulReturnCode;     // IN
    ULONG          ulWidth;          // OUT from display driver
    ULONG          ulHeight;         // OUT from display driver
    ULONG          ulDepth;          // OUT from display driver
    ULONG          ulPitch;          // OUT from display driver
    ULONG          ulPrimaryOffset;  // OUT from display driver
    ULONG          ulNumDACs;        // OUT from display driver (NT only)
    ULONG          ulNumActiveDACs;  // OUT from display driver
    ULONG          ulActiveDACs;     // OUT from display driver (NT only)
    ULONG * __ptr32 ulPanningPtr;    // OUT from display driver (9x only)
    ULONG * __ptr32 ulFullscreenPtr; // OUT from display driver (9x only)
} NV_PRIMARY_INFO32;

typedef struct _NV_OPENGL_COMMAND32
{
    ULONG          NVSignature;
    ULONG          command;           // Handle to resource manager client used in NvFree
    ULONG          numDevices;        // Number of devices in the system
    VOID * __ptr32 hWnd;              // Window handle
    VOID * __ptr32 hDC;               // GDI device context handle
    ULONG          hClient;           // Handle to resource manager client
    ULONG          processHandle;     // Process handle from client associated with globalData
    VOID * __ptr32 globalData;        // Pointer to global shared data for destroy
    VOID * __ptr32 other;             // Pointer to whatever
} NV_OPENGL_COMMAND32;

typedef struct _NV_OPENGL_CLIP_LIST_DATA32
{
    int            clipChangedCount;
    int            rgnDataSize;
    void * __ptr32 rgnData;
    void * __ptr32 rect;
} NV_OPENGL_CLIP_LIST_DATA32;

typedef struct _NV_WNDOBJ_SETUP32
{
    ULONG          NVSignature;
    VOID * __ptr32 hWnd;                // Handle to window
    VOID * __ptr32 hDC;                 // handle to GDI device context
    ULONG          numDevices;          // Number of devices in the system
    PIXELFORMATDESCRIPTOR pfd;
} NV_WNDOBJ_SETUP32;

typedef struct {
    ULONG          type;
    CHAR * __ptr32 keyName;
    VOID * __ptr32 keyVal;
    ULONG          keyValSize;
    ULONG          keyValMaxSize;
    ULONG          retStatus;
} *__PNVRegKeyInfo32, __NVRegKeyInfo32;

#endif // NV_ESC_DEF32_INCLUDED
