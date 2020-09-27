// AutoDocs.h : header file
//
#ifndef __AUTODOCS_H__
#define __AUTODOCS_H__

#include <utilauto.h>

////////////////////////////////////////////////////////////////////////////////////

class CAutoDocuments : public CAutoObj
{
	friend class CEnumVariantObjs;

	//	DECLARE_DYNACREATE(CAutoDocuments)
	DECLARE_DYNAMIC(CAutoDocuments)


// Attributes
public:
	// The collection will clean up the list. Not the caller.
	CAutoDocuments() ;
	virtual ~CAutoDocuments();

// Operations
public:
    DECLARE_DUAL_ERRORINFO()

	void DeleteCollection(); // Delete the collection and release all of its items. [@sync]
	void Sync() ;			 // Replaved current colleciton with a new up to date collection. [@sync]

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAutoDocuments)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CDispatchList* m_pPtrlData;

	DS_BEGIN_DUAL_INTERFACE_PART(Documents)
		STDMETHOD(get_Count)(THIS_ long FAR* Count);
		STDMETHOD(get_Application)(THIS_ IDispatch * FAR* ppApplication);
		STDMETHOD(get_Parent)(THIS_ IDispatch * FAR* ppParent);
		STDMETHOD(get__NewEnum)(THIS_ IUnknown * FAR* _NewEnum);
		STDMETHOD(Item)(THIS_ VARIANT index, IDispatch * FAR* Item);
		STDMETHOD(SaveAll)(THIS_ VARIANT vtBoolPrompt,  DsSaveStatus FAR* pSaved) ;
		STDMETHOD(CloseAll)(THIS_ VARIANT vtSaveChanges, DsSaveStatus FAR* pSaved) ;
		STDMETHOD(Add)(THIS_ BSTR docType, VARIANT vtReserved, IDispatch * FAR* ppDocument) ;
		STDMETHOD(Open)(THIS_ BSTR filename, VARIANT vtDocType, VARIANT vtBoolReadOnly, IDispatch * FAR* ppDocument) ;
	DS_END_DUAL_INTERFACE_PART(Documents)

	DS_DECLARE_ENABLE_DUAL(CAutoDocuments, Documents)


	// Generated message map functions
	//{{AFX_MSG(CAutoDocuments)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// Removed See DualImpl.doc | 	DECLARE_OLECREATE(CAutoDocuments)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAutoDocuments)
	afx_msg long GetCount();
	afx_msg LPDISPATCH GetApplication();
	afx_msg LPDISPATCH GetParent();
	afx_msg LPDISPATCH Item(const VARIANT FAR& index);
	afx_msg LPDISPATCH Add(LPCTSTR docType, const VARIANT & vtReserved);
	afx_msg LPDISPATCH Open(LPCTSTR filename, const VARIANT FAR& doctype, const VARIANT FAR& readonly);
	//}}AFX_DISPATCH
	afx_msg LPUNKNOWN _NewEnum();
	afx_msg DsSaveStatus SaveAll(const VARIANT FAR& vtBoolPrompt);
	afx_msg DsSaveStatus CloseAll(const VARIANT FAR& vtLongSaveChanges);

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif //__AUTODOCS_H__
