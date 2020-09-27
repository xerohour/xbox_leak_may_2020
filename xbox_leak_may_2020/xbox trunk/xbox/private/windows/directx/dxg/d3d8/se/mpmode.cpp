/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mpmode.cpp
 *  Content:    Mode set code.
 *
 ***************************************************************************/

#include "precomp.hpp"

// Optimize this module for size (favor code space)
 
#pragma optimize("s", on)

#ifdef STARTUPANIMATION
namespace D3DK
#else
namespace D3D
#endif
{

// !!! Do not use this !!!
//
// This variable only exists so its address can be exported for the
// TV encoder test.  Do not use it directly.  Call GetDisplayType
// instead.
//
extern "C" extern DWORD D3D___AvInfo;

//-----------------------------------------------------------------------------
// Table that maps av packs, regions and sizes to a display mode.
//
D3DCONST DISPLAYMODE g_DisplayModes[] =
{
    // NOTE: This table is searched from the front to back with the
    //   first match being taken.  A zero entry means that all possible
    //   values will match except for flags.

    // 
    // US/Canada/Mexico
    //

    { AV_PACK_VGA             | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_VGA                    },
    { AV_PACK_VGA             | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_VGA                    },
    { AV_PACK_VGA             | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                      1280,  720,  AV_MODE_1280x720_TO_VGA                   },
    { AV_PACK_VGA             | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                      1920, 1080,  AV_MODE_1920x1080_TO_VGA                  },

    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | AV_FLAGS_HDTV_480p,      720,  480,  AV_MODE_720x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480p,      720,  480,  AV_MODE_720x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | 0                   | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_FPAR_TO_480P              },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_FPAR_TO_480P              },

    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_720p,      1280, 720,  AV_MODE_1280x720P_TO_720P                 },

    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_TO_NTSC_YPrPb_16x9        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | AV_FLAGS_HDTV_480i,      720,  480,  AV_MODE_720x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      720,  480,  AV_MODE_720x480_TO_NTSC_YPrPb_16x9        },

    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_TO_NTSC_YPrPb_16x9        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | AV_FLAGS_HDTV_480i,      720,  240,  AV_MODE_720x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      720,  240,  AV_MODE_720x480_TO_NTSC_YPrPb_16x9        },
                                                                                                                                  
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb_16x9   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb_16x9   },

    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_1080i,     1920, 1080, AV_MODE_1920x1080I_TO_1080I               },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_1080i,     1920,  540, AV_MODE_1920x1080I_TO_1080I               },

    // NTSC RGB

    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_M_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_M_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_M_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_M_RGB_16x9        },

    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_M_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_M_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_M_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_M_RGB_16x9        },

    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_M_RGB        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_M_RGB_16x9   },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_M_RGB        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_M_RGB_16x9   },

    // All other AV packs are the same.

    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_M_YC              },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_M_YC_16x9         },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_M_YC              },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_M_YC_16x9         },

    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_M_YC              },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_M_YC_16x9         },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_M_YC              },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_M_YC_16x9         },

    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_M_YC         },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_M_YC_16x9    },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_M_YC         },
    { 0                       | AV_STANDARD_NTSC_M | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_M_YC_16x9    },

    // 
    // Japan
    //

    { AV_PACK_VGA             | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_VGA                    },
    { AV_PACK_VGA             | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_VGA                    },
    { AV_PACK_VGA             | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                      1280,  720,  AV_MODE_1280x720_TO_VGA                   },
    { AV_PACK_VGA             | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                      1920, 1080,  AV_MODE_1920x1080_TO_VGA                  },
                                                                                                                   
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | AV_FLAGS_HDTV_480p,      720,  480,  AV_MODE_720x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480p,      720,  480,  AV_MODE_720x480_TO_480P                   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | 0                   | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_FPAR_TO_480P              },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480p,      640,  480,  AV_MODE_640x480_FPAR_TO_480P              },
                                                                                                                   
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | 0                   | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_720p,      1280, 720,  AV_MODE_1280x720P_TO_720P                 },
                                                                                                                   
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_TO_NTSC_YPrPb_16x9        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | AV_FLAGS_HDTV_480i,      720,  480,  AV_MODE_720x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      720,  480,  AV_MODE_720x480_TO_NTSC_YPrPb_16x9        },
                                                                                                                   
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_TO_NTSC_YPrPb_16x9        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | AV_FLAGS_HDTV_480i,      720,  240,  AV_MODE_720x480_TO_NTSC_YPrPb             },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_480i,      720,  240,  AV_MODE_720x480_TO_NTSC_YPrPb_16x9        },
                                                                                                                                  
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb_16x9   },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb        },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | AV_FLAGS_HDTV_480i,      640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_YPrPb_16x9   },

    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_1080i,     1920, 1080, AV_MODE_1920x1080I_TO_1080I               },
    { AV_PACK_HDTV            | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | AV_FLAGS_HDTV_1080i,     1920,  540, AV_MODE_1920x1080I_TO_1080I               },
                                                                                                                   
    // NTSC RGB                                                                                                    
                                                                                                                   
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_J_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_J_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_J_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_J_RGB_16x9        },
                                                                                                                   
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_J_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_J_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_J_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_J_RGB_16x9        },
                                                                                                                   
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_J_RGB        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_J_RGB_16x9   },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_J_RGB        },
    { AV_PACK_SCART           | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_J_RGB_16x9   },

    // All other AV packs are the same.                                                                            
                                                                                                                   
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_J_YC              },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_NTSC_J_YC_16x9         },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_J_YC              },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_NTSC_J_YC_16x9         },
                                                                                                                   
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_J_YC              },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_NTSC_J_YC_16x9         },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_J_YC              },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_NTSC_J_YC_16x9         },

    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_J_YC         },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_NTSC_J_YC_16x9    },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_J_YC         },
    { 0                       | AV_STANDARD_NTSC_J | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_NTSC_J_YC_16x9    },

    //
    // Europe
    //

    { AV_PACK_VGA             | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_VGA                    },
    { AV_PACK_VGA             | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_VGA                    },
    { AV_PACK_VGA             | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                      1280,  720,  AV_MODE_1280x720_TO_VGA                   },
    { AV_PACK_VGA             | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | 0                   | 0                   | 0                 | 0,                      1920, 1080,  AV_MODE_1920x1080_TO_VGA                  },
                     
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_I_YPrPb            },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_I_YPrPb_16x9       },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_I_YPrPb            },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_I_YPrPb_16x9       },

    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  576,  AV_MODE_640x576_TO_PAL_I_YPrPb            },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  576,  AV_MODE_640x576_TO_PAL_I_YPrPb_16x9       },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  576,  AV_MODE_720x576_TO_PAL_I_YPrPb            },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  576,  AV_MODE_720x576_TO_PAL_I_YPrPb_16x9       },
                                                                                                                   
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  288,  AV_MODE_640x576_TO_PAL_I_YPrPb            },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  288,  AV_MODE_640x576_TO_PAL_I_YPrPb_16x9       },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  288,  AV_MODE_720x576_TO_PAL_I_YPrPb            },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  288,  AV_MODE_720x576_TO_PAL_I_YPrPb_16x9       },

    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_I_YPrPb       },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_I_YPrPb_16x9  },    

    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  576,  AV_MODE_640x576_FPAR_TO_PAL_I_YPrPb       },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  576,  AV_MODE_640x576_FPAR_TO_PAL_I_YPrPb_16x9  },    
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  288,  AV_MODE_640x576_FPAR_TO_PAL_I_YPrPb       },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  288,  AV_MODE_640x576_FPAR_TO_PAL_I_YPrPb_16x9  },    

    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_60_YPrPb           },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_60_YPrPb_16x9      },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_60_YPrPb           },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_60_YPrPb_16x9      },
                                                                                                                   
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_PAL_60_YPrPb           },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_PAL_60_YPrPb_16x9      },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_PAL_60_YPrPb           },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_PAL_60_YPrPb_16x9      },

    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_60_YPrPb      },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_60_YPrPb_16x9 },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_PAL_60_YPrPb      },
    { AV_PACK_HDTV            | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_PAL_60_YPrPb_16x9 },

    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_I_RGB              },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_I_RGB_16x9         },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_I_RGB              },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_I_RGB_16x9         },

    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  576,  AV_MODE_640x576_TO_PAL_I_RGB              },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  576,  AV_MODE_640x576_TO_PAL_I_RGB_16x9         },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  576,  AV_MODE_720x576_TO_PAL_I_RGB              },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  576,  AV_MODE_720x576_TO_PAL_I_RGB_16x9         },
                                                                                                                   
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  288,  AV_MODE_640x576_TO_PAL_I_RGB              },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  288,  AV_MODE_640x576_TO_PAL_I_RGB_16x9         },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  288,  AV_MODE_720x576_TO_PAL_I_RGB              },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  288,  AV_MODE_720x576_TO_PAL_I_RGB_16x9         },

    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_I_RGB         },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_I_RGB_16x9    },    

    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  576,  AV_MODE_640x576_FPAR_TO_PAL_I_RGB         },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  576,  AV_MODE_640x576_FPAR_TO_PAL_I_RGB_16x9    },    
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  288,  AV_MODE_640x576_FPAR_TO_PAL_I_RGB         },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  288,  AV_MODE_640x576_FPAR_TO_PAL_I_RGB_16x9    },    

    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_60_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_60_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_60_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_60_RGB_16x9        },
                                                                                                                   
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_PAL_60_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_PAL_60_RGB_16x9        },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_PAL_60_RGB             },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_PAL_60_RGB_16x9        },

    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_60_RGB        },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_60_RGB_16x9   },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_PAL_60_RGB        },
    { AV_PACK_SCART           | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_PAL_60_RGB_16x9   },

    // All other AV packs are the same.                                                                           

    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_I_YC               },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_I_YC_16x9          },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_I_YC               },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_I_YC_16x9          },

    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  576,  AV_MODE_640x576_TO_PAL_I_YC               },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  576,  AV_MODE_640x576_TO_PAL_I_YC_16x9          },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  576,  AV_MODE_720x576_TO_PAL_I_YC               },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  576,  AV_MODE_720x576_TO_PAL_I_YC_16x9          },   

    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  288,  AV_MODE_640x576_TO_PAL_I_YC               },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  288,  AV_MODE_640x576_TO_PAL_I_YC_16x9          },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  288,  AV_MODE_720x576_TO_PAL_I_YC               },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  288,  AV_MODE_720x576_TO_PAL_I_YC_16x9          },

    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_I_YC          },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_I_YC_16x9     },

    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  576,  AV_MODE_640x576_FPAR_TO_PAL_I_YC          },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  576,  AV_MODE_640x576_FPAR_TO_PAL_I_YC_16x9     },    
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  288,  AV_MODE_640x576_FPAR_TO_PAL_I_YC          },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_50Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  288,  AV_MODE_640x576_FPAR_TO_PAL_I_YC_16x9     },    

    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_60_YC              },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  480,  AV_MODE_640x480_TO_PAL_60_YC_16x9         },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_60_YC              },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  480,  AV_MODE_720x480_TO_PAL_60_YC_16x9         },
                                                                                                                                                                                                                    
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_PAL_60_YC              },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       640,  240,  AV_MODE_640x480_TO_PAL_60_YC_16x9         },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_PAL_60_YC              },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | 0                 | 0,                       720,  240,  AV_MODE_720x480_TO_PAL_60_YC_16x9         },

    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_60_YC         },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_INTERLACED | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  480,  AV_MODE_640x480_FPAR_TO_PAL_60_YC_16x9    },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | 0                   | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_PAL_60_YC         },
    { 0                       | AV_STANDARD_PAL_I  | AV_FLAGS_60Hz | AV_FLAGS_FIELD      | AV_FLAGS_WIDESCREEN | AV_FLAGS_10x11PAR | 0,                       640,  240,  AV_MODE_640x480_FPAR_TO_PAL_60_YC_16x9    },

    // Runoff.
                                                                                          
    { 0xFFFFFFFF,                                                                                                                                             0,    0,    0                                         },
};

D3DCONST DWORD g_DisplayModeCount = sizeof(g_DisplayModes) / sizeof(DISPLAYMODE);

//-----------------------------------------------------------------------------
// Queries the hardware and user settings for the current AvPack, region
// and user display settings.
//
DWORD 
CMiniport::GetPresentFlagsFromAvInfo(
    DWORD AvInfo
    )
{
    DWORD Flags = 0;

    if (AvInfo & AV_FLAGS_WIDESCREEN)
    {
        Flags |= D3DPRESENTFLAG_WIDESCREEN;
    }
    
    if (AvInfo & AV_FLAGS_INTERLACED)
    {
        Flags |= D3DPRESENTFLAG_INTERLACED;
    }
    else if (AvInfo & AV_FLAGS_FIELD)
    {
        // Field implies interlaced.
        Flags |= D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_FIELD;
    }
    else
    {
        Flags |= D3DPRESENTFLAG_PROGRESSIVE;
    }

    if (AvInfo & AV_FLAGS_10x11PAR)
    {
        Flags |= D3DPRESENTFLAG_10X11PIXELASPECTRATIO;
    }

    return Flags;
}

//-----------------------------------------------------------------------------
// Queries the hardware and user settings for the current AvPack, region
// and user display settings.
//
DWORD
CMiniport::GetDisplayCapabilities()
{
    if (!D3D__AvInfo)
    {
        AvSendTVEncoderOption(NULL, 
                              AV_QUERY_AV_CAPABILITIES, 
                              0, 
                              &D3D__AvInfo);
    }

    return D3D__AvInfo;
}

//-----------------------------------------------------------------------------
// Finds the beginning of a section of the table for the current 
// AV pack and region.
//
const DISPLAYMODE *GetTableForCurrentAvInfo()
{
    DWORD SystemAvInfo = CMiniport::GetDisplayCapabilities();

    ULONG AvRegion      = SystemAvInfo & AV_STANDARD_MASK;
    ULONG AvPack        = SystemAvInfo & AV_PACK_MASK;

    //
    // Search the display mode table.
    //

    const DISPLAYMODE *pMode;

    DWORD i, c = g_DisplayModeCount;

    // Whiz through the table looking for the correct region.
    for (i = 0; i < c; i++)
    {
        pMode = &g_DisplayModes[i];

        if ((pMode->AvInfo & AV_STANDARD_MASK) == AvRegion)
        {
            break;
        }
    }

    ASSERT(i != c);

    // Whiz through the region looking for the right pack or 
    // a generic AV pack.
    //
    for (; i < c; i++)
    {
        pMode = &g_DisplayModes[i];

        DWORD ModeAvPack = pMode->AvInfo & AV_PACK_MASK;

        if (!ModeAvPack || ModeAvPack == AvPack)
        {
            break;
        }
    }

    ASSERT(i != c);
    ASSERT((pMode->AvInfo & AV_STANDARD_MASK) == AvRegion);

    return pMode;
}


//-----------------------------------------------------------------------------
// Tells the kernel to set the video mode according to the desired size, 
// format along with the current AV pack and region.
//
void
CMiniport::SetVideoMode(
    ULONG Width,
    ULONG Height,
    ULONG Refresh,
    ULONG Flags,
    D3DFORMAT Format,
    ULONG PresentationInterval,
    ULONG Pitch
    )
{
    BYTE* RegisterBase = (BYTE*)m_RegisterBase;

    Format = MapToLinearD3DFORMAT(Format);
    ULONG Depth = VideoBitsPerPixelOfD3DFORMAT(Format);    

    //SRC: NV4SetMode

    const DISPLAYMODE *pMode = GetTableForCurrentAvInfo();

    DWORD SystemAvInfo = CMiniport::GetDisplayCapabilities();

    ULONG ModeAvPack    = pMode->AvInfo & AV_PACK_MASK;
    ULONG AvPack        = SystemAvInfo & AV_PACK_MASK;
    ULONG AvStandard    = SystemAvInfo & AV_STANDARD_MASK;
    ULONG AvRefresh     = SystemAvInfo & AV_REFRESH_MASK;
    ULONG AvHDTVModes   = 0;
    ULONG AvField       = Flags & (D3DPRESENTFLAG_INTERLACED | D3DPRESENTFLAG_PROGRESSIVE);

    // Field implies interlaced.
    if (Flags & D3DPRESENTFLAG_FIELD)
    {
        AvField &= ~D3DPRESENTFLAG_INTERLACED;
    }

    // Get the supported HDTV modes.
    if (AvPack == AV_PACK_HDTV)
    {
        AvHDTVModes     = SystemAvInfo & AV_HDTV_MODE_MASK;
    }

    // Override the refresh if it was specified explicitly.
    switch(Refresh)
    {
    case 0:
        break;

    case 50:
        AvRefresh = AV_FLAGS_50Hz;
        break;

    case 60:
        AvRefresh = AV_FLAGS_60Hz;
        break;

    default:
        NODEFAULT("SetVideoMode - Bad refresh rate\n");
    }

    // Look for the specific mode in this area.
    DWORD DisplayMode = 0;

    if (AvPack != AV_PACK_NONE)
    {
        for (;; pMode++)
        {
            // Did we leave this AV pack?
            if ((pMode->AvInfo & AV_PACK_MASK) != ModeAvPack)
            {
                break;
            }

            // Reject any HTDV modes that we do not support.
            if (AvPack == AV_PACK_HDTV 
                && !(pMode->AvInfo & AvHDTVModes)
                && (pMode->AvInfo & AV_HDTV_MODE_MASK) != 0)
            {
                continue;
            }

            // Reject anything of the wrong size.
            if (pMode->Width != Width || pMode->Height != Height)
            {
                continue;
            }

            // Reject the wrong widescreenness.
            if (!(pMode->AvInfo & AV_FLAGS_WIDESCREEN) != !(Flags & D3DPRESENTFLAG_WIDESCREEN))
            {
                continue;
            }

            // Reject the wrong fieldness.
            if (!(pMode->AvInfo & AV_FLAGS_FIELD) != !(Flags & D3DPRESENTFLAG_FIELD))
            {
                continue;
            }

            // Reject the wrong funky pixel aspect rationess.
            if (!(pMode->AvInfo & AV_FLAGS_10x11PAR) != !(Flags & D3DPRESENTFLAG_10X11PIXELASPECTRATIO))
            {
                continue;
            }

            // Reject unwanted refresh rates.
            if (!(pMode->AvInfo & AvRefresh))
            {
                continue;
            }

            // Reject unwanted interlace modes.
            if (AvField
                && !((AvField & D3DPRESENTFLAG_INTERLACED) && (pMode->AvInfo & AV_FLAGS_INTERLACED))
                && !((AvField & D3DPRESENTFLAG_PROGRESSIVE) && !(pMode->AvInfo & AV_FLAGS_INTERLACED)))
            {
                continue;
            }

            DisplayMode = pMode->DisplayMode;
            break;
        }
    }

#ifndef STARTUPANIMATION

    if (DisplayMode == 0 && AvPack != AV_PACK_NONE)
    {
        DXGRIP("SetVideoMode - Unsupported display mode.");
    }

#endif

    // Check to make sure that this setup supports widescreen/refresh.
    if (DisplayMode)
    {
        if (!(SystemAvInfo & AvRefresh))
        {
            DXGRIP(("SetVideoMode - Current AV setup does not support the requested refresh rate."));

            DisplayMode = 0;
        }

        // Does this system support widescreen?
        if ((AvPack != AV_PACK_HDTV || !(pMode->AvInfo & (AV_FLAGS_HDTV_720p | AV_FLAGS_HDTV_1080i)))
            && (Flags & D3DPRESENTFLAG_WIDESCREEN)
            && !(SystemAvInfo & AV_FLAGS_WIDESCREEN))
        {
            DXGRIP(("SetVideoMode - Current system does not support widescreen mode."));
        }
    }

    //
    // The mode change will happen on the next present...save the mode information
    // that will be set at that time.
    //

    // Save the new mode and format
    m_DisplayMode = DisplayMode;
    m_Format = Format;
    m_SurfacePitch = Pitch;

    m_VBlanksBetweenFlips = (PresentationInterval & 3);
    m_OrImmediate = FALSE;
    if (PresentationInterval != D3DPRESENT_INTERVAL_IMMEDIATE)
    {
        if (m_VBlanksBetweenFlips == 0)
            m_VBlanksBetweenFlips = 1;

        if (PresentationInterval & D3DPRESENT_INTERVAL_IMMEDIATE)
            m_OrImmediate = TRUE;
    }

    // Make sure that the gamma gets reset.
    m_GammaUpdated[m_GammaCurrentIndex] = TRUE;

    // Save the current AV mode.
    m_CurrentAvInfo = pMode->AvInfo;

    // Tell present to enable the display.
    m_FirstFlip = TRUE;
}

} // namespace D3D

