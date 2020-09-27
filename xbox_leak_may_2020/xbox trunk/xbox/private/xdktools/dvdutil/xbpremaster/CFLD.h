// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CFLD.h
// Contents:  FLD file wrapper.
// Revisions: 7-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ CLASSES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Class:   CFLD
// Purpose: Encapsulates an FLD file.  Provides accessors to the various entries in the FLD file.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CFLD : public CFile
{
public:

    // ++++ CONSTRUCTION-RELATED FUNCTIONS ++++++++++++++++++++++++++++++++++++

    // CFLD         -- CFLD Constructor
    CFLD(CDlg *pdlg, CStep_Progress *pstep, char *szFilename);


    // ++++ MEMBER-ACCESS FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++

    bool IsWrongVersion() { return m_fWrongVersion; }

    bool ReadNextObjectInfo(DWORD *pdwType, DWORD *pdwLSN, DWORD *pdwBlockSize, char *szName);

    char m_szDVDRoot[MAX_PATH];

private:
    
    bool m_fWrongVersion;
};


