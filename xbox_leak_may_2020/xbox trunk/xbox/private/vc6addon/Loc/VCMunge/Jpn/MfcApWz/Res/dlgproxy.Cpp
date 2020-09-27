// $$dlgautoproxy_ifile$$.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "$$root$$.h"
#include "$$dlgautoproxy_hfile$$.h"
#include "$$dlg_hfile$$.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$DLGAUTOPROXY_CLASS$$

IMPLEMENT_DYNCREATE($$DLGAUTOPROXY_CLASS$$, $$DLGAUTOPROXY_BASE_CLASS$$)

$$DLGAUTOPROXY_CLASS$$::$$DLGAUTOPROXY_CLASS$$()
{
	EnableAutomation();
	
	// オートメーション オブジェクトがアクティブである限り、アプリケーションを 
	// 実行状態にしてください、コンストラクタは AfxOleLockApp を呼び出します。
	AfxOleLockApp();

$$IF(VERBOSE)
	// アプリケーションのメイン ウィンドウ ポインタを通してダイアログ
	// へアクセスします。プロキシの内部ポインタからダイアログへのポイ
	// ンタを設定し、ダイアログの戻りポインタをこのプロキシへ設定しま
	// す。
$$ENDIF //VERBOSE
	ASSERT (AfxGetApp()->m_pMainWnd != NULL);
	ASSERT_VALID (AfxGetApp()->m_pMainWnd);
	ASSERT_KINDOF($$DLG_CLASS$$, AfxGetApp()->m_pMainWnd);
	m_pDialog = ($$DLG_CLASS$$*) AfxGetApp()->m_pMainWnd;
	m_pDialog->m_pAutoProxy = this;
}

$$DLGAUTOPROXY_CLASS$$::~$$DLGAUTOPROXY_CLASS$$()
{
$$IF(VERBOSE)
	// すべてのオブジェクトがオートメーションで作成された場合にアプリケーション
	// を終了するために、デストラクタが AfxOleUnlockApp を呼び出すます。
	// 他の処理の間に、メイン ダイアログを破壊します。
$$ENDIF //VERBOSE
	if (m_pDialog != NULL)
		m_pDialog->m_pAutoProxy = NULL;
	AfxOleUnlockApp();
}

void $$DLGAUTOPROXY_CLASS$$::OnFinalRelease()
{
$$IF(VERBOSE)
	// オートメーション オブジェクトに対する最後の参照が解放される時に
	// OnFinalRelease が呼び出されます。基本クラスは自動的にオブジェク
	// トを削除します。基本クラスを呼び出す前に、オブジェクトで必要な特
	// 別な後処理を追加してください。

$$ENDIF //VERBOSE
	$$DLGAUTOPROXY_BASE_CLASS$$::OnFinalRelease();
}

BEGIN_MESSAGE_MAP($$DLGAUTOPROXY_CLASS$$, $$DLGAUTOPROXY_BASE_CLASS$$)
	//{{AFX_MSG_MAP($$DLGAUTOPROXY_CLASS$$)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP($$DLGAUTOPROXY_CLASS$$, $$DLGAUTOPROXY_BASE_CLASS$$)
	//{{AFX_DISPATCH_MAP($$DLGAUTOPROXY_CLASS$$)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// メモ: VBA からタイプ セーフなバインディングをサポートするために IID_I$$Safe_root$$
//  のサポートを追加します。この IID は .ODL ファイル内のディスパッチ インターフェイス 
//  へアタッチされる GUID と合致しなければなりません。

// {$$DISPIID_CLSID_ODL$$}
static const IID IID_I$$Safe_root$$ =
$$DISPIID_CLSID$$;

BEGIN_INTERFACE_MAP($$DLGAUTOPROXY_CLASS$$, $$DLGAUTOPROXY_BASE_CLASS$$)
	INTERFACE_PART($$DLGAUTOPROXY_CLASS$$, IID_I$$Safe_root$$, Dispatch)
END_INTERFACE_MAP()

$$IF(VERBOSE)
// IMPLEMENT_OLECREATE2 マクロはこのプロジェクトの StdAfx.h 内に定義されています。
$$ENDIF //VERBOSE
// {$$APP_CLSID_REG$$}
IMPLEMENT_OLECREATE2($$DLGAUTOPROXY_CLASS$$, "$$Safe_root$$.Application", $$APP_CLSID_MACRO$$)

/////////////////////////////////////////////////////////////////////////////
// $$DLGAUTOPROXY_CLASS$$ メッセージ ハンドラ
