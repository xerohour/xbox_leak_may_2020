// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CWindow_Scratch.h
// Contents:  
// Revisions: 14-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CWindow_Scratch : public CWindow
{
public:
    CWindow_Scratch(DWORD dwWindowCreationFlags, CWindow *pwindowParent);
    ~CWindow_Scratch();
    bool Init();
    void Uninit();

    void ToggleView();
    void AddObject(CObject *pobj);
    void OnSize();

    bool m_fVisible;
    void FinishedDrag(CListView *plv, POINT pt);
    void DragMove(POINT pt);
    void EraseAllSelectionBars();
    bool InsertObjectList(CObjList *pol);

    bool OnNotify(WPARAM wparam, LPARAM lparam);
    void Reset() {m_plistview->Clear(); }
    BOOL DerivedWndProc(UINT uMsg, WPARAM wparam, LPARAM lparam);
    
    bool PersistTo(CFile *pfile);
    bool CreateFrom(CFile *pfile);

    bool HandleFileRemoval(CObjList *pol);
    void SetGroupable(bool fGroupable);
    void SetUngroupable(bool fUngroupable);
    CObject *FindObjectByFullFileName(char *szName);

    friend class CWindow_Main;
    CListView *m_plistview;
private:
    CWindow *m_pwindowParent;
};

