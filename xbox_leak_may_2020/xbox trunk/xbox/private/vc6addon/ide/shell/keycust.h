// keycust.h : header file
//

#include "tap.h"
#include "resource.h"
#include "customiz.h"

#define FILL_NO_SEL		-2		// SetCurSel(-1) deselects everything.
								// This says don't even call SetCurSel()

#define NUM_ACC_RES_TABLES	4

struct AccResTable
{
	LPSTR lpstrResID;
	const char* name;
};

typedef struct tagAccCmd
{
	UINT id;
	const char* name;
} AccCmd;

typedef struct tagAccGrp
{
	const char* name;
	const UINT* list;
} AccGrp;

typedef struct tagAccTable
{
	HACCEL hAcc;
	const char* name;
} AccTable;

extern AccResTable AccResTables[];
extern AccCmd AccCommands[];
extern AccGrp AccGroups[];

/////////////////////////////////////////////////////////////////////////////
// CKeyCustDlg dialog

class CKeyListBox : public CListBox
{
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
};

class CKeyCustDlg : public CCommandTab
{
// Construction
public:
	CKeyCustDlg(AccTable* pAccTable, int nAccTables);
	~CKeyCustDlg();

// Dialog Data
	//{{AFX_DATA(CKeyCustDlg)
	enum { IDD = IDDP_CUSTOMIZE_KEYBOARD };
	CComboBox	m_ScopeList;
	CListBox	m_CmdList;
	CComboBox	m_CatList;
	//}}AFX_DATA

	CTap m_Tap;

	int m_nAccelTables;
	CKeymap* m_pKeymap;
	int m_iCurScope;

	CKeyListBox m_keyList;
	CAppToolGroups* m_pAppToolGroups;

// Operations
protected:
	virtual int InitScopeList(int nSel = 0);
	virtual int FillCategoriesList(int nSel = 0);
	virtual int FillCommandsList(int nCat = -1, int nSel = 0);
	virtual int FillKeysList(UINT nID = -1, int nSel = -1);
	void AddKeymapToListBox(CKeymap* pKeymap, CListBox* pList, UINT nID, 
		const char* szPrefix, WORD kcPrefix, BOOL bInherited = FALSE);

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	//REVIEW: Support for a dirty command cache
	virtual BOOL Activate(CTabbedDialog* pParentWnd, CPoint position);

	// Generated message map functions
	//{{AFX_MSG(CKeyCustDlg)
	virtual BOOL OnInitDialog();
	afx_msg int OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex);
	afx_msg void OnSelChangeCatList();
	afx_msg void OnSelChangeCmdList();
	afx_msg void OnClickedAssign();
	afx_msg void OnClickedRemove();
	afx_msg void OnClickedResetAll();
	afx_msg void OnSelChangeScopeList();
	afx_msg void OnSelChangeKeyList();
	//}}AFX_MSG
	afx_msg void OnChangeTap();

	DECLARE_MESSAGE_MAP()

// Helper functions
public:
	int MaintainListSel(CListBox* pList, DWORD dwData);
	DWORD AccelToDWord(ACCEL& accel);
	ACCEL DWordToAccel(DWORD dw);
	BOOL GetCurrentAssignment(CKeymap* pKeymap, CString& str);
	BOOL IsMenuAccel( WORD kc );
};
