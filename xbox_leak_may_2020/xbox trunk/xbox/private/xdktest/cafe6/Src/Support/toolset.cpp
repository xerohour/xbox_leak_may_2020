/////////////////////////////////////////////////////////////////////////////
// toolset.cpp
//
// email	date		change
// briancr	11/04/94	created
//
// copyright 1994 Microsoft

// Implementation of the CToolset class

#include "stdafx.h"
#include "afxdllx.h"
//#include "resource.h"
#include "settings.h"
#include "envpage.h"
#include "target.h"
#include "toolset.h"
#include "guiv1.h"
#include "idetarg.h"
#include "bldtools.h"

#define new DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CToolset
/*
CString CToolset::m_strToolsets[] = { "Visual C++ for x86",
										"Visual C++ for Macintosh",
										"Visual C++ for PowerMac",
										"Visual C++ for MIPS",
										"Visual C++ for Alpha",
										"Visual C++ for Win32s",
										"Visual C++ for PowerPC",
										"" };

CString CToolset::m_strToolsetSwitches[] = { "win32_x86",
											"mac_68k",
											"mac_ppc",
											"win32_mips",
											"win32_alpha",
											"win32s_x86",
											"win32_ppc",
											"" };

CString CToolset::m_strLanguage[] = { "English",
										"Japanese",
										"" };

*/

CString CToolset::m_strBuildType[] = { "Debug",
										"Retail",
										"" };

CString CToolset::m_strEnvvars[] = { "Path",
									"Include",
									"Lib",
									"" };


CToolset::CToolset(BUILD_TYPE nBuildType, HANDLE hOutput)
: m_strLang("\\English"),
  m_strDebug("\\" + m_strBuildType[m_nBuildType = nBuildType]),
  m_hOutput(hOutput),
  m_pLog(NULL),
  m_lang(TL_ENGLISH)
{
}

CToolset::~CToolset()
{
	// delete the settings object
	if (m_psettingsEnv) {
		delete m_psettingsEnv;
	}
	if (m_psettingsToolset) {
		delete m_psettingsToolset;
	}

	FreeLibrary(m_hStringsDll);
}

BOOL CToolset::ToolsetOptions(void)
{
	CTarget* pTool;
	POSITION pos;
	int i;

	// display the options dialog

	// create the tabbed options dialog
	CPropertySheet psToolset(IDS_ToolsetOptions);

	// create the standard toolset pages
	// create the environment page
	CEnvPage pageEnv(m_psettingsEnv);

	// add each page to the sheet
	psToolset.AddPage(&pageEnv);
	for (pos = m_listTools.GetHeadPosition(); pos != NULL; ) {
		pTool = m_listTools.GetNext(pos);
		psToolset.AddPage(pTool->GetPage());
	}

	// bring up the tabbed dialog
	psToolset.DoModal();

	// remove the tool pages from the sheet
	for (pos = m_listTools.GetHeadPosition(); pos != NULL; ) {
		pTool = m_listTools.GetNext(pos);
		psToolset.RemovePage(pTool->GetPage());
	}

	// set the environment settings in each target
	for (pos = m_listTools.GetHeadPosition(); pos != NULL; ) {
		pTool = m_listTools.GetNext(pos);
		for (i = 0; !m_strEnvvars[i].IsEmpty(); i++) {
			pTool->SetEnvironmentVar(m_strEnvvars[i], m_psettingsEnv->GetTextValue(m_strEnvvars[i]));
		}
	}
	return TRUE;
}

BOOL CToolset::AddTool(CTarget *pTool)
{
	// the target and the page cannot be NULL
	ASSERT(pTool);
	if (!pTool) {
		return FALSE;
	}

	// add it to the list of tools
	m_listTools.AddTail(pTool);

	return TRUE;
}

BOOL CToolset::ApplySettings(CSettings* pSettings)
{
	CTarget* pTool;
	POSITION pos;
	BOOL bResult = TRUE;

	// the settings object must be valid
	ASSERT(pSettings);

	// is the language defined?
	if (pSettings->IsDefined(settingLanguage)) {
		// if so, adjust the registry key we use
		m_psettingsToolset->SetTextValue(settingLanguage, pSettings->GetTextValue(settingLanguage));
		m_psettingsToolset->SetTextValue(settingLanguageKey, "\\" + m_psettingsToolset->GetTextValue(settingLanguage));
	}

	// set the key to use and initialize the settings for that key
	m_psettingsEnv->SetKey(m_strBaseKey + m_psettingsToolset->GetTextValue(settingLanguageKey) + m_psettingsToolset->GetTextValue(settingDebugBuildKey));
	InitializeEnvSettings();

	// apply these settings to all the targets
	// apply the cmd line to all targets
	for (pos = m_listTools.GetHeadPosition(); pos != NULL; ) {
		pTool = m_listTools.GetNext(pos);
		bResult &= pTool->ApplySettings(pSettings);
	}

	return bResult;
}

BOOL CToolset::InitializeSettings(void)
{
	return InitializeEnvSettings() & InitializeToolsetSettings();
}

BOOL CToolset::InitializeEnvSettings(void)
{
	int i;
	CString strEnvvar;
	BOOL bResult = TRUE;
	CTarget *pTool;

	// the environment settings object must be valid
	ASSERT(m_psettingsEnv);
	if (!m_psettingsEnv) {
		return FALSE;
	}

	// initialize environment settings
	for (i = 0; !m_strEnvvars[i].IsEmpty(); i++) {
		bResult &= (GetEnvironmentVariable(m_strEnvvars[i], strEnvvar.GetBuffer(1024), 1023) != 0);
		strEnvvar.ReleaseBuffer();
		if (bResult) {	
			m_psettingsEnv->SetTextValue(m_strEnvvars[i], strEnvvar);
		}
	}

	// load settings from the registry
	bResult &= m_psettingsEnv->ReadRegistry();
	// write the settings back out to initialize registry
	bResult &= m_psettingsEnv->WriteRegistry();

	// set the environment settings in each target
	for (POSITION pos = m_listTools.GetHeadPosition(); pos != NULL; ) {
		pTool = m_listTools.GetNext(pos);
		for (i = 0; !m_strEnvvars[i].IsEmpty(); i++) {
			pTool->SetEnvironmentVar(m_strEnvvars[i], m_psettingsEnv->GetTextValue(m_strEnvvars[i]));
		}
	}

	return bResult;
}

BOOL CToolset::InitializeToolsetSettings(void)
{
	BOOL bResult = TRUE;

	// the toolset settings object must be valid
	ASSERT(m_psettingsToolset);
	if (!m_psettingsToolset) {
		return FALSE;
	}

	// intialize toolset settings
	bResult &= m_psettingsToolset->SetTextValue(settingLanguage, "English");
	bResult &= m_psettingsToolset->SetTextValue(settingDebugBuildKey, m_strDebug);
	bResult &= m_psettingsToolset->SetTextValue(settingLanguageKey, "\\" + m_psettingsToolset->GetTextValue(settingLanguage));

	return bResult;
}

BOOL CToolset::SetLog(CLog* pLog)
{
	CTarget *pTool;

	// shouldn't set the log to NULL
	ASSERT(pLog);

	// store the toolset's log
	m_pLog = pLog;

	// set the log for each target
	for (POSITION pos = m_listTools.GetHeadPosition(); pos != NULL; ) {
		pTool = m_listTools.GetNext(pos);
		pTool->SetLog(m_pLog);
	}

	return TRUE;
}

CLog* CToolset::GetLog()
{
  return m_pLog;
}

const CToolPtrList& CToolset::GetToolList()
{
  return m_listTools;
}

BOOL CToolset::SetLanguage(CToolset::ToolsetLangs lang)
{
	// store the language
	m_lang = lang;

	// REVIEW(briancr): when the class hierarchy is rearranged
	// so that CO classes can get access to their parent and their
	// toolset, we won't need to set the language in the support
	// system; the CO class or whatever needs to know the language
	// can query the toolset.

	// the name of the dll that contains all the localized ide strings.
	CString cstrStringsDllName;

	// set the language for the support system.
	switch(m_lang)
	{
		case TL_ENGLISH:
		{
			SetLang(CAFE_LANG_ENGLISH);
			cstrStringsDllName = "StringsEnglish.dll";
			break;
		}
		case TL_GERMAN: 
		{
			SetLang(CAFE_LANG_GERMAN);
			cstrStringsDllName = "StringsGerman.dll";
			break;
		}
		case TL_JAPANESE: 
		{
			SetLang(CAFE_LANG_JAPANESE);
			cstrStringsDllName = "StringsJapanese.dll";
			break;
		}
		default: 
		{
			return FALSE;
			break;
		}
	}

	// load the library that contains the localized ide strings.
	m_hStringsDll = LoadLibrary(cstrStringsDllName);
	// store the handle so that the support layer can call GetLangDllHandle() to get it.
	SetLangDllHandle(m_hStringsDll);
	return TRUE;
}

CToolset::ToolsetLangs CToolset::GetLanguage(void)
{
	return m_lang;
}

/////////////////////////////////////////////////////////////////////////////
// CDevTools

CDevTools::CDevTools()
: m_pIDE(NULL),
  m_pCompilerDriver(NULL),
  m_pExecutable(NULL)
{
}

CDevTools::CDevTools(BUILD_TYPE nBuildType, HANDLE hOutput)
: CToolset(nBuildType, hOutput),
  m_pIDE(NULL),
  m_pCompilerDriver(NULL),
  m_pExecutable(NULL)
{
}

CDevTools::~CDevTools()
{
	if (m_pIDE)
		delete m_pIDE;
  if (m_pCompilerDriver)
    delete m_pCompilerDriver;
  if (m_pExecutable)
    delete m_pExecutable;
}

/////////////////////////////////////////////////////////////////////////////
// CVCTools

CExecutable* CVCTools::GetExecutable(LPCSTR szExeName)
{
	m_pExecutable->SetFileName(szExeName);
  return m_pExecutable;
}


/////////////////////////////////////////////////////////////////////////////
// CX86Tools

BOOL CX86Tools::Initialize(CString strBaseKey)
{
	BOOL bResult = TRUE;

	// set key for X86Tools and create settings object
	strBaseKey += "\\"+strX86ToolsKey;
	// save the base key for this toolset
	m_strBaseKey = strBaseKey;

	// create a new environment settings object
	m_psettingsEnv = new CSettings(m_strBaseKey + m_strLang + m_strDebug);
	// create a new toolset settings object
	m_psettingsToolset = new CSettings(m_strBaseKey);

	// create an IDE
	m_pIDE = new CX86IDE;
	bResult &= AddTool(m_pIDE);

	// create a compiler driver
	m_pCompilerDriver = new CX86CompilerDriver(m_hOutput);
	bResult &= AddTool(m_pCompilerDriver);

	// create an executable
	m_pExecutable = new CX86Executable(m_hOutput);
	AddTool(m_pExecutable);

	//TODO(CFlaat): implement prop page for executable
	//	bResult &= AddTool(m_pExecutable);

	//TODO(CFlaat): add compiler/executable class for other toolsets

	// initialize settings object
	bResult &= InitializeSettings();

	// initialize the target objects.
	m_pIDE->Initialize(m_nBuildType, m_strBaseKey);
	m_pCompilerDriver->Initialize(m_nBuildType, m_strBaseKey);
	m_pExecutable->Initialize(m_nBuildType, m_strBaseKey);

	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
// CWin32sTools
BOOL CWin32sTools::Initialize(CString strBaseKey)
{
	BOOL bResult = TRUE;

	// set key for Win32sTools and create settings object
	strBaseKey += "\\"+strWin32sToolsKey;
	// save the base key for this toolset
	m_strBaseKey = strBaseKey;

	// create a new environment settings object
	m_psettingsEnv = new CSettings(m_strBaseKey + m_strLang + m_strDebug);
	// create a new toolset settings object
	m_psettingsToolset = new CSettings(m_strBaseKey);

	// create an IDE
	m_pIDE = new CWin32sIDE;
	bResult &= AddTool(m_pIDE);

	// initialize settings object
	bResult &= InitializeSettings();

	// initialize the target object
	m_pIDE->Initialize(m_nBuildType, m_strBaseKey);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// C68KTools

BOOL C68KTools::Initialize(CString strBaseKey)
{
	BOOL bResult = TRUE;

	// set key for 68KTools and create settings object
	strBaseKey += "\\"+str68KToolsKey;
	// save the base key for this toolset
	m_strBaseKey = strBaseKey;

	// create a new environment settings object
	m_psettingsEnv = new CSettings(m_strBaseKey + m_strLang + m_strDebug);
	// create a new toolset settings object
	m_psettingsToolset = new CSettings(m_strBaseKey);

	// create an IDE
	m_pIDE = new C68KIDE;
	bResult &= AddTool(m_pIDE);

	// initialize settings object
	bResult &= InitializeSettings();

	// initialize the target object
	m_pIDE->Initialize(m_nBuildType, m_strBaseKey);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CPMacTools

BOOL CPMacTools::Initialize(CString strBaseKey)
{
	BOOL bResult = TRUE;

	// set key for PMacTools and create settings object
	strBaseKey += "\\"+strPMacToolsKey;
	// save the base key for this toolset
	m_strBaseKey = strBaseKey;

	// create a new environment settings object
	m_psettingsEnv = new CSettings(m_strBaseKey + m_strLang + m_strDebug);
	// create a new toolset settings object
	m_psettingsToolset = new CSettings(m_strBaseKey);

	// create an IDE
	m_pIDE = new CPMacIDE;
	bResult &= AddTool(m_pIDE);

	// initialize settings object
	bResult &= InitializeSettings();

	// initialize the target object
	m_pIDE->Initialize(m_nBuildType, m_strBaseKey);

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CMIPSTools

BOOL CMIPSTools::Initialize(CString strBaseKey)
{
	BOOL bResult = TRUE;

	// set key for MIPSTools and create settings object
	strBaseKey += "\\"+strMIPSToolsKey;
	// save the base key for this toolset
	m_strBaseKey = strBaseKey;

	// create a new environment settings object
	m_psettingsEnv = new CSettings(m_strBaseKey + m_strLang + m_strDebug);
	// create a new toolset settings object
	m_psettingsToolset = new CSettings(m_strBaseKey);

	// create an IDE
	m_pIDE = new CMIPSIDE;
	bResult &= AddTool(m_pIDE);

	// initialize settings object
	bResult &= InitializeSettings();

	// initialize the target object
	m_pIDE->Initialize(m_nBuildType, m_strBaseKey);


	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
// CAlphaTools

BOOL CAlphaTools::Initialize(CString strBaseKey)
{
	BOOL bResult = TRUE;

	// set key for AlphaTools and create settings object
	strBaseKey += "\\"+strAlphaToolsKey;
	// save the base key for this toolset
	m_strBaseKey = strBaseKey;

	// create a new environment settings object
	m_psettingsEnv = new CSettings(m_strBaseKey + m_strLang + m_strDebug);
	// create a new toolset settings object
	m_psettingsToolset = new CSettings(m_strBaseKey);

	// create an IDE
	m_pIDE = new CAlphaIDE;
	bResult &= AddTool(m_pIDE);

	// initialize settings object
	bResult &= InitializeSettings();

	// intialize the target object
	m_pIDE->Initialize(m_nBuildType, m_strBaseKey);

	return bResult;
}
/////////////////////////////////////////////////////////////////////////////
// CPowerPCTools

BOOL CPowerPCTools::Initialize(CString strBaseKey)
{
	BOOL bResult = TRUE;

	// set key for PowerPCTools and create settings object
	strBaseKey += "\\"+strPowerPCToolsKey;
	// save the base key for this toolset
	m_strBaseKey = strBaseKey;

	// create a new environment settings object
	m_psettingsEnv = new CSettings(m_strBaseKey + m_strLang + m_strDebug);
	// create a new toolset settings object
	m_psettingsToolset = new CSettings(m_strBaseKey);

	// create an IDE
	m_pIDE = new CPowerPCIDE;
	bResult &= AddTool(m_pIDE);

	// initialize settings object
	bResult &= InitializeSettings();

	// intialize the target object
	m_pIDE->Initialize(m_nBuildType, m_strBaseKey);


	return bResult;
}


// toolset creation

__declspec(dllexport) CToolset * CreateToolset(CPlatform::COS *pOS, CPlatform::CLanguage *pLang, CToolset::BUILD_TYPE eBT, HANDLE hOutput)
{
    CToolset *pToolset;

    if (*pOS == CPlatform::Mac68K)
        pToolset = new C68KTools(eBT, hOutput);
    else if (*pOS == CPlatform::MacPowerPC)
        pToolset = new CPMacTools(eBT, hOutput);
    else if (*pOS == CPlatform::Win32Mips) 
        pToolset = new CMIPSTools(eBT, hOutput);
    else if (*pOS == CPlatform::Win32Alpha)
        pToolset = new CAlphaTools(eBT, hOutput);
    else if (*pOS == CPlatform::Win32PowerPC)
        pToolset = new CPowerPCTools(eBT, hOutput);
    else if (*pOS == CPlatform::Win32s)
        pToolset = new CWin32sTools(eBT, hOutput);
    else // win32s will just be x86 for now
        pToolset = new CX86Tools(eBT, hOutput);


// TODO(CFlaat): axe CToolset::TL_ENGLISH, etc. & use CPlatform::CLanguage

    if (*pLang == CPlatform::English)
    {
        pToolset->SetLanguage(CToolset::TL_ENGLISH);
    }
    else if (*pLang == CPlatform::German)
    {
        pToolset->SetLanguage(CToolset::TL_GERMAN);
    }
    else if (*pLang == CPlatform::Japanese)
    {
        pToolset->SetLanguage(CToolset::TL_JAPANESE);
    }

    return pToolset;

}



///////////////////////////////////////////////////////////////////////////////
//	DLL support

static AFX_EXTENSION_MODULE extensionDLL = { NULL, NULL };

extern "C" __declspec(dllexport) int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("Toolset DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(extensionDLL, hInstance);

		// Insert this DLL into the resource chain
		new CDynLinkLibrary(extensionDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("Toolset DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(extensionDLL);

	}
	return 1;   // ok

	UNREFERENCED_PARAMETER(lpReserved);
}
