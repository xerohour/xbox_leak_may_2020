/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1997, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
******************************************************************************/

/*******************************************************************************
* Filename:       VoicLD.h
*                                                                              
* Purpose:        voicing calculation for LDPDA.
*                                                                              
* Functions:     VoicingLD(), FindBand4VoicLD()
*
* Author/Date:    Wei Wang, 12/97
*
********************************************************************************
* Modifications: Reduce the length of refining window spectrum from 2048 to 112.
*                Use fix point indices to speech up code.
*                                                                              
* Comments:                                                                    
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/pea/VoicLD.h_v   1.1   02 Apr 1998 14:47:04   weiwang  $
******************************************************************************/
#ifndef _VOICLD_H_
#define _VOICLD_H_

/******************************************************************************
* Function:  VoicingLD()
*
* Action:    Estimate voicing probability and use it for final pitch seletion
*
* Input:     fPitch     -- pitch period
*            pfSW       -- complex spectrum
*            iNBand     -- length of spectrum for voicing probability 
*                          calculation
*
* Output:    none
*
* Globals:   none
*
* Return:    estimated voicing probability
*******************************************************************************
*
* Implementation/Detailed Description:
*
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
*
******************************************************************************/
float VoicingLD( float        fPitch,
                 const float  *pfSW, 
                 int          iNBand
               );



/******************************************************************************
* Function:  FindBand4VoicLD()
*
* Action:    Find out spectrum band for voicing calculation
*
* Input:     iBaseBand   -- band for pitch estimation
*            fMinPitch   -- minimum pitch period for voicing calculation
*
* Output:    none
*
* Globals:   none
*
* Return:    band for voicing calculation
*******************************************************************************
*
* Implementation/Detailed Description:
*
*******************************************************************************
*
* Modifications:
*
* Comments:  
*
* Concerns/TBD:
*
******************************************************************************/
int FindBand4VoicLD ( int   iBaseBand, 
                      float fMinPitch
                    );

#endif
