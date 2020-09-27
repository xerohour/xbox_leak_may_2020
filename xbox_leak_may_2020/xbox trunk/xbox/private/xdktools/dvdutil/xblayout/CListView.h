// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      clistview.h
// Contents:  
// Revisions: 14-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CWindow;
class CObject;
class CObjList;
class CObj_Group;
class CWorkspace;
class CObjNodeList;

// Number of columns to display
#define NUM_COLUMNS 4

class CListView : public CInitedObject
{
public:
    int m_iLayer;
    bool Init(CWindow *pwindow, bool fInsideOut, bool fDisplayLSNs);
    ~CListView();

    void MoveTo(int nX, int nY, int nW, int nH);

    void Clear();
    void AddObject(CObject *pobj);
    void AddObjectAtIndex(CObject *pobj, int iIndex);

    HWND GetHwnd() {return m_hwnd; }
    
    int CompareProc(LPARAM lParam1, LPARAM lParam2);
    void SetVisible(bool fVisible);

    void SelectAll();
    HWND m_hwnd;

    void BeginDrag();
    void EndDrag(POINT pt);
    void ItemSelected(int iItem, bool fSelected);

    BOOL WndProc(UINT uMsg, WPARAM wparam, LPARAM lparam);

    bool MoveSelection(WORD wVKey);

    void DrawInsertionBar(POINT ptScreen);
    void ErasePreviousSelectionBar();
    bool GetListOfSelectedObjects(CObjList *polToMove);
    bool RemoveObjectList(CObject *rgpol[]);
    bool RemoveObject(CObject *po);

    void ClearSelected();

    int m_iPrevBarEntry;
    int m_iDropPoint;
    bool m_fInsideOut;
    CObject *GroupSelectedItems(CObj_Group *pogNew);
    void UngroupSelectedItems(CWorkspace *pws, int nLayer);

    CObject *m_pobjDropAt;

    void ObjectDropped();
    CObject *GetFirstSelectedObject();
    CObject *GetLastSelectedObject();

    CObject *GetObjectFromIndex(int iItem);
    int GetIndexFromObject(CObject *pobj);
    bool CheckDroppedOnSelf();

    bool HandleDrawItem(LPDRAWITEMSTRUCT pdis);
    bool CanItemChange(int iItem, DWORD dwOldState, DWORD dwNewState);

    void GetSelectedFileCountAndSize(int *pcFiles, int *pnbyFileSize);

    void StoreSelectedObjList();
    void ClearSelectedObjList();
    bool m_fSelectedObjList;
    bool m_fModified;
	void Sort();
    void SetFocus(bool fHaveFocus);
private:

    void GetColumnSizes(int rgnColSize[NUM_COLUMNS]);
    int  GetLineLength();
    int  GetHeaderHeight();
    int  GetColumnHeight();
    int  GetTotalNumItems();
    int  GetNumVisibleRows();
    int  GetTopRow();

    bool IsGroupable(int iPrevItem, int iItem);
    bool IsUngroupable(int iItem);

    void UpdateDrag(POINT pt);

    void HandleFocusState(LPDRAWITEMSTRUCT pdis);
    void HandleSelectionState(LPDRAWITEMSTRUCT pdis, char *szBuf);
    void DrawGridLines(LPDRAWITEMSTRUCT pdis);

    WNDPROC m_wndprocOld;
    char m_rgszColumnNames[NUM_COLUMNS][MAX_PATH];
    bool m_fInvert;
    bool m_fDragging;
    CWindow *m_pwindow;
    RECT m_rc;
    bool m_fSelectingAll;
    int m_iScrollPos;
    int m_nColumns;
    void CheckDragScroll();
    bool m_fPrevTop, m_fPrevBot;

    DWORD m_dwMouseX, m_dwMouseY;
    bool m_fHaveFocus;
    bool IsItemSelected(int iItem);
    int m_iFocusItem;
    HBITMAP m_hbmp, m_hbmpSel;
    int m_fSelecting;
};
