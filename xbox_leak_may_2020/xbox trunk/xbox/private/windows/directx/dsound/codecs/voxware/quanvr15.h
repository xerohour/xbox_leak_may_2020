/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       quanvr15.h                                                           
*                                                                              
* Purpose:        File containing all quantization definitions for vr15.
*                                                                              
* Author/Date:    Rob Zopf  06/01/97                                                       
********************************************************************************
*                                                                      
*******************************************************************************/
#ifndef QUANVR15_H
#define QUANVR15_H 

#define	TABLESIZE_LSPVQ_VR15            16
#define	TABLEBITS_LSPVQ_VR15             4
#define	NUMCAND_LSPVQ_VR15              16   
#define NUMSTAGES_LSPVQ_VR15_VOICED      8
#define NUMSTAGES_LSPVQ_VR15_MIXED      8
#define NUMSTAGES_LSPVQ_VR15_UNVOICED    3

#define CLASS_BITS_VR15                  2

#define GAIN_BITS_VR15_VOICED            6
#define F0_BITS_VR15_VOICED              7
#define LSP_BITS_VR15_VOICED             (NUMSTAGES_LSPVQ_VR15_VOICED*TABLEBITS_LSPVQ_VR15)
#define TOTAL_BITS_VR15_VOICED           (CLASS_BITS_VR15+GAIN_BITS_VR15_VOICED+F0_BITS_VR15_VOICED+LSP_BITS_VR15_VOICED)

#define GAIN_BITS_VR15_MIXED             GAIN_BITS_VR15_VOICED
#define F0_BITS_VR15_MIXED               F0_BITS_VR15_VOICED
#define PV_BITS_VR15_MIXED               3
#define LSP_BITS_VR15_MIXED              (NUMSTAGES_LSPVQ_VR15_MIXED*TABLEBITS_LSPVQ_VR15)
#define TOTAL_BITS_VR15_MIXED            (CLASS_BITS_VR15+GAIN_BITS_VR15_MIXED+F0_BITS_VR15_MIXED+LSP_BITS_VR15_MIXED+PV_BITS_VR15_MIXED)


#define GAIN_BITS_VR15_UNVOICED          5
#define LSP_BITS_VR15_UNVOICED           (NUMSTAGES_LSPVQ_VR15_UNVOICED*TABLEBITS_LSPVQ_VR15)
#define TOTAL_BITS_VR15_UNVOICED         (CLASS_BITS_VR15+GAIN_BITS_VR15_UNVOICED+LSP_BITS_VR15_UNVOICED)

#define TOTAL_BITS_VR15_SILENCE          CLASS_BITS_VR15 

/* use whole range to quantize pitch */
#define QUANT_MIN_PITCH_VR     8     /* 1000 Hz */
#define QUANT_MAX_PITCH_VR     160   /* 50 Hz */

#define BYTESPERFRAME_VR                 7

#define SILENCE_FRAME 3
#define UV_FRAME      2
#define MIX_FRAME     1
#define V_FRAME       0

#define PV_RESET      0.05    /* if pv < PV_RESET, set pv = 0.0 */
#define PV_V_BOUND    0.70    /* above which, pv will be synthesized at PV_V */
#define PV_V          0.94       /* */

#define ENERGY_HISTORY  10

#define RATE_V     0.10F
#define RATE_MIX   0.55F
#define RATE_UV    0.10F
#define RATE_S     0.25F

#endif /* QUANVR15_H */
