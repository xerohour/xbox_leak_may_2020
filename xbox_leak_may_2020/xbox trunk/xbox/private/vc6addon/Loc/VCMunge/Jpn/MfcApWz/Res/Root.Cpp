// $$root$$.cpp : アプリケーション用クラスの機能定義を行います。
//

#include "stdafx.h"
#include "$$root$$.h"

#include "$$frame_hfile$$.h"
$$IF(MDICHILD)
#include "$$child_frame_hfile$$.h"
$$ENDIF //MDICHILD
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
#include "$$ipframe_hfile$$.h"
$$ENDIF
$$IF(CRecordView || CDaoRecordView ||COleDBRecordView)
#include "$$recset_hfile$$.h"
$$ENDIF
$$IF(!NODOCVIEW)
#include "$$doc_hfile$$.h"
$$IF(PROJTYPE_EXPLORER)
#include "$$treeview_hfile$$.h"
$$ELSE
#include "$$view_hfile$$.h"
$$ENDIF
$$ENDIF

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$APP_CLASS$$

BEGIN_MESSAGE_MAP($$APP_CLASS$$, $$APP_BASE_CLASS$$)
	//{{AFX_MSG_MAP($$APP_CLASS$$)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
$$IF(VERBOSE)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
$$ENDIF
$$IF(NODOCVIEW)
$$IF(PROJTYPE_MDI)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
$$ENDIF //PROJTYPE_MDI
$$ENDIF //NODOCVIEW
	//}}AFX_MSG_MAP
$$IF(!NODOCVIEW)
$$IF(!DB_NO_FILE)
	// 標準のファイル基本ドキュメント コマンド
	ON_COMMAND(ID_FILE_NEW, $$APP_BASE_CLASS$$::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, $$APP_BASE_CLASS$$::OnFileOpen)
$$ENDIF //!DB_NO_FILE
$$ENDIF //!NODOCVIEW
$$IF(PRINT)
	// 標準の印刷セットアップ コマンド
	ON_COMMAND(ID_FILE_PRINT_SETUP, $$APP_BASE_CLASS$$::OnFilePrintSetup)
$$ENDIF //PRINT
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// $$APP_CLASS$$ クラスの構築

$$APP_CLASS$$::$$APP_CLASS$$()
{
$$IF(VERBOSE)
	// TODO: この位置に構築用コードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
$$ENDIF
}

$$IF(ATL_SUPPORT)
// ATL Module object
CComModule _Module;

$$ENDIF
/////////////////////////////////////////////////////////////////////////////
// 唯一の $$APP_CLASS$$ オブジェクト

$$APP_CLASS$$ theApp;
$$IF(FULL_SERVER || MINI_SERVER || CONTAINER_SERVER || AUTOMATION)

$$IF(VERBOSE)
// この ID はこのアプリケーションが統計的にユニークになるように生成されました。
// もし、特別な ID を指定したいならば、変更してもかまいません。

// {$$APP_CLSID_REG$$}
$$ENDIF
static const CLSID clsid =
$$APP_CLSID$$;
$$ENDIF

/////////////////////////////////////////////////////////////////////////////
// $$APP_CLASS$$ クラスの初期化

BOOL $$APP_CLASS$$::InitInstance()
{
$$IF(OLEDB)
	CoInitialize(NULL);
$$ENDIF // OLEDB
$$IF(SOCKETS)
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

$$ENDIF //SOCKETS
$$IF(CONTAINER || CONTAINER_SERVER || MINI_SERVER || FULL_SERVER || AUTOMATION)
	// OLE ライブラリの初期化
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

$$ENDIF //CONTAINER || CONTAINER_SERVER || MINI_SERVER || FULL_SERVER || AUTOMATION
$$IF(OLECTL)
	AfxEnableControlContainer();

$$ENDIF //OLECTL
	// 標準的な初期化処理
$$IF(VERBOSE)
	// もしこれらの機能を使用せず、実行ファイルのサイズを小さく
	// したければ以下の特定の初期化ルーチンの中から不必要なもの
	// を削除してください。
$$ENDIF
$$IF(3D)

#ifdef _AFXDLL
	Enable3dControls();		// 共有 DLL の中で MFC を使用する場合にはここを呼び出してください。
#else
	Enable3dControlsStatic();	// MFC と静的にリンクしている場合にはここを呼び出してください。
#endif
$$ENDIF //3D

	// 設定が保存される下のレジストリ キーを変更します。
$$IF(VERBOSE)
	// TODO: この文字列を、会社名または所属など適切なものに
	// 変更してください。
$$ENDIF
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

$$IF(!NODOCVIEW)
	LoadStdProfileSettings($$SIZE_MRU$$);  // 標準の INI ファイルのオプションをローﾄﾞします (MRU を含む)
$$ENDIF

$$IF(NODOCVIEW)
$$IF(VERBOSE)
	// メイン ウインドウを作成するとき、このコードは新しいフレーム ウインドウ オブジェクトを作成し、
	// それをアプリケーションのメイン ウインドウにセットします
$$ENDIF

$$IF( PROJTYPE_MDI )
	CMDIFrameWnd* pFrame = new CMainFrame;
$$ELIF( NODOCVIEW )
	CMainFrame* pFrame = new CMainFrame;
$$ELSE // Doc View 形式の SDI
	CRuntimeClass* pClass = RUNTIME_CLASS(CMainFrame);
	CFrameWnd* pFrame = (CFrameWnd*) pClass->CreateObject();
	ASSERT_KINDOF(CFrameWnd, pFrame);
$$ENDIF
	m_pMainWnd = pFrame;

$$IF(PROJTYPE_SDI)
$$IF(!NODOCVIEW)
	// 空の context をフレームに読み込みます

	CCreateContext context;
$$ELSE
	// フレームをリソースからロードして作成します
$$ENDIF

	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL, 
$$IF(!NODOCVIEW)
		&context);
$$ELSE
		NULL);
$$ENDIF

$$ELSE // MDI
	// メイン MDI フレームを作成します
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;

	// 共通の MDI メニューとアクセラレータ テーブルを読み込みます
$$IF(VERBOSE)
	//TODO: 追加のメンバ変数を加えて、アプリケーションが必要とする
	//      追加のメニュータイプのために呼び出しをロードしてください。 
$$ENDIF

	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_$$DOC$$TYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_$$DOC$$TYPE));
$$ENDIF // PROJTYPE_SDI

$$ELSE // NODOCVIEW
$$IF(VERBOSE)
	// アプリケーション用のドキュメント テンプレートを登録します。ドキュメント テンプレート
	//  はドキュメント、フレーム ウィンドウとビューを結合するために機能します。
$$ELSE
	// ドキュメント テンプレートを登録します。
$$ENDIF
$$IF(PROJTYPE_MDI)

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_$$DOC$$TYPE,
$$ELSE

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
$$ENDIF
		RUNTIME_CLASS($$DOC_CLASS$$),
$$IF(PROJTYPE_MDI)
$$IF(MDICHILD)
		RUNTIME_CLASS($$CHILD_FRAME_CLASS$$), // カスタム MDI 子フレーム
$$ELSE //!MDICHILD
		RUNTIME_CLASS(CMDIChildWnd),          // 標準 MDI 子フレーム
$$ENDIF //MDICHILD
$$ELSE //!MDI
		RUNTIME_CLASS($$FRAME_CLASS$$),       // メイン SDI フレーム ウィンドウ
$$ENDIF
$$IF(PROJTYPE_EXPLORER)
		RUNTIME_CLASS($$TREEVIEW_CLASS$$));
$$ELSE
		RUNTIME_CLASS($$VIEW_CLASS$$));
$$ENDIF
$$IF(CONTAINER || CONTAINER_SERVER)
$$IF(PROJTYPE_MDI)
	pDocTemplate->SetContainerInfo(IDR_$$DOC$$TYPE_CNTR_IP);
$$ELSE
	pDocTemplate->SetContainerInfo(IDR_CNTR_INPLACE);
$$ENDIF
$$ENDIF
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	pDocTemplate->SetServerInfo(
$$IF(PROJTYPE_MDI)
		IDR_$$DOC$$TYPE_SRVR_EMB, IDR_$$DOC$$TYPE_SRVR_IP,
$$ELSE
		IDR_SRVR_EMBEDDED, IDR_SRVR_INPLACE,
$$ENDIF
		RUNTIME_CLASS($$IPFRAME_CLASS$$));
$$ENDIF
	AddDocTemplate(pDocTemplate);
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER || AUTOMATION)
$$IF(VERBOSE)

	// ドキュメント テンプレートに COleTemplateServer を接続します。
	// COleTemplateServer はドキュメント テンプレートで指定された
	// 情報を使って OLE コンテナに要求する代わりに新規ドキュメント
	// を作成します。
$$ENDIF
$$IF(PROJTYPE_MDI)
	m_server.ConnectTemplate(clsid, pDocTemplate, FALSE);

$$IF(VERBOSE)
	// 実行するとすべての OLE サーバー ファクトリを登録します。
	// 他のアプリケーションからオブジェクトを作るために OLE ライブラリを使用可能にします。
$$ENDIF
	COleTemplateServer::RegisterAll();
$$IF(VERBOSE)
		// メモ: MDI アプリケーションはコマンドラインで /Embedding か /Automation が
		//       指定されていなくてもすべてのサーバー オブジェクトを登録します。
$$ENDIF
$$ELSE //!MDI
	m_server.ConnectTemplate(clsid, pDocTemplate, TRUE);
$$IF(VERBOSE)
		// メモ: SDI アプリケーションはコマンドラインで /Embedding か /Automation
		//       が指定されている時だけサーバー オブジェクトを登録します。
$$ENDIF
$$ENDIF
$$ENDIF

$$IF(PROJTYPE_MDI)
	// メイン MDI フレーム ウィンドウを作成
	$$FRAME_CLASS$$* pMainFrame = new $$FRAME_CLASS$$;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

$$IF(!MINI_SERVER)
$$// サフィックスがある場合にのみ DragAcceptFiles を呼び出します。
$$// MDI アプリケーションでは、m_pMainWnd の設定直後に呼び出されます。
$$IF(HAS_SUFFIX)
	// ドラッグ/ドロップ のオープンを許可します
	m_pMainWnd->DragAcceptFiles();

$$ENDIF //SUFFIX
$$ENDIF //!MINI_SERVER
$$ENDIF //MDI
$$IF(!MINI_SERVER)
$$IF(HAS_SUFFIX)
	// DDE Execute open を使用可能にします。
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

$$ENDIF //SUFFIX
$$ENDIF //!MINI_SERVER
$$ENDIF // NODOCVIEW
$$IF(!NODOCVIEW)
	// DDE、file open など標準のシェル コマンドのコマンドラインを解析します。
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
$$ENDIF

$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER || AUTOMATION)
$$IF(VERBOSE)
	// OLE サーバーとして起動されているか確認します。
$$ENDIF
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
$$IF(!PROJTYPE_MDI)
$$IF(VERBOSE)
		// 実行するとすべての OLE サーバー ファクトリを登録します。
		// 他のアプリケーションからオブジェクトを作るために OLE ライブラリを使用可能にします。
$$ENDIF //VERBOSE
		COleTemplateServer::RegisterAll();

$$ENDIF //!MDI
		// アプリケーションが /Embedding か /Automation で実行されている時には
		// メイン ウィンドウ は表示できません。
		return TRUE;
	}

$$IF(VERBOSE)
	// システム レジストリが壊れていてサーバー アプリケーションがスタンド アロンで
	// 起動された時には、システム レジストリを更新してください。
$$ENDIF
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
$$IF(ACTIVE_DOC_SERVER)
	m_server.UpdateRegistry(OAT_DOC_OBJECT_SERVER);
$$ELSE
	m_server.UpdateRegistry(OAT_INPLACE_SERVER);
$$ENDIF	// ACTIVE_DOC_SERVER
$$ELIF(AUTOMATION)
	m_server.UpdateRegistry(OAT_DISPATCH_OBJECT);
$$ENDIF
$$IF(AUTOMATION)
	COleObjectFactory::UpdateRegistryAll();
$$ENDIF

$$ENDIF //MINI_SERVER || FULL_SERVER || CONTAINER_SERVER || AUTOMATION
$$IF(MINI_SERVER)
$$IF(VERBOSE)
	// ミニサーバーがスタンド アロンで実行されている時、レジストリは更新されユーザーはサーバー
	// を使用するためにコンテナでオブジェクトの挿入ダイアログを使用するよう指示されます。
	// ミニサーバーはスタンド アロンのユーザー インターフェイスを持ちません。
$$ENDIF //VERBOSE
	AfxMessageBox(IDP_USE_INSERT_OBJECT);
	return FALSE;
$$ELSE //!MINI_SERVER
$$IF(!NODOCVIEW)
	// コマンドラインでディスパッチ コマンドを指定します。
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
$$ENDIF
$$IF(PROJTYPE_MDI)
$$IF(VERBOSE)

	// メイン ウィンドウが初期化されたので、表示と更新を行います。
$$ENDIF
$$IF(NODOCVIEW)
	pFrame->ShowWindow($$SW_ARG$$);
	pFrame->UpdateWindow();
$$ELSE
	pMainFrame->ShowWindow($$SW_ARG$$);
	pMainFrame->UpdateWindow();
$$ENDIF
$$ELIF(PROJTYPE_SDI)
$$IF(VERBOSE)

	// メイン ウィンドウが初期化されたので、表示と更新を行います。
$$ENDIF
$$IF(!NODOCVIEW)
	m_pMainWnd->ShowWindow($$SW_ARG$$);
	m_pMainWnd->UpdateWindow();
$$ELSE
	pFrame->ShowWindow($$SW_ARG$$);
	pFrame->UpdateWindow();
$$ENDIF
$$// サフィックスがある場合は DragAcceptFiles だけを呼び出してください。
$$// SDI アプリケーションの場合 ProcessShellCommand の後で呼び出してください。
$$IF(HAS_SUFFIX)

	// ドラッグ/ドロップ オープンを許可します
	m_pMainWnd->DragAcceptFiles();
$$ENDIF //SUFFIX
$$ENDIF //MDI/SDI

	return TRUE;
$$ENDIF //!MINI_SERVER
}

$$IF(NODOCVIEW)
/////////////////////////////////////////////////////////////////////////////
// $$APP_CLASS$$ message handlers

$$IF(PROJTYPE_MDI)
$$IF(NODOCVIEW)
int $$APP_CLASS$$::ExitInstance() 
{
$$IF(VERBOSE)
	//TODO: 追加したリソースがある場合にはそれらも処理してください
$$ENDIF
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	return CWinApp::ExitInstance();
}
$$ENDIF // NODOCVIEW
$$ENDIF // PROJTYPE_MDI

$$IF(PROJTYPE_MDI || !NODOCVIEW ) // Doc View 形式でない SDI では必要ありません
void $$APP_CLASS$$::OnFileNew() 
{
$$IF(PROJTYPE_SDI)
	CString strUntitled;
	CFrameWnd* pFrameWnd = DYNAMIC_DOWNCAST(CFrameWnd, m_pMainWnd);
	if (pFrameWnd != NULL)
	{
$$IF(VERBOSE)
		// TODO: メモリのファイル固有のデータを閉じて、破棄してください。 
		// 一度終了したフレームウインドウのタイトルをリセットするために
		// 次の if 文を実行してください。
$$ENDIF

		if (strUntitled.LoadString(AFX_IDS_UNTITLED))
			pFrameWnd->UpdateFrameTitleForDocument(strUntitled);
		else
			pFrameWnd->UpdateFrameTitleForDocument(NULL);
	}
$$ELSE
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);

	// 新しい MDI 子ウィンドウを作成します
	pFrame->CreateNewChild(
		RUNTIME_CLASS(CChildFrame), IDR_$$DOC$$TYPE, m_hMDIMenu, m_hMDIAccel);
$$ENDIF // PROJTYPE_SDI
}
$$ENDIF // PROJTYPE_MDI || !NODOCVIEW


$$ENDIF

/////////////////////////////////////////////////////////////////////////////
// アプリケーションのバージョン情報で使われる CAboutDlg ダイアログ

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ダイアログ データ
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard 仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
	//{{AFX_MSG(CAboutDlg)
		// メッセージ ハンドラはありません。
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
		// メッセージ ハンドラはありません。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// ダイアログを実行するためのアプリケーション コマンド
void $$APP_CLASS$$::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// $$APP_CLASS$$ メッセージ ハンドラ
