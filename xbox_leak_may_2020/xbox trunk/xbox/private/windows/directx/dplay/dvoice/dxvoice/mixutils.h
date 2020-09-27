/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		MixerUtils.h
 *  Content:	Base class for dp/dnet abstraction
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 07/06/99		rodtoll	Utility functions for mixing audio 
 *
 ***************************************************************************/

#ifndef __MIXERUTILS_H
#define __MIXERUTILS_H

void FillBufferWithSilence( LONG *buffer, LONG frameSize );
void MixInBuffer( LONG *mixerBuffer, BYTE *sourceBuffer, LONG frameSize );
void NormalizeBuffer( BYTE *targetBuffer, LONG *mixerBuffer, LONG frameSize );

#endif

