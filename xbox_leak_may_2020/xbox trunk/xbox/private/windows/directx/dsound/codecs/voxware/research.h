/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       research.h                                                           
*                                                                              
* Purpose:        File to handle research (temporary) compile switches.
*                                                                              
* Author/Date:    Rob Zopf  02/17/97                                                       
********************************************************************************
*                                                                      
*******************************************************************************/
#ifndef RESEARCH_H
#define RESEARCH_H

/***** The following 3 precompiler definitions are for pitch range checking. If the model code
 and quantization can support pitch range from 10 to 180, then we should switch all the 
 definition to 0s ****/
#define USE_DOUBLE_PITCH            0  /**** use double pitch residue amplitude calculation when pitch < 16 ****/

#define LOG_DOMAIN_INTERPOLATION 0

#define RESID_FLAT_SYN 0

#define UV_PITCH 64 /* hz */

#define USE_MULTIPLE_PITCH_FOR_UV   1

#define USE_MEASURED_PHASE 0

#define ROB_NEW  1

/* Change quantization of energy. Those definition will move to model.h 
   when it's finalized. */
#define ScaleEngUp         1.6027F /* (1.6027F) factors for reusing the old quantization table */
#define ScaleEngDown       0.623947F  /*   (0.623947F) */


/* Change residue amplitude calculation. */

#define NEW_QUAN_PITCH     1      /* 0: old pitch quantizer.
				     1: new pitch quantizer from 50Hz to 1000Hz  */

#define NOT_QUAN_AMP       0   /* 1: not quantize it . 0: quantize it */

#define NOT_QUAN_ENG       0   /* 1: not quantize it ,  0: quantize it */
#define NEW_GAINCB_5Bit    1

#define NOT_QUAN_LSF       0

#define NOT_QUAN_PITCH     0

#define NOT_QUAN_PV        0


#define COMPLEX_VOICING      1

#endif /* RESEARCH_H */
