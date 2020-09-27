// $$wndview_ifile$$.cpp : $$WNDVIEW_CLASS$$ クラスの動作の定義を行います。
//

#include "stdafx.h"
#include "$$root$$.h"
#include "$$wndview_hfile$$.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$WNDVIEW_CLASS$$

$$WNDVIEW_CLASS$$::$$WNDVIEW_CLASS$$()
{
}

$$WNDVIEW_CLASS$$::~$$WNDVIEW_CLASS$$()
{
}


BEGIN_MESSAGE_MAP($$WNDVIEW_CLASS$$,$$WNDVIEW_BASE_CLASS$$ )
	//{{AFX_MSG_MAP($$WNDVIEW_CLASS$$)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// $$WNDVIEW_CLASS$$ メッセージ ハンドラ

BOOL $$WNDVIEW_CLASS$$::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void $$WNDVIEW_CLASS$$::OnPaint() 
{
	CPaintDC dc(this); // 描画用のデバイス コンテキスト
	
$$IF(VERBOSE)
	// TODO: メッセージ ハンドラのコードをここに追加してください。
$$ENDIF
	
	// 描画のために CWnd::OnPaint を呼び出してはいけません。
}

