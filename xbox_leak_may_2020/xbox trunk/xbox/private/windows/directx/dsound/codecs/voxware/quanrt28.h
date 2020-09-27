/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       quanrt29.h                                                           
*                                                                              
* Purpose:        File containing all quantization definitions for rt29.
*                                                                              
* Author/Date:    Rob Zopf  03/18/97                                                       
********************************************************************************
*                                                                      
*******************************************************************************/
#ifndef QUANRT28_H
#define QUANRT28_H 

#define GAIN_BITS_28                     6
#define	PV_BITS_28                       4
#define	F0_BITS_28                       8
#define	LSP_BITS_28                      41
#define	LSP_BITS_1_28                    5
#define	LSP_BITS_2_28                    5
#define	LSP_BITS_3_28                    5
#define	LSP_BITS_4_28                    5
#define	LSP_BITS_5_28                    4
#define	LSP_BITS_6_28                    4
#define	LSP_BITS_7_28                    4
#define	LSP_BITS_8_28                    3
#define	LSP_BITS_9_28                    3
#define	LSP_BITS_10_28                   3
#define	LSP_TABLES_28                    10
#define	AMP_BITS_28                      4
#define	AMP_TABLES_28                    2
#define AMP_BITS_1_28                    4  
#define AMP_BITS_2_28                    0  
#define AMP_BITS_3_28                    0 
#define RESERVED_BITS_28                    1

#define TOTAL_BITS_28                    (GAIN_BITS_28+PV_BITS_28+F0_BITS_28+LSP_BITS_28+AMP_BITS_28+RESERVED_BITS_28)

/* use whole range to quantize pitch */
#define QUANT_MIN_PITCH_28     8     /* 1000 Hz */
#define QUANT_MAX_PITCH_28     160   /* 50 Hz */

#define BYTESPERFRAME_28                 8



#endif /* QUANRT28_H */
