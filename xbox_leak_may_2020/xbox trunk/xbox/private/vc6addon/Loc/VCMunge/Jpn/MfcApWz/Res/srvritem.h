// $$srvritem_hfile$$.h : $$SRVRITEM_CLASS$$ クラスの宣言およびインターフェイスの定義をします。
//

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class $$SRVRITEM_CLASS$$ : public $$SRVRITEM_BASE_CLASS$$
{
	DECLARE_DYNAMIC($$SRVRITEM_CLASS$$)

// コンストラクタ
public:
	$$SRVRITEM_CLASS$$($$DOC_CLASS$$* pContainerDoc);

// アトリビュート
	$$DOC_CLASS$$* GetDocument() const
		{ return ($$DOC_CLASS$$*)$$SRVRITEM_BASE_CLASS$$::GetDocument(); }

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します。
	//{{AFX_VIRTUAL($$SRVRITEM_CLASS$$)
	public:
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
	//}}AFX_VIRTUAL

// インプリメンテーション
public:
	~$$SRVRITEM_CLASS$$();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void Serialize(CArchive& ar);   // ドキュメント I/O に対してオーバーライドされます。
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
