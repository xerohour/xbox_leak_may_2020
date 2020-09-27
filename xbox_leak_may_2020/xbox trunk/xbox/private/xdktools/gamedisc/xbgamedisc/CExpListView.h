// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      clistview.h
// Contents:  
// Revisions: 14-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define NUM_COLUMNS 6


class CExpListView : public CMyListView
{
public:
    BOOL Init(HWND hwndParent);
    void UpdateItem(void *pvItem, DWORD dwItemState, int iColumn, HDC hdcDest, RECT *prcDest);
    DWORD GetItemWidth(HDC hdc, void *pvItem, int iColumn);

    int CompareObjects(const void *pv1, const void *pv2);

    virtual BOOL Override_LButtonDblClick(sMouseInfo *pmi, DWORD dwFlags);
    virtual BOOL Override_LButtonDown(sMouseInfo *pmi, DWORD dwFlags);
    virtual BOOL Override_MouseMove(sMouseInfo *pmi, DWORD dwFlags);
    virtual void Notify_SelectChange(void *pvObj, BOOL fSelected);

    void SetDir(CFileObject *pfoFolder);
    void AddObject(void *pvObj);
    void RemoveObject(void *pvObj);
    void RefreshObject(void *pvObj);
    void SetChildrenCheckState(CFileObject *pfo);

private:
    void UpdateCheckState();


    ULARGE_INTEGER m_uliSelectedSize;
    int m_nSelected;
    HBITMAP m_hbmpChecked, m_hbmpUnchecked, m_hbmpUncheckedLit, m_hbmpCheckedLit;
    HFONT m_hfont;
    HIMAGELIST m_himlFileIcons;
    CFileObject *m_pfoDisplay;
    CFileObject *m_poMouseOver;
    int m_iMouseOverItem;
};

