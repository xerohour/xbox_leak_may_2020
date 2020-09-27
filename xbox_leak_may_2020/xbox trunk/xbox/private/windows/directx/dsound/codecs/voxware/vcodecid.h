/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1998, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       vCodecID.h      (Voxware codec IDs)                                                        
*                                 
* Dependencies:                                          
* 
* $Header:   P:/r_and_d/archives/common/vci/vCodecID.h_v   1.8   17 Nov 1998 09:35:56   epiphanyv  $
*
*******************************************************************************/

#ifndef _VCODECID_H_
#define _VCODECID_H_


/***************************/
/**    Codec Class IDs    **/
/************************************************/
/*  Each codec class must have a unique ID      */
/*  number which is communicated to the VCT     */
/*  and used to identify a particular bitstream */
/*  If the bitstream changes, the "new" codec   */
/*  must get a new class ID.  The old codec     */
/*  will not change.                            */
/************************************************/
/* !!!! CODEC CLASS ID'S CAN NEVER CHANGE !!!!  */
/************************************************/


/* Low Memory Embedded Codecs */
#define VOXWARE_CODEC_LME_16               0x07000001L  /* LME16            16khz */
#define VOXWARE_CODEC_LME_24               0x07000002L  /* LME24            16khz */
#define VOXWARE_CODEC_LME_32               0x07000004L  /* LME32            16khz */


/* Wideband Codecs */
/* HF64 */
#define VOXWARE_CODEC_HF_64                 0x06000001L  /* HF64            16khz */

/* Low Complexity Codecs */
/* LC34 */
#define VOXWARE_CODEC_LC_34                 0x05000001L  /* LC34             8khz */

#define VOXWARE_CODEC_LC_16                 0x05000002L  /* LC16             8khz */
#define VOXWARE_CODEC_LC_24                 0x05000004L  /* LC24            11khz */
#define VOXWARE_CODEC_LC_32                 0x05000008L  /* LC32            16khz */

/* RT24 */
#define VOXWARE_CODEC_RT_8K                 0x01000001L  /* RT24     2,400       8khz */

/* RT29 Bitstream Compatible */
#define VOXWARE_CODEC_RT_8K_HQ29            0x01000010L  /* RT29     2,977       8khz */

/* RT28   (was RT29 Plus) */
#define VOXWARE_CODEC_RT_8K_28              0x01000011L  /* RT28     2.8kbps     8khz    VoxChat */

/* RT24 HQ */
#define VOXWARE_CODEC_RT_8K_HQ24            0x01000004L  /* RT24HQ   not released with VDK */

/* RT - Variable Rate codecs */
#define VOXWARE_CODEC_RT_8K_VR12            0x01000012L  /* VR12     not released with VDK */
#define VOXWARE_CODEC_RT_8K_VR15            0x01000013L  /* VR15     not released with VDK */

/* ULC15 -- Variable Rate codecs */
#define VOXWARE_CODEC_RT_8K_ULC15           0x01000014L  /* ULC15    not released with VDK */


/* RT24 UQ */
#define VOXWARE_CODEC_RT_8K_UQ              0x0100000CL  /* RT24UQ   not released with VDK */

/* Audio Codecs v1.0  (shipped with VDK v1.1.8) */
#define VOXWARE_CODEC_AC_8K                 0x08000001L  /* AC8       8,000 bps  8kHz v1.0 bitstream */
#define VOXWARE_CODEC_AC_11K                0x08000002L  /* AC8       8,000 bps  8kHz v1.0 bitstream */
#define VOXWARE_CODEC_AC_16K                0x08000003L  /* AC8       8,000 bps  8kHz v1.0 bitstream */
#define VOXWARE_CODEC_AC_22K                0x08000004L  /* AC8       8,000 bps  8kHz v1.0 bitstream */

/* Audio Codecs v2.0 (shipped with VCT v1.5.0) */
#define VOXWARE_CODEC_AC_8K_V2              0x02000001L  /* AC8       8,000 bps  8kHz  v2.0 bitstream */
#define VOXWARE_CODEC_AC_11K_V2             0x02000002L  /* AC11     10,000 bps  11kHz v2.0 bitstream */
#define VOXWARE_CODEC_AC_16K_V2             0x02000003L  /* AC16     16,000 bps  16kHz v2.0 bitstream */
#define VOXWARE_CODEC_AC_22K_V2             0x02000004L  /* AC22     24,000 bps  22kHz v2.0 bitstream */
#define VOXWARE_CODEC_AC_32K_V2             0x02000005L  /* AC22     24,000 bps  22kHz v2.0 bitstream */
#define VOXWARE_CODEC_AC_40K_V2             0x02000006L  /* AC22     24,000 bps  22kHz v2.0 bitstream */
#define VOXWARE_CODEC_AC_48K_V2             0x02000007L  /* AC22     24,000 bps  22kHz v2.0 bitstream */

/* Audio Codecs Stereo (shipped with VCT v1.5.0) */
#define VOXWARE_CODEC_AC_8K_STEREO          0x04000001L  /* AC8       8,000 bps  8kHz    Stereo */
#define VOXWARE_CODEC_AC_11K_STEREO         0x04000002L  /* AC11     10,000 bps  11kHz   Stereo */
#define VOXWARE_CODEC_AC_16K_STEREO         0x04000003L  /* AC16     16,000 bps  16kHz   Stereo */
#define VOXWARE_CODEC_AC_22K_STEREO         0x04000004L  /* AC22     24,000 bps  22kHz   Stereo */
#define VOXWARE_CODEC_AC_64K_STEREO         0x04000005L  /* AC22     24,000 bps  22kHz   Stereo */
#define VOXWARE_CODEC_AC_80K_STEREO         0x04000006L  /* AC22     24,000 bps  22kHz   Stereo */
#define VOXWARE_CODEC_AC_96K_STEREO         0x04000007L  /* AC22     24,000 bps  22kHz   Stereo */


/* Scaleable Codecs */
#define VOXWARE_CODEC_SC_8K_3               0x03000003L  /* SC3       3200 bps   8kHz */
#define VOXWARE_CODEC_SC_8K_6               0x03000006L  /* SC6       6400 bps   8kHz */
#define VOXWARE_CODEC_SC_16K_10             0x03000010L  /* SC10      Not Yet Designed  16 kHz */


/*****************************/
/**    Unique Codec ID's    **/
/***************************************************/
/*  Each codec must have a unique codec ID.        */
/*  Codec ID's are different from codec class ID's */
/*  If for example you have two RT24 codecs, one   */
/*  regular and one that's MMX enhanced then       */
/*  both codecs have the same 'codec class id',    */
/*  but they must both have unique codec id's.     */
/***************************************************/
/*      !!!! CODEC ID'S CAN NEVER CHANGE !!!!      */
/***************************************************/


/* Low Memory Embedded Codecs */
#define VOXWARE_CODEC_ID_LME_16              0x07000001L  /* LME16            16khz */
#define VOXWARE_CODEC_ID_LME_24              0x07000002L  /* LME24            16khz */
#define VOXWARE_CODEC_ID_LME_32              0x07000004L  /* LME32            16khz */


/* Wideband Codecs */
/* HF64 */
#define VOXWARE_CODEC_ID_HF_64              0x06000001L  /* HF64            16khz */

/* Low Complexity Codecs */
/* LC34 */
#define VOXWARE_CODEC_ID_LC_34              0x05000002L  /* LC34            8khz */

#define VOXWARE_CODEC_ID_LC_16              0x05000004L  /* LC16            16khz */
#define VOXWARE_CODEC_ID_LC_24              0x05000008L  /* LC24            11khz */
#define VOXWARE_CODEC_ID_LC_32              0x05000010L  /* LC32             8khz */

/* RT29 HQ Bitstream Compatible */
#define VOXWARE_CODEC_ID_RT_8K_HQ29         0x01000001L  /* class = VOXWARE_CODEC_RT_8K_HQ29 */
/* RT29 HQ Bitstream Compatible Low Delay */
#define VOXWARE_CODEC_ID_RT_8K_HQ29_LD      0x01000005L  /* class = VOXWARE_CODEC_RT_8K_HQ29 */


/* RT24 */
#define VOXWARE_CODEC_ID_RT_8K_24           0x01000002L  /* class = VOXWARE_CODEC_RT_8K */
/* RT24 Low Delay */
#define VOXWARE_CODEC_ID_RT_8K_24_LD        0x01000007L  /* class = VOXWARE_CODEC_RT_8K */


/* RT28 (was RT29 Plus) */
#define VOXWARE_CODEC_ID_RT_8K_28           0x01000003L  /* class = VOXWARE_CODEC_RT_8K_28 */
/* RT28 Low Delay */
#define VOXWARE_CODEC_ID_RT_8K_28_LD        0x01000006L  /* class = VOXWARE_CODEC_RT_8K_28 */

/* VR12 */
#define VOXWARE_CODEC_ID_RT_8K_VR12         0x01000020L  /* VR12     not released with VDK */

/* VR15 */
#define VOXWARE_CODEC_ID_RT_8K_VR15         0x01000021L  /* VR15     not released with VDK */

/* ULC15 */
#define VOXWARE_CODEC_ID_RT_8K_ULC15        0x01000022L  /* ULC15    not released with VDK */


/* Audio Codecs v1.0 */
#define VOXWARE_CODEC_ID_AC_8K              0x01000008L  /* class = VOXWARE_CODEC_AC_8K  */
#define VOXWARE_CODEC_ID_AC_11K             0x01000009L  /* class = VOXWARE_CODEC_AC_11K */
#define VOXWARE_CODEC_ID_AC_16K             0x0100000AL  /* class = VOXWARE_CODEC_AC_16K */
#define VOXWARE_CODEC_ID_AC_22K             0x0100000BL  /* class = VOXWARE_CODEC_AC_22K */


/* Audio Codecs v2.0 */
#define VOXWARE_CODEC_ID_AC_8K_V2           0x02000001L  /* class = VOXWARE_CODEC_AC_8K_V2  */
#define VOXWARE_CODEC_ID_AC_11K_V2          0x02000002L  /* class = VOXWARE_CODEC_AC_11K_V2 */
#define VOXWARE_CODEC_ID_AC_16K_V2          0x02000003L  /* class = VOXWARE_CODEC_AC_16K_V2 */
#define VOXWARE_CODEC_ID_AC_22K_V2          0x02000004L  /* class = VOXWARE_CODEC_AC_22K_V2 */
#define VOXWARE_CODEC_ID_AC_32K_V2          0x02000005L  /* class = VOXWARE_CODEC_AC_22K_V2 */
#define VOXWARE_CODEC_ID_AC_40K_V2          0x02000006L  /* class = VOXWARE_CODEC_AC_22K_V2 */
#define VOXWARE_CODEC_ID_AC_48K_V2          0x02000007L  /* class = VOXWARE_CODEC_AC_22K_V2 */


/* Audio Codecs Stereo */
#define VOXWARE_CODEC_ID_AC_8K_STEREO       0x04000001L  /* class = VOXWARE_CODEC_AC_8K_STEREO  */
#define VOXWARE_CODEC_ID_AC_11K_STEREO      0x04000002L  /* class = VOXWARE_CODEC_AC_11K_STEREO */
#define VOXWARE_CODEC_ID_AC_16K_STEREO      0x04000003L  /* class = VOXWARE_CODEC_AC_16K_STEREO */
#define VOXWARE_CODEC_ID_AC_22K_STEREO      0x04000004L  /* class = VOXWARE_CODEC_AC_22K_STEREO */
#define VOXWARE_CODEC_ID_AC_64K_STEREO      0x04000005L  /* class = VOXWARE_CODEC_AC_22K_STEREO */
#define VOXWARE_CODEC_ID_AC_80K_STEREO      0x04000006L  /* class = VOXWARE_CODEC_AC_22K_STEREO */
#define VOXWARE_CODEC_ID_AC_96K_STEREO      0x04000007L  /* class = VOXWARE_CODEC_AC_22K_STEREO */

/* Scaleable Codecs */
#define VOXWARE_CODEC_ID_SC_8K_3_GENERIC    0x03010003L  /* class = VOXWARE_CODEC_SC_8K_3 */
#define VOXWARE_CODEC_ID_SC_8K_6_GENERIC    0x03010006L  /* class = VOXWARE_CODEC_SC_8K_6 */
#define VOXWARE_CODEC_ID_SC_16K_10_GENERIC  0x03010010L  /* class = VOXWARE_CODEC_SC_16K_10 */


#endif /* _VCODECID_H_ */




