#include "std.h"
#include "msgdialog.h"

//-------------------------------------------------------------------------------------------------

HRESULT CMsgDialog::SetParameters( const MSGPARAMETERS& params )
{
 
    SetMessage( params.m_pStrMessageID );
   
    if ( params.m_bUseBigButtons )
    {
        SetButtonText( eMessage_BIGYES, params.m_pStrYesID );
        SetButtonText( eMessage_BIGNO, params.m_pStrNoID );
        SetButtonText( eMessage_CENTER, params.m_pStrCenterButtonID );
        SetButtonText( eMessage_YES, NULL );
        SetButtonText( eMessage_NO, NULL );
    }
    else
    {
        SetButtonText( eMessage_YES, params.m_pStrYesID );
        SetButtonText( eMessage_NO, params.m_pStrNoID );
        SetButtonText( eMessage_CENTER, params.m_pStrCenterButtonID );
        SetButtonText( eMessage_BIGYES, NULL );
        SetButtonText( eMessage_BIGNO, NULL );
    }

	HRESULT hr = S_OK;
    TG_Shape* pShape = m_pRoot->FindObject( "progress bar" );
    if ( pShape )
    {
        pShape->setVisible( 0 );
    }

	if(!m_pButtons.size())
	{
		return hr;
	}

	if(m_pButtons.size() && !m_pButtons[m_nActiveButtonIndex]->IsShowing())
	{
		FindNextActiveButton();	
	}

	m_pButtons[m_nActiveButtonIndex]->SetState(eButtonHighlighted);
    return hr;
    
}



//-------------------------------------------------------------------------------------------------

void CMsgDialog::SetButtonText( eButtonId ID, const TCHAR* pText )
{
    int buttonIndex = FindButton( ID );
    ASSERT( buttonIndex > -1 );
    if ( buttonIndex != -1 )
    {
        if( pText )
        {
            m_pButtons[buttonIndex]->Show( 1 );
            m_pButtons[buttonIndex]->SetTextId( pText );
        }
        else
        {
            m_pButtons[buttonIndex]->Show( 0 );
        }
    }
}

// in case the message needs to change
//-------------------------------------------------------------------------------------------------
HRESULT CMsgDialog::SetMessage( const TCHAR* pStrIDMessage  )
{
   int nTextId = FindTextObjForShape( "TEXT_PANEL" );
   if ( nTextId != -1 )
   {
        CText* pText = m_pTexts[nTextId];
        pText->SetTextId( pStrIDMessage );
   }

   return S_OK;

}

HRESULT CMsgDialog::FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime )
{
    HRESULT hr = S_OK;

    CPrimitiveScene::FrameMove(GamepadInput, elapsedTime );
   
    // presumably OK Button pressed
	if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_A] || 
	   GamepadInput.wPressedButtons & XINPUT_GAMEPAD_START)
	{
		DbgPrint("CMsgDialog::FrameMove - Scene:%s, A/START is pressed\n", SceneName[m_eSceneId]);

        eButtonId buttonID = m_pButtons[m_nActiveButtonIndex]->GetButtonId();

        if (  buttonID ==  eMessage_NO || buttonID == eMessage_BIGNO )
        {
           hr = XBMSG_CANCEL;
        }
        else
        {
           hr = XBMSG_OK;
        }
	}

	// cancel button pressed, but only if its visible
    else if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_B] ||
	   GamepadInput.wPressedButtons & XINPUT_GAMEPAD_BACK)
	{
		DbgPrint("CMsgDialog::FrameMove - Scene:%s, B/BACK is pressed\n", SceneName[m_eSceneId]);
		// scene is destroyed by now do not touch any private data
		hr = XBMSG_BACK;
	}

    return hr;


}

HRESULT CWaitCursor::SetParameters( const MSGPARAMETERS& params )
{
     m_pWaitFunction = params.m_pWaitFunction;
     m_fTime = 0.f;

     return S_OK;
}
  
HRESULT CWaitCursor::FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime )
{
   HRESULT hr = S_OK;
   if ( m_pWaitFunction )
      hr = m_pWaitFunction();

   if ( m_pRoot )
       m_pRoot->FrameMove( elapsedTime );

   m_Camera.FrameMove( elapsedTime );

   m_fTime += elapsedTime;

   if ( m_fTime < 5.f && m_eSceneId == eConnectAnim )
       hr = S_OK;

   // should probably put a min time length in here
   return hr;

}





//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

LARGE_INTEGER oldTime = {0,0};
HRESULT TestProgressDlg( float* pProgress )
{
    LARGE_INTEGER counterFreq;
    QueryPerformanceFrequency( &counterFreq );

    if ( !oldTime.QuadPart )
        QueryPerformanceCounter( &oldTime );

    LARGE_INTEGER newTime;
    QueryPerformanceCounter( &newTime );

    float time = ((float)newTime.QuadPart - (float)oldTime.QuadPart)/(float)counterFreq.QuadPart;

    *pProgress = time/30.f;

    if ( time > 30.f )
    {
        memset( &oldTime, 0, sizeof ( LARGE_INTEGER ) );
        return XBMSG_OK;
    }


    return S_OK;
}

CProgressDialog::CProgressDialog()
{
    m_pSliderBar = NULL;
    m_pCallback = NULL;
}

//-------------------------------------------------------------------------------------------------

HRESULT CProgressDialog::SetParameters( const MSGPARAMETERS& params )
{
    m_pCallback = params.m_pFunction;
    CMsgDialog::SetParameters( params );

    TG_Shape* pShape = m_pRoot->FindObject( "progress bar" );
    if ( pShape )
    {
        pShape->setVisible( 1 );
        m_pSliderBar = pShape->FindObject( "slider" );
    }

    return S_OK;

}

//-------------------------------------------------------------------------------------------------


HRESULT CProgressDialog::FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime )
{
    // need to call function here, figure out how much time is left on the clock
    // and then update the graphic appropriately.
    float fTime = 0.f;
    HRESULT hr = S_OK;

    if ( m_pCallback )
        hr = m_pCallback( &fTime );
    
    SetProgress( fTime );
    // presumably only cancel works???
	if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_A] || 
	   GamepadInput.wPressedButtons & XINPUT_GAMEPAD_START)
	{
		DbgPrint("CProgressDlg::FrameMove - Scene:%s, A/START is pressed\n", SceneName[m_eSceneId]);
        
        hr = XBMSG_CANCEL;
		return hr;
	}

		// cancel button pressed, but only if its visible
    else if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_B] ||
	   GamepadInput.wPressedButtons & XINPUT_GAMEPAD_BACK)
	{
		DbgPrint("CProgressDlg::FrameMove - Scene:%s, B/BACK is pressed\n", SceneName[m_eSceneId]);
		// scene is destroyed by now do not touch any private data
		hr = XBMSG_BACK;
	}
	return hr;
}

float   CProgressDialog::SetProgress( float newProgress )
{
    if ( m_pSliderBar )
    {
        m_pSliderBar->SetScale( newProgress, 1.0f, 1.0f );
    }

    return S_OK;

}



//-------------------------------------------------------------------------------------------------
