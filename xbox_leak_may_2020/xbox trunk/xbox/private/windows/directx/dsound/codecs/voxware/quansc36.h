/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:       quansc36.h
*
* Purpose:        File containing all quantization definitions for sc36.
*
* Author/Date:    Bob Dunn  06/03/97
*******************************************************************************
*
* Comments: WARNING: WARNING: WARNING: (Yes, that means you should read this!)

*            The bit allocations for both SC3 and SC6 MUST be byte aligned 
*            or the VCI and other things may break.  The byte alignment can
*            be accomplished by modifying UNUSED_BITS_SC3
*            and/or UNUSED_BITS_SC6.
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/include/quansc36.h_v   1.4   13 Apr 1998 16:17:36   weiwang  $
*
******************************************************************************/

#ifndef _QUANSC36_H
#define _QUANSC36_H

/*------------------------------------------------------
  Pitch quantization for SC3 and SC6
------------------------------------------------------*/
#define BITS_PITCH_SC3              8
#define MAX_QUAN_PITCH_PERIOD_SC3   160.0F   /* 50 Hz */
#define MIN_QUAN_PITCH_PERIOD_SC3   8.0F     /* 1000 Hz */

/*------------------------------------------------------
  Gain quantization for SC3 and SC6
------------------------------------------------------*/
#define BITS_GAIN_SC3               6
#define MIN_QUAN_GAIN               -3.0F
#define MAX_QUAN_GAIN               12.0F


/*------------------------------------------------------
  Voicing quantization for SC3 and SC6
------------------------------------------------------*/
#define BITS_PV_SC3                 4 /*%#  5 #%*/


/*------------------------------------------------------
  ASK quantization for SC3 and SC6
------------------------------------------------------*/
#define BITS_ASKS_SC36             44


#define STAGE1_BITS_ASKS_0_1_SC36      5
#define STAGE2_BITS_ASKS_0_1_SC36      5
#define STAGE1_BITS_ASKS_2_3_SC36      5
#define STAGE2_BITS_ASKS_2_3_SC36      4
#define STAGE1_BITS_ASKS_4_5_SC36      5
#define STAGE2_BITS_ASKS_4_5_SC36      3
#define STAGE1_BITS_ASKS_6_7_SC36      5
#define STAGE2_BITS_ASKS_6_7_SC36      2
#define STAGE1_BITS_ASKS_8_9_SC36      5
#define STAGE2_BITS_ASKS_8_9_SC36      0
#define STAGE1_BITS_ASKS_10_11_SC36    5
#define STAGE2_BITS_ASKS_10_11_SC36    0

#define RESIDUAL_BITS_ASKS_8_9_SC6     3
#define RESIDUAL_BITS_ASKS_10_11_SC6   3

/*------------------------------------------------------
  Phase quantization for SC6
------------------------------------------------------*/
#define BITS_PHASE_SC6             56

#define BITS_PHASE_PAIR_SC6   (BITS_PHASE_SC6/SC6_PHASES)

/*------------------------------------------------------
  Reserved bits for SC3 and SC6.  These are place
    holders in the bitstream for bits to be defined
    in the future definition.
------------------------------------------------------*/
#define RESERVED_BITS_SC3           1  /* one reserved bit for SC3 */

#define RESERVED_BITS_SC6           1  /* one reserved bit in SC6 */


/*------------------------------------------------------
  These bits are flags for the VCI to determine if
    the embeded bitstream contains higher rate data.
------------------------------------------------------*/
#define VCI_FLAG_BITS_SC3           1
#define VCI_FLAG_BITS_SC6           1


/*------------------------------------------------------
  Total bit number of bits for SC3 and SC6
------------------------------------------------------*/
#define TOTAL_BITS_SC3    (  BITS_PITCH_SC3 \
                           + BITS_GAIN_SC3 \
                           + BITS_PV_SC3 \
                           + BITS_ASKS_SC36 \
                           + RESERVED_BITS_SC3 \
                           + VCI_FLAG_BITS_SC3 )

#define ADDITIONAL_BITS_SC6 (  BITS_PHASE_SC6 \
                             + RESIDUAL_BITS_ASKS_8_9_SC6 \
                             + RESIDUAL_BITS_ASKS_10_11_SC6 \
                             + RESERVED_BITS_SC6 \
                             + VCI_FLAG_BITS_SC6 )


/*------------------------------------------------------
  Total bit number of bytes for SC3 and SC6
------------------------------------------------------*/
#define TOTAL_BYTES_SC3             (TOTAL_BITS_SC3>>3)
#define ADDITIONAL_BYTES_SC6        (ADDITIONAL_BITS_SC6>>3)


#endif /* _QUANSC36_H */

