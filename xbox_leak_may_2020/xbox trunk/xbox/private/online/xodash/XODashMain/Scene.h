///////////////////////////////////////////////////////////////////////////////
// Scene.h
// base class for all individual screens.
// This class holds all the geometry, plus lists of interactive objects
//////////////////////////////////////////////////////////////////////////////
#pragma once

#include "std.h"
#include "Node.h"
#include "camera.h"
#include "StringObj.h"
#include "XOConst.h"
#include "SmartObject.h"
#include "XBStopWatch.h"
#include "Audio.h"
#include "PrimitiveScene.h"

#include <vector>

using namespace std;

class TG_Shape;
class CText;
class FitIniFile;

struct XBGAMEPAD;




// this separation is reqired for AseConv to detach SceneNavigator from Scene
class CScene: public CPrimitiveScene
{
public:
	CScene();
	virtual ~CScene();
	HRESULT Initialize(char* pUrl, char* pButtonTextName,  eSceneId eCurSceneId, bool bAutoTest = true);
	HRESULT FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime );
	virtual HRESULT BindData() {return S_OK;}
	virtual void SetFocus();
	static HRESULT RenderGradientBackground( DWORD dwTopColor, 
                                                  DWORD dwBottomColor );
};


typedef vector <CButton*> CharRowVector;
typedef vector <CharRowVector*> Keyboard;


class CKeyboardScene :public CScene 
{
public:
	HRESULT FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime );
	CKeyboardScene();
	HRESULT Initialize(char* pUrl, char* pButtonTextName, eSceneId eCurSceneId, bool bAutoTest = true);
	virtual ~CKeyboardScene();
	HRESULT Cleanup();
	void Dump(bool bAllData = false);
	inline void SetTitle(CText* pText) {ASSERT(pText); m_pTitleText =	pText;}
	void AutoTest(eAutoTestData eState);

private:

	int m_nActiveRow;     // row number of highlighted button - zero based
	int m_nActiveColumn;  // column number of highlighted button - zero based
	int m_nRealPosition;  // real (column) position - required for navigation since rows have different number of columns
	

	bool m_bCapitalActive;  // if Capital characterset is active or not
	bool m_bAccentActive;   // Accents are active
	bool m_bSymbolsActive;  // Symbols are active
	bool m_bShiftActive;    // Shift is active

	CText* m_pInputText;    // accumulate the input here
	CText* m_pTitleText;    // Title for the keyboard - different for different screens
	Keyboard m_pKeyboard;   
	vector<CText*> m_pSymbols; // characters only
	int m_nActiveCharSetIndex; // there are three character sets: letters, symbols, accents
	CAudioClip m_AudioClip;    // play sound based on input

private:
	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();
	HRESULT PressedA();
	void PressedX();
	HRESULT BuildKeyboard();
	void SetCharacterSet();
	void FindControlButton(eButtonId eButton, int* pnRow, int* pnCol);
	static WCHAR  ToLower( WCHAR c );
	static WCHAR  ToUpper( WCHAR c );
	void UpdateInputText(CText* pCurText);
	bool m_bResetInput; 

};

typedef vector <CharRowVector*> Keypad;

class CKeypadScene :public CScene 
{
public:
    CKeypadScene(bool keypadPeriod);
    virtual ~CKeypadScene();

	HRESULT FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime );
	HRESULT Initialize(char* pUrl, char* pTextButtonUrl, eSceneId eCurSceneId, bool bAutoTest = true);
	HRESULT Cleanup();
	void Dump(bool bAllData = false);
	inline void SetTitle(CText* pText) {ASSERT(pText); m_pTitleText =	pText;}
	void AutoTest(eAutoTestData eState);

private:

	int m_nActiveRow;     // row number of highlighted button - zero based
	int m_nActiveColumn;  // column number of highlighted button - zero based
	int m_nRealPosition;  // real (column) position - required for navigation since rows have different number of columns
    bool m_bkeypadPeriod;

	CText* m_pInputText;    // accumulate the input here
	CText* m_pTitleText;    // Title for the keyboard - different for different screens
	Keypad m_pKeypad;   

private:
	void MoveUp();
	void MoveDown();
	void MoveLeft();
	void MoveRight();
	HRESULT PressedA();
	HRESULT BuildKeypad();
	void UpdateInputText(CText* pCurText);
    void SetCharacterSet();
    bool m_bResetInput;
};

class CCellWallScene :public CScene 
{
public:
	CCellWallScene();
	virtual ~CCellWallScene();
	HRESULT Initialize(char* pUrl, char* pButtonTextName,  eSceneId eCurSceneId, bool bAutoTest = true);
	HRESULT FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime );
	HRESULT Render( DWORD flags );
    virtual void AutoTest(eAutoTestData);
    void AutoTest(XBGAMEPAD& GamepadInput);
	HRESULT Cleanup();
    void    HideText(const char *pName);
    void    HideObject(const char *pName);
	void    RenderSelect(bool renderState);
	void    RenderBack(bool renderState);

protected:
	void CellWallRotate(XBGAMEPAD& GamepadInput);
private:
	static void*       m_pVideoMemCellWall;
	static void*       m_pVideoMemSelect;
	static void*       m_pVideoMemBack;

	static TG_Shape*	s_pCellWallSphere;
	static TG_Shape*	s_pSelectButton;
	static TG_Shape*	s_pBackButton;

	static CText*		m_pSelectText;
    static CText*		m_pBackText;
	bool bRenderSelect;
	bool bRenderBack;

	// Waver member variables
	float m_field;
	DWORD m_dwFrameTick;
	double m_now;

};


class CNTS_StatusScene;
class CNTS: public CCellWallScene
{
public:
	CNTS(bool bHide = true): m_bHideContinue(bHide), m_bEditMode(true) {}
	HRESULT BindData();
	HRESULT SetValue(const TCHAR*);
	const TCHAR* GetValue(eButtonId);
	HRESULT FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime );

private:
	void SetButtonsState(const TCHAR* szValue);	
	bool m_bEditMode;
	bool m_bHideContinue;
};

class CNTS_ConfigChoice: public CCellWallScene
{
public:
	HRESULT Initialize(char* pUrl, char* pButtonTextName,  eSceneId eCurSceneId, bool bAutoTest = true);
	HRESULT FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime );
};

class CNTS_SettingsScene :public CCellWallScene
{
public:
	HRESULT FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime );
};

class CNTS_StatusScene :public CNTS_SettingsScene
{
public:
	HRESULT Initialize(char* pUrl, char* pButtonTextName,  eSceneId eCurSceneId, bool bAutoTest = true);
	HRESULT FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime );
private:
	void SetIcon(int nIndex, long eStatus, bool bDHCP=true, bool bHideText = true);
	void UpdateScene();

	long m_nNetworkStatus;
	bool m_bFinalState;
};

class CNTSHelpScene: public CCellWallScene
{
public:
	HRESULT Initialize(char* pUrl, char* pButtonTextName,  eSceneId eCurSceneId, bool bAutoTest = true);
};

// online main menu scene, probalby poorly named
class CACT_OnlineScene :public CCellWallScene
{
public:
	HRESULT FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime );
  	virtual HRESULT Initialize(char* pUrl, char* pButtonTextName,  eSceneId eCurSceneId, bool bAutoTest = true);
    static HRESULT AttemptMachineLogon();

private:

    static HRESULT m_hrLogon;


};

// base class for all scene's that use account data
class CACT_Scene :public CCellWallScene
{
public:
  	virtual HRESULT Initialize(char* pUrl, char* pButtonTextName,  eSceneId eCurSceneId, bool bAutoTest = true);
	HRESULT FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime );
    
    void UpdateButtons();
    HRESULT Validate( eButtonId id, const TCHAR* szValue );
    HRESULT SetValue(const TCHAR* szValue);


};

class CACT_SignupAccount :public CACT_Scene
{
public:
    CACT_SignupAccount();
    virtual ~CACT_SignupAccount();

    virtual HRESULT Initialize(char* pUrl, char* pButtonTextUrl,  eSceneId eCurSceneId, bool bAutoTest);
	HRESULT FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime );

private:
    static bool  m_bDisplayParentMessage;
    static int   m_nSignupAge;
};
