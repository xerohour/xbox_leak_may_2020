// $$dlg_hfile$$.h : ヘッダー ファイル
//

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

$$IF(AUTOMATION)
class $$DLGAUTOPROXY_CLASS$$;

$$ENDIF //AUTOMATION
/////////////////////////////////////////////////////////////////////////////
// $$DLG_CLASS$$ ダイアログ

class $$DLG_CLASS$$ : public $$DLG_BASE_CLASS$$
{
$$IF(AUTOMATION)
	DECLARE_DYNAMIC($$DLG_CLASS$$);
	friend class $$DLGAUTOPROXY_CLASS$$;

$$ENDIF
// 構築
public:
	$$DLG_CLASS$$(CWnd* pParent = NULL);	// 標準のコンストラクタ
$$IF(AUTOMATION)
	virtual ~$$DLG_CLASS$$();
$$ENDIF

// ダイアログ データ
	//{{AFX_DATA($$DLG_CLASS$$)
	enum { IDD = IDD_$$SAFE_ROOT$$_DIALOG };
		// メモ: この位置に ClassWizard によってデータ メンバが追加されます。
	//}}AFX_DATA

	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL($$DLG_CLASS$$)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV のサポート
	//}}AFX_VIRTUAL

// インプリメンテーション
protected:
$$IF(AUTOMATION)
	$$DLGAUTOPROXY_CLASS$$* m_pAutoProxy;
$$ENDIF
	HICON m_hIcon;
$$IF(AUTOMATION)

	BOOL CanExit();
$$ENDIF //AUTOMATION

	// 生成されたメッセージ マップ関数
	//{{AFX_MSG($$DLG_CLASS$$)
	virtual BOOL OnInitDialog();
$$IF(ABOUT)	
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
$$ENDIF
$$IF(HELP)
	afx_msg void OnDestroy();
$$ENDIF
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
$$IF(AUTOMATION)
	afx_msg void OnClose();
	virtual void OnOK();
	virtual void OnCancel();
$$ENDIF //AUTOMATION
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
