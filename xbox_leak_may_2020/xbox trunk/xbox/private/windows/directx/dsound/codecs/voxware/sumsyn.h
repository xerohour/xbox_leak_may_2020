/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
#ifndef SUMSYN_H
#define SUMSYN_H

void SumSyn(STACK_R void *pvSumSynBlk, float *Amp, float Pitch, float Pitch_1, float *Phase, 
            float multPitch, short pitch_mult, short nHarm, short vHarm, short frameLen, 
            float interpFactor, float *outSpeech);

unsigned short InitSumSyn(void **hSumSynBlk);

unsigned short FreeSumSyn(void **hSumSynBlk);

#endif


