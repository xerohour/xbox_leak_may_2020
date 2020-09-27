// SceneDialog.cpp : implementation file
//

#include "stdafx.h"
#include "XODashBuilder.h"
#include "SceneDialog.h"
#include "ButtonDialog.h"
#include "CheckedOut.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool CheckReadOnly(char *szFileName)
{
    DWORD nFileAttrib = GetFileAttributes(szFileName);
    if(nFileAttrib & FILE_ATTRIBUTE_READONLY)
        return true;

    return false;
}

bool CheckSceneFiles(void)
{
    bool nResults = CheckReadOnly("..\\xodashlib\\ButtonName.h");
    nResults |= CheckReadOnly("..\\xodashlib\\ButtonId.h");
    nResults |= CheckReadOnly("..\\xodashlib\\ButtonHelp.h");
    nResults |= CheckReadOnly("..\\XODashMain\\NavigationDesc.cpp");
    nResults |= CheckReadOnly("..\\XODashMain\\NavigationMap.cpp");
    nResults |= CheckReadOnly("..\\xodashlib\\SceneName.h");
    nResults |= CheckReadOnly("..\\xodashlib\\SceneId.h");

    if(nResults)
    {
        return true;
    }
    
    return false;
}

/////////////////////////////////////////////////////////////////////////////
// CSceneDialog dialog


CSceneDialog::CSceneDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSceneDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSceneDialog)
	m_SceneIDField = _T("");
	//}}AFX_DATA_INIT
}


void CSceneDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSceneDialog)
	DDX_Control(pDX, IDC_DELETEBUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_BUTTONS, m_ButtonsButton);
	DDX_Control(pDX, IDC_EDITSCENE, m_EditButton);
	DDX_Control(pDX, IDC_NEWSCENE, m_NewSceneButton);
	DDX_Control(pDX, IDC_SAVEBUTTON, m_SaveButton);
	DDX_Control(pDX, IDC_EDIT4, m_SceneName);
	DDX_Control(pDX, IDC_EDIT3, m_FieldSceneID);
	DDX_Control(pDX, IDC_CHECK1, m_ShowOnTop);
	DDX_Control(pDX, IDC_EDIT2, m_TextName);
	DDX_Control(pDX, IDC_EDIT1, m_FilePath);
	DDX_Control(pDX, IDC_LIST1, m_SceneList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSceneDialog, CDialog)
	//{{AFX_MSG_MAP(CSceneDialog)
	ON_WM_SHOWWINDOW()
	ON_LBN_SELCHANGE(IDC_LIST1, OnSelchangeSceneList)
	ON_BN_CLICKED(IDC_EDITSCENE, OnEditscene)
	ON_BN_CLICKED(IDC_NEWSCENE, OnNewscene)
	ON_BN_CLICKED(IDC_SAVEBUTTON, OnSavebutton)
	ON_BN_CLICKED(IDC_BUTTONS, OnButtons)
	ON_BN_CLICKED(IDC_DELETEBUTTON, OnDeletebutton)
	ON_BN_CLICKED(IDC_DEBUGBUTTON, OnDebugbutton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSceneDialog::FillSceneList(void)
{
    m_SceneList.ResetContent();

    // Populate our UI with the initial values
    VSceneList::iterator i;
    i = CurrentScenes.m_gVSceneList.begin();
    for(int j = 0; j < CurrentScenes.m_gVSceneList.size(); j++)
    {
        m_SceneList.AddString((*i)->m_pzVSceneName);
        i++;
    }
}

void CSceneDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);

    FillSceneList();

    m_SceneList.SetCurSel(0);
    OnSelchangeSceneList();

    // Fields
    m_SceneName.SetReadOnly(true);
    m_TextName.SetReadOnly(true);
	m_FilePath.SetReadOnly(true);
    m_FieldSceneID.SetReadOnly(true);
    m_ShowOnTop.EnableWindow(false);

    // List
    m_SceneList.EnableWindow(true);

    // Buttons
    m_EditButton.EnableWindow(true);
    m_NewSceneButton.EnableWindow(true);
    m_SaveButton.EnableWindow(false);
    m_ButtonsButton.EnableWindow(true);
	m_DeleteButton.EnableWindow(true);

    bSceneChange  = false;
    bSceneNew     = false;
}

// Event when the user changes the selection in the scene list
void CSceneDialog::OnSelchangeSceneList() 
{
	// Getting the name of the scene selected
    nCurEditScene = m_SceneList.GetCurSel();
    char* pzCurrentString = new char[255];
    m_SceneList.GetText(nCurEditScene, pzCurrentString);

    // Finding the Scene in the current list of scenes
    VSceneList::iterator i;
    for(i = CurrentScenes.m_gVSceneList.begin(); i != CurrentScenes.m_gVSceneList.end(); i++)
    {
        if(strcmp((*i)->m_pzVSceneName, pzCurrentString) == 0)
        {
            // Populate the data fields
            if(strcmp((*i)->m_pzVSceneShowTop, "true") == 0)
                m_ShowOnTop.SetCheck(1);
            else
                m_ShowOnTop.SetCheck(0);
            
            m_TextName.SetWindowText((*i)->m_pzVSceneTextName);
	        m_FilePath.SetWindowText((*i)->m_pzVScenePath);
            m_FieldSceneID.SetWindowText((*i)->m_pzVSceneID);
            m_SceneName.SetWindowText((*i)->m_pzVSceneName);
            break;
        }
    }

    m_SceneName.SetReadOnly(true);
    m_TextName.SetReadOnly(true);
	m_FilePath.SetReadOnly(true);
    m_FieldSceneID.SetReadOnly(true);
    m_ShowOnTop.EnableWindow(false);
    
    m_SceneList.EnableWindow(true);

    delete [] pzCurrentString;
}

void CSceneDialog::OnEditscene() 
{
	// Text fields
    m_SceneName.SetReadOnly(false);
    m_TextName.SetReadOnly(false);
	m_FilePath.SetReadOnly(false);
    m_FieldSceneID.SetReadOnly(false);
    m_ShowOnTop.EnableWindow(true);

    // Scene list
    m_SceneList.EnableWindow(false);

    // Buttons
    m_EditButton.EnableWindow(false);
    m_NewSceneButton.EnableWindow(false);
    m_SaveButton.EnableWindow(true);
    m_ButtonsButton.EnableWindow(false);
	m_DeleteButton.EnableWindow(false);
    
    bSceneChange  = true;
}

void CSceneDialog::OnNewscene() 
{
	// Text fields
    m_SceneName.SetReadOnly(false);
    m_SceneName.SetWindowText("");
    m_TextName.SetReadOnly(false);
    m_TextName.SetWindowText("");
	m_FilePath.SetReadOnly(false);
    m_FilePath.SetWindowText("");
    m_FieldSceneID.SetReadOnly(false);
    m_FieldSceneID.SetWindowText("");
    m_ShowOnTop.EnableWindow(true);
    m_ShowOnTop.SetCheck(true);

    // Scene list
    m_SceneList.EnableWindow(false);
    m_SceneList.SetCurSel(-1);

    // Buttons
    m_EditButton.EnableWindow(false);
    m_NewSceneButton.EnableWindow(false);
    m_SaveButton.EnableWindow(true);
    m_ButtonsButton.EnableWindow(false);
	m_DeleteButton.EnableWindow(false);
    
    bSceneChange  = false;
    bSceneNew     = true;
}

void CSceneDialog::OnSavebutton() 
{
    // Get data from fields into an array of strings
    CString textString[5];
    if(m_ShowOnTop.GetCheck())
        textString[0] = "true";
    else
        textString[0] = "false";

    m_TextName.GetWindowText(textString[1]);
	m_FilePath.GetWindowText(textString[2]);
    m_FieldSceneID.GetWindowText(textString[3]);
    m_SceneName.GetWindowText(textString[4]);

    // If only editing existing data, plug back into data structure
    if(bSceneChange)
    {
        // Save any edits if any
        char* pzEditedScene = new char[255];
        m_SceneList.GetText(nCurEditScene, pzEditedScene);
        VSceneList::iterator j;
        for(j = CurrentScenes.m_gVSceneList.begin(); j != CurrentScenes.m_gVSceneList.end(); j++)
        {
            if(strcmp((*j)->m_pzVSceneName, pzEditedScene) == 0)
            {
                (*j)->m_pzVSceneShowTop  = textString[0];
                (*j)->m_pzVSceneTextName = textString[1];
                (*j)->m_pzVScenePath     = textString[2];
                (*j)->m_pzVSceneID       = textString[3];
                (*j)->m_pzVSceneName     = textString[4];
                break;
            }
        }
		delete [] pzEditedScene;
    }
    
    // If adding a new scene, create the scene, and insert it into the list
    else if(bSceneNew)
    {
        VScene *newScene = new VScene();
        newScene->m_pzVSceneShowTop  = textString[0];
        newScene->m_pzVSceneTextName = textString[1];
        newScene->m_pzVScenePath     = textString[2];
        newScene->m_pzVSceneID       = textString[3];
        newScene->m_pzVSceneName     = textString[4];

        CurrentScenes.m_gVSceneList.push_back(newScene);
    }

    // Update list based on changes, and move selection to proper item
    FillSceneList();
    nCurEditScene = m_SceneList.FindString(0, textString[4]);
    m_SceneList.SetCurSel(nCurEditScene);
    OnSelchangeSceneList();

	// Text fields
    m_SceneName.SetReadOnly(true);
    m_TextName.SetReadOnly(true);
	m_FilePath.SetReadOnly(true);
    m_FieldSceneID.SetReadOnly(true);
    m_ShowOnTop.EnableWindow(false);

    // Scene list
    m_SceneList.EnableWindow(true);

    // Buttons
    m_EditButton.EnableWindow(true);
    m_NewSceneButton.EnableWindow(true);
    m_SaveButton.EnableWindow(false);
    m_ButtonsButton.EnableWindow(true);
	m_DeleteButton.EnableWindow(true);

    bSceneChange  = false;
    bSceneNew     = false;
}

void CSceneDialog::OnOK() 
{
	if(bSceneChange || bSceneNew)
        OnSavebutton(); 
	
	CDialog::OnOK();

    CurrentScenes.writeButtonNames("..\\xodashlib\\ButtonName.h");
    CurrentScenes.writeButtonIds("..\\xodashlib\\ButtonId.h");
    CurrentScenes.writeButtonHelp("..\\xodashlib\\ButtonHelp.h");
    CurrentScenes.writeNavDesc("..\\XODashMain\\NavigationDesc.cpp");
    CurrentScenes.writeNavMap("..\\XODashMain\\NavigationMap.cpp");
    CurrentScenes.writeSceneNames("..\\xodashlib\\SceneName.h");
    CurrentScenes.writeSceneIds("..\\xodashlib\\SceneId.h");
    CurrentScenes.writeScenesToLoad("..\\media\\scenetoload.csv");
}

void CSceneDialog::OnCancel() 
{
	CDialog::OnCancel();
}

void CSceneDialog::OnButtons() 
{
	// Display the Button Dialog box now that we are initialized with a scene
	CButtonDialog *buttonDlg;
    buttonDlg = new CButtonDialog();
	buttonDlg->getSceneCollection(&CurrentScenes);

	// Getting the name of the scene selected
    nCurEditScene = m_SceneList.GetCurSel();
    CString pzCurrentString;
    m_SceneList.GetText(nCurEditScene, pzCurrentString);
	buttonDlg->getSelectedScene(pzCurrentString);

    buttonDlg->DoModal();

	delete buttonDlg;
}

void CSceneDialog::OnDeletebutton() 
{
	// Getting the name of the scene selected
    nCurEditScene = m_SceneList.GetCurSel();
    CString pzCurrentString;
    m_SceneList.GetText(nCurEditScene, pzCurrentString);

    // Finding the Scene in the current list of scenes
    VSceneList::iterator i;
    for(i = CurrentScenes.m_gVSceneList.begin(); i != CurrentScenes.m_gVSceneList.end(); i++)
    {
        if(strcmp((*i)->m_pzVSceneName, pzCurrentString) == 0)
        {
			// Remove the Scene element from the data structure
			delete (*i);
			CurrentScenes.m_gVSceneList.remove(*i);
            break;
        }
    }

	FillSceneList();
    nCurEditScene = 0;
    m_SceneList.SetCurSel(nCurEditScene);
    OnSelchangeSceneList();
}

BOOL CSceneDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    // Check first to see if the scene and button files are checked out 
    if(CheckSceneFiles())
    {
        // Files are not checked out yet
        ShellExecute(GetSafeHwnd(), "open", "checkout.bat", "", NULL, SW_SHOW );
    }

    // Load up our scenes
    CurrentScenes.initalize();
    bSceneChange  = false;
    bSceneNew     = false;
    nCurEditScene = 0;
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSceneDialog::OnDebugbutton() 
{
    VButtonList::iterator j;
    for(j = CurrentScenes.m_gVButtonList.begin(); j != CurrentScenes.m_gVButtonList.end(); j++)
    {
        // for every scenes button, print out the name to the debugger
        CString outputStr;
        outputStr.Format("%s\n", (*j)->m_pVButtonName);
        OutputDebugString(outputStr);
    }
    OutputDebugString("\n\n");
}
