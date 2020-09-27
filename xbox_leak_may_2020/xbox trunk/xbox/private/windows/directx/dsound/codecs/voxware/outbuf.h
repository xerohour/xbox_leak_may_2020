/*******************************************************************************
*                        Voxware Proprietary Material                          *
*                        Copyright 1996, Voxware, Inc                          *
*                            All Rights Reserved                               *
*                                                                              *
*                       DISTRIBUTION PROHIBITED without                        *
*                      written authorization from Voxware                      *
*******************************************************************************/

/*******************************************************************************
* Filename:       OutBuf.h                                                          
*                                                                              
* Purpose:        Prepare all frame parameters for the current frame or subframe                                                            
*                                                                              
* Author/Date:    Rob Zopf  03/01/96                                                             
********************************************************************************
* $Header:   /export/phobos/pvcs/r_and_d/archives/realtime/decode/OutBuf.h_v   1.0   26 Jan 1998 10:52:08   johnp  $                                                                     
*******************************************************************************/
#ifndef OUTBUF_H
#define OUTBUF_H

typedef struct tagOutputBuffer
{
   short                        *outBufferB1;           /* double-buffered output 1 */
   short                        *outBufferB2;           /* double-buffered output 2 */
   short                        dataB;                  /* amount of data currently buffered */
   short                        *outBufferB;            /* pointer to current buffer (B1 or B2 */
   short                        dataAstart;             /* start of left over data in A */
   short                        dataAend;               /* end of left over data in A */
   short                        writeBuffer;            /* indicates if there is a full buffer */

} OutputBuffer;

unsigned short VoxInitOutputBuffers(void **OutputBuffer_mblk);

unsigned short VoxFreeOutputBuffers(void **OutputBuffer_mblk);

unsigned short VoxOutputManager(void *OutputBuffer_mblk, int outLen, float *InBuf);

#endif /* OUTBUF_H */

