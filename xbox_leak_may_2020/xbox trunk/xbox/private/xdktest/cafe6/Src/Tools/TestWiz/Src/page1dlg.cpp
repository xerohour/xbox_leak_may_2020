// page1dlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestWiz.h"
#include "testawx.h"
#include "page1dlg.h"
#include "tabcusto.h"
#include "apitabdl.h"
#include "scriptr.h"
#include "scriptpr.h"


#ifdef _DEBUG						  
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPage1Dlg dialog

TEXTMETRIC tm ;
int cxChar, cxCaps, cyChar ;

// Pens and Brushes we need to show selections.
CBrush brSelRect ;
CBrush brUnSelRect ;
CPen   penUnSelRect ;
CPen   penSelRect ;
CBitmap bmapUnCheck, bmapCheck, bmapNomoreItems, bmapMoreItems,	bmapNone, bmapTriangle, bmapTestCase, bmapWarn ;							 


CPage1Dlg::CPage1Dlg()
	: CAppWizStepDlg(CPage1Dlg::IDD)
{
	//{{AFX_DATA_INIT(CPage1Dlg)
	m_DriverClass = _T("");
	m_DriverH = _T("");
	m_DriverCpp = _T("");
	m_TestOwner = _T("");
	m_TestSetClass = _T("");
	m_TestSetCpp = _T("");
	m_TestSetH = _T("");
	m_TestTitle = _T("");
	//}}AFX_DATA_INIT
}


void CPage1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CAppWizStepDlg::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPage1Dlg)
	DDX_Control(pDX, IDC_EDITSCRIPT, m_EditScriptButton);
	DDX_Control(pDX, IDC_DRIVER, m_DriverCtrl);
	DDX_Control(pDX, IDC_TESTSET, m_TestSetCtrl);
	DDX_Text(pDX, IDC_DRIVER, m_DriverClass);
	DDX_Text(pDX, IDC_DRIVER_H, m_DriverH);
	DDV_MaxChars(pDX, m_DriverH, 12);
	DDX_Text(pDX, IDC_DRIVERC_PP, m_DriverCpp);
	DDV_MaxChars(pDX, m_DriverCpp, 12);
	DDX_Text(pDX, IDC_OWNER, m_TestOwner);
	DDX_Text(pDX, IDC_TESTSET, m_TestSetClass);
	DDX_Text(pDX, IDC_TESTSET_CPP, m_TestSetCpp);
	DDV_MaxChars(pDX, m_TestSetCpp, 12);
	DDX_Text(pDX, IDC_TESTSET_H, m_TestSetH);
	DDV_MaxChars(pDX, m_TestSetH, 12);
	DDX_Text(pDX, IDC_TITLE, m_TestTitle);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPage1Dlg, CAppWizStepDlg)
	//{{AFX_MSG_MAP(CPage1Dlg)
	ON_BN_CLICKED(IDC_EDITSCRIPT, OnEditscript)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPage1Dlg message handlers

BOOL CPage1Dlg::OnInitDialog() 
{
	CAppWizStepDlg::OnInitDialog();

	// Get project name
	CString strRoot, Rootpart ;

	awx.m_Dictionary.Lookup("Root",strRoot) ;

	// If the name is greater than 4 chars, just use the first 4.
	if(strRoot.GetLength() - 4)
	{
		Rootpart = strRoot.Left(4) ;
	}
	else
		Rootpart = strRoot ;
	// Generate the driver Classname.
	CString strValue ;

	awx.m_Dictionary.SetAt("cases_cpp",Rootpart+"case") ;
	awx.m_Dictionary.SetAt("cases_h",Rootpart+"case");
	awx.m_Dictionary.SetAt("type_cpp",Rootpart+"sub") ;
	awx.m_Dictionary.SetAt("type_h",Rootpart+"sub") ;
	awx.m_Dictionary.SetAt("PROJTYPE_DLL","1") ;

//  Insert the default values in the dialog. 
	awx.m_Dictionary.Lookup("type_h",strValue);     
	m_DriverH = strValue + ".h";     
	
	awx.m_Dictionary.Lookup("type_cpp",strValue);     
	m_DriverCpp  = strValue +".cpp";     

	m_TestOwner = "VCBU QA";      
 
	awx.m_Dictionary.Lookup("cases_cpp",strValue);     
	m_TestSetCpp  = strValue +".cpp";     

	awx.m_Dictionary.Lookup("cases_h",strValue);     
	m_TestSetH  = strValue +".h";     

	m_TestTitle= "TestWizard Generated script"; 
	
	//Uppercase first letter.
	CString ch = "C" ;
	strRoot = ch + strRoot ;
	//ch.MakeUpper() ;
	//strRoot = ch+strRoot.Right(strRoot.GetLength() -1) ;

	m_TestSetClass = strRoot+"IDETest"; 
    m_DriverClass = strRoot+"SubSuite" ;
    
    // Initialize the generated code values to empty so we can create a default empty script.
    awx.m_Dictionary.SetAt("Script_Text","") ; 
    awx.m_Dictionary.SetAt("TestFunctions",""); 
    awx.m_Dictionary.SetAt("ConfDeclarations", "") ; 
    awx.m_Dictionary.SetAt("FuncDeclarations","") ; 

   	CString numCases ;
   	numCases.Format("%d",0);
	awx.m_Dictionary.SetAt("NumCases",numCases) ;
        
   	UpdateData(FALSE) ;

   	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CPage1Dlg::OnDismiss()
{
	UpdateData(TRUE) ;

	if(m_DriverClass.IsEmpty())
	{
		MessageBox("More info is needed on this dialog","Missing Infomation",MB_OK|MB_ICONSTOP) ;
		return FALSE ;
	}

	//All these items need have to have a valid string.
	if(m_DriverH.IsEmpty()||
		m_DriverCpp.IsEmpty()||
		m_TestSetCpp.IsEmpty()||
		m_TestSetH.IsEmpty()||
		m_TestSetClass.IsEmpty()||
		m_DriverClass.IsEmpty())
	{
		MessageBox("All the editcontrols need a valid string.","Missing Infomation",MB_OK|MB_ICONSTOP) ;
		return FALSE ;
	}

	awx.m_Dictionary.SetAt("cases_cpp",m_TestSetCpp.SpanExcluding(".")) ;
	awx.m_Dictionary.SetAt("cases_h",m_TestSetH.SpanExcluding("."));
	awx.m_Dictionary.SetAt("type_cpp",m_DriverCpp.SpanExcluding(".")) ;
	awx.m_Dictionary.SetAt("type_h",m_DriverH.SpanExcluding(".")) ;
	awx.m_Dictionary.SetAt("TestOwner",m_TestOwner) ;
	awx.m_Dictionary.SetAt("TestClass",m_TestSetClass.SpanExcluding(".")) ;
	awx.m_Dictionary.SetAt("TestTitle",m_TestTitle) ;
	awx.m_Dictionary.SetAt("DriverClass",m_DriverClass) ;

	return TRUE ;
}
							 
BOOL CPage1Dlg::InitDrawingObjects() 
{
	static int FirstTime = 1 ;
	// Only do this stuff once.
	
	CClientDC ccldc(this) ;         
    ccldc.GetTextMetrics(&tm);
    cxChar = tm.tmAveCharWidth ;
    cxCaps = (tm.tmPitchAndFamily &1 ? 3 : 2) * cxChar/ 2 ;
    cyChar = tm.tmHeight + tm.tmExternalLeading ;

	// Create the brushes we need.
	// Only do this stuff once.
	if(FirstTime)
	{ 
		brSelRect.CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT)) ;
	 	brUnSelRect.CreateSolidBrush(GetSysColor(COLOR_WINDOW)) ; 
	   	penUnSelRect.CreatePen(PS_SOLID,2,GetSysColor(COLOR_WINDOW)) ;
	   	penSelRect.CreatePen(PS_SOLID,1,GetSysColor(COLOR_WINDOWFRAME)) ; 

		bmapTestCase.LoadBitmap(IDB_MOREITEMS) ; // Specifically for the TestCase tree. 
		bmapUnCheck.LoadBitmap(IDB_UNCHECK) ;
		bmapCheck.LoadBitmap(IDB_MYCHECK);
		bmapNomoreItems.LoadBitmap(IDB_NOMOREITEMS);
		bmapMoreItems.LoadBitmap(IDB_MOREITEMS);
		bmapNone.LoadBitmap(IDB_BLANKBITMAP);
		bmapTriangle.LoadBitmap(IDB_TRIANGLE);
		bmapWarn.LoadBitmap(IDB_WARNING);

		FirstTime = 0 ;
	}

	return TRUE; 
}

void CPage1Dlg::OnEditscript() 
{
	// TODO: Add your control notification handler code here
	TabCustomize CustomizePage ;
	ApiTabdlg	ApiPage ;

	ScriptPropSheet ScriptEditor("TestCase script editor",GetParent()) ;
	
	ScriptEditor.AddPage(&ApiPage) ;
	ScriptEditor.AddPage(&CustomizePage) ;

	OnDismiss(); //Updates the dictionary before bringing up the script dialog.
	
	InitDrawingObjects() ;// Sets up the pens, brushes, and loads the bitmaps.

	if(ScriptEditor.DoModal() == IDOK)
	{	//Disable the class editcontrols, since the user has made some script commitments which they can't change.
		m_TestSetCtrl.EnableWindow(FALSE) ;
		m_DriverCtrl.EnableWindow(FALSE) ;
		m_EditScriptButton.EnableWindow(FALSE) ;

		//Cleanup all the allocated memory.
		CScriptTree::ScriptHead->DeleteChild(ALL) ;
	}

}
