#ifndef _TRACKPTH_H_
#define _TRACKPTH_H_

/***********************************************************************
 *
 * Filename:  trackPth.h
 *
 * Purpose:  tracking the pitch based on one future frame and one or two
 *           previous frames. And make the decision.
 *           
 * Reference:  (1) ITU-T Draft G.723, p.13.
 *             (2) Inmarsat-M Voce Coding System Description, p15-17.
 *
 * Author:     Wei Wang
 *
 * Date:       Oct. 08, 1996
 *
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/pda/trackPth.h_v   1.2   12 May 1998 12:55:16   weiwang  $
 ***********************************************************************/

/*************************************************************
 * Fucntion:   trackPitch()
 *
 * Action:    do forward and backward pitch tracking using 
 *            biased cost function.
 *
 * Input:     myPDATrack_mblk:  pointer for pitch tracking.
 *
 * Output:    pitch value of frame 0.
 *************************************************************/

short VoxTrackPitch(STACK_R float *costs1, CorrPDA_mblk *myCorrPDA_mblk);



#endif /* end of #ifndef _TRACKPTH_H_ */


