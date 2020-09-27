/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
 
/*******************************************************************************
* Filename:       Voicing.h
*
* Purpose:        Single-band voicing probability calculation
*                 using the peaks out of the square-law nonlinearity
*
* Functions:      VoxSTCVoicing()
*                 ModifyPv()
*
* Author/Date:    Bob McAulay, 11/10/96
********************************************************************************
* Modifications:Ilan Berci/ 10/96
*                 ModifyPv() added by Wei Wang
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/Voicing.h_v   1.1   11 Apr 1998 15:17:56   bobd  $
*******************************************************************************/

#ifndef _VOICING_H
#define _VOICING_H

#define DBMIN   4.0F             /* unvoiced if SNR < DBMIN...was 4.0 */
#define DBMID   4.5F             /* variable threshold...was 4.5 */
#define DBMAX   13.0F            /*   voiced if SNR > DBMAX */

/* maximum spread for differential frequencies...was 850. */
#define SQVOC_LO_EDGE   850.0F   

void VoxSTCVoicing( STACK_R 
                    float *Pv_km1, 
                    float *pfLogSeeEnv,
                    float *pfPitchPeriod, 
                    float *Pv,
                    float *PeakPwr, 
                    float *PeakFrq, 
                    int    iPeakCount
                  );

float ModifyPv( float fCorrCost, 
                float fPv, 
                float fPv_1, 
                float fPitch,
                float fPitch_1
              );

#endif  /* _VOICING_H*/


