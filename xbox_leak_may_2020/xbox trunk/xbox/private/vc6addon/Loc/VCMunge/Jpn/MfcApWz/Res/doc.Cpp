// $$doc_ifile$$.cpp : $$DOC_CLASS$$ クラスの動作の定義を行います。
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
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
#include "$$srvritem_hfile$$.h"
$$ENDIF

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$DOC_CLASS$$

IMPLEMENT_DYNCREATE($$DOC_CLASS$$, $$DOC_BASE_CLASS$$)

BEGIN_MESSAGE_MAP($$DOC_CLASS$$, $$DOC_BASE_CLASS$$)
	//{{AFX_MSG_MAP($$DOC_CLASS$$)
$$IF(VERBOSE)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
$$ENDIF
	//}}AFX_MSG_MAP
$$IF(CONTAINER || CONTAINER_SERVER)
	// デフォルトの OLE コンテナの定義を使用可能にします。
$$IF(!CRichEditView)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, $$DOC_BASE_CLASS$$::OnUpdatePasteMenu)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE_LINK, $$DOC_BASE_CLASS$$::OnUpdatePasteLinkMenu)
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_CONVERT, $$DOC_BASE_CLASS$$::OnUpdateObjectVerbMenu)
	ON_COMMAND(ID_OLE_EDIT_CONVERT, $$DOC_BASE_CLASS$$::OnEditConvert)
$$ENDIF //!CRichEditView
	ON_UPDATE_COMMAND_UI(ID_OLE_EDIT_LINKS, $$DOC_BASE_CLASS$$::OnUpdateEditLinksMenu)
	ON_COMMAND(ID_OLE_EDIT_LINKS, $$DOC_BASE_CLASS$$::OnEditLinks)
	ON_UPDATE_COMMAND_UI_RANGE(ID_OLE_VERB_FIRST, ID_OLE_VERB_LAST, $$DOC_BASE_CLASS$$::OnUpdateObjectVerbMenu)
$$ENDIF
$$IF(MAPI)
	ON_COMMAND(ID_FILE_SEND_MAIL, OnFileSendMail)
	ON_UPDATE_COMMAND_UI(ID_FILE_SEND_MAIL, OnUpdateFileSendMail)
$$ENDIF //MAPI
END_MESSAGE_MAP()

$$IF(AUTOMATION)
BEGIN_DISPATCH_MAP($$DOC_CLASS$$, $$DOC_BASE_CLASS$$)
	//{{AFX_DISPATCH_MAP($$DOC_CLASS$$)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

$$IF(VERBOSE)
// メモ: VBA からバインドされているタイプ セーフをサポートするために IID_I$$Safe_root$$  のサポートを追加します。
//       この IID は、.ODL ファイルの中のディスプインターフェイスにアタッチされている GUID にマッチしていな 
//       ければなりません。 

// {$$DISPIID_CLSID_ODL$$}
$$ENDIF //VERBOSE
static const IID IID_I$$Safe_root$$ =
$$DISPIID_CLSID$$;

BEGIN_INTERFACE_MAP($$DOC_CLASS$$, $$DOC_BASE_CLASS$$)
	INTERFACE_PART($$DOC_CLASS$$, IID_I$$Safe_root$$, Dispatch)
END_INTERFACE_MAP()

$$ENDIF //AUTOMATION
/////////////////////////////////////////////////////////////////////////////
// $$DOC_CLASS$$ クラスの構築/消滅

$$DOC_CLASS$$::$$DOC_CLASS$$()
{
$$IF(COMPFILE)
$$IF(VERBOSE)
	// OLE 複合ファイルを使用します。
$$ENDIF
	EnableCompoundFile();

$$ENDIF //COMPFILE
$$IF(VERBOSE)
	// TODO: この位置に１度だけ呼ばれる構築用のコードを追加してください。

$$ENDIF
$$IF(AUTOMATION)
	EnableAutomation();

	AfxOleLockApp();
$$ENDIF
}

$$DOC_CLASS$$::~$$DOC_CLASS$$()
{
$$IF(AUTOMATION)
	AfxOleUnlockApp();
$$ENDIF
}

BOOL $$DOC_CLASS$$::OnNewDocument()
{
	if (!$$DOC_BASE_CLASS$$::OnNewDocument())
		return FALSE;
$$IF(PROJTYPE_SDI)
$$IF(CEditView)

	((CEditView*)m_viewList.GetHead())->SetWindowText(NULL);
$$ENDIF //CEditView
$$ENDIF //PROJTYPE_SDI

$$IF(VERBOSE)
	// TODO: この位置に再初期化処理を追加してください。
	// (SDI ドキュメントはこのドキュメントを再利用します。)

$$ENDIF
	return TRUE;
}
$$IF(CRichEditView)

$$CNTRITEM_BASE_CLASS$$* $$DOC_CLASS$$::CreateClientItem(REOBJECT* preo) const
{
	// このクラスの const 指定を無効にするためにキャストします
	return new $$CNTRITEM_CLASS$$(preo, ($$DOC_CLASS$$*) this);
}
$$ENDIF //CRichEditView
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)

/////////////////////////////////////////////////////////////////////////////
// $$DOC_CLASS$$ サーバー インプリメンテーション

COleServerItem* $$DOC_CLASS$$::OnGetEmbeddedItem()
{
$$IF(VERBOSE)
	// OnGetEmbeddedItem はドキュメントと関連付けられている COleServerItem 
	// を得るためにフレームワークから必要な時にだけ呼ばれます。

$$ENDIF
	$$SRVRITEM_CLASS$$* pItem = new $$SRVRITEM_CLASS$$(this);
	ASSERT_VALID(pItem);
	return pItem;
}
$$ENDIF
$$IF(ACTIVE_DOC_SERVER)

/////////////////////////////////////////////////////////////////////////////
// $$DOC_CLASS$$ ActiveX ドキュメント サーバー インプリメンテーション

CDocObjectServer *$$DOC_CLASS$$::GetDocObjectServer(LPOLEDOCUMENTSITE pDocSite)
{
	return new CDocObjectServer(this, pDocSite);
}
$$ENDIF


$$IF(!DB_NO_FILE)

/////////////////////////////////////////////////////////////////////////////
// $$DOC_CLASS$$ シリアライゼーション

void $$DOC_CLASS$$::Serialize(CArchive& ar)
{
$$IF(CEditView)
$$IF(VERBOSE)
	// CEditView はすべてのシリアライゼーション ハンドルのどれかのエディットコントロールを含んでいます。
$$ENDIF //VERBOSE
	((CEditView*)m_viewList.GetHead())->SerializeRaw(ar);
$$ELSE
	if (ar.IsStoring())
	{
$$IF(VERBOSE)
		// TODO: この位置に保存用のコードを追加してください。
$$ENDIF
	}
	else
	{
$$IF(VERBOSE)
		// TODO: この位置に読み込み用のコードを追加してください。
$$ENDIF
	}
$$IF(CONTAINER || CONTAINER_SERVER)
$$IF(VERBOSE)

	// 基本クラス $$DOC_BASE_CLASS$$ を呼び出すことによってコンテナ ドキュメントの
	// COleClientItem オブジェクトのシリアライゼーションが可能になります。
$$ENDIF
$$IF(CRichEditView)
	// TODO: テキストとしてシリアライズしたい場合は、CRichEditDoc::m_bRTF をセットしてください。
$$ENDIF //CEditView
	$$DOC_BASE_CLASS$$::Serialize(ar);
$$ENDIF // Container
$$ENDIF // CEditView
}
$$ENDIF //!DB_NO_FILE

/////////////////////////////////////////////////////////////////////////////
// $$DOC_CLASS$$ クラスの診断

#ifdef _DEBUG
void $$DOC_CLASS$$::AssertValid() const
{
	$$DOC_BASE_CLASS$$::AssertValid();
}

void $$DOC_CLASS$$::Dump(CDumpContext& dc) const
{
	$$DOC_BASE_CLASS$$::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// $$DOC_CLASS$$ コマンド
