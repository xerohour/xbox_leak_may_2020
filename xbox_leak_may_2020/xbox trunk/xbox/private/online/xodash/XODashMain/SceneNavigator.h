//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
// Author: Natalyp
//-------------------------------------------------------------
#pragma once 

#include "StringObj.h"
#include "XOConst.h"
#include "PrimitiveScene.h"
#include <vector>
 
using namespace std;
 
class CScene;
struct XBGAMEPAD;
struct MSGPARAMETERS;

//-------------------------------------------------------------
// Name: SceneDesription
// Purpose: holds relevant data for scene's manipulation
// Description: 
//-------------------------------------------------------------
struct SceneDescription
{
	char m_url[MAX_URL_SIZE];   // xbg file to load art from
    char m_buttonTextUrl[64];   // one xbg could contain more than one screen, this is a tag where to look for 
	// this screen specific data (text, buttons, etc.)inside of the xbg file
	bool m_bVisibleOnDeactive;  // true if the scene should be present when the child scene is active
	CPrimitiveScene* m_pScene;
	eButtonId m_eButtonInFocus;   // keep track of highlighted button, when navigating back highligh this button 
	// instead of the first one

};



//-------------------------------------------------------------
// Name: SceneChild
// Purpose: navigation map for a specific scene
// Description:
//-------------------------------------------------------------
struct SceneChild
{
	eSceneId m_Children[MAX_CHILD_SCENE_NUM];  // screens to navigate to
	eButtonId m_ButtonId[MAX_BUTTON_IN_SCENE]; // highlighted button that caused navigation to different screen
	// has one to one correlation with m_Children
	ButtonCallBack m_fnCallBack[MAX_BUTTON_IN_SCENE];
};

//-------------------------------------------------------------
// Name:	ExitPoint
// Purpose: Define exit points when the dash wash launched by a title
// Description:  
//-------------------------------------------------------------
struct ExitPoint
{
	DWORD m_dwReason;
	eSceneId m_ExitPointId;
	eButtonId m_ButtonId;
};

//-------------------------------------------------------------
// Name: CSceneNavigator
// Purpose: The class is responsible for changing the screens based 
//			on user input and/or server/application conditions
// Description: singleton
//-------------------------------------------------------------
class CSceneNavigator
{
public:
	static CSceneNavigator* Instance();
	HRESULT Initialize();
	HRESULT Cleanup();
	HRESULT Render(DWORD dwFlags);

	HRESULT FrameMove(XBGAMEPAD& , float);
	void Dump(bool bAllData = false);
	void GetButtonsCallBack(eSceneId SceneId, vector<eButtonId> ButtonIds, vector<ButtonCallBack> *pFnVec );
	void MoveNextScene(eButtonId = eNullButtonId);
	void MovePreviousScene();
	eButtonId GetFocus(eSceneId SceneId);

	HRESULT RunDialog(MSGPARAMETERS* pData);
	HRESULT CleanupDialog();


protected:
      CSceneNavigator();
      CSceneNavigator(const CSceneNavigator&);
      CSceneNavigator& operator= (const CSceneNavigator&);

private:
    static CSceneNavigator* pinstance;

	eSceneId m_FlowStack[MAX_NAVIGATOR_STACK_SIZE];
	int m_nStackTop;
	CPrimitiveScene* m_pCurrentScene;
	eSceneId m_eCurrentSceneId;

// Launch data if the Dash is launched by a title
	DWORD m_dwTitleID;
    DWORD m_dwLaunchReason;
    DWORD m_dwLaunchContext;
    DWORD m_dwLaunchParameter1;
    DWORD m_dwLaunchParameter2;
	bool m_bLaunchFromTitle;

	static SceneChild m_SceneHierarchy[MAX_CHILD_SCENE_NUM]; 
	static SceneDescription m_SceneDataMap[MAX_SCENE_NUM];
	static ExitPoint m_ExitPoint[MAX_EXIT_POINT_NUM];

private:
	static eSceneId GetSceneId(char*);
	static eButtonId GetButtonId(char*);
	HRESULT CreateScene(eSceneId eNewScene, bool bCreateNew = true);
	int FindButtonIndex(eSceneId, eButtonId);
	int GetButtonNum(eSceneId);
	void SetActiveButton(int nIndex);

    bool    IsDialogScene( eSceneId SceneId );
	
	eSceneId FindEntryPoint();
	bool IsExitPoint(eButtonId eButtonId);
	void BootToTitle();
	

// validation of Navigator's maps
#ifdef DEBUG
	void ValidateData();
#endif
};


