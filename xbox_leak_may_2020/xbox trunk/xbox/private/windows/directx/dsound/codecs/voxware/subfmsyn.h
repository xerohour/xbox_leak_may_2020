/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1998, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:       SubFmSyn.h
*                                                                              
* Purpose:        synthesis speech of each sub-frame
*                                                                              
* Functions:      SubFrameSynth()
*                                                                              
* Author/Date:    Original developed by Bob McAulay and Bob Dunn 1/97
*******************************************************************************
* Modifications: Moved out from DecSC.c by Wei Wang, 4/98
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/SubFmSyn.h_v   1.1   13 Apr 1998 16:17:08   weiwang  $
******************************************************************************/

#ifndef _SUBFMSYN_H_
#define _SUBFMSYN_H_

/*******************************************************************************
*
* Function:  SubFrameSynth()
*
* Action:   synthesis the speech of each sub-frame.
*
* Input:    pDecMemBlk -- decoder structure
*           pParamMblk -- parameter structure
*           pfSpeechBuf -- output speech buffer
*           pfEnv -- envelope spectrum
*           pfPhase -- minimum phases of current frame
*           piSynSamples -- pointer to the number of output speech samples
*
* Output:   pfSpeechBuf -- output speech
*           piSynSamples -- number of output speech samples
*
* Globals:   none
*
* Return:    none
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

void SubFrameSynth( STACK_R 
                    void           *hDecMemBlk, 
                    void           *hSubParamMblk,
                    float          *pfSpeechBuf,
                    float          *pfEnv,
                    float          *pfPhase,
                    int            *piSynSamples
                  );
#endif

