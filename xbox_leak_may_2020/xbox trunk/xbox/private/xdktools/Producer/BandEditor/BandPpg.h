#if !defined(BANDPPG_H__3BD2BA34_46E7_11D0_89AC_00A0C9054129__INCLUDED_)
#define BANDPPG_H__3BD2BA34_46E7_11D0_89AC_00A0C9054129__INCLUDED_

// BandPpg.h : Declaration of the CBandPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CBandPropPage : See BandPpg.cpp.cpp for implementation.

class CBandPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CBandPropPage)
	DECLARE_OLECREATE_EX(CBandPropPage)

// Constructor
public:
	CBandPropPage();

// Dialog Data
	//{{AFX_DATA(CBandPropPage)
	enum { IDD = IDD_PROPPAGE_BAND };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CBandPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(BANDPPG_H__3BD2BA34_46E7_11D0_89AC_00A0C9054129__INCLUDED)
