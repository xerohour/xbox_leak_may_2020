// $$child_frame_ifile$$.cpp : $$CHILD_FRAME_CLASS$$ クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "$$root$$.h"

#include "$$child_frame_hfile$$.h"
$$IF(PROJTYPE_EXPLORER)
#include "$$treeview_hfile$$.h"
#include "$$view_hfile$$.h"
$$ENDIF

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$CHILD_FRAME_CLASS$$

IMPLEMENT_DYNCREATE($$CHILD_FRAME_CLASS$$, $$CHILD_FRAME_BASE_CLASS$$)

BEGIN_MESSAGE_MAP($$CHILD_FRAME_CLASS$$, $$CHILD_FRAME_BASE_CLASS$$)
	//{{AFX_MSG_MAP($$CHILD_FRAME_CLASS$$)
$$IF(VERBOSE)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
$$ENDIF
$$IF(NODOCVIEW)
		ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
		ON_WM_SETFOCUS()
		ON_WM_CREATE()
$$ENDIF
$$IF( REBAR )
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
		ON_WM_CREATE()
$$ENDIF // (ANY SERVER)
$$ENDIF // REBAR
	//}}AFX_MSG_MAP
$$IF(PROJTYPE_EXPLORER)
$$IF(CListView)
	ON_UPDATE_COMMAND_UI_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnUpdateViewStyles)
	ON_COMMAND_RANGE(AFX_ID_VIEW_MINIMUM, AFX_ID_VIEW_MAXIMUM, OnViewStyle)
$$ENDIF
$$ENDIF
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// $$CHILD_FRAME_CLASS$$ クラスの構築/消滅

$$CHILD_FRAME_CLASS$$::$$CHILD_FRAME_CLASS$$()
{
$$IF(VERBOSE)
	// TODO: メンバ初期化コードをこの位置に追加してください。
	
$$ENDIF
}

$$CHILD_FRAME_CLASS$$::~$$CHILD_FRAME_CLASS$$()
{
}
$$IF(SPLITTER_MDI || PROJTYPE_EXPLORER)

BOOL $$CHILD_FRAME_CLASS$$::OnCreateClient( LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
$$IF(PROJTYPE_EXPLORER)
	// 分割ウィンドウを作成します
	if (!m_wndSplitter.CreateStatic(this, 1, 2))
		return FALSE;

	if (!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS($$TREEVIEW_CLASS$$), CSize(100, 100), pContext) ||
		!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS($$VIEW_CLASS$$), CSize(100, 100), pContext))
	{
		m_wndSplitter.DestroyWindow();
		return FALSE;
	}

	return TRUE;
$$ELSE //!PROJTYPE_EXPLORER
	return m_wndSplitter.Create( this,
$$IF(VERBOSE)
		2, 2,                 // TODO: 行と列の数を調整してください。
		CSize( 10, 10 ),      // TODO: 最小の区画サイズを調整してください。
$$ELSE //!VERBOSE
		2, 2,
		CSize( 10, 10 ),
$$ENDIF //VERBOSE
		pContext );
$$ENDIF //PROJTYPE_EXPLORER
}
$$ENDIF //SPLITTER || EXPLORER

BOOL $$CHILD_FRAME_CLASS$$::PreCreateWindow(CREATESTRUCT& cs)
{
$$IF(VERBOSE)
	// TODO: この位置で CREATESTRUCT cs の設定を行って、Window クラスまたは
	//       スタイルを変更してください。

$$ENDIF //VERBOSE
	if( !$$CHILD_FRAME_BASE_CLASS$$::PreCreateWindow(cs) )
		return FALSE;

$$IF(CHILD_FRAME_STYLES)
	cs.style = WS_CHILD | WS_VISIBLE | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU
		| FWS_ADDTOTITLE$$CHILD_FRAME_STYLE_FLAGS$$;

$$ENDIF //CHILD_FRAME_STYLES
$$IF(NODOCVIEW)
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

$$ENDIF
	return TRUE;
}

$$IF(CHILD_FRAME_MAXIMIZED || CHILD_FRAME_MINIMIZED)
void $$CHILD_FRAME_CLASS$$::ActivateFrame(int nCmdShow)
{
$$IF(VERBOSE)
	// TODO: この関数を修正してフレームがアクティブにされる方法を変更してください。

$$ENDIF //VERBOSE
$$IF(CHILD_FRAME_MAXIMIZED)
	nCmdShow = SW_SHOWMAXIMIZED;
$$ENDIF //CHILD_FRAME_MAXIMIZED
$$IF(CHILD_FRAME_MINIMIZED)
	nCmdShow = SW_SHOWMINIMIZED;
$$ENDIF //CHILD_FRAME_MINIMIZED
	$$CHILD_FRAME_BASE_CLASS$$::ActivateFrame(nCmdShow);
}
$$ENDIF //CHILD_FRAME_MAXIMIZED || CHILD_FRAME_MINIMIZED


/////////////////////////////////////////////////////////////////////////////
// $$CHILD_FRAME_CLASS$$ クラスの診断

#ifdef _DEBUG
void $$CHILD_FRAME_CLASS$$::AssertValid() const
{
	$$CHILD_FRAME_BASE_CLASS$$::AssertValid();
}

void $$CHILD_FRAME_CLASS$$::Dump(CDumpContext& dc) const
{
	$$CHILD_FRAME_BASE_CLASS$$::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// $$CHILD_FRAME_CLASS$$ クラスのメッセージハンドラ
$$IF(PROJTYPE_EXPLORER)
$$IF(CListView)
$$VIEW_CLASS$$* $$CHILD_FRAME_CLASS$$::GetRightPane()
{
	CWnd* pWnd = m_wndSplitter.GetPane(0, 1);
	$$VIEW_CLASS$$* pView = DYNAMIC_DOWNCAST($$VIEW_CLASS$$, pWnd);
	return pView;
}

void $$CHILD_FRAME_CLASS$$::OnUpdateViewStyles(CCmdUI* pCmdUI)
{
$$IF(VERBOSE)
	// TODO: このコードを変更/拡張して、 View メニューの選択を処理する
	// ようにしてください。
$$ENDIF //VERBOSE

	$$VIEW_CLASS$$* pView = GetRightPane(); 

$$IF(VERBOSE)
	// もし右側のペインが作成されないかビューではないなら、
	// この範囲でコマンドを無効にします
$$ENDIF //VERBOSE

	if (pView == NULL)
		pCmdUI->Enable(FALSE);
	else
	{
		DWORD dwStyle = pView->GetStyle() & LVS_TYPEMASK;

$$IF(VERBOSE)
		// もしコマンドが ID_VIEW_LINEUP ならば、 LVS_ICON or LVS_SMALLICON 
		// モードになった場合だけ、このコマンドを有効にします

$$ENDIF //VERBOSE
		if (pCmdUI->m_nID == ID_VIEW_LINEUP)
		{
			if (dwStyle == LVS_ICON || dwStyle == LVS_SMALLICON)
				pCmdUI->Enable();
			else
				pCmdUI->Enable(FALSE);
		}
		else
		{
$$IF(VERBOSE)
			// そうでなければ、ドットを使ってビューのスタイルを反映します
$$ENDIF //VERBOSE
			pCmdUI->Enable();
			BOOL bChecked = FALSE;

			switch (pCmdUI->m_nID)
			{
			case ID_VIEW_DETAILS:
				bChecked = (dwStyle == LVS_REPORT);
				break;

			case ID_VIEW_SMALLICON:
				bChecked = (dwStyle == LVS_SMALLICON);
				break;

			case ID_VIEW_LARGEICON:
				bChecked = (dwStyle == LVS_ICON);
				break;

			case ID_VIEW_LIST:
				bChecked = (dwStyle == LVS_LIST);
				break;

			default:
				bChecked = FALSE;
				break;
			}

			pCmdUI->SetRadio(bChecked ? 1 : 0);
		}
	}
}


void $$CHILD_FRAME_CLASS$$::OnViewStyle(UINT nCommandID)
{
$$IF(VERBOSE)
	// TODO: このコードを変更/拡張して、 View メニューの選択を処理する
	// ようにしてください。
$$ENDIF //VERBOSE
	$$VIEW_CLASS$$* pView = GetRightPane();

$$IF(VERBOSE)
	// もし右側のペインが作成されて $$VIEW_CLASS$$ ならば 、
	// メニュー コマンドを処理します
$$ENDIF //VERBOSE
	if (pView != NULL)
	{
		DWORD dwStyle = -1;

		switch (nCommandID)
		{
		case ID_VIEW_LINEUP:
			{
$$IF(VERBOSE)
				// リスト コントロールのアイコンをグリッド位置に合わせます
$$ENDIF //VERBOSE
				CListCtrl& refListCtrl = pView->GetListCtrl();
				refListCtrl.Arrange(LVA_SNAPTOGRID);
			}
			break;

$$IF(VERBOSE)
		// 他のコマンドはリスト コントロールのスタイルを変更します
$$ENDIF //VERBOSE
		case ID_VIEW_DETAILS:
			dwStyle = LVS_REPORT;
			break;

		case ID_VIEW_SMALLICON:
			dwStyle = LVS_SMALLICON;
			break;

		case ID_VIEW_LARGEICON:
			dwStyle = LVS_ICON;
			break;

		case ID_VIEW_LIST:
			dwStyle = LVS_LIST;
			break;
		}

$$IF(VERBOSE)
		//スタイル変更; ウィンドウは自動的に再描画します
$$ENDIF //VERBOSE
		if (dwStyle != -1)
			pView->ModifyStyle(LVS_TYPEMASK, dwStyle);
	}
}
$$ENDIF
$$ENDIF
$$IF(NODOCVIEW)
void $$CHILD_FRAME_CLASS$$::OnFileClose() 
{
$$IF(VERBOSE)
	// このフレームを閉じるには、WM_CLOSE を送ります。このメッセージは、
	// システム メニューの [閉じる] を選択した場合と同じです。
$$ENDIF

	SendMessage(WM_CLOSE);
}

int $$CHILD_FRAME_CLASS$$::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// フレームのクライアント領域全体を占めるビューを作成します。
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, 
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}

	return 0;
}

void $$CHILD_FRAME_CLASS$$::OnSetFocus(CWnd* pOldWnd) 
{
	CMDIChildWnd::OnSetFocus(pOldWnd);

	m_wndView.SetFocus();
}

BOOL $$CHILD_FRAME_CLASS$$::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	// ビューに最初にコマンドを処理する機会を与えます。
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;
	
	// 処理されなかった場合にはデフォルトの処理を行います。
	return CMDIChildWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

$$ENDIF
