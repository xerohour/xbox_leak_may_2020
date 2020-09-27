//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
// Author: Natalyp
//-------------------------------------------------------------
#include "std.h"
#include "Scene.h"

#include "NetConfig.h"
#include "XBInput.h"
#include "globals.h"
#include "InputHandle.h"
#include "Dialog.h"
#include "SceneNavigator.h"



//-------------------------------------------------------------
// Function name	: CNTS::SetValue
// Description	    : set value for currently highlighted button
// Return type		: HRESULT 
// Argument         : const TCHAR* szValue
//-------------------------------------------------------------
HRESULT CNTS::SetValue(const TCHAR* szValue)
{
	HRESULT hr = g_NetConfig.SetValue(m_pButtons[m_nActiveButtonIndex]->GetButtonId(), const_cast<TCHAR*> (szValue) );
	if (FAILED(hr))
	{
		return hr;
	}
	const CText* pInput = m_pButtons[m_nActiveButtonIndex]->GetInputTextObject();
    if ( pInput )
    {
	    (const_cast <CText*> (pInput)) ->SetText(szValue);
    }
	return hr;
}


static const TCHAR cszStaticIp[] = _T("STATIC_IP");
static const TCHAR cszDynamicIp[] = _T("DYNAMIC_IP");
static const TCHAR cszContinue[] = _T("CONTINUE");
static const TCHAR cszToggle[] = _T("TOGGLE");


//-------------------------------------------------------------
// Function name	: CNTS::BindData
// Description	    : part of initialization process, retrieve the data from the Data object
// Return type		: HRESULT 
//-------------------------------------------------------------
HRESULT CNTS::BindData()
{
	HRESULT hr = S_OK;
	for(size_t i =0; i<m_pButtons.size(); i++)
	{
		const TCHAR* szValue = g_NetConfig.GetValue(m_pButtons[i]->GetButtonId());
		
		if(m_pButtons[i]->m_eType == eTwoTextButton)
		{
			const CText* pcInput =m_pButtons[i]->GetInputTextObject();
         
			CText* pButtonText = const_cast<CText*> (m_pButtons[i]->GetTextObject());

			CText* pInput = const_cast<CText*> (pcInput); 

			if(_tcsstr(pButtonText->GetTextId(), cszToggle) )
			{ 
				pInput ->SetTextId(szValue);
				SetButtonsState(szValue);
			}
			else if ( pInput )
			{
				pInput->SetText(szValue);
			}
		}
	}
// hide continue button if necessary	
	if (m_bHideContinue)
	{
		int nIndex = FindButtonForTextObj(cszContinue);
		if(nIndex == -1 && m_eSceneId != eNTS_IPId)
		{
			//all screens except of IP should have this button
			ASSERT(false);
		}
		if(nIndex >=0 && nIndex < (int)m_pButtons.size())
		{
			m_pButtons[nIndex]->Show(false);
		}

	}
	return hr;
}


//-------------------------------------------------------------
// Function name	: CNTS::FrameMove
// Description	    : Update an edited previously value, handle toggle Static/Dynamic button
// Return type		: HRESULT 
// Argument         : XBGAMEPAD& GamepadInput
// Argument         : float elapsedTime
//-------------------------------------------------------------
HRESULT CNTS::FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime )
{
	HRESULT hr = S_OK;

	if (g_szUserInput.IsNewInput())
	{
		hr = SetValue(g_szUserInput.GetInputText());
		// do not reset invalid value it'll reappear next time keyboard pops up
		if(FAILED(hr))
		{
			DbgPrint("CNTS::FrameMove - Failed to set value, hr = 0x%x\n", hr);
		}
		else 
		{
			g_szUserInput.ResetInputText();
		}
	}

	bool bActiveToggle = false;
	if(SUCCEEDED(hr))
	{		

		if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_A] || 
		   GamepadInput.wPressedButtons & XINPUT_GAMEPAD_START)
		{
			CText* pText = const_cast <CText*> (m_pButtons[m_nActiveButtonIndex]->GetTextObject());
			CText* pInput = const_cast <CText*> (m_pButtons[m_nActiveButtonIndex]->GetInputTextObject());

			if(_tcsstr(pText->GetTextId(), cszToggle) )
			{
				g_NetConfig.SetValue(m_pButtons[m_nActiveButtonIndex]->GetButtonId(), NULL);
				const TCHAR* szValue= g_NetConfig.GetValue(m_pButtons[m_nActiveButtonIndex]->GetButtonId());
				bActiveToggle = true;
				if(m_pButtons[m_nActiveButtonIndex]->m_eType == eTwoTextButton)
				{
                    if ( pInput )
                    {
					    pInput ->SetTextId(szValue);
                    }
					// TBD change the button from and to disable
					SetButtonsState(szValue);
					// refresh all the fields
					BindData();
				
				}
				else
				{
					// this button must have two texts
					ASSERT(false);
				}
			}
			else // A is pressed set value into the global buffer for keyboard to load
			{
				if(pInput && pInput->GetText())
				{
					g_szUserInput.SetInputText(pInput->GetText());
					// for IP and DNS screens we need to collect IP address, limit the size of input to the IP_ADDRESS_SIZE
					if(m_eSceneId == eNTS_IPId || m_eSceneId == eNTS_DNSId || m_eSceneId == eNTS_DNSTSId)
					{
						g_szUserInput.SetInputMaxLength(IP_ADDRESS_SIZE);
					}
				}
			}
		} // if A is pressed

		

		if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_B] || 
		   GamepadInput.wPressedButtons & XINPUT_GAMEPAD_BACK)
		{
			return CCellWallScene::FrameMove(GamepadInput, elapsedTime);
		}
	}

	if(FAILED(hr))
	{
		AutoTest(GamepadInput);
		RunOKDlg( hr );
		CSceneNavigator::Instance()->MoveNextScene(m_pButtons[m_nActiveButtonIndex]->GetButtonId());
	}

	if( !m_bEditMode || bActiveToggle || FAILED(hr) )
	{
		AutoTest(GamepadInput);
		CCellWallScene::CellWallRotate(GamepadInput);
		hr = CPrimitiveScene::FrameMove(GamepadInput, elapsedTime);
		return hr;
	}

	return CCellWallScene::FrameMove(GamepadInput, elapsedTime);

}


void CNTS::SetButtonsState(const TCHAR* szValue)
{
	eButtonState eState = eButtonDefault;
	if(_tcsicmp(szValue, cszDynamicIp) == 0 )
	{
		eState = eButtonDisabled;
		m_bEditMode = false;
	}
	else
	{
		m_bEditMode = true;
	}
	// the first button is toggle do not disabled it
	for (size_t i = 1; i<m_pButtons.size(); i++)
	{
		m_pButtons[i]->SetState(eState);
	}
}

