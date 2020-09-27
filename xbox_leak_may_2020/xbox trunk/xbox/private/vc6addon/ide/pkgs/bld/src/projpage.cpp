//
// CProjPage and other Project Window property page classes
//
// Created 8/30/93 [matthewt]

#include "stdafx.h"	// our standard AFX include
#pragma hdrstop
#include "projpage.h"	// our local class declarations
#include "projprop.h"	// our project properties
#include "projcomp.h"	// our spawner, and IDE interface (for DiffDlls)
#include "resource.h"	// our resource IDs
#include "optndlg.h"	// the parent of these pages (Project.Settings)
#include "optnui.h"		// using Validate_Destination()
#include "bldslob.h"	// using Validate_Destination()
#include "miscdlg.h"

#ifdef _DEBUG
#include "..\dbg_res\resource.h"
#endif

IMPLEMENT_DYNAMIC(CProjSlobPageTab, CSlobPageTab)
IMPLEMENT_DYNAMIC(CProjPage, CSlobPage)
IMPLEMENT_DYNAMIC(CToolGeneralPage, CProjSlobPageTab)
IMPLEMENT_DYNAMIC(CJavaToolGeneralPage, CProjSlobPageTab)
IMPLEMENT_DYNAMIC(COptsDebugTab, CProjSlobPageTab)
IMPLEMENT_DYNAMIC(CExtOptsPage, CProjSlobPageTab)
IMPLEMENT_DYNAMIC(CExtTargetOptsPage, CProjSlobPageTab)
#define PRELINKSTEP
#ifdef PRELINKSTEP
IMPLEMENT_DYNAMIC(CPreLinkPage, CProjSlobPageTab)
IMPLEMENT_DYNAMIC(CPostBuildPage, CProjSlobPageTab)
#endif

//IMPLEMENT_DYNAMIC(CDebugGeneralPage, CDebugPageTab)
//IMPLEMENT_DYNAMIC(CDebugAdditionalDllPage, CDebugPageTab)


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern void EnableControl(CWnd* pWnd, GPT gpt);		// implemented in proppage.cpp

void GetStrFromGrid(CStringListGridWnd* pGrid, CString& value)
{
	if (NULL==pGrid)
		return;

	ASSERT( pGrid->IsKindOf(RUNTIME_CLASS(CGridControlWnd)) );

	CStringListGridRow::c_bIsMove = TRUE;
	(void)pGrid->AcceptControl(FALSE);
	CStringListGridRow::c_bIsMove = FALSE;

	value.Empty();

	CString str;
	int iItem = 0;
	int cItems = pGrid->GetCount();

	while (iItem < cItems)
	{
		// get our strings from the grid
		if (pGrid->GetText(iItem, str))
		{
			// append newline.
			if (0!=iItem)
				value += _T("\t");

			value += str;
		}

		// next item
		iItem++;
	}
}


void SetStrFromGrid(CStringListGridWnd* pGrid, const CString& value)
{
	if (NULL==pGrid)
		return;

	ASSERT( pGrid->IsKindOf(RUNTIME_CLASS(CGridControlWnd)) );

	int iStart = 0;

	// reset our grid
	pGrid->ResetContent();

	while (iStart < value.GetLength())
	{
		int iEnd = iStart + value.Mid(iStart).Find(_T('\t'));

		if (iEnd>iStart)
		{
			pGrid->AddString(value.Mid(iStart,iEnd-iStart));
		}
		else if (iEnd<iStart)
		{
			pGrid->AddString(value.Mid(iStart));
			break;
		}
		iStart = iEnd + 1;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Base-Class for all Project Window tabs

//
// review(tomse): CProjSlobPageTab::Validate() is the only override of the base class,
// CSlobPageTab.  The description for CProjSlobPageTab says:
// "We override Validate, to bypass Undo/Redo support".  Since undo/redo is no longer
// bypassed, CProjSlobPageTab::Validate() is virtually the same as CSlobPageTab::Validate()
// except that CProjSlobPageTab::Validate() doesn't 
// handle pCtlMap->m_nCtlType == CControlMap::editStrCap.  CProjSlobPageTab can be removed
// and class derived from it can be derived from CSlobPageTab instead.
//

BOOL CProjSlobPageTab::Validate()
{
	BOOL bValid = TRUE;
	CWnd* pCtl;

	if (m_nValidateID == NULL)
		return TRUE;

	ASSERT(m_pSlob != NULL);

	CControlMap* pCtlMap = FindControl(m_nValidateID);
	ASSERT(pCtlMap != NULL);

	pCtl = GetDlgItem(pCtlMap->m_nCtlID);
	ASSERT(pCtl != NULL);

	CString strBuf;
	pCtl->GetWindowText(strBuf);

	CString strCurVal;
	if (m_pSlob->GetStrProp(pCtlMap->m_nProp, strCurVal) == valid)
	{
 		if (strCurVal == strBuf)
		{
			m_nValidateID = NULL;
			return TRUE;
		}
	}

	ASSERT(pCtlMap->m_nCtlType == CControlMap::comboText ||
		   pCtlMap->m_nCtlType == CControlMap::symbolCombo ||
		   pCtlMap->m_nCtlType == CControlMap::edit ||
		   pCtlMap->m_nCtlType == CControlMap::editInt ||
		   pCtlMap->m_nCtlType == CControlMap::editNum
		  );

	CMultiSlob* pSelection = NULL;
	CSlobWnd* pSlobWnd = m_pSlob->GetSlobWnd();
	if (pSlobWnd != NULL)
		pSelection = (CMultiSlob*)pSlobWnd->GetSelection();
	if (pSelection != NULL && !pSelection->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
		pSelection = NULL;

	//theUndoSlob.BeginUndo(IDS_UNDO_PROPEDIT);
	bValid = m_pSlob->SetStrProp(pCtlMap->m_nProp, strBuf);
	//theUndoSlob.EndUndo();

	OnPropChange(pCtlMap->m_nProp);

	if (bValid)
		m_nValidateID = NULL;
	else
		pCtl->SetFocus();

	return bValid;
}

///////////////////////////////////////////////////////////////////////////////
// Base-Class property page for all Project Window property pages

BOOL CProjPage::IsPageDisabled()
{
	return g_Spawner.SpawnActive();
}

///////////////////////////////////////////////////////////////////////////////
// External project options property page

///////////////////////////////////////////////////////////////////////////////
// no Properties property pages
CProjNoPropsPage g_NoPropsPage;

BEGIN_IDE_CONTROL_MAP(CProjNoPropsPage, IDDP_PROJ_NO_PROPS, IDS_NO_PROPS)
END_IDE_CONTROL_MAP()

///////////////////////////////////////////////////////////////////////////////
// General property pages

// 
// Targets
//
CTargetGeneralPage g_TargetGeneralPage;
BEGIN_IDE_CONTROL_MAP(CTargetGeneralPage, IDDP_TARGET_GENERAL, IDS_GENERAL)
	MAP_AUTO_EDIT(IDC_TARGET_NAME, P_ProjItemFullPath)
	MAP_THIN_TEXT(IDC_TARGET_DATE, P_ProjItemDate)
	MAP_AUTO_EDIT(IDC_SCC_STATUS, P_SccStatus)
END_IDE_CONTROL_MAP()
//

//
// Groups
//
CGrpGeneralPage g_GrpGeneralPage;
BEGIN_IDE_CONTROL_MAP(CGrpGeneralPage, IDDP_GRP_GENERAL, IDS_GENERAL)
	MAP_EDIT_LIMIT(IDC_GRP_NAME, P_FakeProjItemName,254)
	MAP_EDIT(IDC_GROUPFILTER, P_FakeGroupFilter)
END_IDE_CONTROL_MAP()

//
// Dependencies
//
CDepGeneralPage g_DepGeneralPage;
BEGIN_IDE_CONTROL_MAP(CDepGeneralPage, IDDP_DEP_GENERAL, IDS_GENERAL)
	MAP_AUTO_EDIT(IDC_FILE_NAME, P_ProjItemFullPath)
	MAP_THIN_TEXT(IDC_FILE_DATE, P_ProjItemDate)
END_IDE_CONTROL_MAP()
//

//
// Files
//
CFileGeneralPage g_FileGeneralPage;
BEGIN_IDE_CONTROL_MAP(CFileGeneralPage, IDDP_FILE_GENERAL, IDS_GENERAL)
	MAP_AUTO_EDIT(IDC_FILE_NAME, P_ProjItemFullPath)
	MAP_THIN_TEXT(IDC_FILE_DATE, P_ProjItemDate)
	MAP_EDIT(IDC_ORG_FILE_NAME, P_FakeProjItemOrgPath)
	MAP_AUTO_EDIT(IDC_SCC_STATUS, P_SccStatus)
END_IDE_CONTROL_MAP()

BOOL CFileGeneralPage::OnPropChange(UINT nProp)
{
	return CProjPage::OnPropChange(nProp);
}

BOOL CGrpGeneralPage::Validate( void )
{
	// are the old and new different ?
	CString strNewGrp, strGrp;
	CString strNewFilter, strFilter;

	GetDlgItem(IDC_GRP_NAME)->GetWindowText(strNewGrp);
	((CFileItem *)m_pSlob)->GetStrProp(P_ProjItemName, strGrp);

	StripLeadingAndTrailingWhite( strNewGrp );

	if( strNewGrp != strGrp )
	{
		if (strNewGrp.IsEmpty())
		{
			MsgBox (Error, IDS_NO_EMPTY_GROUP) ;

			CEdit *pEdit = (CEdit *) GetDlgItem( IDC_GRP_NAME );
			pEdit->SetWindowText( strGrp );
			pEdit->SetFocus();
			return FALSE;
		}

		CString strExternalDeps;
		strExternalDeps.LoadString(IDS_EXT_DEPENDENCIES);
		if (strNewGrp.CompareNoCase(strExternalDeps)==0)
		{
		
			CString	strError;
			MsgBox(Error, MsgText(strError, IDS_ERR_BAD_FOLDER, strNewGrp));

			CEdit *pEdit = (CEdit *) GetDlgItem( IDC_GRP_NAME );
			pEdit->SetWindowText( strGrp );
			pEdit->SetFocus();
			return FALSE;
		}


		// looks okay
		((CProjGroup *)m_pSlob)->SetStrProp(P_ProjItemName, strNewGrp);
		((CProjGroup *)m_pSlob)->GetProject()->DirtyProject();
	}

	GetDlgItem(IDC_GROUPFILTER)->GetWindowText(strNewFilter);
	((CFileItem *)m_pSlob)->GetStrProp(P_GroupDefaultFilter, strFilter);

	StripLeadingAndTrailingWhite( strNewFilter );

	if( strNewFilter != strFilter )
	{
		// UNDONE: validate filter
		((CProjGroup *)m_pSlob)->SetStrProp(P_GroupDefaultFilter, strNewFilter);
		((CProjGroup *)m_pSlob)->GetProject()->DirtyProject();
	}

	return CProjPage::Validate();
}

BOOL CFileGeneralPage::Validate( void )
{
	// are the old and new different ?
	CString strNewOrg;
	CString strOrg;

	GetDlgItem(IDC_ORG_FILE_NAME)->GetWindowText(strNewOrg);
	((CFileItem *)m_pSlob)->GetStrProp(P_ProjItemOrgPath,strOrg);
	if( strNewOrg != strOrg ){
		// they are different. should we disallow ?
		BOOL bAllow = TRUE;

		CString strPath;
		CPath pathOriginal;
		CPath pathNew;

	    // Get the original path
		((CFileItem *)m_pSlob)->GetStrProp(P_ProjItemFullPath,strPath);
		pathOriginal.Create(strPath);

		// Get The project directory
		CDir  dirProject;
		const CPath *pProjPath = ((CFileItem *)m_pSlob)->GetProject()->GetFilePath();
		dirProject.CreateFromPath(*pProjPath);
		// get the new path
		CString strNew = strNewOrg;
		ReplaceEnvVars(strNew);
		pathNew.CreateFromDirAndRelative(dirProject,strNew);
		BOOL bOldExists = pathOriginal.ExistsOnDisk();
		BOOL bNewExists = pathNew.ExistsOnDisk();
		CString strOldName = pathOriginal.GetFileName();
		CString strNewName = pathNew.GetFileName();
		
		NOTIFYRENAME notifyRename;
		notifyRename.OldFileName = pathOriginal;
		notifyRename.NewFileName= pathNew;
		if (pathNew == pathOriginal)
		{
			// they resolve to the same place. thats OK !
			((CFileItem *)m_pSlob)->GetProject()->DirtyProject();
			((CFileItem *)m_pSlob)->SetStrProp(P_ProjItemOrgPath,strNewOrg);
		}
		else if ((strOldName.CompareNoCase(strNewName)==0) && (bNewExists || !bOldExists) && (theApp.NotifyPackages(PN_QUERY_RENAME_FILE, (void *)&notifyRename)))
		{
			// or they resolve to the same name at least, but the
			// path was wrong

			// if these paths look reasonable, notify anyone who might care
			theApp.NotifyPackages(PN_RENAME_FILE, (void *)&notifyRename);
			// REVIEW: are these necessary?
			((CFileItem *)m_pSlob)->GetProject()->DirtyProject();
			((CFileItem *)m_pSlob)->SetStrProp(P_ProjItemOrgPath,strNewOrg);
		}
		else {
			// reset and say invalid path
			CString strMsg;
			
			if (MsgBox(Question,
				   MsgText(strMsg, IDS_WARN_RESET_DEFAULT_ORGPATH, (const TCHAR *)strNewOrg,(const TCHAR *)strPath),
				   MB_YESNO)==IDYES) {
				GetDlgItem(IDC_ORG_FILE_NAME)->SetWindowText(strOrg);
			}
			CWnd* pCtl;
			pCtl = GetDlgItem(IDC_ORG_FILE_NAME);
			pCtl->SetFocus();
			return FALSE;
		}
	}

	return CProjPage::Validate();
}

//

class CUseMFCEnum : public CLocalizedEnum
{
public:
	CUseMFCEnum() {wAllowMFCFilter = NoMFC | LibMFC | DllMFC;}

	virtual CLocalizedEnumerator* GetListL() {return &c_list[0];}
	virtual POSITION GetHeadPosition()
	{
		POSITION pos = CLocalizedEnum::GetHeadPosition(), posT;
		CLocalizedEnumerator * pEnum;																			  	
		posT = pos;
		while (!AllowEnum(posT, pEnum)) {pos = posT;}
		return pos;
	}

	virtual CLocalizedEnumerator* GetNextL(POSITION& pos)
	{
		CLocalizedEnumerator * pEnum, * pEnumT;
		pEnum = CLocalizedEnum::GetNextL(pos);

		POSITION posT = pos;
		// skip ones we don't recognise
 		while (!AllowEnum(posT, pEnumT)) {pos = posT;}
		return pEnum;
	}

	typedef enum {NoMFC = 0x1, LibMFC = 0x2, DllMFC = 0x4} AllowMFC;
	BOOL AllowEnum(POSITION & pos, CLocalizedEnumerator * & pEnum);
	__inline void AllowMFC(WORD wAllow) {wAllowMFCFilter = wAllow;}

	static CLocalizedEnumerator c_list[];

private:
	WORD wAllowMFCFilter;
};

BOOL CUseMFCEnum::AllowEnum(POSITION & pos, CLocalizedEnumerator * & pEnum)
{
	if (pos == (POSITION)NULL)	return TRUE;
	pEnum = CLocalizedEnum::GetNextL(pos);
	return ((pEnum->val == NoUseMFC && (wAllowMFCFilter & NoMFC) != 0) ||
			(pEnum->val == UseMFCInLibrary && (wAllowMFCFilter & LibMFC) != 0) ||
			(pEnum->val == UseMFCInDll && (wAllowMFCFilter & DllMFC) != 0)
		   );	// filter
}

CUseMFCEnum UseMFCEnum;

BEGIN_LOCALIZED_ENUM_LIST(CUseMFCEnum)
	LOCALIZED_ENUM_ENTRY(IDS_MFC_NONE /*"Not Using MFC"*/, NoUseMFC)
	LOCALIZED_ENUM_ENTRY(IDS_MFC_LIBRARY /*"Use MFC in a Static Library"*/, UseMFCInLibrary)
	LOCALIZED_ENUM_ENTRY(IDS_MFC_DLL /*"Use MFC in a Shared Dll (mfc30(d).dll)"*/, UseMFCInDll)
END_LOCALIZED_ENUM_LIST()

///////////////////////////////////////////////////////////////////////////////
// Tool General property page - shown in the Project.Tool Options 'mondo' dialog
BEGIN_IDE_CONTROL_MAP(CToolGeneralPage, IDDP_TOOL_GENERAL, IDS_GENERAL)
	MAP_COMBO_LIST(IDC_PROJ_USEMFC, P_ProjUseMFC, UseMFCEnum)
	MAP_EDIT(IDC_OUTDIRS_INT, P_OutDirs_Intermediate)
	MAP_EDIT(IDC_OUTDIRS_TRG, P_OutDirs_Target)
	MAP_CHECK(IDC_ITEM_EXCLUDE, P_ItemExcludedFromBuild)
	MAP_CHECK(IDC_ITEM_NOTOOL, P_ItemIgnoreDefaultTool)
	MAP_CHECK(IDC_PERCONFIG_DEPS, P_Proj_PerConfigDeps)
END_IDE_CONTROL_MAP()

BEGIN_MESSAGE_MAP(CToolGeneralPage, CProjSlobPageTab)
	//{{AFX_MSG_MAP(CToolGeneralPage)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_SET_DEFAULT, OnSetDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CToolGeneralPage::OnCreate(LPCREATESTRUCT lpcs)
{
	// can we create the window?
	if (CProjSlobPageTab::OnCreate(lpcs) == -1)
		return -1;	// no

	return 0;	// ok
}

BOOL IsLearningBox();

void CToolGeneralPage::InitPage()
{
	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	BOOL fIsProject = ((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CProject));
	BOOL fIsUtilProj = FALSE; // UNDONE
	// get the project type for the slob
	CProjType* pProjType = ((CProxySlob*)m_pSlob)->GetProjType();
	if (pProjType != NULL)
	{
		// Java: for Java, we want to hide the browser file name
		fIsUtilProj = (pProjType->GetUniqueTypeId() == CProjType::generic);
	}

	// If the project is an internal makefile then we can get the
	// platform by going through the project directly.
	// (Use the CProxySlob platform enumeration in case of multi-selection)

	// MFC is only supported for C++
	if (g_BldSysIFace.LanguageSupported(CPlusPlus))
	{
		BOOL fUsingMFC = g_prjcompmgr.MFCIsInstalled();
		BOOL fAppWizMfc = FALSE;		// App-Wiz generated this Use of MFC?
		BOOL fMfcInLib, fMfcInDLL;		// multi-selection of items' platform supports mfc in lib/dll?
		fMfcInLib = fMfcInDLL = fUsingMFC;

		if (fUsingMFC)
		{
			if (m_pSlob->GetIntProp(P_ProjAppWizUseMFC, fAppWizMfc) != valid)
				fAppWizMfc = TRUE;		// don't allow NoMFC for a mix

			((CProxySlob *)m_pSlob)->InitPlatformEnum();
			const CPlatform * pPlatform;
			while (fMfcInDLL && (pPlatform = ((CProxySlob *)m_pSlob)->NextPlatform()) != (const CPlatform *)NULL)
				fMfcInDLL = fMfcInDLL && ((pPlatform->GetAttributes() & PIA_Allow_MFCinDLL) != 0);
		}
		if( IsLearningBox() ){
			fMfcInLib = FALSE;
		}

		// only allow project setting for Use MFC
		UseMFCEnum.AllowMFC((!fAppWizMfc ? CUseMFCEnum::NoMFC : 0) |
							(fMfcInLib ? CUseMFCEnum::LibMFC : 0) |
							(fMfcInDLL ? CUseMFCEnum::DllMFC : 0));
	}
	
	// call our base-class first, before we enable/disable controls ourselves
	CProjSlobPageTab::InitPage();

	// allow perconfig deps setting only for projects
	GetDlgItem(IDC_PERCONFIG_DEPS)->EnableWindow(TRUE);
	GetDlgItem(IDC_PERCONFIG_DEPS)->ShowWindow(fIsProject ? SW_SHOW : SW_HIDE);

	// MFC is only supported for C++
	if (g_BldSysIFace.LanguageSupported(CPlusPlus))
	{
		GetDlgItem(IDC_USEMFC_TEXT)->EnableWindow(fIsProject);	// Enable 'Use MFC' text?
		GetDlgItem(IDC_PROJ_USEMFC)->EnableWindow(fIsProject);	// Enable 'Use MFC' checkbox?
		if (!fIsProject)
			((CComboBox *)GetDlgItem(IDC_PROJ_USEMFC))->SetCurSel(-1);			// nuke the selection

		GetDlgItem(IDC_USEMFC_TEXT)->ShowWindow(!fIsUtilProj ? SW_SHOW : SW_HIDE);	// Show 'Use MFC' text
		GetDlgItem(IDC_PROJ_USEMFC)->ShowWindow(!fIsUtilProj ? SW_SHOW : SW_HIDE);	// Show 'Use MFC' checkbox
	}

	// don't allow setting of target dirs for anything but the project
	GetDlgItem(IDC_OUTDIRS_TRGTXT)->EnableWindow(fIsProject);	// 'Targ Dir' text
	GetDlgItem(IDC_OUTDIRS_TRG)->EnableWindow(fIsProject);	// 'Targ Dir' editbox

	BOOL fIsFileItem =  ((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CFileItem));

	// don't allow item exclude from build for anything but files
	GetDlgItem(IDC_ITEM_EXCLUDE)->ShowWindow(fIsFileItem ? SW_SHOW : SW_HIDE);

	// don't allow item ignore default tool for anything but files
	GetDlgItem(IDC_ITEM_NOTOOL)->ShowWindow(
		fIsFileItem ? SW_SHOW :
	 SW_HIDE);

	// state of 'Reset...'
	UpdateState();
}

void CToolGeneralPage::UpdateState()
{
	// disable if it doesn't have any props of ours for children
	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));

	BOOL fEnableReset = FALSE;	// no by default
	POSITION pos = ((CProxySlob *)m_pSlob)->GetHeadPosition();
	while (!fEnableReset && (pos != (POSITION)NULL))
	{
		ConfigurationRecord * pcr = ((CProxySlob *)m_pSlob)->GetNext(pos)->pcr;
		fEnableReset =
			!(pcr->BagSame(BaseBag, CloneBag, P_ProjUseMFC, P_ProjUseMFC) &&
			  pcr->BagSame(BaseBag, CloneBag, P_OutDirs_Intermediate, P_OutDirs_Intermediate) &&
			  pcr->BagSame(BaseBag, CloneBag, P_OutDirs_Target, P_OutDirs_Target) &&
			  pcr->BagSame(BaseBag, CloneBag, P_ItemIgnoreDefaultTool, P_ItemIgnoreDefaultTool) &&
			  pcr->BagSame(BaseBag, CloneBag, P_ItemExcludedFromBuild, P_ItemExcludedFromBuild)
			 );
	}

	// UNDONE: validate ignore based on exclude
	BOOL fIsProject = ((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CProject));
	CButton * pIgnore = (CButton *)GetDlgItem(IDC_ITEM_NOTOOL);
	if ((!fIsProject) && (pIgnore) && (pIgnore->IsWindowVisible()))
	{
#if 0
		CButton * pExclude = (CButton *)GetDlgItem(IDC_ITEM_EXCLUDE);
		BOOL bEnableIgnore = (pExclude && pExclude->GetCheck()==0);
		if (pIgnore->IsWindowEnabled() != bEnableIgnore)
		{
			pIgnore->EnableWindow(bEnableIgnore);
		}
#else
		ASSERT(((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CFileItem)));
		pIgnore->EnableWindow(TRUE); // always enable if fileitem
#endif
	}

	CWnd * pwndButton = GetDlgItem(IDC_SET_DEFAULT); ASSERT(pwndButton != (CWnd *)NULL);
	// optimized to make *one* call to IsWindowEnabled() and only enable/disable if needed
	if (fEnableReset == !(pwndButton->IsWindowEnabled()))
	{
		pwndButton->EnableWindow(fEnableReset);
	}
}

BOOL CToolGeneralPage::OnPropChange(UINT nProp)
{
	if (nProp == P_OutDirs_Intermediate || nProp == P_OutDirs_Target)
	{
		CString str;
		int nLen;
		ASSERT(GetSlob());
		if (GetSlob()->GetStrProp(nProp, str) == valid)
		{
			CString oldstr = str;

			str.TrimLeft();
			str.TrimRight();
			nLen = str.GetLength();
			if ((nLen >=2) && (str[0] == '\"') && (str[nLen - 1] == '\"'))
			{
				// If quotes exist around the dir name, yank 'em
				str = str.Mid(1, nLen-2);

				// Trim right side again now that quotes have been stripped.
				str.TrimRight();
				GetSlob()->SetStrProp(nProp, str);
			}
			else if (nLen!=oldstr.GetLength()) // Comparing lengths faster than whole string
			{
				//
				// Warning: Only SetStrProp if string has changed since
				// SetStrProp will recurse into OnPropChange.
				//

				//
				// Restore old string and then only trim right-side
				//
				str = oldstr;
				str.TrimRight();
				if (str.GetLength()!=oldstr.GetLength())
					GetSlob()->SetStrProp(nProp, str);
			}
		}
	}

	UpdateState();
	BOOL retval = CProjSlobPageTab::OnPropChange(nProp);

	if (nProp == P_ItemIgnoreDefaultTool)
	{
		((CProjOptionsDlg *)GetParent())->m_bDirtyTabs = TRUE;
		GetParent()->PostMessage(WM_SELECT_ITEM);
	}
	return retval;
}

void CToolGeneralPage::OnSetDefault()
{
	// Give user a chance to bail...
	if (MsgBox(Question,IDS_WARN_RESET_DEFAULT_PROPERTIES, MB_YESNO)==IDNO)
		return;

	// we set to default by copying the prop bag from the base
	// prop bag to the current propbag
	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	POSITION pos = ((CProxySlob *)m_pSlob)->GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		OptTreeNode * pNode = ((CProxySlob *)m_pSlob)->GetNext(pos);

		// copy the page props from base->current
		ConfigurationRecord * pcr = pNode->pcr;
		pcr->BagCopy(BaseBag, CloneBag, P_ProjUseMFC, P_ProjUseMFC, TRUE);

		// Ensure using MFC after this
		// (do the set prop to force this update)
		CProp * pprop = pcr->GetPropBag(CloneBag)->FindProp(P_ProjUseMFC);
		if (pprop != (CProp *)NULL && pprop->m_nType == integer)
			m_pSlob->SetIntProp(P_ProjUseMFC, ((CIntProp *)pprop)->m_nVal);

		pcr->BagCopy(BaseBag, CloneBag, P_OutDirs_Intermediate, P_OutDirs_Intermediate, TRUE);
		pcr->BagCopy(BaseBag, CloneBag, P_OutDirs_Target, P_OutDirs_Target, TRUE);
		pcr->BagCopy(BaseBag, CloneBag, P_ItemIgnoreDefaultTool, P_ItemIgnoreDefaultTool, TRUE);
		pcr->BagCopy(BaseBag, CloneBag, P_ItemExcludedFromBuild, P_ItemExcludedFromBuild, TRUE);
	}

	// show these new prop values in the page
	InitPage();

	// start search from 'Reset...' button
	CWnd * pwndNext = GetNextDlgTabItem(GetDlgItem(IDC_SET_DEFAULT));
	if (pwndNext != (CWnd *)NULL)
		pwndNext->SetFocus();	// give to next WS_TABSTOP
	else
		pwndNext->GetParent()->SetFocus();	// give to our tabbed dialog

}
BOOL CToolGeneralPage::Validate()
{
	CString strTrg;
	CString strInt;
	static BOOL bMacrosWarned = FALSE;
	static BOOL bSpacesWarned = FALSE;

	GetDlgItem(IDC_OUTDIRS_TRG)->GetWindowText(strTrg);	// 'Targ Dir' editbox
	GetDlgItem(IDC_OUTDIRS_INT)->GetWindowText(strInt);	// 'Int Dir' editbox

	if (!bMacrosWarned)
	{
		int nPosTrg = strTrg.Find(_T("$("));
		int nPosInt = strInt.Find(_T("$("));
		if (nPosTrg != -1 || nPosInt != -1)
		{
			// No colon - must specify a volume
			// name
			InformationBox(IDS_NO_MACROS_HERE);
			bMacrosWarned = TRUE;
		}
	}
	if (!bSpacesWarned)
	{
		int nPosTrg = strTrg.Find(_T(" "));
		int nPosInt = strInt.Find(_T(" "));
		if (nPosTrg != -1 || nPosInt != -1)
		{
			// No colon - must specify a volume
			// name
			InformationBox(IDS_NO_SPACES_HERE);
			bSpacesWarned = TRUE;
		}
	}
 	return CProjSlobPageTab::Validate();
}
///////////////////////////////////////////////////////////////////////////////
// Java Tool General property page - shown in the Project.Tool Options 'mondo' dialog
BEGIN_IDE_CONTROL_MAP(CJavaToolGeneralPage, IDDP_JAVA_TOOL_GENERAL, IDS_GENERAL)
	MAP_EDIT(IDC_JAVA_EXTRA_CLASSPATH, P_Java_ExtraClasses)
	MAP_EDIT(IDC_OUTDIRS_TRG, P_OutDirs_Target)
	MAP_CHECK(IDC_ITEM_EXCLUDE, P_ItemExcludedFromBuild)
END_IDE_CONTROL_MAP()

BEGIN_MESSAGE_MAP(CJavaToolGeneralPage, CProjSlobPageTab)
	//{{AFX_MSG_MAP(CJavaToolGeneralPage)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_SET_DEFAULT, OnSetDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

int CJavaToolGeneralPage::OnCreate(LPCREATESTRUCT lpcs)
{
	// can we create the window?
	if (CProjSlobPageTab::OnCreate(lpcs) == -1)
		return -1;	// no

	return 0;	// ok
}

void CJavaToolGeneralPage::InitPage()
{
	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	BOOL fIsProject = ((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CProject));

	// If the project is an internal makefile then we can get the
	// platform by going through the project directly.
	// (Use the CProxySlob platform enumeration in case of multi-selection)

	// call our base-class first, before we enable/disable controls ourselves
	CProjSlobPageTab::InitPage();

	// don't allow setting of target dirs for anything but the project
	GetDlgItem(IDC_JAVA_EXTRA_CLASSPATH_TXT)->EnableWindow(fIsProject);	// 'extra classes' text
	GetDlgItem(IDC_JAVA_EXTRA_CLASSPATH)->EnableWindow(fIsProject);	// 'Extra classes' editbox
	GetDlgItem(IDC_OUTDIRS_TRG)->EnableWindow(fIsProject);	// 'output dir' text
	GetDlgItem(IDC_OUTDIRS_TRGTXT)->EnableWindow(fIsProject);	// 'output dir' editbox

	// don't allow item exclude from build for anything but files
	GetDlgItem(IDC_ITEM_EXCLUDE)->ShowWindow
		(((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CFileItem)) ? SW_SHOW : SW_HIDE);

	// state of 'Reset...'
	UpdateState();
}

void CJavaToolGeneralPage::UpdateState()
{
	// disable if it doesn't have any props of ours for children
	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));

	BOOL fEnableReset = FALSE;	// no by default
	POSITION pos = ((CProxySlob *)m_pSlob)->GetHeadPosition();
	while (!fEnableReset && (pos != (POSITION)NULL))
	{
		ConfigurationRecord * pcr = ((CProxySlob *)m_pSlob)->GetNext(pos)->pcr;
		fEnableReset =
			!(pcr->BagSame(BaseBag, CloneBag, P_OutDirs_Target, P_OutDirs_Target) &&
			  pcr->BagSame(BaseBag, CloneBag, P_ItemExcludedFromBuild, P_ItemExcludedFromBuild)
			 );
	}

	CWnd * pwndButton = GetDlgItem(IDC_SET_DEFAULT); ASSERT(pwndButton != (CWnd *)NULL);
	// optimized to make *one* call to IsWindowEnabled() and only enable/disable if needed
	if (pwndButton->IsWindowEnabled())
	{
		if (!fEnableReset)	pwndButton->EnableWindow(FALSE);
	}
	else
	{
		if (fEnableReset)	pwndButton->EnableWindow(TRUE);
	}
}

BOOL CJavaToolGeneralPage::OnPropChange(UINT nProp)
{
	if (nProp == P_OutDirs_Target)
	{
		// If quotes exist around the dir name, yank 'em
		CString str;
		int nLen;
		ASSERT(GetSlob());
		if (GetSlob()->GetStrProp(P_OutDirs_Target, str) == valid)
		{
			str.TrimLeft();
			str.TrimRight();
			nLen = str.GetLength();
			if ((nLen >=2) && (str[0] == '\"') && (str[nLen - 1] == '\"'))
			{
				str = str.Mid(1, nLen-2);
				GetSlob()->SetStrProp(P_OutDirs_Target, str);
			}
		}
	}

	UpdateState();
	return CProjSlobPageTab::OnPropChange(nProp);
}

void CJavaToolGeneralPage::OnSetDefault()
{
	// Give user a chance to bail...
	if (MsgBox(Question,IDS_WARN_RESET_DEFAULT_PROPERTIES, MB_YESNO)==IDNO)
		return;

	// we set to default by copying the prop bag from the base
	// prop bag to the current propbag
	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	POSITION pos = ((CProxySlob *)m_pSlob)->GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		OptTreeNode * pNode = ((CProxySlob *)m_pSlob)->GetNext(pos);

		// copy the page props from base->current
		ConfigurationRecord * pcr = pNode->pcr;

		pcr->BagCopy(BaseBag, CloneBag, P_OutDirs_Target, P_OutDirs_Target, TRUE);
		pcr->BagCopy(BaseBag, CloneBag, P_ItemExcludedFromBuild, P_ItemExcludedFromBuild, TRUE);
	}

	// show these new prop values in the page
	InitPage();

	// start search from 'Reset...' button
	CWnd * pwndNext = GetNextDlgTabItem(GetDlgItem(IDC_SET_DEFAULT));
	if (pwndNext != (CWnd *)NULL)
		pwndNext->SetFocus();	// give to next WS_TABSTOP
	else
		pwndNext->GetParent()->SetFocus();	// give to our tabbed dialog
}

///////////////////////////////////////////////////////////////////////////////
// External project options property pages
BEGIN_IDE_CONTROL_MAP(CExtOptsPage, IDDP_EXTERNALMAKE_OPTIONS, IDS_GENERAL)
	MAP_EDIT(IDC_EXTOPTS_CMDLINE, P_ExtOpts_CmdLine)
	MAP_EDIT(IDC_EXTOPTS_REBUILDOPT, P_ExtOpts_RebuildOpt)
	MAP_EDIT(IDC_EXTOPTS_TARGET, P_ExtOpts_Targ)
	MAP_EDIT(IDC_EXTOPTS_BSCNAME, P_ExtOpts_BscName)
	MAP_COMBO_TEXT(IDC_EXTOPTS_PLATFORM, P_ExtOpts_Platform, g_thePlatformEnum)
END_IDE_CONTROL_MAP()

BEGIN_MESSAGE_MAP(CExtOptsPage, CProjSlobPageTab)
	//{{AFX_MSG_MAP(CExtOptsPage)
	ON_BN_CLICKED(IDC_SET_DEFAULT, OnSetDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExtOptsPage::InitPage()
{
	CProjSlobPageTab::InitPage();

	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	ASSERT(((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CProject)));

	POSITION pos = ((CProxySlob *)m_pSlob)->GetHeadPosition();

	while (NULL!=pos)
	{
		OptTreeNode* pNode = ((CProxySlob *)m_pSlob)->GetNext(pos);
		if ( NULL!=pNode && NULL!=pNode->pItem )
		{
			CProject* pProject = pNode->pItem->GetProject();
			if (pProject->m_bProjIsExe)
			{
				GetDlgItem(IDC_EXTOPTS_REBUILDOPT)->EnableWindow(FALSE);
				break;
			}
		}
	}


	// Orion bug #13013: can't change platform for .exe projects
	CWnd * pWnd = GetDlgItem(IDC_EXTOPTS_PLATFORM);
	if (pWnd != NULL)
	{
		pWnd->EnableWindow(FALSE);
		pWnd = GetDlgItem(IDC_EXTOPTS_PLATFORM_TXT);
		ASSERT(pWnd);
		pWnd->EnableWindow(FALSE);
	}

	// determine if the platform is Java
	CString strUIDescription;
	if (((CProxySlob*)m_pSlob)->GetStrProp(P_ExtOpts_Platform, strUIDescription) == valid)
	{
		CPlatform* pPlatform;
		g_prjcompmgr.LookupPlatformByUIDescription(strUIDescription, pPlatform);

		// is the platform Java
		if (pPlatform->GetUniqueId() == java)
		{
			int rgControls[] = { IDC_EXTOPTS_BSCNAME_TXT, IDC_EXTOPTS_BSCNAME,
								IDC_EXTOPTS_PLATFORM_TXT, IDC_EXTOPTS_PLATFORM };
			const int crgControls = sizeof(rgControls) / sizeof(int);
			for (int i = 0; i < crgControls; i++)
			{
				// hide each control
				CWnd* pWnd = GetDlgItem(rgControls[i]);
				ASSERT(pWnd != NULL);
				if (pWnd != NULL)
					pWnd->ShowWindow(SW_HIDE);
			}
		}
	}

	// update our 'Reset...' state
	UpdateState();
}

void CExtOptsPage::UpdateState()
{
	// disable if it doesn't have any props of ours for children
	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));

	BOOL fEnableReset = FALSE; // by default no
	POSITION pos = ((CProxySlob *)m_pSlob)->GetHeadPosition();
	while (!fEnableReset && (pos != (POSITION)NULL))
	{
		ConfigurationRecord * pcr = ((CProxySlob *)m_pSlob)->GetNext(pos)->pcr;
		fEnableReset =
			!(pcr->BagSame(BaseBag, CloneBag, P_ExtOpts_CmdLine, P_ExtOpts_CmdLine) &&
			  pcr->BagSame(BaseBag, CloneBag, P_ExtOpts_RebuildOpt, P_ExtOpts_RebuildOpt) &&
  			  pcr->BagSame(BaseBag, CloneBag, P_ExtOpts_Targ, P_ExtOpts_Targ) &&
			  pcr->BagSame(BaseBag, CloneBag, P_ExtOpts_BscName, P_ExtOpts_BscName) &&
			  pcr->BagSame(BaseBag, CloneBag, P_ExtOpts_Platform, P_ExtOpts_Platform) 
			 );
	}

	CWnd * pwndButton = GetDlgItem(IDC_SET_DEFAULT); ASSERT(pwndButton != (CWnd *)NULL);
	// optimized to make *one* call to IsWindowEnabled() and only enable/disable if needed
	if (pwndButton->IsWindowEnabled())
	{
		if (!fEnableReset)	pwndButton->EnableWindow(FALSE);
	}
	else
	{
		if (fEnableReset)	pwndButton->EnableWindow(TRUE);
	}
}

BOOL CExtOptsPage::OnPropChange(UINT idProp)
{
	UpdateState();

	CString strT;
	CSlob * pSlob = GetSlob(); ASSERT(pSlob != (CSlob *)NULL);

	if (idProp == P_ExtOpts_CmdLine)
	{
		GetDlgItem(IDC_BUILDCMD_TEXT)->EnableWindow
			(pSlob->GetStrProp(P_ExtOpts_CmdLine, strT) != invalid);
	}
	else if (idProp == P_ExtOpts_RebuildOpt)
	{
		GetDlgItem(IDC_REBUILDALL_TEXT)->EnableWindow
			(pSlob->GetStrProp(P_ExtOpts_RebuildOpt, strT) != invalid);
	}
	else if (idProp == P_ExtOpts_Platform)
	{
		// FUTURE (matthewt): clear the arguments / caller fields when we change platforms?

		// clear the arguments / caller fields when we change platforms
		// do the clear here .. beware that the slob is a CProxySlob
	}

	return CProjSlobPageTab::OnPropChange(idProp);
}

void CExtOptsPage::OnSetDefault()
{
	// Give user a chance to bail...
	if (MsgBox(Question,IDS_WARN_RESET_DEFAULT_PROPERTIES, MB_YESNO)==IDNO)
		return;

	// we set to default by copying the prop bag from the base
	// prop bag to the current propbag
	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	POSITION pos = ((CProxySlob *)m_pSlob)->GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		OptTreeNode * pNode = ((CProxySlob *)m_pSlob)->GetNext(pos);

		// copy the page props from base->current
		ConfigurationRecord * pcr = pNode->pcr;
		pcr->BagCopy(BaseBag, CloneBag, P_ExtOpts_CmdLine, P_ExtOpts_CmdLine, TRUE);
		pcr->BagCopy(BaseBag, CloneBag, P_ExtOpts_RebuildOpt, P_ExtOpts_RebuildOpt, TRUE);
		pcr->BagCopy(BaseBag, CloneBag, P_ExtOpts_BscName, P_ExtOpts_BscName, TRUE);
		pcr->BagCopy(BaseBag, CloneBag, P_ExtOpts_Targ, P_ExtOpts_Targ, TRUE);
		pcr->BagCopy(BaseBag, CloneBag, P_ExtOpts_Platform, P_ExtOpts_Platform, TRUE);
	}

	// show these new prop values in the page
	InitPage();

	// start search from 'Reset...' button
	CWnd * pwndNext = GetNextDlgTabItem(GetDlgItem(IDC_SET_DEFAULT));
	if (pwndNext != (CWnd *)NULL)
		pwndNext->SetFocus();	// give to next WS_TABSTOP
	else
		pwndNext->GetParent()->SetFocus();	// give to our tabbed dialog
}

BOOL CExtOptsPage::Validate()
{
    return CProjSlobPageTab::Validate() ;
}

///////////////////////////////////////////////////////////////////////////////
// External project options property pages
BEGIN_IDE_CONTROL_MAP(CExtTargetOptsPage, IDDP_EXTTARGET_OPTIONS, IDS_GENERAL)
	MAP_EDIT(IDC_EXTOPTS_CMDLINE, P_Proj_CmdLine)
	MAP_EDIT(IDC_EXTOPTS_REBUILDOPT, P_Proj_RebuildOpt)
	MAP_EDIT(IDC_EXTOPTS_TARGET, P_Proj_Targ)
	MAP_EDIT(IDC_EXTOPTS_BSCNAME, P_Proj_BscName)
//	MAP_EDIT(IDC_EXTOPTS_WORKINGDIR, P_Proj_WorkingDir)
END_IDE_CONTROL_MAP()

BEGIN_MESSAGE_MAP(CExtTargetOptsPage, CProjSlobPageTab)
	//{{AFX_MSG_MAP(CExtTargetOptsPage)
	ON_BN_CLICKED(IDC_SET_DEFAULT, OnSetDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CExtTargetOptsPage::InitPage()
{
	CProjSlobPageTab::InitPage();

	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	ASSERT(((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CProject)));

	POSITION pos = ((CProxySlob *)m_pSlob)->GetHeadPosition();

	while (NULL!=pos)
	{
		OptTreeNode* pNode = ((CProxySlob *)m_pSlob)->GetNext(pos);
		if ( NULL!=pNode && NULL!=pNode->pItem )
		{
			CProject* pProject = pNode->pItem->GetProject();
			if (pProject->m_bProjIsExe)
			{
				GetDlgItem(IDC_EXTOPTS_REBUILDOPT)->EnableWindow(FALSE);
				break;
			}
		}
	}


	// get the project type for the slob
	CProjType* pProjType = ((CProxySlob*)m_pSlob)->GetProjType();
	if (pProjType != NULL)
	{
		// Java: for Java, we want to hide the browser file name
		if (pProjType->GetUniquePlatformId() == java)
		{
			int rgControls[] = { IDC_EXTOPTS_BSCNAME_TXT, IDC_EXTOPTS_BSCNAME };
			const int crgControls = sizeof(rgControls) / sizeof(int);
			for (int i = 0; i < crgControls; i++)
			{
				// hide each control
				CWnd* pWnd = GetDlgItem(rgControls[i]);
				ASSERT(pWnd != NULL);
				if (pWnd != NULL)
					pWnd->ShowWindow(SW_HIDE);
			}
		}
	}

	// update our 'Reset...' state
	UpdateState();
}

void CExtTargetOptsPage::UpdateState()
{
	// disable if it doesn't have any props of ours for children
	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));

	BOOL fEnableReset = FALSE; // by default no
	POSITION pos = ((CProxySlob *)m_pSlob)->GetHeadPosition();
	while (!fEnableReset && (pos != (POSITION)NULL))
	{
		ConfigurationRecord * pcr = ((CProxySlob *)m_pSlob)->GetNext(pos)->pcr;
		fEnableReset =
			!(pcr->BagSame(BaseBag, CloneBag, P_Proj_CmdLine, P_Proj_CmdLine) &&
			  pcr->BagSame(BaseBag, CloneBag, P_Proj_RebuildOpt, P_Proj_RebuildOpt) &&
  			  pcr->BagSame(BaseBag, CloneBag, P_Proj_Targ, P_Proj_Targ) &&
			  pcr->BagSame(BaseBag, CloneBag, P_Proj_BscName, P_Proj_BscName)
 			 // pcr->BagSame(BaseBag, CloneBag, P_Proj_WorkingDir, P_Proj_WorkingDir)
			 );
	}

	CWnd * pwndButton = GetDlgItem(IDC_SET_DEFAULT); ASSERT(pwndButton != (CWnd *)NULL);
	// optimized to make *one* call to IsWindowEnabled() and only enable/disable if needed
	if (pwndButton->IsWindowEnabled())
	{
		if (!fEnableReset)	pwndButton->EnableWindow(FALSE);
	}
	else
	{
		if (fEnableReset)	pwndButton->EnableWindow(TRUE);
	}
}

BOOL CExtTargetOptsPage::OnPropChange(UINT idProp)
{
	UpdateState();

	CString strT;
	CSlob * pSlob = GetSlob(); ASSERT(pSlob != (CSlob *)NULL);

	if (idProp == P_Proj_CmdLine)
	{
		GetDlgItem(IDC_BUILDCMD_TEXT)->EnableWindow
			(pSlob->GetStrProp(P_Proj_CmdLine, strT) != invalid);
	}
	else if (idProp == P_Proj_RebuildOpt)
	{
		GetDlgItem(IDC_REBUILDALL_TEXT)->EnableWindow
			(pSlob->GetStrProp(P_Proj_RebuildOpt, strT) != invalid);
	}
	else if (idProp == P_ExtOpts_Platform)
	{
		// FUTURE (matthewt): clear the arguments / caller fields when we change platforms?

		// clear the arguments / caller fields when we change platforms
		// do the clear here .. beware that the slob is a CProxySlob
	}

	return CProjSlobPageTab::OnPropChange(idProp);
}

void CExtTargetOptsPage::OnSetDefault()
{
	// Give user a chance to bail...
	if (MsgBox(Question,IDS_WARN_RESET_DEFAULT_PROPERTIES, MB_YESNO)==IDNO)
		return;

	// we set to default by copying the prop bag from the base
	// prop bag to the current propbag
	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	POSITION pos = ((CProxySlob *)m_pSlob)->GetHeadPosition();
	while (pos != (POSITION)NULL)
	{
		OptTreeNode * pNode = ((CProxySlob *)m_pSlob)->GetNext(pos);

		// copy the page props from base->current
		ConfigurationRecord * pcr = pNode->pcr;
		pcr->BagCopy(BaseBag, CloneBag, P_Proj_CmdLine, P_Proj_CmdLine, TRUE);
		pcr->BagCopy(BaseBag, CloneBag, P_Proj_RebuildOpt, P_Proj_RebuildOpt, TRUE);
		pcr->BagCopy(BaseBag, CloneBag, P_Proj_BscName, P_Proj_BscName, TRUE);
		pcr->BagCopy(BaseBag, CloneBag, P_Proj_Targ, P_Proj_Targ, TRUE);
//		pcr->BagCopy(BaseBag, CloneBag, P_Proj_WorkingDir, P_Proj_WorkingDir, TRUE);
	}

	// show these new prop values in the page
	InitPage();

	// start search from 'Reset...' button
	CWnd * pwndNext = GetNextDlgTabItem(GetDlgItem(IDC_SET_DEFAULT));
	if (pwndNext != (CWnd *)NULL)
		pwndNext->SetFocus();	// give to next WS_TABSTOP
	else
		pwndNext->GetParent()->SetFocus();	// give to our tabbed dialog
}

BOOL CExtTargetOptsPage::Validate()
{
    return CProjSlobPageTab::Validate() ;
}

BEGIN_MESSAGE_MAP(COptsDebugTab, CProjSlobPageTab)
	//{{AFX_MSG_MAP(COptsDebugTab)
	ON_CBN_SELCHANGE(IDC_MINI_PAGE, OnMiniPageSelect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL COptsDebugTab::OnInitDialog()
{
	// call the base-class
	if (!CProjSlobPageTab::OnInitDialog())
		return FALSE;

 	// get our listbox
	m_pcboMiniPage = (CComboBox *)GetDlgItem(IDC_MINI_PAGE);
	ASSERT(m_pcboMiniPage != (CWnd *)NULL);
		
	// init. one-time stuff yet?
	if (!m_pProxySlob)
	{
		// remember our parent's CProxySlob
		m_pProxySlob = &((CProjOptionsDlg *)GetParent())->m_ProxySlob;

		// get the width/height of our debug option tab
		CRect rectTab; GetWindowRect(rectTab);

		CWnd * pWnd; CRect rectTxt;
		pWnd = GetDlgItem(IDC_MINI_PAGE); ASSERT(pWnd != (CWnd *)NULL);

		// and set the top of the mini page...
		pWnd->GetWindowRect(rectTxt); ScreenToClient(&rectTxt);
		m_cyPageTop = rectTxt.bottom + 3;
		
		// get the first page we looked at
		// and find it's size, make the tool option tab
		// big enough for this page plus the tool option tab
		// controls in the reserved 'top-half' of the tab
		
		// create the first page		
		COptionMiniPage * pPage = CreatePage(0);
		ASSERT(pPage != (COptionMiniPage *)NULL);

		CRect page; pPage->GetWindowRect(page);
		int cyPage = page.Height() - 1;
		SetWindowPos(NULL, 0, 0, max(rectTab.Width(), page.Width()), rectTab.Height() + cyPage, 
					 SWP_NOMOVE | SWP_NOACTIVATE);
	}

	// fill our listbox
	COptionMiniPage * pPage = (COptionMiniPage *)NULL;
	CString strPageName;
	for (int i = 0; i <= m_rgPages.GetUpperBound(); i++)
	{
		// add the names of our mini-pages into the combo-box
		pPage = (COptionMiniPage *)m_rgPages[i];
		
		// first one is called 'General'
		if (i == 0)
			strPageName.LoadString(IDS_GENERAL);
		else
			pPage->GetName(strPageName);

		m_pcboMiniPage->AddString(strPageName);
	}

	// do we need to hide or show the 'General' listbox?
	GetDlgItem(IDC_PAGE_CATEGORY)->ShowWindow(i > 1 ? SW_SHOW : SW_HIDE);	// static text
	m_pcboMiniPage->ShowWindow(i > 1 ? SW_SHOW : SW_HIDE);	// listbox
	
	return TRUE; // success
}

void COptsDebugTab::CommitTab()
{
	// save any mini pages settings that aren't properties
	for (int i = 0; i <= m_rgPages.GetUpperBound(); i++) {
		((CDebugPageTab *)m_rgPages[i])->CommitPage();
	}

	CProjSlobPageTab::CommitTab();
}

void COptsDebugTab::OnMiniPageSelect()
{
	BOOL fValid;	// valid?
	// validate current COptionMiniPage
	COptionMiniPage * pPage = GetOptionPage(m_nCurrPage);
	ASSERT(pPage);

	if (!pPage->m_isValidating)
	{
		pPage->m_isValidating = TRUE;
		fValid = pPage->Validate();	// validate page
		pPage->m_isValidating = FALSE;	
	}

	if (fValid) {
		int iPage = m_pcboMiniPage->GetCurSel();
		ASSERT (iPage != CB_ERR);	// can't have no selection!
		SelectPage(iPage);
	} else	{
		// Can't leave this page - restore the old category 
		m_pcboMiniPage->SetCurSel(m_nCurrPage);
	}
}

void COptsDebugTab::SelectPage(UINT nPage)
{
	// no-op if there is no current page change
	if (nPage == m_nCurrPage)
		return;

	COptionMiniPage * pPage = GetOptionPage(m_nCurrPage);

	// activate the new one
	if (!ActivatePage(nPage))
		return;

	// deactivate the old one
	DeactivatePage(m_nCurrPage);

	// this is now our current
	m_nCurrPage = nPage;
}

BOOL COptsDebugTab::AddOptionPage(COptionMiniPage * pPage)
{
	if (pPage == (COptionMiniPage *)NULL)
		return FALSE;	// no creation

	m_rgPages.Add(pPage);
	return TRUE;
}

void COptsDebugTab::RemoveAllPages()
{
	// remove any of the mini pages we created
	int nPages = m_rgPages.GetSize();
	for (int i = 0; i < nPages; i++)
	{
		// destroy the window
		COptionMiniPage* pPage = (COptionMiniPage *)m_rgPages[i];

		pPage->DestroyWindow();
		delete pPage;
	}
}

BOOL COptsDebugTab::Activate(CTabbedDialog * pParentWnd, CPoint position)
{
	// call our base-class first
	if (!CDlgTab::Activate(pParentWnd, position))
		return FALSE;

	// find the category to select if first activation of this tab
	if (m_nCurrPage == -1)
		if ((m_nCurrPage = m_pcboMiniPage->FindStringExact(-1, m_strInitCategory)) == CB_ERR)
			m_nCurrPage = 0;	// if can't find category, default to first

	// select the page for this cateogyr in our listbox of pages
	m_pcboMiniPage->SetCurSel(m_nCurrPage);

	// make sure that while this tab is up 
	// the option parser/generator is using
	// the approp. COptionHandler
#if 0
	if (m_popthdlr != (COptionHandler *)NULL)
		g_prjoptengine.SetOptionHandler(m_popthdlr);
 
	// make sure the ProxySlob knows about us
	((CProxySlob *)g_prjoptengine.GetPropertyBag())->SetOptionHandler(m_popthdlr);
#endif

	// activate our first page
	if (!ActivatePage(m_nCurrPage))
		return FALSE;
		
	// make sure our state is correct
	//UpdateState();

	// ok
	return TRUE;
}

void COptsDebugTab::Deactivate(CTabbedDialog* pParentWnd)
{
	// call our base-class first
	CDlgTab::Deactivate(pParentWnd);

	// de-activate our current page
	DeactivatePage(m_nCurrPage);
}

BOOL COptsDebugTab::ActivatePage(UINT nPage)
{
	COptionMiniPage * pPage = CreatePage(nPage);
	if (pPage == (COptionMiniPage *)NULL)	return FALSE;

	// initialise the contents of the page
	pPage->InitPage();

	// show the page visible
	pPage->ShowWindow(SW_SHOWNOACTIVATE);	
	pPage->EnableWindow(TRUE);

	// Help will correspond to the currently active page
 	m_nIDHelp = pPage->GetDlgID();

	return TRUE;
}

void COptsDebugTab::DeactivatePage(UINT nPage)
{
	COptionMiniPage * pPage = GetOptionPage(nPage);

	// hide this page
	pPage->EnableWindow(FALSE);
	pPage->ShowWindow(SW_HIDE);

	// terminate
	pPage->TermPage();
}

BOOL COptsDebugTab::SetCurrentMiniPage(CString & strTitle)
{
	// have we activated a page yet?
	int iSel;
	if (m_pcboMiniPage)
		if ((iSel = m_pcboMiniPage->FindStringExact(-1, strTitle)) == CB_ERR)
			return FALSE;
	
	if (m_nCurrPage == -1)
		m_strInitCategory = strTitle;	// prime before activation of tab
	else
		SelectPage(iSel); // select new page
	return TRUE;
}

BOOL COptsDebugTab::GetCurrentMiniPage(CString & strTitle)
{
	if (!m_pcboMiniPage)	return FALSE;

	int iSel;
	if ((iSel = m_pcboMiniPage->GetCurSel()) == CB_ERR)	return FALSE;

	// return the text of the current selection
	m_pcboMiniPage->GetLBText(iSel, strTitle);
	return TRUE;
}

COptionMiniPage * COptsDebugTab::CreatePage(UINT nPage)
{
	COptionMiniPage * pPage = GetOptionPage(nPage);

	// need to create?
	if (pPage->m_hWnd == (HWND)NULL)
	{
		// can we create?
		if (!pPage->Create(this))
			return (COptionMiniPage *)NULL;

		// show the page in the correct position
		// (we use the 'Options:' static text as the ref. point
		// the original 'Options:' static text plus edit-box
		// are moved down)
		CWnd * pWnd = GetDlgItem(IDC_MINI_PAGE); ASSERT(pWnd != (CWnd *)NULL);
		pPage->SetWindowPos(pWnd, 0, m_cyPageTop, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return pPage;
}

BOOL COptsDebugTab::ValidateTab()
{
	BOOL fValid;	// valid?

	// validate current COptionMiniPage
	COptionMiniPage * pPage = GetOptionPage(m_nCurrPage);

	if (!pPage->m_isValidating)
	{
		pPage->m_isValidating = TRUE;
		fValid = pPage->Validate();	// validate page
		pPage->m_isValidating = FALSE;	
	}
	else
		fValid = FALSE;	// can't do during validation

 	// reset 'to-do' validation if the we are valid
	if (fValid)
		pPage->m_nCurrentEditID = 0;

	return fValid;
}

BOOL COptsDebugTab::OnPropChange(UINT nProp)
{
	return CProjSlobPageTab::OnPropChange(nProp);
}

BEGIN_IDE_CONTROL_MAP(COptsDebugTab, IDDP_DEBUG_CATEGORY, IDS_DEBUG_OPTIONS)
END_IDE_CONTROL_MAP()

BEGIN_IDE_CONTROL_MAP(CDebugGeneralPage, IDDP_DEBUG_OPTIONS, IDS_DEBUG_OPTIONS)
	MAP_EDIT(IDC_CALLING_PROG, P_Caller)
	MAP_EDIT(IDC_PROG_WORKDIR, P_WorkingDirectory)
	MAP_EDIT(IDC_PROG_ARGS, P_Args)
	MAP_EDIT(IDC_REMOTE_TARGET, P_RemoteTarget)
END_IDE_CONTROL_MAP()

BEGIN_MESSAGE_MAP(CDebugGeneralPage, CDebugPageTab)
	//{{AFX_MSG_MAP(CDebugGeneralPage)
		ON_BN_CLICKED(IDM_BROWSE_FOR_DEBUG_EXE, OnDebugBrowse)
		ON_BN_CLICKED(IDM_USE_TESTCONTAINER, OnUseTestContainer)
		ON_BN_CLICKED(IDM_USE_WEBBROWSER, OnUseWebBrowser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_POPUP_MENU(ExeFinder)
	MENU_ITEM(IDM_BROWSE_FOR_DEBUG_EXE)
	MENU_ITEM(IDM_USE_TESTCONTAINER)
	MENU_ITEM(IDM_USE_WEBBROWSER)
END_POPUP_MENU()

BOOL CDebugGeneralPage::OnInitDialog()
{
	// call the base class
	CDebugPageTab::OnInitDialog();

	m_btnExeFinder.SubclassDlgItem(IDC_DEBUG_BROWSE, this);

	m_btnExeFinder.SetPopup(MENU_CONTEXT_POPUP(ExeFinder));
	return TRUE;
}

void CDebugGeneralPage::InitPage()
{
	COptionMiniPage::InitPage();

	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	ASSERT(((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CProject)));

	// If the project is an internal makefile then we can get the
	// platform by going through the project directly.
	// (Use the CProxySlob platform enumeration in case of multi-selection)

	BOOL bRemoteTarget = TRUE;	// multi-selection of items' platform supports remote targ.?
	BOOL bWorkingDir = TRUE;	// multi-selection of items' platform supports working dir?
	BOOL bProgArgs = TRUE;		// multi-selection of items' platform supports program arguments?
	BOOL bRemoteIsUNC = 2;		// ambiguous

	((CProxySlob *)m_pSlob)->InitPlatformEnum();
	const CPlatform * pPlatform;
	while ((pPlatform = ((CProxySlob *)m_pSlob)->NextPlatform()) != (const CPlatform *)NULL)
	{
		UINT uPlatform = pPlatform->GetUniqueId();

		// is this UNC and we alread have a non-UNC name?
		BOOL bRemoteIsUNC_T = uPlatform != mac68k && uPlatform != macppc;
		if (bRemoteIsUNC != 2 && bRemoteIsUNC_T != bRemoteIsUNC)
			bRemoteTarget = FALSE;	// don't enable the remote target for OS with different remote file specs.
		else
		{
			bRemoteIsUNC = bRemoteIsUNC_T;
			bRemoteTarget = bRemoteTarget && ((pPlatform->GetAttributes() & PIA_Supports_RemoteDbg) != 0);
		}
		bWorkingDir = bWorkingDir && ((pPlatform->GetAttributes() & PIA_Allow_WorkingDir) != 0);
		bProgArgs = bProgArgs && ((pPlatform->GetAttributes() & PIA_Allow_ProgArgs) != 0);
	}

	// FUTURE (matthewt): allow Reset... for external projects														   
	//GetDlgItem(IDC_SET_DEFAULT)->ShowWindow(g_pActiveProject->m_bProjIsExe ? SW_HIDE : SW_SHOW);

	// does this selection allow a working directory?
	GetDlgItem(IDC_PROG_WORKDIR_TXT)->EnableWindow(bWorkingDir);
	GetDlgItem(IDC_PROG_WORKDIR)->EnableWindow(bWorkingDir);

	// does this selction allow a remote executable target?
	GetDlgItem(IDC_REMOTE_TARGET_TXT)->EnableWindow(bRemoteTarget);
	GetDlgItem(IDC_REMOTE_TARGET)->EnableWindow(bRemoteTarget);

	// does this selection allow program arguments?
	GetDlgItem(IDC_PROG_ARGS_TXT)->EnableWindow(bProgArgs);
	GetDlgItem(IDC_PROG_ARGS)->EnableWindow(bProgArgs);

	// update the 'Reset...' state
	//UpdateState();
}

void CDebugGeneralPage::CommitPage()
{
	// No non-property info we need to store.  Do nothing.
}

BOOL CDebugGeneralPage::Validate()
{
	if (m_nValidateID == IDC_REMOTE_TARGET)
	{
		BOOL bMacTarget = FALSE;

		ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
		((CProxySlob *)m_pSlob)->InitPlatformEnum();
		const CPlatform * pPlatform;
		while ((pPlatform = ((CProxySlob *)m_pSlob)->NextPlatform()) != (const CPlatform *)NULL)
		{
			bMacTarget = bMacTarget || (pPlatform->GetUniqueId() == mac68k);
		}

		if (bMacTarget)
		{
			// We must force macintosh path and file name
			// validation on the field as we have at least
			// one selected macintosh target in the tree
			// control.
			CString strRemote;
			BOOL bValidated = FALSE;

			CWnd * pWnd = GetDlgItem(IDC_REMOTE_TARGET);
			ASSERT(pWnd != (CWnd *)NULL);
			pWnd->GetWindowText(strRemote);

			if (!strRemote.IsEmpty())
			{
				// Must have a colon but cannot be the
				// first character in the string as this
				// indicates we have a relative pathname.
				int nPos = strRemote.Find(_T(':'));
				if (nPos == -1)
				{
					// No colon - must specify a volume
					// name
					InformationBox(IDS_MUST_SPECIFY_VOLUME_NAME, strRemote);
				}
				if (nPos == 0)
				{
					// Colon as first character - no
					// partial pathnames allowed.
					InformationBox(IDS_NO_PARTIAL_PATHNAMES, strRemote);
				}
				if (nPos > 0)
					// Pathname ok.
					bValidated = TRUE;

				if (!bValidated)
				{
					pWnd = GetDlgItem(IDC_REMOTE_TARGET);
					pWnd->SetFocus();
					((CEdit *)pWnd)->SetSel(0, -1);
					return FALSE;
				}
			}
		}
	}

	return COptionMiniPage::Validate();
}


void CDebugGeneralPage::OnDebugBrowse() 
{
	// TODO: Add your control notification handler code here
	CDbgBrowseDlg fDlg;

	if(fDlg.DoModal() == IDOK)
	{
		CEdit* ceBrowse = (CEdit *) GetDlgItem(IDC_CALLING_PROG);
		ceBrowse->SetFocus();
		ceBrowse->SetWindowText(fDlg.GetFileName());
	}
}

void CDebugGeneralPage::OnUseWebBrowser()
{
	CEdit* ceBrowse = (CEdit *) GetDlgItem(IDC_CALLING_PROG);
	ceBrowse->SetFocus();
	ceBrowse->SetWindowText(CProject::s_strWebBrowser);
}

void CDebugGeneralPage::OnUseTestContainer()
{
	CEdit* ceBrowse = (CEdit *) GetDlgItem(IDC_CALLING_PROG);
	ceBrowse->SetFocus();
	ceBrowse->SetWindowText(CProject::s_strTestContainer);
}

BEGIN_IDE_CONTROL_MAP(CDebugAdditionalDllPage, IDDP_DEBUG_ADD_DLL, IDS_DEBUG_ADD_DLL)
	MAP_CHECK(IDC_LOAD_DLL_PROMPT, P_PromptForDlls)
END_IDE_CONTROL_MAP()

BEGIN_MESSAGE_MAP(CDebugAdditionalDllPage, CDebugPageTab)
	//{{AFX_MSG_MAP(CToolGeneralPage)
	ON_WM_DESTROY()
	ON_ELLIPSE(IDC_DLLS_LIST, OnBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CDebugAdditionalDllPage::OnInitDialog()
{
	CRect rc;
	HBLDTARGET hTarget;
	CString str;

	if ( !COptionMiniPage::OnInitDialog() ) {
		return FALSE;
	}

	// Get the correct location for the grid
	CWnd *pReplace = GetDlgItem(IDC_GRID_HOLDER);
	ASSERT(pReplace != NULL);

	// Init GridList
	g_BldSysIFace.InitTargetEnum();
	while ((hTarget = g_BldSysIFace.GetNextTarget(str)) != NO_TARGET)
	{
		GRIDINFO *pGI = new GRIDINFO;

		pGI->hTarget = hTarget;
		pGI->pGrid = new CDLLGridWnd;
		BOOL fRet = pGI->pGrid->ReplaceControl(this, IDC_GRID_HOLDER, IDC_DLLS_LIST,
			WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | WS_TABSTOP, 0, GRIDWND_TB_NEWDEL, FALSE);
		
		pGI->pGrid->AddRow(new CDLLGridRow(pGI->pGrid));

		pGI->pGrid->ShowWindow(SW_HIDE);
		m_GridList.AddTail(pGI);
	}

	// HACK - If the project file is an exe, we won't get any targets
	if (m_GridList.IsEmpty())
	{
		ASSERT( ((CProxySlob *)m_pSlob)->IsSingle() );

		GRIDINFO *pGI = new GRIDINFO;
		hTarget = g_BldSysIFace.GetTarget(
			((CProxySlob *)m_pSlob)->GetBaseConfig()->GetConfigurationName(), ACTIVE_BUILDER);

		pGI->hTarget = hTarget;
		pGI->pGrid = new CDLLGridWnd;
		
		BOOL fRet = pGI->pGrid->ReplaceControl(this, IDC_GRID_HOLDER, IDC_DLLS_LIST,
			WS_VISIBLE | WS_CHILD | WS_BORDER | WS_VSCROLL | WS_TABSTOP, 0, GRIDWND_TB_NEWDEL, FALSE);
		
		pGI->pGrid->AddRow(new CDLLGridRow(pGI->pGrid));

		pGI->pGrid->ShowWindow(SW_HIDE);
		m_GridList.AddTail(pGI);
	}

	m_pGridActive = ((GRIDINFO *)m_GridList.GetHead())->pGrid;

	// Load info from src package (which got it from the vcp file)
	InitGrids();

	// and kill the holder window
	pReplace->DestroyWindow();

	return TRUE;
}

void CDebugAdditionalDllPage::OnDestroy()
{
	while (!m_GridList.IsEmpty())
	{
		GRIDINFO *pGI = (GRIDINFO *)m_GridList.RemoveHead();
		
		pGI->pGrid->DestroyWindow();
		delete pGI->pGrid;
		delete pGI;
	}
}

void CDebugAdditionalDllPage::TermPage()
{
	//billjoy - validate grid info	

	COptionMiniPage::TermPage();
}

void CDebugAdditionalDllPage::InitPage()
{
	BOOL fRemoteTL = FALSE;
	BOOL fSingleTarget;
	int cx;
	CRect Rect;

	COptionMiniPage::InitPage();

	// Hide the old grid
	m_pGridActive->ShowWindow(SW_HIDE);

	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	fSingleTarget = ((CProxySlob *)m_pSlob)->IsSingle();
	if (fSingleTarget)
	{
		// Activate this target's grid
		HBLDTARGET hTarget = g_BldSysIFace.GetTarget(
			((CProxySlob *)m_pSlob)->GetBaseConfig()->GetConfigurationName(), ACTIVE_BUILDER);
		POSITION pos = m_GridList.GetHeadPosition();
		while (pos != NULL)
		{
			GRIDINFO *pGI = (GRIDINFO *)m_GridList.GetNext(pos);
			if (pGI->hTarget == hTarget) {
				m_pGridActive = pGI->pGrid;
				break;
			}
		}
	}
	else 
	{
		m_pGridActive = ((GRIDINFO *)m_GridList.GetHead())->pGrid;
	}

	// Show the new grid
	m_pGridActive->ShowWindow(SW_SHOW);

	m_pGridActive->EnableWindow(fSingleTarget);
	m_pGridActive->m_fEnabled = fSingleTarget;


#if 01 // The columns should have already been sized in OnInitDialog
	// Format the grid.  Only show the Remote Name column if the current TL of 
	// the selected platform is a remote TL

	if (fSingleTarget) 
	{
		((CProxySlob *)m_pSlob)->InitPlatformEnum();
		const CPlatform * pPlatform;
		pPlatform = ((CProxySlob *)m_pSlob)->NextPlatform();
		ASSERT(pPlatform);

		int i = pPlatform->GetCurrentTLIndex();
		fRemoteTL = !(pPlatform->GetTLFlags(i) & TFL_LOCAL);
	}
	
	m_pGridActive->GetCaptionRow()->GetRect(Rect);
	cx = Rect.Width() - m_pGridActive->GetColumnWidth(COL_PRELOAD);
	if (fRemoteTL) {
		m_pGridActive->SetColumnWidth(COL_LOCALNAME, cx/2);
		cx -= cx/2;
		m_pGridActive->SetColumnWidth(COL_REMOTENAME, cx);
	} else {
		m_pGridActive->SetColumnWidth(COL_LOCALNAME, cx);
		m_pGridActive->SetColumnWidth(COL_REMOTENAME, 0);
	}
	m_pGridActive->m_fShowRemoteCol = fRemoteTL;
#endif

	m_pGridActive->ResetSize();
	m_pGridActive->Invalidate(FALSE); // INEFFICIENT
	m_pGridActive->ResetScrollBars();

	ASSERT(((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CProject)));

	GetDlgItem(IDC_LOAD_DLL_PROMPT)->EnableWindow(TRUE);

	// Select the first row
	VERIFY(m_pGridActive->SetCurSel(0));

}


void CDebugAdditionalDllPage::CommitPage()
{
	SaveGrids();
}

void CDebugAdditionalDllPage::InitGrids()
{
	if (g_VPROJIdeInterface.Initialize()) 	// make sure we are initialised!
	{
		POSITION pos = m_GridList.GetHeadPosition();
		while (pos != NULL)
		{
			GRIDINFO *pGI = (GRIDINFO *)m_GridList.GetNext(pos);

			// Add blank rows to the grid for the src package to fill in.			
			int cRows;
			g_VPROJIdeInterface.GetDLLInfo()->GetDLLCount((ULONG)pGI->hTarget, &cRows);
			for (int i=0; i < cRows; i++)
			{
				pGI->pGrid->AddRow(new CDLLGridRow(pGI->pGrid));
			}

			CPtrList listDLLs;
			// get the DLL info...
			g_VPROJIdeInterface.GetDLLInfo()->InitDLLList((ULONG)pGI->hTarget, listDLLs);

			int iRow = 0;
			POSITION pos = listDLLs.GetHeadPosition();
			while (pos != NULL)
			{
				DLLREC *pDLLRec = (DLLREC *)listDLLs.GetNext(pos);
				// The last row will always be a new row
				CDLLGridRow *pRow = (CDLLGridRow *)pGI->pGrid->GetRow(iRow++);
				pRow->m_fPreload		= pDLLRec->fPreload;
				pRow->m_strLocalName	= pDLLRec->strLocalName;
				pRow->m_strRemoteName	= pDLLRec->strRemoteName;
			}
		}
	}

}

void CDebugAdditionalDllPage::SaveGrids()
{
	if (g_VPROJIdeInterface.Initialize()) 	// make sure we are initialised!
	{
		POSITION pos = m_GridList.GetHeadPosition();
		while (pos != NULL)
		{
			GRIDINFO *pGI = (GRIDINFO *)m_GridList.GetNext(pos);

			int cRows = pGI->pGrid->GetRowCount() - 1;  // don't count the new row
			CPtrList listDLLs;
			
			for (int i = 0; i < cRows; i++)
			{
				DLLREC *pDLLRec = new DLLREC;
				CDLLGridRow *pRow = pGI->pGrid->GetRow(i);
				pDLLRec->fPreload = pRow->m_fPreload;
				pDLLRec->strLocalName = pRow->m_strLocalName;
				pDLLRec->strRemoteName = pRow->m_strRemoteName;
				pDLLRec->fDelete = pRow->m_fDelete;
				listDLLs.AddTail(pDLLRec);
			}

			g_VPROJIdeInterface.GetDLLInfo()->SaveDLLList((ULONG)pGI->hTarget, listDLLs);

			while (!listDLLs.IsEmpty())
			{
				delete listDLLs.RemoveHead();
			}
		}
	}
}

BOOL CDebugAdditionalDllPage::Validate()
{
#if 0
		// only do this if the current config. is in the selection
		ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
		POSITION pos = ((CProxySlob *)m_pSlob)->GetHeadPosition();
		while (pos != (POSITION)NULL)
		{
			OptTreeNode * pNode = ((CProxySlob *)m_pSlob)->GetNext(pos);

			if (pNode->pItem->GetActiveConfig() == pNode->pcr)
			{
#endif
				if (!m_pGridActive->AcceptControl())
					return FALSE;

				UINT idMsg;
				if (!Validate_UpdateAdditionalDLLs(idMsg))
					return FALSE;

				if (idMsg != (UINT)-1)
				{
					// set this as an exit message
					// parent is COptsDebugTab?
				 	CWnd * pWndParent = GetParent();
					ASSERT(pWndParent != NULL);
					ASSERT(pWndParent->IsKindOf(RUNTIME_CLASS(COptsDebugTab)));

				 	pWndParent = pWndParent->GetParent();
					ASSERT(pWndParent != NULL);
					ASSERT(pWndParent->IsKindOf(RUNTIME_CLASS(CProjOptionsDlg)));

					((CProjOptionsDlg *)pWndParent)->SetOnOKMessage(idMsg);
				}
#if 0
				break;	// we're ok, don't look at anymore configs.
			}
		}
	}
#endif //0
	return COptionMiniPage::Validate();
}

BOOL CDebugAdditionalDllPage::Validate_UpdateAdditionalDLLs(UINT & idMsg)
{
	//int iRow;

	// Default is no message
	idMsg = (UINT)-1;

	// If multiple targets selected, then the grid is inactive so just return.
	// (besides, w/ multiple targets selected, GetBaseConfig will crash 8-)
	if ( !((CProxySlob *)m_pSlob)->IsSingle() )	
	{
		return TRUE;
	}

	HBLDTARGET hTarget = g_BldSysIFace.GetTarget(
		((CProxySlob *)m_pSlob)->GetBaseConfig()->GetConfigurationName(), ACTIVE_BUILDER);
#if 0	// FUTURE - we probably should verify the local DLLs in the grid, but
		// there's a problem in that FFindDLL doesn't know which target is selected,
		// so it doesn't know which platform to match...
	int cRows = m_pGridActive->GetRowCount() - 1;  // don't count the new row
	CPtrList listDLLs;
	
	for (int i = 0; i < cRows; i++)
	{
		DLLREC *pDLLRec = new DLLREC;
		CDLLGridRow *pRow = m_pGridActive->GetRow(i);
		pDLLRec->fPreload = pRow->m_fPreload;
		pDLLRec->strLocalName = pRow->m_strLocalName;
		pDLLRec->strRemoteName = pRow->m_strRemoteName;
		pDLLRec->fDelete = pRow->fDelete;
		listDLLs.AddTail(pDLLRec);
	}

	if (g_VPROJIdeInterface.Initialize() &&	// make sure we are initialised!
		g_VPROJIdeInterface.GetDLLInfo()->DiffDlls(hTarget, listDLLs, &iRow, &idMsg) != S_OK)
	{
		// make sure the edit has the focus, and then select the offending text
		m_pGridActive->SetFocus();
		m_pGridActive->Select(m_pGridActive->GetRow(iRow));
		while (!listDLLs.IsEmpty())
		{
			delete listDLLs.RemoveHead();
		}
		return FALSE;
	}
	while (!listDLLs.IsEmpty())
	{
		delete listDLLs.RemoveHead();
	}
#endif
	return TRUE;
}

void CDebugAdditionalDllPage::OnBrowse()
{
	CString strLocalDLL;
	CString strFilter, strT;
	CString strTitle;
	CFileDialog dlg(TRUE);

	VERIFY(strTitle.LoadString (IDS_MODULE_BROWSE_TITLE));
	dlg.m_ofn.lpstrTitle = strTitle;

	dlg.m_ofn.Flags |= OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NONETWORKBUTTON;
	
	VERIFY(strT.LoadString(IDS_FILTER_DLLS));
	AppendFilterSuffix(strFilter, dlg.m_ofn, strT);
	VERIFY(strT.LoadString(IDS_FILTER_OCXS));
	AppendFilterSuffix(strFilter, dlg.m_ofn, strT);
	VERIFY(strT.LoadString(IDS_FILTER_ALL));
	AppendFilterSuffix(strFilter, dlg.m_ofn, strT);

	dlg.m_ofn.lpstrFilter = strFilter;
	dlg.m_ofn.nFilterIndex = 1;

	for ( ; ; ) {
		if (dlg.DoModal() == IDCANCEL) {
			return;
		}
		strLocalDLL = dlg.GetPathName();
		CEdit* pEdit = (CEdit*)m_pGridActive->GetDlgItem(1);

		ASSERT(pEdit != NULL);
		pEdit->SetWindowText(strLocalDLL);
		pEdit->SetSel(0, -1);
		pEdit->SetFocus();
		return;
#if 0
		if (CheckEXEForDebug(strLocalDLL, TRUE)) {
			((CEdit *)GetDlgItem(IDC_REMOTE_MAP_LOCAL_NAME))->SetWindowText(strLocalDLL);
			return;
		}
#endif
	}
}

BOOL CDebugPageTab::Validate(void)
{
	// FUTURE - don't need to override if we aren't doing anything
	return COptionMiniPage::Validate();
}

#ifdef PRELINKSTEP
///////////////////////////////////////////////////////////////////////////////
// Tool General property page - shown in the Project.Tool Options 'mondo' dialog
BEGIN_IDE_CONTROL_MAP(CPreLinkPage, IDDP_PRELINK_OPTIONS, IDS_PRELINK)
	MAP_EDIT(IDC_PRELINK_DESC, P_PreLink_Description)

	// !!! IDC_PRELINK_CMDS and IDC_POSTBUILD_CMDS are not really edit controls.
	// CPreLinkPage or CPostBuildPage::Validate() will intercept these controls.
	{ IDC_PRELINK_CMDS, IDC_PRELINK_CMDS, P_PreLink_Command, (CControlMap::CTL_TYPE)100, 0x0, NULL },
END_IDE_CONTROL_MAP()

BEGIN_MESSAGE_MAP(CPreLinkPage, CProjSlobPageTab)
	//{{AFX_MSG_MAP(CPreLinkPage)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_IDE_CONTROL_MAP(CPostBuildPage, IDDP_POSTBUILD_OPTIONS, IDS_POSTBUILD)
	MAP_EDIT(IDC_POSTBUILD_DESC, P_PostBuild_Description)

	// !!! IDC_PRELINK_CMDS and IDC_POSTBUILD_CMDS are not really edit controls.
	// CPreLinkPage or CPostBuildPage::Validate() will intercept these controls.
	{ IDC_POSTBUILD_CMDS, IDC_POSTBUILD_CMDS, P_PostBuild_Command, (CControlMap::CTL_TYPE)100, 0x0, NULL },
END_IDE_CONTROL_MAP()

BEGIN_MESSAGE_MAP(CPostBuildPage, CProjSlobPageTab)
	//{{AFX_MSG_MAP(CPostBuildPage)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static BOOL m_bPreLinkPostBuildPageIgnoreChange = FALSE;

int CPreLinkPage::OnCreate(LPCREATESTRUCT lpcs)
{
	// can we create the window?
	if (CProjSlobPageTab::OnCreate(lpcs) == -1)
		return -1;	// no

	return 0;	// ok
}

BOOL CPreLinkPage::OnInitDialog()
{
	// call the base-class
	if (!CProjSlobPageTab::OnInitDialog())
		return FALSE;

	if (GetSlob()->GetStrProp(P_PreLink_Command, m_strPreLinkCmds) != valid)
		m_strPreLinkCmds.Empty();

	// create our grid controls	
	if (!m_PreLinkCmds.ReplaceControl(this,
								   IDC_PRELINK_CMDS, IDC_PRELINK_CMDS,
								   WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP,
								   WS_EX_CLIENTEDGE, GRIDWND_TB_ALL) )
		return FALSE;	// failure

	m_PreLinkCmds.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	m_PreLinkCmds.ShowWindow(SW_SHOW);	
	m_PreLinkCmds.EnableWindow(TRUE);

#if 0
	// load out menu buttons
	if (!m_btnDirMacro.SubclassDlgItem(IDC_DIR_MACRO, this))
		return FALSE;	// not ok

	if (!m_btnFileMacro.SubclassDlgItem(IDC_FILE_MACRO, this))
		return FALSE;	// not ok

	m_btnDirMacro.SetPopup(MENU_CONTEXT_POPUP(ToolDirMacro));
	m_btnFileMacro.SetPopup(MENU_CONTEXT_POPUP(ToolFileMacro));

	// disable the directories/files buttons
	m_btnDirMacro.EnableWindow(FALSE);
	m_btnFileMacro.EnableWindow(FALSE);

	// no grid has the last focus
	m_pgridLastFocus = (CStringListGridWnd *)NULL;
#endif

	return TRUE;	// ok
}

void CPreLinkPage::InitPage()
{
	CProjSlobPageTab::InitPage();
#if 0

	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	BOOL fIsProject = ((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CProject));

	CString strCurVal;
	if (m_pSlob->GetStrProp(P_PreLink_Command, strCurVal) == valid)
	{
		SetStrFromGrid((CStringListGridWnd*)GetDlgItem(IDC_PRELINK_CMDS), strCurVal);
	}
#endif

	// state of 'Reset...'
	UpdateState();
}

void CPreLinkPage::UpdateState()
{
	// disable if it doesn't have any props of ours for children
	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));

	GetDlgItem(IDC_PRELINK_DESC)->EnableWindow(TRUE);

	m_PreLinkCmds.EnableWindow(TRUE);
	m_PreLinkCmds.GetToolBar()->EnableWindow(TRUE);
}

BOOL CPreLinkPage::OnPropChange(UINT nProp)
{
	BOOL bChanged = TRUE;
	CString str;

	if ( nProp != P_PreLink_Command )
	{
		return CProjSlobPageTab::OnPropChange(nProp);
	}

	CControlMap* pCtlMap = FindProp(nProp);
	CWnd* pWndCtl = GetDlgItem(pCtlMap->m_nCtlID);

	GPT gpt = m_pSlob->GetStrProp(pCtlMap->m_nProp, str);

	CString strWindow;
	GetStrFromGrid((CStringListGridWnd*)pWndCtl,strWindow);

	if (gpt == invalid || gpt == ambiguous)
		str = "";	// invalid or indeterminate

	if (str != strWindow)
	{
		m_bPreLinkPostBuildPageIgnoreChange = TRUE;

		SetStrFromGrid((CStringListGridWnd*)pWndCtl,str);

		m_bPreLinkPostBuildPageIgnoreChange = FALSE;
	}
	else
	{
		bChanged = FALSE;
	}
	EnableControl(pWndCtl, valid);
//	EnableControl(pPromptWnd, gpt);

	UpdateState();
	return bChanged;
}

BOOL CPreLinkPage::Validate()
{
#if 1
	if (m_nValidateID == NULL)
		return TRUE;

	ASSERT(m_pSlob != NULL);

	CControlMap* pCtlMap = FindControl(m_nValidateID);
	ASSERT(pCtlMap != NULL);

	// Handle are special controls.
	switch (pCtlMap->m_nCtlID)
	{
	case IDC_PRELINK_CMDS:
		{
			BOOL bValid = TRUE;
			CWnd* pCtl = GetDlgItem(pCtlMap->m_nCtlID);
			ASSERT(pCtl != NULL);

			if (!((CStringListGridWnd*)pCtl)->AcceptControl())
				return FALSE;
	
			CString strBuf;
			GetStrFromGrid((CStringListGridWnd*)pCtl,strBuf);

			CString strCurVal;
			if (m_pSlob->GetStrProp(pCtlMap->m_nProp, strCurVal) == valid)
			{
 				if (strCurVal == strBuf)
				{
					m_nValidateID = NULL;
					return TRUE;
				}
			}
			CMultiSlob* pSelection = NULL;
			CSlobWnd* pSlobWnd = m_pSlob->GetSlobWnd();
			if (pSlobWnd != NULL)
				pSelection = (CMultiSlob*)pSlobWnd->GetSelection();
			if (pSelection != NULL && !pSelection->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
				pSelection = NULL;

			//theUndoSlob.BeginUndo(IDS_UNDO_PROPEDIT);
			bValid = m_pSlob->SetStrProp(pCtlMap->m_nProp, strBuf);
			//theUndoSlob.EndUndo();

			OnPropChange(pCtlMap->m_nProp);

			if (bValid)
				m_nValidateID = NULL;
			else
				pCtl->SetFocus();

			return bValid;
		}
		break;

	default:
		return CProjSlobPageTab::Validate();
		break;
	}
	ASSERT(0);
	return FALSE;

#else
    if (CProjSlobPageTab::Validate())
	{
		UpdateData(TRUE);
		GetSlob()->SetStrProp(P_PreLink_Command, m_strPreLinkCmds);
		return TRUE;
	}
	return FALSE;
#endif
}

BOOL CPreLinkPage::OnCommand(UINT wParam, LONG lParam)
{
	CControlMap* pCtlMap = FindControl(LOWORD(wParam));
	CString strBuf;
	BOOL bString = FALSE;
	BOOL bValidateReq = FALSE;

	if (m_pSlob == NULL || pCtlMap == NULL ||
		(m_nValidateID && LOWORD(wParam) != ((UINT) m_nValidateID) &&
		pCtlMap->m_nCtlType != CControlMap::check &&
		pCtlMap->m_nCtlType != CControlMap::radio))
	{
		// If the control is not found in the map, just pass this
		// message on for default processing...
		// Or if we have a validate ID already, this message is from
		// another control.
		return CDlgTab::OnCommand(wParam, lParam);
	}

	HWND hWndCtl = (HWND)(lParam);
	UINT nNotify = HIWORD(wParam);

	ASSERT(m_pSlob != NULL);

	switch (pCtlMap->m_nCtlID)
	{
	case IDC_PRELINK_CMDS:
		if (nNotify == GLN_CHANGE
			&& !m_bPreLinkPostBuildPageIgnoreChange )
			m_nValidateID = pCtlMap->m_nCtlID;
		else if (nNotify == GLN_KILLFOCUS)
		{
			((CStringListGridWnd *)GetDlgItem(pCtlMap->m_nCtlID))->AcceptControl();
			bValidateReq = TRUE;
		}
		break;


	default:
		return CProjSlobPageTab::OnCommand(wParam, lParam);
		break;
	}


	if (bValidateReq)
	{
		PostMessage(WM_USER_VALIDATEREQ);
		return CDlgTab::OnCommand(wParam, lParam);
	}

	return TRUE;
}

int CPostBuildPage::OnCreate(LPCREATESTRUCT lpcs)
{
	// can we create the window?
	if (CProjSlobPageTab::OnCreate(lpcs) == -1)
		return -1;	// no

	return 0;	// ok
}

BOOL CPostBuildPage::OnInitDialog()
{
	// call the base-class
	if (!CProjSlobPageTab::OnInitDialog())
		return FALSE;

	if (GetSlob()->GetStrProp(P_PostBuild_Command, m_strPostBuildCmds) != valid)
		m_strPostBuildCmds.Empty();

	// create our grid controls	
	if (!m_PostBuildCmds.ReplaceControl(this,
									 IDC_POSTBUILD_CMDS, IDC_POSTBUILD_CMDS,
									 WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP,
									 WS_EX_CLIENTEDGE, GRIDWND_TB_ALL) )
		return FALSE;	// failure

	m_PostBuildCmds.ModifyStyleEx(0, WS_EX_CLIENTEDGE);

	m_PostBuildCmds.ShowWindow(SW_SHOW);	
	m_PostBuildCmds.EnableWindow(TRUE);

#if 0
	// load out menu buttons
	if (!m_btnDirMacro.SubclassDlgItem(IDC_DIR_MACRO, this))
		return FALSE;	// not ok

	if (!m_btnFileMacro.SubclassDlgItem(IDC_FILE_MACRO, this))
		return FALSE;	// not ok

	m_btnDirMacro.SetPopup(MENU_CONTEXT_POPUP(ToolDirMacro));
	m_btnFileMacro.SetPopup(MENU_CONTEXT_POPUP(ToolFileMacro));

	// disable the directories/files buttons
	m_btnDirMacro.EnableWindow(FALSE);
	m_btnFileMacro.EnableWindow(FALSE);

	// no grid has the last focus
	m_pgridLastFocus = (CStringListGridWnd *)NULL;
#endif

	return TRUE;	// ok
}

void CPostBuildPage::InitPage()
{
	CProjSlobPageTab::InitPage();
#if 0

	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));
	BOOL fIsProject = ((CProxySlob *)m_pSlob)->IsSortOf(RUNTIME_CLASS(CProject));

	CString strCurVal;
	if (m_pSlob->GetStrProp(P_PostBuild_Command, strCurVal) == valid)
	{
		SetStrFromGrid((CStringListGridWnd*)GetDlgItem(IDC_POSTBUILD_CMDS), strCurVal);
	}
#endif

	// state of 'Reset...'
	UpdateState();
}

void CPostBuildPage::UpdateState()
{
	// disable if it doesn't have any props of ours for children
	ASSERT(m_pSlob->IsKindOf(RUNTIME_CLASS(CProxySlob)));

	GetDlgItem(IDC_POSTBUILD_DESC)->EnableWindow(TRUE);

	m_PostBuildCmds.EnableWindow(TRUE);
	m_PostBuildCmds.GetToolBar()->EnableWindow(TRUE);
}

BOOL CPostBuildPage::OnPropChange(UINT nProp)
{
	BOOL bChanged = TRUE;
	CString str;

	if ( nProp != P_PostBuild_Command )
	{
		return CProjSlobPageTab::OnPropChange(nProp);
	}

	CControlMap* pCtlMap = FindProp(nProp);
	CWnd* pWndCtl = GetDlgItem(pCtlMap->m_nCtlID);

	GPT gpt = m_pSlob->GetStrProp(pCtlMap->m_nProp, str);

	CString strWindow;
	GetStrFromGrid((CStringListGridWnd*)pWndCtl,strWindow);

	if (gpt == invalid || gpt == ambiguous)
		str = "";	// invalid or indeterminate

	if (str != strWindow)
	{
		m_bPreLinkPostBuildPageIgnoreChange = TRUE;

		SetStrFromGrid((CStringListGridWnd*)pWndCtl,str);

		m_bPreLinkPostBuildPageIgnoreChange = FALSE;
	}
	else
	{
		bChanged = FALSE;
	}
	EnableControl(pWndCtl, valid);
//	EnableControl(pPromptWnd, gpt);

	UpdateState();
	return bChanged;
}

BOOL CPostBuildPage::Validate()
{
#if 1
	if (m_nValidateID == NULL)
		return TRUE;

	ASSERT(m_pSlob != NULL);

	CControlMap* pCtlMap = FindControl(m_nValidateID);
	ASSERT(pCtlMap != NULL);

	// Handle are special controls.
	switch (pCtlMap->m_nCtlID)
	{
	case IDC_POSTBUILD_CMDS:
		{
			BOOL bValid = TRUE;
			CWnd* pCtl = GetDlgItem(pCtlMap->m_nCtlID);
			ASSERT(pCtl != NULL);

			if (!((CStringListGridWnd*)pCtl)->AcceptControl())
				return FALSE;
	
			CString strBuf;
			GetStrFromGrid((CStringListGridWnd*)pCtl,strBuf);

			CString strCurVal;
			if (m_pSlob->GetStrProp(pCtlMap->m_nProp, strCurVal) == valid)
			{
 				if (strCurVal == strBuf)
				{
					m_nValidateID = NULL;
					return TRUE;
				}
			}
			CMultiSlob* pSelection = NULL;
			CSlobWnd* pSlobWnd = m_pSlob->GetSlobWnd();
			if (pSlobWnd != NULL)
				pSelection = (CMultiSlob*)pSlobWnd->GetSelection();
			if (pSelection != NULL && !pSelection->IsKindOf(RUNTIME_CLASS(CMultiSlob)))
				pSelection = NULL;

			//theUndoSlob.BeginUndo(IDS_UNDO_PROPEDIT);
			bValid = m_pSlob->SetStrProp(pCtlMap->m_nProp, strBuf);
			//theUndoSlob.EndUndo();

			OnPropChange(pCtlMap->m_nProp);

			if (bValid)
				m_nValidateID = NULL;
			else
				pCtl->SetFocus();

			return bValid;
		}
		break;

	default:
		return CProjSlobPageTab::Validate();
		break;
	}
	ASSERT(0);
	return FALSE;

#else
    if (CProjSlobPageTab::Validate())
	{
		UpdateData(TRUE);
		GetSlob()->SetStrProp(P_PostBuild_Command, m_strPostBuildCmds);
		return TRUE;
	}
	return FALSE;
#endif
}

BOOL CPostBuildPage::OnCommand(UINT wParam, LONG lParam)
{
	CControlMap* pCtlMap = FindControl(LOWORD(wParam));
	CString strBuf;
	BOOL bString = FALSE;
	BOOL bValidateReq = FALSE;

	if (m_pSlob == NULL || pCtlMap == NULL ||
		(m_nValidateID && LOWORD(wParam) != ((UINT) m_nValidateID) &&
		pCtlMap->m_nCtlType != CControlMap::check &&
		pCtlMap->m_nCtlType != CControlMap::radio))
	{
		// If the control is not found in the map, just pass this
		// message on for default processing...
		// Or if we have a validate ID already, this message is from
		// another control.
		return CDlgTab::OnCommand(wParam, lParam);
	}

	HWND hWndCtl = (HWND)(lParam);
	UINT nNotify = HIWORD(wParam);

	ASSERT(m_pSlob != NULL);

	switch (pCtlMap->m_nCtlID)
	{
	case IDC_POSTBUILD_CMDS:
		if (nNotify == GLN_CHANGE
			&& !m_bPreLinkPostBuildPageIgnoreChange )
			m_nValidateID = pCtlMap->m_nCtlID;
		else if (nNotify == GLN_KILLFOCUS)
		{
			((CStringListGridWnd *)GetDlgItem(pCtlMap->m_nCtlID))->AcceptControl();
			bValidateReq = TRUE;
		}
		break;


	default:
		return CProjSlobPageTab::OnCommand(wParam, lParam);
		break;
	}


	if (bValidateReq)
	{
		PostMessage(WM_USER_VALIDATEREQ);
		return CDlgTab::OnCommand(wParam, lParam);
	}

	return TRUE;
}

#endif //#ifdef PRELINKSTEP

//============================================================
// DEBUG-ONLY PROJECT INFORMATION PROP PAGE SUPPORT
//
// The following section of code supports the "project information"
// property pages.  To see these pages, you must have a DEBUG
// build and throw the /Y switch on the command line.
//============================================================

#ifdef _DEBUG

class CPropListEnum : public CEnum
{
private:
			int				m_nSizePrevList;
		// Number of entries in previous list returned.

			CEnumerator *	m_prevListReturned;
		// Keep track of the previous list returned, if any,
		// so we can free it next time we're asked for the list.

			void			FreePrevList();

public:
							CPropListEnum() { m_prevListReturned = NULL; }
							~CPropListEnum() { FreePrevList(); }
	virtual	CEnumerator *	GetList();
};

struct PROPLIST_ENTRY
{
	UINT	nPropId;
	TCHAR *	szPropName;
};

#define DEF_PROPLIST_ENTRY(prop)	{ prop, #prop }

PROPLIST_ENTRY g_PropList[] =
{
// -- general properties (from ..\include\projprop.h) --
	DEF_PROPLIST_ENTRY(P_ProjActiveConfiguration),
	DEF_PROPLIST_ENTRY(P_Caller),
	DEF_PROPLIST_ENTRY(P_Args),
	DEF_PROPLIST_ENTRY(P_PromptForDlls),
	DEF_PROPLIST_ENTRY(P_RemoteTarget),
	DEF_PROPLIST_ENTRY(P_ExtOpts_CmdLine),
	DEF_PROPLIST_ENTRY(P_ExtOpts_RebuildOpt),
	DEF_PROPLIST_ENTRY(P_ExtOpts_Targ),
	DEF_PROPLIST_ENTRY(P_ExtOpts_BscName),
	DEF_PROPLIST_ENTRY(P_ProjConfiguration),
	DEF_PROPLIST_ENTRY(P_ProjOriginalType),
	DEF_PROPLIST_ENTRY(P_OutDirs_Intermediate),
	DEF_PROPLIST_ENTRY(P_OutDirs_Target),
	DEF_PROPLIST_ENTRY(P_ItemExcludedFromBuild),
	DEF_PROPLIST_ENTRY(P_ProjUseMFC),
	DEF_PROPLIST_ENTRY(P_NoDeferredBscmake),
	DEF_PROPLIST_ENTRY(P_UseDebugLibs),
	DEF_PROPLIST_ENTRY(P_Proj_IgnoreExportLib),
	DEF_PROPLIST_ENTRY(P_ItemIgnoreDefaultTool),
};

TCHAR * GetPropString(UINT nPropId, CProp * pProp)
{
	int		i, cch;
	TCHAR	szNum[32];
	TCHAR *	sz;

	for (i=0 ; i<sizeof(g_PropList)/sizeof(PROPLIST_ENTRY) ; ++i)
	{
		if (g_PropList[i].nPropId == nPropId)
		{
			cch = _tcslen(g_PropList[i].szPropName);

			if (pProp->m_nType == integer)
			{
				sprintf(szNum,
						"\t%d (0x%08lX)",
						((CIntProp *)pProp)->m_nVal,
						((CIntProp *)pProp)->m_nVal);

				cch += _tcslen(szNum) + 1;

				sz = new TCHAR[cch];
				_tcscpy(sz, g_PropList[i].szPropName);
				_tcscat(sz, szNum);
			}
			else if (pProp->m_nType == string)
			{
				const TCHAR *	szVal;

				szVal = ((CStringProp *)pProp)->m_strVal;
				cch += _tcslen(szVal) + 2;

				sz = new TCHAR[cch];
				_tcscpy(sz, g_PropList[i].szPropName);
				_tcscat(sz, _T("\t"));
				_tcscat(sz, szVal);
			}
			else
			{
				cch += _tcslen(_T("<Unknown Prop Type>")) + 2;

				sz = new TCHAR[cch];
				_tcscpy(sz, g_PropList[i].szPropName);
				_tcscat(sz, _T("\t"));
				_tcscat(sz, _T("<Unknown Prop Type>"));
			}

			return sz;
		}
	}

	// Unknown Prop ID
	sprintf(szNum, "\t%d (0x%08lX)", nPropId, nPropId);
	cch = _tcslen(_T("<Unknown Prop ID>")) + _tcslen(szNum) + 1;
	sz = new TCHAR[cch];
	_tcscpy(sz, _T("<Unknown Prop ID>"));
	_tcscat(sz, szNum);
	return sz;
}

void CPropListEnum::FreePrevList()
{
	int		n, nTotal;
	TCHAR *	sz;

	if (m_prevListReturned == NULL)
		return;

	nTotal = m_nSizePrevList;

	for (n=0 ; n<nTotal ; ++n)
	{
		if ((sz = (TCHAR *)m_prevListReturned[n].szId) != NULL)
			delete [] sz;
	}

	delete [] m_prevListReturned;

	m_prevListReturned = NULL;
}

CEnumerator * CPropListEnum::GetList()
{
	int			n;
	C3dPropertyPage *	pPage;
	CProjItem *	pItem;
	CPropBag *	pBag;

	// Free previous list, if any
	FreePrevList();

	// Create the new list and populate it
	if	(((pPage = GetCurrentPropertyPage()) != NULL)
		&&
		((pItem = (CProjItem *)pPage->GetSlob()) != NULL))
	{
		int			nCount, nPropBag;
		POSITION	pos;
		UINT		nPropId;
		CProp *		pProp;
		CString		strTarget;

		ASSERT(pItem->IsKindOf(RUNTIME_CLASS(CProjItem)));

		VERIFY(pItem->GetStrProp(P_D_Target, strTarget) == valid);
		VERIFY(pItem->GetIntProp(P_D_PropBag, nPropBag) == valid);

		int idOldBag = pItem->UsePropertyBag(BaseBag);
		VERIFY ((pBag = pItem->GetPropBag(strTarget)) != NULL);
		(void) pItem->UsePropertyBag(idOldBag);

		nCount = pBag->GetPropCount();
		n = 0;
		m_prevListReturned = new CEnumerator[nCount + 1];

		for (pos=pBag->GetStartPosition() ; pos!=NULL ; )
		{
			pBag->GetNextAssoc(pos, nPropId, pProp);

			m_prevListReturned[n].szId = GetPropString(nPropId, pProp);
			m_prevListReturned[n].val = nPropId;

			++n;
		}
	}
	else
	{
		m_prevListReturned = new CEnumerator[1];
		n = 0;
	}

	// Mark end of list
	m_prevListReturned[n].szId = NULL;
	m_prevListReturned[n].val = 0;

	m_nSizePrevList = n;

	return m_prevListReturned;
}

CPropListEnum g_thePropListEnum;

CProjInfoPage g_ProjInfoPage;

BEGIN_IDE_CONTROL_MAP(CProjInfoPage, IDDP_PROJINFO, IDS_PROJINFO)
	MAP_COMBO_TEXT(IDC_D_TRG_LISTBOX, P_D_Target, g_theConfigEnum)
	MAP_LIST(IDC_D_PROPS_LISTBOX, P_D_PropList, g_thePropListEnum)
	MAP_THIN_TEXT(IDC_D_PROP_TYPE, P_D_PropType)
	MAP_EDIT(IDC_D_PROP_VALUE, P_D_PropValue)
	MAP_RADIO(IDC_D_DEFAULT_BAG, IDC_D_BASE_BAG, 0, P_D_PropBag)
END_IDE_CONTROL_MAP()

extern CString	g_D_strTargetCur;	// PROJITEM.CPP

void CProjInfoPage::InitPage()
{
	int		n, nPropBag, cxTabStops = 110;
	BOOL	bFound;
	CString	str;

	// Make sure our current target is valid
	VERIFY(GetSlob()->GetStrProp(P_D_Target, str) == valid);

	if	(
		(str.IsEmpty())
		||
		(((CProjItem *)GetSlob())->GetPropBag(str) == NULL)
		)
	{
		GetSlob()->GetStrProp(P_ProjActiveConfiguration, str);
		// Note that we DON'T want to do a SetStrProp() on P_D_Target
		// here since that will cause our page to be re-init'ed, which
		// we don't want right now.  Instead, just set the actual
		// string directly.
		g_D_strTargetCur = str;
	}

	// Now initialize the page.
	CProjPage::InitPage();

	// See if the current prop ID is valid -- if not, select the
	// first one in the (sorted) list.
	VERIFY(GetSlob()->GetIntProp(P_D_PropList, n) == valid);
	VERIFY(GetSlob()->GetIntProp(P_D_PropBag, nPropBag) == valid);

	CProjItem * pItem = (CProjItem *)GetSlob();

	int idOldBag = pItem->UsePropertyBag(BaseBag);
	bFound = pItem->GetPropBag(str)->FindProp(n) != NULL;
	(void)pItem->UsePropertyBag(idOldBag);

	if (!bFound)
	{
		// Get the data for the first item in the (sorted) list and make
		// this our current value for the prop P_D_PropList.
		if (((CListBox *)GetDlgItem(IDC_D_PROPS_LISTBOX))->GetCount() == 0)
		{
			GetSlob()->SetIntProp(P_D_PropList, -1);
		}
		else
		{
			GetSlob()->SetIntProp(	P_D_PropList,
									((CListBox *)GetDlgItem(IDC_D_PROPS_LISTBOX))->GetItemData(0));
		}
	}

	// Set tabstops for the main listbox.
	((CListBox *)GetDlgItem(IDC_D_PROPS_LISTBOX))->SetTabStops(cxTabStops);
}

#endif	// _DEBUG

//============================================================
// END OF DEBUG-ONLY PROJECT INFORMATION PROP PAGE SUPPORT
//============================================================

