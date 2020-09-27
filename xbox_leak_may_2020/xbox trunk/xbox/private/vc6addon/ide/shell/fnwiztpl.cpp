// fnwiztpl.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "fnwiztpl.h"
#include "oleref.h"
#include "bwizmgr.h"
#include "resource.h"
#include "prjapi.h"   //IPkgProject
#include "iswsguid.h" //IID_IWebPkgProject
#include "iswsapi.h"  //IWebProject
IMPLEMENT_DYNAMIC(CFileNewWizardTemplate, CPartTemplate)

CFileNewWizardTemplate :: CFileNewWizardTemplate (
	CPackage*	pPackage,
	REFCLSID	clsid,
	CString		strDesc, HICON hIcon /*=NULL*/) :
	CPartTemplate(IDR_DOCOBJECT /*IDR_FNWIZOBJECT*/,
		RUNTIME_CLASS(CPartDoc),
		RUNTIME_CLASS(CPartFrame),
		RUNTIME_CLASS(CPartView),
		pPackage,
		NULL,
		clsid)
{
	m_strDocStrings = _T("\n");		//  no default window title
	m_strDocStrings += _T("FileNew\n");// user visible name for default document
	// following lines for File.New
	m_strDocStrings += strDesc;		// for User Visible name for File New
	m_strDocStrings += _T("\n");
	// Assumption:
	// assuming all files for the time being
	m_strDocStrings += _T("All Files (*.*)\n");	// user visible name for FileOpen
	m_strDocStrings += _T(".~~~\n");	// user visible extension for FileOpen
   // Use Default Icon if hIcon is null
   m_hIcon = (hIcon == NULL) ? theApp.LoadIcon(IDI_FILENEWWIZDEFAULT) : hIcon;
}

CDocument* CFileNewWizardTemplate :: OpenDocumentFile(
	LPCTSTR		pszcPathName,
	BOOL		bMakeVisible)
{
	// Dont call this.  You should call NewDocumentFile
	ASSERT(!_T("FileNewWizards need a Project. Use NewDocumentFile"));
	return NULL;
}


// Execute the New File Wizard
CPartDoc* CFileNewWizardTemplate::NewDocumentFile
(
   LPCTSTR     pszFileName,
   IPkgProject *pProject,
   LPCTSTR     pszFolder
)
{
#define	NUM_PARAMS	6

	HRESULT	hr;
	COleRef<IBuilderWizardManager>	srpBWM;

	hr = theApp.GetService(SID_SBuilderWizardManager, IID_IBuilderWizardManager,
		(void**)&srpBWM);
	ASSERT(SUCCEEDED(hr));
	if(SUCCEEDED(hr))
	{
		COleRef<IDispatch>	srpBldrDisp;
		COleRef<IDispatch>	srpAppDisp;
		COleRef<IUnknown>	srpunk;

		CWnd*		pMainWnd = AfxGetMainWnd();
		ASSERT(pMainWnd);
		HWND		hMainWnd = pMainWnd->GetSafeHwnd();

		// IStudio bug fix (by scotg)
		// for VB4 created inproc servers, it is recommended
		// (by the VB group) to cocreate the object asking for
		// IUknown and then using the returned punk to QI for
		// the desired interface.
		hr = srpBWM->GetBuilder(m_clsid, 
			// REVIEW(cgomes): Should use BLDGETOPT_FAUTOMAPGUID 
			// rather than 0once MapBuilderCATIDToCLSID is fixed
			0,
			hMainWnd, &srpAppDisp, NULL, IID_IUnknown, (IUnknown**)&srpunk);
		ASSERT(SUCCEEDED(hr));
		hr = srpunk->QueryInterface(IID_IDispatch, (void**)&srpBldrDisp);
		ASSERT(SUCCEEDED(hr));
		if(hr == S_OK)
		{
			DISPID		diExecute;
			LPOLESTR	szExecute = (L"Execute");
			LCID		lcid = MAKELCID(
							MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
							SORT_DEFAULT);

			hr = srpBldrDisp->GetIDsOfNames(IID_NULL, &szExecute, 1, lcid, &diExecute);
			ASSERT(SUCCEEDED(hr));
			if(SUCCEEDED(hr))
			{
				// set up the dispparms.  They are:
				// 1. app object's IDispatch,
				// 2. hwndPromptOwner,
				// 3. IServiceProvider
				// 4. Project name,
				// 5. path name,
				// 6. bool flag to indicate "add to project"
				
				UINT		cArgs = NUM_PARAMS;
				DISPPARAMS	dp;

				dp.rgvarg = new VARIANTARG[cArgs];
				dp.cArgs = cArgs;
				dp.rgdispidNamedArgs = NULL;
				dp.cNamedArgs = 0;

				// since the machine pushes items on stack, store them in reverse order

				// param 1 (app object's IDispatch)
				::VariantInit(&(dp.rgvarg[--cArgs]));	
				dp.rgvarg[cArgs].vt = VT_DISPATCH;
				dp.rgvarg[cArgs].pdispVal = srpAppDisp.Disown();

				// param 2 (hwndOwner)
				::VariantInit(&(dp.rgvarg[--cArgs]));	
				dp.rgvarg[cArgs].vt = VT_I4;
				dp.rgvarg[cArgs].lVal = (long)hMainWnd;

				// param 3 (IServiceProvider interface)
				::VariantInit(&(dp.rgvarg[--cArgs]));
				dp.rgvarg[cArgs].vt = VT_UNKNOWN;
				dp.rgvarg[cArgs].punkVal = NULL;

#if 1
				// Get Project URL
				BSTR bstrProjectName = NULL;
				COleRef<IWebProject> srpWebProject;
				if(pProject == NULL ||
					FAILED(pProject->GetAutomationObject((IDispatch**)&srpWebProject)) ||
					FAILED(srpWebProject->get_VirtualRoot(&bstrProjectName)))
				{
					bstrProjectName = ::SysAllocString(L"");
				}
				ASSERT(bstrProjectName != NULL);
#else
				// Get the Project Name
				LPCOLESTR pszProjectName = NULL;
				LPCOLESTR pszProjFullPath = NULL;
				BSTR      bstrProjectName = NULL;
				DWORD     dwPackageID;
				DWORD     dwFlags;
				// Call GetProjectInfo to get back project name
				if(pProject != NULL && 
				   SUCCEEDED(pProject->GetProjectInfo(&pszProjectName, 
													  &pszProjFullPath, 
													  &dwPackageID, &dwFlags)))
				{
				   ASSERT(pszProjectName != NULL);
				   bstrProjectName = ::SysAllocString(pszProjectName);
				   ::AfxFreeTaskMem((void*)pszProjectName);
				   pszProjectName = NULL;
				   ::AfxFreeTaskMem((void*)pszProjFullPath);
				   pszProjFullPath = NULL;
				}
				else
				   bstrProjectName = ::SysAllocString(L"");
				ASSERT(bstrProjectName != NULL);
#endif
            
				// param 3 (Project Name)
				::VariantInit(&(dp.rgvarg[--cArgs]));	
				dp.rgvarg[cArgs].vt = VT_BSTR;
				dp.rgvarg[cArgs].bstrVal = bstrProjectName; // Freed by VariantClear

				// Format path name.  This should be the Folder \\ FileName
				CString		strPathName(pszFolder);
				if(!strPathName.IsEmpty())
				   // add \ to end of path b/c filename will be appended
				   strPathName += _T("\\");
				strPathName += pszFileName;

				// param 4 (path name)
				::VariantInit(&(dp.rgvarg[--cArgs]));	
				dp.rgvarg[cArgs].vt = VT_BSTR;
				dp.rgvarg[cArgs].bstrVal = strPathName.AllocSysString(); // Freed by Variant Clear

				// param 5 ("Add to Project" flag)
				// File New Wizards always add to Project, so this flag is use less
				::VariantInit(&(dp.rgvarg[--cArgs]));	
				dp.rgvarg[cArgs].vt = VT_BOOL;
				dp.rgvarg[cArgs].boolVal = VARIANT_TRUE;		//TRUE;

				VARIANT	varReturn;

				::VariantInit(&varReturn);

				srpBWM->EnableModeless(FALSE);
				hr = srpBldrDisp->Invoke(diExecute, IID_NULL, lcid,
					DISPATCH_METHOD, &dp, &varReturn, NULL, NULL);
				srpBWM->EnableModeless(TRUE);
				ASSERT(SUCCEEDED(hr));
				if(SUCCEEDED(hr))
				{
					// check the return value
					ASSERT(VT_BOOL == V_VT(&varReturn));
					if(V_BOOL(&varReturn))
					{
						// TODO
						// do whatever we need to do on success
					}
					::VariantClear(&varReturn);
				}
				else
					::AfxMessageBox(IDS_NOFILENEWWIZSUPPORT);

				// free the variants
				for(cArgs = 0; cArgs < NUM_PARAMS; cArgs++)
					::VariantClear(&(dp.rgvarg[cArgs]));

				// free the variant array
				delete [] dp.rgvarg;
			}
			else
				::AfxMessageBox(IDS_NOFILENEWWIZSUPPORT);
		}
		else
			::AfxMessageBox(IDS_REGISTRYCORRUPTED);
	}
	else
		::AfxMessageBox(IDS_REGISTRYCORRUPTED);

	return NULL;
}

//
//	BOOL CFileNewWizardTemplate::CanBeAddedToProject
//
//	Description:
//		This FileNewWizard can only be added to IWebPkgProjects.
//
//	Arguments:
//		IPkgProject *pProject:
//
//	Return (BOOL): TRUE if Project CLSID == IID_IWebPkgProject(s), FALSE otw
//
BOOL CFileNewWizardTemplate::CanBeAddedToProject(IPkgProject *pProject)
{
	if(NULL != pProject)
	{
		CLSID clsid;
		HRESULT hr = pProject->GetClassID(&clsid);
		ASSERT(SUCCEEDED(hr));
		return (SUCCEEDED(hr) && (clsid == IID_IWebPkgProject));
	}
	return FALSE;
}
