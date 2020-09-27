========================================================================
       MICROSOFT FOUNDATION CLASS ライブラリ : $$Root$$
========================================================================


$$IF(PROJTYPE_DLL)
AppWizard が生成した $$Root$$ には、Microsoft Foundation Class の基本的な
使い方が示されています。DLL 作成のひな型としてお使いください。

このファイルには $$Root$$ を構成している各ファイルの概要説明が含まれています。
$$ELSE
AppWizard が生成した $$Root$$ には、Microsoft Foundation Class の基本的な
使い方が示されています。アプリケーション作成のひな型としてお使いください。

このファイルには $$Root$$ を構成している各ファイルの概要説明が含まれています。
$$ENDIF //PROJTYPE_DLL

$$Root$$.dsp
    このファイル (プロジェクト ファイル) はプロジェクト レベルの情報を含み、
    シングル プロジェクトまたはサブ プロジェクトのビルドに使用されます。他の
    ユーザーとプロジェクト ファイル (.dsp) を共有できますが、メイク ファイル
    はローカルにエクスポートしてください。

$$IF(PROJTYPE_DLL)
$$IF(EXTDLL)
$$Root$$.cpp
    このファイルは DllMain() の定義を含む DLL のメイン ソースファイルです。
$$ELSE //!EXTDLL
$$Root$$.h
	このファイルは DLL ファイルのメイン ヘッダー ファイルです。
	$$APP_CLASS$$ アプリケーションクラスを宣言します。

$$Root$$.cpp
	このファイルは $$APP_CLASS$$ アプリケーションクラスを含むアプリケーション
	の中心となるソースファイルです。
$$IF(AUTOMATION)
	プロシージャ内のサーバーが要求する OLE エントリ ポイント も含んでいます。
$$ENDIF //AUTOMATION
$$ENDIF //EXTDLL

$$IF(AUTOMATION)
$$Root$$.odl
    このファイルはアプリケーションのタイプ ライブラリ用のオブジェクト記述言語
    ソース コードを含んでいます。
$$ENDIF //AUTOMATION
$$ELSE //!PROJTYPE_DLL
$$Root$$.h
    このファイルはアプリケーションの中心となるインクルードファイルです。この
    ファイルは他のプロジェクトの固有のインクルードファイル（Resource.h  も含
    みます） をインクルードし、また $$APP_CLASS$$ アプリケーションクラスを宣
    言します。

$$Root$$.cpp
    このファイルは $$APP_CLASS$$ アプリケーションクラスを含むアプリケーション
    の中心となるソースファイルです。
$$ENDIF //PROJTYPE_DLL

$$Root$$.rc
    このファイルはプログラムが使用する Microsoft Windows のリソースを列挙しま
    す。このファイルは RES サブディレクトリに保存されているアイコン、ビットマ
    ップ、カーソルを含みます。このファイルは Microsoft Visual C++ で直接
    編集可能です。

$$Root$$.clw
    このファイルは ClassWizard  が既存のクラスの編集やクラスを新規作成すると
    きに使う情報を持っています。またこのファイルは、ClassWizard によるメッセ
    ージ マップやダイアログ データ マップの編集や生成、プロトタイプ メンバ関
    数の生成に必要な情報も持っています。
$$IF(!PROJTYPE_DLL)

res\$$Root$$.ico
    このファイルは、アプリケーションのアイコンとして使用されるアイコンファイ
    ルです。このアイコンはリソース ファイル $$Root$$.rc によってインクルード
    されます。
$$ENDIF //!PROJTYPE_DLL

res\$$Root$$.rc2
    このファイルは Microsoft Visual C++ によって編集されないリソースを含んでいま
    す。このファイルにリソース エディタで編集されないすべてのリソースを記述
    してください。
$$IF(!PROJTYPE_DLL)
$$IF(CONTAINER_SERVER || FULL_SERVER || MINI_SERVER || AUTOMATION || HAS_SUFFIX)

$$Root$$.reg
    このレジストリ ファイルはフレームワークの設定法を紹介するためのサンプル
    ファイルです。
$$IF(PROJTYPE_DLG)
    アプリケーションと一緒に使用するファイルです。
$$ELSE //!DLG
    アプリケーションと一緒に使用するか、または削除してデフォルトの登録 
     RegisterShellFileTypes を使用します。
$$ENDIF //PROJTYPE_DLG
$$ENDIF //OLE || HAS_SUFFIX

$$IF(AUTOMATION)
$$Root$$.odl
    このファイルはアプリケーションのタイプ ライブラリ用のオブジェクト記述言語
    ソース コードを含んでいます。
$$ENDIF //AUTOMATION
$$ENDIF //!PROJTYPE_DLL

$$IF(PROJTYPE_DLL)
$$Root$$.def
    このファイルは DLL が Microsoft Windows 上で実行される時に必要な情報を
    持っています。例えば DLL の名前や説明、エクスポートされる関数などのパラ
    メータを定義します。
$$ENDIF //PROJTYPE_DLL

$$IF(!PROJTYPE_DLL)
$$IF(!PROJTYPE_DLG)
/////////////////////////////////////////////////////////////////////////////

メイン フレーム ウィンドウ:

$$FRAME_HFILE$$.h, $$FRAME_IFILE$$.cpp
    これらのファイルはフレーム クラス $$FRAME_CLASS$$ を含みます。
$$IF(PROJTYPE_MDI)
    フレーム クラス $$FRAME_CLASS$$ は CMDIFrameWnd クラスから派生し、MDI の
    フレームを制御します。
$$ELSE
    フレーム クラス $$FRAME_CLASS$$ は CFrameWnd クラスから派生し、SDI の
    フレームを制御します。
$$ENDIF

$$IF(TOOLBAR)
res\Toolbar.bmp
    このビットマップ ファイルはツールバーのイメージとして使います。 ツールバ
    ーやステータスバーの初期設定は $$FRAME_CLASS$$ クラスによって構築されま
    す。ツールバーのボタンコントロールを追加したい時にはツールバーのビットマップを
    リソースエディタで編集し、$$Root$$.rc ファイル内の IDR_MAINFRAME TOOLBAR 配列を
    変更してください。
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)

res\IToolbar.bmp
    サーバー アプリケーションが他のコンテナで起動されたときに、このビット
    マップ ファイルがツールバーのイメージとして使われます。このツールバーは
    $$IPFRAME_CLASS$$ クラスによって構築されます。
    このビットマップは、多くの非サーバー コマンドが除かれていることを除いて 
    res\Toolbar.bmp 内のビットマップと共通です。

$$ENDIF //SERVER
$$ENDIF //TOOLBAR
$$IF(PROJTYPE_MDI)	//If an MDI app, it will contain a ChildFrm class
/////////////////////////////////////////////////////////////////////////////

チャイルド フレーム ウィンドウ:

ChildFrm.h, ChildFrm.cpp
    これらのファイルは CChildFrame クラスの定義とインプリメンテーションを行います。
	このクラスは MDI アプリケーションの子ウィンドウをサポートするためのものです。

$$ENDIF //PROJTYPE_MDI
/////////////////////////////////////////////////////////////////////////////

$$IF(!NODOCVIEW)
$$IF(!DB_NO_FILE)
AppWizard は 1 つのドキュメントタイプと 1 つのビューを作成します。

$$DOC_HFILE$$.h, $$DOC_IFILE$$.cpp - ドキュメント
    これらのファイルは $$DOC_CLASS$$ クラスを含みます。特別なドキュメント デ
    ータを付け加えたり、$$DOC_CLASS$$::Serialize を使ったファイルのセーブや
    ロード機能をインプリメントしたいときはこのファイルを編集してください。
$$ELSE
AppWizard は 1 つのビューを作成します。
$$ENDIF

$$VIEW_HFILE$$.h, $$VIEW_IFILE$$.cpp - ビュー
    これらのファイルは $$VIEW_CLASS$$ クラスを含みます。
$$IF(!DB_NO_FILE)
    $$VIEW_CLASS$$ オブジェクトは $$DOC_CLASS$$ オブジェクトを表示するために
    使います。
$$ENDIF
$$IF(CRecordView)
//ODBC データベース アプリケーション:
$$RECSET_HFILE$$.h, $$RECSET_IFILE$$.cpp - レコードセット
	これらのファイルはレコードセット クラス $$RECSET_CLASS$$ の定義とインプリメンテーションを
	行います。このクラスはデータベースのクエリとコレクションを実行するためのものです。

$$ENDIF
$$IF(CDaoRecordView)
//DAO データベース アプリケーション:
$$RECSET_HFILE$$.h, $$RECSET_IFILE$$.cpp - レコードセット
	これらのファイルはレコードセット クラス $$RECSET_CLASS$$ の定義とインプリメンテーションを
	行います。このクラスはデータベースのクエリとコレクションを実行するためのものです。

$$ENDIF
$$IF(PROJTYPE_MDI)
res\$$DOC_IFILE$$.ico
    このファイルは $$DOC_CLASS$$ クラスの MDI 子ウィンドウ用アイコンとして使
    われるアイコンファイルです。このアイコンはリソースファイル $$Root$$.rc
    によってインクルードされます。
$$ENDIF //MDI
$$ENDIF

$$IF(CONTAINER || FULL_SERVER || MINI_SERVER || CONTAINER_SERVER)
/////////////////////////////////////////////////////////////////////////////

AppWizard は OLE のリンクと埋め込みに関するクラスも作成しています。

$$IF(CONTAINER || CONTAINER_SERVER)
$$CNTRITEM_HFILE$$.h, $$CNTRITEM_IFILE$$.cpp 
	このクラスは OLE ｵﾌﾞｼﾞｪｸﾄの操作に使われます。OLE オブジェクトは
	$$VIEW_CLASS$$ クラス によって表示され、$$DOC_CLASS$$ クラスの一
	部分としてシリアライズされます。

$$ENDIF
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
$$SRVRITEM_HFILE$$.h, $$SRVRITEM_IFILE$$.cpp 
    このクラスは $$DOC_CLASS$$ クラスを OLE システムに接続し、必要であれ
    ばドキュメントにリンクを設定するために使われます。
$$IPFRAME_HFILE$$.h, $$IPFRAME_IFILE$$.cpp 
    このクラスは COleIPFrameWnd から派生し、埋め込み先で起動されている間
    すべてのフレームの機能を制御します。
$$ENDIF
$$ENDIF
$$ELSE //!!PROJTYPE_DLG

/////////////////////////////////////////////////////////////////////////////

$$IF(AUTOMATION)
AppWizard は 1 つのダイアログ クラスおよびオートメーション プロキシ クラスを作成します:
$$ELSE //!AUTOMATION
AppWizard は 1 つのダイアログ クラスを作成します:
$$ENDIF //AUTOMATION

$$DLG_HFILE$$.h, $$DLG_IFILE$$.cpp - ダイアログ
    これらのファイルは $$DLG_CLASS$$ クラスを含みます。このクラスは 
    アプリケーションのメイン ダイアログの動作を定義します。ダイアログ
    テンプレートは Microsoft Visual C++ で編集可能な $$Root$$.rc に含
    まれます。
$$IF(AUTOMATION)

$$DLGAUTOPROXY_HFILE$$.h, $$DLGAUTOPROXY_IFILE$$.cpp - オートメーション オブジェクト 
    これらのファイルは $$DLGAUTOPROXY_CLASS$$ クラスを含みます。このクラスは
    ダイアログ用の "オートメーション プロキシ" クラスと呼ばれます、理由はオー
    トメーション コントローラがダイアログへアクセスするために使用可能なオート
    メーション メソッドおよびプロパティの表示を行うためです。これらのメソッド
    およびプロパティはダイアログ クラスから直接表示されません、それはダイアログ
    ベースの MFC アプリケーション モデルの場合、オートメーション オブジェクト
    とユーザー インターフェイスを別々にしておくことが簡潔で簡単だからです。
$$ENDIF //AUTOMATION

$$ENDIF //!PROJTYPE_DLG
$$IF(HELP)
/////////////////////////////////////////////////////////////////////////////

ヘルプ サポート:

hlp\$$Root$$.hpj
    このファイルはアプリケーションのヘルプ ファイルを作成するときにヘルプ
    コンパイラが使用するヘルプ プロジェクト ファイルです。

hlp\*.bmp
    これらのファイルは Microsoft Foundation Class ライブラリの標準コマンドに
    関する標準ヘルプ ファイルのトピックで使用するビットマップ ファイルです。

hlp\*.rtf
    これらのファイルは Microsoft Foundation Class ライブラリの標準コマンドや
    スクリーン オブジェクトに関するトピックを持つ標準ヘルプ ファイルのソースです。
$$ENDIF //HELP

$$ENDIF //!PROJTYPE_DLL
/////////////////////////////////////////////////////////////////////////////
その他の標準ファイル:

StdAfx.h, StdAfx.cpp
    これらのファイルはプリコンパイル済ヘッダー ファイル (PCH) $$Root$$.pch 
    やプリコンパイル済型ファイル StdAfx.obj を構築するために使われるファイルです。

Resource.h
    このファイルは新規リソース ID を定義する標準ヘッダー ファイルです。
    Microsoft Visual C++ はこのファイルを読み込み更新します。

/////////////////////////////////////////////////////////////////////////////
その他の注意:

"TODO:" で始まるコメントは、ソースコードの追加やカスタマイズの必要な場所を示
します。
$$IF(PROJTYPE_MDI || PROJTYPE_SDI || PROJTYPE_DLG)

アプリケーションで共有 DLL 内で MFC を使用する場合や、アプリケーションが
オペレーティング システムで現在使用している言語以外の言語を使用している場合は、
Microsoft Visual C++ CD-ROM から、ローカライズされたリソースに対応する
MFC42XXX.DLL を system または system32 ディレクトリにコピーして、ファイル名を 
MFC42LOC.DLL にする必要があります。("XXX" は言語の省略形を現します。例えば、
MFC42JPN.DLL は日本語に翻訳されたリソースを含みます。)
このようにしなかった場合には、アプリケーション内のいくつかの UI 要素はオペ
レーティング システムの言語のまま残ります。
$$ENDIF //MDI,SDI,DLG

/////////////////////////////////////////////////////////////////////////////
