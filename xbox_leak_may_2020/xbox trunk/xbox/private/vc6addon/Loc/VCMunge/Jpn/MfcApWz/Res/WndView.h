// $$wndview_hfile$$.h : $$WNDVIEW_CLASS$$ クラスのインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// $$WNDVIEW_CLASS$$ ウィンドウ

class $$WNDVIEW_CLASS$$ : public $$WNDVIEW_BASE_CLASS$$
{
// コンストラクタ
public:
	$$WNDVIEW_CLASS$$();

// アトリビュート
public:

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL($$WNDVIEW_CLASS$$)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~$$WNDVIEW_CLASS$$();

	// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG($$WNDVIEW_CLASS$$)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ が新しい定義を加える場合には、この行よりも前に追加します。

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
