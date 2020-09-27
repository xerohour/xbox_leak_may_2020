///////////////////////////////////////////////////////////////////////////////
//	MyCASES.CPP
//											 
//	Created by :			Date :				    
//		Ivan				12/11/93
//
//	Description :								 
//		Sample test for COProject APIs		 

#include "stdafx.h"
#include "clwcases.h"

#define VERIFY_TEST_SUCCESS(TestCase)\
	TestCase == ERROR_SUCCESS
												 
IMPLEMENT_GET_NAME(ClassWizardTestCases, "Wizard BVT ", NULL)
IMPLEMENT_GET_NUM_TESTS(ClassWizardTestCases, 1, 1)

ClassWizardTestCases::ClassWizardTestCases()
{
}

int ClassWizardTestCases::Run(void* pData /*=NULL*/)
{
BEGIN_TESTSET();

XSAFETY;
											 
ClassWizard() ; // This is the function which tests out the different APIs

return EndTestSet();	// returns how many failed in this test set
}


BOOL ClassWizardTestCases::ClassWizard(PDESC)				 
{ 	
    
	DESC( "Test Classwizard templates", "ClassWizard" );
    int i, j, k, exitJ, exitK = -99 ;
	COProject proj ;						 
	UIClassWizard	ClsWiz ; 

	proj.DeleteFromProjectDir("clwtest") ; // Cleanup directory
    // Create an AppWiz app
														   
	UIAppWizard apWiz = UIWB.AppWizard() ;		 
	apWiz.SetName("clwtest") ;							    
	apWiz.Create();  
	apWiz.NextPage(); // ODBC page		 
	
	// Setup Database options
	apWiz.SetDBOptions(UIAW_DB_VIEWFILE) ;
	MST.WButtonClick(apWiz.GetLabel(APPWZ_IDC_DATA_SOURCE)) ; //belongs in apwiz class
	
	UIDataBaseDlgs DBdlgs ;
	DBdlgs.WaitForSources() ;
	DBdlgs.Sources.Name("SL") ;
	DBdlgs.Sources.Close() ;

	DBdlgs.WaitForLogin() ;
	DBdlgs.login.ID("sa") ;
	DBdlgs.login.Pw("") ;
	DBdlgs.login.Close() ;
	
	DBdlgs.WaitForTables() ;
	DBdlgs.tables.Name(1) ;
	DBdlgs.tables.Close() ;
  	
  	apWiz.NextPage(); // To OLE options

	// Setup OLE options.
	apWiz.SetOLEOptions(UIAW_OLE_CONTAINER_SERVER);
	apWiz.SetOLEOptions(UIAW_OLE_YES_AUTO);
	apWiz.Finish() ;
  	apWiz.ConfirmCreate();

	// Create several dialog resources
	COResScript coRes ;

	for (i = 0; i < 5; i++)
	{
		coRes.CreateResource( IDSS_RT_DIALOG );
	}
	i = 0;
	//coRes.Save() ;							  
	// Add new classes to the project
	ClsWiz.Display() ;
	for (k = 1; exitK != ERROR_ERROR; k++)
	 	exitK = ClsWiz.AddClass(k) ;
	ClsWiz.EditCode() ;

	// Add all available messages to all classes
	CString Class = "                                                   "; 
	exitK = !ERROR_ERROR ;
	exitJ = !ERROR_ERROR ;
	ClsWiz.Display() ;
	for (k = 1; exitK != ERROR_ERROR; k++)
	{
	  for (j = 1; exitJ != ERROR_ERROR; j++) 
	  {
	  	for (i = 1; ClsWiz.AddFunction(k,i,j) != ERROR_ERROR; i++) ;
		exitJ = ClsWiz.AddFunction(k,1,j+1) ;
	  }

	   Class = ClsWiz.GetActiveClass() ;
	   ClsWiz.Close() ;
	   proj.Build(15) ;
	   WriteLog(VERIFY_TEST_SUCCESS(proj.VerifyBuild()), Class + ":  with all its messages was built." ) ;
	   ClsWiz.Display() ;
	   exitJ = !ERROR_ERROR ;
	   exitK = ClsWiz.AddFunction(k+1,1,1) ;
	}
	ClsWiz.Close() ;
	return TRUE ; 

}


