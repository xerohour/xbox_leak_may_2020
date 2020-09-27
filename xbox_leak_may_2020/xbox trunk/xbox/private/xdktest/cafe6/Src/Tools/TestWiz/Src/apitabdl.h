// apipage.h : header file
//
#include "treelist.h"
#include "owdlist.h"
#include "data.h"
#include "testawx.h"

/////////////////////////////////////////////////////////////////////////////
// ApiTabdlg dialog

#ifndef _APITABDLG_
#define _APITABDLG_

#define API_FT_TEXT 0x0001

class ApiTabdlg : public CPropertyPage
{
    DECLARE_DYNCREATE(ApiTabdlg)
// Construction
public:
	ApiTabdlg(); 
    ~ApiTabdlg();
// Dialog Data
	CAreasLB  m_areaList;
	OwnerDrawLB  m_ScriptList ;
	//{{AFX_DATA(ApiTabdlg)
	enum { IDD = IDD_API };
	CEdit	m_ScriptItemCtrl;
	CListBox	m_directivesList;
		CButton	m_AddButton;
		BOOL	m_HideVarsBraces;
		CStatic	m_apiDescription;
		CListBox	m_parameterList;
		CListBox	m_apiList;
		CComboBox	m_categoryCB;
		CString		m_category;
	CString	m_ScriptItemTxt;
	int		m_iDisplayClassNames;
	//}}AFX_DATA


// Attributes
public:
	CStringList     m_stringList;
	CAPIArray       m_apiArray;
    CAreaArray      m_areaArray;

	CString			m_strCurrentCategory;
    CString         m_strCurrentArea;
    CString         m_strCurrentSubarea;
	CString			m_strCurrentAPI;

	BOOL			m_bIsOldFormat;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(ApiTabdlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

    virtual BOOL OnSetActive(void);
	virtual void OnOK() ;
	virtual BOOL OnKillActive() ;

// Operations
public:
    UINT    OpenAPIData(LPCSTR szFileName, UINT nFileType = API_FT_TEXT);
    void    LoadAPIList(void);
    void    DisplayAreas(void);

    BOOL    InterpretLine(LPCSTR szNewLine);
    char*  RemoveExtraChars(char* pchLine);
    
	void    FillStructures( LPCSTR szLine );
	LPCSTR	SetCurrentCategory( LPCSTR szLine);
    LPCSTR  FillAreaStruct(LPCSTR szLine);
 	LPCSTR  FillBaseClassStruct(LPCSTR szLine);
	LPCSTR  FillClassStruct(LPCSTR szLine);
	LPCSTR  FillSubareaStruct(LPCSTR szLine);
    LPCSTR  FillAPIStruct(LPCSTR szLine );
	LPCSTR  FillDescStruct(LPCSTR szLine);
	LPCSTR  FillReturnStruct(LPCSTR szLine);
	LPCSTR	FillParamStruct(LPCSTR szLine, CAPI* pAPI = NULL);
	LPCSTR  FillParamDescStruct(LPCSTR szLine);

    int     FindArea(LPCSTR szArea = NULL);
	BOOL	IsArea(int index);
	void	ExpandArea(int index, BOOL bExpand = TRUE);
	void	DisplaySubareas( int index, int i=-1 );
	void	HideSubareas( int index, int i=-1 );
	void	DisplayAreaAPIs( int index );
	void	DisplaySubareaAPIs( int index );
	CString	GetAreaName( int index );
	void	DisplayParameters( int index, BOOL bAddedToScript = FALSE );
	void	DisplayAPIDescription( int index );
	void	DisplayParamDescription( int index );
	CString CreateVarFromClassName(CString& strClassName);
	void 	OnInsert(); 


// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(ApiTabdlg)
	afx_msg void OnSelchangeCategory();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListAreas();
	afx_msg void OnSelchangeListAreas();
	afx_msg void OnSelchangeListApi();
	afx_msg void OnDblclkListApi();
	afx_msg void OnAdd();
	afx_msg void OnDblclkScriptlist();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnPopupDelete();
	afx_msg void OnPopuupWarnings();
	afx_msg void OnHideVarsAndBraces();
	afx_msg void OnPaint();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMoveDown();
	afx_msg void OnMoveUp();
	afx_msg void OnDblclkDirectives();
	afx_msg void OnNewtestcase();
	afx_msg void OnSelchangeScriptlist();
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSetfocusListApi();
	afx_msg void OnKillfocusListApi();
	afx_msg void OnDisplayClassNames();
	afx_msg void OnSelchangeListParameters();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // _APITABDLG_
