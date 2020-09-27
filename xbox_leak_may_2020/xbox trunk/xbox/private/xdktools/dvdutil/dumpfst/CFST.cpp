// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CFST.cpp
// Contents:  FST file wrapper.  The FST file format is defined by the AMC tools.
// Revisions: 7-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"

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

#define NUM_XDATA_SECTORS 1715632

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFST::CFST
// Purpose:   Opens the specified FST file and reads the header information from it.
// Arguments: szFilename    -- Name of the fst file to open.
// Return:    
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CFST::CFST(char *szFilename) : CFile(szFilename)
{
    WORD  wByteOrder;
    DWORD dwTimestamp;
    int   cSectors;
    
    // Check to see if the file was successfully opened
    if (!IsInited())
        return;

    // Read the various necessary values out of the FST file's header block

    // The Byte order of the FST file can be big or little endian.  We only support one method
    // here.  If the FST file format changes, then we need to handle that here.
    if (Read((BYTE*)&wByteOrder, 2) != S_OK)
        return;

    assert (wByteOrder != 0xCDAB);

    if (wByteOrder != 0xABCD)
    {
        // This is a new format FST file (AMC changed their format)

        // Read in the AMC Header information
        TFileHeader tfh;
        TMediaHeader tmh;

        if (Read(((BYTE*)&tfh) + 2, sizeof(tfh) - 2) != S_OK)
            return;
        
        if (Read((BYTE*)&tmh, sizeof(tmh)) != S_OK)
            return;

        // Read the byte order
        if (Read((BYTE*)&wByteOrder, 2) != S_OK)
            return;

        assert (wByteOrder == 0xABCD);
    }

    // The timestamp is ignored here
    if (Read((BYTE*)&dwTimestamp, 4) != S_OK)
        return;

    // The number of sectors is also ignored (we know it to be a fixed size)
    if (Read((BYTE*)&cSectors, 4) != S_OK)
        return;

    if (cSectors != 1715632*2)
    {
        // This is a new format FST file (AMC changed their format to remove
        // the sector count).  In that case, we just read the number of entries
        m_cEntries = cSectors;

        // Force the number of sectors to a known value.  This will eventually
        // change when different sectors counts are possible
        cSectors = 1715632*2;
    }
    else
    {
        // Old format FST file
        // Read the number of files referenced in this FST file.
        if (Read((BYTE*)&m_cEntries, 4) != S_OK)
            return;
    }

    // Read the full size of the string table.  The string table contains all of the filenames.
    if (Read((BYTE*)&m_cbyStringTableSize, 4) != S_OK)
        return;
    
    // Allocate space for our string and entry tables
    m_ptes = new sTableEntry[m_cEntries];
    m_pchStringTable = new char[m_cbyStringTableSize];

    // Read in the entire entry and string tables
    if (Read((BYTE*)m_ptes, m_cEntries * sizeof(sTableEntry)) != S_OK)
        return;
    if (Read((BYTE*)m_pchStringTable, m_cbyStringTableSize) != S_OK)
        return;

    // Determine the first entry on layer 1
    for (int i = 0; i < m_cEntries; i++)
    {
        if (m_ptes[i].m_dwStart >= NUM_XDATA_SECTORS)
        {
            m_iFirstLayer1Entry = i;
            break;
        }
    }
    if (i == m_cEntries)
    {
        // No entries on layer 1!
        m_iFirstLayer1Entry = m_cEntries;
    }

    SetInited(S_OK);
    return;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFST::~CFST
// Purpose:   CFST destructor.  Clean up after ourselves.
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CFST::~CFST()
{
    // Delete our string and entry tables
    if (m_pchStringTable)
        delete[] m_pchStringTable;

    if (m_ptes)
        delete[] m_ptes; 
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFST::GetEntry
// Purpose:   
// Arguments: nLayer        -- Layer of the entry (0 or 1)
//            iEntry        -- Index of the entry to read (0 == first)
//            szName        -- Will be filled with the full path\filename of the entry
//            pnStartPSN    -- Will be filled with the start PSN of the entry
//            pnEndPSN      -- Will be filled with the end PSN of the entry
//            pdwOffset     -- Will be filled with the offset into the source file.
// Return:    'true' if successfully obtained.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CFST::GetEntry(int nLayer, int iEntry, char szName[MAX_PATH],
                    int *pnStartPSN, int *pnEndPSN, DWORD *pdwOffset,
                    BOOL fPackage, char szPackageDVDRoot[MAX_PATH])
{
    // If this is layer 1 and there are no entries in layer 1, return the requested entry doesn't
    // exist
    if (nLayer == 1 && m_iFirstLayer1Entry == (DWORD)m_cEntries)
        return false;

    // Offset entries in layer 1 to point at the first entry in the table
    if (nLayer == 1)
        iEntry += m_iFirstLayer1Entry;

    // If requested entry > than # of entries in the FST file, then return that the entry doesn't
    // exist
    if ((nLayer == 0 && iEntry >= (int)m_iFirstLayer1Entry) ||
        (nLayer == 1 && iEntry >= m_cEntries))
        return false;

    // Get a pointer to the requested entry in our entry table
    sTableEntry *pst = &m_ptes[iEntry];

    // Fill in the caller-requested information
    if (fPackage)
    {
        // Need to replace the root in the FST file with the temporary root
        // that we extracted all of the files to.
        char *szRelDir = &m_pchStringTable[pst->m_iDir] +
                         strlen(szPackageDVDRoot) - 1;

        // We know this FST exists in <temproot>\_amc\main.fst, so use that.
        strcpy(szName, m_szFilename);

        // Replace "\_amc\main.fst" with the relative directory & filename
        sprintf(szName + strlen(szName) - 13, "%s%s%s",szRelDir,
                (*szRelDir == '\0') ? "" :  "\\",
                &m_pchStringTable[pst->m_iName]);
    }
    else
    {
        sprintf(szName, "%c:%s/%s", m_szFilename[0], &m_pchStringTable[pst->m_iDir],
                &m_pchStringTable[pst->m_iName]);
    }
    *pnStartPSN = pst->m_dwStart;
    *pnEndPSN   = pst->m_dwStop;
    *pdwOffset  = pst->m_dwOffset;

    // Offset the layer one PSNs to be base-zero.
    if (nLayer == 1)
    {
        *pnStartPSN -= NUM_XDATA_SECTORS;
        *pnEndPSN -= NUM_XDATA_SECTORS;
    }

    // Return that the returned values are valid.
    return true;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CFST::Dump
// Purpose:   Dumps contents of FST object to stdout
// Arguments: None
// Return:    None
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CFST::Dump()
{
    int iLayer = 0;
    printf ("Layer 0:\n");
    printf ("File Name                                   LSN Start     LSN End  File Offset\n");
    printf ("------------------------------------------------------------------------------\n");
    for (int iEntry = 0; iEntry < m_cEntries; iEntry++)
    {
        sTableEntry *pst = &m_ptes[iEntry];
        char szName[MAX_PATH];

        sprintf(szName, "%s/%s", &m_pchStringTable[pst->m_iDir],
                &m_pchStringTable[pst->m_iName]);
       
        printf("%-40s   0x%08x  0x%08x  (0x%08x)\n", szName, pst->m_dwStart, pst->m_dwStop, pst->m_dwOffset);
        if (iLayer == 0 && pst->m_dwStart >= 1715632)
        {
            printf("\nLayer 1:\n");
            printf ("File Name                                   LSN Start     LSN End  File Offset\n");
            printf ("------------------------------------------------------------------------------\n");
            iLayer = 1;
        }
    }

    if (iLayer == 0)
    {
        printf("\nLayer 1:\n\n");
        printf ("File Name                                   LSN Start     LSN End  File Offset\n");
        printf ("------------------------------------------------------------------------------\n");
    }
}