/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename: FadeIO.h                                                                    
*                                                                              
* Purpose: Fade-In/ Fade-Out during frame loss conditions                                                                    
*                                                                              
* Functions:                                                                   
*                                                                              
* Author/Date: RZ/ 02-26-97                                                                
********************************************************************************
* Modifications:
*                   
* Comments:                                                                    
* 
* Concerns:
*
* 
*******************************************************************************/
#ifndef FADEIO_H
#define FADEIO_H

#define SILENCE_PITCH              125.0F
#define RUNNING_AVE                0.97F
#define ENERGY_INCREASE            1.01F
#define FADEIN_LEN                 3
#define INITIAL_BACKGROUND_ENERGY  500.0F;

#define SILENCE                    0
#define NORMAL                     1

typedef struct tagFadeIO
{
   short   CurrentFrameState;
   short   LastFrameState;
   float   BackgroundEnergyEst;
   float  *BackgroundLsf;
   short   FadeInCount;

} FadeIOstruct;

void VoxFadeIO(void *pvFadeIOMemBlk, void *pvParamMemBlk);
unsigned short VoxinitFadeIO(void **hFadeIOMemBlk);
unsigned short VoxfreeFadeIO(void **hFadeIOMemBlk);

#endif /* FADEIO_H */
