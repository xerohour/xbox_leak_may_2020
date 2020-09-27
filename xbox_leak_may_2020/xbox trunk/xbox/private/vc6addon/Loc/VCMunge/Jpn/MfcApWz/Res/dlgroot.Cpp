// $$root$$.cpp : アプリケーション用クラスの定義を行います。
//

#include "stdafx.h"
#include "$$root$$.h"
#include "$$dlg_hfile$$.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// $$APP_CLASS$$

BEGIN_MESSAGE_MAP($$APP_CLASS$$, $$APP_BASE_CLASS$$)
	//{{AFX_MSG_MAP($$APP_CLASS$$)
$$IF(VERBOSE)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
$$ENDIF
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// $$APP_CLASS$$ クラスの構築

$$APP_CLASS$$::$$APP_CLASS$$()
{
$$IF(VERBOSE)	
	// TODO: この位置に構築用のコードを追加してください。
	// ここに InitInstance 中の重要な初期化処理をすべて記述してください。
$$ENDIF
}

/////////////////////////////////////////////////////////////////////////////
// 唯一の $$APP_CLASS$$ オブジェクト

$$APP_CLASS$$ theApp;

/////////////////////////////////////////////////////////////////////////////
// $$APP_CLASS$$ クラスの初期化

BOOL $$APP_CLASS$$::InitInstance()
{
$$IF(SOCKETS)
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

$$ENDIF //SOCKETS
$$IF(AUTOMATION)
	// OLE ライブラリの初期化
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

$$ENDIF //AUTOMATION
$$IF(OLECTL)
	AfxEnableControlContainer();

$$ENDIF //OLECTL
	// 標準的な初期化処理
$$IF(VERBOSE)
	// もしこれらの機能を使用せず、実行ファイルのサイズを小さくしたけ
	//  れば以下の特定の初期化ルーチンの中から不必要なものを削除して
	//  ください。
$$ENDIF

$$IF(3D)
#ifdef _AFXDLL
	Enable3dControls();			// 共有 DLL 内で MFC を使う場合はここをコールしてください。
#else
	Enable3dControlsStatic();	// MFC と静的にリンクする場合はここをコールしてください。
#endif

$$ENDIF //3D
$$IF(AUTOMATION)
$$IF(VERBOSE)
	// OLE サーバーとして起動されている時にはコマンドラインを解析します。
$$ENDIF
	if (RunEmbedded() || RunAutomated())
	{
$$IF(VERBOSE)
		// 実行するとすべての OLE サーバー ファクトリ を登録します。
		//  他のアプリケーションからオブジェクトを作るために OLE ライブラリを使用可能にします。
$$ENDIF
		COleTemplateServer::RegisterAll();
	}
	else
	{
$$IF(VERBOSE)
		// システム レジストリが壊れていてサーバー アプリケーションがスタンド アロンで
		// 起動された時には、システム レジストリを更新してください。
$$ENDIF
		COleObjectFactory::UpdateRegistryAll();
	}

$$ENDIF //AUTOMATION
	$$DLG_CLASS$$ dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
$$IF(VERBOSE)
		// TODO: ダイアログが <OK> で消された時のコードを
		//       記述してください。
$$ENDIF
	}
	else if (nResponse == IDCANCEL)
	{
$$IF(VERBOSE)
		// TODO: ダイアログが <ｷｬﾝｾﾙ> で消された時のコードを
		//       記述してください。
$$ENDIF
	}

	// ダイアログが閉じられてからアプリケーションのメッセージ ポンプを開始するよりは、
	// アプリケーションを終了するために FALSE を返してください。
	return FALSE;
}
