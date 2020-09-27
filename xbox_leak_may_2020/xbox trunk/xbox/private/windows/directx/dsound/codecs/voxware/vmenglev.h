/******************************************************************************
 *                                                                            *
 *		                  Voxware Proprietary Material                        *
 *		                  Copyright 1996, Voxware, Inc.                       *
 *		                       All Rights Resrved                             *
 *                                                                            *
 *		                 DISTRIBUTION PROHIBITED without                      *
 *		               written authorization from Voxware                     *
 *                                                                            *
 ******************************************************************************/

/******************************************************************************
 *                                                                            *
 * FILENAME:  vmEngLev.h                                                       *
 * PURPOSE:   Calculates the energy in the last frame compressed                *
 * AUTHOR:    Ilan Berci 03/97                                                   *
 *                                                                            *
 *****************************************************************************/

#ifndef VMENGLEV_H
#define VMENGLEV_H

#ifndef _VCI_RETCODE_
#define _VCI_RETCODE_
typedef unsigned short VCI_RETCODE; /* Voxware Core Interface error type.     */
                                    /* This variable is also defined in       */
                                    /* vciError.h.                            */
#endif  /** #ifndef _VCI_RETCODE_ **/



float EnergyToLevel(float Energy);
float LevelToEnergy(float Level);

#endif /* VMENGLEV_H */


