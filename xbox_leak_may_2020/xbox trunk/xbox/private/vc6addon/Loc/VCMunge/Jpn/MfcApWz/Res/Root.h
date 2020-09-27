// $$root$$.h : $$ROOT$$ アプリケーションのメイン ヘッダー ファイル
//

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // メイン シンボル

/////////////////////////////////////////////////////////////////////////////
// $$APP_CLASS$$:
// このクラスの動作の定義に関しては $$root$$.cpp ファイルを参照してください。
//

class $$APP_CLASS$$ : public $$APP_BASE_CLASS$$
{
public:
	$$APP_CLASS$$();

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL($$APP_CLASS$$)
	public:
	virtual BOOL InitInstance();
$$IF(PROJTYPE_MDI)
$$IF(NODOCVIEW)
	virtual int ExitInstance();
$$ENDIF
$$ENDIF
	//}}AFX_VIRTUAL

// インプリメンテーション
$$IF(FULL_SERVER || MINI_SERVER || CONTAINER_SERVER || AUTOMATION)
	COleTemplateServer m_server;
$$IF(VERBOSE)
		// ドキュメントを作成するためのサーバー オブジェクト
$$ENDIF
$$ENDIF
$$IF(NODOCVIEW)
$$IF(PROJTYPE_MDI)
protected:
	HMENU m_hMDIMenu;
	HACCEL m_hMDIAccel;
$$ENDIF

public:
$$ENDIF
	//{{AFX_MSG($$APP_CLASS$$)
	afx_msg void OnAppAbout();
$$IF(NODOCVIEW)
$$IF(PROJTYPE_MDI)
	afx_msg void OnFileNew();
$$ENDIF
$$ENDIF
$$IF(VERBOSE)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
$$ENDIF
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

$$IF(VBX)
/////////////////////////////////////////////////////////////////////////////
// VB イベントの外部宣言

//{{AFX_VBX_REGISTER()
//}}AFX_VBX_REGISTER
$$ENDIF //VBX

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
