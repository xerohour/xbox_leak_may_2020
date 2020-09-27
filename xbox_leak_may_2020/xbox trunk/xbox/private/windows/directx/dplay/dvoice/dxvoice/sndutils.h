/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		sndutils.h
 *  Content:	Declares sound related untility functions
 *		
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 07/16/99		rodtoll	Created
 * 09/01/2000   georgioc started port to xbox
 ***************************************************************************/

//
// This module contains the definition of sound relatedt utility
// functions.  Functions in this module manipulate WAVEFORMATEX
// structures and provide full duplex initialization / testing
// facilities.
//
// This module also contains the routines used to measure peak
// of an audio buffer and for voice activation.
//
//
#ifndef __SOUNDUTILS_H
#define __SOUNDUTILS_H

BYTE FindPeak( BYTE *data, DWORD frameSize, BOOL eightBit );    
WAVEFORMATEX *CreateWaveFormat( short formatTag, BOOL stereo, int hz, int bits );

#endif
