/******************************************************************************
*                        Voxware Proprietary Material
*                        Copyright 1996, Voxware, Inc
*                            All Rights Reserved
*
*                       DISTRIBUTION PROHIBITED without
*                      written authorization from Voxware
******************************************************************************/

/******************************************************************************
* Filename:        vciQoTSC.h
*
* Purpose:         provide QoT (quanlity of transmission) for SC codec
*
* Functions:   
*
* Author/Date:     Rob Zopf
*******************************************************************************
*
* Modifications: cleaned up structure and code by Wei Wang, 02/98
*
* Comments:
*
* Concerns:
*
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/vci/vciQoTSC.h_v   1.2   16 Mar 1998 17:20:16   weiwang  $
******************************************************************************/

#ifndef _VCI_QOT_SC
#define _VCI_QOT_SC

#ifdef __cplusplus
extern "C"
{
#endif


/******************************************************************************
*
* Function:  vciQoTDecodeSC()
*
* Action:    Decodes one frame of speech under QoT using SC codec.
*
* Input:   pDecodeMemBlk   - a pointer to the codec's private data storage 
*                             returned from the codec initialization process.
*          pvciCodecIOBlk  - a pointer to the standard codec I/O structure which 
*                            is used to describe the output buffer where the 
*                            decoded frame is to be written.
*          pvciQoTMemBlk  - a pointer to the QoT's private data storage.
*          wLookaheadCount - the number of frame s"ahead" of this frame in time 
*                            for which look-ahead infromation is available.
*
*          wLookbackCount  - the number of frames "back" in time for which 
*                            look-back information is present.
*
*          lpaFrames       - a pointer to an array of structures describing the 
*                            frames for which frame information is available.
*                            This array starts with the frame n-wLookbackCount
*                            and continues until frame n+wLookaheadCount for a 
*                            total of wLookbackCount+wLookaheadCount+1 frames.
*                            The current frame (n) would be at 
*                            lpaFrames[wLookbackCount].
*
* Output:  pvciCodecIOBlk  - an updated I/O block containing synthesized speech
*                            in PCM format, with the number of samples present
*                            indicated by wSamplesInPCMBuffer.
*          pDecodeMemBlk -   updated codec structure
*          pvciQoTMemBlk -   updated QoT structure
*
* Globals:   none
*
* Return:    VCI error code
*******************************************************************************/


VCI_RETCODE vciQoTDecodeSC(void               *pDecodeMemBlk, 
                           VCI_CODEC_IO_BLOCK *pvciCodecIOBlk,
                           void               *pvciQoTMemBlk, 
                           short              wLookaheadCount,
                           short              wLookbackCount, 
                           const QoTFrame     *lpaFrames);

/******************************************************************************
* Function:  vciQoTLookSetup() 
*
* Action:    Set up the optimal number of look-ahead and look-back frames
*
* Input:    hDecodeMemBlk - handle to the Decoder Memory block
*
* Output:    wLookaheadCount - the recommended number of look-ahead frames
*            wLookbackCount  - the recommended number of look-back frames
*
* Globals:    none
*
* Return:     VCI_RETCODE : VCI return code
*******************************************************************************/


VCI_RETCODE vciQoTLookSetupSC(void  *hDecodeMemBlk, 
                              short *wLookaheadCount,
                              short *wLookbackCount );


/******************************************************************************
* Function:  vciInitQoTSC()
*
* Action:    initialize the QoT structure
*
* Input:    hQoTMemBlk -- pointer to the structure be intialized
*           pvCodecMemBlk -- codec memory structure with initialized pVciMethodStruct.
*
* Output:    hQoTMemBlk -- the initialized QoT structure
*
* Globals:    none
*
* Return:     VCI_RETCODE : VCI return code
*******************************************************************************/

VCI_RETCODE vciInitQoTSC(void **hQoTMemBlk, 
                         void *pvCodecMemBlk);


/******************************************************************************
* Function:  vciFreeQoTSC()
*
* Action:    free the memory of the QoT structure
*
* Input:    hQoTMemBlk -- pointer to the structure 
*
* Output:    hQoTMemBlk -- the memory freed QoT structure
*
* Globals:    none
*
* Return:     VCI_RETCODE : VCI return code
*******************************************************************************/

VCI_RETCODE vciFreeQoTSC(void **hQoTMemBlk);
 
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _VCI_QOT_SC */
