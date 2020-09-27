/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		agcva1.h
 *  Content:	Concrete class that implements CAutoGainControl
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *  12/01/99	pnewson Created it
 *  01/31/2000	pnewson re-add support for absence of DVCLIENTCONFIG_AUTOSENSITIVITY flag
 *  03/03/2000	rodtoll	Updated to handle alternative gamevoice build.   
 *  04/25/2000  pnewson Fix to improve responsiveness of AGC when volume level too low
 *
 ***************************************************************************/

#ifndef _VOICEENCODER_H_
#define _VOICEENCODER_H_

#define VOICE_MESSAGE_TYPE_SILENCE 0x00
#define VOICE_MESSAGE_TYPE_VOICE   0x01

typedef struct _VOICE_ENCODER_HEADER
{
	BYTE	bType;				// packet type
	BYTE	bMsgNum;			// Message # for message
	WORD	wSeqNum;			// Sequence # for message

} VOICE_ENCODER_HEADER, *PVOICE_ENCODER_HEADER;

#endif


