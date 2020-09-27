//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
// Author: Natalyp
//-------------------------------------------------------------
#include "std.h"
#include "scene.h"
#include "XBInput.h"
#include "globals.h"
#include "NetConfig.h"


//-------------------------------------------------------------
// Function name	: CNTS_SettingsScene::FrameMove
// Description	    : Handle vertical(1-4 buttons) navigation and horizontal (5-6) navigation
// Return type		: HRESULT 
// Argument         : XBGAMEPAD& GamepadInput
// Argument         : float elapsedTime
//-------------------------------------------------------------
HRESULT CNTS_SettingsScene::FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime )
{
	ASSERT(m_pButtons.size() == 6);
	bool bCallAutoTest = false;
	if(m_nActiveButtonIndex == 4)
	{
		if (GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT || GamepadInput.fX1 == 1)
		{
			m_bVerticalButtons = false;	
		} 
		else if(GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP || GamepadInput.fY1 == 1)
		{
			m_bVerticalButtons = true;	
		}
		else if ( GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT || GamepadInput.fX1 == -1)
		{
			bCallAutoTest = true;
		}
		else if (GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ||
			GamepadInput.fY1 == -1)
		{
			bCallAutoTest = true;
		}
	}
	else if(m_nActiveButtonIndex == 5)
	{
		m_bVerticalButtons = false;
		// RRRRgh - forced by PM to add it 
		if(GamepadInput.wPressedButtons & XINPUT_GAMEPAD_DPAD_UP || GamepadInput.fY1 == 1)
		{
			m_bVerticalButtons = true; 
			CPrimitiveScene::SetActiveButton(m_eSceneId == eNTS_SettingsId? eNTS_settingsPPPoE : eNTS_Status04);
			bCallAutoTest = true;
		}
	}
	if(bCallAutoTest)
	{
		AutoTest(GamepadInput);
		return S_OK;
	}
	
	// In Settings screen "Connect" saves new configuration to the hard drive, "Cancel" rollback all recent changes
	if(m_eSceneId == eNTS_SettingsId)
	{
		if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_A] || GamepadInput.wPressedButtons & XINPUT_GAMEPAD_START) 
		{
			if(m_nActiveButtonIndex == 4)
			{
				// something wrong with the screen unexpected button
				ASSERT(m_pButtons[m_nActiveButtonIndex]->GetButtonId() == eNTS_settingsConnect);
				g_NetConfig.SaveConfig();
				g_NetConfig.ResetNetworkState();
			}
			else if (m_nActiveButtonIndex == 5)
			{
				// something wrong with the screen unexpected button
				ASSERT(m_pButtons[m_nActiveButtonIndex]->GetButtonId() == eNTS_settingsCancel);
				g_NetConfig.RestoreConfig();
			}
		}
		
		else if (GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_B] || GamepadInput.wPressedButtons & XINPUT_GAMEPAD_BACK) 
		{
			// Back button is pressed, save the changes
			g_NetConfig.SaveConfig();
		}
	}
	return CCellWallScene::FrameMove(GamepadInput, elapsedTime );
}