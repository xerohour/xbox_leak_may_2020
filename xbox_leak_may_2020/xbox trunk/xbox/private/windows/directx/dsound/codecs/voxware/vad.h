/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       VAD.h                                                             
*                                                                              
* Purpose:        Implementation of QCELP's proposed standard.  This procedure
*		          uses their method for estimating the background noise and
*		          their formula for calculating the silence threshold.                                                            
*                                                                              
* Author/Date:    Rob Zopf 
*                 1/7/95 Modified and Rewritten for the CODEC and TeleVox by Wei Wang                                                           
********************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/encode/VAD.h_v   1.0   26 Jan 1998 10:55:00   johnp  $                                                                     
*******************************************************************************/
#ifndef VAD_H
#define VAD_H

#define LOW_THRESH_LIM        160000.0F
#define HIGH_THRESH_LIM      5059644.0F

#define HIGH_ESTIMATE         500000.0F

#define INIT_NOISE_LEVEL   HIGH_THRESH_LIM
#define INIT_NOISE_THRSH   1.563645e+07F  /**** by calculation ****/

#define SILENCE        0
#define VOICED         1

#define MTH_MIN(a, b)   (((float)a<=(float)b)?(float)a:(float)b)
#define MTH_MAX(a, b)   (((float)a>=(float)b)?(float)a:(float)b)

/* voice activity detection structure */
typedef struct
{
  float threshold;
  float estimate;
  float meanVoiceEng;
  short count;
  short medianZ[3];

  float frameEng;
  float maxEng;
  short medianZ2[3];
  unsigned short HangOverCount;
} VAD;


unsigned short VoxInitVad(void **fcp);

unsigned short VoxVoiceClass(void *fcp, float frame_E, float Pv,
                                 unsigned short *pVclass);

#endif /* VAD_H */

