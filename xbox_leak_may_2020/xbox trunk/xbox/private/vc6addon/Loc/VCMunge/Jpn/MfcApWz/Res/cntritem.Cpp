// $$cntritem_ifile$$.cpp : $$CNTRITEM_CLASS$$ クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "$$root$$.h"

$$IF(CRecordView || CDaoRecordView || COleDBRecordView)
#include "$$recset_hfile$$.h"
$$ENDIF
#include "$$doc_hfile$$.h"
#include "$$view_hfile$$.h"
#include "$$cntritem_hfile$$.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$CNTRITEM_CLASS$$ インプリメンテーション

IMPLEMENT_SERIAL($$CNTRITEM_CLASS$$, $$CNTRITEM_BASE_CLASS$$, 0)

$$IF(CRichEditView)
$$CNTRITEM_CLASS$$::$$CNTRITEM_CLASS$$(REOBJECT* preo, $$DOC_CLASS$$* pContainer)
	: $$CNTRITEM_BASE_CLASS$$(preo, pContainer)
$$ELSE //!CRichEditView
$$CNTRITEM_CLASS$$::$$CNTRITEM_CLASS$$($$DOC_CLASS$$* pContainer)
	: $$CNTRITEM_BASE_CLASS$$(pContainer)
$$ENDIF //CRichEditView
{
$$IF(VERBOSE)
	// TODO: この場所に構築処理用のコードを追加してください。
	
$$ENDIF
}

$$CNTRITEM_CLASS$$::~$$CNTRITEM_CLASS$$()
{
$$IF(VERBOSE)
	// TODO: この場所に後処理用コードを追加してください。
	
$$ENDIF
}
$$IF(!CRichEditView)

void $$CNTRITEM_CLASS$$::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);

	$$CNTRITEM_BASE_CLASS$$::OnChange(nCode, dwParam);
$$IF(VERBOSE)

	// アイテムが編集されている時には (組み込み先でも 開いていても)
	// アイテムの状態またはその内容の画面表示の変更に対し、OnChange
	// 通知を送ります。

	// TODO: UpdateAllViews を呼び出してアイテムを無効にしてください。
	//  (アプリケーション特有のヒントを添えて)

	GetDocument()->UpdateAllViews(NULL);
		// すべてのビューをヒント情報なしでアップデートしました。
$$ENDIF
}

BOOL $$CNTRITEM_CLASS$$::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);
$$IF(VERBOSE)

	// 組み込み先が有効な間は $$CNTRITEM_CLASS$$::OnChangeItemPosition は組み込み先
	// ウィンドウの位置を変更するためにサーバーによって呼び出されます。通常、これは
	// サーバー ドキュメントのデータを変更したことによって範囲が変わったり、組み込み
	// 先のサイズを変更したことの結果です。
	//
	// アイテムを新しい位置に移動するために基本
	// クラスの $$CNTRITEM_BASE_CLASS$$::SetItemRects を呼び出します。
	//
	//
$$ENDIF

	if (!$$CNTRITEM_BASE_CLASS$$::OnChangeItemPosition(rectPos))
		return FALSE;

$$IF(VERBOSE)
	// TODO: アイテムの矩形/範囲のキャッシュを更新してください。

$$ENDIF
	return TRUE;
}

$$IF(!ACTIVE_DOC_CONTAINER)
void $$CNTRITEM_CLASS$$::OnGetItemPosition(CRect& rPosition)
{
	ASSERT_VALID(this);
$$IF(VERBOSE)

	// 組み込み先が起動している間、$$CNTRITEM_CLASS$$::OnGetItemPosition はアイテムの
	// 位置を決定するために呼ばれます。AppWizard で作成されたデフォルトのコードは単純
	// にハードコーディングされた矩形を返します。
	// 通常、この矩形は使われているビューに関連するアイテムの現在の位置を反映します。
	// $$CNTRITEM_CLASS$$::GetActiveView を呼び出すことによってビューを得ることができ
	// ます。

	// TODO: rPosition にはピクセル単位で正しい矩形を返してください。
$$ENDIF

	rPosition.SetRect(10, 10, 210, 210);
}
$$ENDIF

void $$CNTRITEM_CLASS$$::OnActivate()
{
$$IF(!ACTIVE_DOC_CONTAINER)
    // フレームごとに一つだけアイテムのビジュアル編集を可能にします。
    $$VIEW_CLASS$$* pView = GetActiveView();
    ASSERT_VALID(pView);
    COleClientItem* pItem = GetDocument()->GetInPlaceActiveItem(pView);
    if (pItem != NULL && pItem != this)
        pItem->Close();
    
    $$CNTRITEM_BASE_CLASS$$::OnActivate();
$$ENDIF
}

void $$CNTRITEM_CLASS$$::OnDeactivateUI(BOOL bUndoable)
{
	$$CNTRITEM_BASE_CLASS$$::OnDeactivateUI(bUndoable);

    // オブジェクトがアウトサイド イン オブジェクトではない場合は非表示にします。
    DWORD dwMisc = 0;
    m_lpObject->GetMiscStatus(GetDrawAspect(), &dwMisc);
    if (dwMisc & OLEMISC_INSIDEOUT)
        DoVerb(OLEIVERB_HIDE, NULL);
}

void $$CNTRITEM_CLASS$$::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

$$IF(VERBOSE)
	// 最初の基本クラスの呼び出しは $$CNTRITEM_BASE_CLASS$$
	// データへ読み込まれます。$$CNTRITEM_CLASS$$::GetDocument
	// から返された m_pDocument ポインタを設定してから基本クラ
	// スを呼び出して最初のシリアライズを行ってください。
$$ENDIF
	$$CNTRITEM_BASE_CLASS$$::Serialize(ar);

$$IF(VERBOSE)
	// $$CNTRITEM_CLASS$$ に対してデータの保存/読み込みを行います。
$$ENDIF
	if (ar.IsStoring())
	{
$$IF(VERBOSE)
		// TODO: この場所に保存用のコードを追加します。
$$ENDIF
	}
	else
	{
$$IF(VERBOSE)
		// TODO: この場所に読み込み用のコードを追加します。
$$ENDIF
	}
}
$$IF(CONTAINER_SERVER)

BOOL $$CNTRITEM_CLASS$$::CanActivate()
{
$$IF(VERBOSE)
	// サーバーで編集されているときには組み込み先での編集は
	// サポートされていません。 この場合は組み込み先の起動  
	// を無効にしてください。
$$ENDIF
	$$DOC_CLASS$$* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	ASSERT(pDoc->IsKindOf(RUNTIME_CLASS(COleServerDoc)));
	if (pDoc->IsInPlaceActive())
		return FALSE;

$$IF(VERBOSE)
	// その他はデフォルトの処理に任せます。
$$ENDIF
	return COleClientItem::CanActivate();
}
$$ENDIF
$$ENDIF //!CRichEditView

/////////////////////////////////////////////////////////////////////////////
// $$CNTRITEM_CLASS$$ クラスの診断

#ifdef _DEBUG
void $$CNTRITEM_CLASS$$::AssertValid() const
{
	$$CNTRITEM_BASE_CLASS$$::AssertValid();
}

void $$CNTRITEM_CLASS$$::Dump(CDumpContext& dc) const
{
	$$CNTRITEM_BASE_CLASS$$::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
