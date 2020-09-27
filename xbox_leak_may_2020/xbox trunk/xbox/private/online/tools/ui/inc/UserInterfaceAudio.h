///////////////////////////////////////////////////////////////////////////////
//
// Name: UserInterfaceAudio.h
//
// Author: Dan Rosenstein (danrose)
//
// Description: the Interface for the audio portion of the UI
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include <xtl.h>
#include <xdbg.h>
#include "wavemedia.h"

//
// the number of audio files
//

static const DWORD NUM_AUDIO_FILES = 4;

//
// the audio files
//

static LPSTR AUDIO_FILES[] = { "d:\\media\\sounds\\back_button.wav",
							   "d:\\media\\sounds\\keyboard_stroke.wav",
							   "d:\\media\\sounds\\menu_item_select.wav",
							   "d:\\media\\sounds\\select_button.wav" };

//
// enum for audio sounds
//

static const DWORD SOUND_BACK_BUTTON     = 0;
static const DWORD SOUND_KEYBOARD_STROKE = 1;
static const DWORD SOUND_MENU_ITEM       = 2;
static const DWORD SOUND_SELECT_BUTTON   = 3;

//
// decleration of audio ui class
//

class CUserInterfaceAudio {

public:

	//
	// Constructor and destructor for UI audio
	//

	CUserInterfaceAudio( void );
	~CUserInterfaceAudio( void );

	//
	// methods to init and play audio
	//

	HRESULT Initialize( void );

	HRESULT PlayBackButton()     { return Play( SOUND_BACK_BUTTON     ); };
	HRESULT PlayKeyboardStroke() { return Play( SOUND_KEYBOARD_STROKE ); };
	HRESULT PlayMenuItem()       { return Play( SOUND_MENU_ITEM       ); };
	HRESULT PlaySelectButton()   { return Play( SOUND_SELECT_BUTTON   ); };

private:

	//
	// internal play method
	//

	HRESULT Play( DWORD dwSound );

	//
	// the dsound object
	//

	LPDIRECTSOUND m_pDSound;

	//
	// the wave array
	//

	CWaveMedia m_sounds[NUM_AUDIO_FILES];

};
