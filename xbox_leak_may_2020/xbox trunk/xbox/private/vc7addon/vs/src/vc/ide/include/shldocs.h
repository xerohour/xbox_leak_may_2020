/////////////////////////////////////////////////////////////////////////////
//	SHLDOCS.H
//		Document and view base classes for packages.

#ifndef __SHLDOCS_H__
#define __SHLDOCS_H__

struct FINDREPLACEDISK;

class CSlob;

#include "path.h"
#include "slob.h"

//#undef AFX_DATA
//#define AFX_DATA AFX_EXT_DATA

/////////////////////////////////////////////////////////////////////////////
//	Classes defined in this header file
//	class CMultiDocTemplate;
		class CPartTemplate;
//	class COleDocument;
		class CPartDoc;
//	class CView;
//		class CSlobWnd;
			class CPartView;
//	class CMDIChildWnd;
		class CPartFrame;

      
/////////////////////////////////////////////////////////////////////////////
//	CPartTemplate
//		Base doc template class for packages.

class CPartTemplate : public CMultiDocTemplate
{
	DECLARE_DYNAMIC(CPartTemplate)

public:
   // File New Tab Type Enumeration
   enum FileNewTabType
   { 
      newnone, 
      newfile, 
      newtemplate, 
      newproject, 
      newworkspace, 
      newother 
   };
   
// Constructors
public:
	CPartTemplate(UINT nIDResource, CRuntimeClass* pDocClass,
		CRuntimeClass* pViewClass, LPCTSTR szAutoType);
	~CPartTemplate();
	
	// setup for OLE containers
	void SetContainerInfo();
	
	REFCLSID GetTemplateClsid() { return m_clsid; }
	LPCTSTR GetTemplateAutomationType() { return m_strAutoType; }

	virtual Confidence MatchDocType(LPCTSTR pszPathName,
					CDocument*& rpDocMatch);

	// Save all the templates docs.  If bQuery is true, then ask
	// fisrt.  nFlags allows the filters
	BOOL SaveAll(BOOL bQuery = FALSE, UINT nFlags = 0);

	virtual CDocument* OpenDocumentFile(const TCHAR* pszPathName,
		BOOL bMakeVisible = TRUE);
	virtual void InitialUpdateFrame(CFrameWnd* pFrame, CDocument* pDoc,
		BOOL bMakeVisible = TRUE);
	virtual CFrameWnd* CreateNewFrame( CDocument* pDoc, CFrameWnd* pOther ); 
	virtual CView * CreateNewView (CDocument * pDoc, HWND hwndParent, int x, int y, int cx, int cy);
	virtual CDocument* OpenDocumentFile(const TCHAR* pszPathName,
		HWND hwndParent, int x, int y, int cx, int cy);

	// Override to validate path for file creation (called from File.New dialog)
	virtual BOOL CanCreateDocument(const TCHAR* pszPathName) { return(TRUE); }

   // GetTabType returns an enum that indicates which tab to go on.
   // default impl. returns newfile, iff GetDocString() returns valid string
   virtual FileNewTabType GetTabType();
	
// Attributes
public:
	enum FileNewTab { Files, Templates };
	FileNewTab	m_TabName;  // Tab name can be 'Files' or 'Templates', default is 'Files'

protected:
	CLSID		m_clsid;
	CString		m_strAutoType;
	UINT		m_nIconID;

	friend class CPartDoc;
};

#define FILTER_NONE        0x0000
#define FILTER_DEBUG       0x0001
#define FILTER_PROJECT     0x0002
#define FILTER_NON_PROJECT 0x0004
#define FILTER_NEW_FILE    0x0008
#define FILTER_PROJ_RC     0x0010
#define FILTER_DOCUMENT    0x0020

/////////////////////////////////////////////////////////////////////////////
//	CPartDoc
//		Base document class for packages.

class CPartDoc : public COleDocument
{
private:
	using COleDocument::OnSaveDocument;

public:
	CPartDoc();
	DECLARE_DYNCREATE(CPartDoc)

// Attributes
public:
	virtual BOOL IsReadOnly();
	virtual BOOL IsReadOnlyEx(BOOL bHandleError = TRUE);
	virtual void SetReadOnly(BOOL bReadOnly = TRUE);
	virtual BOOL IsReadOnlyOnDisk(void) const { return m_bReadOnlyOnDisk; }
	virtual void SetReadOnlyOnDisk(BOOL bReadOnlyOnDisk) { m_bReadOnlyOnDisk = bReadOnlyOnDisk; }
	
	void SetModifiedFlag(BOOL bModified = TRUE);
	virtual BOOL GetValidFilters(UINT** paIDS, UINT* pnInitial);

	virtual BOOL IsFiltered(UINT nFlags);
	virtual BOOL FDisplayTitle()	{return FALSE;}
	
// Operations
public:
	virtual void ActivateDoc();
	virtual BOOL AttemptCloseDoc();
	virtual BOOL SaveModified();

// Implementation
public:
	virtual ~CPartDoc();
	virtual BOOL DoSave(const TCHAR* pszPathName, BOOL bReplace = TRUE);
	virtual BOOL Replace(const TCHAR* pszPathName, UINT nSaveType = 0);
	virtual void Serialize(CArchive& ar);	// overridden for document i/o
	virtual CSlob* GetSlob();

#ifdef _DEBUG
	virtual	void AssertValid() const;
	virtual	void Dump(CDumpContext& dc) const;
#endif
public:
	// The shell needs to be able to do processing either before or after
	// a document is created, opened, saved, or closed. 
	// This is required mainly so the shell can fire Ole Automation events.
	// Therefore, packages should not override the MFC virtual functions
	// On*Document. Instead the packages should override the functions
	// *Document. 

	// Packages should not override any of the following functions!
	// I repeat don't override these functions!
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(const TCHAR* pszPathName);
	virtual BOOL OnSaveDocument(const TCHAR* pszPathName, UINT nSaveType = 0);
	// Call this to close the document. In its default implementation it will call PreCloseDocument and OnCloseDocument.
	virtual void OnCloseDocument();

protected:
	// Packages should override the following functions.
	virtual BOOL NewDocumentHook();
	virtual BOOL OpenDocumentHook(const TCHAR* pszPathName);
	virtual BOOL SaveDocumentHook(const TCHAR* pszPathName, UINT nSaveType = 0);
	virtual void CloseDocumentHook();
	
	// The following functions are called by the shell after the
	// shell calls the functions above which actually does the real work.
	// These functions main purpose in life is to fire automation events.
	// BTW, we can change them to virtuals at a later date if neccessary.
	BOOL PostNewDocument(BOOL bResult) ;
	BOOL PostOpenDocument(BOOL bResult) ;
	BOOL PostSaveDocument(BOOL bResult) ;
	virtual void PreCloseDocument(); 	// called before closure, while document is still valid

	// If derived classes need to bypass the shell's implementations and
	// get to MFC's implementation, they should call the following functions.
	BOOL ParentNewDocumentHook();
	BOOL ParentOpenDocumentHook(const TCHAR* pszPathName);
	BOOL ParentSaveDocumentHook(const TCHAR* pszPathName, UINT nSaveType = 0);
	void ParentCloseDocumentHook();

	virtual BOOL CanCloseFrame(CFrameWnd *pFrame);
	virtual void SetDocString(UINT nID) { m_nIDString = nID; }
	virtual LPCTSTR GetDefaultTitle()
					{ return m_strTitle; }

public:
	virtual BOOL GetDocString(CString& rString,
		enum CDocTemplate::DocStringIndex index) const;
	
	virtual LONG_PTR GetDocLong(int nVar);
	virtual LONG_PTR SetDocLong(int nVar, LONG_PTR lData);
	
protected:
	BOOL m_bReadOnly;
	BOOL m_bReadOnlyOnDisk;

	UINT m_nIDString;
	CPartTemplate* m_pProxyTemplate;

// DER - added afx_msg to following...
// Generated message map functions
protected:
	//{{AFX_MSG(CPartDoc)
	afx_msg void OnFileSaveAs();
	afx_msg void OnFileSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	static BOOL CanSaveAll();

private:
	static CPartDoc* s_pSavableDoc;
};

//	GetDocLong enum
//		The shell reserves values less than 1000 for GetDocLong.
//		Packages may define values >= 1000 to allow other packages
//		to set values within their documents.  Useful for doc
//		template proxying.

enum
{
	GDL_PROXYTEMPLATE						= 100,
	GDL_CMP_CANCLOSE_CALLBACK				= 1001
};


/////////////////////////////////////////////////////////////////////////////
//	CPartView view
//		View base class for package documents.
//		(For dockable windows, see below.)

class CPartView : public CSlobWnd
{
protected: // create from serialization only
	CPartView();
	DECLARE_DYNCREATE(CPartView)

// Attributes
public:
	CPartDoc* GetDocument();
	virtual CPartTemplate* GetDocTemplate();

    virtual CSlob* GetSelection() { return NULL; }
    virtual CSlob* GetEditSlob() { return NULL; }

	virtual BOOL IsDependent() { return FALSE; }
	virtual BYTE* GetInitData() { return NULL; }

	virtual DWORD GetHelpID() { return 0; }
	virtual BOOL GetHelpWord(CString& strHelp) { return FALSE; }
	virtual	BOOL SetDialogPos(BOOL fFirstTime, HWND hDlg);

	virtual void OnNewPalette()
		{ }

	virtual int GetAssociatedFiles(CStringArray & saFiles, BOOL bSelected = TRUE);

#ifndef NO_VBA
	virtual LPDISPATCH GetSelectionObject(LPDISPATCH pdisp = NULL);
#endif	// NO_VBA

// Operations
public:
	virtual ~CPartView();
	
	virtual BOOL CanClose();

	virtual void OnLoadWorkspace(); // called when loaded from workspace
	virtual void OnFindNext(BOOL* pbSuccess = NULL); // called for a find

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView,
					CView* pDeactiveView);
//TODO
//    virtual void RecalculateIME();

	// Find and Replace operations:
	// These should be overriden by the view.
	// FirstSearch means that this search will be the first
	// in a (possible) subsequent serie of searches. The view should
	// remember where it started so that it can stop searching when
	// all the document has been searched.
	// Return value: TRUE if the string is found,
	//      FALSE if the string is not found, or if SearchStyle is
	//		"ContinueSearch" and all the document has been searched once.
	virtual BOOL FindString(FINDREPLACEDISK *pfr);

	// MarkAll: marks all occurrences of the search string (for instance, 
	// the source editor puts a bookmark on each line)
	virtual BOOL MarkAll(FINDREPLACEDISK *pfr);
		
	// ReplaceString: If bReplaceAll is TRUE, the view should do that.
	// Otherwise, if the selection equals szFind, replace the text,
	// then call FindNext or FindPrev.
	virtual BOOL ReplaceString(FINDREPLACEDISK *pfr, BOOL bReplaceAll);

	// GetFindSelectionRect: Returns the rect (in screen coords) of the selection, 
	// so that the find dialog can move out of the way.
	virtual BOOL GetFindSelectionRect(CRect* prc);

	// GetFindString: Provides the string used to initialize the Find What field.
	// This is used in Edit.Find, Edit.Replace
	virtual BOOL GetFindString(CString& rstr);

	// GetFindReplaceFlags: Allows the view to disable find, replace, regular expressions, etc.
	virtual BOOL GetFindReplaceFlags(DWORD *pdwFlagsFind, DWORD *pdwFlagsReplace);

	// GetSelectionInterface: get the view's guid, and an interface for 
	// selection and state information.
	// REVIEW(davidga): I don't like the name.  How about GetStateInfoInterface
	// Note: bobz this will AddRef the piUnk
	virtual BOOL GetSelectionInterface(GUID* pguid, IUnknown** ppiUnk);

	// (bobz) for cases where we just want to identify the editor
	virtual BOOL GetEditorID(GUID* pguid)
	{
		IUnknown* piUnk = NULL;
		BOOL fRet = GetSelectionInterface(pguid, &piUnk);
		if (fRet)
		{
			VSASSERT(piUnk != NULL, "Selection interface is NULL!");
			if (piUnk != NULL)	// overkill but defensive - bobz
				piUnk->Release();
		}
		return fRet;
	}

//TODO
//protected:
//	CPacket* m_pPacket;

// Generated message map functions
protected:
	//{{AFX_MSG(CPartView)
	afx_msg void OnEditFind();
	afx_msg void OnEditFindPrev();
	afx_msg void OnEditFindNext();
	afx_msg void OnToggleFindCase();
	afx_msg void OnToggleFindRE(void);
	afx_msg void OnToggleFindWord(void);
	afx_msg void OnToggleSearchAllDocs(void);
	afx_msg void OnUpdateFindReplace(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToggleMatchCase(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToggleMatchWord(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToggleSearchAllDocs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateToggleMatchRegEx(CCmdUI* pCmdUI);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	friend class CPartTemplate;

	friend class CPartFrame;

protected:
	// Find helper function
	BOOL DoFind();
};

#ifndef _DEBUG	// debug version in partvw.cpp
inline CPartDoc* CPartView::GetDocument()
   { return (CPartDoc*) m_pDocument; }
#endif

// FindInString: This function will use the FINDREPLACEDISK 
// variable to figure out if it should be using Reg Expressions, Case
// sensitivity and whole word matching, and to know what to search for.
BOOL FindInString(FINDREPLACEDISK *pfr, const TCHAR *szBuf,
	BOOL backwardsSearch,
	/* in/out */ const char **pszStart, /* out */ ULONG *pcbSize);

/////////////////////////////////////////////////////////////////////////////
//	CPartFrame
//		MDI child frame base class for packages.

struct WORKSPACE_INFO
{
	CPartFrame* pWnd;
	BYTE* pInitData;
	BYTE* pCurData;
};



class CPartFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CPartFrame)

private:
	using CMDIChildWnd::OnUpdateFrameMenu;

// Static workspace variables.
public:
	static BOOL s_bLockFrame;

	static void LockFrame(BOOL bLock);
	static void LockFrame(HWND hwndAfter);
	static void SetInitData(BYTE* pData);

// Construction
public:
	CPartFrame();
	virtual ~CPartFrame() ;	

// Attributes.
public:
	HWND m_hwndView;
	virtual void ActivateFrame(int nCmdShow = -1);
	virtual void OnUpdateFrameMenu(BOOL bActive, CWnd* pActivateWnd, 
		HMENU hMenuAlt);

	virtual BOOL IsDependent();
	virtual BYTE* GetInitData();
	virtual CString GetWorkspaceMoniker(BOOL bRelative);
	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	virtual void OnNewPalette();

	// Closes this window
	void CloseFrame() ;

// Message handlers	
protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	//{{AFX_MSG(CPartFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnMDIActivate(BOOL bActivate, CWnd*, CWnd*);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg LRESULT  OnEnterSizeMove(WPARAM, LPARAM);
	afx_msg LRESULT OnExitSizeMove(WPARAM, LPARAM);
	afx_msg void OnMenuSelect( UINT nItemID, UINT nFlags, HMENU hSysMenu );
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// c_bFirstCtrlTab, MDINextWindow, PreTranslateMessage, and 
	// OnSysCommand are used to customize the way Ctrl+Tab switches 
	// between document windows.  The 'new' way works more like Alt+Tab 
	// in that alternative uses cause the top-most two windows to toggle 
	// positions, but continuous usage (without releasing the Ctrl key) 
	// cycles between all document windows.
	//
	// If bLinear is true, then it works like a normal next window command. This is what the window menu versions do,
	// and what would be useful for automation, etc.
	//
public:
	void MDINextWindow(BOOL bNext, BOOL bLinear=FALSE);

protected:
	static HWND s_hwndAfter;

	// DocObject support
	// We need these to group the activation process
	static BOOL s_bInActivation;
	static BOOL s_bInDocObjClose;

public:
	static CPartFrame *s_pLastActiveFrame;
};

// Helper for GetWindow cycling of MDI.
extern BOOL IsValidMDICycleMember(CWnd* pWnd);
// Helper function that Activates an ExistingDoc
extern CPartDoc* ActivateExistingDoc(LPCTSTR lpszFileName,	CDocTemplate* pTemplate);

//#undef AFX_DATA
//#define AFX_DATA NEAR

#endif // __SHLDOCS_H__
