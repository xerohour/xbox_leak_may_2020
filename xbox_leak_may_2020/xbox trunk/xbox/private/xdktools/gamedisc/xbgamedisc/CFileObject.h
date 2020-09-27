// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CFileObject.h
// Contents:  
// Revisions: 29-Nov-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


class CFileObject;

class CFileObjectNode
{
    friend class CFileObjectList;
    CFileObjectNode(CFileObject *pfo);
    CFileObject *m_pfo;
    CFileObjectNode *m_pfonPrev, *m_pfonNext;
};

class CFileObjectList
{
public:

    CFileObjectList();
    void Add(CFileObject *pobj);
    void Remove(CFileObject *pobj);
    CFileObject *GetByName(char *szName);
    CFileObject *GetFirst();
    CFileObject *GetCur();
    CFileObject *GetNext();
    ULARGE_INTEGER GetSize() {return m_uliSize;}
    DWORD GetCount() {return m_cObjects; }
private:
    int m_cObjects;
    ULARGE_INTEGER m_uliSize;
    CFileObjectNode *m_pfonCur;
    CFileObjectNode *m_pfonHead;
};

class CFileObject
{
public:
    CFileObject(char *szObjName, WIN32_FIND_DATA *pwfd,
                CChangeQueue *pcq = NULL);
    CFileObject(CFileObject *pfoSource);
    CFileObject(CFile *pfileSrc);

    ~CFileObject();
    void AddChild(CFileObject *pfoChild);
    void RemoveChild(CFileObject *pfoChild);
    CFileObject *GetFirstChild();
    CFileObject *GetNextChild();
    CFileObject *GetChildByName(char *szName);

    BOOL PersistTo(CFile *pfileDest);

    void UpdateSizeTime();
    void LoadTypeInfo();

    // File attribute information (name, size, etc)
    CFileObject *m_pfoParent;
    CFileObject *m_pfoOrigParent;   // This is necessary since the cq obj may have been removed from its parent!
    char m_szName[MAX_PATH];
    char m_szPath[MAX_PATH];
    ULARGE_INTEGER m_uliSize;
    FILETIME m_ftModified;
    CFileObjectList m_lpfoChildren;
    BOOL m_fIsDir;
    DWORD m_dwa;              // File attributes

    // ChangeThread-related information
    CFileObjectNode *m_pfon;
    CChangeQueue *m_pcq;
    CFileObject *m_pfoChangeThread, *m_pfoMainThread;

    
    // Information displayed in the Explorer View
    HTREEITEM m_hti;
    eCheckState m_checkstate;
    int m_nDepth;

    char m_szType[255];     // Type of the file (Application, DLL, etc)
    char m_szSize[30];      // String representation of liSize
    char m_szModified[30];  // String representation of ftModified
    char m_szLocation[15];  // Excluded, Unplaced, Layer 0, or Layer 1
    int  m_iIcon;           // Index into the list of system icons
    BOOL m_fMouseOver;      // Used for highlighting the 'included' checkbox


    // Information displayed in the Layer view
    char m_szFolder[MAX_PATH]; 
    char m_szSizeOnDisk[20]; 
    char m_szSectorRange[30];
    CFileObject *m_pfoInner;
    CFileObject *m_pfoOuter;

    bool m_fInited;
    HIMAGELIST m_himl;
    HICON m_hIcon;

private:
    bool m_fTypeInfoLoaded;

};