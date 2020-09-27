// cstm1dlg.cpp : implementation file
//

#include "stdafx.h"
#include "apwiztes.h"
#include "cstm1dlg.h"
#include "apwizaw.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static iCount = 0 ;
/////////////////////////////////////////////////////////////////////////////
// CCustom1Dlg dialog


CCustom1Dlg::CCustom1Dlg()
	: CAppWizStepDlg(CCustom1Dlg::IDD)
{
	//{{AFX_DATA_INIT(CCustom1Dlg)
	m_Iteration = 0;
	//}}AFX_DATA_INIT
}


void CCustom1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CAppWizStepDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCustom1Dlg)
	DDX_Control(pDX, IDC_ITERATION, m_test);
	DDX_Text(pDX, IDC_ITERATION, m_Iteration);
	//}}AFX_DATA_MAP
}

// This is called whenever the user presses Next, Back, or Finish with this step
//  present.  Do all validation & data exchange from the dialog in this function.
BOOL CCustom1Dlg::OnDismiss()
{

	if (!UpdateData(TRUE))
		return FALSE;
    
	// TODO: Set template variables based on the dialog's data.
	switch(m_Iteration)
	{
	 // Dialog app with options turned on, 
	 case 0:
	 {

    	apwiztesaw.m_Dictionary.RemoveKey("PROJTYPE_MDI") ;	
		apwiztesaw.m_Dictionary.RemoveKey("PROJTYPE_SDI") ;	

   		apwiztesaw.m_Dictionary.SetAt("PROJTYPE_DLG","1") ;
		apwiztesaw.m_Dictionary.SetAt("HELP","1") ;
		apwiztesaw.m_Dictionary.SetAt("3D","1") ;
		//		apwiztesaw.m_Dictionary.SetAt("",) ; WinSocket
		apwiztesaw.m_Dictionary.SetAt("ABOUT","1") ;
		apwiztesaw.m_Dictionary.SetAt("VERBOSE","1") ;
		apwiztesaw.m_Dictionary.SetAt("MFCDLL","1") ;
		m_Iteration++ ;
	}
	break;

	// Dialog app with options turned off
		 case 1:
	 {
   		apwiztesaw.m_Dictionary.SetAt("PROJTYPE_DLG","1") ;
		apwiztesaw.m_Dictionary.RemoveKey("HELP") ;
		apwiztesaw.m_Dictionary.RemoveKey("3D") ;
		//		apwiztesaw.m_Dictionary.SetAt("",) ; WinSocket
		apwiztesaw.m_Dictionary.RemoveKey("ABOUT") ;
		apwiztesaw.m_Dictionary.RemoveKey("VERBOSE") ;
		apwiztesaw.m_Dictionary.RemoveKey("MFCDLL") ;
		m_Iteration++ ;
	}
	break;

	// SDI app options
	case 2:
	{
		apwiztesaw.m_Dictionary.RemoveKey("PROJTYPE_DLG") ;	
   		apwiztesaw.m_Dictionary.SetAt("PROJTYPE_SDI","1") ;
		apwiztesaw.m_Dictionary.SetAt("DB","1") ; //Database
		//OLE options	
		apwiztesaw.m_Dictionary.SetAt("FULL_SERVER","1") ;
		apwiztesaw.m_Dictionary.SetAt("AUTOMATION","1") ;
		//App features 
		apwiztesaw.m_Dictionary.SetAt("TOOLBAR","1") ;
		apwiztesaw.m_Dictionary.SetAt("STATUSBAR","1") ;
		apwiztesaw.m_Dictionary.SetAt("PRINT","1") ;
		apwiztesaw.m_Dictionary.SetAt("HELP","1") ;
		apwiztesaw.m_Dictionary.SetAt("3D","1") ;
		apwiztesaw.m_Dictionary.SetAt("TOOLBAR","1") ;
		// WinSockets and MAPI support
		/*apwiztesaw.m_Dictionary.SetAt("WINSOCKETS","1") ;
		apwiztesaw.m_Dictionary.SetAt("MAPI","1") ; */
		//MRU list
		apwiztesaw.m_Dictionary.SetAt("HAS_MRU","1") ;
		apwiztesaw.m_Dictionary.SetAt("SIZE_MRU","0") ;


		apwiztesaw.m_Dictionary.RemoveKey("VERBOSE") ;
		apwiztesaw.m_Dictionary.RemoveKey("MFCDLL") ;
		m_Iteration++ ;
	}
	break;
	//SDI app with  most options turned off.
	case 3:
	{
		apwiztesaw.m_Dictionary.RemoveKey("PROJTYPE_DLG") ;	
   		apwiztesaw.m_Dictionary.SetAt("PROJTYPE_SDI","1") ;
		apwiztesaw.m_Dictionary.RemoveKey("DB") ; //Database
		//OLE options
		apwiztesaw.m_Dictionary.SetAt("CONTAINTER","1") ;
		apwiztesaw.m_Dictionary.SetAt("AUTOMATION","1") ;
		//App features 
		apwiztesaw.m_Dictionary.RemoveKey("TOOLBAR") ;
		apwiztesaw.m_Dictionary.RemoveKey("STATUSBAR") ;
		apwiztesaw.m_Dictionary.RemoveKey("PRINT") ;
		apwiztesaw.m_Dictionary.RemoveKey("HELP") ;
		apwiztesaw.m_Dictionary.RemoveKey("3D");
		// WinSockets and MAPI support
		/*apwiztesaw.m_Dictionary.RemoveKey("WINSOCKETS") ;
		apwiztesaw.m_Dictionary.RemoveKey("MAPI") ; */
		//MRU list
		apwiztesaw.m_Dictionary.RemoveKey("HAS_MRU") ;
		apwiztesaw.m_Dictionary.RemoveKey("SIZE_MRU");

		apwiztesaw.m_Dictionary.SetAt("VERBOSE","1") ;
		apwiztesaw.m_Dictionary.SetAt("MFCDLL","1") ;
		m_Iteration++ ;
	}
	break ;
 	case 4:
	{
	apwiztesaw.m_Dictionary.RemoveKey("PROJTYPE_DLG") ;	
	apwiztesaw.m_Dictionary.RemoveKey("PROJTYPE_SDI") ;
	apwiztesaw.m_Dictionary.SetAt("PROJTYPE_MDI","1") ;
	apwiztesaw.m_Dictionary.RemoveKey("DB") ; //Database

	//OLE options
	apwiztesaw.m_Dictionary.SetAt("MINI_SERVER","1") ;
	apwiztesaw.m_Dictionary.SetAt("AUTOMATION","1") ;
	//App features 
	apwiztesaw.m_Dictionary.RemoveKey("TOOLBAR") ;
	apwiztesaw.m_Dictionary.RemoveKey("STATUSBAR") ;
	apwiztesaw.m_Dictionary.RemoveKey("PRINT") ;
	apwiztesaw.m_Dictionary.RemoveKey("HELP") ;
	apwiztesaw.m_Dictionary.RemoveKey("3D");
	// WinSockets and MAPI support
	/*apwiztesaw.m_Dictionary.RemoveKey("WINSOCKETS") ;
	apwiztesaw.m_Dictionary.RemoveKey("MAPI") ; */
	//MRU list
	apwiztesaw.m_Dictionary.RemoveKey("HAS_MRU") ;
	apwiztesaw.m_Dictionary.RemoveKey("SIZE_MRU");
	apwiztesaw.m_Dictionary.SetAt("VERBOSE","1") ;
	apwiztesaw.m_Dictionary.SetAt("MFCDLL","1") ;
	m_Iteration++ ;
	}
  }
return TRUE;	// return FALSE if the dialog shouldn't be dismissed
}


BEGIN_MESSAGE_MAP(CCustom1Dlg, CAppWizStepDlg)
	//{{AFX_MSG_MAP(CCustom1Dlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCustom1Dlg message handlers
