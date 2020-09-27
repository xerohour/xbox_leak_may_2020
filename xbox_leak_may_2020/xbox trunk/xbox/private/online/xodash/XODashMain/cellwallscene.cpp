#include "std.h"

#include "Scene.h"
#include "utilities.h"
#include "XGMath.h"
#include "TGL.h"
#include "globals.h"
#include "IniFile.h"
#include "SceneNavigator.h"
#include "XBInput.h"

// static member declarations
void* CCellWallScene::m_pVideoMemCellWall    = NULL;
void* CCellWallScene::m_pVideoMemSelect      = NULL;
void* CCellWallScene::m_pVideoMemBack        = NULL;
TG_Shape* CCellWallScene::s_pCellWallSphere	 = NULL;
TG_Shape* CCellWallScene::s_pBackButton		 = NULL;
TG_Shape* CCellWallScene::s_pSelectButton	 = NULL;
CText* CCellWallScene::m_pSelectText         = NULL;
CText* CCellWallScene::m_pBackText           = NULL;

CCellWallScene::CCellWallScene():
	CScene()
{
	bRenderSelect		 = true;
	bRenderBack			 = true;

	m_field = D3DX_PI / 4.0f;
    m_now = 0.f;
}

CCellWallScene::~CCellWallScene()
{
	Cleanup();
}

HRESULT CCellWallScene::Initialize(char* pUrl, char* pButtonTextUrl,  eSceneId eCurSceneId, bool bAutoTest)
{
	HRESULT hr = S_OK;
    
    do {

        // Load Cell Wall if not already loaded
        if(CCellWallScene::s_pCellWallSphere == NULL)
        {
            hr = LoadFromXBG( g_pd3dDevice, XBG_PATH_ANSI"cellwall.xbg", "cellwall" );
            BREAKONFAIL(hr, "CCellWallScene::Cannot load cellwall.xbg");

            CCellWallScene::s_pCellWallSphere = m_pRoot;
            m_pRoot = NULL;
            CCellWallScene::m_pVideoMemCellWall = m_pVideoMem;
            m_pVideoMem = NULL;
        }

        // Load Back Button if not already loaded 
        if(CCellWallScene::s_pBackButton == NULL)
        {
            hr = LoadFromXBG( g_pd3dDevice, XBG_PATH_ANSI"backbutton.xbg", "backbutton" );
            BREAKONFAIL(hr, "CCellWallScene::Cannot load backbutton.xbg");

            CCellWallScene::s_pBackButton = m_pRoot;
            m_pRoot = NULL;
            CCellWallScene::m_pVideoMemBack = m_pVideoMem;
            m_pVideoMem = NULL;

		    ASSERT(m_pButtons.size() == 1);
		    m_pButtons.clear();

            ASSERT( m_pTexts.size() == 1 );
            CCellWallScene::m_pBackText = m_pTexts[0];
            m_pTexts.clear();
        }

        // Load Select Button if not already loaded
        if(CCellWallScene::s_pSelectButton == NULL)
        {
            hr = LoadFromXBG( g_pd3dDevice, XBG_PATH_ANSI"selectbutton.xbg", "selectbutton" );
            BREAKONFAIL(hr, "CCellWallScene::Cannot load selectbutton.xbg");

            CCellWallScene::s_pSelectButton = m_pRoot;
            m_pRoot = NULL;
            CCellWallScene::m_pVideoMemSelect = m_pVideoMem;
            m_pVideoMem = NULL;
		
		    ASSERT(m_pButtons.size() == 1);
		    m_pButtons.clear();

            ASSERT( m_pTexts.size() == 1 );
            CCellWallScene::m_pSelectText = m_pTexts[0];
            m_pTexts.clear();
        }

        // Initialize the scene
        hr = CScene::Initialize(pUrl, pButtonTextUrl, eCurSceneId, true);
        BREAKONFAIL(hr, "CScene::Initialize fail to init CCellWallScene");
    } while(0);
	if (FAILED(hr))
	{
		Cleanup();
	}

	return hr;
}

#if DEBUG
static bool CellWallMove = false;
#endif //DEBUG

HRESULT CCellWallScene::FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime )
{
    if ( s_pBackButton )
        s_pBackButton->FrameMove( elapsedTime );

    if ( s_pSelectButton )
        s_pSelectButton->FrameMove( elapsedTime );

    if ( s_pCellWallSphere )
        s_pCellWallSphere->FrameMove( elapsedTime );

    HRESULT hr = CScene::FrameMove( GamepadInput, elapsedTime );
	
	if(hr == XBAPPWRN_SCENE_CHANGED)
	{ // scene is destroyed, do not touch private data
		return hr;
	}

	CellWallRotate(GamepadInput);
	return hr;
}



void CCellWallScene::CellWallRotate(XBGAMEPAD& GamepadInput)
{
	m_dwFrameTick = GetTickCount();
    float oldNow = (float)m_now;
	m_now = m_dwFrameTick / 1000.0;

#if DEBUG
    // Support for testing to stop the back sphere from rotating
    if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER] &&
       GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
	{
        CellWallMove = !CellWallMove;
	}

    if(CellWallMove)
    {
        m_now = 0.0;
    }
#endif //DEBUG
   

	float fRpm = .75f;

	float fAngle = sinf((float) (m_now * D3DX_PI * fRpm / 60.0f)) * m_field / 2.0f;
  	float fOldAngle = sinf((float) (oldNow * D3DX_PI * fRpm / 60.0f)) * m_field / 2.0f;


	CCellWallScene::s_pCellWallSphere->LocalRotateZ( fAngle - fOldAngle);
}


HRESULT CCellWallScene::Render( DWORD flags )
{
	// Render the Cell Wall
	ASSERT( CCellWallScene::s_pCellWallSphere );
    if ( CCellWallScene::s_pCellWallSphere )
	{
		if ( m_Light.Type != 0 )
		{
		   g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		   g_pd3dDevice->SetLight( 0, &m_Light );
		}
		else
		{
			g_pd3dDevice->SetRenderState(D3DRS_AMBIENT, 0x002f2f2f);
		}
		m_Camera.SetUp(g_pd3dDevice);
		CCellWallScene::s_pCellWallSphere->Render();

		// Render the A Button
		if( bRenderSelect )
		{
			ASSERT( CCellWallScene::s_pSelectButton );
			if ( CCellWallScene::s_pSelectButton )
            {
				CCellWallScene::s_pSelectButton->Render();
                CCellWallScene::m_pSelectText->Render( flags );
            }
		}
	
		// Render the B Button
		if( bRenderBack )
		{
			ASSERT( CCellWallScene::s_pBackButton );
			if ( CCellWallScene::s_pBackButton )
            {
				CCellWallScene::s_pBackButton->Render();
                CCellWallScene::m_pBackText->Render( flags );
            }
		}
	}

    // Render the main Scene
	CScene::Render( flags );

	return S_OK;
}

void CCellWallScene::AutoTest(eAutoTestData eState)
{
	switch(eState)
	{
	case eSceneInit:
		DbgPrint("\nAUTO:CELLWALL, A_BUTTON=%d, B_BUTTON=%d\n",bRenderSelect, bRenderBack);
		break;
	}
}

void CCellWallScene::AutoTest(XBGAMEPAD& GamepadInput)
{

}

HRESULT CCellWallScene::Cleanup()
{
	HRESULT hr = S_OK;

	CScene::Cleanup();

	return hr;
}

void CCellWallScene::RenderSelect(bool renderState)
{
	bRenderSelect = renderState;
}

void CCellWallScene::RenderBack(bool renderState)
{
	bRenderBack = renderState;
}

void CCellWallScene::HideObject(const char *pName)
{
    TG_Shape* pObject = m_pRoot->FindObject(pName);
    if ( pObject )
        pObject->setVisible(false);
}

void CCellWallScene::HideText(const char *pName)
{
	int nIndex = FindTextObjForShape(pName);
	CText *pInputText = m_pTexts[nIndex];
	pInputText->SetText(_T(""));
}
