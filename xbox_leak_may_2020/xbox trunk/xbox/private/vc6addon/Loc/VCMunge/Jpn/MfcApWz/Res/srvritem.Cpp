// $$srvritem_ifile$$.cpp : $$SRVRITEM_CLASS$$ アプリケーション用クラスの機能定義を行います。
//

#include "stdafx.h"
#include "$$root$$.h"

$$IF(CRecordView || CDaoRecordView)
#include "$$recset_hfile$$.h"
$$ENDIF
#include "$$doc_hfile$$.h"
#include "$$srvritem_hfile$$.h"
$$IF(CONTAINER_SERVER)
#include "$$cntritem_hfile$$.h"
$$ENDIF

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$SRVRITEM_CLASS$$ インプリメンテーション

IMPLEMENT_DYNAMIC($$SRVRITEM_CLASS$$, $$SRVRITEM_BASE_CLASS$$)

$$SRVRITEM_CLASS$$::$$SRVRITEM_CLASS$$($$DOC_CLASS$$* pContainerDoc)
	: $$SRVRITEM_BASE_CLASS$$(pContainerDoc, TRUE)
{
$$IF(VERBOSE)
	// TODO: この位置に１度だけ呼ばれる構築用のコードを追加してください。
	//  (例えば、アイテム データ ソース用の特別なクリップボード形式の追加)
$$ENDIF
}

$$SRVRITEM_CLASS$$::~$$SRVRITEM_CLASS$$()
{
$$IF(VERBOSE)
	// TODO: この場所に後処理を追加してください。
$$ENDIF
}

void $$SRVRITEM_CLASS$$::Serialize(CArchive& ar)
{
$$IF(VERBOSE)
	// $$SRVRITEM_CLASS$$::Serialize はアイテムがクリップボードにコピーさ
	// れているときには、フレームワークによって呼ばれます。これは OLE コール
	// バック OnGetClipboardData を通り越して自動的に行わせることができます。
	// 埋め込みアイテムのデフォルトは簡単なドキュメントのシリアライズ関数の代
	// わりをします。
	// リンクをサポートしているときは、ドキュメントの一部をシリアライズします。

$$ENDIF
	if (!IsLinkedItem())
	{
		$$DOC_CLASS$$* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		pDoc->Serialize(ar);
	}
}

BOOL $$SRVRITEM_CLASS$$::OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize)
{
$$IF(VERBOSE)
	// 多くのアプリケーションでは、アイテムのコンテンツ アスペクトの描画だけをハンドルします。
	//  OnDrawEx がオーバーライドしている DVASPECT_THUMBNAIL で他のアスペクトもサポートする
	// 時には、OnGetExtent を追加するアスペクトもハンドルするように変更してください。
	//
	//

$$ENDIF
	if (dwDrawAspect != DVASPECT_CONTENT)
		return $$SRVRITEM_BASE_CLASS$$::OnGetExtent(dwDrawAspect, rSize);
$$IF(VERBOSE)

	// $$SRVRITEM_CLASS$$::OnGetExtent はアイテム全体の HIMETRIC 単位の範囲を得る
	// ために 呼ばれます。デフォルトは、単純にハードコーディングされた値を返します。
	//
$$ENDIF

	$$DOC_CLASS$$* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

$$IF(VERBOSE)
	// TODO: 適当なサイズに置き換えてください。

$$ENDIF
	rSize = CSize(3000, 3000);   // 3000 x 3000 HIMETRIC 単位

	return TRUE;
}

BOOL $$SRVRITEM_CLASS$$::OnDraw(CDC* pDC, CSize& rSize)
{
$$IF(VERBOSE)
	// rSize を使用する場合はここを削除してください
$$ENDIF
	UNREFERENCED_PARAMETER(rSize);

	$$DOC_CLASS$$* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

$$IF(VERBOSE)
	// TODO: マッピング モードと範囲の設定。
	//  (範囲は通常 OnGetExtent から返されたサイズと同じです)
$$ENDIF
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowOrg(0,0);
	pDC->SetWindowExt(3000, 3000);

$$IF(VERBOSE)
	// TODO: この場所に描画用のコードを追加します。任意で HIMETRIC の範囲を塗りつぶしてください。
	//       メタファイル デバイス コンテキスト (pDC) のすべてを描画します。

$$IF(CONTAINER_SERVER)
	// TODO: $$CNTRITEM_CLASS$$ の埋め込みオブジェクトも描画してください。

$$ENDIF
$$ENDIF
$$IF(CONTAINER_SERVER)
	// 以下に示すコードは最初のアイテムを適当な位置へ描画します。

	// TODO: このコードは実際の描画用のコードが確定したら削除してください。

	POSITION pos = pDoc->GetStartPosition();
	$$CNTRITEM_CLASS$$* pItem = ($$CNTRITEM_CLASS$$*)pDoc->GetNextClientItem(pos);
	if (pItem != NULL)
		pItem->Draw(pDC, CRect(10, 10, 1010, 1010));
$$ENDIF
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// $$SRVRITEM_CLASS$$ クラスの診断

#ifdef _DEBUG
void $$SRVRITEM_CLASS$$::AssertValid() const
{
	$$SRVRITEM_BASE_CLASS$$::AssertValid();
}

void $$SRVRITEM_CLASS$$::Dump(CDumpContext& dc) const
{
	$$SRVRITEM_BASE_CLASS$$::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
