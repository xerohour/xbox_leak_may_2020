///////////////////////////////////////////////////////////////////////////////
//	CTRCASES.CPP
//
//	Created by :			Date :
//		ivanl					4/8/95
//
//	Description :
//		Implementation of the CCtrlWizardCases class
//

#include "stdafx.h"
#include "ctrsnap.h"
#include "resource.h"

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS

extern char* szRes ;
extern HWND hSubWnd ;
CString LangDllName ;
CString msg ;

IMPLEMENT_TEST(CCtrlWizardCases, CTest, "ControlWizard snap", -1, CSniffDriver)

void CCtrlWizardCases::Run(void)
{
	iteration = 1 ;
	LangDlls = 0 ;
	m_pLog->Comment("in ctrsnap.cpp");

  // Flush the strings first
	LangDllName.Empty() ;
	CurrDir.Empty() ;
	msg.Empty() ;
	ProjDir.Empty() ;
   	LangDllName =" " ;            

	szDir.LoadString(IDS_CTRWIZDIR) ; 
	ProjName.LoadString(IDS_CTRWIZNAME) ;
   	//ProjDir = szDir ;  
	ProjDir = ProjName;
    prj.DeleteFromProjectDir(ProjDir) ;
	NewProject() ;
	SetProjOptions() ;
	SetCtrlOptions() ;
	GenerateProject() ;
	m_pLog->Comment("add methods");
	AddControlMethods() ;
	m_pLog->Comment("add events");
	AddControlEvents() ;
	m_pLog->Comment("add properties");
	AddControlProperty() ;

}

///////////////////////////////////////////////////////////////////////////////
//	Test Cases

// Create extension project
void CCtrlWizardCases::NewProject(void)
{

// REVIEW(briancr): we can't support AppWizard at the UWBFrame level--it's in COProject
//	m_ctrlwiz = UIWB.AppWizard();
	m_ctrlwiz = prj.AppWizard(TRUE);
	if( !m_ctrlwiz.IsValid() )
	{
		m_pLog->RecordFailure("Did not launch Control AppWizard from File/New/Project");
		EXPECT_EXEC(FALSE, "Need Custom Appwizar to test anymore");	// no sense continuing
	}
	m_ctrlwiz.SetDir(m_strCWD) ;
	m_ctrlwiz.SetName(ProjName);
	m_ctrlwiz.SetSubDir(szDir);
	Sleep(5000);
	m_ctrlwiz.Create();				

	prj.Attach();
}

void CCtrlWizardCases::SetProjOptions()
{
	m_ctrlwiz.SetCount(1) ;
	m_ctrlwiz.SetLicenceOpt(1);
   	m_ctrlwiz.NextPage();
}

//Select the Custom Option
void CCtrlWizardCases::SetCtrlOptions()
{
	CString Name = "Control_" ;

        for(int i = 1; i < 2; i++)
	{
		CString cnt;
		cnt.Format("%d",i) ;
		m_ctrlwiz.SetActive(i) ;
        m_ctrlwiz.SubClassCtrl(2) ; //button
		m_ctrlwiz.SummaryInfo() ;

		m_ctrlwiz.SumDlg.SetShortName(Name+cnt);
		m_ctrlwiz.SumDlg.Close() ;
	}
}

void CCtrlWizardCases::GenerateProject()
{	
	m_ctrlwiz.NextPage() ;
	if(!m_ctrlwiz.ConfirmCreate())
	{
 		m_pLog->RecordFailure(" Problem creating a control project, or opening it in the IDE");
	}
}

void CCtrlWizardCases::AddControlProperty() 
{
	UIClassWizard	ClsWiz ;
	UIAddProperty * PropDlg = ClsWiz.AddOleProperty() ;

// Loop through and add all the stock properties.
	int StockCnt = PropDlg->GetStockCount() ;
	for(int i = 1 ;i < StockCnt ;i++ )
	{
		PropDlg->SetExtName(i) ;
		PropDlg->SetImplementation(STOCK) ;
		PropDlg->Create() ;
		PropDlg = ClsWiz.AddOleProperty() ;
	}
	MST.DoKeys("{ESC}"); // Close propert dialog.

	// Loop through and create properties for each of the return types. 
	PropDlg = ClsWiz.AddOleProperty() ;
	int TypesCnt = PropDlg->GetRetTypeCount() ;
	
	for(i = 1 ;i <= TypesCnt ;i++ )
	{   
		CString count ;
		count.Format("%d",i);
		PropDlg->SetImplementation(MEMBER_VAR) ;
		PropDlg->SetExtName(PropDlg->SetReturnType(i)+"MemVar" +count);
		PropDlg->Create() ;
		PropDlg = ClsWiz.AddOleProperty() ;
	}
	MST.DoKeys("{ESC}"); // Close propert dialog.

	// Use Get/Set implemantation.
		// Loop through and create properties for each of the return types. 
	PropDlg = ClsWiz.AddOleProperty() ;
	m_pLog->RecordInfo("getting return type count");
	TypesCnt = PropDlg->GetRetTypeCount() ;
	m_pLog->RecordInfo("type count is %d",TypesCnt);
	m_pLog->RecordInfo("setting implementation");
	PropDlg->SetImplementation(GET_SET_MEHOD) ;
	//ParamTypeCount doesn't work so hard-coding parameter count
	//anita george - 10/2/97
	//int ParamCnt = PropDlg->ParamTypeCount() ;
	int ParamCnt = 50;

	int FormalPrm = (ParamCnt / TypesCnt) + 1;
	
// Work around bug # 9930.
    //DoKeys("{ENTER}") ;
	m_pLog->RecordInfo("setting external name");
	PropDlg->SetExtName(PropDlg->SetReturnType(1)+"GetSetTest");
	m_pLog->RecordInfo("creating");
	PropDlg->Create() ;

	for(i = 1 ;i <= TypesCnt ;i++ )
	{  

		m_pLog->RecordInfo("in for loop");
		m_pLog->RecordInfo("i is %d",i);
		m_pLog->RecordInfo("AddOleProperty");
		PropDlg = ClsWiz.AddOleProperty() ;
		m_pLog->RecordInfo("setting implementation");
		PropDlg->SetImplementation(GET_SET_MEHOD) ;
		m_pLog->RecordInfo("SetExtName");
		PropDlg->SetExtName(PropDlg->SetReturnType(i)+"GetSetProp");
		for(int j= 0; j<= FormalPrm; j++ ) 
		{
			m_pLog->RecordInfo("j is %d",j);
			m_pLog->RecordInfo("FormalPrm is %d",FormalPrm);
			m_pLog->RecordInfo("ParamCnt is %d",ParamCnt);

			// Provides a value in range of the available param types. Base = 1.
			int item = (((i-1)* FormalPrm)+j) % (ParamCnt +1) ;
			m_pLog->RecordInfo("item is %d",item);
			if (item == 1)
				item = FormalPrm+1 ; // Don't need the first one since we create one by default.
			m_pLog->RecordInfo("item is %d",item);
			m_pLog->RecordInfo("AddParameter");
			PropDlg->AddParameter(item, "NULL");
		} 
		PropDlg->Create() ;
	}
	ClsWiz.Close() ;
    prj.Build(15) ;
    if (!VERIFY_TEST_SUCCESS(prj.VerifyBuild())) {
          m_pLog->RecordFailure("Building with all Property types and implementations");
     }

}	



void CCtrlWizardCases::AddControlMethods() 
{
	UIClassWizard	ClsWiz ;
	UIAddMethod * MethDlg = ClsWiz.AddOleMethod() ;

	//This will create the DoClick method that is one of the
	//defaults
	m_pLog->Comment("adding DoClick");
	MethDlg->SetExtName(1) ;
	MethDlg->SetImplementation(STOCK) ;
	MethDlg->Create() ;
 
	//This will create the Refresh method that is one of the
	//defaults
	MethDlg = ClsWiz.AddOleMethod();//This opens up the Add Method dialog
	m_pLog->Comment("adding Refresh");
	MethDlg->SetExtName(2) ;
	MethDlg->SetImplementation(STOCK) ;
	MethDlg->Create() ;
 

	MethDlg = ClsWiz.AddOleMethod();//This opens up the Add Method dialog
	int TypesCnt = MethDlg->GetRetTypeCount(); //Will type TEST into external name
											   //so we can get all the return types
	m_pLog->RecordInfo("typescnt is %d", TypesCnt);
	int ParamCnt = MethDlg->ParamTypeCount();	//Get the number of parameter types available
	m_pLog->RecordInfo("paramcnt is %d", ParamCnt);
	int FormalPrm = (ParamCnt / TypesCnt) + 1;	//Will be used below to create "random" param types
	m_pLog->RecordInfo("formalprm is %d", FormalPrm);
	
// Work around bug # 9930.  //Not a VC98 bug number.  A-RCahn 9-19-97
    //DoKeys("{ENTER}") ;
	//MethDlg->SetExtName(MethDlg->SetReturnType(1)+"TestMethod");
	//MethDlg->Create() ;
	CString methodName;
	// Loop through and create methods for each of the return types. 
	for(int i = 1 ;i <= TypesCnt ;i++ )
	{
		MethDlg = ClsWiz.AddOleMethod();//Open the addMethod dialog
										//AddOleMethod will delete old MethDlg
		methodName = MethDlg->SetReturnType(i); //Select the next return type
		methodName += "Method"; //Make a method name name called "<returntype>Method"
		MethDlg->SetExtName(methodName); //Set the external name to "<returntype>Method"
		//WARNING:  Major Hack Alert
		//Because of problems with the Class Wizard's Add Member Dialog
		//We tab three times here to get to the ParameterList prior to 
		//calling AddParameter below.
		//Obviously this hard coding of tabbing is bad and will eventually
		//cause this test to break again.  A-RCahn 9-19-97
		DoKeys("{TAB}");
		DoKeys("{TAB}");
		DoKeys("{TAB}");
		for(int j= 1; j<= FormalPrm; j++ )
		{
			//WARNING:  Because of problems with the Class Wizard's Add Member Dialog
			//The code below has been commented out because the &Parameter list: string
			//is missing from the Parameter listbox.  A-RCahn 9-19-97
			// Provides a value in range of the available param types. Base = 1.
			//int item = (((i-1)* FormalPrm)+j) % (ParamCnt +1) ;
			//if (item == 1)
			//	item = FormalPrm+1 ; // Don't need the first one since we create one by default.
			MethDlg->AddParameter(j, "NULL");
		}

		MethDlg->Create() ; //Hit OK to create the new method
		methodName.Empty(); //Flush the old string for reuse above.

	} //loop to next return type
	ClsWiz.Close() ;//We're done creating methods
	//Now let's build to test for success
    m_pLog->RecordInfo("Building with all method types");
	prj.Build(15) ;
    if (!VERIFY_TEST_SUCCESS(prj.VerifyBuild()))
	{
          m_pLog->RecordFailure("Failure occured building new methods");
    }

}	

void CCtrlWizardCases::AddControlEvents() 
{
	UIClassWizard	ClsWiz ;
	m_pLog->RecordInfo("ready to do AddOleEvent");
	UIAddEvent * EventDlg = ClsWiz.AddOleEvent() ;
	int StockCnt = EventDlg->GetStockCount() ;
	// Create 2 stock events.
	m_pLog->RecordInfo("stock count is %d",StockCnt);
	for(int i = 1; i<= StockCnt; i++)
	{
		m_pLog->RecordInfo("i is %d",i);
		EventDlg->SetExtName(i) ;
		EventDlg->SetImplementation(STOCK) ;
		EventDlg->Create() ;
		EventDlg = ClsWiz.AddOleEvent() ;
	}
	//m_pLog->RecordInfo("done in for loop");
	MST.DoKeys("{ESC}"); // Cancel out of the methods dialog the last time.

	//when we continue the test from here, cafe eventually crashes while creating the
	//event called TestEvent.  I don't have time to investigate now, but
	//I'm leaving my print statements in here for anyone who plans on fixing this.
	//anita george - 9/30/97.
	
	//m_pLog->RecordInfo("Loop through and create Events for each of the return types.");
	// Loop through and create Events for each of the return types. 
	/*EventDlg = ClsWiz.AddOleEvent() ;
	int ParamCnt = EventDlg->ParamTypeCount() ;
	int FormalPrm = 2 ;
	
// Work around bug # 9930.
    DoKeys("{ENTER}") ;
	m_pLog->RecordInfo("ready to setextname to testevent");
	EventDlg->SetExtName("TestEvent") ;
	m_pLog->RecordInfo("create");
	EventDlg->Create() ;
	m_pLog->RecordInfo("ParamCnt is %s",ParamCnt);
	for(i = 1 ;i <= (ParamCnt/2)+1 ;i++ )
	{
		m_pLog->RecordInfo("i is %d",i);
		EventDlg = ClsWiz.AddOleEvent() ;
		CString count ;
		count.Format("%d",i);
		CString EvName = "Event" ;
		EventDlg->SetExtName(EvName+count);
		m_pLog->RecordInfo("FormalPrm is %s",FormalPrm);
		for(int j= 0; j<= FormalPrm; j++ )
		{
			m_pLog->RecordInfo("j is %d",j);
			// Provides a value in range of the available param types. Base = 1.
		//	int item = i +j % (ParamCnt +1) ;
			int item = (((i-1)* FormalPrm)+j) % (ParamCnt +1) ;
			m_pLog->RecordInfo("item is %d",item);
			if (item == 1)
				item = FormalPrm+1 ; // Don't need the first one since we create one by default.
			EventDlg->AddParameter(item, "NULL");
		}
		EventDlg->Create() ;
	}	*/
	ClsWiz.Close() ;
	prj.Build(15) ;
    if (!VERIFY_TEST_SUCCESS(prj.VerifyBuild())) {
          m_pLog->RecordFailure("Building with all event types");
    }

}	

