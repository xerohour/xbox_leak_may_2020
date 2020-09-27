// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CDVD.cpp
// Contents:  Declaration of numerous functions for the CDVD class.
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ DEFINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// NUM_SECTORS          -- The number of sectors of the ximage region on each
//                         layer of the disc.
#define NUM_SECTORS 1715632

// NUM_PLACEHOLDERS     -- Total of 16 placeholders (8 per side).
#define NUM_PLACEHOLDERS 16


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ GLOBAL VARIABLES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


char g_szRootFolderName[MAX_PATH];

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::CDVD
// Purpose:   CDVD Constructor
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CDVD::CDVD() : CInitedObject()
{
    HRESULT hrInit = S_OK;

    // Create the DVD's two layers
    m_rgpolLayer[0] = new CObjList;
    m_rgpolLayer[1] = new CObjList;
    if (m_rgpolLayer[0] == NULL || m_rgpolLayer[1] == NULL)
    {
        hrInit = E_OUTOFMEMORY;
        goto done;
    }
    
    // Mark that layer 1's LSN order is opposite of 'normal', where normal is
    // arbitrarily defined to match Layer 0 (and the scratch window).
    m_rgpolLayer[1]->SetInsideOut(true);

done:

    // Set the result of initialization
    SetInited(hrInit);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::~CDVD
// Purpose:   CDVD Destructor
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CDVD::~CDVD()
{
    // Delete our layers
    if (m_rgpolLayer[0])
        delete m_rgpolLayer[0];
    if (m_rgpolLayer[1])
        delete m_rgpolLayer[1];
}

static DWORD gs_rgdwSecurityLSNs[16];

static DWORD gs_rgdwInitSecLSNs[19] =
{
    93748, 171166, 248584, 326002, 403420, 480838, 558256, 635674, 713092, 790510,
    867928, 945345, 1022763, 1100181, 1177599, 1255017, 1332435, 1409853, 1642107
};


static const int gsc_nJitter = 3892;

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::SetInitialPlaceholderLSNs
// Purpose:   
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CDVD::SetInitialPlaceholderLSNs()
{
    int iEntry;
    int cRuns = 0;
    BYTE rgbyPH[20];    // Note: The final entry is just for a bookend
    DWORD rgdwTemp[8];

    // -1 == layer 0 bias, 0 == even layer bias, +1 == layer 1 bias
    // 2 == don't place.
    memset(gs_rgdwSecurityLSNs, 0, 16*4);

    // Start out with everything unplaced.  Drop an 'endpoint' at the very
    // end to catch the last run
    memset(rgbyPH, 0, sizeof(rgbyPH));
    rgbyPH[19] = 2;

    // don't place 3 of them (randomly chosen)
    for (int i = 0; i < 3; i++) {
        do {
            iEntry = rand()%19;
        } while (rgbyPH[iEntry]);
        rgbyPH[iEntry] = 2;
    }

    // Count how many odd runs there are
    int cOddRuns = 0;
    int nRunStart = -1;
    
    for (i = 0; i < 20; i++) {
        if (rgbyPH[i] == 2) {
            if (i > nRunStart)
                cOddRuns += (i - nRunStart-1) % 2;
            nRunStart = i;
        }
    }

    // For the remaining entries, place them based on runs
    nRunStart = 0;
    int iRuns = 0;
    for (i = 0; i < 20; i++) {
        if (rgbyPH[i] == 2) {
            if (i > nRunStart) {
                if ((i-nRunStart)%2){
                    memset(rgbyPH + nRunStart, (iRuns < cOddRuns / 2) ? -1 : 1, i - nRunStart);
                    iRuns++;
                }
            }
            nRunStart = i+1;
        }
    }

    // Walk the list a second time, this time actually setting the placeholder LSNs
    int rgiSec[2] = {0, 8};
    int iLayer = 0;
    BYTE byPrevPH = 2;
    for (i = 0; i < 19; i++) {
        if (byPrevPH != rgbyPH[i]) {
            // new run
            if (rgbyPH[i] == 1) iLayer = 1;
            else iLayer = 0;
            byPrevPH = rgbyPH[i];
        }
        // place it
        if (rgbyPH[i] != 2) {
            int nJitter = (rand() % (gsc_nJitter*2)) - gsc_nJitter;
            gs_rgdwSecurityLSNs[rgiSec[iLayer]++] += gs_rgdwInitSecLSNs[i] + nJitter;
        }
        iLayer = 1 - iLayer;
    }
    // Reverse the order of the layer 1 PHs and convert their temp LSNs to final LSNs
    for (i = 8; i < 16; i++)
        rgdwTemp[i-8] = 2*1715632 - gs_rgdwSecurityLSNs[23-i] - 4096;
    memcpy(&gs_rgdwSecurityLSNs[8], rgdwTemp, 8 * 4);

    assert (rgiSec[0] == 8 && rgiSec[1] == 16);
}


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::Reset
// Purpose:   Reset the DVD to a pristine state.  This means the only objects
//            are the default ones (bookends, placeholders, and volume
//            descriptor)
// Arguments: None.
// Return:    true if successful; false otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::Reset()
{
    if (!IsInited())
        return false;

    StartProfile(PROFILE_RESET);
    // Clean out all old objects in the object lists
    m_onlPH.RemoveAll();
    m_rgpolLayer[0]->Reset();
    m_rgpolLayer[1]->Reset();

    // Create the default objects
    CObject *poStart0  = new CObj_Bookend();
    CObject *poStart1  = new CObj_Bookend();
    CObject *poEnd0    = new CObj_Bookend();
    CObject *poEnd1    = new CObj_Bookend();
    CObj_VolDesc *povd = new CObj_VolDesc();
    if (!poStart0 || !poStart1 || !poEnd0 || !poEnd1 || !povd)
        FatalError(E_OUTOFMEMORY);

    // Insert the starting bookends onto the layers.  These are solely to make
    // traversing the lists easier.
    InsertForced(m_rgpolLayer[0], poStart0, 0);
    InsertForced(m_rgpolLayer[1], poStart1, NUM_SECTORS);

    // Insert the volume descriptor onto layer 0 at LSN 32 (as per gdformat.h).
    InsertForced(m_rgpolLayer[0], povd, 32);
    
    // Add the security placeholders to the list.  Preset their LSNs to known-
    // valid values.
    SetInitialPlaceholderLSNs();

    for (int i = 0; i < NUM_PLACEHOLDERS; i++)
    {
        // Create the security placeholder
        CObject *poSecurity = new CObj_Security();
        if (!poSecurity)
        {
            EndProfile(PROFILE_RESET);
            return false;
        }
        
        // Insert the security object into the 'disc' at the appropriate LSN
        InsertForced(i < NUM_PLACEHOLDERS/2 ? m_rgpolLayer[0] : m_rgpolLayer[1],
                     poSecurity, gs_rgdwSecurityLSNs[i]);
    }

    // Add the ending bookends
    InsertForced(m_rgpolLayer[0], poEnd0, NUM_SECTORS);
    InsertForced(m_rgpolLayer[1], poEnd1, NUM_SECTORS*2);

    // Track that we haven't yet warned the user if there are too many files in
    // their layout.
    m_fWarnedTooManyFiles = false;

    RefreshPlaceholders();
    EndProfile(PROFILE_RESET);
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::PopulateToList
// Purpose:   Populates the specified listview with the objects in the
//            specified layer.
// Arguments: nLayer        -- The layer to populate from.
//            plistview     -- The listview to populate to.
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CDVD::PopulateToList(int nLayer, CListView *plistview)
{
    StartProfile(PROFILE_POPULATETOLIST);
    // Determine which listview the specified layer refers to.
    CObjList *plo = m_rgpolLayer[nLayer];

    // Keep track of which objects, if any, where selected in the listview prior
    // to clearing it, so that we can display them as selected post-insertion.
    // Also stores the current scroll position of the list for later use.
 //   plistview->StoreSelectedObjList();
    
    // Clear out the listview control.
    plistview->Clear();

    // Add the objects in the layer to the listview control
    CObject *poCur = plo->GetInside();
    CObject *pobjNext = plo->GetNextOuter();
    while (poCur)
    {
        // There can be no empty space inside of a group
        if (poCur->m_pog == NULL)
        {
            if (plo->IsInsideOut())
            {
                // Is there empty space between the current object and the next
                // one?  If so, then explicitly add an 'empty' object to the
                // listview so that it's obvious to the user.  We don't add it
                // to the layer though.
                if (pobjNext && (poCur->m_dwLSN - pobjNext->m_dwLSN >
                    pobjNext->m_dwBlockSize) && poCur->GetType() != OBJ_GROUP)
                {
                    CObj_Empty *poe = new CObj_Empty((poCur->m_dwLSN -
                          (pobjNext->m_dwLSN + pobjNext->m_dwBlockSize))*2048);
                    if (!poe)
                        FatalError(E_OUTOFMEMORY);
                    poe->m_dwLSN = pobjNext->m_dwLSN + pobjNext->m_dwBlockSize;
                    plistview->AddObject(poe);
                }
            }
            else
            {
                // Is there empty space between the current object and the next
                // one?  If so, then explicitly add an 'empty' object to the
                // listview so that it's obvious to the user.  We don't add it
                // to the layer though.
                if (pobjNext && (poCur->m_dwLSN + poCur->m_dwBlockSize < 
                                                            pobjNext->m_dwLSN))
                {
                    CObj_Empty *poe = new CObj_Empty((pobjNext->m_dwLSN -
                                  (poCur->m_dwLSN+poCur->m_dwBlockSize))*2048);
                    if (!poe)
                        FatalError(E_OUTOFMEMORY);
                    poe->m_dwLSN = poCur->m_dwLSN + poCur->m_dwBlockSize;
                    plistview->AddObject(poe);
                }
            }
        }

        // Don't add bookends or groups (they are not visible in the listview
        if (poCur->GetType() != OBJ_BE && poCur->GetType() != OBJ_GROUP)
            plistview->AddObject(poCur);
        
        // Step to the next object to be added to the listview
        poCur = pobjNext;
        pobjNext = plo->GetNextOuter();
    }

    plistview->m_fModified = false;

    // We're done with the list of selected objects so clear it out.  Also
    // restore the original scroll position.
//    plistview->ClearSelectedObjList();
    EndProfile(PROFILE_POPULATETOLIST);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::FileExists
// Purpose:   Determine if a file exists on the dvd.  Searched by full pathname
//            Examines both layers to see if the specified file is present.
// Arguments: szFullFileName        -- Full path name of the file to search for
// Return:    'true' if the file exists on the dvd.
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::FileExists(char *szFullFileName)
{
    if (m_rgpolLayer[0]->FindObjectByFullFileName(szFullFileName) || 
        m_rgpolLayer[1]->FindObjectByFullFileName(szFullFileName))
        return true;

    return false;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::GetNumFiles
// Purpose:   Get the number of files and folders on the specified layer
// Arguments: nLayer        -- The layer to get the number of files on.
// Return:    The number of files on the specified layer
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CDVD::GetNumFiles(int nLayer)
{
    // Return the number of files and folders in the layer
    return m_rgpolLayer[nLayer]->GetNumObjects(OBJ_FILE | OBJ_FOLDER);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::GetNumEmptySectors
// Purpose:   Get the number of empty sectors on the specified layer
// Arguments: nLayer        -- The layer to get the number of empty sectors on.
// Return:    The number of empty sectors on the specified layer
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CDVD::GetNumEmptySectors(int nLayer)
{
    // 1716532 sectors available on an empty disk.  Subtract how much is used
    // by the different objects, and we know how much space is left
    return NUM_SECTORS -
           m_rgpolLayer[nLayer]->GetObjectSizes(OBJ_FILE    | OBJ_FOLDER |
                                                OBJ_VOLDESC | OBJ_SEC);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::GetNumUsedSectors
// Purpose:   Get the number of sectors used by files and folders on the layer
// Arguments: nLayer        -- The layer to check.
// Return:    The number of sectors used by files and folders on the layer
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int CDVD::GetNumUsedSectors(int nLayer)
{
    // Return the number of sectors used by files and folders
    return m_rgpolLayer[nLayer]->GetObjectSizes(OBJ_FILE | OBJ_FOLDER);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::RemoveObjectsFromLayer
// Purpose:   Remove a set of objects from the specified layer.
// Arguments: pol           -- The list of objects to remove from the layer
//            nLayer        -- The layer to remove the objects from
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::RemoveObjectsFromLayer(CObjList *pol, int nLayer)
{
    CObject *poCur = pol->GetOutside();
    while (poCur)
    {
        CObject *poNext = poCur->m_poInner;
        m_rgpolLayer[nLayer]->Remove(poCur);
        poCur = poNext;
    }
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::RemoveObjectFromLayer
// Purpose:   Removes a single object from a layer
// Arguments: pobj          -- The object to remove from the layer
//            nLayer        -- The layer to remove the object from.
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::RemoveObjectFromLayer(CObject *pobj, int nLayer)
{
    m_rgpolLayer[nLayer]->Remove(pobj);
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::RemoveChildObjects
// Purpose:   Remove all of the objects which belong to the folder, and add
//            them to the list of objects.
// Arguments: pof           -- Folder to check against.
//            polChildren   -- List to which objects inside 'pof' are added.
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::RemoveChildObjects(CObj_Folder* pof, CObjList *polChildren)
{
    // For each layer...
    for (int i = 0; i < 2; i++)
    {
        // For each file or folder object on both layers, if its parent folder
        // matches the one passed in, then remove it and add it to the list
        CObject *poCur = m_rgpolLayer[i]->GetInside();
        while (poCur)
        {
            CObject *poNext = poCur->m_poOuter;
            if (poCur->m_pof == pof)
            {
                poCur->RemoveFromList();
                polChildren->AddToTail(poCur);
            }
            poCur = poNext;
        }
    }
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::CompactLayer
// Purpose:   Compact the specified layer so that all files are as far outside
//            as possible.  Security placeholders are not moved during this
//            process.
// Arguments: nLayer            -- Layer to compact (0 or 1)
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::CompactLayer(int nLayer)
{
    StartProfile(PROFILE_COMPACTLAYER);
    CObjList olToInsert;
    CObjList *ploLayer = m_rgpolLayer[nLayer];
    
    // Remove all non-placeholder objects and add them to the list of objects
    // to be placed back into the layer.
    CObject *poCur = ploLayer->GetOutside()->m_poInner;
    while (poCur->GetType() != OBJ_BE)
    {
        CObject *poNext = poCur->m_poInner;
        if (poCur->GetType() != OBJ_SEC && poCur->GetType() != OBJ_VOLDESC)
        {
            ploLayer->Remove(poCur);
            if (poCur->m_pog == NULL)
                olToInsert.AddToHead(poCur);
        }
        poCur = poNext;
    }

    // At this point, the layer contains only placeholders and bookends.
    // Start out pointing at the outermost object (the outside bookend).
    CObject *poCompacted = ploLayer->GetOutside();
    CObject *poToInsert = olToInsert.GetOutside();
    while (poToInsert)
    {
        CObject *poInner = poCompacted->m_poInner;
        CObject *poNextInsert = poToInsert->m_poInner;
        
        olToInsert.Remove(poToInsert);

        // If the object is a member of a group, then ignore it and let the
        // group handle it atomically.  When the group itself is added, its
        // contained objects will be added at that point.
        if (poToInsert->m_pog)
            goto next;

        // Check if there's enough room to place the next object in the 'to
        // insert' list.
        while (true)
        {
            DWORD dwEmptySpace;
            if (ploLayer->IsInsideOut())
                dwEmptySpace = poInner->m_dwLSN - (poCompacted->m_dwLSN + poCompacted->m_dwBlockSize);
            else
                dwEmptySpace = poCompacted->m_dwLSN - (poInner->m_dwLSN + poInner->m_dwBlockSize);

            if (dwEmptySpace >= poToInsert->m_dwBlockSize)
                break;

            // Not enough space between the compacted object and the object
            // immediate 'inside' of it.  Step to the next object and
            // try again
            poCompacted = poInner;
            poInner = poCompacted->m_poInner;
            if (poCompacted->GetType() == OBJ_BE)
            {
                // Ran out of room!  We shouldn't ever get here since we know
                // that there was enough space *before* compacting.
                assert(false);
                EndProfile(PROFILE_COMPACTLAYER);
                return false;
            }
        }

        // Insert poToInsert inside poCompacted
        ploLayer->InsertInside(poToInsert, poCompacted);
        if (ploLayer->IsInsideOut())
            poToInsert->m_dwLSN = poCompacted->m_dwLSN + poCompacted->m_dwBlockSize;
        else
            poToInsert->m_dwLSN = poCompacted->m_dwLSN - poToInsert->m_dwBlockSize;

        if (poToInsert->GetType() == OBJ_GROUP)
        {
            DWORD dwPrevLSN = poToInsert->m_dwLSN;
            CObject *poPrevContained = poToInsert;
            CObject *poContained;
            if (ploLayer->IsInsideOut())
                poContained = ((CObj_Group*)poToInsert)->m_gol.Tail();
            else
                poContained = ((CObj_Group*)poToInsert)->m_gol.Head();
            while (poContained)
            {
                poContained->m_dwLSN = dwPrevLSN;
                ploLayer->InsertOutside(poContained, poPrevContained);
                dwPrevLSN += poContained->m_dwBlockSize;
                if (ploLayer->IsInsideOut())
                    poContained = ((CObj_Group*)poToInsert)->m_gol.Prev();
                else
                {
                    poPrevContained = poContained;
                    poContained = ((CObj_Group*)poToInsert)->m_gol.Next();
                }
            }
        }
        poCompacted = poToInsert;

next:
        // Handle the next object in the 'to insert' list
        poToInsert = poNextInsert;
    }
    EndProfile(PROFILE_COMPACTLAYER);
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::RefreshRelationships
// Purpose:   Refreshes the child->parent folder relationship for all file
//            and folder objects on the DVD
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CDVD::RefreshRelationships()
{
    StartProfile(PROFILE_REFRESHRELATIONSHIPS);
    // For each layer...
    for (int i = 0; i < 2; i++)
    {
        // For each file or folder object on the layer, find its parent folder
        // object and store a pointer to it.
        CObjList *ploLayer = m_rgpolLayer[i];
        CObject *poCur = ploLayer->GetInside();
        while (poCur)
        {
            // If it's a file or folder...
            if (poCur->GetType() == OBJ_FILE || poCur->GetType() == OBJ_FOLDER)
            {
                // Find the file or folder that it 'belongs to' (ie is within).
                CObject *poCheck = NULL;
                for (int j = 0; j < 2 && !poCheck; j++)
                {
                    poCheck = m_rgpolLayer[j]->GetInside();
                    while (poCheck)
                    {
                        if (poCheck->GetType() == OBJ_FOLDER)
                        {
                            // It's a folder; is it the right one?
                            if (!lstrcmpi(poCur->m_szFolder, poCheck->m_szFullFileName))
                            {
                                // yep - create the relationship.
                                poCur->m_pof = (CObj_Folder*)poCheck;
                                break;
                            }
                        }
                        poCheck = poCheck->m_poOuter;
                    }
                }
            }
            poCur = poCur->m_poOuter;
        }
    }
    EndProfile(PROFILE_REFRESHRELATIONSHIPS);
}
