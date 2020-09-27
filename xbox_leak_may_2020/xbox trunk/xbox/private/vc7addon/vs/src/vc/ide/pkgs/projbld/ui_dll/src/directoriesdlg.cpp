// DirectoriesDlg.cpp : Implementation of CDirsDlg
#include "stdafx.h"
#include "DirectoriesDlg.h"
#include "bldpkg.h"
#include "vcarchy.h"
#include "profile.h"

/////////////////////////////////////////////////////////////////////////////
// CDirsDlg
    
CDirectoriesDlg::CDirectoriesDlg()
{
	m_nLastToolset = -1;
	m_nToolset = 0;
}

CDirectoriesDlg::~CDirectoriesDlg()
{
}

HRESULT CDirectoriesDlg::GetPlatformsEnumerator(IEnumVARIANT** ppEnum)
{
	CHECK_POINTER_NULL(ppEnum);

	VCProjectEngine* pProjEngine = g_pBuildPackage->GetProjectEngine();

    CComPtr<IDispatch> spDisp;
	HRESULT hr = pProjEngine->get_Platforms(&spDisp);
   	CComQIPtr<IVCCollection> spCollection = spDisp;

	VSASSERT(spCollection, "Could not get platforms collection.");
	RETURN_ON_FAIL_OR_NULL(hr, spCollection);
	
	hr = spCollection->_NewEnum(reinterpret_cast<IUnknown **>( ppEnum ) );
	VSASSERT(SUCCEEDED(hr) && *ppEnum, "Failed to get an enumerator on the platforms collection");
	RETURN_ON_FAIL_OR_NULL(hr, *ppEnum);

	(*ppEnum)->Reset();
	return hr;
}
LRESULT CDirectoriesDlg::OnFont(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	return 0;  // Let the system set the focus
}

LRESULT CDirectoriesDlg::OnInitDialog(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CStringW strDirsOpt, strToolsetName;	// yes, ANSI
	CWindow pathCombo(GetDlgItem(IDC_DIRS_TYPE));
	CWindow platformCombo(GetDlgItem(IDC_DIRS_TOOLSETS));

	USES_CONVERSION;

	CComPtr<IEnumVARIANT>		spEnum;
	HRESULT hr = GetPlatformsEnumerator(&spEnum);
	RETURN_ON_FAIL_OR_NULL(hr, spEnum);

	int currIndex = 0;
	while ( true )
	{
		CComVariant var;
		CComQIPtr<VCPlatform> spPlatform;
		CComBSTR bstrName;
		
		hr = spEnum->Next(1, &var, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE ) break;

		spPlatform = var.punkVal;
		if (spPlatform == NULL)
			continue;

		hr = spPlatform->get_Name(&m_pathBlockList[currIndex].m_platformName);
		
		::SendDlgItemMessage(m_hWnd,IDC_DIRS_TOOLSETS, CB_ADDSTRING, currIndex,
			(LPARAM)W2T(m_pathBlockList[currIndex].m_platformName) );

		spPlatform->get_ExecutableDirectories(&m_pathBlockList[currIndex].m_dirLists[DIRLIST_PATH]);
		spPlatform->get_IncludeDirectories(&m_pathBlockList[currIndex].m_dirLists[DIRLIST_INC]);
		spPlatform->get_ReferenceDirectories(&m_pathBlockList[currIndex].m_dirLists[DIRLIST_REF]);
		spPlatform->get_LibraryDirectories(&m_pathBlockList[currIndex].m_dirLists[DIRLIST_LIB]);
		spPlatform->get_SourceDirectories(&m_pathBlockList[currIndex].m_dirLists[DIRLIST_SRC]);
		currIndex++;
	}
    
	LoadDescription(VCPLATID_ExecutableDirectories, & m_strDescription[DIRLIST_PATH] );
	LoadTitle(VCPLATID_ExecutableDirectories, & m_strName[DIRLIST_PATH] );
	LoadDescription(VCPLATID_IncludeDirectories, & m_strDescription[DIRLIST_INC] );
	LoadTitle(VCPLATID_IncludeDirectories, & m_strName[DIRLIST_INC] );
	LoadDescription(VCPLATID_ReferenceDirectories, & m_strDescription[DIRLIST_REF] );
	LoadTitle(VCPLATID_ReferenceDirectories, & m_strName[DIRLIST_REF] );
	LoadDescription(VCPLATID_LibraryDirectories, & m_strDescription[DIRLIST_LIB] );
	LoadTitle(VCPLATID_LibraryDirectories, & m_strName[DIRLIST_LIB] );
	LoadDescription(VCPLATID_SourceDirectories, & m_strDescription[DIRLIST_SRC] );
	LoadTitle(VCPLATID_SourceDirectories, & m_strName[DIRLIST_SRC] );

	platformCombo.SendMessage(CB_SETCURSEL, 0);
	m_nToolset = 0;

	
	// Set the directory list type combo.
	FillOptionsList();
	pathCombo.SendMessage(CB_SETCURSEL, (const int)DIRLIST_PATH);

	CComPtr<IUnknown> pUnk;
	hr = ExternalQueryService(SID_SDirList, IID_IUnknown, (void **)&pUnk);
 
	RECT rectChild = { 0, 26, 263, 151 };
	m_wndDirList.Create(m_hWnd, rectChild, L"{AFD20A96-EED8-4D8C-A4EA-18AB96F6C432}",
		WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 0, IDC_DIRLIST);
	m_wndDirList.QueryControl(&m_pList);
	DoOnSize();

	// Establish the listbox contents.  This will also disable
	// any buttons which depend on a selection in the listbox.
	m_type = DIRLIST_PATH;	// make sure we're actually set to what we want to be!!!
	ResetListbox();

	return 1;  // Let the system set the focus
}

LRESULT CDirectoriesDlg::OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;

	DoOnSize();
	return 0;
}

void CDirectoriesDlg::DoOnSize()
{
	if (!m_wndDirList)
		return;

	// What we're doing here is filling the bulk of the dialog with the directory picker.
	// We're picking up the tool set combo box location merely to act as an anchor for the
	// upper left corner of the directory picker
	RECT rectParent, rectToolSetCombo, rectDirList;
	GetWindowRect(&rectParent);
	::GetWindowRect(GetDlgItem(IDC_DIRS_TOOLSETS), &rectToolSetCombo);

	rectDirList.left = 0;
	rectDirList.right = rectParent.right - rectParent.left;
	rectDirList.top = rectToolSetCombo.bottom - rectParent.top + ((rectToolSetCombo.bottom - rectToolSetCombo.top)/2);
	rectDirList.bottom = rectParent.bottom - rectParent.top;
	m_wndDirList.MoveWindow(&rectDirList);
}

void CDirectoriesDlg::FillOptionsList()
{
	CWindow pathCombo(GetDlgItem(IDC_DIRS_TYPE));

	CStringW strDirsOpt;

	if (!strDirsOpt.LoadString(IDS_DIRS_EXECUTABLE))
		strDirsOpt = "Executable files";
	::SendDlgItemMessage(m_hWnd,IDC_DIRS_TYPE,CB_ADDSTRING, DIRLIST_PATH, (LPARAM)(const wchar_t*)strDirsOpt);

	if (!strDirsOpt.LoadString(IDS_DIRS_INCLUDE))
		strDirsOpt = "Include files";
	::SendDlgItemMessage(m_hWnd,IDC_DIRS_TYPE,CB_ADDSTRING, DIRLIST_INC, (LPARAM)(const wchar_t*)strDirsOpt);

	if (!strDirsOpt.LoadString(IDS_DIRS_REFERENCE))
		strDirsOpt = "Reference files";
	::SendDlgItemMessage(m_hWnd,IDC_DIRS_TYPE,CB_ADDSTRING, DIRLIST_REF, (LPARAM)(const wchar_t*)strDirsOpt);

	if (!strDirsOpt.LoadString(IDS_DIRS_LIB))
		strDirsOpt = "Library files";
	::SendDlgItemMessage(m_hWnd,IDC_DIRS_TYPE,CB_ADDSTRING, DIRLIST_LIB, (LPARAM)(const wchar_t*)strDirsOpt);

	if (!strDirsOpt.LoadString(IDS_DIRS_SOURCE))
		strDirsOpt = "Source files";
	::SendDlgItemMessage(m_hWnd,IDC_DIRS_TYPE,CB_ADDSTRING, DIRLIST_SRC, (LPARAM)(const wchar_t*)strDirsOpt);

	BOOL bHandled = TRUE;
	OnSelChangeDirOptions(0, 0, 0, bHandled);
}

void CDirectoriesDlg::LoadDescription(UINT idDesc, BSTR * bstrDesc )
{
	if (idDesc <= 0)
		return;

	CStringW strFullDesc;
	CStringW strDesc;

	strFullDesc.LoadString(idDesc);

	int nColon = strFullDesc.Find(L": ");
	if (nColon < 0)
	{
		strDesc = strFullDesc;
	}
	else
	{
		int nLen = strFullDesc.GetLength();
		strDesc = strFullDesc.Right(strFullDesc.GetLength()-nColon-2);
	}
	strDesc.TrimLeft();
	strDesc.TrimRight();

	*bstrDesc = strDesc.AllocSysString();
}

void CDirectoriesDlg::LoadTitle(UINT idDesc, BSTR * bstrName)
{
	if (idDesc <= 0)
		return;

	CStringW strFullDesc;
	CStringW strName;
	strFullDesc.LoadString(idDesc);

	int nColon = strFullDesc.Find(L": ");
	if (nColon < 0)
	{
		strName = strFullDesc;
	}
	else
	{
		int nLen = strFullDesc.GetLength();
		strName = strFullDesc.Left(nColon);
	}
	strName.TrimLeft();
	strName.TrimRight();
	*bstrName = strName.AllocSysString();
}


void CDirectoriesDlg::ResetListbox()
{
	if( m_type < DIRLIST_PATH || m_type >= C_DIRLIST_TYPES || m_nToolset< 0 || m_nToolset >= _MAX_PLATFORMS)
		return;		// nothing to do

	RetrieveListboxInfo(m_nLastToolset, m_LastType);
	
	if( m_pList )
	{
		m_pList->put_Title( m_strName[m_type] );
		m_pList->put_Description( m_strDescription[m_type] );

		m_pList->Reset();
		CStringW temp = m_pathBlockList[m_nToolset].m_dirLists[m_type];
		int loc = temp.Find(L';');
		while( -1 != loc )
		{
			CComBSTR bstr = temp.Left( loc );
			temp = temp.Right( temp.GetLength() - loc -1);
			m_pList->Add(bstr);
			loc = temp.Find(';');
		}
		if (!temp.IsEmpty())	// pick up if there is one more on the end...
		{
			CComBSTR bstr = temp;
			m_pList->Add(bstr);
		}
	}
}

void CDirectoriesDlg::CleanUpString(int nToolset, DIRLIST_TYPE type)
{
	CComBSTR bstrDirList = m_pathBlockList[nToolset].m_dirLists[type];

	CStringW csDirList = bstrDirList;

	// first replace "\r\n"
	int loc = csDirList.Find(L"\r\n");
	while( -1 != loc )
	{
		csDirList.Delete( loc, 2 );
		csDirList.Insert( loc, L";");
		loc = csDirList.Find(L" ; ");
	}
	
	// now replace " ; "
	loc = csDirList.Find(L" ; ");
	while( -1 != loc )
	{
		csDirList.Delete( loc, 3 );
		csDirList.Insert( loc, L";");
		loc = csDirList.Find(L" ; ");
	}

	// now replace " ;"
	loc = csDirList.Find(L" ;");
	while( -1 != loc )
	{
		csDirList.Delete( loc, 2 );
		csDirList.Insert( loc, L";");

		loc = csDirList.Find(L" ;");
	}

	// now replace "; "
	loc = csDirList.Find(L"; ");
	while( -1 != loc )
	{
		csDirList.Delete( loc, 2 );
		csDirList.Insert( loc, L";");

		loc = csDirList.Find(L"; ");
	}

	// save it away.
	m_pathBlockList[nToolset].m_dirLists[type] = csDirList;
}

void CDirectoriesDlg::RetrieveListboxInfo(int nToolset, DIRLIST_TYPE type)
{
	if (nToolset < 0 || (int)type < 0)
	{
		m_nLastToolset = m_nToolset;
		m_LastType = m_type;

		return;
	}

	if( m_pList )
	{
		CComBSTR listData;
		//
		long i;
		long j;
		m_pList->get_Count( &j );
		for( i=0; i< j; i++)
		{
			CComBSTR bstrItem;
			m_pList->Index(i, &bstrItem);
			if (bstrItem.Length() > 0)
			{
				listData += bstrItem;
				listData += L";";
			}
		}

		m_pathBlockList[nToolset].m_dirLists[type] = listData;
		CleanUpString( nToolset, type );
	}

	m_nLastToolset = m_nToolset;
	m_LastType = m_type;
}

LRESULT CDirectoriesDlg::OnSelChangeDirOptions(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CWindow pathCombo(GetDlgItem(IDC_DIRS_TYPE));

	INT nSel = (INT) pathCombo.SendMessage(CB_GETCURSEL, 0L, 0L);

	if (m_type == (DIRLIST_TYPE)nSel)
		return TRUE;	// nothing changed = nothing to do

	m_LastType = m_type;
	m_type = (DIRLIST_TYPE)nSel;

	ResetListbox();
	return TRUE;
}

LRESULT CDirectoriesDlg::OnSelChangeDirToolset(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
	CWindow platformCombo(GetDlgItem(IDC_DIRS_TOOLSETS));

	INT		nSel = (INT) platformCombo.SendMessage(CB_GETCURSEL, 0L, 0L);
	if (m_nToolset == nSel)
		return TRUE;	// nothing changed, so nothing to do

	m_nLastToolset = m_nToolset;
	m_nToolset = nSel;

	ResetListbox();	
	return TRUE;
}


LRESULT CDirectoriesDlg::OnHelp(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	CComPtr<Help> pHlp;
	CComBSTR bstrTopic;
	bstrTopic = L"vc.proj.properties";

	HRESULT hr = E_FAIL;
	hr = ExternalQueryService(SID_SVsHelp, __uuidof(Help), (void **)&pHlp);
	if( pHlp )
		hr = pHlp->DisplayTopicFromF1Keyword(bstrTopic);
	return hr;
}


LRESULT CDirectoriesDlg::OnApply(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
{
	RetrieveListboxInfo(m_nToolset, m_type);

	CComPtr<IEnumVARIANT>		spEnum;
	HRESULT hr = GetPlatformsEnumerator(&spEnum);
	RETURN_ON_FAIL_OR_NULL(hr, spEnum);
	
	int currIndex = 0;
    while ( true )
    {
		CComVariant var;
		CComQIPtr<VCPlatform> spPlatform;
		CComBSTR bstrName;
		
		hr = spEnum->Next(1, &var, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE ) 
			break;

		spPlatform = var.punkVal;
		if (spPlatform == NULL)
			continue;

		spPlatform->put_ExecutableDirectories(m_pathBlockList[currIndex].m_dirLists[DIRLIST_PATH]);
		spPlatform->put_IncludeDirectories(m_pathBlockList[currIndex].m_dirLists[DIRLIST_INC]);
		spPlatform->put_ReferenceDirectories(m_pathBlockList[currIndex].m_dirLists[DIRLIST_REF]);
		spPlatform->put_LibraryDirectories(m_pathBlockList[currIndex].m_dirLists[DIRLIST_LIB]);
		spPlatform->put_SourceDirectories(m_pathBlockList[currIndex].m_dirLists[DIRLIST_SRC]);
		currIndex++;
    }

	WritePathsToRegistry();
	return 0;
}


HRESULT CDirectoriesDlg::WritePathsToRegistry( void )
{
	CComPtr<IEnumVARIANT> spEnum;
	HRESULT	hr = GetPlatformsEnumerator(&spEnum);
	RETURN_ON_FAIL_OR_NULL(hr, spEnum);

	int currIndex = 0;
    while ( true )
    {
		CComVariant var;
		CComQIPtr<VCPlatform> spPlatform;
		CComBSTR bstrName;
		
		hr = spEnum->Next(1, &var, NULL);
		RETURN_ON_FAIL(hr);
		if( hr == S_FALSE ) break;

		spPlatform = var.punkVal;
		if (spPlatform == NULL)
			continue;

		CStringW strEntry = L"VC\\VC_OBJECTS_PLATFORM_INFO\\";
		hr = spPlatform->get_Name(&bstrName);
		strEntry += bstrName;
		strEntry += L"\\Directories";

		CComBSTR bstrPath;
		spPlatform->get_ExecutableDirectories(&bstrPath);
		WriteRegPath(strEntry, L"Path Dirs", bstrPath, TRUE );
		bstrPath.Empty();
		spPlatform->get_IncludeDirectories(&bstrPath);
		WriteRegPath(strEntry, L"Include Dirs", bstrPath, TRUE );
		bstrPath.Empty();
		spPlatform->get_ReferenceDirectories(&bstrPath);
		WriteRegPath(strEntry, L"Reference Dirs", bstrPath, TRUE );
		bstrPath.Empty();
		spPlatform->get_LibraryDirectories(&bstrPath);
		WriteRegPath(strEntry, L"Library Dirs", bstrPath, TRUE );
		spPlatform->get_SourceDirectories(&bstrPath);
		WriteRegPath(strEntry, L"Source Dirs", bstrPath, TRUE );
		currIndex++;
    }

	return S_OK;
}
