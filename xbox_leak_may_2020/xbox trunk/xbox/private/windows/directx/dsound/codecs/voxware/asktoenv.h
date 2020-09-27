/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:       AskToEnv.h
*                                                                              
* Purpose:        Compute magnitude and phase envelopes from arcsin of
*                   reflection coefficients. 
*                                                                              
* Functions:      VoxAskToEnv()
*                                                                              
* Author/Date:    Bob Dunn 1/98
*******************************************************************************
* Modifications:                                                               
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/AskToEnv.h_v   1.4   10 Apr 1998 11:46:34   weiwang  $
******************************************************************************/
#ifndef ASKTOENV_H
#define ASKTOENV_H

/*******************************************************************************
*
* Function:  VoxAskToEnv()
*
* Action:    Get the spectrum envelope from ASKs.
*
* Input:    *pfASK -- input ASKs
*           fLog2Gain -- Log2 of residuel gain
*           pfEnv -- pointer to the output envelope amplitudes
*           pfPhase -- pointer to the output phases
*
* Output:   pfEnv -- output envelope amplitudes
*           pfPhase -- the output phases
*
* Globals:   none
*
* Return:    void
*******************************************************************************/

void VoxAskToEnv( STACK_R 
                  const float *pfASK, 
                  const float  fLog2Gain, 
                  float       *pfEnv, 
                  float       *pfPhase
                 );

#endif /* ASKTOENV_H */

