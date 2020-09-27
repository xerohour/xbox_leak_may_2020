// $$dlg_ifile$$.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "$$root$$.h"
#include "$$dlg_hfile$$.h"
$$IF(AUTOMATION)
#include "$$dlgautoproxy_hfile$$.h"
$$ENDIF //AUTOMATION

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

$$IF(ABOUT)
/////////////////////////////////////////////////////////////////////////////
// アプリケーションのバージョン情報で使われている CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard は仮想関数のオーバーライドを生成します
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// メッセージ ハンドラがありません。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

$$ENDIF //ABOUT
/////////////////////////////////////////////////////////////////////////////
// $$DLG_CLASS$$ ダイアログ
$$IF(AUTOMATION)

IMPLEMENT_DYNAMIC($$DLG_CLASS$$, $$DLG_BASE_CLASS$$);
$$ENDIF //AUTOMATION

$$DLG_CLASS$$::$$DLG_CLASS$$(CWnd* pParent /*=NULL*/)
	: $$DLG_BASE_CLASS$$($$DLG_CLASS$$::IDD, pParent)
{
	//{{AFX_DATA_INIT($$DLG_CLASS$$)
		// メモ: この位置に ClassWizard によってメンバの初期化が追加されます。
	//}}AFX_DATA_INIT
$$IF(VERBOSE)
	// メモ: LoadIcon は Win32 の DestroyIcon のサブシーケンスを要求しません。
$$ENDIF
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
$$IF(AUTOMATION)
	m_pAutoProxy = NULL;
$$ENDIF //AUTOMATION
}
$$IF(AUTOMATION)

$$DLG_CLASS$$::~$$DLG_CLASS$$()
{
$$IF(VERBOSE)
	// このダイアログ用のオートメーション プロキシがある場合は、このダイアログ
	// へのポインタを NULL に戻します、それによってダイアログが削除されたこと
	// がわかります。
$$ENDIF //VERBOSE
	if (m_pAutoProxy != NULL)
		m_pAutoProxy->m_pDialog = NULL;
}
$$ENDIF //AUTOMATION

void $$DLG_CLASS$$::DoDataExchange(CDataExchange* pDX)
{
	$$DLG_BASE_CLASS$$::DoDataExchange(pDX);
	//{{AFX_DATA_MAP($$DLG_CLASS$$)
		// メモ: この場所には ClassWizard によって DDX と DDV の呼び出しが追加されます。
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP($$DLG_CLASS$$, $$DLG_BASE_CLASS$$)
	//{{AFX_MSG_MAP($$DLG_CLASS$$)
$$IF(ABOUT)
	ON_WM_SYSCOMMAND()
$$ENDIF
$$IF(HELP)
	ON_WM_DESTROY()
$$ENDIF
$$IF(AUTOMATION)
	ON_WM_CLOSE()
$$ENDIF
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// $$DLG_CLASS$$ メッセージ ハンドラ

BOOL $$DLG_CLASS$$::OnInitDialog()
{
	$$DLG_BASE_CLASS$$::OnInitDialog();
$$IF(ABOUT)

	// "バージョン情報..." メニュー項目をシステム メニューへ追加します。

	// IDM_ABOUTBOX はコマンド メニューの範囲でなければなりません。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
$$ENDIF //ABOUT

$$IF(VERBOSE)
	// このダイアログ用のアイコンを設定します。フレームワークはアプリケーションのメイン
	// ウィンドウがダイアログでない時は自動的に設定しません。
$$ENDIF //VERBOSE
	SetIcon(m_hIcon, TRUE);			// 大きいアイコンを設定
	SetIcon(m_hIcon, FALSE);		// 小さいアイコンを設定
	
	// TODO: 特別な初期化を行う時はこの場所に追加してください。
	
	return TRUE;  // TRUE を返すとコントロールに設定したフォーカスは失われません。
}
$$IF(ABOUT)

void $$DLG_CLASS$$::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		$$DLG_BASE_CLASS$$::OnSysCommand(nID, lParam);
	}
}
$$ENDIF //ABOUT
$$IF(HELP)

void $$DLG_CLASS$$::OnDestroy()
{
	WinHelp(0L, HELP_QUIT);
	$$DLG_BASE_CLASS$$::OnDestroy();
}
$$ENDIF //HELP

// もしダイアログボックスに最小化ボタンを追加するならば、アイコンを描画する
// コードを以下に記述する必要があります。MFC アプリケーションは document/view
// モデルを使っているので、この処理はフレームワークにより自動的に処理されます。

void $$DLG_CLASS$$::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 描画用のデバイス コンテキスト

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// クライアントの矩形領域内の中央
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// アイコンを描画します。
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		$$DLG_BASE_CLASS$$::OnPaint();
	}
}

$$IF(VERBOSE)
// システムは、ユーザーが最小化ウィンドウをドラッグしている間、
// カーソルを表示するためにここを呼び出します。
$$ENDIF
HCURSOR $$DLG_CLASS$$::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
$$IF(AUTOMATION)
$$IF(VERBOSE)

// コントローラがオブジェクトの 1 つををまだ保持している場合、
// オートメーションサーバーはユーザーが UI を閉じる際に終了で
// きません。これらのメッセージ ハンドラはプロキシがまだ使用中
// かどうかを確認し、それから UI が非表示になりますがダイアロ
// グはそれが消された場合その場所に残ります。
$$ENDIF //VERBOSE

void $$DLG_CLASS$$::OnClose() 
{
	if (CanExit())
		$$DLG_BASE_CLASS$$::OnClose();
}

void $$DLG_CLASS$$::OnOK() 
{
	if (CanExit())
		$$DLG_BASE_CLASS$$::OnOK();
}

void $$DLG_CLASS$$::OnCancel() 
{
	if (CanExit())
		$$DLG_BASE_CLASS$$::OnCancel();
}

BOOL $$DLG_CLASS$$::CanExit()
{
$$IF(VERBOSE)
	// プロキシ オブジェクトがまだ残っている場合、オートメーション	
	// コントローラはこのアプリケーションをまだ保持しています。 
	// ダイアログの周囲は残しますが UI は非表示になります。
$$ENDIF //VERBOSE
	if (m_pAutoProxy != NULL)
	{
		ShowWindow(SW_HIDE);
		return FALSE;
	}

	return TRUE;
}
$$ENDIF //AUTOMATION
