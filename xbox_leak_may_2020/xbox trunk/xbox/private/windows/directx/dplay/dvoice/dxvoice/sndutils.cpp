/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		micutils.cpp
 *  Content:
 *		This module contains the implementation of sound related utility
 *		functions.  Functions in this module manipulate WAVEFORMATEX
 *		structures and provide full duplex initialization / testing
 *		facilities.
 *
 *		This module also contains the routines used to measure peak
 *		of an audio buffer and for voice activation.
 *		
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 07/16/99		rodtoll	Created
 * 07/30/99		rodtoll	Updated util functions to take GUIDs and allow for 
 *                      users to pre-create capture/playback devices and
 *						pass them into InitXXXXDuplex
 * 08/25/99		rodtoll	General Cleanup/Modifications to support new 
 *						compression sub-system. 
 * 08/30/99		rodtoll	Added new playback format param to sound init 
 * 09/03/99		rodtoll	Fixed return codes on InitFullDuplex
 * 09/20/99		rodtoll	Now checks for invalid GUIDs for playback/record
 * 10/05/99		rodtoll	Added DPF_MODNAMEs
 * 10/29/99		rodtoll	Bug #113726 - Fixed memory leak when full duplex
 *						fails caused by architecture update.
 * 11/12/99		rodtoll	Updated full duplex test to use new abstracted recording
 *						and playback systems.  
 *				rodtoll	Updated to allow passing of sounddeviceconfig flags in dwflags
 *						parameter to init is effected by the flags specified by user
 *				rodtoll	Sound buffers (rec and playback) are now set to silence before
 *						recording/playback is started.
 * 11/22/99		rodtoll	Removed unnessessary set of recording buffer to silence.
 * 12/01/99		rodtoll	Bug #115783 - Will always adjust volume of default device
 *						Updated for new parameters added by above bug.
 * 12/08/99		rodtoll	Bug #121054 - Support for capture focus and removed flags
 *						from buffer, allow dsound to manage buffer location.
 * 01/21/2000	pnewson	Fixed error cleanup code in InitHalfDuplex
 * 01/27/2000	rodtoll	Updated tests to accept buffer descriptions and play flags/priority  
 * 02/10/2000	rodtoll	Removed more capture focus
 * 02/23/2000	rodtoll	Fix to allow to run on dsound7.  
 * 05/19/2000   rodtoll Bug #35395 - Unable to run two copies of DPVHELP on same system without 
 *                      DirectX 8 installed.
 * 06/21/2000	rodtoll Bug #35767 - Must implement ability to use effects processing on voice buffers
 *						Updated sound initialization routines to handle buffers being passed in.
 * 07/12/2000	rodtoll Bug #31468 - Add diagnostic spew to logfile to show what is failing the HW Wizard
 *
 ***************************************************************************/
#define NODSOUND
#include "dvntos.h"
#include "dsoundp.h"

#include "sndutils.h"
#include "in_core.h"

#undef DPF_MODNAME
#define DPF_MODNAME "FindPeak16Bit"
// FindPeak16Bit
//
// This function determines what the peak for a buffer
// of 16 bit audio is.  Peak is defined as the loudest 
// sample in a set of audio data rated on a scale of 
// between 0 and 100.  
//
// Parameters:
// BYTE *data -
//		Pointer to the buffer containing the audio data
//      to find the peak of. 
// DWORD frameSize -
//		The size in bytes of the audio data we are
//      checking.
//
// Returns:
// BYTE -
// The peak of the audio buffer, a value between 0 and 100.
//		
BYTE FindPeak16Bit( short *data, DWORD frameSize )
{
    int peak,       
        tmpData;

    frameSize /= 2;
    peak = 0;
    
    for( int index = 0; index < (LONG)frameSize; index++ )
    {
        tmpData = data[index];

        if( tmpData < 0 )
        {
            tmpData *= -1;
        }

        if( tmpData > peak )
        {
            peak = tmpData;
        }
    }

    tmpData = (peak * 100) / 0x7FFF;

    return (BYTE) tmpData;
}

#undef DPF_MODNAME
#define DPF_MODNAME "FindPeak"
// FindPeak
//
// This function determines what the peak for a buffer
// of 8 or 16 bit audio is.  Peak is defined as the loudest 
// sample in a set of audio data rated on a scale of 
// between 0 and 100.  
//
// Parameters:
// BYTE *data -
//		Pointer to the buffer containing the audio data
//      to find the peak of. 
// DWORD frameSize -
//		The size in bytes of the audio data we are
//      checking.
// BOOL eightBit -
//		Determins if the buffer is 8 bit or not.  Set to 
//      TRUE for 8 bit data, FALSE for 16 bit data.  
//
// Returns:
// BYTE -
// The peak of the audio buffer, a value between 0 and 100.
//		
BYTE FindPeak( BYTE *data, DWORD frameSize, BOOL eightBit )
{
    return FindPeak16Bit( (signed short *) data, frameSize );
}


#undef DPF_MODNAME
#define DPF_MODNAME "CreateWaveFormat"
// CreateWaveFormat
//
// This utility function is used to allocate and fill WAVEFORMATEX 
// structures for the various formats used. This function
// currently supports the following formats:
//
// WAVE_FORMAT_DSPGROUP_TRUESPEECH
// WAVE_FORMAT_GSM610
// WAVE_FORMAT_LH_CODEC
// WAVE_FORMAT_PCM
//
// The function will allocate the required memory for the sturcture
// (including extra bytes) as required by the format and will fill
// in all the members of the sturcture.  The structure which is 
// returned belongs to the caller and must be deallocated by the
// caller.
//
// Parameters:
// short formatTag -
//		The format tag for the wav format.
//
// BOOL stereo -
//		Specify TRUE for stereo, FALSE for mono
//
// int hz - 
//		Specify the sampling rate of the format.  E.g. 22050
//
// int bits - 
//		Specify the number of bits / sample.  E.g. 8 or 16
//
// Returns:
// WAVEFORMATEX * - 
//		A pointer to a newly allocated WAVEFORMATEX structure 
//      for the specified format, or NULL if format is not supported
//
WAVEFORMATEX *CreateWaveFormat( short formatTag, BOOL stereo, int hz, int bits ) 
{

	switch( formatTag ) {
	case WAVE_FORMAT_PCM:
		{
			WAVEFORMATEX *format		= new WAVEFORMATEX;

            if( format == NULL )
            {
				goto EXIT_MEMALLOC_CREATEWAV;            
			}
			
			format->wFormatTag			= WAVE_FORMAT_PCM;
			format->nSamplesPerSec		= hz;
			format->nChannels			= (stereo) ? 2 : 1;
			format->wBitsPerSample		= (WORD) bits;
			format->nBlockAlign			= (bits * format->nChannels / 8);
			format->nAvgBytesPerSec		= format->nSamplesPerSec * format->nBlockAlign;
			format->cbSize				= 0;
			return format;
		}
		break;
    default:
        ASSERT( TRUE );
	}

EXIT_MEMALLOC_CREATEWAV:

	DPVF( DPVF_ERRORLEVEL, "Unable to alloc buffer for waveformat, or invalid format" );
	return NULL;
}

