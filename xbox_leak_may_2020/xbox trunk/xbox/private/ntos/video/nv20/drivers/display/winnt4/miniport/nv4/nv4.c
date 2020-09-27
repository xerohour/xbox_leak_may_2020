//******************************************************************************
//
// Copyright (c) 1992  Microsoft Corporation
//
// Module Name:
//
//     NV4.C
//
// Abstract:
//
//     This is code specific to NV4
//
// Environment:
//
//     kernel mode only
//
// Notes:
//
// Revision History:
//
//******************************************************************************

//******************************************************************************
//
// Copyright (c) 1996,1997  NVidia Corporation. All Rights Reserved
//
//******************************************************************************

//
// Used to turn on MultiDevice support for NT 4 (smae is used for the display driver)
//
//#define NT4_MULTI_DEV


#include "dderror.h"
#include "devioctl.h"
#include "miniport.h"

#include "ntddvdeo.h"
#include "video.h"

#include "nv.h"

#include "nv4_ref.h"

#include "nv32.h"
#include "nvMultiMon.h"
#include "nvsubch.h"
#include "nvalpha.h"

#include "cmdcnst.h"
#include <arb_fifo.h>
#include <nvntioctl.h>

#include "modes.h"
#include "tv.h"

#include "monitor.h"
#include "modedefs.h"
#include "vesadata.h"

#include "nvos.h"
#include "rm.h"

#include "nvreg.h"
#include "nvcm.h"
#include "nv_name.h"

#include "cr11_ref.h"

VOID FlatPanelCrtc(U016);
VOID NV_OEMEnableExtensions(PHW_DEVICE_EXTENSION HwDeviceExtension);
VOID NV_ReadAndCopyRegistryData(PHW_DEVICE_EXTENSION,WCHAR *,U016 *);
VOID LoadDefaultRegistrySwitchValues(PHW_DEVICE_EXTENSION);
VOID BubbleSort(PHW_DEVICE_EXTENSION,PMODE_ENTRY,ULONG);
VOID NVSaveSpecificRegistersForPwrMgmt(PHW_DEVICE_EXTENSION);
VOID NVRestoreSpecificRegistersForPwrMgmt(PHW_DEVICE_EXTENSION);
VOID NV4ChipFBPowerSave(PHW_DEVICE_EXTENSION);
VOID NV4ChipFBPowerRestore(PHW_DEVICE_EXTENSION);
VOID NV4SaveInstanceMemory(PHW_DEVICE_EXTENSION);
VOID NV4RestoreInstanceMemory(PHW_DEVICE_EXTENSION);
VOID ReadMonitorRestrictionModeList(PHW_DEVICE_EXTENSION HwDeviceExtension);

//
// To fix a soft reboot hang on the laptops.
//
#if (_WIN32_WINNT >= 0x0500)
extern BOOLEAN
NVResetHW(
    PVOID hwDeviceExtension,
    ULONG Column,
    ULONG Rows
    );
#endif

//******************************************************************************
// External Declarations
//******************************************************************************

extern U016 ModeSetTable[];
extern U016 DMTOverrideTable[][13];
extern CRTC_OVERRIDE crt_override[];
extern EXTREGS eregs[];
extern U016 tblClockFreq[];
extern U016 VBESetModeEx(PHW_DEVICE_EXTENSION, U016, PGTF_TIMINGS, PDMT_OVERRIDES, U016);
extern VOID SetGlobalHwDev(PHW_DEVICE_EXTENSION pHwDevExt);
extern MODESET_FIFO DACFifoTable[];
extern unsigned int GTFTimingTable[][12];
extern U016 ValidModeTable[];
extern U016 registry_data[];

extern ULONG my_strcmp(PUCHAR ,PUCHAR);
extern VOID my_strupr(PUCHAR);
extern VOID my_strcpy(PUCHAR , PUCHAR);

extern VOID NV10ChipFBPowerSave(PHW_DEVICE_EXTENSION);
extern VOID NV10ChipFBPowerRestore(PHW_DEVICE_EXTENSION);
extern VOID NV10SaveInstanceMemory(PHW_DEVICE_EXTENSION);
extern VOID NV10RestoreInstanceMemory(PHW_DEVICE_EXTENSION);

extern ULONG GetTimingDacCommonModesetCode(PHW_DEVICE_EXTENSION HwDeviceExtension, HEAD_RESOLUTION_INFO *pResolution, DAC_TIMING_VALUES * pTimingInfo);


//******************************************************************************
// Global tables default registry settings
// (Type, ValueName, SwitchIsPresent, Offset of structure member)
//******************************************************************************

NT_NV_REGISTRY_SWITCH_OFFSET Nv_Registry_Switch_Offset_Table[] =
    {


// STUB OUT FOR NOW SINCE \common\NVREG.H is constantly changing 
// and breaking the NT build.  When NVREG.H settles down, then
// we can put this function back in

    { IS_DWORD , "PLACEHOLDER" ,                                offsetof(NT_NV_REGISTRY, Display.DisplayType)                   }

//    //**************************************************************************
//    // Display Registry Switch Names
//    //**************************************************************************
//
//    { IS_STRING , NV4_REG_DRV_LAST_DEVICE ,                   offsetof(NT_NV_REGISTRY, Display.LastDisplayDevice[0])          },
//    { IS_STRING , NV4_REG_DRV_LAST_CRT_MODE ,                 offsetof(NT_NV_REGISTRY, Display.LastCRTMode[0])                },
//    { IS_STRING , NV4_REG_DRV_LAST_DFP_MODE ,                 offsetof(NT_NV_REGISTRY, Display.LastDFPMode[0])                },
//    { IS_STRING , NV4_REG_DRV_LAST_NTSC_MODE ,                offsetof(NT_NV_REGISTRY, Display.LastNTSCMode[0])               },
//    { IS_STRING , NV4_REG_DRV_LAST_PAL_MODE ,                 offsetof(NT_NV_REGISTRY, Display.LastPALMode[0])                },
//    { IS_STRING , NV4_REG_DRV_CRT_MODE ,                      offsetof(NT_NV_REGISTRY, Display.CRTMode[0])                    },
//    { IS_STRING , NV4_REG_DRV_DFP_MODE ,                      offsetof(NT_NV_REGISTRY, Display.DFPMode[0])                    },
//    { IS_STRING , NV4_REG_DRV_NTSC_MODE ,                     offsetof(NT_NV_REGISTRY, Display.NTSCMode[0])                   },
//    { IS_STRING , NV4_REG_DRV_PAL_MODE ,                      offsetof(NT_NV_REGISTRY, Display.PALMode[0])                    },
//    { IS_STRING , NV4_REG_DRV_DEFAULT_CRT_VERT_REFRESH_RATE , offsetof(NT_NV_REGISTRY, Display.DefaultCRTRefreshRate[0])      },
//    { IS_STRING , NV4_REG_DRV_DEFAULT_DFP_VERT_REFRESH_RATE , offsetof(NT_NV_REGISTRY, Display.DefaultDFPRefreshRate[0])      },
//    { IS_STRING , NV4_REG_DRV_OPTIMAL_CRT_VERT_REFRESH_RATE , offsetof(NT_NV_REGISTRY, Display.OptimalCRTRefreshRate[0])      },
//    { IS_STRING , NV4_REG_DRV_OPTIMAL_DFP_VERT_REFRESH_RATE , offsetof(NT_NV_REGISTRY, Display.OptimalDFPRefreshRate[0])      },
//    { IS_STRING , NV4_REG_DRV_DISPLAY_DEVICE_TYPE ,           offsetof(NT_NV_REGISTRY, Display.DisplayDeviceType[0])          },
//    { IS_DWORD , "Display Type" ,                             offsetof(NT_NV_REGISTRY, Display.DisplayType)                   },
//    { IS_DWORD , "TV Type" ,                                  offsetof(NT_NV_REGISTRY, Display.TVType)                        },
//    { IS_DWORD  , NV4_REG_DRV_MONITOR_TIMING ,                offsetof(NT_NV_REGISTRY, Display.MonitorTiming)                 },
//    { IS_DWORD  , NV4_REG_DRV_CURSOR_ALPHA ,                  offsetof(NT_NV_REGISTRY, Display.CursorAlpha)                   },
//
//
//    //**************************************************************************
//    // DirectDraw Registry Switch Names
//    //**************************************************************************
//
//    { IS_DWORD , NV4_REG_VPE_ENABLE ,                         offsetof(NT_NV_REGISTRY, DirectDraw.VPENABLE)                   },
//    { IS_DWORD , NV4_REG_OVL_MODE ,                           offsetof(NT_NV_REGISTRY, DirectDraw.OverlayMode)                },
//    { IS_DWORD , NV4_REG_OVLCC_ENABLE ,                       offsetof(NT_NV_REGISTRY, DirectDraw.OverlayColorControlEnable)  },
//    { IS_DWORD , NV4_REG_OVLCC_BRIGHTNESS ,                   offsetof(NT_NV_REGISTRY, DirectDraw.OverlayBrightness)          },
//    { IS_DWORD , NV4_REG_OVLCC_CONTRAST ,                     offsetof(NT_NV_REGISTRY, DirectDraw.OverlayContrast)            },
//    { IS_DWORD , NV4_REG_OVLCC_HUE ,                          offsetof(NT_NV_REGISTRY, DirectDraw.OverlayHue)                 },
//    { IS_DWORD , NV4_REG_OVLCC_SATURATION ,                   offsetof(NT_NV_REGISTRY, DirectDraw.OverlaySaturation)          },
//    { IS_DWORD , NV4_REG_OVLCC_GAMMA ,                        offsetof(NT_NV_REGISTRY, DirectDraw.OverlayGamma)               },
//    { IS_DWORD , NV4_REG_OVLCC_SHARPNESS ,                    offsetof(NT_NV_REGISTRY, DirectDraw.OverlaySharpness)           },
//
//    //**************************************************************************
//    // Direct3d Registry Switch Names
//    //**************************************************************************
//
//    { IS_DWORD , D3D_REG_ANTIALIASENABLE_STRING ,             offsetof(NT_NV_REGISTRY, Direct3D.ANTIALIASENABLE)              },
//    { IS_DWORD , D3D_REG_CKCOMPATABILITYENABLE_STRING ,       offsetof(NT_NV_REGISTRY, Direct3D.COLORKEYCOMPATABILITYENABLE)  },
//    { IS_DWORD , "CONTROLTRAFFIC"                    ,        offsetof(NT_NV_REGISTRY, Direct3D.CONTROLTRAFFIC)               },
//    { IS_DWORD , D3D_REG_DIRECTMAPENABLE_STRING ,             offsetof(NT_NV_REGISTRY, Direct3D.DIRECTMAPENABLE)              },
//    { IS_DWORD , D3D_REG_FOGTABLEENABLE_STRING ,              offsetof(NT_NV_REGISTRY, Direct3D.FOGTABLENABLE)                },
//    { IS_DWORD , D3D_REG_LOGOENABLE_STRING ,                  offsetof(NT_NV_REGISTRY, Direct3D.LOGOENABLE)                   },
//    { IS_DWORD , D3D_REG_SSYNCENABLE_STRING ,                 offsetof(NT_NV_REGISTRY, Direct3D.SCENESYNCENABLE)              },
//    { IS_DWORD , D3D_REG_TILINGENABLE_STRING ,                offsetof(NT_NV_REGISTRY, Direct3D.TILINGENABLE)                 },
//    { IS_DWORD , D3D_REG_USERMIPMAPENABLE_STRING ,            offsetof(NT_NV_REGISTRY, Direct3D.USERMIPMAPENABLE)             },
//    { IS_DWORD , D3D_REG_VIDEOTEXTUREENABLE_STRING ,          offsetof(NT_NV_REGISTRY, Direct3D.VIDEOTEXTUREENABLEENABLE)     },
//    { IS_DWORD , D3D_REG_VSYNCENABLE_STRING ,                 offsetof(NT_NV_REGISTRY, Direct3D.VSYNCENABLE)                  },
//    { IS_DWORD , D3D_REG_WBUFFERENABLE_STRING ,               offsetof(NT_NV_REGISTRY, Direct3D.WENABLE)                      },
//    { IS_DWORD , D3D_REG_Z24ENABLE_STRING ,                   offsetof(NT_NV_REGISTRY, Direct3D.Z24ENABLE)                    },
//    { IS_DWORD , D3D_REG_AAMETHOD_STRING ,                    offsetof(NT_NV_REGISTRY, Direct3D.AAMETHOD)                     },
//    { IS_DWORD , D3D_REG_AUTOMIPMAPMETHOD_STRING ,            offsetof(NT_NV_REGISTRY, Direct3D.AUTOMIPMAPMETHOD)             },
//    { IS_DWORD , D3D_REG_CKREF_STRING ,                       offsetof(NT_NV_REGISTRY, Direct3D.CKREF)                        },
//    { IS_DWORD , "DX6ENABLE" ,                                offsetof(NT_NV_REGISTRY, Direct3D.DX6ENABLE)                    },
//    { IS_DWORD , D3D_REG_MIPMAPDITHERMODE_STRING ,            offsetof(NT_NV_REGISTRY, Direct3D.ANISOTOPIC4TAP)               },
//    { IS_DWORD , D3D_REG_PAL8TEXCONVERT_STRING ,              offsetof(NT_NV_REGISTRY, Direct3D.PAL8TEXTURECONVERT)           },
//    { IS_DWORD , "PMTRIGGER" ,                                offsetof(NT_NV_REGISTRY, Direct3D.PMTRIGGER)                    },
//    { IS_DWORD , D3D_REG_TEXELALIGNMENT_STRING ,              offsetof(NT_NV_REGISTRY, Direct3D.TEXELALIGNMENT)               },
//    { IS_DWORD , D3D_REG_VALIDATEZMETHOD_STRING ,             offsetof(NT_NV_REGISTRY, Direct3D.VALIDATEZMETHOD)              },
//    { IS_DWORD , D3D_REG_WFORMAT16_STRING ,                   offsetof(NT_NV_REGISTRY, Direct3D.W16FORMAT)                    },
//    { IS_DWORD , D3D_REG_WFORMAT32_STRING ,                   offsetof(NT_NV_REGISTRY, Direct3D.W32FORMAT)                    },
//    { IS_DWORD , D3D_REG_AGPTEXCUTOFF_STRING ,                offsetof(NT_NV_REGISTRY, Direct3D.AGPTEXCUTOFF)                 },
//    { IS_DWORD , D3D_REG_D3DCONTEXTMAX_STRING ,               offsetof(NT_NV_REGISTRY, Direct3D.D3DCONTEXTMAX)                },
//    { IS_DWORD , D3D_REG_D3DTEXTUREMAX_STRING ,               offsetof(NT_NV_REGISTRY, Direct3D.D3DTEXTUREMAX)                },
//    { IS_DWORD , D3D_REG_DMAMINPUSHCOUNT_STRING ,             offsetof(NT_NV_REGISTRY, Direct3D.DMAMINPUSHCOUNT)              },
//    { IS_DWORD , D3D_REG_LODBIAS_STRING ,                     offsetof(NT_NV_REGISTRY, Direct3D.LODBIASADJUST)                },
//    { IS_DWORD , D3D_REG_MINVIDTEXSIZE_STRING ,               offsetof(NT_NV_REGISTRY, Direct3D.MINVIDEOTEXSIZE)              },
//    { IS_DWORD , D3D_REG_AUTOMIPMAPLEVELS_STRING ,            offsetof(NT_NV_REGISTRY, Direct3D.MIPMAPLEVELS)                 },
//    { IS_DWORD , D3D_REG_PERFSTRATEGY_STRING ,                offsetof(NT_NV_REGISTRY, Direct3D.PERFSTRATEGY)                 },
//    { IS_DWORD , D3D_REG_PRERENDERLIMIT_STRING ,              offsetof(NT_NV_REGISTRY, Direct3D.NOVSYNCPREREDNERLIMIT)        },
//    { IS_DWORD , D3D_REG_PUSHBUFFERSIZEMAX_STRING ,           offsetof(NT_NV_REGISTRY, Direct3D.DMAPUSHBUFFERSIZEMAX)         },
//    { IS_DWORD , D3D_REG_TEXHEAPSIZEMAX_STRING ,              offsetof(NT_NV_REGISTRY, Direct3D.TEXHEAP)                      },
//    { IS_DWORD , D3D_REG_WSCALE16_STRING ,                    offsetof(NT_NV_REGISTRY, Direct3D.WSCALE16)                     },
//    { IS_DWORD , D3D_REG_WSCALE24_STRING ,                    offsetof(NT_NV_REGISTRY, Direct3D.WSCALE24)                     },
//
//    //**************************************************************************
//    // System Registry Switch Names
//    //**************************************************************************
//
//    { IS_DWORD , "Reserved1" ,                                offsetof(NT_NV_REGISTRY, System.Super7Compat)                  },
//    { IS_DWORD , "Reserved2" ,                                offsetof(NT_NV_REGISTRY, System.Super7Compat)                  },
//    { IS_DWORD , "Reserved3" ,                                offsetof(NT_NV_REGISTRY, System.Super7Compat)                  },
//    { IS_DWORD , "Reserved4" ,                                offsetof(NT_NV_REGISTRY, System.Super7Compat)                  },
//    { IS_DWORD , "Reserved5" ,                                offsetof(NT_NV_REGISTRY, System.Super7Compat)                  },
//    { IS_DWORD , "Reserved6" ,                                offsetof(NT_NV_REGISTRY, System.Super7Compat)                  },
//    { IS_DWORD , "Reserved7" ,                                offsetof(NT_NV_REGISTRY, System.Super7Compat)                  },
//    { IS_DWORD , "Reserved8" ,                                offsetof(NT_NV_REGISTRY, System.Super7Compat)                  },
//    { IS_DWORD , "Reserved9" ,                                offsetof(NT_NV_REGISTRY, System.Super7Compat)                  },
//    { IS_DWORD , "Reserved10" ,                               offsetof(NT_NV_REGISTRY, System.Super7Compat)                  },
//    { IS_DWORD , "Reserved11" ,                               offsetof(NT_NV_REGISTRY, System.Super7Compat)                  },

    };
    

//******************************************************************************
// Global tables for registry mode table data
//******************************************************************************

U016 NV4_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV5_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV0A_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NVVANTA_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV5M64_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV5ULTRA_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV10_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV10DDR_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV10GL_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV11_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV11DDR_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV11M_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV11GL_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 CRUSH11_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV15_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV15DDR_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV15BR_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV15GL_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV20_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV20_1_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV20_2_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
U016 NV20_3_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];


//******************************************************************************
// Global tables for Monitor restriction mode list data
//******************************************************************************
U016 MonitorRestrictionModeList0_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];  // For head0
U016 MonitorRestrictionModeList1_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];  // For head1

// global device extension pointer - We need it to distinguish between NV3/NV4..
extern PHW_DEVICE_EXTENSION HwDeviceExtension;

extern VP_STATUS
NVRegistryCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );


extern VP_STATUS
NVReadRegistrySwitchesCallback(
    PVOID HwDeviceExtension,
    PVOID Context,
    PWSTR ValueName,
    PVOID ValueData,
    ULONG ValueLength
    );

__inline static size_t cbStrLenW(
    IN UNALIGNED const WCHAR *wsz)
{
    size_t cbWsz = 0;

    for(; *wsz; wsz++)
        cbWsz += sizeof( WCHAR);

    return( cbWsz + sizeof( WCHAR));
}

#if (_WIN32_WINNT >= 0x0500)

//******************************************************************************
// Tables used by DPMS_SetPowerState (NT5 only)
//******************************************************************************

UCHAR tblDPMSStates[4]      = { 0, 2, 1, 4 }                    ;   //on, stdby, suspend, off
UCHAR tblDPMSSettings[4]    = { 0x000, 0x080, 0x040, 0x0C0 }    ;

//******************************************************************************
// Forward declarations
//******************************************************************************

VOID NV_ATCOff(PHW_DEVICE_EXTENSION);
VOID NV_ATCOn(PHW_DEVICE_EXTENSION);
VOID NV_ResetATCFlipFlop(PHW_DEVICE_EXTENSION);

#endif


//******************************************************************************
// Use a slightly different mode timing table for NV4
//******************************************************************************


//
// CRTC Timings modes for 60Hz, 70Hz, 72Hz, 75Hz, 85Hz, 100Hz, 120Hz
//
// Currently we're using hardcoded table values, but there is no reason why we
// don't just use the standard GTF algorithms and compute any frequency on the
// fly (once we've got a GTF function that doesn't use floating point).
//
// Horizontal Total, Start, End
// Vertical Total, Start, End
// DotClock, Refresh
// HSync polarity
// VSync polarity
//
// Max out all the modes above our pixclk (~250).
//
U016 ModeTimingTable[NUMBER_OF_MODES*NUMBER_OF_RATES][10] =
{
    //0:
    // Settings for 1600x1200:
    2160,1704,1880,1242,1201,1204,16096,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2176,1712,1888,1249,1201,1204,19025,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2176,1712,1888,1251,1201,1204,19600,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2192,1720,1896,1253,1201,1204,20599,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2192,1720,1896,1260,1201,1204,23476,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2192,1720,1896,1260,1201,1204,23476,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2192,1720,1896,1260,1201,1204,23476,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //1:
    // Settings for 1280x1024:
    1712,1360,1496,1060,1025,1028,10888,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1728,1368,1504,1066,1025,1028,12894,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1728,1368,1504,1067,1025,1028,13275,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1728,1368,1504,1069,1025,1028,13854,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1744,1376,1512,1075,1025,1028,15936,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1760,1376,1520,1085,1025,1028,19096,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1776,1384,1528,1097,1025,1028,23379,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //2:
    // Settings for 1152x864:
    1520,1216,1336,895,865,868,8162 ,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1536,1224,1344,900,865,868,9677 ,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1536,1224,1344,901,865,868,9964 ,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1552,1224,1352,902,865,868,10499,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1552,1224,1352,907,865,868,11965,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1568,1232,1360,915,865,868,14347,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1584,1240,1368,926,865,868,17601,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //3:
    // Settings for 1024x768:
    1344,1080,1184,795,769,772,6411 ,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1360,1080,1192,800,769,772,7616 ,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1360,1080,1192,801,769,772,7843 ,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1360,1080,1192,802,769,772,8180 ,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1376,1088,1200,807,769,772,9439 ,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    //1392,1096,1208,814,769,772,11331,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1392,1096,1208,814,769,772,11370,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1408,1104,1216,823,769,772,13905,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //4:
    // Settings for 960x720:
    1248,1008,1104,746,721,724,5586 ,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1264,1008,1112,750,721,724,6636 ,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1264,1008,1112,751,721,724,6835 ,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1280,1016,1120,752,721,724,7219 ,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1280,1016,1120,756,721,724,8225 ,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1296,1024,1128,763,721,724,9888 ,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1312,1032,1136,772,721,724,12154,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //5:
    // Settings for 800x600:
    1024,832,912,622,601,604,3822,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1040,840,920,625,601,604,4550,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1040,840,920,626,601,604,4687,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1040,840,920,627,601,604,4891,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1056,840,928,630,601,604,5655,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1072,848,936,636,601,604,6818,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    1088,856,944,643,601,604,8395,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //6:
    // Settings for 640x480:
    800,656,720,497,481,484,2386,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    816,664,728,500,481,484,2856,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    816,664,728,501,481,484,2943,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    816,664,728,502,481,484,3072,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    832,672,736,505,481,484,3571,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    848,680,744,509,481,484,4316,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    848,680,744,515,481,484,5241,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //7:
    // Settings for 640x400:
    //There is no 60Hz timings for 640x400 as defined by IBM VGA
    //784,648,712,415,401,404,1952,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    800,656,720,417,401,404,2335,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    800,656,720,417,401,404,2335,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    800,656,720,418,401,404,2408,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    800,656,720,418,401,404,2508,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    816,664,728,421,401,404,2920,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    832,672,736,424,401,404,3528,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    848,680,744,429,401,404,4366,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //8:
    // Settings for 512x384:
    // NOTE: This is a scan-doubled version of 512x768 timings
    672,536,592,795,769,772,3205,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    672,536,592,800,769,772,3763,7000,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    688,544,600,801,769,772,3968,7200,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    688,544,600,802,769,772,4138,7500,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    688,544,600,807,769,772,4719,8500,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    688,544,600,814,769,772,5600,10000,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,
    704,552,608,823,769,772,6953,12000,BUFFER_HSYNC_NEGATIVE, BUFFER_VSYNC_POSITIVE,

    //9:
    // Settings for 480x360:
    // NOTE: This is a scan-doubled version of 480x720 timings
    624,504,552,746,721,724,2793,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    640,512,560,750,721,724,3360,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    640,512,560,751,721,724,3461,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    640,512,560,752,721,724,3610,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    640,512,560,756,721,724,4113,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    656,512,568,763,721,724,5005,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    656,512,568,772,721,724,6077,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //10:
    // Settings for 400x300:
    // NOTE: This is a scan-doubled version of 400x600 timings
    512,416,456,622,601,604,1911,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    528,424,464,625,601,604,2310,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    528,424,464,626,601,604,2380,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    528,424,464,627,601,604,2483,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    528,424,464,630,601,604,2827,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    528,424,464,636,601,604,3358,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    544,432,472,643,601,604,4198,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //11:
    // Settings for 320x400:
    //
    // !!! This mode should never be set !!! since DDraw doesn't support it yet
    //
    //384,320,352,415,401,404,956 ,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,417,401,404,1168,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,417,401,404,1168,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,418,401,404,1204,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,418,401,404,1254,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,421,401,404,1489,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,424,401,404,1764,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //12:
    // Settings for 320x240:
    // NOTE: This is a scan-doubled version of 320x480 timings
    400,328,360,497,481,484,1193,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,500,481,484,1400,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,501,481,484,1501,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,502,481,484,1566,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,505,481,484,1786,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,509,481,484,2117,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    432,344,376,515,481,484,2670,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //13:
    // Settings for 320x200:
    // NOTE: This a scan-doubled version of 320x400 timings
    //There is no 60Hz timings for 320x200 as defined by IBM VGA
    //384,320,352,415,401,404,956 ,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,417,401,404,1168,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,417,401,404,1168,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,418,401,404,1204,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    400,328,360,418,401,404,1254,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,421,401,404,1489,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,424,401,404,1764,10000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    416,336,368,429,401,404,2142,12000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //14:
    // Settings for 1920x1200:
    2592,2048,2256,1242,1201,1204,19316,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2608,2056,2264,1249,1201,1204,22802,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2624,2064,2272,1251,1201,1204,23635,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2624,2064,2272,1253,1201,1204,24659,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2592,2048,2256,1242,1201,1204,19316,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2592,2048,2256,1242,1201,1204,19316,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2592,2048,2256,1242,1201,1204,19316,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //15:
    // Settings for 1920x1080:
    2576,2040,2248,1118,1081,1084,17280,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2608,2056,2264,1124,1081,1084,20520,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2608,2056,2264,1126,1081,1084,21144,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2608,2056,2264,1128,1081,1084,22064,7500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2624,2064,2272,1134,1081,1084,25293,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2624,2064,2272,1134,1081,1084,25293,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2624,2064,2272,1134,1081,1084,25293,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

    //16:
    // Settings for 1800x1440:
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2456,1928,2128,1490,1441,1444,21957,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

//17:
    // Settings for 1600x1024:
    2144,1704,1872,1060,1025,1028,13636,6000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2160,1704,1880,1066,1025,1028,16118,7000,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2160,1704,1880,1067,1025,1028,16594,7200,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

// An OEM Customer wants a SPECIFIC timing...which is NOT GTF or DMT compliant ...so we'll accomodate them...
// (They're using a widescreen Sony monitor)

// Standard GTF
//    2176,1712,1888,1070,1025,1028,17695,7600,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,

// OEM requested
    2096,1632,1792,1070,1027,1030,17045,7600,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_NEGATIVE,


    2176,1712,1888,1075,1025,1028,19883,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2176,1712,1888,1075,1025,1028,19883,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,
    2176,1712,1888,1075,1025,1028,19883,8500,BUFFER_HSYNC_NEGATIVE,BUFFER_VSYNC_POSITIVE,


};

//******************************************************************************
// Forward Function Declarations
//******************************************************************************
VP_STATUS
NVSetMode(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_MODE Mode,
    ULONG ModeSize
    );

BOOLEAN
NVGetNVInfo(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    );


VOID NV4_UpdateArbitrationSettings(PHW_DEVICE_EXTENSION);

BOOL NV4SetMode(PHW_DEVICE_EXTENSION,PMODE_ENTRY);
VOID NV_SetColorLookup(PHW_DEVICE_EXTENSION,PVIDEO_CLUT,ULONG);
VOID NV4_InitPalette(PHW_DEVICE_EXTENSION);
ULONG NVMapMemoryRanges(PVOID HwDeviceExtension);
ULONG NVMapFrameBuffer(PVOID HwDeviceExtension);
//BOOLEAN NVIsPresent(PHW_DEVICE_EXTENSION,PVIDEO_ACCESS_RANGE,PULONG);
BOOLEAN NVIsPresent(PHW_DEVICE_EXTENSION,PULONG);
VOID NV4EnableCursor(PHW_DEVICE_EXTENSION);
ULONG NVMapVgaPaletteRegisters(PHW_DEVICE_EXTENSION);
VOID NV4_ProgramTV( U008 );

//******************************************************************************
//
//  Function: NVSetMode()
//
//  Routine Description:
//
//      This routine sets the adapter into the requested mode.
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//      Mode - Pointer to the structure containing the information about the
//          font to be set.
//
//      ModeSize - Length of the input buffer supplied by the user.
//
//  Return Value:
//
//      ERROR_INSUFFICIENT_BUFFER if the input buffer was not large enough
//          for the input data.
//
//      ERROR_INVALID_PARAMETER if the mode number is invalid.
//
//      NO_ERROR if the operation completed successfully.
//
//******************************************************************************

VP_STATUS NVSetMode(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_MODE Mode,
    ULONG ModeSize
    )


    {

    PMODE_ENTRY pRequestedMode;             // NV specific
    VP_STATUS status;
    USHORT usDataSet, usTemp, usDataClr;
    PUSHORT  pBios = NULL;
    VIDEO_X86_BIOS_ARGUMENTS biosArguments;
    ULONG OutData;
    LONG x_inc,y_inc;
    ULONG x,y;
    ULONG i;
    ULONG Data;
    PMODE_ENTRY ModeEntry;

    PHWINFO NVInfo;

    VideoDebugPrint((1, "NVSetMode - entry\n"));

    //**************************************************************************
    // Check if the size of the data in the input buffer is large enough.
    //**************************************************************************

    if (ModeSize < sizeof(VIDEO_MODE))
    {

        VideoDebugPrint((1, "SetMode - ERROR_INSUFFICIENT_BUFFER\n"));
        return ERROR_INSUFFICIENT_BUFFER;
    }

    //**************************************************************************
    // Init pointer to Mode data (NV3 or NV4)
    //**************************************************************************

    ModeEntry = (PMODE_ENTRY)&(HwDeviceExtension->ValidModeTable[0]);


    //**************************************************************************
    // Check to see if we are requesting a valid mode
    //**************************************************************************

    if ( (Mode->RequestedMode >= HwDeviceExtension->NumRegistryModes)   ||
         (!ModeEntry[Mode->RequestedMode].ValidMode)  )
        {

        VideoDebugPrint((1, "SetMode - ERROR_INVALID_PARAMETER\n"));
        return ERROR_INVALID_PARAMETER;

        }


    //**************************************************************************
    // Our driver maps video memory first, and then sets the mode.
    // However, the Dumb Frame Buffer drivers sets the mode FIRST, and
    // then maps the video memory.  So first check to see if we've
    // got a valid frame buffer ptr, before we clear memory.
    //**************************************************************************

    if (HwDeviceExtension->FrameBufferPtr !=NULL)
        {
        //**********************************************************************
        // Clear the framebuffer prior to setting the video mode.
        // This is for aesthetics only.
        // Back up 1Mb to make absolutely sure we don't walk over any instance memory
        //**********************************************************************
        for (i = 0; i < ( (HwDeviceExtension->AdapterMemorySize-0x100000) >> 2); i++)
            {
            ((PULONG) HwDeviceExtension->FrameBufferPtr)[i] = 0;
            }
        }

    //**************************************************************************
    // Get pointer to the requested mode
    //**************************************************************************
    if(HwDeviceExtension->DeskTopInfo.ulDesktopMode & NV_ENABLE_VIRTUAL_DESKTOP)
    {
        PMODE_ENTRY pDesktopMode; 
        ULONG ulDac;
        ULONG ulWidth;
        ULONG ulHeight;
        ULONG ulRefresh;
        ULONG ulPixelDepth;

        pDesktopMode = &(ModeEntry[Mode->RequestedMode]);

        // Display driver is setting mode through class for both DAC.
        // Miniport driver set to the primary dac by default;
        ulDac = HwDeviceExtension->DeskTopInfo.ulDeviceDisplay[0];
        ulWidth = HwDeviceExtension->DeskTopInfo.ulDisplayWidth[ulDac];
        ulHeight = HwDeviceExtension->DeskTopInfo.ulDisplayHeight[ulDac];
        ulRefresh = HwDeviceExtension->DeskTopInfo.ulDisplayRefresh[ulDac];
        ulPixelDepth = HwDeviceExtension->DeskTopInfo.ulDisplayPixelDepth[ulDac];

        for (i = 0; i < HwDeviceExtension->NumRegistryModes; i++)   
        {
            if(!ModeEntry[i].ValidMode)
                continue;

            if( (ModeEntry[i].Depth == ulPixelDepth) &&
                (ModeEntry[i].RefreshRate == ulRefresh) &&
                (ModeEntry[i].Width == ulWidth) &&
                (ModeEntry[i].Height == ulHeight) )
            {
                Mode->RequestedMode = i;
                break;
            }
        }
        if(i == HwDeviceExtension->NumRegistryModes)
            return (ERROR_INVALID_PARAMETER);
    }

    pRequestedMode = &(ModeEntry[Mode->RequestedMode]);

    //**************************************************************************
    // Initialize the Graphics Mode and Engine.
    //**************************************************************************

    if (NV4SetMode(HwDeviceExtension, pRequestedMode) == FALSE)
        return(ERROR_NOT_ENOUGH_MEMORY);

    //**************************************************************************
    // Store the new mode value.
    //**************************************************************************

    HwDeviceExtension->CurrentMode = pRequestedMode;
    HwDeviceExtension->ModeIndex = Mode->RequestedMode;

#if _WIN32_WINNT >= 0x0500
    HwDeviceExtension->ulDualViewStatus |= DUALVIEW_STATUS_GRAPHICS_MODE;
#endif

    VideoDebugPrint((1, "NVSetMode - exit\n"));
    return NO_ERROR;

    } //end NVSetMode()


//******************************************************************************
//
//  Function:   NVGetNVInfo()
//
//  Routine Description:
//
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//  Return Value:
//
//      None.
//
//******************************************************************************

BOOLEAN NVGetNVInfo(PHW_DEVICE_EXTENSION HwDeviceExtension)


    {
    PHWINFO NVInfo;
    ULONG refresh;
    ULONG i,j;
    ULONG Size;
    BOOLEAN status=TRUE;
    ULONG   FullChipId,ChipId;
    ULONG   TopOfFB;
    U016    rc;
    PCI_SLOT_NUMBER slot;

    //**************************************************************************
    // Get ptr to NVInfo structure and clear it out.
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);
    VideoPortZeroMemory(NVInfo, sizeof(HWINFO));


#if (_WIN32_WINNT >= 0x0500)

    //**************************************************************************
    // WIN2K:   We need to determine what chip we're running on for Win2K!
    //          This was NOT obtained in NVIsPresent, so we need to
    //          determine it HERE, because we now have access to the
    //          the hardware registers !!
    //**************************************************************************

    ChipId = REG_RD32(NV_PBUS_PCI_NV_0);
    ChipId >>= 16;                              // Get top 16 bits (bits 16 thru 31)
    ChipId &= 0xffff;                           // Just look at 16 bits

    FullChipId = ChipId;                        // Save complete id
    ChipId &= 0xfffc;                           // Ignore Sub Revisions 1 thru 3

    switch (ChipId)
        {
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV04:

            //******************************************************************
            // NV4 (original TNT)
            //******************************************************************
            HwDeviceExtension->ulChipID     = NV4_DEVICE_NV4_ID;
            break;

        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV05_DEVID0:

            //******************************************************************
            // NV5 (TNT2)   ID = 28
            //******************************************************************
            HwDeviceExtension->ulChipID     = NV5_DEVICE_NV5_ID;

            //******************************************************************
            // NV5 (TNT2 ULTRA)  ID = 29
            //******************************************************************

            if (FullChipId == NV_CONFIG_PCI_NV_0_DEVICE_ID_NV05_DEVID1)
                HwDeviceExtension->ulChipID     = NV5ULTRA_DEVICE_NV5ULTRA_ID;


            break;

        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV06_DEVID0:

            //******************************************************************
            // VANTA (ID = 2C)
            //******************************************************************
            HwDeviceExtension->ulChipID     = NV5VANTA_DEVICE_NV5VANTA_ID;

            //******************************************************************
            // (TNT2 Model 64) (ID = 2D)
            //******************************************************************

            if (FullChipId == NV_CONFIG_PCI_NV_0_DEVICE_ID_NV06_DEVID1)
                HwDeviceExtension->ulChipID     = NV5MODEL64_DEVICE_NV5MODEL64_ID;

            break;

        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV0A_DEVID0:

            //******************************************************************
            // NV0A
            //******************************************************************
            HwDeviceExtension->ulChipID     = NV0A_DEVICE_NV0A_ID;
            break;

        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID0:

            //******************************************************************
            // NV10
            //******************************************************************
            HwDeviceExtension->ulChipID     = NV10_DEVICE_NV10_ID;

            //******************************************************************
            // NV10DDR (NV10 DDR)  ID = 101
            //******************************************************************

            if (FullChipId == NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID1)
                HwDeviceExtension->ulChipID     = NV10DDR_DEVICE_NV10DDR_ID;

            //******************************************************************
            // NV10GL (NV10 GL)  ID = 103
            //******************************************************************

            if (FullChipId == NV_CONFIG_PCI_NV_0_DEVICE_ID_NV10_DEVID3)
                HwDeviceExtension->ulChipID     = NV10GL_DEVICE_NV10GL_ID;


            break;


        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV11_DEVID0:

            //******************************************************************
            // NV11 ID = 110
            //******************************************************************
            HwDeviceExtension->ulChipID     = NV11_DEVICE_NV11_ID;

            //******************************************************************
            // NV11DDR (NV11 DDR)  ID = 111
            //******************************************************************

            if (FullChipId == NV_CONFIG_PCI_NV_0_DEVICE_ID_NV11_DEVID1)
                HwDeviceExtension->ulChipID     = NV11DDR_DEVICE_NV11DDR_ID;

            //******************************************************************
            // NV11M (NV11 Mobile)  ID = 112
            //******************************************************************

            if (FullChipId == NV_CONFIG_PCI_NV_0_DEVICE_ID_NV11_DEVID2)
                HwDeviceExtension->ulChipID     = NV11M_DEVICE_NV11M_ID;

            //******************************************************************
            // NV11GL (NV11 GL)  ID = 113
            //******************************************************************

            if (FullChipId == NV_CONFIG_PCI_NV_0_DEVICE_ID_NV11_DEVID3)
                HwDeviceExtension->ulChipID     = NV11GL_DEVICE_NV11GL_ID;

            break;


        case NV_CONFIG_PCI_NV_0_DEVICE_ID_CRUSH11_DEVID0:

            //******************************************************************
            // Crush11 ID = 1A0
            //******************************************************************
            HwDeviceExtension->ulChipID     = NV11_DEVICE_CRUSH11_ID;

            break;
               
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID0:

            //******************************************************************
            // NV15 ID = 150
            //******************************************************************
            HwDeviceExtension->ulChipID     = NV15_DEVICE_NV15_ID;

            //******************************************************************
            // NV15DDR (NV15 DDR)  ID = 151
            //******************************************************************

            if (FullChipId == NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID1)
                HwDeviceExtension->ulChipID     = NV15DDR_DEVICE_NV15DDR_ID;

            //******************************************************************
            // NV15BR (NV15 BR)  ID = 152
            //******************************************************************

            if (FullChipId == NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID2)
                HwDeviceExtension->ulChipID     = NV15BR_DEVICE_NV15BR_ID;


            //******************************************************************
            // NV15GL (NV15 GL)  ID = 153
            //******************************************************************

            if (FullChipId == NV_CONFIG_PCI_NV_0_DEVICE_ID_NV15_DEVID3)
                HwDeviceExtension->ulChipID     = NV15GL_DEVICE_NV15GL_ID;


            break;

        
        case NV_CONFIG_PCI_NV_0_DEVICE_ID_NV20_DEVID0:

            //******************************************************************
            // NV20 ID = 200
            //******************************************************************
            HwDeviceExtension->ulChipID     = NV20_DEVICE_NV20_ID;

            //******************************************************************
            // NV20 ID = 201
            //******************************************************************

            if (FullChipId == NV_CONFIG_PCI_NV_0_DEVICE_ID_NV20_DEVID1)
                HwDeviceExtension->ulChipID     = NV20_DEVICE_NV20_1_ID;

            //******************************************************************
            // NV20 ID = 202
            //******************************************************************

            if (FullChipId == NV_CONFIG_PCI_NV_0_DEVICE_ID_NV20_DEVID2)
                HwDeviceExtension->ulChipID     = NV20_DEVICE_NV20_2_ID;

            //******************************************************************
            // NV20 ID = 203
            //******************************************************************

            if (FullChipId == NV_CONFIG_PCI_NV_0_DEVICE_ID_NV20_DEVID3)
                HwDeviceExtension->ulChipID     = NV20_DEVICE_NV20_3_ID;

            break;


        default:

            //******************************************************************
            // Unsupported Chip
            //******************************************************************
            status=FALSE;
            return(status);
        }
#endif


    //**************************************************************************
    // Now that we're using a Resource Manager, the majority of the
    // NVInfo structure fields are no longer used (These values were used
    // in the older 1.00 driver).  Since they're no longer used, just
    // zero them out.
    //**************************************************************************

    NVInfo->Pram.CurrentSize        = 0;
    NVInfo->Pram.HashTableAddr      = 0;
    NVInfo->Pram.HashDepth          = 0;
    NVInfo->Pram.FifoRunoutAddr     = 0;
    NVInfo->Pram.RunOutMask         = 0;
    NVInfo->Pram.FifoContextAddr    = 0;
    NVInfo->Pram.FreeInstSize       = 0;
    NVInfo->Pram.FreeInstBase       = 0;
    NVInfo->Dac.CursorImageInstance = 0;


    if ((HwDeviceExtension->ulChipID == NV10_DEVICE_NV10_ID)       ||
        (HwDeviceExtension->ulChipID == NV10DDR_DEVICE_NV10DDR_ID) ||
        (HwDeviceExtension->ulChipID == NV10GL_DEVICE_NV10GL_ID)   ||
        (HwDeviceExtension->ulChipID == NV11_DEVICE_NV11_ID)       ||
        (HwDeviceExtension->ulChipID == NV11DDR_DEVICE_NV11DDR_ID) ||
        (HwDeviceExtension->ulChipID == NV11M_DEVICE_NV11M_ID)     ||
        (HwDeviceExtension->ulChipID == NV11GL_DEVICE_NV11GL_ID)   ||
        (HwDeviceExtension->ulChipID == NV11_DEVICE_CRUSH11_ID)    ||
        (HwDeviceExtension->ulChipID == NV15_DEVICE_NV15_ID)       ||
        (HwDeviceExtension->ulChipID == NV15DDR_DEVICE_NV15DDR_ID) ||
        (HwDeviceExtension->ulChipID == NV15BR_DEVICE_NV15BR_ID)   ||
        (HwDeviceExtension->ulChipID == NV15GL_DEVICE_NV15GL_ID))
    {
        switch (PFB_REG_RD_DRF(PFB_Base,_PFB,_CFG,_BUS))
        {
            case NV_PFB_CFG_BUS_64:
                NVInfo->Dac.InputWidth = 64;
                break;
            case NV_PFB_CFG_BUS_128:
                NVInfo->Dac.InputWidth = 128;
                break;
            case NV_PFB_CFG_BUS_32:
                NVInfo->Dac.InputWidth = 32;
                break;
        }
    }
    else if ((HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_ID)       ||
             (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_1_ID)     ||
             (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_2_ID)     ||
             (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_3_ID))
    {
        NVInfo->Dac.InputWidth = 128;
    }
    else
    {
        switch (PFB_REG_RD_DRF(PFB_Base, _PFB, _BOOT_0, _RAM_WIDTH_128))
        {
            case NV_PFB_BOOT_0_RAM_WIDTH_128_OFF:
                NVInfo->Dac.InputWidth = 64;
                break;
            case NV_PFB_BOOT_0_RAM_WIDTH_128_ON:
                NVInfo->Dac.InputWidth = 128;
                break;
        }
    }

    //**************************************************************************
    // Get Chip revision and implementation
    //**************************************************************************

//    NVInfo->Chip.Implementation = PMC_REG_RD_DRF(PMC_Base,_PMC,_BOOT_0,_IMPLEMENTATION);
//    NVInfo->Chip.Revision = PMC_REG_RD_DRF(PMC_Base,_PMC,_BOOT_0,_FIB_REVISION);
//    NVInfo->Chip.Architecture = PMC_REG_RD_DRF(PMC_Base,_PMC,_BOOT_0,_ARCHITECTURE);
//    NVInfo->Chip.Manufacturer = PMC_REG_RD_DRF(PMC_Base,_PMC,_BOOT_0,_MANUFACTURER);

    //**************************************************************************
    // Get memory type
    //**************************************************************************

//    NVInfo->Framebuffer.RamType = PEXTDEV_REG_RD_DRF(PEXTDEV_Base,_PEXTDEV,_BOOT_0,_STRAP_RAM_TYPE) ==
//                            NV_PEXTDEV_BOOT_0_STRAP_RAM_TYPE_EDO ? BUFFER_DRAM : BUFFER_VRAM;

    if ((HwDeviceExtension->ulChipID == NV10_DEVICE_NV10_ID)         ||
        (HwDeviceExtension->ulChipID == NV10DDR_DEVICE_NV10DDR_ID)   ||
        (HwDeviceExtension->ulChipID == NV10GL_DEVICE_NV10GL_ID)     ||
        (HwDeviceExtension->ulChipID == NV11_DEVICE_NV11_ID)         ||
        (HwDeviceExtension->ulChipID == NV11DDR_DEVICE_NV11DDR_ID)   ||
        (HwDeviceExtension->ulChipID == NV11M_DEVICE_NV11M_ID)       ||
        (HwDeviceExtension->ulChipID == NV11GL_DEVICE_NV11GL_ID)     ||
        (HwDeviceExtension->ulChipID == NV15_DEVICE_NV15_ID)         ||
        (HwDeviceExtension->ulChipID == NV15DDR_DEVICE_NV15DDR_ID)   ||
        (HwDeviceExtension->ulChipID == NV15BR_DEVICE_NV15BR_ID)     ||
        (HwDeviceExtension->ulChipID == NV15GL_DEVICE_NV15GL_ID)     ||
        (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_ID)         ||
        (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_1_ID)       ||
        (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_2_ID)       ||
        (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_3_ID))

    {
        NVInfo->Framebuffer.RamSize = PFB_REG_RD32(PFB_Base,NV_PFB_CSTATUS);
        NVInfo->Framebuffer.RamSizeMb = NVInfo->Framebuffer.RamSize >> 20;
    }
    else if  (HwDeviceExtension->ulChipID == NV11_DEVICE_CRUSH11_ID)
    {
        slot.u.AsULONG = 0x0;    // make sure all fields are initialized
        slot.u.bits.DeviceNumber   = 0;
        slot.u.bits.FunctionNumber = 1;

        HalGetBusDataByOffset(PCIConfiguration,     // bus data type
                              0,                    // bus number
                              slot.u.AsULONG,       // slot number
                              &TopOfFB,             // buffer
                              CR_CMC_CFG0 & 0xFF,   // config space offset
                              sizeof(ULONG));       // buffer size

        NVInfo->Framebuffer.RamSizeMb =
                            CR_DRF_VAL(_CMC, _CFG0, _TOP_OF_FB, TopOfFB) + 1;
        NVInfo->Framebuffer.RamSize = NVInfo->Framebuffer.RamSizeMb << 20;
       
    }
    else
    {
        //**************************************************************************
        // Get amount of VRAM
        // NOTE: These values are DIFFERENT from those defined in NV3 !!
        //**************************************************************************

        switch (PFB_REG_RD_DRF(PFB_Base,_PFB,_BOOT_0, _RAM_AMOUNT))
            {
            case NV_PFB_BOOT_0_RAM_AMOUNT_32MB:
                NVInfo->Framebuffer.RamSizeMb = 32;
                NVInfo->Framebuffer.RamSize   = 0x02000000;
                break;
            case NV_PFB_BOOT_0_RAM_AMOUNT_4MB:
                NVInfo->Framebuffer.RamSizeMb = 4;
                NVInfo->Framebuffer.RamSize   = 0x00400000;
                break;
            case NV_PFB_BOOT_0_RAM_AMOUNT_8MB:
                NVInfo->Framebuffer.RamSizeMb = 8;
                NVInfo->Framebuffer.RamSize   = 0x00800000;
                break;
            case NV_PFB_BOOT_0_RAM_AMOUNT_16MB:
                NVInfo->Framebuffer.RamSizeMb = 16;
                NVInfo->Framebuffer.RamSize   = 0x01000000;
                break;
            default:
                //******************************************************************
                // Invalid memory configuration
                //******************************************************************
                status=FALSE;
                return(status);
            }


        //**************************************************************************
        // Check for UMA usage
        //
        // Note that this is only valid for NV0A, but the other NV4 variants hardwire these
        // bit fields to zero.
        //**************************************************************************

        if (PFB_REG_RD_DRF(PFB_Base,_PFB, _BOOT_0, _UMA) == NV_PFB_BOOT_0_UMA_ENABLE)
            switch (PFB_REG_RD_DRF(PFB_Base,_PFB, _BOOT_0, _UMA_SIZE))
            {
                case NV_PFB_BOOT_0_UMA_SIZE_2M:
                    NVInfo->Framebuffer.RamSizeMb = 2;
                    NVInfo->Framebuffer.RamSize   = 0x00200000;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_4M:
                    NVInfo->Framebuffer.RamSizeMb = 4;
                    NVInfo->Framebuffer.RamSize   = 0x00400000;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_6M:
                    NVInfo->Framebuffer.RamSizeMb = 6;
                    NVInfo->Framebuffer.RamSize   = 0x00600000;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_8M:
                    NVInfo->Framebuffer.RamSizeMb = 8;
                    NVInfo->Framebuffer.RamSize   = 0x00800000;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_10M:
                    NVInfo->Framebuffer.RamSizeMb = 10;
                    NVInfo->Framebuffer.RamSize   = 0x00A00000;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_12M:
                    NVInfo->Framebuffer.RamSizeMb = 12;
                    NVInfo->Framebuffer.RamSize   = 0x00C00000;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_14M:
                    NVInfo->Framebuffer.RamSizeMb = 14;
                    NVInfo->Framebuffer.RamSize   = 0x00E00000;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_16M:
                    NVInfo->Framebuffer.RamSizeMb = 16;
                    NVInfo->Framebuffer.RamSize   = 0x01000000;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_18M:
                    NVInfo->Framebuffer.RamSizeMb = 18;
                    NVInfo->Framebuffer.RamSize   = 0x01200000;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_20M:
                    NVInfo->Framebuffer.RamSizeMb = 20;
                    NVInfo->Framebuffer.RamSize   = 0x01400000;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_22M:
                    NVInfo->Framebuffer.RamSizeMb = 22;
                    NVInfo->Framebuffer.RamSize   = 0x01600000;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_24M:
                    NVInfo->Framebuffer.RamSizeMb = 24;
                    NVInfo->Framebuffer.RamSize   = 0x01800000;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_26M:
                    NVInfo->Framebuffer.RamSizeMb = 26;
                    NVInfo->Framebuffer.RamSize   = 0x01A00000;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_28M:
                    NVInfo->Framebuffer.RamSizeMb = 28;
                    NVInfo->Framebuffer.RamSize   = 0x01C00000;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_30M:
                    NVInfo->Framebuffer.RamSizeMb = 30;
                    NVInfo->Framebuffer.RamSize   = 0x01E00000;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_32M:
                    NVInfo->Framebuffer.RamSizeMb = 32;
                    NVInfo->Framebuffer.RamSize   = 0x02000000;
                    break;
                default:
                    //******************************************************************
                    // Invalid memory configuration
                    //******************************************************************
                    status=FALSE;
                    return(status);
            }
    }

    //**************************************************************************
    // Default to 640x480x16x1 framebuffer with VGA timing
    //**************************************************************************

    NVInfo->Framebuffer.DpmLevel          = 0;
    NVInfo->Framebuffer.Resolution        = RESOLUTION_640X480;
    NVInfo->Framebuffer.Depth             = 16;
    NVInfo->Framebuffer.RefreshRate       = 60;
    NVInfo->Framebuffer.Count             = 1;
    NVInfo->Framebuffer.ActiveCount       = 1;
    NVInfo->Framebuffer.ActiveMask        = 1;
    NVInfo->Framebuffer.Current           = 0;
    NVInfo->Framebuffer.FlipUsageCount    = 0;
    NVInfo->Framebuffer.FlipTo            = 0;
    NVInfo->Framebuffer.FlipFrom          = 0;
    NVInfo->Framebuffer.UpdateFlags       = 0;
    NVInfo->Framebuffer.HorizFrontPorch   = 0;
    NVInfo->Framebuffer.HorizSyncWidth    = 0;
    NVInfo->Framebuffer.HorizBackPorch    = 0;
    NVInfo->Framebuffer.HorizDisplayWidth = 0;
    NVInfo->Framebuffer.VertFrontPorch    = 0;
    NVInfo->Framebuffer.VertSyncWidth     = 0;
    NVInfo->Framebuffer.VertBackPorch     = 0;
    NVInfo->Framebuffer.VertDisplayWidth  = 0;
    NVInfo->Framebuffer.HSyncPolarity     = 0;
    NVInfo->Framebuffer.VSyncPolarity     = 0;
    NVInfo->Framebuffer.CSync             = 0;
    NVInfo->Framebuffer.ConfigPageHeight  = 0;
    NVInfo->Pram.CurrentSize              = 0;
    NVInfo->Pram.AvailableSize[0]         =  NV_PRAM_DEVICE_SIZE_5_5_KBYTES + NV_PRAM_MIN_SIZE_INSTANCE_MEM;
    NVInfo->Pram.AvailableSize[1]         =  NV_PRAM_DEVICE_SIZE_9_5_KBYTES + NV_PRAM_MIN_SIZE_INSTANCE_MEM;
    NVInfo->Pram.AvailableSize[2]         =  NV_PRAM_DEVICE_SIZE_17_5_KBYTES + NV_PRAM_MIN_SIZE_INSTANCE_MEM;
    NVInfo->Pram.AvailableSize[3]         =  NV_PRAM_DEVICE_SIZE_33_5_KBYTES + NV_PRAM_MIN_SIZE_INSTANCE_MEM;

    //**************************************************************************
    // Default Refresh Rate
    //**************************************************************************

    NVInfo->Framebuffer.RefreshRate = 60;
    NVInfo->Framebuffer.HSyncPolarity = BUFFER_HSYNC_NEGATIVE;
    NVInfo->Framebuffer.VSyncPolarity = BUFFER_HSYNC_NEGATIVE;

    //**************************************************************************
    // Default resolution
    //**************************************************************************

    NVInfo->Framebuffer.Resolution        = RESOLUTION_640X480;
    NVInfo->Framebuffer.HorizDisplayWidth = 640;
    NVInfo->Framebuffer.VertDisplayWidth  = 480;
    NVInfo->Framebuffer.Depth             = 16;

    //**************************************************************************
    // Determine refresh rate
    //**************************************************************************

    if (NVInfo->Framebuffer.RefreshRate < 70)
       refresh = 0;
    if (NVInfo->Framebuffer.RefreshRate < 72)
        refresh = 1;
    else if (NVInfo->Framebuffer.RefreshRate < 75)
        refresh = 2;
    else if (NVInfo->Framebuffer.RefreshRate < 85)
        refresh = 3;
    else if (NVInfo->Framebuffer.RefreshRate < 100)
        refresh = 4;
    else if (NVInfo->Framebuffer.RefreshRate < 120)
        refresh = 5;
    else
        refresh = 6;

    //**************************************************************************
    // Update the DAC flags and types
    //**************************************************************************

    NVInfo->Dac.UpdateFlags = 0;
    NVInfo->Dac.FinishFlags = 0;

    //**************************************************************************
    // Initialize the cursor data (transparent)
    //**************************************************************************

//    NVInfo->Dac.CursorType = DAC_CURSOR_TWO_COLOR_XOR;
    NVInfo->Dac.CursorEmulation = FALSE;
    NVInfo->Dac.CursorExclude= FALSE;
    NVInfo->Dac.CursorColor1 = 0x00000000;
    NVInfo->Dac.CursorColor2 = 0x00ffffff;
    NVInfo->Dac.CursorColor3 = 0x00000000;

    for (i=0;i<128;i++)
        NVInfo->Dac.CursorImagePlane[0][i] = 0x00;

    for (i=0;i<128;i++)
        NVInfo->Dac.CursorImagePlane[1][i] = 0xff;

    for (i=0;i<32;i++)
        for (j=0;j<32;j++)
            NVInfo->Dac.CursorColorImage[i][j]= 0x00000000;

    //**************************************************************************
    // Initialize palette to gray scale
    //**************************************************************************

    for (i=0;i<256;i++)
        NVInfo->Dac.Palette[i] = (i<<16) | (i<<8) | i;

    //**************************************************************************
    // Initialize Power Management state
    //**************************************************************************

    NVInfo->Dac.DpmLevel = 0;

    //**************************************************************************
    // Initialize Video Clock
    //**************************************************************************

    NVInfo->Dac.VClk= 0;

    //**************************************************************************
    // return success
    //**************************************************************************

    return(status);
    }

VOID NVUpdateRegNVInfo(PHW_DEVICE_EXTENSION HwDeviceExtension)
    {
    PWSTR   pwszChip, pwszAdapterString;
    ULONG   i, cbChip, cbAdapterString;

    pwszChip = L"NVxx";
    cbChip = sizeof(L"NVxx");
    pwszAdapterString = L"NVxx";
    cbAdapterString = sizeof(L"NVxx");

    for(i = 0; i < sizeof(sChipInfo) / sizeof(CHIP_INFO); i++)
    {
        if (HwDeviceExtension->ulChipID == sChipInfo[i].ulDevID)
        {
            pwszChip = sChipInfo[i].pwszChip;
            cbChip = cbStrLenW(pwszChip);
            pwszAdapterString = sChipInfo[i].pwszAdapterString; 
            cbAdapterString = cbStrLenW(pwszAdapterString);
            break;
        }
    }

    VideoPortSetRegistryParameters(HwDeviceExtension,
                                   L"HardwareInformation.ChipType",
                                   pwszChip,
                                   cbChip);

    VideoPortSetRegistryParameters(HwDeviceExtension,
                                   L"HardwareInformation.AdapterString",
                                   pwszAdapterString,
                                   cbAdapterString);

    VideoPortSetRegistryParameters(HwDeviceExtension,
                                   L"HardwareInformation.DacType",
                                   L"Integrated RAMDAC",
                                   sizeof(L"Integrated RAMDAC") );

    VideoPortSetRegistryParameters(HwDeviceExtension,
                                   L"HardwareInformation.MemorySize",
                                   &(HwDeviceExtension->NvInfo.Framebuffer.RamSize),
                                   sizeof(ULONG));
    }


//******************************************************************************
//
//  Function:   NV4SetMode
//
//  Routine Description:
//
//  Arguments:
//
//  Return Value:
//
//******************************************************************************

BOOL NV4SetMode(PHW_DEVICE_EXTENSION HwDeviceExtension,
                        PMODE_ENTRY RequestedMode  )
    {
    PHWINFO NVInfo;
    PUCHAR crtc;
    ULONG OffScreenMemory;
    ULONG OnScreenMemory;
    UCHAR  mode, bitdepth, doubled;
    USHORT vesaMode;
    VIDEO_REQUEST_PACKET requestPacket;
    STATUS_BLOCK statusblock;
    PDMT_OVERRIDES pDMTOverrides;
    U016   UseDMTFlag;
    ULONG CurrentValue;
    BOOLEAN bRefreshRateDefault;
    MODE_TIMING_VALUES timingInfo;
    GTF_TIMINGS vbe_timings;
    unsigned int vesaStatus;
    ULONG result;
    
    //**************************************************************************
    //    Current Issues with New Style Modeset Code:
    //
    //    1) NT 3.51 will no longer work with new style modeset code, because
    //       RmSetMode calls RmUnloadState and RmLoadState.
    //       (NT 3.51 does not work with a Resource Manager to my knowledge).
    //
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);
    NVInfo->Framebuffer.HorizDisplayWidth   = RequestedMode->Width;
    NVInfo->Framebuffer.VertDisplayWidth    = RequestedMode->Height;
    NVInfo->Framebuffer.Depth               = RequestedMode->Depth;
    NVInfo->Framebuffer.RefreshRate         = RequestedMode->RefreshRate;

    if ( !(HwDeviceExtension->DeskTopInfo.ulDesktopMode & NV_ENABLE_CLASS_SET_MODE))
    {

        // Added to support GTF or DMT method selection under WinNT4 and Win2K
        if (HwDeviceExtension->bUseGTF)
        {

            //**********************************************************************
            // Here we use GTF timings as requested by the user
            //**********************************************************************

            vesaStatus = vesaGetGTFTimings(NVInfo->Framebuffer.HorizDisplayWidth ,
                                           NVInfo->Framebuffer.VertDisplayWidth,    
                                           NVInfo->Framebuffer.RefreshRate,
                                           &timingInfo);

            if (vesaStatus)
            {
                // Could not find a GTF timing for this mode
                return(FALSE);
            }

        }
        else
        {

            //**************************************************************************
            // Here we use DMT timings  (instead of GTF timings)
            //**************************************************************************

            vesaStatus = vesaGetDMTTimings(NVInfo->Framebuffer.HorizDisplayWidth ,
                                           NVInfo->Framebuffer.VertDisplayWidth,    
                                           NVInfo->Framebuffer.RefreshRate,
                                           &timingInfo);

            //**************************************************************************
            // If DMT timings don't exist for this mode, use the GTF timings
            //**************************************************************************

            if (vesaStatus)
            {

                //**********************************************************************
                // Set mode using GTF timings if DMT timings don't exist
                //**********************************************************************

                vesaStatus = vesaGetGTFTimings(NVInfo->Framebuffer.HorizDisplayWidth ,
                                               NVInfo->Framebuffer.VertDisplayWidth,    
                                               NVInfo->Framebuffer.RefreshRate,
                                               &timingInfo);

                if (vesaStatus)
                {
                    // Could not find a DMT / GTF timing for this mode
                    return(FALSE);
                }
            }
        }
    }




    //**************************************************************************
    // Still need to make sure the 'global' hwdevice variable is in sync.
    // (We need to eventually get rid of this global hwdev variable !!!)
    //**************************************************************************

    SetGlobalHwDev(HwDeviceExtension);

    //**************************************************************************
    // Unlock the registers
    //**************************************************************************

    NV_OEMEnableExtensions(HwDeviceExtension);          

    //**************************************************************************
    // Determine which modes are 'doubled'
    //**************************************************************************

    doubled = 0;  

    if ((NVInfo->Framebuffer.VertDisplayWidth == 384 )  || 
        (NVInfo->Framebuffer.VertDisplayWidth == 360 )  || 
        (NVInfo->Framebuffer.VertDisplayWidth == 300 )  || 
        (NVInfo->Framebuffer.VertDisplayWidth == 240 )  || 
        (NVInfo->Framebuffer.VertDisplayWidth == 200 ))   
        {
        doubled = 1;
        }
        
    if( !(HwDeviceExtension->DeskTopInfo.ulDesktopMode & NV_ENABLE_CLASS_SET_MODE))
    {
        //**************************************************************************
        // Pass the timings to the RM and let it set the mode (via DacProgramCRTC)
        // as well as fixup the modeset arbitration.
        //**************************************************************************


        if (RmSetMode(HwDeviceExtension->DeviceReference,
                  timingInfo.HorizontalVisible,
                  timingInfo.HorizontalBlankStart,
                  timingInfo.HorizontalRetraceStart,
                  timingInfo.HorizontalRetraceEnd,
                  timingInfo.HorizontalBlankEnd,
                  timingInfo.HorizontalTotal,
                  timingInfo.VerticalVisible,
                  timingInfo.VerticalBlankStart,
                  timingInfo.VerticalRetraceStart,
                  timingInfo.VerticalRetraceEnd,
                  timingInfo.VerticalBlankEnd,
                  timingInfo.VerticalTotal,
                  NVInfo->Framebuffer.Depth,
                  NVInfo->Framebuffer.RefreshRate,
                  NVInfo->Framebuffer.HorizDisplayWidth ,
                  NVInfo->Framebuffer.VertDisplayWidth,  
                  timingInfo.PixelClock,
                  timingInfo.HSyncpolarity,
                  timingInfo.VSyncpolarity,
                  doubled) != TRUE)
        {
        return(FALSE);
        }
    }
    //**************************************************************************
    // Save these values off so when we come back from hibernation,
    // we'll know what values to restore
    //**************************************************************************

    HwDeviceExtension->LastWidth        = NVInfo->Framebuffer.HorizDisplayWidth; 
    HwDeviceExtension->LastHeight       = NVInfo->Framebuffer.VertDisplayWidth;
    HwDeviceExtension->LastDepth        = NVInfo->Framebuffer.Depth;
    HwDeviceExtension->LastRefreshRate  = NVInfo->Framebuffer.RefreshRate;


    //**************************************************************************
    // Ask the RM if overlay is allowed at this resolution
    //**************************************************************************

    RmConfigGetKernel(HwDeviceExtension->DeviceReference, NV_CFG_VIDEO_OVERLAY_ALLOWED,&result) ;

    if (result)
        HwDeviceExtension->bHwVidOvl = TRUE;
    else        
        HwDeviceExtension->bHwVidOvl = FALSE;

    //**************************************************************************
    // TV cursor adjust code
    //**************************************************************************

    if (HwDeviceExtension->TvIsPresent)
        {
        U008  ht,hrs,hbs,tvhbe,tvhbs;

        //**********************************************************************
        // Unlock the registers
        //**********************************************************************

        NV_OEMEnableExtensions(HwDeviceExtension);          

        //**********************************************************************
        // Calculate cursor min and max values (in pixels) for TV.
        //**********************************************************************

        CRTC_RD(NV_CIO_CR_HDT_INDEX, ht);   // h total
        CRTC_RD(NV_CIO_CR_HRS_INDEX, hrs);  // h retrace start
        CRTC_RD(NV_CIO_CR_HBS_INDEX, hbs);  // h blank start
        tvhbe = (U008) PRAMDAC_REG_RD32(PRAMDAC_Base,NV_PRAMDAC_TV_HBLANK_END);
        tvhbs = (U008) PRAMDAC_REG_RD32(PRAMDAC_Base,NV_PRAMDAC_TV_HBLANK_START);

        NVInfo->Dac.TVCursorMin = (ht+5-(hrs-2))*8 - tvhbe;
        NVInfo->Dac.TVCursorMax = (ht+5-((hrs-2)+hbs+1))*8 - tvhbs;
//        NVInfo->Dac.TVCursorMin += 20; // What should these values actually be?
//        NVInfo->Dac.TVCursorMax += 20; // What should these values actually be?
        NVInfo->Dac.TVCursorMin = 0;     // Set to zero for now   
        NVInfo->Dac.TVCursorMax = 0;     // Set to zero for now


        }
        

    //**************************************************************************
    // WHQL fixup code:
    //
    // The following code should probably be removed, and is leftover
    // code from the OLD style modeset code.  But we NEED to check
    // that the NEW style modeset code (ie RmSetMode) does indeed set
    // the following registers correctly.  Otherwise, this could cause
    // various WHQL tests to fail.  So for now, leave this code INTACT!
    //**************************************************************************

    {
    //**************************************************************************
    // Specify 5:5:5 or 5:6:5 format
    // ModeSet code currently defaults to 5:6:5 (in OEMSetRegs)
    //**************************************************************************

    if (RequestedMode->Depth == 16)
        {
        //**********************************************************************
        // Currently the display driver only handles 5:5:5 format...
        // So for now, just specify 5:5:5 always until we add the 5:6:5 specific modes
        //**********************************************************************
        PRAMDAC_REG_WR32(PRAMDAC_Base, NV_PRAMDAC_GENERAL_CONTROL,
            (DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _ALT_MODE, _16)) |
               (DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _BPC, _8BITS)));
        }

    else if (RequestedMode->Depth == 8)
        {
        // if 8 bit mode, make sure we're using an 8bit palette
        //if (pDev.Framebuffer.Depth == 8)
        PRAMDAC_REG_WR32(PRAMDAC_Base, NV_PRAMDAC_GENERAL_CONTROL,
                (DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _BPC, _8BITS)) );
        }

    //**************************************************************************
    // Make sure Bit 20 is always set for 32bpp modes
    //**************************************************************************

    if (RequestedMode->Depth == 32)
        {
        CurrentValue = PRAMDAC_REG_RD32(PRAMDAC_Base,NV_PRAMDAC_GENERAL_CONTROL);
        CurrentValue |= (DRF_DEF(_PRAMDAC, _GENERAL_CONTROL, _BPC, _8BITS)) ;
        PRAMDAC_REG_WR32(PRAMDAC_Base, NV_PRAMDAC_GENERAL_CONTROL,CurrentValue);
        }


    //**************************************************************************
    // Make sure to turn off 2D dithering.  Otherwise, HCT tests will fail!
    //**************************************************************************

    CurrentValue = PGRAPH_REG_RD32(PGRAPH_Base,NV_PGRAPH_DEBUG_3);

    //**************************************************************************
    // Mask off the 2d dither bits
    //**************************************************************************

    CurrentValue &= (~ ( (DRF_DEF(_PGRAPH, _DEBUG_3, _POSTDITHER_2D, _ENABLED )) |
                         (DRF_DEF(_PGRAPH, _DEBUG_3, _PREDITHER_2D, _ENABLED )) ));


    PGRAPH_REG_WR32(PGRAPH_Base,NV_PGRAPH_DEBUG_3,CurrentValue);

    }
    

    //**************************************************************************
    // If flat panel is present, make sure to clear out the following registers
    // because video bios's after 09/02/99 may touch these registers.
    // These registers should probably get cleared in dacSetFlatPanelMode in the RM
    //**************************************************************************

    if (HwDeviceExtension->FlatPanelIsPresent == TRUE)
        {
        REG_WR32(NV_PRAMDAC_FP_DEBUG_1, 0x00000000);
        REG_WR32(NV_PRAMDAC_FP_DEBUG_2, 0x00000000);
        }


    //**************************************************************************
    // Another ALI 1541 workaround fix
    //**************************************************************************


    if (HwDeviceExtension->AliChipset_Problem_Found==TRUE)
    
        {

        //**********************************************************************
        //
        // This chipset has another bug, confirmed by ALI, where it cannot
        // accept fast DEVSEL# at 100MHz bus speeds.  We cannot adjust our
        // DEVSEL# speeds, but we can delay our TRDY by one clock cycle, which
        // should have the same effect.
        //
        // Note that we (NV) have seen no improvement in any of the ALI1541
        // issues with this change, but ALI is adament there is a bug in the 1541
        // having to do with this logic.  Better safe than sorry.
        //
        // This bit is present in NV4, NV5, and NV10.
        //
        //**********************************************************************

        CurrentValue = REG_RD32(NV_PBUS_DEBUG_1);
        CurrentValue |= (1 << DRF_SHIFT(NV_PBUS_DEBUG_1_PCIS_WRITE));
        REG_WR32(NV_PBUS_DEBUG_1,  CurrentValue);

        }
               

    return(TRUE);
    } // end NV4SetMode()

//******************************************************************************
//
// Function: NV_SetColorLookup()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VOID NV_SetColorLookup(
    PHW_DEVICE_EXTENSION HwDeviceExtension,
    PVIDEO_CLUT ClutBuffer,
    ULONG ClutBufferSize
    )
    {
    ULONG i;

    //**************************************************************************
    // wait for vblank
    //**************************************************************************
    if(HwDeviceExtension->curMonitorPowerState == VideoPowerOn)
    {
        if(HwDeviceExtension->curAdapterPowerState == VideoPowerOn)
        {
            while (!(PRMCIO_REG_RD08(PRMCIO_Base, NV_PRMCIO_INP0__COLOR) & 0x08));
        }
    }

    //**************************************************************************
    //  Specify first palette register
    //**************************************************************************
    USERDAC_REG_WR08(USERDAC_Base, NV_USER_DAC_WRITE_MODE_ADDRESS, (UCHAR) ClutBuffer->FirstEntry);

    //**************************************************************************
    //  Set CLUT registers directly on the hardware
    //**************************************************************************

    for (i = 0; i < ClutBuffer->NumEntries; i++)
        {
        USERDAC_REG_WR08(USERDAC_Base, NV_USER_DAC_PALETTE_DATA, (UCHAR) ClutBuffer->LookupTable[i].RgbArray.Red);
        USERDAC_REG_WR08(USERDAC_Base, NV_USER_DAC_PALETTE_DATA, (UCHAR) ClutBuffer->LookupTable[i].RgbArray.Green);
        USERDAC_REG_WR08(USERDAC_Base, NV_USER_DAC_PALETTE_DATA, (UCHAR) ClutBuffer->LookupTable[i].RgbArray.Blue);
        }

    }

//******************************************************************************
//
// Function: NV4_SetPixMixBits()
//
// Routine Description: Set the NV_PRAMDAC_GENERAL_CONTROL_PIXMIX to 
//                      NV_PRAMDAC_GENERAL_CONTROL_PIXMIX_ON
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV_SetPixMixBits(
    PHW_DEVICE_EXTENSION    HwDeviceExtension
)
{
    ULONG   ulGeneralCtrl;

    ulGeneralCtrl = PRAMDAC_REG_RD32(PRAMDAC_Base, NV_PRAMDAC_GENERAL_CONTROL);

    //
    //  Set NV_PRAMDAC_GENERAL_CONTROL_PIXMIX_ON
    //

    ulGeneralCtrl |= 0x30;
    PRAMDAC_REG_WR32(PRAMDAC_Base, NV_PRAMDAC_GENERAL_CONTROL,ulGeneralCtrl);
}

//******************************************************************************
//
// Function: NV4_InitPalette()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VOID NV4_InitPalette(
    PHW_DEVICE_EXTENSION HwDeviceExtension
    )
    {
    USHORT i;

    //**************************************************************************
    // Blast out our palette values.  First Init to index 0
    //**************************************************************************
    USERDAC_REG_WR08(USERDAC_Base, NV_USER_DAC_WRITE_MODE_ADDRESS, (UCHAR) 0);

    //**************************************************************************
    // Init to a gray scale so something will appear instead of just black
    // 256 * 3 (for each R,G, and B component)
    //**************************************************************************

    for (i = 0; i < 256; i++)
        {
        //**********************************************************************
        // Just initialize to a gray scale for now.
        //**********************************************************************
        USERDAC_REG_WR08(USERDAC_Base, NV_USER_DAC_PALETTE_DATA, (UCHAR) i);
        USERDAC_REG_WR08(USERDAC_Base, NV_USER_DAC_PALETTE_DATA, (UCHAR) i);
        USERDAC_REG_WR08(USERDAC_Base, NV_USER_DAC_PALETTE_DATA, (UCHAR) i);
        }

    return;


    }


//******************************************************************************
//
// Function:    NVMapMemoryRanges()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

ULONG NVMapMemoryRanges(PVOID HwDeviceExtension)

    {
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    PHYSICAL_ADDRESS CrtcRange;
    PHYSICAL_ADDRESS TimerRange;
    PHYSICAL_ADDRESS NV_PhysBaseAddress;
    PHYSICAL_ADDRESS NV_PhysFBAddress;
    ULONG CrtcLength;
    ULONG TimerLength;
    ULONG NV_RegisterLength;
    ULONG NV_FrameBufferLength;
    PHWREG CrtcAddress;
    PHWREG TimerAddress;
    PHWREG NV_LinearBaseAddress;
    PHWREG NV_LinearFBAddress;
    PHYSICAL_ADDRESS ConfigPciRange;
    ULONG ConfigPciLength;
    PHWREG ConfigPciAddress;
    ULONG CurrentValue;

    //**************************************************************************
    // Make SURE to always ENABLE bus mastering!  Our chip needs it ENABLED !!
    // (Some DELL Bios's disable bus mastering on our card)
    //**************************************************************************

    //**************************************************************************
    // First get access to the CONFIG_PCI_NV1 register
    //**************************************************************************

    ConfigPciRange.HighPart  = 0x00000000;
    ConfigPciRange.LowPart   = hwDeviceExtension->NVAccessRanges[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    ConfigPciRange.LowPart   += NV_PBUS_PCI_NV_1;
    ConfigPciLength          = 0x1000;   // Arbitrary 4k length

    ConfigPciAddress=VideoPortGetDeviceBase(hwDeviceExtension,
                                         ConfigPciRange,
                                         ConfigPciLength, FALSE);
    if (ConfigPciAddress==NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map Config PCI Reg\n"));
        return(ERROR_INVALID_PARAMETER);
        }

    //**************************************************************************
    // Make sure bus mastering is always ENABLED !!  (Bit 2)
    // Read current value then enable the bus_master bit.
    //**************************************************************************

    CurrentValue = TEMP_REG_RD32(ConfigPciAddress);
    CurrentValue |= (1 << DRF_SHIFT(NV_PBUS_PCI_NV_1_BUS_MASTER));
    TEMP_REG_WR32(ConfigPciAddress,  CurrentValue);

    //**************************************************************************
    // Free up this temporary range
    //**************************************************************************

    VideoPortFreeDeviceBase(hwDeviceExtension, ConfigPciAddress);


    //**************************************************************************
    // Make SURE to disable VBLANK and TIMER interrupts on startup!
    // (Some COMPAQ systems do not reset the chip, so these interrupts
    // may inadvertently get left enabled.  These interrupts (vblank/timer) then
    // enter our interrupt handler before its ready to accept them)
    //**************************************************************************

    //**************************************************************************
    // First get access to VBLANK interrupt enable register
    //**************************************************************************

    CrtcRange.HighPart  = 0x00000000;
    CrtcRange.LowPart   = hwDeviceExtension->NVAccessRanges[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    CrtcRange.LowPart   += NV_PCRTC_INTR_EN_0;
    CrtcLength          = 0x1000;   // Arbitrary 4k length

    CrtcAddress=VideoPortGetDeviceBase(hwDeviceExtension,
                                         CrtcRange,
                                         CrtcLength, FALSE);
    if (CrtcAddress==NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map Crtc Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }

    //**************************************************************************
    // Get access to TIMER interrupt enable register
    //**************************************************************************

    TimerRange.HighPart  = 0x00000000;
    TimerRange.LowPart   = hwDeviceExtension->NVAccessRanges[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    TimerRange.LowPart   += NV_PTIMER_INTR_EN_0 ;
    TimerLength          = 0x1000;   // Arbitrary 4k length

    TimerAddress=VideoPortGetDeviceBase(hwDeviceExtension,
                                         TimerRange,
                                         TimerLength, FALSE);
    if (TimerAddress==NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map Timer Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }

    //**************************************************************************
    // Make sure these interrupts are always DISABLED initially!
    //**************************************************************************

    TEMP_REG_WR32(CrtcAddress,  0x00000000);
    TEMP_REG_WR32(TimerAddress, 0x00000000);

    //**************************************************************************
    // Free up these temporary ranges
    //**************************************************************************

    VideoPortFreeDeviceBase(hwDeviceExtension, CrtcAddress);
    VideoPortFreeDeviceBase(hwDeviceExtension, TimerAddress);

    //**************************************************************************
    // Map entire NV register space
    //**************************************************************************

    NV_PhysBaseAddress.HighPart = 0x00000000;
    NV_PhysBaseAddress.LowPart  = hwDeviceExtension->NVAccessRanges[NV_PHYSICAL_BASE_ADDRESS].RangeStart.LowPart;
    NV_RegisterLength           = 0x1000000;    // 16M

    if ( (  NV_LinearBaseAddress = VideoPortGetDeviceBase(hwDeviceExtension,
                                                          NV_PhysBaseAddress,
                                                          NV_RegisterLength, FALSE)) == NULL)
        {
        VideoDebugPrint((1, "NVFindAdapter - Failed to map PGRAPH Regs\n"));
        return(ERROR_INVALID_PARAMETER);
        }
    hwDeviceExtension->NvRegisterBase = NV_LinearBaseAddress;

    //**************************************************************************
    // Initialize the 'individual' memory ranges
    //**************************************************************************

    hwDeviceExtension->NV1_Lin_PMC_Registers    = NV_LinearBaseAddress + (DEVICE_BASE(NV_PMC)/4);
    hwDeviceExtension->NV1_Lin_PFB_Registers    = NV_LinearBaseAddress + (DEVICE_BASE(NV_PFB)/4);
    hwDeviceExtension->NV1_Lin_PFIFO_Registers  = NV_LinearBaseAddress + (DEVICE_BASE(NV_PFIFO)/4);
    hwDeviceExtension->NV1_Lin_PBUS_Registers   = NV_LinearBaseAddress + (DEVICE_BASE(NV_PBUS)/4);
    hwDeviceExtension->NV1_Lin_PGRAPH_Registers = NV_LinearBaseAddress + (DEVICE_BASE(NV_PGRAPH)/4);
    hwDeviceExtension->NV1_Lin_PEXTDEV_Registers= NV_LinearBaseAddress + (DEVICE_BASE(NV_PEXTDEV)/4);
    hwDeviceExtension->NV3_Lin_PRMVIO_Registers = NV_LinearBaseAddress + (DEVICE_BASE(NV_PRMVIO)/4);
    hwDeviceExtension->NV3_Lin_PRMCIO_Registers = NV_LinearBaseAddress + (DEVICE_BASE(NV_PRMCIO)/4);
    hwDeviceExtension->NV3_Lin_PRAMDAC_Registers= NV_LinearBaseAddress + (DEVICE_BASE(NV_PRAMDAC)/4);
    hwDeviceExtension->NV3_Lin_USERDAC_Registers= NV_LinearBaseAddress + (DEVICE_BASE(NV_USER_DAC)/4);
    hwDeviceExtension->NV3_Lin_PVIDEO_Registers = NV_LinearBaseAddress + (DEVICE_BASE(NV_PVIDEO)/4);
    hwDeviceExtension->NV1_Lin_USER_Registers   = NV_LinearBaseAddress + (DEVICE_BASE(NV_USER)/4);
    hwDeviceExtension->pBios = (PUCHAR)(&((hwDeviceExtension->NvRegisterBase)->Reg008[DEVICE_BASE(NV_PRAMIN)]));

    //**************************************************************************
    // Return successful
    //**************************************************************************

    return(NO_ERROR);

    } // NVMapMemoryRanges()

//******************************************************************************
//
// Function:    NVMapFrameBuffer()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

ULONG NVMapFrameBuffer(PVOID HwDeviceExtension)

    {
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    VIDEO_MEMORY_INFORMATION memoryInformation;
    PHWINFO NVInfo;
    PHYSICAL_ADDRESS NVFrameBuffer;
    ULONG inIoSpace;
    ULONG status;
    ULONG MapMemoryAdjust;

    //**************************************************************************
    // Get the amount of memory available on the card
    // (Determined in GetNVInfo)
    //**************************************************************************

    NVInfo = &(hwDeviceExtension->NvInfo);
    memoryInformation.VideoRamLength = NVInfo->Framebuffer.RamSize;

    //**************************************************************************
    // Set VideoRamBase to zero, to map range at any location in the
    // logical address space of the current process
    //**************************************************************************

    memoryInformation.VideoRamBase = 0;

    //**************************************************************************
    // Get the physical address of the frame buffer
    //**************************************************************************

    NVFrameBuffer.HighPart = 0x00000000;
    NVFrameBuffer.LowPart  = hwDeviceExtension->NVAccessRanges[NV_PHYSICAL_DFB_ADDRESS].RangeStart.LowPart;
    NVFrameBuffer.LowPart += RangeOffsets[NV_PDFB_INDEX].ulOffset;

    //**************************************************************************
    // Always set write combining for NT4.0 or later
    // Do NOT set it for NT 3.51 !!!!
    //**************************************************************************

#if (_WIN32_WINNT >= 0x0400)
    inIoSpace = VIDEO_MEMORY_SPACE_P6CACHE;
#else
    inIoSpace = 0;
#endif

    //**************************************************************************
    // When calling RmPostNvDevice, and passing in our ptr to the frame buffer,
    // RmPostNvDevice needs to read and write a little bit PAST the end
    // of video memory in order to size it.  
    // 
    // So, in order to accomodate this and prevent a fatal exception error, 
    // we'll allocate just a little bit more memory than the actual amount needed. 
    // (Only necessary for Win2k or better)
    //**************************************************************************

    MapMemoryAdjust = memoryInformation.VideoRamLength;

#if (_WIN32_WINNT >= 0x0500)
    MapMemoryAdjust += BIOS_MEM_SIZE_ADJUST_FACTOR;
#endif


    status = VideoPortMapMemory(hwDeviceExtension,
                                 NVFrameBuffer,
                                 &(MapMemoryAdjust),
                                 &(inIoSpace),
                                 &(memoryInformation.VideoRamBase));


#if _WIN32_WINNT < 0x0500

    //**************************************************************************
    // Special Case unattended setup issue:
    //
    //      Normally, a 32Mb video card will install successfully on a system 
    //      with 32Mb of system memory, when the display driver is installed manually.
    //
    //      However, UNATTENDED setup of a 32Mb video card on a system with 32Mb
    //      may fail.  And what makes matters worse is that during unattended setup,
    //      the driver is usually installed BEFORE the service packs are applied.
    //      (Display driver should REALLY be installed AFTER the service pack is applied)
    //
    //      To alleviate this situation and allow a 32Mb video card to be installed
    //      via unattended setup, we'll try just allocating 16Mb if the above VideoPortMapMemory 
    //      call fails.  This is REALLY not valid (and may result in the driver
    //      not working correctly), but it will allow the driver to install
    //      and get past unattended setup.
    //
    //      After the service pack is applied, subsequent VideoPortMapMemory calls should 
    //      then succeed with no problem.    
    //       
    //**************************************************************************

    if (status != NO_ERROR)
        {

        NVInfo->Framebuffer.RamSizeMb = 16;
        NVInfo->Framebuffer.RamSize   = 0x01000000;

        memoryInformation.VideoRamLength = NVInfo->Framebuffer.RamSize;
        memoryInformation.VideoRamBase = 0;


        status = VideoPortMapMemory(hwDeviceExtension,
                                     NVFrameBuffer,
                                     &(memoryInformation.VideoRamLength),
                                     &(inIoSpace),
                                     &(memoryInformation.VideoRamBase));
        }
#endif



    hwDeviceExtension->FrameBufferPtr = memoryInformation.VideoRamBase;
    hwDeviceExtension->FrameBufferBase= memoryInformation.VideoRamBase;

    //**************************************************************************
    // If this call fails, driver should revert back to standard VGA
    //**************************************************************************

    return(status);



    }


//******************************************************************************
//
// Function: NVIsPresent()
//
// Routine Description:
//
// Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
// Return Value:
//
//******************************************************************************


BOOLEAN NVIsPresent(PHW_DEVICE_EXTENSION HwDeviceExtension,
                      PULONG NV1Slot)

    {
    USHORT  usVendorId;
    USHORT  usDeviceId;
    ULONG   ulSlot     = 0;
    ULONG   i;

        //
        // NOTE: the board detection part in this routine was moved into DetectNextDevice (see the bottom of this file)
        // 


        //**********************************************************************
        // Init NV memory range offset values to zero
        //**********************************************************************

        for (i=0 ; i<NV_NUM_RANGES ;i++)
          {
          RangeOffsets[i].ulOffset = 0;
          RangeOffsets[i].ulLength = 0;
          }

        //**********************************************************************
        // Initialize the actual offset values
        //**********************************************************************

        RangeOffsets[NV_PMC_INDEX].ulOffset = DEVICE_BASE(NV_PMC);
        RangeOffsets[NV_PMC_INDEX].ulLength = DEVICE_EXTENT(NV_PMC);

        RangeOffsets[NV_PFB_INDEX].ulOffset = DEVICE_BASE(NV_PFB);
        RangeOffsets[NV_PFB_INDEX].ulLength = DEVICE_EXTENT(NV_PFB);

        RangeOffsets[NV_PFIFO_INDEX].ulOffset = DEVICE_BASE(NV_PFIFO);
        RangeOffsets[NV_PFIFO_INDEX].ulLength = DEVICE_EXTENT(NV_PFIFO);

        RangeOffsets[NV_PBUS_INDEX].ulOffset = DEVICE_BASE(NV_PBUS);
        RangeOffsets[NV_PBUS_INDEX].ulLength = DEVICE_EXTENT(NV_PBUS);

        RangeOffsets[NV_PGRAPH_INDEX].ulOffset = DEVICE_BASE(NV_PGRAPH);
        RangeOffsets[NV_PGRAPH_INDEX].ulLength = DEVICE_EXTENT(NV_PGRAPH);

        RangeOffsets[NV_PEXTDEV_INDEX].ulOffset = DEVICE_BASE(NV_PEXTDEV);
        RangeOffsets[NV_PEXTDEV_INDEX].ulLength = DEVICE_EXTENT(NV_PEXTDEV);

        RangeOffsets[NV_PRMVIO_INDEX].ulOffset = DEVICE_BASE(NV_PRMVIO);
        RangeOffsets[NV_PRMVIO_INDEX].ulLength = DEVICE_EXTENT(NV_PRMVIO);

        RangeOffsets[NV_PRMCIO_INDEX].ulOffset = DEVICE_BASE(NV_PRMCIO);
        RangeOffsets[NV_PRMCIO_INDEX].ulLength = DEVICE_EXTENT(NV_PRMCIO);

        RangeOffsets[NV_PRAMDAC_INDEX].ulOffset = DEVICE_BASE(NV_PRAMDAC);
        RangeOffsets[NV_PRAMDAC_INDEX].ulLength = DEVICE_EXTENT(NV_PRAMDAC);

        RangeOffsets[NV_USERDAC_INDEX].ulOffset = DEVICE_BASE(NV_USER_DAC);
        RangeOffsets[NV_USERDAC_INDEX].ulLength = DEVICE_EXTENT(NV_USER_DAC);

        RangeOffsets[NV_PVIDEO_INDEX].ulOffset = DEVICE_BASE(NV_PVIDEO);
        RangeOffsets[NV_PVIDEO_INDEX].ulLength = DEVICE_EXTENT(NV_PVIDEO);

        //**********************************************************************
        // Currently allows us to write to 64k of PRAMIN memory
        //**********************************************************************

        RangeOffsets[NV_PRAMIN_INDEX].ulOffset = (ULONG) (DEVICE_BASE(NV_PRAMIN) - DEVICE_BASE(NV_PDFB));
        RangeOffsets[NV_PRAMIN_INDEX].ulLength = 0x10000;

        RangeOffsets[NV_USER_INDEX].ulOffset = DEVICE_BASE(NV_USER);
        RangeOffsets[NV_USER_INDEX].ulLength = 0x10000;

        //**********************************************************************
        // Just allocate memory for the GRAPHICS STATUS register
        // Allocate minimal 16 bytes because we're only mapping 1 register
        //**********************************************************************

        RangeOffsets[NV_PGRAPH_STATUS_INDEX].ulOffset = NV_PGRAPH_STATUS;
        RangeOffsets[NV_PGRAPH_STATUS_INDEX].ulLength = 16;

        //**********************************************************************
        // Setmode still uses PFB_CONFIG
        //**********************************************************************

        RangeOffsets[NV_PFB_CONFIG_0_INDEX].ulOffset = NV_PFB_CONFIG_0;
        RangeOffsets[NV_PFB_CONFIG_0_INDEX].ulLength = 16;

        //**********************************************************************
        // DUMB FRAME BUFFER location
        // The actual amount of memory is determined later in NV_GetNVInfo,
        // but for now, just default to 4Mb.  This value will be fixed up
        // in NV_GetNVInfo so that IOCTL_VIDEO_MAP_MEMORY will subsequently
        // map in the correct number of bytes.
        //**********************************************************************

        RangeOffsets[NV_PDFB_INDEX].ulOffset = 0x0;
        RangeOffsets[NV_PDFB_INDEX].ulLength =  0x400000;

        return(TRUE);
    }



//******************************************************************************
//
// Function:    MiniportGetRegistryValue
//
// Routine Description:
//
//    Unlock the CRTC registers
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

BOOL MiniportGetRegistryValue
    (PHW_DEVICE_EXTENSION HwDeviceExtension,
     U032 Type,
     char *Node,
     char *Name,
     VOID *pData)

    {
    ULONG NumRegValueEntries = sizeof(Nv_Registry_Switch_Offset_Table) / sizeof(NT_NV_REGISTRY_SWITCH_OFFSET);    
    ULONG i=0;   
    BOOL FoundFlag = FALSE;
    ULONG FoundIndex;
    BOOL status;
    char *RegValuePtr;
    char TempNameString[REG_STRING_DATA_MAX];
    WCHAR parameterName[80];
    

    //**************************************************************************
    // This function is called by the RM to get registry switch values.
    // Normally, the RM expects a 'Win9x' type registry, with various levels
    // of keys.  But with WinNT, all our registry switches must fit in
    // ONE key (ie. a flat structure).
    //
    // This routine attempts to find the registry switch name in our
    // registry switch table.  If it's present, we return the value (or string) back.
    // Otherwise, we return false   
    //**************************************************************************

    //**************************************************************************
    // First convert the registry value name string, and
    // convert to all upper case
    //**************************************************************************
    
    my_strcpy(TempNameString,Name);
    my_strupr(TempNameString);
    
    //**************************************************************************
    // Next, Search our registry table to see if this registry switch exists
    // in our 'local' registry structure.
    //**************************************************************************
        
    while ( (i < NumRegValueEntries) && (FoundFlag == FALSE) )
        {
        //**********************************************************************
        // Convert the registry switch names in our local registry table to upper case
        //**********************************************************************

        my_strupr(&Nv_Registry_Switch_Offset_Table[i].RegName[0]);

        //**********************************************************************
        // Now compare and see if this registry switch exists in our table
        // If so, then get the index into our registry switch table.
        //**********************************************************************
        
        status = my_strcmp(&Nv_Registry_Switch_Offset_Table[i].RegName[0], TempNameString);
        if (status == 0)
            {
            FoundFlag = TRUE;
            FoundIndex = i;
            }
                        
        i++;                
        }


    //**************************************************************************
    // If the registry switch was found in our table, then
    // get the corresponding value from our RegistrySwitch structure 
    // in our HwDeviceExtension.  Otherwise, return FALSE. 
    //**************************************************************************

    if (FoundFlag==TRUE)

        {

        if (Type == IS_STRING)

            {
            //******************************************************************
            // Copy the String value
            //******************************************************************

            RegValuePtr = (CHAR *) ((PUCHAR)&HwDeviceExtension->NV_Registry_Table + Nv_Registry_Switch_Offset_Table[FoundIndex].Offset); 
            my_strcpy(pData, RegValuePtr);
            }

        else if (Type == IS_DWORD)

            {
            //******************************************************************
            // Return the DWORD value
            //******************************************************************

            RegValuePtr = (CHAR *) ((PUCHAR)&HwDeviceExtension->NV_Registry_Table + Nv_Registry_Switch_Offset_Table[FoundIndex].Offset); 
            *(ULONG *)pData = *(ULONG *)RegValuePtr;
            

            //******************************************************************
            // Check if we should IGNORE the registry value
            //******************************************************************

            if ( *(ULONG *)pData == IGNORE_REGISTRY_SWITCH)
                return(FALSE);



            }
                
        return(TRUE);
        }
                
          
    else

        {

        //**********************************************************************
        // Unknown registry switch encountered.
        // This may happen if the RM is looking at a Win9x registry switch
        // and it hasn't been added to the NT registry switch table.
        // 
        // In that case, just read the registry switch as a normal
        // value entry (instead of inside our own local NT registry structure)
        // First, convert the parameter into a unicode string
        //**********************************************************************


        for (i = 0; Name[i]; i++)
            {
            parameterName[i] = Name[i];
            }
        parameterName[i] = (WCHAR)NULL;


        status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       parameterName,
                                       FALSE,
                                       NVReadRegistrySwitchesCallback,
                                       (ULONG *)pData);

        if (status == NO_ERROR)
            return(TRUE);
        else
            return(FALSE);            

        }
        
                    
       
    }
    

//******************************************************************************
//
// Function:    LoadRegistrySettings
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID LoadRegistrySettings(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    ULONG status;

    //**************************************************************************
    // There are 2 ways we can store registry switches for our NT driver:
    // 
    //   1) Store each registry switch in its own value entry
    //
    //      Advantages:     Easy to edit individual entries with RegEdit
    //      Disadvantages:  Lots of value entries are created which clutter
    //                      up the registry key.  If we need to delete them,
    //                      we may have to delete the entries one by one.
    //
    //   2) Store ALL the registry switches as a chunk in one value entry
    //         
    //      Advantages:     All the registry switches are in one place, not as unwieldly
    //      Disadvantes:    Not as easy to edit individual entries with RegEdit.
    //                          
    // 
    //   For now, we'll go with option 2) and store the switches as
    //   one chunk in a Value Entry.
    //**************************************************************************

    //**************************************************************************
    // First, check and see if our NT registry value entry (which contains our
    // local registry switches) is present or not.  If it's present, then
    // just read it straight into our registry structure.
    //**************************************************************************

    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       L"NVREGSWITCHES",
                                       FALSE,
                                       NVReadRegistrySwitchesCallback,
                                       &(HwDeviceExtension->NV_Registry_Table));

    if (status == NO_ERROR)
        {
        return;
        }
        
    else
    
        {
        //**********************************************************************
        // If the registry value entry was NOT present, then
        //   1) Load up our registry structure with default values, then
        //   2) Write the registry structure to the registry.   
        //**********************************************************************

        LoadDefaultRegistrySwitchValues(HwDeviceExtension);
                
        //**********************************************************************
        // Write the registry switches to the registry as one binary chunk (a structure)
        //**********************************************************************

        status = VideoPortSetRegistryParameters(HwDeviceExtension,
                                       L"NVREGSWITCHES",
                                       &(HwDeviceExtension->NV_Registry_Table),
                                       sizeof(HwDeviceExtension->NV_Registry_Table));

        return;        
        }

    }
    

//******************************************************************************
//
// Function:    LoadDefaultRegistrySwitchValues
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID LoadDefaultRegistrySwitchValues(
    PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    ULONG status;

// STUB OUT FOR NOW SINCE \common\NVREG.H is constantly changing 
// and breaking the NT build.  When NVREG.H settles down, then
// we can put this function back in

//    //**************************************************************************
//    // Default DISPLAY registry switch values        
//    //**************************************************************************
//
//    my_strcpy(&(HwDeviceExtension->NV_Registry_Table.Display.LastDisplayDevice[0]),     NV4_REG_DRV_LAST_DEVICE_CRT );
//    my_strcpy(&(HwDeviceExtension->NV_Registry_Table.Display.LastCRTMode[0]),           "640,480,8,60"  );
//    my_strcpy(&(HwDeviceExtension->NV_Registry_Table.Display.LastDFPMode[0]),           "640,480,8,60"  );
//    my_strcpy(&(HwDeviceExtension->NV_Registry_Table.Display.LastNTSCMode[0]),          "640,480,8,60"  );
//    my_strcpy(&(HwDeviceExtension->NV_Registry_Table.Display.LastPALMode[0]),           "640,480,8,60"  );
//    my_strcpy(&(HwDeviceExtension->NV_Registry_Table.Display.CRTMode[0]),               "640,480,8,60"  );
//    my_strcpy(&(HwDeviceExtension->NV_Registry_Table.Display.DFPMode[0]),               "640,480,8,60"  );
//    my_strcpy(&(HwDeviceExtension->NV_Registry_Table.Display.NTSCMode[0]),              "640,480,8,60"  );
//    my_strcpy(&(HwDeviceExtension->NV_Registry_Table.Display.PALMode[0]),               "640,480,8,60"  );
//    my_strcpy(&(HwDeviceExtension->NV_Registry_Table.Display.DefaultCRTRefreshRate[0]), "60" );
//    my_strcpy(&(HwDeviceExtension->NV_Registry_Table.Display.DefaultDFPRefreshRate[0]), "60" );
//    my_strcpy(&(HwDeviceExtension->NV_Registry_Table.Display.OptimalCRTRefreshRate[0]), "60" );
//    my_strcpy(&(HwDeviceExtension->NV_Registry_Table.Display.OptimalDFPRefreshRate[0]), "60" );
//    my_strcpy(&(HwDeviceExtension->NV_Registry_Table.Display.MonitorTiming[0]),          NV4_REG_DRV_MONITOR_TIMING_DMT);
//
//    HwDeviceExtension->NV_Registry_Table.Display.DisplayType =                           DISPLAY_TYPE_MONITOR;
//    HwDeviceExtension->NV_Registry_Table.Display.TVType =                                DISPLAY_TYPE_MONITOR;
//    HwDeviceExtension->NV_Registry_Table.Display.CursorAlpha =                           0;
//
//    //**************************************************************************
//    // Default DIRECTDRAW registry switch values        
//    //**************************************************************************
//
//    HwDeviceExtension->NV_Registry_Table.DirectDraw.VPENABLE                    = 0;
//    HwDeviceExtension->NV_Registry_Table.DirectDraw.OverlayMode                 = 0;
//    HwDeviceExtension->NV_Registry_Table.DirectDraw.OverlayColorControlEnable   = 0;
//    HwDeviceExtension->NV_Registry_Table.DirectDraw.OverlayBrightness           = 0;
//    HwDeviceExtension->NV_Registry_Table.DirectDraw.OverlayContrast             = 0;
//    HwDeviceExtension->NV_Registry_Table.DirectDraw.OverlayHue                  = 0;
//    HwDeviceExtension->NV_Registry_Table.DirectDraw.OverlaySaturation           = 0;
//    HwDeviceExtension->NV_Registry_Table.DirectDraw.OverlayGamma                = 0;
//    HwDeviceExtension->NV_Registry_Table.DirectDraw.OverlaySharpness            = 0;
//
//    //**************************************************************************
//    // Default DIRECT3D registry switch values        
//    //**************************************************************************
//
//    HwDeviceExtension->NV_Registry_Table.Direct3D.ANTIALIASENABLE               = 0;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.COLORKEYCOMPATABILITYENABLE   = 0;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.CONTROLTRAFFIC                = 0;             
//    HwDeviceExtension->NV_Registry_Table.Direct3D.DIRECTMAPENABLE               = 0;            
//    HwDeviceExtension->NV_Registry_Table.Direct3D.FOGTABLENABLE                 = 0;              
//    HwDeviceExtension->NV_Registry_Table.Direct3D.LOGOENABLE                    = 0;                 
//    HwDeviceExtension->NV_Registry_Table.Direct3D.SCENESYNCENABLE               = 0;            
//    HwDeviceExtension->NV_Registry_Table.Direct3D.TILINGENABLE                  = 0;                
//    HwDeviceExtension->NV_Registry_Table.Direct3D.USERMIPMAPENABLE              = 0;            
//    HwDeviceExtension->NV_Registry_Table.Direct3D.VIDEOTEXTUREENABLEENABLE      = 0;    
//    HwDeviceExtension->NV_Registry_Table.Direct3D.VSYNCENABLE                   = 0;                 
//    HwDeviceExtension->NV_Registry_Table.Direct3D.WENABLE                       = 0;                     
//    HwDeviceExtension->NV_Registry_Table.Direct3D.Z24ENABLE                     = 0;                   
//
//    HwDeviceExtension->NV_Registry_Table.Direct3D.AAMETHOD                      = D3D_REG_AAMETHOD_MIN;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.AUTOMIPMAPMETHOD              = D3D_REG_AUTOMIPMAPMETHOD_BILINEAR; 
//    HwDeviceExtension->NV_Registry_Table.Direct3D.CKREF                         = D3D_REG_CKREF_MIN;                       
//    HwDeviceExtension->NV_Registry_Table.Direct3D.DX6ENABLE                     = 0;                   
//    HwDeviceExtension->NV_Registry_Table.Direct3D.ANISOTOPIC4TAP                = D3D_REG_MIPMAPDITHERMODE_DISABLE;  
//    HwDeviceExtension->NV_Registry_Table.Direct3D.PAL8TEXTURECONVERT            = D3D_REG_PAL8TEXCONVERT_NONE; 
//    HwDeviceExtension->NV_Registry_Table.Direct3D.PMTRIGGER                     = 0;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.TEXELALIGNMENT                = D3D_REG_TEXELALIGNMENT_ZOH_CENTER;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.VALIDATEZMETHOD               = D3D_REG_VALIDATEZMETHOD_FLEXIBLE;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.W16FORMAT                     = D3D_REG_WFORMAT_FIXED;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.W32FORMAT                     = D3D_REG_WFORMAT_FIXED;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.AGPTEXCUTOFF                  = D3D_REG_DEFAULT_AGPTEXCUTOFF;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.D3DCONTEXTMAX                 = D3D_REG_DEFAULT_D3DCONTEXTMAX; 
//    HwDeviceExtension->NV_Registry_Table.Direct3D.D3DTEXTUREMAX                 = D3D_REG_DEFAULT_D3DTEXTUREMAX;  
//    HwDeviceExtension->NV_Registry_Table.Direct3D.DMAMINPUSHCOUNT               = D3D_REG_DEFAULT_DMAMINPUSHCOUNT;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.LODBIASADJUST                 = D3D_REG_DEFAULT_LODBIASADJUST;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.MINVIDEOTEXSIZE               = D3D_REG_DEFAULT_MINVIDEOTEXSIZE;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.MIPMAPLEVELS                  = D3D_REG_DEFAULT_MIPMAPLEVELS;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.PERFSTRATEGY                  = 0;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.NOVSYNCPREREDNERLIMIT         = D3D_REG_DEFAULT_PRERENDERLIMIT;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.DMAPUSHBUFFERSIZEMAX          = D3D_REG_DEFAULT_PUSHBUFFERSIZEMAX;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.TEXHEAP                       = D3D_REG_DEFAULT_TEXHEAPSIZEMAX;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.WSCALE16                      = D3D_REG_DEFAULT_WSCALE16;
//    HwDeviceExtension->NV_Registry_Table.Direct3D.WSCALE24                      = D3D_REG_DEFAULT_WSCALE24;
//
//    //**************************************************************************
//    // Default SYSTEM registry switch values        
//    //**************************************************************************
//                    
//    HwDeviceExtension->NV_Registry_Table.System.Super7Compat                    = 0;    
//    HwDeviceExtension->NV_Registry_Table.System.FlipOnHSync                     = 0;     
//    HwDeviceExtension->NV_Registry_Table.System.FilterOverride                  = 0;  
//    HwDeviceExtension->NV_Registry_Table.System.NoNT4AGP                        = 0;        
//    HwDeviceExtension->NV_Registry_Table.System.George                          = IGNORE_REGISTRY_SWITCH;          
//    HwDeviceExtension->NV_Registry_Table.System.Gracie                          = IGNORE_REGISTRY_SWITCH;          
//    HwDeviceExtension->NV_Registry_Table.System.TVOutOnPrimary                  = IGNORE_REGISTRY_SWITCH;  
//    HwDeviceExtension->NV_Registry_Table.System.Enable256Burst                  = IGNORE_REGISTRY_SWITCH;  
//    HwDeviceExtension->NV_Registry_Table.System.DisableMPCDetect                = IGNORE_REGISTRY_SWITCH;  
//    HwDeviceExtension->NV_Registry_Table.System.MemoryOverride                  = IGNORE_REGISTRY_SWITCH;  
//    HwDeviceExtension->NV_Registry_Table.System.RTLOverride                     = IGNORE_REGISTRY_SWITCH;  
//
//    //**************************************************************************
//    // Default NVTWEAK Control Panel registry switch values ?       
//    //**************************************************************************

    }
    
//******************************************************************************
//
// Function:   BuildAvailableModesTable
//
// Routine Description:
//
//    Unlock the CRTC registers
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID BuildAvailableModesTable(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {

    U016 i;
    U016 NumGTFModes;    
    U016 GTFWidth,GTFHeight,GTFDepth,GTFRefresh;
    U016 RegWidth,RegHeight,RegDepth,RegRefresh;
    U032 *GTFModePtr;
    U016 *RegDataPtr;
    U016 *ValidModePtr, *BaseOfValidModeTable;
    BOOL FoundRefreshFlag;
    ULONG status;
    PMODE_ENTRY PModePtr;            
    ULONG widthDivide = 1, heightDivide = 1; // For NV11 MultiHead support.
    

    //**************************************************************************
    // The NVx_MODES modetable entries for each chip type will always be stored 
    // in the \device0 key, since the INF doesn't appear to be able to store them
    // in device1,device2, etc...   As a result, we always store and get modetable
    // registry data from the \device0 key for ALL chip types, and then save them
    // off in global mode tables. Essentially, all modetable registry data will
    // only be read when the FIRST nv device is found.   
    //    
    // That is, VideoPortGetRegistryParameters() will SUCCEED when reading the
    // mode table value entries from \device0, but will FAIL when it tries 
    // to read them from \device1\2\3...because the modetable entries won't be present!
    //
    // We use global modedata tables, so that we can copy modetable data
    // into the hwdeviceextension of each device found.
    //
    // The following is a simplified explanation:
    //
    // 1) hkey_local_machine\system\currentcontrolset\services\nv4\device0\
    //      
    //       nv4_modes      -> This value entry contains mode data for nv4
    //       nv5_modes      -> This value entry contains mode data for nv5
    //       nv5m64_modes   -> This value entry contains mode data for nv5m64
    //       nvvanta_modes  -> This value entry contains mode data for nvvanta
    //       etc...
    //
    // 2) When ..\NVx\device0 is found, all the mode data will be read from the
    //    registry into the global mode data tables:
    //
    //       NV4_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
    //       NV5_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
    //       NV0A_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
    //       NVVANTA_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
    //       etc....
    //
    // 3) For each device, the mode data for the corresponding chip
    //    type is copied into the ValidModeData in the device's
    //    hwDeviceExtension->ValidModeTable[]
    //  
    //**************************************************************************
                    
    //**************************************************************************
    // Init the global modedata tables with an 0xFFFF terminator.
    // That way, we can tell if the mode registry data was sucessfully copied.
    // (This could happen if the driver was not properly installed, using an
    // INF without mode table data)
    //
    // We ONLY initialize these mode tables the first time the driver is initialized.
    // We can tell by looking at the first entry, which should be 8bpp, width=640,
    // height=480).
    //**************************************************************************

    if ( !( (NV4_registry_data[0] == 8) && (NV4_registry_data[1] == 640) && (NV4_registry_data[2] == 480) && (NV4_registry_data[3] == 60)) )
        {    
        NV4_registry_data[0]        = 0xFFFF ;
        NV5_registry_data[0]        = 0xFFFF ;
        NV0A_registry_data[0]       = 0xFFFF ;
        NVVANTA_registry_data[0]    = 0xFFFF ; 
        NV5M64_registry_data[0]     = 0xFFFF ;
        NV5ULTRA_registry_data[0]   = 0xFFFF ; 
        NV10_registry_data[0]       = 0xFFFF ;
        NV10DDR_registry_data[0]    = 0xFFFF ;
        NV10GL_registry_data[0]     = 0xFFFF ;

        NV11_registry_data[0]       = 0xFFFF ;
        NV11DDR_registry_data[0]    = 0xFFFF ;
        NV11M_registry_data[0]      = 0xFFFF ;
        NV11GL_registry_data[0]     = 0xFFFF ;
        CRUSH11_registry_data[0]    = 0xFFFF ;

        NV15_registry_data[0]       = 0xFFFF ;
        NV15DDR_registry_data[0]    = 0xFFFF ;
        NV15BR_registry_data[0]     = 0xFFFF ;
        NV15GL_registry_data[0]     = 0xFFFF ;

        NV20_registry_data[0]       = 0xFFFF ;
        NV20_1_registry_data[0]     = 0xFFFF ;
        NV20_2_registry_data[0]     = 0xFFFF ;
        NV20_3_registry_data[0]     = 0xFFFF ;

            
        //*********************************************************************
        // Now copy the registry data per each specific chip type
        // and copy it into our mode tables
        //*********************************************************************

        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV4_MODES",         &(NV4_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV5_MODES",         &(NV5_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV0A_MODES",        &(NV0A_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NVVANTA_MODES",     &(NVVANTA_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV5M64_MODES",      &(NV5M64_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV5ULTRA_MODES",    &(NV5ULTRA_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV10_MODES",        &(NV10_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV10DDR_MODES",     &(NV10DDR_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV10GL_MODES",      &(NV10GL_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV11_MODES",        &(NV11_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV11DDR_MODES",     &(NV11DDR_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV11M_MODES",       &(NV11M_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV11GL_MODES",      &(NV11GL_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"CRUSH11_MODES",     &(CRUSH11_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV15_MODES",        &(NV15_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV15DDR_MODES",     &(NV15DDR_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV15BR_MODES",      &(NV15BR_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV15GL_MODES",      &(NV15GL_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV20_MODES",        &(NV20_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV20_1_MODES",      &(NV20_1_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV20_2_MODES",      &(NV20_2_registry_data[0] ));
        NV_ReadAndCopyRegistryData(HwDeviceExtension, L"NV20_3_MODES",      &(NV20_3_registry_data[0] ));



        }
    
    //**************************************************************************
    // 
    // GTFTimingTable Format in vesadata.c (ulong values):
    //
    //  WIDTH, HEIGHT, REFRESH, ....timing values
    //  WIDTH, HEIGHT, REFRESH, ....timing values
    //  WIDTH, HEIGHT, REFRESH, ....timing values
    //  WIDTH, HEIGHT, REFRESH, ....timing values
    //
    // Registry Data Format (word values)
    //
    //  BITDEPTH, WIDTH, HEIGHT, Refresh1, Refresh2, etc...., 0 ,
    //  BITDEPTH, WIDTH, HEIGHT, Refresh1, Refresh2, etc...., 0 ,
    //  BITDEPTH, WIDTH, HEIGHT, Refresh1, Refresh2, etc...., 0 ,
    //  BITDEPTH, WIDTH, HEIGHT, Refresh1, Refresh2, etc...., 0 ,
    //  etc...    
    //  0,
    //
    // ValidModeTable[]  (2 bytes each data item)
    //
    //  TRUE or FALSE, WIDTH, HEIGHT, DEPTH, REFRESH, 
    //  TRUE or FALSE, WIDTH, HEIGHT, DEPTH, REFRESH, 
    //  TRUE or FALSE, WIDTH, HEIGHT, DEPTH, REFRESH, 
    //  ..
    //  ..
    //   
    //**************************************************************************

    //**************************************************************************
    // This variable is used to keep track of how many modes
    // are initially specified in the REGISTRY.  These modes will 
    // be trimmed down further according to the amount of video memory on the card.
    // The final number of modes available will be stored in 
    //
    //  hwDeviceExtension->NumAvailableModes;
    //
    //**************************************************************************

    HwDeviceExtension->NumRegistryModes=0;

    //**************************************************************************
    // We allow modes according to the following:
    //
    //    1) What modes are actually available in vesadata.c (in the GTFTimingTable)
    //       We use the GTF table because it's a superset of the DMT table,
    //       in terms of the modes which are supported
    //    2) What modes are specified in the registry
    // 
    //**************************************************************************

    NumGTFModes =  (USHORT)vesaGetNumGTFEntries();

    //**************************************************************************
    // Get ptr to valid mode table (which we build here)
    //**************************************************************************

    ValidModePtr            = &(HwDeviceExtension->ValidModeTable[0]);
    BaseOfValidModeTable    = &(HwDeviceExtension->ValidModeTable[0]);

    //**************************************************************************
    // Build the ValidModeTable.  
    //
    // We allow modes which are:
    //
    //      1) Available in the GTFTimingTable    AND 
    //      2) Specified in the registry
    //
    // The GTF table does NOT specify depths.  The registry data DOES specify bit depths.
    // 
    // NOTE: If for some reason, the mode table was NOT initialized
    //       (ie...the mode table was not found in the registry), we'll
    //       default to ALL modes specified in the GTFTimingTable
    //
    //**************************************************************************

    //**************************************************************************
    // Determine which mode table to use
    //**************************************************************************

    switch (HwDeviceExtension->ulChipID)

        {
        case NV4_DEVICE_NV4_ID:

            RegDataPtr = &NV4_registry_data[0];                                    
            break;

        case NV5_DEVICE_NV5_ID:

            RegDataPtr = &NV5_registry_data[0];                                    
            break;

        case NV0A_DEVICE_NV0A_ID:

            RegDataPtr = &NV0A_registry_data[0];                                    
            break;

        case NV5VANTA_DEVICE_NV5VANTA_ID:

            RegDataPtr = &NVVANTA_registry_data[0];                                    
            break;

        case NV5MODEL64_DEVICE_NV5MODEL64_ID:

            RegDataPtr = &NV5M64_registry_data[0];                                    
            break;

        case NV5ULTRA_DEVICE_NV5ULTRA_ID:

            RegDataPtr = &NV5ULTRA_registry_data[0];                                    
            break;

        case NV10_DEVICE_NV10_ID:

            RegDataPtr = &NV10_registry_data[0];                                    
            break;

        case NV10DDR_DEVICE_NV10DDR_ID:

            RegDataPtr = &NV10DDR_registry_data[0];                                    
            break;

        case NV10GL_DEVICE_NV10GL_ID:

            RegDataPtr = &NV10GL_registry_data[0];                                    
            break;

        case NV11_DEVICE_NV11_ID:

            RegDataPtr = &NV11_registry_data[0];                                    
            break;

        case NV11DDR_DEVICE_NV11DDR_ID:

            RegDataPtr = &NV11DDR_registry_data[0];                                    
            break;

        case NV11M_DEVICE_NV11M_ID:

            RegDataPtr = &NV11M_registry_data[0];                                    
            break;

        case NV11GL_DEVICE_NV11GL_ID:

            RegDataPtr = &NV11GL_registry_data[0];                                    
            break;

        case NV11_DEVICE_CRUSH11_ID:

            RegDataPtr = &CRUSH11_registry_data[0];                                    
            break;


        case NV15_DEVICE_NV15_ID:

            RegDataPtr = &NV15_registry_data[0];                                    
            break;

        case NV15DDR_DEVICE_NV15DDR_ID:

            RegDataPtr = &NV15DDR_registry_data[0];                                    
            break;

        case NV15BR_DEVICE_NV15BR_ID:

            RegDataPtr = &NV15BR_registry_data[0];                                    
            break;


        case NV15GL_DEVICE_NV15GL_ID:

            RegDataPtr = &NV15GL_registry_data[0];                                    
            break;

        case NV20_DEVICE_NV20_ID:

            RegDataPtr = &NV20_registry_data[0];                                    
            break;

        case NV20_DEVICE_NV20_1_ID:

            RegDataPtr = &NV20_1_registry_data[0];                                    
            break;

        case NV20_DEVICE_NV20_2_ID:

            RegDataPtr = &NV20_2_registry_data[0];                                    
            break;

        case NV20_DEVICE_NV20_3_ID:

            RegDataPtr = &NV20_3_registry_data[0];                                    
            break;

        default:

            RegDataPtr = &NV4_registry_data[0];                                    
            break;

        }


    //**************************************************************************
    // Parse each line from the registry.
    // For each refresh rate (of each mode), check if the refresh is present in the
    // vesa data table.  If so, then it's a valid mode. 
    // Registry data is terminated with 0xffff
    //**************************************************************************

    while (*RegDataPtr != 0xffff)
        {
        RegDepth   = *(RegDataPtr);             // Get Depth from registry
        RegWidth   = *(RegDataPtr+1);           // Get Width from registry
        RegHeight  = *(RegDataPtr+2);           // Get Height from registry

        //**********************************************************************
        // Advance to the registry refresh rates for this particular mode
        //**********************************************************************

        RegDataPtr+=3;

        //**********************************************************************
        // Each mode entry in the registry data is NULL terminated.
        // Parse all refresh rates for this mode, and see if they're
        // also present in the vesadata GTF timing table
        //**********************************************************************

        while (*RegDataPtr !=0)
            {        
            RegRefresh  = *(RegDataPtr++);      // Get Refresh from registry data

            //******************************************************************
            // Search the vesa GTF table to see if this mode is supported
            //******************************************************************

            i=0;
            GTFModePtr = &(GTFTimingTable[i][0]);
            FoundRefreshFlag = FALSE;              

            while ( (i<NumGTFModes) && (FoundRefreshFlag == FALSE) )
                {                                                       
                //**************************************************************
                // Get width,height,refresh from GTF table
                //**************************************************************

                GTFWidth = (U016)(*(GTFModePtr));           // Get Width from GTF
                GTFHeight = (U016)(*(GTFModePtr+1));        // Get Height from GTF
                GTFRefresh = (U016)(*(GTFModePtr+2));       // Get Refresh*100
                GTFRefresh /= 100;                          // Normalize the value
                                   
                //**************************************************************
                // See if we found a match in the GTF table
                //**************************************************************

                if (MULTIMON_MODE(RegWidth, RegHeight)) 
                    {
                    if (HORIZONTAL_MODE(RegWidth, RegHeight))
                        {
                            widthDivide = 2;
                            heightDivide = 1;
                        }
                    else
                        {
                        if (VERTICAL_MODE(RegWidth, RegHeight))
                            {
                            widthDivide = 1;
                            heightDivide = 2;
                            }
                        }
                    }
                        
                    {
                    //**********************************************************
                    // Generate 8bpp entry for this refresh rate
                    //**********************************************************

                    *ValidModePtr      = TRUE;              // Valid Flag
                    *(ValidModePtr+1)  = RegWidth;          // Width
                    *(ValidModePtr+2)  = RegHeight;         // Height
                    *(ValidModePtr+3)  = RegDepth;          // Depth                                                                 
                    *(ValidModePtr+4)  = RegRefresh;        // Refresh                                                               
                    ValidModePtr +=5;                       // Next Valid Mode

                    //**********************************************************
                    // Keep track of how many modes have currently been specified
                    //**********************************************************

                    HwDeviceExtension->NumRegistryModes++;

                    //**********************************************************
                    // DEBUG safety check
                    // Make sure we never go past the end of the
                    // allocated valid mode table. (back off 30 words from end of table)
                    //**********************************************************

                    if (ValidModePtr > (U016 *)((PUCHAR)BaseOfValidModeTable + (MAX_VALID_MODE_TABLE_DATA_WORDS * 2) - (30*2) ))
                        {
                        VideoDebugPrint((0, "!!! ERROR:                                 !!!\n"));
                        VideoDebugPrint((0, "!!! ERROR: ERROR ERROR ERROR ERROR ERROR   !!!\n"));
                        VideoDebugPrint((0, "!!! ERROR: Went past end of ValidModeTable !!!\n"));
                        VideoDebugPrint((0, "!!! ERROR: Went past end of ValidModeTable !!!\n"));
                        VideoDebugPrint((0, "!!! ERROR: Went past end of ValidModeTable !!!\n"));
                        VideoDebugPrint((0, "!!! ERROR: ERROR ERROR ERROR ERROR ERROR   !!!\n"));
                        VideoDebugPrint((0, "!!! ERROR:                                 !!!\n"));
                        }
                
                    FoundRefreshFlag = TRUE;
                    }
                
                //**************************************************************
                // Advance to next GTF entry
                // (12 items per each mode entry in the GTF timing table)
                //**************************************************************

                GTFModePtr += 12;
                i++;


                } // while (i < NumGTFModes)...
                
            } // while (*RegDataPtr !=0)...


        //**********************************************************************
        // Skip past null terminator
        //**********************************************************************

        RegDataPtr++;
                
        } // while (*RegDataPtr != 0xffff)...

    
    // Print the master mode list for debugging purposes.
    {
        MODE_ENTRY *PModeEntry;
        VideoDebugPrint((7,"buildAvailableModes(): Printing Master Mode List: NumRegstryModes: %d\n", HwDeviceExtension->NumRegistryModes));
        RegDataPtr = &HwDeviceExtension->ValidModeTable[0];
    
         
        for (i=0; i < HwDeviceExtension->NumRegistryModes; i++)
        {
            VideoDebugPrint((7,"%d, (%d, %d), %d bpp, %d HZ\n",
                            *(RegDataPtr + 0),
                            *(RegDataPtr + 1),
                            *(RegDataPtr + 2),
                            *(RegDataPtr + 3),
                            *(RegDataPtr + 4)));
           RegDataPtr += 5;
        }
        VideoDebugPrint((7,"End.................\n"));
    }

        // Read in the Monitor restriction mode lists for each head.
        ReadMonitorRestrictionModeList (HwDeviceExtension);
                
    }




//******************************************************************************
//
// Function:   NV_ReadAndCopyRegistryData
//
// Routine Description:
//
//    Unlock the CRTC registers
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VOID NV_ReadAndCopyRegistryData(
PHW_DEVICE_EXTENSION HwDeviceExtension,
WCHAR *ModeDataNameStr,
U016 *DstRegDataPtr)

    {
    U016 *SrcRegDataPtr;
    ULONG status;
        
    //**************************************************************************
    // Parse INF and determine which modes to support
    //
    // WARNING:  When reading modetable registry data, the registry callback
    //           routine gets called SEVERAL times for each modetable. That is,
    //           it does NOT get called once for each table as one would expect.
    //           Instead, the registry callback routine gets called for EACH line
    //           appended to the string.  But the CONTEXT ptr does NOT automatically
    //           get incremented.  So we'll have to keep track of it MANUALLY,
    //           so we can continue storing data from where we last left off.
    //           See the registry callback function for more information.
    //**************************************************************************

    HwDeviceExtension->RegistryDataOffset = 0;

    //**************************************************************************
    // The NVx_MODES modetable entries for each chip type will always be stored 
    // in the \device0 key, since the INF doesn't appear to be able to store them
    // in device1,device2, etc...   As a result, we always store and get modetable
    // registry data from the \device0 key for ALL chip types, and then save them
    // off in global mode tables. Essentially, all modetable registry data will
    // only be read when the FIRST nv device is found.   
    //    
    // That is, VideoPortGetRegistryParameters() will SUCCEED when reading the
    // mode table value entries from \device0, but will FAIL when it tries 
    // to read them from \device1\2\3...because the modetable entries won't be present!
    //
    // We use global modedata tables, so that we can copy modetable data
    // into the hwdeviceextension of each device found.
    //
    // The following is a simplified explanation:
    //
    // 1) hkey_local_machine\system\currentcontrolset\services\nv4\device0\
    //      
    //       nv4_modes      -> This value entry contains mode data for nv4
    //       nv5_modes      -> This value entry contains mode data for nv5
    //       nv5m64_modes   -> This value entry contains mode data for nv5m64
    //       nvvanta_modes  -> This value entry contains mode data for nvvanta
    //       etc...
    //
    // 2) When nvx\device0 is found, all the mode data will be read from the
    //    registry into the global mode data tables:
    //
    //       NV4_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
    //       NV5_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
    //       NV0A_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
    //       NVVANTA_registry_data[MAX_STRING_REGISTRY_DATA_WORDS];
    //       etc....
    //
    // 3) For each device, the mode data for the corresponding chip
    //    type is copied into the ValidModeData in the device's
    //    hwDeviceExtension->ValidModeTable[]
    //  
    //**************************************************************************

    status = VideoPortGetRegistryParameters(HwDeviceExtension,
                                       ModeDataNameStr,
                                       FALSE,
                                       NVRegistryCallback,
                                       &registry_data[0]);

    if (status == NO_ERROR)
        {
        //**********************************************************************
        // Terminate the temporary registry data buffer with 0xffff !
        //**********************************************************************

        registry_data[HwDeviceExtension->RegistryDataOffset / 2] = 0xffff;

        //**********************************************************************
        // Now copy the registry data per specific chip type
        //**********************************************************************
        
        SrcRegDataPtr= &(registry_data[0]);
        
        while (*SrcRegDataPtr != 0xffff)
            {
            *DstRegDataPtr = *SrcRegDataPtr;// Copy the data
            
            DstRegDataPtr++;
            SrcRegDataPtr++;            
            }
    
        *DstRegDataPtr = 0xffff;           // Null terminate the list

        }
    else
        {
        //**********************************************************************
        // Error Case (when VideoPortGetRegistryParameters fails because
        // the ValueEntry for the mode data is NOT present)
        //
        // The NVx_MODES keys are only located in the \device0 key.
        // That is, ALL the registry mode data tables are copied when the FIRST
        // device is installed.  When additional instances of the adapter
        // are found, we don't no need to do anything since we've already
        // copied the data.
        //**********************************************************************
        
        }


    }
                                




//******************************************************************************
//
// Function:   BubbleSort
//
// Routine Description:
//
//    Unlock the CRTC registers
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VOID BubbleSort(
PHW_DEVICE_EXTENSION HwDeviceExtension,
PMODE_ENTRY BaseModePtr,
ULONG NumModes)

    {

    ULONG i,j;
    PMODE_ENTRY LeftModePtr, RightModePtr;
    MODE_ENTRY TempMode;


    //**************************************************************************
    // Sort by height
    //**************************************************************************

    for (i=0; i < NumModes; i++)
        {
        for (j=NumModes-1; j > i; j--)
            {
            LeftModePtr  = &BaseModePtr[j-1];
            RightModePtr = &BaseModePtr[j];                

            if (LeftModePtr->Height > RightModePtr->Height)
                {
                //**************************************************************
                // Swap mode entries, smaller modes first
                //**************************************************************
                
                TempMode = (*LeftModePtr);
                (*LeftModePtr)  = (*RightModePtr);
                (*RightModePtr) = TempMode;
                
                }

            }
            
        }


    //**************************************************************************
    // Sort by depth
    //**************************************************************************

    for (i=0; i < NumModes; i++)
        {
        for (j=NumModes-1; j > i; j--)
            {
            LeftModePtr  = &BaseModePtr[j-1];
            RightModePtr = &BaseModePtr[j];                

            if (LeftModePtr->Depth > RightModePtr->Depth)
                {
                //**************************************************************
                // Swap mode entries, smaller depths first
                //**************************************************************
                
                TempMode = (*LeftModePtr);
                (*LeftModePtr)  = (*RightModePtr);
                (*RightModePtr) = TempMode;
                
                }

            }
            
        }
        
        
        
    }
    

//******************************************************************************
//
// Function:   NV_OEMEnableExtensions
//
// Routine Description:
//
//    Unlock the CRTC registers
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV_OEMEnableExtensions(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    U008 lock;

    //**************************************************************************
    // For NV4, registers are locked using the NV_CIO_SR_LOCK register
    // (That is,the locking mechanism has been moved to CRTC register space,
    // where it should have been in the first place).  We need to
    // use the NV4_REF header file, so this function is locatedin NV4.C
    //**************************************************************************

    CRTC_RD(NV_CIO_SR_LOCK_INDEX,lock);
    CRTC_WR(NV_CIO_SR_LOCK_INDEX,NV_CIO_SR_UNLOCK_RW_VALUE);


    }


//******************************************************************************
//
// Function:   NV4 OEMDisableExtensions
//
// Routine Description:
//
//    Lock the CRTC registers
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV_OEMDisableExtensions(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    U008 lock;

    //**************************************************************************
    // For NV4, registers are locked using the NV_CIO_SR_LOCK register
    // (That is,the locking mechanism has been moved to CRTC register space,
    // where it should have been in the first place).  We need to
    // use the NV4_REF header file, so this function is locatedin NV4.C
    //**************************************************************************

    CRTC_RD(NV_CIO_SR_LOCK_INDEX,lock);
    CRTC_WR(NV_CIO_SR_LOCK_INDEX,NV_CIO_SR_LOCK_VALUE);

    }


//******************************************************************************
//
// Function:   NV4_OEMSetRegs
//
// Routine Description:
//
//       OEMSetRegs - Set extended registers (standard VGA has been set)
//
//       Entry:  ES:DI = Standard VGA parameter pointer
//               DS = Seg0
//       Exit:   None
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VOID NV_OEMSetRegs(MODEDESC *mi)
{
    U016  wv;                     // Write value{
    U016  i, c;
    U032   lwv;
    U032   lrv;
    U008   cv;

    //*************************************************************************
    // NV4 version of this function is slightly different than
    // the NV3 version. (565 mode and TV stuff)
    //*************************************************************************

    if (mi->mdCMode != 0xFF)        // Do we have a CRTC Override Table?
    {
        // Yep - program it
        WritePriv08(NV_PRMVIO_MISC__WRITE, crt_override[mi->mdCMode].CO_Misc);

        wv = crt_override[mi->mdCMode].CO_ClockMode;
        wv <<= 8;
        wv |= 0x01;
        WriteIndexed(NV_PRMVIO_SRX, wv); // Write SR01

        wv = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x11);    // Read CR11
        wv &= 0x7FFF;               // Unlock CR0-7
        WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR11

        // Do CR0-CR8 sequentially
        LoadIndexRange(NV_PRMCIO_CRX__COLOR, 0, 8, (U008 *)&(crt_override[mi->mdCMode].HTotal));

        wv = crt_override[mi->mdCMode].CO_CellHeight;
        wv <<= 8;
        wv |= 0x09;
        WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR09

        // Do CR10-CR12 sequentially
        LoadIndexRange(NV_PRMCIO_CRX__COLOR, 0x10, 0x03, (U008 *)&(crt_override[mi->mdCMode].CO_VSyncStart));

        // Do CR15-CR16 sequentially
        LoadIndexRange(NV_PRMCIO_CRX__COLOR, 0x15, 0x02, (U008 *)&(crt_override[mi->mdCMode].CO_VBlankStart));
    }

    // Program Extended Registers

    wv = eregs[mi->mdEMode].xrOffset;
    wv <<= 13;
    wv &= 0xE000;                   // Clear start address bits
    wv |= 0x19;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR19

    wv = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x1A);
    wv &= 0xC0FF;                   // Preserve sync disables
    wv |= 0x3800;                   // Set reserved bits and text mode

    c = (eregs[mi->mdEMode].xrFlags);

    if (c & 0x01)
        wv |= 0x0100;               // Set address wrap

    i = (eregs[mi->mdEMode].xrVOver);
    i &= 0x20;
    i >>= 0x03;
    wv |= (i << 8);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR1A

    // Default Fifo
    wv = 0x011B;                    // Default CR1B value for VGA modes
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR1A

    wv = c;
    wv &= 0x06;
    wv <<= 8;
    wv |= 0x1C;                     // CR1C

    i = (eregs[mi->mdEMode].xrPixFmt);
    i &= 0x30;                      // Mask off chain-4 optimize bits
    i >>= 1;            // (shr 4-3?)

    wv |= (i << 8);                 // Set chain 4 optimize bits
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR1A


    wv = 0x001D;                    // Clear CR1D & 1E
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR1D

    wv = 0x001E;                    // Clear CR1D & 1E
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Write CR1E

    wv = 0x1020;                    // Fifo Setting for Standar VGA modes
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Set low water mark at CR20

    c = (eregs[mi->mdEMode].xrVOver); // Vertical Overflow
    c &= 0x1F;                      // Mask our bits

    i = (eregs[mi->mdEMode].xrOffset);
    i &= 0x08;                      // Bit 3
    i <<= 2;                        // Move to bit 5
    c |= i;
    wv =  (c << 8) | 0x25;          // Write data to CR25
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Set CR25


    wv = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x28);
    wv &= 0xF8FF;                   // Preserve sync disables

    c = eregs[mi->mdEMode].xrPixFmt;
    c &= 0x07;                      // Set bits [2:0] - clear rest
    wv |= (c << 8);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Set CR28

    c = eregs[mi->mdEMode].xrPixFmt;
    c &= 0xC0;
    c >>= 6;

    i = eregs[mi->mdEMode].xrVOver;
    i &= 0xC0;
    i >>= 4;
    c |= i;
    wv = ((c << 8) | 0x2D);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);  // Set CR2D

    wv = 0x29;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);
    wv = 0x2A;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);


    //**************************************************************************
    // Don't touch CR33 if we're on a flat panel
    //**************************************************************************

    if (HwDeviceExtension->FlatPanelIsPresent == TRUE)
        {

        // Do CR29-32
        for (i = 0x30; i <= 0x32; i++)
            {
            wv = (U016)i;
            WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);
            }

        }

    else

        {


        // Do CR29-33
        for (i = 0x30; i <= 0x33; i++)
            {
            wv = (U016)i;
            WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);
            }

        }


    wv = (((eregs[mi->mdEMode].xrIntlace) << 8) | 0x39);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);

    //**************************************************************************
    // Don't set clock if we're on a flat panel
    //**************************************************************************

    //OEMSetClock(eregs[mi->mdEMode].xrClkSel);
    if (HwDeviceExtension->FlatPanelIsPresent != TRUE)
        OEMSetClock(tblClockFreq[eregs[mi->mdEMode].xrClkSel]);

    wv = (((eregs[mi->mdEMode].xrVOffset) << 8) | 0x13);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);

    //**************************************************************************
    // No TV support yet for NV class chips
    //**************************************************************************

//    if (isTV())

    if (HwDeviceExtension->TvIsPresent)

        {
            NV4_ProgramTV( mi->mdTVMode );
        }

#if 0
    //
    // Set VCLK divide by 2 setting
    //
    wv = (eregs[mi->mdEMode].xrPixFmt);
    lwv = (U032)(wv & 0x08);

    lwv <<= (DRF_SHIFT(NV_PRAMDAC_PLL_COEFF_SELECT_VCLK_RATIO) - 3);

    // Read current coefficient
    //
    // Note that this code preserves the VPLL_BYBASS bit which is nessesary for TV
    lrv = ReadPriv32(NV_PRAMDAC_PLL_COEFF_SELECT);

    // Mask off the bits we'll use.
    //
    // NOTE: Careful: the BIT definitions have CHANGED from NV3 to NV4 !!!!
    //
    lrv &= (U032)(((U032)(1L << DRF_SHIFT(NV_PRAMDAC_PLL_COEFF_SELECT_VCLK_RATIO)) |
           ((U032)(1L << DRF_SHIFT(NV_PRAMDAC_PLL_COEFF_SELECT_VPLL_SOURCE))) |
           ((U032)(7L << DRF_SHIFT(NV_PRAMDAC_PLL_COEFF_SELECT_SOURCE)))) ^ -1L);

    lrv |= lwv;         // Set the VCLK Ratio

    // Now we have to set to programmed VPLL based on XTAL
    //
    // NOTE: Careful: the BIT definitions have CHANGED from NV3 to NV4 !!!!
    //
    lrv |= (U032)(2L << DRF_SHIFT(NV_PRAMDAC_PLL_COEFF_SELECT_SOURCE));

    WritePriv32(lrv, NV_PRAMDAC_PLL_COEFF_SELECT);
#endif


    //**************************************************************************
    // If flat panel, we may need to adjust CRTC's
    //**************************************************************************

    if (HwDeviceExtension->FlatPanelIsPresent == TRUE)

        FlatPanelCrtc(mi->mdXRes);

    else

        WritePriv32(0x10000700, NV_PRAMDAC_PLL_COEFF_SELECT);

    //**************************************************************************
    // If flat panel, leave DAC alone
    //**************************************************************************

    if (HwDeviceExtension->FlatPanelIsPresent != TRUE)

        {

        lrv = ReadPriv32(NV_PRAMDAC_GENERAL_CONTROL);

        lrv &= (U032)(((1L << DRF_SHIFT(NV_PRAMDAC_GENERAL_CONTROL_ALT_MODE)) |
                 (1L << DRF_SHIFT(NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE)) |
                 (1L << DRF_SHIFT(NV_PRAMDAC_GENERAL_CONTROL_BPC))) ^ -1L);

        lrv |= (U032)(1L << DRF_SHIFT(NV_PRAMDAC_GENERAL_CONTROL_VGA_STATE));

        wv = (eregs[mi->mdEMode].xrOffset);
        lwv = (U032)(wv & 0x80);       // Set 565 Mode

        lwv <<= (DRF_SHIFT(NV_PRAMDAC_GENERAL_CONTROL_ALT_MODE) - 7L);

        lrv |= lwv;

        cv = (eregs[mi->mdEMode].xrFlags);
        cv ^= (U008)-1;
        cv &= 0x08;                 // Isolate DAC width

        lwv = (U032)(cv);          // Dac Width
        lwv <<= (U032)(DRF_SHIFT(NV_PRAMDAC_GENERAL_CONTROL_BPC) - 3L);

        lrv |= lwv;

        cv = (eregs[mi->mdEMode].xrPixFmt);
        cv ^= (U008)-1;                   // Invert
        cv &= 0x02;                 // Isolate index/gamma bit

        lwv = (U032)(cv);

        //KJK lwv <<= (U032)(DRF_SHIFT(NV_PRAMDAC_GENERAL_CONTROL_IDC_MODE) - 1L);
        lrv |= lwv;

        WritePriv32(lrv, NV_PRAMDAC_GENERAL_CONTROL);

        // Disable tiling
        lrv = ReadPriv32(NV_PFB_CONFIG_0);
        lrv &= (U032)((1L << DRF_SHIFT(NV_PFB_CONFIG_0_TILING)) ^ -1L);
        lrv |= (NV_PFB_CONFIG_0_TILING_DISABLED << DRF_SHIFT(NV_PFB_CONFIG_0_TILING));

        //**********************************************************************
        // Do NOT touch this register for NV10 !!
        //**********************************************************************

        if ( !((HwDeviceExtension->ulChipID == NV10_DEVICE_NV10_ID)       ||
               (HwDeviceExtension->ulChipID == NV10DDR_DEVICE_NV10DDR_ID) ||
               (HwDeviceExtension->ulChipID == NV10GL_DEVICE_NV10GL_ID)   ||
               (HwDeviceExtension->ulChipID == NV11_DEVICE_NV11_ID)       ||
               (HwDeviceExtension->ulChipID == NV11DDR_DEVICE_NV11DDR_ID) ||
               (HwDeviceExtension->ulChipID == NV11M_DEVICE_NV11M_ID)     ||
               (HwDeviceExtension->ulChipID == NV11GL_DEVICE_NV11GL_ID)   ||
               (HwDeviceExtension->ulChipID == NV11_DEVICE_CRUSH11_ID)    ||
               (HwDeviceExtension->ulChipID == NV15_DEVICE_NV15_ID)       ||
               (HwDeviceExtension->ulChipID == NV15DDR_DEVICE_NV15DDR_ID) ||
               (HwDeviceExtension->ulChipID == NV15BR_DEVICE_NV15BR_ID)   ||
               (HwDeviceExtension->ulChipID == NV15GL_DEVICE_NV15GL_ID)   ||
               (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_ID)       ||
               (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_1_ID)     ||
               (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_2_ID)     ||
               (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_3_ID)))

            {
               WritePriv32(lrv, NV_PFB_CONFIG_0);
            }
            

        }


    WritePriv08(NV_PRMCIO_CRX__COLOR, 0x18);    // Select CR18 (why?)


}



//******************************************************************************
//
// Function:   NV4_ProgramTV
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VOID NV4_ProgramTV(U008 tvmode)

    {
    // TODO:
    }


//******************************************************************************
//
// Function:   NVGetMonitorType
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NVGetMonitorType(PHW_DEVICE_EXTENSION HwDeviceExtension)

    {
    ULONG result;
    PHWINFO NVInfo;
    NV_CFGEX_GET_EDID_PARAMS EdidParams;
    ULONG EdidBufferSize;
    
        
    //**************************************************************************
    // Get ptr to NVInfo structure 
    //**************************************************************************

    NVInfo = &(HwDeviceExtension->NvInfo);
    
    //**************************************************************************
    // Ask the RM what's attached to the card (Flat Panel/Monitor/ or TV)
    //**************************************************************************

    HwDeviceExtension->TvIsPresent              = FALSE;
    HwDeviceExtension->FlatPanelIsPresent       = FALSE; 

    //**************************************************************************
    // By default, a standard VGA monitor is attached.
    // Check if either a Flat Panel or TV is attached
    //**************************************************************************

    RmConfigGetKernel(HwDeviceExtension->DeviceReference, NV_CFG_VIDEO_MONITOR_TYPE ,&result) ;

    if (result == MONITOR_TYPE_FLAT_PANEL)
        {
        HwDeviceExtension->FlatPanelIsPresent   = TRUE;

        //**********************************************************************
        // Get max resolution supported by this display type
        //**********************************************************************

        RmConfigGetKernel(HwDeviceExtension->DeviceReference, NV_CFG_SCREEN_MAX_WIDTH ,&HwDeviceExtension->MaxFlatPanelWidth) ;

        RmConfigGetKernel(HwDeviceExtension->DeviceReference, NV_CFG_SCREEN_MAX_HEIGHT ,&HwDeviceExtension->MaxFlatPanelHeight) ;

        }

    // GK: HACKHACK: Temporary. Once we verify the EDID info per head, we don't need any of this MaxFlatPanelWidth stuff.
    if (HwDeviceExtension->MaxFlatPanelWidth == 0)
    {
        HwDeviceExtension->MaxFlatPanelWidth = 1024;
    }
    if (HwDeviceExtension->MaxFlatPanelHeight == 0)
    {
        HwDeviceExtension->MaxFlatPanelHeight = 768;
    }


    if ((result == MONITOR_TYPE_NTSC) || (result == MONITOR_TYPE_PAL))
        HwDeviceExtension->TvIsPresent          = TRUE;

    //**************************************************************************
    // This TV code used to work on NV3.. Need to test on NV4 or better...
    //**************************************************************************

   if (HwDeviceExtension->TvIsPresent)                // NO TV support yet !
       {
       // don't diff between NTSC & PAL for now
       NVInfo->Framebuffer.MonitorType = NV_MONITOR_NTSC;

       // Preset the default values
       NVInfo->Framebuffer.Underscan_x = DST_X_RES;
       NVInfo->Framebuffer.Scale_x = X_RES << 20;
       NVInfo->Framebuffer.Scale_x /= NVInfo->Framebuffer.Underscan_x;

       NVInfo->Framebuffer.Underscan_y = DST_Y_RES;
       NVInfo->Framebuffer.Scale_y = Y_RES << 20;
       NVInfo->Framebuffer.Scale_y /= NVInfo->Framebuffer.Underscan_y;

       // No filtering needed for NV5 or better
       NVInfo->Framebuffer.FilterEnable = 0;
       }
    else
       {
       NVInfo->Framebuffer.MonitorType = NV_MONITOR_VGA;

       // If filtering gets enabled for VGA, assume 1:1 scaling
       NVInfo->Framebuffer.Underscan_x = X_RES;
       NVInfo->Framebuffer.Scale_x = (1 << 20);        // default to 1:1

       NVInfo->Framebuffer.Underscan_y = Y_RES;
       NVInfo->Framebuffer.Scale_y = (1 << 20);

       // Assume filtering is disabled
       NVInfo->Framebuffer.FilterEnable = 0;
       }



    }

//******************************************************************************
//
// Function:   NV_FixLowWaterMark
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VOID NV_FixLowWaterMark(U016 clock)

    {
    U016  wv;
    U032   lwv;
    U016  i;
    U032  clockMHz;

    wv = ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x28);
    wv >>= 8;
    wv &= 0x03;       // mask pixel format bits

    // Check dac format
    if (wv)
        {
        wv--;
        clockMHz = clock;
        clockMHz <<= (U008)wv;
        clockMHz /= 100;

        for (i = 0; clockMHz > DACFifoTable[i].Freq ; i++)
            ;   // Find the proper FIFO value
        
        if  ( (HwDeviceExtension->ulChipID == NV10_DEVICE_NV10_ID)       ||
              (HwDeviceExtension->ulChipID == NV10DDR_DEVICE_NV10DDR_ID) ||
              (HwDeviceExtension->ulChipID == NV10GL_DEVICE_NV10GL_ID)   ||
              (HwDeviceExtension->ulChipID == NV11_DEVICE_NV11_ID)       ||
              (HwDeviceExtension->ulChipID == NV11DDR_DEVICE_NV11DDR_ID) ||
              (HwDeviceExtension->ulChipID == NV11M_DEVICE_NV11M_ID)     ||
              (HwDeviceExtension->ulChipID == NV11GL_DEVICE_NV11GL_ID)   ||
              (HwDeviceExtension->ulChipID == NV11_DEVICE_CRUSH11_ID)    ||
              (HwDeviceExtension->ulChipID == NV15_DEVICE_NV15_ID)       ||
              (HwDeviceExtension->ulChipID == NV15DDR_DEVICE_NV15DDR_ID) ||
              (HwDeviceExtension->ulChipID == NV15BR_DEVICE_NV15BR_ID)   ||
              (HwDeviceExtension->ulChipID == NV15GL_DEVICE_NV15GL_ID)   ||
              (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_ID)       ||
              (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_1_ID)     ||
              (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_2_ID)     ||
              (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_3_ID))
        
            {
            // for NV10, bit 4 is the 128-bit bit.
            lwv = ReadPriv32(NV_PFB_CFG) & 0x00000010L;
            }
        else
            {
            // Bit 2 is the 128-bit bit.
            lwv = ReadPriv32(NV_PFB_BOOT_0) & 0x00000004L;
            }

        if (lwv)
            {
            // 128 bit
            // Do WV first, as I get's blown away
            wv = ((DACFifoTable[i].LowWaterMark128) << 8) | 0x20;
            i  = ((DACFifoTable[i].BurstSize128) << 8) | 0x1B;

            }
        else
            {
            // 64 bit
            wv = ((DACFifoTable[i].LowWaterMark64) << 8) | 0x20;
            i  = ((DACFifoTable[i].BurstSize64) << 8) | 0x1B;
            }

        WriteIndexed(NV_PRMCIO_CRX__COLOR, i);
        WriteIndexed(NV_PRMCIO_CRX__COLOR, wv);
        }
    }





//******************************************************************************
//
// Function:   NV_OEMGetMemSize
//
// Routine Description:
//
//
//       OEMGetMemSize - Return the pre-calculated memory size
//
//       Entry:  None
//       Exit:   AX = Number of 64K blocks
//
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

U016 NV_OEMGetMemSize(VOID)
{
    U032 msize;
    U016 ms;


    if  ((HwDeviceExtension->ulChipID == NV10_DEVICE_NV10_ID)       ||
         (HwDeviceExtension->ulChipID == NV10DDR_DEVICE_NV10DDR_ID) ||
         (HwDeviceExtension->ulChipID == NV10GL_DEVICE_NV10GL_ID)   ||
         (HwDeviceExtension->ulChipID == NV11_DEVICE_NV11_ID)       ||
         (HwDeviceExtension->ulChipID == NV11DDR_DEVICE_NV11DDR_ID) ||
         (HwDeviceExtension->ulChipID == NV11M_DEVICE_NV11M_ID)     ||
         (HwDeviceExtension->ulChipID == NV11GL_DEVICE_NV11GL_ID)   ||
         (HwDeviceExtension->ulChipID == NV11_DEVICE_CRUSH11_ID)    ||
         (HwDeviceExtension->ulChipID == NV15_DEVICE_NV15_ID)       ||
         (HwDeviceExtension->ulChipID == NV15DDR_DEVICE_NV15DDR_ID) ||
         (HwDeviceExtension->ulChipID == NV15BR_DEVICE_NV15BR_ID)   ||
         (HwDeviceExtension->ulChipID == NV15GL_DEVICE_NV15GL_ID)   ||
         (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_ID)       ||
         (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_1_ID)     ||
         (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_2_ID)     ||
         (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_3_ID))
    {
        ms = (U016)(PFB_REG_RD32(PFB_Base,NV_PFB_CSTATUS) >> 16);
    }
    else
    {
        //*************************************************************************
        // NV4 version of this function is slightly different than
        // the NV3 version. (We want to use #DEFINES from NV4_REF for NV4,
        // and #DEFINES from NV3_REF for NV3)
        //*************************************************************************

        msize = ReadPriv32(NV_PFB_BOOT_0) & 0x00000003;
        ms = (U016)msize;

        if (ms == NV_PFB_BOOT_0_RAM_AMOUNT_4MB)
            ms = 64;            // 4MB
        else if (ms == NV_PFB_BOOT_0_RAM_AMOUNT_8MB)
            ms = 128;           // 8MB
        else if (ms == NV_PFB_BOOT_0_RAM_AMOUNT_16MB)
            ms = 256;           // 16MB
        else
            ms = 512;           // 32Mb

        //**************************************************************************
        // Check for UMA usage
        //
        // Note that this is only valid for NV0A, but the other NV4 variants hardwire these
        // bit fields to zero.
        //**************************************************************************

        if (PFB_REG_RD_DRF(PFB_Base,_PFB, _BOOT_0, _UMA) == NV_PFB_BOOT_0_UMA_ENABLE)
        {
            switch (PFB_REG_RD_DRF(PFB_Base,_PFB, _BOOT_0, _UMA_SIZE))
            {
                case NV_PFB_BOOT_0_UMA_SIZE_2M:
                    ms = 2 << 4;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_4M:
                    ms = 4 << 4;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_6M:
                    ms = 6 << 4;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_8M:
                    ms = 8 << 4;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_10M:
                    ms = 10 << 4;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_12M:
                    ms = 12 << 4;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_14M:
                    ms = 14 << 4;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_16M:
                    ms = 16 << 4;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_18M:
                    ms = 18 << 4;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_20M:
                    ms = 20 << 4;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_22M:
                    ms = 22 << 4;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_24M:
                    ms = 24 << 4;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_26M:
                    ms = 26 << 4;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_28M:
                    ms = 28 << 4;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_30M:
                    ms = 30 << 4;
                    break;
                case NV_PFB_BOOT_0_UMA_SIZE_32M:
                    ms = 32 << 4;
                    break;
            }
        }
    }

    return ms;

}




//******************************************************************************
//
// Function:   NV4_dacCalculateArbitration
//
// Routine Description:
//
//      Calculate the closest arbitration values for a given system configuration
//
//      NOTE: Please excuse this code.  In comes from the hardware group...
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV4_dacCalculateArbitration
(
           nv4_fifo_info *fifo,
           nv4_sim_state  *arb
)
{
  int data, m,n,p, pagemiss, cas,width, video_enable, color_key_enable, bpp, align;
  int nvclks, mclks, pclks, vpagemiss, crtpagemiss, vbs;
  int found, mclk_extra, mclk_loop, cbs, m1, p1;
  int xtal_freq, mclk_freq, pclk_freq, nvclk_freq, mp_enable;
  int us_m, us_n, us_p, video_drain_rate, crtc_drain_rate;
  int vpm_us, us_video, vlwm, video_fill_us, cpm_us, us_crt,clwm;
  int craw, vraw;
  fifo->valid = 1;
  pclk_freq = arb->pclk_khz; // freq in KHz
  mclk_freq = arb->mclk_khz;
  nvclk_freq = arb->nvclk_khz;
  pagemiss = arb->mem_page_miss;
  cas = arb->mem_latency;
  width = arb->memory_width >> 6;
  video_enable = arb->enable_video;
  color_key_enable = arb->gr_during_vid;
  bpp = arb->pix_bpp;
  align = arb->mem_aligned;
  mp_enable = arb->enable_mp;
  clwm = 0;
  vlwm = 0;
  cbs = 128;
  pclks = 2; // lwm detect.

  nvclks = 2; // lwm -> sync.
  nvclks += 2; // fbi bus cycles (1 req + 1 busy)
  nvclks += 1; // fbi reqsync

  mclks = 5; // Fifo
  mclks += 3; // MA0 -> MA2
  mclks += 1; // pad->out
  mclks += cas; // Cas Latency.
  mclks += 1; // pad in
  mclks += 1; // latch data
  mclks += 1; // fifo load
  mclks += 1; // fifo write
  mclk_extra = 3; // Margin of error

  nvclks += 2; // fifo sync
  nvclks += 1; // read data
  nvclks += 1; // fbi_rdata
  nvclks += 1; // crtfifo load

  if(mp_enable)
    mclks+=4; // Mp can get in with a burst of 8.
  // Extra clocks determined by heuristics

  nvclks += 0;
  pclks += 0;
  found = 0;
  while(found != 1) {
    fifo->valid = 1;
    found = 1;
    mclk_loop = mclks+mclk_extra;
    us_m = mclk_loop *1000*1000 / mclk_freq; // Mclk latency in us
    us_n = nvclks*1000*1000 / nvclk_freq;// nvclk latency in us
    us_p = nvclks*1000*1000 / pclk_freq;// nvclk latency in us
    if(video_enable) {
      video_drain_rate = pclk_freq * 2; // MB/s
      crtc_drain_rate = pclk_freq * bpp/8; // MB/s

      vpagemiss = 2; // self generating page miss
      vpagemiss += 1; // One higher priority before

      crtpagemiss = 2; // self generating page miss

      vpm_us = (vpagemiss * pagemiss)*1000*1000/mclk_freq;


      if(nvclk_freq * 2 > mclk_freq * width) // nvclk is faster than mclk
    video_fill_us = cbs*1000*1000 / 16 / nvclk_freq ;
      else
    video_fill_us = cbs*1000*1000 / (8 * width) / mclk_freq;

      us_video = vpm_us + us_m + us_n + us_p + video_fill_us;

      vlwm = us_video * video_drain_rate/(1000*1000);
      vlwm++; // fixed point <= float_point - 1.  Fixes that
      vbs = 128;

      if(vlwm > 128) vbs = 64;
      if(vlwm > (256-64)) vbs = 32;

      if(nvclk_freq * 2 > mclk_freq * width) // nvclk is faster than mclk
    video_fill_us = vbs *1000*1000/ 16 / nvclk_freq ;
      else
    video_fill_us = vbs*1000*1000 / (8 * width) / mclk_freq;

      cpm_us = crtpagemiss  * pagemiss *1000*1000/ mclk_freq;
      us_crt =
    us_video  // Wait for video
    +video_fill_us // Wait for video to fill up
    +cpm_us // CRT Page miss
    +us_m + us_n +us_p // other latency
    ;
      clwm = us_crt * crtc_drain_rate/(1000*1000);
      clwm++; // fixed point <= float_point - 1.  Fixes that
    } else {
      crtc_drain_rate = pclk_freq * bpp/8; // bpp * pclk/8

      crtpagemiss = 2; // self generating page miss
      crtpagemiss += 1; // MA0 page miss
      cpm_us = crtpagemiss  * pagemiss *1000*1000/ mclk_freq;
      us_crt =  cpm_us + us_m + us_n + us_p ;
      clwm = us_crt * crtc_drain_rate/(1000*1000);
      clwm++; // fixed point <= float_point - 1.  Fixes that
    }
    /*
      Overfill check:

      */

    m1 = clwm + cbs - 512; /* Amount of overfill */
    p1 = m1 * pclk_freq / mclk_freq; /* pclk cycles to drain */
    p1 = p1 * bpp / 8; // bytes drained.

    if((p1 < m1) && (m1 > 0)) {
    fifo->valid = 0;
    found = 0;
    if(mclk_extra ==0)   found = 1; // Can't adjust anymore!
    mclk_extra--;
    }
    else if(video_enable){
      if((clwm > 511) || (vlwm > 255)) {
    fifo->valid = 0;
    found = 0;
    if(mclk_extra ==0)   found = 1; // Can't adjust anymore!
    mclk_extra--;
      }
    } else {
      if(clwm > 519){ // Have some margin
    fifo->valid = 0;
    found = 0;
    if(mclk_extra ==0)   found = 1; // Can't adjust anymore!
    mclk_extra--;
      }
    }
    craw = clwm;
    vraw = vlwm;
    if(clwm < 384) clwm = 384;
    if(vlwm < 128) vlwm = 128;
    data = (int)(clwm);
    //  printf("CRT LWM: %f bytes, prog: 0x%x, bs: 256\n", clwm, data );
    fifo->graphics_lwm = data;   fifo->graphics_burst_size = 128;
    //    fifo->craw = craw;

    data = (int)((vlwm+15));
    //  printf("VID LWM: %f bytes, prog: 0x%x, bs: %d\n, ", vlwm, data, vbs );
    fifo->video_lwm = data;  fifo->video_burst_size = vbs;
  }
}



//******************************************************************************
//
// Function:   NV4_UpdateArbitrationSettings
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************


VOID NV4_UpdateArbitrationSettings
(
    PHW_DEVICE_EXTENSION HwDeviceExtension
)
{

    nv4_fifo_info fifo_data;
    nv4_sim_state sim_data;
    U008 i, lock;
    U032 M, N, O, P , crystal , NVClk, VClk;

    //
    // Determine current strap crystal frequency (in Hz)
    //
    if (PEXTDEV_REG_RD_DRF(PEXTDEV_Base,_PEXTDEV, _BOOT_0, _STRAP_CRYSTAL) == NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL_13500K)
        crystal = 13500000;
    else if (PEXTDEV_REG_RD_DRF(PEXTDEV_Base,_PEXTDEV, _BOOT_0, _STRAP_CRYSTAL) == NV_PEXTDEV_BOOT_0_STRAP_CRYSTAL_14318180)
        crystal = 14318180;



    M = PRAMDAC_REG_RD_DRF(PRAMDAC_Base,_PRAMDAC, _MPLL_COEFF, _MDIV);
    N = PRAMDAC_REG_RD_DRF(PRAMDAC_Base,_PRAMDAC, _MPLL_COEFF, _NDIV);
    P = PRAMDAC_REG_RD_DRF(PRAMDAC_Base,_PRAMDAC, _MPLL_COEFF, _PDIV);
    O = 1;

    HwDeviceExtension->NvInfo.Dac.MPllM = M;
    HwDeviceExtension->NvInfo.Dac.MPllN = N;
    HwDeviceExtension->NvInfo.Dac.MPllO = O;
    HwDeviceExtension->NvInfo.Dac.MPllP = P;
    if ( (!P) && (HwDeviceExtension->ulChipID == NV4_DEVICE_NV4_ID))
        P = 1;      // never really zero
    HwDeviceExtension->NvInfo.Dac.MClk  = (N * crystal / (1 << P) / M);



    M = PRAMDAC_REG_RD_DRF(PRAMDAC_Base, _PRAMDAC, _VPLL_COEFF, _MDIV);
    N = PRAMDAC_REG_RD_DRF(PRAMDAC_Base, _PRAMDAC, _VPLL_COEFF, _NDIV);
    P = PRAMDAC_REG_RD_DRF(PRAMDAC_Base, _PRAMDAC, _VPLL_COEFF, _PDIV);
    O = 1;

    HwDeviceExtension->NvInfo.Dac.VPllM = M;
    HwDeviceExtension->NvInfo.Dac.VPllN = N;
    HwDeviceExtension->NvInfo.Dac.VPllO = O;
    HwDeviceExtension->NvInfo.Dac.VPllP = P;
    VClk = HwDeviceExtension->NvInfo.Dac.VClk  = (N * crystal / (1 << P) / M);



    M = PRAMDAC_REG_RD_DRF(PRAMDAC_Base,_PRAMDAC, _NVPLL_COEFF, _MDIV);
    N = PRAMDAC_REG_RD_DRF(PRAMDAC_Base,_PRAMDAC, _NVPLL_COEFF, _NDIV);
    P = PRAMDAC_REG_RD_DRF(PRAMDAC_Base,_PRAMDAC, _NVPLL_COEFF, _PDIV);
    O = 1;

    NVClk  = (N * crystal / (1 << P) / M);

    //
    // Last minute kludge to work around bug in NV4 arbitration
    // algorithm.
    //

    if (((HwDeviceExtension->NvInfo.Framebuffer.Depth < 16) && (VClk > 180000000)) ||
        ((HwDeviceExtension->NvInfo.Framebuffer.Depth == 16) && (VClk > 130000000)) ||
        ((HwDeviceExtension->NvInfo.Framebuffer.Depth > 16) && (VClk > 80000000)))
       HwDeviceExtension->bHwVidOvl = FALSE;
    else
        {
        //
        // Build the sim table as if hw video overlay was enabled
        //
        sim_data.pix_bpp        = (UCHAR)HwDeviceExtension->NvInfo.Framebuffer.Depth;
        sim_data.enable_video   = 1;
        sim_data.enable_mp      = 0;
        sim_data.memory_width   = HwDeviceExtension->NvInfo.Dac.InputWidth;;
        sim_data.mem_latency    = 3;
        sim_data.mem_aligned    = 1;
        sim_data.mem_page_miss  = 10;
        sim_data.gr_during_vid  = 0;
        sim_data.pclk_khz       = HwDeviceExtension->NvInfo.Dac.VClk / 1000;    // in kHz, not MHz
        sim_data.mclk_khz       = HwDeviceExtension->NvInfo.Dac.MClk / 1000;    // in kHz, not MHz
        sim_data.nvclk_khz      = NVClk / 1000;                                 // in kHz, not MHz

        //
        // Get those new numbers
        //
        NV4_dacCalculateArbitration(&fifo_data, &sim_data);
        HwDeviceExtension->bHwVidOvl = (fifo_data.valid != 0);
        }

    //
    // Build the sim table using current system settings
    //
    sim_data.pix_bpp        = (UCHAR)HwDeviceExtension->NvInfo.Framebuffer.Depth;
    sim_data.enable_video   = 0;
    sim_data.enable_mp      = 0;
    sim_data.memory_width   = HwDeviceExtension->NvInfo.Dac.InputWidth;;
    sim_data.mem_latency    = (char)PFB_REG_RD_DRF(PFB_Base,_PFB, _CONFIG_1, _CAS_LATENCY); // 3
    sim_data.mem_aligned    = 1;
    sim_data.mem_page_miss  = (char)(PFB_REG_RD_DRF(PFB_Base,_PFB, _CONFIG_1, _RAS_RAS)  +  PFB_REG_RD_DRF(PFB_Base,_PFB, _CONFIG_1, _READ_TO_PCHG)); // 10
    sim_data.gr_during_vid  = 0;
    sim_data.pclk_khz       = HwDeviceExtension->NvInfo.Dac.VClk / 1000;    // in kHz, not MHz
    sim_data.mclk_khz       = HwDeviceExtension->NvInfo.Dac.MClk / 1000;    // in kHz, not MHz
    sim_data.nvclk_khz      = NVClk / 1000;                                 // in kHz, not MHz


    //
    // Get those new numbers
    //
    NV4_dacCalculateArbitration(&fifo_data, &sim_data);

    //
    // If valid settings found, update the hardware
    //
    if (fifo_data.valid)
    {
        //
        // Set the DAC FIFO Thresholds and burst size
        //
        PVIDEO_REG_WR32(PVIDEO_Base, NV_PVIDEO_FIFO_THRES, fifo_data.video_lwm >> 1);
        switch (fifo_data.video_burst_size)
        {
            case 128:
                PVIDEO_REG_WR32(PVIDEO_Base, NV_PVIDEO_FIFO_BURST, 3);
                break;

            case 64:
                PVIDEO_REG_WR32(PVIDEO_Base, NV_PVIDEO_FIFO_BURST, 2);
                break;

            case 32:
                PVIDEO_REG_WR32(PVIDEO_Base, NV_PVIDEO_FIFO_BURST, 1);
                break;
        }


        //
        // Update the CRTC watermarks
        //
        // Unlock CRTC extended regs
        CRTC_RD(NV_CIO_SR_LOCK_INDEX, lock);
        CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_UNLOCK_RW_VALUE);

        //
        // Set the CRTC watermarks and burst size
        //
        CRTC_WR(NV_CIO_CRE_FFLWM__INDEX, fifo_data.graphics_lwm >> 3);

        switch (fifo_data.graphics_burst_size)
        {
            case 256:
                CRTC_WR(NV_CIO_CRE_FF_INDEX, 4);
                break;

            case 128:
                CRTC_WR(NV_CIO_CRE_FF_INDEX, 3);
                break;

            case 64:
                CRTC_WR(NV_CIO_CRE_FF_INDEX, 2);
                break;

            case 32:
                CRTC_WR(NV_CIO_CRE_FF_INDEX, 1);
                break;

            case 16:
                CRTC_WR(NV_CIO_CRE_FF_INDEX, 0);
                break;
        }

        //
        // Relock if necessary
        //
        if (lock == 0)
            CRTC_WR(NV_CIO_SR_LOCK_INDEX, NV_CIO_SR_LOCK_VALUE);

        return;
    }
    else
        //
        // No valid setting was found!!!  Either we fail this configuration
        // or we live with the current default settings for this mode.
        //
        return;

}


//******************************************************************************
//
// Function:    NVEnableBusMastering()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NVEnableBusMastering(PHW_DEVICE_EXTENSION HwDeviceExtension)
    {

    ULONG CurrentValue;

    //**************************************************************************
    // Make sure bus mastering is always ENABLED !!  (Bit 2)
    // Read current value then enable the bus_master bit.
    //**************************************************************************

    CurrentValue = REG_RD32(NV_PBUS_PCI_NV_1);
    CurrentValue |= (1 << DRF_SHIFT(NV_PBUS_PCI_NV_1_BUS_MASTER));
    REG_WR32(NV_PBUS_PCI_NV_1,  CurrentValue);

    }

//******************************************************************************
//
// Function:    NVSaveSpecificRegistersForPwrMgmt()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NVSaveSpecificRegistersForPwrMgmt(PHW_DEVICE_EXTENSION HwDeviceExtension)
    {

    ULONG CurrentValue;

    //**************************************************************************
    // This code was copied from ..\nvalloc\win9x\vxd\osapi.c 
    // for the case NVRM_API_POWER_DOWN:
    //**************************************************************************

    //**************************************************************************
    // Save off instance memory, mainly because the Win9x DX7 driver
    // allocates DMA contexts, but doesn't free them up. (One time init)
    // If we lose power , then all instance memory associated with those
    // DMA contexts are gone.  So we need to save instance mem here before we power down.
    //**************************************************************************
    NV4SaveInstanceMemory(HwDeviceExtension);

    //**************************************************************************
    // Save off miscellaneous registers not reloaded by RmLoadState
    //**************************************************************************

    if ((HwDeviceExtension->ulChipID == NV10_DEVICE_NV10_ID)       ||
        (HwDeviceExtension->ulChipID == NV10DDR_DEVICE_NV10DDR_ID) ||
        (HwDeviceExtension->ulChipID == NV10GL_DEVICE_NV10GL_ID)   ||
        (HwDeviceExtension->ulChipID == NV11_DEVICE_NV11_ID)       ||
        (HwDeviceExtension->ulChipID == NV11DDR_DEVICE_NV11DDR_ID) ||
        (HwDeviceExtension->ulChipID == NV11M_DEVICE_NV11M_ID)     ||
        (HwDeviceExtension->ulChipID == NV11GL_DEVICE_NV11GL_ID)   ||
        (HwDeviceExtension->ulChipID == NV11_DEVICE_CRUSH11_ID)    ||
        (HwDeviceExtension->ulChipID == NV15_DEVICE_NV15_ID)       ||
        (HwDeviceExtension->ulChipID == NV15DDR_DEVICE_NV15DDR_ID) ||
        (HwDeviceExtension->ulChipID == NV15BR_DEVICE_NV15BR_ID)   ||
        (HwDeviceExtension->ulChipID == NV15GL_DEVICE_NV15GL_ID)   ||
        (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_ID)       ||
        (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_1_ID)     ||
        (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_2_ID)     ||
        (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_3_ID))
        {
        NV10ChipFBPowerSave(HwDeviceExtension);
        }
                
    else

        {
        NV4ChipFBPowerSave(HwDeviceExtension);
        }

    }


//******************************************************************************
//
// Function:    NVRestoreSpecificRegistersForPwrMgmt()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NVRestoreSpecificRegistersForPwrMgmt(PHW_DEVICE_EXTENSION HwDeviceExtension)
    {

    ULONG CurrentValue;


    //**************************************************************************
    // Restore chip dependent FB power registers
    //**************************************************************************

    if ((HwDeviceExtension->ulChipID == NV10_DEVICE_NV10_ID)       ||
        (HwDeviceExtension->ulChipID == NV10DDR_DEVICE_NV10DDR_ID) ||
        (HwDeviceExtension->ulChipID == NV10GL_DEVICE_NV10GL_ID)   ||
        (HwDeviceExtension->ulChipID == NV11_DEVICE_NV11_ID)       ||
        (HwDeviceExtension->ulChipID == NV11DDR_DEVICE_NV11DDR_ID) ||
        (HwDeviceExtension->ulChipID == NV11M_DEVICE_NV11M_ID)     ||
        (HwDeviceExtension->ulChipID == NV11GL_DEVICE_NV11GL_ID)   ||
        (HwDeviceExtension->ulChipID == NV11_DEVICE_CRUSH11_ID)    ||
        (HwDeviceExtension->ulChipID == NV15_DEVICE_NV15_ID)       ||
        (HwDeviceExtension->ulChipID == NV15DDR_DEVICE_NV15DDR_ID) ||
        (HwDeviceExtension->ulChipID == NV15BR_DEVICE_NV15BR_ID)   ||
        (HwDeviceExtension->ulChipID == NV15GL_DEVICE_NV15GL_ID)   ||
        (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_ID)       ||
        (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_1_ID)     ||
        (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_2_ID)     ||
        (HwDeviceExtension->ulChipID == NV20_DEVICE_NV20_3_ID))
        {
        NV10ChipFBPowerRestore(HwDeviceExtension);
        }

    else
                    
        {
        NV4ChipFBPowerRestore(HwDeviceExtension);
        }


    //**************************************************************************
    // Save off instance memory, mainly because Win9x DX7 driver
    // allocates DMA contexts, but doesn't free them up.
    // If we lose power , then all instance memory associated with those
    // DMA contexts are gone.  So we need to save them here before we power down.
    //**************************************************************************
    NV4RestoreInstanceMemory(HwDeviceExtension);

    }


//******************************************************************************
//
// Function:    NV4ChipFBPowerSave()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV4ChipFBPowerSave (PHW_DEVICE_EXTENSION HwDeviceExtension)
    {
    // power down mode - max out memory refresh value
    HwDeviceExtension->Power_refresh = REG_RD32(NV_PFB_DEBUG_0);               // refresh
    FLD_WR_DRF_NUM(_PFB, _DEBUG_0, _REFRESH_COUNTX64, 0x01);       // max refresh
    // Chip has been init'd with the BIOS init tables, unless the BIOS
    // has an incompatible structure and the table was not found, in which case
    // we used a default table, which may not have the correct RAM config, so
    // we save away the current config for restoration above.
    HwDeviceExtension->Power_PfbBoot0     = REG_RD32(NV_PFB_BOOT_0);               // RAM config
    HwDeviceExtension->Power_PfbConfig0   = REG_RD32(NV_PFB_CONFIG_0);           // RAM config
    HwDeviceExtension->Power_PfbConfig1   = REG_RD32(NV_PFB_CONFIG_1);           // RAM config
    HwDeviceExtension->Power_PextdevBoot0 = REG_RD32(NV_PEXTDEV_BOOT_0);       // READ STRAP REG
    HwDeviceExtension->Power_PfifoRamHT   = REG_RD32(NV_PFIFO_RAMHT);
    HwDeviceExtension->Power_PfifoRamRO   = REG_RD32(NV_PFIFO_RAMRO);
    HwDeviceExtension->Power_PfifoRamFC   = REG_RD32(NV_PFIFO_RAMFC);
    }
    
 
//******************************************************************************
//
// Function:    NV4ChipFBPowerRestore()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV4ChipFBPowerRestore (PHW_DEVICE_EXTENSION HwDeviceExtension)
    {
    // power up mode - restore previous refresh value
    U032 D0_DELAY_RETRIES = 10; 
    U032 retry = 0;             
    U032 i;                     

    REG_WR32(NV_PFB_DEBUG_0, HwDeviceExtension->Power_refresh);          // refresh
    REG_WR32(NV_PFB_BOOT_0,  HwDeviceExtension->Power_PfbBoot0);         // RAM config
    REG_WR32(NV_PFB_CONFIG_0, HwDeviceExtension->Power_PfbConfig0);      // RAM config
    REG_WR32(NV_PFB_CONFIG_1, HwDeviceExtension->Power_PfbConfig1);      // RAM config

//  A delay seems to be necessary for i820 on Win9x.
//  The problem hasn't been demonstrated on W2K, but its here for safety
//  Reading NV_PMC_ENABLE is simply a convenient delay    
    while (REG_RD32(NV_PEXTDEV_BOOT_0) != HwDeviceExtension->Power_PextdevBoot0) 
    {
        for (i=0; i<2000; i++) 
        {
            REG_RD32(NV_PMC_ENABLE);
        }
        REG_WR32(NV_PEXTDEV_BOOT_0, HwDeviceExtension->Power_PextdevBoot0);    // WRITE STRAP REG
        retry++;
        if (retry == D0_DELAY_RETRIES) 
            break;
    }

    REG_WR32(NV_PFIFO_RAMHT, HwDeviceExtension->Power_PfifoRamHT);      
    REG_WR32(NV_PFIFO_RAMRO, HwDeviceExtension->Power_PfifoRamRO);      
    REG_WR32(NV_PFIFO_RAMFC, HwDeviceExtension->Power_PfifoRamFC);      

    // Touching the CONFIG_0 register messes up the VGA text screen.
    // Unfortunately, SetMode doesn't occur for a while, so this garbage text screen
    // may linger for a while,  so we'll blank the screen to hide it.
    // (SetMode will turn it back on)
    
    REG_RD32(NV_PRMCIO_INP0__COLOR);  // Reset ATC FlipFlop
    REG_WR08(NV_PRMCIO_ARX, 0x00);    // Turn off screen at AR
    REG_RD32(NV_PRMCIO_INP0__COLOR);  // Reset ATC FlipFlop
    }
    

//******************************************************************************
//
// Function:    NV4SaveInstanceMemory()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV4SaveInstanceMemory(PHW_DEVICE_EXTENSION HwDeviceExtension)
    {
    U032 *fb_sv;
    U032 i;
                
    fb_sv = (U032 *)&(HwDeviceExtension->SavedInstanceMemory[0]);
    
    for(i=0; i < ((HwDeviceExtension->TotalInstanceMemory)/4); i++)
         *fb_sv++ = REG_RD32(NV_PRAMIN_DATA032(i));   // from the top--64k BIOS + instance memory
                                                      // this actually decrements from the top
    }

//******************************************************************************
//
// Function:    NV4RestoreInstanceMemory()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV4RestoreInstanceMemory(PHW_DEVICE_EXTENSION HwDeviceExtension)
    {
    U032 *fb_sv;
    U032 data;
    U032 i;
                    
    fb_sv = (U032 *)&(HwDeviceExtension->SavedInstanceMemory[0]);
    
    for(i=0; i < ((HwDeviceExtension->TotalInstanceMemory)/4); i++)
       {
       data = *fb_sv++;
       REG_WR32(NV_PRAMIN_DATA032(i), data);
       }            

    }


//******************************************************************************
//
// Function:    NVEnableVGASubsystem()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NVEnableVGASubsystem(PHW_DEVICE_EXTENSION HwDeviceExtension)
    {


    //**************************************************************************
    // Make sure VGA IO subsystem is alive !!!
    //**************************************************************************

    REG_WR08(NV_PRMVIO_VSE2,  1);


    }

//******************************************************************************
//
// Function:    NVClearMutexPmeAudBuff0()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NVClearMutexPmeAudBuff0(PHW_DEVICE_EXTENSION HwDeviceExtension)
    {

    // Do nothing, this function is only used by NV3
    }


//******************************************************************************
//
// Function:    NVExtractBiosImage()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NVExtractBiosImage(PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    UCHAR *buffer;
    UCHAR char1,char2;
    U032 CurrentValue;
    U032 i;
    
    //**************************************************************************
    // Get ptr to buffer where we'll store the bios image
    //**************************************************************************

    buffer = (UCHAR *)(&(HwDeviceExtension->SavedBiosImageArray[0]));

    char1 = REG_RD08(DEVICE_BASE(NV_PROM) + 0);
    char2 = REG_RD08(DEVICE_BASE(NV_PROM) + 1);

    //**************************************************************************
    // First attempt to get the BIOS image from PROM
    // (Look for the 55,AA signature)
    //**************************************************************************
    
    if ((char1 == 0x55) && (char2 == 0xAA))
    {
        //**********************************************************************
        // Read BIOS image via PROM
        //**********************************************************************
        for (i = 0; i < SAVED_BIOS_IMAGE_SIZE; i++)
        {
            buffer[i] = REG_RD08(DEVICE_BASE(NV_PROM) + i);
        }
    }
    else
    {
#if (_WIN32_WINNT >= 0x0500) && !defined(_WIN64)
        UCHAR *pVideoRomData;
        pVideoRomData = VideoPortGetRomImage(HwDeviceExtension, NULL, 0, SAVED_BIOS_IMAGE_SIZE);

        if(pVideoRomData != NULL)
        {
            char1 = pVideoRomData[0];
            char2 = pVideoRomData[1];
        }

        if ((char1 == 0x55) && (char2 == 0xAA))
        {
            for (i = 0; i < SAVED_BIOS_IMAGE_SIZE; i++)
            {
                buffer[i] = pVideoRomData[i];
            }
        }
        else
#endif // WINN32_WINNT >= 0x0500 NT5 only
        {
            //**********************************************************************
            // Otherwise, get BIOS image from PRAMIN
            //**********************************************************************

            for (i = 0; i < SAVED_BIOS_IMAGE_SIZE; i++)
            {
                buffer[i] = REG_RD08(DEVICE_BASE(NV_PRAMIN) + i);
            }
        }
    }            
}

//******************************************************************************
//
// Function:    FlatPanelCrtc
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

// some CRTC bit definitions
// CR07
#define VT08    1
#define VT09    0x20
#define VRS08   0x04
#define VRS09   0x80
#define EXT_VRS 0x84
#define VDE08   0x02
#define VDE09   0x40
#define VBS08   0x08
// CR09
#define VBS09   0x20
// CR25
#define VT10    0x01
#define VDE10   0x02
#define VRS10   0x04
#define VBS10   0x08

VOID FlatPanelCrtc( U016 XRes )
{
U032    DacHcrtc, DacHt, Ht, Hrs, Hre, DacVcrtc, DacVt, Vt, Vrs, Vre, Vbs, data32;
U016    Attr, Seq;
U008    Cr07, Cr09, Cr11, Cr25;

    // Make sure to always default to centered mode for now
    // Read the current control register value, and force it to be centered
    
    data32 = PRAMDAC_REG_RD32(PRAMDAC_Base,NV_PRAMDAC_FP_TG_CONTROL);

    // Clear out all the bits

    data32 &= (~ ((DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _MODE, _CENTER )) |
                  (DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _MODE, _NATIVE )) |
                  (DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _CENTER, _HORIZ )) |
                  (DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _CENTER, _VERT  )) |
                  (DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _CENTER, _BOTH  ))   ));

    // Force it to be centered (automatic centering)
    data32 |= (DRF_DEF(_PRAMDAC, _FP_TG_CONTROL, _MODE, _CENTER ));
    
    PRAMDAC_REG_WR32(PRAMDAC_Base, NV_PRAMDAC_FP_TG_CONTROL,data32);


    // check for pixel doubled modes
    if (XRes < 640)
    {
        Seq = ReadIndexed(NV_PRMVIO_SRX, 1);
        Seq |= 0x800;   // divide pixel clock to double size
        WriteIndexed(NV_PRMVIO_SRX, Seq);
    }

    // Make sure attr is set correctly
    Cr07 = PRMCIO_REG_RD08(PRMCIO_Base,NV_PRMCIO_INP0__COLOR); // read 3DA to toggle (data = don't care)
    PRMCIO_REG_WR08(PRMCIO_Base,NV_PRMCIO_ARX, 0x30);          // select reg 0x10, bit 5 set to access reg (not palette)
    PRMCIO_REG_WR08(PRMCIO_Base,NV_PRMCIO_ARX, 0x01);          // clear bit 5

    // New rules
    // VRS = VT - 3
    // VRE = VT - 2
    // HRS = HT - 3
    // HRE = HT - 2
    Ht = (ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x00)>>8);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)((Ht-3)<<8 | 0x04));   // HRS
    Hre = ((ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x05)>>8) & 0xe0);    // read CR05 and mask 4:0
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(((Ht-2) | Hre)<<8 | 0x05));   // HRE = Ht - 2, CR04[4:0]

    // gather VT bits
    Vt = (ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x06)>>8);
    Cr07 = (U008)(ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x07)>>8);
    Cr25 = (U008)(ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x25)>>8);
    if (Cr07 & VT08)
        Vt |= 0x0100;
    if (Cr07 & VT09)
        Vt |= 0x0200;
    if (Cr25 & VT10)
        Vt |= 0x0400;

    // VRS: CR10,7,25
    Vrs = Vt - 3;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Vrs<<8 | 0x10));
    Cr07 &= ~EXT_VRS;
    if (Vrs & 0x100) Cr07 |= VRS08;
    if (Vrs & 0x200) Cr07 |= VRS09;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Cr07<<8 | 0x07));
    if (Vrs & 0x400) Cr25 |= VRS10;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Cr25<<8 | 0x25));

    // VRE: CR11[3:0]
    Vre = (Vt - 2) & 0x0f;
    Cr11 = (U008)(ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x11)>>8);
    Vre |= (Cr11 & 0xf0);
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Vre<<8 | 0x11));

    // VBS = VRS
    Cr09 = (U008)(ReadIndexed(NV_PRMCIO_CRX__COLOR, 0x09)>>8);
    Vbs = Vrs;

    // write VBS
    Cr07 &= ~VBS08;
    if (Vbs & 0x100) Cr07 |= VBS08;
    Cr09 &= ~VBS09;
    if (Vbs & 0x200) Cr09 |= VBS09;
    Cr25 &= ~VBS10;
    if (Vbs & 0x400) Cr25 |= VBS10;
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Vbs<<8 | 0x15));
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Cr07<<8 | 0x07));
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Cr09<<8 | 0x09));
    WriteIndexed(NV_PRMCIO_CRX__COLOR, (U016)(Cr25<<8 | 0x25));

    // wait for vsync

    while (PRMCIO_REG_RD08(PRMCIO_Base,NV_PRMCIO_INP0__COLOR) & 0x08);     // wait for active display
    while (!(PRMCIO_REG_RD08(PRMCIO_Base,NV_PRMCIO_INP0__COLOR) & 0x08));  // wait for retrace start

    data32 = PRAMDAC_REG_RD32(PRAMDAC_Base,NV_PRAMDAC_FP_DEBUG_0);           // usual macro won't help us here
    data32 &= ~(NV_PRAMDAC_FP_DEBUG_0_TEST_BOTH << 16);    // clear VCNTR bits
    PRAMDAC_REG_WR32(PRAMDAC_Base,NV_PRAMDAC_FP_DEBUG_0, data32);
}




//******************************************************************************
//
// Function:    Disable Interrupt
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV_DisableNVInterrupts
(
    PHW_DEVICE_EXTENSION HwDeviceExtension
)
{
    ULONG CurrentValue;

    //**************************************************************************
    // Make sure that we're not currently in an interrupt routine first
    // If interrupts are disabled, then someone is currently handling
    // the interrupts.  Wait for Interrupts to get re-activated.
    //**************************************************************************

    CurrentValue = PMC_REG_RD32(PMC_Base, NV_PMC_INTR_EN_0);
    while (CurrentValue == 0)
        CurrentValue = PMC_REG_RD32(PMC_Base, NV_PMC_INTR_EN_0);

    //**************************************************************************
    // Save off current value of PMC_INTR_EN_0
    //**************************************************************************

    CurrentValue = PMC_REG_RD32(PMC_Base, NV_PMC_INTR_EN_0);

    HwDeviceExtension->SavedPMCState = CurrentValue;

    //**************************************************************************
    // Disable NV Interrupts
    //**************************************************************************

    PMC_REG_WR32(PMC_Base, NV_PMC_INTR_EN_0, 0x00000000);
}


//******************************************************************************
//
// Function:    Re-Enable Interrupt
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV_ReEnableNVInterrupts
(
    PHW_DEVICE_EXTENSION HwDeviceExtension
)
{
    PMC_REG_WR32(PMC_Base, NV_PMC_INTR_EN_0, HwDeviceExtension->SavedPMCState);

}


//******************************************************************************
//
// Function:    ManualTextModeSet
//
// Routine Description:
//
//              This code was ported from the BIOS
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV_ManualTextModeSet
(
    PHW_DEVICE_EXTENSION HwDeviceExtension
)
{
    U008 byteValue;
    U032 dwordValue;
    U008 savedCrtcIndex;
    U008 savedLockValue;
    U008 initialLockValue;
            
    //**************************************************************************
    // Unlock extended registers
    //**************************************************************************
                    
    savedCrtcIndex = REG_RD08(NV_PRMCIO_CRX__COLOR); // 3d4

    CRTC_RD(NV_CIO_SR_LOCK_INDEX , savedLockValue);  // 1f lock index
    initialLockValue = savedLockValue;
    savedLockValue &=1;

    CRTC_RD(NV_CIO_CRE_SCRATCH1__INDEX , byteValue); // scratch index 2c
    byteValue &=0xfe;
    byteValue |=savedLockValue;
    CRTC_WR(NV_CIO_CRE_SCRATCH1__INDEX , byteValue); // scratch index 2c
    
    CRTC_WR(NV_CIO_SR_LOCK_INDEX , NV_CIO_SR_UNLOCK_RW_VALUE ); // 0x57 unlock value

    //**************************************************************************
    // OEMPreSetMode
    //**************************************************************************

    if (HwDeviceExtension->FlatPanelIsPresent)      // Flat panel is present
        {
        CRTC_WR(0x21, 0xba);
        }
    else if (HwDeviceExtension->TvIsPresent)        // TV is present
        {
        ;       // Do nothing
        }
    else
        {
        CRTC_WR(0x21, 0xfa);                        // CRT present
        }
                             
    //**************************************************************************
    // ATCOFF
    //**************************************************************************

// cli
    byteValue = REG_RD08(NV_PRMCIO_INP0__MONO);     // 3ba
    byteValue = REG_RD08(NV_PRMCIO_INP0__COLOR);    // 3da
    REG_WR08(NV_PRMCIO_ARX, 0);                     // 3c0
// sti

    //**************************************************************************
    // OEMPreSetRegs
    //**************************************************************************
    
    CRTC_RD(0x28, byteValue);                       // 3d4
    byteValue &=0xf8;
    CRTC_WR(0x28, byteValue);
    
    CRTC_RD(0x1a, byteValue);                       // 3d4
    byteValue |=2;
    CRTC_WR(0x1a, byteValue);

    //**************************************************************************
    // SetRegs
    //**************************************************************************
    
    REG_WR08(NV_PRMVIO_SRX, 0);                     // 3c4
    REG_WR08(NV_PRMVIO_SR_RESET, 1);                // 3c5
    
    REG_WR08(NV_PRMVIO_MISC__WRITE , 0x67);         // 3c2
                               

    REG_WR08(NV_PRMVIO_SRX, 1);                     // 3c4
    REG_WR08(NV_PRMVIO_SR_RESET, 0);                // 3c5

    REG_WR08(NV_PRMVIO_SRX, 2);                     // 3c4
    REG_WR08(NV_PRMVIO_SR_RESET, 3);                // 3c5

    REG_WR08(NV_PRMVIO_SRX, 3);                     // 3c4
    REG_WR08(NV_PRMVIO_SR_RESET, 0);                // 3c5

    REG_WR08(NV_PRMVIO_SRX, 4);                     // 3c4
    REG_WR08(NV_PRMVIO_SR_RESET, 2);                // 3c5

    REG_WR08(NV_PRMVIO_SRX, 0);                     // 3c4
    REG_WR08(NV_PRMVIO_SR_RESET, 3);                // 3c5

    //**************************************************************************
    // CRTC Regs
    //**************************************************************************

    CRTC_WR(0x11, 0x20);                            // 3d4
    CRTC_WR(0x00, 0x5f);
    CRTC_WR(0x01, 0x4f);
    CRTC_WR(0x02, 0x50);
    CRTC_WR(0x03, 0x82);
    CRTC_WR(0x04, 0x55);
    CRTC_WR(0x05, 0x81);
    CRTC_WR(0x06, 0xbf);
    CRTC_WR(0x07, 0x1f);
    CRTC_WR(0x08, 0x00);
    CRTC_WR(0x09, 0x4f);
    CRTC_WR(0x0a, 0x0d);
    CRTC_WR(0x0b, 0x0e);
    CRTC_WR(0x0c, 0x00);
    CRTC_WR(0x0d, 0x00);
    CRTC_WR(0x0e, 0x00);
    CRTC_WR(0x0f, 0x00);
    CRTC_WR(0x10, 0x9c);
    CRTC_WR(0x11, 0x8e);
    CRTC_WR(0x12, 0x8f);
    CRTC_WR(0x13, 0x28);
    CRTC_WR(0x14, 0x1f);
    CRTC_WR(0x15, 0x96);
    CRTC_WR(0x16, 0xb9);
    CRTC_WR(0x17, 0xa3);
    CRTC_WR(0x18, 0xff);

    //**************************************************************************
    // GDC Regs
    //**************************************************************************

    REG_WR08(NV_PRMVIO_GRX,   0x0);                 // 3ce
    REG_WR08(NV_PRMVIO_GX_SR, 0x0);            

    REG_WR08(NV_PRMVIO_GRX,   0x1);            
    REG_WR08(NV_PRMVIO_GX_SR, 0x0);            

    REG_WR08(NV_PRMVIO_GRX,   0x2);            
    REG_WR08(NV_PRMVIO_GX_SR, 0x0);            

    REG_WR08(NV_PRMVIO_GRX,   0x3);            
    REG_WR08(NV_PRMVIO_GX_SR, 0x0);            

    REG_WR08(NV_PRMVIO_GRX,   0x4);            
    REG_WR08(NV_PRMVIO_GX_SR, 0x0);            

    REG_WR08(NV_PRMVIO_GRX,   0x5);            
    REG_WR08(NV_PRMVIO_GX_SR, 0x10);            

    REG_WR08(NV_PRMVIO_GRX,   0x6);            
    REG_WR08(NV_PRMVIO_GX_SR, 0x0e);            

    REG_WR08(NV_PRMVIO_GRX,   0x7);            
    REG_WR08(NV_PRMVIO_GX_SR, 0x0);            

    REG_WR08(NV_PRMVIO_GRX,   0x8);            
    REG_WR08(NV_PRMVIO_GX_SR, 0xff);            

    //**************************************************************************
    // Full Cpu ON
    //**************************************************************************
    
    REG_WR08(NV_PRMVIO_SRX, 1);                     // 3c4
    byteValue = REG_RD08(NV_PRMVIO_SR_RESET);       // 3c5
    byteValue |=0x20;
    REG_WR08(NV_PRMVIO_SRX, 1);                     // 3c4
    REG_WR08(NV_PRMVIO_SR_RESET,byteValue  );       // 3c5

    //**************************************************************************
    // OEMSetRegs
    //**************************************************************************

    CRTC_WR(0x19, 0x0);

    CRTC_RD(0x1a,byteValue);
    byteValue &=0xc0;
    byteValue |=0x38;
    byteValue |=1;
    byteValue |=4;
    CRTC_WR(0x1a, byteValue);

    CRTC_WR(0x1b, 0x3);
    CRTC_WR(0x1c, 0x18);
    CRTC_WR(0x1d, 0x0);
    CRTC_WR(0x1e, 0x0);

    CRTC_WR(0x20, 0x20);
    CRTC_WR(0x25, 0x0);

    CRTC_RD(0x28,byteValue);
    byteValue &=0xf8;
    CRTC_WR(0x28, byteValue);


    CRTC_WR(0x2d, 0x0);
    CRTC_WR(0x29, 0x0);
    CRTC_WR(0x2a, 0x0);
    CRTC_WR(0x30, 0x0);
    CRTC_WR(0x31, 0x0);
    CRTC_WR(0x32, 0x0);

    CRTC_RD(0x33,byteValue);
    byteValue &= 0x7f;
    CRTC_WR(0x33, byteValue);

    CRTC_WR(0x39, 0xff);


    dwordValue = REG_RD32(NV_PRAMDAC_PLL_COEFF_SELECT);
    dwordValue &=0xeffffdfe;
    if (HwDeviceExtension->FlatPanelIsPresent)      // Flat panel is present
        dwordValue |= 0x00000200;
    REG_WR32(NV_PRAMDAC_PLL_COEFF_SELECT,dwordValue);

    dwordValue = REG_RD32(NV_PRAMDAC_GENERAL_CONTROL);
    dwordValue &= 0xffefeeef;
    dwordValue |= 0x00000100;
    dwordValue |= 0x00000010;
    REG_WR32(NV_PRAMDAC_GENERAL_CONTROL,dwordValue);

    REG_WR08(NV_PRMCIO_CRX__COLOR, 0x18);                    

    //**************************************************************************
    // Skip Load Palette and LoadSysfont
    //**************************************************************************

    //**************************************************************************
    // Skip OemFixupSetMode/ FPEndSetMode for now    (Flat panel only)
    //**************************************************************************

//    FPEndSetMode();

    //**************************************************************************
    // SetDPMSOff
    // Turn off DPMS on CRT only
    //**************************************************************************

    if ((!(HwDeviceExtension->FlatPanelIsPresent)) &&   // Flat panel not present
        (!(HwDeviceExtension->TvIsPresent)) )           // TV not present
        {    
        CRTC_RD(0x1a, byteValue);
        byteValue&=0x3f;
        CRTC_WR(0x1a, byteValue);

        //**********************************************************************
        // Full CPU Off
        //**********************************************************************
    
        REG_WR08(NV_PRMVIO_SRX, 1);                     // 3c4
        byteValue = REG_RD08(NV_PRMVIO_SR_RESET);       // 3c5
        byteValue &=0xdf;
        REG_WR08(NV_PRMVIO_SRX, 1);                     // 3c4
        REG_WR08(NV_PRMVIO_SR_RESET,byteValue);         // 3c5
    
        //**********************************************************************
        // ATCON
        //**********************************************************************
// cli    
        byteValue = REG_RD08(NV_PRMCIO_INP0__MONO);     // 3ba
        byteValue = REG_RD08(NV_PRMCIO_INP0__COLOR);    // 3da
        REG_WR08(NV_PRMCIO_ARX, 0x20);                  // 3c0
        byteValue = REG_RD08(NV_PRMCIO_INP0__MONO);     // 3ba
        byteValue = REG_RD08(NV_PRMCIO_INP0__COLOR);    // 3da
// sti
        }
            
    //**************************************************************************
    // Full CPU Off
    //**************************************************************************

    REG_WR08(NV_PRMVIO_SRX, 1);                     // 3c4
    byteValue = REG_RD08(NV_PRMVIO_SR_RESET);       // 3c5
    byteValue &=0xdf;
    REG_WR08(NV_PRMVIO_SRX, 1);                     // 3c4
    REG_WR08(NV_PRMVIO_SR_RESET,byteValue);         // 3c5

    //**************************************************************************
    // ATCON
    //**************************************************************************

//cli
    byteValue = REG_RD08(NV_PRMCIO_INP0__MONO);     // 3ba
    byteValue = REG_RD08(NV_PRMCIO_INP0__COLOR);    // 3da
    REG_WR08(NV_PRMCIO_ARX, 0x20);                  // 3c0
    byteValue = REG_RD08(NV_PRMCIO_INP0__MONO);     // 3ba
    byteValue = REG_RD08(NV_PRMCIO_INP0__COLOR);    // 3da
//sti

    //**************************************************************************
    // Skip OEMPostSetMode  (TV only)
    //**************************************************************************

    //**************************************************************************
    // Restore / Lock extended registers if necessary
    //**************************************************************************

    CRTC_RD(NV_CIO_CRE_SCRATCH1__INDEX , byteValue); // scratch index 2c
    byteValue &=0xfe;                                // remove bit 0
    CRTC_WR(NV_CIO_CRE_SCRATCH1__INDEX , byteValue); // scratch index 2c

    if (initialLockValue == 0)
        {
        CRTC_WR(NV_CIO_SR_LOCK_INDEX,NV_CIO_SR_LOCK_VALUE);
        }
    else if (initialLockValue == 1)
        {
        CRTC_WR(NV_CIO_SR_LOCK_INDEX,NV_CIO_SR_UNLOCK_RO_VALUE);
        }
    else if (initialLockValue == 3)
        {
        CRTC_WR(NV_CIO_SR_LOCK_INDEX,NV_CIO_SR_UNLOCK_RW_VALUE);
        }

    //**************************************************************************
    // Restore the original CRTC index register
    //**************************************************************************

    REG_WR08(NV_PRMCIO_CRX__COLOR, savedCrtcIndex); // 3d4

    }



#if (_WIN32_WINNT >= 0x0500)

//******************************************************************************
//
// Function:    NV_VBE_DPMS_GetPowerState()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

BOOLEAN NV_VBE_DPMS_GetPowerState(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG *CurrentPowerState)

    {
    UCHAR lock;
    UCHAR cr1a;
    UCHAR result;

    //**************************************************************************
    // Save off original lock value, then unlock extended registers
    //**************************************************************************

    CRTC_RD(NV_CIO_SR_LOCK_INDEX,lock);
    CRTC_WR(NV_CIO_SR_LOCK_INDEX,NV_CIO_SR_UNLOCK_RW_VALUE);

    //**************************************************************************
    // Read CR1A
    //
    //   Bit 7 Hsync Toggle disable.  When set to a 1, this bit forces the Hsync to
    //            inactive state (high or low as programmed in the bit-6 of the
    //            Miscellaneous output register).
    //
    //   Bit 6 Vsync Toggle disable.  When set to a 1, this bit forces the Vsync to
    //            inactive state (high or low as programmed in the bit-7 of the
    //            Miscellaneous output register).
    //
    //**************************************************************************

    CRTC_RD(NV_CIO_CRE_RPC1_INDEX, cr1a);

    //**************************************************************************
    // Isolate DPMS bits (CR1A bits 7 & 6)
    //**************************************************************************

    cr1a >>=14;
    *CurrentPowerState = tblDPMSStates[cr1a];

    //**************************************************************************
    // Relock the extended registers, if necessary
    //**************************************************************************

    //**************************************************************************
    // Relock the extended registers, if necessary
    //**************************************************************************

    if (lock == 0)
        {
        CRTC_WR(NV_CIO_SR_LOCK_INDEX,NV_CIO_SR_LOCK_VALUE);
        }
    else if (lock == 1)
        {
        CRTC_WR(NV_CIO_SR_LOCK_INDEX,NV_CIO_SR_UNLOCK_RO_VALUE);
        }
    else if (lock == 3)
        {
        CRTC_WR(NV_CIO_SR_LOCK_INDEX,NV_CIO_SR_UNLOCK_RW_VALUE);
        }


    return(TRUE);
    }


//******************************************************************************
//
// Function:    NV_VBE_DPMS_SetPowerState()
//
// Routine Description:
//
//       VBE Function 10h
//       VBE_DPMS - Display Power Management Signaling functions (DPMS).
//
//       Entry:  AX = 4F10h
//               BL = Subfunction
//               ES:DI = Null pointer
//               DS = Seg0
//
//               Other registers dependent on subfunction
//               Subfunction 0:
//                       None
//               Subfunction 1:
//                       BH = Requested power state
//                            0 = On
//                            1 = Stand by
//                            2 = Suspend
//                            4 = Off
//                            8 = Reduced on
//               Subfunction 2:
//                       None
//
//       Exit:   AX = VBE return status
//               Other registers dependent on subfunction:
//               Subfunction 0:
//                       BH = States supported:
//                            bit 0      Stand by
//                            bit 1      Suspend
//                            bit 2      Off
//                            bit 3      Reduced on
//                            bits 4-7   Reserved
//                       BL = VBE/PM Version number:
//                            bits 0-3   Minor version number (0)
//                            bits 4-7   Major version number (1)
//               Subfunction 1:
//                       None
//               Subfunction 2:
//                       BH = Power state currently requested by controller
//                            0 = On
//                            1 = Stand by
//                            2 = Suspend
//                            4 = Off
//                            8 = Reduced on
//
//       The VESA committee defined a method of signalling a monitor
//       to shutdown or to go into standby mode. The sync signals are
//       used in the following manner:
//
//       H Sync  V Sync  Result
//       ======  ======  ======
//       Pulses  Pulses  Monitor is active
//       None    Pulses  Monitor is in "stand-by" mode
//       Pulses  None    Monitor is in "suspend" mode
//       None    None    Monitor is in "shut down" mode
//
//       Note: ES, DS, SI have been pushed on the stack already
//
// Arguments:
//
// Return Value:
//
//******************************************************************************
BOOLEAN NV_VBE_DPMS_SetPowerState(PHW_DEVICE_EXTENSION HwDeviceExtension, ULONG PowerState)
{
    BOOLEAN bRet = 0;
    ULONG i;

    for(i = 0; i < HwDeviceExtension->DeskTopInfo.ulNumberDacsActive; i++)
    {
        
        switch (PowerState)
        {

            case VideoPowerOn:
            case VideoPowerHibernate:
                bRet |= 
                    RmSetDisplayPowerState
                    (
                        HwDeviceExtension->DeviceReference,
                        HwDeviceExtension->DeskTopInfo.ulDeviceDisplay[i],
                        TRUE
                    );
                HwDeviceExtension->ulRmTurnedOffMonitor = FALSE;
                break;

            case VideoPowerStandBy:
                    //RmSetDisplayPowerState...
                    break;

            case VideoPowerSuspend:
                    //RmSetDisplayPowerState...
                    break;

            case VideoPowerOff:
                bRet |= 
                    RmSetDisplayPowerState
                    (
                        HwDeviceExtension->DeviceReference,
                        HwDeviceExtension->DeskTopInfo.ulDeviceDisplay[i],
                        FALSE
                    );
                HwDeviceExtension->ulRmTurnedOffMonitor = TRUE;
                break;
        }
    }    

    return(bRet);

}

//******************************************************************************
//
// Function:    GetCurrentDPMSState(HwDeviceExtension)
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

U032 GetCurrentDPMSState(PVOID HwDeviceExtension)

    {
    PHW_DEVICE_EXTENSION hwDeviceExtension = HwDeviceExtension;
    U032 Value;
    
    
    Value = hwDeviceExtension->CurrentDPMSState;
    return(Value);
    }


//******************************************************************************
//
// Function:    NV_ATCOff()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV_ATCOff(PHW_DEVICE_EXTENSION HwDeviceExtension)
    {

    NV_ResetATCFlipFlop(HwDeviceExtension);        // Reset ATC to index state

    PRMCIO_REG_WR08(PRMCIO_Base,NV_PRMCIO_ARX, 0x0);

    }


//******************************************************************************
//
// Function:    NV_ATCOn()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV_ATCOn(PHW_DEVICE_EXTENSION HwDeviceExtension)
    {

    NV_ResetATCFlipFlop(HwDeviceExtension);    // Reset ATC to index state

    PRMCIO_REG_WR08(PRMCIO_Base,NV_PRMCIO_ARX, 0x20);

    NV_ResetATCFlipFlop(HwDeviceExtension);    // For compatibility with DOS EDIT

    }


//******************************************************************************
//
// Function:    NV_ResetATCFlipFlop()
//
// Routine Description:
//
// Arguments:
//
// Return Value:
//
//******************************************************************************

VOID NV_ResetATCFlipFlop(PHW_DEVICE_EXTENSION HwDeviceExtension)
    {
    volatile U008 value;

    value = PRMCIO_REG_RD08(PRMCIO_Base,NV_PRMCIO_INP0__COLOR);
    value = PRMCIO_REG_RD08(PRMCIO_Base,NV_PRMCIO_INP0__MONO);

    }


#endif // WINN32_WINNT >= 0x0500 NT5 only



//******************************************************************************
//
//  Function: NVGetTimingForDac()
//
//  Routine Description:
//
//      This routine get timing values for dac at giving mode.
//  Arguments:
//
//      HwDeviceExtension - Pointer to the miniport driver's device extension.
//
//
//  Return Value:
//      TRUE - successful; FALSE - failed
//
//******************************************************************************
BOOL NVGetTimingForDac(PHW_DEVICE_EXTENSION hwDeviceExtension, HEAD_RESOLUTION_INFO *pResolution, DAC_TIMING_VALUES * pTimingInfo)
{
    GetTimingDacCommonModesetCode(hwDeviceExtension, pResolution, pTimingInfo);
    return (TRUE);
}

//
// Read in the monitor restriction mode list per head from the registry.
//
VOID ReadMonitorRestrictionModeList(PHW_DEVICE_EXTENSION HwDeviceExtension)
{
    U016 *ValidModePtr;
    U016 *RegDataPtr;
    ULONG NumRegistryModes;
    U016  *BaseOfValidModeTable;
    ULONG i;
    U016 RegWidth,RegHeight,RegDepth,RegRefresh;
    
    // init the Monitor restriction mode tables.
    MonitorRestrictionModeList0_registry_data[0]     = 0xFFFF ;
    MonitorRestrictionModeList1_registry_data[0]     = 0xFFFF ;
    HwDeviceExtension->NumRestrictionModes0 = 0;
    HwDeviceExtension->NumRestrictionModes1 = 0;
    // copy the monitor restriction modelists for both heads.
    NV_ReadAndCopyRegistryData(HwDeviceExtension, L"MonitorModeList0",      &(MonitorRestrictionModeList0_registry_data[0] ));
    NV_ReadAndCopyRegistryData(HwDeviceExtension, L"MonitorModeList1",      &(MonitorRestrictionModeList1_registry_data[0] ));

    for (i=0; i < NV_NO_DACS; i++)
    {
        RegDataPtr = (i ? (U016 *)&MonitorRestrictionModeList1_registry_data[0] : (U016 *)&MonitorRestrictionModeList0_registry_data[0]);
        ValidModePtr = (i ? (U016 *)&HwDeviceExtension->RestrictionModeTable1[0] : (U016 *)&HwDeviceExtension->RestrictionModeTable0[0]);
        BaseOfValidModeTable = ValidModePtr;
        NumRegistryModes = 0;
    
        //**************************************************************************
        // Parse each line from the registry.
        // For each refresh rate (of each mode), check if the refresh is present in the
        // vesa data table.  If so, then it's a valid mode. 
        // Registry data is terminated with 0xffff
        //**************************************************************************

        while (*RegDataPtr != 0xffff)
            {
            RegDepth   = *(RegDataPtr);             // Get Depth from registry
            RegWidth   = *(RegDataPtr+1);           // Get Width from registry
            RegHeight  = *(RegDataPtr+2);           // Get Height from registry

            //**********************************************************************
            // Advance to the registry refresh rates for this particular mode
            //**********************************************************************

            RegDataPtr+=3;

            //**********************************************************************
            // Each mode entry in the registry data is NULL terminated.
            // Parse all refresh rates for this mode
            //**********************************************************************

            while (*RegDataPtr !=0)
            {        
                RegRefresh  = *(RegDataPtr++);      // Get Refresh from registry data

                //**********************************************************
                // Generate 8bpp entry for this refresh rate
                //**********************************************************

                *ValidModePtr      = TRUE;              // Valid Flag
                *(ValidModePtr+1)  = RegWidth;          // Width
                *(ValidModePtr+2)  = RegHeight;         // Height
                *(ValidModePtr+3)  = RegDepth;          // Depth                                                                 
                *(ValidModePtr+4)  = RegRefresh;        // Refresh                                                               
                ValidModePtr +=5;                       // Next Valid Mode

                //**********************************************************
                // Keep track of how many modes have currently been specified
                //**********************************************************

                NumRegistryModes++;

                //**********************************************************
                // DEBUG safety check
                // Make sure we never go past the end of the
                // allocated valid mode table. (back off 30 words from end of table)
                //**********************************************************

                if (ValidModePtr > (U016 *)((PUCHAR)BaseOfValidModeTable + (MAX_VALID_MODE_TABLE_DATA_WORDS * 2) - (30*2) ))
                {
                    VideoDebugPrint((0, "!!! ERROR:                                 !!!\n"));
                    VideoDebugPrint((0, "!!! ERROR: ERROR ERROR ERROR ERROR ERROR   !!!\n"));
                    VideoDebugPrint((0, "!!! ERROR: Went past end of ValidModeTable !!!\n"));
                    VideoDebugPrint((0, "!!! ERROR: Went past end of ValidModeTable !!!\n"));
                    VideoDebugPrint((0, "!!! ERROR: Went past end of ValidModeTable !!!\n"));
                    VideoDebugPrint((0, "!!! ERROR: ERROR ERROR ERROR ERROR ERROR   !!!\n"));
                    VideoDebugPrint((0, "!!! ERROR:                                 !!!\n"));
                }
            } // while (*RegDataPtr !=0)...

            //**********************************************************************
            // Skip past null terminator
            //**********************************************************************

            RegDataPtr++;
                
        } // while (*RegDataPtr != 0xffff)...

        //
        // Save the number of restriction modes for each head
        //
        if (i == 0) 
        {
            HwDeviceExtension->NumRestrictionModes0 = NumRegistryModes;
        }
        else
        {
            HwDeviceExtension->NumRestrictionModes1 = NumRegistryModes;
        }
    } // for each head
}

#if (_WIN32_WINNT >= 0x0500)
//
// Gets called while shutting down.
// Disables interrupts as they are left on by the ResMan to make full screen DOS VGA mode
// work correctly.
//
BOOLEAN
NVResetHW(
    PVOID hwDeviceExtension,
    ULONG Column,
    ULONG Rows
    )
{
    PHW_DEVICE_EXTENSION HwDeviceExtension;
    HwDeviceExtension = (PHW_DEVICE_EXTENSION)hwDeviceExtension;

    // Disable the interrupts.
    PMC_REG_WR32(PMC_Base, NV_PMC_INTR_EN_0, 0x00000000);

    // Since we are not setting the VGA mode, return FALSE so that the HAL can do the
    // equivalent of Int10, mode 3.
    return(FALSE);
}
#endif (_WIN32_WINNT >= 0x0500)


//
// DetectDevice is called from DetectNextDevice and checks whether in psnSlot is a nvidia device and if so it will claim
//              it's resources (if bClaimResources is TRUE). It also checks whether the device is a primary VGA device.
//  NOTE :      This is the place to add new board IDs (and it should remain the only place in the miniport)
//

VP_STATUS DetectDevice(PHW_DEVICE_EXTENSION   pHDE,                             // our devices 'global' data struct
                        PCI_SLOT_NUMBER       psnSlot,                          // the slot to check
                        ULONG                 ulNumAccessRanges,                // is needed the to claim the resources, for now it's always 2
                        PVIDEO_ACCESS_RANGE   pAccessRanges,                    // if bClaimResources == TRUE this exports the device's mapping
                        USHORT                *pusVendorID,
                        USHORT                *pusDeviceID,
                        BOOLEAN               *pbIsPrimaryDevice,
                        BOOLEAN               bClaimResources)
{
    VP_STATUS         status = ERROR_DEV_NOT_EXIST;
    PCI_COMMON_CONFIG pcc;
    ULONG             ulBytes;
    ULONG             ulVendorID;
    ULONG             ulDeviceID;
    PVOID             pvVendorID;
    PVOID             pvDeviceID;
    ULONG             ulSlot;
#if (_WIN32_WINNT >= 0x0500)
    ULONG             VgaStatus;
#endif

    ASSERT(pHDE);
    ASSERT(ulNumAccessRanges);
    ASSERT(pAccessRanges);
    ASSERT(pusVendorID);
    ASSERT(pusDeviceID);
    ASSERT(pbIsPrimaryDevice);

    VideoPortZeroMemory(&pcc, sizeof(PCI_COMMON_CONFIG));

    ulBytes = VideoPortGetBusData(pHDE, 
                                    PCIConfiguration,
                                    psnSlot.u.AsULONG,             // assuming that function = 0 , W2K ignores this 
                                    &pcc,
                                    0,
                                    PCI_COMMON_HDR_LENGTH);

    if (PCI_COMMON_HDR_LENGTH == ulBytes)
    {
        if (NV4_VENDOR_ID == pcc.VendorID) // most trivial check
        {
            //
            // list of boards supported now
            //
            switch (pcc.DeviceID)
            {
            case NV20_DEVICE_NV20_ID:
            case NV20_DEVICE_NV20_1_ID:
            case NV20_DEVICE_NV20_2_ID:
            case NV20_DEVICE_NV20_3_ID:
                ulNumAccessRanges++;            // NV20 needs one more access ranges
            case NV4_DEVICE_NV4_ID:
            case NV5_DEVICE_NV5_ID:
            case NV5ULTRA_DEVICE_NV5ULTRA_ID:
            case NV5VANTA_DEVICE_NV5VANTA_ID:
            case NV5MODEL64_DEVICE_NV5MODEL64_ID:
            case NV0A_DEVICE_NV0A_ID:
            case NV10_DEVICE_NV10_ID:
            case NV10DDR_DEVICE_NV10DDR_ID:
            case NV10GL_DEVICE_NV10GL_ID:
            case NV11_DEVICE_NV11_ID:
            case NV11DDR_DEVICE_NV11DDR_ID:
            case NV11M_DEVICE_NV11M_ID:
            case NV11GL_DEVICE_NV11GL_ID:
            case NV11_DEVICE_CRUSH11_ID:
            case NV15_DEVICE_NV15_ID:
            case NV15DDR_DEVICE_NV15DDR_ID:
            case NV15BR_DEVICE_NV15BR_ID:
            case NV15GL_DEVICE_NV15GL_ID:
                *pusVendorID = pcc.VendorID;
                *pusDeviceID = pcc.DeviceID;

#if (_WIN32_WINNT >= 0x0500)
                //
                // On IA64 it isn't save to rely on PCI_ENABLE_IO_SPACE to detect the
                // primary device. Since Win2K it is possible to query the VGA status:
                //
                if (NO_ERROR == VideoPortGetVgaStatus(pHDE, &VgaStatus))
                {
                    // VgaStatus (0) indicates that VGA is not enabled -> secondary device
                    // VgaStatus (1) indicates that VGA is enabled -> primary device
                    *pbIsPrimaryDevice = (1 == VgaStatus); 
                }
                else
#endif
                {
                    //
                    //  if the PCI io space is enabled we are on the primary VGA device
                    //
                    *pbIsPrimaryDevice = pcc.Command & PCI_ENABLE_IO_SPACE ? TRUE : FALSE;
                }

                //
                // if we´re not the primary device we´ll have to enable MMIO and BUS MASTER
                //
                if (! *pbIsPrimaryDevice)
                {
                    //
                    // if NT4_MULTI_DEV is not defined, behave like original driver did -> don't recognize disabled board
                    //
#if (defined (NT4_MULTI_DEV) || (_WIN32_WINNT >= 0x0500))
                    // just prepare common config that VPGetAccessRanges can read the physical aperture
                    pcc.Command |=  PCI_ENABLE_MEMORY_SPACE | PCI_ENABLE_BUS_MASTER;

                    ulBytes = VideoPortSetBusData(pHDE, 
                                                PCIConfiguration,
                                                psnSlot.u.AsULONG,             
                                                &pcc.Command,
                                                offsetof(PCI_COMMON_CONFIG, Command),
                                                sizeof(ULONG));
                    if (sizeof(ULONG) != ulBytes)
#endif
                    {
                        status = ERROR_DEV_NOT_EXIST;
                        goto EXIT;
                    }
                }

#if (_WIN32_WINNT < 0x0500)
                ulSlot      = psnSlot.u.AsULONG;                // start looking at this slot
                ulVendorID  = (ULONG)(pcc.VendorID) & 0xffff;
                ulDeviceID  = (ULONG)(pcc.DeviceID) & 0xffff;
                pvVendorID  = (PVOID)&ulVendorID;
                pvDeviceID  = (PVOID)&ulDeviceID;
#else
                // W2K automatically uses the right slot, so all slot information is useless or invalid here
                ulSlot      = 0;                                // has to be 0 on W2K and above
                pvVendorID  = NULL;
                pvDeviceID  = NULL;
#endif
                //
                // VideoPortGetAccessRanges fills pAccessRanges (and pvVendor/DeviceID which obviously should match the ones we
                //                          found using VideoPortGetBusData 8-) )
                //
                if (bClaimResources)
                {
                    status = VideoPortGetAccessRanges(pHDE, 
                                                      0,
                                                      NULL,
                                                      ulNumAccessRanges,
                                                      pAccessRanges,
                                                      pvVendorID,
                                                      pvDeviceID, 
                                                      &ulSlot);
                    if (NO_ERROR == status)
                    {
                        //
                        // check data consistency for NT 4 only as in W2K ulSlot is alwas 0 
                        //
#if (_WIN32_WINNT < 0x0500) 
                        if (ulSlot != psnSlot.u.AsULONG ||
                          (USHORT)ulVendorID != *pusVendorID ||
                          (USHORT)ulDeviceID != *pusDeviceID)
                        {
                            //
                            // the last VideoPortGetAccessRanges has claimed resources but obviously
                            // for the wrong slot, so we unclaim everything with NumAccessRanges = 0
                            //
                            status = VideoPortGetAccessRanges(pHDE, 
                                                              0,
                                                              NULL,
                                                              0,
                                                              pAccessRanges,
                                                              pvVendorID,
                                                              pvDeviceID, 
                                                              &ulSlot);
                            ASSERT(NO_ERROR == status);
                            status = ERROR_DEV_NOT_EXIST;
                            goto EXIT;
                        }
#endif

                        //
                        // for some reason VideoPortGetAccessRanges enables PortIO in our command register
                        // so we have to reset this for non primary devices
                        //
                        if (! *pbIsPrimaryDevice)
                        {
                            ulBytes = VideoPortGetBusData(pHDE, 
                                                            PCIConfiguration,
                                                            psnSlot.u.AsULONG,             // assuming that function = 0
                                                            &pcc,
                                                            0,
                                                            PCI_COMMON_HDR_LENGTH);
                            if (ulBytes == PCI_COMMON_HDR_LENGTH)
                            {
                                // just prepare common config that VPGetAccessRanges can read the physical aperture
                                pcc.Command &=  ~PCI_ENABLE_IO_SPACE;

                                ulBytes = VideoPortSetBusData(pHDE, 
                                                            PCIConfiguration,
                                                            psnSlot.u.AsULONG,             // assuming that function = 0
                                                            &pcc,
                                                            0,
                                                            PCI_COMMON_HDR_LENGTH);
                                ASSERT(ulBytes == PCI_COMMON_HDR_LENGTH);
                            }
                        }
                    }
                }
                else
                {
                    status = NO_ERROR;
                }
                break;

                // no need for default branch
            }
        }
    }
EXIT:
    return status;
}


