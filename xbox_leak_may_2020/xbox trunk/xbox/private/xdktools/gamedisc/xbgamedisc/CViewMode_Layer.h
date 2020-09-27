// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CViewMode_Layer.h
// Contents:  
// Revisions: 31-Oct-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef enum eLayer {LV_LAYER0 = 0, LV_LAYER1, LV_UNPLACED};
typedef enum eLayerViewMode {LVM_LAYER0, LVM_LAYER1, LVM_BOTHLAYERS};

class CViewMode_Layer : public CViewMode
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

    CUnplacedWindow m_unplaced;
private:
    void Resize();
    void ViewLayer(eLayerViewMode lvm, DWORD dwCmd);

    CLayerListView m_rglv[3];
    CSplitter m_splitter;
    eLayerViewMode m_lvmCur;
    int m_nW, m_nH;
    BOOL m_fVisible;
};

extern CViewMode_Layer g_vmLayer;
