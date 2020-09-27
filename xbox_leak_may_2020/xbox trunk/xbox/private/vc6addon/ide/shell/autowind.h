// AutoWindowDefault.h : header file
//

#include <utilauto.h>

#define _DUAL_


class CPartFrame;

/////////////////////////////////////////////////////////////////////////////
// CAutoWindowDefault command target

class CAutoWindowDefault : public CAutoWindowObj
{
	DECLARE_DYNAMIC(CAutoWindowDefault)

	CAutoWindowDefault(CPartFrame* pFrame);           // protected constructor used by dynamic creation

// Attributes
public:
	CPartFrame* m_pAssociatedPartFrame ;

	void NoFrame() ;

// Operations
public:
#ifdef _DUAL_
  DECLARE_DUAL_ERRORINFO()
#endif

// Overrides

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoWindowDefault)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

	// CPartFrame calls this so we NULL m_pAssociatedPartFrame.
	void AssociatedObjectReleased();

// Implementation
protected:
	#ifdef _DUAL_
	DS_BEGIN_DUAL_INTERFACE_PART(GenericWindow)
		STDMETHOD(get_Caption)(THIS_ BSTR FAR* pbstrCaption) ;
		STDMETHOD(get_Type)(THIS_ BSTR FAR* pbstrCaption) ;
		STDMETHOD(put_Active)(THIS_ VARIANT_BOOL bActive) ;
		STDMETHOD(get_Active)(THIS_ VARIANT_BOOL FAR* pbActive) ;
		STDMETHOD(put_Left)(THIS_ long lVal) ;
		STDMETHOD(get_Left)(THIS_ long FAR* plVal) ;
		STDMETHOD(put_Top)(THIS_ long lVal) ;
		STDMETHOD(get_Top)(THIS_ long FAR* plVal) ;
		STDMETHOD(put_Height)(THIS_ long lVal) ;
		STDMETHOD(get_Height)(THIS_ long FAR* plVal) ;
		STDMETHOD(put_Width)(THIS_ long lVal) ;
		STDMETHOD(get_Width)(THIS_ long FAR* plVal) ;
		STDMETHOD(get_Index)(THIS_ long FAR* plVal) ;
		STDMETHOD(get_Next)(THIS_ IDispatch * FAR* ppDispatch) ;
		STDMETHOD(get_Previous)(THIS_ IDispatch * FAR* ppDispatch) ;
		STDMETHOD(put_WindowState)(THIS_ DsWindowState lVal) ;
		STDMETHOD(get_WindowState)(THIS_ DsWindowState FAR* plVal) ;
		STDMETHOD(get_Application)(THIS_ IDispatch * FAR* ppDispatch) ;
		STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* ppDispatch) ;
		STDMETHOD(Close)(THIS_ VARIANT vtSaveChanges, DsSaveStatus FAR* pSaved) ;
		DS_DECLARE_VTBL_PAD_10()
	DS_END_DUAL_INTERFACE_PART(GenericWindow)

	DS_DECLARE_ENABLE_DUAL(CAutoWindowDefault, GenericWindow)
#endif

	virtual ~CAutoWindowDefault();

	// Generated message map functions
	//{{AFX_MSG(CAutoWindowDefault)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

/* dualimpl.doc
	DECLARE_OLECREATE(CAutoWindowDefault)
*/

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAutoWindowDefault)
	afx_msg BSTR GetCaption();
	afx_msg LPDISPATCH GetParent() ;
	afx_msg LPDISPATCH GetApplication() ;
	afx_msg BSTR GetType();
	afx_msg BOOL GetActive();
	afx_msg void SetActive(BOOL bNewValue);
	afx_msg long GetLeft();
	afx_msg long GetTop();
	afx_msg long GetHeight();
	afx_msg long GetWidth();
	afx_msg long GetIndex();
	afx_msg LPDISPATCH GetNext();
	afx_msg LPDISPATCH GetPrevious();
	afx_msg DsWindowState GetWindowState();
	afx_msg void SetWindowState(DsWindowState nNewValue);
	afx_msg void SetLeft(long nNewValue);
	afx_msg void SetTop(long nNewValue);
	afx_msg void SetHeight(long nNewValue);
	afx_msg void SetWidth(long nNewValue);
	afx_msg DsSaveStatus Close(const VARIANT FAR& boolSaveChanges);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
