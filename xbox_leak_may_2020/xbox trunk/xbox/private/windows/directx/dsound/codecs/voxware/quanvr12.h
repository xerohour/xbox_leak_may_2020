/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       quanvr12.h                                                           
*                                                                              
* Purpose:        File containing all quantization definitions for vr12.
*                                                                              
* Author/Date:    Rob Zopf  03/18/97                                                       
********************************************************************************
*                                                                      
*******************************************************************************/
#ifndef QUANVR12_H
#define QUANVR12_H 

#define	TABLESIZE_LSPVQ_VR12            16 
#define	TABLEBITS_LSPVQ_VR12             4
#define	NUMCAND_LSPVQ_VR12              16   
#define NUMSTAGES_LSPVQ_VR12_VOICED      6
#define NUMSTAGES_LSPVQ_VR12_MIXED      6
#define NUMSTAGES_LSPVQ_VR12_UNVOICED    2

#define CLASS_BITS_VR12                  2

#define GAIN_BITS_VR12_VOICED            5
#define F0_BITS_VR12_VOICED              7
#define LSP_BITS_VR12_VOICED             (NUMSTAGES_LSPVQ_VR12_VOICED*TABLEBITS_LSPVQ_VR12)
#define TOTAL_BITS_VR12_VOICED           (CLASS_BITS_VR12+GAIN_BITS_VR12_VOICED+F0_BITS_VR12_VOICED+LSP_BITS_VR12_VOICED)

#define GAIN_BITS_VR12_MIXED            GAIN_BITS_VR12_VOICED
#define PV_BITS_VR12_MIXED              3
#define F0_BITS_VR12_MIXED              F0_BITS_VR12_VOICED
#define LSP_BITS_VR12_MIXED             (NUMSTAGES_LSPVQ_VR12_MIXED*TABLEBITS_LSPVQ_VR12)
#define TOTAL_BITS_VR12_MIXED           (CLASS_BITS_VR12+GAIN_BITS_VR12_MIXED+PV_BITS_VR12_MIXED+F0_BITS_VR12_MIXED+LSP_BITS_VR12_MIXED)


#define GAIN_BITS_VR12_UNVOICED          5
#define LSP_BITS_VR12_UNVOICED           (NUMSTAGES_LSPVQ_VR12_UNVOICED*TABLEBITS_LSPVQ_VR12)
#define TOTAL_BITS_VR12_UNVOICED         (CLASS_BITS_VR12+GAIN_BITS_VR12_UNVOICED+LSP_BITS_VR12_UNVOICED)

#define TOTAL_BITS_VR12_SILENCE          CLASS_BITS_VR12 

/* use whole range to quantize pitch */
#define QUANT_MIN_PITCH_VR     8     /* 1000 Hz */
#define QUANT_MAX_PITCH_VR     160   /* 50 Hz */

#define BYTESPERFRAME_VR                 7

#define SILENCE_FRAME 3
#define UV_FRAME      2
#define MIX_FRAME     1
#define V_FRAME       0

#define PV_RESET      0.05    /* if pv < PV_RESET, set pv = 0.0 */
#define PV_V_BOUND          0.70    /* above which, pv will be synthesized at PV_V */
#define PV_V          0.94       /* */

#define ENERGY_HISTORY  10

#define RATE_V     0.1F
#define RATE_MIX   0.55F
#define RATE_UV    0.1F
#define RATE_S     0.25F

#endif /* QUANVR12_H */
