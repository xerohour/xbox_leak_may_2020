//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
// Author: Natalyp
//-------------------------------------------------------------
#pragma once
#include "std.h"
#include "Scene.h"
#include "tgl.h"
#include "materiallibrary.h"
#include "globals.h"
#include "NetConfig.h"
#include "XBInput.h"

static const char szIconFail[] = "Icon_fail";
static const char szIconPass[] = "Icon_pass";
static const char szIconDisable[] = "Icon_untested";

struct ButtonInfo
{
	TCHAR* pText;
	TCHAR* pHelpText;
};



static ButtonInfo sButtonsInfo[] =
{
	{/*szIconFail,*/ _T("STAT_F_CABLE"), _T("HELP_STAT_F_CABLE")},
	{/*szIconPass,*/ _T("STAT_S_CABLE"), _T("HELP_STAT_S_CABLE")},
	{/*szIconDisable,*/ _T("STAT_F_PENDING"), _T("HELP_STAT_F_PENDING")},

	{/*szIconFail,*/ _T("STAT_F_DHCP"), _T("HELP_STAT_F_DHCP")},
	{/*szIconPass,*/ _T("STAT_S_DHCP"), _T("HELP_STAT_S_DHCP")},
	{/*szIconDisable,*/ _T("STAT_F_PENDING"), _T("HELP_STAT_F_PENDING")},

	{/*szIconFail,*/ _T("STAT_F_PPPOE"), _T("HELP_STAT_F_PPPOE")},
	{/*szIconPass,*/ _T("STAT_S_PPPOE"), _T("HELP_STAT_S_PPPOE")},
	{/*szIconDisable,*/ _T("STAT_F_PENDING"), _T("HELP_STAT_F_PENDING")},

	{/*szIconFail,*/ _T("STAT_F_DNS"), _T("HELP_STAT_F_DNS")},
	{/*szIconPass,*/ _T("STAT_S_DNS"), _T("HELP_STAT_S_DNS")},
	{/*szIconDisable,*/ _T("STAT_F_PENDING"), _T("HELP_STAT_F_PENDING")},

	{/*szIconFail,*/ _T("STAT_F_SERVICES"), _T("HELP_STAT_F_SERVICES")},
	{/*szIconPass,*/ _T("STAT_S_SERVICES"), _T("HELP_STAT_S_SERVICES")},
	{/*szIconDisable,*/ _T("STAT_F_PENDING"), _T("HELP_STAT_F_PENDING")},


};


//-------------------------------------------------------------
// Function name	: CNTS_StatusScene::Initialize
// Description	    : 
// Return type		: HRESULT 
// Argument         : char* pUrl
// Argument         : char* pButtonTextName
// Argument         : eSceneId eCurSceneId
// Argument         : bool bAutoTest
//-------------------------------------------------------------
HRESULT CNTS_StatusScene::Initialize(char* pUrl, char* pButtonTextName,  eSceneId eCurSceneId, bool bAutoTest)
{
	HRESULT hr = CNTS_SettingsScene::Initialize(pUrl, pButtonTextName, eCurSceneId, bAutoTest);
	if(FAILED(hr))
	{
		DbgPrint("CNTS_StatusScene::Initialize - failed to init base class hr = 0x%x\n", hr);
		return hr;
	}

	for(size_t i = 0; i<m_pButtons.size(); i++)
	{
		SetIcon(i,eDisableIcon);
	}
	m_nNetworkStatus = -1;
	m_bFinalState = false;
	UpdateScene();
	return hr;
}



//-------------------------------------------------------------
// Function name	: CNTS_StatusScene::SetIcon
// Description	    : Choose on of the three available icons(pass, fail, pending)
//                    and hide the rest two icons
// Return type		: void 
// Argument         : int nIndex
// Argument         : long eStatus
// Argument         : bool bDHCP
//-------------------------------------------------------------
void CNTS_StatusScene::SetIcon(int nIndex, long eStatus, bool bDHCP/*=true*/, bool bShowText/*=true*/)
{
	ASSERT(nIndex >= 0 && nIndex < (int) m_pButtons.size());
	// first four buttons are dynamic
	if(nIndex < 0 || nIndex >= 4)
	{
		return;
	}
	int nButtonInfoIndex = 0;
	switch (nIndex)
	{
	case 0:
		nButtonInfoIndex = 0;
		break;
	case 1:
		bDHCP? nButtonInfoIndex = 1 : nButtonInfoIndex = 2;
		break;
	case 2:
    case 3:
		nButtonInfoIndex = nIndex+1;
		break;
	default:
		ASSERT(false);
	}

	nButtonInfoIndex = nButtonInfoIndex*3+eStatus;
	TG_Shape* pRoot = const_cast<TG_Shape*> (m_pButtons[nIndex]->GetShape());
	TG_Shape* pFailIcon = pRoot->FindObject(szIconFail);
	TG_Shape* pSuccessIcon = pRoot->FindObject(szIconPass);
	TG_Shape* pDisableIcon = pRoot->FindObject(szIconDisable);
	ASSERT(pFailIcon && pSuccessIcon && pDisableIcon);
	if(!pFailIcon || !pSuccessIcon || !pDisableIcon)
	{
		return;
	}

	pFailIcon->setVisible(false);
	pSuccessIcon->setVisible(false);
	pDisableIcon->setVisible(false);

	switch (eStatus)
	{
	case eFailIcon:
		pFailIcon->setVisible(true);
		break;
	case eSuccessIcon:
		pSuccessIcon->setVisible(true);
		break;
	case eDisableIcon:
		if(bShowText)
		{
			pDisableIcon->setVisible(true);
		}
		break;
	default:
		ASSERT(0);
	}

	m_pButtons[nIndex]->SetTextId(sButtonsInfo[nButtonInfoIndex].pText);
	m_pButtons[nIndex]->SetHelpText(sButtonsInfo[nButtonInfoIndex].pHelpText);
	CText* pText = const_cast<CText*> (m_pButtons[nIndex]->GetTextObject());
	pText -> Show(bShowText);

}


//-------------------------------------------------------------
// Function name	: CNTS_StatusScene::FrameMove
// Description	    : Get the current network status, change the icons and focus as appropriate                 
// Return type		: HRESULT 
// Argument         : XBGAMEPAD& GamepadInput
// Argument         : float elapsedTime
//-------------------------------------------------------------
HRESULT CNTS_StatusScene::FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime )
{
	UpdateScene();
	// while we are not done, allow to navigate back only
	if(!m_bFinalState)
	{
		if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_B] ||
		   GamepadInput.wPressedButtons & XINPUT_GAMEPAD_BACK)	
		 
		{
			return CNTS_SettingsScene::FrameMove(GamepadInput, elapsedTime);		
		}

		AutoTest(GamepadInput);
		CCellWallScene::CellWallRotate(GamepadInput);
		return S_OK;
	}
	return CNTS_SettingsScene::FrameMove(GamepadInput, elapsedTime);
}


//-------------------------------------------------------------
// Function name	: CNTS_StatusScene::UpdateScene
// Description	    : discover current Network state and update 
//                    presentation if necessary
//					  For a transit state (e.g eCablePass) hide all the following stages data
// Return type		: void 
//-------------------------------------------------------------
void CNTS_StatusScene::UpdateScene()
{
	eNetworkState eCurState = g_NetConfig.CheckNetworkStatus();
	if (m_nNetworkStatus != eCurState)
	{
		m_nNetworkStatus = eCurState;
		for(size_t i = 0; i<m_pButtons.size(); i++)
		{
			 SetIcon(i,eDisableIcon, true, false);
		}
		switch(eCurState)
		{
		case eCableFail: // final state
			for(size_t i=0; i< m_pButtons.size(); i++)
			{
				SetIcon(i, eFailIcon);
			}
			DbgPrint ("CNTS_StatusScene::UpdateScene - network state: Cable fail\n");
			break;

		case eCablePass: // transit state, continue
			SetIcon(0, eSuccessIcon);
			CPrimitiveScene::SetActiveButton(eNTS_Status02);
			SetIcon(1,eDisableIcon);
			DbgPrint ("CNTS_StatusScene::UpdateScene - network state: Cable pass\n");
			break;

		case eIPPass: // transit state, continue
			SetIcon(0, eSuccessIcon);
			SetIcon(1, eSuccessIcon);
			SetIcon(2,eDisableIcon);
			CPrimitiveScene::SetActiveButton(eNTS_Status03);
			DbgPrint ("CNTS_StatusScene::UpdateScene - network state: IP pass\n");
			break;

		case eDNSFail: // final state
			CPrimitiveScene::SetActiveButton(eNTS_Status03); // highlight the failed state
			SetIcon(0, eSuccessIcon);
			SetIcon(1, eSuccessIcon);
			SetIcon(2, eFailIcon);
			SetIcon(3, eFailIcon);
			m_bFinalState = true;
			DbgPrint ("CNTS_StatusScene::UpdateScene - network state: DNS fail\n");
			break;

		case eServiceFail:  // final state
			CPrimitiveScene::SetActiveButton(eNTS_Status04);
			SetIcon(0, eSuccessIcon);
			SetIcon(1, eSuccessIcon);
			SetIcon(2, eSuccessIcon);
			SetIcon(3, eFailIcon);
			m_bFinalState = true;
			DbgPrint ("CNTS_StatusScene::UpdateScene - network state: Service fail\n");
			break;

		case eDone:  //final state 
			CPrimitiveScene::SetActiveButton(eNTS_Status06);
			for(size_t i = 0; i<m_pButtons.size(); i++)
			{
				SetIcon(i,eSuccessIcon);
			}
			m_bFinalState = true;
			DbgPrint ("CNTS_StatusScene::UpdateScene - network state: Connected\n");
			break;
		default:
			break;
		}
		
	}
}

