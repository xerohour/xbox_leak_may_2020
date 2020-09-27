/*******************************************************************************
*                         Voxware Proprietary Material                         *
*                         Copyright 1996, Voxware, Inc                         *
*                             All Rights Reserved                              *
*                                                                              *
*                        DISTRIBUTION PROHIBITED without                       *
*                       written authorization from Voxware                     *
*******************************************************************************/
 
/*******************************************************************************
* Filename:       KLPitch.h
*
* Purpose:        Set the scale factor for unvoiced speech.
*
* Functions:      float fSetKLFactor()
*
* Author/Date:    Original developed by Bob McAulay and Bob Dunn 1/97
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns: 
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/KLPitch.h_v   1.1   13 Apr 1998 18:26:18   weiwang  $
*******************************************************************************/

/*******************************************************************************
*
* Function:  fSetKLFactor()
*
* Action:    Determine scale factor for unvoiced harmonics
*
* Input:     float fAnaPitch  -> the analysis pitch period
*            float fSynF0   -> the synthesis pitch in DFT samples
*
* Output:    none
*
* Globals:   none
*
* Return:    float fLogAlpha  -> the scale factor (to add in log base 2)
********************************************************************************
*
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
#ifndef KLPITCH_H
#define KLPITCH_H

float fSetKLFactor( float fAnaPitch, float fSynPitch );

#endif /* KLPITCH_H */

