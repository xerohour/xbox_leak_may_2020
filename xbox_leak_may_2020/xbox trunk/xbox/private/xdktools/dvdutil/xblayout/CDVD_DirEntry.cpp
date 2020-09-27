// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      cdvd_direntry.cpp
// Contents:  Directory Entry-related functionality for the CDVD class
// Revisions: 6-Jul-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"
#include <xboxverp.h>

extern DWORD BlockSize(ULONGLONG ullSize);

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Define the upper case table used for file name comparisons.  This matches the
// case table used by the file system.
//
const UCHAR GdfUpperCaseTable[256] = {
    0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
    0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,
    0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
    0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
    0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,
    0x60,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
    0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x7b,0x7c,0x7d,0x7e,0x7f,
    0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,
    0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,
    0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
    0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0xbc,0xbd,0xbe,0xbf,
    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
    0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0xdf,
    0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,
    0xd0,0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xf7,0xd8,0xd9,0xda,0xdb,0xdc,0xdd,0xde,0x3f
};

int GdfCompareFileNames(LPCSTR pszFileName1,  LPCSTR pszFileName2)
{
    LONG n1, n2;
    LPCSTR Limit;
    UCHAR c1, c2;

    //
    // The following code is adapted from RtlCompareString in ntos\rtl\string.c.
    //

    n1 = (LONG)lstrlen(pszFileName1);
    n2 = (LONG)lstrlen(pszFileName2);
    Limit = pszFileName1 + (n1 <= n2 ? n1 : n2);
    while (pszFileName1 < Limit) {
        c1 = *pszFileName1++;
        c2 = *pszFileName2++;
        if (c1 != c2) {
            c1 = GdfUpperCaseTable[c1];
            c2 = GdfUpperCaseTable[c2];
            if (c1 != c2) {
                return (LONG)c1 - (LONG)c2;
            }
        }
    }
    return n1 - n2;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CompareObjectNames
// Purpose:   Comparison function for qsort.
// Arguments: pvElem1       -- The first element to compare.
//            pvElem2       -- The second element to compare.
// Return:    <0 if (elem1<elem2), 0 if (elem1==elem2), >0 if (elem1>elem2)
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int _cdecl CompareObjectNames(const void *pvElem1, const void *pvElem2)
{
    // pvElem1 and pvElem2 are passed in as pointers to array elements.  Do
    // some wacky casting to get the actual object pointers.
    CObject *po1 = ((CObject*)*(DWORD*)pvElem1);
    CObject *po2 = ((CObject*)*(DWORD*)pvElem2);

    // Do a string comparison on the names of the objects.
    return (GdfCompareFileNames(po1->m_szName, po2->m_szName));
}
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::PopulateDirEntries
// Purpose:   Populate pof's list of GDF_MAX_DIRECTORY_ENTRY structures.
// Arguments: pof           -- The folder whose items are to be sorted.
//            rgpoSorted     -- The list of object pointers, already sorted.
//            piIndex       -- Current index of GDF_MAX_DIRECTORY_ENTRY to
//                             populate.
//            iMin, iMax    -- Sorted parameters.
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::PopulateDirEntries(CObj_Folder *pof, CObject *rgpoSorted[],
                              int *piIndex, int iMin, int iMax)
{
    // Check if we've exhausted this branch of the search tree
    if (iMax < iMin)
        return false;

    StartProfile(PROFILE_POPULATEDIRENTRIES);
    // Determine the object midway between the two endpoints.
    int iObject = iMin + (iMax - iMin) / 2;

    // Keep a pointer to the object that we're creating a directory entry for.
    CObject *poToInsert = rgpoSorted[iObject];

    // Keep a pointer to the particular directory entry we're populating.
    GDF_MAX_DIRECTORY_ENTRY *pgdfde = &pof->m_prgGDFMDE[*piIndex];
    
    // Populate the directory entry

    // The first sector member points at the Logical Sector Number of the
    // file-system object (ie relative to the start of ximage0.dat)
    pgdfde->FirstSector = poToInsert->m_dwLSN;
    pgdfde->FileSize = poToInsert->m_dwBlockSize * 2048;
    
    // If the object being 'inserted into' pof's list of direntries is a file,
    // then set its size in the direntry.  If it's a folder, though, its size
    // may not have been calculated yet -- as a result, we can't store it yet.
    // Instead, we store the index of the appropriate direntry in the object
    // (we already store a pointer to poInsert's parent folder object).
    if (poToInsert->GetType() == OBJ_FILE)
    {
        // Note that the file size member can only support files up to 4GB in size.
        // This is a limitation of gdfs (see gdformat.h), and means we don't can
        // ignore m_uliSize.HighPart;
        pgdfde->FileSize = poToInsert->m_uliSize.LowPart;
    }
    else
    {
        poToInsert->m_iFolderDirEntry = *piIndex;
    }

    // Set the file's attributes.  The only attribute that's examined by gdfx
    // is FILE_ATTRIBUTE_DIRECTORY (see gdformat.h).
    if (poToInsert->GetType() == OBJ_FOLDER)
        pgdfde->FileAttributes = FILE_ATTRIBUTE_DIRECTORY;
    else
        pgdfde->FileAttributes = FILE_ATTRIBUTE_NORMAL;

    // Set the length of the file's name
    pgdfde->FileNameLength = strlen(poToInsert->m_szName);

    // Note that the maximum length of a filename is 255 bytes, as compared
    // to Win32's MAX_PATH value of 1024.  If the user tried to insert a file
    // with too long of a name, it should have been caught elsewhere.  However,
    // we assert here "just in case".
    assert(strlen(poToInsert->m_szName) < 256);

    // Copy the name of the object into the directory entry.  We don't want
    // to NULL terminate, so we use memcpy instead of strcpy
    memcpy((char*)&pgdfde->FileName[0], poToInsert->m_szName,
           pgdfde->FileNameLength);

    // The index on the left will point at the 'next' entry that's created.
    pgdfde->LeftEntryIndex  = ++(*piIndex);

    // Recursively handle the entries on the left
    if (!PopulateDirEntries(pof, rgpoSorted, piIndex, iMin, iObject-1))
    {
        // If there weren't any more objects 'before' this one, then mark the
        // left index as zero to indicate that fact to the OS.
        pgdfde->LeftEntryIndex = 0;
    }

    // The index on the right will be the one after all the left children have
    // been handled (they have already updated piindex accordingly, so just 
    // set to whatever value was passed back up the recursion tree).
    pgdfde->RightEntryIndex  = (USHORT)*piIndex;

    // Recursively handle the entries on the right.
    if (!PopulateDirEntries(pof, rgpoSorted, piIndex, iObject + 1, iMax))
    {
        // If there weren't any more objects 'after' this one, then mark the
        // right index as zero to indicate that fact to the OS.
        pgdfde->RightEntryIndex = 0;
    }
    EndProfile(PROFILE_POPULATEDIRENTRIES);
    return true;
}

char g_rgszLayerResizedFolder[500][MAX_PATH];
int  g_iCurResizeLayer;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::RefreshFolderDirectoryEntry
// Purpose:   Refresh the list of directory entries pertaining to the specified
//            folder's contents.
// Arguments: pof       -- The folder whose directory entries are to be
//                         refreshed.
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::RefreshFolderDirectoryEntry(CObj_Folder *pof, bool fBuildForEmulation)
{
    // 1. Determine the list of objects on the DVD that exist in the folder.
    // 2. Sort the list.
    // 3. Generate the list of GDF_DIRECTORY_ENTRY structures cooresponding to
    //    the folder's list of contained objects.

    static int rgnPadding[] = {0, 3, 2, 1};

    // onl      -- The list of objects that exist in the folder
    CObjNodeList onl;

    StartProfile(PROFILE_REFRESHFOLDERDIRECTORYENTRY);

    // We need to track which folder is the root folder, so check if this one is it.
    if (!lstrcmpi(pof->m_szFullFileName, g_szRootFolderName))
        m_povdRoot = pof;

    // For each layer...
    for (int nLayer = 0; nLayer <= 1; nLayer++)
    {
        CObjList *polLayer = m_rgpolLayer[nLayer];

        // Iterate over all objects in the layer.
        // Note: Don't use 'GetInside' or 'GetNextOuter' since the
        // RefreshDirectoryEntries function is already using them, and they
        // aren't reentrant.
        CObject *poCur = polLayer->m_poHead;
        while (poCur)
        {
            // If the object is a file-system object (folder or file), check
            // to see if it resides in the passed-in folder.
            if (poCur != pof && (poCur->GetType() == OBJ_FOLDER || poCur->GetType() == OBJ_FILE))
            {
                // Does the folder or file reside in 'pof'?
                if (!lstrcmpi(poCur->m_szFolder, pof->m_szFullFileName))
                {
                    // Don't add the object if it is in the scratch window
                    if (poCur->m_pol != NULL)
                    {
                        // Add it to the temporary list of objects residing in 'pof'
                        onl.Add(poCur);
                    }
                }
            }

            // Move to the next entry in the current layer
            if (nLayer == 0) 
                poCur = poCur->m_poOuter;
            else
                poCur = poCur->m_poInner;
        }
    }

    // 'onl' now contains the list of objects that reside in 'pof'.
    int cItems = onl.NumItems();

    // Move the list into an array for easier indexing.
    CObject **rgpoSorted = new CObject*[cItems];
    if (!rgpoSorted)
        FatalError(E_OUTOFMEMORY);
    onl.PopulateArray(rgpoSorted, onl.NumItems());

    // Sort the list using quicksort.
    qsort((void*)rgpoSorted, cItems, sizeof(CObject*), &CompareObjectNames);

    // Create the list of GDF_DIRECTORY_ENTRY structures for 'pof'

    // Delete the previous list, if one was previously created.
    if (pof->m_prgGDFMDE)
        delete[] pof->m_prgGDFMDE;
    
    // Allocate space for the list of directory entries
    pof->m_prgGDFMDE = new GDF_MAX_DIRECTORY_ENTRY[cItems];
    if (pof->m_prgGDFMDE == NULL)
        FatalError(E_OUTOFMEMORY);

    // Track how many directory entries are in the folder.
    pof->m_cDirEntries = cItems;

    // Populate pof's GDF_MAX_DIRECTORY_ENTRY structures with everything except
    // the final index values (which can't be calculated until all the other
    // information is known for all of the objects).
    int iObject = 0;
    PopulateDirEntries(pof, rgpoSorted, &iObject, 0, cItems-1);

    // We no longer need the array of objects
    delete[] rgpoSorted;

    // We now calculate the true LeftentryIndex and RightEntryIndex values for the
    // GDF_MAX_DIRECTORY_ENTRY structures.  At this point they simply contain the array
    // index of the appropriate entry.  Instead they need to contain the byte
    // offset (div 4) of the entry from the first entry.

    // first, a pass to determine the offset from the start of the block for each 
    // entry.
    int nOffsetFromStart = 0;
    bool fTooManyFiles = false;
    for (int i = 0; i < cItems; i++)
    {
        // Calculate the size of the entry we're about to add.
        int nEntrySize = sizeof(GDF_DIRECTORY_ENTRY) +
                         pof->m_prgGDFMDE[i].FileNameLength - 1;

        // We need to be sure that the entry ends on a DWORD-boundary since the
        // index values are shifted up by 4.
        nEntrySize += rgnPadding[nEntrySize % 4];

        // A single directory entry can't be bigger than one sector
        assert(nEntrySize <= 2048);

        // If the start and end points are on different sectors, then we need
        // to move the offset forward to the start of the next sector.
        if (nOffsetFromStart / 2048 != (nOffsetFromStart + nEntrySize) / 2048)
        {
            // Yep, we cross a boundary.  Move forward until we're pointing at 
            // the next sector
            nOffsetFromStart += 2048 - (nOffsetFromStart % 2048);
        }

        if (nOffsetFromStart >= 0x40000)
            fTooManyFiles = true;

        // Store where the current entry starts (relative to the start of the
        // folder's block of directory entries).
        pof->m_prgGDFMDE[i].OffsetFromStart = (USHORT)(nOffsetFromStart / 4);

        // Update the offset for the next entry.
        nOffsetFromStart += nEntrySize;
    }

    // Next, perform a second pass to set the actual index values, now that
    // we know the offsets of each entry.
    for (i = 0; i < cItems; i++)
    {
        GDF_MAX_DIRECTORY_ENTRY *pgdfmdeLeft  = &pof->m_prgGDFMDE[pof->m_prgGDFMDE[i].LeftEntryIndex];
        GDF_MAX_DIRECTORY_ENTRY *pgdfmdeRight = &pof->m_prgGDFMDE[pof->m_prgGDFMDE[i].RightEntryIndex];

        // As per gdformat.h, the indices are stored (div 4).
        pof->m_prgGDFMDE[i].LeftEntryIndex  = pgdfmdeLeft->OffsetFromStart;
        pof->m_prgGDFMDE[i].RightEntryIndex = pgdfmdeRight->OffsetFromStart;
    }

    // Also track the new size of the folder's block of directory entries.
    DWORD dwNewBlockSize = BlockSize(pof->m_uliSize.LowPart);

    // If the size of the folder's entries has shrunk, then we simply maintain
    // the original size so that the extra space is padded.  However, if the
    // size of the folder's entries has grown, then we have to do a relayout.
    // In this case, continue on, track the farthest out object on the current
    // layer so that we only relayout once per layer.
    if (dwNewBlockSize > pof->m_dwBlockSize)
    {
        // Size grew.  Track the object
        strcpy(g_rgszLayerResizedFolder[g_iCurResizeLayer++], pof->m_szFullFileName);
        pof->m_dwBlockSize = dwNewBlockSize;
    }
    
    // Always update the size -- the code above is only concerned with when the
    // block size changes.
    pof->m_uliSize.QuadPart = nOffsetFromStart;

    // Warn if there are more than 64k worth of directory entries in this folder
    if (fTooManyFiles && !m_fWarnedTooManyFiles)
    {
        MessageBox(NULL, "WARNING: Your game disc layout contains too many "
                         "files in a single directory.  Your disc image\r\n"
                         "may not function correctly.",
                         "Too Many Files in Directory",
                   MB_ICONWARNING | MB_APPLMODAL | MB_OK);

        // Don't warn on every folder, just the first one.
        m_fWarnedTooManyFiles = true;
    }
    EndProfile(PROFILE_REFRESHFOLDERDIRECTORYENTRY);
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::RefreshDirectoryEntries
// Purpose:   Called after the contents of one or more folders have been
//            modified.  Determines the list of GDF_DIRECTORY_ENTRY structures
//            that coorespond to the folder(s)'s contained files.  Also
//            recalculates the volume descriptor (since the location of the
//            root directory may have changed).
// Arguments: None
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::RefreshDirectoryEntries(CWorkspace *pws, bool fBuildForEmulation)
{
    SYSTEMTIME st;
    FILETIME   ft, ftLocal;
    bool fWarnRelayout = false, fErrRelayout = false;
    StartProfile(PROFILE_REFRESHDIRECTORYENTRIES);

start:

    // We'll want to track which object is the volume descriptor so that we
    // can update it accordingly afterwards.
    CObj_VolDesc *povd = NULL;

    m_povdRoot = NULL;
    strcpy(g_rgszLayerResizedFolder[0], "");
    strcpy(g_rgszLayerResizedFolder[1], "");

    g_iCurResizeLayer = 0;

    // Iterate over each folder on the DVD and refresh its directory entry list
    for (int nLayer = 0; nLayer <= 1; nLayer++)
    {
        // Start at the inside of the disc, and work outwards.
        CObject *poCur = m_rgpolLayer[nLayer]->GetInside();
        while (poCur)
        {
            // If the object is a folder, then refresh it's directory entries
            if (poCur->GetType() == OBJ_FOLDER)
                if (!RefreshFolderDirectoryEntry((CObj_Folder*)poCur, fBuildForEmulation))
                {
                    EndProfile(PROFILE_REFRESHDIRECTORYENTRIES);
                    return false;
                }
                
            // Track where the volume descriptor is; we'll need to populate it
            // once we're done (when we know where the root folder is)
            if (poCur->GetType() == OBJ_VOLDESC)
                povd = (CObj_VolDesc*)poCur;

            // Get the next outermost object on the disc.
            poCur = m_rgpolLayer[nLayer]->GetNextOuter();
        }
    }

    // Iterate over each folder on the DVD again, this time set the actual size
    // of the entries.  This is necessary as a second pass because we didn't know
    // the size of any folders that were added if they were handled later on in
    // the loop above.  Now, however, we know how big each folder is, so we
    // can set the size in any reference to it in another folder's direntry.
    for (nLayer = 0; nLayer <= 1; nLayer++)
    {
        // For each layer...
        CObjList *polLayer = m_rgpolLayer[nLayer];

        // Start at the inside of the disc, and work outwards.
        CObject *poCur = polLayer->GetInside();
        while (poCur)
        {
            // If the object is a folder, then store its size in it's parent
            // folder's direntry (the info for which we stored during the first
            // pass earlier).
            if (poCur->GetType() == OBJ_FOLDER && poCur->m_pof)
            {
                assert(poCur->m_iFolderDirEntry != -1);
                GDF_MAX_DIRECTORY_ENTRY *pgdfde = &poCur->m_pof->m_prgGDFMDE[poCur->m_iFolderDirEntry];
                assert(pgdfde);

                pgdfde->FileSize = BlockSize(poCur->m_uliSize.LowPart) * 2048;
            }

            // Get the next outermost object on the disc.
            poCur = polLayer->GetNextOuter();
        }
    }

    bool fRepeat = false;
    
    for (int i = 0; i < g_iCurResizeLayer; i++)
    {
        CObjList *ploLayer;
        CObj_Folder *pof = (CObj_Folder*)m_rgpolLayer[0]->FindObjectByFullFileName(g_rgszLayerResizedFolder[i]);
        if (pof)
            ploLayer = m_rgpolLayer[0];
        else
        {
            pof = (CObj_Folder*)m_rgpolLayer[1]->FindObjectByFullFileName(g_rgszLayerResizedFolder[i]);
            ploLayer = m_rgpolLayer[1];
        }
        assert(pof);

        // Now that we know how big all of the folders' directories are, check to 
        // see if any of them grew bigger than they were before.  We have already
        // stored pointers to the farthest out changed-size folder on each layer;
        // remove and reinsert them.  This will cause the desired relayout.
        if (pof != NULL)
        {
            // We resized at least one folder on the layer.  pof points at
            // the farthest out resized folder -- Remove it and reinsert it
            // to force reinsertion of it and everything above it.

            // First back everything up -- if the new-sized folder can't be
            // inserted back into the layer, then we want to restore everything
            // (since the insertion would have cancelled out part way through,
            // leaving everything in an unknown state), and then just drop the
            // new-sized folder into the scratch window.
            pws->QueueSnapspot();

            // We'll want to insert the folder back at it's original position,
            // so track that now.
            CObject *poInsertAt = pof->m_poInner;

            // Remove the folder from its list in preparation for re-insertion
            pof->RemoveFromList();

            // Reinsert the folder back into the list at its original position.
            if (!Insert(ploLayer, pof, poInsertAt))
            {
                // Failed to reinsert it!  Move the folder object to the
                // scratch window.

                // First, restore the world to the state before the insert.
                pws->DequeueSnapspot();

                // We need to 're-find' the folder since the Dequeue created
                // a whole new set of objects (thus invalidating our old pof)
                pof = (CObj_Folder*)m_rgpolLayer[0]->FindObjectByFullFileName(g_rgszLayerResizedFolder[i]);
                if (!pof)
                    pof = (CObj_Folder*)m_rgpolLayer[1]->FindObjectByFullFileName(g_rgszLayerResizedFolder[i]);
        
                assert(pof);
                
                // We need to re-remove the folder from the list (since the
                // dequeue placed it back in there)
                pof->RemoveFromList();

                // If this is the initial directory refresh then insert it into
                // layer 1; otherwise, into the scratch
                if (!fBuildForEmulation)
                {
                    if (!Insert(m_rgpolLayer[1], pof, NULL))
                        pws->m_pwindow->m_pscratch->AddObject(pof);
                }
                else
                {
                    // Add the folder to the scratch window
                    pws->m_pwindow->m_pscratch->AddObject(pof);

                    // Mark that we're going to need to warn the user of this
                    // situation.
                    fErrRelayout = true;
                }
            }
            else
            {
                // Succeeded on reinserting.  Track that we should warn the user
                // when we're done.
                fWarnRelayout = true;
            }
            
            // We need to update ALL folders'  direntries since their files may
            // have changed LSN and/or FileSize
            fRepeat = true;
        }
    }
    if (fRepeat)
        goto start;

    if (fBuildForEmulation)
    {
        if (fErrRelayout)
        {
            // Inform the user of the error.
            MessageBoxResource(pws->m_pwindow->m_hwnd, IDS_ERR_DIRGROW2_TEXT, IDS_ERR_DIRGROW2_CAPTION, MB_ICONEXCLAMATION | MB_APPLMODAL | MB_OK);
            pws->SetModified();
            pws->UpdateWindowCaption();
        }
        else if (fWarnRelayout)
        {
            MessageBoxResource(pws->m_pwindow->m_hwnd, IDS_ERR_DIRGROW_TEXT, IDS_ERR_DIRGROW_CAPTION, MB_ICONWARNING | MB_APPLMODAL | MB_OK);
            pws->SetModified();
            pws->UpdateWindowCaption();
        }
    }

    CompactLayer(0);
    CompactLayer(1);
    pws->UpdateViews();

    // Refresh the volume descriptor
    
    // A volume descriptor and root directory must have been found.
    assert(povd);
    assert(m_povdRoot);

    // Create the volume descriptor time stamp
    GetSystemTime(&st);
    
    // Convert the system time to file time, and then the file time to UTC file
    // time (as required per gdformat.h).
    SystemTimeToFileTime(&st, &ft);
    LocalFileTimeToFileTime(&ft, &ftLocal);

    // Update the contents of the volume descriptor.

    // Write out the head and tail signatures
    memcpy(povd->m_gdfvd.HeadSignature, GDF_VOLUME_DESCRIPTOR_SIGNATURE,
           GDF_VOLUME_DESCRIPTOR_SIGNATURE_LENGTH);
    memcpy(povd->m_gdfvd.TailSignature, GDF_VOLUME_DESCRIPTOR_SIGNATURE,
           GDF_VOLUME_DESCRIPTOR_SIGNATURE_LENGTH);

    // Write out the Logical Sector Number of the root directory.
    povd->m_gdfvd.RootDirectoryFirstSector = m_povdRoot->m_dwLSN;

    // Write out the size of the root directory's list of directory entries.
    povd->m_gdfvd.RootDirectoryFileSize = m_povdRoot->m_uliSize.LowPart;

    // Write out the volume time stamp.
    memcpy(&povd->m_gdfvd.TimeStamp.QuadPart, &ftLocal, sizeof(ftLocal));

    // The remaining bytes of the sector must be filled with zeroes.
    memset(&povd->m_gdfvd.Reserved, 0, sizeof(povd->m_gdfvd.Reserved));

    // Add version stamp information to the sector following the volume desc.
    memset(&povd->m_verinfo, 0, sizeof(povd->m_verinfo));

    // Add the signature
    strcpy((char*)povd->m_verinfo.rgbySig, VERSION_SIG);
    
    // Set the xblayout version stamp
    int nMajor, nMinor, nBuild, nQFE;
    sscanf(VER_PRODUCTVERSION_STR, "%d.%d.%d.%d", &nMajor, &nMinor, &nBuild,
                                                  &nQFE);
    *(WORD*)(&povd->m_verinfo.rgbyLayoutVersion[0]) = (WORD)nMajor;
    *(WORD*)(&povd->m_verinfo.rgbyLayoutVersion[2]) = (WORD)nMinor;
    *(WORD*)(&povd->m_verinfo.rgbyLayoutVersion[4]) = (WORD)nBuild;
    *(WORD*)(&povd->m_verinfo.rgbyLayoutVersion[6]) = (WORD)nQFE;

    // xbPremaster will handle storing it's version stamp...
    EndProfile(PROFILE_REFRESHDIRECTORYENTRIES);
    return true;
}
