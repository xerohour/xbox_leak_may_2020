/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:          VAD.h                                                        
*                                                                              
* Purpose:           Voice Activity Detector                                                          
*                                                                              
* Functions:         VAD(), initVAD(), freeVAD()                                                          
*                                                                              
* Author/Date:       Rob Zopf/10/09/98 - Code based on JGA                                                          
********************************************************************************
* Modifications:
*                   
* Comments:                                                                      
* 
* Concerns:
*
* 
*******************************************************************************/


#define ACTIVE_SPEECH  1


/*******************************************************************************
* Function: VoiceActivity()
*
* Action: Perform Voice Activity Detection
*
* Input: *hVADMemBlk       - the VAD memory block 
*        FutureFrameEnergy - the energy of the next frame (the VAD uses a 
*                            one frame lookahead to avoid misclassifying onsets)
*        PitchCorr         - the normalized autocorrelation at the pitch lag 
*                            for the current frame.
*
* Output:
*        *VAD              - the activity flag : active speech =  ACTIVE_SPEECH
*                                                background    = !ACTIVE_SPEECH
*        *BckgrndSpeech    - a flag that signals the presence of possible 
*                            speech in the background noise.  This flag could
*                            be used to set Pv to zero, or be used to control
*                            the mode of a variable rate codec.
*                                                bckgrnd speech=  ACTIVE_SPEECH
*                                                non-speech    = !ACTIVE_SPEECH 
*
* Globals:      none
*
* Return:	none
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/

void VoiceActivity(void *hVADMemBlk, float FutureFrameEnergy, float PitchCorr, short *VAD,
         short *BckgrndSpeech);

/*******************************************************************************
* Function: initVAD()
*
* Action:
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:		  VOXErr : Voxware return code
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
unsigned short initVAD(void **hVADMemBlk);


/*******************************************************************************
* Function: freeVAD()
*
* Action:
*
* Input:
*
* Output:
*
* Globals:        none
*
* Return:		  VOXErr : Voxware return code
********************************************************************************
* Implementation/Detailed Description:
*
* References:
********************************************************************************
* Modifications:
*
* Comments:
*
* Concerns/TBD:
*******************************************************************************/
void freeVAD(void **hVADMemBlk);
