// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      CFileObject.cpp
// Contents:  
// Revisions: 29-Nov-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// "stdafx.h"       -- stdafxiled header file
#include "stdafx.h"


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ FUNCTIONS ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CFileObjectNode::CFileObjectNode(CFileObject *pfo)
{
    m_pfo = pfo;
    pfo->m_pfon = this;
    m_pfonPrev = m_pfonNext = NULL;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CFileObjectList::CFileObjectList()
{
    m_pfonHead = m_pfonCur = NULL;
    m_uliSize.QuadPart = 0;
    m_cObjects = 0;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CFileObjectList::Add(CFileObject *pfo)
{
    CFileObjectNode *pfonNew = new CFileObjectNode(pfo);
    if (m_pfonHead == NULL)
        m_pfonHead = pfonNew;
    else
    {
        pfonNew->m_pfonNext = m_pfonHead;
        m_pfonHead->m_pfonPrev = pfonNew;
        m_pfonHead = pfonNew;
    }
    m_cObjects++;
    m_uliSize.QuadPart += pfo->m_uliSize.QuadPart;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CFileObjectList::Remove(CFileObject *pfo)
{
    CFileObjectNode *pfon = pfo->m_pfon;
    if (pfon == m_pfonHead)
    {
        m_pfonHead = m_pfonHead->m_pfonNext;
        if (m_pfonHead)
            m_pfonHead->m_pfonPrev = NULL;
    }
    else
    {
        if (pfon->m_pfonPrev)
            pfon->m_pfonPrev->m_pfonNext = pfon->m_pfonNext;
        if (pfon->m_pfonNext)
            pfon->m_pfonNext->m_pfonPrev = pfon->m_pfonPrev;
    }
    if (pfon == m_pfonCur)
        m_pfonCur = NULL;
    delete pfo->m_pfon;
    pfo->m_pfon = NULL;
    m_uliSize.QuadPart -= pfo->m_uliSize.QuadPart;
    m_cObjects--;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CFileObject *CFileObjectList::GetByName(char *szName)
{
    CFileObjectNode *pfonCur = m_pfonHead;
    while (pfonCur)
    {
        if (!_stricmp(pfonCur->m_pfo->m_szName, szName))
            return pfonCur->m_pfo;
        pfonCur = pfonCur->m_pfonNext;
    }
    
    return NULL;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CFileObject *CFileObjectList::GetCur()
{
    if (m_pfonCur == NULL)
        return NULL;
    else
        return m_pfonCur->m_pfo;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CFileObject *CFileObjectList::GetFirst()
{
    m_pfonCur = m_pfonHead;
    return GetCur();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CFileObject *CFileObjectList::GetNext()
{
    if (m_pfonCur == NULL)
        return GetFirst();
    m_pfonCur = m_pfonCur->m_pfonNext;
    return GetCur();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
CFileObject::CFileObject(char *szObjName, WIN32_FIND_DATA *pwfd,
                         CChangeQueue *pcq)
{
    SYSTEMTIME stModified;
    FILETIME ft;

    // szObjName must contain full path string
    assert(strrchr(szObjName, '\\') != NULL);
    
    m_uliSize.HighPart = pwfd->nFileSizeHigh;
    m_uliSize.LowPart  = pwfd->nFileSizeLow;
    m_ftModified       = pwfd->ftLastWriteTime;
    m_fIsDir           = pwfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
    
    m_pfoParent        = NULL;
    m_pfoChangeThread  = NULL;
    m_pfoMainThread    = NULL;

    m_hti  = NULL;
    m_checkstate = CHECKSTATE_CHECKED;
    m_nDepth = 0;
   

    // Type info
    m_fMouseOver = FALSE;
    sprintf(m_szPath, szObjName);
    strcpy(m_szName, pwfd->cFileName);
    strcpy(m_szLocation, "Excluded");

    m_dwa              = pwfd->dwFileAttributes;
    m_ftModified       = pwfd->ftLastWriteTime;
    m_uliSize.LowPart  = pwfd->nFileSizeLow;
    m_uliSize.HighPart = pwfd->nFileSizeHigh;
    m_fTypeInfoLoaded  = false;

    LoadTypeInfo();

    FileTimeToLocalFileTime(&m_ftModified, &ft);
    FileTimeToSystemTime(&ft, &stModified);
    char *pszAMPM;
    if (stModified.wHour > 12)
    {
        stModified.wHour -= 12;
        pszAMPM = "PM";
    }
    else
        pszAMPM = "AM";
    strcpy(m_szFolder, "undone");
    strcpy(m_szSizeOnDisk, "undone");
    strcpy(m_szSectorRange, "undone");

    sprintf(m_szModified, "%02d/%02d/%d  %d:%02d %s",
        stModified.wMonth, stModified.wDay, stModified.wYear,
        stModified.wHour, stModified.wMinute, pszAMPM);

    if (m_uliSize.QuadPart != 0)
    {
        char sz[1024];
        strcpy(sz, "                  ");
        FormatByteString(m_uliSize, sz);
        sprintf(m_szSize, sz);
    }
    else
        strcpy(m_szSize, "");

    m_pcq = pcq;
    m_fInited = true;
}

void CFileObject::UpdateSizeTime()
{
    WIN32_FILE_ATTRIBUTE_DATA wfad;
    if (GetFileAttributesEx(m_szPath, GetFileExInfoStandard, &wfad) == 0)
    {
        // Couldn't find the file - must have been deleted; leave size/time
        // info alone.
        return;
    }

    m_ftModified      = wfad.ftLastWriteTime;
    m_uliSize.LowPart  = wfad.nFileSizeLow;
    m_uliSize.HighPart = wfad.nFileSizeHigh;

    SYSTEMTIME stModified;
    FILETIME ft;
    FileTimeToLocalFileTime(&m_ftModified, &ft);
    FileTimeToSystemTime(&ft, &stModified);
    char *pszAMPM;
    if (stModified.wHour > 12)
    {
        stModified.wHour -= 12;
        pszAMPM = "PM";
    }
    else
        pszAMPM = "AM";

    sprintf(m_szModified, "%02d/%02d/%d  %d:%02d %s",
        stModified.wMonth, stModified.wDay, stModified.wYear,
        stModified.wHour, stModified.wMinute, pszAMPM);

    if (m_uliSize.QuadPart != 0)
    {
        char sz[1024];
        FormatByteString(m_uliSize, sz);
        sprintf(m_szSize, sz);
    }
    else
        strcpy(m_szSize, "");
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CFileObject::CFileObject(CFileObject *pfoSource)
{
    // undone: copy constructor?
    // undone: copying unnecessary vars
    m_pfoParent = NULL;

    strcpy(m_szLocation, pfoSource->m_szLocation);
    strcpy(m_szName, pfoSource->m_szName);
    strcpy(m_szType, pfoSource->m_szType);
    strcpy(m_szSize, pfoSource->m_szSize);
    strcpy(m_szModified, pfoSource->m_szModified);
    strcpy(m_szPath, pfoSource->m_szPath);
    m_hti  = NULL;
    m_checkstate = CHECKSTATE_UNCHECKED;
    m_nDepth = 0;
    m_uliSize = pfoSource->m_uliSize;
    m_ftModified = pfoSource->m_ftModified;
    m_dwa = pfoSource->m_dwa;
    m_fIsDir = pfoSource->m_fIsDir;
    m_fMouseOver = FALSE;
    m_iIcon = pfoSource->m_iIcon;
    m_pfoChangeThread = NULL;
    m_pfoMainThread = NULL;
    m_pfon = NULL;
    m_pcq = NULL;

    strcpy(m_szFolder, "undone");
    strcpy(m_szSizeOnDisk, "undone");
    strcpy(m_szSectorRange, "undone");
    m_pfoInner = NULL;
    m_pfoOuter = NULL;
    m_fInited = true;
}

CFileObject::~CFileObject()
{
    if (m_pfoParent && m_pfon)
        m_pfoParent->RemoveChild(this);
    m_pfoParent = NULL;
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CFileObject::AddChild(CFileObject *pfoChild)
{
    if (pfoChild == NULL)
        return;
    pfoChild->m_pfoParent = this;
    pfoChild->m_pfoOrigParent = this;
    m_lpfoChildren.Add(pfoChild);
    if (m_pcq)
        m_pcq->Enqueue(EVENT_ADDED, pfoChild);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void CFileObject::RemoveChild(CFileObject *pfoChild)
{
    // Keep track of pointer to parent object for main thread wil need it.
    if (m_pcq == NULL)
        pfoChild->m_pfoParent = NULL;
    m_lpfoChildren.Remove(pfoChild);
    if (m_pcq)
        m_pcq->Enqueue(EVENT_REMOVED, pfoChild);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CFileObject *CFileObject::GetFirstChild()
{
    return m_lpfoChildren.GetFirst();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CFileObject *CFileObject::GetNextChild()
{
    return m_lpfoChildren.GetNext();
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

CFileObject *CFileObject::GetChildByName(char *szName)
{
    return m_lpfoChildren.GetByName(szName);
}

// Write this object's contents to the specified destination file
BOOL CFileObject::PersistTo(CFile *pfileDest)
{
    char *szRelPath = m_szPath + strlen(g_szRootDir);
    if (!pfileDest->WriteString(szRelPath) || 
        !pfileDest->WriteString(m_szLocation) ||        // recreate?
        !pfileDest->WriteString(m_szType) ||            // recreate?
        !pfileDest->WriteString(m_szSize) ||            // recreate?
        !pfileDest->WriteString(m_szModified) ||
        !pfileDest->WriteDWORD(m_checkstate) ||
        !pfileDest->WriteDWORD(m_nDepth) ||
        !pfileDest->WriteData(&m_uliSize, sizeof m_uliSize) ||
        !pfileDest->WriteData(&m_ftModified, sizeof m_ftModified) ||
        !pfileDest->WriteDWORD(m_dwa) ||
        !pfileDest->WriteString(m_szFolder) ||          // recreate?
        !pfileDest->WriteString(m_szSizeOnDisk) ||      // recreate?
        !pfileDest->WriteString(m_szSectorRange) ||     // recreate?
        !pfileDest->WriteDWORD(m_lpfoChildren.GetCount()))
        return FALSE;        

    // Recurse into children
    CFileObject *pfoChild = m_lpfoChildren.GetFirst();
    while (pfoChild)
    {
        if (!pfoChild->PersistTo(pfileDest))
            return FALSE;
        pfoChild = m_lpfoChildren.GetNext();
    }

    return TRUE;
}

CFileObject::CFileObject(CFile *pfileSrc)
{
    char szRelPath[MAX_PATH];
    DWORD nChildren;
    if (!pfileSrc->ReadString(szRelPath) || 
        !pfileSrc->ReadString(m_szLocation) ||      // recreate?
        !pfileSrc->ReadString(m_szType) ||          // recreate?
        !pfileSrc->ReadString(m_szSize) ||          // recreate?
        !pfileSrc->ReadString(m_szModified) ||
        !pfileSrc->ReadDWORD((DWORD*)&m_checkstate) ||
        !pfileSrc->ReadDWORD((DWORD*)&m_nDepth) ||
        !pfileSrc->ReadData(&m_uliSize, sizeof m_uliSize) ||
        !pfileSrc->ReadData(&m_ftModified, sizeof m_ftModified) ||
        !pfileSrc->ReadDWORD(&m_dwa) ||
        !pfileSrc->ReadString(m_szFolder) ||        // recreate?
        !pfileSrc->ReadString(m_szSizeOnDisk) ||    // recreate?
        !pfileSrc->ReadString(m_szSectorRange) ||   // recreate?
        !pfileSrc->ReadDWORD(&nChildren))
        return;

    // Generate path and name from szRelPath
    sprintf(m_szPath, "%s%s", g_szRootDir, szRelPath);
    if (strrchr(szRelPath, '\\'))
        strcpy(m_szName, strrchr(szRelPath, '\\') + 1);
    else
    {
        strcpy(m_szName, "");
    }
    
    m_hti        = NULL;
    m_fIsDir     = m_dwa & FILE_ATTRIBUTE_DIRECTORY ? true : false;
    m_fMouseOver = FALSE;
    m_iIcon      = 0;
    m_pfon       = NULL;
    m_pcq        = NULL;
    m_pfoInner   = NULL;
    m_pfoOuter   = NULL;
    m_pfoParent  = NULL;
    m_pfoChangeThread = NULL;
    m_pfoMainThread   = NULL;
    m_fTypeInfoLoaded = false;
  
    LoadTypeInfo();

    // Recurse into children
    for (DWORD i = 0; i < nChildren; i++)
    {
        CFileObject *pfoChild = new CFileObject(pfileSrc);
        if (!pfoChild->m_fInited)
            return;
        AddChild(pfoChild);
    }

    m_fInited = true;
}
