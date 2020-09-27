// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      TreeView.h
// Contents:  
// Revisions: 15-Oct-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

enum {IMAGE_XROOT = 0, IMAGE_XDRIVE, IMAGE_UNCHECK, IMAGE_CHECK, IMAGE_TRISTATE,
      IMAGE_FOLDEROPEN, IMAGE_FOLDERCLOSED};

class CFolderInfo;
class CFileHierarchy;
class CTreeView
{
public:
    CTreeView();
    BOOL Init(HWND hwndParent);
    void Uninit();

    BOOL Populate(CFileObject *pfo);
    BOOL HandleMouseDown(int nX, int nY);
    void Resize(RECT *prc);
    void HandleSelection(LPNMTREEVIEW pnmhdr);

    void SetCurFolder(CFileObject *pfo);

    void AddObject(CFileObject *pfo);
    void RemoveObject(CFileObject *pfo);
    void Clear();
    BOOL SetCheckState(HTREEITEM hItem, eCheckState checkstate);
    BOOL RecurseSetCheckState(HTREEITEM hItem, eCheckState checkstate);
    void RecurseDownSetState(HTREEITEM htiCur, eCheckState checkstate);
    void RecurseUpSetState(HTREEITEM htiCur);
    void SetVisible(bool fVisible) { ShowWindow(m_hwnd, fVisible ? SW_SHOW : SW_HIDE); 
                                     ShowWindow(m_hwndStatic, fVisible ? SW_SHOW : SW_HIDE); }

private:

    BOOL InitImageList();
    HTREEITEM AddItem(CFileObject *pfo, int nLevel, HTREEITEM htiParent);
    void ToggleCheckState(HTREEITEM htiCur);
    eCheckState GetCheckState(HTREEITEM hItem);
    INT GetTVItemLPARAM(HTREEITEM hti);
    
    HIMAGELIST m_himl;
    HWND m_hwnd, m_hwndStatic;
};


