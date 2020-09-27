///////////////////////////////////////////////////////////////////////////////
//  uogaldlg.h
//
//  Created by :            Date :
//      EnriqueP              10/11/94
//
//  Description :
//      Declaration of the UIOGalleryDlg class
//

#ifndef __UIOG_DLG_H__
#define __UIOG_DLG_H__

#include "..\sym\vproj.h"
#include "..\shl\ucommdlg.h"
#include "..\shl\wbutil.h"
#include "prjxprt.h"
#include "afxcmn.h"			// Win95 controls
#include "..\wrk\uiwrkspc.h"


#ifndef __UIWINDOW_H__
   #error include 'udialog.h' before including this file
#endif


///////////////////////////////////////////////////////////////////////////////
//  UIOGalleryDlg, UOGCustomDlg & UOGImportDlg   Symbols

#define ID_OG_TABCTRL			0x402	// Control that has all the Tabs in the main dialog
#define ID_OG_CUSTOMIZE			0x03F5  // Customize button in main dialog
#define ID_OG_INSERT			0x03F2	// Insert button in main dialog
#define ID_OG_LISTCTRL			0x0406	// List Control in OG Main dialog 
#define ID_OG_SUBDIALOG			0x0001	// Window that contains list control and tab control

#define ID_OG_IMPORT			0x0006  // Import button in Customize dialog

#define ID_OG_FILE_NAME			0x0480	// File Name edit box in Import Dialog
#define ID_OG_COPY_TO_GALLERY	0x0412	// Copy to Gallery check box in Import Dialog
#define ID_OG_IMPORT_IMPORT		0x0001	// Import button in Import Dialog

typedef enum {ROGD_REPLACE = 0, ROGD_RESTORE} ROGD_VAL;

///////////////////////////////////////////////////////////////////////////////
//  UIOGalleryDlg class

// BEGIN_CLASS_HELP
// ClassName: UIOGalleryDlg
// BaseClass: UIDialog
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UIOGalleryDlg : public UIFileDlg
{
   	UIWND_COPY_CTOR (UIOGalleryDlg, UIFileDlg) ;
public:
	virtual ~UIOGalleryDlg() { }

	inline CString ExpectedTitle(void) const
        { return  GetLocString(IDSS_OG_TITLE); }            // REVIEW:Title:  GetLocString(IDSS_OG_TITLE)

// Attributes
public:
	
	CListCtrl	m_OgletList;	// List control that contains components

// API'S
public:
	HWND Display(void);
	HWND Close(void);
	BOOL LookIn(LPCSTR szLocation);
	int Import( CString& strOglet, BOOL bCopyToGallery = FALSE);
	HWND Insert( CString& strOglet);
	HWND Insert( int nOglet);
	BOOL Insert(LPCSTR szComponent);

	int SelectComponent( CString& strOglet );
	int SelectComponent( int nOglet );
	BOOL SelectComponent(LPCSTR szComp);
	LPTSTR GetSelectedComponentText(void);

	CString	Description(void);

	int ActivateCategory( CString& strCategory );
	LPTSTR ActivateCategory( UINT nPage );
	HWND CreateCategory( CString& strCategory );

	int FindFileInGallery(CString ProjName, CString OgxName); 
	BOOL FindClassInClassView(CString ProjName, CString ClassName); 
	void AddToProject(CString FolderName, CString FileName, CString FullFileName); 
	void AddToGallery();
	BOOL DeleteFileInGallery(CString FolderName,CString FullFileName, CString OgxFileName); 

/*	BOOL VerifyInsert( OG_TEMPLATE ogTemplate);	*/

// Internal Functions
public:
	UINT GetPageCount(void);
	LPTSTR GetActivePageText(void);
	LPTSTR ActivateFirstPage(void);
	LPTSTR ActivateNextPage( BOOL bRight = TRUE);
	HWND GetTabHWnd(void);

// Other OG Functions
public:
	BOOL DeleteOGDatabase(CString& strPath);
	BOOL RenameOGDatabase(CString& strPath, CString& strNewName, ROGD_VAL nRestore = ROGD_REPLACE);
	LPTSTR GetGalleryDir(CString& strPath);

	BOOL VerifyFilesInDir(CStringList* pFileList, CString& strPath ); 
};

/**************************************************************/
//  UOGCustomDlg class  

class PRJ_CLASS UOGCustomDlg : public UIDialog
{
   	UIWND_COPY_CTOR (UOGCustomDlg, UIDialog) ;

// API'S
public:
	HWND Display(void);
	// HWND Close(void);  Do we need to overide it?

	inline CString ExpectedTitle(void) const
	{ return GetLocString(IDSS_OG_CUSTOM_TITLE); }		// REVIEW:Title:  GetLocString(IDSS_OG_CUSTOM_TITLE)

};

/**************************************************************/
//  UOGImportDlg class  

class PRJ_CLASS UOGImportDlg : public UIDialog
{
   	UIWND_COPY_CTOR (UOGImportDlg, UIDialog) ;

// API'S
public:
	HWND Display(void);
	// HWND Close(void);  Do we need to overide it?
	int SetName(CString& strName);
	int CopyToGallery( BOOL bCopyToGallery = TRUE);
	int Import(void);

	inline CString ExpectedTitle(void) const
	{ return "Import Component"; }		// REVIEW:Title:  GetLocString(IDSS_OG_IMPORT)

};

#endif //__UIOG_DLG_H__

