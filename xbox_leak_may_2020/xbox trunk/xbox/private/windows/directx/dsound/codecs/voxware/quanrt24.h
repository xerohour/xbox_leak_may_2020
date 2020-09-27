/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       quanrt24.h    
*                                                                              
* Purpose:        File containing all quantization definitions.
*                                                                              
* Author/Date:    Rob Zopf  03/17/97                                                       
********************************************************************************
*                                                                      
*******************************************************************************/
#ifndef QUANRT24_H
#define QUANRT24_H

#define GAIN_BITS_24                     6
#define	PV_BITS_24                       4
#define	F0_BITS_24                       8
#define	LSP_BITS_24                      35
#define	LSP_BITS_1_24                    3
#define	LSP_BITS_2_24                    4
#define	LSP_BITS_3_24                    4
#define	LSP_BITS_4_24                    4
#define	LSP_BITS_5_24                    4
#define	LSP_BITS_6_24                    4
#define	LSP_BITS_7_24                    3
#define	LSP_BITS_8_24                    3
#define	LSP_BITS_9_24                    3
#define	LSP_BITS_10_24                   3
#define	LSP_TABLES_24                    10
#define RESERVED_BITS_24                 1

#define TOTAL_BITS_24                    (GAIN_BITS_24+PV_BITS_24+F0_BITS_24+LSP_BITS_24+RESERVED_BITS_24)

#define QUANT_MIN_PITCH_24               16   
#define QUANT_MAX_PITCH_24               125 

#define BYTESPERFRAME_24                 7



#endif /* QUANRT24_H */
