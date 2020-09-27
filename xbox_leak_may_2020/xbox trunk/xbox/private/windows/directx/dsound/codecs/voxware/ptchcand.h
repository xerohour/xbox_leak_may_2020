/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
******************************************************************************/

/*******************************************************************************
* Filename:       PtchCand.h
*                                                                              
* Purpose:        Choose the best pitch candidates for further analysis.
*                                                                              
* Functions:      
*
* Author/Date:    Original developed by Gerard Aguilar.
*                 Enhanced by Wei Wang.
*
********************************************************************************
* Modifications:
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/PtchCand.h_v   1.1   02 Apr 1998 14:47:00   weiwang  $
******************************************************************************/

#ifndef _PTCH_CAND_H_
#define _PTCH_CAND_H_


/******************************************************************************
* Function:  SelectPitchCandidates()
*
* Action:    Select most possible candidates.
*
* Input:     pfMag       -- compressed magnitude spectrum
*            iBaseBand   -- length of base bandwidth
*            piPeakFreq  -- peak indecies
*            pfPitchCand -- address to the output pitch candidates
*            pfCandCost  -- address to the cost values of selected candidates
*            fLastP0     -- previous pitch for backward tracking
*            flastCost   -- the cost value of previous pitch period
*            piHP_Index  -- address to the index of pfPitchCand to get the high-
*                           possibility pitch candidate
*
* Output:    pfPitchCand -- pitch periods of selected candidates
*            piHP_Index  -- index of pfPitchCand to get the high-
*                           possibility pitch candidate
*
* Globals:   none
*
* Return:    number of candidates
******************************************************************************/
int SelectPitchCandidates( STACK_R 
                           const float *pfMag, 
                           int         iBaseBand, 
		 	   const int   *piPeakFreq, 
                           float       *pfPitchCand, 
			   float       *pfCandCost, 
                           float       fLastP0,
                           float       fLastCost,
			   int         *piHP_Index
                         );

#endif /* _PTCH_CAND_H_ */


