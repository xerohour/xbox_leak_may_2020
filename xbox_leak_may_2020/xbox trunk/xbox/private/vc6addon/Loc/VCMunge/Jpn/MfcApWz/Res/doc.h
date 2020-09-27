// $$doc_hfile$$.h : $$DOC_CLASS$$ クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
$$IF(CRecordView || CDaoRecordView || COleDBRecordView)
#include "$$recset_hfile$$.h"
$$ENDIF

$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)

class $$SRVRITEM_CLASS$$;
$$ENDIF // SERVERS

class $$DOC_CLASS$$ : public $$DOC_BASE_CLASS$$
{
protected: // シリアライズ機能のみから作成します。
	$$DOC_CLASS$$();
	DECLARE_DYNCREATE($$DOC_CLASS$$)

// アトリビュート
public:
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	$$SRVRITEM_CLASS$$* GetEmbeddedItem()
		{ return ($$SRVRITEM_CLASS$$*)$$DOC_BASE_CLASS$$::GetEmbeddedItem(); }
$$ENDIF
$$IF(CRecordView || CDaoRecordView || COleDBRecordView)
	$$RECSET_CLASS$$ $$RECSET_VARIABLE$$;
$$ENDIF

// オペレーション
public:

//オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL($$DOC_CLASS$$)
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	protected:
	virtual COleServerItem* OnGetEmbeddedItem();
$$ENDIF
	public:
	virtual BOOL OnNewDocument();
$$IF(!DB_NO_FILE)
	virtual void Serialize(CArchive& ar);
$$ENDIF
	//}}AFX_VIRTUAL
$$IF(CRichEditView)
	virtual $$CNTRITEM_BASE_CLASS$$* CreateClientItem(REOBJECT* preo) const;
$$ENDIF //CRichEditView

// インプリメンテーション
public:
	virtual ~$$DOC_CLASS$$();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
$$IF(ACTIVE_DOC_SERVER)
	virtual CDocObjectServer* GetDocObjectServer(LPOLEDOCUMENTSITE pDocSite);
$$ENDIF

// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG($$DOC_CLASS$$)
$$IF(VERBOSE)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
$$ENDIF
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
$$IF(AUTOMATION)

	// 生成された OLE ディスパッチ マップ関数
	//{{AFX_DISPATCH($$DOC_CLASS$$)
$$IF(VERBOSE)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
$$ENDIF
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
$$ENDIF
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
