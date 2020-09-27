// exeaw.cpp : implementation file
//

#include "stdafx.h"
#include "exe.h"
#include "exeaw.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This is called immediately after the custom AppWizard is loaded.  Initialize
//  the state of the custom AppWizard here.
void CXslAppWiz::InitCustomAppWiz()
{
#if 0
	// Create a new dialog chooser; CDialogChooser's constructor initializes
	//  its internal array with pointers to the steps.
	m_pChooser = new CDialogChooser;

	// Add build step to .hpj if there is one
	m_Dictionary[_T("HELP")] = _T("1");

	// Set the maximum number of steps.
	SetNumberOfSteps(LAST_DLG);

	// sets the right language DLL up for us
	m_Dictionary[_T("USE_DEFAULT_LANGUAGE")] = _T("1");
#endif
    SetNumberOfSteps(0);

    m_Dictionary[_T("PROJTYPE_XBOX")] = _T("1");
    m_Dictionary[_T("PROJTYPE_LIB")] = _T("TRUE");

	// TODO: Add any other custom AppWizard-wide initialization here.
}

// This is called just before the custom AppWizard is unloaded.
void CXslAppWiz::ExitCustomAppWiz()
{
#if 0
	// Deallocate memory used for the dialog chooser
	ASSERT(m_pChooser != NULL);
	delete m_pChooser;
	m_pChooser = NULL;
#endif

	// TODO: Add code here to deallocate resources used by the custom AppWizard
}

#if 0
// This is called when the user clicks "Create..." on the New Project dialog
//  or "Next" on one of the custom AppWizard's steps.
CAppWizStepDlg* CXslAppWiz::Next(CAppWizStepDlg* pDlg)
{
	// Delegate to the dialog chooser
	return NULL; //m_pChooser->Next(pDlg);
}

// This is called when the user clicks "Back" on one of the custom
//  AppWizard's steps.
CAppWizStepDlg* CXslAppWiz::Back(CAppWizStepDlg* pDlg)
{
	// Delegate to the dialog chooser
	return NULL; // m_pChooser->Back(pDlg);
}
#endif

void CXslAppWiz::GetPlatforms(CStringList& rPlatforms)
{
    POSITION pos = rPlatforms.GetHeadPosition();
	while(pos)
	{
		POSITION posCur = pos;
		CString str = rPlatforms.GetNext(pos);
		if(str != "Xbox")
			rPlatforms.RemoveAt(posCur);
	}
}

void CXslAppWiz::CustomizeProject(IBuildProject* pProject)
{
	// TODO: Add code here to customize the project.  If you don't wish
	//  to customize project, you may remove this virtual override.

#if 0
    IConfigurations *picfgs;
    if(FAILED(pProject->get_Configurations(&picfgs)))
        picfgs = NULL;
	IConfiguration *picfg;
	long ccfg;
	if(!picfgs || SUCCEEDED(picfgs->get_Count(&ccfg)))
		ccfg = 0;
	for(int icfg = 1; icfg <= ccfg; ++icfg)
	{
		VARIANT v;
		v.vt = VT_I4;
		v.lVal = icfg;
		if(FAILED(picfgs->Item(v, &picfg)))
			picfg = NULL;
		BSTR bstrTool, bstrOpt;
		bstrTool = SysAllocString(L"cl.exe");
		bstrOpt = SysAllocString(L"/WX");
		if(picfg)
		{
			picfg->AddToolSettings(bstrTool, bstrOpt, v);
			picfg->Release();
		}
		SysFreeString(bstrTool);
		SysFreeString(bstrOpt);
	}
	if(picfgs)
		picfgs->Release();
#endif
	
	// This is called immediately after the default Debug and Release
	//  configurations have been created for each platform.  You may customize
	//  existing configurations on this project by using the methods
	//  of IBuildProject and IConfiguration such as AddToolSettings,
	//  RemoveToolSettings, and AddCustomBuildStep. These are documented in
	//  the Developer Studio object model documentation.

	// WARNING!!  IBuildProject and all interfaces you can get from it are OLE
	//  COM interfaces.  You must be careful to release all new interfaces
	//  you acquire.  In accordance with the standard rules of COM, you must
	//  NOT release pProject, unless you explicitly AddRef it, since pProject
	//  is passed as an "in" parameter to this function.  See the documentation
	//  on CCustomAppWiz::CustomizeProject for more information.
	long lNumConfigs;
	CComPtr<IConfigurations> pConfigs;
	pProject->get_Configurations(&pConfigs);
	pConfigs->get_Count(&lNumConfigs);
	for (long j = 1 ; j < lNumConfigs+1 ; j++)
	{
		CComBSTR bszTool;
		CComBSTR bszSwitch;
		CComVariant Varj = j;
		CComVariant VarDummy;
		//Get each individual configuration
		CComPtr<IConfiguration> pConfig;
		pConfigs->Item(Varj, &pConfig);
		
		CComVariant VarDisp = pConfig;
		CComBSTR bszStr;
		pConfig->get_Name(&bszStr);
		// m_strBuildingConfigurationName = bszStr;

#if 0
		bszTool = "cl.exe";
		bszSwitch = "/D \"_AFXDLL\"";
		pConfig->RemoveToolSettings(bszTool,bszSwitch,VarDummy);
		
		bszTool = "link.exe";
		bszSwitch = "kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib";
		pConfig->AddToolSettings(bszTool,bszSwitch,VarDummy);
#endif
		
		bszTool = "MFC";
		bszSwitch = "0";
		pConfig->AddToolSettings(bszTool,bszSwitch,VarDummy);
		
                bszTool = "cl.exe";
                bszSwitch = "/D \"_MBCS\"";
                pConfig->RemoveToolSettings(bszTool,bszSwitch,VarDummy);

		pConfig->MakeCurrentSettingsDefault(VarDummy);
	}
}


// Here we define one instance of the CXslAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global Xslaw.
CXslAppWiz Xslaw;

