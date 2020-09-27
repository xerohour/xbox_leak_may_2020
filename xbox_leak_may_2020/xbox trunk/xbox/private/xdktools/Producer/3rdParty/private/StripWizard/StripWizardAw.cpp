// StripWizardaw.cpp : implementation file
//

#include "stdafx.h"
#include "StripWizard.h"
#include "StripWizardaw.h"

#ifdef _PSEUDO_DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This is called immediately after the custom AppWizard is loaded.  Initialize
//  the state of the custom AppWizard here.
void CStripWizardAppWiz::InitCustomAppWiz()
{
	// There are no steps in this custom AppWizard.
	SetNumberOfSteps(0);

	// Add build step to .hpj if there is one
	m_Dictionary[_T("HELP")] = _T("1");

	// Inform AppWizard that we're making a DLL.
	m_Dictionary[_T("PROJTYPE_DLL")] = _T("1");

	// TODO: Add any other custom AppWizard-wide initialization here.
}

// This is called just before the custom AppWizard is unloaded.
void CStripWizardAppWiz::ExitCustomAppWiz()
{
	// TODO: Add code here to deallocate resources used by the custom AppWizard
}

// This is called when the user clicks "Create..." on the New Project dialog
CAppWizStepDlg* CStripWizardAppWiz::Next(CAppWizStepDlg* pDlg)
{
	ASSERT(pDlg == NULL);	// By default, this custom AppWizard has no steps

	// Set template macros based on the project name entered by the user.

	// Get value of $$root$$ (already set by AppWizard)
	CString strRoot;
	m_Dictionary.Lookup(_T("root"), strRoot);
	
	// Set value of $$Doc$$, $$DOC$$
	CString strDoc = strRoot.Left(6);
	m_Dictionary[_T("Doc")] = strDoc;
	strDoc.MakeUpper();
	m_Dictionary[_T("DOC")] = strDoc;

	// Set value of $$MAC_TYPE$$
	strRoot = strRoot.Left(4);
	int nLen = strRoot.GetLength();
	if (strRoot.GetLength() < 4)
	{
		CString strPad(_T(' '), 4 - nLen);
		strRoot += strPad;
	}
	strRoot.MakeUpper();
	m_Dictionary[_T("MAC_TYPE")] = strRoot;

	// Generate GUIDs for the different interfaces/tracks/libraries/classes
	GUID guidLibrary, clsidTrack, guidInterface, clsidStripMgr;
	if( SUCCEEDED( CoCreateGuid( &guidLibrary ) ) 
	&&	SUCCEEDED( CoCreateGuid( &clsidTrack ) )
	&&	SUCCEEDED( CoCreateGuid( &guidInterface ) )
	&&	SUCCEEDED( CoCreateGuid( &clsidStripMgr ) ))
	{
		CString strGuid;
		strGuid.Format( _T("%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),		
			guidLibrary.Data1, guidLibrary.Data2, guidLibrary.Data3,
			guidLibrary.Data4[0], guidLibrary.Data4[1], guidLibrary.Data4[2], guidLibrary.Data4[3],
			guidLibrary.Data4[4], guidLibrary.Data4[5], guidLibrary.Data4[6], guidLibrary.Data4[7]);
		m_Dictionary[_T("LIBRARY_GUID")] = strGuid;

		strGuid.Format( _T("0x%lx, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x"),		
			clsidTrack.Data1, clsidTrack.Data2, clsidTrack.Data3,
			clsidTrack.Data4[0], clsidTrack.Data4[1], clsidTrack.Data4[2], clsidTrack.Data4[3],
			clsidTrack.Data4[4], clsidTrack.Data4[5], clsidTrack.Data4[6], clsidTrack.Data4[7]);
		m_Dictionary[_T("TRACK_CLSID")] = strGuid;

		strGuid.Format( _T("%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),		
			guidInterface.Data1, guidInterface.Data2, guidInterface.Data3,
			guidInterface.Data4[0], guidInterface.Data4[1], guidInterface.Data4[2], guidInterface.Data4[3],
			guidInterface.Data4[4], guidInterface.Data4[5], guidInterface.Data4[6], guidInterface.Data4[7]);
		m_Dictionary[_T("INTERFACE_GUID")] = strGuid;

		strGuid.Format( _T("%08lX-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X"),		
			clsidStripMgr.Data1, clsidStripMgr.Data2, clsidStripMgr.Data3,
			clsidStripMgr.Data4[0], clsidStripMgr.Data4[1], clsidStripMgr.Data4[2], clsidStripMgr.Data4[3],
			clsidStripMgr.Data4[4], clsidStripMgr.Data4[5], clsidStripMgr.Data4[6], clsidStripMgr.Data4[7]);
		m_Dictionary[_T("STRIPMGR_CLSID")] = strGuid;
	}

	// Return NULL to indicate there are no more steps.  (In this case, there are
	//  no steps at all.)
	return NULL;
}

void CStripWizardAppWiz::CustomizeProject(IBuildProject* pProject)
{
	// TODO: Add code here to customize the project.  If you don't wish
	//  to customize project, you may remove this virtual override.
	
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

	// Add shared files
	VARIANT varReserved = {0, 0, 0, 0};
	CString strFileName = _T("..\\shared\\BaseMgr.cpp");
	pProject->AddFile( strFileName.AllocSysString(), varReserved );
	strFileName = _T("..\\shared\\BasePropPageManager.cpp");
	pProject->AddFile( strFileName.AllocSysString(), varReserved );
	strFileName = _T("..\\shared\\BaseStrip.cpp");
	pProject->AddFile( strFileName.AllocSysString(), varReserved );
	strFileName = _T("..\\shared\\GroupBitsPPG.cpp");
	pProject->AddFile( strFileName.AllocSysString(), varReserved );
	strFileName = _T("..\\shared\\RiffStrm.cpp");
	pProject->AddFile( strFileName.AllocSysString(), varReserved );
	strFileName = _T("..\\shared\\SelectedRegion.cpp");
	pProject->AddFile( strFileName.AllocSysString(), varReserved );
	strFileName = _T("..\\shared\\StaticPropPageManager.cpp");
	pProject->AddFile( strFileName.AllocSysString(), varReserved );

	IConfigurations *pConfigurations;
	if( SUCCEEDED( pProject->get_Configurations( &pConfigurations ) ) )
	{
		IConfiguration *pIConfiguration;
		VARIANT varIndex;
		varIndex.vt = VT_I4;
		V_I4( &varIndex ) = 1;

		while( SUCCEEDED( pConfigurations->Item( varIndex, &pIConfiguration ) ) )
		{
			// Add linker settings
			strFileName = _T("link.exe");
			CString strSettings = _T("DMUSProdGUID.lib TimelineGUID.lib SegmentDesignerGUID.lib dxguid.lib /libpath:\"..\\lib\"");
			pIConfiguration->AddToolSettings( strFileName.AllocSysString(), strSettings.AllocSysString(), varReserved );

			// Add compiler settings
			strFileName = _T("cl.exe");
			strSettings = _T("/I \"..\\include\" /I \"..\\shared\" /I \".\"");
			pIConfiguration->AddToolSettings( strFileName.AllocSysString(), strSettings.AllocSysString(), varReserved );


			// Get value of $$Safe_root$$ (already set by AppWizard)
			CString strSafeRoot;
			m_Dictionary.Lookup(_T("Safe_root"), strSafeRoot);

			strFileName = strSafeRoot + _T("StripMgr.idl");

			strSettings.Format(_T("/tlb \"%sStripMgr.tlb\" /h \"%sStripMgr.h\""), strSafeRoot, strSafeRoot);

			// Add .idl settings
			pIConfiguration->AddFileSettings( strFileName.AllocSysString(), strSettings.AllocSysString(), varReserved );

			// Add DLL registration step
			CString strDescription = _T("Registering DLL...");
			strFileName = _T("$(OutDir)\\regsvr.tmp");
			strSettings = _T("regsvr32 /s /c \"$(TargetPath)\" > $(OutDir)\\regsvr.tmp");
			pIConfiguration->AddCustomBuildStep( strSettings.AllocSysString(), strFileName.AllocSysString(), strDescription.AllocSysString(), varReserved );

			pIConfiguration->Release();
			V_I4( &varIndex)++;
		}
		pConfigurations->Release();
	}
}


// Here we define one instance of the CStripWizardAppWiz class.  You can access
//  m_Dictionary and any other public members of this class through the
//  global StripWizardaw.
CStripWizardAppWiz StripWizardaw;

