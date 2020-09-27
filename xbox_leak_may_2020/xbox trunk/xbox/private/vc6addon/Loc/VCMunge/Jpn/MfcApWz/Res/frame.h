// $$frame_hfile$$.h : $$FRAME_CLASS$$ クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
$$IF(PROJTYPE_SDI)
$$IF(PROJTYPE_EXPLORER)


class $$VIEW_CLASS$$;
$$ELIF(NODOCVIEW)


#include "$$wndview_hfile$$.h"
$$ENDIF
$$ENDIF 

$$IF(PROJTYPE_MDI)
class $$FRAME_CLASS$$ : public $$FRAME_BASE_CLASS$$
{
	DECLARE_DYNAMIC($$FRAME_CLASS$$)
public:
	$$FRAME_CLASS$$();
$$ELSE // SDI
class $$FRAME_CLASS$$ : public $$FRAME_BASE_CLASS$$
{
	
$$IF(NODOCVIEW)
public:
	$$FRAME_CLASS$$();
protected: 
	DECLARE_DYNAMIC($$FRAME_CLASS$$)
$$ELSE
protected: // シリアライズ機能のみから作成します。
	$$FRAME_CLASS$$();
	DECLARE_DYNCREATE($$FRAME_CLASS$$)
$$ENDIF
$$ENDIF // PROJTYPEMDI

// アトリビュート
$$IF(SPLITTER_SDI || PROJTYPE_EXPLORER)
$$IF(PROJTYPE_SDI)
protected:
	CSplitterWnd m_wndSplitter;
$$ENDIF 
$$ENDIF //SPLITTER_SDI
public:

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL($$FRAME_CLASS$$)
$$IF(SPLITTER_SDI || FRAME_STYLES || PROJTYPE_EXPLORER)
	public:
$$ENDIF
$$IF(SPLITTER_SDI || PROJTYPE_EXPLORER)
$$IF(PROJTYPE_SDI)
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
$$ENDIF 
$$ENDIF
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
$$IF(NODOCVIEW)
$$IF(PROJTYPE_SDI)
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
$$ENDIF 
$$ENDIF
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~$$FRAME_CLASS$$();
$$IF(PROJTYPE_SDI)
$$IF(PROJTYPE_EXPLORER)
$$IF(CListView)
	$$VIEW_CLASS$$* GetRightPane();
$$ENDIF	// CListView
$$ENDIF	// PROJTYPE_EXPLORER
$$ENDIF	// PROJTYPE_SDI
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
$$IF(TOOLBAR || STATUSBAR || REBAR)

protected:  // コントロール バー用メンバ
$$IF(STATUSBAR)	
	CStatusBar  m_wndStatusBar;
$$ENDIF //STATUSBAR
$$IF(TOOLBAR)
	CToolBar    m_wndToolBar;
$$ENDIF //TOOLBAR
$$IF(REBAR)
	CReBar      m_wndReBar;
	CDialogBar      m_wndDlgBar;
$$ENDIF //REBAR
$$ENDIF //TOOLBAR || STATUSBAR || REBAR
$$IF(NODOCVIEW)
$$IF(PROJTYPE_SDI)
	$$WNDVIEW_CLASS$$    m_wndView;
$$ENDIF 
$$ENDIF

// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG($$FRAME_CLASS$$)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
$$IF(NODOCVIEW)
$$IF(PROJTYPE_SDI)
	afx_msg void OnSetFocus(CWnd *pOldWnd);
$$ENDIF 
$$ENDIF
$$IF(VERBOSE)
		// メモ - ClassWizard はこの位置にメンバ関数を追加または削除します。
		//        この位置に生成されるコードを編集しないでください。
$$ENDIF
	//}}AFX_MSG
$$IF(PROJTYPE_EXPLORER)
$$IF(PROJTYPE_SDI)
$$IF(CListView)
	afx_msg void OnUpdateViewStyles(CCmdUI* pCmdUI);
	afx_msg void OnViewStyle(UINT nCommandID);
$$ENDIF
$$ENDIF
$$ENDIF
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
