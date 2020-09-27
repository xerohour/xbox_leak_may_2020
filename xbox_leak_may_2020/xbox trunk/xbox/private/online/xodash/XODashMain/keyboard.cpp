#include "std.h"
#include "Scene.h"
#include "Font.h"
#include "tgl.h"
#include "XBInput.h"
#include "SceneNavigator.h"
#include "globals.h"
#include "InputHandle.h"

CFont* GetFont(const TCHAR* szFaceName); 


static const char cszKeyboardTitle[] = "TEXT_NAMESOUNDTRACK";
static const char cszInputText[] = "TEXT_ENTRY";

static const TCHAR cszAccentTextId[]     =  _T("ACCENTS"); 
static const TCHAR cszAlphabetTextId[]   =  _T("ALPHABET");
static const TCHAR cszSymbolsTextId[]    =  _T("SYMBOLS");
static const TCHAR cszSpaceTextId[]      =  _T("SPACE");
static const TCHAR cszBackspaceTextId[]  =  _T("BACKSPACE");
static const TCHAR cszLeftShiftTextId[]  =  _T("LEFT_SHIFT");
static const TCHAR cszRightShiftTextId[] =  _T("RIGHT_SHIFT");

const float cfOffsetX = .1343f;
const float cfOffsetY = .1067f;



static eButtonId eFirstColumnButtons[] = 
{
	eDoneButtonId,    
	eShiftButtonId,   
	eCapsLockButtonId, 
	eAccentsButtonId,  
	eSymbolsButtonId
};


static eButtonId eLastRow[] = 
{
	eSpaceButtonId,
	eLessThanButtonId, 
	eGreaterThanButtonId, 
};
	

#define _nbsp		0x00a0 // non-breaking space
#define _iexcl		0x00a1 // ¡
#define _cent		0x00a2 // ¢
#define _pound		0x00a3 // £
#define _curren		0x00a4 // ¤
#define _yen		0x00a5 // ¥
#define _brvbar		0x00a6 // ¦
#define _sect		0x00a7 // §
#define _uml		0x00a8 // ¨
#define _copy		0x00a9 // ©
#define _ordf		0x00aa // ª
#define _laquo		0x00ab // «
#define _not		0x00ac // ¬
#define _shy		0x00ad // soft hyphen
#define _reg		0x00ae // ®
#define _macr		0x00af // ¯
#define _deg		0x00b0 // °
#define _plusmn		0x00b1 // ±
#define _sup2		0x00b2 // ²
#define _sup3		0x00b3 // ³
#define _acute		0x00b4 // ´
#define _micro		0x00b5 // µ
#define _para		0x00b6 // ¶
#define _middot		0x00b7 // ·
#define _cedil		0x00b8 // ¸
#define _sup1		0x00b9 // ¹
#define _ordm		0x00ba // º
#define _raquo		0x00bb // »
#define _iquest		0x00bf // ¿
#define _Agrave		0x00c0 // À
#define _Aacute		0x00c1 // Á
#define _Acirc		0x00c2 // Â
#define _Atilde		0x00c3 // Ã
#define _Auml		0x00c4 // Ä
#define _Aring		0x00c5 // Å
#define _AElig		0x00c6 // Æ
#define _Ccedil		0x00c7 // Ç
#define _Egrave		0x00c8 // È
#define _Eacute		0x00c9 // É
#define _Ecirc		0x00ca // Ê
#define _Euml		0x00cb // Ë
#define _Igrave		0x00cc // Ì
#define _Iacute		0x00cd // Í
#define _Icirc		0x00ce // Î
#define _Iuml		0x00cf // Ï
#define _ETH		0x00d0 // Ð
#define _Ntilde		0x00d1 // Ñ
#define _Ograve		0x00d2 // Ò
#define _Oacute		0x00d3 // Ó
#define _Ocirc		0x00d4 // Ô
#define _Otilde		0x00d5 // Õ
#define _Ouml		0x00d6 // Ö
#define _times		0x00d7 // ×
#define _Oslash		0x00d8 // Ø
#define _Ugrave		0x00d9 // Ù
#define _Uacute		0x00da // Ú
#define _Ucirc		0x00db // Û
#define _Uuml		0x00dc // Ü
#define _Yacute		0x00dd // Ý
#define _THORN		0x00de // Þ
#define _szlig		0x00df // ß
#define _agrave		0x00e0 // à
#define _aacute		0x00e1 // á
#define _acirc		0x00e2 // â
#define _atilde		0x00e3 // ã
#define _auml		0x00e4 // ä
#define _aring		0x00e5 // å
#define _aelig		0x00e6 // æ
#define _ccedil		0x00e7 // ç
#define _egrave		0x00e8 // è
#define _eacute		0x00e9 // é
#define _ecirc		0x00ea // ê
#define _euml		0x00eb // ë
#define _igrave		0x00ec // ì
#define _iacute		0x00ed // í
#define _icirc		0x00ee // î
#define _iuml		0x00ef // ï
#define _eth		0x00f0 // ð
#define _ntilde		0x00f1 // ñ
#define _ograve		0x00f2 // ò
#define _oacute		0x00f3 // ó
#define _ocirc		0x00f4 // ô
#define _otilde		0x00f5 // õ
#define _ouml		0x00f6 // ö
#define _divide		0x00f7 // ÷
#define _oslash		0x00f8 // ø
#define _ugrave		0x00f9 // ù
#define _uacute		0x00fa // ú
#define _ucirc		0x00fb // û
#define _uuml		0x00fc // ü
#define _yacute		0x00fd // ý
#define _thorn		0x00fe // þ
#define _yuml		0x00ff // ÿ

#define _euro		0x20ac // euro sign

static const WORD rgkeyAlpha [] =
{
	L'1',	L'2',	L'3',	L'4',	L'5',	L'6',	L'7',	L'8',	L'9',	L'0',
	L'a',	L'b',	L'c',	L'd',	L'e',	L'f',	L'g',	L'h',	L'i',	L'j',
	L'k',	L'l',	L'm',	L'n',	L'o',	L'p',	L'q',	L'r',	L's',	L't',
	L'u',	L'v',	L'w',	L'x',	L'y',	L'z'
};

static const WORD rgkeySymbol [] =
{
	L'(',	L')',	L'&',	L'_',	L'^',		L'%',	L'\\',	L'/',	L'@',	L'#',
	L'[',	L']',	L'$',	_pound,	_euro,		_yen,	L';',	L':',	L'\'',	L'"',
	L'<',	L'>',	L'?',	L'!',	_iquest,	_iexcl,	L'-',	L'*',	L'+',	L'=',
	L'{',	L'}',	_laquo,	_raquo,	',',		L'.',	
};

static const WORD rgkeyAccent [] =
{
	L'1',		L'2',		L'3',		L'4',		L'5',		L'6',		L'7',		L'8',		L'9',		L'0',
	_agrave,	_aacute,	_acirc,		_auml,		_egrave,	_eacute,	_ecirc,		_euml,		_igrave,	_iacute,
	_icirc,		_iuml,		_ograve,	_oacute,	_ocirc,		_otilde,	_ugrave,	_uacute,	_ucirc,		_uuml,
	_yacute,	_yuml,		_ccedil,	_szlig,		_ntilde,	_aelig
};


static const WORD* rgKeys[] = {rgkeyAlpha,  rgkeySymbol, rgkeyAccent};


CKeyboardScene::CKeyboardScene():
	m_nActiveRow (0),
	m_nActiveColumn(0),
	m_nRealPosition(0),
	m_bCapitalActive(false),
	m_bAccentActive(false),
	m_bSymbolsActive(false),
	m_bShiftActive(false),
	m_nActiveCharSetIndex(0),
	m_pTitleText(NULL),
	m_pInputText(NULL),
	CScene(),
	m_bResetInput(false)

{

}

HRESULT CKeyboardScene::Initialize(char* pUrl, char* pTextButtonUrl, eSceneId eCurSceneId, bool bAutoTest)
{
	HRESULT hr = CScene::Initialize(pUrl, pTextButtonUrl, eCurSceneId, false);
	if (FAILED(hr))
	{
		DbgPrint("CScene::Initialize fail to init CKeyboardScene");
		return hr;
	}
	hr = BuildKeyboard();
	if(FAILED(hr))
	{
		DbgPrint("CScene::Initialize fail to Buildkeyboard");
		return hr;
	}

	// it should be exactly 5 rows
    ASSERT(m_pKeyboard.size() == 5);
	ASSERT((*m_pKeyboard[0]).size() == 11);
	ASSERT((*m_pKeyboard[1]).size() == 11);
	ASSERT((*m_pKeyboard[2]).size() == 11);
	ASSERT((*m_pKeyboard[3]).size() == 8);
	ASSERT((*m_pKeyboard[4]).size() == 4);

	ASSERT(countof(rgkeyAlpha) == countof(rgkeySymbol));
	ASSERT(countof(rgkeyAlpha) == countof(rgkeyAccent));

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

HRESULT CKeyboardScene::FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime )
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

		
		CPrimitiveScene::AutoTest(GamepadInput);
		ASSERT(m_nActiveRow >= 0 && m_nActiveColumn >= 0);
		ASSERT((size_t) m_nActiveRow < m_pKeyboard.size() && (size_t)m_nActiveColumn < (*m_pKeyboard[m_nActiveRow]).size());
	
		if(GamepadInput.wButtons & XINPUT_GAMEPAD_DPAD_DOWN ||
		GamepadInput.fY1 == -1)
		{
			DbgPrint("CKeyboardScene::FrameMove - Scene:%s, DOWN is pressed\n", SceneName[m_eSceneId]);
			MoveDown();	
			AutoTest(eChangeFocus);
            PlaySound( eKeyboardSound, eSoundHighlight ); 
		}

		if(GamepadInput.wButtons & XINPUT_GAMEPAD_DPAD_UP ||
			GamepadInput.fY1 == 1)
		{
			DbgPrint("CKeyboardScene::FrameMove - Scene:%s, UP is pressed\n", SceneName[m_eSceneId]);
			MoveUp();
			AutoTest(eChangeFocus);
            PlaySound( eKeyboardSound, eSoundHighlight ); 
		}

		if(GamepadInput.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT ||
			   GamepadInput.fX1 == 1)
		{
			DbgPrint("CKeyboardScene::FrameMove - Scene:%s, RIGHT is pressed\n", SceneName[m_eSceneId]);
			MoveRight();
			AutoTest(eChangeFocus);
            PlaySound( eKeyboardSound, eSoundHighlight ); 
		}

		if(GamepadInput.wButtons & XINPUT_GAMEPAD_DPAD_LEFT ||
			GamepadInput.fX1 == -1)
		{
			DbgPrint("CKeyboardScene::FrameMove - Scene:%s, LEFT is pressed\n", SceneName[m_eSceneId]);
			MoveLeft();
			AutoTest(eChangeFocus);
            PlaySound( eKeyboardSound, eSoundHighlight ); 
		}
		
		if(GamepadInput.bAnalogButtons[ XINPUT_GAMEPAD_A ] > XINPUT_GAMEPAD_MAX_CROSSTALK)
		{
			DbgPrint("CKeyboardScene::FrameMove - Scene:%s, A is pressed\n", SceneName[m_eSceneId]);
	

			hr = PressedA();
			
			if (hr == XBAPPWRN_SCENE_CHANGED)
			{
				// scene is destroyed by now do not touch any private data 
				g_szUserInput.Dirty(true);
				return hr;
			}

           // PressedA takes care of pressing sounds
		}

		if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
		{
	        eSound  soundType = eSoundAPress;
            if ( g_szUserInput.Empty() )
                soundType = eSoundDisabled;

		    CText BackSpace;
			BackSpace.SetTextId(cszBackspaceTextId);
			UpdateInputText(&BackSpace);

             PlaySound( eKeyboardSound, soundType ); 
		}

		if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_BLACK])
		{
			CText Space;
			Space.SetTextId(cszSpaceTextId);
			UpdateInputText(&Space);

            PlaySound( eKeyboardSound, eSoundAPress ); 

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
		
		if(GamepadInput.bAnalogButtons[ XINPUT_GAMEPAD_X ] > XINPUT_GAMEPAD_MAX_CROSSTALK)
		{
			DbgPrint("CKeyboardScene::FrameMove - Scene:%s, X is pressed\n", SceneName[m_eSceneId]);
			PressedX();
		}

		if(GamepadInput.bPressedAnalogButtons[XINPUT_GAMEPAD_B] ||
			GamepadInput.wPressedButtons & XINPUT_GAMEPAD_BACK)
		{
            PlaySound( eKeyboardSound, eSoundBPress );
			CSceneNavigator::Instance()->MovePreviousScene();
			// scene is destroyed by now do not touch any private data
			g_szUserInput.Dirty(false);
			return hr;
		}
	} while(0);	

	ASSERT(m_nActiveRow >= 0 && m_nActiveColumn >= 0 && m_nRealPosition >= 0);
	ASSERT((size_t) m_nActiveRow < m_pKeyboard.size() && (size_t)m_nActiveColumn < (*m_pKeyboard[m_nActiveRow]).size());	
	
	for(size_t i = 0; i<m_pTexts.size(); i++)
	{
		m_pTexts[i]->FrameMove(elapsedTime);
	}


	return hr;
}

void CKeyboardScene::MoveUp()
{	
	(*m_pKeyboard[m_nActiveRow])[m_nActiveColumn] ->SetState(eButtonDefault);
	m_nActiveRow--;
	if(m_nActiveRow < 0)
	{
		m_nActiveRow = 4;
	}
	switch(m_nActiveRow)
	{
	case 2: 
		m_nActiveColumn = m_nRealPosition;
		break;
	case 3:
		m_nActiveColumn = min(7,m_nRealPosition);
		break;
	case 4:
		m_nRealPosition = m_nActiveColumn;
		if(m_nActiveColumn == 0)
			break;

		if(m_nActiveColumn <7)
		{
			m_nActiveColumn = 1;
			break;
		}
		// 7-8 maps to <
		if(m_nRealPosition == 7 || m_nRealPosition == 8)
		{
			m_nActiveColumn = 2;
			break;
		}
		if(m_nRealPosition == 9 || m_nRealPosition == 10)
		{
			m_nActiveColumn = 3;
			break;
		}
		ASSERT(false);
		break;
	default:
		ASSERT(m_nActiveRow == 1 || m_nActiveRow == 0);
	}
	(*m_pKeyboard[m_nActiveRow])[m_nActiveColumn] ->SetState(eButtonHighlighted);

}

void CKeyboardScene::MoveDown()
{
	(*m_pKeyboard[m_nActiveRow])[m_nActiveColumn] ->SetState(eButtonDefault);
	m_nActiveRow = (++m_nActiveRow) % 5;
	switch (m_nActiveRow)
	{
		case 0:
			m_nActiveColumn = m_nRealPosition;
			break;
		case 2:
			m_nRealPosition = m_nActiveColumn; // save it 
			break;
		case 3:
			m_nActiveColumn = min(7,m_nActiveColumn);
			break;
		case 4:
			// map 1-6 to Space
			if(m_nActiveColumn == 0)
			{
				break;
			}
			if(m_nRealPosition < 7)
			{
				m_nActiveColumn = 1;
				break;
			}
			// 7-8 maps to <
			if(m_nRealPosition == 7 || m_nRealPosition == 8)
			{
				m_nActiveColumn = 2;
				break;
			}
			if(m_nRealPosition == 9 || m_nRealPosition == 10)
			{
				m_nActiveColumn = 3;
				break;
			}
			ASSERT(false);
			break;
		default:
			ASSERT(m_nActiveRow == 1);
	}
	(*m_pKeyboard[m_nActiveRow])[m_nActiveColumn] ->SetState(eButtonHighlighted);

}
void CKeyboardScene::MoveLeft()
{
	(*m_pKeyboard[m_nActiveRow])[m_nActiveColumn] ->SetState(eButtonDefault);
	m_nActiveColumn--;
	if(m_nActiveColumn < 0)
	{
		switch(m_nActiveRow)
		{
		case 0:
		case 1:
		case 2:
			m_nActiveColumn = 10;
			break;
		case 3:
			m_nActiveColumn = 7;
			break;
		case 4:
			m_nActiveColumn = 3;
			break;

		default:
			ASSERT(false);
		};
	}

	switch(m_nActiveRow)
	{
	case 0:
	case 1:
	case 2:
		m_nRealPosition = m_nActiveColumn;
		break;
	case 3:
		m_nRealPosition = ((7 == m_nActiveColumn) ? 10 : m_nActiveColumn);
		break;
	case 4:
		if(m_nActiveColumn == 0)
		{
			m_nRealPosition = 0;
			break;
		}
		if(m_nActiveColumn == 1)
		{
			m_nRealPosition = 6;
			break;
		}

		if(m_nActiveColumn == 2)
		{
			m_nRealPosition = 8;
			break;
		}
		
		if(m_nActiveColumn == 3)
		{
			m_nRealPosition = 10;
			break;
		}
	default:
		ASSERT(false);
	}
	(*m_pKeyboard[m_nActiveRow])[m_nActiveColumn] ->SetState(eButtonHighlighted);
    


}

void CKeyboardScene::MoveRight()
{
	(*m_pKeyboard[m_nActiveRow])[m_nActiveColumn] ->SetState(eButtonDefault);
	switch(m_nActiveRow)
	{
	case 0:
	case 1:
	case 2:
		m_nActiveColumn = (++m_nActiveColumn)%11;
		m_nRealPosition = m_nActiveColumn;
		break;
    case 3:
		m_nActiveColumn = (++m_nActiveColumn)%8;
		m_nRealPosition = min(7, m_nActiveColumn);
		break;
	case 4:
		m_nActiveColumn = (++m_nActiveColumn)%4;
		if(m_nActiveColumn < 2)
		{
			m_nRealPosition = m_nActiveColumn;
			break;
		}
		if(m_nActiveColumn == 2)
		{
			m_nRealPosition = 7;
			break;
		}
		if(m_nActiveColumn == 3)
		{
			m_nRealPosition = 9;
			break;
		}

		break;

	default:
		ASSERT(false);
	};
	(*m_pKeyboard[m_nActiveRow])[m_nActiveColumn] ->SetState(eButtonHighlighted);	

 
}

HRESULT CKeyboardScene::BuildKeyboard()
{

	HRESULT hr = S_OK;
	for (int i=0; i < 5; i++)
	{
		CharRowVector* pNextRow = new CharRowVector;
		if(pNextRow)
		{
			m_pKeyboard.push_back(pNextRow);
		}
	}
	if(m_pKeyboard.size() < 5)
	{
		hr = E_OUTOFMEMORY;
		return hr;
	}

	// the exact number of buttons should 45 = 36 symbols + 9 buttons
	ASSERT(m_pButtons.size() == 45);
	int nCurButtonIndex = 0;
	for(size_t i = 0; i < m_pKeyboard.size(); i++)
	{
		for(size_t j = 0; j < 11; j++)
		{
			if (i == 3 && j > 7)
			{
				break;
			}
			if(i == 4 && j > 3)
			{
				break;
			}
			ASSERT(nCurButtonIndex < 45);
			m_pKeyboard[i]->push_back(m_pButtons[nCurButtonIndex++]);

		}
	}

	for(size_t i = 0; i < m_pButtons.size() - 5; i++)
	{
		// skip all the buttons and hold only input symbols
		if( i % 11 == 0)
			continue;
		m_pSymbols.push_back(const_cast<CText*> (m_pButtons[i]->GetTextObject())  );
	}
	SetCharacterSet();
	int nIndex = FindTextObjForShape(cszInputText);
	ASSERT(nIndex!=-1);
	m_pInputText = m_pTexts[nIndex];
	m_pInputText->SetText(_T(""));

	nIndex = FindTextObjForShape(cszKeyboardTitle);
	ASSERT(nIndex!=-1);
	m_pTitleText = m_pTexts[nIndex];
	m_pTitleText->SetText(_T("KB TITLE"));
	
	return hr;
}


CKeyboardScene::~CKeyboardScene()
{
	Cleanup();
	CScene::Cleanup();

}

HRESULT CKeyboardScene::Cleanup()
{
	// TBD clean up 
	HRESULT hr = S_OK;

// just unacquire the pointers, scene will take care of memory relase.
	for (size_t i =0; i<m_pKeyboard.size(); i++)
	{
		CharRowVector* pCurRow = m_pKeyboard[i];		
		pCurRow->erase(pCurRow->begin(), pCurRow->end());
		delete pCurRow;
		pCurRow = NULL;
	}

	m_pKeyboard.erase(m_pKeyboard.begin(), m_pKeyboard.end());

	m_pSymbols.erase(m_pSymbols.begin(), m_pSymbols.end());
	return hr;
}



void CKeyboardScene::FindControlButton(eButtonId eButton, int* pnRow, int* pnCol)
{
	
	size_t j=0 ;
	*pnRow = -1;
	*pnCol = -1;
	bool bFound = false;
	
	for (size_t i =0; i<m_pKeyboard.size(); i++)
	{
		if(bFound)
		{
			break;
		}

		CharRowVector* pCurRow = m_pKeyboard[i];
		for(j=0; j< m_pKeyboard[i]->size(); j++)
		{
			if((*pCurRow)[j] && ((*pCurRow)[j])->GetButtonId() == eButton)
			{
				bFound = true;
				*pnRow = i;
				*pnCol = j;
				break;
			}
		}
	}


}

void CKeyboardScene::Dump(bool bAllData )
{
	if(bAllData)
	{
		DbgPrint("=====================================================\n");
		DbgPrint("Keyboard dump\n");
		DbgPrint("=====================================================\n");

		for (size_t i =0; i<m_pKeyboard.size(); i++)
		{
			CharRowVector* pCurRow = m_pKeyboard[i];

			for(size_t j=0; j< m_pKeyboard[i]->size(); j++)
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

HRESULT CKeyboardScene::PressedA()
{
	CButton* pCurButton = (*m_pKeyboard[m_nActiveRow])[m_nActiveColumn];

	eButtonId eCurButton = pCurButton->GetButtonId();
    	
    eSoundType  soundType = pCurButton->GetSoundType();

	CText* pCurText = (const_cast<CText*> (pCurButton->GetTextObject()));
	HRESULT hr = S_OK;
	switch(eCurButton)
	{
	case eDoneButtonId:
		CSceneNavigator::Instance()->MovePreviousScene();
		hr = XBAPPWRN_SCENE_CHANGED;
		break;
	case eShiftButtonId:
		m_bShiftActive = true;
		m_bCapitalActive = !m_bCapitalActive;
		SetCharacterSet();
		break;

	case eCapsLockButtonId:
		m_bCapitalActive = !m_bCapitalActive;
		SetCharacterSet();
		break;

	case eAccentsButtonId:
		m_bAccentActive = !m_bAccentActive; 
		m_bAccentActive? m_nActiveCharSetIndex = 2 : m_nActiveCharSetIndex = 0;
		SetCharacterSet();
		
		m_bAccentActive? pCurText->SetTextId(cszAlphabetTextId) : pCurText->SetTextId(cszAccentTextId);
		if(m_bAccentActive)
		{
			int nRow, nCol; 
			FindControlButton(eSymbolsButtonId, &nRow, &nCol);
			ASSERT(nRow!=-1 && nCol!=-1);
			CButton* pButton = (*m_pKeyboard[nRow])[nCol];
			ASSERT(pButton);
			CText* pText = const_cast<CText*> (pButton->GetTextObject());
			ASSERT(pText);
			if(_tcsicmp(pText->GetTextId(), pCurText->GetTextId()) == 0)
			{
				pText->SetTextId(cszSymbolsTextId);
			}
		}
		break;

	case eSymbolsButtonId:
		
		m_bSymbolsActive = !m_bSymbolsActive;
		m_bSymbolsActive? m_nActiveCharSetIndex = 1 : m_nActiveCharSetIndex = 0;
		SetCharacterSet();
		
		m_bSymbolsActive? pCurText->SetTextId(cszAlphabetTextId) : pCurText->SetTextId(cszSymbolsTextId);
		if(m_bSymbolsActive)
		{
			int nRow, nCol; 
			FindControlButton(eAccentsButtonId, &nRow, &nCol);
			ASSERT(nRow!=-1 && nCol!=-1);
			CButton* pButton = (*m_pKeyboard[nRow])[nCol];
			ASSERT(pButton);
			CText* pText = const_cast<CText*> (pButton->GetTextObject());
			ASSERT(pText);
			if(_tcsicmp(pText->GetTextId(), pCurText->GetTextId()) == 0)
			{
				pText->SetTextId(cszAccentTextId);
			}
		}
		
		break;
	default:
		// input character
		UpdateInputText(pCurText);
		// shift affects only the next typed character
		if(m_bShiftActive)
		{
			m_bCapitalActive = !m_bCapitalActive;
			SetCharacterSet();
			m_bShiftActive = false;
		}
	}

    PlaySound( eKeyboardSound, eSoundAPress );

	return hr;
}

void CKeyboardScene::SetCharacterSet()
{	
	ASSERT(m_nActiveCharSetIndex < countof(rgKeys));
	const WORD* pCur = rgKeys[m_nActiveCharSetIndex];
	static WORD delta = (_agrave - _Agrave);

	for (int i=0; i<countof(rgkeyAlpha); i++)
	{
		TCHAR szSymbol[2];
		
		if(m_bCapitalActive)
		{
			_stprintf(szSymbol,_T("%c"),ToUpper(rgKeys[m_nActiveCharSetIndex][i]) );
		}
		else
		{
			_stprintf(szSymbol,_T("%c"), ToLower(rgKeys[m_nActiveCharSetIndex][i]) );
		}
		m_pSymbols[i]->SetText(szSymbol);
	}
}

void CKeyboardScene::PressedX()
{
	m_nActiveCharSetIndex = (++m_nActiveCharSetIndex)%3;
	ASSERT(m_nActiveCharSetIndex < countof(rgKeys));
	SetCharacterSet();
}


//-----------------------------------------------------------------------------
// Name: ToUpper()
// Desc: Convert WCHAR to upper case. Handles accented characters properly.
//-----------------------------------------------------------------------------
WCHAR  CKeyboardScene::ToUpper( WCHAR c ) // static
{
    // The code solution is slower but smaller
    if( c >= 'a' && c <= 'z' )
        return c - ('a' - 'A');
    if( c >= 0xE0 && c <= 0xFD )
        return c - (0xE0 - 0xC0);
    return c;
}




//-----------------------------------------------------------------------------
// Name: ToLower()
// Desc: Convert WCHAR to lower case. Handles accented characters properly.
//-----------------------------------------------------------------------------
WCHAR  CKeyboardScene::ToLower( WCHAR c ) // static
{
    // The code solution is slower but smaller
    if( c >= 'A' && c <= 'Z' )
        return c + ( 'a' - 'A' );
    if( c >= 0xC0 && c <= 0xDD )
        return c + ( 0xE0 - 0xC0 );
    return c;

}


void  CKeyboardScene::UpdateInputText(CText* pCurText)
{
		bool bAddChar = true;
		bool bInputProcessed = false;

		// too many possibilities:
		// basic idea: when you first time popup the keyboard it includes original string (if any) in the input
		// field. If string is not empty, first user input dictates whether to erase original value or not
		// If the very first user input is left/right trigger or '<'/'>' then preserve the string, otherwise
		// remove the string.
		if(!m_bResetInput)
		{
			if(pCurText->GetTextId())
			{
			
				if ((_tcsicmp(cszLeftShiftTextId, pCurText->GetTextId()) == 0))
				{
					m_pInputText->MoveCursorHome();
					bInputProcessed = true;
				}
				else if ( (_tcsicmp(cszRightShiftTextId, pCurText->GetTextId()) == 0 ))
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
		int nLen = _tcslen(m_pInputText->GetText());
		if(_tcslen(m_pInputText->GetText()) >= g_szUserInput.GetInputMaxLength())
		{
			bAddChar = false;
		}
		
		TCHAR szBuf[MAX_INPUT_TEXT_SIZE+1];
		ZeroMemory(szBuf, (MAX_INPUT_TEXT_SIZE+1)*sizeof(TCHAR));
		if(pCurText->GetTextId() && !bInputProcessed)
		{
			if(_tcsicmp(cszBackspaceTextId, pCurText->GetTextId())  == 0)
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
			else if (_tcsicmp(cszLeftShiftTextId, pCurText->GetTextId()) == 0)
			{
				m_pInputText->MoveCursorLeft();
				bInputProcessed = true;
			}
			else if (_tcsicmp(cszRightShiftTextId, pCurText->GetTextId()) == 0)
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
			if(pCurText->GetTextId() && _tcsicmp(cszSpaceTextId, pCurText->GetTextId()) == 0)
			{
				_tcscpy(szSymbol,_T(" "));
			}
			else
			{
				ASSERT(_tcslen(pszCurSymbol) == 1);
				_tcscpy(szSymbol,pszCurSymbol);
			}
			
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
			g_szUserInput.SetInputText(m_pInputText->GetText());
		}
		
}



void CKeyboardScene::AutoTest(eAutoTestData eState)
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

		CButton* pCurButton = (*m_pKeyboard[m_nActiveRow])[m_nActiveColumn];

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