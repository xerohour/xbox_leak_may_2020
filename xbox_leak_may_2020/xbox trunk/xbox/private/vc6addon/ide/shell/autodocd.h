// AutoDocD.h : header file
//
#ifndef __AutoDocD_H__
#define __AutoDocD_H__

#include <utilauto.h>

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

#define _DUAL_

class CPartDoc ;


/////////////////////////////////////////////////////////////////////////////
// CAutoDocumentDefault command target

class CAutoDocumentDefault : public CAutoObj
{
	DECLARE_DYNAMIC(CAutoDocumentDefault)

	CAutoDocumentDefault(CPartDoc* pDoc);           // protected constructor used by dynamic creation

// Attributes
public:
	CPartDoc* m_pDoc;
	void NoDoc() ;

// Operations
public:
#ifdef _DUAL_
  DECLARE_DUAL_ERRORINFO()
#endif

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoDocumentDefault)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

	// CPartDoc calls this so we NULL m_pDoc.
	void AssociatedObjectReleased();

// Implementation
	virtual ~CAutoDocumentDefault();
protected:

#ifdef _DUAL_
	DS_BEGIN_DUAL_INTERFACE_PART(GenericDocument)
		STDMETHOD(get_Name)(THIS_ BSTR FAR* pName) ;
		STDMETHOD(get_FullName)(THIS_ BSTR FAR* pName) ;
		STDMETHOD(get_Application)(THIS_ IDispatch * FAR* ppApplication) ;
		STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* ppParent) ;
		STDMETHOD(get_Path)(THIS_ BSTR FAR* pPath) ;
		STDMETHOD(get_Saved)(THIS_ VARIANT_BOOL FAR* pSaved) ;
		STDMETHOD(get_ActiveWindow)(THIS_ IDispatch * FAR* ppWindow) ;
		STDMETHOD(get_ReadOnly)(THIS_ VARIANT_BOOL FAR* pReadOnly) ;
		STDMETHOD(put_ReadOnly)(THIS_ VARIANT_BOOL ReadOnly) ;
		STDMETHOD(get_Type)(THIS_ BSTR FAR* pType) ;
		STDMETHOD(get_Windows)(THIS_ IDispatch * FAR* ppWindows) ;
		STDMETHOD(put_Active)(THIS_ VARIANT_BOOL bActive) ;
		STDMETHOD(get_Active)(THIS_ VARIANT_BOOL FAR* pbActive) ;
		STDMETHOD(NewWindow)(THIS_ IDispatch * FAR* ppWindow) ;
		STDMETHOD(Save)(THIS_ VARIANT vFilename, VARIANT vBoolPrompt, DsSaveStatus FAR* pSaved) ;
		STDMETHOD(Undo)(THIS_ VARIANT_BOOL FAR* pSuccess) ;
		STDMETHOD(Redo)(THIS_ VARIANT_BOOL FAR* pSuccess) ;
		STDMETHOD(PrintOut)(THIS_ VARIANT_BOOL FAR* pSuccess) ;
		STDMETHOD(Close)(THIS_ VARIANT vSaveChanges, DsSaveStatus FAR* pSaved) ;
		DS_DECLARE_VTBL_PAD_10()
	DS_END_DUAL_INTERFACE_PART(GenericDocument)

	DS_DECLARE_ENABLE_DUAL(CAutoDocumentDefault, GenericDocument)
#endif

	// Generated message map functions
	//{{AFX_MSG(CAutoDocumentDefault)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

/* Removed see DualImpl.doc
	DECLARE_OLECREATE(CAutoDocumentDefault)
*/

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAutoDocumentDefault)
	afx_msg BSTR GetName();
	afx_msg BSTR GetFullName();
	afx_msg LPDISPATCH GetApplication();
	afx_msg LPDISPATCH GetParent();
	afx_msg BSTR GetPath();
	afx_msg BOOL GetSaved();
	afx_msg void SetSaved(BOOL bNewValue);
	afx_msg BSTR GetType();
	afx_msg BOOL GetReadOnly();
	afx_msg void SetReadOnly(BOOL bReadOnly) ;
	afx_msg LPDISPATCH GetActiveWindow();
	afx_msg LPDISPATCH GetWindows();
	afx_msg BOOL GetActive();
	afx_msg void SetActive(BOOL bNewValue);
	afx_msg BOOL Redo();
	afx_msg BOOL Undo();
	afx_msg LPDISPATCH NewWindow();
	afx_msg BOOL PrintOut();
	//}}AFX_DISPATCH

	afx_msg DsSaveStatus Close(const VARIANT FAR& longSaveChanges);
	afx_msg DsSaveStatus Save(const VARIANT FAR& filename, const VARIANT FAR& longSaveChanges);
	
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

#undef AFX_DATA
#define AFX_DATA NEAR

#endif // __AutoDocD_H__
