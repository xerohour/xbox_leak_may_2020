///////////////////////////////////////////////////////////////////////////////
//
// Name: wavemedia.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the wave media class
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <xdbg.h>

class CWaveMedia
{
public:

	//
    // Constructors and Destructors
	//

    CWaveMedia( void );
    ~CWaveMedia( void );

	//
	// see if sound is playing
	//

    BOOL IsPlaying( void );         
	
	//
	// Play and stop the sound
	//

    HRESULT Play( void );                             
    HRESULT Stop( void );              
	
	//
	// reset the wave to the beginning
	//

    HRESULT ResetToBeginning( void ); 
	
	//
	// set the volume of the wave
	//

    HRESULT SetVolume( LONG lVolume );            
    
	//
	// Initialize the wave file
	//

    HRESULT Initialize( LPSTR szWaveFileName, LONG lDefaultVolume = 0 );

private:

	//
	// Load the WAV file in to our local DSound Buffer
	//

    HRESULT ProcessWaveFile( LPSTR szWaveFileName );                            
	
	//
	// Pointer to the local DSound buffer
	//

	LPDIRECTSOUNDBUFFER m_pDSoundBuffer;  

	//
	// Pointer to our XFileMediaObject file
	//

	XFileMediaObject*   m_pFMOFile;
	
	//
	// XMedia Packet
	//

    XMEDIAPACKET        m_xmp; 	                               
};
