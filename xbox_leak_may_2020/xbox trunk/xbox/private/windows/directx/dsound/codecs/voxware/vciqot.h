/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        vciQoT.h
*
* Purpose:         public structure for VCI QOT
*
* Functions:       
*
* Author/Date:     Robert Zopf 08/18/97
*******************************************************************************
*
* Modifications: 
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/vci/vciQoT.h_v   1.1   13 Feb 1998 10:42:40   weiwang  $
******************************************************************************/
#ifndef VCI_QOT
#define VCI_QOT
/*---------------------------------------------------------------------------
                    P U B L I C   S T R U C T U R E S 
  ---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif

/* This structure dwscribes a single frame. */
typedef struct
{
   /* TRUE if the frame is present */
   int                      bFrameIsPresent;
  
   /* If the frame is present, these identify
    * the start of the frame.  Otherwise, they
    * are unused.
    */
   unsigned char            *pchFrameFirstByte;

  /* indicate the bit offset of the first byte */
   unsigned char             chBitOffset;
} QoTFrame;

 
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* VCI_QOT */

