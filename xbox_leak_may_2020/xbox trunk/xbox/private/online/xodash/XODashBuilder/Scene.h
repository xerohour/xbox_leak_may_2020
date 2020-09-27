#pragma once
#include "stdafx.h"
#include <list>

using namespace std;

class VButton
{
public:
    VButton();
    ~VButton();

    CString m_pVButtonName;
    CString m_pVButtonID;
    CString m_pVHelpText;
};

class VButtonScene
{
public:
    VButtonScene();
    ~VButtonScene();

    VButton *m_pVButton;

    CString m_pVButtonLink;
};

typedef list<VButtonScene*> VButtonSceneList;
typedef list<VButton*> VButtonList;

class VScene
{
public:
    VScene();
    ~VScene();

    // General Scene Information
    CString   m_pzVSceneName;
    CString   m_pzVSceneID;
    
    CString   m_pzVScenePath;
    CString   m_pzVSceneTextName;
    CString   m_pzVSceneShowTop;
    CString   m_pzVButtonFocus;
   
    VButtonSceneList	m_VButtonList;
};

typedef list<VScene*> VSceneList;
typedef list<CString> VStringList;

class VSceneCollection
{
public:
    VSceneCollection();
    ~VSceneCollection();

    HRESULT initalize(void);

    VSceneList	m_gVSceneList;
    VButtonList	m_gVButtonList;

    HRESULT writeButtonNames(char *fileName);
    HRESULT writeButtonIds(char *fileName);
    HRESULT writeButtonHelp(char *fileName);
    HRESULT writeNavDesc(char *fileName);
    HRESULT writeNavMap(char *fileName);
    HRESULT writeSceneNames(char *fileName);
    HRESULT writeSceneIds(char *fileName);
    HRESULT writeScenesToLoad(char *fileName);

private:
    HRESULT initalizeButtons(void);
    HRESULT initalizeScenes(void);
    HRESULT readNavMap(void);
    HRESULT readNavDesc(void);
};
