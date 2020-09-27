// bwizdlg.cpp : implementation file
//

#include "stdafx.h"
#include "bwizdlg.h"
#include "shell.h"	// szKeyRoot && szCompanyName strings(initexit.cpp)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//
// local constants
enum tag_IMAGE_INDEXES
{
   IMAGE_BUILDER=0,
   IMAGE_CATEGORY,
   NUM_IMAGES
};

static LPCTSTR s_szCLSID            = _T("CLSID");
static LPCTSTR s_szValueMetaCATID   = _T("META CATID");
static LPCTSTR s_szSubKeyMetaCATID  = _T("Type");
static LPCTSTR s_szSlash			= _T("\\");

CBWizEnumItem::CBWizEnumItem(LPCTSTR szCLSID, LPCTSTR szDesc)
{
	m_strCLSID		= szCLSID;
	m_strDesc		= szDesc;
}

CBWizEnumItem::CBWizEnumItem()
{
	m_strCLSID		= _T("");
	m_strDesc		= _T("");
}


/////////////////////////////////////////////////////////////////////////////
//
// locally scoped functions
//
static BOOL GetCategoryDescription(LPCTSTR szCategory, CString* pstrDescription)
{
   //validate params
   ASSERT(NULL != szCategory && NULL != pstrDescription);
   if(NULL == szCategory || NULL == pstrDescription)
      return FALSE;

   // Open HKEY_CLASSES_ROOT\Component Categories\Category
   CRegLite regCategory;
   CString  strKey;
   strKey =  CBuilderWizardManager::szSysCompCatKey;
   strKey += s_szSlash;
   strKey += szCategory;
   if(!regCategory.Open(HKEY_CLASSES_ROOT, strKey))
      return FALSE;

   // get the description based on LCID
   strKey.Empty();
   strKey.Format(TEXT("%X"), GetThreadLocale());
   if(!regCategory.GetValue(strKey, pstrDescription))
   {
      // Get US version
      strKey.Empty();
      strKey.Format(TEXT("%X"), MAKELCID(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), SORT_DEFAULT));
      if(!regCategory.GetValue(strKey, pstrDescription))
         return FALSE;
   }
   ASSERT(!pstrDescription->IsEmpty());
   return (!pstrDescription->IsEmpty());
}

static BOOL GetCLSIDDescription(LPCTSTR szCLSID, CString* pstrDescription)
{
   //validate params
   ASSERT(NULL != szCLSID && NULL != pstrDescription);
   if(NULL == szCLSID || NULL == pstrDescription)
      return FALSE;

   // Open HKEY_CLASSES_ROOT\CSLID\{clsid}
   CRegLite regCategory;
   CString  strKey;
   strKey = s_szCLSID;
   strKey += s_szSlash;
   strKey += szCLSID;
   if(!regCategory.Open(HKEY_CLASSES_ROOT, strKey))
      return FALSE;

   // read in "" value
   if(!regCategory.GetValue(TEXT(""), pstrDescription))
   {
      //REVIEW(cgomes): Get US Version if Foreign Version failed!!
      return FALSE;
   }
   //cleanup
   ASSERT(!pstrDescription->IsEmpty());
   return (!pstrDescription->IsEmpty());
}

static HICON GetDefaultIcon(LPCTSTR szCLSID)
{
	//validate params
	ASSERT(NULL != szCLSID);
	if(NULL == szCLSID)
		return FALSE;
	
	// Now extract the Icon 
	CString strKey = _T("CLSID\\");
	strKey += szCLSID;
	strKey += _T("\\DefaultIcon");
	// Open Default Icon Key
	HKEY hKey;
	if(ERROR_SUCCESS != ::RegOpenKeyEx(HKEY_CLASSES_ROOT, strKey, 0, KEY_READ, &hKey))
		return NULL;
								
	// Alloc mem for icon path, and read it in
	DWORD 	dwLength = MAX_PATH;
	DWORD	dwType   = REG_SZ;
	CString strIcon;
	LPTSTR 	pszIcon  = strIcon.GetBuffer(dwLength);
	DWORD 	regErr   = ::RegQueryValueEx(hKey, NULL, 0, &dwType, (LPBYTE)pszIcon, &dwLength);
	::RegCloseKey(hKey);
	hKey = NULL;
	if(regErr != ERROR_SUCCESS)
		return NULL;
	ASSERT(REG_SZ == dwType);
	ASSERT(dwLength);

	// Get Icon Index
	TCHAR *pch = _tcsrchr(pszIcon, _T(','));
	int nOffset = 0;
	if(pch)
	{
		pch[0] = _T('\0');
		nOffset = _ttoi(&pch[1]);
	}
	// Get small (16x16) icon
	HICON hIcon = NULL;
	::ExtractIconEx(pszIcon, nOffset, NULL, &hIcon, 1);
	ASSERT(hIcon != NULL);
	return hIcon;
}	

inline BOOL IsMetaCATID(CRegLite* pReg)
{
   ASSERT(NULL != pReg);
   return pReg->IsValueEqual(s_szSubKeyMetaCATID, s_szValueMetaCATID);
}

//UNDONE$: Move this func to bwizmgr.cpp
extern BOOL IsMetaCATID(HKEY hCATID)
{
   ASSERT(NULL != hCATID);
   if(NULL == hCATID)
      return FALSE;

   CRegLite regCATID;
   if(regCATID.SetHKEY(hCATID))
      return IsMetaCATID(&regCATID);

   return FALSE;
}

static BOOL IsCategory(LPCTSTR szCategory)
{
	ASSERT(NULL != szCategory);

	// Open HKEY_CLASSES_ROOT\Component Categories\Category
	CRegLite regCategory;
	CString  strKey;
	strKey =  CBuilderWizardManager::szSysCompCatKey;
	strKey += s_szSlash;
	strKey += szCategory;
	return regCategory.Open(HKEY_CLASSES_ROOT, strKey);
}


/////////////////////////////////////////////////////////////////////////////
//
// CRegLite implementation
//
CRegLite::CRegLite()
{
   m_hKey             = NULL;
   m_cbSubKeys        = 0;
   m_cbMaxSubKeyName  = 0;
   m_cbMaxClassName   = 0;
   m_cbValues         = 0;
   m_cbMaxValueName   = 0;
   m_cbMaxValueData   = 0;
   m_fFreeKey         = FALSE;
}

CRegLite::~CRegLite()
{
   if(m_fFreeKey && NULL != m_hKey)
      Close();
   m_hKey             = NULL;
   m_cbSubKeys        = 0;
   m_cbMaxSubKeyName  = 0;
   m_cbMaxClassName   = 0;
   m_cbValues         = 0;
   m_cbMaxValueName   = 0;
   m_cbMaxValueData   = 0;
}

BOOL  CRegLite::InitData()
{
   ASSERT(NULL != m_hKey);
   if(ERROR_SUCCESS != ::RegQueryInfoKey(m_hKey, NULL, NULL, NULL,
                           &m_cbSubKeys, &m_cbMaxSubKeyName,
                           &m_cbMaxClassName, &m_cbValues,
                           &m_cbMaxValueName, &m_cbMaxValueData, NULL, NULL))
   {
      ASSERT(FALSE);
      Close();
      return FALSE;
   }
   return TRUE;
}

BOOL  CRegLite::Open(HKEY hParent, LPCTSTR szSubKey)
{
   // validate params
   ASSERT(NULL != hParent && NULL != szSubKey);
   if(NULL == hParent || NULL == szSubKey)
      return FALSE;

   // close existing key, if any
   if(m_hKey)
      Close();

   if(ERROR_SUCCESS != ::RegOpenKeyEx(hParent, szSubKey, 0, KEY_READ, &m_hKey))
      return FALSE;
   ASSERT(NULL != m_hKey);

   // set flag to free the key
   m_fFreeKey = TRUE;

   // Init Reg Class
   return InitData();
}

void CRegLite::Close()
{
   // free the key only if flag is set, and key is valid
   if(m_fFreeKey && NULL != m_hKey)
      ::CloseHandle(m_hKey);

   m_hKey      = NULL;
   m_fFreeKey  = FALSE;
}

BOOL CRegLite::SetHKEY(HKEY hKey)
{
   //validate params
   ASSERT(NULL != hKey);
   if(NULL == hKey)
      return FALSE;

   // close existing key, if any
   ASSERT(NULL == m_hKey);
   if(NULL != m_hKey)
      Close();

   // Init Reg Class
   m_hKey = hKey;
   m_fFreeKey = FALSE; // Clear Flag: DONT FREE the KEY
   return InitData();
}

BOOL CRegLite::GetValue(LPCTSTR szSubKey, CString* pstrData)
{
   // validate params
   ASSERT(NULL != szSubKey && NULL != pstrData);
   if(NULL == szSubKey || NULL == pstrData)
      return FALSE;

   // check key
   ASSERT(NULL != m_hKey);
   if(NULL == m_hKey)
      return FALSE;

   // calc size & alloc buffer for data
   DWORD cbData = m_cbMaxValueData + sizeof(TCHAR);
   BYTE* pbData = new BYTE[ cbData ];

   //read value
   DWORD dwType = REG_SZ;
   if(ERROR_SUCCESS != ::RegQueryValueEx(m_hKey, szSubKey, NULL, &dwType,
                                            pbData, &cbData))
   {
      delete [] pbData;
      return FALSE;
   }
   //return data
   ASSERT((cbData <= m_cbMaxValueData + sizeof(TCHAR)) && dwType == REG_SZ);
   *pstrData = (LPTSTR) pbData;
   //cleanup
   delete []pbData;
   return TRUE;
}

BOOL CRegLite::IsValueEqual(LPCTSTR szSubKey, LPCTSTR szValue)
{
   ASSERT(NULL != szSubKey || NULL != szValue);
   if(NULL == szSubKey || NULL == szValue)
      return FALSE;

   // get the data to compare with
   CString strData;
   if(!GetValue(szSubKey, &strData))
      return FALSE;

   // return result of Comparing Data and value
   return (!strData.CompareNoCase(szValue));
}

/////////////////////////////////////////////////////////////////////////////
// CTreeLite implementation

CTreeLite::CTreeLite()
{
   m_hWnd         = NULL;
   m_cbMaxImages  = 0;
   m_fInitialized = FALSE;
}

CTreeLite::~CTreeLite()
{
   m_hWnd         = NULL;
   m_cbMaxImages  = 0;
   if(m_fInitialized)
      m_iml.DeleteImageList();
}

BOOL CTreeLite::SetTreeCtrl(HWND hwndDlg, int nIDDlgItem)
{
   ASSERT(NULL != hwndDlg);
   if(NULL == hwndDlg)
      return FALSE;
   
   // get hwnd of TreeCtrl   
   m_hWnd = ::GetDlgItem(hwndDlg, nIDDlgItem);
   ASSERT(NULL != m_hWnd);
   if(NULL == m_hWnd)
      return FALSE;
   
   // create image list
   if(!m_iml.Create(::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), 
                   TRUE, m_cbMaxImages, 0))
   {
      ASSERT(!TEXT("ImageList Create Failed"));
      m_hWnd = NULL;
      return FALSE;
   }
 
   // imagelist for tree ctrl
   TreeView_SetImageList(m_hWnd, m_iml.GetSafeHandle(), TVSIL_NORMAL);
 
   m_fInitialized = TRUE;
   return TRUE;
}

BOOL CTreeLite::AddIconImage(UINT uiResource)
{
	//LoadIcon does not require DestroyIcon in Win32
	//Load icon to be added to image list
	HICON hIcon = AfxGetApp()->LoadIcon(uiResource);
	ASSERT(NULL != hIcon);
	if(NULL == hIcon)
		return FALSE;
	//Add icon to image list	
	return (-1 != m_iml.Add(hIcon));
}

int CTreeLite::AddIconImage(HICON hIcon)
{
	ASSERT(NULL != hIcon);
	if(NULL == hIcon)
		return -1;
	//Add icon to image list	
	return m_iml.Add(hIcon);
}


int CTreeLite::GetImageCount()
{
   // validation
   ASSERT(m_fInitialized);
   if(m_fInitialized)
      return m_iml.GetImageCount();
   return 0;   
}

HTREEITEM CTreeLite::GetFirstVisible()
{
   // validation
   ASSERT(m_fInitialized);
   if(!m_fInitialized)
      return NULL;
      
   return TreeView_GetFirstVisible(m_hWnd);
}

HTREEITEM CTreeLite::GetNextVisible(HTREEITEM hti)
{
   // validation
   ASSERT(m_fInitialized && NULL != hti);
   if(!m_fInitialized || NULL == hti)
      return NULL;
   
   return TreeView_GetNextVisible(m_hWnd, hti);
}

LPARAM CTreeLite::GetItemData(HTREEITEM hti)
{
   // validation
   ASSERT(m_fInitialized && NULL != hti);
   if(!m_fInitialized || NULL == hti)
      return NULL;
      
   TV_ITEM  tvi;
   tvi.mask    = TVIF_PARAM;
   tvi.hItem   = hti;
   if(TreeView_GetItem(m_hWnd, &tvi))
      return tvi.lParam;
   return NULL;   
}

void CTreeLite::SelectItem(HTREEITEM hti)
{
   // validation
   ASSERT(m_fInitialized);
   if(m_fInitialized)
      TreeView_SelectItem(m_hWnd, hti);
}

HTREEITEM CTreeLite::GetRoot()
{
   // validation
   ASSERT(m_fInitialized);
   if(!m_fInitialized)
      return FALSE;
   
   return TreeView_GetRoot(m_hWnd);
}

HTREEITEM CTreeLite::GetSelectedItem()
{
   // validation
   ASSERT(m_fInitialized);
   if(!m_fInitialized)
      return FALSE;
   
   return TreeView_GetSelection(m_hWnd);
}

HTREEITEM CTreeLite::InsertItem(TV_INSERTSTRUCT* ptvis)
{
   // validation
   ASSERT(m_fInitialized);
   if(!m_fInitialized)
      return FALSE;

   return TreeView_InsertItem(m_hWnd, ptvis);
}

inline BOOL CTreeLite::Expand(HTREEITEM hti)
{
   // validation
   ASSERT(m_fInitialized && NULL != hti);
   if(!m_fInitialized || NULL == hti)
      return NULL;
   
   return TreeView_Expand(m_hWnd, hti, TVE_EXPAND);
}

/////////////////////////////////////////////////////////////////////////////
// CBldWizSelectDlg dialog

CBldWizSelectDlg::CBldWizSelectDlg(CWnd* pParent /*=NULL*/)
   : C3dDialog(CBldWizSelectDlg::IDD, pParent)
{
   //{{AFX_DATA_INIT(CBldWizSelectDlg)
   //}}AFX_DATA_INIT
   // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
   m_clsidSel  = CLSID_NULL;
   m_cBuilders = 0;
}

CBldWizSelectDlg::~CBldWizSelectDlg()
{
   return;
}

BOOL CBldWizSelectDlg::SetCATID(HKEY hKey)
{ 
   ASSERT(NULL != hKey);
   if(NULL == hKey)
      return FALSE;
   return m_regCATID.SetHKEY(hKey);
}

BOOL CBldWizSelectDlg::InitTreeCtrl(void)
{
   // Init TreeLite and add images
   m_tree.SetMaxImages(NUM_IMAGES);
   if(m_tree.SetTreeCtrl(m_hWnd, IDC_BWIZ_TREE) &&
      // IDI_BUILDER == IMAGE_BUILDER == 0 == First Image
      m_tree.AddIconImage(IDI_BUILDER) &&
      // IDI_CATEGORY == IMAGE_CATEGORY == 1 == Second Image
      m_tree.AddIconImage(IDI_CATEGORY))
   {
      ASSERT(NUM_IMAGES == m_tree.GetImageCount());
      return TRUE;
   }
   return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CBldWizSelectDlg message map

BEGIN_MESSAGE_MAP(CBldWizSelectDlg, C3dDialog)
   //{{AFX_MSG_MAP(CBldWizSelectDlg)
   ON_NOTIFY(TVN_SELCHANGED, IDC_BWIZ_TREE, OnSelchangedSeltree)
   ON_NOTIFY(NM_DBLCLK, IDC_BWIZ_TREE, OnDblclkSeltree)
   ON_NOTIFY(TVN_DELETEITEM, IDC_BWIZ_TREE, OnDeleteItemSeltree)
   //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBldWizSelectDlg message handlers

BOOL CBldWizSelectDlg::OnInitDialog()
{
	C3dDialog::OnInitDialog();

	///////////////////////////////////
	// must build this key ourselves.  we can not use theApp.GetRegistryKeyName()
	// as this builds a key that takes into account the /i switch.  since these
	// settings are in HKLM and /i is an HKCU setting, we would end up with 
	// a bogus HKLM path.
	// CString strDevCompCat = GetRegistryKeyName();
	// strDevCompCat += s_szSlash;
	// strDevCompCat += CBuilderWizardManager::szSysCompCatKey;
	CString strDevCompCat;
	strDevCompCat =  g_szKeyRoot;
	strDevCompCat += s_szSlash;
	strDevCompCat += g_szCompanyName;
	strDevCompCat += s_szSlash;
	strDevCompCat += theApp.GetExeString(DefaultRegKeyName);
	strDevCompCat += s_szSlash;
	strDevCompCat += CBuilderWizardManager::szSysCompCatKey;
	
	if(!m_regDevCompCat.Open(HKEY_LOCAL_MACHINE, strDevCompCat) || !InitTreeCtrl())
	{
		ASSERT(!_T("m_regDevCompCat.Open(HKEY_LOCAL_MACHINE, strDevCompCat)"));
		::AfxMessageBox(IDS_MISSINGWIZARD);
		EndDialog(-1);
		return TRUE;
	}

	// Add the items to the TreeCtrl   
	// Add Meta Category will do the right thing for Cateories and Meta Categories
	AddMetaCategory(&m_regCATID);

	//check if any builders added
	// If no builders being displayed then cancel the dialog
	if (0 == m_cBuilders)
	{
#if defined _DEBUG
		::AfxMessageBox(_T("CBldWizSelectDlg::OnInitDialog(No Builders found in Category)"));
#endif
		::AfxMessageBox(IDS_MISSINGWIZARD);
		EndDialog(-1);
		return TRUE;
	}

	// Select Root Item
	m_tree.SelectItem(m_tree.GetRoot());
   
	return TRUE;
}

void CBldWizSelectDlg::OnOK()
{
   LPTSTR      szCLSID;
   HTREEITEM   hItem = m_tree.GetSelectedItem();
   ASSERT(NULL != hItem);

   //get string CLSID from lParam, and convert to CLSID
   USES_CONVERSION;
   if(NULL != hItem && (szCLSID = (LPTSTR)m_tree.GetItemData(hItem)))
      ::CLSIDFromString(T2OLE(szCLSID), &m_clsidSel);

   //cleanup
   C3dDialog::OnOK();
}

void CBldWizSelectDlg::OnDblclkSeltree(NMHDR* pNMHDR, LRESULT* pResult)
{
	// validate parameters
	ASSERT(NULL != pNMHDR && NULL != pResult);
	if(NULL == pNMHDR || NULL == pResult)
		return;
	*pResult = 0; // allow expansion for Catid double click

	HTREEITEM hti = m_tree.GetSelectedItem();
	ASSERT(hti != NULL);
	if(hti == NULL)
		return;
		
	// Check if double clicked on a builder	
	if(m_tree.GetItemData(hti))
	{
		*pResult = 1; // dis-allow expansion
		// Simulate btnOK Click
		OnOK();
	}
}

void CBldWizSelectDlg::OnSelchangedSeltree(NMHDR* pNMHDR, LRESULT* pResult)
{
   // validate parameters
   ASSERT(NULL != pNMHDR && NULL != pResult);
   if(NULL == pNMHDR || NULL == pResult)
      return;
   *pResult = 0;
   
   // Enable OK button only when selecting a builder
   NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
   ASSERT(NULL != pNMTreeView->itemNew.hItem);
   ::EnableWindow(::GetDlgItem(m_hWnd, IDOK), pNMTreeView->itemNew.hItem && 
                        m_tree.GetItemData(pNMTreeView->itemNew.hItem));
}

void CBldWizSelectDlg::OnDeleteItemSeltree(NMHDR* pNMHDR, LRESULT* pResult)
{
   // validate parameters
   ASSERT(NULL != pNMHDR && NULL != pResult);
   if(NULL == pNMHDR || NULL == pResult)
      return;
   *pResult = 0;
   
   // Enable OK button only when selecting a builder
   NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
   ASSERT(NULL != pNMTreeView->itemOld.hItem || NULL != pNMTreeView->itemOld.lParam);
   if(pNMTreeView->itemOld.hItem && pNMTreeView->itemOld.lParam)
   {
      delete (LPTSTR)pNMTreeView->itemOld.lParam;
#if defined _DEBUG
      // decrecment # of builders, this should be 0 when finally done
      --m_cBuilders;
#endif
   }
}

HTREEITEM CBldWizSelectDlg::DisplayBuilder
(
   LPCTSTR        szCLSID,
   HTREEITEM      hParent
)
{
	ASSERT(NULL != szCLSID);

	CString strDescription;
	if(!GetCLSIDDescription(szCLSID, &strDescription))
	{
		TRACE("GetCLSIDDescription(%s) failed", szCLSID);
		return NULL;
	}
	
	// Get Default Icon & Add it to ImageList
	int  	nImageIndex = IMAGE_BUILDER;
	HICON	hIcon = GetDefaultIcon(szCLSID);
	if(hIcon != NULL)
	{
		// Add icon to ImageList for Treeview
		nImageIndex = m_tree.AddIconImage(hIcon);
		ASSERT(nImageIndex != -1);
		if(nImageIndex == -1)
			// Failed to add image, so set to Builder
			nImageIndex =  IMAGE_BUILDER;
	}
	
	// Taking advantage of the Default Behavior for Images.
	// By default item's image is the first image in the list
	// Our first image is IDI_BUILDER.
	TV_INSERTSTRUCT tvis;
	tvis.hParent		= hParent;
	tvis.hInsertAfter	= TVI_LAST;
	tvis.item.mask		= TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvis.item.pszText	= (LPTSTR)(LPCTSTR)strDescription;
	tvis.item.cchTextMax = strDescription.GetLength();
	tvis.item.lParam 	= (LPARAM)_tcsdup(szCLSID); // To be freed on Dialog exit
	tvis.item.iImage	= nImageIndex;
	tvis.item.iSelectedImage = nImageIndex;
	ASSERT(NULL != tvis.item.lParam);
	if(NULL == tvis.item.lParam)
		// out of memory
		return NULL;

	// Insert the Builder into dialog
	HTREEITEM hti = m_tree.InsertItem(&tvis);
	if(hti != NULL)
		//increment # of builders
		m_cBuilders++;

	return hti;
}

HTREEITEM CBldWizSelectDlg::DisplayCategory
(
	LPCTSTR		  szCategory,
	HTREEITEM		hParent
)
{
	ASSERT(szCategory != NULL);
	if(szCategory == NULL)
		return NULL;

	CString strDescription;
	if(!GetCategoryDescription(szCategory, &strDescription))
	{
		TRACE("GetCategoryDescription(%s) failed", szCategory);
		return NULL;
	}

	TV_INSERTSTRUCT tvis;
	tvis.hParent			= hParent;
	tvis.hInsertAfter	 = TVI_LAST;
	tvis.item.mask		 = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvis.item.pszText	 = (LPTSTR)(LPCTSTR)strDescription;
	tvis.item.cchTextMax = strDescription.GetLength();
	tvis.item.iImage			= IMAGE_CATEGORY;
	tvis.item.iSelectedImage = IMAGE_CATEGORY;
	tvis.item.lParam	  = NULL;
	return m_tree.InsertItem(&tvis);
}

BOOL CBldWizSelectDlg::AddCategory(CRegLite* pReg, HTREEITEM htiParent /*=TV_ROOT*/)
{
	ASSERT(pReg != NULL);
	if(pReg == NULL || pReg->GetCbMaxSubKeyName() == 0)
		return FALSE;

	DWORD       nUnused;
	TCHAR*      pszSubKeyName = new TCHAR[ pReg->GetCbMaxSubKeyName()+1 ];
	DWORD       cSubKeys = pReg->GetCSubKeys();
	ASSERT(cSubKeys);

	for(DWORD nCnt = 0; nCnt < cSubKeys; ++nCnt)
	{
		nUnused = pReg->GetCbMaxSubKeyName();
		// if we can't enum the key, or we got back the
		// default component key, don't do anything
		if(ERROR_SUCCESS != ::RegEnumKeyEx(pReg->GetHandle(), nCnt, pszSubKeyName,
		                                   &nUnused, NULL, NULL, NULL, NULL))
			continue;

		// skip Default Component
		if(!lstrcmp(pszSubKeyName, _T("Default Component")))
			continue;
		 
		// Display the builder
		DisplayBuilder(pszSubKeyName, htiParent);
	} // for

	//cleanup
	delete [] pszSubKeyName;
	return TRUE;
}

BOOL CBldWizSelectDlg::AddMetaCategory(CRegLite* pReg, HTREEITEM htiParent /*=TV_ROOT*/)
{
	ASSERT(pReg != NULL);
	if(pReg == NULL)
		return FALSE;

	DWORD       nUnused;
	DWORD       cSubKeys = pReg->GetCSubKeys();
	TCHAR*      pszSubKeyName = new TCHAR[ pReg->GetCbMaxSubKeyName()+1 ];
	HTREEITEM   hti;
	CRegLite    regCATID;

	//REVIEW(cgomes): Check for loop in nested CATID
	for(DWORD nCnt = 0; nCnt < cSubKeys; ++nCnt)
	{
		nUnused = pReg->GetCbMaxSubKeyName();
		// if we can't enum the key, or we got back the
		// default component key, don't do anything
		if(ERROR_SUCCESS != ::RegEnumKeyEx(pReg->GetHandle(), nCnt, pszSubKeyName,
		                                  &nUnused, NULL, NULL, NULL, NULL))
			continue;
		
		// open the sub key within the current META Category
		if(!regCATID.Open(pReg->GetHandle(), pszSubKeyName))
			continue;
			
		if(IsCategory(pszSubKeyName))
		{
			// Display Category
			hti = DisplayCategory(pszSubKeyName, htiParent);
			if(hti == NULL)
				continue;
		}
		else
		{
			// Found builder so display it
			DisplayBuilder(pszSubKeyName, htiParent);
			// Builder is a leaf, so go to next sub key
			continue;
		}
		
		// Display contents of category.
		// Add Meta Category will do the right thing for Cateories and Meta Categories
		AddMetaCategory(&regCATID, hti);

		//expand category   
		m_tree.Expand(hti);
   } // for

   delete [] pszSubKeyName;
   return TRUE;
}

