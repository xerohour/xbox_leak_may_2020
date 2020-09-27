// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      cdvd_persist.cpp
// Contents:  Persistance-related functionality for the CDVD class
// Revisions: 6-Jul-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"
#include "IFld.h"

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

static char *g_szVirtualDvdHeaderString = "AMC Virtual Media";

extern time_t g_timeLastSaved;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::PersistTo
// Purpose:   
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Save our contents to the specified FLD file
bool CDVD::PersistTo(CFile *pfile)
{
    StartProfile(PROFILE_PERSISTTO);    

    // Write the name of the root folder
    pfile->WriteString(g_szRootFolderName);

    if (m_rgpolLayer[0]->PersistTo(pfile) &&
		m_rgpolLayer[1]->PersistTo(pfile))
    {
        EndProfile(PROFILE_PERSISTTO);    
        return true;
    }
    EndProfile(PROFILE_PERSISTTO);    
	return false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::CreateFrom
// Purpose:   
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::CreateFrom(CFile *pfile)
{
    StartProfile(PROFILE_CREATEFROM);    
    m_onlPH.RemoveAll();

    m_rgpolLayer[0]->Reset();
    m_rgpolLayer[1]->Reset();

    // Get the name of the root folder
    pfile->ReadString(g_szRootFolderName);

    if (!SetCurrentDirectory(g_szRootFolderName))
    {
        char sz[MAX_PATH + 300];
        sprintf(sz, "The FLD file points at a DVD root location (%s) which\n"
                    "is not currently accessible.  Please ensure the path is "
                    "accessible\nand try again.", g_szRootFolderName);
        MessageBox(NULL, sz,  "Invalid DVD Root Location",
                   MB_ICONEXCLAMATION | MB_OK | MB_APPLMODAL);
        EndProfile(PROFILE_CREATEFROM);    
        return false;
    }

    if (m_rgpolLayer[0]->CreateFrom(pfile) && 
		m_rgpolLayer[1]->CreateFrom(pfile))
    {
        EndProfile(PROFILE_CREATEFROM);    
        return true;
    }

    EndProfile(PROFILE_CREATEFROM);    
    return false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::PersistFST
// Purpose:   
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::PersistFST(char *szBinDir, char *szBinName, CFile *pfile)
{
    CObjList *polLayer;
    StartProfile(PROFILE_PERSISTFST);

    // Write out the AMC Header information
    TFileHeader tfh;
    memset(&tfh, 0, sizeof(TFileHeader));
    strcpy(tfh.m_szFileType, g_szVirtualDvdHeaderString);
    strcpy(tfh.m_szMediaType, "Xbox DVD-ROM");
    if (!pfile->WriteData(&tfh, sizeof(TFileHeader)))
        return false;

    TMediaHeader tmh;
    memset(&tmh, 0, sizeof(TMediaHeader));
    tmh.uNumSectorsLayer0 = 1715632;
    tmh.uNumSectorsLayer1 = 1715632;
    if (!pfile->WriteData(&tmh, sizeof(TMediaHeader)))
        return false;

    // Write out the byte order.
    if (!pfile->WriteWORD(0xABCD))
        return false;

    // Write out a timestamp.
    if (!pfile->WriteDWORD(g_timeLastSaved))
        return false;

    // Write out the number of sectors
// Removed because AMC FST file format changed in latest release.
//    if (!pfile->WriteDWORD(1715632*2))
//        return false;

    // Write out the total number of files on both layers
    int cFiles = GetNumFiles(0) + GetNumFiles(1) + 1;
    if (!pfile->WriteDWORD(cFiles))
        return false;

    // Create the entry table
    sTableEntry *rgte = new sTableEntry[cFiles];
    if (!rgte)
        FatalError(E_OUTOFMEMORY);

    // Create the string table that contains the full path names of all the files
    // Allocate the maximum space that could be required (rather than enumerating
    // the files for exact length).  Only the necessary amount will be written to disk.
    char *rgStrings = new char[cFiles*MAX_PATH];
    if (!rgStrings)
        FatalError(E_OUTOFMEMORY);

    int iEntry = 0;
    int iUdfsDir, iUdfsName;
    int iStringLoc = 0;
    bool fUdfsEmitted = false;

    // Populate the entry table and string table with our file information
    for (int nLayer = 0; nLayer <= 1; nLayer++)
    {
        polLayer = m_rgpolLayer[nLayer];

        // offset by 2 in code below is because we don't want to store the drive ("C:\") (emulator doesn't like it).

        CObject *poCur;
        if (nLayer == 0)
            poCur = polLayer->GetInside();
        else
            poCur = polLayer->GetOutside();
        while (poCur)
        {
            if (poCur->GetType() == OBJ_FILE || poCur->GetType() == OBJ_FOLDER || poCur->GetType() == OBJ_VOLDESC)
            {
                sTableEntry *pteCur = &rgte[iEntry];
                pteCur->m_dwStart = poCur->m_dwLSN;
                pteCur->m_dwStop  = poCur->m_dwLSN + poCur->m_dwBlockSize - 1;

                if (poCur->GetType() == OBJ_FOLDER || poCur->GetType() == OBJ_VOLDESC)
                {
                    // For folders, point at udfs.bin and appropriate offset
                    if (!fUdfsEmitted)
                    {
                        // udfs.bin not emitted yet.  do so now.
                        // output the directory
                        iUdfsDir = iStringLoc;
                        strcpy(rgStrings + iStringLoc, szBinDir + 2);
                        iStringLoc += strlen(rgStrings + iStringLoc) + 1;

                        // output the name
                        iUdfsName = iStringLoc;
                        strcpy(rgStrings + iStringLoc, szBinName);
                        iStringLoc += strlen(rgStrings + iStringLoc) + 1;

                        fUdfsEmitted = true;
                    }
                    pteCur->m_iDir  = iUdfsDir;
                    pteCur->m_iName = iUdfsName;
                    if (poCur->GetType() == OBJ_FOLDER)
                        pteCur->m_dwOffset = ((CObj_Folder*)poCur)->m_dwUdfsOffset;
                    else
                        pteCur->m_dwOffset = ((CObj_VolDesc*)poCur)->m_dwUdfsOffset;
                }
                else
                {
                    // Only need to write out folders once; if already written then
                    // don't write again.
                    for (int i = 0; i < iEntry; i++)
                    {
                        if (!lstrcmpi(&rgStrings[rgte[i].m_iDir], poCur->m_szFolder))
                        {
                            // The folder already exists
                            break;
                        }
                    }
                    if (i == iEntry)
                    {
                        // Folder doesn't exist yet
                        pteCur->m_iDir = iStringLoc;
                        strcpy(rgStrings + iStringLoc, poCur->m_szFolder + 2);
                        iStringLoc += strlen(poCur->m_szFolder) + 1;
                    }
                    else
                        pteCur->m_iDir = rgte[i].m_iDir;

                    pteCur->m_dwOffset = 0;

                    pteCur->m_iName   = iStringLoc;
                    strcpy(rgStrings + iStringLoc, poCur->m_szName);
                    iStringLoc += strlen(poCur->m_szName) + 1;
                }
                iEntry++;
            }

            if (nLayer == 0)
                poCur = polLayer->GetNextOuter();
            else
                poCur = polLayer->GetNextInner();
        }
    }
    
    // Write out the full size of the string table.
    if (!pfile->WriteDWORD(iStringLoc))
        return false;

    // Write out the entry table
    if (!pfile->WriteData(rgte, sizeof(sTableEntry)*cFiles))
        return false;

    // Write out the string table
    if (!pfile->WriteData(rgStrings, iStringLoc))
        return false;
    
    EndProfile(PROFILE_PERSISTFST);
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::PersistBIN
// Purpose:   Enumerate the contents of gdfs.bin to the specified file.  This
//            includes the Volume Descriptor object, and the directory entries
//            cooresponding to all folder objects in the layout.  The folder
//            objects themselves already contain blocks of directory entries
//            describing their objects; we just need to output that information
//            and gaurantee that it is laid out in such a fashion that doesn't
//            violate certain rules (ie entries can't cross sector boundaries).
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::PersistBIN(CWorkspace *pws, CFile *pfile)
{
    // The BIN file consists of the directory entries for all of the files in
    // the layout.  Each of the folders in the layout contain a block of directory
    // entry structures -- those contain the information that needs to be
    // emitted here.  We convert each of them from a GDF_MAX_DIRECTORY_ENTRY
    // structure (our info+ structure) to the GDF_DIRECTORY_ENTRY structure
    // that the xbox file system is expecting.
    StartProfile(PROFILE_PERSISTBIN);
    static int rgnPadding[] = {0, 3, 2, 1};

    // We need to pad out sectors when directory entries try to straddle them,
    // so precreate a max-sized padding array that we can just memcpy out of.
    BYTE rgbyPadding[2048];
    memset(rgbyPadding, GDF_DIRECTORY_STREAM_PADDING, 2048);

    CObj_Folder *pof;

    // Keep track of the total number of bytes written to the
    // file -- this will be used to check sector boundaries.
    int cBytesWritten = 0;

    // Refresh the folders' directory entries.
    if (!RefreshDirectoryEntries(pws, true))
    {
        // Failed on the refresh.  User has already been notified
        EndProfile(PROFILE_PERSISTBIN);
        return false;
    }

    // Find each folder on the layout and emit its directory entries.  We also
    // keep track of where in the gdfs.bin file the folder's entries are laid
    // out since the FST file generation will need to know the offset into the BIN.
    for (int nLayer = 0; nLayer <= 1; nLayer++)
    {
        CObjList *polLayer = m_rgpolLayer[nLayer];

        // For each object in the layer...
        CObject *poCur = polLayer->GetInside();
        while (poCur)
        {
            switch(poCur->GetType())
            {
            case OBJ_VOLDESC:
                // The object is the volume descriptor object.
                // Track the location of the volume descriptor in the file
                ((CObj_VolDesc*)poCur)->m_dwUdfsOffset = pfile->GetCurLoc();

                // Emit the GDF_VOLUME_DESCRIPTOR information to the file
                pfile->WriteData(&((CObj_VolDesc*)poCur)->m_gdfvd,
                                 sizeof(GDF_VOLUME_DESCRIPTOR));
                // Note: There is no need to pad the sector here since the
                // size of the GDF_VOLUME_DESCRIPTOR is 2048.

                // Emit the xbLayout version information to the file
                pfile->WriteData(&((CObj_VolDesc*)poCur)->m_verinfo,
                                 sizeof(VERSION_INFO));
                break;

            case OBJ_FOLDER:
                // The object is a folder object.  The folder's directory
                // entries were precalculated in RefreshFolderDirectoryEntry,
                // so we just have to do a little bit of conversion (from our
                // data structure to the GDF_DIRECTORY_ENTRY that the OS is
                // expected), and handle padding as necessary.

                // We know the current object is a folder object, so cast a.
                // pointer to it.
                pof = (CObj_Folder*)poCur;

                // Track the folder's location in the gdfs.bin file.
                pof->m_dwUdfsOffset = pfile->GetCurLoc();

                // Emit the folder's directory entry information.  We need to be
                // sure two things happen:
                //  1. An entry doesn't cross a sector boundary.
                //  2. Each sector is padded with GDF_DIRECTORY_STREAM_PADDING.

                // For each directory entry in the folder...
                for (int i = 0; i < pof->m_cDirEntries; i++)
                {
                    // Store a pointer to our directory entry "info+" structure
                    GDF_MAX_DIRECTORY_ENTRY *pgdfmde = &pof->m_prgGDFMDE[i];

                    // Determine how many bytes we'll need to write out for the
                    // current directory entry.
                    int cBytesToWrite = sizeof(GDF_DIRECTORY_ENTRY) +
                                        pgdfmde->FileNameLength - 1;

                    // We need to be sure that the entry ends on a DWORD-
                    // boundary since the index values are shifted up by 4.
                    int nPadding = rgnPadding[cBytesToWrite % 4];
 
                    // Check to see if the current entry will cross a sector
                    // boundary
                    if (cBytesWritten / 2048 != (cBytesWritten + cBytesToWrite + nPadding) / 2048)
                    {
                        // We'll cross a sector boundary if we try to write out
                        // the current directory entry.  Instead, pad out the
                        // remainder of the current sector so that we write the
                        // current directory entry starting a the next sector.
                        // The index values were already calculated properly in
                        // the RefreshFolderDirectoryEntry funtion, so we don't
                        // have to modify them here.
                        pfile->WriteData(rgbyPadding, 2048 - (cBytesWritten % 2048));
                        cBytesWritten += 2048 - (cBytesWritten % 2048);
                    }

                    // Now that we know we're at a safe place to write out the
                    // directory entry, write out the GDF_DIRECTORY_ENTRY-
                    // specific information from the GDF_MAX_DIRECTORY_ENTRY
                    // structure (the latter structure is designed such that
                    // the GDF_DIRECTORY_ENTRY values occur first, so we can
                    // just do a simple write.
                    pfile->WriteData(pgdfmde, cBytesToWrite);

                    // Pad out to a DWORD boundary
                    if (nPadding)
                    {
                        pfile->WriteData(rgbyPadding, nPadding);
                        cBytesWritten += nPadding;
                    }
                    
                    // Track how many bytes we've written to gdfs.bin so far.
                    cBytesWritten += cBytesToWrite;
                }

                // Pad the remainder of the sector.
                int nExtra = 2048 - (cBytesWritten % 2048);
                pfile->WriteData(rgbyPadding, nExtra);
                cBytesWritten += nExtra;
                break;
            }

            poCur = polLayer->GetNextOuter();
        }
    }
    EndProfile(PROFILE_PERSISTBIN);
   
    return true;
}
