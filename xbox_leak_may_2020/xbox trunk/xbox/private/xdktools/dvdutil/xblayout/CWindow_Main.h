// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CWindow_Main.h
// Contents:  
// Revisions: 13-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define LV_LAYER0  0
#define LV_LAYER1  1
#define LV_SCRATCH 2

class CWindow_Main : public CWindow
{
public:
    CWindow_Main(DWORD dwWindowCreationFlags, HINSTANCE hinst);
    BOOL DerivedWndProc(UINT uMsg, WPARAM wparam, LPARAM lparam);
    BOOL ButtonPressed(int idButton);
    bool Init();
    void Uninit();

    void OnSize();
    bool OnNotify(WPARAM wparam, LPARAM lparam);
    void SetCurLayer(int nLayer);

    CListView *m_rgplv[3];
	CListView *m_plvCur;
    CStatusBar *m_pstatusbar;
    CToolBar *m_ptoolbar;
    CWindow_Scratch *m_pscratch;
    CColorBar *m_pcolbarLeft, *m_pcolbarMid, *m_pcolbarRight;

    int GetFocusLayer();
    void FinishedDrag(CListView *plv, POINT pt);
    void DragMove(POINT pt);
    void EraseAllSelectionBars();

    void SetGroupable(bool fGroupable);
    void SetUngroupable(bool fUngroupable);
    void OnGroup();
    void OnUngroup();
    void UpdateStatusBar();
    //void UpdateScratch();

    int m_nCurLayerView;
    bool CheckModifiedFiles();

private:
    // Each window has exactly one workspace.
    CWorkspace *m_pws;

    CListView *GetLVFromHwnd(HWND hwnd);

    bool CMF_HandleFiles();
    bool m_fCMF_FilesInUnplaced;
    bool m_fCMF_FilesResized;

    bool m_fDefaultXbePresent;
    int WhichListViewUnderPt(POINT pt);
    bool BuildForEmulation();
    bool CheckNeedBuildForEmulation();
    char m_szWorkingDir[MAX_PATH];
    HWND m_hwndTip;
};

extern HINSTANCE g_hinst;