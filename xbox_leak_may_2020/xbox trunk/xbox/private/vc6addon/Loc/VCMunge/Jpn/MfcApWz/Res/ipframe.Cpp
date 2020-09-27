// $$ipframe_ifile$$.cpp : $$IPFRAME_CLASS$$ クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "$$root$$.h"

#include "$$ipframe_hfile$$.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$IPFRAME_CLASS$$

IMPLEMENT_DYNCREATE($$IPFRAME_CLASS$$, $$IPFRAME_BASE_CLASS$$)

BEGIN_MESSAGE_MAP($$IPFRAME_CLASS$$, $$IPFRAME_BASE_CLASS$$)
	//{{AFX_MSG_MAP($$IPFRAME_CLASS$$)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
$$IF(HELP)
	// グローバル ヘルプ コマンド
	ON_COMMAND(ID_HELP_FINDER, $$IPFRAME_BASE_CLASS$$::OnHelpFinder)
	ON_COMMAND(ID_HELP, $$IPFRAME_BASE_CLASS$$::OnHelp)
	ON_COMMAND(ID_DEFAULT_HELP, $$IPFRAME_BASE_CLASS$$::OnHelpFinder)
	ON_COMMAND(ID_CONTEXT_HELP, $$IPFRAME_BASE_CLASS$$::OnContextHelp)
$$ENDIF
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// $$IPFRAME_CLASS$$ クラスの構築/消滅

$$IPFRAME_CLASS$$::$$IPFRAME_CLASS$$()
{
}

$$IPFRAME_CLASS$$::~$$IPFRAME_CLASS$$()
{
}

int $$IPFRAME_CLASS$$::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if ($$IPFRAME_BASE_CLASS$$::OnCreate(lpCreateStruct) == -1)
		return -1;

$$IF(VERBOSE)
	// CResizeBar は埋め込み先でのサイズの変更を実現します。
$$ENDIF
	if (!m_wndResizeBar.Create(this))
	{
		TRACE0("Failed to create resize bar\n");
		return -1;      // 作成に失敗
	}

$$IF(VERBOSE)
	// デフォルトで、ドロップ ターゲットをフレームウィンドウと共に登録し
	// ないのは、良い考えです。これは "falling through" ドロップからコ
	// ンテナのドラッグ アンド ドロップ サポートを保護します。
$$ENDIF
	m_dropTarget.Register(this);

	return 0;
}

$$IF(TOOLBAR)
$$IF(VERBOSE)
// OnCreateControlBars はコンテナ アプリケーションのウィンドウ上にツールバーを作成するために、
// フレームワークから呼ばれます。pWndFrame はコンテナのトップ レベルのフレーム ウィンドウで、
// 常に NULL ではありません。pWndDoc はドキュメント レベルのフレーム ウィンドウでコンテナが SDI
// アプリケーションのときに NULL になります。サーバー アプリケーションは他のウィンドウに MFC の
// コントロール バーを配置できます。
$$ENDIF
BOOL $$IPFRAME_CLASS$$::OnCreateControlBars(CFrameWnd* pWndFrame, CFrameWnd* pWndDoc)
{
$$IF(VERBOSE)
	// pWndDoc を使用する場会はこれを削除します。
$$ENDIF
	UNREFERENCED_PARAMETER(pWndDoc);

$$IF(VERBOSE)
	// メッセージが正しいアプリケーションに渡されるように、このウィンドウにオーナーを設定します。
$$ENDIF
	m_wndToolBar.SetOwner(this);

$$IF(VERBOSE)
	// クライアントのフレーム ウィンドウ上にツールバーを作成。
$$ENDIF
$$IF(REBAR)
	if (!m_wndToolBar.CreateEx(pWndFrame) ||
$$ELSE
	if (!m_wndToolBar.CreateEx(pWndFrame, TBSTYLE_FLAT,WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
$$ENDIF
$$IF(PROJTYPE_MDI)
		!m_wndToolBar.LoadToolBar(IDR_$$DOC$$TYPE_SRVR_IP))
$$ELSE
		!m_wndToolBar.LoadToolBar(IDR_SRVR_INPLACE))
$$ENDIF
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;
	}

$$IF(REBAR)
$$IF(VERBOSE)
	// このウィンドウのオーナーを設定し、メッセージが正しいアプリケーションに送られるようにします
$$ENDIF
	m_wndDlgBar.SetOwner(this);

$$IF(VERBOSE)
	// クライアントのフレーム ウインドウにダイアログ バーを作成します
$$ENDIF
$$IF(PROJTYPE_MDI)
	if (!m_wndDlgBar.Create(pWndFrame, IDR_$$DOC$$TYPE_SRVR_IP, 
$$ELSE
	if (!m_wndDlgBar.Create(pWndFrame, IDR_SRVR_INPLACE,
$$ENDIF
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return FALSE;
	}

$$IF(VERBOSE)
	// このウィンドウのオーナーを設定し、メッセージが正しいアプリケーションに送られるようにします
$$ENDIF
	m_wndReBar.SetOwner(this);

$$IF(VERBOSE)
	// クライアントのフレーム ウインドウに rebar を作成します
$$ENDIF
	if (!m_wndReBar.Create(pWndFrame) ||
		!m_wndReBar.AddBar(&m_wndToolBar) ||
		!m_wndReBar.AddBar(&m_wndDlgBar))
	{
		TRACE0("Failed to create rebar\n");
		return FALSE;
	}

$$IF(VERBOSE)
	// TODO: ツール チップスが必要ない場合、ここを削除してください。
$$ENDIF
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndDlgBar.SetBarStyle(m_wndDlgBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);
$$ELSE
$$IF(VERBOSE)
	// TODO: ツール バーをドッキング可能にしない場合は以下の３行を
	//       削除してください。
$$ENDIF
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	pWndFrame->EnableDocking(CBRS_ALIGN_ANY);
	pWndFrame->DockControlBar(&m_wndToolBar);
$$ENDIF

	return TRUE;
}

$$ENDIF //TOOLBAR
BOOL $$IPFRAME_CLASS$$::PreCreateWindow(CREATESTRUCT& cs)
{
$$IF(VERBOSE)
	// TODO: この位置で CREATESTRUCT cs を修正して Window クラスまたはスタイル
	//       を修正してください。

$$ENDIF //VERBOSE
	return $$IPFRAME_BASE_CLASS$$::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// $$IPFRAME_CLASS$$ クラスの診断

#ifdef _DEBUG
void $$IPFRAME_CLASS$$::AssertValid() const
{
	$$IPFRAME_BASE_CLASS$$::AssertValid();
}

void $$IPFRAME_CLASS$$::Dump(CDumpContext& dc) const
{
	$$IPFRAME_BASE_CLASS$$::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// $$IPFRAME_CLASS$$ コマンド
