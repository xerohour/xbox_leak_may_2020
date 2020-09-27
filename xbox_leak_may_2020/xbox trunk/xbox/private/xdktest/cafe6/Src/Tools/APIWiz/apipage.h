// apipage.h : header file
//
#include "areaslb.h"
#include "data.h"

/////////////////////////////////////////////////////////////////////////////
// CAPIPage dialog

#ifndef _CAPIPAGE_
#define _CAPIPAGE_

#define API_FT_TEXT 0x0001

class CAPIPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CAPIPage)
// Construction
public:
	CAPIPage(); 
    ~CAPIPage();
// Dialog Data
	CAreasLB  m_areaList;
	//{{AFX_DATA(CAPIPage)
	enum { IDD = IDD_API };
	CEdit		m_ApiCallCtrl;
	CStatic		m_apiDescription;
	CListBox	m_parameterList;
	CListBox	m_apiList;
	CComboBox	m_categoryCB;
	CString		m_category;
	int		m_iDisplayClassNames;
	int		m_iDisplayReturnType;
	int		m_iDisplayParams;
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
	//{{AFX_VIRTUAL(CAPIPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	virtual void OnOK() ;

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
	//{{AFX_MSG(CAPIPage)
	afx_msg void OnSelchangeCategory();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkListAreas();
	afx_msg void OnSelchangeListAreas();
	afx_msg void OnSelchangeListApi();
	afx_msg void OnDblclkListApi();
	afx_msg void OnSetfocusListApi();
	afx_msg void OnKillfocusListApi();
	afx_msg void OnSelchangeListParameters();
	afx_msg void OnDisplayClassnames();
	afx_msg void OnDisplayReturnType();
	afx_msg void OnDisplayParams();
	afx_msg void OnAddApi();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // _CAPIPAGE_
