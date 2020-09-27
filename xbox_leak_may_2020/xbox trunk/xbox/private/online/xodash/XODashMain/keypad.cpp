#include "std.h"
#include "Scene.h"
#include "Font.h"
#include "tgl.h"
#include "XBInput.h"
#include "SceneNavigator.h"
#include "globals.h"
#include "InputHandle.h"

CFont* GetFont(const TCHAR* szFaceName); 

static const TCHAR czsKbPressSoundUrl[] = _T("KeyboardStrokePress.wav");
static const TCHAR czsKbSelectSoundUrl[] = _T("KeyboardStrokeSelect.wav");

static const char cszKeypadTitle[] = "text_title";
static const char cszKeypadInputText[] = "text_input";

static const TCHAR cszKeypadBackspaceTextId[]  =  _T("BACKSPACE");
static const TCHAR cszKeypadLeftShiftTextId[]  =  _T("LEFT_SHIFT");
static const TCHAR cszKeypadRightShiftTextId[] =  _T("RIGHT_SHIFT");

static const WORD rgkeypadPeriod [] =
{
	L'1',	L'2',	L'3',
	L'4',	L'5',	L'6',
	L'7',	L'8',	L'9',
	L'0',	L'.'
};

static const WORD rgkeypadBasic [] =
{
	L'1',	L'2',	L'3',
	L'4',	L'5',	L'6',
	L'7',	L'8',	L'9',
	L'0',	L''
};

CKeypadScene::CKeypadScene(bool keypadPeriod)
{
    m_nActiveRow    = 0;
    m_nActiveColumn = 0;
    m_nRealPosition = 0;
	m_pTitleText    = NULL;
	m_pInputText    = NULL;
    m_bkeypadPeriod = keypadPeriod;
    CScene();
    m_bResetInput   = false;
}

HRESULT CKeypadScene::Initialize(char* pUrl, char* pTextButtonUrl, eSceneId eCurSceneId, bool bAutoTest)
{
	HRESULT hr = CScene::Initialize(pUrl, pTextButtonUrl, eCurSceneId, false);
	if (FAILED(hr))
	{
		DbgPrint("CScene::Initialize fail to init CKeypadScene");
		return hr;
	}
	hr = BuildKeypad();
	if(FAILED(hr))
	{
		DbgPrint("CScene::Initialize fail to Build Keypad");
		return hr;
	}

	// it should be exactly 4 rows
    ASSERT(m_pKeypad.size() == 4);
	ASSERT((*m_pKeypad[0]).size() == 4);
	ASSERT((*m_pKeypad[1]).size() == 5);
	ASSERT((*m_pKeypad[2]).size() == 4);
	ASSERT((*m_pKeypad[3]).size() == 2);

	Dump();
	AutoTest(eSceneInit);
	if(!g_szUserInput.Empty())
	{
		m_pInputText->SetText(g_szUserInput.GetInputText());
		m_pInputText->HideCursor();
		// first char typed by a user should substitute this string
		m_bResetInput = false;
	}
	return hr;
}

HRESULT CKeypadScene::FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime )
{
	HRESULT hr = S_OK;
	eInputState eCurState = GetInputState(GamepadInput);
	
	do
	{
		if(eCurState == eNoInput)
		{
			break;
		}

		// allow repetition only for up, down, left, right
		if(eCurState == eRepetitivePress)
		{
			if(
				GamepadInput.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ||
				GamepadInput.fY1 == -1 ||
				GamepadInput.wButtons & XINPUT_GAMEPAD_DPAD_UP ||
				GamepadInput.fY1 == 1  ||
				GamepadInput.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ||
				GamepadInput.fX1 == 1 ||
				GamepadInput.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ||
				GamepadInput.fX1 == -1 
				)
				; // continue execution and handle repetition of navigation keys
			else 
			{
				break;
			}

		}
			
		ASSERT(m_nActiveRow >= 0 && m_nActiveColumn >= 0);
		ASSERT((size_t) m_nActiveRow < m_pKeypad.size() && (size_t)m_nActiveColumn < (*m_pKeypad[m_nActiveRow]).size());
	
        // dump the currently pressed joystick input
        CPrimitiveScene::AutoTest(GamepadInput);

		if(GamepadInput.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ||
		GamepadInput.fY1 == -1)
		{
			DbgPrint("CKeypadScene::FrameMove - Scene:%s, DOWN is pressed\n", SceneName[m_eSceneId]);
			MoveDown();	
			AutoTest(eChangeFocus);
            PlaySound( eKeyboardSound, eSoundHighlight ); 

		}

		if(GamepadInput.wButtons & XINPUT_GAMEPAD_DPAD_UP ||
			GamepadInput.fY1 == 1)
		{
			DbgPrint("CKeypadScene::FrameMove - Scene:%s, UP is pressed\n", SceneName[m_eSceneId]);
			MoveUp();
			AutoTest(eChangeFocus);
            PlaySound( eKeyboardSound, eSoundHighlight ); 

		}

		if(GamepadInput.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ||
			   GamepadInput.fX1 == 1)
		{
			DbgPrint("CKeypadScene::FrameMove - Scene:%s, RIGHT is pressed\n", SceneName[m_eSceneId]);
			MoveRight();
			AutoTest(eChangeFocus);
            PlaySound( eKeyboardSound, eSoundHighlight ); 
		}

		if(GamepadInput.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ||
			GamepadInput.fX1 == -1)
		{
			DbgPrint("CKeypadScene::FrameMove - Scene:%s, LEFT is pressed\n", SceneName[m_eSceneId]);
			MoveLeft();
			AutoTest(eChangeFocus);            
            PlaySound( eKeyboardSound, eSoundHighlight ); 

		}
        
		if(GamepadInput.bAnalogButtons[ XINPUT_GAMEPAD_A ] > XINPUT_GAMEPAD_MAX_CROSSTALK)
		{
			DbgPrint("CKeypadScene::FrameMove - Scene:%s, A is pressed\n", SceneName[m_eSceneId]);
			
            hr = PressedA();

			if (hr == XBAPPWRN_SCENE_CHANGED)
			{
				// new input is available
				g_szUserInput.Dirty(true);
				// scene is destroyed by now do not touch any private data 
				return hr;
			}
		}

		if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
		{
	
            eSound  soundType = eSoundAPress;
            if ( g_szUserInput.Empty() )
                soundType = eSoundDisabled;
		    
            CText BackSpace;
			BackSpace.SetTextId(cszKeypadBackspaceTextId);
			UpdateInputText(&BackSpace);
            PlaySound( eKeyboardSound, soundType ); 
		}

		if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_LEFT_TRIGGER])
		{
			int nPos = 1;
			if(!m_bResetInput)
			{
				m_pInputText->MoveCursorHome(); 
				m_bResetInput = !m_bResetInput;
			}
			else
			{
                if ( m_pInputText->GetCursorPosition() < 1 )
                {
                    PlaySound( eKeyboardSound, eSoundDisabled );
                }
				m_pInputText->MoveCursorLeft();
			}
		}

		if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_RIGHT_TRIGGER])
		{
			
			int nPos = 1;
			int nNewPos = 0;
			if(!m_bResetInput)
			{
				m_pInputText->MoveCursorEnd();
				m_bResetInput = !m_bResetInput;
			}
			else
			{
				if ( !m_pInputText->GetText() || (unsigned long)m_pInputText->GetCursorPosition() >= _tcslen(m_pInputText->GetText() ) )
                {
                    PlaySound( eKeyboardSound, eSoundDisabled );
                }
                m_pInputText->MoveCursorRight();
			}
		}

		if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_B] ||
			GamepadInput.wPressedButtons & XINPUT_GAMEPAD_BACK)
		{
            PlaySound( eKeyboardSound, eSoundBPress );
			CSceneNavigator::Instance()->MovePreviousScene();
			// discard the new string, back button was pressed
			g_szUserInput.Dirty(false);
			// scene is destroyed by now do not touch any private data
			return hr;
		}

	} while(0);	

	ASSERT(m_nActiveRow >= 0 && m_nActiveColumn >= 0 && m_nRealPosition >= 0);
	ASSERT((size_t) m_nActiveRow < m_pKeypad.size() && (size_t)m_nActiveColumn < (*m_pKeypad[m_nActiveRow]).size());	
    
	for(size_t i = 0; i<m_pTexts.size(); i++)
	{
		m_pTexts[i]->FrameMove(elapsedTime);
	}
	
	return hr;
}

void CKeypadScene::MoveUp()
{	
	(*m_pKeypad[m_nActiveRow])[m_nActiveColumn]->SetState(eButtonDefault);
	m_nActiveRow--;

	switch(m_nActiveColumn)
	{
    case 0:
        if(m_nActiveRow < 0)
        {
            m_nActiveRow = 3;
            m_nRealPosition = 0;
        }
		break;
    case 1:
        if(m_nActiveRow < 0)
        {
            m_nActiveRow = 3;
            m_nActiveColumn = 0;
            m_nRealPosition = 1;
        }
		break;
	case 2: 
        if(m_nActiveRow < 0)
        {
            if(m_bkeypadPeriod)  // period key
            {
		        m_nActiveRow = 3;
                m_nActiveColumn = 1;
                m_nRealPosition = 2;
            }
            else
            {
		        m_nActiveRow = 2;
                m_nRealPosition = 2;
            }
        }
		break;
	case 3:
    case 4:
        if(m_nActiveRow < 0)
       	    m_nActiveRow = 2;

        // moving onto the arrows
        if(m_nActiveRow == 1)
        {
            if(m_nRealPosition > 2)
                m_nActiveColumn = m_nRealPosition;
            else
                m_nRealPosition = m_nActiveColumn;
        }

        // moving off the arrows
        if(m_nActiveRow == 0)
        {
            m_nRealPosition = m_nActiveColumn;
            m_nActiveColumn = 3;
        }
        break;
	default:
		ASSERT(false);
	};

    if(m_nActiveRow == 2)  // just moved off of the last row
    {
        switch(m_nActiveColumn)
	    {
            case 0:
            case 1:
            case 2:
                m_nActiveColumn = m_nRealPosition;
                break;
            default:
		        ASSERT(m_nActiveColumn == 3);
        };
    }

	(*m_pKeypad[m_nActiveRow])[m_nActiveColumn] ->SetState(eButtonHighlighted);
}

void CKeypadScene::MoveDown()
{
	(*m_pKeypad[m_nActiveRow])[m_nActiveColumn] ->SetState(eButtonDefault);
	m_nActiveRow = (++m_nActiveRow) % 4;

    switch(m_nActiveColumn)
    {
        case 0:
            // snap to 0 button
            if(m_nActiveRow == 3)
            {
                m_nActiveColumn = 0;
                m_nRealPosition = 0;
            }
            break;
        case 1:
            // snap to 0 button
            if(m_nActiveRow == 3)
            {
                m_nActiveColumn = 0;
                m_nRealPosition = 1;
            }
            break;
        case 2:
            if(m_nActiveRow == 3)
            {
                if(m_bkeypadPeriod)  // period key
                {
                    m_nActiveColumn = 1;
                    m_nRealPosition = 2;
                }
                else
                {
                    m_nActiveRow = 0;
                    m_nRealPosition = 2;
                }
            }
            break;
        case 3:
        case 4:
            if(m_nActiveRow == 3)
                m_nActiveRow = 0;

            // moving onto the arrows
            if(m_nActiveRow == 1)
            {
                if(m_nRealPosition > 2)
                    m_nActiveColumn = m_nRealPosition;
                else
                    m_nRealPosition = m_nActiveColumn;
            }

            // moving off the arrows
            if(m_nActiveRow == 2)
            {
                m_nRealPosition = m_nActiveColumn;
                m_nActiveColumn = 3;
            }

            break;
        default:
            ASSERT(false);
    };
    if(m_nActiveRow == 0)  // just moved off of the last row
    {
        switch(m_nActiveColumn)
	    {
            case 0:
            case 1:
            case 2:
                m_nActiveColumn = m_nRealPosition;
                break;
            default:
		        ASSERT(m_nActiveColumn == 3);
        };
    }
	(*m_pKeypad[m_nActiveRow])[m_nActiveColumn] ->SetState(eButtonHighlighted);

}
void CKeypadScene::MoveLeft()
{
	(*m_pKeypad[m_nActiveRow])[m_nActiveColumn] ->SetState(eButtonDefault);
	m_nActiveColumn--;

    if(m_nActiveColumn < 0)
	{
		switch(m_nActiveRow)
		{
		case 0:
            m_nActiveColumn = 3;
			break;
		case 1:
            m_nActiveColumn = 4;
			break;
		case 2:
            m_nActiveColumn = 3;
			break;
		case 3:
            if(m_bkeypadPeriod)  // period key
            {
                m_nActiveColumn = 1;
                m_nRealPosition = 2;
            }
            else
            {
                m_nActiveColumn = 0;
            }
			break;
		default:
			ASSERT(false);
		};
	}
    if(m_nActiveColumn == 0 && m_nActiveRow == 3 && m_bkeypadPeriod) // moving to zero button
    {
        m_nRealPosition = 1;
    }

	(*m_pKeypad[m_nActiveRow])[m_nActiveColumn] ->SetState(eButtonHighlighted);
}

void CKeypadScene::MoveRight()
{
	(*m_pKeypad[m_nActiveRow])[m_nActiveColumn] ->SetState(eButtonDefault);
    m_nActiveColumn++;

    switch(m_nActiveRow)
	{
	case 0:
        if(m_nActiveColumn > 3)
            m_nActiveColumn = 0;
        break;
	case 1:
        if(m_nActiveColumn > 4)
            m_nActiveColumn = 0;
        break;
	case 2:
        if(m_nActiveColumn > 3)
            m_nActiveColumn = 0;
        break;
    case 3:
        if(m_bkeypadPeriod)  // period key
        {
            if(m_nActiveColumn > 1)
            {
                m_nActiveColumn = 0;
                m_nRealPosition = 0;
            }
        }
        else
        {
            if(m_nActiveColumn > 0)
            {
                m_nActiveColumn = 0;
            }
        }
        break;
	default:
		ASSERT(false);
	};
    if(m_nActiveColumn == 1 && m_nActiveRow == 3 && m_bkeypadPeriod) // moving to period button
    {
        m_nRealPosition = 2;
    }
	(*m_pKeypad[m_nActiveRow])[m_nActiveColumn] ->SetState(eButtonHighlighted);
}

HRESULT CKeypadScene::BuildKeypad()
{

	HRESULT hr = S_OK;

    for (int i=0; i < 4; i++)
	{
		CharRowVector* pNextRow = new CharRowVector;
		if(pNextRow)
		{
			m_pKeypad.push_back(pNextRow);
		}
	}
	if(m_pKeypad.size() < 4)
	{
		hr = E_OUTOFMEMORY;
		return hr;
	}

	// the exact number of buttons should 15 = 11 symbols + 4 buttons
	ASSERT(m_pButtons.size() == 15);

	int nCurButtonIndex = 0;
	for(size_t i = 0; i < m_pKeypad.size(); i++)
	{
		for(size_t j = 0; j < 5; j++)
		{
            if ((i == 0 || i == 2) && j > 3)
			{
				break;
			}
			if (i == 3 && j > 1)
			{
                break;
			}

			ASSERT(nCurButtonIndex < 15);
			m_pKeypad[i]->push_back(m_pButtons[nCurButtonIndex++]);
		}
	}

    // Hide the key if not using the period
    if(!m_bkeypadPeriod)
    {
        TG_Shape* pPeriodButton = m_pRoot->FindObject("button_period");
        pPeriodButton->setVisible(false);
    }

    // Set the default focus to the first key
    (*m_pKeypad[0])[3]->SetState(eButtonDefault);
    (*m_pKeypad[0])[0]->SetState(eButtonHighlighted);

	SetCharacterSet();
	int nIndex = FindTextObjForShape(cszKeypadInputText);
	ASSERT(nIndex!=-1);
	m_pInputText = m_pTexts[nIndex];
	m_pInputText->SetText(_T(""));
	m_pInputText->MoveCursorRight(1);

	nIndex = FindTextObjForShape(cszKeypadTitle);
	ASSERT(nIndex!=-1);
	m_pTitleText = m_pTexts[nIndex];
	m_pTitleText->SetText(_T("KeyPad Title"));

	return hr;
}

static const char* kpbuttonIndex [] =
{
	"text_01",	"text_02",	"text_03",
	"text_04",	"text_05",	"text_06",
	"text_07",	"text_08",	"text_09",
	"text_00",	"text_period"
};

void CKeypadScene::SetCharacterSet()
{	    
    for(int i = 0; i < countof(rgkeypadPeriod); i++)
    {
        TCHAR szSymbol[2];
        if(!m_bkeypadPeriod)  // sets period button text to null
	        _stprintf(szSymbol,_T("%c"), rgkeypadBasic[i] );
        else
            _stprintf(szSymbol,_T("%c"), rgkeypadPeriod[i] );

        m_pTexts[FindTextObjForShape(kpbuttonIndex[i])]->SetText(szSymbol);
    }
}

CKeypadScene::~CKeypadScene()
{
	Cleanup();
	CScene::Cleanup();
}

HRESULT CKeypadScene::Cleanup()
{
	// TBD clean up 
	HRESULT hr = S_OK;

    // just unacquire the pointers, scene will take care of memory relase.
	for (size_t i = 0; i < m_pKeypad.size(); i++)
	{
		CharRowVector* pCurRow = m_pKeypad[i];		
        pCurRow->erase(pCurRow->begin(), pCurRow->end());
		delete pCurRow;
        pCurRow = NULL;
	}

	m_pKeypad.erase(m_pKeypad.begin(), m_pKeypad.end());
	return hr;
}

void CKeypadScene::Dump(bool bAllData )
{
	if(bAllData)
	{
		DbgPrint("=====================================================\n");
		DbgPrint("Keypad dump\n");
		DbgPrint("=====================================================\n");

		for (size_t i =0; i<m_pKeypad.size(); i++)
		{
			CharRowVector* pCurRow = m_pKeypad[i];

			for(size_t j=0; j< m_pKeypad[i]->size(); j++)
			{
				DbgPrint("\n++++++++++++++++++++++++++\n");
				(*pCurRow)[j]->Dump(true);
				DbgPrint("\n++++++++++++++++++++++++++\n");
			}
			DbgPrint("---------------------------------------------------\n");
		}
		DbgPrint("=====================================================\n");
	}
}

HRESULT CKeypadScene::PressedA()
{
	CButton* pCurButton = (*m_pKeypad[m_nActiveRow])[m_nActiveColumn];

	eButtonId eCurButton = pCurButton->GetButtonId();

    eSound sound =   eSoundAPress; 

	CText* pCurText = (const_cast<CText*> (pCurButton->GetTextObject()));
	HRESULT hr = S_OK;
	switch(eCurButton)
	{
	case eKeypadDoneButtonId:
		CSceneNavigator::Instance()->MovePreviousScene();
		hr = XBAPPWRN_SCENE_CHANGED;
		break;

	default:
		// input character
		UpdateInputText(pCurText);
        break;
	}

    PlaySound( eKeyboardSound, eSoundAPress );


    return hr;
}

void  CKeypadScene::UpdateInputText(CText* pCurText)
{
		bool bAddChar = true;
		bool bInputProcessed = false;

		if(!m_bResetInput)
		{
			if(pCurText->GetTextId())
			{
			
				if ((_tcsicmp(cszKeypadLeftShiftTextId, pCurText->GetTextId()) == 0))
				{
					m_pInputText->MoveCursorHome();
					bInputProcessed = true;
				}
				else if ( (_tcsicmp(cszKeypadRightShiftTextId, pCurText->GetTextId()) == 0 ))
				{
					m_pInputText->MoveCursorEnd();
					bInputProcessed = true;
				}
			}
			else
			{
				m_pInputText->SetText(_T(""));
				m_pInputText->MoveCursorHome();
			}
			m_bResetInput = !m_bResetInput;	
		}

		// stop getting input after the max len is reached
		if(_tcslen(m_pInputText->GetText()) >= g_szUserInput.GetInputMaxLength())
		{
			bAddChar = false;
		}
		
		TCHAR szBuf[MAX_INPUT_TEXT_SIZE+1];
		ZeroMemory(szBuf, (MAX_INPUT_TEXT_SIZE+1)*sizeof(TCHAR));
		if(pCurText->GetTextId() && !bInputProcessed)
		{
			if(_tcsicmp(cszKeypadBackspaceTextId, pCurText->GetTextId())  == 0)
			{	
				int nOrigLen =_tcslen(m_pInputText->GetText());
				int nPos = m_pInputText->GetCursorPosition();
				const TCHAR* pszInputText = m_pInputText->GetText();
				if( nOrigLen > 0 && nPos >= 1)
				{
					_tcsncpy(szBuf, pszInputText, nPos-1);
					_tcsncpy(szBuf+nPos-1,pszInputText+nPos,nOrigLen - nPos);

					szBuf[nOrigLen-1] = '\0';
				}// if we are at the begining of the buffer, preserve original string
				else if (nPos <= 1)
				{
					_tcscpy(szBuf, pszInputText);
				}
				else 
				{
					szBuf[0] = '\0';
				}
				m_pInputText->SetText(szBuf);
				m_pInputText->MoveCursorLeft();
				bInputProcessed = true;
			}
			else if (_tcsicmp(cszKeypadLeftShiftTextId, pCurText->GetTextId()) == 0)
			{
				m_pInputText->MoveCursorLeft();
				bInputProcessed = true;
			}
			else if (_tcsicmp(cszKeypadRightShiftTextId, pCurText->GetTextId()) == 0)
			{
				m_pInputText->MoveCursorRight();
				bInputProcessed = true;
			}
		}

		if(bAddChar && !bInputProcessed)
		{
			TCHAR szSymbol[2];
			const TCHAR* pszInputText = m_pInputText->GetText();
			const TCHAR* pszCurSymbol = pCurText->GetText();

            ASSERT(_tcslen(pszCurSymbol) == 1);
			_tcscpy(szSymbol,pszCurSymbol);
			
			int nPos = m_pInputText->GetCursorPosition();
			if(nPos < 0)
			{
				m_pInputText->MoveCursorRight();
				nPos = m_pInputText->GetCursorPosition();
			}
			
			_tcsncpy(szBuf, pszInputText, nPos);
			_tcsncpy(szBuf+nPos,szSymbol, 1);
			_tcsncpy(szBuf+nPos+1,pszInputText+nPos,_tcslen(pszInputText) - nPos);

			szBuf[_tcslen(pszInputText) +1] = '\0';
			m_pInputText->SetText(szBuf);
			m_pInputText->MoveCursorRight();		
		}
		g_szUserInput.SetInputText(m_pInputText->GetText());
}




void CKeypadScene::AutoTest(eAutoTestData eState)
{
	switch(eState)
	{
	case eSceneInit:
		DbgPrint("\nAUTO:SCREEN=SCREEN_ID[%d], SCREEN_NAME[%s]\n",m_eSceneId, SceneName[m_eSceneId]);
		for(size_t i = 0; i< m_pButtons.size(); i++)
		{	
			m_pButtons[i]->AutoTest();
		}
		break;
	case eChangeFocus:

		CButton* pCurButton = (*m_pKeypad[m_nActiveRow])[m_nActiveColumn];

		eButtonId eFocusButton = pCurButton->GetButtonId();
		ASSERT(eFocusButton < countof(ButtonName));

		DbgPrint("\nAUTO:FOCUS=BUTTON_ID[%d], BUTTON_NAME[%s]\n",eFocusButton, eFocusButton == eNullButtonId? "NULL": ButtonName[eFocusButton]);
		if (m_pHelpText)
		{
			const TCHAR* pszHelp = m_pHelpText->GetTextId();
			char szBuf[256];
			ZeroMemory(szBuf, countof(szBuf));
			Ansi(szBuf,pszHelp, min(countof(szBuf)-1, _tcslen(pszHelp)) );
			DbgPrint("\nAUTO:TEXT=HELP_TEXT[%s]\n",szBuf);
		}
		break;
	}
}