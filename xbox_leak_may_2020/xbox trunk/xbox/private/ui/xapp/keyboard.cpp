#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Runner.h"

extern CNode* GetTextNode(const TCHAR* szText, float nWidth);
extern bool g_bActiveKey;
extern TCHAR* g_szText;
extern int g_nTextChar;
extern int g_nCurLanguage;

#define MAX_LENGTH		31

#define modeAlpha		0
#define modeSymbol		1
#define modeAccent		2

#define jmodeHiragana	0
#define jmodeKatakana	1
#define jmodeEnglish	2

#define keyShift		1
#define keyCaps			2
#define keyAlpha		3
#define keySym			4
#define keyAcc			5
#define keyLT			6
#define keyRT			7
#define keyBS			8
#define keyDone			9
#define keySpace		10

// Japanese keys
#define keyHiragana     11
#define keyKatakana		12
#define keyEnglish		13

#define keyVoid			14

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
	keyDone,	L'1',	L'2',	L'3',	L'4',	L'5',	L'6',	L'7',	L'8',	L'9',	L'0',
	keyShift,	L'a',	L'b',	L'c',	L'd',	L'e',	L'f',	L'g',	L'h',	L'i',	L'j',
	keyCaps,	L'k',	L'l',	L'm',	L'n',	L'o',	L'p',	L'q',	L'r',	L's',	L't',
	keyAcc,		L'u',	L'v',	L'w',	L'x',	L'y',	L'z',	keyBS,	keyBS,	keyBS,	keyBS,
	keySym,		keySpace,	keySpace,	keySpace,	keySpace,	keySpace,	keySpace,	keyLT,	keyLT,	keyRT,	keyRT,
};

static const WORD rgkeySymbol [] =
{
	keyDone,	L'(',	L')',	L'&',	L'_',	L'^',		L'%',	L'\\',	L'/',	L'@',	L'#',
	keyShift,	L'[',	L']',	L'$',	_pound,	_euro,		_yen,	L';',	L':',	L'\'',	L'"',
	keyCaps,	L'<',	L'>',	L'?',	L'!',	_iquest,	_iexcl,	L'-',	L'*',	L'+',	L'=',
	keyAcc,		L'{',	L'}',	_laquo,	_raquo,	',',		L'.',	keyBS,	keyBS,	keyBS,	keyBS,
	keyAlpha,	keySpace,	keySpace,	keySpace,	keySpace,	keySpace,		keySpace,	keyLT,	keyLT,	keyRT,	keyRT,
};

static const WORD rgkeyAccent [] =
{
	keyDone,	L'1',		L'2',		L'3',		L'4',		L'5',		L'6',		L'7',		L'8',		L'9',		L'0',
	keyShift,	_agrave,	_aacute,	_acirc,		_auml,		_egrave,	_eacute,	_ecirc,		_euml,		_igrave,	_iacute,
	keyCaps,	_icirc,		_iuml,		_ograve,	_oacute,	_ocirc,		_otilde,	_ugrave,	_uacute,	_ucirc,		_uuml,
	keyAlpha,	_yacute,	_yuml,		_ccedil,	_szlig,		_ntilde,	_aelig,		keyBS,		keyBS,		keyBS,		keyBS,
	keySym,		keySpace,	keySpace,	keySpace,	keySpace,	keySpace,	keySpace,	keyLT,		keyLT,		keyRT,		keyRT,
};

static const WORD* rgmode [] =
{
	rgkeyAlpha,
	rgkeySymbol,
	rgkeyAccent
};

static const WORD HiraganaSet [] =
{
	0x3042,	0x3044,	0x3046,	0x3048,	0x304A,	0x308F,	0x3092,	0x3093,	keyVoid,keyVoid,keyHiragana,
	0x304B,	0x304D,	0x304F,	0x3051,	0x3053,	0x3041,	0x3043,	0x3045,	0x3047,	0x3049, keyKatakana,
	0x3055,	0x3057,	0x3059,	0x305B,	0x305D,	0x3063,	0x3083,	0x3085,	0x3087,	0x308e, keyEnglish,
	0x305F,	0x3061,	0x3064,	0x3066,	0x3068,	0x304c,	0x304e,	0x3050,	0x3052,	0x3054, keySpace,
	0x306a,	0x306b,	0x306c,	0x306d,	0x306e,	0x3056,	0x3058,	0x305a,	0x305c,	0x305e, keyBS,
	0x306f,	0x3072,	0x3075,	0x3078,	0x307b,	0x3060,	0x3062,	0x3065,	0x3067,	0x3069, keyLT,
	0x307E,	0x307F,	0x3080,	0x3081,	0x3082,	0x3070,	0x3073,	0x3076,	0x3079,	0x307c, keyRT,
	0x3084,	keyVoid,0x3086,	keyVoid,0x3088,	0x3071,	0x3074,	0x3077,	0x307a,	0x307d, keyVoid,
	0x3089,	0x308a,	0x308b,	0x308c,	0x308d,	0x30fc,	0x3001,	0x3002,	0x300c,	0x300d, keyDone,

};

static const WORD KatakanaSet [] =
{
	0x30A2,	0x30A4,	0x30A6,	0x30A8,	0x30AA,	0x30EF,	0x30F2,	0x30F3,	0x30f4,	keyVoid,keyHiragana,
	0x30AB,	0x30AD,	0x30AF,	0x30B1,	0x30B3,	0x30A1,	0x30A3,	0x30A5,	0x30A7,	0x30A9,	keyKatakana,
	0x30B5,	0x30B7,	0x30B9,	0x30BB,	0x30BD,	0x30C3,	0x30E3,	0x30E5,	0x30E7,	0x30EE, keyEnglish,
	0x30BF,	0x30C1,	0x30C4,	0x30C6,	0x30C8,	0x30AC,	0x30AE,	0x30B0,	0x30B2,	0x30B4, keySpace,
	0x30CA,	0x30CB,	0x30CC,	0x30CD,	0x30CE,	0x30B6,	0x30B8,	0x30BA,	0x30BC,	0x30BE, keyBS,
	0x30CF,	0x30D2,	0x30D5,	0x30D8,	0x30DB,	0x30C0,	0x30C2,	0x30C5,	0x30C7,	0x30C9, keyLT,
	0x30DE,	0x30DF,	0x30E0,	0x30E1,	0x30E2,	0x30D0,	0x30D3,	0x30D6,	0x30D9,	0x30DC, keyRT,
	0x30E4,	keyVoid,0x30E6,	keyVoid,0x30E8,	0x30D1,	0x30D4,	0x30D7,	0x30DA,	0x30DD, keyVoid,
	0x30E9,	0x30EA,	0x30EB,	0x30EC,	0x30ED,	0x30fc,	0x3001,	0x3002,	0x300c,	0x300d, keyDone,
};

static const WORD EnglishSet [] =
{
	0x41,	0x42,	0x43,	0x44,	0x45,	0x61,	0x62,	0x63,	0x64,	0x65,	keyHiragana,
	0x46,	0x47,	0x48,	0x49,	0x4A,	0x66,	0x67,	0x68,	0x69,	0x6A,	keyKatakana,
	0x4B,	0x4C,	0x4D,	0x4E,	0x4F,	0x6B,	0x6C,	0x6D,	0x6E,	0x6F,	keyEnglish,
	0x50,	0x51,	0x52,	0x53,	0x54,	0x70,	0x71,	0x72,	0x73,	0x74,	keySpace,
	0x55,	0x56,	0x57,	0x58,	0x59,	0x75,	0x76,	0x77,	0x78,	0x79,	keyBS,
	0x5A,	0x22,	0x27,	0x40,	0x23,	0x7A,	0x28,	0x29,	0x7B,	0x7D,	keyLT,
	0x26,	0x5E,	0x24,	0xA5,	0x25,	0x2D,	0x2B,	0x3D,	0x2A,	0x2F,	keyRT,
	0x30,	0x31,	0x32,	0x33,	0x34,	0x3F,	0x21,	0x3A,	0x3B,	0x5C,	keyVoid,
	0x35,	0x36,	0x37,	0x38,	0x39,	0x3C,	0x3E,	0x2C,	0x2E,	0x5F,	keyDone,
};

static const WORD* japmode [] =
{
	HiraganaSet,
	KatakanaSet,
	EnglishSet
};

class CKeyboard : public CNode
{
	DECLARE_NODE(CKeyboard, CNode)
public:
	CKeyboard();
	~CKeyboard();

	CNodeArray m_keys;
	CNode* m_frame;
	CNode* m_text;
	CNode* m_control;

	TCHAR* m_string;

	bool m_shift;
	bool m_caps;
	int m_mode;
	int m_jmode;

    void selectKey(int nRow, int nColumn);
	bool validKey(int nRow, int nColumn);
	void selectUp();
	void selectDown();
	void selectLeft();
	void selectRight();
	void activate();

	void Backspace();
	void Delete();
	void CursorLeft();
	void CursorRight();
	void Shift();
	void CycleMode();
	void Insert(const TCHAR* szInsert);

	void Advance(float nSeconds);
	void Render();
	bool OnSetProperty(const PRD* pprd, const void* pvValue);


protected:
	TCHAR m_rgch [MAX_LENGTH + 1];
	int m_nLength;
	int m_nCursor;

	int m_nRow;
	int m_nColumn;

	TCHAR GetCharFromKey(WORD key);

	DECLARE_NODE_PROPS()
	DECLARE_NODE_FUNCTIONS();
};

IMPLEMENT_NODE("Keyboard", CKeyboard, CNode)

START_NODE_PROPS(CKeyboard, CNode)
	NODE_PROP(pt_nodearray, CKeyboard, keys)
	NODE_PROP(pt_node, CKeyboard, frame)
	NODE_PROP(pt_node, CKeyboard, text)
	NODE_PROP(pt_node, CKeyboard, control)
	NODE_PROP(pt_integer, CKeyboard, mode)
	NODE_PROP(pt_integer, CKeyboard, jmode)
	NODE_PROP(pt_boolean, CKeyboard, shift)
	NODE_PROP(pt_boolean, CKeyboard, caps)
	NODE_PROP(pt_string, CKeyboard, string)
END_NODE_PROPS()

START_NODE_FUN(CKeyboard, CNode)
    NODE_FUN_VII(selectKey)
	NODE_FUN_VV(selectUp)
	NODE_FUN_VV(selectDown)
	NODE_FUN_VV(selectLeft)
	NODE_FUN_VV(selectRight)
	NODE_FUN_VV(activate)
	NODE_FUN_VV(Backspace)
	NODE_FUN_VV(Delete)
	NODE_FUN_VV(CursorLeft)
	NODE_FUN_VV(CursorRight)
	NODE_FUN_VV(Shift)
	NODE_FUN_VV(CycleMode)
	NODE_FUN_VS(Insert)
END_NODE_FUN()

CKeyboard::CKeyboard() :
	m_control(NULL),
	m_frame(NULL),
	m_text(NULL),
	m_mode(modeAlpha),
	m_jmode(jmodeHiragana),
	m_shift(false),
	m_caps(false),
	m_string(NULL)
{
	m_nLength = 0;
	m_nCursor = -1; // select all
	m_nRow = 0;
	m_nColumn = 0;
}

CKeyboard::~CKeyboard()
{
	if (m_text != NULL)
		m_text->Release();

	if (m_frame != NULL)
		m_frame->Release();

	if (m_control != NULL)
		m_control->Release();

	delete [] m_string;
}

void CKeyboard::selectKey(int nRow, int nColumn)
{
	int numRows = 4;
	if(g_nCurLanguage == 1)  // Japanese
		numRows = 8;

    if( ( nRow < 0 ) || ( nRow > numRows ) )
    {
        m_nRow = 0;
    }
    else
    {
        m_nRow = nRow;
    }

    if( ( nColumn < 0 ) || ( nColumn > 10 ) )
    {
        m_nColumn = 0;
    }
    else
    {
        m_nColumn = nColumn;
    }
}

bool CKeyboard::validKey(int nRow, int nColumn)
{
	const WORD* rgkey;
	if(g_nCurLanguage == 1)  // Japanese
		rgkey = japmode[m_jmode];
	else
		rgkey = rgmode[m_mode];

	WORD key = rgkey[nRow * 11 + nColumn];

	if(key == keyVoid) // missing key
		return false;

	return true;
}


void CKeyboard::selectUp()
{
retryUp:
	int numRows = 4;
	if(g_nCurLanguage == 1)  // Japanese
		numRows = 8;

	if (m_nRow == 0)
	{
		m_nRow = numRows;
		if(validKey(m_nRow, m_nColumn))
			return;
		else
			goto retryUp;
	}

	m_nRow -= 1;
	if(!validKey(m_nRow, m_nColumn))
		goto retryUp;
}

void CKeyboard::selectDown()
{
retryDown:
	int numRows = 4;
	if(g_nCurLanguage == 1)  // Japanese
		numRows = 8;

	if (m_nRow == numRows)
	{
		m_nRow = 0;
		if(validKey(m_nRow, m_nColumn))
			return;
		else
			goto retryDown;
	}

	m_nRow += 1;
	if(!validKey(m_nRow, m_nColumn))
		goto retryDown;
}

void CKeyboard::selectLeft()
{
retryLeft:
	if (m_nColumn == 0)
	{
		m_nColumn = 10;
		if(validKey(m_nRow, m_nColumn))
			return;
		else
			goto retryLeft;
	}

	if(g_nCurLanguage == 1)  // Japanese
	{
		m_nColumn -= 1;
		if(!validKey(m_nRow, m_nColumn))
			goto retryLeft;
	}
	else
	{
		if (m_nRow == 3)
		{
			if (m_nColumn > 6)
				m_nColumn = 6;
			else
				m_nColumn -= 1;
		}
		else if (m_nRow == 4)
		{
			if (m_nColumn > 8)
				m_nColumn = 8;
			else if (m_nColumn > 6)
				m_nColumn = 6;
			else
				m_nColumn = 0;
		}
		else
		{
			m_nColumn -= 1;
		}
	}
}

void CKeyboard::selectRight()
{
retryRight:
	if (m_nColumn == 10)
	{
		m_nColumn = 0;
		if(validKey(m_nRow, m_nColumn))
			return;
		else
			goto retryRight;
	}
	
	if(g_nCurLanguage == 1)  // Japanese
	{
		m_nColumn += 1;
		if(!validKey(m_nRow, m_nColumn))
			goto retryRight;
	}
	else
	{
		if (m_nRow == 3)
		{
			if (m_nColumn >= 7)
			{
				m_nColumn = 0;
				return;
			}

			m_nColumn += 1;
		}
		else if (m_nRow == 4)
		{
			if (m_nColumn >= 9)
			{
				m_nColumn = 0;
				return;
			}

			if (m_nColumn == 0)
				m_nColumn = 1;
			else if (m_nColumn < 7)
				m_nColumn = 7;
			else if (m_nColumn < 9)
				m_nColumn = 9;
		}
		else
		{
			m_nColumn += 1;
		}
	}
}

void CKeyboard::activate()
{
	const WORD* rgkey;
	if(g_nCurLanguage == 1)  // Japanese
		rgkey = japmode[m_jmode];
	else
		rgkey = rgmode[m_mode];

	WORD key = rgkey[m_nRow * 11 + m_nColumn];

	switch (key)
	{
	case keyDone:
		{
			// BLOCK: Make sure we have something other than spaces...
			{
				for (int i = 0; i < m_nLength; i += 1)
				{
					if (m_rgch[i] != ' ')
						break;
				}

				if (i == m_nLength)
				{
					CallFunction(this, _T("OnError"));
					break;
				}
			}

            // Trim the Right Spaces from the String
            TCHAR* prgcUserString = m_rgch + ( m_nLength - 1 );

            while( *prgcUserString == _T(' ') )
            {
                *prgcUserString = _T('\0');
                prgcUserString--;
            }

            // Trim the Left Spaces from the String
            prgcUserString = m_rgch;
            while( *prgcUserString == _T(' ') )
                prgcUserString++;

            // Copy good characters of the string
            if( prgcUserString != m_rgch )
                MoveMemory( m_rgch, prgcUserString, sizeof(TCHAR) * (_tcslen(prgcUserString) + 1) );

			delete [] m_string;
			m_string = new TCHAR [m_nLength + 1];
			CopyChars(m_string, m_rgch, m_nLength);
			m_string[m_nLength] = 0;

			CallFunction(this, _T("OnDone"));
		}
		break;

	case keyShift:
		Shift();
		break;

	case keyCaps:
		if (m_mode != modeAlpha && m_mode != modeAccent)
			CallFunction(this, _T("OnError"));
		else
			m_caps = !m_caps;
		break;

	case keyAlpha:
		m_mode = modeAlpha;
		break;

	case keySym:
		m_mode = modeSymbol;
		break;

	case keyAcc:
		m_mode = modeAccent;
		break;

	case keyHiragana:
		m_jmode = jmodeHiragana;
		break;

	case keyKatakana:
		m_jmode = jmodeKatakana;
		break;

	case keyEnglish:
		m_jmode = jmodeEnglish;
		break;

	case keyLT:
		CursorLeft();
		break;

	case keyRT:
		CursorRight();
		break;

	case keyBS:
		Backspace();
		break;

	case keySpace:
		if (m_nCursor == -1)
		{
			m_nLength = 0;
			m_nCursor = 0;
		}

		if (m_nLength < MAX_LENGTH)
		{
			MoveMemory(&m_rgch[m_nCursor + 1], &m_rgch[m_nCursor], (m_nLength - m_nCursor) * sizeof (TCHAR));
			m_rgch[m_nCursor] = _T(' ');
			m_nCursor += 1;
			m_nLength += 1;
			//m_shift = false;
		}
		break;

	default:
		if (m_nCursor == -1)
		{
			m_nLength = 0;
			m_nCursor = 0;
		}

		if (m_nLength < MAX_LENGTH)
		{
			MoveMemory(&m_rgch[m_nCursor + 1], &m_rgch[m_nCursor], (m_nLength - m_nCursor) * sizeof (TCHAR));
			m_rgch[m_nCursor] = GetCharFromKey(key);
			m_nCursor += 1;
			m_nLength += 1;
			m_shift = false;
		}
        else
        {
            // We want to play the error sound if the user has "filled" the string and attempts to add
            // another character
            CallFunction(this, _T("OnError"));
        }

		break;
	}
}

void CKeyboard::Backspace()
{
	if (m_nCursor == -1)
	{
		m_nLength = 0;
		m_nCursor = 0;
	}

	if (m_nCursor > 0)
	{
		MoveMemory(&m_rgch[m_nCursor - 1], &m_rgch[m_nCursor], (m_nLength - m_nCursor) * sizeof (TCHAR));
		m_nLength -= 1;
		m_nCursor -= 1;
	}
    else
    {
        // We want to play the error sound if the user has no character to backspace
        CallFunction(this, _T("OnError"));
    }
}

void CKeyboard::Delete()
{
	if (m_nCursor == -1)
	{
		m_nLength = 0;
		m_nCursor = 0;
	}

	if (m_nCursor < m_nLength)
	{
		CursorRight();
		Backspace();
	}
    else
    {
        // We want to play the error sound if the user has no character to delete
        CallFunction(this, _T("OnError"));
    }
}

void CKeyboard::CursorLeft()
{
	if (m_nCursor == -1)
		m_nCursor = 0;
	else if (m_nCursor > 0)
		m_nCursor -= 1;
	else
		CallFunction(this, _T("OnError"));
}

void CKeyboard::CursorRight()
{
	if (m_nCursor == -1)
		m_nCursor = m_nLength;
	else if (m_nCursor < m_nLength)
		m_nCursor += 1;
	else
		CallFunction(this, _T("OnError"));
}

void CKeyboard::Shift()
{
	if (m_mode != modeAlpha && m_mode != modeAccent)
		CallFunction(this, _T("OnError"));
	else
		m_shift = !m_shift;
}

void CKeyboard::CycleMode()
{
	m_mode += 1;    // English Keyboard
    m_jmode += 1;   // Japanese Keyboard

    // English Keyboard
	if (m_mode > 2)
		m_mode = 0;

    // Japanese Keyboard
	if (m_jmode > 2)
		m_jmode = 0;
}

void CKeyboard::Insert(const TCHAR* szInsert)
{
	if (m_nCursor == -1)
	{
		m_nLength = 0;
		m_nCursor = 0;
	}

	int cchInsert = _tcslen(szInsert);

	if (m_nLength + cchInsert > MAX_LENGTH)
	{
		CallFunction(this, _T("OnError"));
		return;
	}

	MoveMemory(&m_rgch[m_nCursor + cchInsert], &m_rgch[m_nCursor], (m_nLength - m_nCursor) * sizeof (TCHAR));
	CopyChars(&m_rgch[m_nCursor], szInsert, cchInsert);
	m_nCursor += cchInsert;
	m_nLength += cchInsert;
	m_shift = false;
}

TCHAR CKeyboard::GetCharFromKey(WORD key)
{
	ASSERT(key >= 32);

	TCHAR ch = (TCHAR)key;

	if (m_caps ^ m_shift)
		ch = (TCHAR)CharUpper((LPTSTR)ch);

	return ch;
}

bool CKeyboard::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	if ((int)pprd->pbOffset == offsetof(m_string))
	{
		TCHAR* szNew = *(TCHAR**)pvValue;
		if (szNew == NULL)
			szNew = _T("");
		int cchNew = _tcslen(szNew);
		if (cchNew > MAX_LENGTH)
			cchNew = MAX_LENGTH;
		CopyChars(m_rgch, szNew, cchNew);
		m_nLength = cchNew;
		m_nCursor = -1;
	}

	return true;
}

void CKeyboard::Render()
{
	const WORD* rgkey;
	if(g_nCurLanguage == 1)  // Japanese
		rgkey = japmode[m_jmode];
	else
		rgkey = rgmode[m_mode];

	if (m_frame != NULL)
		m_frame->Render();

	if (m_text != NULL)
	{
		m_rgch[m_nLength] = 0;
		g_szText = m_rgch;
		g_nTextChar = m_nCursor;
		m_text->Render();
		g_nTextChar = -1;
	}

	int keyLength = 0;
	if(g_nCurLanguage == 1)  // Japanese
	{
		if (m_keys.GetLength() != 99)
		{
			TRACE(_T("\001Japanese keyboard does not have 99 keys!\n"));
			return;
		}
		keyLength = 99;
	}
	else
	{
		if (m_keys.GetLength() != 45)
		{
			TRACE(_T("\001Keyboard does not have 45 keys!\n"));
			return;
		}
		keyLength = 55;
	}

	TCHAR szBuf [2];
	int nNode = 0;
	for (int i = 0; i < keyLength; i += 1)
	{
		if(g_nCurLanguage != 1)  // Not Japanese
		{
			if (i > 0 && rgkey[i] == rgkey[i - 1])
				continue;
		}

		CNode* pNode = m_keys.GetNode(nNode);
		int nKey = m_nRow * 11 + m_nColumn;

		if(g_nCurLanguage != 1)  // Not Japanese
		{
			if (nKey > 40 && nKey <= 43)
				nKey = 40;
			else if (nKey > 45 && nKey <= 50)
				nKey = 45;
			else if (nKey == 52)
				nKey = 51;
			else if (nKey == 54)
				nKey = 53;
		}

		switch (rgkey[i])
		{
		case keyShift:
			g_szText = _T("SHIFT");
			break;

		case keyCaps:
			g_szText = _T("CAPS LOCK");
			break;

		case keyAlpha:
			g_szText = _T("ALPHABET");
			break;

		case keySym:
			g_szText = _T("SYMBOLS");
			break;

		case keyAcc:
			g_szText = _T("ACCENTS");
			break;

		case keyEnglish:
			g_szText = _T("ENGLISH");
			break;

		case keyHiragana:
			g_szText = _T("HIRAGANA");
			break;

		case keyKatakana:
			g_szText = _T("KATAKANA");
			break;

		case keyLT:
			g_szText = _T("<");
			break;

		case keyRT:
			g_szText = _T(">");
			break;

		case keyBS:
			g_szText = _T("BACKSPACE");
			break;

		case keyDone:
			g_szText = _T("DONE");
			break;

		case keySpace:
			g_szText = _T("SPACE");
			break;
		
		case keyVoid:
			g_szText = _T(" ");
			break;

		default:
			szBuf[0] = (TCHAR)rgkey[i];
			szBuf[1] = 0;

			if(g_nCurLanguage != 1)  // Not Japanese
			{
				if (m_caps ^ m_shift)
					szBuf[0] = (TCHAR)CharUpper((LPTSTR)szBuf[0]);
			}

			g_szText = szBuf;
		break;
		}

		g_bActiveKey = (i == nKey);
		if(rgkey[i] != keyVoid)  // key is visible
			pNode->Render();

		nNode += 1;
	}
}

void CKeyboard::Advance(float nSeconds)
{
	CNode::Advance(nSeconds);

	if (m_control != NULL)
		m_control->Advance(nSeconds);
}
