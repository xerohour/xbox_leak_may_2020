/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:      SumCos.h
* Purpose:
* Functions:
* Author/Date:
*******************************************************************************
*
* Modifications:  Bob Dunn 3/27/97 added sum of cosines synthesis and
*                    FHT synthesis
*
* Comments:
*
* Concerns:       FHT and sum of cosines make the assumption that the  
*                    frequencies to be synthesized are harmoniclly related
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/SumCos.h_v   1.0   16 Apr 1998 13:24:24   bobd  $
*
******************************************************************************/
#ifndef SUMCOS_H
#define SUMCOS_H

void SumCos( float  fPitchDFT, 
             float *pfAmps, 
             float *pfPhase, 
             int    iHarm, 
             int    iSynSubFrameLength, 
             float *pfOutputBuffer
           );

#endif /* SUMCOS_H */

