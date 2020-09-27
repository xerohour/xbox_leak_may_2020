// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CObject.h
// Contents:  
// Revisions: 14-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#define LSN_UNPLACED -1

#define LSN_AUTOPLACE -2

typedef enum eObjectType {
    OBJ_SEC       = 0x0001,
    OBJ_BE        = 0x0002,
    OBJ_VOLDESC   = 0x0004,
    OBJ_FILE      = 0x0008,
    OBJ_FOLDER    = 0x0010,
    OBJ_GROUP     = 0x0020,
    OBJ_EMPTY     = 0x0040,
    OBJ_ENDOFLIST = 0x0080};

typedef enum eInsert {INSERT_OUTSIDEEDGE = 1, INSERT_INSIDEEDGE};


class CListView;
class CObjList;
class CObj_Group;
class CObj_Folder;

class CObject : public CInitedObject
{
public:
    friend class CObjList;
    friend class CListView;
    friend class CDVD;
    friend class CWindow_Scratch;

    CObject();
    virtual ~CObject() {};
    virtual eObjectType GetType() = NULL;
	virtual bool PersistTo(CFile *pfile);
    void SetGroup(CObj_Group *pog);
    void SetGroupBeforeObject(CObj_Group *pog, CObject *poLoc);
    void SetGroupAfterObject(CObj_Group *pog, CObject *poLoc);
    void RemoveFromList();

    // An object can belong to at most one group at any one time.
    CObj_Group *m_pog;

    DWORD m_dwBlockSize;

//protected:
    // List navigation vars
    CObject *m_poInner;
    CObject *m_poOuter;
    CObjList *m_pol;

    // Geography-view vars
    DWORD m_dwLSN;
    ULARGE_INTEGER m_uliSize;
    char m_szName[MAX_PATH];
    char m_szFolder[MAX_PATH];
    char m_szFullFileName[MAX_PATH];
    bool m_fIgnoreNextUnselect;
    bool m_fWasSelected;

    CObj_Folder *m_pof;
    int m_iFolderDirEntry;
    DWORD m_dwParentFolderId;

    // Used for CheckModifiedFiles
    bool m_fCMF_Present;
    bool m_fCMF_Resized;
};

// PlaceableObject list
class CObjList
{
public:
    CObjList();
    ~CObjList();

    CObject *GetOutside()
    {
        return (m_fInsideOut) ? m_poCur = m_poHead : m_poCur = m_poTail;
    }
    
    CObject *GetInside()
    {
        return (m_fInsideOut) ? m_poCur = m_poTail : m_poCur = m_poHead;
    }
    
    CObject *GetStart()
    {
        return (m_fInsideOut) ? m_poTail : m_poHead;
    }

    CObject *GetNextInner()
    {
        if (m_poCur == NULL) return NULL; else return m_poCur = m_poCur->m_poInner;
    }
    
    CObject *GetNextOuter()
    {
        if (m_poCur == NULL) return NULL; else return m_poCur = m_poCur->m_poOuter;
    }

    CObject *FindObjectByFullFileName(char *szName);
    int GetNumObjects(DWORD dwObjTypes);
    int GetObjectSizes(DWORD dwObjTypes);

    void Reset();
    void Clear();
    void AddToHead(CObject *ppo);
    void AddToTail(CObject *ppo);
    void Remove(CObject *ppo);
    bool InsertBefore(CObject *poToAdd, CObject *poLoc);
    bool InsertAfter(CObject *poToAdd, CObject *poLoc);

	bool PersistTo(CFile *pfile);
	bool CreateFrom(CFile *pfile);
    void SetInsideOut(bool fInsideOut) {m_fInsideOut = fInsideOut; }
    bool IsInsideOut() {return m_fInsideOut;}
    bool InsertInside(CObject *poToAdd, CObject *poLoc)
    {
//        if (m_fInsideOut) return InsertAfter(poToAdd, poLoc);
  //      else return InsertBefore(poToAdd, poLoc);
        return InsertBefore(poToAdd, poLoc);
    }
    bool InsertOutside(CObject *poToAdd, CObject *poLoc)
    {
//        if (m_fInsideOut) return InsertBefore(poToAdd, poLoc);
//        else return InsertAfter(poToAdd, poLoc);
        return InsertAfter(poToAdd, poLoc);
    }
//private:
    bool m_fInsideOut;
    bool m_fModified;
    CObject *m_poHead;
    CObject *m_poTail;
    CObject *m_poCur;
};

class CObj_Security : public CObject
{
public:
    CObj_Security();
    eObjectType GetType() {return OBJ_SEC; }
};


class CObj_Bookend : public CObject
{
public:
    CObj_Bookend();
    eObjectType GetType() {return OBJ_BE; }
};

class CObj_Empty : public CObject
{
public:
    CObj_Empty(DWORD dwSize);
    eObjectType GetType() {return OBJ_EMPTY; }
};

#define VERSION_SIG "XBOX_DVD_LAYOUT_TOOL_SIG"

struct VERSION_INFO 
{
    BYTE rgbySig[32];
    BYTE rgbyLayoutVersion[8];
    BYTE rgbyPremasterVersion[8];
    BYTE rgPadding[2048-32-8-8];    // fill with zeros
};

class CObj_VolDesc : public CObject
{
public:
    CObj_VolDesc();
    eObjectType GetType() {return OBJ_VOLDESC; }
    GDF_VOLUME_DESCRIPTOR m_gdfvd;
    VERSION_INFO m_verinfo;
    DWORD m_dwUdfsOffset;
};


class CObj_File : public CObject
{
public:
    CObj_File(char *szFolder, char *szFilename, DWORD dwSizeHigh, DWORD dwSizeLow);
    eObjectType GetType() {return OBJ_FILE; }
};

// GDF_MAX_DIRECTORY_ENTRY  -- This structure is define in order to make
// handling the directory entries easier.
typedef struct {
    USHORT LeftEntryIndex;
    USHORT RightEntryIndex;
    ULONG FirstSector;
    ULONG FileSize;
    UCHAR FileAttributes;
    UCHAR FileNameLength;
    UCHAR FileName[255];
    USHORT OffsetFromStart;
} GDF_MAX_DIRECTORY_ENTRY;

// folder = file system directory
class CObj_Folder : public CObject
{
public:
    CObj_Folder(char *szFolder, char *szFilename);
    ~CObj_Folder();
    eObjectType GetType() {return OBJ_FOLDER; }
    DWORD m_dwUdfsOffset;
    GDF_MAX_DIRECTORY_ENTRY *m_prgGDFMDE;
    int m_cDirEntries;

    // Used for persistence
    DWORD m_dwFolderId;
};



// GROUPED OBJECT CODE
class CObjNode
{
public:
    CObjNode(CObject *pobj) {m_pobj = pobj; m_ponNext = m_ponPrev = NULL; }
    CObject  *m_pobj;
    CObjNode *m_ponNext;
    CObjNode *m_ponPrev;
};

class CObjNodeList
{
public:
    CObjNodeList()
    {
        m_ponCur = m_ponHead = m_ponTail = NULL;
        m_cItems = 0;
    }
    
    ~CObjNodeList()
    {
        while (m_ponHead)
        {
            CObjNode *ponNext = m_ponHead->m_ponNext;
            delete m_ponHead;
            m_ponHead = ponNext;
        }
    }

    CObjNode *GetObjectNode(CObject *pobj)
    {
        CObjNode *ponCur = m_ponHead;
        while (ponCur)
        {
            if (ponCur->m_pobj == pobj)
                break;
            ponCur = ponCur->m_ponNext;
        }
        return ponCur;
    }

    void Add(CObject *pobj)
    {
        CObjNode *ponNew = new CObjNode(pobj);
        if (!m_ponHead)
            m_ponHead = m_ponTail = ponNew;
        else
        {
            ponNew->m_ponPrev = m_ponTail;
            m_ponTail->m_ponNext = ponNew;
            m_ponTail = ponNew;
        }
        m_cItems++;
    }

    void PopulateArray(CObject *rgo[], int cItems)
    {
        assert (cItems <= NumItems());
        CObjNode *ponCur = m_ponHead;
        int iItem = 0;
        while (ponCur && cItems)
        {
            rgo[iItem++] = ponCur->m_pobj;
            ponCur = ponCur->m_ponNext;
            cItems--;
        }
    }

    void AddBefore(CObject *pobj, CObject *poLoc)
    {
        // Add 'pobj' before poLoc
        CObjNode *ponNew = new CObjNode(pobj);

        // Find poOutside
        CObjNode *pon = m_ponHead;
        while  (pon)
        {
            if (pon->m_pobj == poLoc)
            {
                // Found the object.  Insert pobj before it.
                if (pon->m_ponPrev)
                {
                    pon->m_ponPrev->m_ponNext = ponNew;
                    ponNew->m_ponPrev = pon->m_ponPrev;
                }
                else
                    m_ponHead = ponNew;

                pon->m_ponPrev = ponNew;
                ponNew->m_ponNext = pon;
                return;
            }
            pon = pon->m_ponNext;
        }
        // Shouldn't ever get here
        assert(false);
    }

    void AddAfter(CObject *pobj, CObject *poLoc)
    {
        // Add 'pobj' after 'poLoc'
        CObjNode *ponNew = new CObjNode(pobj);

        // Find poOutside
        CObjNode *pon = m_ponHead;
        while  (pon)
        {
            if (pon->m_pobj == poLoc)
            {
                // Found the object.  Insert pobj before it.
                if (pon->m_ponNext)
                {
                    pon->m_ponNext->m_ponPrev = ponNew;
                    ponNew->m_ponNext = pon->m_ponNext;
                }
                else
                    m_ponTail = ponNew;

                pon->m_ponNext = ponNew;
                ponNew->m_ponPrev = pon;
                return;
            }
            pon = pon->m_ponNext;
        }
        // Shouldn't ever get here
        assert(false);
    }

    void Remove(CObject *pobj)
    {
        // Find the node that contains the specified object
        CObjNode *pon = m_ponHead;
        while (pon)
        {
            CObjNode *ponNext = pon->m_ponNext;
            if (pon->m_pobj == pobj)
            {
                // found it.  remove it
                if (pon->m_ponPrev)
                    pon->m_ponPrev->m_ponNext = pon->m_ponNext;
                else
                    m_ponHead = pon->m_ponNext;
                if (pon->m_ponNext)
                    pon->m_ponNext->m_ponPrev = pon->m_ponPrev;
                else
                    m_ponTail = pon->m_ponPrev;
                delete pon;
                pobj->m_pog = NULL;
                m_cItems--;
                return;
            }
            pon = ponNext;
        }
        // if here, then the object wasn't in the list to begin with.
        assert(false);
    }

    void RemoveAll()
    {
        while (m_ponHead)
        {
            CObjNode *ponNext = m_ponHead->m_ponNext;
            m_ponHead->m_pobj->m_pog = NULL;
            delete m_ponHead;
            m_ponHead = ponNext;
        }
        m_ponTail = NULL;
        m_cItems = 0;
    }

    bool IsMember(CObject *pobj)
    {
        CObjNode *ponCur = m_ponHead;
        while (ponCur)
        {
            if (ponCur->m_pobj == pobj)
                return true;
            ponCur = ponCur->m_ponNext;
        }
        return false;
    }

    bool IsFirstMember(CObject *pobj)
    {
        if (!m_ponHead) return false;
        return (m_ponHead->m_pobj == pobj);
    }

    bool IsLastMember(CObject *pobj)
    {
        if (!m_ponTail) return false;
        return (m_ponTail->m_pobj == pobj);
    }

    CObject *Head() {m_ponCur = m_ponHead; if (m_ponCur) return m_ponCur->m_pobj; else return NULL; }
    CObject *Tail() {m_ponCur = m_ponTail; if (m_ponCur) return m_ponCur->m_pobj; else return NULL; }
    CObject *Next()
    {
        if (!m_ponCur) return NULL;
        m_ponCur = m_ponCur->m_ponNext;
        if (m_ponCur) return m_ponCur->m_pobj; else return NULL;
    }
    CObject *Prev()
    {
        if (!m_ponCur) return NULL;
        m_ponCur = m_ponCur->m_ponPrev;
        if (m_ponCur) return m_ponCur->m_pobj; else return NULL;
    }

    int NumItems() {return m_cItems; }
//private:
    CObjNode *m_ponCur;
    CObjNode *m_ponHead;
    CObjNode *m_ponTail;
    int m_cItems;
};


// group = collection of files.  
class CObj_Group : public CObject
{
public:
    CObj_Group();
    eObjectType GetType() {return OBJ_GROUP; }
    void InsertGroupedObject(CObject *pobj);
    void InsertGroupedObjectBeforeObject(CObject *pobj, CObject *poLoc);
    void InsertGroupedObjectAfterObject(CObject *pobj, CObject *poLoc);
    void RemoveGroupedObject(CObject *pobj);
    void RemoveAllGroupedObjects();

    int GetNumGroupedItems()
    {
        return m_gol.NumItems();
    }
    CObjNodeList m_gol;
    DWORD m_dwUdfsOffset;
    DWORD m_dwPersistId;
};
