/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/
 
/*******************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/Compress.h_v   1.1   11 Apr 1998 15:18:18   bobd  $
*******************************************************************************/
#ifndef _COMPRESS_H
#define _COMPRESS_H

/******************************************************************************
*
* Function:   ScaleAndCompress()
*
* Action:     Scales and array and compress using fourth root compression.
*
* Input:      const float *pfIn      --> input array
*             int          iLength   --> length of input array
*             float        fMaxVal   --> inverse of scale factor (maximum value)
*
* Output:     float       *pfOut     --> output array
*
* Globals:    none
*
* Return:     none
*******************************************************************************
*
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
*
* Modifications:
*
* Comments:    In place operation is supported.
*
* Concerns/TBD:
******************************************************************************/
void ScaleAndCompress( const float *pfIn,
                       int          iLength,
                       float        fMaxVal,
                       float       *pfOut
                     );

#endif /* _COMPRESS_H */

