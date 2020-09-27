// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CFileHierarchy.h
// Contents:  
// Revisions: 13-Dec-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

typedef enum eCheckState {CHECKSTATE_CHECKED, CHECKSTATE_UNCHECKED, CHECKSTATE_TRISTATE, CHECKSTATE_UNDEFINED};

class CFileObject;


class CFileHierarchy
{
public:
    CFileHierarchy() {};
    ~CFileHierarchy() {};
    BOOL Init() {m_pfiRoot = NULL; return TRUE;}
    BOOL SetRoot(char *szRootDir);
    BOOL Add(CFileObject *pfo);
    BOOL Remove(CFileObject *pfo);
    CFileObject *m_pfiRoot;
    char m_szRootDir[MAX_PATH];
    void RemoveAll();
    BOOL PersistTo(CFile *pfileDest);
    BOOL CreateFrom(CFile *pfileDest, char *szRoot);
    CFileObject *RecurseCreateFrom(CFile *pfileDest);

private:
    CFileObject *RecursePopulate(char *szFolder, WIN32_FIND_DATA *pwfd, int nDepth);
    void RemoveAllChildren(CFileObject *pfoFolder);

    // used for updating progress bar
    int m_cFiles, m_cFolders;
};

