/*
 * nvReg.h
 *
 * Registry keys and functions for the NVIDIA drivers
 *
 * Copyright (c) 1997-1999, NVIDIA Corporation.  All rights reserved.
 *
 * THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
 * NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
 * IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
 */

/* Note to OEMs:
   Please leave these strings intact.  There are utilities published on the
   net which let users edit these registry keys, and the utils expect the
   values to be in this location.  It will also allow us to occasionally post
   messages in RIVA-related web sites discussing how these values can affect
   performance and quality.  Thanks!
*/

//*****************************************************************************
// ----------------------------  NOTE  ----------------------------------------
//
// Only #define constants belong in this file.  This file is used for other
// operating systems besides Windows and should be kept clean of Windows
// specific types.
//
// ----------------------------------------------------------------------------
//*****************************************************************************


//-----------------------------------------------------------------------------
// Base
//-----------------------------------------------------------------------------

#define NV_MAX_REG_KEYNAME_LEN 256

#define NV04_REG_PRODUCT_NAME                   "RIVA TNT"
#define NV10_REG_PRODUCT_NAME                   "GeForce 256"
#define NV10GL_REG_PRODUCT_NAME                 "Quadro"

//  There are keys which are global and apply to all NVidia boards in a
//  system and those keys which are board specific. The global keys all
//  use the following path as a base:
#if defined(unix)
#define NV4_REG_GLOBAL_BASE_KEY     ""
#define NV4_REG_GLOBAL_BASE_PATH    "_NV_"
#else
#define NV4_REG_GLOBAL_BASE_KEY     HKEY_LOCAL_MACHINE
#define NV4_REG_GLOBAL_BASE_PATH    "SOFTWARE\\NVIDIA Corporation\\Global"
#endif
#define NV4_REG_MAX_STR_SIZE        256

//  For each sub-system, the following key should be tacked onto the
//  GLOBAL_BASE_PATH given above and specific registry values for that
// sub-system placed inside the resulting key.
#define NV4_REG_SUBKEY                  "NVidia"
#define NV4_REG_DISPLAY_DRIVER_SUBKEY   "Display"
#define NV4_REG_DIRECT_DRAW_SUBKEY      "DirectDraw"
#define NV4_REG_DIRECT3D_SUBKEY         "Direct3D"
#define NV4_REG_RESOURCE_MANAGER_SUBKEY "System"
#define NV4_REG_OGL_SUBKEY              "OpenGL"
#define NV4_REG_OGL_DEBUG_SUBKEY        "OpenGL\\Debug"
#define NV4_REG_CONTROL_PANEL_SUBKEY    "NVTweak"
#define NV4_REG_DESKTOP_MANAGER_SUBKEY  "NVDesk"
#define NV4_REG_MEDIA_PORT_SUBKEY       "MediaPort"
#define NV4_REG_STEREO_SUBKEY           "Stereo3D"

// For the board specific keys (also called "local" keys), the registry
// path must be run-time determined. An escape has been added to the
// display driver to return a registry path string. Each instance of the
// display driver in a multi-mon system will return a different
// registry path that is specific to the board/slot/device, etc. You
// should use this as the base path for local keys and, as with the
// global keys, tack on one of the above sub-system strings and then
// place your registry values in the resulting key. The escape to
// return the local base path is defined in
// ...drivers\common\win9x\inc\escape.h

//-----------------------------------------------------------------------------
// Display Driver
//-----------------------------------------------------------------------------

// These may be used by many labels
#define NV4_REG_DRV_TRUE                         "1"
#define NV4_REG_DRV_FALSE                        "0"

// This is for QA, so that any mode can be set.
#define NV4_REG_DRV_ALLOW_ALL_MODES             "AllowAllModes"

// This is to allow any mode up to 2048x1536 to be set as the desktop.
#define NV4_REG_DRV_ALLOW_MIGHTY_MODES          "AllowMightyModes"

// This tells what the boot device will be.
#define NV4_REG_DRV_BOOT_DEVICE_TYPE            "ForcedBootDeviceType"

// This tells us whether to use the RM or the reg for getting a
// forced boot device type.
#define NV4_REG_DRV_USE_HW_SELECTED_DEVICE      "UseHwSelectedDevice"

// This tells us whether or not to use a separate registry key for the
// clone desktop mode and the standard desktop mode.
#define NV4_REG_DRV_USE_CLONE_DESKTOP_MODE      "UseCloneDesktopMode"

// This tells us whether to restrict the desktop mode to the greatest
// EDID mode of the lesser display device.
#define NV4_REG_DRV_RESTRICT_LARGEST_MODE       "RestrictLargestMode"

// This tells us whether we should invalidate 1400x1050 as a mode when
// we are not on a 1400x1050 panel.
#define NV4_REG_DRV_1400x1050_OVERRIDE          "Override1400x1050"

// This tells us whether to send a StopImage method to the VideoLutCursorDac
// class after a modeset. This will blank the screen.                                                                 
#define NV4_REG_DRV_MODESET_STOP_IMAGE          "ModesetStopImage"

// If this key exists, it tells us the maximum allowable mode for the DFP.
#define NV4_REG_DRV_MAX_DFP_MODE                "MaxDfpMode"

// The physical device attachments are given as PhysicalDevice0,
// PhysicalDevice1, etc. This is the base string.
#define NV4_REG_DRV_PHYSICAL_DEVICE             "PhysicalDevice"

// The following reg keys are string values which always look like
// "xres,yres,bpp"  e.g. "1024,768,16"
#define NV4_REG_DRV_FORCED_DESKTOP_MODE         "ForcedDesktopMode"
#define NV4_REG_DRV_LAST_DESKTOP_MODE           "LastDesktopMode"
#define NV4_REG_DRV_LAST_CLONE_DESKTOP_MODE     "LastCloneDesktopMode"

// This keeps track of the last tv format
#define NV4_REG_DRV_LAST_TV_FORMAT              "LastTVFormat"

// These are possible values for the LastTVFormat
#define NV4_REG_DRV_NTSCM_TVFORMAT              "NTSCM"
#define NV4_REG_DRV_NTSCJ_TVFORMAT              "NTSCJ"
#define NV4_REG_DRV_PALM_TVFORMAT               "PALM"
#define NV4_REG_DRV_PALA_TVFORMAT               "PALA"
#define NV4_REG_DRV_PALN_TVFORMAT               "PALN"
#define NV4_REG_DRV_PALNC_TVFORMAT              "PALNC"

// The following reg keys are string values which always look like
// "xres,yres,bpp,refresh_rate"  e.g. "1024,768,16,75"
// These are local keys.
#define NV4_REG_DRV_LAST_CRT_MODE               "LastCRTMode"
#define NV4_REG_DRV_LAST_DFP_MODE               "LastDFPMode"
#define NV4_REG_DRV_LAST_NTSCM_MODE             "LastNTSCMMode"
#define NV4_REG_DRV_LAST_NTSCJ_MODE             "LastNTSCJMode"
#define NV4_REG_DRV_LAST_PALM_MODE              "LastPALMMode"
#define NV4_REG_DRV_LAST_PALA_MODE              "LastPALAMode"
#define NV4_REG_DRV_LAST_PALN_MODE              "LastPALNMode"
#define NV4_REG_DRV_LAST_PALNC_MODE             "LastPALNCMode"

// The following reg keys are string values which always look like
// "xres,yres,bpp,refresh_rate"  e.g. "1024,768,16,75"
// These are local keys.
#define NV4_REG_DRV_FORCED_MODE                 "ForcedMode"
#define NV4_REG_DRV_LAST_MODE                   "LastMode"

// The following registry values are used when the user requests adapter
// default refresh rate. They are all string values which look like:
// "60" or "75" etc. If the keys do not exist, the adapter default value
// is obtained in another way.
// These are local keys.
#define NV4_REG_DRV_DEFAULT_CRT_VERT_REFRESH_RATE   "DefaultCRTRefreshRate"
#define NV4_REG_DRV_DEFAULT_DFP_VERT_REFRESH_RATE   "DefaultDFPRefreshRate"

// The following registry values are used when the user requests optimal
// refresh rate. They are all string values which look like:
// "60" or "75" etc. If the keys do not exist, the optimal refresh value
// is obtained in another way.
// These are local keys.
#define NV4_REG_DRV_OPTIMAL_CRT_VERT_REFRESH_RATE   "OptimalCRTRefreshRate"
#define NV4_REG_DRV_OPTIMAL_DFP_VERT_REFRESH_RATE   "OptimalDFPRefreshRate"

// This registry value is used to override the timing standard used
// for the monitor.
#define NV4_REG_DRV_MONITOR_TIMING                  "MonitorTiming"
#define NV4_REG_DRV_MONITOR_TIMING_DMT              "DMT"
#define NV4_REG_DRV_MONITOR_TIMING_GTF              "GTF"

// This is used for the cursor alpha amount
#define NV4_REG_DRV_CURSOR_ALPHA                    "CursorAlpha"

// The following key is used to indicate that the driver should
// read the EDID and use it. It is a string value and the
// possible options are "0" and "1".
#define NV4_REG_DRV_FORCE_EDID_READ                 "ForceEdidRead"

// The following reg key ius a string value which always looks like
// "xres,yres,bpp,refresh_rate"  e.g. "1024,768,16,75"
// It is a local key. It gives the highest permissable mode when
// their is no EDID and no INF installed for the monitor.
#define NV4_REG_DRV_MONITOR_LIMIT_MODE              "NoEdidNoInfLimitMode"

// The RestrictOptimal key causes the modeset DLL to look for an
// exact Xres,Yres match with an EDID mode. If one exists, then
// that refresh rate will be the one used. If not, then other
// logic determines the refresh rate. The possible values are
// NV4_REG_DRV_TRUE or NV4_REG_DRV_FALSE
#define NV4_REG_DRV_USE_REFRESH_RATE_OF_XYRES_EDID_MODE_MATCH   "RestrictOptimal"

// These are erased by the driver at boot time.
#define POWERUPFLAGS                                "PowerUpFlags"

// These are used by the driver for dual-head support
#define NV4_REG_DRV_VIRTUALDESKTOP                  "VirtualDesktop"
#define NV4_REG_DRV_AUTOPANMODE                     "AutoPanMode"

// This key is read to indicate whether we should run single monitor,
// multimon or clone mode.
#define NV4_REG_DRV_DESKTOPSTATE                    "DesktopState"
#define NV4_REG_DRV_DESKTOP_STATE_NORMAL            "Single"
#define NV4_REG_DRV_DESKTOP_STATE_MULTI_MON         "Multi"
#define NV4_REG_DRV_DESKTOP_STATE_CLONE             "Clone"

// This key is read to determine whether or not to check for an SXGA Panel.
#define NV4_REG_DRV_CHECK_SXGA_PANEL                "CheckSXGAPanel"

// This key is read to determine whether or not to set panning for the clone device.
#define NV4_REG_DRV_PANNING_FLAG                    "CloneDevicePanningFlag"


//-----------------------------------------------------------------------------
// DirectDraw
//-----------------------------------------------------------------------------

// default is enabled, define and set to zero to disable
#define NV4_REG_VPE_ENABLE              "VPEENABLE"

// EnumDDRefresh (default = 0)
//      0 = do not enumerate refresh rates
//      1 = only enumerate refresh rates of modes above 640x480
//      2 = enumerate all refresh rates
#define NV4_REG_ENUM_REFRESH                "EnumDDRefresh"
#define   NV4_REG_ENUM_REFRESH_DISABLE      0
#define   NV4_REG_ENUM_REFRESH_ENABLEBIG    1
#define   NV4_REG_ENUM_REFRESH_ENABLEALL    2

// all the overlay settings are local

// OverlayMode (default = 0)
//      0 = always use overlay upscale
//      1 = use vertical blt upscale if sufficient resources
//      2 = use temporal filter
//      3 = this is not the first frame (gets autoset after 1 frame)
//      8-15 = temporal filter factor
#define NV4_REG_OVL_MODE                "OverlayMode"
#define   NV4_REG_OVL_MODE_VUPSCALE     0x1
#define   NV4_REG_OVL_MODE_TFILTER      0x2
#define   NV4_REG_OVL_MODE_NOTFIRST     0x4
#define   NV4_REG_OVL_MODE_TF_PRECOPY   0x8
#define   NV4_REG_OVL_MODE_LATEFLIPSYNC 0x10
#define   NV4_REG_OVL_MODE_DFILTER      0x20
#define   NV4_REG_OVL_MODE_DF_PRECOPY   0x40
#define   NV4_REG_OVL_MODE_TFACTOR      15:8
#define   NV4_REG_OVL_MODE_DFACTOR      23:16

// OverlayMode2 (default = 0)
//   bits 0-3 = Full screen mirror device number (1 based, zero means disable)
//      bit 4 = Preserve aspect in full screen mirror mode
//      bit 5 = Fix aspect ratio to 16:9, bit 4 must be set also
//      bit 6 = Track aspect ratio of overlay, bit 4 must be set also
//      bit 7 = Fix aspect ratio to 4:3, bit 4 must also be set
//      bits 8-11 = Video zoom quadrant
//                  0: zoom disabled
//                  1: top left quadrant
//                  2: top right quadrant
//                  3: bottom left quadrant
//                  4: bottom right quadrant
//                  5: center
//      bits 12-23 = Video zoom factor, 0 = 1x, 255 = 2x
//      bit 24 = Let the driver pick the full screen display mode
//      bit 25 = Track overlay zoom
//      bit 26 = Enable TV devices
#define NV4_REG_OVL_MODE2                       "OverlayMode2"
#define     NV4_REG_OVL_MODE2_FSMASK            0x007FFFFF
#define     NV4_REG_OVL_MODE2_FSDEVICEMASK      0x0000000F
#define     NV4_REG_OVL_MODE2_FSASPECTMASK      0x000000F0
#define     NV4_REG_OVL_MODE2_FSZOOMQUADMASK    0x00000F00
#define     NV4_REG_OVL_MODE2_FSZOOMFACTORMASK  0x000FF000
#define     NV4_REG_OVL_MODE2_FSDEVICE          0:3
#define     NV4_REG_OVL_MODE2_FSASPECT          0x00000010
#define     NV4_REG_OVL_MODE2_FSFIXEDANIMORPHIC 0x00000020
#define     NV4_REG_OVL_MODE2_FSTRACKOVLASPECT  0x00000040
#define     NV4_REG_OVL_MODE2_FSFIXEDTV         0x00000080
#define     NV4_REG_OVL_MODE2_FSZOOMQUAD        8:11
#define     NV4_REG_OVL_MODE2_FSZOOMFACTOR      12:23
#define     NV4_REG_OVL_MODE2_FSSETMODE         0x00100000
#define     NV4_REG_OVL_MODE2_FSTRACKOVLZOOM    0x00200000
#define     NV4_REG_OVL_MODE2_FSENABLETV        0x00400000

// OverlayMode3 (default = 0);
//   bit 0 = Allow overlay creation in clone mode and win2k span mode
//   bits 8-11 = Overlay zoom quadrant
//               0: zoom disabled
//               1: top left quadrant
//               2: top right quadrant
//               3: bottom left quadrant
//               4: bottom right quadrant
//               5: center
//   bits 12-23: Overlay zoom factor, 0 = 1x, 255 = 2x
#define NV4_REG_OVL_MODE3                                   "OverlayMode3"
#define     NV4_REG_OVL_MODE3_ALLOWOVL                      0x00000001
#define     NV4_REG_OVL_MODE3_DXVA_BACK_END_ALPHA_PREFERRED 0x00000002
#define     NV4_REG_OVL_MODE3_OVLZOOMMASK                   0x000FFF00
#define     NV4_REG_OVL_MODE3_OVLZOOMQUADMASK               0x00000F00
#define     NV4_REG_OVL_MODE3_OVLZOOMFACTORMASK             0x000FF000
#define     NV4_REG_OVL_MODE3_OVLZOOMQUAD                   8:11
#define     NV4_REG_OVL_MODE3_OVLZOOMFACTOR                 12:23

// VPPInvMask (default = 0)
//      Inverse mask for all VPP function enables.  Should exactly match
//      command flags defined in ddVPP.h
#define NV4_REG_VPP_INV_MASK                "VPPInvMask"
#define   NV4_REG_VPP_INV_MASK_ODD          0x1
#define   NV4_REG_VPP_INV_MASK_EVEN         0x2
#define   NV4_REG_VPP_INV_MASK_BOB          0x4
#define   NV4_REG_VPP_INV_MASK_INTERLEAVED  0x8
#define   NV4_REG_VPP_INV_MASK_VIDEOPORT    0x10
#define   NV4_REG_VPP_INV_MASK_WAIT         0x20
#define   NV4_REG_VPP_SAVE_STATE_DISABLE    0x40
#define   NV4_REG_VPP_RESTORE_STATE_DISABLE 0x80
#define   NV4_REG_VPP_CONVERT_DISABLE       0x100
#define   NV4_REG_VPP_SUBPICTURE_DISABLE    0x200
#define   NV4_REG_VPP_PRESCALE_DISABLE      0x400
#define   NV4_REG_VPP_COLOURCONTROL_DISABLE 0x800
#define   NV4_REG_VPP_TEMPORAL_DISABLE      0x1000
#define   NV4_REG_VPP_OPTIMIZEFLIP_DISABLE  0x2000
#define   NV4_REG_VPP_DEINTERLACE_DISABLE   0x4000
#define   NV4_REG_VPP_FSMIRROR_DISABLE      0x8000
#define   NV4_REG_VPP_DMABLIT_DISABLE       0x10000
#define   NV4_REG_VPP_MASTER_DISABLE        0x80000000

// VPPMaxSurfaces
//      Determines the maximum number of work surfaces VPP is allowed to use.
//      Legal values are:
//          6 - Full functionality and performance
//          4 - Full functionality, no superpipelining
//          3 - One stage allowed, superpipelined
//          2 - One stage allowed, no superpipelining
//          0 - VPP disabled
#define NV4_REG_VPP_MAX_SURFACES            "VPPMaxSurfaces"

// OverlayColorControlEnable (default = 0)
//      0 = disable
//      1 = enable with vertical chroma subsampling enabled (use this one)
//      2 = enable with vertical chroma subsampling disabled
// OverlayBrightness (default = 0)
// OverlayContrast (default = 100)
// OverlayHue (default = 0)
// OverlaySaturation (default = 100)
// OverlayGamma (not implemented)
// OverlaySharpness (not implemented)
#define NV4_REG_OVLCC_ENABLE            "OverlayColorControlEnable"
#define NV4_REG_OVLCC_BRIGHTNESS        "OverlayBrightness"
#define NV4_REG_OVLCC_CONTRAST          "OverlayContrast"
#define NV4_REG_OVLCC_HUE               "OverlayHue"
#define NV4_REG_OVLCC_SATURATION        "OverlaySaturation"
#define NV4_REG_OVLCC_GAMMA             "OverlayGamma"
#define NV4_REG_OVLCC_SHARPNESS         "OverlaySharpness"

#define NV4_REG_OVLCC_BRIGHTNESS_DEFAULT  0
#define NV4_REG_OVLCC_CONTRAST_DEFAULT    100
#define NV4_REG_OVLCC_HUE_DEFAULT         0
#define NV4_REG_OVLCC_SATURATION_DEFAULT  100

// VideoBusMasterMode (default = 0)
//      Determine which hacks to implement for bus mastering TV tuners (will not work on WINNT)
//      0 = No special hacks
//      1 = If no flips detected and 1 overlay surface, force into autoflip mode
//      2 = If 1 overlay surface, force autoflip
//      3 = Force autoflip
#define NV4_REG_VIDEO_BUS_MASTER_MODE   "VideoBusMasterMode"
#define   NV4_REG_VBMM_NOHACK           0
#define   NV4_REG_VBMM_DETECT           1
#define   NV4_REG_VBMM_SINGLEOVL        2
#define   NV4_REG_VBMM_FORCE            3

//-----------------------------------------------------------------------------
// Direct3D
//-----------------------------------------------------------------------------

// To add registry settings:
// 1. choose a category:
//    boolean, enumerated non-boolean, non-enumerated non-boolean, or string
// 2. add definitions as required for that category. make sure to include a definition
//    for the default value (and PLEASE ALPHABETIZE and use the same ordering for ALL
//    OF WHAT FOLLOWS!)
// 3. add a reg_entry to the definition of reg_struc in global.h
// 4. add the string to the initialization of reg_struc in global.cpp
// 5. add the default value to the initializations at the top of D3DReadRegistry()
// 6. add a routine to actually read and cache the registry value in D3DReadRegistry()

// ------------ Boolean enables / disables ------------------------------------

// Each boolean enable gets one bit in a control word.
#define D3D_REG_BIT_ALTERNATEZENABLE                    0
#define D3D_REG_BIT_ANTIALIASENABLE                     1
#define D3D_REG_BIT_ANTIALIASDYNAMICENABLE              2
#define D3D_REG_BIT_ANTIALIASFORCEENABLE                3
#define D3D_REG_BIT_ANTIALIASCONVOLUTIONFASTMODE        4
#define D3D_REG_BIT_CKCOMPATABILITYENABLE               5
#define D3D_REG_BIT_CONTROLTRAFFICENABLE                6
// unused                       7
#define D3D_REG_BIT_DIRECTMAPENABLE                     8
// unused                       9
#define D3D_REG_BIT_ENFORCESTRICTTRILINEAR              10
// unused                       11
#define D3D_REG_BIT_FLUSHAFTERBLITENABLE                12
#define D3D_REG_BIT_FOGTABLEENABLE                      13
#define D3D_REG_BIT_FORCEBLITWAITFLAGENABLE             14
#define D3D_REG_BIT_LIMITQUEUEDFBBLITSENABLE            15
// unused                       16
#define D3D_REG_BIT_LOGOENABLE                          17
#ifdef TEXFORMAT_CRD
// unused                       18
#else
#define D3D_REG_BIT_PALETTEENABLE                       18
#endif
#define D3D_REG_BIT_SQUASHWENABLE                       19
#define D3D_REG_BIT_SSYNCENABLE                         20
#ifdef TEXFORMAT_CRD
// unused                       21
#else
#define D3D_REG_BIT_TEXTURECOMPRESSIONENABLE            21
#endif
// unused                       22
#define D3D_REG_BIT_TEXTUREMANAGEMENTENABLE             23
#define D3D_REG_BIT_TILINGENABLE                        24
#define D3D_REG_BIT_ZCULLENABLE                         25
#define D3D_REG_BIT_USERMIPMAPENABLE                    26
#define D3D_REG_BIT_VIDEOTEXTUREENABLE                  27
#define D3D_REG_BIT_VS_HOS_EMULATION                    28
#define D3D_REG_BIT_WBUFFERENABLE                       29
#define D3D_REG_BIT_Z24ENABLE                           30
#define D3D_REG_BIT_ZCOMPRESSENABLE                     31

// ALTERNATEZENABLE
#define D3D_REG_ALTERNATEZENABLE_STRING                 "ALTERNATEZENABLE"
#define D3D_REG_ALTERNATEZENABLE_MASK                   (1 << D3D_REG_BIT_ALTERNATEZENABLE)
#define D3D_REG_ALTERNATEZENABLE_DISABLE                (0 << D3D_REG_BIT_ALTERNATEZENABLE)
#define D3D_REG_ALTERNATEZENABLE_ENABLE                 (1 << D3D_REG_BIT_ALTERNATEZENABLE)

// ANTIALIASENABLE
#define D3D_REG_ANTIALIASENABLE_STRING                  "ANTIALIASENABLE"
#define D3D_REG_ANTIALIASENABLE_MASK                    (1 << D3D_REG_BIT_ANTIALIASENABLE)         // Enable Anti-Aliasing support.
#define D3D_REG_ANTIALIASENABLE_DISABLE                 (0 << D3D_REG_BIT_ANTIALIASENABLE)         // Disable Anti-Aliasing support.
#define D3D_REG_ANTIALIASENABLE_ENABLE                  (1 << D3D_REG_BIT_ANTIALIASENABLE)         // Enable Anti-Aliasing support.

// ANTIALIASDYNAMICENABLE
#define D3D_REG_ANTIALIASDYNAMICENABLE_STRING           "ANTIALIASDYNAMICENABLE"
#define D3D_REG_ANTIALIASDYNAMICENABLE_MASK             (1 << D3D_REG_BIT_ANTIALIASDYNAMICENABLE)  // Enable Dynamic Anti-Aliasing support.
#define D3D_REG_ANTIALIASDYNAMICENABLE_DISABLE          (0 << D3D_REG_BIT_ANTIALIASDYNAMICENABLE)  // Disable Dynamic Anti-Aliasing support.
#define D3D_REG_ANTIALIASDYNAMICENABLE_ENABLE           (1 << D3D_REG_BIT_ANTIALIASDYNAMICENABLE)  // Enable Dynamic Anti-Aliasing support.

// ANTIALIASFORCEENABLE
#define D3D_REG_ANTIALIASFORCEENABLE_STRING             "ANTIALIASFORCEENABLE"
#define D3D_REG_ANTIALIASFORCEENABLE_MASK               (1 << D3D_REG_BIT_ANTIALIASFORCEENABLE)
#define D3D_REG_ANTIALIASFORCEENABLE_DISABLE            (0 << D3D_REG_BIT_ANTIALIASFORCEENABLE)
#define D3D_REG_ANTIALIASFORCEENABLE_ENABLE             (1 << D3D_REG_BIT_ANTIALIASFORCEENABLE)

// ANTIALIASCONVOLUTIONFASTMODE
#define D3D_REG_ANTIALIASCONVOLUTIONFASTMODE_STRING     "ANTIALIASCONVOLUTIONFASTMODE"
#define D3D_REG_ANTIALIASCONVOLUTIONFASTMODE_MASK       (1 << D3D_REG_BIT_ANTIALIASCONVOLUTIONFASTMODE)
#define D3D_REG_ANTIALIASCONVOLUTIONFASTMODE_DISABLE    (0 << D3D_REG_BIT_ANTIALIASCONVOLUTIONFASTMODE)
#define D3D_REG_ANTIALIASCONVOLUTIONFASTMODE_ENABLE     (1 << D3D_REG_BIT_ANTIALIASCONVOLUTIONFASTMODE)

// COLORKEYCOMPATIBILITYENABLE
#define D3D_REG_CKCOMPATABILITYENABLE_STRING            "COLORKEYCOMPATABILITYENABLE"
#define D3D_REG_CKCOMPATABILITYENABLE_MASK              (1 << D3D_REG_BIT_CKCOMPATABILITYENABLE)
#define D3D_REG_CKCOMPATABILITYENABLE_DISABLE           (0 << D3D_REG_BIT_CKCOMPATABILITYENABLE)
#define D3D_REG_CKCOMPATABILITYENABLE_ENABLE            (1 << D3D_REG_BIT_CKCOMPATABILITYENABLE)

// CONTROLTRAFFIC
#define D3D_REG_CONTROLTRAFFICENABLE_STRING             "CONTROLTRAFFIC"
#define D3D_REG_CONTROLTRAFFICENABLE_MASK               (1 << D3D_REG_BIT_CONTROLTRAFFICENABLE)
#define D3D_REG_CONTROLTRAFFICENABLE_DISABLE            (0 << D3D_REG_BIT_CONTROLTRAFFICENABLE)
#define D3D_REG_CONTROLTRAFFICENABLE_ENABLE             (1 << D3D_REG_BIT_CONTROLTRAFFICENABLE)

// DIRECTMAPENABLE
#define D3D_REG_DIRECTMAPENABLE_STRING                  "DIRECTMAPENABLE"
#define D3D_REG_DIRECTMAPENABLE_MASK                    (1 << D3D_REG_BIT_DIRECTMAPENABLE)
#define D3D_REG_DIRECTMAPENABLE_DISABLE                 (0 << D3D_REG_BIT_DIRECTMAPENABLE)
#define D3D_REG_DIRECTMAPENABLE_ENABLE                  (1 << D3D_REG_BIT_DIRECTMAPENABLE)

// ENFORCESTRICTTRILINEAR
#define D3D_REG_ENFORCESTRICTTRILINEAR_STRING           "ENFORCESTRICTTRILINEAR"
#define D3D_REG_ENFORCESTRICTTRILINEAR_MASK             (1 << D3D_REG_BIT_ENFORCESTRICTTRILINEAR)
#define D3D_REG_ENFORCESTRICTTRILINEAR_DISABLE          (0 << D3D_REG_BIT_ENFORCESTRICTTRILINEAR)
#define D3D_REG_ENFORCESTRICTTRILINEAR_ENABLE           (1 << D3D_REG_BIT_ENFORCESTRICTTRILINEAR)

// FLUSHAFTERBLITENABLE
#define D3D_REG_FLUSHAFTERBLITENABLE_STRING             "FLUSHAFTERBLITENABLE"
#define D3D_REG_FLUSHAFTERBLITENABLE_MASK               (1 << D3D_REG_BIT_FLUSHAFTERBLITENABLE)
#define D3D_REG_FLUSHAFTERBLITENABLE_DISABLE            (0 << D3D_REG_BIT_FLUSHAFTERBLITENABLE)
#define D3D_REG_FLUSHAFTERBLITENABLE_ENABLE             (1 << D3D_REG_BIT_FLUSHAFTERBLITENABLE)

// FOGTABLEENABLE
#define D3D_REG_FOGTABLEENABLE_STRING                   "FOGTABLEENABLE"
#define D3D_REG_FOGTABLEENABLE_MASK                     (1 << D3D_REG_BIT_FOGTABLEENABLE)       // Enable Fog Table support
#define D3D_REG_FOGTABLEENABLE_DISABLE                  (0 << D3D_REG_BIT_FOGTABLEENABLE)       // Disable Fog Table support.
#define D3D_REG_FOGTABLEENABLE_ENABLE                   (1 << D3D_REG_BIT_FOGTABLEENABLE)       // Enable Fog Table support

// FORCEBLITWAITFLAGENABLE
#define D3D_REG_FORCEBLITWAITFLAGENABLE_STRING          "FORCEBLITWAITFLAGENABLE"
#define D3D_REG_FORCEBLITWAITFLAGENABLE_MASK            (1 << D3D_REG_BIT_FORCEBLITWAITFLAGENABLE)
#define D3D_REG_FORCEBLITWAITFLAGENABLE_DISABLE         (0 << D3D_REG_BIT_FORCEBLITWAITFLAGENABLE)
#define D3D_REG_FORCEBLITWAITFLAGENABLE_ENABLE          (1 << D3D_REG_BIT_FORCEBLITWAITFLAGENABLE)

// LIMITQUEUEDFBBLITSENABLE
#define D3D_REG_LIMITQUEUEDFBBLITSENABLE_STRING         "LIMITQUEUEDFBBLITSENABLE"
#define D3D_REG_LIMITQUEUEDFBBLITSENABLE_MASK           (1 << D3D_REG_BIT_LIMITQUEUEDFBBLITSENABLE)
#define D3D_REG_LIMITQUEUEDFBBLITSENABLE_DISABLE        (0 << D3D_REG_BIT_LIMITQUEUEDFBBLITSENABLE)
#define D3D_REG_LIMITQUEUEDFBBLITSENABLE_ENABLE         (1 << D3D_REG_BIT_LIMITQUEUEDFBBLITSENABLE)

// LOGOENABLE
#define D3D_REG_LOGOENABLE_STRING                       "LOGOENABLE"
#define D3D_REG_LOGOENABLE_MASK                         (1 << D3D_REG_BIT_LOGOENABLE)            // Enable NVIDIA logo
#define D3D_REG_LOGOENABLE_DISABLE                      (0 << D3D_REG_BIT_LOGOENABLE)            // Disable NVIDIA logo
#define D3D_REG_LOGOENABLE_ENABLE                       (1 << D3D_REG_BIT_LOGOENABLE)            // Enable NVIDIA logo

#ifndef TEXFORMAT_CRD
// PALETTEENABLE
#define D3D_REG_PALETTEENABLE_STRING                    "PALETTEENABLE"
#define D3D_REG_PALETTEENABLE_MASK                      (1 << D3D_REG_BIT_PALETTEENABLE)
#define D3D_REG_PALETTEENABLE_DISABLE                   (0 << D3D_REG_BIT_PALETTEENABLE)
#define D3D_REG_PALETTEENABLE_ENABLE                    (1 << D3D_REG_BIT_PALETTEENABLE)
#endif

// SQUASHW
#define D3D_REG_SQUASHW_STRING                          "SQUASHW"
#define D3D_REG_SQUASHW_MASK                            (1 << D3D_REG_BIT_SQUASHWENABLE)
#define D3D_REG_SQUASHW_DISABLE                         (0 << D3D_REG_BIT_SQUASHWENABLE)
#define D3D_REG_SQUASHW_ENABLE                          (1 << D3D_REG_BIT_SQUASHWENABLE)

// SSYNCENABLE
// This is a major hack to work around input lag in stupid applications that
// want to use blits instead of flips to do there screen updates but then don't
// make any getblitstatus calls to see if the blit has completed before beginning
// to render the next frame.
// This is not something that you want to have enabled unless you absolutely need
// to have it enabled.
#define D3D_REG_SSYNCENABLE_STRING                      "SCENESYNCENABLE"
#define D3D_REG_SSYNCENABLE_MASK                        (1 << D3D_REG_BIT_SSYNCENABLE)
#define D3D_REG_SSYNCENABLE_DISABLE                     (0 << D3D_REG_BIT_SSYNCENABLE)
#define D3D_REG_SSYNCENABLE_ENABLE                      (1 << D3D_REG_BIT_SSYNCENABLE)

#ifndef TEXFORMAT_CRD
// TEXTURECOMPRESSIONENABLE
#define D3D_REG_TEXTURECOMPRESSIONENABLE_STRING         "TEXTURECOMPRESSIONENABLE"
#define D3D_REG_TEXTURECOMPRESSIONENABLE_MASK           (1 << D3D_REG_BIT_TEXTURECOMPRESSIONENABLE)
#define D3D_REG_TEXTURECOMPRESSIONENABLE_DISABLE        (0 << D3D_REG_BIT_TEXTURECOMPRESSIONENABLE)
#define D3D_REG_TEXTURECOMPRESSIONENABLE_ENABLE         (1 << D3D_REG_BIT_TEXTURECOMPRESSIONENABLE)
#endif

// TEXTUREMANAGEMENTENABLE
#define D3D_REG_TEXTUREMANAGEMENTENABLE_STRING          "TEXTUREMANAGEMENTENABLE"
#define D3D_REG_TEXTUREMANAGEMENTENABLE_MASK            (1 << D3D_REG_BIT_TEXTUREMANAGEMENTENABLE)
#define D3D_REG_TEXTUREMANAGEMENTENABLE_DISABLE         (0 << D3D_REG_BIT_TEXTUREMANAGEMENTENABLE)
#define D3D_REG_TEXTUREMANAGEMENTENABLE_ENABLE          (1 << D3D_REG_BIT_TEXTUREMANAGEMENTENABLE)

// TILINGENABLE
#define D3D_REG_TILINGENABLE_STRING                     "TILINGENABLE"
#define D3D_REG_TILINGENABLE_MASK                       (1 << D3D_REG_BIT_TILINGENABLE)
#define D3D_REG_TILINGENABLE_DISABLE                    (0 << D3D_REG_BIT_TILINGENABLE)
#define D3D_REG_TILINGENABLE_ENABLE                     (1 << D3D_REG_BIT_TILINGENABLE)

// ZCULLENABLE
#define D3D_REG_ZCULLENABLE_STRING                      "ZCULLENABLE"
#define D3D_REG_ZCULLENABLE_MASK                        (1 << D3D_REG_BIT_ZCULLENABLE)
#define D3D_REG_ZCULLENABLE_DISABLE                     (0 << D3D_REG_BIT_ZCULLENABLE)
#define D3D_REG_ZCULLENABLE_ENABLE                      (1 << D3D_REG_BIT_ZCULLENABLE)

// USERMIPMAPENABLE - enable User supplied mipmaps
#define D3D_REG_USERMIPMAPENABLE_STRING                 "USERMIPMAPENABLE"
#define D3D_REG_USERMIPMAPENABLE_MASK                   (1 << D3D_REG_BIT_USERMIPMAPENABLE)
#define D3D_REG_USERMIPMAPENABLE_DISABLE                (0 << D3D_REG_BIT_USERMIPMAPENABLE)
#define D3D_REG_USERMIPMAPENABLE_ENABLE                 (1 << D3D_REG_BIT_USERMIPMAPENABLE)

// VIDEOTEXTUREENABLE - enable texturing from video memory
#define D3D_REG_VIDEOTEXTUREENABLE_STRING               "VIDEOTEXTUREENABLE"
#define D3D_REG_VIDEOTEXTUREENABLE_MASK                 (1 << D3D_REG_BIT_VIDEOTEXTUREENABLE)
#define D3D_REG_VIDEOTEXTUREENABLE_DISABLE              (0 << D3D_REG_BIT_VIDEOTEXTUREENABLE)
#define D3D_REG_VIDEOTEXTUREENABLE_ENABLE               (1 << D3D_REG_BIT_VIDEOTEXTUREENABLE)

// VertexShader/Higher order surface emulation for NV10/NV15 class parts
#define D3D_REG_BIT_VS_HOS_EMULATION                    28
#define D3D_REG_VS_HOS_EMULATION_STRING                 "VS_HOS_EMULATION"
#define D3D_REG_VS_HOS_EMULATION_MASK                   (1 << D3D_REG_BIT_VS_HOS_EMULATION)
#define D3D_REG_VS_HOS_EMULATION_DISABLE                (0 << D3D_REG_BIT_VS_HOS_EMULATION)
#define D3D_REG_VS_HOS_EMULATION_ENABLE                 (1 << D3D_REG_BIT_VS_HOS_EMULATION)

// WBUFFERING
#define D3D_REG_WBUFFERENABLE_STRING                    "WENABLE"
#define D3D_REG_WBUFFERENABLE_MASK                      (1 << D3D_REG_BIT_WBUFFERENABLE)
#define D3D_REG_WBUFFERENABLE_DISABLE                   (0 << D3D_REG_BIT_WBUFFERENABLE)
#define D3D_REG_WBUFFERENABLE_ENABLE                    (1 << D3D_REG_BIT_WBUFFERENABLE)

// Z24ENABLE
#define D3D_REG_Z24ENABLE_STRING                        "Z24ENABLE"
#define D3D_REG_Z24ENABLE_MASK                          (1 << D3D_REG_BIT_Z24ENABLE)
#define D3D_REG_Z24ENABLE_DISABLE                       (0 << D3D_REG_BIT_Z24ENABLE)
#define D3D_REG_Z24ENABLE_ENABLE                        (1 << D3D_REG_BIT_Z24ENABLE)

// ZCOMPRESSENABLE
#define D3D_REG_ZCOMPRESSENABLE_STRING                  "ZCOMPRESSENABLE"
#define D3D_REG_ZCOMPRESSENABLE_MASK                    (1 << D3D_REG_BIT_ZCOMPRESSENABLE)
#define D3D_REG_ZCOMPRESSENABLE_DISABLE                 (0 << D3D_REG_BIT_ZCOMPRESSENABLE)
#define D3D_REG_ZCOMPRESSENABLE_ENABLE                  (1 << D3D_REG_BIT_ZCOMPRESSENABLE)

//// not currently in use - rel6 maybe?
// ANTIALIASENABLE
#define D3D_REG_ANTIALIASDYNAMICENABLE_STRING           "ANTIALIASDYNAMICENABLE"
#define D3D_REG_ANTIALIASDYNAMICENABLE_MASK             (1 << D3D_REG_BIT_ANTIALIASDYNAMICENABLE)         // Enable Dynamic Anti-Aliasing support.
#define D3D_REG_ANTIALIASDYNAMICENABLE_DISABLE          (0 << D3D_REG_BIT_ANTIALIASDYNAMICENABLE)         // Disable Dynamic Anti-Aliasing support.
#define D3D_REG_ANTIALIASDYNAMICENABLE_ENABLE           (1 << D3D_REG_BIT_ANTIALIASDYNAMICENABLE)         // Enable Dynamic Anti-Aliasing support.

// Default values for registry configurable driver settings.
#define D3D_REG_DEFAULT_ALTERNATEZENABLE                D3D_REG_ALTERNATEZENABLE_DISABLE
#define D3D_REG_DEFAULT_ANTIALIASENABLE                 D3D_REG_ANTIALIASENABLE_ENABLE            // enable Anti Aliasing.
#define D3D_REG_DEFAULT_ANTIALIASDYNAMICENABLE          D3D_REG_ANTIALIASDYNAMICENABLE_ENABLE     // enable dynamic Anti Aliasing.
#define D3D_REG_DEFAULT_ANTIALIASFORCEENABLE            D3D_REG_ANTIALIASFORCEENABLE_DISABLE      // Disable forced Anti Aliasing.
#define D3D_REG_DEFAULT_ANTIALIASCONVOLUTIONFASTMODE    D3D_REG_ANTIALIASCONVOLUTIONFASTMODE_DISABLE // Disable the fast mode for 5x and 9x AA by default
#define D3D_REG_DEFAULT_CKCOMPATABILITYENABLE           D3D_REG_CKCOMPATABILITYENABLE_ENABLE      // Perform application colorkey fixups.
#define D3D_REG_DEFAULT_CONTROLTRAFFICENABLE            D3D_REG_CONTROLTRAFFICENABLE_ENABLE
#define D3D_REG_DEFAULT_DIRECTMAPENABLE                 D3D_REG_DIRECTMAPENABLE_ENABLE            // Enable Direct Mapping of Texture Combiners by default.
#define D3D_REG_DEFAULT_ENFORCESTRICTTRILINEAR          D3D_REG_ENFORCESTRICTTRILINEAR_ENABLE     // Enforces multitexture-trilinear ot NOT use dithering approximation
#define D3D_REG_DEFAULT_FLUSHAFTERBLITENABLE            D3D_REG_FLUSHAFTERBLITENABLE_DISABLE      // Disable flush after blit on wait flag
#define D3D_REG_DEFAULT_FOGTABLEENABLE                  D3D_REG_FOGTABLEENABLE_ENABLE             // Enable software implemented fog table support.
#define D3D_REG_DEFAULT_FORCEBLITWAITFLAGENABLE         D3D_REG_FORCEBLITWAITFLAGENABLE_DISABLE   // Disable force the BLT_WAIT flag
#define D3D_REG_DEFAULT_LIMITQUEUEDFBBLITSENABLE        D3D_REG_LIMITQUEUEDFBBLITSENABLE_DISABLE  // Disable limiting the max number of queued FB blits
#define D3D_REG_DEFAULT_LOGOENABLE                      D3D_REG_LOGOENABLE_DISABLE
#ifndef TEXFORMAT_CRD
#define D3D_REG_DEFAULT_PALETTEENABLE                   D3D_REG_PALETTEENABLE_ENABLE              // Enable 8-bit textures (relevant only >=DX8. always disabled for <=DX7)
#endif
#define D3D_REG_DEFAULT_SQUASHW                         D3D_REG_SQUASHW_DISABLE
#define D3D_REG_DEFAULT_SSYNCENABLE                     D3D_REG_SSYNCENABLE_DISABLE               // MAJOR APP HACK should never be enbled by default!!
#ifndef TEXFORMAT_CRD
#define D3D_REG_DEFAULT_TEXTURECOMPRESSIONENABLE        D3D_REG_TEXTURECOMPRESSIONENABLE_ENABLE   // Enable Texture compression so we can 'disable' for badly behaving apps
#endif
#define D3D_REG_DEFAULT_TEXTUREMANAGEMENTENABLE         D3D_REG_TEXTUREMANAGEMENTENABLE_ENABLE    // enable driver-based texture management
#define D3D_REG_DEFAULT_TILINGENABLE                    D3D_REG_TILINGENABLE_ENABLE
#define D3D_REG_DEFAULT_ZCULLENABLE                     D3D_REG_ZCULLENABLE_ENABLE                // Enable z occlusion culling by default (applicable to nv20 ff.)
#define D3D_REG_DEFAULT_USERMIPMAPENABLE                D3D_REG_USERMIPMAPENABLE_ENABLE           // Enable use of user supplied mip maps
#define D3D_REG_DEFAULT_VIDEOTEXTUREENABLE              D3D_REG_VIDEOTEXTUREENABLE_ENABLE         // Enable texturing from video memory
#define D3D_REG_DEFAULT_WBUFFERENABLE                   D3D_REG_WBUFFERENABLE_ENABLE
#define D3D_REG_DEFAULT_Z24ENABLE                       D3D_REG_Z24ENABLE_ENABLE                  // Enable 24bit z-exports by default
#define D3D_REG_DEFAULT_ZCOMPRESSENABLE                 D3D_REG_ZCOMPRESSENABLE_ENABLE            // Enable z compression by default (applicable to nv20 ff.)

// ----------- Non-boolean settings with enumerated values ------------------

// AntiAliasQuality definitions.
#define D3D_REG_ANTIALIASQUALITY_STRING                 "ANTIALIASQUALITY"
#define D3D_REG_ANTIALIASQUALITY_MIN                    0
#define D3D_REG_ANTIALIASQUALITY_MAX                    7

//// not currently in use - rel6 maybe?
// AntiAliasDynamicFPS definitions.
#define D3D_REG_ANTIALIASDYNAMICFPS_STRING              "ANTIALIASDYNAMICFPS"
#define D3D_REG_ANTIALIASDYNAMICFPS_MIN                 10
#define D3D_REG_ANTIALIASDYNAMICFPS_MAX                 500

// CAPTURE CONFIG
#define D3D_REG_CAPTURECONFIG_STRING                    "CAPTURECONFIG"
#define D3D_REG_CAPTURECONFIG_ALLOC4X                   0x01
#define D3D_REG_CAPTURECONFIG_FORCEPCI                  0x02

// CAPTUREENABLE
#define D3D_REG_CAPTUREENABLE_STRING                    "CAPTUREENABLE"
#define D3D_REG_CAPTUREENABLE_DISABLE                   0x00
#define D3D_REG_CAPTUREENABLE_RECORD                    0x01
#define D3D_REG_CAPTUREENABLE_PLAY                      0x02

// Colorkey Reference Values range.
#define D3D_REG_CKREF_STRING                            "CKREF"
#define D3D_REG_CKREF_MIN                               0x00
#define D3D_REG_CKREF_MAX                               0x7F

// Force Anisotropic definitions.
#define D3D_REG_ANISOTROPICLEVEL_STRING                 "ANISOTROPICLEVEL"
#define D3D_REG_ANISOTROPICLEVEL_PASSIVE                0xFFFFFFFF  // do what the app wants (else force to the value specified)

// MipMapDitherEnable definitions.
#define D3D_REG_MIPMAPDITHERMODE_STRING                 "ANISOTROPIC4TAP"
#define D3D_REG_MIPMAPDITHERMODE_DISABLE                0           // Disable mipmap dithering (true trilinear)
#define D3D_REG_MIPMAPDITHERMODE_ENABLE                 1           // Enable mipmap dithering (fake trilinear)
#define D3D_REG_MIPMAPDITHERMODE_SMART                  2           // smart dithering (enabled at higher resolutions)
#define D3D_REG_MIPMAPDITHERMODE_MAX                    2

#ifdef TEXFORMAT_CRD
// surface formats exported
#define D3D_REG_SURFACEFORMATSDX7_STRING                "SURFACEFORMATSDX7"
#define D3D_REG_SURFACEFORMATSDX8_STRING                "SURFACEFORMATSDX8"
#define D3D_REG_SURFACEFORMATS_BASIC                    0x00000001
#define D3D_REG_SURFACEFORMATS_BUMPMAP                  0x00000002
#define D3D_REG_SURFACEFORMATS_DXT                      0x00000004
#define D3D_REG_SURFACEFORMATS_PALETTIZED               0x00000008
#define D3D_REG_SURFACEFORMATS_NVXN                     0x00000010
#define D3D_REG_SURFACEFORMATS_HILO                     0x00000020
#define D3D_REG_SURFACEFORMATS_ZETA                     0x00000040
#define D3D_REG_SURFACEFORMATS_MISC                     0x00000080
#endif  // TEXFORMAT_CRD

// TexelAlignment definitions.
// Bit flags (bit 0 = ZOH, bit 1 = FOH, bit 2 = Texel Origin)
#define D3D_REG_TEXELALIGNMENT_STRING                   "TEXELALIGNMENT"
#define D3D_REG_TEXELALIGNMENT_ZOH_CENTER               0x00
#define D3D_REG_TEXELALIGNMENT_ZOH_CORNER               0x01
#define D3D_REG_TEXELALIGNMENT_ZOH_MASK                 0x01
#define D3D_REG_TEXELALIGNMENT_FOH_CENTER               0x00
#define D3D_REG_TEXELALIGNMENT_FOH_CORNER               0x02
#define D3D_REG_TEXELALIGNMENT_FOH_MASK                 0x02
#define D3D_REG_TEXELALIGNMENT_TEXEL_CENTER             0x00
#define D3D_REG_TEXELALIGNMENT_TEXEL_CORNER             0x04
#define D3D_REG_TEXELALIGNMENT_TEXEL_MASK               0x04
#define D3D_REG_TEXELALIGNMENT_MAX                      0x07

// TEXTUREMANAGESTRATEGY
#define D3D_REG_TEXTUREMANAGESTRATEGY_STRING            "TEXTUREMANAGESTRATEGY"
#define D3D_REG_TEXTUREMANAGESTRATEGY_AGGRESSIVE        0x1
#define D3D_REG_TEXTUREMANAGESTRATEGY_LAZY              0x0

// ValidateZMethod definitions.
#define D3D_REG_VALIDATEZMETHOD_STRING                  "VALIDATEZMETHOD"
#define D3D_REG_VALIDATEZMETHOD_FLEXIBLE                0           // Driver will match z-buffer to rendering depth.
#define D3D_REG_VALIDATEZMETHOD_SRTRETURNNOTHANDLED     1           // Same as FLEXIBLE but returns NOTHANDLED from Set Render Target.
#define D3D_REG_VALIDATEZMETHOD_STRICT                  2           // Z-Buffer must always match rendering depth.
#define D3D_REG_VALIDATEZMETHOD_MIXED                   3           // DX6 applications = STRICT, DX5 applications = flexible
#define D3D_REG_VALIDATEZMETHOD_MAX                     3

// V-Sync modes
#define D3D_REG_VSYNCMODE_STRING                        "VSYNCMODE"
#define D3D_REG_VSYNCMODE_PASSIVE                       0           // just do what the app tells us to do
#define D3D_REG_VSYNCMODE_FORCEOFF                      1           // override app and don't vsync
#define D3D_REG_VSYNCMODE_FORCEON                       2           // override app and vsync
#define D3D_REG_VSYNCMODE_MAX                           2

// WFormat definitions.
#define D3D_REG_WFORMAT16_STRING                        "W16FORMAT"
#define D3D_REG_WFORMAT32_STRING                        "W32FORMAT"
#define D3D_REG_WFORMAT_FIXED                           0x1
#define D3D_REG_WFORMAT_FLOAT                           0x2
#define D3D_REG_WFORMAT_MIN                             0x1
#define D3D_REG_WFORMAT_MAX                             0x2

// defaults for the non-boolean enumerated values
#define D3D_REG_DEFAULT_CAPTURCONFIG                    (D3D_REG_CAPTURECONFIG_ALLOC4X | D3D_REG_CAPTURECONFIG_FORCEPCI)
#define D3D_REG_DEFAULT_CAPTURENABLE                    D3D_REG_CAPTUREENABLE_RECORD
#define D3D_REG_DEFAULT_ANISOTROPICLEVEL                D3D_REG_ANISOTROPICLEVEL_PASSIVE
#define D3D_REG_DEFAULT_MIPMAPDITHERMODE                D3D_REG_MIPMAPDITHERMODE_SMART          // use true trilinear sometimes
#ifdef TEXFORMAT_CRD
#define D3D_REG_DEFAULT_SURFACEFORMATSDX7               (D3D_REG_SURFACEFORMATS_BASIC | D3D_REG_SURFACEFORMATS_BUMPMAP    |  \
                                                         D3D_REG_SURFACEFORMATS_DXT   | D3D_REG_SURFACEFORMATS_PALETTIZED |  \
                                                         D3D_REG_SURFACEFORMATS_ZETA  | D3D_REG_SURFACEFORMATS_MISC)
#define D3D_REG_DEFAULT_SURFACEFORMATSDX8               (D3D_REG_SURFACEFORMATS_BASIC | D3D_REG_SURFACEFORMATS_BUMPMAP    |  \
                                                         D3D_REG_SURFACEFORMATS_DXT   | D3D_REG_SURFACEFORMATS_PALETTIZED |  \
                                                         D3D_REG_SURFACEFORMATS_NVXN  | D3D_REG_SURFACEFORMATS_HILO       |  \
                                                         D3D_REG_SURFACEFORMATS_ZETA  | D3D_REG_SURFACEFORMATS_MISC)
#endif  // TEXFORMAT_CRD
#define D3D_REG_DEFAULT_TEXELALIGNMENT                  (D3D_REG_TEXELALIGNMENT_ZOH_CORNER | D3D_REG_TEXELALIGNMENT_FOH_CORNER | D3D_REG_TEXELALIGNMENT_TEXEL_CENTER)
#define D3D_REG_DEFAULT_TEXTUREMANAGESTRATEGY           D3D_REG_TEXTUREMANAGESTRATEGY_AGGRESSIVE
#define D3D_REG_DEFAULT_VALIDATEZMETHOD                 D3D_REG_VALIDATEZMETHOD_FLEXIBLE        // D3D_REG_VALIDATEZMETHOD_MIXED
#define D3D_REG_DEFAULT_VSYNCMODE                       D3D_REG_VSYNCMODE_PASSIVE
#define D3D_REG_DEFAULT_W16FORMAT                       D3D_REG_WFORMAT_FIXED
#define D3D_REG_DEFAULT_W32FORMAT                       D3D_REG_WFORMAT_FLOAT

// ------- Non-boolean settings with non-enumerated values ------------------

// NOT of address of pointer to command-line string
#define D3D_REG_AACOMPATIBILITYBITS_STRING              "AACOMPATIBILITYBITS"

// Reads nvCelsiusAACompatibility.h
#define D3D_REG_AAREADCOMPATIBILITYFILE_STRING          "AAREADCOMPATIBILITYFILE"

// AntiAliasQuality definitions.
#define D3D_REG_ANTIALIASQUALITY_STRING                 "ANTIALIASQUALITY"
#define D3D_REG_ANTIALIASQUALITY_MIN                    0
#define D3D_REG_ANTIALIASQUALITY_MAX                    7

// AntiAliasDynamicFPS definitions.
#define D3D_REG_ANTIALIASDYNAMICFPS_STRING              "ANTIALIASDYNAMICFPS"
#define D3D_REG_ANTIALIASDYNAMICFPS_MIN                 10
#define D3D_REG_ANTIALIASDYNAMICFPS_MAX                 500

// AGP texture cutoff
#define D3D_REG_AGPTEXCUTOFF_STRING                     "AGPTEXCUTOFF"

// starting number for played capture files
#define D3D_REG_CAPTUREPLAYFILENUM_STRING               "CAPTUREPLAYFILENUM"

// starting number for recorded capture files
#define D3D_REG_CAPTURERECORDFILENUM_STRING             "CAPTURERECORDFILENUM"

// Colorkey Reference Values range.
#define D3D_REG_CKREF_STRING                            "CKREF"
#define D3D_REG_CKREF_MIN                               0x00
#define D3D_REG_CKREF_MAX                               0x7F

// D3D contexts
#define D3D_REG_D3DCONTEXTMAX_STRING                    "D3DCONTEXTMAX"

// D3D contexts
#define D3D_REG_D3DTEXTUREMAX_STRING                    "D3DTEXTUREMAX"

// DPF debug level
#define D3D_REG_DEBUGLEVEL_STRING                       "DEBUGLEVEL"

// LOD bias adjust
#define D3D_REG_LODBIAS_STRING                          "LODBIASADJUST"

// minimum video texture size
#define D3D_REG_MINVIDTEXSIZE_STRING                    "MINVIDEOTEXSIZE"

// size of the pci texture heap. non-zero value will override value determined by the driver
#define D3D_REG_PCITEXHEAPSIZE_STRING                   "PCITEXHEAPSIZE"

// performance strategy
#define D3D_REG_PERFSTRATEGYOR_STRING                   "PSOR"
#define D3D_REG_PERFSTRATEGYAND_STRING                  "PSAND"

// Prerender limits
#define D3D_REG_PRERENDERLIMIT_STRING                   "PRERENDERLIMIT"
#define D3D_REG_PRERENDERLIMIT_MIN                      1
#define D3D_REG_PRERENDERLIMIT_MAX                      1000

// maximum push buffer size
#define D3D_REG_PUSHBUFFERSIZEMAX_STRING                "DMAPUSHBUFFERSIZEMAX"

// WScale 16/24 definitions.
#define D3D_REG_WSCALE16_STRING                         "WSCALE16"
#define D3D_REG_WSCALE24_STRING                         "WSCALE24"
#define D3D_REG_WSCALE16_MAX                            0x00010000
#define D3D_REG_WSCALE24_MAX                            0x01000000

// positively shift z to accomodate apps that give us slightly negtive z
#define D3D_REG_ZBIAS_STRING                            "ZBIAS"

// defaults for the non-boolean, non-enumerated values
#define D3D_REG_DEFAULT_AACOMPATIBILITYBITS             0
#define D3D_REG_DEFAULT_AAREADCOMPATIBILITYFILE         0
#define D3D_REG_DEFAULT_ANTIALIASQUALITY                1
#define D3D_REG_DEFAULT_ANTIALIASDYNAMICFPS             (1000 / 30)
#define D3D_REG_DEFAULT_AGPTEXCUTOFF                    1024
#define D3D_REG_DEFAULT_CAPTUREPLAYFILENUM              0
#define D3D_REG_DEFAULT_CAPTURERECORDFILENUM            0
#define D3D_REG_DEFAULT_CKREF                           D3D_REG_CKREF_MIN
#define D3D_REG_DEFAULT_D3DCONTEXTMAX                   64                  // D3D Context heap allocation max.
#define D3D_REG_DEFAULT_D3DTEXTUREMAX                   1024                // D3D Texture heap allocation max.
#define D3D_REG_DEFAULT_DEBUGLEVEL                      0                   // no debug output
#define D3D_REG_DEFAULT_LODBIASADJUST                   0
#define D3D_REG_DEFAULT_MINVIDEOTEXSIZE                 1                   // minimum texture size to be placed in video memory.
#define D3D_REG_DEFAULT_PCITEXHEAPSIZE                  0                   // size of PCI texture heap. (0 => driver-determined value)
#define D3D_REG_DEFAULT_PRERENDERLIMIT                  3
#define D3D_REG_DEFAULT_PUSHBUFFERSIZEMAX               0x40000             // maximum push buffer size.
#define D3D_REG_DEFAULT_WSCALE16                        D3D_REG_WSCALE16_MAX
#define D3D_REG_DEFAULT_WSCALE24                        D3D_REG_WSCALE24_MAX
#define D3D_REG_DEFAULT_ZBIAS                           0.f

// ------------------------------- Strings ----------------------------------

#define D3D_REG_STRING_LENGTH                           64                  // maximum allowed string length

// push-buffer capture playback file name
#define D3D_REG_CAPTUREPLAYFILENAME_STRING              "CAPTUREPLAYFILENAME"

// push-buffer capture playback path
#define D3D_REG_CAPTUREPLAYPATH_STRING                  "CAPTUREPLAYPATH"

// push-buffer capture recording path
#define D3D_REG_CAPTURERECORDFILENAME_STRING            "CAPTURERECORDFILENAME"

// push-buffer capture recording path
#define D3D_REG_CAPTURERECORDPATH_STRING                "CAPTURERECORDPATH"

// string defaults
#define D3D_REG_DEFAULT_CAPTUREPLAYFILENAME             "capture"
#define D3D_REG_DEFAULT_CAPTUREPLAYPATH                 "c:\\"
#define D3D_REG_DEFAULT_CAPTURERECORDFILENAME           "capture"
#define D3D_REG_DEFAULT_CAPTURERECORDPATH               "c:\\"


//-----------------------------------------------------------------------------
// OpenGL
//-----------------------------------------------------------------------------

#define NV4_REG_OGL_BUFFER_FLIPPING_CONTROL             "FlippingControl"
#define   NV4_REG_OGL_BUFFER_FLIPPING_CONTROL_BLIT      0
#define   NV4_REG_OGL_BUFFER_FLIPPING_CONTROL_FLIP      1
#define   NV4_REG_OGL_BUFFER_FLIPPING_CONTROL_AUTO      2
#define   NV4_REG_OGL_BUFFER_FLIPPING_CONTROL_DEFAULT   NV4_REG_OGL_BUFFER_FLIPPING_CONTROL_AUTO

#define NV_REG_OGL_BUFFER_REGION_EXT                    "BufferRegionExt"
#define   NV_REG_OGL_BUFFER_REGION_EXT_OFF              0
#define   NV_REG_OGL_BUFFER_REGION_EXT_ON               1
#define   NV_REG_OGL_BUFFER_REGION_EXT_DEFAULT          NV_REG_OGL_BUFFER_REGION_EXT_ON

#define NV_REG_OGL_BUFFER_REGION_USE_VIDMEM             "BufferRegionUseVidMem"
#define   NV_REG_OGL_BUFFER_REGION_USE_VIDMEM_OFF       0
#define   NV_REG_OGL_BUFFER_REGION_USE_VIDMEM_ON        1
#define   NV_REG_OGL_BUFFER_REGION_USE_VIDMEM_DEFAULT   NV_REG_OGL_BUFFER_REGION_USE_VIDMEM_ON

#define NV_REG_OGL_DEBUG_RENDERER                       "Renderer"
#define   NV_REG_OGL_DEBUG_RENDERER_NVIDIA              0
#define   NV_REG_OGL_DEBUG_RENDERER_SOFTWARE            1
#define   NV_REG_OGL_DEBUG_RENDERER_MICROSOFT           2
#define   NV_REG_OGL_DEFAULT_DEBUG_RENDERER             NV_REG_OGL_DEBUG_RENDERER_NVIDIA

#define NV_REG_OGL_DEBUG_PMENABLE                       "PMEnable"
#define   NV_REG_OGL_DEBUG_PMENABLE_ENABLED             1
#define   NV_REG_OGL_DEBUG_PMENABLE_DISABLED            0
#define   NV_REG_OGL_DEFAULT_DEBUG_PMENABLE             NV_REG_OGL_DEBUG_PMENABLE_DISABLED

#define NV_REG_OGL_DEFAULT_SWAP_INTERVAL                "DefaultSwapInterval"
#define   NV_REG_OGL_DEFAULT_SWAP_INTERVAL_TEAR         0
#define   NV_REG_OGL_DEFAULT_SWAP_INTERVAL_VSYNC        1
#define   NV_REG_OGL_DEFAULT_SWAP_INTERVAL_DISABLE      0xffffffff
#define   NV_REG_OGL_DEFAULT_SWAP_INTERVAL_DEFAULT      NV_REG_OGL_DEFAULT_SWAP_INTERVAL_VSYNC

#define NV_REG_OGL_MAX_NVACCEL                          "MaxNVACCEL"
#define NV_REG_OGL_DEFAULT_MAX_NVACCEL                  0

#define NV_REG_OGL_NV15_ALINES                          "NV15Alines"
#define   NV_REG_OGL_DEFAULT_NV15_ALINES                0
#define   NV_REG_OGL_NV15_ALINES_ACTUAL                 0
#define   NV_REG_OGL_NV15_ALINES_FORCE                  1
#define   NV_REG_OGL_NV15_ALINES_DISABLE                2

#define NV_REG_OGL_CLIPPED_ALINES                       "77345d17C"
#define   NV_REG_OGL_CLIPPED_ALINES_DEFAULT             0
#define   NV_REG_OGL_CLIPPED_ALINES_ENABLE              1

#define NV_REG_OGL_VERTEX_PROGRAM                       "VertexProgram"
#define   NV_REG_OGL_VERTEX_PROGRAM_OPTIMIZE            0
#define   NV_REG_OGL_VERTEX_PROGRAM_NO_LIVEDEAD         1

#define NV_REG_OGL_VPIPE                                "Vpipe"
#define   NV_REG_OGL_VPIPE_OPTIMIZE                     0
#define   NV_REG_OGL_VPIPE_DISABLE_BATCHED_IMMEDIATE    2
#define   NV_REG_OGL_VPIPE_DISABLE_BATCHED_DLISTS       4
#define   NV_REG_OGL_VPIPE_DISABLE_CDE                  8
#define   NV_REG_OGL_VPIPE_FORCE_GENERIC_CPU            0x0400
#define   NV_REG_OGL_VPIPE_REREAD_REGISTRY_ON_CLEAR     0x8000
#define   NV_REG_OGL_VPIPE_TOGGLE_BATCHING_ON_CLEAR     0x00040000

#define NV_REG_OGL_TRIPLE_BUFFER                        "EnableTripleBuffer"
#define   NV_REG_OGL_TRIPLE_BUFFER_DISABLE              0
#define   NV_REG_OGL_TRIPLE_BUFFER_ENABLE               1
#define   NV_REG_OGL_TRIPLE_BUFFER_DEFAULT              NV_REG_OGL_TRIPLE_BUFFER_DISABLE

#define NV_REG_OGL_WINDOW_FLIPPING                      "EnableWindowFlipping"
#define   NV_REG_OGL_WINDOW_FLIPPING_ENABLE             1
#define   NV_REG_OGL_WINDOW_FLIPPING_DISABLE            0
#define   NV_REG_OGL_WINDOW_FLIPPING_DEFAULT            NV_REG_OGL_WINDOW_FLIPPING_DISABLE
#define   NV_REG_OGL_WINDOW_FLIPPING_DEFAULT_GL         NV_REG_OGL_WINDOW_FLIPPING_ENABLE

#define NV_REG_OGL_OVERLAY_SUPPORT                      "EnableOverlaySupport"
#define NV_REG_OGL_OVERLAY_SUPPORT_OFF                  0
#define NV_REG_OGL_OVERLAY_SUPPORT_ON                   1
#define NV_REG_OGL_OVERLAY_SUPPORT_DEFAULT              NV_REG_OGL_OVERLAY_SUPPORT_OFF

#define NV_REG_OGL_OVERLAY_MERGEBLIT_TIMER_MS           "OverlayMergeBlitTimerMs"
#define NV_REG_OGL_OVERLAY_MERGEBLIT_TIMER_MS_DEFAULT   40 //ms
#define NV_REG_OGL_OVERLAY_MERGEBLIT_TIMER_MS_OFF       0  //0ms => off

#define NV_REG_OGL_STEREO_SUPPORT                       "EnableStereoSupport"
#define NV_REG_OGL_STEREO_SUPPORT_OFF                   0
#define NV_REG_OGL_STEREO_SUPPORT_ON                    1
#define NV_REG_OGL_STEREO_SUPPORT_DEFAULT               NV_REG_OGL_STEREO_SUPPORT_OFF

#define NV_REG_OGL_API_STEREO_MODE                              "APIStereoMode"
#define NV_REG_OGL_API_STEREO_MODE_SHUTTER_GLASSES              0 // stereo through shutter glasses
#define NV_REG_OGL_API_STEREO_MODE_VERTICAL_INTERLACED_RIGHT_0  1 // use vertical interlace monitor, right eye on first column
#define NV_REG_OGL_API_STEREO_MODE_VERTICAL_INTERLACED_LEFT_0   2 // use vertical interlace monitor, left eye on first column
#define NV_REG_OGL_API_STEREO_MODE_TVINVIEW_LEFT_DAC0           3 // use two dacs, dac 0 will show left eye
#define NV_REG_OGL_API_STEREO_MODE_TVINVIEW_RIGHT_DAC0          4 // use two dacs, dac 0 will show right eye
#define NV_REG_OGL_API_STEREO_MODE_DEFAULT                      NV_REG_OGL_API_STEREO_MODE_SHUTTER_GLASSES

// MSchwarzer 9/28/00 these keys are used to enable bundles of bugfixes for special applications
#define NV_REG_OGL_APPLICATION_KEY                      "ApplicationKey"
#define NV_REG_OGL_APPLICATION_KEY_NONE                 0
#define NV_REG_OGL_APPLICATION_KEY_STANDARD             1
#define NV_REG_OGL_APPLICATION_KEY_SOFTIMAGE3D          2
#define NV_REG_OGL_APPLICATION_KEY_3DSMAX               3
#define NV_REG_OGL_APPLICATION_KEY_3DPAINT              4
#define NV_REG_OGL_APPLICATION_KEY_MAYA                 5
#define NV_REG_OGL_APPLICATION_KEY_LIGHTSCAPE           6
#define NV_REG_OGL_APPLICATION_KEY_DEFAULT              NV_REG_OGL_APPLICATION_KEY_NONE

#define NV_REG_OGL_FORCE_16BIT_Z                        "EnableForce16BitZ"
#define NV_REG_OGL_FORCE_16BIT_Z_DISABLE                0
#define NV_REG_OGL_FORCE_16BIT_Z_ENABLE                 1
#define NV_REG_OGL_FORCE_16BIT_Z_DEFAULT                NV_REG_OGL_FORCE_16BIT_Z_DISABLE

#define NV_REG_OGL_SINGLE_BACKDEPTH_BUFFER              "EnableSingleBackDepthBuffer"
#define NV_REG_OGL_SINGLE_BACKDEPTH_DISABLE             0
#define NV_REG_OGL_SINGLE_BACKDEPTH_ENABLE              1
#define NV_REG_OGL_SINGLE_BACKDEPTH_BUFFER_DEFAULT      NV_REG_OGL_SINGLE_BACKDEPTH_DISABLE
#define NV_REG_OGL_SINGLE_BACKDEPTH_BUFFER_DEFAULT_GL   NV_REG_OGL_SINGLE_BACKDEPTH_ENABLE

#define NV_REG_OGL_DMAPUSH_BUFSIZE_WORDS                "CmdBufSizeWords"

#define NV_REG_OGL_DMAPUSH_MIN_PUSH_COUNT               "CmdBufMinWords"

#define NV_REG_OGL_DL_STAGING_BUFFER_SIZE_WORDS         "DLStagingBufferSizeWords"
#define NV_REG_OGL_DL_STAGING_BUFFER_SIZE_WORDS_DEFAULT ((9*1024*1024)/4)

#define NV_REG_OGL_TARGET_FLUSHCOUNT                    "TargetFlushCount"

#define NV_REG_OGL_FORCE_MULTITEX                       "ForceMultiTexture"
#define NV_REG_OGL_DISABLE_MULTITEX                     0
#define NV_REG_OGL_ENABLE_MULTITEX                      1
#define NV_REG_OGL_DEFAULT_MULTITEX                     NV_REG_OGL_ENABLE_MULTITEX

#define NV_REG_OGL_MAX_FRAMES_ALLOWED                   "MaxFramesAllowed"
#define NV_OGL_DEFAULT_MAX_FRAMES_ALLOWED               2
#define NV_MAX_FRAMES_ALLOWED                           10

#define NV_REG_OGL_MAX_TEX_SIZE                         "MaxTexSize"
#define NV_OGL_DEFAULT_MAX_TEX_SIZE                     2048

#define NV_REG_OGL_PCI_TEXHEAP_SIZE                     "MaxPCITexHeapSize"
#define NV_REG_OGL_PCI_TEXHEAP_SIZE_DEFAULT             (5*1024*1024)
#define NV_REG_OGL_PCI_TEXHEAP_SIZE_MAX_NT              (16*1024*1024)

#define NV_REG_MULTI_MONITOR_ADVANCED_ENABLE            "MultiMonAdvEnable"
#define NV_OGL_DEFAULT_MULTI_MONITOR_ADVANCED_ENABLE    0

#define NV_REG_OGL_DEFAULT_LOG_ANISO                    "DefaultLogAniso"
#define NV_REG_OGL_DEFAULT_LOG_ANISO_DEFAULT            0

#define NV_REG_OGL_RENDER_QUALITY_FLAGS                 "RenderQualityFlags"
#define NV_REG_OGL_RENDER_QUALITY_FLAGS_MMDITHER        0x00000001

// XXXmjc delete this flag ASAP!
#define NV_REG_OGL_RENDER_QUALITY_FLAGS_ANISOENABLE     0x00000002

/* Default texel size */
#define NV_REG_OGL_RENDER_QUALITY_FLAGS_TEXBITS_MASK     0x0000000c
#define NV_REG_OGL_RENDER_QUALITY_FLAGS_TEXBITS_AUTO     0x00000000
#define NV_REG_OGL_RENDER_QUALITY_FLAGS_TEXBITS_16       0x00000004
#define NV_REG_OGL_RENDER_QUALITY_FLAGS_TEXBITS_32       0x00000008
#define NV_REG_OGL_RENDER_QUALITY_FLAGS_TEXBITS_RESERVED 0x0000000c

#define NV_REG_OGL_TEX_DIRECT_LOAD                      "TextureDirectLoad"
#define NV_REG_OGL_TEX_DIRECT_LOAD_DISABLE              0
#define NV_REG_OGL_TEX_DIRECT_LOAD_ALLOW                1
#define NV_REG_OGL_TEX_DIRECT_LOAD_FORCE                2
#define NV_REG_OGL_TEX_DIRECT_LOAD_AUTO                 3
#define NV_REG_OGL_TEX_DIRECT_LOAD_DEFAULT              NV_REG_OGL_TEX_DIRECT_LOAD_DISABLE

/* One Minus Depth Buffering */
#define NV_REG_OGL_RENDER_QUALITY_FLAGS_ONE_MINUS_Z     0x00000010

#define NV_REG_OGL_RENDER_QUALITY_FLAGS_DEFAULT         (0)

#define NV_REG_OGL_TEX_MEMORY_SPACE_ENABLES             "TexMemorySpaceEnables"
#define NV_REG_OGL_TEX_MEMORY_SPACE_ENABLES_NONE        0
#define NV_REG_OGL_TEX_MEMORY_SPACE_ENABLES_SYS         1
#define NV_REG_OGL_TEX_MEMORY_SPACE_ENABLES_VID         2
#define NV_REG_OGL_TEX_MEMORY_SPACE_ENABLES_VIDSYS      3
#define NV_REG_OGL_TEX_MEMORY_SPACE_ENABLES_DEFAULT     3

#define NV4_REG_OGL_PALETTED_TEX_IN_VID_MEM             "PalettedTexInVidMem"
#define NV4_REG_OGL_DEFAULT_PALETTED_TEX_IN_VID_MEM     1

#define NV4_REG_OGL_TEX_PRECACHE                        "TexturePrecache"

#define NV4_REG_OGL_TEX_STAGING_BUFFER_SIZE             "TexStagingBufferSize"
#define NV4_REG_OGL_TEX_STAGING_BUFFER_SIZE_DEFAULT     (512*1024)

#define NV4_REG_OGL_TEX_STAGING_BUFFER_MULT_SIZE        "TexStagingBufferMultSize"
#define NV4_REG_OGL_STAGING_BUFFER_MULT_SIZE_DEFAULT    (256*1024)

// XXXmjc delete this key ASAP!
#define NV_REG_OGL_FULLSCENE_ANTIALIASING               "EnableFSAA"
#define NV_REG_OGL_FULLSCENE_ANTIALIASING_ENABLED       1
#define NV_REG_OGL_FULLSCENE_ANTIALIASING_DISABLED      0
#define NV_REG_OGL_FULLSCENE_ANTIALIASING_DEFAULT       NV_REG_OGL_FULLSCENE_ANTIALIASING_DISABLED

// XXXmjc delete this key ASAP!
#define NV_REG_OGL_FSAA_QUALITY                         "FSAAQuality"
#define NV_REG_OGL_FSAA_QUALITY_15x15                   0 // 1.5x1.5
#define NV_REG_OGL_FSAA_QUALITY_2x2_LODBIAS             1
#define NV_REG_OGL_FSAA_QUALITY_2x2                     2
#define NV_REG_OGL_FSAA_QUALITY_DEFAULT                 NV_REG_OGL_FSAA_QUALITY_15x15
#define NV_REG_OGL_FSAA_QUALITY_MAXIMUM                 2 // clamp to [0,this]

/*
** The way this key works has changed dramatically between rel6 and rel7.
** There used to be one key that controlled whether FSAA was on at all,
** and another to determine the quality.  Now, there's just one key.
** NV10 has 4 levels of FSAA (none, 1.5x1.5, 2x2 LOD-bias, 2x2).  NV20
** has 3 levels of FSAA (none, 2x, 4x).  The way each setting is interpreted
** on each chip is as follows:
**
** Setting                            NV10 Meaning            NV20 Meaning
** NV_REG_OGL_FSAA_MODE_NONE          No AA                   No AA
** NV_REG_OGL_FSAA_MODE_2x            Invalid                 2x AA bilinear
** NV_REG_OGL_FSAA_MODE_2x_5t         Invalid                 2x AA quincunx
** NV_REG_OGL_FSAA_MODE_15x15         1.5x1.5 AA              Invalid
** NV_REG_OGL_FSAA_MODE_4x            2x2 AA                  4x AA bilinear
** NV_REG_OGL_FSAA_MODE_4x_9t         Invalid                 4x AA gaussian
**
** If a setting is invalid, the driver will treat it the same as no AA.
** The panel should not allow the user to set this key to an invalid value
** on the current chip, and the UI needs to be chip-dependent.
*/
#define NV_REG_OGL_FSAA_MODE                            "FSAAMode"
#define NV_REG_OGL_FSAA_MODE_NONE                       0
#define NV_REG_OGL_FSAA_MODE_2x                         1
#define NV_REG_OGL_FSAA_MODE_2x_5t                      2
#define NV_REG_OGL_FSAA_MODE_15x15                      3
#define NV_REG_OGL_FSAA_MODE_4x                         4
#define NV_REG_OGL_FSAA_MODE_4x_9t                      5
#define NV_REG_OGL_FSAA_MODE_DEFAULT                    NV_REG_OGL_FSAA_MODE_NONE
#define NV_REG_OGL_FSAA_MODE_MAXIMUM                    5 // clamp to [0,this]

#define NV_REG_OGL_S3TC_QUALITY                         "S3TCQuality"
#define NV_REG_OGL_S3TC_QUALITY_FORCE_DXT3              1
#define NV_REG_OGL_S3TC_QUALITY_DEFAULT                 0

#define NV_REG_OGL_FORCE_GENERIC_CPU                    "ForceGenericCPU"
#define NV_REG_OGL_FORCE_GENERIC_CPU_ON                 1
#define NV_REG_OGL_FORCE_GENERIC_CPU_OFF                0
#define NV_REG_OGL_FORCE_GENERIC_CPU_DEFAULT            NV_REG_OGL_FORCE_GENERIC_CPU_OFF

#define NV_REG_OGL_APP_SOFTIMAGE                        "App_SoftImage"
#define NV_REG_OGL_APP_SOFTIMAGE_DEFAULT                0
#define NV_REG_OGL_APP_SOFTIMAGE_38                     38
#define NV_REG_OGL_APP_SOFTIMAGE_39                     39

#define NV_REG_OGL_APP_SUPPORTBITS                      "App_SupportBits"
#define NV_REG_OGL_APP_SUPPORTBITS_DEFAULT              0
#define NV_REG_OGL_APP_SUPPORTBITS_SYNCGDI              0x00000001
#define NV_REG_OGL_APP_SUPPORTBITS_PROE_2000I2          0x00000002

#define NV_REG_OGL_NV20_EMULATE                         "NV20Emulate"
#define NV_REG_OGL_NV20_EMULATE_ON                      1
#define NV_REG_OGL_NV20_EMULATE_OFF                     0
#define NV_REG_OGL_NV20_EMULATE_DEFAULT                 NV_REG_OGL_FORCE_GENERIC_CPU_OFF

#define NV_REG_OGL_SERVER_SWAP_NT4                      "ServerSwapNT4"
#define NV_REG_OGL_SERVER_SWAP_NT4_DEFAULT              0

#define NV_REG_OGL_PERF_STRAT_IMM_MODE                  "094313"
#define NV_REG_OGL_PERF_STRAT_IMM_MODE_FORCE_DIRECT     0x09431392
#define NV_REG_OGL_PERF_STRAT_IMM_MODE_FORCE_BATCH      0x29313490
#define NV_REG_OGL_PERF_STRAT_IMM_MODE_FORCE_CACHE      0x13920943
#define NV_REG_OGL_PERF_STRAT_IMM_MODE_DEFAULT          0

/* OGL Capture file */
#define NV_REG_OGL_CAPTURE_ENABLE                       "CaptureEnable"
#define NV_REG_OGL_CAPTURE_ENABLE_DISABLE               0x00
#define NV_REG_OGL_CAPTURE_ENABLE_RECORD                0x01
#define NV_REG_OGL_CAPTURE_ENABLE_PLAY                  0x02
#define NV_REG_OGL_CAPTURE_ENABLE_DEFAULT               NV_REG_OGL_CAPTURE_ENABLE_RECORD

#define NV_REG_OGL_CAPTURE_RECORD_PATH                  "CaptureRecordPath"
#define NV_REG_OGL_CAPTURE_RECORD_PATH_DEFAULT          "c:\\"
#define NV_REG_OGL_CAPTURE_RECORD_FILENAME              "CaptureRecordFilename"
#define NV_REG_OGL_CAPTURE_RECORD_FILENAME_DEFAULT      "capture"

#define NV_REG_OGL_CAPTURE_PLAY_PATH                    "CapturePlayPath"
#define NV_REG_OGL_CAPTURE_PLAY_PATH_DEFAULT            "c:\\"
#define NV_REG_OGL_CAPTURE_PLAY_FILENAME                "CapturePlayFilename"
#define NV_REG_OGL_CAPTURE_PLAY_FILENAME_DEFAULT        "capture"

#define NV_REG_OGL_CAPTURE_WHEN_FLIP                    1
#define NV_REG_OGL_CAPTURE_WHEN_READPIXELS              2

#define NV_REG_OGL_CAPTURE_RECORD_WHEN                  "CaptureRecordWhen"
#define NV_REG_OGL_CAPTURE_RECORD_WHEN_DEFAULT          NV_REG_OGL_CAPTURE_WHEN_FLIP

#define NV_REG_OGL_CAPTURE_PLAY_WHEN                    "CapturePlayWhen"
#define NV_REG_OGL_CAPTURE_PLAY_WHEN_DEFAULT            NV_REG_OGL_CAPTURE_WHEN_FLIP

#define NV_REG_OGL_CAPTURE_PLAY_WHERE                   "CapturePlayWhere"
#define NV_REG_OGL_CAPTURE_PLAY_WHERE_FRONT             0
#define NV_REG_OGL_CAPTURE_PLAY_WHERE_BACK              1
#define NV_REG_OGL_CAPTURE_PLAY_WHERE_DEFAULT           NV_REG_OGL_CAPTURE_PLAY_WHERE_FRONT

// NV_REG_OGL_TEXCLAMP defines texture clamping behavior when GL_CLAMP is set.
//   EDGE means always use CLAMP_TO_EDGE.
//   USE_HW means use CLAMP if the HW supports it, otherwise CLAMP_TO_EDGE.
//   SPEC means follow the spec, even if it means software rasterization.
#define NV_REG_OGL_TEXCLAMP                             "TextureClampBehavior"
#define NV_REG_OGL_TEXCLAMP_EDGE                        0
#define NV_REG_OGL_TEXCLAMP_USE_HW                      1
#define NV_REG_OGL_TEXCLAMP_SPEC                        2
#define NV_REG_OGL_TEXCLAMP_NVALUES                     3
#define NV_REG_OGL_TEXCLAMP_DEFAULT                     NV_REG_OGL_TEXCLAMP_EDGE


//-----------------------------------------------------------------------------
// ResMan
//-----------------------------------------------------------------------------

#define NV_REG_SYS_SUPER7_COMPATIBILITY                 "Super7Compat"
#define NV_REG_OGL_DEFAULT_ALLOW_AGP_PUSH_BUF_FOR_MAC   0
#define NV_REG_OGL_DEFAULT_ALLOW_AGP_PUSH_BUF           1
#define NV_REG_OGL_DEFAULT_ALLOW_VID_PUSH_BUF           0
#define NV_REG_OGL_DISABLE_AGP_PUSH_BUF                 0
#define NV_REG_OGL_ENABLE_AGP_PUSH_BUF                  1

#define PERSISTENT_AGP_SIZE                             "PersistAGPSize"
#define NV_MAX_AGP_SIZE                                 "MaxAGPSize"
#define NV5_REG_SYS_HSYNC_FLIP                          "FlipOnHSync"
#define MAXREFRESHRATE                                  "MaxRefreshRate"
#define MAXOPTIMALREFRESHRATE                           "MaxOptimalRefreshRate"
#define OPTIMALREFRESHNOTFROMEDID                       "OptimalRefreshNotFromEDID"
#define DMTOVERRIDE                                     "DMTOVERRIDE"
#define MONITORTIMING                                   "MonitorTiming"
#define CURSORCACHE                                     "CursorCache"
#define DISPLAY_TYPE                                    "Display Type"
#define DISPLAY2_TYPE                                   "Display2 Type"
#define TV_TYPE                                         "TV Type"
#define POWERUPFLAGS                                    "PowerUpFlags"
#define TV_OUT                                          "TV Out"
#define NEWSTYLEMODESET                                 "NewStyleModeSet"
#define FLATPANELMODE                                   "FlatPanelMode"
#define RESOURCE_MANAGER_SUBKEY_ADDITION                "NVIDIA"


// Forces the push buffer to reside in a particular location.  Used by ddraw
// and OGL.  This is a local key.  Setting this will override the Super7Compat key
#define NV_REG_SYS_PB_LOCATION                          "PushBufferLocale"
#define   NV_REG_SYS_PB_LOCATION_DEFAULT                0
#define   NV_REG_SYS_PB_LOCATION_SYSTEM                 1
#define   NV_REG_SYS_PB_LOCATION_AGP                    2
#define   NV_REG_SYS_PB_LOCATION_VIDEO                  3

//-----------------------------------------------------------------------------
// Control Panel
//-----------------------------------------------------------------------------

#define NV_REG_CPL_D3D_SETTINGS_SUBKEY                   NV4_REG_DIRECT3D_SUBKEY
#define NV_REG_CPL_OGL_SETTINGS_SUBKEY                   NV4_REG_OGL_SUBKEY

#define NV_REG_CPL_COLOR_SUBKEY                          "Color"
#define NV_REG_CPL_COLOR_VALUE_CURRENT                   "Current"

#define NV_REG_CPL_COLOR_SCHEMES_SUBKEY                  "Schemes"

#define NV_REG_CPL_TV_OUT_SUBKEY                         "TVOut"

#define NV_REG_CPL_OEM_DEFAULTS_SUBKEY                   "Defaults"

#define NV_REG_CPL_DISPLAY_MODES_SUBKEY                  "DisplayModes"
#define NV_REG_CPL_DISPLAY_MODES_VALUE_MONITOR           "Analog Monitor"
#define NV_REG_CPL_DISPLAY_MODES_VALUE_DFP               "Digital Flat Panel"
#define NV_REG_CPL_DISPLAY_MODES_VALUE_TV                "TV"

#define NV_REG_CPL_OVL_SETTINGS_SUBKEY                   "Overlay"

// path to "skins" DLL provided by OEMs or IHVs
#define NV_REG_CPL_OEM_GRAPHICS_LIB                      "NvCplGfxLib"

#define NV_REG_CPL_VALUE_UNLOCK_GOODIES                  "CoolBits"
#define NV_REG_CPL_DATA_UNLOCK_GOODIES_VSYNC             0x00000001
#define NV_REG_CPL_DATA_UNLOCK_GOODIES_HWCLOCK           0x00000002

#define NV_REG_HARDWARE_SUBKEY                           "Hardware"
#define NV_REG_HARDWARE_CLOCK_ENABLE                     "Marge"    // overclocking enable
#define NV_REG_HARDWARE_CLOCK_CONTROL_USER               "Krusty"   // user defined clock speeds
#define NV_REG_HARDWARE_CLOCK_CONTROL_DEFAULT            "Bart"     // BIOS default clock speeds
#define NV_REG_HARDWARE_CLOCK_ENCRYPT_FLAGS              "Homer"    // encryption flags
#define NV_REG_HARDWARE_CLOCK_NO_WARN_DLG                "Lisa"     // turn off warning dialog
#define NV_REG_HARDWARE_CLOCK_DETECT_VALUE               "NvHardwareControl"
#define NV_REG_HARDWARE_CLOCK_DETECT_CMD_LINE            "RUNDLL32.EXE NVQTWK.DLL,NvHWCtl"
#define NV_REG_HARDWARE_CLOCK_INIT_VALUE                 "NvInitialize"
#define NV_REG_HARDWARE_CLOCK_INIT_CMD_LINE              "RUNDLL32.EXE NVQTWK.DLL,NvXTInit"

#define NV_REG_DESK_MANAGER_VALUE_NAME                   "DesktopManager"   //XXX deprecated!
#define NV_REG_DESK_MANAGER_HOTKEY_SUBKEY                "HotKeys"  //XXX deprecated!
#define NV_REG_DESK_MANAGER_APPASSOC_SUBKEY              "Apps"
#define NV_REG_DESK_MANAGER_APPASSOC_DESKTOP             "Desktop"
#define NV_REG_DESK_MANAGER_APPASSOC_MONITOR             "StartingMonitor"
#define NV_REG_DESK_MANAGER_APPASSOC_FLAGS               "Flags"
#define NV_REG_DESK_MANAGER_APPASSOC_PLACEMENT           "Placement"
#define NV_REG_DESK_MANAGER_APPASSOC_CLASS               "Class"
#define NV_REG_DESK_MANAGER_GLOBALS_SUBKEY               "Globals"
#define NV_REG_DESK_MANAGER_GLOBALS_FLAGS                "Flags"
#define NV_REG_DESK_MANAGER_GLOBALS_TLSCREEN             "TLScreen"
#define NV_REG_DESK_MANAGER_GLOBALS_HKMOVE               "HKMove"
#define NV_REG_DESK_MANAGER_GLOBALS_HKMOVEALL            "HKMoveAll"
#define NV_REG_DESK_MANAGER_GLOBALS_HKSWITCH             "HKSwitch"
#define NV_REG_DESK_MANAGER_GLOBALS_HKGATHER             "HKGather"
#define NV_REG_DESK_MANAGER_GLOBALS_HKZOOM               "HKZoom"
#define NV_REG_DESK_MANAGER_GLOBALS_HKZOOMIN             "HKZoomIn"
#define NV_REG_DESK_MANAGER_GLOBALS_HKZOOMOUT            "HKZoomOut"
#define NV_REG_DESK_MANAGER_GLOBALS_ALTTABMON            "CoolSwitchMon"
#define NV_REG_DESK_MANAGER_GLOBALS_ZOOMLEVEL            "DefaultZoomLevel"
#define NV_REG_DESK_MANAGER_GLOBALS_ZOOMREFRESH          "ZoomRefreshRate"
#define NV_REG_DESK_MANAGER_GLOBALS_ZOOMWKEYS            "ZoomWheelKeys"
#define NV_REG_DESK_MANAGER_GLOBALS_ZOOMDELAY            "ZoomSwitchDelay"
#define NV_REG_DESK_MANAGER_CPLPOS                       "PanelPos"

/////////////////////////////////////
// bit flags for the QuickTweak properties settings

#define NV_REG_CPL_GLOBAL_VALUE_QUICKTWEAK_FLAGS         "QuickTweak"

#define NV_REG_CPL_GLOBAL_DATA_QUICKTWEAK_DEFAULT        0x00010020

#define NV_REG_FLAGS_NOCONFIRM                           0x00000001
#define NV_REG_FLAGS_LBUTTON                             0x00000010
#define NV_REG_FLAGS_RBUTTON                             0x00000020
#define NV_REG_FLAGS_BOTHBUTTONS                         0x00000030
#define NV_REG_FLAGS_COOLMENUS                           0x00000100
#define NV_REG_FLAGS_QUICKTWEAK_ALWAYS                   0x00000200
#define NV_REG_FLAGS_QUICKTWEAK_HIDDEN                   0x00000400
#define NV_REG_FLAGS_SUPPORT_INDUCE                      0x00000800
#define NV_REG_FLAGS_RESTORE_VIRTUAL                     0x00001000
#define NV_REG_FLAGS_TASKBAR_LOGO                        0x00010000
#define NV_REG_FLAGS_TASKBAR_BLUE                        0x00020000
#define NV_REG_FLAGS_TASKBAR_RED                         0x00040000

#define NV_REG_FLAGS_DEFAULT                             (NV_REG_FLAGS_RBUTTON | NV_REG_FLAGS_TASKBAR_LOGO)

/////////////////////////////////////
// Flag to turn on the control panel agp/hardware page
#define NV_REG_CPL_ENABLE_HARDWARE_PAGE "NvCplEnableHardwarePage"
#define NV_REG_CPL_ENABLE_HARDWARE_PAGE_ENABLE  1
#define NV_REG_CPL_ENABLE_HARDWARE_PAGE_DISABLE 0
#define NV_REG_CPL_ENABLE_HARDWARE_PAGE_DEFAULT NV_REG_CPL_ENABLE_HARDWARE_PAGE_DISABLE

/////////////////////////////////////
// bit flags for hiding particular property pages or dialogs
#define NV_REG_CPL_HIDE_PROPPAGES          "NoPages"

#define NVCPL_HIDE_COLOR_PAGE              0x00000001
#define NVCPL_HIDE_D3D_PAGE                0x00000002
#define NVCPL_HIDE_OGL_PAGE                0x00000004
#define NVCPL_HIDE_OVERLAY_PAGE            0x00000008
#define NVCPL_HIDE_OPTIONS_PAGE            0x00000010
#define NVCPL_HIDE_TASKBAR_PAGE            NVCPL_HIDE_OPTIONS_PAGE
#define NVCPL_HIDE_OUTPUT_DEVICE_PAGE      0x00000020
#define NVCPL_HIDE_CRT_POSITIONING_DLG     0x00000040
#define NVCPL_HIDE_DFP_POSITIONING_DLG     0x00000080
#define NVCPL_HIDE_TV_POSITIONING_DLG      0x00000100
#define NVCPL_HIDE_INTERNET_OPTIONS        0x00000200
#define NVCPL_HIDE_CRT_TIMING_DLG          0x00000400
#define NVCPL_HIDE_ALL_CRT_SETTINGS        (NVCPL_HIDE_CRT_POSITIONING_DLG | NVCPL_HIDE_CRT_TIMING_DLG)
#define NVCPL_HIDE_AA_PAGE                 0x00000800
#define NVCPL_HIDE_MONITOR_SCALING_OPTION  0x00002000
#define NVCPL_HIDE_BIOS_FLASH_CAPS         0x00004000
#define NVCPL_HIDE_EXTENDED_OVERLAY_DIALOG 0x00008000
#define NVCPL_HIDE_HARDWARE_DIALOG         0x00010000
#define NVCPL_HIDE_POWERMIZER_DIALOG       0x00020000
#define NVCPL_HIDE_WATERMARK               0x00040000

#ifdef  STEREO_SUPPORT
//-----------------------------------------------------------------------------
// Registry settings for stereo support. For now are sitting in the Global section.
//-----------------------------------------------------------------------------
#define NV_REG_STEREO_ENABLED                            "StereoEnable"                //Stereo can be potentially activated.
#define NV_REG_STEREO_STEREOVIEWER                       "StereoViewer"                //Defines the name of the stereo viewer driver.
#define NV_REG_STEREO_STEREOVIEWERTYPE                   "StereoViewerType"            //Defines the stereo viewer type.
#define NV_REG_STEREO_ADJUSTENABLED                      "StereoAdjustEnable"          //Allow stereo adjustments by hot keys.
#define NV_REG_STEREO_DISABLE_TnL                        "StereoDisableTnL"            //Disables hardware T&L. Allowes to reach more stereo effect.
#define NV_REG_STEREO_SEPARATION                         "StereoSeparation"            //Pre-transformed separation (ProjectMatrix.31).
#define NV_REG_STEREO_CONVERGENCE                        "StereoConvergence"           //Pre-transformed convergence (ProjectMatrix.41).
#define NV_REG_STEREO_CONVERGENCEMULTIPLIER              "StereoConvergenceMultiplier" //Defines correlation between pre and post-transformed convergence.
#define NV_REG_STEREO_RHW2DDETECTIONMIN                  "RHW2DDetectionMin"           //RHW far limit for 2D object detection.
#define NV_REG_STEREO_RHWGREATERATSCREEN                 "RHWGreaterAtScreen"          //Objects closer than this boundary are 2D.
#define NV_REG_STEREO_RHWEQUALATSCREEN                   "RHWEqualAtScreen"            //All objects parallel to screen are 2D.
#define NV_REG_STEREO_RHWLESSATSCREEN                    "RHWLessAtScreen"             //All objects parallel to screen are 2D.
#define NV_REG_STEREO_AUTOCONVERGENCE                    "AutoConvergence"             //Automatically focus on the closest object.
#define NV_REG_STEREO_CONVERGENCEADJUSTPACE              "AutoConvergenceAdjustPace"   //Per frame Convergence adjust pace in AutoConvergence mode.
#define NV_REG_STEREO_HOTKEY_TOGGLE                      "StereoToggle"                //The hot key to toggle stereo.
#define NV_REG_STEREO_HOTKEY_VERTICALADJUST_MORE         "StereoVerticalAdjustMore"    //The hot key to increase the vertical separation.
#define NV_REG_STEREO_HOTKEY_VERTICALADJUST_LESS         "StereoVerticalAdjustLess"    //The hot key to reduce the vertical separation.
#define NV_REG_STEREO_HOTKEY_HORIZONTALADJUST_MORE       "StereoHorizontalAdjustMore"  //The hot key to increase the horizontal separation.
#define NV_REG_STEREO_HOTKEY_HORIZONTALADJUST_LESS       "StereoHorizontalAdjustLess"  //The hot key to reduce the horizontal separation.
#define NV_REG_STEREO_HOTKEY_SEPARATIONADJUST_MORE       "StereoSeparationAdjustMore"  //The hot key to increase the stereo separation.
#define NV_REG_STEREO_HOTKEY_SEPARATIONADJUST_LESS       "StereoSeparationAdjustLess"  //The hot key to increase the stereo separation.
#define NV_REG_STEREO_HOTKEY_CONVERGENCEADJUST_MORE      "StereoConvergenceAdjustMore" //The hot key to increase the convergence degree.
#define NV_REG_STEREO_HOTKEY_CONVERGENCEADJUST_LESS      "StereoConvergenceAdjustLess" //The hot key to reduce the convergence degree.
#define NV_REG_STEREO_HOTKEY_SUGGESTSETTINGS             "StereoSuggestSettings"       //The hot key to automatically set suggested stereo settings.
#define NV_REG_STEREO_HOTKEY_UNSUGGESTSETTINGS           "StereoUnsuggestSettings"     //The hot key to restore stereo settings before suggestion.
#define NV_REG_STEREO_HOTKEY_LASER_X_PLUS                "LaserAdjustXPlus"            //The hot key to increase the laser sight X position
#define NV_REG_STEREO_HOTKEY_LASER_X_MINUS               "LaserAdjustXMinus"           //The hot key to decrease the laser sight X position
#define NV_REG_STEREO_HOTKEY_LASER_Y_PLUS                "LaserAdjustYPlus"            //The hot key to increase the laser sight Y position
#define NV_REG_STEREO_HOTKEY_LASER_Y_MINUS               "LaserAdjustYMinus"           //The hot key to decrease the laser sight Y position
#define NV_REG_STEREO_FAVORSZ                            "FavorSZ"                     //Rather use SZ in stereo calculations than RHW
#define NV_REG_STEREO_LASERSIGHT                         "LaserSight"                  //Use Nvidia proprietary laser sight.
#define NV_REG_STEREO_LASERSIGHTFILE                     "LaserSightFile"              //Defines the BMP file used as a laser sight.
#define NV_REG_STEREO_LASERSIGHTENABLED                  "LaserSightEnabled"           //Masks on/off use of a laser sight.
#define NV_REG_STEREO_LASERSIGHTPROPERTY                 "LaserSightProperty"          //Laser sight min/max size and transparency.
#define NV_REG_STEREO_DEFAULTON                          "StereoDefaultOn"             //Turn stereo on when the game starts
#define NV_REG_STEREO_FRUSTUMADJUSTMODE                  "FrustumAdjustMode"           //0 - do nothing, 1 - stretch in X, 2 - clear corresponding edges for each eye.
#define NV_REG_STEREO_MONITORSIZE                        "MonitorSize"                 //The monitor diagonal for max. stereo separation calculation.
#define NV_REG_STEREO_NOORTHOSTEREO                      "NoOrthoStereo"               //Assumes that Ortho porjection is used for 2D objects (OpenGL).
#define NV_REG_STEREO_MAXVERTEXCOUNT                     "MaxVertexCount"              //Max. amount of vertices we allow in VB (for D3D SW TnL games only).
#define NV_REG_STEREO_PARTIALCLEARMODE                   "PartialClearMode"            //0 - do as requested, 1 - do full clear instad, 2 - ignore
#define NV_REG_STEREO_LASERXADJUST                       "LaserXAdjust"
#define NV_REG_STEREO_LASERYADJUST                       "LaserYAdjust"
#define NV_REG_STEREO_POPUP_ENABLED                      "PopupAgents"                  //Use Nvidia proprietary popup agents
#define NV_REG_STEREO_POPUP_SUBKEY                       "PopupAgentOptions"            //subkey under which options lies
#define NV_REG_STEREO_POPUP_STAT                         "PopupStat"                    //on/off Stereo statistics
#define NV_REG_STEREO_POPUP_FPS                          "PopupFPS"                     //on/off frame rate info popup
#define NV_REG_STEREO_POPUP_HOTKEY                       "PopupHotkey"                  //what hotkey will toggle all popups
#define NV_REG_STEREO_POPUP_FPS_HOTKEY                   "FPSHotkey"                    //what hotkey will toggle FPS popup
#define NV_REG_STEREO_POPUP_STATS_HOTKEY                 "StatHotkey"                   //what hotkey will toggle StereoStat popup
#endif  //STEREO_SUPPORT

//----------------End of file NVREG.H----(do not edit below this line or remove this line) -----


