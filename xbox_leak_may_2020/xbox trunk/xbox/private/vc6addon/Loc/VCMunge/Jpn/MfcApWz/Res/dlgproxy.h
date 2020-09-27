// $$dlgautoproxy_hfile$$.h : ヘッダー ファイル
//

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class $$DLG_CLASS$$;

/////////////////////////////////////////////////////////////////////////////
// $$DLGAUTOPROXY_CLASS$$ コマンド ターゲット

class $$DLGAUTOPROXY_CLASS$$ : public $$DLGAUTOPROXY_BASE_CLASS$$
{
	DECLARE_DYNCREATE($$DLGAUTOPROXY_CLASS$$)

	$$DLGAUTOPROXY_CLASS$$();           // 動的生成で使用される protected コンストラクタ

// アトリビュート
public:
	$$DLG_CLASS$$* m_pDialog;

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します
	//{{AFX_VIRTUAL($$DLGAUTOPROXY_CLASS$$)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	virtual ~$$DLGAUTOPROXY_CLASS$$();

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG($$DLGAUTOPROXY_CLASS$$)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_OLECREATE($$DLGAUTOPROXY_CLASS$$)

	// 生成された OLE ディスパッチ マップ関数
	//{{AFX_DISPATCH($$DLGAUTOPROXY_CLASS$$)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
