// $$ipframe_hfile$$.h : $$IPFRAME_CLASS$$ クラスの宣言およびインターフェイスの定義を行います。
//

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class $$IPFRAME_CLASS$$ : public $$IPFRAME_BASE_CLASS$$
{
	DECLARE_DYNCREATE($$IPFRAME_CLASS$$)
public:
	$$IPFRAME_CLASS$$();

// アトリビュート
public:

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL($$IPFRAME_CLASS$$)
$$IF(TOOLBAR)
	public:
	virtual BOOL OnCreateControlBars(CFrameWnd* pWndFrame, CFrameWnd* pWndDoc);
$$ENDIF
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~$$IPFRAME_CLASS$$();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
$$IF(TOOLBAR)
	CToolBar    m_wndToolBar;
$$IF(REBAR)
	CDialogBar m_wndDlgBar;
	CReBar m_wndReBar;
$$ENDIF
$$ENDIF
	COleDropTarget	m_dropTarget;
	COleResizeBar   m_wndResizeBar;

// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG($$IPFRAME_CLASS$$)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
$$IF(VERBOSE)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
$$ENDIF
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
