// ButtonPicker.cpp : implementation file
//

#include "stdafx.h"
#include "XODashBuilder.h"
#include "ButtonPicker.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CButtonPicker dialog


CButtonPicker::CButtonPicker(CWnd* pParent /*=NULL*/)
	: CDialog(CButtonPicker::IDD, pParent)
{
	//{{AFX_DATA_INIT(CButtonPicker)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

    CurrentScenes = NULL;
}


void CButtonPicker::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CButtonPicker)
	DDX_Control(pDX, IDC_BUTTONLIST, m_ButtonList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CButtonPicker, CDialog)
	//{{AFX_MSG_MAP(CButtonPicker)
	ON_WM_SHOWWINDOW()
	ON_LBN_DBLCLK(IDC_BUTTONLIST, OnDblclkButtonlist)
	ON_BN_CLICKED(IDC_CANCELBUTTON, OnCancelbutton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CButtonPicker::getSceneCollection(VSceneCollection *pScenes)
{
	// Get the pointer to the scenelist
	CurrentScenes = pScenes;
}

/////////////////////////////////////////////////////////////////////////////
// CButtonPicker message handlers

void CButtonPicker::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
    // Populate the list with all of the possible buttons
    VButtonList::iterator i;
    for(i = CurrentScenes->m_gVButtonList.begin(); i != CurrentScenes->m_gVButtonList.end(); i++)
    {
        m_ButtonList.AddString((*i)->m_pVButtonName);
    }	
}

void CButtonPicker::OnDblclkButtonlist() 
{
	// User has double clicked a button to choose it.
    int nCurSelection = m_ButtonList.GetCurSel();

    CString newButton;
    m_ButtonList.GetText(nCurSelection, newButton);
//    m_returnButton = new char[strlen((LPCTSTR)newButton) + 1];
    strcpy(m_returnButton, (LPCTSTR)newButton);

    CDialog::OnOK();
}

BOOL CButtonPicker::DestroyWindow() 
{
    CurrentScenes = NULL;
	
	return CDialog::DestroyWindow();
}

void CButtonPicker::OnCancelbutton() 
{
    CDialog::OnCancel();	
}
