// $$treeview_hfile$$.h : $$TREEVIEW_CLASS$$ クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class $$DOC_CLASS$$;

class $$TREEVIEW_CLASS$$ : public CTreeView
{
protected: // シリアライズ機能のみから作成します。
	$$TREEVIEW_CLASS$$();
	DECLARE_DYNCREATE($$TREEVIEW_CLASS$$)

// アトリビュート
public:
	$$DOC_CLASS$$* GetDocument();

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL($$TREEVIEW_CLASS$$)
	public:
	virtual void OnDraw(CDC* pDC);  // このビューを描画する際にオーバーライドされます。
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
$$IF(PRINT)
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
$$ENDIF
	virtual void OnInitialUpdate(); // 構築後の最初の１度だけ呼び出されます。
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~$$TREEVIEW_CLASS$$();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG($$TREEVIEW_CLASS$$)
$$IF(VERBOSE)
		// メモ -  ClassWizard はこの位置にメンバ関数を追加または削除します。
		//         この位置に生成されるコードを編集しないでください。
$$ENDIF
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // $$treeview_ifile$$.cpp ファイルがデバッグ環境の時使用されます。
inline $$DOC_CLASS$$* $$TREEVIEW_CLASS$$::GetDocument()
   { return ($$DOC_CLASS$$*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
