// $$treeview_ifile$$.cpp : $$TREEVIEW_CLASS$$ クラスの動作の定義を行います。
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
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
$$ENDIF
	//}}AFX_MSG_MAP
$$IF(PRINT)
	// 標準印刷コマンド
	ON_COMMAND(ID_FILE_PRINT, $$TREEVIEW_BASE_CLASS$$::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, $$TREEVIEW_BASE_CLASS$$::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, $$TREEVIEW_BASE_CLASS$$::OnFilePrintPreview)
$$ENDIF //PRINT
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$ クラスの構築/消滅

$$TREEVIEW_CLASS$$::$$TREEVIEW_CLASS$$()
{
$$IF(VERBOSE)
	// TODO: この場所に構築用のコードを追加してください。

$$ENDIF
}

$$TREEVIEW_CLASS$$::~$$TREEVIEW_CLASS$$()
{
}

BOOL $$TREEVIEW_CLASS$$::PreCreateWindow(CREATESTRUCT& cs)
{
$$IF(VERBOSE)
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

$$ENDIF //VERBOSE
	return $$TREEVIEW_BASE_CLASS$$::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$ クラスの描画

void $$TREEVIEW_CLASS$$::OnDraw(CDC* pDC)
{
	$$DOC_CLASS$$* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
$$IF(VERBOSE)

	// TODO: この場所にネイティブ データ用の描画コードを追加します。
$$ENDIF //VERBOSE
}

$$IF(PRINT)

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$ クラスの印刷

BOOL $$TREEVIEW_CLASS$$::OnPreparePrinting(CPrintInfo* pInfo)
{
	// デフォルトの印刷準備
	return DoPreparePrinting(pInfo);
}

void $$TREEVIEW_CLASS$$::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
$$IF(VERBOSE)
	// TODO: 印刷前の特別な初期化処理を追加してください。
$$ENDIF //VERBOSE
}

void $$TREEVIEW_CLASS$$::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
$$IF(VERBOSE)
	// TODO: 印刷後の後処理を追加してください。
$$ENDIF //VERBOSE
}
$$ENDIF //PRINT

void $$TREEVIEW_CLASS$$::OnInitialUpdate()
{
	$$TREEVIEW_BASE_CLASS$$::OnInitialUpdate();
$$IF(VERBOSE)

	// TODO:  GetTreeCtrl() メンバ関数の呼び出しを通して直接そのリスト コントロールに
	//  アクセスすることによって TreeView をアイテムで固定できます。
$$ENDIF //VERBOSE
}

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$ クラスの診断

#ifdef _DEBUG
void $$TREEVIEW_CLASS$$::AssertValid() const
{
	$$TREEVIEW_BASE_CLASS$$::AssertValid();
}

void $$TREEVIEW_CLASS$$::Dump(CDumpContext& dc) const
{
	$$TREEVIEW_BASE_CLASS$$::Dump(dc);
}

$$DOC_CLASS$$* $$TREEVIEW_CLASS$$::GetDocument() // 非デバッグ バージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS($$DOC_CLASS$$)));
	return ($$DOC_CLASS$$*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// $$TREEVIEW_CLASS$$ クラスのメッセージ ハンドラ
