////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

//
// FILE:       library\hardware\mpgcodec\generic\mpgcodec.h
// AUTHOR:		Martin Stephan
// COPYRIGHT:	(c) 1999 Viona Development GmbH.  All Rights Reserved.
// CREATED:		22.11.1999
//
// PURPOSE: 	shared definitions for MPEG2 encoder and decoder --- Header file
//					most of these definitions were previously defined in mpegdec.h
//					and were now moved to mpgcodec.h!
//
// HISTORY:
//

#ifndef MPGCODEC_H
#define MPGCODEC_H


enum MPEGCommand {mpc_none, 	  		// 0x0
                  mpc_start,    		// 0x1
                  mpc_play,     		// 0x2
                  mpc_seek,     		// 0x3
                  mpc_stop,     		// 0x4
                  mpc_step,     		// 0x5
                  mpc_freeze,   		// 0x6
                  mpc_resync,   		// 0x7
                  mpc_resyncue, 		// 0x8
                  mpc_cue,				// 0x9
                  mpc_end,				// 0xA
                  mpc_params,			// 0xB
                  mpc_stepkey,		// 0xC
                  mpc_scan,			// 0xD
						mpc_reverse,		// 0xE
						mpc_trickplay,		// 0xF
						mpc_seekaudio,		// 0x10
						mpc_resyncaudio,	// 0x11
						mpc_audio_test		// 0x12	Audio tests : pink noise, test tone
						};

enum MPEGState   {mps_reset, mps_preempted, mps_initial,
                  mps_frozen, mps_playing, mps_stepping,
                  mps_seeking, mps_resyncing, mps_stopped,
                  mps_scanning, mps_trickplaying, mps_testing,
						mps_capturing};

enum MPEGElementaryStreamType
	{
	MST_VIDEO,
	MST_AUDIO,
	MST_SUBPICTURE
	};


#endif // MPGCODEC_H
