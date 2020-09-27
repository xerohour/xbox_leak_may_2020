//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
// Author: Natalyp
//-------------------------------------------------------------
#include "std.h"
#include "Scene.h"
#include "tgl.h"
#include "XBInput.h"
#include "globals.h"
#include "NetConfig.h"

static const char szProgressBar[] = "progress bar";
static const char szTextFrame[]="TEXT_PANEL";
static const TCHAR szPanelTextId[]=_T("NTS_TEXT_PANEL");


//-------------------------------------------------------------
// Function name	: CNTS_ConfigChoice::Initialize
// Description	    : Hide buttons irrelevant for this screen
// Return type		: HRESULT 
// Argument         : char* pUrl
// Argument         : char* pButtonTextName
// Argument         : eSceneId eCurSceneId
// Argument         : bool bAutoTest
//-------------------------------------------------------------
HRESULT CNTS_ConfigChoice::Initialize(char* pUrl, char* pButtonTextName,  eSceneId eCurSceneId, bool bAutoTest)
{
	HRESULT hr = CCellWallScene::Initialize(pUrl, pButtonTextName, eCurSceneId, bAutoTest);
	if(FAILED(hr))
	{
		return hr;
	}
	// TBD change focus if necessary
	return hr;
}

HRESULT CNTS_ConfigChoice::FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime )
{
	if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_A] || GamepadInput.wPressedButtons & XINPUT_GAMEPAD_START) 
	{
		if(m_pButtons[m_nActiveButtonIndex]->GetButtonId() == eNTS_ConfigChoice01)
		{
			// kick network discovery process from the beginning
			g_NetConfig.ResetNetworkState();
		}
	}
	return CCellWallScene::FrameMove(GamepadInput, elapsedTime );
}


