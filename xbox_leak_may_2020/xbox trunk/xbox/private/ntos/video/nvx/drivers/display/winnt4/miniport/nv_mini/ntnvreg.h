 /***************************************************************************\
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.  Users and possessors of this source code are     *|
|*     hereby granted a nonexclusive,  royalty-free copyright license to     *|
|*     use this code in individual and commercial software.                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*       Copyright 1993-1998 NVIDIA, Corporation.  All rights reserved.      *|
|*                                                                           *|
|*     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY     *|
|*     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"     *|
|*     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-     *|
|*     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,     *|
|*     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-     *|
|*     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL     *|
|*     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-     *|
|*     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-     *|
|*     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION     *|
|*     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF     *|
|*     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.     *|
|*                                                                           *|
|*     U.S. Government  End  Users.   This source code  is a "commercial     *|
|*     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),     *|
|*     consisting  of "commercial  computer  software"  and  "commercial     *|
|*     computer  software  documentation,"  as such  terms  are  used in     *|
|*     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-     *|
|*     ment only as  a commercial end item.   Consistent with  48 C.F.R.     *|
|*     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),     *|
|*     all U.S. Government End Users  acquire the source code  with only     *|
|*     those rights set forth herein.                                        *|
|*                                                                           *|
 \***************************************************************************/



//******************************************************************************
// Common typedefs
//******************************************************************************

typedef unsigned char   U008;
typedef unsigned short  U016;
typedef unsigned long   U032;

//******************************************************************************
// Type of REGISTRY entry
//******************************************************************************

#define IS_BINARY       0
#define IS_STRING       1
#define IS_DWORD        2

//******************************************************************************
// REGISTRY data length max values
//******************************************************************************

#define REG_STRING_DATA_MAX 30

//******************************************************************************
// Maximum number of NV registry switches
//******************************************************************************

#define MAX_NV_REGISTRY_SWITCHES   100

//******************************************************************************
// Dword value used to IGNORE the presence of the registry switch
//******************************************************************************

#define IGNORE_REGISTRY_SWITCH     0x99999999

//******************************************************************************
// NT Registry Switches:
// ====================
//
// The implementation of NT registry keys are different than Win9x,
// since NT is more restricted when it comes to setting values in its registry.
// Essentially, for NT, we only use a 'flat' registry structure, as opposed to 
// several different keys (directory structure) in Win9x.
//  
// For Win9x, the registy keys and values are stored under 
//          \HKEY_LOCAL_MACHINE\SOFTWARE\NVIDIA Corporation\Riva TNT\
// 
// For WinNT, the registry values are all stored under 
//          \HKEY_LOCAL_MACHINE\System\CurrentControlSet\Services\nv\DeviceXX
//
// Even though the implementation is different on NT, we'll attempt to keep the
// registry functionality as close to Win9x as possible.
//
//
// NOTE: Since OPENGL is NOT part of the NT display driver (it'a separate DLL),
//       it can read the registry by itself, and does NOT have its registry
//       switches in the same location as all the switches.
//       It's switches are still located in .\Software\NVIDIA Corporation\Riva TNT\
//
//
//      Please Refer to NVREG.H in the \drivers\common\inc directory to
//              see how this structure was obtained
//
//******************************************************************************

typedef struct _NT_NV_REGISTRY_SWITCH_OFFSET
    {
    U032    RegType;
    U008    RegName[REG_STRING_DATA_MAX];
    U032    Offset;
    } NT_NV_REGISTRY_SWITCH_OFFSET;


typedef struct _NT_NV_REGISTRY_DISPLAY
    {
    U008    LastDisplayDevice[REG_STRING_DATA_MAX];     // String
    U008    LastCRTMode[REG_STRING_DATA_MAX];           // String
    U008    LastDFPMode[REG_STRING_DATA_MAX];           // String
    U008    LastNTSCMode[REG_STRING_DATA_MAX];          // String
    U008    LastPALMode[REG_STRING_DATA_MAX];           // String
    U008    CRTMode[REG_STRING_DATA_MAX];               // String
    U008    DFPMode[REG_STRING_DATA_MAX];               // String
    U008    NTSCMode[REG_STRING_DATA_MAX];              // String
    U008    PALMode[REG_STRING_DATA_MAX];               // String
    U008    DefaultCRTRefreshRate[REG_STRING_DATA_MAX]; // String
    U008    DefaultDFPRefreshRate[REG_STRING_DATA_MAX]; // String
    U008    OptimalCRTRefreshRate[REG_STRING_DATA_MAX]; // String
    U008    OptimalDFPRefreshRate[REG_STRING_DATA_MAX]; // String
    U008    DisplayDeviceType[REG_STRING_DATA_MAX];     // String
    U008    MonitorTiming[REG_STRING_DATA_MAX];         // String
    U032    DisplayType;                                // Dword
    U032    TVType;                                     // Dword
    U032    CursorAlpha;                                // Dword
    } NT_NV_REGISTRY_DISPLAY;

typedef struct _NT_NV_REGISTRY_DIRECTDRAW
    {
    U032    VPENABLE;                                   // Dword
    U032    OverlayMode;                                // Dword
    U032    OverlayColorControlEnable;                  // Dword
    U032    OverlayBrightness;                          // Dword
    U032    OverlayContrast;                            // Dword
    U032    OverlayHue;                                 // Dword
    U032    OverlaySaturation;                          // Dword
    U032    OverlayGamma;                               // Dword
    U032    OverlaySharpness;                           // Dword
    } NT_NV_REGISTRY_DIRECTDRAW;


typedef struct _NT_NV_REGISTRY_DIRECT3D
    {
    U032    ANTIALIASENABLE;                            // Dword
    U032    COLORKEYCOMPATABILITYENABLE;                // Dword
    U032    CONTROLTRAFFIC;                             // Dword
    U032    DIRECTMAPENABLE;                            // Dword
    U032    FOGTABLENABLE;                              // Dword
    U032    LOGOENABLE;                                 // Dword
    U032    SCENESYNCENABLE;                            // Dword
    U032    TILINGENABLE;                               // Dword
    U032    USERMIPMAPENABLE;                           // Dword
    U032    VIDEOTEXTUREENABLEENABLE;                   // Dword
    U032    VSYNCENABLE;                                // Dword
    U032    WENABLE;                                    // Dword
    U032    Z24ENABLE;                                  // Dword        
    U032    AAMETHOD;                                   // Dword
    U032    AUTOMIPMAPMETHOD;                           // Dword
    U032    CKREF;                                      // Dword
    U032    DX6ENABLE;                                  // Dword
    U032    ANISOTOPIC4TAP;                             // Dword
    U032    PAL8TEXTURECONVERT;                         // Dword
    U032    PMTRIGGER;                                  // Dword
    U032    TEXELALIGNMENT;                             // Dword
    U032    VALIDATEZMETHOD;                            // Dword
    U032    W16FORMAT;                                  // Dword
    U032    W32FORMAT;                                  // Dword
    U032    AGPTEXCUTOFF;                               // Dword       
    U032    D3DCONTEXTMAX;                              // Dword
    U032    D3DTEXTUREMAX;                              // Dword
    U032    DMAMINPUSHCOUNT;                            // Dword
    U032    LODBIASADJUST;                              // Dword     
    U032    MINVIDEOTEXSIZE;                            // Dword
    U032    MIPMAPLEVELS;                               // Dword       
    U032    PERFSTRATEGY;                               // Dword
    U032    NOVSYNCPREREDNERLIMIT;                      // Dword
    U032    DMAPUSHBUFFERSIZEMAX;                       // Dword
    U032    TEXHEAP;                                    // Dword
    U032    WSCALE16;                                   // Dword
    U032    WSCALE24;                                   // Dword
    
    
    } NT_NV_REGISTRY_DIRECT3D;


typedef struct _NT_NV_REGISTRY_SYSTEM
    {
    U032    Super7Compat;                               // Dword  
    U032    FlipOnHSync;                                // Dword                     
    U032    FilterOverride;                             // Dword                  
    U032    NoNT4AGP;                                   // Dword
    U032    George;                                     // Dword
    U032    Gracie;                                     // Dword
    U032    TVOutOnPrimary;                             // Dword
    U032    Enable256Burst;                             // Dword
    U032    DisableMPCDetect;                           // Dword
    U032    MemoryOverride;                             // Dword
    U032    RTLOverride;                                // Dword
    } NT_NV_REGISTRY_SYSTEM;


typedef struct _NT_NV_REGISTRY_NVTWEAK
    {
    U032    test;                                       // Dword
    } NT_NV_REGISTRY_NVTWEAK;
                     



    //**************************************************************************
    // Complete Registry Structure
    //**************************************************************************


typedef struct _NT_NV_REGISTRY
    {
    //**************************************************************************
    // DISPLAY Registry switches
    //**************************************************************************

    NT_NV_REGISTRY_DISPLAY Display;
            
    //**************************************************************************
    // DIRECTDRAW Registry switches
    //**************************************************************************

    NT_NV_REGISTRY_DIRECTDRAW DirectDraw;

    //**************************************************************************
    // DIRECT3D Registry switches
    //**************************************************************************

    NT_NV_REGISTRY_DIRECT3D Direct3D;

    //**************************************************************************
    // RESMAN Registry switches
    //**************************************************************************

    NT_NV_REGISTRY_SYSTEM System;

    //**************************************************************************
    // NVTWEAK Control Panel Registry switches
    //**************************************************************************

    NT_NV_REGISTRY_NVTWEAK NVTweak;


    } NT_NV_REGISTRY;
