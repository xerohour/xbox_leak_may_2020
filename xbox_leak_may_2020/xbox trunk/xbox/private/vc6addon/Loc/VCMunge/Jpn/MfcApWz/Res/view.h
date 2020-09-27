// $$view_hfile$$.h : $$VIEW_CLASS$$ クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

$$IF(CONTAINER || CONTAINER_SERVER)
class $$CNTRITEM_CLASS$$;
$$ENDIF
$$IF(CRecordView || CDaoRecordView || COleDBRecordView)
class $$RECSET_CLASS$$;
$$ENDIF

class $$VIEW_CLASS$$ : public $$VIEW_BASE_CLASS$$
{
protected: // シリアライズ機能のみから作成します。
	$$VIEW_CLASS$$();
	DECLARE_DYNCREATE($$VIEW_CLASS$$)
$$IF(CRecordView || CDaoRecordView || COleDBRecordView || CFormView)

public:
	//{{AFX_DATA($$VIEW_CLASS$$)
	enum{ IDD = IDD_$$SAFE_ROOT$$_FORM };
$$IF(CRecordView || CDaoRecordView || COleDBRecordView)
	$$RECSET_CLASS$$* m_pSet;
$$ENDIF
	// メモ: ClassWizard によってこの位置にデータ メンバが追加されます。
	//}}AFX_DATA
$$ENDIF

// アトリビュート
public:
	$$DOC_CLASS$$* GetDocument();
$$IF(CONTAINER || CONTAINER_SERVER)
$$IF(!CRichEditView)
$$IF(VERBOSE)
	//  m_pSelection は現在の $$CNTRITEM_CLASS$$ に対する選択を保持しています。
	// 多くのアプリケーションでこのようなメンバ変数は、複数の選択や $$CNTRITEM_CLASS$$
	// オブジェクトではないオブジェクトの選択を十分に表現できません。
	// この機構は選択機能を応用しようとするプログラマの理解を助けるた
	// めに組み込まれています。

	// TODO: この選択方法をアプリケーションに適した方法に置き換えてください。
$$ENDIF //VERBOSE
	$$CNTRITEM_CLASS$$* m_pSelection;
$$ENDIF //!CRichEditView
$$ENDIF //CONTAINERS

// オペレーション
public:

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL($$VIEW_CLASS$$)
	public:
$$IF(CRecordView)
	virtual CRecordset* OnGetRecordset();
$$ENDIF
$$IF(CDaoRecordView)
	virtual CDaoRecordset* OnGetRecordset();
$$ENDIF
$$IF(COleDBRecordView)
	virtual CRowset* OnGetRowset();
$$ENDIF
$$IF(CFormView || CRecordView || CDaoRecordView || COleDBRecordView)
$$ELIF(!CRichEditView);
	virtual void OnDraw(CDC* pDC);  // このビューを描画する際にオーバーライドされます。
$$ENDIF // !(CFormView || CRecordView || CDaoRecordView)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
$$IF(CFormView || CRecordView || CDaoRecordView || COleDBRecordView)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV のサポート
$$ENDIF //CFormView || CRecordView || CDaoRecordView
$$IF(CScrollView || CONTAINER || CONTAINER_SERVER || CRecordView || CDaoRecordView || COleDBRecordView || CTreeView || CListView || CFormView || CHtmlView)
	virtual void OnInitialUpdate(); // 構築後の最初の１度だけ呼び出されます。
$$ENDIF
$$IF(PRINT)
$$IF(!CHtmlView)
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
$$ENDIF
$$IF(CRichEditView||CHtmlView)
$$ELSE
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
$$ENDIF //!CRichEditView||CHtmlView
$$IF(CFormView || ACTIVE_DOC_CONTAINER)
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
$$ENDIF //CFormView
$$ENDIF //PRINT
$$IF(CONTAINER || CONTAINER_SERVER)
$$IF(!CRichEditView)
	virtual BOOL IsSelected(const CObject* pDocItem) const;// コンテナ サポート
$$ENDIF //!CRichEditView
$$ENDIF //CONTAINER || CONTAINER_SERVER
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	virtual ~$$VIEW_CLASS$$();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成されたメッセージ マップ関数
protected:
	//{{AFX_MSG($$VIEW_CLASS$$)
$$IF(VERBOSE)
		// メモ -  ClassWizard はこの位置にメンバ関数を追加または削除します。
		//         この位置に生成されるコードを編集しないでください。
$$ENDIF
$$IF(CONTAINER || CONTAINER_SERVER)
	afx_msg void OnDestroy();
$$IF(!CRichEditView)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnInsertObject();
	afx_msg void OnCancelEditCntr();
$$ENDIF //!CRichEditView
$$ENDIF //CONTAINER || CONTAINER_SERVER
$$IF(MINI_SERVER || FULL_SERVER || CONTAINER_SERVER)
	afx_msg void OnCancelEditSrvr();
$$ENDIF //SERVERS
	//}}AFX_MSG
$$IF(PROJTYPE_EXPLORER)
$$IF(CListView)
	afx_msg void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
$$ENDIF
$$ENDIF
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // $$view_ifile$$.cpp ファイルがデバッグ環境の時使用されます。
inline $$DOC_CLASS$$* $$VIEW_CLASS$$::GetDocument()
   { return ($$DOC_CLASS$$*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
