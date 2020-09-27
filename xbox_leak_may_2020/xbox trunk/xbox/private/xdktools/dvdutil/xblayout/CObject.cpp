// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CObject.cpp
// Contents:  
// Revisions: 14-Jun-2001: Created (jeffsim)
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- Precompiled header file
#include "stdafx.h"

bool g_fResizedVolDesc;


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CObject::CObject()
{
    m_poInner = m_poOuter = NULL;
//    m_poTreeParent = NULL;
  //  m_fTreeViewable = false;
    m_dwLSN = LSN_UNPLACED;
    m_pog = NULL;
    m_pol = NULL;
    m_fIgnoreNextUnselect = false;
    m_fWasSelected = false;
    m_pof = NULL;
    m_iFolderDirEntry = -1;
    m_fCMF_Present = true;
    m_fCMF_Resized = false;
}

DWORD BlockSize(ULONGLONG ullSize)
{
    DWORD dwSize = (DWORD)(ullSize / 2048 + ((ullSize % 2048) ? 1 : 0));
    if (dwSize == 0) dwSize = 1;
    return dwSize;
}

CObj_Bookend::CObj_Bookend() : CObject()
{
    m_uliSize.QuadPart = 0;
    m_dwBlockSize = BlockSize(m_uliSize.QuadPart);
    LoadString(g_hinst, IDS_OBJECT_BOOKEND, m_szName, MAX_PATH);
    strcpy(m_szFolder, "");
    sprintf(m_szFullFileName, "%s\\%s", m_szFolder, m_szName);

    SetInited(S_OK);
}

CObj_Empty::CObj_Empty(DWORD dwSize) : CObject()
{
    m_uliSize.QuadPart = dwSize;
    m_dwBlockSize = BlockSize(m_uliSize.QuadPart);
    LoadString(g_hinst, IDS_OBJECT_EMPTY, m_szName, MAX_PATH);
    strcpy(m_szFolder, "");
    sprintf(m_szFullFileName, "%s\\%s", m_szFolder, m_szName);

    SetInited(S_OK);
}

CObj_VolDesc::CObj_VolDesc() : CObject()
{
    m_uliSize.QuadPart = sizeof(GDF_VOLUME_DESCRIPTOR) + 2048;
    m_dwBlockSize = BlockSize(m_uliSize.QuadPart);
    LoadString(g_hinst, IDS_OBJECT_VOLDESC, m_szName, MAX_PATH);
    strcpy(m_szFolder, "");
    sprintf(m_szFullFileName, "%s\\%s", m_szFolder, m_szName);

    SetInited(S_OK);
}

CObj_Security::CObj_Security() : CObject()
{
    // We add two blocks to the 4096 size so that we can force the actual
    // placeholder to a two-sector boundary
    m_uliSize.QuadPart = 2048 * 4098;
    m_dwBlockSize = BlockSize(m_uliSize.QuadPart);
    LoadString(g_hinst, IDS_OBJECT_SECURITY, m_szName, MAX_PATH);
    strcpy(m_szFolder, "");
    sprintf(m_szFullFileName, "%s\\%s", m_szFolder, m_szName);
    SetInited(S_OK);
}

CObj_File::CObj_File(char *szFolder, char *szFilename, DWORD dwSizeHigh, DWORD dwSizeLow) : CObject()
{
    m_uliSize.HighPart = dwSizeHigh;
    m_uliSize.LowPart  = dwSizeLow;

    m_dwBlockSize = BlockSize(m_uliSize.QuadPart);
    strcpy(m_szName, szFilename);
    strcpy(m_szFolder, szFolder);
    sprintf(m_szFullFileName, "%s\\%s", m_szFolder, m_szName);

    SetInited(S_OK);
}

CObj_Folder::CObj_Folder(char *szFolder, char *szFilename) : CObject()
{
    strcpy(m_szName, szFilename);
    strcpy(m_szFolder, szFolder);
    sprintf(m_szFullFileName, "%s\\%s", m_szFolder, m_szName);

    m_uliSize.HighPart = 0;
    m_uliSize.LowPart  = 2;
    m_dwBlockSize = BlockSize(m_uliSize.QuadPart);

    m_prgGDFMDE = NULL;
    SetInited(S_OK);
}

CObj_Folder::~CObj_Folder()
{
    if (m_prgGDFMDE)
        delete[] m_prgGDFMDE;
}

CObj_Group::CObj_Group() : CObject()
{
    m_uliSize.QuadPart = 0;
    m_dwBlockSize = BlockSize(m_uliSize.QuadPart);
    strcpy(m_szName, "Group");
    strcpy(m_szFolder, "");
    sprintf(m_szFullFileName, "%s\\%s", m_szFolder, m_szName);

    SetInited(S_OK);
}
void CObject::SetGroup(CObj_Group *pog)
{
    // If we're already a member of a group, then remove ourself from it
    if (m_pog)
        m_pog->RemoveGroupedObject(this);
        
    // Add ourselves to the specified group
    if (pog)
        pog->InsertGroupedObject(this);
    
    // Track which group we're now in
    m_pog = pog;
}

void CObject::SetGroupBeforeObject(CObj_Group *pog, CObject *poOutside)
{
    // Add this object to the 'pog' group, inside of the object specified
    // in 'poOutside'

    // If we're already a member of a group, then remove ourself from it
    if (m_pog)
        m_pog->RemoveGroupedObject(this);
        
    // Add ourselves to the specified group
    if (pog)
        pog->InsertGroupedObjectBeforeObject(this, poOutside);
    
    // Track which group we're now in
    m_pog = pog;
}

void CObject::SetGroupAfterObject(CObj_Group *pog, CObject *poOutside)
{
    // If we're already a member of a group, then remove ourself from it
    if (m_pog)
        m_pog->RemoveGroupedObject(this);
        
    // Add ourselves to the specified group
    if (pog)
        pog->InsertGroupedObjectAfterObject(this, poOutside);
    
    // Track which group we're now in
    m_pog = pog;
}

void CObj_Group::RemoveGroupedObject(CObject *pobj)
{
    assert(pobj->m_pog == this);
    m_gol.Remove(pobj);
    m_dwBlockSize -= pobj->m_dwBlockSize;
}

void CObj_Group::RemoveAllGroupedObjects()
{
    m_gol.RemoveAll();
    m_dwBlockSize = 0;
}

void CObj_Group::InsertGroupedObject(CObject *pobj)
{
    assert(pobj->m_pog == NULL);

    // Add the object to the end of this group's list of grouped objects
    m_gol.Add(pobj);
    m_dwBlockSize += pobj->m_dwBlockSize;
}

void CObj_Group::InsertGroupedObjectBeforeObject(CObject *pobj, CObject *poOutside)
{
    assert(pobj->m_pog == NULL);

    // Find the object specified by 'poOutside'.  Insert this group 'inside' of it.
    m_gol.AddBefore(pobj, poOutside);
    
    m_dwBlockSize += pobj->m_dwBlockSize;
}
void CObj_Group::InsertGroupedObjectAfterObject(CObject *pobj, CObject *poOutside)
{
    assert(pobj->m_pog == NULL);

    m_gol.AddAfter(pobj, poOutside);
    
    m_dwBlockSize += pobj->m_dwBlockSize;
}


CObjList::CObjList()
{
    m_poHead = NULL;
    m_poTail = NULL;
    m_fInsideOut = false;
    m_fModified = false;
}

CObjList::~CObjList()
{
    // clear out contents
    Clear();
}

void CObjList::Reset()
{
    CObject *pobjNext;

    // Delete all entries
    while (m_poHead)
    {
        if (m_fInsideOut)
            pobjNext = m_poHead->m_poInner;
        else
            pobjNext = m_poHead->m_poOuter;
        delete m_poHead;
        m_poHead = pobjNext;
    }
    m_poTail = NULL;
    m_fModified = true;
}
void CObjList::Clear()
{
    while (m_poHead)
        m_poHead->RemoveFromList();

    m_poTail = NULL;
    m_fModified = true;
}

int CObjList::GetNumObjects(DWORD dwObjTypes)
{
    CObject *poCur;
    int cObjects = 0;

    if (m_fInsideOut)
        poCur = m_poTail;
    else
        poCur = m_poHead;

    while (poCur)
    {
        if (poCur->GetType() & dwObjTypes)
            cObjects++;
        poCur = poCur->m_poOuter;
    }
    return cObjects;
}

int CObjList::GetObjectSizes(DWORD dwObjTypes)
{
    CObject *poCur;
    DWORD dwSize = 0;

    if (m_fInsideOut)
        poCur = m_poTail;
    else
        poCur = m_poHead;

    while (poCur)
    {
        if (poCur->GetType() & dwObjTypes)
            dwSize+=poCur->m_dwBlockSize;
        poCur = poCur->m_poOuter;
    }
    return dwSize;
}


// Add to the tail, no questions asked
void CObjList::AddToTail(CObject *ppo)
{
    if (!m_poHead)
    {
        // First entry in list
        m_poHead = m_poTail = ppo;
    }
    else
    {
        if (m_fInsideOut)
        {
            m_poTail->m_poInner = ppo;
            ppo->m_poOuter = m_poTail;
            m_poTail = ppo;
        }
        else
        {
            m_poTail->m_poOuter = ppo;
            ppo->m_poInner = m_poTail;
            m_poTail = ppo;
        }
    }
    ppo->m_pol = this;
    m_fModified = true;
}

// Add to the head, no questions asked
void CObjList::AddToHead(CObject *ppo)
{
    if (!m_poHead)
    {
        // First entry in list
        m_poHead = m_poTail = ppo;
    }
    else
    {
        if (m_fInsideOut)
        {
            m_poHead->m_poOuter = ppo;
            ppo->m_poInner = m_poHead;
            m_poHead = ppo;
        }
        else
        {
            m_poHead->m_poInner = ppo;
            ppo->m_poOuter = m_poHead;
            m_poHead = ppo;
        }
    }
    ppo->m_pol = this;
    m_fModified = true;
}

void CObject::RemoveFromList()
{
    if (m_pol)
        m_pol->Remove(this);
}

void CObjList::Remove(CObject *ppo)
{
    if (m_poHead == ppo)
        m_poHead = m_poHead->m_poOuter;
    if (m_poTail == ppo)
        m_poTail = m_poTail->m_poInner;

    if (ppo->m_poInner)
        ppo->m_poInner->m_poOuter = ppo->m_poOuter;

    if (ppo->m_poOuter)
        ppo->m_poOuter->m_poInner = ppo->m_poInner;

    ppo->m_poInner = NULL;
    ppo->m_poOuter = NULL;
    ppo->m_pol = NULL;
    ppo->m_dwLSN = LSN_UNPLACED;
    m_fModified = true;
}

bool CObjList::InsertBefore(CObject *poToAdd, CObject *poLoc)
{
    if (!poLoc->m_poInner)
        m_poHead = poToAdd;
    else
        poLoc->m_poInner->m_poOuter = poToAdd;

    poToAdd->m_poInner = poLoc->m_poInner;
    poLoc->m_poInner = poToAdd;
    poToAdd->m_poOuter = poLoc;
    poToAdd->m_pol = this;

    m_fModified = true;
    return true;
}

bool CObjList::InsertAfter(CObject *poToAdd, CObject *poLoc)
{
    if (poLoc->m_poOuter == NULL)
        m_poTail = poToAdd;
    else
        poLoc->m_poOuter->m_poInner = poToAdd;
    poToAdd->m_poOuter = poLoc->m_poOuter;
    poLoc->m_poOuter = poToAdd;
    poToAdd->m_poInner = poLoc;
    poToAdd->m_pol = this;
    m_fModified = true;
    return true;
}

CObject *CObjList::FindObjectByFullFileName(char *szName)
{
    if (szName == NULL || *szName == '\0')
        return NULL;
	CObject *poCur = GetInside();
    while (poCur)
    {
        if (!lstrcmpi(szName, poCur->m_szFullFileName))
            return poCur;
        poCur = poCur->m_poOuter;
    }
    return NULL;
}

// save out contents to the specified file
bool CObjList::PersistTo(CFile *pfile)
{
	CObject *poCur = GetInside();
	
	// Force writing out NULL object at end
	while (poCur)
	{
		if (!poCur->PersistTo(pfile))
			return false;
		poCur = poCur->m_poOuter;
	}
	if (!pfile->WriteDWORD(OBJ_ENDOFLIST))
		return false;
	return true;
}

// a little more work than persist to since we need to determine the object types, put them
// in the list, etc
bool CObjList::CreateFrom(CFile *pfile)
{
	DWORD dwType, dwLSN, dwBlockSize, dwGroupId, dwFolderId, dwParentFolderId;
    bool  fWasSelected;
	ULARGE_INTEGER uliSize;
	char szName[MAX_PATH];
	char szFolder[MAX_PATH];

    CObjNodeList onlGroups;
    m_fModified = true;

	while (1)
	{
		// Get next object type
		if (!pfile->ReadDWORD(&dwType))
			return false;
        if (dwType == OBJ_ENDOFLIST)
            break;

		// read all fields to temp vars
		if (!pfile->ReadDWORD(&dwLSN))
			return false;
		if (!pfile->ReadDWORD(&dwBlockSize))
			return false;
		if (!pfile->ReadDWORD(&uliSize.LowPart))
			return false;
		if (!pfile->ReadDWORD(&uliSize.HighPart))
			return false;
		if (!pfile->ReadString(szName))
			return false;
		if (!pfile->ReadString(szFolder))
			return false;
        if (!pfile->ReadDWORD(&dwGroupId))
            return false;

        if (!pfile->ReadDWORD(&dwParentFolderId))
            return false;

		CObject *pobj;
		switch(dwType)
		{
		case OBJ_BE:
			pobj = new CObj_Bookend();
			break;
        case OBJ_VOLDESC:
			pobj = new CObj_VolDesc();

            // If the size *read* was one block, then we've just resized it to 2
            if (dwBlockSize == 1)
                g_fResizedVolDesc = true;
			break;
		case OBJ_SEC:
			pobj = new CObj_Security();
            // We force the file size to match what comes out of the file, not
            // how big we think it should be, so we can hit the placeholder
            // check code and resize if necessary
            pobj->m_uliSize = uliSize;
            pobj->m_dwBlockSize = dwBlockSize;
			break;
		case OBJ_GROUP:
			pobj = new CObj_Group();
			break;
		case OBJ_FILE:
			pobj = new CObj_File(szFolder, szName, uliSize.HighPart, uliSize.LowPart);
			break;
		case OBJ_FOLDER:
			pobj = new CObj_Folder(szFolder, szName);
            if (!pobj)
                FatalError(E_OUTOFMEMORY);

            // Also get and store the id for the folder -- this id is used to
            // map which files belong to this folder.
            if (!pfile->ReadDWORD(&dwFolderId))
                return false;
            ((CObj_Folder*)pobj)->m_dwFolderId = dwFolderId;
            pobj->m_uliSize.QuadPart = uliSize.QuadPart;
            pobj->m_dwBlockSize = dwBlockSize;
			break;
		}
        if (!pobj)
            FatalError(E_OUTOFMEMORY);

        // For memory snapshots, track selection state as well
        if (pfile->m_fMemoryFile)
        {
	        if (!pfile->ReadBYTE((BYTE*)&fWasSelected))
		        return false;
	        if (!pfile->ReadBYTE((BYTE*)&pobj->m_fCMF_Resized))
		        return false;
        }
        else
            fWasSelected = false;        

        pobj->m_dwLSN = dwLSN;
        pobj->m_fWasSelected = fWasSelected;
        pobj->m_dwParentFolderId = dwParentFolderId;
        
        // If the new object is a group, then as it to the list of groups so
        // that subsequently added objects that were initially members of the
        // group can be put back into the group.
        if (dwGroupId)
        {
            if (dwType == OBJ_GROUP)
            {
                // It's a group; add it to the list of groups
                ((CObj_Group*)pobj)->m_dwPersistId = dwGroupId;
                onlGroups.Add(pobj);
            }
            else
            {
                // It's an object that belongs in a group.  Map the group id to
                // the actual group object and add the object to it.  Note that
                // groups are always saved to file before the objects that it
                // contains, so we know that a groupid will match to an
                // existing group.
                CObject *poCur = onlGroups.Head();
                while (poCur)
                {
                    if (((CObj_Group*)poCur)->m_dwPersistId == dwGroupId)
                    {
                        // Found the group
                        pobj->SetGroup((CObj_Group*)poCur);
                        break;
                    }
                    poCur = onlGroups.Next();
                }
            }
        }
		// add new object to end of list
        if (m_fInsideOut)
    		AddToHead(pobj);
        else
    		AddToTail(pobj);
	}

    // In the second pass, map folder ids to folders
    // Create the list of folders (do this since normally far more files than
    // folders, and this allows us to skip unnecessarily searching through files
    // many times).
    CObjNodeList onlFolders;
    CObject *poCur = GetInside();
    while (poCur)
    {
        if (poCur->GetType() == OBJ_FOLDER)
            onlFolders.Add(poCur);
        poCur = poCur->m_poOuter;
    }

    poCur = GetInside();
    while (poCur)
    {
        if (poCur->GetType() == OBJ_FILE || poCur->GetType() == OBJ_FOLDER)
        {
            // Find the folder that maps to the current object's folder id
            CObject *poTest = onlFolders.Head();
            while (poTest)
            {
                if (poTest != poCur)
                {
                    if (poCur->m_dwParentFolderId == ((CObj_Folder*)poTest)->m_dwFolderId)
                    {
                        poCur->m_pof = (CObj_Folder*)poTest;
                        break;
                    }
                }
                poTest = onlFolders.Next();
            }
        }
        poCur = poCur->m_poOuter;
    }
    return true;
}

bool CObject::PersistTo(CFile *pfile)
{
	if (!pfile->WriteDWORD(GetType()))
		return false;
	if (!pfile->WriteDWORD(m_dwLSN))
		return false;
	if (!pfile->WriteDWORD(m_dwBlockSize))
		return false;
	if (!pfile->WriteDWORD(m_uliSize.LowPart))
		return false;
	if (!pfile->WriteDWORD(m_uliSize.HighPart))
		return false;
	if (!pfile->WriteString(m_szName))
		return false;
	if (!pfile->WriteString(m_szFolder))
		return false;

    // If this object is a member of a group, then write out an identifier so
    // that we can determine group containership on reload.  For this identifier,
    // we just use the actual CObj_Group object pointer - we don't use it as a
    // pointer on reload, but instead just as a unique id.
    // If this object is a group, then write out it's memory address instead so
    // that the loading code can map the group id to the group
    if (GetType() == OBJ_GROUP)
    {
        if (!pfile->WriteDWORD((DWORD)this))
            return false;
    }
    else
    {
        if (!pfile->WriteDWORD((DWORD)m_pog))
            return false;
    }

    // Similarly, write out the object's folder object as a unique identifier.
    // If this object is a folder itself, then also write out it's "id".
    if (!pfile->WriteDWORD((DWORD)m_pof))
        return false;
    if (GetType() == OBJ_FOLDER)
        if (!pfile->WriteDWORD((DWORD)this))
            return false;

    // For memory snapshots, track selection state as well
    if (pfile->m_fMemoryFile)
    {
	    if (!pfile->WriteBYTE(m_fWasSelected))
		    return false;
	    if (!pfile->WriteBYTE(m_fCMF_Resized))
		    return false;
    }

	return true;
}
