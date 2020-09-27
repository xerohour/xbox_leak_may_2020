// ButtonDialog.cpp : implementation file
//

#include "stdafx.h"
#include "XODashBuilder.h"
#include "ButtonDialog.h"
#include "ButtonPicker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CButtonDialog dialog

CButtonDialog::CButtonDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CButtonDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CButtonDialog)
	//}}AFX_DATA_INIT

	CurrentScenes = NULL;
	nCurEditButton = 0;
}

void CButtonDialog::getSceneCollection(VSceneCollection *pScenes)
{
	// Get the pointer to the scenelist
	CurrentScenes = pScenes;
}

void CButtonDialog::getSelectedScene(CString selectedScene)
{
	m_pzSelectedScene = selectedScene;
}

void CButtonDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CButtonDialog)
	DDX_Control(pDX, IDC_DELETEBUTTON, m_DeleteButton);
	DDX_Control(pDX, IDC_EDITBUTTON, m_EditButton);
	DDX_Control(pDX, IDC_SAVEBUTTON, m_SaveButton);
	DDX_Control(pDX, IDC_NEWBUTTON, m_NewButton);
	DDX_Control(pDX, IDC_BUTTONLIST, m_ButtonList);
	DDX_Control(pDX, IDC_HELPTEXT, m_ButtonHelp);
	DDX_Control(pDX, IDC_BUTTONLINK, m_ButtonLink);
	DDX_Control(pDX, IDC_BUTTONID, m_ButtonID);
	DDX_Control(pDX, IDC_BUTTONNAME, m_ButtonName);
	DDX_Control(pDX, IDC_SCENENAME, m_SceneName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CButtonDialog, CDialog)
	//{{AFX_MSG_MAP(CButtonDialog)
	ON_WM_SHOWWINDOW()
	ON_LBN_SELCHANGE(IDC_BUTTONLIST, OnSelchangeButtonlist)
	ON_BN_CLICKED(IDC_NEWBUTTON, OnNewbutton)
	ON_BN_CLICKED(IDC_EDITBUTTON, OnEditbutton)
	ON_BN_CLICKED(IDC_SAVEBUTTON, OnSavebutton)
	ON_BN_CLICKED(IDC_DELETEBUTTON, OnDeletebutton)
	ON_BN_CLICKED(IDC_DEBUGBUTTON, OnDebugbutton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CButtonDialog message handlers

void CButtonDialog::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// Fill in Scene name and Button List
	m_SceneName.SetWindowText(m_pzSelectedScene);

    // Get a pointer to the selected scene
    VSceneList::iterator i;
    i = CurrentScenes->m_gVSceneList.begin();
    for(int j = 0; j < CurrentScenes->m_gVSceneList.size(); j++)
    {
		if((*i)->m_pzVSceneName.Compare(m_pzSelectedScene) == 0)
        {
			m_pScene = (*i);
            break;
		}
        i++;
    }
	FillButtonList();
    if( m_ButtonList.GetCount() > 0 )
    {
	    m_ButtonList.SetCurSel(0);
	    OnSelchangeButtonlist();
    }

    // Fields
    m_ButtonName.SetReadOnly(true);
    m_ButtonID.SetReadOnly(true);
	m_ButtonLink.SetReadOnly(true);
    m_ButtonHelp.SetReadOnly(true);

    // List
    m_ButtonList.EnableWindow(true);

    // Buttons
    m_EditButton.EnableWindow(true);
    m_NewButton.EnableWindow(true);
    m_SaveButton.EnableWindow(false);
	m_DeleteButton.EnableWindow(true);

	bButtonChange = false;
	bButtonNew	  = false;
}

void CButtonDialog::FillButtonList(void)
{
    m_ButtonList.ResetContent();

    // Get a pointer to the selected scene
    VButtonSceneList::iterator i;
    for(i = m_pScene->m_VButtonList.begin(); i != m_pScene->m_VButtonList.end(); i++)
    {
		m_ButtonList.AddString((*i)->m_pVButton->m_pVButtonName);
    }

}

void CButtonDialog::OnSelchangeButtonlist() 
{
	// Getting the name of the scene selected
    nCurEditButton = m_ButtonList.GetCurSel();
    CString pzCurrentString;
    m_ButtonList.GetText(nCurEditButton, pzCurrentString);

    // Finding the Scene in the current list of scenes
    VButtonSceneList::iterator i;
    for(i = m_pScene->m_VButtonList.begin(); i != m_pScene->m_VButtonList.end(); i++)
    {
        if((*i)->m_pVButton->m_pVButtonName.Compare(pzCurrentString) == 0)
        {
            // Populate the data fields
			m_ButtonName.SetWindowText((*i)->m_pVButton->m_pVButtonName);
			m_ButtonID.SetWindowText  ((*i)->m_pVButton->m_pVButtonID);
			m_ButtonHelp.SetWindowText((*i)->m_pVButton->m_pVHelpText);
			m_ButtonLink.SetWindowText((*i)->m_pVButtonLink);
	
            break;
        }
    }	
}

void CButtonDialog::OnEditbutton() 
{
    // Fields
    m_ButtonName.SetReadOnly(false);
    m_ButtonID.SetReadOnly(false);
	m_ButtonLink.SetReadOnly(false);
    m_ButtonHelp.SetReadOnly(false);

    // List
    m_ButtonList.EnableWindow(false);

    // Buttons
    m_EditButton.EnableWindow(false);
    m_NewButton.EnableWindow(false);
    m_SaveButton.EnableWindow(true);
	m_DeleteButton.EnableWindow(false);

	bButtonChange = true;
}

void CButtonDialog::OnNewbutton() 
{
    // Fields
    m_ButtonName.SetReadOnly(false);
	m_ButtonName.SetWindowText("");
    m_ButtonID.SetReadOnly(false);
	m_ButtonID.SetWindowText("");
	m_ButtonLink.SetReadOnly(false);
	m_ButtonLink.SetWindowText("");
    m_ButtonHelp.SetReadOnly(false);
	m_ButtonHelp.SetWindowText("");

    // List
    m_ButtonList.EnableWindow(false);
	m_ButtonList.SetCurSel(-1);

    // Buttons
    m_EditButton.EnableWindow(false);
    m_NewButton.EnableWindow(false);
    m_SaveButton.EnableWindow(true);
	m_DeleteButton.EnableWindow(false);

	bButtonNew = true;

    // Load up the button picker
    CButtonPicker *pickerDialog;
    pickerDialog = new CButtonPicker();
    pickerDialog->getSceneCollection(CurrentScenes);
    pickerDialog->DoModal();

    // has the user choosen a value?
    if(pickerDialog->m_returnButton != NULL)
    {
        // find the button in the list
        VButtonList::iterator i;
        for(i = CurrentScenes->m_gVButtonList.begin(); i != CurrentScenes->m_gVButtonList.end(); i++)
        {
            if(strcmp((*i)->m_pVButtonName, pickerDialog->m_returnButton) == 0)
            {
                // Found a match
	            m_ButtonName.SetWindowText((*i)->m_pVButtonName);
	            m_ButtonID.SetWindowText((*i)->m_pVButtonID);
	            m_ButtonLink.SetWindowText("eNullSceneId");
	            m_ButtonHelp.SetWindowText((*i)->m_pVHelpText);
            }
        }	
    }

    delete pickerDialog;
}

void CButtonDialog::OnSavebutton() 
{
    // Get data from fields into an array of strings
    CString textString[4];
	m_ButtonName.GetWindowText(textString[0]);
	m_ButtonID.GetWindowText(textString[1]);
	m_ButtonHelp.GetWindowText(textString[2]);
	m_ButtonLink.GetWindowText(textString[3]);

    // If only editing existing data, plug back into data structure
    if(bButtonChange)
    {
        // Save any edits if any
        CString pzEditedButton;
        m_ButtonList.GetText(nCurEditButton, pzEditedButton);
        VButtonSceneList::iterator j;
        for(j = m_pScene->m_VButtonList.begin(); j != m_pScene->m_VButtonList.end(); j++)
        {
            if((*j)->m_pVButton->m_pVButtonName.Compare(pzEditedButton) == 0)
            {
                (*j)->m_pVButton->m_pVButtonName  = textString[0];
                (*j)->m_pVButton->m_pVButtonID    = textString[1];
                (*j)->m_pVButton->m_pVHelpText    = textString[2];
                (*j)->m_pVButtonLink              = textString[3];
                break;
            }
        }
    }
    // If adding a new button, create the button, and insert it into the list
    else if(bButtonNew)
    {
        // Check to see if the button already exists in the global array
        VButtonList::iterator j;
        bool buttonExists = false;
        for(j = CurrentScenes->m_gVButtonList.begin(); j != CurrentScenes->m_gVButtonList.end(); j++)
        {
            if((*j)->m_pVButtonName.Compare(textString[0]) == 0)
            {
                // Push button onto the scene button array
                VButtonScene *newLinkButton    = new VButtonScene();
                newLinkButton->m_pVButton      = (*j);
                newLinkButton->m_pVButtonLink  = textString[3];
                m_pScene->m_VButtonList.push_back(newLinkButton);

                buttonExists = true;
                break;
            }
        }

        if(!buttonExists)
        {
            // Create a new button in the global list
            VButton *newButton = new VButton();
            newButton->m_pVButtonName = textString[0];
            newButton->m_pVButtonID   = textString[1];
            newButton->m_pVHelpText   = textString[2];

            CurrentScenes->m_gVButtonList.push_back(newButton);

            // Create a button for the Scene
            VButtonScene *newLinkButton = new VButtonScene();
            newLinkButton->m_pVButton     = newButton;
            newLinkButton->m_pVButtonLink = textString[3];

            m_pScene->m_VButtonList.push_back(newLinkButton);
        }
    }

    // Update list based on changes, and move selection to proper item
    FillButtonList();
    nCurEditButton = m_ButtonList.FindString(0, textString[0]);
    m_ButtonList.SetCurSel(nCurEditButton);
    OnSelchangeButtonlist();

    // Fields
    m_ButtonName.SetReadOnly(true);
    m_ButtonID.SetReadOnly(true);
	m_ButtonLink.SetReadOnly(true);
    m_ButtonHelp.SetReadOnly(true);

    // List
    m_ButtonList.EnableWindow(true);

    // Buttons
    m_EditButton.EnableWindow(true);
    m_NewButton.EnableWindow(true);
    m_SaveButton.EnableWindow(false);
	m_DeleteButton.EnableWindow(true);

    bButtonChange  = false;
    bButtonNew     = false;
}

void CButtonDialog::OnDeletebutton() 
{
	// Getting the name of the scene selected
    nCurEditButton = m_ButtonList.GetCurSel();
    CString pzCurrentString;
    m_ButtonList.GetText(nCurEditButton, pzCurrentString);

    // Finding the Button in the current list of Buttons
    VButtonSceneList::iterator i;
    for(i = m_pScene->m_VButtonList.begin(); i != m_pScene->m_VButtonList.end(); i++)
    {
        if((*i)->m_pVButton->m_pVButtonName.Compare(pzCurrentString) == 0)
        {
			// Remove the Button link from the data structure
            if((*i)->m_pVButtonLink.GetLength())
            {
                (*i)->m_pVButtonLink.Empty();
            }
			m_pScene->m_VButtonList.remove(*i);
            break;
        }
    }

	FillButtonList();
    nCurEditButton = 0;
    m_ButtonList.SetCurSel(nCurEditButton);
    OnSelchangeButtonlist();		
}

void CButtonDialog::OnDebugbutton() 
{
    VButtonList::iterator j;
    for(j = CurrentScenes->m_gVButtonList.begin(); j != CurrentScenes->m_gVButtonList.end(); j++)
    {
        // for every scenes button, print out the name to the debugger
        CString outputStr;
        outputStr.Format("%s\n", (*j)->m_pVButtonName);
        OutputDebugString(outputStr);
    }
    OutputDebugString("\n\n");
}
