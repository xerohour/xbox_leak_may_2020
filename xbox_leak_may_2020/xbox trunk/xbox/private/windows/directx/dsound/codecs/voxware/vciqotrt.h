/******************************************************************************
 *                                                                            *
 *                      Voxware Proprietary Material                          *
 *                      Copyright 1996, Voxware, Inc.                         *
 *                           All Rights Resrved                               *
 *                                                                            *
 *                     DISTRIBUTION PROHIBITED without                        *
 *                   written authorization from Voxware                       *
 *                                                                            *
 *****************************************************************************/

/******************************************************************************
 *                                                                            *
 * FILENAME:  vciQoT.h                                                        *
 * PURPOSE:   QoT structures                                                  *
 * AUTHOR:    Robert Zopf 08/18/97                                            *
 *                                                                            *
 * $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/vci/vciQoTRT.h_v   1.0   26 Jan 1998 11:21:10   johnp  $
 *
 *****************************************************************************/
#ifndef _VCI_QOT_RT
#define _VCI_QOT_RT
/*---------------------------------------------------------------------------
                    P U B L I C   S T R U C T U R E S 
  ---------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif

VCI_RETCODE vciQoTLookSetupRT(
                  void  *hDecodeMemBlk,
                  short *wLookaheadCount,
                  short *wLookbackCount );

VCI_RETCODE vciQoTDecodeRT(
                  void *pDecodeMemBlk,
                  VCI_CODEC_IO_BLOCK *pvciCodecIOBlk,
                  void *pvciQoTMemBlk,
                  short  wLookaheadCount,
                  short  wLookbackCount,
                  QoTFrame *lpaFrames );

VCI_RETCODE vciInitQoTRT(void **hQoTMemBlk, void *pvCodecMemBlk);
VCI_RETCODE vciFreeQoTRT(void **hQoTMemBlk);
 
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _VCI_QOT_RT */
