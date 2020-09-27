/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        vciPlus.h
*
* Purpose:     Definitions and structures for VCI Plus
*
* Author/Date: John Puterbaugh, Ilan Berci & Epiphany Vera
*******************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/vci/vciPlus.h_v   1.1   13 Feb 1998 10:42:38   weiwang  $
******************************************************************************/
#ifndef VCIPLUS_H
#define VCIPLUS_H


#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __COMPATIBILITY_FUNCTION
#define __COMPATIBILITY_FUNCTION

#define COMPATIBILITY_VFONT   (1)
#define COMPATIBILITY_VAD_AGC (1<<1)
#define COMPATIBILITY_PLUS    (1<<2) /* only needed for backward compatibility */

typedef unsigned short (COMPATIBILITY_FUNCTION)(unsigned short VemPlugin,unsigned long *pVemCodID);
#endif /* __COMPATIBILITY_FUNCTION */

#define VCI_CODEC_HAS_WARPING             (1)
#define VCI_CODEC_HAS_SILENCE_INJECTION   (1 << 1) 
#define VCI_CODEC_HAS_PITCH_SHIFT         (1 << 2) /* not supported anymore */   
#define VCI_CODEC_HAS_GET_ENERGY          (1 << 3)
#define VCI_CODEC_HAS_FRAME_TRAVERSAL     (1 << 4)
#define VCI_PRIME_FLUSH                   (1 << 5)
#define VCI_CODEC_IS_SCALEABLE            (1 << 6)
#define VCI_STREAM_MANIPULATION           (1 << 7)

typedef struct tagVCI_PLUS_INFO_BLOCK {
   unsigned short wPlusFlags;
   float dwMinWarp; 
   float dwMaxWarp;

} VCI_PLUS_INFO_BLOCK;

#ifdef __cplusplus
}
#endif

#endif /* VCIPLUS_H */
