// $$root$$.cpp : DLL 用の初期化処理の定義を行います。
//

#include "stdafx.h"
$$IF(EXTDLL)
#include <afxdllx.h>
$$ELSE // !EXTDLL
#include "$$root$$.h"
$$ENDIF //!EXTDLL

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

$$IF(EXTDLL)
$$ELSE
//
//	メモ!
//
//		この DLL が MFC DLL に対して動的にリンクされる場合、
//		MFC 内で呼び出されるこの DLL からエクスポートされた
//		どの関数も関数の最初に追加される AFX_MANAGE_STATE 
//		マクロを含んでいなければなりません。
//
//		例:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 通常関数の本体はこの位置にあります
//		}
//
//		このマクロが各関数に含まれていること、MFC 内の
//		どの呼び出しより優先することは非常に重要です。
//		これは関数内の最初のステートメントでなければな
//		らないことを意味します、コンストラクタが MFC 
//		DLL 内への呼び出しを行う可能性があるので、オブ
//		ジェクト変数の宣言よりも前でなければなりません。
//
//		詳細については MFC テクニカル ノート 33 および
//		58 を参照してください。
//
$$ENDIF

$$IF(EXTDLL)
static AFX_EXTENSION_MODULE $$safe_root$$DLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
$$IF(VERBOSE)
	// lpReserved を使う場合はここを削除してください
$$ENDIF
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("$$ROOT$$.DLL Initializing!\n");
		
$$IF(VERBOSE)
		// 拡張 DLL を１回だけ初期化します。
$$ENDIF
		if (!AfxInitExtensionModule($$safe_root$$DLL, hInstance))
			return 0;

$$IF(VERBOSE)
		// この DLL をリソース チェインへ挿入します。
		// メモ: 拡張 DLL が MFC アプリケーションではなく
		//   MFC 標準 DLL (ActiveX コントロールのような)
		//   に暗黙的にリンクされる場合、この行を DllMain
		//   から削除して、この拡張 DLL からエクスポート
		//   された別の関数内へ追加してください。  
		//   この拡張 DLL を使用する標準 DLL はこの拡張 DLL
		//   を初期化するために明示的にその関数を呼び出します。 
		//   それ以外の場合は、CDynLinkLibrary オブジェクトは
		//   標準 DLL のリソース チェインへアタッチされず、
		//   その結果重大な問題となります。

$$ENDIF
		new CDynLinkLibrary($$safe_root$$DLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("$$ROOT$$.DLL Terminating!\n");
$$IF(VERBOSE)
		// デストラクタが呼び出される前にライブラリを終了します
$$ENDIF
		AfxTermExtensionModule($$safe_root$$DLL);
	}
$$IF(VERBOSE)
	return 1;   // ok
$$ELSE
	return 1;
$$ENDIF
}
$$ELSE //!EXTDLL
/////////////////////////////////////////////////////////////////////////////
// $$APP_CLASS$$

BEGIN_MESSAGE_MAP($$APP_CLASS$$, $$APP_BASE_CLASS$$)
	//{{AFX_MSG_MAP($$APP_CLASS$$)
		// メモ - ClassWizard はこの位置にマッピング用のマクロを追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// $$APP_CLASS$$ の構築

$$APP_CLASS$$::$$APP_CLASS$$()
{
$$IF(VERBOSE)
	// TODO: この位置に構築用のコードを追加してください。
	// ここに InitInstance の中の重要な初期化処理をすべて記述してください。
$$ENDIF
}

/////////////////////////////////////////////////////////////////////////////
// 唯一の $$APP_CLASS$$ オブジェクト

$$APP_CLASS$$ theApp;
$$IF(AUTOMATION || SOCKETS)

/////////////////////////////////////////////////////////////////////////////
// $$APP_CLASS$$ の初期化

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
	// 実行するとすべての OLE サーバー ファクトリ を登録します。
	// 他のアプリケーションからオブジェクトを作るために OLE ライブラリを使用可能にします。
	COleObjectFactory::RegisterAll();

$$ENDIF //AUTOMATION
	return TRUE;
}
$$ENDIF //AUTOMATION || SOCKETS
$$IF(AUTOMATION)

/////////////////////////////////////////////////////////////////////////////
// プロシージャ内のサーバーで必要な特別なエントリ ポイント

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}

// DllRegisterServer のエクスポートによって regsvr.exe を使用できます
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll();
	return S_OK;
}
$$ENDIF //AUTOMATION
$$ENDIF //EXTDLL
