/*==========================================================================
 *
 *  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:		dvengine.cpp
 *  Content:	Implementation of CDirectVoiceEngine's static functions
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 * 07/19/99		rodtoll	Created
 * 07/29/99		rodtoll	Added static members to load default settings
 * 08/10/99		rodtoll	Removed the TODOs
 * 08/25/99		rodtoll	General Cleanup/Modifications to support new 
 *						compression sub-system.  
 *						Added default parameter reads from the registry
 * 08/30/99		rodtoll	Distinguish between primary buffer format and
 *						playback format.
 *						Changed playback format to be 8Khz, 16Bit mono
 * 10/05/99		rodtoll	Additional comments/DPFs
 * 10/07/99		rodtoll	Updated to work in Unicode 
 * 02/08/2000	rodtoll	Bug #131496 - Selecting DVSENSITIVITY_DEFAULT results in voice
 *						never being detected
 * 03/03/2000	rodtoll	Updated to handle alternative gamevoice build.   
 * 04/21/2000  rodtoll   Bug #32889 - Does not run on Win2k on non-admin account
 * 04/24/2000   rodtoll Bug #33203 - Aureal Vortex plays back at the wrong rate
 * 07/12/2000	rodtoll Bug #31468 - Add diagnostic spew to logfile to show what is failing the HW Wizard
 *
 ***************************************************************************/
#include "dvengine.h"
#include "dvshared.h"
#include "sndutils.h"
#include "in_core.h"

// Registry settings and their defaults
#define DPVOICE_DEFAULT_DEFAULTAGGRESSIVENESS		15

#define DPVOICE_DEFAULT_DEFAULTQUALITY				15

//
// BUGBUG Tune this value OR tune the VA autoactivation parameters
//

#define DPVOICE_DEFAULT_DEFAULTSENSITIVITY			15

#define DPVOICE_DEFAULT_ASO							FALSE


#define DPVOICE_DEFAULT_PRIMARYFORMAT				CreateWaveFormat( WAVE_FORMAT_PCM, FALSE, 8000, 16 )



// Initialize static member variables

DWORD CDirectVoiceEngine::s_dwCompressedFrameSize = 0;
DWORD CDirectVoiceEngine::s_dwUnCompressedFrameSize = 0;
DWORD CDirectVoiceEngine::s_dwDefaultSensitivity = DPVOICE_DEFAULT_DEFAULTSENSITIVITY;

PWAVEFORMATEX CDirectVoiceEngine::s_lpwfxPrimaryFormat = DPVOICE_DEFAULT_PRIMARYFORMAT;

#undef DPF_MODNAME
#define DPF_MODNAME "CDirectVoiceEngine::Startup"
//
// Startup
//
// Called to load global settings and compression info from the registry.  
//
HRESULT CDirectVoiceEngine::Startup()
{
	HRESULT hr;

    s_dwDefaultSensitivity = DPVOICE_DEFAULT_DEFAULTSENSITIVITY;
    s_lpwfxPrimaryFormat = DPVOICE_DEFAULT_PRIMARYFORMAT;

    s_dwCompressedFrameSize = 0;
    s_dwUnCompressedFrameSize = 0; 
    ASSERT(CDirectVoiceEngine::s_lpwfxPrimaryFormat);


	return DV_OK;
}

