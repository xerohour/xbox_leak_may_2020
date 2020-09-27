/************************** Module Header *************************************
//                                                                             *
//  Module Name: enable.c                                                      *
//                                                                             *
//  This module contains the functions that enable and disable the             *
//  driver, the pdev, and the surface.                                         *
//                                                                             *
//  Copyright (c) 1992-1996 Microsoft Corporation                              *
//                                                                             *
//******************************************************************************

/*******************************************************************************
 *                                                                             *
 *   Copyright (C) 1997 NVidia Corporation. All Rights Reserved.               *
 *                                                                             *
 ******************************************************************************/

#include "precomp.h"
#include "driver.h"
#include "nvcm.h"
#include "ddminint.h"
#ifdef NVD3D
    #include "dx95type.h"
    #include "nvntd3d.h"
    #include "nvProcMan.h"
    #include "ddmini.h"
#endif
#include "oglDD.h"
#include "ogloverlay.h"
#include "oglutils.h"
#include "oglstereo.h"
#include "oglflip.h"
#include "nvdoublebuf.h"
#include "pixelfmt.h"
#include "linkedlist.h"
#include "savescreenbits.h"

// To get the NV11 twinview definitions
#include "nvMultiMon.h"

extern void FreeMemoryMultiMon(PDEV *ppdev);
extern BOOLEAN   TwinViewDataOK(PPDEV ppdev, NVTWINVIEWDATA *pData);
extern VOID ReadToshibaHotKeyMode(HANDLE      hDriver, TOSHIBA_HOTKEY_MODE *pMode);
extern VOID WriteToshibaHotKeyMode(HANDLE      hDriver, TOSHIBA_HOTKEY_MODE *pMode);

// 2 next functions are utils functions in nvMultiMon.c
//
extern void GetRegValueW(HANDLE hDriver, PWSTR keyName, PVOID keyData, ULONG keyDataSize);
extern void SetRegValueW(HANDLE hDriver, PWSTR keyName, PVOID keyData, ULONG keyDataSize);


extern BOOLEAN bQueryGDIModePruning(ULONG ulTwinView_State, ULONG ulPrimaryHead, ULONG ulPrimaryDeviceMask,
        ULONG ulHead, ULONG ulDeviceMask, ULONG ulACPISystem, ULONG ulGDIModePruning);

#if DBG
extern VOID GetDebugLevelRegValue(PPDEV ppdev);
#endif

#if defined(DX7) && (NVARCH >= 0x04)

#ifndef _WIN64
extern BOOL __stdcall nvDeterminePerformanceStrategy(GLOBALDATA *pDriverData, PDEV* ppdev);
#endif // D3D is not 64 bit code clean , skip for now...

#endif // DX7

// local prototypes
#ifdef NV3
BOOL bNV3CreateStdPatches(PDEV *ppdev);
VOID vNV3DestroyStdPatches(PDEV *ppdev, BOOL CreateStdPatchFailFlag);
#endif // NV3
VOID RenderRectangle(PDEV *ppdev);

VOID vCalibrate_XferSpeeds(PDEV *ppdev);    // Calibration routine to decide xfer chunk size
                                            // when doing a vram ram to sys mem xfer -steph


// save old client on list of PDEVs using this client

//******************************************************************************
//
//  GDIINFO ggdiDefault
//
//  This contains the default GDIINFO fields that are passed back to GDI
//  during DrvEnablePDEV.
//
//  NOTE: This structure defaults to values for an 8bpp palette device.
//        Some fields are overwritten for different colour depths.
//
//******************************************************************************

GDIINFO ggdiDefault = {
    GDI_DRIVER_VERSION,
    DT_RASDISPLAY,          // ulTechnology
    0,                      // ulHorzSize (filled in later)
    0,                      // ulVertSize (filled in later)
    0,                      // ulHorzRes (filled in later)
    0,                      // ulVertRes (filled in later)
    0,                      // cBitsPixel (filled in later)
    0,                      // cPlanes (filled in later)
    20,                     // ulNumColors (palette managed)
    0,                      // flRaster (DDI reserved field)

    0,                      // ulLogPixelsX (filled in later)
    0,                      // ulLogPixelsY (filled in later)

    TC_RA_ABLE,             // flTextCaps -- If we had wanted console windows
                            //   to scroll by repainting the entire window,
                            //   instead of doing a screen-to-screen blt, we
                            //   would have set TC_SCROLLBLT (yes, the flag is
                            //   bass-ackwards).

    0,                      // ulDACRed (filled in later)
    0,                      // ulDACGreen (filled in later)
    0,                      // ulDACBlue (filled in later)

    0x0024,                 // ulAspectX
    0x0024,                 // ulAspectY
    0x0033,                 // ulAspectXY (one-to-one aspect ratio)

    1,                      // xStyleStep
    1,                      // yStyleSte;
    3,                      // denStyleStep -- Styles have a one-to-one aspect
                            //   ratio, and every 'dot' is 3 pixels long

    { 0, 0 },               // ptlPhysOffset
    { 0, 0 },               // szlPhysSize

    256,                    // ulNumPalReg

    // These fields are for halftone initialization.  The actual values are
    // a bit magic, but seem to work well on our display.

    {                       // ciDevice
       { 6700, 3300, 0 },   //      Red
       { 2100, 7100, 0 },   //      Green
       { 1400,  800, 0 },   //      Blue
       { 1750, 3950, 0 },   //      Cyan
       { 4050, 2050, 0 },   //      Magenta
       { 4400, 5200, 0 },   //      Yellow
       { 3127, 3290, 0 },   //      AlignmentWhite
       20000,               //      RedGamma
       20000,               //      GreenGamma
       20000,               //      BlueGamma
       0, 0, 0, 0, 0, 0     //      No dye correction for raster displays
    },

    0,                       // ulDevicePelsDPI (for printers only)
    PRIMARY_ORDER_CBA,       // ulPrimaryOrder
    HT_PATSIZE_4x4_M,        // ulHTPatternSize
    HT_FORMAT_8BPP,          // ulHTOutputFormat
    HT_FLAG_ADDITIVE_PRIMS,  // flHTFlags
    0,                       // ulVRefresh
    0,                       // ulPanningHorzRes
    0,                       // ulPanningVertRes
    0,                       // ulBltAlignment
};

//*********************** Public cStructure *************************************
//
//  DEVINFO gdevinfoDefault
//
//  This contains the default DEVINFO fields that are passed back to GDI
//  during DrvEnablePDEV.
//
//  NOTE: This structure defaults to values for an 8bpp palette device.
//        Some fields are overwritten for different colour depths.
//
//******************************************************************************

#define SYSTM_LOGFONT {16,7,0,0,700,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,\
                       CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,\
                       VARIABLE_PITCH | FF_DONTCARE,L"System"}
#define HELVE_LOGFONT {12,9,0,0,400,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,\
                       CLIP_STROKE_PRECIS,PROOF_QUALITY,\
                       VARIABLE_PITCH | FF_DONTCARE,L"MS Sans Serif"}
#define COURI_LOGFONT {12,9,0,0,400,0,0,0,ANSI_CHARSET,OUT_DEFAULT_PRECIS,\
                       CLIP_STROKE_PRECIS,PROOF_QUALITY,\
                       FIXED_PITCH | FF_DONTCARE, L"Courier"}

DEVINFO gdevinfoDefault = {
    (GCAPS_OPAQUERECT       |
    GCAPS_MONO_DITHER       |
     GCAPS_DITHERONREALIZE  |
     GCAPS_ALTERNATEFILL    |
     GCAPS_WINDINGFILL      |
     GCAPS_COLOR_DITHER     //|
     //GCAPS_ASYNCMOVE
#ifndef NVGDI
      | GCAPS_DIRECTDRAW
#endif
      ),
    SYSTM_LOGFONT,                              // lfDefaultFont
    HELVE_LOGFONT,                              // lfAnsiVarFont
    COURI_LOGFONT,                              // lfAnsiFixFont
    0,                                          // cFonts
    BMF_8BPP,                                   // iDitherFormat
    8,                                          // cxDither
    8,                                          // cyDither
    0                                           // hpalDefault (filled in later)
};

#ifdef NVD3D
//*********************************************************************
// This is the GLOBALDATA struct used to store the default settings
// for each GLOBALDATA struct created dynamically at DrvEnablePDEV
// time.
//*********************************************************************
GLOBALDATA  DefaultGlobalData;


//******************************************************************************
// This semaphore is used by the DX and GDI drivers to synchronize access to the
// DX GLOBALDATA struct.
//******************************************************************************
ENGSAFESEMAPHORE  NvSem = {(HSEMAPHORE) 0, 0L};
#endif // NVD3D

//************************* Public Structure ***********************************
//
//  FUNCTION TABLE:    DFVFN gadrvfn[]
//
//  Build the driver function table gadrvfn with function index/address
//  pairs.  This table tells GDI which DDI calls we support, and their
//  location (GDI does an indirect call through this table to call us).
//
//  Why haven't we implemented DrvSaveScreenBits?  To save code.
//
//  When the driver doesn't hook DrvSaveScreenBits, USER simulates on-
//  the-fly by creating a temporary device-format-bitmap, and explicitly
//  calling DrvCopyBits to save/restore the bits.  Since we already hook
//  DrvCreateDeviceBitmap, we'll end up using off-screen memory to store
//  the bits anyway (which would have been the main reason for implementing
//  DrvSaveScreenBits).  So we may as well save some working set.
//
//******************************************************************************

#ifdef NT4_MULTI_DEV
extern DRVFN gadrvfn[];
extern ULONG gcdrvfn;
#else //#ifdef NT4_MULTI_DEV

#if MULTI_BOARDS

//******************************************************************************
// Multi-board support has its own thunks...
//******************************************************************************

DRVFN gadrvfn[] = {
    {   INDEX_DrvEnablePDEV,            (PFN) MulEnablePDEV         },
    {   INDEX_DrvCompletePDEV,          (PFN) MulCompletePDEV       },
    {   INDEX_DrvDisablePDEV,           (PFN) MulDisablePDEV        },
    {   INDEX_DrvEnableSurface,         (PFN) MulEnableSurface      },
    {   INDEX_DrvDisableSurface,        (PFN) MulDisableSurface     },
    {   INDEX_DrvAssertMode,            (PFN) MulAssertMode         },
    {   INDEX_DrvMovePointer,           (PFN) MulMovePointer        },
    {   INDEX_DrvSetPointerShape,       (PFN) MulSetPointerShape    },
    {   INDEX_DrvDitherColor,           (PFN) MulDitherColor        },
    {   INDEX_DrvSetPalette,            (PFN) MulSetPalette         },
    {   INDEX_DrvCopyBits,              (PFN) MulCopyBits           },
    {   INDEX_DrvBitBlt,                (PFN) MulBitBlt             },
    {   INDEX_DrvTextOut,               (PFN) MulTextOut            },
    {   INDEX_DrvGetModes,              (PFN) MulGetModes           },
    {   INDEX_DrvStrokePath,            (PFN) MulStrokePath         },
//  {   INDEX_DrvFillPath,              (PFN) MulFillPath           }, not supported
    {   INDEX_DrvPaint,                 (PFN) MulPaint              },
    {   INDEX_DrvRealizeBrush,          (PFN) MulRealizeBrush       },
    {   INDEX_DrvDestroyFont,           (PFN) MulDestroyFont        },
    // Note that we don't support DrvCreateDeviceBitmap for multi-boards
    // Note that we don't support DrvDeleteDeviceBitmap for multi-boards
    // Note that we don't support DrvStretchBlt for multi-boards
    // Note that we don't support DrvLineTo for multi-boards
    // Note that we don't support DrvEscape for multi-boards
    // Note that we don't support DrvDirectDraw functions for multi-boards
};

#elif DBG && !defined(QUIET)

//******************************************************************************
// On Checked builds, thunk everything through Dbg calls...
//******************************************************************************

DRVFN gadrvfn[] = {
    {   INDEX_DrvEnablePDEV,            (PFN) DbgEnablePDEV         },
    {   INDEX_DrvCompletePDEV,          (PFN) DbgCompletePDEV       },
    {   INDEX_DrvDisablePDEV,           (PFN) DbgDisablePDEV        },
    {   INDEX_DrvEnableSurface,         (PFN) DbgEnableSurface      },
    {   INDEX_DrvDisableSurface,        (PFN) DbgDisableSurface     },
    {   INDEX_DrvAssertMode,            (PFN) DbgAssertMode         },
    {   INDEX_DrvMovePointer,           (PFN) DbgMovePointer        },
    {   INDEX_DrvSetPointerShape,       (PFN) DbgSetPointerShape    },
    {   INDEX_DrvDitherColor,           (PFN) DbgDitherColor        },
    {   INDEX_DrvSetPalette,            (PFN) DbgSetPalette         },
    {   INDEX_DrvCopyBits,              (PFN) DbgCopyBits           },
    {   INDEX_DrvBitBlt,                (PFN) DbgBitBlt             },
    {   INDEX_DrvTextOut,               (PFN) DbgTextOut            },
    {   INDEX_DrvGetModes,              (PFN) DbgGetModes           },
    {   INDEX_DrvLineTo,                (PFN) DbgLineTo             },
    {   INDEX_DrvStrokePath,            (PFN) DbgStrokePath         },
//    {   INDEX_DrvFillPath,              (PFN) DbgFillPath           }, // not suppported
    {   INDEX_DrvPaint,                 (PFN) DbgPaint              },
    {   INDEX_DrvRealizeBrush,          (PFN) DbgRealizeBrush       },
    {   INDEX_DrvCreateDeviceBitmap,    (PFN) DbgCreateDeviceBitmap },
    {   INDEX_DrvDeleteDeviceBitmap,    (PFN) DbgDeleteDeviceBitmap },
//  {   INDEX_DrvStretchBlt,            (PFN) DbgStretchBlt         }, // not implemented or hooked
    {   INDEX_DrvDestroyFont,           (PFN) DbgDestroyFont        },
#ifndef NVGDI
    {   INDEX_DrvGetDirectDrawInfo,     (PFN) DrvGetDirectDrawInfo  },
    {   INDEX_DrvEnableDirectDraw,      (PFN) DrvEnableDirectDraw   },
    {   INDEX_DrvDisableDirectDraw,     (PFN) DrvDisableDirectDraw  },
#endif
    {   INDEX_DrvSynchronize,           (PFN) DbgSynchronize        },
    {   INDEX_DrvEscape,                (PFN) DbgEscape             },
    {   INDEX_DrvDrawEscape,            (PFN) DbgDrawEscape         },
    {   INDEX_DrvResetPDEV,             (PFN) DbgResetPDEV          },
    {   INDEX_DrvDescribePixelFormat,   (PFN) DrvDescribePixelFormat},
    {   INDEX_DrvSetPixelFormat,        (PFN) DrvSetPixelFormat     },
    {   INDEX_DrvSwapBuffers,           (PFN) DrvSwapBuffers        },

    #if _WIN32_WINNT >= 0x0500
    {   INDEX_DrvIcmSetDeviceGammaRamp, (PFN) DbgIcmSetDeviceGammaRamp},
    {   INDEX_DrvDeriveSurface,         (PFN) DbgDeriveSurface},
    {   INDEX_DrvNotify,                (PFN) DbgDrvNotify},
    #ifndef NV3
    {   INDEX_DrvAlphaBlend,            (PFN) DbgAlphaBlend},           // not supported in nv3
    {   INDEX_DrvTransparentBlt,        (PFN) DbgTransparentBlt},       // not supported in nv3
    #endif
    #endif
#if (NVARCH >= 0x4)
    {   INDEX_DrvSaveScreenBits,        (PFN) DrvSaveScreenBits     },
#endif
};
#else

//******************************************************************************
// On Free builds, directly call the appropriate functions...
//******************************************************************************

DRVFN gadrvfn[] = {
    {   INDEX_DrvEnablePDEV,            (PFN) DrvEnablePDEV         },
    {   INDEX_DrvCompletePDEV,          (PFN) DrvCompletePDEV       },
    {   INDEX_DrvDisablePDEV,           (PFN) DrvDisablePDEV        },
    {   INDEX_DrvEnableSurface,         (PFN) DrvEnableSurface      },
    {   INDEX_DrvDisableSurface,        (PFN) DrvDisableSurface     },
    {   INDEX_DrvAssertMode,            (PFN) DrvAssertMode         },
    {   INDEX_DrvMovePointer,           (PFN) DrvMovePointer        },
    {   INDEX_DrvSetPointerShape,       (PFN) DrvSetPointerShape    },
    {   INDEX_DrvDitherColor,           (PFN) DrvDitherColor        },
    {   INDEX_DrvSetPalette,            (PFN) DrvSetPalette         },
    {   INDEX_DrvCopyBits,              (PFN) DrvCopyBits           },
    {   INDEX_DrvBitBlt,                (PFN) DrvBitBlt             },
    {   INDEX_DrvTextOut,               (PFN) DrvTextOut            },
    {   INDEX_DrvGetModes,              (PFN) DrvGetModes           },
    {   INDEX_DrvLineTo,                (PFN) DrvLineTo             },
    {   INDEX_DrvStrokePath,            (PFN) DrvStrokePath         },
//    {   INDEX_DrvFillPath,              (PFN) DrvFillPath           }, // not supported
    {   INDEX_DrvPaint,                 (PFN) DrvPaint              },
    {   INDEX_DrvRealizeBrush,          (PFN) DrvRealizeBrush       },
    {   INDEX_DrvCreateDeviceBitmap,    (PFN) DrvCreateDeviceBitmap },
    {   INDEX_DrvDeleteDeviceBitmap,    (PFN) DrvDeleteDeviceBitmap },
//  {   INDEX_DrvStretchBlt,            (PFN) DrvStretchBlt         }, // not implemented or hooked
    {   INDEX_DrvDestroyFont,           (PFN) DrvDestroyFont        },
#ifndef NVGDI
    {   INDEX_DrvGetDirectDrawInfo,     (PFN) DrvGetDirectDrawInfo  },
    {   INDEX_DrvEnableDirectDraw,      (PFN) DrvEnableDirectDraw   },
    {   INDEX_DrvDisableDirectDraw,     (PFN) DrvDisableDirectDraw  },
#endif
    {   INDEX_DrvSynchronize,           (PFN) DrvSynchronize        },
    {   INDEX_DrvEscape,                (PFN) DrvEscape             },
    {   INDEX_DrvDrawEscape,            (PFN) DrvDrawEscape         },
    {   INDEX_DrvResetPDEV,             (PFN) DrvResetPDEV          },
    {   INDEX_DrvDescribePixelFormat,   (PFN) DrvDescribePixelFormat},
    {   INDEX_DrvSetPixelFormat,        (PFN) DrvSetPixelFormat     },
    {   INDEX_DrvSwapBuffers,           (PFN) DrvSwapBuffers        },

    #if _WIN32_WINNT >= 0x0500
    {   INDEX_DrvIcmSetDeviceGammaRamp, (PFN) DrvIcmSetDeviceGammaRamp},
    {   INDEX_DrvDeriveSurface,         (PFN) DrvDeriveSurface},
    {   INDEX_DrvNotify,                (PFN) DrvNotify},
    #ifndef NV3
    {   INDEX_DrvAlphaBlend,            (PFN) DrvAlphaBlend},           // not supported in nv3
    {   INDEX_DrvTransparentBlt,        (PFN) DrvTransparentBlt},       // not supported in nv3
    #endif
    #endif
#if (NVARCH >= 0x4)
    {   INDEX_DrvSaveScreenBits,        (PFN) DrvSaveScreenBits     },
#endif
};
#endif

ULONG gcdrvfn = sizeof(gadrvfn) / sizeof(DRVFN);

#endif // NT4_MULTI_DEV


//******************************************************************************
//
//  Function:   GetDisplayUniqueness
//
//  Routine Description:
//
//      Returns the display uniqueness.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

ULONG GetDisplayUniqueness(PDEV *ppdev)
    {
    return ppdev->iUniqueness;
    }

//******************************************************************************
//
//  Function:   DrvResetPDEV
//
//  Routine Description:
//
//      Notifies the driver of a dynamic mode change.
//
//  Arguments:
//
//  Return Value:
//
//      TRUE : okay, now GDI is allowed to continue with the mode switch
//      FALSE: Stop with the mode switch, return to the old mode. This causes
//             GDI to open a dialog that a reboot is necessary!
//
//******************************************************************************


BOOL DrvResetPDEV(
DHPDEV dhpdevOld,
DHPDEV dhpdevNew)

    {
    PDEV* ppdevNew = (PDEV*) dhpdevNew;
    PDEV* ppdevOld = (PDEV*) dhpdevOld;
    ULONG                       i;
    NV_OPENGL_GLOBAL_DATA*      nvGlobalData;
    long                        status;
    ULONG                       ulTemp;
    HSEMAPHORE                  csFifoTemp;
    NV_OPENGL_CLIENT_INFO      *clientInfo;
    BOOL                        bRet = FALSE; // default error exit

    // Do some test for OpenGL that might cause problems
    // in the new resolution
    if (!bOglResetPDEV(ppdevOld, ppdevNew))
        goto Exit;

    ppdevNew->iUniqueness             = ppdevOld->iUniqueness + 1;
#ifdef NVD3D
    ppdevNew->bDXGblStateInitialized = ppdevOld->bDXGblStateInitialized;
#endif // NVD3D
    ppdevOld->pfnAcquireOglMutex(ppdevOld);

    ppdevNew->dwGlobalModeSwitchCount = ppdevOld->dwGlobalModeSwitchCount;

    if (HWGFXCAPS_QUADRO_GENERIC(ppdevOld))
    {
        // The reference counter must be unique as long as the system isn't rebooted
        ppdevNew->ulSaveScreenBitsRefCounter = ppdevOld->ulSaveScreenBitsRefCounter;
        bSaveScreenBitsGarbageCollection(ppdevOld, 0); // remove all elements we've collected (GDI bug)
    }

    if (ppdevOld->oglGlobalPageMdl)
        {
        // This swap is done because we can't have global memory
        // allocated within the display driver.  PDEV is local
        // to current instantiation so we swap memory old and new
        // pointers and information.
        //
        // Swap new and old OGL pointers.  The old OGL pointer
        // is in use by clients so we can't free it but we can free
        // the new one.  So, swap them and the new one will get freed
        // when the disable PDEV call is made.
        //
        ULONG *oglGlobalPagePtr;           // OGL mutex lock variable
        ULONG oglGlobalPageSize;           // OGL mutex lock size in bytes
        PVOID oglGlobalPageMdl;            // MDL to locked down mutex page
        PVOID origOglGlobalPagePtr;        // OGL Ptr for allocated memory

        // tmp = New
        oglGlobalPagePtr  = ppdevNew->oglGlobalPagePtr;
        oglGlobalPageSize = ppdevNew->oglGlobalPageSize;
        oglGlobalPageMdl  = ppdevNew->oglGlobalPageMdl;
        origOglGlobalPagePtr = ppdevNew->origOglGlobalPagePtr;

        // New = Old
        ppdevNew->oglGlobalPagePtr  = ppdevOld->oglGlobalPagePtr;
        ppdevNew->oglGlobalPageSize = ppdevOld->oglGlobalPageSize;
        ppdevNew->oglGlobalPageMdl  = ppdevOld->oglGlobalPageMdl;
        ppdevNew->origOglGlobalPagePtr = ppdevOld->origOglGlobalPagePtr;

        // old = tmp
        ppdevOld->oglGlobalPagePtr  = oglGlobalPagePtr;
        ppdevOld->oglGlobalPageSize = oglGlobalPageSize;
        ppdevOld->oglGlobalPageMdl  = oglGlobalPageMdl;
        ppdevOld->origOglGlobalPagePtr = origOglGlobalPagePtr;

        nvGlobalData = (NV_OPENGL_GLOBAL_DATA *)ppdevNew->oglGlobalPagePtr;
        if (nvGlobalData)
            {
            ppdevNew->oglMutexPtr      = &nvGlobalData->oglMutex;
            ppdevNew->oglModeSwitchPtr = &nvGlobalData->oglModeSwitch;
            }

        nvGlobalData = (NV_OPENGL_GLOBAL_DATA *)ppdevOld->oglGlobalPagePtr;
        if (nvGlobalData)
            {
            ppdevOld->oglMutexPtr      = &nvGlobalData->oglMutex;
            ppdevOld->oglModeSwitchPtr = &nvGlobalData->oglModeSwitch;
            }
        }

    OglAcquireGlobalMutex();
    if (globalOpenGLData.oglClientListHead.clientList)
        {
        globalOpenGLData.oglClientListHead.clientList->refCount++;
        }


    // Update width and height of screen in each node.
    clientInfo = NULL; // start search at top of list
    while (bOglGetNextClient(&clientInfo))
        {
        NV_OPENGL_DRAWABLE_INFO *clientDrawableInfo;

        ASSERT(bOglHasDrawableInfo(clientInfo));

#ifdef NV_MAIN_MULTIMON
        // NOTE: We want to check the old PDEV with
        // the one inside the clientInfo, so in a 
        // Multi-Monitor situation, we don't update
        // clients with the modeset PDEV.  Since we
        // keep multiple clients per-app/per-monitor,
        // we would end up having all the clients
        // point to the same monitor.  This is bad.
        if ( clientInfo->ppdev == ppdevOld ) {
            clientInfo->ppdev  = ppdevNew;
            clientDrawableInfo = clientInfo->clientDrawableInfo;
            if (clientDrawableInfo)
                {
                clientDrawableInfo->cxScreen = ppdevNew->cxScreen;
                clientDrawableInfo->cyScreen = ppdevNew->cyScreen;
                clientDrawableInfo->cxcyChanged = 1;
                }
            }
        }
#else
        clientInfo->ppdev  = ppdevNew;
        clientDrawableInfo = clientInfo->clientDrawableInfo;
        if (clientDrawableInfo)
            {
            clientDrawableInfo->cxScreen = ppdevNew->cxScreen;
            clientDrawableInfo->cyScreen = ppdevNew->cyScreen;
            clientDrawableInfo->cxcyChanged = 1;
            }
        }
#endif


    OglReleaseGlobalMutex();

    // release mutex taken with previous ppdevOld->pfnAcquireOglMutex(ppdevOld);
    ulTemp = ppdevNew->bReleaseOglMutex;
    ppdevNew->bReleaseOglMutex = ppdevOld->bReleaseOglMutex;
    ppdevOld->bReleaseOglMutex = ulTemp;

    csFifoTemp = ppdevNew->csFifo;
    ppdevNew->csFifo = ppdevOld->csFifo;
    ppdevOld->csFifo = csFifoTemp;

    ppdevNew->bSupportOgl = ppdevOld->bSupportOgl;

    OglEnableModeSwitchUpdate(ppdevNew);

    ppdevNew->pfnReleaseOglMutex(ppdevNew);

    // we need to do this when switching back to a previously left desktopsize
    ppdevNew->oglDDrawSurfaceCount = 0;
    bClearPointerList(&ppdevNew->oglDDrawSurfaceCountList);

    // Transfer the state of the NVSVC service from the old prev to the new pdev.
    ppdevNew->ulNVSVCStatus = ppdevOld->ulNVSVCStatus;

    bRet = TRUE;
Exit:
    return(bRet);
    }


//******************************************************************************
//
//  Function:   DrvEnableDriver
//
//  Routine Description:
//
//      Enables the driver by retrieving the drivers function table and version.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOL DrvEnableDriver(
ULONG          iEngineVersion,
ULONG          cj,
DRVENABLEDATA* pded)

    {
    //**************************************************************************
    // Engine Version is passed down so future drivers can support previous
    // engine versions.  A next generation driver can support both the old
    // and new engine conventions if told what version of engine it is
    // working with.  For the first version the driver does nothing with it.
    //**************************************************************************
    //**************************************************************************
    // Fill in as much as we can.
    //**************************************************************************
    if (cj >= sizeof(DRVENABLEDATA))
        pded->pdrvfn = gadrvfn;

    if (cj >= (sizeof(ULONG) * 2))
        pded->c = gcdrvfn;

    //**************************************************************************
    // DDI version this driver was targeted for is passed back to engine.
    // Future graphic's engine may break calls down to old driver format.
    //**************************************************************************

    if (cj >= sizeof(ULONG))

#if _WIN32_WINNT >= 0x0500  // Win2k or later OS
        pded->iDriverVersion = DDI_DRIVER_VERSION_NT5;
#else // NT4 build uses NT4 DDK, so use version available in NT4 DDK WINDDI.H
        pded->iDriverVersion = DDI_DRIVER_VERSION;
#endif

    //**************************************************************************
    // Save the DDI version so we can tell the miniport which version of NT is running.
    //**************************************************************************
    //iOsVersion = iEngineVersion;

#if defined(DX7) && (NVARCH >= 0x04)

    //**************************************************************************
    // Create the semaphore used to sync access to the DX GLOBALDATA
    //**************************************************************************
    EngInitializeSafeSemaphore(&NvSem);

    //**************************************************************************
    // Initialize data which is GLOBAL to the DX7 driver. This data is not
    // associated with a particular device.
    //**************************************************************************
#ifndef _WIN64
    NvWin2KInitDXGlobals();
#endif // D3D is not 64 bit code clean , skip for now...

#endif  // DX7

    OglInitLayerPalette();

#ifdef NV_MAIN_MULTIMON
    /* Initialize the global PDEV list for OpenGL. */
    OglInitPDEVList();
#endif

    return(TRUE);
    }

//******************************************************************************
//
//  Function:   DrvDisableDriver
//
//  Routine Description:
//
//      Tells the driver it is being disabled. Release any resources allocated in
//      DrvEnableDriver.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID DrvDisableDriver(VOID)
    {
#if defined(DX7) && (NVARCH >= 0x04)
    //**************************************************************************
    // Create the semaphore used to sync access to the DX GLOBALDATA
    //**************************************************************************
    EngDeleteSafeSemaphore(&NvSem);
#endif  // DX7

#ifdef NV_MAIN_MULTIMON
    /* Destroy the global PDEV list for OpenGL. */
    OglDestroyPDEVList();
#endif

    return;
    }

//******************************************************************************
//
//  Function:   DrvEnablePDEV
//
//  Routine Description:
//
//    Initializes a bunch of fields for GDI, based on the mode we've been asked
//    to do.  This is the first thing called after DrvEnableDriver, when GDI
//    wants to get some information about us.
//
//    (This function mostly returns back information; DrvEnableSurface is used
//    for initializing the hardware and driver components.)
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

DHPDEV DrvEnablePDEV(
        DEVMODEW*   pdm,            // Contains data pertaining to REQUESTED MODE
        PWSTR       pwszLogAddr,    // Logical address
        ULONG       cPat,           // Count of standard patterns
        HSURF*      phsurfPatterns, // Buffer for standard patterns
        ULONG       cjCaps,         // Size of buffer for device caps 'pdevcaps'
        ULONG*      pdevcaps,       // Buffer for device caps, also known as 'gdiinfo'
        ULONG       cjDevInfo,      // Number of bytes in device info 'pdi'
        DEVINFO*    pdi,            // Device information
        HDEV        hdev,           // HDEV, used for callbacks
        PWSTR       pwszDeviceName, // Device name
        HANDLE      hDriver)        // Kernel driver handle

{
    PDEV* ppdev;
    ULONG ReturnedDataLength;
    NV_COMMON_DEVICE_INFO CommonDeviceInfo;

    //**************************************************************************
    // Future versions of NT had better supply 'devcaps' and 'devinfo'
    // structures that are the same size or larger than the current
    // structures:
    //**************************************************************************
    if ((cjCaps < sizeof(GDIINFO)) || (cjDevInfo < sizeof(DEVINFO)))
        {
        DISPDBG((2, "DrvEnablePDEV - Buffer size too small"));
        goto ReturnFailure0;
        }

    //**************************************************************************
    // Allocate a physical device structure.  Note that we definitely
    // rely on the zero initialization:
    //**************************************************************************

    ppdev = EngAllocMem(FL_ZERO_MEMORY, sizeof(PDEV), ALLOC_TAG);
    if (ppdev == NULL)
        {
        DISPDBG((2, "DrvEnablePDEV - Failed EngAllocMem"));
        goto ReturnFailure0;
        }
    memset(ppdev, 0, sizeof(PDEV));

    ppdev->hDriver = hDriver;
#if DBG
    GetDebugLevelRegValue(ppdev);
#endif

    if (ppdev && pdm)
    {
        DISPDBG((6,"DrvEnablePDev(): ppdev: 0x%x, requesting mode: (%d, %d), %d bpp, %d HZ\n",
            ppdev,pdm->dmPelsWidth,pdm->dmPelsHeight,pdm->dmBitsPerPel,pdm->dmDisplayFrequency));
    }

    //********************************************************************
    // Store our 'Unique' signature in the PPDEV
    //********************************************************************

    ppdev->NVSignature = NV_SIGNATURE;

    //********************************************************************
    // Store global settings in the PDEV. BUGBUG: GLOBAL VARS NOT ALLOWED!!!
    //********************************************************************

    ppdev->bEnableIF09 = TRUE; //bEnableIF09;

    //**************************************************************************
    //
    // OpenGL code:
    //
    // Create OGL mutex pointer and lock down the memory page
    //
    //**************************************************************************
    CreateOglGlobalMemory(ppdev);
    ppdev->bSupportOgl = (ULONG)TRUE; // Enable OpenGL support by default

#ifdef NV_MAIN_MULTIMON
    /* Add the ppdev to the global PDEV list for OpenGL.  See oglmultimon.c */
    OglAddPDEVToList(ppdev);
#endif

#ifdef NV3
        ppdev->bOglSingleBackDepthSupported = (ULONG)FALSE; // Not available on NV3
#else        
        ppdev->bOglSingleBackDepthSupported = (ULONG)TRUE; // Enable single-back-depth surfaces
#endif // NV3

    ppdev->DoubleBuffer.bEnabled = FALSE;

    

    //**************************************************************************
    // Get the current screen mode information.  Set up device caps and
    // devinfo:
    //**************************************************************************

    if (!bInitializeModeFields(ppdev, (GDIINFO*) pdevcaps, pdi, pdm))
        {
        DISPDBG((2, "DrvEnablePDEV - Failed bInitializeModeFields"));
        goto ReturnFailure1;
        }

    //**************************************************************************
    // Save position relative to desktop (0,0 on single monitor system and
    // relative to primary display on multi-monitor system)
    //**************************************************************************
#if _WIN32_WINNT >= 0x0500
    ppdev->left = pdm->dmPosition.x;
    ppdev->top = pdm->dmPosition.y;
#else
    // ASSUME NT4 builds...
    // will be set by NT4 multiboard wrapper
    ppdev->left = 0;
    ppdev->top = 0;
#endif // NVD3D

    //**************************************************************************
    // Initialize palette information.
    //**************************************************************************

    if (!bInitializePalette(ppdev, pdi))
        {
        DISPDBG((2, "DrvEnablePDEV - Failed bInitializePalette"));
        goto ReturnFailure1;
        }

    //**************************************************************************
    // We are running a version of NT which is >= 4.0. This means we can safely
    // enable P6 write combining when mapping the framebuffer. The following IOCTL
    // sets a flag in the device state which tells the miniport to do just that.
    // This needs to be done for every physical NV device enabled by this driver.
    //**************************************************************************
    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_ENABLE_WRITE_COMBINING,
                           NULL,
                           0,
                           NULL,
                           0,
                           &ReturnedDataLength))
        {
        DISPDBG((1, "DrvEnablePDEV - IOCTL_VIDEO_ENABLE_WRITE_COMBINING failed"));
        }

    //
    // Get common device information from miniport
    //
    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_GET_COMMON_DEVICE_INFO,
                           NULL,
                           0,
                           &CommonDeviceInfo,
                           sizeof(NV_COMMON_DEVICE_INFO),
                           &ReturnedDataLength))
    {
        DISPDBG((1, "DrvEnablePDEV - VIDEO_GET_COMMON_DEVICE_INFO failed"));
        goto ReturnFailure1;
    }
    ppdev->ulDeviceReference   = CommonDeviceInfo.ulDeviceReference;
    ppdev->ulEnableDualView    = CommonDeviceInfo.ulEnableDualView;
    ppdev->ulDualViewSecondary = CommonDeviceInfo.ulDualViewSecondary;
#if _WIN32_WINNT >= 0x0500
    ppdev->OsVersionInfo       = CommonDeviceInfo.OsVersionInfo;
    ppdev->SysMemInfo          = CommonDeviceInfo.SysMemInfo;
#endif // _WIN32_WINNT >= 0x0500

    //**************************************************************************
    // Get the OGL stencil registry switch
    //**************************************************************************

    ppdev->OglStencilSwitch = FALSE;

    if (EngDeviceIoControl(
            ppdev->hDriver,
            IOCTL_VIDEO_GET_OGL_STENCIL_SWITCH,
            NULL,
            0,
            &ppdev->OglStencilSwitch,
            sizeof(DWORD),
            &ReturnedDataLength))
    {
        DISPDBG((1, "DrvEnablePDEV - VIDEO_GET_OGL_STENCIL_SWITCH failed"));
    }


    // On NT4, there is no global DriverData struct.  Here we allocate and 
        // hang it on the PDEV.
        //
#if IS_WINNT4
    if (ppdev->pDriverData == NULL)
    {
        ppdev->pDriverData = EngAllocMem(FL_ZERO_MEMORY, sizeof(GLOBALDATA), ALLOC_TAG);
        if (ppdev->pDriverData == NULL)
        {
            goto ReturnFailure1;
        }
        ppdev->pDriverData->ppdev = ppdev;
    }
#endif

#if defined(DX7) && !defined(_WIN64) && (NVARCH >=0x04)
    if (NvWin2KAllocDriverData(ppdev))
    {
        DISPDBG((1, "DrvEnablePDEV - Allocation of DX7 GLOBALDATA failed"));
        goto ReturnFailure1;
    }

    //**************************************************************************
    // Initialize fixed 'global' data which is device specific (fixed on a
    // per device basis).
    //**************************************************************************
    nvDeterminePerformanceStrategy(ppdev->pDriverData, ppdev);
    ppdev->pDriverData->NvSemRefCnt = 0;

#endif  // DX7

    //**************************************************************************
    // Initialize nvidia class list structure
    //**************************************************************************
    ppdev->nvNumClasses = 0;
    ppdev->nvClassList = NULL;

    //**************************************************************************
    // Fill in the Eng..() procs with the default GDI routines.
    // This gets overloaded with the NV_Eng.._DBLBUF() procs in nvdoublebuf.c
    // when we are in double buffer mode
    //**************************************************************************
    ppdev->pfnEngBitBlt     = EngBitBlt;
    ppdev->pfnEngCopyBits   = EngCopyBits;
    ppdev->pfnEngTextOut    = EngTextOut;
    ppdev->pfnEngLineTo     = EngLineTo;
    ppdev->pfnEngStrokePath = EngStrokePath;
#if _WIN32_WINNT >= 0x0500
    ppdev->pfnEngAlphaBlend = EngAlphaBlend;
    ppdev->pfnEngTransparentBlt = EngTransparentBlt;
#endif

    //
    // Clear the default request for NVSVC to induce a modeset. It will be set later in HandleBiosHeads()
    // if needed. Also set the default status of NVSVC as stopped.
    //
    ppdev->ulInduceModeChangeDeviceMask = 0;
    ppdev->ulNVSVCStatus = NVSVC_STATUS_STOPPED;

    return((DHPDEV) ppdev);

ReturnFailure1:
    DrvDisablePDEV((DHPDEV) ppdev);

ReturnFailure0:
    DISPDBG((1, "Failed DrvEnablePDEV"));

    return(0);
    }

//******************************************************************************
//
//  Function:   DrvDisablePDEV
//
//  Routine Description:
//
//     Release the resources allocated in DrvEnablePDEV.  If a surface has been
//     enabled DrvDisableSurface will have already been called.
//
//     Note that this function will be called when previewing modes in the
//     Display Applet, but not at system shutdown.  If you need to reset the
//     hardware at shutdown, you can do it in the miniport by providing a
//     'HwResetHw' entry point in the VIDEO_HW_INITIALIZATION_DATA structure.
//
//     Note: In an error, we may call this before DrvEnablePDEV is done.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID DrvDisablePDEV(DHPDEV dhpdev)
{
    PDEV* ppdev;

    ppdev = (PDEV*) dhpdev;

    vUninitializePalette(ppdev);

#if IS_WINNT4
    if (ppdev->pDriverData != NULL)
    {
        EngFreeMem(ppdev->pDriverData);
        ppdev->pDriverData = NULL;
    }
#endif

    //**************************************************************************
    //
    // OpenGL code:
    //
    // Destroy OGL mutex pointer and unlock down the memory page
    //
    //**************************************************************************
    DestroyOglGlobalMemory(ppdev);

#ifdef NV_MAIN_MULTIMON
    /* Remove the ppdev from the global PDEV list for OpenGL.  See oglmultimon.c */    
    OglRemovePDEVFromList(ppdev);
#endif

    // Free up any memory alloced by display driver for multimon purposes.
    FreeMemoryMultiMon(ppdev);

    EngFreeMem(ppdev);
}

//******************************************************************************
//
//  Function:   DrvCompletePDEV
//
//  Routine Description:
//
//      Store the HPDEV, the engines handle for this PDEV, in the DHPDEV.
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID DrvCompletePDEV(
DHPDEV dhpdev,
HDEV   hdev)
{
    ((PDEV*) dhpdev)->hdevEng = hdev;
}

//******************************************************************************
//
//  Function:   DrvSynchronize
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

VOID DrvSynchronize(DHPDEV dhpdev, RECTL  *prcl)
{
    PDEV*           ppdev;

    ppdev = (PDEV *)dhpdev;

    //**************************************************************************
    // Call the NV1/NV3/NV4 specific version of 'waiting while the engine is busy' routine
    //**************************************************************************

    ppdev->pfnWaitEngineBusy( ppdev );

    return;
}


#define CALIBRATION_ENABLED // enable calibration in DrvEnableSurface



//******************************************************************************
//
//  Function:   HSURF DrvEnableSurface
//
//      Creates the drawing surface, initializes the hardware, and initializes
//      driver components.  This function is called after DrvEnablePDEV, and
//      performs the final device initialization.
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//
//      None.
//
//******************************************************************************

HSURF DrvEnableSurface(
DHPDEV dhpdev)
    {
    PDEV*   ppdev;
    HSURF   hsurf;
    DSURF*  pdsurf;
    BYTE*   TextureDataPtr;
    ULONG   i;
    PULONG  pulBmp;
    PULONG DMABufferStart;
    PULONG DMABufferEnd;
    PULONG VRAMStart;
    PULONG VRAMEnd;
    BOOL   UseDFBPrimary;
    ppdev = (PDEV*) dhpdev;

    // Turn off the DrvAssertMode flag.
    ppdev->ulDrvAssertModeTRUE = 0;

    //**************************************************************************
    // First enable all the subcomponents.
    //
    // Note that the order in which these 'Enable' functions are called
    // may be significant in low off-screen memory conditions, because
    // the off-screen heap manager may fail some of the later
    // allocations...
    //**************************************************************************

    //**************************************************************************
    // EnableHardware mainly does 4 things:
    //      1) Gets access to framebuffer memory
    //      2) Gets access to miscellaneous memory mapped hardware registers
    //      3) Sets the mode, then fills in the ppdev fields (AssertModeHardware)
    //      4) Init the function table (NV1 functions vs NV3 functions)
    //**************************************************************************


    if (!bEnableHardware(ppdev))
        goto ReturnFailure;

    //**************************************************************************
    // Call the NV1/NV3 specific version of 'waiting while the engine is busy' routine
    // ??? NEED TO INCLUDE THIS TO PREVENT INCONSISTENT LOCKUP ???
    //**************************************************************************

    ppdev->pfnWaitEngineBusy(ppdev);

    // test rect render patch (jsw)
    //RenderRectangle(ppdev);

    //**************************************************************************
    // EnableOffscreenHeap mainly does 3 things:
    //      1) Inits the heap to contain all of Video Memory, then..
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //!!! JOEHACK the next two should be done in EnableSurface to decouple  !!!
    //!!!         offscreen mgmt & bitmap caching from primary surface init !!!
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //      2) Allocates a permanent region for the visible screen
    //      3) Creates the wrapper surface objects that GDI will use,
    //         when we use GDI to draw to our offscreen DFB's.
    //**************************************************************************
    if (!bEnableOffscreenHeap(ppdev))
        goto ReturnFailure;

    //**************************************************************************
    // EnablePointer currently does nothing (just returns TRUE)
    //**************************************************************************

    if (!bEnablePointer(ppdev))
        goto ReturnFailure;

    //**************************************************************************
    // EnablePointer currently does nothing (just returns TRUE)
    // since we don't do glyph caching
    //**************************************************************************

    if (!bEnableText(ppdev))
        goto ReturnFailure;

    //**************************************************************************
    // EnableBrushCache currently does nothing (just returns TRUE)
    // since we don't do brush caching
    //**************************************************************************

    //**************************************************************************
    // Inits the 8bpp palette registers
    //**************************************************************************

    if (!bEnablePalette(ppdev))
        goto ReturnFailure;

    //**************************************************************************
    // EnableDirectDraw currently does measures vertical refresh rate
    // and sets the bit DIRECTDRAW_CAPABLE
    //**************************************************************************

    if (!bEnableDirectDraw(ppdev))
        goto ReturnFailure;

    //**************************************************************************
    // Initialize the ohScreen struct with primary screen info.  This should
    // be done before the DSURF stuct for the primary screen is initialized.
    //**************************************************************************

    ppdev->ohScreen.pdsurf         = &ppdev->dsurfScreen;
    ppdev->ohScreen.next           = NULL;
    ppdev->ohScreen.prev           = NULL;
    ppdev->ohScreen.nextEmpty      = NULL;
    ppdev->ohScreen.pHeap          = NULL;
    ppdev->ohScreen.ulSizeInBytes  = ppdev->cyScreen *
                                     ppdev->cxScreen * ppdev->cjPelSize;
    ppdev->ohScreen.ulOffset       = 0;
    ppdev->ohScreen.ulLinearStride = (ULONG)ppdev->lDelta;

    //**************************************************************************
    // Now create our private (DSURF) surface structure.
    // This DSURF structure is specifically used to denote the physical screen.
    //
    // Whenever we get a call to draw directly to the screen, we'll get
    // passed a pointer to a SURFOBJ whose 'dhpdev' field will point
    // to our PDEV structure, and whose 'dhsurf' field will point to the
    // following DSURF structure.
    //
    // Every device bitmap we create in DrvCreateDeviceBitmap will also
    // have its own unique DSURF structure allocated (but will share the
    // same PDEV).  To make our code more polymorphic for handling drawing
    // to either the screen or an off-screen bitmap, we have the same
    // structure for both.
    //
    //JOEHACK - obsolete comment???
    // NV: For NV, this is not the case.  Since we started with a Dumb Frame
    //     Buffer driver, we used EngCreateBitmap to create a surface
    //     for the visible screen, so that we could let GDI draw directly
    //     to it.  Since then, Offscreen DFB's have now been implemented, and we
    //     can now go back to using EngCreateDeviceSurface to create the surface
    //     and have the code be more consistent.  But that would require
    //     BitBlt, TextOut and other code to be reworked (DFB testing code
    //     will need to be changed as well as wrapper surface functionality,
    //     so that we can tell if a bitmap is in system memory, or in VRAM).
    //     For now, we'll just continue using EngCreateBitmap. Basically, it
    //     doesn't seem to make that much of a difference.  If we decide
    //     to switch back to having our primary surface being Device-Managed,
    //     we'll need to add back the following code.
    //
    //     The bottom line, is that we can always default back to GDI to draw
    //     to ANY of our surface objects.
    //**************************************************************************

    pdsurf = &ppdev->dsurfScreen;

    //**********************************************************************
    // DSURF structures denote whether a DFB exists in offscreen memory (DT_SCREEN)
    // or whether it's been converted to a DIB (DT_DIB) in system memory.
    // Since this DSURF structure represents the physical screen, we init
    // the DSURF fields with the physical screen attributes.
    //**********************************************************************

    pdsurf->dt           = DT_SCREEN;           // Not to be confused with a DIB
    pdsurf->sizl.cx      = ppdev->cxScreen;     // Width of screen surface
    pdsurf->sizl.cy      = ppdev->cyScreen;     // Height of screen surface
    pdsurf->ppdev        = ppdev;               // Pointer to physical device
    pdsurf->poh          = &ppdev->ohScreen;    // The screen is a surface, too
    pdsurf->LinearPtr    = ppdev->pjScreen;
    pdsurf->LinearStride = (ULONG) ppdev->lDelta;

    //**************************************************************************
    // Next, have GDI create the actual SURFOBJ.
    //
    // Our drawing surface is going to be 'device-managed', meaning that
    // GDI cannot draw on the framebuffer bits directly, and as such we
    // create the surface via EngCreateDeviceSurface.  By doing this, we ensure
    // that GDI will only ever access the bitmaps bits via the Drv calls
    // that we've HOOKed.
    //
    // If we could map the entire framebuffer linearly into main memory
    // (i.e., we didn't have to go through a 64k aperture), it would be
    // beneficial to create the surface via EngCreateBitmap, giving GDI a
    // pointer to the framebuffer bits.  When we pass a call on to GDI
    // where it can't directly read/write to the surface bits because the
    // surface is device managed, it has to create a temporary bitmap and
    // call our DrvCopyBits routine to get/set a copy of the affected bits.
    // Fer example, the OpenGL component prefers to be able to write on the
    // framebuffer bits directly.
    //
    // NOTE: For NV, we don't have this problem.  We create the surface
    //       using EngCreateBitmap.  This allows us to pass any call we want
    //       to back to GDI. (Driver can run in Dumb Framebuffer mode if desired)
    //**************************************************************************

    // Always use a STYPE_DEVICE for our primary
    hsurf = (HSURF) EngCreateDeviceSurface( (DHSURF)pdsurf,
                                            pdsurf->sizl,
                                            ppdev->iBitmapFormat );
        
    
    if (hsurf == 0)
    {
        DISPDBG((1, "DrvEnableSurface - Failed EngCreateBitmap/DeviceSurface"));
        goto ReturnFailure;
    }

    ppdev->hsurfScreen = hsurf;     // Remember it for clean-up

    //**************************************************************************
    // Now associate the surface and the PDEV.
    //
    // We have to associate the surface we just created with our physical
    // device so that GDI can get information related to the PDEV when
    // it's drawing to the surface (such as, for example, the length of
    // styles on the device when simulating styled lines).
    //**************************************************************************

    if (!EngAssociateSurface(hsurf, ppdev->hdevEng, ppdev->flHooks ))
    {
        DISPDBG((1, "DrvEnableSurface - Failed EngAssociateSurface"));
        goto ReturnFailure;
    }

    //**************************************************************************
    // DrvEnableSurface Succeeded!
    //**************************************************************************

    if (HWGFXCAPS_QUADRO_GENERIC(ppdev))
    {
        // Create the linked list used to store DrvSaveScreenBits data
        if(!bLinkedListCreate(&(ppdev->pLinkedListSaveScreenBits), LL_CLASS_SAVESCREENBITS))
        {
            //  If failed, return immediately

            goto ReturnFailure;
        }
    }

    DISPDBG((5, "Passed DrvEnableSurface"));


    //**************************************************************************
    // Call the NV1/NV3 specific version of 'waiting while the engine is busy' routine
    // ??? NEED TO INCLUDE THIS TO PREVENT INCONSISTENT LOCKUP ???
    //**************************************************************************

    ppdev->pfnWaitEngineBusy(ppdev);

    // OpenGL init
    if (!bOglServicesInit(ppdev))
    {
        DISPDBG((1, "DrvEnableSurface - Failed bOglServicesInit"));
        goto ReturnFailure;
    }

    #ifdef  CALIBRATION_ENABLED
    
    vCalibrate_XferSpeeds(ppdev);   // Just before returning from this last stage
                                    // of the installation of the driver/pdev,
                                    // calibrate to setup ppdev->globalXferOverheadFactor
    #endif


#if _WIN32_WINNT >= 0x0500
    // In W2K the video card's acceleration level can be dynamically set through the Display Applet
    // Here we read the value because we need to disable e.g. page flipping on a sw cursor
    // as long as our double pumping double pumps even on to of opengl surfaces
    if (!EngQueryDeviceAttribute(ppdev->hdevEng, QDA_ACCELERATION_LEVEL, NULL, 0, &ppdev->ulDriverCompatibilityMode, sizeof(ULONG)))
#endif
    {
        // on NT4 or if we failed we assume full acceleration
        ppdev->ulDriverCompatibilityMode = 0;
    }

    return(hsurf);

    //**************************************************************************
    // DrvEnableSurface Failed!
    //**************************************************************************

ReturnFailure:

    DrvDisableSurface((DHPDEV) ppdev);
    ppdev->bEnabled = FALSE;

    DISPDBG((0, "Failed DrvEnableSurface"));

    return(0);
    }

//******************************************************************************
//
//  Function:   DrvDisableSurface
//
//  Routine Description:
//
//     Free resources allocated by DrvEnableSurface.  Release the surface.
//
//     Note that this function will be called when previewing modes in the
//     Display Applet, but not at system shutdown.  If you need to reset the
//     hardware at shutdown, you can do it in the miniport by providing a
//     'HwResetHw' entry point in the VIDEO_HW_INITIALIZATION_DATA structure.
//
//     Note: In an error case, we may call this before DrvEnableSurface is
//           completely done.
//
//  Arguments:
//
//  Return Value:
//
//
//******************************************************************************


VOID DrvDisableSurface(
DHPDEV dhpdev)

    {
    PDEV*   ppdev;
    BOOLEAN bHwDisabled;

    ppdev = (PDEV*) dhpdev;

    //**************************************************************************
    // Note: In an error case, some of the following relies on the
    //       fact that the PDEV is zero-initialized, so fields like
    //       'hsurfScreen' will be zero unless the surface has been
    //       sucessfully initialized, and makes the assumption that
    //       EngDeleteSurface can take '0' as a parameter.
    //**************************************************************************

    //**************************************************************************
    // DisableDirectDraw -> frees up memory taken my Direct Draw)
    // DisablePalette    -> does nothing
    // DisableBrushCache -> does nothing (we don't put brushes in offscreen memory)
    // DisableText       -> does nothing (we don't do glyph caching)
    // DisablePointer    -> does nothing
    // DisableOffscreenHeap -> frees up system memory used for offscreen heap
    //                         management, as well as the wrapper 'punt' surface objects
    //JOEHACK - move punt surfobj creation/deletion from  E/Doffscreenheap to E/Dsurface and fix the comment
    // DisableBanking    -> does nothing (we don't do banking in NV)
    // DisableHardware   -> frees up memory used to map frame buffer , as well
    //                      as memory used to map miscellaneous memory mapped
    //                      register ranges
    //**************************************************************************
    if (HWGFXCAPS_QUADRO_GENERIC(ppdev))
    {
        // Free the linked list used to store DrvSaveScreenBits data
        bSaveScreenBitsGarbageCollection(ppdev, 0); // remove all elements we've collected (GDI bug)
        bLinkedListDestroy(&(ppdev->pLinkedListSaveScreenBits));
    }

    bOglServicesDisable(ppdev);

    vDisableDirectDraw(ppdev);
    vDisablePalette(ppdev);
    vDisableText(ppdev);
    vDisablePointer(ppdev);
    vDisableOffscreenHeap(ppdev);
    vDisableHardware(ppdev);
    if (ppdev->hsurfScreen != NULL)
        EngDeleteSurface(ppdev->hsurfScreen);

    }

//******************************************************************************
//
//  Function:   AssertModeEnable
//
//  Routine Description:
//
//      Called by DrvAssertMode(dhpdev, TRUE);
//
//  Arguments:
//
//  Return Value:
//
//
//******************************************************************************
BOOL AssertModeEnable(PDEV* ppdev)
{
    ULONG   ReturnedDataLength;

    //**************************************************************************
    // We are running a version of NT which is >= 4.0. This means we can safely
    // enable P6 write combining when mapping the framebuffer. The following IOCTL
    // sets a flag in the device state which tells the miniport to do just that.
    // This needs to be done for every physical NV device enabled by this driver.
    //**************************************************************************
    if (EngDeviceIoControl(ppdev->hDriver,
                           IOCTL_VIDEO_ENABLE_WRITE_COMBINING,
                           NULL,
                           0,
                           NULL,
                           0,
                           &ReturnedDataLength))
        {
        DISPDBG((1, "AssertModeEnable - IOCTL_VIDEO_ENABLE_WRITE_COMBINING failed"));
        }

    //**********************************************************************
    // Enable - Switch back to graphics mode
    //**********************************************************************

    //**********************************************************************
    // We have to enable every subcomponent in the reverse order
    // in which it was disabled:
    //**********************************************************************


    if (bAssertModeHardware(ppdev, TRUE))
    {
#if (_WIN32_WINNT >= 0x0500) && !defined(NV3)
        if (ppdev->bAgp) {
            ppdev->pDriverData->GARTPhysicalBase = ppdev->AgpHeap->lpHeap->fpGARTDev;
            ppdev->pDriverData->GARTLinearHeapOffset = ppdev->AgpHeap->lpHeap->fpGARTLin;
        }
#endif

        //******************************************************************
        // This really doesn't do anything, since BitBlt already
        // attempts to put DFB DIBs back into offscreen VRAM if possible
        //******************************************************************
        bAssertModeOffscreenHeap(ppdev, TRUE);  // We don't need to check
                                                //   return code with TRUE
        //******************************************************************
        // Init heap for direct draw if it was previously active.
        //******************************************************************
        if (ppdev->flStatus & STAT_DIRECTDRAW_ENABLED)
            if (!bAssertModeDirectDraw(ppdev, TRUE))
                return(FALSE);


        //******************************************************************
        // Re-enable the cursor
        //******************************************************************

        vAssertModePointer(ppdev, TRUE);

        if (!bOglAssertMode(ppdev, TRUE))
            return(FALSE);

        ppdev->bEnabled = TRUE;
        ppdev->dsurfScreen.LinearPtr = ppdev->pjScreen;

        return(TRUE);
    }
    return(FALSE);
}

//******************************************************************************
//
//  Function:   AssertModeDisable
//
//  Routine Description:
//
//      Called by DrvAssertMode(dhpdev, TRUE);
//
//  Arguments:
//
//  Return Value:
//
//
//******************************************************************************
BOOL AssertModeDisable(PDEV* ppdev)
{
    BOOL  bReleaseOpenGLMutex = FALSE;
    ULONG ulRet;

    //**************************************************************************
    // If OpenGL is enabled, then wait for current push buffer to complete.
    //**************************************************************************

    //  Indicate that we are currently disabled, and should not call
    //  accelerated codes.

    ppdev->bEnabled = FALSE;

    if (!bOglAssertMode(ppdev, FALSE))
        return(FALSE);


    if (OglIsEnabled(ppdev))
    {
        ppdev->pfnAcquireOglMutex(ppdev);
        ppdev->pfnWaitForChannelSwitch(ppdev);
        ppdev->NVFreeCount = 0;
        bReleaseOpenGLMutex = TRUE;
    }

    //**********************************************************************
    // Disable - Switch to full-screen mode
    //**********************************************************************
    if(ppdev->pjScreen != NULL)
    {
        ULONG ulBytesToClear,ul;

        // clear visible screen
        ulBytesToClear = ppdev->lDelta * ppdev->cyScreen;

        for(ul = 0; ul < (ulBytesToClear / sizeof(ULONG)); ul++)
        {
            ((PULONG) ppdev->pjScreen)[ul] = 0;
        }
    }


    //**********************************************************************
    // NT4 - Disable directdraw offscreen heap if enabled.
    // NT5 - Destroy DX6 objects.
    //**********************************************************************
    if (!bAssertModeDirectDraw(ppdev, FALSE))
    {
        if (bReleaseOpenGLMutex == TRUE)
        {
            ppdev->pfnReleaseOglMutex(ppdev);
        }
        // Don't return so soon; let clean up other stuff.
        //return(FALSE);
    }

    //**********************************************************************
    // AssertModePalette doesn't do anything for NV.
    // We'll just leave it as a placeholder
    //**********************************************************************

    vAssertModePalette(ppdev, FALSE);

    //**********************************************************************
    // AssertModeBrushCache doesn't do anything for NV,
    // since we don't implement brushes in offscreen vram.
    //**********************************************************************

    //**********************************************************************
    // AssertModeBrushCache doesn't do anything for NV,
    // since we don't do glyph caching.
    // We'll just leave it as a placeholder
    //**********************************************************************

    vAssertModeText(ppdev, FALSE);

    //**********************************************************************
    // Disable/hide the cursor
    //**********************************************************************

    vAssertModePointer(ppdev, FALSE);

    //**********************************************************************
    // Make sure to move offscreen DFB's to system memory
    //**********************************************************************
    if (bAssertModeOffscreenHeap(ppdev, FALSE))
    {
    //******************************************************************
    // Now do the actual mode switch to full-screen
    //******************************************************************

        if (bAssertModeHardware(ppdev, FALSE))
        {
            if (bReleaseOpenGLMutex == TRUE)
            {
                ppdev->pfnReleaseOglMutex(ppdev);
                if (ppdev->oglMutexPtr) *(ppdev->oglMutexPtr) = 0;
            }

            if (EngDeviceIoControl(ppdev->hDriver,
                               IOCTL_VIDEO_SET_UNIQUENESS_VGA,
                               &ppdev->iUniqueness,
                               sizeof(ULONG),
                               NULL,
                               0,
                               &ulRet))
            {
                DISPDBG((1, "AssertModeDisable - Failed SET_UNIQUENESS_VGA IOCTL"));
            }


            return(TRUE);
        }
    }                                       //   return code with TRUE

    //**********************************************************************
    // Re-enable the cursor
    //**********************************************************************

    vAssertModePointer(ppdev, TRUE);

    if (bReleaseOpenGLMutex == TRUE)
    {
        ppdev->pfnReleaseOglMutex(ppdev);
        if (ppdev->oglMutexPtr) *(ppdev->oglMutexPtr) = 0;
    }

    return(FALSE);
}

//******************************************************************************
//
//  Function:   vDisableAcceleration
//
//  Routine Description:
//
//      
//
//  Arguments:
//
//  Return Value:
//
//
//******************************************************************************
VOID vDisableAcceleration(PDEV* ppdev)
{
    ULONG ulReturnedDataLength;

    // Reset the chip
    if (EngDeviceIoControl(ppdev->hDriver,
                       IOCTL_VIDEO_RESET_DEVICE,
                       NULL,
                       0,
                       NULL,
                       0,
                       &ulReturnedDataLength))
    {
        DISPDBG((1, "bAssertModeHardware - Failed reset IOCTL"));
    }

    // setup new mode by call miniport throught RmSetMode because
    // display driver has trouble to create/initialize the HW objects.
    if (EngDeviceIoControl(ppdev->hDriver,
                       IOCTL_VIDEO_SET_CURRENT_MODE,
                       &ppdev->ulMode,  // input buffer
                       sizeof(DWORD),
                       &ppdev->bHwVidOvl,
                       sizeof(BOOLEAN),
                       &ulReturnedDataLength))
    {
        DISPDBG((1, "bAssertModeHardware - Failed VIDEO_SET_CURRENT_MODE"));
    }

    // Disable all accelerations
    ppdev->bEnabled = FALSE; 
}

//******************************************************************************
//
//  Function:   DrvAssertMode
//
//  Routine Description:
//
//      This asks the device to reset itself to the mode of the pdev passed in.
//
//  Arguments:
//
//  Return Value:
//
//
//******************************************************************************
BOOL DrvAssertMode(
DHPDEV  dhpdev,
BOOL    bEnable)
{
    PDEV* ppdev;
    TOSHIBA_HOTKEY_MODE ToshibaHotKeyMode;

    ppdev = (PDEV*) dhpdev;

    if (bEnable)
    {
        // Indicate we are in DrvAssertMode(TRUE) so that we can obey the BOOT_DEVICEs for laptops.
        ppdev->ulDrvAssertModeTRUE = 1;
        // if we are already enabled, do nothing.
        if (ppdev->bEnabled)
        {
            DISPDBG((1, "DrvAssertMode(TRUE):Already enabled. Returning"));
            return(TRUE);
        }
    }
    else
    {
        // Indicate we are not in DrvAssertMode(TRUE)
        ppdev->ulDrvAssertModeTRUE = 0;
        if (!ppdev->bEnabled)
        {
            DISPDBG((1, "DrvAssertMode(FALSE): Already disabled. Returning"));
            return(TRUE);
        }
    }

#if _WIN32_WINNT >= 0x0500

    if (bEnable)
    {
        // Read in the Toshiba Hotkey mode. This is only for win2K or later.
        ToshibaHotKeyMode.ValidMode = 0;
        ReadToshibaHotKeyMode(ppdev->hDriver, &ToshibaHotKeyMode);
        if (ToshibaHotKeyMode.ValidMode == 1)
        {
            // We are here because of a hotkey switch. However, GDI is keeping the same mode so no need to
            // force GDI into the same mode.
            // Now turn off the hotkey mode.
            ToshibaHotKeyMode.ValidMode = 0;
            WriteToshibaHotKeyMode(ppdev->hDriver, &ToshibaHotKeyMode);
        }

        if(!AssertModeEnable(ppdev))
        {

            // Has trouble to assert mode
            // Let's disable them if any of them enabled


            if(!AssertModeDisable(ppdev))
            {
                //  If failed, exit immediately to avoid risk of access violation later
                //  in the code.

                // Free up any memory alloced by display driver for multimon purposes.
                FreeMemoryMultiMon(ppdev);
                return(FALSE);
            }

            // Disable acceleration
            vDisableAcceleration(ppdev);
            DISPDBG((0, "DrvAssertMode - all 2D accelerations are disable."));

        }
    }
    else
    {
        AssertModeDisable(ppdev);
        // Free up any memory alloced by display driver for multimon purposes.
        FreeMemoryMultiMon(ppdev);
    }
    // Always return TRUE to make OS kernal happy.
    return(TRUE);

#else

    if (bEnable)
    {
        if(AssertModeEnable(ppdev))
            return(TRUE);
    }
    else
    {
        if(AssertModeDisable(ppdev))
        {
            // Free up any memory alloced by display driver for multimon purposes.
            FreeMemoryMultiMon(ppdev);
            return(TRUE);
        }
    }
    DISPDBG((0, "DrvAssertMode - failed (bEnable = %d)", bEnable));
    // Free up any memory alloced by display driver for multimon purposes.
    FreeMemoryMultiMon(ppdev);
    return(FALSE);

#endif

}

//******************************************************************************
//
//  Function:   DrvGetModes
//
//  Routine Description:
//
//      Returns the list of available modes for the device.
//
//  Arguments:
//
//  Return Value:
//
//
//******************************************************************************

ULONG DrvGetModes(
HANDLE      hDriver,
ULONG       cjSize,
DEVMODEW*   pdm)

    {
    DWORD cModes;
    DWORD cbOutputSize;
    PVIDEO_MODE_INFORMATION pVideoModeInformation;
    PVIDEO_MODE_INFORMATION pVideoTemp;
    DWORD cOutputModes = cjSize / (sizeof(DEVMODEW) + DRIVER_EXTRA_SIZE);
    DWORD cbModeSize;
    DWORD cNumGDIModes;

    DISPDBG((6,"DrvGetModes() Enter\n"));

    //**************************************************************************
    // getAvailableModes mainly does 2 things
    //      1) Gets the TOTAL number of modes supported by the miniport
    //      2) Returns a pointer to the array of modes. The modes
    //         whose length is set to 0 are not valid for the display driver
    //**************************************************************************

    cModes = getAvailableModes(hDriver,
                            (PVIDEO_MODE_INFORMATION *) &pVideoModeInformation,
                            &cbModeSize, &cNumGDIModes);

    //**************************************************************************
    // If this fails, NT will go back to VGA
    //**************************************************************************

    if (cModes == 0 || cNumGDIModes == 0)
        {
        DISPDBG((1, "DrvGetModes failed to get mode information"));
        return(0);
        }


    //**************************************************************************
    // pdm points to the buffer where we'll store the valid modes
    // (DEVMODEW structures).  If pdm == NULL, we need to return
    // the number of bytes required to hold all the mode data.
    //**************************************************************************

    if (pdm == NULL)
        {
        cbOutputSize = cNumGDIModes * (sizeof(DEVMODEW) + DRIVER_EXTRA_SIZE);
        }

    else

        {
        //**********************************************************************
        // Now copy the information for the supported modes back into the
        // output buffer
        //**********************************************************************

        cbOutputSize = 0;

        pVideoTemp = pVideoModeInformation;

        do  {

            //******************************************************************
            // Now copy only the modes which this display driver supports
            //******************************************************************

            if (pVideoTemp->Length != 0)
                {

                //**************************************************************
                // Make sure we don't go past the end of the mode
                // buffer supplied to us
                //**************************************************************

                if (cOutputModes == 0)
                    {
                    break;
                    }

                //**************************************************************
                // Zero the entire structure to start off with.
                //**************************************************************

                memset(pdm, 0, sizeof(DEVMODEW));

                //**************************************************************
                // Set the name of the device to the name of the DLL.
                //**************************************************************

                DISPDBG((6, "exporting: (%d, %d) at %d bpp, %d HZ\n",
                    pVideoTemp->VisScreenWidth,pVideoTemp->VisScreenHeight,
                    pVideoTemp->BitsPerPlane,pVideoTemp->Frequency));

                memcpy(pdm->dmDeviceName, DLL_NAME, sizeof(DLL_NAME));

                pdm->dmSpecVersion      = DM_SPECVERSION;
                pdm->dmDriverVersion    = DM_SPECVERSION;
                pdm->dmSize             = sizeof(DEVMODEW);
                pdm->dmDriverExtra      = DRIVER_EXTRA_SIZE;

                pdm->dmBitsPerPel       = pVideoTemp->NumberOfPlanes *
                                          pVideoTemp->BitsPerPlane;
                pdm->dmPelsWidth        = pVideoTemp->VisScreenWidth;
                pdm->dmPelsHeight       = pVideoTemp->VisScreenHeight;
                pdm->dmDisplayFrequency = pVideoTemp->Frequency;
                pdm->dmDisplayFlags     = 0;

                pdm->dmFields           = DM_BITSPERPEL       |
                                          DM_PELSWIDTH        |
                                          DM_PELSHEIGHT       |
                                          DM_DISPLAYFREQUENCY |
                                          DM_DISPLAYFLAGS     ;

                //**************************************************************
                // Go to the next DEVMODE entry in the buffer.
                // cOutputModes represents the number of mode entries
                // left in the buffer supplied to us.
                //**************************************************************

                cOutputModes--;

                pdm = (LPDEVMODEW) ( ((BYTE *)pdm) + sizeof(DEVMODEW) +
                                                   DRIVER_EXTRA_SIZE);

                cbOutputSize += (sizeof(DEVMODEW) + DRIVER_EXTRA_SIZE);

                }

            //******************************************************************
            // Check next miniport mode
            //******************************************************************

            pVideoTemp = (PVIDEO_MODE_INFORMATION)
                (((PUCHAR)pVideoTemp) + cbModeSize);


            } while (--cModes);

        }

    //**************************************************************************
    // MAKE sure to free up this memory, which was allocated in
    // getAvailableModes()
    //**************************************************************************

    EngFreeMem(pVideoModeInformation);
    DISPDBG((1,"Returning number of modes: %d\n",(cbOutputSize / (sizeof(DEVMODEW) + DRIVER_EXTRA_SIZE))));

    return(cbOutputSize);
    }

//******************************************************************************
//


//  Function:   bInitializeModeFields
//
//  Routine Description:
//
//      Initializes a bunch of fields in the pdev, devcaps (aka gdiinfo), and
//      devinfo based on the requested mode.
//
//  Arguments:
//
//  Return Value:
//
//
//******************************************************************************


BOOL bInitializeModeFields(
PDEV*     ppdev,
GDIINFO*  pgdi,
DEVINFO*  pdi,
DEVMODEW* pdm)

    {
    ULONG                   cModes;
    PVIDEO_MODE_INFORMATION pVideoBuffer;
    PVIDEO_MODE_INFORMATION pVideoModeSelected;
    PVIDEO_MODE_INFORMATION pVideoTemp;
    BOOL                    bSelectDefault;
    VIDEO_MODE_INFORMATION  VideoModeInformation;
    ULONG                   cbModeSize;
    ULONG i;
    TOSHIBA_HOTKEY_MODE     ToshibaHotKeyMode;
    DWORD cNumGDIModes;

    //**************************************************************************
    // Call the miniport to get mode information
    //**************************************************************************

    cModes = getAvailableModes(ppdev->hDriver, &pVideoBuffer, &cbModeSize, &cNumGDIModes);
    if (cModes == 0)
        goto bInitializeModeFields_ReturnFalse;

#if _WIN32_WINNT >= 0x0500
    // Read in the Toshiba Hotkey mode.
    ToshibaHotKeyMode.ValidMode = 0;
    ReadToshibaHotKeyMode(ppdev->hDriver, &ToshibaHotKeyMode);
    if (ToshibaHotKeyMode.ValidMode == 1)
    {
        // We are here because of a hotkey switch. So overwrite the 'pdm'. This will automatically percolate the desired mode
        // to 'pgdi'.
        pdm->dmPelsWidth = ToshibaHotKeyMode.Width;
        pdm->dmPelsHeight = ToshibaHotKeyMode.Height;
        pdm->dmBitsPerPel = ToshibaHotKeyMode.Depth;
        pdm->dmDisplayFrequency = ToshibaHotKeyMode.RefreshRate;

        // Now turn off the hotkey mode.
        ToshibaHotKeyMode.ValidMode = 0;
        WriteToshibaHotKeyMode(ppdev->hDriver, &ToshibaHotKeyMode);
    }

#endif _WIN32_WINNT >= 0x0500

    //****************************************************************************************
    // Now check if we were asked to force a mode: if we started this 
    // one-time-after-reboot-forced-mode sequence, but we haven't rebooted yet,
    // we just clear the flag that indicate the start of this sequence. This means
    // the user switched mode before rebooting.  Else, we do the mode overide if the
    // sequence is at the "end" (only the mode overide is missing)
    // This sequence goes like :  
    //                            hwinit()        -> ForceModeNextBoot := 1 (if we want to force this sequence)
    //                            reboot
    //                            hwinit()        -> ForceModeNextBoot := 0, ReBootedNowForceMode.ValidMode := 1
    //                                                 ( if ForceModeNextBoot == 1 )
    //
    //                            drvenablepdev() -> + ReBootedNowForceMode.ValidMode := 0
    //                                               | Overide mode
    //                                               |
    //                                               +--> this is only done if ForceModeNextBoot == 0, and 
    //                                                    ReBootedNowForceMode.ValidMode == 1
    //
    //                                               Else, if ForceModeNextBoot == 1, we halt the sequence here
    //                                               because we're assuming the user sets a mode and that
    //                                               overides our force sequence.
    //
    // SEE \nv_mini\nv.c: CheckForcedResolution()
    //
    // (The next comments are pulled from that:)
    //
    // In DrvEnablePdev(): ***!ASSUMING it is called ONLY on modeset!*** 
    //
    //
    //    1) read the ForceModeNextBoot flag: 
    //
    //                     if it is set ->        clear it ! (the user changed the mode but we haven't rebooted
    //                                            yet because otherwise, on reboot, in this very function,
    //                                            we would have cleared it);   ReBootedNowForceMode.ValidMode is 
    //                                            probably = 0, else the state is inconsistant.
    //                     else 
    //                         -> 
    //                     read the ReBootedNowForceMode.ValidMode flag:  
    //            
    //                         if it is set -> clear it and overide the GDI mode with
    //                                         ReBootedNowForceMode;
    //                         else 
    //                             -> nothing special;
    //
    //  was already induced, we already have a mode to set, from a previous call to this function)
    //
    //****************************************************************************************

    {
        ULONG           ForceModeNextBoot;

        
        ForceModeNextBoot = 0;

        GetRegValueW( ppdev->hDriver, L"ForceModeNextBoot", (PVOID) &ForceModeNextBoot, sizeof(ULONG));

        if( ForceModeNextBoot == 1)
        {
            // Force mode sequence started but we haven't rebooted and the user switch mode, so 
            // overide the sequence

            ForceModeNextBoot = 0;
            SetRegValueW( ppdev->hDriver, L"ForceModeNextBoot", (PVOID) &ForceModeNextBoot, sizeof(ULONG));
        }
        else
        {
            // Check to see if a force mode sequence needs to be completed
            
            MODE_ENTRY      ModeEntry;
        
            GetRegValueW( ppdev->hDriver, L"ReBootedNowForceMode", (PVOID) &ModeEntry, sizeof(MODE_ENTRY));

            if( ModeEntry.ValidMode == 1 )
            {
                // Clear the flag, and overide the mode asked by GDI

                ModeEntry.ValidMode = 0;
                
                // Overwrite the 'pdm' field (DEVMODEW).
                
                pdm->dmPelsWidth  = ModeEntry.Width;
                pdm->dmPelsHeight = ModeEntry.Height;
                pdm->dmBitsPerPel = ModeEntry.Depth;
                pdm->dmDisplayFrequency = ModeEntry.RefreshRate;

                SetRegValueW( ppdev->hDriver, L"ReBootedNowForceMode", (PVOID) &ModeEntry, sizeof(MODE_ENTRY));    
            
            } // ... if validmode (then we overide the mode asked by GDI with the mode in the key)

        }// ...else forcemodenextboot = 0;  (need to check for force mode sequence)
    }

    //*** ...check for a forced mode we wanted to set ****************************************


    //**************************************************************************
    // Now see if the requested mode has a match in that table.
    //**************************************************************************

    pVideoModeSelected = NULL;
    pVideoTemp = pVideoBuffer;

    if ((pdm->dmPelsWidth        == 0) &&
        (pdm->dmPelsHeight       == 0) &&
        (pdm->dmBitsPerPel       == 0) &&
        (pdm->dmDisplayFrequency == 0))
        {
        DISPDBG((1, "Default mode requested"));
        bSelectDefault = TRUE;
        }
    else
        {
        DISPDBG((1, "Requested mode..."));
        DISPDBG((1, "   Screen width  -- %li", pdm->dmPelsWidth));
        DISPDBG((1, "   Screen height -- %li", pdm->dmPelsHeight));
        DISPDBG((1, "   Bits per pel  -- %li", pdm->dmBitsPerPel));
        DISPDBG((1, "   Frequency     -- %li", pdm->dmDisplayFrequency));

        bSelectDefault = FALSE;
        }

    //**************************************************************************
    // Check the requested mode against the miniport modes until we find a match
    //**************************************************************************

    while (cModes--)
        {
        if (pVideoTemp->Length != 0)
            {
            DISPDBG((8, "   Checking against miniport mode:"));
            DISPDBG((8, "      Screen width  -- %li", pVideoTemp->VisScreenWidth));
            DISPDBG((8, "      Screen height -- %li", pVideoTemp->VisScreenHeight));
            DISPDBG((8, "      Bits per pel  -- %li", pVideoTemp->BitsPerPlane *
                                                      pVideoTemp->NumberOfPlanes));
            DISPDBG((8, "      Frequency     -- %li", pVideoTemp->Frequency));

            //******************************************************************
            // If the default mode is requested, use the first mode entry
            //******************************************************************

            if (bSelectDefault ||
                ((pVideoTemp->VisScreenWidth  == pdm->dmPelsWidth) &&
                 (pVideoTemp->VisScreenHeight == pdm->dmPelsHeight) &&
                 (pVideoTemp->BitsPerPlane *
                  pVideoTemp->NumberOfPlanes  == pdm->dmBitsPerPel) &&
                 (pVideoTemp->Frequency       == pdm->dmDisplayFrequency)))
                {
                pVideoModeSelected = pVideoTemp;
                DISPDBG((1, "...Found a mode match!"));
                break;
                }
            }


        //**********************************************************************
        // Check next miniport mode
        //**********************************************************************

        pVideoTemp = (PVIDEO_MODE_INFORMATION)
            (((PUCHAR)pVideoTemp) + cbModeSize);

        }

    //**************************************************************************
    // If no mode has been found, return an error
    //**************************************************************************

    if (pVideoModeSelected == NULL)
        {
        DISPDBG((1, "...Couldn't find a mode match!"));
        EngFreeMem(pVideoBuffer);
        goto bInitializeModeFields_ReturnFalse;
        }

    //**************************************************************************
    // We have chosen the one we want.  Save it in a stack buffer and
    // get rid of allocated memory before we forget to free it.
    //**************************************************************************

    VideoModeInformation = *pVideoModeSelected;
    EngFreeMem(pVideoBuffer);

    #if DEBUG_HEAP
        VideoModeInformation.VisScreenWidth  = 640;
        VideoModeInformation.VisScreenHeight = 480;
    #endif

    //**************************************************************************
    // Set up screen information from the mini-port:
    //**************************************************************************

    ppdev->ulMode           = VideoModeInformation.ModeIndex;
    ppdev->cxScreen         = VideoModeInformation.VisScreenWidth;
    ppdev->cyScreen         = VideoModeInformation.VisScreenHeight;
    ppdev->cBitsPerPel      = VideoModeInformation.BitsPerPlane;
    ppdev->ulGDIRefreshRate      = VideoModeInformation.Frequency;

    // For Twinview purposes
    // GDI tells us the refresh rate for the first head. nVidia panel tells us the refresh rate for the second head.
    ppdev->ulRefreshRate[0]      = VideoModeInformation.Frequency;

    DISPDBG((1, "ScreenStride: %lx", VideoModeInformation.ScreenStride));

    //**************************************************************************
    // We'll slowly add accelerations, starting with BITBLT
    //**************************************************************************
    ppdev->flHooks = (HOOK_SYNCHRONIZEACCESS |
                      HOOK_SYNCHRONIZE       |
                      HOOK_BITBLT            |
                      HOOK_COPYBITS          |
                      HOOK_TEXTOUT           |
                      HOOK_PAINT             |
                      HOOK_STROKEPATH        |
//                      HOOK_FILLPATH          |
                      HOOK_LINETO 
#if _WIN32_WINNT >= 0x0500 
#ifndef NV3
                      | HOOK_ALPHABLEND
                      | HOOK_TRANSPARENTBLT
#endif
#endif                      
                      );


    //**************************************************************
    // It's a device-managed surface; make sure we don't set
    // HOOK_SYNCHRONIZE, otherwise we may confuse GDI:
    //**************************************************************
    ppdev->flHooksDeviceBitmap = ppdev->flHooks & ~HOOK_SYNCHRONIZE;

    //**************************************************************
    // Setting the SYNCHRONIZEACCESS flag tells GDI that we
    // want all drawing to the bitmaps to be synchronized (GDI
    // is multi-threaded and by default does not synchronize
    // device bitmap drawing -- it would be a Bad Thing for us
    // to have multiple threads using the accelerator at the
    // same time):
    //**************************************************************

    ppdev->flHooksDeviceBitmap  |= HOOK_SYNCHRONIZEACCESS;


    //**************************************************************************
    // We currently don't implement the following functions
    //
    //          HOOK_STRETCHBLT         -> Not that important speed wise?
    //                                     Current version doesn't handle DFB's
    //          HOOK_FILLPATH           -> Not implemented
    //          HOOK_STROKEANDFILLPATH  -> Not implemented
    //
    //**************************************************************************

    //**************************************************************************
    // Make sure that Clipping rectangle gets reset by the first hardware function
    // By default, the clipping rectangle should include ALL of video memory
    // (including offscreen). Whenever we change it, we need to set this flag.
    // So, by setting it here, the first function we get to will reset
    // the clipping rectangle.
    //**************************************************************************

    ppdev->NVClipResetFlag=1;

    //**************************************************************************
    // Used to test successful return of functions without doing anything
    // The following variables are just used for debugging and profiling
    //**************************************************************************

    ppdev->NVAbsoluteLimitFlag      = FALSE;
    ppdev->NVStubTest               = FALSE;
    ppdev->NVRecordFunctionsFlag    = 0;
    for (i=0;i<70;i++)
        ppdev->NVLocation[i]=0;

    //**************************************************************************
    // Fill in the GDIINFO data structure with the default 8bpp values:
    // NOTE: This is a global variable/structure!  May need to
    //       change it if we implement multiple board functionality
    //**************************************************************************

    *pgdi = ggdiDefault;

    //**************************************************************************
    // Now overwrite the defaults with the relevant information returned
    // from the kernel driver:
    //**************************************************************************

    pgdi->ulHorzSize        = VideoModeInformation.XMillimeter;
    pgdi->ulVertSize        = VideoModeInformation.YMillimeter;

    pgdi->ulHorzRes         = VideoModeInformation.VisScreenWidth;
    pgdi->ulVertRes         = VideoModeInformation.VisScreenHeight;
    pgdi->ulPanningHorzRes  = VideoModeInformation.VisScreenWidth;
    pgdi->ulPanningVertRes  = VideoModeInformation.VisScreenHeight;

    pgdi->cBitsPixel        = VideoModeInformation.BitsPerPlane;
    pgdi->cPlanes           = VideoModeInformation.NumberOfPlanes;
    pgdi->ulVRefresh        = VideoModeInformation.Frequency;

    pgdi->ulDACRed          = VideoModeInformation.NumberRedBits;
    pgdi->ulDACGreen        = VideoModeInformation.NumberGreenBits;
    pgdi->ulDACBlue         = VideoModeInformation.NumberBlueBits;

    pgdi->ulLogPixelsX      = pdm->dmLogPixels;
    pgdi->ulLogPixelsY      = pdm->dmLogPixels;

    //
    // The analog clock on the lower right end of the task bar needs to be circular shaped always.
    // So adjust the millimeter size of the width or height accordingly for spanning modes.
    //
    if (HORIZONTAL_MODE(VideoModeInformation.VisScreenWidth, VideoModeInformation.VisScreenHeight))
    {
        //
        // Double the horizontal size
        //
        pgdi->ulHorzSize *= 2;

    }
    else
    if (VERTICAL_MODE(VideoModeInformation.VisScreenWidth, VideoModeInformation.VisScreenHeight))
    {
        //
        // Double the vertical size.
        //
        pgdi->ulVertSize *= 2;
    }

    //**************************************************************************
    // Fill in the devinfo structure with the default 8bpp values:
    //**************************************************************************

    *pdi = gdevinfoDefault;

    //**************************************************************************
    // Initialize the ppdev field values according to the bitdepth
    //**************************************************************************

    if (VideoModeInformation.BitsPerPlane == 8)

        {
        ppdev->cjPelSize       = 1;
        ppdev->iBitmapFormat   = BMF_8BPP;
        ppdev->ulWhite         = 0xff;
        ppdev->physicalColorMask = 0xff;

        //**********************************************************************
        // Assuming palette is orthogonal - all colors are same size.
        //**********************************************************************

        ppdev->cPaletteShift   = 8 - pgdi->ulDACRed;
        pdi->flGraphicsCaps   |= (GCAPS_PALMANAGED | GCAPS_COLOR_DITHER);
        DISPDBG((3, "palette shift = %d\n", ppdev->cPaletteShift));
        }

    else if ((VideoModeInformation.BitsPerPlane == 16) ||
             (VideoModeInformation.BitsPerPlane == 15))
        {
        ppdev->cjPelSize       = 2;
        ppdev->iBitmapFormat   = BMF_16BPP;
        ppdev->ulWhite         = 0xffff;
        ppdev->flRed           = VideoModeInformation.RedMask;
        ppdev->flGreen         = VideoModeInformation.GreenMask;
        ppdev->flBlue          = VideoModeInformation.BlueMask;
        ppdev->physicalColorMask = ppdev->flRed | ppdev->flGreen | ppdev->flBlue;

        pgdi->ulNumColors      = (ULONG) -1;
        pgdi->ulNumPalReg      = 0;
        pgdi->ulHTOutputFormat = HT_FORMAT_16BPP;

        pdi->iDitherFormat     = BMF_16BPP;
        pdi->flGraphicsCaps   &= ~(GCAPS_PALMANAGED | GCAPS_COLOR_DITHER);

        #if _WIN32_WINNT >= 0x0500

        //
        //  Allow system to call DrvIcmSetDeviceGammaRamp
        //

        pdi->flGraphicsCaps2 |= GCAPS2_CHANGEGAMMARAMP;
        pdi->flGraphicsCaps2 |= GCAPS2_ALPHACURSOR;

        pdi->flGraphicsCaps2 |= GCAPS2_ICD_MULTIMON;

        #endif

        }

    else

        {
        ASSERTDD(VideoModeInformation.BitsPerPlane == 32,
         "This driver supports only 8, 16, and 32bpp");

        ppdev->cjPelSize       = 4;
        ppdev->iBitmapFormat   = BMF_32BPP;
        ppdev->ulWhite         = 0xffffffff;
        ppdev->flRed           = VideoModeInformation.RedMask;
        ppdev->flGreen         = VideoModeInformation.GreenMask;
        ppdev->flBlue          = VideoModeInformation.BlueMask;
        ppdev->physicalColorMask = ppdev->flRed | ppdev->flGreen | ppdev->flBlue;

        pgdi->ulNumColors      = (ULONG) -1;
        pgdi->ulNumPalReg      = 0;
        pgdi->ulHTOutputFormat = HT_FORMAT_32BPP;

        pdi->iDitherFormat     = BMF_32BPP;
        pdi->flGraphicsCaps   &= ~(GCAPS_PALMANAGED | GCAPS_COLOR_DITHER);

        #if _WIN32_WINNT >= 0x0500

        //
        //  Allow system to call DrvIcmSetDeviceGammaRamp
        //

        pdi->flGraphicsCaps2 |= GCAPS2_CHANGEGAMMARAMP;
        pdi->flGraphicsCaps2 |= GCAPS2_ALPHACURSOR;

        pdi->flGraphicsCaps2 |= GCAPS2_ICD_MULTIMON;

        #endif
        }

    DISPDBG((5, "Passed bInitializeModeFields"));

    return(TRUE);

bInitializeModeFields_ReturnFalse:

    DISPDBG((1, "Failed bInitializeModeFields"));

    return(FALSE);
    }

//******************************************************************************
//
//  Function:   getAvailableModes
//
//  Routine Description:
//
//     Calls the miniport to get the master list of modes in the registry.
//     Returns the number of entries in the videomode buffer 'modeInformation'.
//     0 means no modes are supported by the miniport or that an error occured.
//     The number of modes returned is the total number of modes in the master mode list
//     read from the registry and returned via 'modeInformation'.
//     The actual number of modes supported in the current
//     desktop (standard/clone/span modes and EDID based pruning) which needs to be
//     exported to the GDI via DrvgetModes() is returned in the out parameter 'pcNumGDIModes'.
//     The validated GDI modes in the 'modeINformation' list can be identified by the 'Length'
//     field being non-zero.
//
//     NOTE: the buffer must be freed up by the caller.
//
//  Arguments:
//
//  Return Value:
//
//
//
//******************************************************************************


DWORD getAvailableModes(
HANDLE                   hDriver,
PVIDEO_MODE_INFORMATION* modeInformation,       // Must be freed by caller
DWORD*                   cbModeSize,
DWORD*                   pcNumGDIModes)

{
    ULONG                   ulTemp, ulRet, ulRetSize;
    VIDEO_NUM_MODES         modes;
    PVIDEO_MODE_INFORMATION pVideoTemp;
    NVTWINVIEWDATA sData;
    ULONG   TwinView_State, TwinView_Orientation;
    ULONG   ReturnedDataLength, NumDacsOnBoard;
    GET_NUMBER_DACS   NumberDacs;
    ULONG   PrimaryHead;
    ULONG   PrimaryDeviceMask;
    ULONG   ulDeviceDisplay[NV_NO_DACS];
    ULONG TwinViewRegistryDataOK = 0;
    MODE_ENTRY RequestedMode, BestFitMode;
    ULONG MaxCommonWidth, MaxCommonHeight;
    ULONG i, j;
    ULONG PhysWidth, PhysHeight, ulHead;
    MODE_ENTRY *PhysModeList[NV_NO_DACS];
    ULONG NumPhysModes[NV_NO_DACS];
    MODE_ENTRY *SingleVirtualModeList;
    ULONG NumSingleVirtualModes;
    MODE_ENTRY *PhysModePtr;
    MODE_ENTRY *DestPtr;
    BOOLEAN bFoundPhysMode;
    ULONG MustHaveResolution;
    DWORD cNumGDIModes = 0;

    *pcNumGDIModes = 0;    
    //**************************************************************************
    // Get the number of modes supported by the mini-port
    //**************************************************************************

    if (EngDeviceIoControl(hDriver,
                           IOCTL_VIDEO_QUERY_NUM_AVAIL_MODES,
                           NULL,
                           0,
                           &modes,
                           sizeof(VIDEO_NUM_MODES),
                           &ulTemp))
    {
        DISPDBG((1, "getAvailableModes - Failed VIDEO_QUERY_NUM_AVAIL_MODES"));
        return(0);
    }

    *cbModeSize = modes.ModeInformationLength;

    //**************************************************************************
    // Allocate the buffer for the mini-port to write the modes in.
    //**************************************************************************

    *modeInformation = EngAllocMem(FL_ZERO_MEMORY,
                                   modes.NumModes * modes.ModeInformationLength,
                                   ALLOC_TAG);

    if (*modeInformation == (PVIDEO_MODE_INFORMATION) NULL)
    {
        DISPDBG((0, "getAvailableModes - Failed EngAllocMem"));
        return 0;
    }


    //**************************************************************************
    // See if we are in clone or normal or multimon mode on a dual head system.
    // if we are on a single head system, then we always assume a normal mode.
    //**************************************************************************
    NumberDacs.ulNumberDacsOnBoard = 1;
    NumberDacs.ulNumberDacsConnected = 1;
    if (EngDeviceIoControl(hDriver,
                           IOCTL_VIDEO_GET_NUMBER_DACS,
                           NULL,
                           0,
                           &NumberDacs,
                           sizeof(NumberDacs),
                           &ReturnedDataLength))
    {
        DISPDBG((0, "getAvailableModes() - IOCTL_VIDEO_GET_NUMBER_DACS failed"));
    }
    else
    {
        DISPDBG((6, "Got NumDacsOnBoard: 0x%x, NumDacsConnected: 0x%x\n",NumberDacs.ulNumberDacsOnBoard,
                 NumberDacs.ulNumberDacsConnected ));
    }
    if (NumberDacs.ulNumberDacsOnBoard == 0 || NumberDacs.ulNumberDacsOnBoard > 2)
    {
        DISPDBG((0, "Oops! Invalid NumberDacs.ulNumberDacsOnBoard: 0x%x, assuming single headed board\n",NumberDacs.ulNumberDacsOnBoard));
        NumberDacs.ulNumberDacsOnBoard = 1;
    }
    if (NumberDacs.ulNumberDacsConnected == 0 || NumberDacs.ulNumberDacsConnected > 2)
    {
        DISPDBG((0, "Oops! Invalid NumberDacs.ulNumberDacsConnected: 0x%x, assuming single headed board\n",NumberDacs.ulNumberDacsConnected));
        NumberDacs.ulNumberDacsConnected = 1;
    }

    //
    // Assume a safe default state
    //
    TwinView_State = NVTWINVIEW_STATE_NORMAL;

    PrimaryHead = NumberDacs.ulDeviceDisplay[0];
    PrimaryDeviceMask = NumberDacs.ulDeviceMask[PrimaryHead];

    //
    // Initialize the modelist pointers to NULL
    //
    for (i=0; i < NV_NO_DACS; i++)
    {
        PhysModeList[i] = NULL;
        ulDeviceDisplay[i]= NumberDacs.ulDeviceDisplay[i];
    }
    SingleVirtualModeList = NULL;

    
    if ((NumberDacs.ulNumberDacsOnBoard == 2 && NumberDacs.ulNumberDacsConnected == 2) ||
        (NumberDacs.ulNumberDacsOnBoard == 2 && NumberDacs.ulNumberDacsConnected == 1) ||
        (NumberDacs.ulNumberDacsOnBoard == 1 && NumberDacs.ulNumberDacsConnected == 1))
    {
        
        if (EngDeviceIoControl(hDriver,
                               IOCTL_VIDEO_GET_TWINVIEW_STATE,
                               NULL,
                               0,
                               &sData,
                               sizeof(NVTWINVIEWDATA),
                               &ReturnedDataLength))
        {
            DISPDBG((3, "InitMultiMon() - IOCTL_VIDEO_GET_TWINVIEW_STATE failed"));
            TwinView_State = NVTWINVIEW_STATE_NORMAL;
        }
        else
        {
            DISPDBG((6,"Read the registry state as: 0x%x\n",sData.dwState));

            // sanity check to see if the TwinViewInfo data is valid.
            // 'NULL' is because we don't get a pointer to ppdev in DrvGetModes.
            if (sData.dwState == NVTWINVIEW_STATE_DUALVIEW)
                TwinView_State = sData.dwState;

            if (!TwinViewDataOK((PPDEV)NULL, &sData))
            {
                TwinViewRegistryDataOK = 0; 
            }
            else
            {
                TwinView_State = sData.dwState;
                TwinView_Orientation = sData.dwOrientation;
                PrimaryHead = sData.dwDeviceDisplay[0];
                PrimaryDeviceMask = sData.NVTWINVIEWUNION.nvtwdevdata[PrimaryHead].dwDeviceMask;

                for (i = 0; i < NV_NO_DACS; i++)
                    ulDeviceDisplay[i]= sData.dwDeviceDisplay[i];
                DISPDBG((6,"Read the Primary head as: 0x%x\n",PrimaryHead));
                TwinViewRegistryDataOK = 1;

                if (TwinView_State != NVTWINVIEW_STATE_NORMAL && TwinView_State != NVTWINVIEW_STATE_CLONE &&
                    TwinView_State != NVTWINVIEW_STATE_DUALVIEW && TwinView_State != NVTWINVIEW_STATE_SPAN)
                {
                    DISPDBG((0, "getAvailableModes - Invalid State read from registry: 0x%x,TwinView_State"));
                    return 0;
                }
                if (TwinView_State == NVTWINVIEW_STATE_SPAN)
                {
                    if (TwinView_Orientation != HORIZONTAL_ORIENTATION && TwinView_Orientation != VERTICAL_ORIENTATION)
                    {
                        DISPDBG((0, "getAvailableModes - Invalid Orientation read from registry: 0x%x for MULTIMON state\n",TwinView_Orientation));
                        return 0;
                    }
                }
            }
        }
    }



    //**************************************************************************
    // Ask the mini-port to fill in the available modes.
    //**************************************************************************

    if (EngDeviceIoControl(hDriver,
                           IOCTL_VIDEO_QUERY_AVAIL_MODES,
                           NULL,
                           0,
                           *modeInformation,
                           modes.NumModes * modes.ModeInformationLength,
                           &ulTemp))
    {

        DISPDBG((1, "getAvailableModes - Failed VIDEO_QUERY_AVAIL_MODES"));

        EngFreeMem(*modeInformation);
        *modeInformation = (PVIDEO_MODE_INFORMATION) NULL;

        return(0);
    }

    //**************************************************************************
    // Allocate the temporary buffers to hold the physical modelist
    // for each head and the single resolution virtual mode list.
    //**************************************************************************
    for (i = 0; i < NumberDacs.ulNumberDacsOnBoard; i++)
    {
        PhysModeList[i] = (MODE_ENTRY *)EngAllocMem(FL_ZERO_MEMORY,
                                                    modes.NumModes * sizeof(MODE_ENTRY),
                                                    ALLOC_TAG);
        if (PhysModeList[i] == (MODE_ENTRY *) NULL)
        {
            DISPDBG((0, "getAvailableModes - Failed EngAllocMem for PhysModeList: %d", i));
            goto getAvailableModesFailure;
        }
    }

    SingleVirtualModeList = (MODE_ENTRY *)EngAllocMem(FL_ZERO_MEMORY,
                                                      modes.NumModes * sizeof(MODE_ENTRY),
                                                      ALLOC_TAG);
    if (SingleVirtualModeList == (MODE_ENTRY *) NULL)
    {
        DISPDBG((0, "getAvailableModes - Failed EngAllocMem for PhysModeList1"));
        goto getAvailableModesFailure;
    }

    // Invalidate all the doubled modes from the VirtualModeList.
    NumSingleVirtualModes = 0;
    pVideoTemp = *modeInformation;
    DestPtr = SingleVirtualModeList;
    for (i=0; i<modes.NumModes; i++)
    {
        if (pVideoTemp)
        {
            if (MULTIMON_MODE(pVideoTemp->VisScreenWidth,pVideoTemp->VisScreenHeight))
            {
                // Do not copy.
                pVideoTemp++;
                continue;
            }
            // This is a single resolution virtual mode. So copy it.
            DestPtr->Width = (USHORT)pVideoTemp->VisScreenWidth;
            DestPtr->Height = (USHORT)pVideoTemp->VisScreenHeight;
            DestPtr->Depth = (USHORT)pVideoTemp->BitsPerPlane;
            DestPtr->RefreshRate = (USHORT)pVideoTemp->Frequency;
            DestPtr->ValidMode = TRUE;
            NumSingleVirtualModes++;
            DestPtr++;
            pVideoTemp++;
        }
    }

    //**************************************************************************
    // Now see which of these modes are supported by the display driver.
    // As an internal mechanism, set the length to 0 for the modes we
    // DO NOT support.
    //**************************************************************************

    ulTemp = modes.NumModes;
    pVideoTemp = *modeInformation;

    
    //**************************************************************************
    // Get the physical mode list for all the heads from miniport.
    // We already have this info in PDEV but we don't get the ppdev passed to us
    // in DrvGetModes()
    //**************************************************************************
    for (i=0; i < NumberDacs.ulNumberDacsOnBoard; i++)
    {
        HEAD_MODE_LIST_INFO VirtInfo;
        
        ulHead = ulDeviceDisplay[i];
        VirtInfo.ulHead = ulHead;

        // Use the correct default values if the registry entry is absent
        if (!TwinViewRegistryDataOK)
        {
            VirtInfo.ulDeviceMask = NumberDacs.ulDeviceMask[ulHead];
            VirtInfo.ulDeviceType = NumberDacs.ulDeviceType[ulHead];
            VirtInfo.ulTVType = NumberDacs.ulTVFormat[ulHead];
            VirtInfo.ulEnableDDC = 1;
        }
        else
        {
            VirtInfo.ulDeviceMask = sData.NVTWINVIEWUNION.nvtwdevdata[ulHead].dwDeviceMask;
            VirtInfo.ulDeviceType = sData.NVTWINVIEWUNION.nvtwdevdata[ulHead].dwDeviceType;
            VirtInfo.ulTVType = sData.NVTWINVIEWUNION.nvtwdevdata[ulHead].dwTVFormat;
            VirtInfo.ulEnableDDC = sData.NVTWINVIEWUNION.nvtwdevdata[ulHead].dwEnableDDC;
            VirtInfo.ulEnableDDC = 1;
        }
        
        VirtInfo.pVirtualModeList = SingleVirtualModeList;
        VirtInfo.ulNumVirtualModes = NumSingleVirtualModes;

        VirtInfo.pPhysicalModeList = PhysModeList[ulHead];

        // Call the IOCTL for the miniport to return the physical modes.
        ulRet = EngDeviceIoControl(hDriver,
                                   IOCTL_VIDEO_GET_HEAD_PHYSICAL_MODE_LIST,
                                   (HEAD_MODE_LIST_INFO *)&VirtInfo,
                                   sizeof(HEAD_MODE_LIST_INFO),
                                   NULL,
                                   0,
                                   &(ulRetSize));
        if (ulRet)
        {
            DISPDBG((0,"Oops! IOCTL_VIDEO_GET_HEAD_PHYSICAL_MODE_LIST failed!"));
            return(FALSE);
        }
        else
        {
            if (ulRetSize == 0)
            {
                DISPDBG((0,"Oops! Number of physical modes for head: %d is 0",i));
                return(FALSE);
            }

            NumPhysModes[ulHead] = ulRetSize/sizeof(MODE_ENTRY);

        }
    }
    

    //**************************************************************************
    // Mode is rejected if it is not one plane, or not graphics, or is not
    // one of 8, 16, 32 bits per pel.
    //**************************************************************************

    while (ulTemp--)
    {

        if ((pVideoTemp->NumberOfPlanes != 1 ) ||
            !(pVideoTemp->AttributeFlags & VIDEO_MODE_GRAPHICS) ||
            ((pVideoTemp->BitsPerPlane != 8) &&
             (pVideoTemp->BitsPerPlane != 15) &&
             (pVideoTemp->BitsPerPlane != 16) &&
             (pVideoTemp->BitsPerPlane != 24) && //24bpp
             (pVideoTemp->BitsPerPlane != 32)))
        {
            DISPDBG((2, "Rejecting miniport mode:"));
            DISPDBG((2, "   Screen width  -- %li", pVideoTemp->VisScreenWidth));
            DISPDBG((2, "   Screen height -- %li", pVideoTemp->VisScreenHeight));
            DISPDBG((2, "   Bits per pel  -- %li", pVideoTemp->BitsPerPlane *
                     pVideoTemp->NumberOfPlanes));
            DISPDBG((2, "   Frequency     -- %li", pVideoTemp->Frequency));

            pVideoTemp->Length = 0;
        }


        // export only single resolution modes for the normal or clone modes


        MustHaveResolution = 0;

        if(TwinView_State == NVTWINVIEW_STATE_SPAN && 
            pVideoTemp->VisScreenWidth != 480 && pVideoTemp->VisScreenHeight != 360 &&
            pVideoTemp->VisScreenWidth <= 640 && pVideoTemp->VisScreenHeight <= 480)
        {
            // Also export low resolution for spanning mode for DirectDraw
            MustHaveResolution = 1;
        }
        else  
        {
            // The exception is the 640x480 and 800x600 modes which NT4.0 and Win2K expects to be there always.
            // In win2k, GDI inserts 640x480 and 800x600 modes at 4bpp if the driver does not support it natively.
            // Also PC99 14.8 says that 1024x768 should always be supported.

            if ((pVideoTemp->VisScreenWidth == 640 && pVideoTemp->VisScreenHeight == 480) ||
                (pVideoTemp->VisScreenWidth == 800 && pVideoTemp->VisScreenHeight == 600) ||
                (pVideoTemp->VisScreenWidth == 1024 && pVideoTemp->VisScreenHeight == 768))
            {
            // do not skip these modes. Otherise you will see the ugly 4BPP vga modes show up in NT panel list of modes.
            MustHaveResolution = 1;
            }
        }


        if (TwinView_State == NVTWINVIEW_STATE_NORMAL || TwinView_State == NVTWINVIEW_STATE_CLONE || TwinView_State == NVTWINVIEW_STATE_DUALVIEW)
        {
            if (MULTIMON_MODE(pVideoTemp->VisScreenWidth,pVideoTemp->VisScreenHeight))
            {
                pVideoTemp->Length = 0;
            }
        }

        // export only horizontal doubled modes if the state is MULTIMON and orinetation is HORIZONTAL
        if (TwinView_State == NVTWINVIEW_STATE_SPAN && TwinView_Orientation == HORIZONTAL_ORIENTATION)
        {
            if (!(HORIZONTAL_MODE(pVideoTemp->VisScreenWidth,pVideoTemp->VisScreenHeight)))
            {
                if (!MustHaveResolution)
                {
                    pVideoTemp->Length = 0;
                }

            }
        }

        // export only vertical doubled modes if the state is MULTIMON and orinetation is VERTICAL
        if (TwinView_State == NVTWINVIEW_STATE_SPAN && TwinView_Orientation == VERTICAL_ORIENTATION)
        {
            if (!(VERTICAL_MODE(pVideoTemp->VisScreenWidth,pVideoTemp->VisScreenHeight)))
            {
                if (!MustHaveResolution)
                {
                    pVideoTemp->Length = 0;
                }
            }
        }


        


        //
        // Peter Daifuku's specs says that for Rel 6.0, we limit the desktop to 2X the smaller of the two displays.
        // i.e., no pan-scan for either heads.
        // Also, if the INF specifies PAN_SCAN_SELECTION_DISABLE_ALL such as for Gateway, we disallow pan-scan for both heads
        // in clone mode (and standard and span modes).
        //
        if ((TwinView_State == NVTWINVIEW_STATE_SPAN) ||
            (NumberDacs.PanScanSelection == PAN_SCAN_SELECTION_DISABLE_ALL && (TwinView_State == NVTWINVIEW_STATE_CLONE)))
        {
            // see if this mode has already been rejected.
            if (pVideoTemp->Length != 0)
            {
                if (TwinViewRegistryDataOK)
                {
                    PhysWidth =  pVideoTemp->VisScreenWidth;
                    PhysHeight = pVideoTemp->VisScreenHeight;

                    // Get the physical dimensions of this mode.
                    if (HORIZONTAL_MODE(pVideoTemp->VisScreenWidth, pVideoTemp->VisScreenHeight))
                    {
                        PhysWidth = pVideoTemp->VisScreenWidth / 2;
                    }
                    if (VERTICAL_MODE(pVideoTemp->VisScreenWidth, pVideoTemp->VisScreenHeight))
                    {
                        PhysHeight = pVideoTemp->VisScreenHeight / 2;
                    }

                    //
                    // Now see if this physical mode is supported by both heads. 
                    // If not, we do not support this spanning mode.
                    //
                    for (i = 0; i < NV_NO_DACS; i++)
                    {
                        ulHead = ulDeviceDisplay[i];
                        PhysModePtr = PhysModeList[ulHead];
                        bFoundPhysMode = FALSE;

                        for (j=0; j < NumPhysModes[ulHead]; j++)
                        {
                            // Do not test the refresh rate because we allow it to be different on
                            // both the heads.
                            if (PhysModePtr->Width == PhysWidth && PhysModePtr->Height == PhysHeight &&
                                PhysModePtr->Depth == pVideoTemp->BitsPerPlane &&
                                PhysModePtr->ValidMode)
                            {
                                // Found a match.
                                bFoundPhysMode = TRUE;
                                break;
                            }
                            PhysModePtr++;
                        }

                        // Special case.
                        // Toshiba wants 1400x1050 to be exported only if panel supports it
                        // natively.
                        if (NumberDacs.PanScanSelection == PAN_SCAN_SELECTION_ENABLE_ALL && 
                            PhysWidth == 1400 && PhysHeight == 1050 && 
                            NumberDacs.ulDFPSupports1400x1050 == 0)
                        {
                            // Disallow the mode.
                            pVideoTemp->Length = 0;
                            break;
                        }

                        // Special case
                        // The common modeset code does not allow two low res modes for DFP
                        // These are 480x360 and 960x720. So do not export these modes.
                        // To be more symmetrical, we will disallow these modes irrespective of the 
                        // device types and platforms.
                        if ((PhysWidth == 480 && PhysHeight == 360) ||
                            (PhysWidth == 960 && PhysHeight == 720) )
                        {
                            // Disallow the mode.
                            pVideoTemp->Length = 0;
                            break;
                        }

                        
                        if (!bFoundPhysMode)
                        {
                            // This head does not support pan-scan. So ignore this mode.
                            // On Toshiba, we allow pan-scan for spanning modes also.
                            if (NumberDacs.PanScanSelection == PAN_SCAN_SELECTION_ENABLE_ALL)
                            {
                                // Do nothing
                            }

                            else
                            {
                                // Disallow the mode.
                                pVideoTemp->Length = 0;
                            }


                            break;
                        }
                    } // for each DAC
                } // registry data is OK
            } // Mode is valid
        } // State is MULTIMON


        // In normal mode or clone mode or multimon mdoe, each and every mode exported to GDI should be supported 
        // by the primary monitor as NT/Win2K directly control the primary screen.
        if ((pVideoTemp->Length != 0) &&
            (TwinView_State == NVTWINVIEW_STATE_NORMAL ||
             TwinView_State == NVTWINVIEW_STATE_DUALVIEW ||
             TwinViewRegistryDataOK)
           ) 
        {
            PhysWidth = (unsigned short)(pVideoTemp->VisScreenWidth);
            PhysHeight = (unsigned short)(pVideoTemp->VisScreenHeight);

            //
            // Note: Peter Daifuku's requirement states as follows
            // 1. For normal mode, allow pan/scan for primary. So filter based on refresh rate only.
            //    However, it is off by default unless user explicitly turns on panning in the panel.
            // 2. For clone mode, do not allow pan/scan for primary. So we need to filter based on refresh rate,
            //    width and height.
            // 3. For multimon mode, do not allow pan/scan for primary (and secondary). 
            //    So we need to filter based on refresh rate, width and height.
            //
            if (TwinView_State == NVTWINVIEW_STATE_SPAN)
            {
                // Get the physical dimensions of this mode.
                if (HORIZONTAL_MODE(pVideoTemp->VisScreenWidth, pVideoTemp->VisScreenHeight))
                {
                    PhysWidth = pVideoTemp->VisScreenWidth / 2;
                }
                if (VERTICAL_MODE(pVideoTemp->VisScreenWidth, pVideoTemp->VisScreenHeight))
                {
                    PhysHeight = pVideoTemp->VisScreenHeight / 2;
                }
            }

            if (TwinViewRegistryDataOK)
            {
                ulHead = sData.dwDeviceDisplay[0];
            }
            else
            {
                // Determine whether to use head0 or head1.
                // If you have only one head connected on a dual head card, use the head which
                // has a valid device connected to it.
                if (NumberDacs.ulNumberDacsOnBoard == 1)
                {
                    ulHead = 0;
                }
                else if (NumberDacs.ulNumberDacsOnBoard == 2 && TwinView_State == NVTWINVIEW_STATE_NORMAL)
                {
                    if (NumberDacs.ulDeviceMask[0] == BITMASK_INVALID_DEVICE && NumberDacs.ulDeviceMask[1] != BITMASK_INVALID_DEVICE)
                    {
                        ulHead = 1;
                    }
                    if (NumberDacs.ulDeviceMask[0] != BITMASK_INVALID_DEVICE && NumberDacs.ulDeviceMask[1] == BITMASK_INVALID_DEVICE)
                    {
                        ulHead = 0;
                    }
                    if (NumberDacs.ulDeviceMask[0] == BITMASK_INVALID_DEVICE && NumberDacs.ulDeviceMask[1] == BITMASK_INVALID_DEVICE)
                    {
                        // Pathological case. Nothing is connected!
                        ulHead = 0;
                    }
                    if (NumberDacs.ulDeviceMask[0] != BITMASK_INVALID_DEVICE && NumberDacs.ulDeviceMask[1] != BITMASK_INVALID_DEVICE)
                    {
                        // A valid device is connected to both heads. Let us choose head0 as default.
                        ulHead = 0;
                    }
                }
                else if (NumberDacs.ulNumberDacsOnBoard == 2 && TwinView_State == NVTWINVIEW_STATE_DUALVIEW)
                {
                    ulHead = sData.dwDeviceDisplay[0];
                }
            }

            PhysModePtr = PhysModeList[ulHead];
            bFoundPhysMode = FALSE;

            for (j = 0; j < NumPhysModes[ulHead]; j++)
            {
                if (PhysModePtr->Width == PhysWidth && PhysModePtr->Height == PhysHeight &&
                    PhysModePtr->Depth == pVideoTemp->BitsPerPlane &&
                    PhysModePtr->RefreshRate == pVideoTemp->Frequency &&
                    PhysModePtr->ValidMode)
                {
                    // Found a match.
                    bFoundPhysMode = TRUE;
                    break;
                }
                PhysModePtr++;
            }

            if (!bFoundPhysMode)
            {
                // This head does not support pan-scan. So ignore this mode.
                // On Toshiba laptops, we allow pan-scan on standard mode and clone mode and multimon mode also.
                if ( NumberDacs.PanScanSelection == PAN_SCAN_SELECTION_ENABLE_ALL )
                {
                    // Do nothing.
                }
#if _WIN32_WINNT >= 0x500
                else
                if (bQueryGDIModePruning(TwinView_State, PrimaryHead, PrimaryDeviceMask, 
                        PrimaryHead,  PrimaryDeviceMask, NumberDacs.ACPISystem, NumberDacs.GDIModePruning))
                {
                    //
                    // We let GDI do the mode pruning for the EDID
                    //
                    // Do nothing
                }
#endif
                else
                {
                    // Do not allow this mode.
                    pVideoTemp->Length = 0;
                }
            }

            // Special case.
            // Toshiba wants 1400x1050 to be exported only if panel supports it
            // natively.
            if (NumberDacs.PanScanSelection == PAN_SCAN_SELECTION_ENABLE_ALL && 
                PhysWidth == 1400 && PhysHeight == 1050 && 
                NumberDacs.ulDFPSupports1400x1050 == 0)
            {
                // Disallow the mode.
                pVideoTemp->Length = 0;
            }

            // Special case
            // The common modeset code does not allow two low res modes for DFP
            // These are 480x360 and 960x720. So do not export these modes.
            // To be more symmetrical, we will disallow these modes irrespective of the 
            // device types and platforms.
            if ((PhysWidth == 480 && PhysHeight == 360) ||
                (PhysWidth == 960 && PhysHeight == 720) )
            {
                // Disallow the mode.
                pVideoTemp->Length = 0;
            }

        } // Normal or clone mode or multimon mode.

        //
        // If this mode was found valid for GDI for current desktop settings, increment the counter.
        //
        if (pVideoTemp->Length)
        {
            cNumGDIModes++;
        }
        


        //**********************************************************************
        // Check next mode
        //**********************************************************************

        pVideoTemp = (PVIDEO_MODE_INFORMATION)
                     (((PUCHAR)pVideoTemp) + modes.ModeInformationLength);
    }

    // Free up the temporary memory allocations.
    for (i = 0; i < NumberDacs.ulNumberDacsOnBoard; i++)
    {
        EngFreeMem(PhysModeList[i]);
    }
    EngFreeMem(SingleVirtualModeList);

    *pcNumGDIModes = cNumGDIModes;
    return(modes.NumModes);

getAvailableModesFailure:
    for (i = 0; i < NumberDacs.ulNumberDacsOnBoard; i++)
    {
        if (PhysModeList[i] != NULL)
        {
            EngFreeMem(PhysModeList[i]);
        }
    }
    if (*modeInformation != NULL) 
    {
        EngFreeMem(*modeInformation);
    }
    *pcNumGDIModes = 0;
    return 0;
}

#if (_WIN32_WINNT >= 0x0500)
//******************************************************************************
//
//  Function: DrvNotify
//
//  Routine Description: Attempts to allocate push buf from AGP memory, if
//  there is no AGP memory it falls back to allocating the push buf from
//  system memory.
//
//  Arguments:
//
//  Return Value:
//            TRUE or FALSE
//
//
//******************************************************************************
void DrvNotify(
    SURFOBJ *hsurf,
    ULONG iType,
    PVOID pvData)
    {

    if (hsurf && pvData)
        {
        PDEV* ppdev = (PDEV *)hsurf->dhpdev;

        switch(iType)
            {
            case DN_DEVICE_ORIGIN:
                {
                POINTL *devOrigin;

                devOrigin = (POINTL *)pvData;
                ppdev->left = devOrigin->x;
                ppdev->top = devOrigin->y;
                }
                break;
            default:
                break;
            }
        }

    }

#endif // #if _WIN32_WINNT >= 0x0500

