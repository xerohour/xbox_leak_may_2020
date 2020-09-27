// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CViewMode_Explorer.h
// Contents:  
// Revisions: 31-Oct-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CViewMode_Explorer : public CViewMode
{
public:
    BOOL Init(HWND hwndParent);

    void Show();
    void Hide();
    void Resize(int nW, int nH);
    void HandleEvent(eEvent event, CFileObject *pfo);
    void UpdateIfDirty();
    void Clear();
    void HandleMenuCommand(WORD command);

    CTreeView m_treeview;
    CExpListView m_listview;
    CSplitter m_splitter;
};

extern CViewMode_Explorer g_vmExplorer;