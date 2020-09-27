/******************************************************************************
 *                                                                            *
 *		                  Voxware Proprietary Material                          *
 *		                  Copyright 1996, Voxware, Inc.                         *
 *		                       All Rights Resrved                               *
 *                                                                            *
 *		                 DISTRIBUTION PROHIBITED without                        *
 *		               written authorization from Voxware                       *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
 *                                                                            *
 * FILENAME:  DrpFrame.h                                                      *
 * PURPOSE:   Routines for droping frames during speedup.                     *
 *                                                                            *
 * AUTHOR:                                                                    *
 *                                                                            *
 *****************************************************************************/

#ifndef DRPFRAME_H
#define DRPFRAME_H

short DropFrameFast(float *WarpFact, short *DropCounter, 
					short *dropLeft, short *DropRateId);
#endif /* DRPFRAME_H */




