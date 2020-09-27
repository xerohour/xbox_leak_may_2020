/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        QoTlogic.h
*
* Purpose:         Brains of the codec QoT.
*
* Functions:       QoTFrameLogic
*
* Author/Date:     Robert Zopf 08/18/97
*******************************************************************************
*
* Modifications: 
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/common/vci/QoTlogic.h_v   1.2   16 Mar 1998 17:04:40   weiwang  $
******************************************************************************/

#ifndef QOT_LOGIC_H
#define QOT_LOGIC_H

#define REPEAT_FRAMES     2            /* can repeat up to these many frames */


/******************************************************************************
* Function:  QoTFrameLogic() 
*
* Action:    Brains of the codec QoT.
*
* Input: 
*        wLookaheadCount        - number of frames ahead that frame information
*                                 is present in lpaFrames.
*
*        wLookbackCount         - number of frames back that frame information
*                                 is present in lpaFrames.
*
*        *lpaFrames             - pointer to an array of structures describing
*                                 the frames for which frame information is 
*                                 available.  This array starts with the frame
*                                 [n-wLookbackCount] and continues until frame
*                                 [n+wLookaheadCount] for a total array size of
*                                 wLookbackCount+wLookaheadCount+1.  The current
*                                 frame [n] is located at 
*                                 lpaFrames[wLookbackCount].
*
*        fWarp                  - the desired warp for the current frame.
*
*
* Output:
*        *pwUseFrame            - the index of the frame into lpaFrames of the 
*                                 frame use for synthesis of the current frame.
*        *pfUseFrameWarp        - the warp to apply with *pwUseFrame.
*
*        *pwFrameStatus         - internal memory required by the function. This
*                                 variable keeps track of the number of frames 
*                                 the QoT is up or down with respect to the 
*                                 current time. That is, 0 indicates we are up 
*                                 to date, -n indicates we are currently delayed 
*                                 by n frames, and +n indicates we have 
*                                 synthesized n future frames.
*
*        *pwSkipFrame           - flag to indicate whether or not to skip this 
*                                 frame entirely. This may occur if the current
*                                 frame is not present.
*
*        *pwFadeToSilence       - flag to indicate whether or not to fade to 
*                                 silence for the current frame. This may occure 
*                                 if the current frame is not present.
* Globals:    none
*
* Return:     none
*
******************************************************************************/
void QoTFrameLogic(short wLookaheadCount, 
                   short wLookbackCount,
                   const QoTFrame *lpaFrames, 
                   float fWarp,
                   short *pwUseFrame, 
                   float *pfUseFrameWarp,
                   float *pwFrameStatus, 
                   short *pwSkipFrame,
                   short *pwFadeToSilence);

#endif /* QOT_LOGIC_H */
