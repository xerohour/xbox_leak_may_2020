// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      cdvd_insert.cpp
// Contents:  
// Revisions: 6-Jul-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::InsertObjectList
// Purpose:   
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::InsertObjectList(CObjList *pol, int nLayer, CObject *pobjDropAt)
{
    return Insert(m_rgpolLayer[nLayer], pol, pobjDropAt);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::InsertForced
// Purpose:   
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::InsertForced(CObjList *ploLayer, CObject *poToInsert, DWORD dwLSN)
{
    // This function is only called during initialization; the LSNs are gauranteed to be in the
    // proper order, so we can simply just Add the object as we go
    ploLayer->AddToTail(poToInsert);
    poToInsert->m_dwLSN = dwLSN;
    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::InsertAtEnd
// Purpose:   
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::InsertAtEnd(CObject *poToInsert)
{
    // This function is only called during initialization; the LSNs are gauranteed to be in the
    // proper order, so we can simply just Add the object as we go
    // First try layer 0, if that fails, try layer 1
    if (Insert(m_rgpolLayer[0], poToInsert, NULL))
        return true;
    return Insert(m_rgpolLayer[1], poToInsert, NULL);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::Insert
// Purpose:   
// Arguments: 
// Return:    true if successfully inserted; false otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::Insert(CObject *poToInsert, CObject *poInsertAt)
{
    // First try layer 0, if that fails, try layer 1
    return Insert(m_rgpolLayer[0], poToInsert, poInsertAt) ||
           Insert(m_rgpolLayer[1], poToInsert, poInsertAt);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::Insert
// Purpose:   
// Arguments: 
// Return:    true if successfully inserted; false otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::Insert(CObjList *ploLayer, CObject *poToInsert, CObject *poInsertAt)
{
    CObjList ol;
    
    // Create a temp list for the main Insert function.
    ol.AddToTail(poToInsert);

    return Insert(ploLayer, &ol, poInsertAt);
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::Insert
// Purpose:   
// Arguments: 
// Return:    true if successfully inserted; false otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::Insert(CObjList *ploLayer, CObjList *polToInsert, CObject *poInsertAt)
{
    // We do not have to do this in an 'undo-able' fashion since the originator of this insertion
    // snapshot-ed the workspace before calling in here.  If we determine that the insertion
    // can't be done, then we just return false, and the caller handles restoring the pre-insertion
    // state.
    StartProfile(PROFILE_INSERT);

    ploLayer->m_fModified = true;

    // Check if caller specified an outside insertion
    if (poInsertAt == (CObject*)INSERT_OUTSIDEEDGE)
    {
        poInsertAt = ploLayer->GetOutside();
    }
    if (poInsertAt == (CObject*)INSERT_INSIDEEDGE)
    {
        poInsertAt = ploLayer->GetInside()->m_poOuter;
    }
    else if (poInsertAt == NULL)
    {
        // If the caller didn't specify an insertion point, then we simply insert it after the
        // farthest-inside object already in the list.

        // Start at the inside of the disc and move outward - the first time we hit a non-SEC
        // object, we've found our insertion point.
        poInsertAt = ploLayer->GetInside();

        // Skip the initial bookend
        poInsertAt = ploLayer->GetNextOuter();

        while (poInsertAt->GetType() == OBJ_SEC || poInsertAt->GetType() == OBJ_VOLDESC)
            poInsertAt = ploLayer->GetNextOuter();
    }
    else if (poInsertAt->m_pog)
    {
        // Tried dropping onto a group.  The only time this is allowed is when
        // it's the first item in the group (in which case, the user was actually
        // trying to insert before the group
        poInsertAt = poInsertAt->m_poInner;
    }
 
    CObj_Group *pogInsertion = poInsertAt->m_pog;
    int nDistMoveIn, nDistMoveOut;
    DWORD dwEmptySectors;
    int nLayer = (ploLayer == m_rgpolLayer[0]) ? 0 : 1;

    // We don't allow inserting into a group
    if (poInsertAt->m_pog != NULL)
    {
        EndProfile(PROFILE_INSERT);
        return false;
    }

    // Add all of the non-placeholder objects "inside of" the insertion point
    // to the list of objects to insert. Don't add members of groups -- the
    // group object will be handled as the sum of the contained objects.
    CObject *poCur = poInsertAt->m_poInner;
    while (poCur->GetType() != OBJ_BE)
    {
        CObject *poNext = poCur->m_poInner;
        if (poCur->GetType() != OBJ_SEC && poCur->GetType() != OBJ_VOLDESC)
        {
            ploLayer->Remove(poCur);
            if (poCur->m_pog == NULL)
                polToInsert->AddToHead(poCur);
        }
        poCur = poNext;
    }

    // NOTE: In all of this code, an object 'inside' of another object means closer to the inside
    // of the DVD.  When refering to 'inside a group', it will be explicitly stated.

    // Iterate over each object in the array of objects to insert
    CObject *poToInsert = polToInsert->GetOutside();
    while (poToInsert)
    {
        CObject *poInner = poInsertAt->m_poInner;

        // If the object is a member of a group, then ignore it and let the
        // group handle it atomically.  When the group itself is added, its
        // contained objects will be added at that point.
        while (poToInsert->m_pog)
        {
            poToInsert = polToInsert->GetNextInner();
        }

        // Move the insertion point inward until it's pointing at empty space
        while (true)
        {
            // Calculate how many empty sectors are between the current insertion point
            // and the object immediately inside of it (ie closer to the inside of the dvd).
            if (ploLayer->IsInsideOut())
                dwEmptySectors = poInner->m_dwLSN - (poInsertAt->m_dwLSN + poInsertAt->m_dwBlockSize);
            else
                dwEmptySectors = poInsertAt->m_dwLSN - (poInner->m_dwLSN + poInner->m_dwBlockSize);
            if (dwEmptySectors > 0)
                break;

            poInsertAt = poInsertAt->m_poInner;
            poInner    = poInsertAt->m_poInner;
            if (poInsertAt->GetType() == OBJ_BE)
            {
                // Hit the end of the list (inside of dvd) - no more room!.
                EndProfile(PROFILE_INSERT);
                return false;
            }
        }

        // At this point, m_poInsertAt points at a block of empty space.  Is it *enough* empty
        // space to hold the to-insert object?
        if (poToInsert->m_dwBlockSize <= dwEmptySectors)
        {
            // There's enough space here.  Place the object at the current insertion point.
            polToInsert->Remove(poToInsert);
            ploLayer->InsertInside(poToInsert, poInsertAt);
            if (nLayer == 0)
                poToInsert->m_dwLSN = poInsertAt->m_dwLSN - poToInsert->m_dwBlockSize;
            else
                poToInsert->m_dwLSN = poInsertAt->m_dwLSN + poInsertAt->m_dwBlockSize;

            // Continue with the next object in the 'to insert' list.
            poInsertAt = poToInsert;
            goto next;
        }

        // If the object inside of the current object is a bookend, then there's not enough
        // space for the object!
        if (poInner->GetType() == OBJ_BE)
        {
            // Hit the end of the list.  No need to undo anything.  The caller
            // will handle trying to insert on a different layer (or the scratch
            // window) or warn the user as appropriate
            EndProfile(PROFILE_INSERT);
            return false;
        }

        // Check if there's enough empty space combined between the empty space above poInsertAt
        // and the empty space (if any) between the placeholder above that and the next placeholder.
        // If not, then we can't fit it here, so move on to the next placeholder and try again.
        if (ploLayer->IsInsideOut())
            dwEmptySectors += poInner->m_poInner->m_dwLSN - (poInner->m_dwLSN + poInner->m_dwBlockSize);
        else
            dwEmptySectors += poInner->m_dwLSN - (poInner->m_poInner->m_dwLSN + poInner->m_poInner->m_dwBlockSize);
        if (dwEmptySectors < poToInsert->m_dwBlockSize)
        {
            // Not enough empty space.
            poInsertAt = poInner;

            // *don't* get the next 'to insert' object; repeat the loop with the same one.
            continue;
        }

        // Can't move voldesc, and we're not allowing anything above it. Thus,
        // if at this point the 'inner' object is a volume descriptor, then we
        // can't insert the next object.
        if (poInner->GetType() == OBJ_VOLDESC)
        {
            EndProfile(PROFILE_INSERT);
            return false;
        }

        // Can the security placeholder above the empty space at the current insertion point move
        // down to the insertion point (ie is that a valid location?).  The placeholder itself
        // is passed as well since we need to 'remove' it from the list of placeholders when checking
        // for validity.
        if (CheckValidPlaceholderLSN((CObj_Security*)poInner, poInsertAt->m_dwLSN - poInner->m_dwBlockSize))
            nDistMoveOut = abs(poInsertAt->m_dwLSN - (poInner->m_dwLSN + poInner->m_dwBlockSize));
        else
            nDistMoveOut = INT_MAX;

        // Can the security placeholder be moved up?  Two checks:  (1) there are no other placeholders
        // blocking this placeholder from moving up enough, (2) the position is a valid placeholder
        // position.
        if (!CheckValidPlaceholderLSN((CObj_Security*)poInner, poInsertAt->m_dwLSN - poToInsert->m_dwBlockSize - poInner->m_dwBlockSize))
            nDistMoveIn = INT_MAX;
        else
            nDistMoveIn = abs(poInner->m_dwLSN - (poInsertAt->m_dwLSN - poToInsert->m_dwBlockSize - poInner->m_dwBlockSize));

        // Move the placeholder the minimum valid distance possible
        if (nDistMoveOut == nDistMoveIn && nDistMoveOut == INT_MAX)
        {
            // Can't insert the object at the current position.  Move the current insertion pointer
            // to before the placeholder and try again
            poInsertAt = poInner;
            continue;
        }
        else if (nDistMoveOut < nDistMoveIn)
        {
            // insert the object before the placeholder
            polToInsert->Remove(poToInsert);
            ploLayer->InsertInside(poToInsert, poInner);

            // Set the placeholder's and object's LSNs
            if (nLayer == 0)
            {
                poInner->m_dwLSN    = poInsertAt->m_dwLSN - poInner->m_dwBlockSize;
                poToInsert->m_dwLSN = poInner->m_dwLSN - poToInsert->m_dwBlockSize;
            }
            else
            {
                poInner->m_dwLSN    = poInsertAt->m_dwLSN + poInsertAt->m_dwBlockSize;
                poToInsert->m_dwLSN = poInner->m_dwLSN + poInner->m_dwBlockSize;
            }
            
            // Set current insertion point to before the inserted object
            poInsertAt = poToInsert;
        }
        else
        {
            // Move the placeholder inward

            // Insert the object at the current insertion point
            polToInsert->Remove(poToInsert);
            ploLayer->InsertInside(poToInsert, poInsertAt);

            // Set the placeholder's and object's LSNs
            if (nLayer == 0)
            {
                poInner->m_dwLSN    = poInsertAt->m_dwLSN - poToInsert->m_dwBlockSize - poInner->m_dwBlockSize;
                poToInsert->m_dwLSN = poInsertAt->m_dwLSN - poToInsert->m_dwBlockSize;
            }
            else
            {
                poInner->m_dwLSN    = poInsertAt->m_dwLSN + poInsertAt->m_dwBlockSize + poToInsert->m_dwBlockSize + poInner->m_dwBlockSize;
                poToInsert->m_dwLSN = poInsertAt->m_dwLSN - poInsertAt->m_dwBlockSize;
            }
            
            // Set current insertion point to before the security placeholder
            poInsertAt = poInner;
        }

next:
        // If the newly placed object is a group object, then we need to add all of it's contained
        // objects immediately after the object itself (or before if on layer 1).
        if (poToInsert->GetType() == OBJ_GROUP)
        {
            // We just inserted a group -- we need to also insert all of it's
            // contained objects
            DWORD dwPrevLSN = poToInsert->m_dwLSN;
            CObject *poPrevContained = poToInsert;
            CObject *poContained;
            if (ploLayer->IsInsideOut())
                poContained = ((CObj_Group*)poToInsert)->m_gol.Tail();
            else
                poContained = ((CObj_Group*)poToInsert)->m_gol.Head();
            while (poContained)
            {
                poContained->RemoveFromList();
                poContained->m_dwLSN = dwPrevLSN;
                ploLayer->InsertOutside(poContained, poPrevContained);
                dwPrevLSN += poContained->m_dwBlockSize;
                if (ploLayer->IsInsideOut())
                {
                    poContained = ((CObj_Group*)poToInsert)->m_gol.Prev();
              //      poPrevContained->m_dwLSN = dwPrevLSN;
                }
                else
                {
                    poPrevContained = poContained;
                    poContained = ((CObj_Group*)poToInsert)->m_gol.Next();
                }
            }
        }

        // Get the 'outside'-most object from the to-insert list.
        poToInsert = polToInsert->GetOutside();
    }
    ValidatePlaceholders();
    EndProfile(PROFILE_INSERT);
    return true;
}

#define VALIDLSN_LAYER0_FIRST 89856
#define VALIDLSN_LAYER0_LAST  1646000
#define VALIDLSN_LAYER1_FIRST 1781168
#define VALIDLSN_LAYER1_LAST  3337312
#define NUM_SECTORS 1715632

bool CheckPHError(bool fWarnUser, bool fValid)
{
    if (fValid)
        return false;

    if (fWarnUser)
        MessageBox(NULL, "A placeholder was found in an invalid position.  Please notify Microsoft Xbox tech support immediately.",
                   "Invalid placeholder encountered", MB_ICONEXCLAMATION | MB_OK);
    return true;
}

bool CDVD::ValidatePlaceholders(bool fWarnUser)
{
    CObjNode *ponTemp, *ponCur = m_onlPH.m_ponHead;
    assert(ponCur);

    while (ponCur)
    {
        // verify this guy has the correct updated size hack fix
        if(ponCur->m_pobj->m_dwBlockSize != 4098)
            return false;

        // verify previous same-layer item before is > 132kb away
        ponTemp = ponCur->m_ponPrev;
        while (ponTemp && ponTemp->m_pobj->m_dwLSN / NUM_SECTORS != ponCur->m_pobj->m_dwLSN / NUM_SECTORS)
            ponTemp = ponTemp->m_ponPrev;
        if (ponTemp)
            if (CheckPHError(fWarnUser, ponCur->m_pobj->m_dwLSN - ponTemp->m_pobj->m_dwLSN > 0x21002))
                return false;

        // verify next same-layer item is > 132kb away
        ponTemp = ponCur->m_ponNext;
        while (ponTemp && ponTemp->m_pobj->m_dwLSN / NUM_SECTORS != ponCur->m_pobj->m_dwLSN / NUM_SECTORS)
            ponTemp = ponTemp->m_ponNext;
        if (ponTemp)
            if (CheckPHError(fWarnUser, ponTemp->m_pobj->m_dwLSN - ponCur->m_pobj->m_dwLSN > 0x21002))
                return false;

        // verify previous diff-layer item is > 64kb away
        ponTemp = ponCur->m_ponPrev;
        while (ponTemp && ponTemp->m_pobj->m_dwLSN / NUM_SECTORS == ponCur->m_pobj->m_dwLSN / NUM_SECTORS)
            ponTemp = ponTemp->m_ponPrev;
        if (ponTemp)
        {
            DWORD dwLSNTemp = 2 * 1715632 - ponTemp->m_pobj->m_dwLSN - 0x1002;
            if (CheckPHError(fWarnUser, ponCur->m_pobj->m_dwLSN - dwLSNTemp > 0x11002))
                return false;
        }

        // verify next diff-layer item is > 64kb away
        ponTemp = ponCur->m_ponNext;
        while (ponTemp && ponTemp->m_pobj->m_dwLSN / NUM_SECTORS == ponCur->m_pobj->m_dwLSN / NUM_SECTORS)
            ponTemp = ponTemp->m_ponNext;
        if (ponTemp)
        {
            DWORD dwLSNTemp = 2 * 1715632 - ponTemp->m_pobj->m_dwLSN - 0x1002;
            if (CheckPHError(fWarnUser, dwLSNTemp - ponCur->m_pobj->m_dwLSN > 0x11002))
                return false;
        }

        ponCur = ponCur->m_ponNext;
    }

    return true;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::CheckValidPlaceholderLSN
// Purpose:   Check if the specified placeholder can legally be placed at the
//            specified LSN
// Arguments: 
// Return:    'true' if successful, 'false' otherwise
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
bool CDVD::CheckValidPlaceholderLSN(CObj_Security *posPH, DWORD dwLSN)
{
    // There are a number of rules that must be obeyed in order for an LSN to
    // be valid for placeholder placement:
    //  1. The first possible layer 0 LSN start is 89856
    //  2. The last  possible layer 0 LSN start is 1646000
    //  3. The first possible layer 1 LSN start is 1781168
    //  4. The last  possible layer 1 LSN start is 3337312
    //  5. The end of a Placeholder cannot be any closer than 131072 sectors
    //     from the start of the next placeholder on the same layer.
    //  6. Placeholders on different layers must be separated by a minimum of
    //     65536 sectors from the one's complement of each placeholder's
    //     starting and ending addresses.
    StartProfile(PROFILE_CHECKVALIDPLACEHOLDERLSN);

    bool fValid = false;
    CObjNode *ponPH, *ponBase, *ponCur;
    CObject *poCur;
    DWORD dwLSNThis;

    poCur = m_onlPH.Head();
    if (!poCur)
    {
        EndProfile(PROFILE_CHECKVALIDPLACEHOLDERLSN);
        return false;
    }

    do { DebugOutput("%d  (%d)\r\n", poCur->m_dwLSN, poCur->m_dwLSN  >= 1715632 ? 1715632*2-poCur->m_dwLSN : 0); } while (poCur = m_onlPH.Next());
        
    // Verify that the LSN is within the valid ranges (rules 1-4 above)
    if (dwLSN < VALIDLSN_LAYER0_FIRST || dwLSN > VALIDLSN_LAYER1_LAST ||
        (dwLSN > VALIDLSN_LAYER0_LAST && dwLSN < VALIDLSN_LAYER1_FIRST))
    {
        EndProfile(PROFILE_CHECKVALIDPLACEHOLDERLSN);
        return false;
    }
    
    // Pointers to the placeholders are stored in sorted LSN order (relative
    // to start of layer!).  Find the specified placeholder and remove it from
    // the list in preparation for checking rules 5 and 6.
    ponPH = m_onlPH.GetObjectNode(posPH);
    assert(ponPH);

    // Track which element the removed placeholder was following in case we
    // need to reinsert
    ponBase = ponPH->m_ponPrev;

    // Remove the placeholder from the placeholders list
    m_onlPH.Remove(posPH);
    
    // Now that the placeholder has been removed, find the new point at which
    // the placeholder will be inserted (ie where dwLSN fits in).
    DWORD dwLSN1 = (dwLSN > 1715632) ? 1715632*2 - dwLSN : dwLSN;
    ponCur = m_onlPH.m_ponHead;
    while (ponCur) {
        DWORD dwLSN2 = (ponCur->m_pobj->m_dwLSN > 1715632) ? 
               1715632 * 2 - ponCur->m_pobj->m_dwLSN : ponCur->m_pobj->m_dwLSN;
        if (dwLSN2 > dwLSN1)
            break;
        ponCur = ponCur->m_ponNext;
    }

    // At this point, ponCur points at the placeholder node with the next highest
    // LSN.  posPH will fit in before it (if valid).  If ponCur is NULL, it means
    // that posPH has the highest (layer-relative) LSN.

    // Find the previous and next placeholders on both the same and opposite layer.
    CObjNode *ponPrevSame = ponCur, *ponPrevDiff = ponCur,
             *ponNextSame = ponCur, *ponNextDiff = ponCur;
    if (!ponCur) {
        ponPrevSame = ponPrevDiff = m_onlPH.m_ponTail;
        ponNextSame = ponNextDiff = NULL;
    } else {
        bool fFoundPrevSame = false, fFoundPrevDiff = false,
             fFoundNextSame = false, fFoundNextDiff = false;
        do {
            if (!fFoundPrevSame) {
                ponPrevSame = ponPrevSame->m_ponPrev;
                if (!ponPrevSame || ponPrevSame->m_pobj->m_dwLSN/NUM_SECTORS ==
                                    dwLSN/NUM_SECTORS)
                    fFoundPrevSame = true;
            }
            if (!fFoundPrevDiff) {
                ponPrevDiff = ponPrevDiff->m_ponPrev;
                if (!ponPrevDiff || ponPrevDiff->m_pobj->m_dwLSN/NUM_SECTORS !=
                                    dwLSN/NUM_SECTORS)
                    fFoundPrevDiff = true;
            }
            if (!fFoundNextSame) {
                if (!ponNextSame || ponNextSame->m_pobj->m_dwLSN/NUM_SECTORS ==
                                    dwLSN/NUM_SECTORS)
                    fFoundNextSame = true;
                else
                    ponNextSame = ponNextSame->m_ponNext;
            }
            if (!fFoundNextDiff) {
                if (!ponNextDiff || ponNextDiff->m_pobj->m_dwLSN/NUM_SECTORS !=
                                    dwLSN/NUM_SECTORS)
                    fFoundNextDiff = true;
                else
                    ponNextDiff = ponNextDiff->m_ponNext;
            }
        } while(!(fFoundPrevSame && fFoundPrevDiff && fFoundNextSame && fFoundNextDiff));
    }

    // Check rule 5: same-layer placeholders must be 131072 or more sectors apart
    if (ponNextSame)
        if ((int)(ponNextSame->m_pobj->m_dwLSN - (dwLSN + ponCur->m_pobj->m_dwBlockSize)) < 131072)
            goto done;
    if (ponPrevSame)
        if ((int)(dwLSN - (ponPrevSame->m_pobj->m_dwLSN + ponPrevSame->m_pobj->m_dwBlockSize)) < 131072)
            goto done;

    //  Check rule 6. Placeholders on different layers must be separated by a
    //                minimum of 65536 sectors from the one's complement of
    //                each placeholder's starting and ending addresses.

    // Convert the to-be-inserted PH's LSN to the other PH's numeric space
    dwLSNThis = 2 * 1715632 - dwLSN - 0x1002;
    if (ponNextDiff && abs(dwLSNThis - ponNextDiff->m_pobj->m_dwLSN) < 0x11002)
        goto done;
    if (ponPrevDiff && abs(dwLSNThis - ponPrevDiff->m_pobj->m_dwLSN) < 0x11002)
        goto done;

    // If here, then we passed all rules!
    fValid = true;
    
done:

    if (fValid) {
        // Insert at insertion point
        if (ponCur)
            m_onlPH.AddBefore(posPH, ponCur->m_pobj);
        else
            m_onlPH.Add(posPH);
    } else {
        // restore original point
        if (!ponBase) {
            ponBase = m_onlPH.m_ponHead;
            m_onlPH.AddBefore(posPH, ponBase->m_pobj);
        }
        else
            m_onlPH.AddAfter(posPH, ponBase->m_pobj);
    }

    ValidatePlaceholders();

    EndProfile(PROFILE_CHECKVALIDPLACEHOLDERLSN);
    return fValid;
}

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Function:  CDVD::RefreshPlaceholders
// Purpose:   
// Arguments: None
// Return:    None
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void CDVD::RefreshPlaceholders(bool fValidate)
{
    // refresh the placeholder list.  This must be called after open, new, or 
    // undo/redo operations (ie any time a placeholder could have been moved)
    DWORD dwLSNCur, dwLSNCur2;

    StartProfile(PROFILE_REFRESHPLACEHOLDERS);    
    m_onlPH.RemoveAll();

    // find all placeholders, add them to the list of placeholders in sorted
    // lsn order (relative to start of layer)
    for (int i = 0; i <= 1; i++)
    {
        CObjList *polLayer = m_rgpolLayer[i];
        CObject *poCur = polLayer->GetInside();
        while (poCur)
        {
            // If the object is a file-system object (folder or file), check
            // to see if it resides in the passed-in folder.
            if (poCur->GetType() == OBJ_SEC)
            {
                dwLSNCur = (poCur->m_dwLSN > 1715632) ? 1715632*2-poCur->m_dwLSN : poCur->m_dwLSN;
                // Find insertion point
                CObjNode *ponCur = m_onlPH.m_ponHead;
                while (ponCur) {
                    dwLSNCur2 = (ponCur->m_pobj->m_dwLSN > 1715632) ? 1715632*2-ponCur->m_pobj->m_dwLSN : ponCur->m_pobj->m_dwLSN;
                    if (dwLSNCur2 > dwLSNCur)
                        break;
                    ponCur = ponCur->m_ponNext;
                }
                if (ponCur)
                    m_onlPH.AddBefore(poCur, ponCur->m_pobj);
                else
                    m_onlPH.Add(poCur);
            }
            poCur = polLayer->GetNextOuter();
        }
    }
    if (fValidate)
        ValidatePlaceholders();

#ifdef _DEBUG1
    m_onlPH.Head();
    m_onlPH.Next();
    CheckValidPlaceholderLSN((CObj_Security*)m_onlPH.Next(), 48000+4100+131072);
#endif
    EndProfile(PROFILE_REFRESHPLACEHOLDERS);    
}
