// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      cworkspace.h
// Contents:  
// Revisions: 13-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

class CWindow_Main;
class CDVD;

#define MAX_UNDO_DEPTH 100

class CWorkspace : public CInitedObject
{
public:
    CWorkspace(CWindow_Main *pwindow);
    ~CWorkspace();
    bool Init();

    bool Close();
    void Reset();
    bool Open();
    bool Save();
    bool SaveAs();
    bool New();

    bool PersistTo(char *szFLDName);

    // Sets the path to the source files that represents the root of the dvd.
    bool SetDVDRoot(char *szRoot, bool fRecursive);

    int GetNumFiles(int iLayer);
    int GetNumEmptySectors(int iLayer);
    int GetNumUsedSectors(int iLayer);
 //   bool MoveSelectedObjectsToScratch(int nLayer, CWindow_Scratch *pscratch);
   // bool MoveSelectedObjectsFromScratch(int nLayer, CWindow_Scratch *pscratch);

    bool AddObjectsToLayer(CObjList *pol, int nLayer, CObject *pobjDropAt);
    bool RemoveObjectsFromLayer(CObjList *pol, int nLayer);
    bool RemoveObjectFromLayer(CObject *pobj, int nLayer);
    bool QueueSnapspot();
    bool DequeueSnapspot();

    void UpdateWindowCaption();
    void SetModified();
    void ClearModified();

    // Updates the views onto the workspace's data
    void UpdateViews();

    bool CompactLayer(int nLayer);

    bool HandleDrawItem(WPARAM wparam, LPARAM lparam);

    bool CreateBIN();
    bool CreateFST();

    bool RemoveChildObjects(CObj_Folder* pof, CObjList *pol);

//private:

    CFile *m_rgpfileSnapshot[MAX_UNDO_DEPTH];
    bool m_fModified;

    // Our virtual DVD object.
    CDVD *m_pdvd;

    // Adds the files in the specified folder to our DVD.
    bool AddFiles(char *szFolder, bool fRecursive);

    // A pointer to the window that contains the UI elements allowing interaction with the workspace
    CWindow_Main *m_pwindow;

    // if true, then *no* modifications have been made since NEW or OPEN occurred.
    bool m_fCleanSlate;

    bool m_fDefaultXbePresent;
    char m_szName[MAX_PATH];
};

typedef struct
{
    WORD wMajor;
    WORD wMinor;
    WORD wBuild;
    WORD wQFE;
} sVersion;

#define FLD_SIG "XBOXFLDFILE"
#define FLD_SIG_SIZE sizeof(FLD_SIG)

typedef struct
{
    BYTE  szSig[FLD_SIG_SIZE];
    time_t timeSaved;
    sVersion version;
} sFLDHeader;
