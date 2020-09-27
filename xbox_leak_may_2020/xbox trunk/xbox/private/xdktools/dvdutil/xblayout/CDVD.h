// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      cdvd.h
// Contents:  
// Revisions: 15-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


class CDVD : public CInitedObject
{
public:
    CDVD();
    ~CDVD();
    bool Reset();

    bool InsertForced(CObjList *ploLayer, CObject *pobjToInsert, DWORD dwLSN);

    //bool Insert(CObject *pobjToInsert, CObject *pobjInsertAt);
    //bool Insert(CObjList *ploLayer, CObject *pobjToInsert, CObject *pobjInsertAt);
    //bool Insert(CObject *rgpobjToInsert[], CObject *pobjInsertAt);
//    bool Insert(CObjList *ploLayer, CObject *rgpobjToInsert[], CObject *pobjInsertAt);

    bool Insert(CObjList *ploLayer, CObject *poToInsert, CObject *poInsertAt);
    bool Insert(CObject *poToInsert, CObject *poInsertAt);

    bool Insert(CObjList *ploLayer, CObjList *polToInsert, CObject *poInsertAt);

    bool ValidatePlaceholders(bool fWarnUser = true);
    void PopulateToList(int iLayer, CListView *plistview);
    bool FileExists(char *szFullFileName);

	bool PersistTo(CFile *pfile);
    bool CreateFrom(CFile *pfile);

    bool PersistFST(char *szBinDir, char *szBinName, CFile *pfileFST);
    bool PersistBIN(CWorkspace *pws, CFile *pfile);

    int GetNumFiles(int nLayer);
    int GetNumEmptySectors(int nLayer);
    int GetNumUsedSectors(int nLayer);
  //  bool MoveSelectedObjectsToScratch(int nLayer, CWindow_Scratch *pscratch);
//    bool MoveSelectedObjectsFromScratch(int nLayer, CWindow_Scratch *pscratch);
    bool InsertObjectList(CObjList *pol, int nLayer, CObject *pobjDropAt);
    bool InsertAtEnd(CObject *pobjToInsert);

    bool RemoveObjectFromLayer(CObject *pobj, int nLayer);
    bool RemoveObjectsFromLayer(CObjList *pol, int nLayer);

    bool CompactLayer(int nLayer);

    bool RefreshDirectoryEntries(CWorkspace *pws, bool fBuildForEmulation);
    bool RefreshFolderDirectoryEntry(CObj_Folder *pof, bool fBuildForEmulation);
    bool PopulateDirEntries(CObj_Folder *pof, CObject *rgoSorted[], int *piIndex, int iMin, int iMax);

    bool RemoveChildObjects(CObj_Folder* pof, CObjList *pol);

//private:

    void SetInitialPlaceholderLSNs();

    bool CheckValidPlaceholderLSN(CObj_Security *pobjPlaceholder, DWORD dwLSN);

    void RefreshRelationships();

    // list of placeable objects
    CObjList *m_rgpolLayer[2];

    CObj_Folder *m_povdRoot;

    void RefreshPlaceholders(bool fValidate = true);

    CObjNodeList m_onlPH;
    bool m_fWarnedTooManyFiles;
};

extern char g_szRootFolderName[MAX_PATH];

// Taken from AMC's file "MediaBase.h"
struct TFileHeader
{
    char m_szFileType[32]; // FST? Error Map?
    char m_szMediaType[32]; // DVD, CD, etc
};

struct TMediaHeader
{
    DWORD uNumSectorsLayer0;
    DWORD uNumSectorsLayer1;
    BYTE m_ImpUseArea[120];       // total = 128 bytes
};

