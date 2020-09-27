///////////////////////////////////////////////////////////////////////////////
//	og_cases.CPP
//											 
//	Created by :			
//		Anita George - modified version of og_cases.cpp		
//
//	Description :								 
//		Gallery Sniff test	(new file-open like dialog) 

#include "stdafx.h"
#include "og_cases.h"					 	 
											   
#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

// flag to indicate to class wizard whether app wizard was successful or not
extern BOOL gbAppWiz;

UIOGalleryDlg ogDlg;

IMPLEMENT_TEST(CObjGalTestCases, CTest, "Gallery tests", -1, CSniffDriver)

void CObjGalTestCases::Run(void)
{
	// Remove files created by previous test run
	KillAllFiles(OG_PROJNAME1);	
	KillAllFiles(OG_PROJNAME2);	

	CString strIDEDir = GetSubSuite()->GetIDE()->GetPath();
	CString strGalleryDir = ogDlg.GetGalleryDir(strIDEDir);
	CString strOldGal = strGalleryDir + CString("gallery.old");

	if (CreateNewProj())
	{
		if (ogDlg.DeleteFileInGallery(OG_PROJNAME1,OG_OGX_FULLNAME, OG_OGX_NAME))
		{
			if (CreateNewOGX())
			{
				if (ApplyOglet())
				{
					BuildOglet();
				}
			}
		}
	}

	// Restore gallery database
	ogDlg.DeleteOGDatabase(strGalleryDir);
	ogDlg.RenameOGDatabase(strGalleryDir, CString("gallery.old"), ROGD_RESTORE);

}


///////////////////////////////////////////////////////////////////////////////
//	Test Cases

 
COProject proj2;

BOOL CObjGalTestCases::CreateNewProj()
{
	COProject proj1;
	
	UIAppWizard uaw = proj1.AppWizard();
	m_prjwiz.SetProjType(GetLocString(UIAW_PT_APPWIZ));
	if( !uaw.IsValid() )
	{
		m_pLog->RecordFailure("Did not launch App Wizard from File/New/Project");
		EXPECT_EXEC(FALSE, "Can't continue with Gallery sniff");	// no sense continuing
		return FALSE;
	}
	uaw.SetName(OG_PROJNAME1);
	uaw.SetDir(m_strCWD);
	uaw.Create();
	EXPECT( uaw.GetPage() == UIAW_APPTYPE );
	uaw.SetAppType(UIAW_APP_SDI);
	if (uaw.Finish() != UIAW_CONFIRM) 
	{
		m_pLog->Comment("Problem in confirmation dialog");
	}

	BOOL bCreated = uaw.ConfirmCreate();
	if( bCreated == NULL )
	{
		m_pLog->RecordFailure("Problem creating project, or opening it in the IDE");
		return FALSE;
	}
	else
	{
		m_pLog->Comment("Successfully created project.");
		return TRUE;
	}
}

BOOL CObjGalTestCases::CreateNewOGX()
{
	// Bring up Class Wiz and add a class
	UIClassWizard cw;
	cw.Display();

	UIAddClassDlg acd;
	if (acd.Show())
	{
		acd.SetName(OG_NEW_CLASSNAME);
		acd.SetType(OG_CBUTTON);
		acd.Create();
		acd.WaitUntilGone(30000);	// REVIEW: Create takes a long time !
		MST.WButtonClick(GetLabel(IDOK));		  //  Use suggested location

		//verify that ogx doesn't appear in gallery yet
		//successfully found ogx in Gallery but shouldn't be there yet
		if (ogDlg.Display() != NULL)
		{
			if (ogDlg.FindFileInGallery(OG_PROJNAME1, OG_OGX_FULLNAME) == 1) 
			{
				m_pLog->RecordFailure("Ogx in Gallery and shouldn't be");
				ogDlg.Close();
				return FALSE;
			}
			else
			{
				//if cannot find the new class in classview, we error
				if (!(ogDlg.FindClassInClassView(OG_PROJNAME1, OG_NEW_CLASSNAME)))
				{
					m_pLog->RecordFailure("Cannot find class in ClassView");
					return FALSE;
				}
				else
				{
					ogDlg.AddToGallery();
					//if found ogx in gallery
					if (ogDlg.FindFileInGallery(OG_PROJNAME1, OG_OGX_FULLNAME) == 1)
					{
						m_pLog->RecordInfo("Successfully created Custom OGX");
						return TRUE;

					}
					else
					{
						m_pLog->RecordFailure("Cannot find ogx in Gallery.");
						return FALSE;
					}
				}
			}
		}
		else
		{
			m_pLog->RecordFailure("Gallery didn't display properly");
			return FALSE;
		}
	}
	else
	{
		m_pLog->RecordFailure("Couldn't add new class.");
		DoKeys("{ESCAPE 2}");
		return FALSE;
	}
}

UIAppWizard uaw2;

BOOL CObjGalTestCases::ApplyOglet()
{
	BOOL bResult = FALSE;

	// Create target project where OGX will be applied
	uaw2 = proj2.AppWizard();
	m_prjwiz.SetProjType(GetLocString(UIAW_PT_APPWIZ));
	if( !uaw2.IsValid() )
	{
		m_pLog->RecordFailure("Did not launch App Wizard from File/New/Project");
		EXPECT_EXEC(FALSE, "Can't continue with Gallery sniff");	// no sense continuing
	}
	uaw2.SetName(OG_PROJNAME2);
	uaw2.SetDir(m_strCWD);
	uaw2.Create();
	EXPECT( uaw2.GetPage() == UIAW_APPTYPE );
	uaw2.SetAppType(UIAW_APP_SDI);
	if (uaw2.Finish() != UIAW_CONFIRM) 
	{
		m_pLog->Comment("Problem in confirmation dialog");
	}

	BOOL bCreated = uaw2.ConfirmCreate();	// Returns proj window handle
	if( bCreated == NULL )
		m_pLog->RecordFailure("Problem creating target project, or opening it in the IDE");
	else
	{
		ogDlg.AddToProject(OG_PROJNAME1, OG_OGX_NAME, OG_OGX_FULLNAME);	

		// possible test enhancement:
		// verify proper files got added to fileview and resources to resourceview

		if (!(ogDlg.FindClassInClassView(OG_PROJNAME2, OG_NEW_CLASSNAME)))
		{
			m_pLog->RecordFailure("Cannot find class in ClassView");
		}
		else
		{
			m_pLog->RecordInfo("Successfully Applied Custom OGX");
			bResult = TRUE;
		}

	}
	return bResult;
}	  

void CObjGalTestCases::BuildOglet(void)
{
	proj2.Attach();
	proj2.Build(4);
	if (proj2.VerifyBuild() != ERROR_SUCCESS)
	{
		m_pLog->RecordFailure("Build Oglet: Could not build Project with oglet");
		return;
	}
	proj2.Close();
	m_pLog->RecordInfo("Successfully built project with Custom OGX");
	

}

