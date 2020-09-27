/*******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
*******************************************************************************/
 
/*******************************************************************************
*
* Filename:    EnvToAs.c
*
* Purpose:     Compute predictor coefficients and gain from the log (base 2)
*                of the spine fit to the SeeVoc power peaks.
*
* Functions:
*
* Author/Date: Bob Dunn 12/28/96
*
********************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/EnvToAs.h_v   1.1   11 Apr 1998 15:19:22   bobd  $
*******************************************************************************/

#ifndef LOGENVTOAS_H
#define LOGENVTOAS_H

void LogEnvelopeToAs( STACK_R
                      float *pfLogSeeEnv,
                      int    iOrderAllpole,
                      float *pfGain,
                      float *pfAS,
                      float *pfKS
                     );

#endif /* LOGENVTOAS_H */

