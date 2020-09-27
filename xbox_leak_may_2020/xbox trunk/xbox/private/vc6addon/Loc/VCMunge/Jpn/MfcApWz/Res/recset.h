// $$recset_hfile$$.h : $$RECSET_CLASS$$ クラスの宣言およびインターフェイスの定義をします。
//
/////////////////////////////////////////////////////////////////////////////

#if !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
#define $$FILE_NAME_SYMBOL$$_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

$$IF(!OLEDB)
$$IF(DAO)
class $$RECSET_CLASS$$ : public CDaoRecordset
$$ELSE
class $$RECSET_CLASS$$ : public CRecordset
$$ENDIF
{
public:
$$IF(DAO)
	$$RECSET_CLASS$$(CDaoDatabase* pDatabase = NULL);
$$ELSE
	$$RECSET_CLASS$$(CDatabase* pDatabase = NULL);
$$ENDIF
	DECLARE_DYNAMIC($$RECSET_CLASS$$)

// フィールド/パラメータ データ
$$IF(DAO)
	//{{AFX_FIELD($$RECSET_CLASS$$, CDaoRecordset)
$$ELSE
	//{{AFX_FIELD($$RECSET_CLASS$$, CRecordset)
$$ENDIF
$$RECSET_VARS$$
	//}}AFX_FIELD$$PARAM_VARS$$

// オーバーライド
	// ClassWizard は仮想関数のオーバーライドを生成します
	//{{AFX_VIRTUAL($$RECSET_CLASS$$)
	public:
$$IF(DAO)
	virtual CString GetDefaultDBName();	// レビュー: コメントの取得
$$ELSE
	virtual CString GetDefaultConnect();	// デフォルト の接続文字列
$$ENDIF
	virtual CString GetDefaultSQL(); 	// Recordset の デフォルト SQL
$$IF(DAO)
	virtual void DoFieldExchange(CDaoFieldExchange* pFX);	// RFX のサポート
$$ELSE
	virtual void DoFieldExchange(CFieldExchange* pFX);	// RFX のサポート
$$ENDIF
	//}}AFX_VIRTUAL

// インプリメンテーション
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

};

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
$$ELSE  // !!OLEDB --> OLEDB
class $$DB_TABLECLASS$$
{
public:
	$$DB_TABLECLASS$$()
	{
		memset( (void*)this, 0, sizeof(*this) );
	};

$$DB_VARSINFO$$

BEGIN_COLUMN_MAP($$DB_TABLECLASS$$)
$$DB_COLSINFO$$
END_COLUMN_MAP()

};

class $$RECSET_CLASS$$ : public CCommand<CAccessor<$$DB_TABLECLASS$$> >
{
public:

	HRESULT Open()
	{
		CDataSource 	db;
		CSession	session;
		HRESULT		hr;

		CDBPropSet	dbinit(DBPROPSET_DBINIT);
$$DB_OPEN$$
		hr = db.OpenWithServiceComponents("$$DB_CONNECT$$", &dbinit);
		if (FAILED(hr))
			return hr;

		hr = session.Open(db);
		if (FAILED(hr))
			return hr;

		CDBPropSet	propset(DBPROPSET_ROWSET);
		propset.AddProperty(DBPROP_CANFETCHBACKWARDS, true);
		propset.AddProperty(DBPROP_IRowsetScroll, true);
		propset.AddProperty(DBPROP_IRowsetChange, true);
		propset.AddProperty(DBPROP_UPDATABILITY, DBPROPVAL_UP_CHANGE | DBPROPVAL_UP_INSERT | DBPROPVAL_UP_DELETE );

		hr = CCommand<CAccessor<$$DB_TABLECLASS$$> >::Open(session, "$$DB_SOURCE$$", &propset);

		if (FAILED(hr))
			return hr;

		return MoveNext();
	}

};

//{{AFX_INSERT_LOCATION}}
// $$INSERT_LOCATION_COMMENT$$

#endif // !defined($$FILE_NAME_SYMBOL$$_INCLUDED_)
$$ENDIF // !OLEDB

