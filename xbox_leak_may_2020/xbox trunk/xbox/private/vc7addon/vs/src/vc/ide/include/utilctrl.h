/////////////////////////////////////////////////////////////////////////////
//  UTILCTRL.H
//      Utility control extensions for use in packages:
//
//      -Grid control
//      -Tree control
//      -Check listbox
//      -Icon listbox
//      -Menu button
//      -Tabbed scrollbar

#ifndef __UTILCTRL_H__
#define __UTILCTRL_H__

#ifndef __DLGBASE_H__
#include "dlgbase.h"
#endif

#ifndef __SHLMENU_H__
#include "shlmenu.h"
#endif

#ifndef __SHLBAR_H__
#include "shlbar.h"
#endif

#ifndef __UTIL_H__
#include "util.h"
#endif

class CSlob;
class CPartFrame;

//#undef AFX_DATA
//#define AFX_DATA AFX_EXT_DATA

////////////////////////////////////////////////////////////////////////////
// Grid Classes

class CGridElement;
    class CGridCell;
    class CGridRow;
        class CGridControlRow;
//CWnd
    class CGridWnd;
        class CGridControlWnd;

#define ID_GRD_NEW                      0x4ed0
#define ID_GRD_DELETE                   0x4ed1
#define ID_GRD_MOVEUP                   0x4ed2
#define ID_GRD_MOVEDOWN                 0x4ed3


/////////////////////////////////////////////////////////////////////////////
//  CGridToolBar
//      This is the grid's toolbar 

class CGridToolBar : public CToolBar
{
public:
    enum {
        sizeButtonX = 22,
        sizeButtonY = 20,
        sizeImageX = 14,
        sizeImageY = 12,
    };

    CGridToolBar();
    void SetSizes();
};


/////////////////////////////////////////////////////////////////////////////
//  CStaticVCenter
//      This is a static control that draws text (single line) vertically centered

class CStaticVCenter : public CStatic
{
public:
    afx_msg void OnPaint();
    afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpParams);
    afx_msg void OnNcPaint();

    DECLARE_MESSAGE_MAP()
};

////////////////////////////////////////////////////////////////////////////
// CGridElement -- common base class for things in grids

class CGridElement
{
public:
    virtual ~CGridElement();
    
    void SetCapture();
    void ReleaseCapture();

    void Invalidate(BOOL bErase = FALSE, BOOL bInflate = FALSE);
    
    virtual CGridWnd* GetGrid() const = 0;
    virtual void Draw(CDC* pDC, const CRect& rect) = 0;
    virtual void GetRect(CRect& rect) = 0;
    virtual void ResetSize(CDC* pDC);
    virtual void OnSelect(BOOL bSelected);
    
    virtual CGridElement* ElementFromPoint(const CPoint& point);

    virtual BOOL OnLButtonDown(UINT nFlags, CPoint point);
    virtual BOOL OnLButtonDblClk(UINT nFlags, CPoint point);
    virtual BOOL OnLButtonUp(UINT nFlags, CPoint point);
    virtual BOOL OnMouseMove(UINT nFlags, CPoint point);
    
    virtual void OnActivate(BOOL bActivate, int nColumn);
    virtual void OnControlNotify(UINT nControlID, UINT nNotification);
    virtual CGridRow* GetRow() const;

    BOOL m_bIsCell;
    BOOL m_bWasSelected;
};

/////////////////////////////////////////////////////////////////////////////
// CGridRow -- a row in a grid

class CGridRow : public CGridElement
{
public:
    CGridRow(CGridWnd* pGrid);
    ~CGridRow();
    
    CGridWnd* GetGrid() const
        { return m_pGrid; }
    
    int GetRowHeight() const
        { return m_cyHeight; }

    virtual void GetRect(CRect& rowRect);
    virtual int Compare(const CGridRow* pCompareRow) const;
    
    virtual CSize GetCellSize(CDC* pDC, int nColumn) const;
    
    virtual void Draw(CDC* pDC, const CRect& rect);
    virtual void DrawCell(CDC* pDC, const CRect& rect, int nColumn) = 0;
    
protected:  
    CGridWnd* m_pGrid;
    int m_cyHeight;
    
    friend class CGridWnd;
};

/////////////////////////////////////////////////////////////////////////////
// CGridCell -- once cell in the grid; often sub-classed

class CGridCell : public CGridElement
{
public:
    CGridCell();
    
    virtual void Draw(CDC* pDC, const CRect& cellRect);
    
    virtual CGridRow* GetRow() const;
    
    CGridWnd* GetGrid() const
        { return m_pRow->GetGrid(); }
    
    int GetColumnIndex() const
        { return m_nColumn; }
    
    void Invalidate(BOOL bErase = FALSE);
    void GetRect(CRect& cellRect);
    CSize GetSize() const
        { return m_size; }
    
    virtual void SetText(const TCHAR* szText);
    virtual void ResetSize(CDC* pDC);
    
    CSize m_size;
    CString m_str;
    CGridRow* m_pRow;
    int m_nColumn;
    UINT m_dt; // DrawText() format
};

/////////////////////////////////////////////////////////////////////////////
// CGridControlRow -- a row that supports edit and combo controls

enum GRID_CONTROL_TYPE { none, edit, editCombo, listCombo };

class CGridControlRow : public CGridRow
{
public:
    CGridControlRow(CGridWnd* pGrid);
    ~CGridControlRow();
    
    BOOL CreateControl(GRID_CONTROL_TYPE controlType, int nColumn, DWORD dwStyle = 0);

    // Override to create specialized controls.
    virtual CEdit*  NewEdit( );
    virtual CComboBox* NewCombo( ); 
    virtual void AdjustControlRect ( GRID_CONTROL_TYPE, int nColumn, CRect& textRect);
        
    void GetColumnRect(int nColumn, CRect& rect);
    
    virtual void OnChangeColumnText(int nColumn, const char* szNewText);
    virtual BOOL OnAccept(CWnd* pControlWnd);

    virtual void GetColumnText(int nColumn, CString& str) = 0;
    virtual void AdjustForMargins(CRect& rect, int nColumn);
    virtual int GetNextControlColumn(int nColumn);
    virtual int GetPrevControlColumn(int nColumn);
    virtual void DrawCell(CDC* pDC, const CRect& cellRect, int nColumn);
    virtual void AddListItems(int nColumn, CComboBox* pComboBox);

// Implementation
    void OnControlNotify(UINT nControlID, UINT nNotification);
};

#define cxDefaultGridCellMargin 8
#define cyDefaultGridCellMargin 2

/////////////////////////////////////////////////////////////////////////////
// CGridCellRow -- a row of cells in a grid; rarely sub-classed

class CGridCellRow : public CGridRow
{
public:
    CGridCellRow(CGridWnd* pGrid);
    ~CGridCellRow();
    
    CGridCell* GetCell(int nColumn) const
        { return (CGridCell*)m_cells[nColumn]; }
    
    CSize GetCellSize(int nColumn) const
        { return GetCell(nColumn)->GetSize(); }
    
    virtual void ResetSize(CDC* pDC);
    
    virtual int Compare(const CGridRow* pCompareRow) const;
    
    virtual BOOL CreateCells();
    virtual CGridElement* ElementFromPoint(const CPoint& point);
    virtual void DrawCell(CDC* pDC, const CRect& rect, int nColumn);

protected:  
    CPtrArray m_cells;
};


////////////////////////////////////////////////////////////////////////////
// CGridCaptionCell -- special row at the top of a grid; rarely subclassed

class CGridCaptionCell : public CGridCell
{
public:
    CGridCaptionCell(BOOL bDisplay);
    
    virtual void Draw(CDC* pDC, const CRect& cellRect);

    virtual BOOL OnLButtonDown(UINT nFlags, CPoint point);
    virtual BOOL OnLButtonDblClk(UINT nFlags, CPoint point);
    virtual BOOL OnLButtonUp(UINT nFlags, CPoint point);
    virtual BOOL OnMouseMove(UINT nFlags, CPoint point);
    virtual void ResetSize(CDC* pDC);

    void InvertResizeTracker(CDC* pDC = NULL);
    void SizeFromTracker();

    enum HitZone
    {
        none,
        resizeLeft,
        select,
        resizeRight
    };
    
    HitZone GetHitZone(const CPoint& point);
    
    HitZone m_trackZone; // are we tracking the mouse? why?
    BOOL m_bPressed : 1; // does the caption have that "depressed" look?
    BOOL m_bFixedWidth : 1; // is this column fixed width.
    int m_xSizeTracker; // where is the resize tracking line?
};


////////////////////////////////////////////////////////////////////////////
// CGridCaption -- special row at the top of a grid; rarely subclassed

class CGridCaption : public CGridCellRow
{
public:
    CGridCaption(CGridWnd* pGrid, BOOL bDisplay = TRUE);
    
    void GetRect(CRect& rowRect);
    
    int GetColumnWidth(int nColumn) const
        { return GetCell(nColumn)->m_size.cx; }

    // If column is fixed width, resizing using the mouse is disallowed.
    BOOL IsColumnFixedWidth(int nColumn) const
        { return ((CGridCaptionCell *)GetCell(nColumn))->m_bFixedWidth; }

    void SetColumnFixedWidth(int nColumn, BOOL bFixed = TRUE)
        { ((CGridCaptionCell *)GetCell(nColumn))->m_bFixedWidth = !!bFixed; }
        
    virtual void ResetSize(CDC* pDC);
    BOOL m_bDisplay;
};


/////////////////////////////////////////////////////////////////////////////
// CGridWnd window

// ReplaceControl toolbar flags. T
// You may OR these
#define GRIDWND_TB_NOTOOLBAR    (0)
#define GRIDWND_TB_NEW      (0x0001) // New  button
#define GRIDWND_TB_DELETE   (0x0002) // Delete button
#define GRIDWND_TB_MOVE     (0x0004) // Move Buttons

#define GRIDWND_TB_NEWDEL   (GRIDWND_TB_NEW | GRIDWND_TB_DELETE) // New and Delete buttons
#define GRIDWND_TB_ALL      (GRIDWND_TB_NEW | GRIDWND_TB_DELETE | GRIDWND_TB_MOVE) // New. Delete, Moveup, MoveDown

class CGridWnd : public CWnd
{
    DECLARE_DYNAMIC(CGridWnd)
// Construction
public:
    CGridWnd(int nColumns, BOOL bDisplayCaption = TRUE );
    ~CGridWnd();
    
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
    BOOL CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
    BOOL ReplaceControl(CWnd* pWnd, UINT nIDTemp, UINT nIDGrid, 
        DWORD dwStyle = WS_VISIBLE | WS_CHILD | WS_VSCROLL | WS_TABSTOP, 
        DWORD dwExStyle = WS_EX_CLIENTEDGE,
        DWORD dwTBStyle = GRIDWND_TB_NOTOOLBAR,
        BOOL bRemovePlaceholder = TRUE);
    

// Attributes
public:
    int GetRowCount() const
        { return (int)m_rows.GetCount() - 1; }
    
    int GetColumnCount() const
        { return m_nColumnCount; }
        
    int GetColumnWidth(int nColumn) const
        { return GetCaptionRow()->GetColumnWidth(nColumn); }

    CFont* GetFont () const 
    {   return m_pFont; }

    CFont* GetCaptionFont() const  
    { return m_pCaptionFont ; }
    
    COLORREF GetWindowTextColor() const
    { return (m_bSysDefaultColors) ? GetSysColor(COLOR_WINDOWTEXT) : m_colorWindowText; }

    COLORREF GetWindowColor() const
    { return (m_bSysDefaultColors) ? GetSysColor(COLOR_WINDOW) : m_colorWindow; }
    
    COLORREF GetHighlightColor() const
    { return (m_bSysDefaultColors) ? GetSysColor(COLOR_HIGHLIGHT) : m_colorHighlight; }
    
    COLORREF GetHighlightTextColor() const
    { return (m_bSysDefaultColors) ? GetSysColor(COLOR_HIGHLIGHTTEXT) : m_colorHighlightText; }
            
    int ColumnFromX(int x);

// Operations
public:
    void SetColumnCaption(int nColumn, const char* szCaption);
    void SetColumnWidth(int nColumn, int cxWidth);
    void InvalidateColumn(int nColumn, BOOL bErase = FALSE);
    virtual void SortColumn(int nColumn, BOOL bReverse = FALSE);
    void ReSort();
    void SizeColumnToFit(int nColumn);
    CGridElement* ElementFromPoint(const CPoint& point);
    CGridCellRow* AddCellRow();
    virtual void AddRow(CGridRow* pRow, BOOL bRedraw = TRUE );
    virtual void RemoveRow(CGridRow* pRow, BOOL bRedraw = TRUE );
    void SetFont(CFont * pFont, BOOL bRedraw = TRUE );
    void SetCaptionFont( CFont * pFont, BOOL bRedraw = TRUE );
    void SetColor(COLORREF colorText, COLORREF colorBk, COLORREF colorHighlight,
                    COLORREF colorHighlightText); 
    void EnableMultiSelect( BOOL fEnable = TRUE );
    void EnableAutoSort( BOOL bEnable = TRUE );
    void EnableSort(BOOL bEnable = TRUE);

// Overridables
    virtual BOOL BeginDrag(UINT nFlags, const CPoint& point); // return FALSE to drag select, TRUE for D&D
    virtual void DoDragDrop(CPoint point);
    virtual BOOL ProcessKeyboard(MSG* pMsg, BOOL bPreTrans = FALSE);
    virtual void AdjustDropRow(CGridRow* &rpDropRow);
    
// Implementation
public:
    BOOL SubclassDlgItem(UINT nID, CWnd* pParent);
    
    void ResetSize();
    void ResetScrollBars();

// Row access...
    POSITION GetHeadRowPosition() const;
    
    CGridRow* GetNextRow(POSITION& pos) const
        { return (CGridRow*)m_rows.GetNext(pos); }
    
    CGridRow* GetPrevRow(POSITION& pos) const
        { return (CGridRow*)m_rows.GetPrev(pos); }
    
    CGridRow* GetRowAt(POSITION pos) const
        { return (CGridRow*)m_rows.GetAt(pos); }
        
    CGridCellRow* GetCellRowAt(POSITION pos) const
        { VSASSERT(m_bHasCells, "Gotta have cells to get one!"); return (CGridCellRow*)m_rows.GetAt(pos); }
        
    CGridRow* GetRow(int nRow) const
        { return  nRow == -1 ? NULL : (CGridRow*)m_rows.GetAt(m_rows.FindIndex(nRow + 1)); }
    
    CGridCaption* GetCaptionRow() const
        { return (CGridCaption*)m_rows.GetHead(); }
    
// Selection iteration
    POSITION GetHeadSelPosition() const
        { return m_selection.GetHeadPosition(); }
    
    CGridElement* GetNextSel(POSITION& pos) const
        { return (CGridElement*)m_selection.GetNext(pos); }

        
    virtual void VScroll(int cyScroll);
    
    void ClearSelection();
    void ShowSelection(BOOL bShow = TRUE);
    void Select(CGridElement* pElement, BOOL bAdd = FALSE);
    void Deselect(CGridElement* pElement);
    void SelectBetween(CGridElement* pElement1, CGridElement* pElement2);
    void InvalidateSelection(CGridElement* pElement = NULL);
    BOOL IsSelected(CGridElement* pElement) const
        { return m_selection.Find(pElement) != NULL; }
    BOOL LooksSelected(CGridElement* pElement) const
        { return m_bShowSelection && IsSelected(pElement); };
    BOOL IsPrimary(CGridElement* pElement) const
        { return !m_selection.IsEmpty() && m_selection.GetHead() == pElement; }
        
    void ScrollIntoView(CGridRow* pRow);
    
    void SetActiveElement(CGridElement* pElement, int nColumn = -1);
    virtual BOOL CloseActiveElement();

    void AdjustTopVisible();
    
    CPtrList m_rows;
    int m_nColumnCount;
    
    POSITION m_posTopVisibleRow; // first visible row
    int m_cyScroll; // number of pixels of first row scrolled off top
    
    CGridElement* m_pCaptureElement;
    
    POSITION m_posCurRow;
    int m_nCurColumn;
    CPtrList m_selection;
    CGridElement* m_pActiveElement;
    BOOL m_bShowSelection;
    
    int m_nSortColumn;
    BOOL m_bReverseSort;
    
    BOOL m_bHasCells;
    
    CFont* m_pFont ;  // Font to display normal text.
    CFont* m_pCaptionFont ; 

    // Color handling.
    COLORREF m_colorWindowText;
    COLORREF m_colorWindow;
    COLORREF m_colorHighlightText;
    COLORREF m_colorHighlight;
    BOOL     m_bSysDefaultColors;

    BOOL m_bDragSelecting;
    CGridElement* m_pFirstSelect; // where the mouse went down for a drag-select
    CGridElement* m_pLastSelect;
    CGridRow* m_pDropRow;
    CRect m_rcDropInvalidate;
    BOOL m_bEnableEllipse;
    CButton m_Ellipse;

    BOOL m_fMultiSelect;    // TRUE if grid should support multiselect
    BOOL m_bAutoSort:1;         // TRUE if grid should resort after accepting an entry
    BOOL m_bShowContextPopup:1;
    BOOL m_bShowProperties:1;
    BOOL m_bDestruct:1; // TRUE while in the dtor
    BOOL m_bSort:1;     // TRUE if the grid should support any kind of sorting. 
    BOOL m_bEnableDragRow:1;
    BOOL m_bDragRow:1;
    BOOL m_bBtnDownValid:1; // turned off by killfocus during lbuttondown processing

    BOOL m_bParentIsDialog;

    int m_cxSelectMargin;
    static CPoint c_LastClick;

private:
    CStaticVCenter  m_cstatVCenter; // for subclassing toolbar label drawing
    CGridToolBar* m_pToolBar;
    
public: 
    CStaticVCenter* GetPwndGridLabel()  { return &m_cstatVCenter; }
    CGridToolBar*   GetToolBar()    { return m_pToolBar; }

protected:
    void MoveDown(BOOL bShift, BOOL bControl);
    void MoveUp(BOOL bShift, BOOL bControl);

    // Message map functions
    //{{AFX_MSG(CGridWnd)
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnEnable(BOOL bEnable);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CGridControlWnd

class CGridControlWnd : public CGridWnd
{
    DECLARE_DYNAMIC(CGridControlWnd)
public:
    CGridControlWnd(int nColumns, BOOL bDisplayCaption = TRUE);
    ~CGridControlWnd();
    
    GRID_CONTROL_TYPE m_controlType;
    CWnd* m_pControlWnd;
    int m_nControlColumn;

    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL ProcessKeyboard(MSG* pMsg, BOOL bPreTrans = FALSE);
    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
    virtual void CGridControlWnd::CancelDrag(void);

    // Control handling
    virtual void ActivatePrevControl();
    virtual void ActivateNextControl();
    virtual void CancelControl();
    virtual BOOL AcceptControl(BOOL bDeactivate = TRUE);
    virtual BOOL CloseActiveElement();
    virtual BOOL FGridToolBarCmd(UINT nID, int nCode);


    virtual BOOL OnCmdMsg( UINT nID, int nCode, void *pExtra, AFX_CMDHANDLERINFO *pHandlerInfo );


protected:
    afx_msg void OnGridNew();           //  New button pressed 
    afx_msg void OnGridDelete();        //  Delete button pressed 
    afx_msg void OnGridMoveUp();        //  Move Up button pressed 
    afx_msg void OnGridMoveDown();      //  Move Down button pressed 
    afx_msg void OnDestroy();

    // Message map functions
protected:
    //{{AFX_MSG(CGridWnd)
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnCut();
    afx_msg void OnCopy();
    afx_msg void OnPaste();
    afx_msg void OnClear();
    afx_msg void OnUndo();
    void OnUpdateCut(CCmdUI *pCmdUI);
    void OnUpdateCopy(CCmdUI *pCmdUI);
    void OnUpdatePaste(CCmdUI *pCmdUI);
    void OnUpdateClear(CCmdUI *pCmdUI);
    void OnUpdateUndo(CCmdUI *pCmdUI);
    void OnUpdateRedo(CCmdUI *pCmdUI);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#define GLN_SELCHANGE (4)
#define ON_SELCHANGE(id, memberFxn) \
    { WM_COMMAND, (WORD)GLN_SELCHANGE, (WORD)id, (WORD)id, AfxSig_vv, \
        (AFX_PMSG)memberFxn },

#define GLN_ELLIPSE (5)
#define ON_ELLIPSE(id, memberFxn) \
    { WM_COMMAND, (WORD)GLN_ELLIPSE, (WORD)id, (WORD)id, AfxSig_vv, \
        (AFX_PMSG)memberFxn },

#define GLN_SETFOCUS (6)
#define ON_SETFOCUS(id, memberFxn) \
    { WM_COMMAND, (WORD)GLN_SETFOCUS, (WORD)id, (WORD)id, AfxSig_vv, \
        (AFX_PMSG)memberFxn },

#define GLN_KILLFOCUS (7)
#define ON_KILLFOCUS(id, memberFxn) \
    { WM_COMMAND, (WORD)GLN_KILLFOCUS, (WORD)id, (WORD)id, AfxSig_vv, \
        (AFX_PMSG)memberFxn },

#define GLN_ADDSTRING (8)
#define ON_ADDSTRING(id, memberFxn) \
    { WM_COMMAND, (WORD)GLN_ADDSTRING, (WORD)id, (WORD)id, AfxSig_vv, \
        (AFX_PMSG)memberFxn },

#define GLN_DELETESTRING (9)
#define ON_DELETESTRING(id, memberFxn) \
    { WM_COMMAND, (WORD)GLN_DELETESTRING, (WORD)id, (WORD)id, AfxSig_vv, \
        (AFX_PMSG)memberFxn },

#define GLN_ACTIVATE_CTL (10)
#define ON_ACTIVATE_CTL(id, memberFxn) \
    { WM_COMMAND, (WORD)GLN_ACTIVATE_CTL, (WORD)id, (WORD)id, AfxSig_vv, \
        (AFX_PMSG)memberFxn },

#define GLN_ACCEPT_CTL (11)
#define ON_ACCEPT_CTL(id, memberFxn) \
    { WM_COMMAND, (WORD)GLN_ACCEPT_CTL, (WORD)id, (WORD)id, AfxSig_vv, \
        (AFX_PMSG)memberFxn },

/////////////////////////////////////////////////////////////////////////////
//  Tree control
/////////////////////////////////////////////////////////////////////////////
// Values for CNode::m_nFlags
// CNode -- one entry in a CTreeCtl

// Specify packing becuase this gets used accross DLL's:
// REVIEW64(MikhailA): should it be 8 for IA64?
#pragma pack(4)
class CNode
{
public:
    CNode(DWORD_PTR dwData);
    virtual ~CNode();
    
    virtual CFont* GetFont() const;
    virtual int GetGlyphWidth();
    virtual int GetGlyphHeight();

    CNode* m_pParent;
    int m_nLevels;
    DWORD_PTR m_dwData;
    WORD m_nFlags;
    TCHAR* m_szText;
    int  m_nWidth;
    static int m_nGlyphWidth;
};
#pragma pack()

#define TF_LASTLEVELENTRY       0x0001
#define TF_HASCHILDREN          0x0002
#define TF_EXPANDED             0x0004
#define TF_DISABLED             0x0008
#define TF_EXPANDABLE           0x0020
#define TF_NOT_EXPANDABLE       0x0040
#define TF_DIRTY_CONTENT        0x0080
#define TF_DIRTY_ITEM           0x0010
#define TF_DROPTARGET           0x0100
#define TF_DRAGSOURCE           0x0200
#define TF_CHILD_SORT_ALPHA     0x0400
#define TF_CHILD_SORT_OVERRIDE  0x0800
#define TF_CHILD_SORT_NONE      0x1000

#define LIST_ALPHA_SEARCH_DELAY (1)


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CTreeListBox item

class CTreeListBox : public CListBox
{
    DECLARE_DYNCREATE(CTreeListBox)

    CTreeListBox();

private:
    int ItemFromPoint(const CPoint& point);

    class CTreeCtl* m_pTree;
    BOOL m_bGotMouseDown : 1;
    BOOL m_bDeferedDeselection : 1;
    BOOL m_bStdMouse : 1;
    int m_nDeferedItem;
    int m_nLastItem;

    static int iSlobPixels;
    static CPoint mouseDownPt;
    static CPoint m_mouseDownScreenPt;

    friend class CTreeCtl;

protected:
    virtual BOOL    PreTranslateMessage(MSG *pMsg);

    // Generated message map functions
protected:
    //{{AFX_MSG(CTreeListBox)
    afx_msg void OnLButtonDown(UINT, CPoint);
    afx_msg void OnLButtonUp(UINT, CPoint);
    afx_msg void OnLButtonDblClk(UINT, CPoint);
    afx_msg void OnRButtonUp(UINT, CPoint);
    afx_msg void OnRButtonDown(UINT, CPoint);
    afx_msg void OnContextMenu(CWnd* pWnd, CPoint pt);
    afx_msg void OnMouseMove(UINT, CPoint);
    afx_msg void OnChar(UINT nChar, UINT nRepCount, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CTreeCtl window

class CTreeCtl : public CWnd
{
    DECLARE_DYNCREATE(CTreeCtl)
    DECLARE_MESSAGE_MAP()

// Construction
public:
    CTreeCtl(BOOL bIsMultiSelection = FALSE, BOOL bKeepSelectionRect = FALSE);
    
    BOOL PreCreateWindow(CREATESTRUCT& cs);

// Attributes
public:
    virtual void SetContextHelpID(DWORD dwContextHelpId);

protected:
    BOOL m_bIsMultiSelection : 1;   // Allow multiple selections;
    BOOL m_bKeepSelectionRect : 1;  // keep the selecton rect when not focus
    BOOL m_bEnableDragDrop : 1;
    BOOL m_bShowContextPopup : 1;
    BOOL m_bShowProperties : 1;
    BOOL m_bSortRootNodes : 1;      // are root nodes to be sorted alphabetically?
    BOOL m_bVertLineAction : 1;     // is action taken when vertical line clicked on?
    BOOL m_bPopupClippedText : 1;   // pop up full node text when clicked on?

    CTreeListBox *m_pListBox;   // our only child

    CFont * m_pFont;

    CSize m_sizeM;              // size of an "M" in the current font
                                // used to determine horiz/vert tree bar dimensions

    int m_cyNode;               // height of one node in the list
    int m_cxExtent;             // approximate width of the whole list
    int m_cxOffset;             // horizontal offset for paints (used when a widget
                                // scrollbar is used for a horizontal scroll bar)
    
    int m_EnableUpdates;        // Reference count of update holding;

    CString m_strSearch;        // typomatic search string
    time_t  m_timeLastKey;      // time since last typomatic key stroke

    int m_nLastParentIndex;     // used for cacheing next insertion point in unsorted tree portions
    int m_nNextInsertIndex;     // used for cacheing next insertion point in unsorted tree portions

// Operations
public:
    // expansion/contraction

    // override this method so that containment can be determined
    virtual BOOL NodeIsExpandable(CNode *);
    
    // *must* override this method so that the text of a node can be
    // determined through means other than the passed 'sz' arg to InsertNode
    // N.B. generally this is for the a node that was set dirty
    virtual void GetNodeTitle(CNode *, CString &);

    // This can be overriden if you want to display text different to that
    // specified as the 'sz' text. Unlike GetNodeTitle this method does
    // NOT alter the 'sz' text of the node.
    virtual void GetDisplayText(CNode *, CString &);

    // expand/collapse a node (referenced by index)
    void Expand(int nIndex = -1, BOOL bWholeBranch = FALSE);
    void Collapse(int nIndex = -1);
    void Refresh(int nIndex = -1);
    void RefreshAllRoots();

    // expand all nodes (full-expansion)
    void ExpandAll();

    // is a particular node expanded?
    // override this if you want to determine expandedness another way.
    virtual BOOL IsExpanded(int nIndex = -1);
    
    // does a particular node have children?
    // override this if you want to determine child presence another way.
    virtual BOOL HasChildren(int nIndex = -1);

    // get/set the node dwData (referenced by index)
    DWORD_PTR GetData(int nIndex = -1);
    void SetData(int nIndex, DWORD_PTR dwData);

    // get/set the current selection
    int GetCurSel() { return m_pListBox->GetCurSel(); }
    void SetCurSel(int nIndex) { m_pListBox->SetCurSel(nIndex); }
    virtual void KeySetSel(int nIndex, BOOL bClearSearchBuffer = TRUE);

    // get/set the current selection if multi-select is on
    int GetCount() { return m_pListBox->GetCount(); }
    int GetSelCount() { return m_pListBox->GetSelCount(); }
    int GetSelItems(int nMaxItems, LPINT rgItems) { return m_pListBox->GetSelItems(nMaxItems, rgItems); }
    int SetSel(int nIndex, BOOL bSelect = TRUE) { return m_pListBox->SetSel(nIndex, bSelect); }
    void SelectAll()
        { SetSel(-1, TRUE); }

    // our tree-control node modification functions
    int FindNode(DWORD_PTR dwData);
    int FindNode(const TCHAR * szText, DWORD_PTR & dwData, BOOL fMatchCase = FALSE);
    int InsertNode(int iParentNode, const char* szName, DWORD_PTR dwData);
    void DirtyNode(int iNode, WORD, BOOL bInvalItem = TRUE);    // where the 2nd arg. is TF_DIRTY_???
    void ClearNode(int iNode, WORD, BOOL bInvalItem = TRUE);    // where the 2nd arg. is TF_DIRTY_???
    void RemoveNode(int iNode, BOOL bUpdateSel = TRUE);

    virtual void RecalcHorizontalExtent();
    void ClearSearchBuffer();
    
    int ItemFromPoint(const CPoint& point)
        { return m_pListBox->ItemFromPoint(point); }

    int GetItemRect(int nItem, LPRECT lpRect)
        { return m_pListBox->GetItemRect(nItem, lpRect); }

    virtual void CreateListBox()
        { m_pListBox = new CTreeListBox; }

    virtual CListBox* GetListBox()
        { return m_pListBox; }
        
    int GetItemHeight() const
        { return m_cyNode; }
        
    int GetHorzExtent() const
        { return m_cxExtent; }
        
    void SetHorzExtent(int nExtent)
        { m_cxExtent = nExtent; }
        
    int GetHorzOffset() const
        { return m_cxOffset; }

    void SetHorzOffset(int nOffset)
        { m_cxOffset = nOffset; }

    CSize GetSizeM() const
        { return m_sizeM; }

    BOOL PopsUpClippedText()
        { return m_bPopupClippedText; }

    enum HIT_ZONE { left, vertline, expand, glyph, text, right };
    
    HIT_ZONE HitTest(int nItem, int x);
    
    // override these methods to support your own actions for the user input

    virtual void DoOpenItem(int nIndex);            // called on click on +/- glyphs
    virtual void OnOpen(int nIndex);                // called on Enter key or double-click
    virtual void OnSelect(int nIndex);              // called on selection change

    virtual BOOL OnKeyDown(UINT nChar);             // called on any Keydown msg
    virtual BOOL OnChar(UINT nChar);                    // called on any Char msg

    // specific key handlers
    void OnPrimary();
    void OnExpand();
    void OnExpandBranch();
    void OnExpandAll();
    void OnCollapse();
    
    virtual void OnRightButtonUp(UINT, CPoint);     // called on right mouse button up
    virtual void OnRightButtonDown(UINT, CPoint);   // called on right mouse button down
    virtual void OnContextMenu(CWnd* pWnd, CPoint pt);
    
// Implementation
public:
    virtual ~CTreeCtl();
    void FreeAllTreeData();
    
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    virtual BOOL InsertLevel(CNode* pNode, int nIndex, BOOL bExpandAll = FALSE);
    virtual void DrawNodeGlyph(CDC* pDC, CNode* pNode, BOOL bHighlight, CPoint pt);
    
    BOOL InsertNode(CNode* pParentNode, int iParentNode, const char* szName,
                    DWORD_PTR dwData, CNode*& rpNodeRet, int& riNodeRet, BOOL bUseNodeRet = FALSE);
    virtual void Collapse(CNode* pNode, int nIndex);
    void Refresh(CNode* pNode, int nIndex);
    CNode* GetNode(int nIndex = -1);
    void ResetSizes(CDC* pDC);
    void InvalSelection();
    void InvalItem(int nItem);

    int  GetMaxItemExent () { return m_cxExtent; };

    // For save/loading expanded state:
    BYTE* GetInitData();
    void SizeViewToTree(CPartFrame *pFrame);

    void HoldUpdates ();
    void EnableUpdates ();

    virtual int CompareData(const CNode* pNode1, const CNode* pNode2);
    int CompareNodes(const CNode* pNode1, const CNode* pNode2);

    virtual CNode* CreateNode(DWORD_PTR dwData);
    virtual void DeleteNode(CNode* pNode);

    virtual void BeginDragDrop();

    // Generated message map functions

protected:
    //{{AFX_MSG(CTreeCtl)
    afx_msg int OnCharToItem(UINT nChar, CListBox* pListBox, UINT nIndex);
    afx_msg void OnDestroy();
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaint();
    afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
    afx_msg void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    afx_msg void OnLbDblClk();
    afx_msg void OnLbSelChange();
    afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGetFont(WPARAM wParam, LPARAM lParam);
    
    static CImageWell c_imageWell;
    static int c_cyBorder;      // SM_CYBORDER
    static int c_cyBorderX2;    // SM_CYBORDER * 2
    
    friend class CTreeListBox;
};

#define IDCW_TREELISTBOX    1 // child id for listbox
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// Check listbox
//
// A CCheckList is a listbox with a checkbox to the left of each item.
// The user interface is exactly the same as a listbox with these
// exceptions:
//
//     * Left mouse button clicks (or double clicks) on the checkbox
//       part of an item toggle the check state.
//     * Pressing the spacebar when an item is selected will toggle
//       the check state of that item.
//     * For multi-sel listbox's, the spacebar will cycle through the
//       four state combinations (unselected-unchecked, selected-unchecked,
//       unselected-checked, selected-checked).
//
// These controls may be created explicitly with Create(), or existing
// listbox windows may be "subclassed" with SubclassDlgItem().
//
// When creating one of these (or specifying one in a dialog template)
// make sure the LBS_OWNERDRAWFIXED and LBS_HASSTRINGS styles are set.
//
// The class supports different sized fonts and will ensure that items
// are tall enough to contain both the box and the text.  The font may
// be changed dynamically.
//
// The default implementation uses the item data to store the state
// of the checkbox.  If the item data is needed for some other purpose,
// the SetCheck() and GetCheck() functions should be overrided to
// use an alternate storage location.

#define CLN_CHKCHANGE (20)
#define ON_CLN_CHKCHANGE(id, memberFxn) \
    { WM_COMMAND, (WORD)CLN_CHKCHANGE, (WORD)id, (WORD)id, AfxSig_vv, \
        (AFX_PMSG)memberFxn },

class CCheckList : public CListBox
{
// Construction
public:
    CCheckList();
    BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

// Attributes
public:
    virtual BOOL IsTriState(int nItem) {return FALSE;}
    // nCheck -
    // 0 = unchecked
    // 1 = checked
    // 2 = undeterminate
    virtual void SetCheck(int nItem, int nCheck);
    virtual int GetCheck(int nItem);

// Operations
public:
    BOOL SubclassDlgItem(UINT nID, CWnd* pParent);

// Implementation
protected:
    void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
    void MeasureItem(LPMEASUREITEMSTRUCT lpmis);
    void InvalidateCheck(int nItem);
    void ResetItemHeight();

    virtual void ItemStringOut (CDC * pDC, const CString& strOut, const CRect& rect);
    
    int m_cyItem;   // Height of item including check box
    int m_cyText;   // Height of text only
    static CBitmap c_checkBitmap;
    static CSize c_checkSize;

    // Message map functions
protected:
    //{{AFX_MSG(CCheckList)
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    //}}AFX_MSG
    LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

    virtual void GetDisplayText(int index, CDC *pDC, const CRect&, CString& strOut);
};


/////////////////////////////////////////////////////////////////////////////
// CPropCheckList window - a check-list that can show/set property values

class CPropCheckList : public CCheckList
{
public:
    virtual void OnPropChanged(UINT nIDProp);

    // add an association between item text and property to the list
    int AddItem(const char* szId, UINT nIDProp);

    // which CSlob do the properties in this list belong to?
    inline void SetSlob(CSlob* pSlob) {m_pSlob = pSlob;}

protected:
    // provide property processing
    virtual BOOL IsTriState(int);
    virtual void SetCheck(int, int);
    virtual int GetCheck(int);

private:
    CSlob * m_pSlob;
};

//#undef AFX_DATA
//#define AFX_DATA NEAR

#endif  // __UTILCTRL_H__