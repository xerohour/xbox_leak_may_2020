// CollectionView.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "dlsdesigner.h"
#include "Collection.h"
#include "CollectionCtl.h"
#include "CollectionEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCollectionEditor

IMPLEMENT_DYNCREATE(CCollectionEditor, CFormView)

CCollectionEditor::CCollectionEditor(CCollectionCtrl* parent)
				 : CFormView(CCollectionEditor::IDD)
{
	m_parent = parent;

	//{{AFX_DATA_INIT(CCollectionEditor)
	m_dwVersion1 = 0;
	m_dwVersion2 = 0;
	m_dwVersion3 = 0;
	m_dwVersion4 = 0;
	m_dwSampleCount = 0;
	m_csSubject = _T("");
	m_csName = _T("");
	m_csEngineer = _T("");
	m_csCopyright = _T("");
	m_csComment = _T("");
	m_dwArticCount = 0;
	m_dwInstCount = 0;
	m_dwRegionCount = 0;
	m_dwWaveCount = 0;
	//}}AFX_DATA_INIT
}

CCollectionEditor::~CCollectionEditor()
{
}

void CCollectionEditor::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCollectionEditor)
	DDX_Text(pDX, IDC_VER_1, m_dwVersion1);
	DDV_MinMaxDWord(pDX, m_dwVersion1, 0, 65535);
	DDX_Text(pDX, IDC_VER_2, m_dwVersion2);
	DDV_MinMaxDWord(pDX, m_dwVersion2, 0, 65535);
	DDX_Text(pDX, IDC_VER_3, m_dwVersion3);
	DDV_MinMaxDWord(pDX, m_dwVersion3, 0, 65535);
	DDX_Text(pDX, IDC_VER_4, m_dwVersion4);
	DDV_MinMaxDWord(pDX, m_dwVersion4, 0, 65535);
	DDX_Text(pDX, IDC_SAMPLE_COUNT, m_dwSampleCount);
	DDV_MinMaxDWord(pDX, m_dwSampleCount, 0, 1000000000);
	DDX_Text(pDX, IDC_SUBJECT, m_csSubject);
	DDX_Text(pDX, IDC_NAME, m_csName);
	DDX_Text(pDX, IDC_ENGINEER, m_csEngineer);
	DDX_Text(pDX, IDC_COPYRIGHT, m_csCopyright);
	DDX_Text(pDX, IDC_COMMENT, m_csComment);
	DDX_Text(pDX, IDC_ARTIC_COUNT, m_dwArticCount);
	DDX_Text(pDX, IDC_INST_COUNT, m_dwInstCount);
	DDX_Text(pDX, IDC_REGION_COUNT, m_dwRegionCount);
	DDX_Text(pDX, IDC_WAVE_COUNT, m_dwWaveCount);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCollectionEditor, CFormView)
	//{{AFX_MSG_MAP(CCollectionEditor)
	ON_EN_CHANGE(IDC_ARTIC_COUNT, OnChangeArticCount)
	ON_EN_CHANGE(IDC_COMMENT, OnChangeComment)
	ON_EN_CHANGE(IDC_COPYRIGHT, OnChangeCopyright)
	ON_EN_CHANGE(IDC_ENGINEER, OnChangeEngineer)
	ON_EN_CHANGE(IDC_INST_COUNT, OnChangeInstCount)
	ON_EN_CHANGE(IDC_NAME, OnChangeName)
	ON_EN_CHANGE(IDC_REGION_COUNT, OnChangeRegionCount)
	ON_EN_CHANGE(IDC_SAMPLE_COUNT, OnChangeSampleCount)
	ON_EN_CHANGE(IDC_SUBJECT, OnChangeSubject)
	ON_EN_CHANGE(IDC_VER_1, OnChangeVer1)
	ON_EN_CHANGE(IDC_VER_2, OnChangeVer2)
	ON_EN_CHANGE(IDC_VER_3, OnChangeVer3)
	ON_EN_CHANGE(IDC_VER_4, OnChangeVer4)
	ON_EN_CHANGE(IDC_WAVE_COUNT, OnChangeWaveCount)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCollectionEditor diagnostics

#ifdef _DEBUG
void CCollectionEditor::AssertValid() const
{
	CFormView::AssertValid();
}

void CCollectionEditor::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCollectionEditor message handlers

BOOL CCollectionEditor::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}

void CCollectionEditor::OnInitialUpdate() 
{
	CCollection *pCollection = m_parent->GetCollection();

	m_csName = pCollection->m_Info.m_csName;
	m_csCopyright = pCollection->m_Info.m_csCopyright;
	m_csEngineer = pCollection->m_Info.m_csEngineer;
	m_csSubject = pCollection->m_Info.m_csSubject;
	m_csComment = pCollection->m_Info.m_csComment;
    m_dwSampleCount = pCollection->SampleCount();
	m_dwArticCount = pCollection->ArticulationCount();
	m_dwInstCount = pCollection->InstrumentCount();
	m_dwWaveCount = pCollection->WaveCount();
	m_dwRegionCount = pCollection->RegionCount();
	m_dwVersion1 = HIWORD(pCollection->m_rVersion.dwVersionMS);
	m_dwVersion2 = LOWORD(pCollection->m_rVersion.dwVersionMS);
	m_dwVersion3 = HIWORD(pCollection->m_rVersion.dwVersionLS);
	m_dwVersion4 = LOWORD(pCollection->m_rVersion.dwVersionLS);

	CFormView::OnInitialUpdate();
}

void CCollectionEditor::OnChangeArticCount() 
{
	ASSERT(FALSE);
}

void CCollectionEditor::OnChangeComment() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
		CCollection *pCollection = m_parent->GetCollection();			
		pCollection->m_Info.m_csComment = m_csComment;
	}
	else
	{
		// Add better error handling for J3
	}
}

void CCollectionEditor::OnChangeCopyright() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
		CCollection *pCollection = m_parent->GetCollection();		
		pCollection->m_Info.m_csCopyright = m_csCopyright;
	}
	else
	{
		// Add better error handling for J3
	}
}

void CCollectionEditor::OnChangeEngineer() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
		CCollection *pCollection = m_parent->GetCollection();			
		pCollection->m_Info.m_csEngineer = m_csEngineer;	
	}
	else
	{
		// Add better error handling for J3
	}
}

void CCollectionEditor::OnChangeInstCount() 
{
	ASSERT(FALSE);
}

void CCollectionEditor::OnChangeName() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
		CCollection *pCollection = m_parent->GetCollection();			
        pCollection->m_Info.m_csName = m_csName;
		
		BSTR bstrName = pCollection->m_Info.m_csName.AllocSysString();
		pCollection->SetNodeName(bstrName);
	}
	else
	{
		// Add better error handling for J3
	}
}

void CCollectionEditor::OnChangeRegionCount() 
{
	ASSERT(FALSE);
}

void CCollectionEditor::OnChangeSampleCount() 
{
	ASSERT(FALSE);
}

void CCollectionEditor::OnChangeSubject() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
		CCollection *pCollection = m_parent->GetCollection();			
        pCollection->m_Info.m_csSubject = m_csSubject;
	}
	else
	{
		// Add better error handling for J3
	}
}

void CCollectionEditor::OnChangeVer1() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
		CCollection *pCollection = m_parent->GetCollection();			
		pCollection->m_rVersion.dwVersionMS = (m_dwVersion1 << 16) | m_dwVersion2;
	}
	else
	{
		// Add better error handling for J3
	}
}

void CCollectionEditor::OnChangeVer2() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
		CCollection *pCollection = m_parent->GetCollection();			
		pCollection->m_rVersion.dwVersionMS = (m_dwVersion1 << 16) | m_dwVersion2;
	}
	else
	{
		// Add better error handling for J3
	}
}

void CCollectionEditor::OnChangeVer3() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
		CCollection *pCollection = m_parent->GetCollection();			
		pCollection->m_rVersion.dwVersionLS = (m_dwVersion3 << 16) | m_dwVersion4;
	}
	else
	{
		// Add better error handling for J3
	}
}

void CCollectionEditor::OnChangeVer4() 
{
	BOOL b;
	b = UpdateData(TRUE);	

	if(b)
	{
		CCollection *pCollection = m_parent->GetCollection();			
		pCollection->m_rVersion.dwVersionLS = (m_dwVersion3 << 16) | m_dwVersion4;
	}
	else
	{
		// Add better error handling for J3
	}
}

void CCollectionEditor::OnChangeWaveCount() 
{
	ASSERT(FALSE);
}
