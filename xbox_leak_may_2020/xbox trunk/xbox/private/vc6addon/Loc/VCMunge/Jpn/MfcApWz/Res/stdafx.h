// stdafx.h : 標準のシステム インクルード ファイル、
//            または参照回数が多く、かつあまり変更されない
//            プロジェクト専用のインクルード ファイルを記述します。
//

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Windows ヘッダーから殆ど使用されないスタッフを除外します。

#include <afxwin.h>         // MFC のコアおよび標準コンポーネント
#include <afxext.h>         // MFC の拡張部分
$$IF(CTreeView || CListView || PROJTYPE_EXPLORER)
#include <afxcview.h>
$$ENDIF //CTreeView || CListView
$$IF(PROJTYPE_DLL)

#ifndef _AFX_NO_OLE_SUPPORT
$$ENDIF //PROJTYPE_DLL
$$IF(CONTAINER || MINI_SERVER || FULL_SERVER || CONTAINER_SERVER || PROJTYPE_DLL)
#include <afxole.h>         // MFC の OLE クラス
$$IF(CONTAINER || CONTAINER_SERVER || PROJTYPE_DLL)
#include <afxodlgs.h>       // MFC の OLE ダイアログ クラス
$$ENDIF
$$ENDIF
$$IF(AUTOMATION || PROJTYPE_DLL || OLECTL)
#include <afxdisp.h>        // MFC のオートメーション クラス
$$ENDIF
$$IF(ACTIVE_DOC_SERVER)
#include <afxdocob.h>
$$ENDIF
$$IF(PROJTYPE_DLL)
#endif // _AFX_NO_OLE_SUPPORT

$$ENDIF //PROJTYPE_DLL
$$// データベース ヘッダーをインクルードします。 データベース ヘッダーをインクルードしている DLL または
$$// アプリケーションに対し(データベース ビューなし)、条件付きで DAO と ODBC をインクルードします。
$$// 現在のビューが使用されている時は、 常に対応するヘッダーのみインクルードします。
$$// 最初は、現在のビューが使用されています:
$$IF(CRecordView)
#include <afxdb.h>			// MFC ODBC データベース クラス
$$ELIF(CDaoRecordView)
#include <afxdao.h>			// MFC DAO データベース クラス
$$ELIF(DB || PROJTYPE_DLL)
$$// この位置で最小限度の DB サポートが要求されるか、または DLL です。ビューは選択されていません。

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC データベース クラス
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO データベース クラス
#endif // _AFX_NO_DAO_SUPPORT

$$ENDIF // database/DLL options
#include <afxdtctl.h>		// MFC の Internet Explorer 4 コモン コントロール サポート
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC の Windows コモン コントロール サポート
#endif // _AFX_NO_AFXCMN_SUPPORT
$$IF(CHtmlView)
#include <afxhtml.h>			// MFC HTML ビュー サポート
$$ENDIF

$$IF(SOCKETS)
#include <afxsock.h>		// MFC のソケット拡張機能
$$ENDIF //SOCKETS
$$IF(CRichEditView)
#include <afxrich.h>		// MFC リッチ エディット クラス
$$ENDIF //CRichEditView
$$IF(ATL_SUPPORT)
#include <atlbase.h>
extern CComModule _Module;
$$ENDIF
$$IF(OLEDB)
#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>
#include <atldbcli.h>
#include <afxoledb.h>
$$ENDIF
$$IF(PROJTYPE_DLG)
$$IF(AUTOMATION)

$$IF(VERBOSE)
// このマクロは bMultiInstance パラメータ用に COleObjectFactory コンストラクタ
// TRUE を渡す以外は IMPLEMENT_OLECREATE と同じです。
// オートメーション コントローラによって要求される各オートメーション プロキシ
// オブジェクトに対して起動されるようにこのアプリケーションのインスタンスを分けます。
$$ENDIF //VERBOSE
#ifndef IMPLEMENT_OLECREATE2
#define IMPLEMENT_OLECREATE2(class_name, external_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	AFX_DATADEF COleObjectFactory class_name::factory(class_name::guid, \
		RUNTIME_CLASS(class_name), TRUE, _T(external_name)); \
	const AFX_DATADEF GUID class_name::guid = \
		{ l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } };
#endif // IMPLEMENT_OLECREATE2
$$ENDIF //AUTOMATION
$$ENDIF //PROJTYPE_DLG

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
