// watchrow.h : header file
//

/////////////////////////////////////////////////////////////////////////////

enum {COL_NAME, COL_VALUE, WATCH_NUM_COLUMNS /* should be last */};

// Corresponds to one row in a Watch Pane.

// HitTest return values
enum { WRH_NONE, WRH_GLYPH, WRH_SELMARGIN, WRH_NAME, WRH_VALUE };

class CWatchRow : public CTreeGridRow
{
public:
	CWatchRow(CTreeGridWnd *pGrid, CTM *pTM, CTreeGridRow *pParent = NULL );
	~CWatchRow( );
	typedef CTreeGridRow CBaseClass;

	// virtual constructor for the CWatchRow object, which also allocates the memory.
	virtual CWatchRow * NewWatchRow(CTreeGridWnd *, CTM *, CTreeGridRow *);

	virtual BOOL IsExpandable( ) const;
	virtual BOOL GetChildren(CTreeGridRowArray&);
	virtual DWORD GetChildCount();

	virtual void Update();
	virtual void Restart();	

	virtual void DrawCell(CDC *pDC, const CRect& cellRect, int nColumn);
	virtual CSize GetCellSize(CDC *pDC, int nColumn) const ;
	virtual void ResetSize(CDC *pDC);
	virtual void OnActivate(BOOL bActivate, int nColumn);
	virtual void GetColumnText( int nColumn, CString& str);
	virtual void GetValueStr(CString &str);	// Special func which always gives the display value string.
	virtual BOOL  OnAccept (CWnd* pControlWnd );
	virtual int GetNextControlColumn(int nColumn);
	virtual int	GetPrevControlColumn(int nColumn);
	virtual void OnControlNotify(UINT nControlId,  UINT nNotification);
	virtual void AdjustForMargins(CRect& rect, int nColumn);
	virtual int Compare(const CGridRow* pCompareRow) const;
	virtual void OnSelect(BOOL bSelect);

	// Override to handle the case where the user modified the value of a variable.
	virtual void OnModify();

	CRect   ComputeGlyphRect(CDC *pDC, const CRect& cellRect);
	CRect	GetGlyphRect(const CRect& cellRect ) const;
	virtual  UINT HitTest(CPoint);
	virtual  BOOL OnLButtonDown(UINT, CPoint );
	virtual  BOOL OnLButtonUp(UINT, CPoint );
	virtual  BOOL OnLButtonDblClk(UINT, CPoint );
	virtual  BOOL OnMouseMove(UINT, CPoint );
		
	// Override to ensure that controls created are dockable.
	virtual CEdit* NewEdit( );
	virtual CComboBox* NewCombo( );

	virtual	void AdjustControlRect( GRID_CONTROL_TYPE, int nColumn, CRect& rect);

// Helper functions
public:
	inline CTM * GetTM() const;
	inline CTreeGridWnd * GetTreeGrid();
	inline BOOL IsRetVal( ) const;
	inline int GetPlusColWidth( ) const;
	inline void SetValueChanged(BOOL bSet = TRUE );
	inline BOOL IsValueChanged( );

protected:
	CTM * m_pTM; // Our TM.

	BOOL m_bValueBeingEdited:1 ;
	BOOL m_bHasSynthChild:1;
	BOOL m_bValueChanged:1;

	CSize m_sizeM;	// size of the character "M" in the current font.
	CSize m_dispGlyph ;

	enum { cxMargin = 4 , cyMargin = 1 };

public:	
	enum { cxBitmap = 11, cyBitmap = 11, cxBeforeBmap = 7, cxAfterBmap = 7, cyExtra = 6};

	enum { cxMinPlusColWidth = cxBeforeBmap + cxBitmap + cxAfterBmap };
	enum { cyMinPlusColHeight = cyBitmap + cyExtra };

	enum { IMG_PLUS = 0, IMG_MINUS = 1, IMG_RETVAL = 2 };
	static CImageWell s_imgPlusMinus;
	static int s_cUsage;
											
	friend class CNewWatchRow;
};


class CNewWatchRow : public CTGNewRow
{
public:
	CNewWatchRow(CDbgGridWnd *);
	~CNewWatchRow();
	typedef CTGNewRow CBaseClass;
public:
	virtual void ResetSize(CDC *pDC);
        virtual CSize GetCellSize(CDC *pDC, int nColumn) const;
	virtual void DrawCell(CDC *pDC, const CRect& cellRect, int nColumn);
	virtual void AdjustForMargins(CRect&, int nColumn);
	virtual void OnActivate(BOOL bActivate, int nColumn);
	virtual void OnControlNotify(UINT nControlId,  UINT nNotification);
	virtual BOOL OnMouseMove(UINT nFlags, CPoint);
	virtual int Compare(const CGridRow* pCompareRow) const;
	virtual void OnSelect(BOOL bSelect);

	virtual CEdit* NewEdit();
	virtual CComboBox * NewCombo();
	
	inline CTreeGridWnd * GetTreeGrid();
};


inline CTM * CWatchRow::GetTM( ) const
{
	return m_pTM;
}

inline CTreeGridWnd * CWatchRow::GetTreeGrid( )
{
	return (CTreeGridWnd *)GetGrid( );
}

inline BOOL CWatchRow::IsRetVal( ) const
{
	return GetTM()->IsKindOf(RUNTIME_CLASS(CReturnTM));
}

inline int CWatchRow::GetPlusColWidth() const
{
	return cxMinPlusColWidth + m_nLevels * cxBitmap;
}

inline CTreeGridWnd * CNewWatchRow::GetTreeGrid( )
{
	return (CTreeGridWnd *)GetGrid( );
}

inline void CWatchRow::SetValueChanged(BOOL bSet /* = TRUE*/)
{
	m_bValueChanged = bSet;
}

inline BOOL CWatchRow::IsValueChanged( )
{
	return m_bValueChanged || GetTM()->DidValueChange();
}
