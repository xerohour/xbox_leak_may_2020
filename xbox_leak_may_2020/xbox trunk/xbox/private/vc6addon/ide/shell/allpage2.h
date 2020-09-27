/////////////////////////////////////////////////////////////////////////////
//	ALLPAGE.H
//		Defines all page classes which can be used to display a
//		grid based property page

#ifndef __ALLPAGE_H__
#define __ALLPAGE_H__

#include <proppage.h>

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

#define	TOBEUSED_AFTER_BETA1	1	// new stuff to be used after beta 1

#define	ALLPAGE_GUTTER			3
#define	ALLPAGE_HEADER_HEIGHT	26	// default height if GetDC fails

/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
class CPropItem;
class CPropEdit;
class CPropButton;
class CPropList;
class CPropListBox;

enum ControlType {
	ctrl_Unknown,
	ctrl_Edit,
	ctrl_ComboLimitToList,
	ctrl_BuilderPerPropertyBrowsing,
	ctrl_BuilderProvidePropertyBuilder,
	ctrl_BuilderWizardManager,
	ctrl_BuilderWizardManagerIntrinsic,
	ctrl_BuilderInternal,
	ctrl_ComboNotLimitToList
};

enum DataType {
	data_Unknown = 0,
	data_Int2,			// VT_I2
	data_Int4,			// VT_I4
	data_Bool,			// VT_BOOL
	data_Color,			// VT_COLOR
	data_Font,			// VT_DISPATCH
	data_Picture,		// VT_DISPATCH
	data_UInt2,			// VT_UI2
	data_UInt4,			// VT_UI4
	data_Double,		// VT_R8
	data_String,		// VT_BSTR
	data_EnumString,	// VT_BSTR
	data_EnumInt2,		// VT_I2
	data_EnumInt4,		// VT_I4
	data_Path,			// VT_BSTR (url)
	data_Float,			// VT_R4
	data_Date,			// VT_DATE
	data_Currency		// VT_CY
};

/////////////////////////////////////////////////////////////////////////////
// C3dFontDialog - FontDialog which hides the Color Check Box

class C3dFontDialog : public CFontDialog
{
public:
	C3dFontDialog(LPLOGFONT lplfInitial = NULL,
		DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS,
		CDC* pdcPrinter = NULL,
		CWnd* pParentWnd = NULL);

	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	virtual int DoModal();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CEnumData

class CEnumData
{
// Construction/Destruction
public:
	CEnumData()
			{
				m_bDirty = TRUE;
			}
	~CEnumData()
			{
				// just remove all the elements in the arrays
				m_straEnumStrings.RemoveAll();
				m_dwaCookies.RemoveAll();
			}

	BOOL	IsDirty()
			{
				return m_bDirty;
			}
	void	SetDirty(BOOL bDirty)
			{
				m_bDirty = bDirty;
			}

public:
	CStringArray	m_straEnumStrings;
	CDWordArray		m_dwaCookies;
	BOOL			m_bDirty;
};

/////////////////////////////////////////////////////////////////////////////
// CPropItem

class CPropItem
{
// Construction
public:
	CPropItem(LPCTSTR pszcText, VARTYPE vt, DataType dt, DISPID dispid);
	~CPropItem();

	void	GetNewEnumData();				// alloc memory for m_pEnumData
	BOOL	IsEnumDataDirty()
			{
				return (NULL == m_pEnumData) ? TRUE : m_pEnumData->IsDirty();
			}
	void	SetEnumDataDirty(BOOL bDirty)
			{
				if (NULL != m_pEnumData)
					m_pEnumData->SetDirty(bDirty);
			}
	void	SetDispid(ULONG ulIndex, DISPID dispid);

// Attributes
public:
	CString		m_strText;			// property label
	CString		m_strProp;			// property value
	ControlType	m_ctrlType;			// type of control required on grid
	DataType	m_dataType;			// type of data to be edited
	VARTYPE		m_vt;				// variant type

	union
	{
		int			m_nValue;
		DWORD		m_dwValue;
		float		m_fValue;
		double		m_dValue;
		DATE		m_date;
		CURRENCY	m_currency;
	} unionValue;

	BOOL		m_fEdit:1;		// in editing mode

	CEnumData*	m_pEnumData;	// data for enumeration

	int			m_iIndex;		// index of item for builder type

	// each ITypeInfo corresponds to one of the controls selected
	// i.e, for single selection, this array will contain 1 item
	CPtrArray	m_TypeInfoArrayEnumInt;	// array of ITypeInfo ptrs stored
								// for enumerated I2 and I4 data types
	ITypeInfo*	m_TypeInfo;		// copy of ITypeInfo interface stored in
								// COleAllPage (this is not addref'ed -- so don't release it)

	// NOTE:
	// of the following one of the pointers will be set to NULL depending upon
	// whether the property description was of type vardesc or funcdesc.
	// Also, only one pointer is stored because, when multiple controls are
	// selected, we would have already intersected the properties to make sure
	// that the property types are the same and when we need to get the property
	// again, we only need to use that for the first control selected
	LPVARDESC	m_pvd;		// pointer to vardesc of the first control selected
	LPFUNCDESC	m_pfd;		// pointer to funcdesc of the first control selected

	// The following members are used to cache DISPID's and will be enabled
	// soon -- till then, they will be populated with values and not used
	DISPID		m_Dispid;			// used for single selection
	DISPID*		m_pDispid;			// used for multiple selection

	// NOTE:
	// if you change the setting of the following value in the code, please
	// make sure that it is done properly since a lot of initialization and
	// termination depends upon this number.  Thanks.
	static ULONG	s_ulCountControls;	// count of controls selected
};

/////////////////////////////////////////////////////////////////////////////
// CPropEdit

class CPropEdit : public CEdit
{
	friend class CPropListBox;

// Construction
public:
	CPropEdit();

// Attributes
public:
	CPropItem*	m_pItem;
	CPropList*	m_pLb;
	CPropButton*	m_pBtn;
	
protected:
	BOOL	SaveProperty();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropEdit)
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPropButton

class CPropButton : public CButton
{
// Construction
public:
	CPropButton();

// Attributes
public:
	CPropItem*	m_pItem;

private:
	long	m_lBtnWidth;
	long	m_lBtnHeight;

// Operations
public:
	void	SetBtnDimensions();
	long	GetBtnWidth()
			{
				return m_lBtnWidth;
			}
	long	GetBtnHeight()
			{
				return m_lBtnHeight;
			}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropButton)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

protected:
	//{{AFX_MSG(CPropButton)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CPropList

class CPropList : public CListBox
{
// Construction
public:
	CPropList();

// Attributes
public:
	CPropEdit *m_pTxt;

// Implementation
public:
	void	FillContents(CPropItem *pItem);
	void	PositionToProperty(CRect *pRect, CPropItem *pItem);
	void	SelectNext();
	void	SelectPrev();
	void	SelectLimitToList(LPCTSTR pszcItem);
	void	SelectNextItemInList();

	void	SetParentListBoxPtr(CPropListBox* pPropListBox)
			{
				m_pPropListBox = pPropListBox;
			}

protected:
	void	UpdateEditControl();


	// Generated message map functions
protected:
	//{{AFX_MSG(CPropList)
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	CPropListBox*	m_pPropListBox;
	int				m_iItemHeight;
};

/////////////////////////////////////////////////////////////////////////////
// CPropListBox window

class CPropListBox : public CListBox
{
	friend class CPropEdit;

// Construction
public:
	CPropListBox();

// Attributes
public:
	CPropEdit*		m_pTxt;
	CPropButton*	m_pBtn;
	CPropList*		m_pLb;
	C3dPropertyPage*	m_pPage;

	UINT			m_uiIndex;

// Operations
public:
	BOOL	CreateChildren();
	void	ResetContent();
	BOOL	PreTranslateMessage(MSG* pMsg);

	BOOL	UpdateProperty()
			{
				return ((m_pPage) ? m_pPage->Validate() : TRUE);
			}
	void	ShowEditControl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropListBox)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//}}AFX_VIRTUAL

protected:
	void	SetFocusAndCurSel();

// Implementation
public:
	virtual ~CPropListBox();

protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropListBox)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnDblclk();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnSelchange();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnBtnClick();

	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
class COleAllPage : public CSlobPage
{
public:
	DECLARE_DYNAMIC(COleAllPage)

public:
	COleAllPage();
	~COleAllPage();

	virtual BOOL Create(UINT nIDSheet, CWnd* pWndOwner);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual void InitializePage();
	virtual void InitPage();

	virtual BOOL Validate();
	virtual BOOL UndoPendingValidate();

	virtual LRESULT OnPageHelp(WPARAM wParam, LPARAM lParam);

	virtual void GetPageName(CString& strName)
			{
				strName = m_strName;
			}

	virtual void TermPage();
	virtual CSize GetPageSize();

	BOOL	Apply();
	HRESULT	ExecuteBuilder(CPropItem *pItem);
	HRESULT	SetPictureToNone(CPropItem* pItem);

	int		GetColumnWidth(int iCol)
			{
				if (iCol == 0)
					return m_ihcCol0Width;
				else if (iCol == 1)
					return m_ihcCol1Width;
				else
					return 0;
			}

	void	SetHeaderInfo();
	void	UpdateEnumData(CPropItem* pItem);

protected:
	void	FillPropList();
	BOOL	AddToMap(IDispatch *pDisp, CMapStringToPtr *pMap);
	BOOL	IntersectToMap(CMapStringToPtr *pMap, ULONG ulIndex);
	void	DeleteBindPtr(ITypeInfo *pTypeInfo, DESCKIND *pDescKind, BINDPTR *pBindPtr);	
	HRESULT	ApplyProperty(DISPID dispID, CPropItem* pItem, IDispatch *pDisp);
	void	ChangePageSize(CSize const& sizeNew);

	BOOL	IsIntrinsicBuilder(GUID guidBuilder);
	void	DetermineBuilderType(DISPID dispid, CPropItem* pItem);
	BOOL	DoesTypeInfoHasPathProperties(ITypeInfo* srpTypeInfoStart);

	void	GetNotifySinkInfo();
	void	FreeNotifySinkInfo();

	BOOL	CallGeneralPropertyBuilder(IDispatch* pAppDisp, IDispatch* pBuilderDisp,
				HWND hwndOwner, DISPID dispidExecute, VARIANT* pvarValue);
	BOOL	CallURLBuilder(IDispatch* pAppDisp, IDispatch* pControlDisp,
				IDispatch* pBuilderDisp, HWND hwndOwner, DISPID dispidExecute,
				VARIANT* pvarValue);
	HRESULT	SavePropertyForAllControls(VARIANT* pvarValue,
				CPropItem* pItem);

	ControlType GetControlBuilderType(DISPID dispid, IDispatch* pControlDisp);
	BOOL	GetPictureType(IDispatch* pPictDisp, CString& strProp);
	void	ReleaseInterfaces();
	void	ForcePropertyBrowserToshow();

	BOOL	IsPropTypeReadOnly(LPVARDESC pVarDesc, LPFUNCDESC pFuncDesc,
				BOOL bCheckForPutAndPutRef);
	DataType GetPropType(IDispatch* pDisp, LPTYPEINFO pTypeInfo, LPVARDESC pVarDesc,
				LPFUNCDESC pFuncDesc, BOOL bHasPathProperties);
	DataType DataTypeFromVT(TYPEDESC* pTypeDesc, LPTYPEINFO pTypeInfo);
	BOOL	IsPropAPathProperty(LPTYPEINFO pTypeInfo, DISPID dispid);
	DataType MapDataType(VARTYPE vt);
	BOOL	GetPropName(LPTYPEINFO pTypeInfo, DISPID dispid, CString& strPropName);
	BOOL	GetPropValueForEnumString(IDispatch* pDisp, DISPID dispid,
				CPropItem* pItem, VARIANT* pVarValue);
	BOOL	GetPropValueForEnumInt(LPTYPEINFO pTypeInfo, HREFTYPE hrefType,
				long lEnumValue, CPropItem* pItem, BOOL bSetAllDetails);
	BOOL	GetPropValueForOtherTypes(VARIANT* pVarValue, CPropItem* pItem);
	BOOL	GetPropValue(IDispatch* pDisp, DISPID dispid, VARIANT* pVarValue);
	BOOL	GetPropDetails(IDispatch* pDisp, LPTYPEINFO pTypeInfo,
				HREFTYPE hrefType, DISPID dispid, CPropItem* pItem,
				BOOL bSetAllDetails, VARIANT* pVarValue);
	BOOL	UpdateEnumIntData(int iControlIndex, DISPID dispid,
				CPropItem* pItem, long lEnumValue);
	BOOL	UpdateEnumStringData(IDispatch* pDisp, DISPID dispid,
				CPropItem* pItem, BOOL bUpdatePropValue);
	BOOL	IntersectPropEnumString(IDispatch* pDisp, DISPID dispid,
				CPropItem* pItem);
	BOOL	IntersectPropEnumInt(IDispatch* pDisp, DISPID dispid,
				CPropItem* pItem, LPTYPEINFO pTypeInfo, TYPEDESC* pTypeDesc);
	BOOL	IntersectPropItemData(IDispatch* pDisp, DISPID dispid,
				CPropItem* pItem, LPTYPEINFO pTypeInfo, TYPEDESC* pTypeDesc);
	void	UpdateChangedProperties();

	HRESULT	ExecutePPBBuilder(IDispatch* pDisp, DISPID dispid);
	HRESULT	ExecuteInternalBuilder(CPropItem* pItem, VARIANT* pVarValue);
	HRESULT	ExecuteBWMIntrinsicBuilder(CPropItem* pItem, VARIANT* pVarValue);
	HRESULT	ExecuteBWMAndPPBBuilder(CPropItem* pItem,
				DISPID dispid, VARIANT* pVarValue);

protected:
	CDWordArray	m_dwaCookies;	// dword array of cookies for propertynotifysink

// Attributes
protected:
	BOOL		m_bActive:1;
	BOOL		m_bUpdatingProps;	// kind of mutex to prevent
									// IPropertyNotifySink::OnChanged from triggering
									// in the middle of a BeginUndo before we actually start
									// setting the properties
	BOOL		m_bUpdateLater;		// used to optimize a little
									// when saving several properties, the
									// IPropertyNotifySink::OnChanged gets called
									// several times -- so use this flag to
									// to refresh at once later.

	CPropListBox	m_lbProps;
	CHeaderCtrl	m_hcProps;
	int			m_ihcHeight;
	int			m_ihcCol0Width;
	int			m_ihcCol1Width;
	BOOL		m_bSetHeader;

    COleDispatchDriver m_dispDriver;

	CSize		m_sizeMin;

	IDispatch*	m_pSingleDisp;
	IDispatch**	m_pprgDisp;
	ULONG		m_ulCountControls;
	CString		m_strName;

	// stores the number of ITypeInfo's that we get from the control
	// this is used by CPropItem when the property for one of the selected
	// control updates its value
	ITypeInfo*	m_pSingleTypeInfo;	// single typeinfo used in most cases
	ITypeInfo**	m_ppTypeInfo;	// array of ITypeInfo ptrs stored
	ULONG		m_ulTypeInfos;	// count of ITypeinfo's in the above array

// Message map
protected:
	afx_msg void OnEndTrackHeader(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnTrackHeader(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCommandHelp(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	DECLARE_IDE_CONTROL_MAP()

	// Helper functions for PropertyNotifySink
	HRESULT	OnChanged(DISPID dispID);
	HRESULT OnRequestEdit(DISPID dispID);

// Interface Maps
	// IPropertyNotifySink
	BEGIN_INTERFACE_PART(PropertyNotifySink, IPropertyNotifySink)
		INIT_INTERFACE_PART(COleAllPage, PropertyNotifySink)
		STDMETHOD(OnChanged)(DISPID dispID);
		STDMETHOD(OnRequestEdit)(DISPID dispID);
	END_INTERFACE_PART(PropertyNotifySink);

	DECLARE_INTERFACE_MAP()
};

#ifdef _DEBUG
#undef THIS_FILE
#define THIS_FILE __FILE__
#endif

#undef AFX_DATA
#define AFX_DATA NEAR

#endif		// __ALLPAGE_H__
