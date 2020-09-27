#if !defined(AFX_CONTAINERCTL_H__778A0B9D_6F81_11D3_B45F_00105A2796DE__INCLUDED_)
#define AFX_CONTAINERCTL_H__778A0B9D_6F81_11D3_B45F_00105A2796DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ContainerCtl.h : Declaration of the CContainerCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CContainerCtrl : See ContainerCtl.cpp for implementation.

class CContainerCtrl : public COleControl
{
	DECLARE_DYNCREATE(CContainerCtrl)

// Constructor
public:
	CContainerCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContainerCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CContainerCtrl();

	DECLARE_OLECREATE_EX(CContainerCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CContainerCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CContainerCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CContainerCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CContainerCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CContainerCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CContainerCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CContainerCtrl)
		// NOTE: ClassWizard will add and remove enumeration elements here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTAINERCTL_H__778A0B9D_6F81_11D3_B45F_00105A2796DE__INCLUDED)
