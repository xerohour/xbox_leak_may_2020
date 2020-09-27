#pragma once

#include "std.h"
#include "XOConst.h"
#include <list>
#include <vector>
class TG_Shape;
class CFont;
class File;


using namespace std;

class CSceneKeyboard;
struct XBGAMEPAD;

class SmartObject 
{
public:
	// Keyboard generates buttons, access to TG_Shape is required
	friend CSceneKeyboard;
	SmartObject(TG_Shape* pAppearance = NULL) : m_pAppearance(pAppearance) {}
	virtual ~SmartObject();
	SmartObject(const SmartObject&);
	SmartObject& operator=(const SmartObject&);
    // override as necessary
    virtual HRESULT Initialize() { return S_OK;}
	virtual void Cleanup();
	virtual HRESULT FrameMove( float fElapsedTime ) {return S_OK;};
	virtual HRESULT Render( DWORD flags = 0) { return S_OK;}

    
    virtual HRESULT LoadFromXBG( File* pFile, TG_Shape* m_pRoot  ){return S_OK; }
    
    virtual void Dump(bool bAllData = false) {} 
	inline const TG_Shape* GetShape() { return m_pAppearance;}
	virtual void AutoTest() {}

    virtual void Show( bool bDrawThisObject );
    virtual bool    IsShowing() const;

protected:
	TG_Shape* m_pAppearance;
	void CopyObj(const SmartObject&);
};



struct TEXTVERTEX;
class CText: public SmartObject
{

public:
	CText();
	~CText();
	CText(const CText&);
	CText& operator = (const CText& OtherObj);
	
	void SetTextId(const TCHAR* pNewTextId);
	inline const TCHAR* GetTextId() const { return m_szTextId;}

	void SetText(const TCHAR*);
	inline const TCHAR* GetText() const { return m_szText;}
	void    SetMaterial( long resourceMgrID );

	inline bool IsHelpText() { return m_bIsHelpText;}
	const int GetCursorPosition() {return m_nCursorPosition;}
	int MoveCursorLeft(int nPos = 1); 
	int MoveCursorRight(int nPos = 1);
	inline int HideCursor() { m_nCursorPosition = -1; return m_nCursorPosition;}
	int MoveCursorHome();
	int MoveCursorEnd();

	HRESULT Render( DWORD flags = 0) ;
	HRESULT FrameMove( float fElapsedTime );
	HRESULT Initialize();
    HRESULT LoadFromXBG( File* pFile, TG_Shape* m_pRoot );
	void Cleanup();

	void Dump(bool bAllData = false);

	void AutoTest();

    virtual void Show( bool bDrawThisObject );
    virtual bool    IsShowing() const { return m_bDraw; }



private:

	bool m_bTranslate;    // whether the text needs to be localized ,default is true

	TCHAR* m_szTextId;    // TextId if the text should be translated,
	TCHAR* m_szText;       // text to be displayed 

	const TCHAR* m_szFont;       // Valid Values: body, heading, default is body 
	char m_adjust;      // Valid Values: 'L','R','M'. 
	bool m_bSingleLine;  // Break to next line or not if the width of text is larger then the specified width
	
	float m_height;      // the height of the text rectangular, default is 0, when 0 height is set by geometry 
	float m_width;       // the width of the text rectangular, default is 0, when 0 width is set by geometry 
	// if width is specified and less than the width to render the text then the text is broken to separate lines
    // if m_bSingleLine = false, otherwise (m_bSingleLine = false) text is cut to fit the width
	
	float m_scroll;     // calculated based on ScrollRate - applied only for the text that does not fit the preset rectangular boundaries 
	float m_scrollRate;  // scroll rate for the scrolling text - stores what part of the whole text height should be changed when  scrolling
	float m_scrollDelay;  // speed of scrolling

    float m_fScaleX;        // scale for the mesh
    float m_fScaleY;        // scale for the mesh

	long    m_materialID;       // set the color

    bool  m_bIsHelpText;    // is this a piece of help text that needs to get updated?
	int m_nCursorPosition;  // for input text draw cursor, -1 for static texts
	float m_fLastRedraw;    // if cursor presents, redraw the mesh once in a while

	LPD3DXMESH m_pMesh;  // Mesh is build based on text string  
	D3DXVECTOR3 m_bboxMin, m_bboxMax;  // actual size of text to render calculated 

	float m_timeToScroll;  // calculated 

	void CopyObj(const CText& OtherObj);

    static D3DXMATRIX   s_scaleMatrix;
	DWORD m_nCurLanguage;

    bool    m_bDraw;

private:
	static void VerticalFade(TEXTVERTEX* verts, int nVertexCount, float nTop, float nBottom, float nScroll);
};

class CButton : public SmartObject
{
 
    public: 
        CButton(TG_Shape* pAppearance = NULL );
        virtual ~CButton();
      	virtual void Cleanup();

        virtual void SetState(eButtonState);
        eButtonState GetState() const { return m_eButtonState; }
        bool IsDisabled () const;
        void SetHelpText(const TCHAR* pHelpText);
		const TCHAR* GetHelpText() const;

        virtual void SetTextObjects( CText* pTextObj, CText* pInputText )
		{ // should be NULL
			ASSERT(!pInputText);
			m_pTextObj = pTextObj; 
		}

        virtual void SetTextId( const TCHAR* pTextTopID, const TCHAR* pTextBottomID = NULL );
        virtual void SetText( const TCHAR* pTextTop, const TCHAR* pTextBottom = NULL );

        void            SetSoundType( eSoundType type ){ m_eSound = type; }
        eSoundType      GetSoundType( ) const{ return m_eSound; }

      	virtual inline const CText* GetInputTextObject() { return NULL; }        

		inline const CText* GetTextObject() { return m_pTextObj;}

        virtual HRESULT LoadFromXBG( File* pFile, TG_Shape* pRoot );

        inline void SetCallback(ButtonCallBack );
	    inline eButtonId GetButtonId();
		inline eButtonOrientation GetOrientation();
        virtual void Show( bool bDrawThisObject );

        HRESULT    HandlePress(); // call the callback if there is one
		
		void Dump(bool bAllData = false);

        char     m_textObjId;
		char     m_textObjId2;
		void AutoTest();

		eButtonRTTI m_eType;

	protected:
		static long MaterialMap[eButtonStateMax][3]; // standard, egglow, text
		eButtonState    m_eButtonState; 

        eSoundType      m_eSound;
    private:
		// if required think about TG_Shape creation when new button is created
		// main problem: if text should be associated with specific button then 
		// two texts nodes will be created: one in the copy ctor of the button and
		// one in copy ctor of the text ( remember that text's TG_Shape is a child of the Button)
		CButton(const CButton&);
		CButton operator= (const CButton&);

        TCHAR*           m_pHelpText;  // help string 
		CText*          m_pTextObj;   // pointer to the text object associated with the Button, button is not 
		// responsible for the allocation and release of Text object memory
        
		TG_Shape**      m_pEgglowShapes;
        long            m_nEgglowCount;

        TG_Shape**      m_pStandardShapes;
        long            m_nStandardCount;

	    eButtonId       m_eButtonId ;
	    eButtonOrientation m_eOrientation;
	    ButtonCallBack m_pfnCallBack;

        void CopyObj(const CButton& OtherButton);

};


inline eButtonId CButton::GetButtonId()
{
	return m_eButtonId;
}

inline void CButton::SetCallback(ButtonCallBack pFoo)
{
	m_pfnCallBack = pFoo;
}

inline eButtonOrientation CButton::GetOrientation()
{
	return m_eOrientation;
}

// this button is a button that has two pieces of texts assoiated with it 
// the first piece is a static text with the name of the button, the second one 
// is the input field
class CInputButton : public CButton
{
public:
	virtual void SetTextObjects( CText* pTextObj, CText* pInputText )
	{
		m_pInputTextObj = pInputText;
		CButton::SetTextObjects(pTextObj, NULL);
	}
	virtual inline const CText* GetInputTextObject() { return m_pInputTextObj;}
	CInputButton(TG_Shape* pAppearance = NULL ) : m_pInputTextObj(NULL), CButton(pAppearance) 
	{
		m_eType = eTwoTextButton;
	}

    virtual void SetTextId( const TCHAR* pTextTopID, const TCHAR* pTextBottomID = NULL );
    virtual void SetText( const TCHAR* pTextTop, const TCHAR* pTextBottom = NULL );
    virtual void Show( bool bDrawThisObject );
	virtual void Dump(bool bAllData = false);
	virtual void SetState(eButtonState);
private:
	CText* m_pInputTextObj;

};

// standard list box class.  The way list box's work is that when you scroll,
// we go through and change the text of each button that's part of the list box.  
// Thus we have no smooth scrolling,
// And we aren't terribly fast.
class CListBox : public SmartObject
{
public:
    
    CListBox();
    virtual ~CListBox();
    // need the buttons so we can hook up them....
    virtual HRESULT LoadFromXBG( File* pFile, TG_Shape* pRoot, CButton** pButtons, unsigned long count  );


    virtual void Show( bool bDrawThisObject );

	HRESULT FrameMove( XBGAMEPAD& GamepadInput, float fElapsedTime );

    HRESULT SetItemText( unsigned long index, const TCHAR* pText, const TCHAR* pHelpText );
    HRESULT RemoveAllItems();
    HRESULT RemoveItem( int index );
    HRESULT RemoveItem( const TCHAR* pText );
    // Kind of wierd but the help text needs to be an ID in the resource file, the text does not
    HRESULT AppendItem( const TCHAR* pText, const TCHAR* pHelpText );
    HRESULT AppendItems( const TCHAR** ppText, const TCHAR** ppHelpText, int count );


private:

    typedef list< TCHAR* > TEXT_LIST;

    TEXT_LIST  m_ItemList;
    TEXT_LIST  m_ItemHelpText;
    vector< CButton* > m_pListButtons;

    long       m_nCurTextIndex;

    TG_Shape*           m_pScrollUpButton;
    TG_Shape*           m_pScrollDownButton;

    // suppress
    CListBox( const CListBox& );
    CListBox& operator=( const CListBox& );

    // Helper
    void UpdateText();

    
};

class CSpinner : public SmartObject
{
public:
    CSpinner();
    virtual ~CSpinner();

    // need the buttons so we can hook up them....
    virtual HRESULT LoadFromXBG( File* pFile, TG_Shape* pRoot, CButton** pButtons, unsigned long count  );
    virtual void Show( bool bDrawThisObject );

    HRESULT FrameMove( XBGAMEPAD& GamepadInput, float fElapsedTime );

    void         SetValue( unsigned int nCurVal );
    unsigned int GetValue( void );
    void         SetRange( unsigned int nStartVal, unsigned int nEndVal );
    void         UpdateRange( unsigned int nStartVal, unsigned int nEndVal );
    void         SetPrecision( unsigned int nPrecision );
    HRESULT IncrementValue( void );
    HRESULT DecrementValue( void );


private:
    unsigned int    m_nStartValue;
    unsigned int    m_nEndValue;
    unsigned int    m_nCurrentValue;
    unsigned int    m_nPrecision;

    CButton*        m_pSpinnerButton;

    unsigned int    m_nScrollUpChangeDelay;
    TG_Shape*       m_pScrollUpButton;
    unsigned int    m_nScrollDownChangeDelay;
    TG_Shape*       m_pScrollDownButton;

    // Helper
    void UpdateText();
};
