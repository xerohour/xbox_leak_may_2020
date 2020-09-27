/*************************************************************************

Copyright (C) Microsoft Corporation, 1998 - 1999

Module Name:

	MotionCompDec.h

Abstract:

	Decoder motion compensation routines 

Author:

	Ming-Chieh Lee (mingcl@microsoft.com) July-1998
	Joseph Wu (sjwu@microsoft.com) July-1998
    Chuang Gu (chuanggu@microsoft.com)

Revision History:

*************************************************************************/
#include "wmvdec_api.h"

#ifndef _MOTIONCOMPDEC_H_
#define _MOTIONCOMPDEC_H_

#define MOTION_COMP_ARGS 	        struct tagWMVDecInternalMember *pWMVDec, \
                                    PixelC*              ppxlcPredMB,   /* Predicted Block               */   \
	                                const PixelC*        ppxlcRef,      /* Refferance Block              */   \
                                    I32_WMV                  iWidthFrm,    /* predicted block width         */   \
                                    Bool_WMV                 bInterpolateX, /* need horizantal interpolation */   \
                                    Bool_WMV                 bInterpolateY, /* need vertical interpolation   */\
                                    I32_WMV                  iMixedPelMV

#define MOTION_COMP_ARGS_MC_VEC 	PixelC*              ppxlcPredMB,   /* Predicted Block               */   \
	                                const PixelC*        ppxlcRef,      /* Refferance Block              */   \
                                    I32_WMV                  iWidthPred,    /* predicted block width         */   \
                                    I32_WMV                  iWidthRef     /* referance block width         */  


#define MOTION_COMP_ADD_ERROR_ARGS  struct tagWMVDecInternalMember *pWMVDec, \
                                    PixelC __huge*       ppxlcCurrQMB,      \
                                    const Buffer __huge* ppxliErrorBuf,     \
                                    const PixelC __huge* ppxlcRef,          \
                                    I32_WMV                  iWidthFrm,         \
                                    Bool_WMV                 bXEven,            \
                                    Bool_WMV                 bYEven,    \
                                    I32_WMV iMixedPelMV	


#define MOTION_COMP_ADD_ERROR_ARGS_MC_VEC  PixelC __huge*       ppxlcCurrQMB,      \
                                    const PixelC __huge* ppxlcRef,          \
                                    const Buffer __huge* ppxliErrorBuf,     \
                                    I32_WMV                  iWidthFrm    

#define MOTION_COMP_ZERO_ARGS       PixelC __huge* ppxliCurrQYMB, \
	                                PixelC __huge* ppxliCurrQUMB, \
	                                PixelC __huge* ppxliCurrQVMB, \
	                                const PixelC __huge* ppxliRefYMB, \
	                                const PixelC __huge* ppxliRefUMB, \
	                                const PixelC __huge* ppxliRefVMB, \
	                                I32_WMV iWidthY, \
	                                I32_WMV iWidthUV

////////////////// External (Exported) //////////

Void_WMV g_InitMotionComp (U8_WMV *rgiClippingTable = NULL_WMV, Bool_WMV bAlwaysUseCVersion = FALSE);

#ifndef OPT_ZEROMOTION_ARM
Void_WMV g_MotionCompZeroMotion_WMV (MOTION_COMP_ZERO_ARGS);
#else
extern "C" Void_WMV g_MotionCompZeroMotion_WMV (MOTION_COMP_ZERO_ARGS);
#endif //OPT_ZEROMOTION_ARM
Void_WMV g_MotionCompAndAddError (MOTION_COMP_ADD_ERROR_ARGS);
Void_WMV g_MotionComp (MOTION_COMP_ARGS);
Void_WMV g_MotionCompAndAddErrorRndCtrl (MOTION_COMP_ADD_ERROR_ARGS);
Void_WMV g_MotionCompRndCtrl (MOTION_COMP_ARGS);

#ifdef OPT_MC_VEC
extern Void_WMV (* g_pMCTable[8])(MOTION_COMP_ARGS_MC_VEC);
extern Void_WMV (* g_pMCAndAddErrorTable[8])(MOTION_COMP_ADD_ERROR_ARGS_MC_VEC);
#endif

#if defined(_WMV_TARGET_X86_)
Void_WMV g_MotionCompZeroMotion_MMX_WMV (MOTION_COMP_ZERO_ARGS);
Void_WMV g_MotionCompAndAddError_MMX (MOTION_COMP_ADD_ERROR_ARGS);
Void_WMV g_MotionComp_MMX (MOTION_COMP_ARGS);
Void_WMV g_MotionComp_KNI (MOTION_COMP_ARGS);
Void_WMV g_MotionCompAndAddErrorRndCtrl_MMX (MOTION_COMP_ADD_ERROR_ARGS);
Void_WMV g_MotionCompRndCtrl_MMX (MOTION_COMP_ARGS);
#endif

#endif 
