#if !defined(AFX_MFCAXCWCTL_H__7EFBEC09_CAA8_11D0_84BF_00AA00C00848__INCLUDED_)
#define AFX_MFCAXCWCTL_H__7EFBEC09_CAA8_11D0_84BF_00AA00C00848__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
#define AXCW______
// MfcaxcwCtl.h : Declaration of the CMfcaxcwCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CMfcaxcwCtrl : See MfcaxcwCtl.cpp for implementation.

class CMfcaxcwCtrl : public COleControl
{
	DECLARE_DYNCREATE(CMfcaxcwCtrl)

// Constructor
public:
	CMfcaxcwCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMfcaxcwCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CMfcaxcwCtrl();

	DECLARE_OLECREATE_EX(CMfcaxcwCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CMfcaxcwCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CMfcaxcwCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CMfcaxcwCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CMfcaxcwCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CMfcaxcwCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CMfcaxcwCtrl)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CMfcaxcwCtrl)
		// NOTE: ClassWizard will add and remove enumeration elements here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MFCAXCWCTL_H__7EFBEC09_CAA8_11D0_84BF_00AA00C00848__INCLUDED)
