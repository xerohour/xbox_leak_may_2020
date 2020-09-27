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


class CUnplacedListView : public CMyListView
{
public:
    BOOL Init(HWND hwndParent);
    void UpdateItem(void *pvItem, DWORD dwItemState, int iColumn, HDC hdcDest, RECT *prcDest);
    DWORD GetItemWidth(HDC hdc, void *pvItem, int iColumn);

    int CompareObjects(const void *pv1, const void *pv2);

    virtual BOOL Override_LButtonDblClick(sMouseInfo *pmi, DWORD dwFlags);
    virtual BOOL Override_LButtonDown(sMouseInfo *pmi, DWORD dwFlags);
    virtual BOOL Override_MouseMove(sMouseInfo *pmi, DWORD dwFlags);

    void AddObject(void *pvObj);
    void RemoveObject(void *pvObj);
    void RefreshObject(void *pvObj);

private:
    HFONT m_hfont;
    HIMAGELIST m_himlFileIcons;
};
