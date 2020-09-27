///////////////////////////////////////////////////////////////////////////////
// CPrimitiveScene.h
// base class for all individual screens.
// This class holds all the geometry, plus lists of interactive objects
// all individual screens should derive from this object
//////////////////////////////////////////////////////////////////////////////
#include "std.h"



#include "Scene.h"
#include "XBInput.h"
#include "globals.h"

#include "SceneNavigator.h"
#include "xodash.h"

HRESULT CScene::Initialize(char* pUrl, char* pButtonTextUrl, eSceneId eCurSceneId, bool bAutoTest)
{
	HRESULT hr = CPrimitiveScene::Initialize(pUrl, pButtonTextUrl, eCurSceneId, bAutoTest);
	if (FAILED(hr))
	{
		DbgPrint("CScene::Initialize fail to init CPrimitiveScene");
		return hr;
	}
	
	vector <eButtonId> ButtonIds;
	for (size_t i = 0; i< m_pButtons.size(); i++)
	{
		ButtonIds.push_back(m_pButtons[i]->GetButtonId());
	}

	vector<ButtonCallBack> pfnCallBack;
	CSceneNavigator::Instance()->GetButtonsCallBack(eCurSceneId, ButtonIds, &pfnCallBack); 
	for (size_t i = 0; i< m_pButtons.size(); i++)
	{
		m_pButtons[i]->SetCallback(pfnCallBack[i]);
	}

	hr = BindData();

	if(bAutoTest)
	{
		AutoTest(eSceneInit);
	}
    return S_OK;
}

CScene::CScene() :CPrimitiveScene()
{

}

CScene::~CScene()
{
	CPrimitiveScene::Cleanup();
}


HRESULT CScene::FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime )
{
	HRESULT hr = CPrimitiveScene::FrameMove(GamepadInput, elapsedTime);
	if(FAILED(hr))
	{
		DbgPrint("CScene::FrameMove - Fail in CPrimitiveScene::FrameMove");
		return hr;
	}

	if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_A] || 
	   GamepadInput.wPressedButtons & XINPUT_GAMEPAD_START)
	{
		DbgPrint("CPrimitiveScene::FrameMove - Scene:%s, A/START is pressed\n", SceneName[m_eSceneId]);

		eButtonId eActiveButton = eNullButtonId;
		if (!m_pButtons.empty())
		{
			eActiveButton = m_pButtons[m_nActiveButtonIndex]->GetButtonId();
		}

        eSoundType  soundType = m_pButtons[m_nActiveButtonIndex]->GetSoundType();  
        eSound sound = eSoundAPress; 
      
        if ( m_pButtons[m_nActiveButtonIndex]->GetState() == eButtonDisabled 
            || m_pButtons[m_nActiveButtonIndex]->GetState() == eButtonDisabledHighlighted )
        {
            sound = eSoundDisabled;     
        }
        else
        {
      		CSceneNavigator::Instance()->MoveNextScene(eActiveButton);
       		hr = XBAPPWRN_SCENE_CHANGED;
        }

        PlaySound( soundType, sound );

 
		// scene is destroyed by now do not touch any private data
		return hr;
	}

	else if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_B] ||
	   GamepadInput.wPressedButtons & XINPUT_GAMEPAD_BACK)
	{
		DbgPrint("CPrimitiveScene::FrameMove - Scene:%s, B/BACK is pressed\n", SceneName[m_eSceneId]);
        
        eSoundType  soundType = m_pButtons[m_nActiveButtonIndex]->GetSoundType();
        PlaySound( soundType, eSoundBPress );

		CSceneNavigator::Instance()->MovePreviousScene();
		// scene is destroyed by now do not touch any private data
		hr = XBAPPWRN_SCENE_CHANGED;
		return hr;
	}
	return hr;
}


HRESULT CScene::RenderGradientBackground( DWORD dwTopColor, 
                                                  DWORD dwBottomColor )
{
    // First time around, allocate a vertex buffer
    static LPDIRECT3DVERTEXBUFFER8 g_pVB  = NULL;
    if( g_pVB == NULL )
    {
        g_pd3dDevice->CreateVertexBuffer( 4*5*sizeof(FLOAT), D3DUSAGE_WRITEONLY, 
                                          0L, D3DPOOL_DEFAULT, &g_pVB );
        struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
        BACKGROUNDVERTEX* v;
        g_pVB->Lock( 0, 0, (BYTE**)&v, 0L );
        v[0].p = D3DXVECTOR4(   0,   0, 1.0f, 1.0f ); v[0].color = dwTopColor;
        v[1].p = D3DXVECTOR4( 640,   0, 1.0f, 1.0f ); v[1].color = dwTopColor;
        v[2].p = D3DXVECTOR4(   0, 480, 1.0f, 1.0f ); v[2].color = dwBottomColor;
        v[3].p = D3DXVECTOR4( 640, 480, 1.0f, 1.0f ); v[3].color = dwBottomColor;
        g_pVB->Unlock();
    }

    // Set states
    g_pd3dDevice->SetTexture( 0, NULL );
    g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, FALSE ); 
    g_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE ); 
    g_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
    g_pd3dDevice->SetStreamSource( 0, g_pVB, 5*sizeof(FLOAT) );

    g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

    // Clear the zbuffer
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0L );

    return S_OK;
}


void CScene::SetFocus()
{
	// no buttons, nothing to set focus to
	if(!m_pButtons.size() || m_eSceneId == eNullButtonId)
	{
		return;
	}
	eButtonId eFocusButton = CSceneNavigator::Instance()->GetFocus(m_eSceneId);
	

	if(eFocusButton != eNullButtonId)
	{
		int nIndex = FindButton(eFocusButton);
		// Navigator is absolutely out of sync with the scene
		ASSERT(nIndex != -1);
		if(nIndex !=-1 && m_pButtons[nIndex]->IsShowing())
		{
			SetActiveButton(eFocusButton);
			return;
		}		
	}

	CPrimitiveScene::SetFocus();
}