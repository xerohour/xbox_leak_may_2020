// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CFST.h
// Contents:  FST file wrapper.  The FST file format is defined by the AMC tools.
// Revisions: 7-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ CLASSES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Class:   CFST
// Purpose: Encapsulates an FST file.  Provides accessors to the various entries in the FST file.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class CFST : public CFile
{
public:

    // ++++ CONSTRUCTION-RELATED FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // CFST         -- CFST Constructor
    CFST(char *szFilename);

    // ~CFST        -- CFST Destructor
    ~CFST();


    // ++++ MEMBER-ACCESS FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // GetEntry     -- Gets the information about a particular entry in the FST file
    bool GetEntry(int nLayer, int iEntry, char szName[MAX_PATH],
                  int *pnStartPSN, int *pnEndPSN, DWORD *pdwOffset,
                  BOOL fPackage = false, char szPackageDVDRoot[MAX_PATH] = NULL);

    void Dump();

private:

    // m_cEntries   -- The number of entries in the FST file
    int m_cEntries;

    // m_pchStringTable     -- The string table containing the entries' names
    char *m_pchStringTable;

    // m_cbyStringTableSize -- The size of the string table containing the entries' names
    int m_cbyStringTableSize;

    // sTableEntry  -- Contains information about a particular entry in the FST file
    struct sTableEntry
    {
        // m_dwStart    -- LSN that the entry starts on
        DWORD m_dwStart;
        
        // m_dwStop     -- LSN that the entry stops on
        DWORD m_dwStop;

        // m_iDir       -- String table index for the name of the directory for the entry 
        DWORD m_iDir;

        // m_iName      -- String table index for the name of the file for the entry
        DWORD m_iName;

        // m_dwOffset   -- Offset into the entry at which to begin reading data
        DWORD m_dwOffset;
    };

    // m_ptes       -- The list of entrys
    sTableEntry *m_ptes;

    // m_iFirstLayer1Entry  -- Tracks which entry is the first one found on layer 1.
    DWORD m_iFirstLayer1Entry;
};


