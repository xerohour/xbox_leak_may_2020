#include "std.h"
#include "SceneNavigator.h"
#include "XOConst.h"
#include "Scene.h"
#include "utilities.h"
#include "AccountData.h"
#include "NavigationMap.cpp"
#include "NavigationDesc.cpp"
#include "NavigationExit.cpp"

#include "MsgDialog.h"
#include "xlaunch.h"

// for DbgPrint buffer 
#define MAX_BUF_LEN 200 
extern "C" PLAUNCH_DATA_PAGE* LaunchDataPage;

CSceneNavigator::CSceneNavigator() :
	m_pCurrentScene (NULL),
	m_nStackTop(0),
	m_eCurrentSceneId (eNullSceneId),
	m_bLaunchFromTitle(false),
	m_dwTitleID(0),
    m_dwLaunchReason(0),
    m_dwLaunchContext(0),
    m_dwLaunchParameter1(0),
    m_dwLaunchParameter2(0)
	{}



HRESULT CSceneNavigator::CreateScene(eSceneId eNewScene, bool bCreateNew)
{
	HRESULT hr = S_OK;
	
	CPrimitiveScene* pNewScene = NULL;
	if(bCreateNew)
	{
		// little class factory for different scenes  
		switch(eNewScene)
		{
		case eKeyboardSceneId:
			pNewScene = new CKeyboardScene;
			break;
		case eKeypadSceneId:
			pNewScene = new CKeypadScene(false);
			break;
		case eKeypadPeriodSceneId:
			pNewScene = new CKeypadScene(true);
			break;
		case eCellWallSceneId:
		case eNTS_CableHelpId:
		case eNTS_PPPoEHelpId:
		case eNTS_IPHelpId:
		case eNTS_DNSHelpId:
		case eNTS_ServicesHelpId:
			pNewScene = new CCellWallScene;
			break;
		case eNTS_SettingsId:
			pNewScene = new CNTS_SettingsScene;
			break;
		case eNTS_StatusId:
			pNewScene = new CNTS_StatusScene;
			break;
		case eNTS_HostnameId:
		case eNTS_PPPoEId:
		case eNTS_IPId:
		case eNTS_DNSId:
			pNewScene = new CNTS;
			break;
		case eNTS_HostnameTSId:
		case eNTS_PPPoETSId:
		case eNTS_DNSTSId:
			pNewScene = new CNTS(false);
			break;
        case eACT_OnlineId:
            pNewScene = new CACT_OnlineScene;
            break;
        case eMsgBoxSceneId:
            pNewScene = new CMsgDialog;
            break;
        case eProgressBarSceneId:
            pNewScene = new CProgressDialog;
            break;
		case eNTS_ConfigChoiceId:
			pNewScene = new CNTS_ConfigChoice;
			break;
        case eACT_IdId:
        case eACT_CreditId:
        case eACT_CountryId:
        case eACT_Billing_CountryId:
        case eACT_BillingId:
        case eGEN_Exp_TumblersId:
        case eGen_Large_PanelId:
        case eGEN_Dob_TumblersId:
        case eGEN_Welcome_PanelId:
            pNewScene = new CACT_SignupAccount;
            break;
        case eAnimWait:
        case eConnectAnim:
            pNewScene = new CWaitCursor;
            break;
		default:
			pNewScene = new CScene;

		};
		
		DbgPrint("Moving to %s Scene, url %s\n", SceneName[eNewScene], m_SceneDataMap[eNewScene].m_url);
		if(!pNewScene)
		{
			hr = E_OUTOFMEMORY;
			return hr;
		}

		hr = pNewScene->Initialize(m_SceneDataMap[eNewScene].m_url, m_SceneDataMap[eNewScene].m_buttonTextUrl,  eNewScene);
		if(FAILED(hr))
		{
			DbgPrint("CSceneNavigator::Initialize - failed to Init the Scene %s", SceneName[eNewScene]);
			return hr;
		}
	}
	else
	{
		pNewScene = m_SceneDataMap[m_FlowStack[m_nStackTop]].m_pScene;
	}

	// remove previous scene if exists 
	if(m_pCurrentScene)
	{
		// if the current scene is not visible on deactive (i.e. no other screens supposed to be on top)
		//  or if it is visible, but we move
		// to any scene that is not keyboard, keypad or dialog, we need to release the current scene
		if(!m_SceneDataMap[m_eCurrentSceneId].m_bVisibleOnDeactive ||
			
			(m_SceneDataMap[m_eCurrentSceneId].m_bVisibleOnDeactive && 
            !IsDialogScene( eNewScene ) )
			)
		{	
			delete m_pCurrentScene;
			m_SceneDataMap[m_eCurrentSceneId].m_pScene = NULL;
		}
		else
		{
			// the only scenes which are allowed to be on top of another one
			if(!IsDialogScene( eNewScene ))
			{
				ASSERT(false);
			}
		}
	}

	m_pCurrentScene = pNewScene;
	m_eCurrentSceneId = eNewScene;
	m_SceneDataMap[eNewScene].m_pScene = m_pCurrentScene;
	
	return hr;

}

HRESULT CSceneNavigator::Initialize()
{
	HRESULT hr = S_OK;
	m_nStackTop = 0;

	DWORD dwLaunchType = 0 ;
	LAUNCH_DATA ld;
	ZeroMemory(&ld, sizeof(ld));

// Retrieve the launch data, if any
    if (*LaunchDataPage && (*LaunchDataPage)->Header.dwLaunchDataType == LDT_LAUNCH_DASHBOARD) 
	{
        PLD_LAUNCH_DASHBOARD pLaunchDashboard =
            (PLD_LAUNCH_DASHBOARD) ((*LaunchDataPage)->LaunchData);

        m_dwTitleID = (*LaunchDataPage)->Header.dwTitleId;
        m_dwLaunchReason = pLaunchDashboard->dwReason;
        m_dwLaunchContext = pLaunchDashboard->dwContext;
        m_dwLaunchParameter1 = pLaunchDashboard->dwParameter1;
        m_dwLaunchParameter2 = pLaunchDashboard->dwParameter2;

        PLAUNCH_DATA_PAGE pTemp = *LaunchDataPage;
        *LaunchDataPage = NULL;
        MmFreeContiguousMemory(pTemp);
    	
		m_bLaunchFromTitle = true;
		DbgPrint("CSceneNavigator::Initialize - Reason = [%d]\n", m_dwLaunchReason);
	}
	
	eSceneId eFirstScene = FindEntryPoint();
	DbgPrint("SceneId = [%d], name = [%s]\n", eFirstScene, SceneName[eFirstScene]);

	m_FlowStack[m_nStackTop] = eFirstScene;
	m_eCurrentSceneId = eFirstScene;
	hr = CreateScene(eFirstScene);
	return hr;
}

eSceneId CSceneNavigator::FindEntryPoint()
{
	eSceneId eFirstScene;
	switch(m_dwLaunchReason)
	{
	case XLD_LAUNCH_DASHBOARD_NETWORK_CONFIGURATION:
		eFirstScene = eNTS_ConfigChoiceId;
		break;

	case XLD_LAUNCH_DASHBOARD_NEW_ACCOUNT_SIGNUP:        
		eFirstScene = eGEN_Welcome_PanelId;
		break;

	case XLD_LAUNCH_DASHBOARD_MESSAGE_SERVER_INFO:
	case XLD_LAUNCH_DASHBOARD_POLICY_DOCUMENT:
	case XLD_LAUNCH_DASHBOARD_ONLINE_MENU:
	case XLD_LAUNCH_DASHBOARD_FORCED_NAME_CHANGE:
	case XLD_LAUNCH_DASHBOARD_FORCED_BILLING_EDIT:
		eFirstScene = eACT_OnlineId;
		break;
	default:
		eFirstScene = eACT_OnlineId;
	}
	return eFirstScene;	
}


void CSceneNavigator::Dump(bool bAllData)
{	
#ifdef DEBUG
	ValidateData();
#endif
	if(bAllData)
	{
		DbgPrint("=============================================================\n");
		DbgPrint("SceneNavigator Dump\n");
		DbgPrint("=============================================================\n");
		DbgPrint("Dump m_SceneHierarchy\n");
		
		int nLen = countof(m_SceneHierarchy);

		int nSceneNameSize = countof(SceneName);
		int nButtonNameSize = countof(ButtonName);

		for (int i = 0; m_SceneHierarchy[i].m_Children[0]!= eLastSceneId ; i++)
		{
			DbgPrint("\nScene: [%s]\n\tChildren:\n\t\t", SceneName[i]);
			
			for(int j =0 ; m_SceneHierarchy[i].m_Children[j]!= eNullSceneId; j++)
			{
			  DbgPrint("%s   ", SceneName[m_SceneHierarchy[i].m_Children[j]]);
			}
			if(j == 0)
			{
				DbgPrint("[null]");
			}

			DbgPrint("\n\tButtons: \n\t\t");
			for(int j =0 ; m_SceneHierarchy[i].m_ButtonId[j]!= eNullButtonId; j++)
			{
			  DbgPrint("%s   ", ButtonName[m_SceneHierarchy[i].m_ButtonId[j]]);
			}
			if(j == 0)
			{
				DbgPrint("[null]");
			}
		}
	}
	DbgPrint("Flow stack\n");

	for(int j = m_nStackTop ; j >= 0; j--)
	{
		DbgPrint("%d [Scene = %s]\n", m_FlowStack[j], SceneName[m_FlowStack[j]]);	
	}
	DbgPrint("\n=============================================================\n");
			
}


#ifdef DEBUG
void CSceneNavigator::ValidateData()
{
	DbgPrint("++++++++++++++++++++++++++++++++++++++++\n");
	DbgPrint("Validating CSceneNavigator data ...\n");
	// check that number of buttons = number of callbacks

	for (int i = 0; m_SceneHierarchy[i].m_Children[0]!= eLastSceneId ; i++)
	{
		for(int nChild =0 ; m_SceneHierarchy[i].m_Children[nChild]!= eNullSceneId; nChild++)
			; // empty loop		

		for(int nButton =0 ; m_SceneHierarchy[i].m_ButtonId[nButton]!= eNullButtonId; nButton++)
				; //empty loop 
	//TBD BACK	ASSERT(nChild == nButton); 
	}
	
	// check that all children scenes are present in the SceneName[]
	// check that all buttons are present in the ButtonName[]
	int nSceneNameSize = countof(SceneName);
	int nButtonNameSize = countof(ButtonName);
	for (int i = 0; m_SceneHierarchy[i].m_Children[0]!= eLastSceneId ; i++)
	{
		for(int j =0 ; m_SceneHierarchy[i].m_Children[j]!= eNullSceneId; j++)
		{
			ASSERT(m_SceneHierarchy[i].m_Children[j] < nSceneNameSize);
		}
		
		for(int j =0 ; m_SceneHierarchy[i].m_ButtonId[j]!= eNullButtonId; j++)
		{
		  ASSERT(m_SceneHierarchy[i].m_ButtonId[j] < nButtonNameSize); 
		}
			
	}

}
#endif



void CSceneNavigator::GetButtonsCallBack(
			eSceneId SceneId,  vector<eButtonId> ButtonIds, vector<ButtonCallBack> *pFnVec)
{
	ASSERT(SceneId < countof(SceneName));
#ifdef DEBUG
	int nButtonNum = GetButtonNum(SceneId);

// NAT put it back	ASSERT(nButtonNum == ButtonIds.size());
	if(ButtonIds.size()!= nButtonNum)
	{
		DbgPrint("CSceneNavigator::GetButtonsCallBack - mismatch in button number. Fix it!!!\n");
	}
#endif 

	if(SceneId < countof(SceneName))
	{
		int nIndex = 0;
		for(size_t j =0 ; j < ButtonIds.size(); j++)
		{
			
			nIndex = FindButtonIndex(SceneId, ButtonIds[j]);
			if(SceneId!= eKeyboardSceneId && SceneId!= eKeypadSceneId && SceneId!= eKeypadPeriodSceneId 
				&& SceneId!= eNTS_ConfigChoiceId)
			{
				// DO NOT REMOVE THIS ASSERT, VERIFY THAT THE NUMBER OF BUTTONS YOU SPECIFY IN 
			// NAVIGATION MAP IS THE SAME AS NUMBER OF LINKS TO DIFFERENT SCENES. ALL SCENES
			// IN THE IF CONDITION ARE SPECIAL CASES, FOR EXAMPLE CONFIGCHOICE INCLUDES HIDDEN
			// BUTTONS
				ASSERT(nIndex != -1);
			}
			pFnVec->push_back(m_SceneHierarchy[SceneId].m_fnCallBack[j]);
		}
	}
}

void CSceneNavigator::MoveNextScene(eButtonId eButton)
{
	eSceneId eCurSceneId = m_FlowStack[m_nStackTop];
	
	
	ASSERT(m_nStackTop < MAX_NAVIGATOR_STACK_SIZE);
	ASSERT(eCurSceneId  < countof(SceneName));
	if (m_nStackTop >= MAX_NAVIGATOR_STACK_SIZE)
		return;
	if (eCurSceneId  >= countof(SceneName))
		return;

	int nIndex = FindButtonIndex(eCurSceneId, eButton);
	ASSERT(nIndex != -1);	
	
	// if the Dash is launched by a title and this is an exit point, return back to the title
	if(IsExitPoint(eButton))
	{
		BootToTitle();
	}

	if(nIndex != -1 && m_SceneHierarchy[eCurSceneId].m_Children[nIndex]!=eNullSceneId)
	{
		// save the focus. When navigating back restore the focus as it was before
		m_SceneDataMap[m_eCurrentSceneId].m_eButtonInFocus = eButton;
		eSceneId eNextScene = m_SceneHierarchy[eCurSceneId].m_Children[nIndex];
		// allow self-references. In some cases pressing A on a screen that does not have any further links should
		// leave user on the same screen
		// The default Dash behaviour the user is taken back one screen		
		if(eCurSceneId == eNextScene)
		{
			// do not move anywhere, stay where you are
			return;
		}

		// handle the loops: search the stack for the scene with the same Id, if exists pop up all scenes till you get
		// to the chosen scene
		int nSceneCount = 0;
		for (int nTop = m_nStackTop; nTop >= 0; nTop --)
		{
			// scene could appear not more than one time on the stack
			ASSERT(nSceneCount <=1);
			if(m_FlowStack[nTop] == eNextScene)
			{
				nSceneCount++;
				for (int i = m_nStackTop; i >= nTop; i--)
				{
					// verify why the CScene is left on the stack 
					ASSERT(!m_SceneDataMap[m_FlowStack[nTop]].m_pScene);
					// cleanup the focus history
					m_SceneDataMap[m_FlowStack[i]].m_eButtonInFocus = eNullButtonId;
				}
				m_nStackTop = nTop;
			}

		}
		// otherwise: that's a new scene, push it on the stack
		if(!nSceneCount)
		{
			m_nStackTop ++;
		}
		
	
		m_FlowStack[m_nStackTop] = eNextScene;
		CreateScene(eNextScene);

		
		Dump(false);
	}
	// no more child scenes, come back
	else 
	{
		MovePreviousScene();
	}
	
}

void CSceneNavigator::MovePreviousScene()
{
	if(m_nStackTop <= 0)
	{
		// check if the Dash was launched by a title
		if(m_bLaunchFromTitle)
		{
			BootToTitle();
		}
		return;
	}

	bool bNewScene = true;
	m_nStackTop --;
	// for keyboard and dialogs check if multiple screens are visible. If it's true, do not create new scene
	// just make previous existing scene active
	if(IsDialogScene(m_eCurrentSceneId)) 
	{
		
		ASSERT(m_SceneDataMap[m_FlowStack[m_nStackTop]].m_bVisibleOnDeactive == true 
			&& m_SceneDataMap[m_FlowStack[m_nStackTop]].m_pScene);
		// do not create a new scene, retrieve an existing one
		if(m_SceneDataMap[m_FlowStack[m_nStackTop]].m_pScene)
		{
			bNewScene = false;
		}
	}
	eSceneId eOldScene = m_FlowStack[m_nStackTop];
	HRESULT hr = CreateScene(m_FlowStack[m_nStackTop], bNewScene);
	if (SUCCEEDED(hr))
	{// erase the history after we moved back
		m_SceneDataMap[eOldScene].m_eButtonInFocus = eNullButtonId;
	}
	Dump(false);
}

eSceneId CSceneNavigator::GetSceneId(char* pSceneName)
{
	ASSERT(pSceneName && pSceneName[0]);
	for (int i = 0; i< countof(SceneName); i++)
	{
		if (_stricmp(SceneName[i], pSceneName))
			break;
	}
	if (i == countof(SceneName))
		return eNullSceneId; 
	return static_cast<eSceneId> (i);
}

eButtonId CSceneNavigator::GetButtonId(char* pButtonName)
{
	ASSERT(pButtonName && pButtonName[0]);
	for (int i = 0; i< countof(ButtonName); i++)
	{
		if (_stricmp(ButtonName[i], pButtonName))
			break;
	}
	if (i == countof(ButtonName))
		return eNullButtonId;
	return static_cast<eButtonId> (i);

}

int CSceneNavigator::FindButtonIndex(eSceneId eCurSceneId, eButtonId eButton)
{
	ASSERT(eCurSceneId  < countof(SceneName));
	for(int j =0 ; m_SceneHierarchy[eCurSceneId].m_ButtonId[j]!= eNullButtonId ; j++)
		if(m_SceneHierarchy[eCurSceneId].m_ButtonId[j] == eButton)
		{
			break;
		}

	if (m_SceneHierarchy[eCurSceneId].m_ButtonId[j] == eNullButtonId && eButton!= eNullButtonId)
		return -1;
	return j;
}


int CSceneNavigator::GetButtonNum(eSceneId SceneId)
{
	for(int j =0 ; m_SceneHierarchy[SceneId].m_ButtonId[j]!= eNullButtonId ; j++)
		; //empty loop
	return j;
}

// TBD render all visible scenes
HRESULT CSceneNavigator::Render(DWORD dwFlags)
{
	HRESULT hr = S_OK;
// for keyboards and dialogs, draw the previous scene as well
	if(IsDialogScene(m_eCurrentSceneId) ) 
	{
		// TBD put it back ASSERT(m_nStackTop > 0);

		if(m_nStackTop > 0)
		{
			eSceneId ePrevScene = m_FlowStack[m_nStackTop-1];
			CPrimitiveScene* pPrevScene = m_SceneDataMap[ePrevScene].m_pScene;
            // HKG:  You should be able to render the keyboard by itself if you want
//			ASSERT(pPrevScene);
			if(pPrevScene)
			{
				pPrevScene->Render(dwFlags);
			}
		}
		CScene::RenderGradientBackground( 0xcc000000, 0xcc000000);
	}
	
	m_pCurrentScene->Render(dwFlags);
	return hr;
}


CSceneNavigator* CSceneNavigator::Instance () 
{
	if (pinstance == 0)  // is it the first call?
	{  
		pinstance = new CSceneNavigator; // create sole instance
	}
	return pinstance; // address of sole instance
}


HRESULT CSceneNavigator::Cleanup()
{
// TBD check what else to remove
	HRESULT hr = S_OK;
	delete m_pCurrentScene;
	m_pCurrentScene = NULL;
	return hr;
}


eButtonId CSceneNavigator::GetFocus(eSceneId SceneId)
{
	ASSERT(SceneId != eNullSceneId && SceneId < countof(SceneName));
	
	return m_SceneDataMap[SceneId].m_eButtonInFocus;

}

static float fTotalElapsed = 0;
static bool bFirstTime = true;
HRESULT CSceneNavigator::FrameMove(XBGAMEPAD& GamepadInput, float fElapsedTime)
{
	HRESULT hr = S_OK;
	ASSERT(m_pCurrentScene);
	hr = m_pCurrentScene->FrameMove(GamepadInput, fElapsedTime);

	fTotalElapsed+=fElapsedTime;
	if (fTotalElapsed > 2.0f && !bFirstTime)
	{
//		CreateScene(eKeypadPeriodSceneId);
//		CreateScene(eKeyboardSceneId);
//		CreateScene(eNTS_SettingsId);
// CreateScene(eKeyboardSceneId);
		fTotalElapsed = 0;
	}
	bFirstTime = false;
	return hr;
}


HRESULT CSceneNavigator::RunDialog(MSGPARAMETERS* pData)
{
	HRESULT hr = S_OK;

	m_nStackTop ++;
	m_FlowStack[m_nStackTop] = pData->m_SceneID;
	hr = CreateScene(pData->m_SceneID);

    if ( SUCCEEDED( hr ) )
    {

	    // TBD set all dialog data 

        // NATALY -- I really don't know what I'm doing here ... feel free to change this
        CMsgDialog* pDialog = (CMsgDialog*)m_SceneDataMap[m_FlowStack[m_nStackTop]].m_pScene;
        pDialog->SetParameters( *pData );
    }
	return hr;
}

HRESULT CSceneNavigator::CleanupDialog()
{
	HRESULT hr = S_OK;
	MovePreviousScene();
	return hr;
}

bool    CSceneNavigator::IsDialogScene( eSceneId SceneId )
{
    if (
    SceneId == eKeyboardSceneId ||
    SceneId == eKeypadSceneId ||
    SceneId == eKeypadSceneId ||
    SceneId == eKeypadPeriodSceneId ||
    SceneId == eProgressBarSceneId ||
    SceneId == eMsgBoxSceneId ||
    SceneId == eAnimWait )
    {
        return true;
    }

    return false;
}

bool CSceneNavigator::IsExitPoint(eButtonId eButton)
{
	if(!m_bLaunchFromTitle)
	{
		return false;
	}
	for (int i = 0; m_ExitPoint[i].m_ExitPointId != eLastSceneId; i++)
	{
		if(m_ExitPoint[i].m_dwReason == m_dwLaunchReason 
			&& m_ExitPoint[i].m_ExitPointId == m_eCurrentSceneId
			&& m_ExitPoint[i].m_ButtonId == eButton)
		{
			return true;
		}

	}
	return false;
}

void CSceneNavigator::BootToTitle()
{
	// do not boot anywhere if the Dash was not boot from a title
	ASSERT(m_bLaunchFromTitle);
	if (!m_bLaunchFromTitle)
	{
		return;
	}

	LD_FROM_DASHBOARD ld;
	ZeroMemory(&ld, sizeof(ld));
	ld.dwContext = m_dwLaunchContext;
	DbgPrint("CSceneNavigator::MovePreviousScene - boot back to the title, Context = [%d], DashEntry = [%s]\n",
		m_dwLaunchContext, SceneName[m_eCurrentSceneId]);
	
	XWriteTitleInfoAndReboot("default.xbe", "\\Device\\CdRom0", LDT_FROM_DASHBOARD, m_dwTitleID, (PLAUNCH_DATA)&ld);
}