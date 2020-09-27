/*************************************************************************************************\
act_online.cpp			: Implementation of all account creation and modification scenes
Creation Date		: 2/4/2002 9:47:54 AM
Copyright (c) Microsoft Corporation.  All rights reserved
Author				: Heidi Gaertner
//-------------------------------------------------------------------------------------------------
Notes				: 
\*************************************************************************************************/

#include "std.h"
#include "Scene.h"
#include "XBInput.h"
#include "NetConfig.h"
#include "globals.h"
#include "InputHandle.h"
#include "Dialog.h"
#include "tgl.h"
#include "AccountData.h"
#include "SceneNavigator.h"
#include "netConfig.h"
#include "date.h"
#include "accountdata.h"

HRESULT CACT_OnlineScene::m_hrLogon = -1;
//*************************************************************************************

HRESULT CACT_OnlineScene::Initialize(char* pUrl, char* pButtonTextName,  eSceneId eCurSceneId, bool bAutoTest)
{
    HRESULT hr = CCellWallScene::Initialize( pUrl, pButtonTextName, eCurSceneId, bAutoTest );
    

    // store, we can't pop dialog right here, in case this is the first screen

      return hr;

}

//*************************************************************************************

HRESULT CACT_OnlineScene::FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime )
{
	ASSERT(m_pButtons.size() == 4);
    bool bRenderParentScene = true;

    if ( GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_A] || GamepadInput.wPressedButtons & XINPUT_GAMEPAD_START )
    {
        eButtonId id = m_pButtons[m_nActiveButtonIndex]->GetButtonId();

        switch( id )
        {
        case eACT_Online02:
            {
                // Insure that we have a connection before going any further
                if( eServiceFail == g_NetConfig.CheckNetworkStatus() )
                {
                    RunOKDlg( L"ERR_NOT_CONNECTED" );
                    CSceneNavigator::Instance()->MoveNextScene( eACT_Online03 );
                    bRenderParentScene = false;
                }
            }
            break;
        }
    }

    HRESULT hr;
    if( bRenderParentScene )
        hr = CCellWallScene::FrameMove(GamepadInput, elapsedTime );

    // if we've never logged on, log on 
    if ( m_hrLogon == -1 )
     {
        if ( XBMSG_CANCEL == RunWaitCursor( _T("NTS_DIALOG_CONNECT"), AttemptMachineLogon ) )
        {
            m_hrLogon = XBAPP_ERR_LOGON_CANCELED;
        }
     }
    // we failed at logging on, go to network settings 
    else if ( m_hrLogon == XBAPP_ERR_LOGON_FAILED )
     {
         RunOKDlg( _T("NTS_DIALOG_REDIRECT") );

         CSceneNavigator::Instance()->MoveNextScene( eACT_Online03 );                       

         m_hrLogon = 0;
     }

    return hr;
}


//*************************************************************************************

HRESULT CACT_OnlineScene::AttemptMachineLogon()
{
  eNetworkState ns = g_NetConfig.CheckServiceStatus();
  HRESULT hr = S_OK;

  switch ( ns )
  {
      // all the failure cases I hope, the above function would
      // be nicer if it returned HRESULTS
      case 	eCableFail:
      case  eIPFail:
      case  ePPPoEFail:
      case  eDNSFail:
      case  eServiceFail:
          m_hrLogon = XBAPP_ERR_LOGON_FAILED;
          hr = XBMSG_OK;
          // press the nts buttons
          break;

      case eDone:
          m_hrLogon = 0; // all is clear
          hr = XBMSG_OK;
          break;

      default: // keep pumping
          hr = S_OK;
          break;
  }

  

   return hr;
    

}

//*************************************************************************************
// ACT_Scene, base class for all account objects
//*************************************************************************************
HRESULT CACT_Scene::Initialize(char* pUrl, char* pButtonTextName,  eSceneId eCurSceneId, bool bAutoTest )
{
    HRESULT hr = CCellWallScene::Initialize( pUrl, pButtonTextName, eCurSceneId, bAutoTest );

    UpdateButtons();

    return hr;
}

//*************************************************************************************
// call this after something has changed, or when you enter the screen
//*************************************************************************************
void CACT_Scene::UpdateButtons()
{
    for ( unsigned long i = 0; i < m_pButtons.size(); i++ )
    {
	    CText* pInput = (CText*)m_pButtons[i]->GetInputTextObject();
        if ( pInput )   
        {
            const TCHAR* pText = Accounts::Instance()->GetValue( m_pButtons[i]->GetButtonId() );
            if ( pText )
            {
                pInput->SetText( pText );
            }
        }

        // If a listbox exists in the scene, populate it here
        if ( m_pListBox )
        {
            int   numListItems;
            TCHAR **ppszListItems = NULL;
            TCHAR **ppszListHelp  = NULL;
            Accounts::Instance()->SetListBoxItems(m_eSceneId, &ppszListItems, &ppszListHelp, &numListItems);
            m_pListBox->RemoveAllItems();
            m_pListBox->AppendItems((const TCHAR**)ppszListItems, (const TCHAR**)ppszListHelp, numListItems );
        }

        HRESULT enable = Accounts::Instance()->EnableButton( m_pButtons[i]->GetButtonId() );
        if ( FAILED( enable ) )
        {
            if ( m_pButtons[i]->GetState() == eButtonHighlighted || 
                m_pButtons[i]->GetState() == eButtonDisabledHighlighted )
            {
                m_pButtons[i]->SetState( eButtonDisabledHighlighted );
            }
            else
            {
                m_pButtons[i]->SetState( eButtonDisabled );
            }
        }
        else if ( m_pButtons[i]->GetState() != eButtonHighlighted )
        {
            m_pButtons[i]->SetState( eButtonDefault );
        }

    }
}

//*************************************************************************************
// Base class for all scene objects that use account/billing data info
//*************************************************************************************
HRESULT CACT_Scene::SetValue(const TCHAR* szValue)
{
    eButtonId id = m_pButtons[m_nActiveButtonIndex]->GetButtonId();

	const CText* pInput = m_pButtons[m_nActiveButtonIndex]->GetInputTextObject();
    if ( pInput )    
    { 
        HRESULT validateHR = Accounts::Instance()->SetValue( id, szValue );
        if ( SUCCEEDED( validateHR ) )
        {
    	    (const_cast <CText*> (pInput))->SetText(szValue);

             // need to update buttons that might have been affected
             UpdateButtons();
        }
        else
            return validateHR;
    } 
    
    return S_OK;
}

HRESULT CACT_Scene::Validate( eButtonId id, const TCHAR* szValue )
{
    // make sure this is what we want to see
    HRESULT validateHR  = Accounts::Instance()->Validate( id, szValue );
    bool bRanWaitCursor = false;
    while ( validateHR == XBMSG_NEED_WAITCURSOR )
    {
        validateHR     = RunWaitCursor( NULL, Accounts::UpdateAccountInfo, _T("CANCEL") );
        bRanWaitCursor = true;
    }

    // check return value....
    if ( SUCCEEDED( validateHR ) || (validateHR == XBMSG_OK) )
    {
        UpdateButtons();
        if( bRanWaitCursor )
            return XBMSG_HAD_TO_WAIT;
        else
            return S_OK;
    }
    else // we failed
    {
        if ( ( validateHR & XBAPP_BUTTON_ID) == XBAPP_BUTTON_ID ) // data says go somewhere else
        {
            CSceneNavigator::Instance()->MoveNextScene( (eButtonId)(validateHR & 0x0000ffff) );                       
        }
        else // pop error message 
        {
            validateHR = RunOKDlg( validateHR );
            UpdateButtons(); 
        }
     }

    return validateHR;    
}


HRESULT CACT_Scene::FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime )
{
	HRESULT hr = S_OK;
    bool bCallBaseClass = true;

    if (!g_szUserInput.Empty())
	{
		SetValue(g_szUserInput.GetInputText());
		g_szUserInput.ResetInputText();
	}

     // Check to see if we need to populate the keyboard global buffer
    if ( GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_A] || GamepadInput.wPressedButtons & XINPUT_GAMEPAD_START )
    {
         CText* pText = const_cast <CText*> (m_pButtons[m_nActiveButtonIndex]->GetInputTextObject());
         // should probably put a check to see if the button belongs to this list box
         // but it might not matter
         if ( m_pListBox )
         {
            const TCHAR *pText = m_pButtons[m_nActiveButtonIndex]->GetTextObject()->GetText();
            hr = Accounts::Instance()->SetListBoxValue(m_eSceneId, (TCHAR *)pText);
         }
         else if ( pText )
         {
            g_szUserInput.SetInputText(pText->GetText()); 
         }
         
         // validate no matter what
         if ( FAILED( hr = Validate( m_pButtons[m_nActiveButtonIndex]->GetButtonId(), NULL ) ) )
         {
             // we don't want to move on, the CScene will do that if we aren't careful
             bCallBaseClass = false;
         }
         else if ( hr == XBMSG_HAD_TO_WAIT )
         {
             hr = S_OK;
             bCallBaseClass = false;
             CSceneNavigator::Instance()->MoveNextScene( m_pButtons[m_nActiveButtonIndex]->GetButtonId() );
         }
         else
         {
             UpdateButtons();            
         }
    }

    Accounts::Instance()->FrameMove(); // call all the time, we might spawn tasks when we expect they are coming
	
    if ( bCallBaseClass )
        hr = CCellWallScene::FrameMove(GamepadInput, elapsedTime );
    
    return hr;
    
}


//*************************************************************************************
// ACT Signup Screens
//*************************************************************************************

int  CACT_SignupAccount::m_nSignupAge            = 0;
bool CACT_SignupAccount::m_bDisplayParentMessage = false;

CACT_SignupAccount::CACT_SignupAccount()
{

}

CACT_SignupAccount::~CACT_SignupAccount()
{

}

HRESULT CACT_SignupAccount::Initialize(char* pUrl, char* pButtonTextUrl,  eSceneId eCurSceneId, bool bAutoTest)
{
    HRESULT hr = S_OK;

    hr = CACT_Scene::Initialize(pUrl, pButtonTextUrl, eCurSceneId, bAutoTest);

    switch(eCurSceneId)
    {
    case eACT_CountryId:
        {
        }
        break;
    case eACT_IdId: 
        {
            break;
        }
    case eACT_BillingId:
        {
            HideObject("button_credit");
            HideText("text_NAME");
            HideText("text_credit");
            break;
        }
    case eACT_CreditId:
        {
            HideObject("button_billing");
            HideText("text_NAME");
            HideText("text_billing");               
            break;
        }
    case eGEN_Exp_TumblersId:
        {
            // Our credit card date expiration ranges
            CDateObject currentDate;
            m_pSpinners[1]->SetRange( 1, 12 );
            m_pSpinners[0]->SetRange( currentDate.getFullYear(), currentDate.getFullYear() + 10 );

            m_pSpinners[1]->SetPrecision(2);
            m_pSpinners[0]->SetPrecision(2);

            m_pSpinners[1]->SetValue( Accounts::Instance()->GetNumericValue( eGEN_Exp_TumblersMonth ) );
            m_pSpinners[0]->SetValue( Accounts::Instance()->GetNumericValue( eGEN_Exp_TumblersYear ) );
            break;
        }
    case eGEN_Dob_TumblersId:
        {
            // Date of Birth Tumblers
            CDateObject currentDate;  
            m_pSpinners[2]->SetRange( 1, 12 );  // month
            m_pSpinners[1]->SetRange( 1, 31 );  // day
            m_pSpinners[0]->SetRange( currentDate.getFullYear() - 50, currentDate.getFullYear() );  // year

            m_pSpinners[2]->SetPrecision(2);
            m_pSpinners[1]->SetPrecision(2);
            m_pSpinners[0]->SetPrecision(2);

            m_pSpinners[2]->SetValue( Accounts::Instance()->GetNumericValue( eGEN_Dob_TumblersMonth ) );
            m_pSpinners[1]->SetValue( Accounts::Instance()->GetNumericValue( eGEN_Dob_TumblersDay ) );
            m_pSpinners[0]->SetValue( Accounts::Instance()->GetNumericValue( eGEN_Dob_TumblersYear ) );

            CACT_SignupAccount::m_nSignupAge = 0;
            CACT_SignupAccount::m_bDisplayParentMessage = false;
            break;
        }
    default:

        break;
    }

    return hr;
}
    

HRESULT CACT_SignupAccount::FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime )
{
    HRESULT hr = S_OK;
    bool bSceneFrameMove = true;
 
    switch ( m_eSceneId )
    {
    case eGEN_Dob_TumblersId:
        {
            // Date of Birth Spinners must behave based on days in month
            CDateObject newTime;
            int nMonth = m_pSpinners[2]->GetValue();
            int nDay   = m_pSpinners[1]->GetValue();
            int nYear  = m_pSpinners[0]->GetValue();
 
            int nDaysInMonth = newTime.getDaysInMonth( nMonth, nYear );
            if( nDay > nDaysInMonth ) // month changed with date too high
            {
                nDay = nDaysInMonth;
                m_pSpinners[1]->SetValue( nDay );
            }

            m_pSpinners[1]->UpdateRange( 1, nDaysInMonth );
        }
        break;
    case eACT_CreditId:
        {    
            if( CACT_SignupAccount::m_bDisplayParentMessage )
            {
                RunOKDlg( L"CRED_UNDER_18_MESSAGE" );
                bSceneFrameMove         = false;
                CACT_SignupAccount::m_bDisplayParentMessage = false;
            }
        }
        break;
    }

    // Save the current spinner Values
    if ( GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_A] || GamepadInput.wPressedButtons & XINPUT_GAMEPAD_START )
    {
        eButtonId id = m_pButtons[m_nActiveButtonIndex]->GetButtonId();

        switch( m_eSceneId )
        {
        case eGEN_Welcome_PanelId:
            {
                // User has choosen to decline the Account signup, confirm their choice
                if( id == eGen_Welcome_Panel02 )
                {
                    HRESULT hDialogReturn;
                    hDialogReturn = RunYesNoDlg( L"GEN_WELCOME_EXIT_CONFIRM", L"YES", L"NO", true );
                    if( hDialogReturn == XBMSG_BACK || hDialogReturn == XBMSG_CANCEL )
                    {
                        // Return the user to the welcome screen
                        bSceneFrameMove         = false;
                    }
                }
            }
            break;
        case eACT_IdId:  // Account ID entry
            {
                if( id == eACT_Idcontinue )
                {
                    // Make sure the user is over 13, else return to root menu
                    if( CACT_SignupAccount::m_nSignupAge < 13 )
                    {
                        hr = RunOKDlg( XACT_E_USER_TOO_YOUNG );
                        Accounts::Instance()->ClearNewAccountData();

                        // Navigate them back to the online root
                        bSceneFrameMove         = false;
                        CSceneNavigator::Instance()->MoveNextScene( eACT_idHiddenBadName );
                        break;
                    }
                }
            }
            break;
        case eGEN_Exp_TumblersId:  // Credit card expiration
            {
                CDateObject newTime;
                newTime.clearDate();
                newTime.setMonth( (WORD)m_pSpinners[1]->GetValue() );
                newTime.setYear(  (WORD)m_pSpinners[0]->GetValue() );

                hr = Accounts::Instance()->SetValue( m_pButtons[m_nActiveButtonIndex]->GetButtonId(), &newTime.m_time );
            }
            break;
        case eGEN_Dob_TumblersId:   // Date of Birth Tumbler
            {
                CDateObject newTime;
                newTime.clearDate();
                newTime.setMonth( (WORD)m_pSpinners[2]->GetValue() );
                newTime.setDay(   (WORD)m_pSpinners[1]->GetValue() );
                newTime.setYear(  (WORD)m_pSpinners[0]->GetValue() );

                hr = Accounts::Instance()->SetValue( m_pButtons[m_nActiveButtonIndex]->GetButtonId(), &newTime.m_time );
                
                // Store the users age for later checks
                CACT_SignupAccount::m_nSignupAge = newTime.calcAge();
                
                // If the user is under 18, ask for a parents presence
                if( CACT_SignupAccount::m_nSignupAge < 18 )
                    CACT_SignupAccount::m_bDisplayParentMessage = true;
            }
            break;
        case eGen_Large_PanelId:  // Deal with Cost Screen and Exiting
            {
                HRESULT hDialogReturn = S_OK;

                while( 1 )
                {
                    // Confirm that the user must pay $ to sign up for the service
                    hDialogReturn = RunYesNoDlg( L"SIGNUP_COST_MESSAGE", L"YES", L"NO", true );

                    if( hDialogReturn == XBMSG_CANCEL )      
                    {
                        // They don't want to pay, so we should ask them if they wish to exit
                        HRESULT hDialogExit = RunYesNoDlg( L"SIGNUP_COST_CANCEL", L"YES", L"NO", true );
                
                        if( hDialogExit == XBMSG_OK )
                        {
                            //BUGBUG: Navigate user back to Xdash, or back to game
                            Accounts::Instance()->ClearNewAccountData();
                            CSceneNavigator::Instance()->MoveNextScene( eGen_Large_Panel02 );
                            bSceneFrameMove = false;
                            break;
                        }
                    }
                    else if( hDialogReturn == XBMSG_BACK )
                    {
                        // User wanted to go back from the payment screen
                        bSceneFrameMove = false;
                        break;
                    }
                    else
                    {
                        // Create the Account
                        hr = Accounts::Instance()->Validate( eACT_Cost_Confirm_Yes, L"" );
                        hDialogReturn = RunOKDlg( L"SIGNUP_CONGRATS_MESSAGE" );
                        
                        break;
                    }
                }
            }
        }
    }

    if( bSceneFrameMove )
        hr = CACT_Scene::FrameMove(GamepadInput, elapsedTime );

    return hr;
}
