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
#ifndef QUANRT29_H
#define QUANRT29_H 

#define GAIN_BITS_29                     5
#define	PV_BITS_29                       4
#define	F0_BITS_29                       8
#define	LSP_BITS_29                      41
#define	LSP_BITS_1_29                    5
#define	LSP_BITS_2_29                    5
#define	LSP_BITS_3_29                    5
#define	LSP_BITS_4_29                    5
#define	LSP_BITS_5_29                    4
#define	LSP_BITS_6_29                    4
#define	LSP_BITS_7_29                    4
#define	LSP_BITS_8_29                    3
#define	LSP_BITS_9_29                    3
#define	LSP_BITS_10_29                   3
#define	LSP_TABLES_29                    10
#define	AMP_TABLES_29                    3
#define AMP_BITS_1_29                    3  
#define AMP_BITS_2_29                    3  
#define AMP_BITS_3_29                    3 
#define	AMP_BITS_29                      (AMP_BITS_1_29+AMP_BITS_2_29+AMP_BITS_3_29)
#define RESERVED_BITS_29                 0

#define TOTAL_BITS_29                    (GAIN_BITS_29+PV_BITS_29+F0_BITS_29+LSP_BITS_29+AMP_BITS_29+RESERVED_BITS_29)

/* use whole range to quantize pitch */
#define QUANT_MIN_PITCH_29     16     /* 1000 Hz */
#define QUANT_MAX_PITCH_29     125   /* 50 Hz */

#define BYTESPERFRAME_29                 9

#define NUMSTAGES_AMPVQ_29               3 
#define NUMCAND_AMPVQ_29                 16  
#define TABLESIZE_AMPVQ_29               8


#endif /* QUANRT29_H */
