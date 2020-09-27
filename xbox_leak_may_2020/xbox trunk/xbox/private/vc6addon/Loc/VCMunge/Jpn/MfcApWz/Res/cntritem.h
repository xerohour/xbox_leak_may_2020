// $$cntritem_hfile$$.h : $$CNTRITEM_CLASS$$ クラスの宣言およびインターフェイスの定義をします。
//

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class $$DOC_CLASS$$;
class $$VIEW_CLASS$$;

class $$CNTRITEM_CLASS$$ : public $$CNTRITEM_BASE_CLASS$$
{
	DECLARE_SERIAL($$CNTRITEM_CLASS$$)

// コンストラクタ
public:
$$IF(CRichEditView)
	$$CNTRITEM_CLASS$$(REOBJECT* preo = NULL, $$DOC_CLASS$$* pContainer = NULL);
$$ELSE
	$$CNTRITEM_CLASS$$($$DOC_CLASS$$* pContainer = NULL);
$$ENDIF //CRichEditView
$$IF(VERBOSE)
		// メモ: pContainer を NULL にすると IMPLEMENT_SERIALIZE を行うことができます。
		//  IMPLEMENT_SERIALIZE を行うためにはクラス内に引数のないコンストラクタが必要です。
		//  通常、OLE アイテムは NULL でないドキュメント ポインタで組み込まれています。
		//
$$ENDIF

// アトリビュート
public:
	$$DOC_CLASS$$* GetDocument()
		{ return ($$DOC_CLASS$$*)$$CNTRITEM_BASE_CLASS$$::GetDocument(); }
	$$VIEW_CLASS$$* GetActiveView()
		{ return ($$VIEW_CLASS$$*)$$CNTRITEM_BASE_CLASS$$::GetActiveView(); }

	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL($$CNTRITEM_CLASS$$)
	public:
$$IF(!CRichEditView)
	virtual void OnChange(OLE_NOTIFICATION wNotification, DWORD dwParam);
	virtual void OnActivate();
$$ENDIF //!CRichEditView
	protected:
$$IF(!CRichEditView)
$$IF(!ACTIVE_DOC_CONTAINER)
	virtual void OnGetItemPosition(CRect& rPosition);
$$ENDIF
	virtual void OnDeactivateUI(BOOL bUndoable);
	virtual BOOL OnChangeItemPosition(const CRect& rectPos);
$$IF(CONTAINER_SERVER)
	virtual BOOL CanActivate();
$$ENDIF
$$ENDIF //!CRichEditView
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	~$$CNTRITEM_CLASS$$();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
$$IF(!CRichEditView)
	virtual void Serialize(CArchive& ar);
$$ENDIF //!CRichEditView
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
