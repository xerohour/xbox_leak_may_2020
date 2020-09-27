/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       param.h                                                             
*                                                                              
* Purpose:                                                                    
*                                                                              
* Author/Date:    Ilan Berci 09/96                                                             
********************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/utils/param.h_v   1.2   01 Dec 1998 14:38:02   zopf  $                                                                     
*******************************************************************************/
#ifndef PARAM_H
#define PARAM_H

#define INITIAL_PITCH  100.0F

#define INITIAL_LSF_0  343.0F
#define INITIAL_LSF_1  752.0F
#define INITIAL_LSF_2 1176.0F
#define INITIAL_LSF_3 1536.0F
#define INITIAL_LSF_4 1841.0F
#define INITIAL_LSF_5 2189.0F
#define INITIAL_LSF_6 2463.0F
#define INITIAL_LSF_7 2854.0F
#define INITIAL_LSF_8 3171.0F
#define INITIAL_LSF_9 3555.0F

/***** Parameter structure *****/
typedef struct            /** all members of this structure need to be used in encoder **/
{
  float Pv;               /** voice probability                                        **/
  float Pitch;            /** pitch value                                              **/
  float Energy;           /** residue energy                                           **/
  float *Lsf;             /** line spectrum frequencies -- 10 point                    **/
  float *Amp;             /** residue amplitude -- 60 point                            **/
  float *Alpc;
  float *Blpc;            /** for backward compatibility - VFII - see John or Wei      **/
  short VAD;

} PARAM;

typedef struct tagPrevFrameParam
{
   float Pitch;
   float Pv;
   float Energy;
   float *Amp;
   float *Lsf;
} PrevFrameParam;

typedef struct tagSubFrameParam
{
   float                        Pitch;
   float                        Pv;
   float                        Energy;
   float                        *Amp;
   float                        *Lsf;
   float                        Pitch_1;                /* _1 = prev sub frame */
   float                        Pv_1;
   float                        Energy_1;
   float                        *Amp_1;
   float                        *Lsf_1;

   short                        WarpFrame;
   short                        totalWarpFrame;
   short                        frameLEN;               /* current frame len */
   short                        nSubs;
   
   float                        *AveLsf;
   int                          Hangover;

} SubFrameParam;


unsigned short VoxFreeParam(void **pParam);
unsigned short VoxInitParam(void **hParam, void *pVemMem);

unsigned short VoxInitPrevFrame(void **hPrevFrameParam_mblk);
unsigned short VoxFreePrevFrame(void **hPrevFrameParam_mblk);

unsigned short VoxInitSubframe(void **hSubFrameParam_mblk);
unsigned short VoxFreeSubframe(void **hSubFrameParam_mblk);

#endif /* PARAM_H */


