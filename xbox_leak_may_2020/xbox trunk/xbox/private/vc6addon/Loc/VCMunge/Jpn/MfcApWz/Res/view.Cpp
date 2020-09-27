// $$view_ifile$$.cpp : $$VIEW_CLASS$$ クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "$$root$$.h"

$$IF(CRecordView || CDaoRecordView || COleDBRecordView)
#include "$$recset_hfile$$.h"
$$ENDIF
#include "$$doc_hfile$$.h"
$$IF(CONTAINER || CONTAINER_SERVER)
#include "$$cntritem_hfile$$.h"
$$ENDIF
#include "$$view_hfile$$.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$VIEW_CLASS$$

IMPLEMENT_DYNCREATE($$VIEW_CLASS$$, $$VIEW_BASE_CLASS$$)

BEGIN_MESSAGE_MAP($$VIEW_CLASS$$, $$VIEW_BASE_CLASS$$)
	//{{AFX_MSG_MAP($$VIEW_CLASS$$)
$$IF(VERBOSE)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//    この位置に生成されるコードを編集しないでください。
$$ENDIF
$$IF(CONTAINER || CONTAINER_SERVER)
	ON_WM_DESTROY()
$$IF(!CRichEditView)
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_COMMAND(ID_OLE_INSERT_NEW, OnInsertObject)
	ON_COMMAND(ID_CANCEL_EDIT_CNTR, OnCancelEditCntr)
$$ENDIF //!CRichEditView
$$ENDIF //CONTAINERS
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	ON_COMMAND(ID_CANCEL_EDIT_SRVR, OnCancelEditSrvr)
$$ENDIF
	//}}AFX_MSG_MAP
$$IF(PRINT)
	// 標準印刷コマンド
	ON_COMMAND(ID_FILE_PRINT, $$VIEW_BASE_CLASS$$::OnFilePrint)
$$IF(!CHtmlView)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, $$VIEW_BASE_CLASS$$::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, $$VIEW_BASE_CLASS$$::OnFilePrintPreview)
$$ENDIF
$$ENDIF //PRINT
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// $$VIEW_CLASS$$ クラスの構築/消滅

$$VIEW_CLASS$$::$$VIEW_CLASS$$()
$$IF(CFormView || CRecordView || CDaoRecordView || COleDBRecordView)
	: $$VIEW_BASE_CLASS$$($$VIEW_CLASS$$::IDD)
$$ENDIF
{
$$IF(CFormView || CRecordView || CDaoRecordView || COleDBRecordView)
	//{{AFX_DATA_INIT($$VIEW_CLASS$$)
		// メモ: ClassWizard はこの位置にメンバの初期化処理を追加します。
$$IF(|| CRecordView || CDaoRecordView || COleDBRecordView)
	m_pSet = NULL;
$$ENDIF
	//}}AFX_DATA_INIT
$$ENDIF
$$IF(CONTAINER || CONTAINER_SERVER)
$$IF(!CRichEditView)
	m_pSelection = NULL;
$$ENDIF //!CRichEditView
$$ENDIF //CONTAINERS
$$IF(VERBOSE)
	// TODO: この場所に構築用のコードを追加してください。

$$ENDIF
}

$$VIEW_CLASS$$::~$$VIEW_CLASS$$()
{
}
$$IF(CFormView || CRecordView || CDaoRecordView || COleDBRecordView)

void $$VIEW_CLASS$$::DoDataExchange(CDataExchange* pDX)
{
	$$VIEW_BASE_CLASS$$::DoDataExchange(pDX);
	//{{AFX_DATA_MAP($$VIEW_CLASS$$)
		// メモ: ClassWizard はこの位置に DDX と DDV の呼び出しを追加します。
	//}}AFX_DATA_MAP
}
$$ENDIF

BOOL $$VIEW_CLASS$$::PreCreateWindow(CREATESTRUCT& cs)
{
$$IF(VERBOSE)
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイルを
	//  修正してください。

$$ENDIF //VERBOSE
$$IF(CEditView)
	BOOL bPreCreated = CEditView::PreCreateWindow(cs);
	cs.style &= ~(ES_AUTOHSCROLL|WS_HSCROLL);	// ワード ラップを使用可能にします

	return bPreCreated;
$$ELSE //!CEditView
	return $$VIEW_BASE_CLASS$$::PreCreateWindow(cs);
$$ENDIF //CEditView
}
$$IF(CFormView || CRecordView || CDaoRecordView || COleDBRecordView)
$$ELIF(!CRichEditView)

/////////////////////////////////////////////////////////////////////////////
// $$VIEW_CLASS$$ クラスの描画

void $$VIEW_CLASS$$::OnDraw(CDC* pDC)
{
	$$DOC_CLASS$$* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
$$IF(PROJTYPE_EXPLORER)
$$IF(CListView)
	CListCtrl& refCtrl = GetListCtrl();
	refCtrl.InsertItem(0, "Item!");
$$ENDIF
$$ENDIF
$$IF(VERBOSE)
	// TODO: この場所にネイティブ データ用の描画コードを追加します。
$$ENDIF //VERBOSE
$$IF(CONTAINER || CONTAINER_SERVER)
$$IF(!ACTIVE_DOC_CONTAINER)
$$IF(!CRichEditView)
$$IF(VERBOSE)
	// TODO: ドキュメント内のすべての OLE アイテムも描画してください。

	// 選択範囲を適当な位置に描画します。このコードは実際の描画用のコードが
	// インプリメントされてから削除してください。組み込み先編集を有効にする
	// ために、この位置は $$CNTRITEM_CLASS$$ が返してきた矩形と正確に一致
	// します。

	// TODO: 最終的な描画用のコードが確定したときにはこのコードは削除してください。
$$ENDIF //VERBOSE

	if (m_pSelection == NULL)
	{
		POSITION pos = pDoc->GetStartPosition();
		m_pSelection = ($$CNTRITEM_CLASS$$*)pDoc->GetNextClientItem(pos);
	}
	if (m_pSelection != NULL)
		m_pSelection->Draw(pDC, CRect(10, 10, 210, 210));
$$ENDIF //!CRichEditView
$$ENDIF //ACTIVE_DOC_CONTAINER
$$ENDIF //CONTAINERS
}
$$ENDIF // !(CFormView || CRecordView || CDaoRecordView)
$$IF(CScrollView || CONTAINER || CONTAINER_SERVER || CRecordView || CDaoRecordView || COleDBRecordView || CListView || CTreeView || CFormView || CHtmlView)

void $$VIEW_CLASS$$::OnInitialUpdate()
{
$$IF(CRecordView || CDaoRecordView || COleDBRecordView)
	m_pSet = &GetDocument()->$$RECSET_VARIABLE$$;
$$IF(COleDBRecordView)
	{
		CWaitCursor wait;
		HRESULT hr = m_pSet->Open();
		if (hr != S_OK)
		{
			AfxMessageBox(_T("Record set failed to open."), MB_OK);
$$IF(VERBOSE)
			// Recordet を開かずに現在のレコードを変更しようとすると
			// クラッシュの原因となるため、次 および 前 のレコード
			// コマンドを使用不能にします。
$$ENDIF
			m_bOnFirstRecord = TRUE;
			m_bOnLastRecord = TRUE;
		}				
	}
$$ENDIF
$$ENDIF
	$$VIEW_BASE_CLASS$$::OnInitialUpdate();
$$IF(CRecordView || CDaoRecordView || CFormView)
$$IF(PROJTYPE_SDI)
	GetParentFrame()->RecalcLayout();
$$ENDIF
	ResizeParentToFit();
$$ENDIF

$$IF(VERBOSE)
$$IF(CListView)

	// TODO: GetListCtrl() メンバ関数の呼び出しを通して直接そのリスト コントロールに
	//  アクセスすることによって ListView をアイテムで固定できます。
$$ELIF(CTreeView)

	// TODO:  GetTreeCtrl() メンバ関数の呼び出しを通して直接そのリスト コントロールに
	//  アクセスすることによって TreeView をアイテムで固定できます。
$$ELIF(CHtmlView)
	// TODO: このコードはポピュラーなウェッブ サイトへナビゲートします
	// 任意のウェッブ サイトに変更してください
$$ENDIF //Views
$$ENDIF //VERBOSE
$$IF(CHtmlView)
	Navigate2(_T("http://www.microsoft.com/visualc/"),NULL,NULL);
$$ENDIF //CHTMLVIEW
$$IF(CONTAINER || CONTAINER_SERVER)
$$IF(!CRichEditView)

$$IF(VERBOSE)
	// TODO: 最終的な選択方式のコードが確定したときにはこのコードは削除してください。
$$ENDIF //VERBOSE
	m_pSelection = NULL;    // 選択の初期化

$$ENDIF //!CRichEditView
$$IF(ACTIVE_DOC_CONTAINER)
	//Active documents should always be activated
	COleDocument* pDoc = (COleDocument*) GetDocument();
	if (pDoc != NULL)
	{
		// activate the first one
		POSITION posItem = pDoc->GetStartPosition();
		if (posItem != NULL)
		{
			CDocItem* pItem = pDoc->GetNextItem(posItem);

			// only if it's an Active document
			COleDocObjectItem *pDocObjectItem =
				DYNAMIC_DOWNCAST(COleDocObjectItem, pItem);

			if (pDocObjectItem != NULL)
			{
				pDocObjectItem->DoVerb(OLEIVERB_SHOW, this);
			}
		}
	}
$$ENDIF
$$IF(CRichEditView)

$$IF(VERBOSE)
	// 印刷時の余白を設定します (720 twips = 1/2 inch).
$$ENDIF //VERBOSE
	SetMargins(CRect(720, 720, 720, 720));
$$ENDIF	//CRichEditView
$$ENDIF //CONTAINERS
$$IF(CScrollView)
	CSize sizeTotal;
$$IF(VERBOSE)
	// TODO: このビューのサイズの合計を計算します。
$$ENDIF
	sizeTotal.cx = sizeTotal.cy = 100;
	SetScrollSizes(MM_TEXT, sizeTotal);
$$ENDIF
}
$$ENDIF // CScrollView || CONTAINER || CONTAINER_SERVER || CRecordView || CDaoRecordView || CFormView || CHtmlView
$$IF(PRINT)

/////////////////////////////////////////////////////////////////////////////
// $$VIEW_CLASS$$ クラスの印刷

$$IF(!CHtmlView)
BOOL $$VIEW_CLASS$$::OnPreparePrinting(CPrintInfo* pInfo)
{
$$IF(CEditView)
	// デフォルトの CEditView の印刷準備
	return CEditView::OnPreparePrinting(pInfo);
$$ELSE
$$IF(ACTIVE_DOC_CONTAINER)
	if (!CView::DoPreparePrinting(pInfo))
		return FALSE;
	
	if (!COleDocObjectItem::OnPreparePrinting(this, pInfo))
		return FALSE;

	return TRUE;
$$ELSE
	// デフォルトの印刷準備
	return DoPreparePrinting(pInfo);
$$ENDIF // ACTIVE_DOC_CONTAINER
$$ENDIF // CEditView
}

$$IF(!CRichEditView)
$$IF(CEditView)
void $$VIEW_CLASS$$::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
$$ELSE
void $$VIEW_CLASS$$::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
$$ENDIF //CEditView
{
$$IF(CEditView)
$$IF(VERBOSE)
	// デフォルトの CEditView で印刷を開始します。
$$ENDIF //VERBOSE
	CEditView::OnBeginPrinting(pDC, pInfo);
$$ELSE
$$IF(VERBOSE)
	// TODO: 印刷前の特別な初期化処理を追加してください。
$$ENDIF //VERBOSE
$$ENDIF //CEditView
}

$$IF(CEditView)
void $$VIEW_CLASS$$::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
$$ELSE
void $$VIEW_CLASS$$::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
$$ENDIF //CEditView
{
$$IF(CEditView)
$$IF(VERBOSE)
	// デフォルトの CEditView の印刷後の後処理
$$ENDIF //VERBOSE
	CEditView::OnEndPrinting(pDC, pInfo);
$$ELSE
$$IF(VERBOSE)
	// TODO: 印刷後の後処理を追加してください。
$$ENDIF //VERBOSE
$$ENDIF //CEditView
}
$$ENDIF //!CRichEditView
$$ENDIF // !CHtmlView
$$IF(CFormView || ACTIVE_DOC_CONTAINER)

$$IF(ACTIVE_DOC_CONTAINER)
void $$VIEW_CLASS$$::OnPrint(CDC* pDC, CPrintInfo* pInfo)
$$ELSE
void $$VIEW_CLASS$$::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
$$ENDIF
{
$$IF(VERBOSE)
	// TODO: 印刷用のコードをここに追加してください。
$$ENDIF
$$IF(ACTIVE_DOC_CONTAINER)
	if(pInfo->m_bDocObject)
		COleDocObjectItem::OnPrint(this, pInfo, TRUE);
	else
		CView::OnPrint(pDC, pInfo);
$$ENDIF
}
$$ENDIF //CFormView || OLEDOCCONTAINER
$$ENDIF //PRINT
$$IF(CONTAINER || CONTAINER_SERVER)

void $$VIEW_CLASS$$::OnDestroy()
{
$$IF(VERBOSE)
	// 消滅時にアイテムを非アクティベートします; これは分割ビューが
	// 使用されている場合には重要です。
$$ENDIF
   $$VIEW_BASE_CLASS$$::OnDestroy();
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
   }
}

$$IF(!CRichEditView)

/////////////////////////////////////////////////////////////////////////////
// OLE クライアントのサポートとコマンド

BOOL $$VIEW_CLASS$$::IsSelected(const CObject* pDocItem) const
{
$$IF(VERBOSE)
	// $$CNTRITEM_CLASS$$ オブジェクトだけで構成されている時には
	// 以下に示す方法で十分です。異なる選択方式をハンドルするときには
	// この場所を書き換えてください。

	// TODO: この関数は選択された OLE クライアントのアイテムに対するテストを実現します。

$$ENDIF
	return pDocItem == m_pSelection;
}

void $$VIEW_CLASS$$::OnInsertObject()
{
$$IF(VERBOSE)
	// 新規の $$CNTRITEM_CLASS$$ オブジェクトの情報を得るための
	// 標準的なオブジェクト挿入ダイアログ ボックスを呼び出します。
$$ENDIF
	COleInsertDialog dlg;
$$IF(ACTIVE_DOC_CONTAINER)
	if (dlg.DoModal(COleInsertDialog::DocObjectsOnly) != IDOK)
		return;
$$ELSE
	if (dlg.DoModal() != IDOK)
		return;
$$ENDIF

	BeginWaitCursor();

	$$CNTRITEM_CLASS$$* pItem = NULL;
	TRY
	{
$$IF(VERBOSE)
		// このドキュメントに接続された新規のアイテムを作成します。
$$ENDIF
		$$DOC_CLASS$$* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pItem = new $$CNTRITEM_CLASS$$(pDoc);
		ASSERT_VALID(pItem);

$$IF(VERBOSE)
		// ダイアログ データからアイテムを初期化します。
$$ENDIF
		if (!dlg.CreateItem(pItem))
			AfxThrowMemoryException();  // 例外処理を行います。
		ASSERT_VALID(pItem);

$$IF(ACTIVE_DOC_CONTAINER)
		pItem->DoVerb(OLEIVERB_SHOW, this);
$$ELSE
        if (dlg.GetSelectionType() == COleInsertDialog::createNewItem)
			pItem->DoVerb(OLEIVERB_SHOW, this);
$$ENDIF

		ASSERT_VALID(pItem);
$$IF(VERBOSE)

		// 暫定的なユーザー インターフェイスのデザインとして最後に挿入された
		// アイテムを選択しています。

		// TODO: アプリケーションに適当な選択方式にあわせて書き直してください。
$$ENDIF

		m_pSelection = pItem;   // 最後に挿入されたアイテムを選択
		pDoc->UpdateAllViews(NULL);
	}
	CATCH(CException, e)
	{
		if (pItem != NULL)
		{
			ASSERT_VALID(pItem);
			pItem->Delete();
		}
		AfxMessageBox(IDP_FAILED_TO_CREATE);
	}
	END_CATCH

	EndWaitCursor();
}

$$IF(VERBOSE)
// 以下に示すコマンド ハンドラは組み込み先編集を中止するための標準的な
// キーボード ユーザー インターフェイスを用意しています。ここでコンテナ
// （サーバーではない）は、ディアクティベーションを引き起こします。
$$ENDIF
void $$VIEW_CLASS$$::OnCancelEditCntr()
{
	// このビュー上にある組み込み先で有効なアイテムを閉じます。
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
	{
		pActiveItem->Close();
	}
	ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
}

$$IF(VERBOSE)
// オブジェクトが組み込み先で編集されはじめたときには OnSetFocus と OnSize の特別な
// ハンドリングにはコンテナが必要です。
$$ENDIF
void $$VIEW_CLASS$$::OnSetFocus(CWnd* pOldWnd)
{
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL &&
		pActiveItem->GetItemState() == COleClientItem::activeUIState)
	{
		// 同じビューにあるアイテムにはフォーカスを設定する必要があります。
		CWnd* pWnd = pActiveItem->GetInPlaceWindow();
		if (pWnd != NULL)
		{
			pWnd->SetFocus();   // 基本クラスを呼び出してはいけません。
			return;
		}
	}

	$$VIEW_BASE_CLASS$$::OnSetFocus(pOldWnd);
}

void $$VIEW_CLASS$$::OnSize(UINT nType, int cx, int cy)
{
	$$VIEW_BASE_CLASS$$::OnSize(nType, cx, cy);
	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
		pActiveItem->SetItemRects();
}
$$ENDIF //!CRichEditView
$$ENDIF //CONTAINER || CONTAINER_SERVER
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)

/////////////////////////////////////////////////////////////////////////////
// OLE サーバー サポート

$$IF(VERBOSE)
// 以下のコマンドハンドラは埋めこみ編集セッションをキャンセルするスタンダード キー
// ボード ユーザー インタフェースを用意します。
// ここで、サーバー (コンテナではない) は、ディアクティベーションを引き起こします。
$$ENDIF
void $$VIEW_CLASS$$::OnCancelEditSrvr()
{
	GetDocument()->OnDeactivateUI(FALSE);
}
$$ENDIF //SERVERS

/////////////////////////////////////////////////////////////////////////////
// $$VIEW_CLASS$$ クラスの診断

#ifdef _DEBUG
void $$VIEW_CLASS$$::AssertValid() const
{
	$$VIEW_BASE_CLASS$$::AssertValid();
}

void $$VIEW_CLASS$$::Dump(CDumpContext& dc) const
{
	$$VIEW_BASE_CLASS$$::Dump(dc);
}

$$DOC_CLASS$$* $$VIEW_CLASS$$::GetDocument() // 非デバッグ バージョンはインラインです。
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS($$DOC_CLASS$$)));
	return ($$DOC_CLASS$$*)m_pDocument;
}
#endif //_DEBUG
$$IF(CRecordView || CDaoRecordView ||COleDBRecordView)

/////////////////////////////////////////////////////////////////////////////
// $$VIEW_CLASS$$ データベースのサポート
$$IF(CRecordView)
CRecordset* $$VIEW_CLASS$$::OnGetRecordset()
$$ELIF(CDaoRecordView)
CDaoRecordset* $$VIEW_CLASS$$::OnGetRecordset()
$$ELSE 
CRowset* $$VIEW_CLASS$$::OnGetRowset()
$$ENDIF
{
	return m_pSet;
}

$$ENDIF //CRecordView || CDaoRecordView

/////////////////////////////////////////////////////////////////////////////
// $$VIEW_CLASS$$ クラスのメッセージ ハンドラ
$$IF(PROJTYPE_EXPLORER)
$$IF(CListView)
void $$VIEW_CLASS$$::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
$$IF(VERBOSE)
	// TODO: ユーザーによるウインドウのビュー スタイル変更に対応するコードを追加してください
$$ENDIF
}
$$ENDIF
$$ENDIF
