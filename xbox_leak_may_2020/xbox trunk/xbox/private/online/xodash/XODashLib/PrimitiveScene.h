#pragma once
#include "std.h"
#include "Node.h"
#include "camera.h"
#include "StringObj.h"
#include "XOConst.h"
#include "SmartObject.h"
#include "XBStopWatch.h"
#include "Audio.h"

#include <vector>

using namespace std;

class TG_Shape;
class CText;
class FitIniFile;
class CNetConfig;
struct XBGAMEPAD;



class CPrimitiveScene 
{
	
public:
	// When Initialization is completed, we need to spew screen topology
	// base class will spew it or not based on the bAutoTest flag
	virtual HRESULT Initialize(char* pUrl, char* pButtonTextName,  eSceneId eCurSceneId, bool bAutoTest = true);
	virtual HRESULT Cleanup();
	virtual HRESULT Render( DWORD flags );

    
	virtual HRESULT FrameMove(XBGAMEPAD& GamepadInput, float elapsedTime );

    HRESULT LoadFromXBG( LPDIRECT3DDEVICE8 pd3dDevice, char* strFilename, char* pButtonTextName );
    virtual HRESULT CreateSmartObjects( File* pFile );

	CPrimitiveScene();
    virtual ~CPrimitiveScene();
	virtual void Dump(bool bAllData = false);
	virtual void AutoTest(eAutoTestData);
	void AutoTest(XBGAMEPAD& GamepadInput);
    void AutoTest(eAutoTestData eState, const char* pszPressedKey);

    static void CleanupSounds();

protected: 
	Camera			m_Camera;
	TG_Shape*		m_pRoot;
	D3DLIGHT8		m_Light;

	CStringObj m_szUrlName;
	eSceneId m_eSceneId;

    static CAudioClip*       s_AudioFiles[eLastSoundType][eLastSound];

	vector <CText*>     m_pTexts;
	vector <CButton*>   m_pButtons;
	CListBox*           m_pListBox;
    vector <CSpinner*>  m_pSpinners;
	void*               m_pVideoMem;
    void*               m_pAnimMem;

	CXBStopWatch m_RepeatTimer;
	float m_fRepeatDelay;

	CText* m_pHelpText;

	bool IsAnyButtonActive( const XBGAMEPAD* pGamePad );
	int CountButtons(eButtonOrientation eOrientation);
	int FindButton(eButtonId);
	// find the textobj for given textId
	int FindTextObjForId(const TCHAR* szTextId);
	// find the text object for given NodeName of the TG_Shape
	int FindTextObjForShape(const char* szTextId);
	// find button that includes the Text with given TextId
	int FindButtonForTextObj(const TCHAR* szTextId);
	void SetActiveButton(bool bGoUp);
	void SetActiveButton(eButtonId eButton);
	eInputState GetInputState(XBGAMEPAD& GamepadInput);
	int FindNextActiveButton();
	int FindPrevActiveButton();
	
	virtual void SetFocus();  // set focus on the right button (not necessarily the very first one)  during initialization stage

    static void PlaySound( eSoundType type, eSound whichOne );


	
	bool m_bVerticalButtons;
	int m_nActiveButtonIndex;


};
