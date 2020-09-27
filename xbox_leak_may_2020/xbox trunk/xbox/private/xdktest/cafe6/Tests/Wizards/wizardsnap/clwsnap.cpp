///////////////////////////////////////////////////////////////////////////////
//	clwsnap.CPP
//											 
//	Created by :			Date :				    
//		Ivan				12/11/93
//
//	Description :								 					   
//		Snap test for the ClassWizard  

#include "stdafx.h"
#include "clwsnap.h"				 	  	 
											   		  
#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS
										 
IMPLEMENT_TEST(ClassWizardTestCases, CTest, "ClassWizard snap", -1, CSniffDriver)
UIClassWizard	ClsWiz ;


void ClassWizardTestCases::Run(void)
{

	ClassWizard() ; //Only ClassWizasrd bvt function.
//	DeleteFunctions () ;
	return ;
}


void ClassWizardTestCases::ClassWizard()				 
{ 	
    HWND hwnd ;
    int i, j, k, exitJ, exitK = -99 ;
	projName = "clwtest" ; 
	ClsWiz.SetDataSourceName(m_strCWD+"NWIND.MDB") ;
	proj.DeleteFromProjectDir("clwtest") ; // Cleanup directory
    // Create an AppWiz app
														   
	UIAppWizard apWiz = proj.AppWizard() ;
	apWiz.SetProjType(GetLocString(UIAW_PT_APPWIZ));
	apWiz.SetDir(m_strCWD) ;
	apWiz.SetName(projName) ;
	apWiz.Create();  
	apWiz.NextPage(); // ODBC page		 
	
	// Setup Database options
//#ifdef ODBC
	apWiz.SetDBOptions(UIAW_DB_VIEWFILE) ;
	MST.WButtonClick(apWiz.GetLabel(APPWZ_IDC_DATA_SOURCE)) ; //belongs in apwiz class
	
	UIDataBaseDlgs DBdlgs ;
	DBdlgs.WaitForLogin() ; // dumb wait.

	DBdlgs.SetDataBasePath(m_strCWD+"NWIND.MDB") ;
	DBdlgs.Sources.SetDAOName(DBdlgs.GetDataBasePath()) ;
	DBdlgs.Sources.Close() ;

	DBdlgs.WaitForTables() ;
	DBdlgs.tables.Name(1) ;
	DBdlgs.tables.Close() ;

//#endif //ODBC
	
//#ifdef OLE
	//Going to the next page will cause an AFXMessageBox to appear.
	//The OK button has ID of 1 so NextPage has been overloaded to be able
	//to pass a specific control ID.  If this message box no longer appears
	//or the control that needs to be pressed changes, this test may break.
	//A-RCahn 9-19-1997
  	apWiz.NextPage(1); // To OLE options	SetActiveWindow


	// Setup OLE options.
	apWiz.SetOLEOptions(UIAW_OLE_CONTAINER_SERVER);
	apWiz.SetOLEOptions(UIAW_OLE_YES_AUTO); 

//#endif //_OLE

	apWiz.Finish(1) ;	//This finish will cause a mesage box to appear
						//and the ID of the OK button is 1
						//so Finish has been overloaded so that the control ID
						//can be passed for clicking on.
						//If this message box is no longer invoked by the finish
						//or it changes somehow, this test step can break
						//A-RCahn 9-19-1997
  	apWiz.ConfirmCreate();
	proj.SetPathMembers() ;
	proj.Attach();

	hwnd = MST.WGetActWnd(0) ;
	// Create several dialog resources
	COResScript coRes ;

	for (i = 0; i < 5; i++)
	{
		coRes.CreateResource( IDSS_RT_DIALOG );
	}
	i = 0;

	// FOR TEST SHOULD NOT BE CALLED HERE.
	proj.Build() ;
	proj.WaitUntilBuildDone(5) ;
	// Close all open windows.
	MST.DoKeys("%(W)") ;
	MST.DoKeys("L") ;								 

	//coRes.Save() ;							  
	// Add new classes to the project
	ClsWiz.Display() ;
	AddMemberVars() ;

	for (k = 1; exitK != ERROR_ERROR; k++)
	{
		m_pLog->RecordInfo("k equals %d",k);
		switch(k)
		{
// Skips over classes with known problems.
//these numbers will change every time a new base class is added
//to the dropdown.
		case 3:
			k = 3; //skips adding CAsyncSocket class
		break ;
		case 5:
			k = 5; //skips adding CCachedDataPathProperty class
		break ;
		case 12:
			k = 12; //skips adding CDataPathProperty class
		break ;
		case 26:  //and 27
			k = 27; //skips adding CHttpFilter & CHttpServer classes
		break ;
		case 31:
			k = 31; //skips adding CListView class
		break ;
		case 44:
			k = 44; //skips adding CRecordView class
		break ;
		case 49:
			k = 49; //skips adding CSocket class
		break ;
		case 57:
			k = 57; //skips adding CTreeView class
		break ;
		default:
		exitK = ClsWiz.AddClass(k) ;
		}
	}

	ClsWiz.EditCode() ;
	proj.Build() ;
	proj.WaitUntilBuildDone(5) ;

// Create the *.PCH file 
	SetForegroundWindow(hwnd) ;
	MST.DoKeys("%(W)") ;
	MST.DoKeys("L") ;								 
	// Add all available messages to all classes
	CString Class = "                                                   "; 
	exitK = !ERROR_ERROR ;
	exitJ = !ERROR_ERROR ;
	ClsWiz.Display() ; 
	for (k = 1; k <= ClsWiz.GetClassCount(); k++)
	{	
		switch (k) // May need to skip over classes with known bugs.
		{
			/*case 7:
			case 12:
			case 38:
			break; */
			default:
			for (j = 1; exitJ != ERROR_ERROR; j++) 
			  {
	  			for (i = 1; ClsWiz.AddFunction(k,i,j) != ERROR_ERROR; i++) ;
				exitJ = ClsWiz.AddFunction(k,1,j+1) ;
			  }

			   Class = ClsWiz.GetActiveClass() ;
			   CString CppFile((char )'/0',255) ;
			   CppFile = ClsWiz.GetClassFile() ;		    
					
			   ClsWiz.EditCode() ; // remove after editcode bug fixed.
		//	   ClsWiz.Close() ;
			   MST.DoKeys("%(B)") ;
			   MST.DoKeys("c") ;
			   proj.WaitUntilBuildDone(5) ;
			//close the window 
			   MST.DoKeys("%(F)");
   			   MST.DoKeys("C") ;
 
			   if(!VERIFY_TEST_SUCCESS(proj.VerifyBuild()))
					m_pLog->RecordFailure( Class + ":  with all its messages was built." ) ;
			   else
					m_pLog->RecordInfo(Class + ": successfully built with all its messages was built." ) ;
			   ClsWiz.Display() ;
			   exitJ = !ERROR_ERROR ;
		}
	}
	ClsWiz.Close() ;
	return; 
}

void ClassWizardTestCases::AddMemberVars() 
{
	UIMemVarDlg * VarDlg ;
	ClsWiz.Display() ;
    
	ClsWiz.AddFunction(1,0,0) ; //Select the first class (About class ).
	int ListCount = 0 ;
	int BeginCount, CtrCount ;
	
	CtrCount = BeginCount = ClsWiz.GetControlCount() ;
	for (int Obcnt = 0; Obcnt < CtrCount; Obcnt++) // For each control ID, add members for each of its categories.
	{
		CString CtrIndex ; // We need this to append to the member variable name.
		CtrIndex.Format("%d",Obcnt+1); 

		VarDlg = ClsWiz.AddMemberVar(ListCount +Obcnt+1) ;
		int CatCount = VarDlg->CategoryCount() ;
		MST.DoKeys("{ESC}") ; // Close the dialog; We just wanted to get the category count.
		for (int j = 1; j <= CatCount; j++)
		{
			VarDlg = ClsWiz.AddMemberVar(ListCount+Obcnt+1) ;
			CString CatName = VarDlg->GetCategory(j) ;
			VarDlg->SetVarName(CatName+CtrIndex) ;
			VarDlg->SetCategory(j) ;
			MST.DoKeys("{ENTER}") ;
		}
		ListCount+=ClsWiz.GetControlCount() -BeginCount ;
		BeginCount = ClsWiz.GetControlCount() ;
	}
}

void ClassWizardTestCases::DeleteFunctions()
{ 	
    int i, j, k, exitJ, exitK = -99 ;
	CString CopyName ="CW_Copy" ;
	COProject projCopy ;						 
	projCopy.DeleteFromProjectDir(m_strCWD+CopyName) ; // Cleanup directory
	CreateDirectory(m_strCWD+CopyName, NULL);	

    // Copy the project we added classes/functions to in the previous test to a new directory.
	CopyTree(proj.GetDir(),m_strCWD+CopyName);
	
	// Open the project files.
	if (projCopy.Open(m_strCWD+CopyName+ "\\"+proj.GetName()) != ERROR_SUCCESS) {
		m_pLog->RecordFailure("Unable to open project (%s).", (LPCTSTR)proj.GetName());
		return;
	}
									   
	// Invoke ClassWizard and delete all mapped functions 
	exitK = !ERROR_ERROR ;
	exitJ = !ERROR_ERROR ;
	ClsWiz.Display() ;
	for (k = 1; exitK != ERROR_ERROR; k++)
	{
	  for (j = 1; exitJ != ERROR_ERROR; j++) 
	  {
		int ret ;
	  	for (i = 1; (ret=ClsWiz.DeleteFunction(k,i,j)) != ERROR_ERROR; i++)
		{
			if ( ID_NODELETE == ret)
				m_pLog->RecordFailure( " Above massage handler was not deleted." ) ;
		}
		exitJ = ClsWiz.DeleteFunction(k,1,j+1) ;
	  }
       ClsWiz.EditCode() ;
	   ClsWiz.Close() ;
	   exitJ = !ERROR_ERROR ;
	   exitK = ClsWiz.AddFunction(k+1,1,1) ;
	}
	ClsWiz.Close() ;
	return; 
}

