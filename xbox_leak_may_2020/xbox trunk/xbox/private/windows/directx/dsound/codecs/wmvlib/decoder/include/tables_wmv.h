#ifndef __TABLES_WMV_H_
#define __TABLES_WMV_H_

#include "wmvdec_api.h"

#ifdef __cplusplus
extern "C" {
#endif
extern const U8_WMV g_rgiClapTabDecWMV [1024];
#ifdef __cplusplus
}
#endif

#ifdef  _WMV_TARGET_X86_
#ifdef _XBOX
#define g_bSupportMMX_WMV ((Bool_WMV)TRUE)
#else
extern Bool_WMV g_bSupportMMX_WMV;
#endif
#endif //  _WMV_TARGET_X86_

// For encoder and decoder;
extern I8_WMV* gmvUVtable4;
extern I8_WMV gmvUVtableNew[128];
extern U8_WMV grgiZigzagInv [BLOCK_SQUARE_SIZE+1];
extern U8_WMV  grgiHorizontalZigzagInv [BLOCK_SQUARE_SIZE];
extern U8_WMV  grgiVerticalZigzagInv [BLOCK_SQUARE_SIZE];
extern U8_WMV  grgiZigzagInvRotated [BLOCK_SQUARE_SIZE];

extern U8_WMV grgiZigzagInv_NEW [BLOCK_SQUARE_SIZE+1];
extern U8_WMV grgiZigzagInv_NEW_I [BLOCK_SQUARE_SIZE+1];
extern U8_WMV grgiHorizontalZigzagInv_NEW [BLOCK_SQUARE_SIZE];
extern U8_WMV grgiVerticalZigzagInv_NEW [BLOCK_SQUARE_SIZE];
extern U8_WMV grgiZigzagInvRotated_NEW [BLOCK_SQUARE_SIZE];
extern U8_WMV grgiZigzagInvRotated_NEW_I [BLOCK_SQUARE_SIZE+1];
extern U8_WMV grgiHorizontalZigzagInvRotated_NEW [BLOCK_SQUARE_SIZE];
extern U8_WMV grgiVerticalZigzagInvRotated_NEW [BLOCK_SQUARE_SIZE];

extern U8_WMV grgi8x4ZigzagInv[33];
extern U8_WMV grgi4x8ZigzagInv[33];
extern U8_WMV grgi8x4ZigzagInvRotated[33];
extern U8_WMV grgi4x8ZigzagInvRotated[33];

/// Tabls for HighMotion
extern U8_WMV sm_rgIfNotLastNumOfLevelAtRunIntraY_HghMt [31]; 
extern U8_WMV sm_rgIfLastNumOfLevelAtRunIntraY_HghMt [38]; 
extern U8_WMV sm_rgIfNotLastNumOfRunAtLevelIntraY_HghMt [20];
extern U8_WMV sm_rgIfLastNumOfRunAtLevelIntraY_HghMt [7];
extern U8_WMV sm_rgIfLastNumOfLevelAtRun_HghMt [37];
extern U8_WMV sm_rgIfNotLastNumOfLevelAtRun_HghMt [27];
extern U8_WMV sm_rgIfNotLastNumOfRunAtLevel_HghMt [24];
extern U8_WMV sm_rgIfLastNumOfRunAtLevel_HghMt [10];

/// Tabls for Talking
extern U8_WMV sm_rgIfNotLastNumOfLevelAtRunIntraY_Talking [21]; 
extern U8_WMV sm_rgIfLastNumOfLevelAtRunIntraY_Talking [27]; 
extern U8_WMV sm_rgIfNotLastNumOfRunAtLevelIntraY_Talking [17];
extern U8_WMV sm_rgIfLastNumOfRunAtLevelIntraY_Talking [5];
extern U8_WMV sm_rgIfLastNumOfLevelAtRun_Talking [44];
extern U8_WMV sm_rgIfNotLastNumOfLevelAtRun_Talking [30];
extern U8_WMV sm_rgIfNotLastNumOfRunAtLevel_Talking [15];
extern U8_WMV sm_rgIfLastNumOfRunAtLevel_Talking [6];

/// Tabls for MPEG4
extern U8_WMV sm_rgIfNotLastNumOfLevelAtRunIntraY_MPEG4 [15]; 
extern U8_WMV sm_rgIfLastNumOfLevelAtRunIntraY_MPEG4 [21]; 
extern U8_WMV sm_rgIfNotLastNumOfRunAtLevelIntraY_MPEG4 [28];
extern U8_WMV sm_rgIfLastNumOfRunAtLevelIntraY_MPEG4 [9];
extern U8_WMV sm_rgIfLastNumOfLevelAtRun_MPEG4 [41];
extern U8_WMV sm_rgIfNotLastNumOfLevelAtRun_MPEG4 [27];
extern U8_WMV sm_rgIfNotLastNumOfRunAtLevel_MPEG4 [13];
extern U8_WMV sm_rgIfLastNumOfRunAtLevel_MPEG4 [4];

// g_rgDitherMap
// all DCT, MV tables

#endif
