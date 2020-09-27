/******************************************************************************
*                        Voxware Proprietary Material                         *
*                        Copyright 1996, Voxware, Inc                         *
*                            All Rights Reserved                              *
*                                                                             *
*                       DISTRIBUTION PROHIBITED without                       *
*                      written authorization from Voxware                     *
******************************************************************************/

/******************************************************************************
* Filename:   OutBufSC.h
*                                                                              
* Purpose:    Output speech to PCM buffer
*                                                                              
* Functions:  PreOutputManagerSC(), PostOutputManagerSC()
*
* Author/Date:   Wei Wang, 2/18/98
*******************************************************************************
* Modifications: 
*            
* Comments: Only for half frame synthesizer
* 
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/decode/OutBufSC.h_v   1.4   10 Apr 1998 11:49:32   weiwang  $
******************************************************************************/
#ifndef OUTBUF_H
#define OUTBUF_H


/******************************************************************************
* Function:   void PreOutputManagerSC()
*
* Action: output pfSaveBuffer to psPCMBuffer. Then pvSaveBuffer
*         can be reused later.
*
* Input:      pfSaveBuffer -- samples from previous frame
*             iSaveSamples -- number of samples in pfSaveBuffer
*             psPCMBuffer --  pointer to output speech buffer
*
* Output:     psPCMBuffer -- output speech
*
* Globals:    none
*
* Return:     none
*******************************************************************************
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
* Modifications:
*
* Comments:   iSaveSamples can only be 0 or FRAME_LEN.
*
* Concerns/TBD:
******************************************************************************/

void PreOutputManagerSC(const float *pfSaveBuffer, int iSaveSamples, 
                        short *psPCMBuffer);

/******************************************************************************
* Function:   void PostOutputManagerSC()
*
* Action:     Output speech to PCM buffer and update pfSaveBuffer.
*
* Input:      *pfSaveBuffer -- synthesized speech buffer
*             *piSaveSamples -- number of saved samples
*             iSynSamples -- number of synthesized speech
*             *psPCMBuffer -- pointer to output speech buffer
*
* Output:     *pfSaveBuffer -- saved samples for next frame
*             *piSaveSamples -- updated number of saved samples
*             *psPCMBuffer -- output PCM buffer
*             *pwPCMsamples -- number of samples in psPCMBuffer 
*                              (either 0 or FULL_FRAME_LEN).
*
* Globals:    none
*
* Return:     none
*******************************************************************************
* Implementation/Detailed Description:
*
* References:
*******************************************************************************
* Modifications:
*
* Comments: iSaveSamples is 0 or FRAME_LEN (80) and iSynSamples is
*           0, FRAME_LEN (80) or FULL_FRAME_LEN (160).
*
* Concerns/TBD:  
******************************************************************************/

void PostOutputManagerSC( float          *pfSaveBuffer, 
                          int            *piSaveSamples, 
                          int             iSynSamples,
                          short          *psPCMBuffer, 
                          unsigned short *pwPCMsamples
                        );

#endif /* OUTBUF_H */

