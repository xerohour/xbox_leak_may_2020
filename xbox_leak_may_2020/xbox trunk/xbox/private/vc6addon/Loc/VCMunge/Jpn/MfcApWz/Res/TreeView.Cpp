// $$treeview_ifile$$.cpp : $$TREEVIEW_CLASS$$ �N���X�̓���̒�`���s���܂��B
//

#include "stdafx.h"
#include "$$root$$.h"

#include "$$doc_hfile$$.h"
#include "$$treeview_hfile$$.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$

IMPLEMENT_DYNCREATE($$TREEVIEW_CLASS$$, $$TREEVIEW_BASE_CLASS$$)

BEGIN_MESSAGE_MAP($$TREEVIEW_CLASS$$, $$TREEVIEW_BASE_CLASS$$)
	//{{AFX_MSG_MAP($$TREEVIEW_CLASS$$)
$$IF(VERBOSE)
		// ���� - ClassWizard �͂��̈ʒu�Ƀ}�b�s���O�p�̃}�N����ǉ��܂��͍폜���܂��B
		//        ���̈ʒu�ɐ��������R�[�h��ҏW���Ȃ��ł��������B
$$ENDIF
	//}}AFX_MSG_MAP
$$IF(PRINT)
	// �W������R�}���h
	ON_COMMAND(ID_FILE_PRINT, $$TREEVIEW_BASE_CLASS$$::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, $$TREEVIEW_BASE_CLASS$$::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, $$TREEVIEW_BASE_CLASS$$::OnFilePrintPreview)
$$ENDIF //PRINT
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$ �N���X�̍\�z/����

$$TREEVIEW_CLASS$$::$$TREEVIEW_CLASS$$()
{
$$IF(VERBOSE)
	// TODO: ���̏ꏊ�ɍ\�z�p�̃R�[�h��ǉ����Ă��������B

$$ENDIF
}

$$TREEVIEW_CLASS$$::~$$TREEVIEW_CLASS$$()
{
}

BOOL $$TREEVIEW_CLASS$$::PreCreateWindow(CREATESTRUCT& cs)
{
$$IF(VERBOSE)
	// TODO: ���̈ʒu�� CREATESTRUCT cs ���C������ Window �N���X�܂��̓X�^�C����
	//  �C�����Ă��������B

$$ENDIF //VERBOSE
	return $$TREEVIEW_BASE_CLASS$$::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$ �N���X�̕`��

void $$TREEVIEW_CLASS$$::OnDraw(CDC* pDC)
{
	$$DOC_CLASS$$* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
$$IF(VERBOSE)

	// TODO: ���̏ꏊ�Ƀl�C�e�B�u �f�[�^�p�̕`��R�[�h��ǉ����܂��B
$$ENDIF //VERBOSE
}

$$IF(PRINT)

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$ �N���X�̈��

BOOL $$TREEVIEW_CLASS$$::OnPreparePrinting(CPrintInfo* pInfo)
{
	// �f�t�H���g�̈������
	return DoPreparePrinting(pInfo);
}

void $$TREEVIEW_CLASS$$::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
$$IF(VERBOSE)
	// TODO: ����O�̓��ʂȏ�����������ǉ����Ă��������B
$$ENDIF //VERBOSE
}

void $$TREEVIEW_CLASS$$::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
$$IF(VERBOSE)
	// TODO: �����̌㏈����ǉ����Ă��������B
$$ENDIF //VERBOSE
}
$$ENDIF //PRINT

void $$TREEVIEW_CLASS$$::OnInitialUpdate()
{
	$$TREEVIEW_BASE_CLASS$$::OnInitialUpdate();
$$IF(VERBOSE)

	// TODO:  GetTreeCtrl() �����o�֐��̌Ăяo����ʂ��Ē��ڂ��̃��X�g �R���g���[����
	//  �A�N�Z�X���邱�Ƃɂ���� TreeView ���A�C�e���ŌŒ�ł��܂��B
$$ENDIF //VERBOSE
}

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$ �N���X�̐f�f

#ifdef _DEBUG
void $$TREEVIEW_CLASS$$::AssertValid() const
{
	$$TREEVIEW_BASE_CLASS$$::AssertValid();
}

void $$TREEVIEW_CLASS$$::Dump(CDumpContext& dc) const
{
	$$TREEVIEW_BASE_CLASS$$::Dump(dc);
}

$$DOC_CLASS$$* $$TREEVIEW_CLASS$$::GetDocument() // ��f�o�b�O �o�[�W�����̓C�����C���ł��B
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS($$DOC_CLASS$$)));
	return ($$DOC_CLASS$$*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$ �N���X�̃��b�Z�[�W �n���h��