// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CMyListView.h
// Contents:  
// Revisions: 6-Nov-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef struct
{
    int nX, nY;
    RECT rcItem;
    RECT rcSubItem;
    int iRow, iColumn;
    void *pvItem;
} sMouseInfo;

extern DWORD GetKeyFlags();

typedef struct
{
    void *pvData;
    DWORD dwFlags;
} sListItem;

#define LVF_ITEMSELECTED 1
#define LVF_ITEMFOCUSED  2

#define MAX_COLUMNS 100
#define MK_FORCESEL 0x80000000

class CMyListView
{
public:
    BOOL Init(HWND hwndParent);
    void AddColumn(char *szName, UINT nWidth);
    BOOL DeleteItem(void *pvItem);
    BOOL AddItemAtIndex(void *pvitem, int iLoc);
    void SetVisible(bool fVisible);
    void Resize(RECT *prc);

    void SelectAllItems();
    void UpdateListDisplay();

    void Clear();
    
    LRESULT WndProc(UINT uMsg, WPARAM wparam, LPARAM lparam);

    HWND m_hwnd;

    virtual void UpdateItem(void *pvItem, DWORD dwItemState, int iColumn, HDC hdcDest, RECT *prcDest) = NULL;
    virtual DWORD GetItemWidth(HDC hdc, void *pvItem, int iColumn) = NULL;

    virtual BOOL Override_LButtonDown(sMouseInfo *pmi, DWORD dwFlags) {return TRUE;}
    virtual BOOL Override_LButtonDblClick(sMouseInfo *pmi, DWORD dwFlags) {return TRUE;}
    virtual BOOL Override_LButtonUp(sMouseInfo *pmi, DWORD dwFlags) {return TRUE;}
    virtual BOOL Override_MouseMove(sMouseInfo *pmi, DWORD dwFlags) {return TRUE;}
    
    virtual void Notify_SelectChange(void *pvObj, BOOL fSelected) {}
    virtual int CompareObjects(const void *pv1, const void *pv2) = NULL;
    int m_fInvertSort;
    virtual void AddObject(void *pvObj) = NULL;
    virtual void RemoveObject(void *pvObj) = NULL;

    void UpdateIfDirty();
    int GetHeaderHeight() {return m_nHeaderHeight;}

    BOOL m_fDirty;
protected:
    int m_iSortColumn;
    void SortColumn(int iColumn);
    BOOL GetItemAtPoint(int nX, int nY, int *piItem, int *piColumn);
    void RefreshItem(int iItem);
    void UnselectAllItems();
    void SelectItem(int iItem, DWORD dwFlags);
    void UnselectItem(int iItem);
    void SelectSingleItem(int iItem);
    BOOL IsSelected(int iItem);

    sListItem *m_rgli;
    int m_cItems;

    void SetFocusItem(int iFocusItem);
private:

    void HandleKeyDown(DWORD dwVKey);

    void GetMouseInfoAt(int nX, int nY, sMouseInfo *pmi);

    int GetTopItem();
    int GetBottomItem();

    void HandleButtonDown(int nX, int nY, DWORD dwFlags);
    void HandleButtonDblClick(int nX, int nY, DWORD dwFlags);
    void HandleButtonUp(int nX, int nY, DWORD dwFlags);
    void HandleMouseMove(int nX, int nY, DWORD dwFlags);

    void UpdateAllItems();


    BOOL InitHeader();
    int GetHeaderWidth();
    HWND m_hwndHeader;
    int m_nHeaderHeight;
    int m_nHeaderWidth;

    void UpdateScrollBars();
    void UpdateColumnWidths();

    void HandleHeaderDoubleClick(LPNMHEADER pnmhdr);
    BOOL DrawHeader(LPDRAWITEMSTRUCT pdis);


    int GetTotalItemHeight();
    int m_nItemHeight;

    int m_rgnColWidths[MAX_COLUMNS];
    int m_nCurHScrollLoc;
    int m_nMaxHScroll;
    int m_nCurVScrollLoc;
    int m_nMaxVScroll;

    RECT m_rcStartSize;
    BOOL m_fSizing;
    
    HFONT m_hfont;

    int m_iFocusItem;
    int m_iShiftItem;
    bool m_fHaveFocus;

    HIMAGELIST m_himlSortArrows;

    int m_cMaxItems;
    int m_cAddItemDelta;
};
