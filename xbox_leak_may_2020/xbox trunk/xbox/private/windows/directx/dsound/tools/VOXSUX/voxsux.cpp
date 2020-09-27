/***************************************************************************
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       voxsux.cpp
 *  Content:    Sets the Voxware ACM CODEC to the lowest possible priority.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  05/29/01    dereks  Created.
 *
 ****************************************************************************/

#include <windows.h>

//
// Basic linked-list template class
//

template <class type> class CList
{
protected:
    type *                  m_pHead;        // Pointer to the head of the list
    type *                  m_pTail;        // Pointer to the tail of the list

public:
    CList(void);

public:
    // Node creation, removal
    void AddNode(type *pNode);
    void InsertNode(type *pPrev, type *pNode);
    void RemoveNode(type *pNode);
    void Reset(void);
    
    // Basic list information
    type *GetListHead(void);
    type *GetListTail(void);
};


template <class type>
CList<type>::CList
(
    void
)
{
    m_pHead = NULL;
    m_pTail = NULL;
}


template <class type>
void
CList<type>::AddNode
(
    type *                  pNode
)
{
    InsertNode(m_pTail, pNode);
}


template <class type>
void
CList<type>::InsertNode
(
    type *                  pPrev, 
    type *                  pNode
)                           
{                           
    type *                  pNext   = pPrev ? pPrev->pNext : m_pHead;

    // ASSERT(!pNode->pPrev);
    // ASSERT(!pNode->pNext);

    if(pPrev)
    {
        pPrev->pNext = pNode;
    }

    if(pNext)
    {
        pNext->pPrev = pNode;
    }

    pNode->pPrev = pPrev;
    pNode->pNext = pNext;

    if(m_pHead && m_pTail)
    {
        if(m_pHead == pNext)
        {
            m_pHead = pNode;
        }

        if(m_pTail == pPrev)
        {
            m_pTail = pNode;
        }
    }
    else
    {
        // ASSERT(!m_pHead);
        // ASSERT(!m_pTail);
        
        m_pHead = m_pTail = pNode;
    }
}


template <class type>
void 
CList<type>::RemoveNode
(
    type *                  pNode
)
{
    // ASSERT(pNode);
    
    if(pNode->pPrev)
    {
        pNode->pPrev->pNext = pNode->pNext;
    }

    if(pNode->pNext)
    {
        pNode->pNext->pPrev = pNode->pPrev;
    }

    if(pNode == m_pHead)
    {
        m_pHead = pNode->pNext;
    }

    if(pNode == m_pTail)
    {
        m_pTail = pNode->pPrev;
    }

    pNode->pPrev = NULL;
    pNode->pNext = NULL;
}


template <class type>
void 
CList<type>::Reset
(
    void
)
{
    type *                  pNode   = m_pHead;
    type *                  pNext;

    while(pNode)
    {
        pNext = pNode->pNext;

        pNode->pPrev = NULL;
        pNode->pNext = NULL;

        pNode = pNext;
    }

    m_pHead = NULL;
    m_pTail = NULL;
}


template <class type>
type *
CList<type>::GetListHead
(
    void
)
{ 
    return m_pHead;
}


template <class type>
type *
CList<type>::GetListTail
(
    void
)
{ 
    return m_pTail;
}


//
// String node class
//

class CStringNode
{
public:
    CStringNode *           pPrev;
    CStringNode *           pNext;
    LPSTR                   psz;

public:
    CStringNode(void);
    ~CStringNode(void);

public:
    BOOL Init(LPCSTR psz);
};


CStringNode::CStringNode
(
    void
)
{
    pPrev = NULL;
    pNext = NULL;
    psz = NULL;
}


CStringNode::~CStringNode
(
    void
)
{
    if(psz)
    {
        LocalFree(psz);
    }
}


BOOL
CStringNode::Init
(
    LPCSTR                  pszNew
)
{
    UINT                    nStrLen;

    nStrLen = strlen(pszNew) + 1;

    if(psz = (LPSTR)LocalAlloc(LPTR, nStrLen))
    {
        memcpy(psz, pszNew, nStrLen);
    }
    
    return !!psz;
}


/****************************************************************************
 *
 *  GetPriorityString
 *
 *  Description:
 *      Gets the name for a codec priority value.
 *
 *  Arguments:
 *      DWORD [in]: priority.
 *      LPSTR [out]: priority value string.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

__inline void
GetPriorityString
(
    DWORD                   dwPriority,
    LPSTR                   pszValueName
)
{
    wsprintf(pszValueName, "Priority%lu", dwPriority + 1);
}


/****************************************************************************
 *
 *  CreateCodecNode
 *
 *  Description:
 *      Creates and initializes a codec node.
 *
 *  Arguments:
 *      LPCSTR [in]: codec priority data.
 *      CList & [in]: codec list.
 *
 *  Returns:  
 *      LONG: Win32 error code.
 *
 ****************************************************************************/

LONG
CreateCodecNode
(
    LPCSTR                  pszCodec,
    CList<CStringNode> &    lstCodecs,
    CStringNode **          ppCodecNode = NULL
)
{
    CStringNode *           pCodecNode;

    if(!(pCodecNode = new CStringNode))
    {
        return ERROR_OUTOFMEMORY;
    }

    if(!pCodecNode->Init(pszCodec))
    {
        delete pCodecNode;
        return ERROR_OUTOFMEMORY;
    }

    lstCodecs.AddNode(pCodecNode);

    if(ppCodecNode)
    {
        *ppCodecNode = pCodecNode;
    }

    return 0;
}


/****************************************************************************
 *
 *  FreeCodecNode
 *
 *  Description:
 *      Releases a codec node.
 *
 *  Arguments:
 *      CStringNode * [in]: codec node.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

void
FreeCodecNode
(
    CList<CStringNode> &    lstCodecs,
    CStringNode *           pCodecNode
)
{
    lstCodecs.RemoveNode(pCodecNode);

    delete pCodecNode;
}


/****************************************************************************
 *
 *  GetPriorityCodec
 *
 *  Description:
 *      Gets the name of a codec from the priority list and adds it to our
 *      own.
 *
 *  Arguments:
 *      HKEY [in]: registry key.
 *      DWORD [in]: priority value.
 *      CList & [in]: codec list.
 *      LPLONG [out]: status code.
 *
 *  Returns:  
 *      BOOL: TRUE if enumeration should continue.
 *
 ****************************************************************************/

BOOL
GetPriorityCodec
(
    HKEY                    hkey,
    DWORD                   dwPriority,
    CList<CStringNode> &    lstCodecs,
    LPLONG                  plStatus
)
{
    CHAR                    szValueName[0x100];
    CHAR                    szValueData[0x100];
    DWORD                   dwType;
    DWORD                   dwSize;
    LONG                    lStatus;

    GetPriorityString(dwPriority, szValueName);

    dwSize = sizeof(szValueData);
    
    if(lStatus = RegQueryValueEx(hkey, szValueName, NULL, &dwType, (LPBYTE)szValueData, &dwSize))
    {
        if(ERROR_FILE_NOT_FOUND == lStatus)
        {
            lStatus = 0;
        }
        
        *plStatus = lStatus;
        return FALSE;
    }

    if(REG_SZ != dwType)
    {
        *plStatus = ERROR_INVALID_DATA;
        return FALSE;
    }

    if(lStatus = CreateCodecNode(szValueData, lstCodecs))
    {
        *plStatus = lStatus;
        return FALSE;
    }

    *plStatus = 0;
    
    return TRUE;
}


/****************************************************************************
 *
 *  main
 *
 *  Description:
 *      Application entry point.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      INT: Win32 error code (0 on success).
 *
 ****************************************************************************/
 
INT
main
(
    void
)
{
    static const LPCSTR     pszRegKey           = "Software\\Microsoft\\Multimedia\\Audio Compression Manager\\Priority v4.00";
    static const LPCSTR     pszVoxName          = "msacm.voxacm160";
    static const LPCSTR     pszXBName           = "msacm.xbadpcm";
    static const LPCSTR     pszXBFull           = "1, msacm.xbadpcm";
    HKEY                    hkey                = 0;
    LONG                    lStatus;
    DWORD                   dwPriority;
    CList<CStringNode>      lstCodecs;
    CStringNode *           pCodecNode;
    CHAR                    szValueName[0x100];

    //
    // Open the ACM priority registry key
    //

    lStatus = RegOpenKey(HKEY_CURRENT_USER, pszRegKey, &hkey);

    //
    // Enumerate all existing priority values and add them to the codec
    // list.
    //

    for(dwPriority = 0; !lStatus; dwPriority++)
    {
        if(!GetPriorityCodec(hkey, dwPriority, lstCodecs, &lStatus))
        {
            break;
        }
    }

    //
    // Make sure the Xbox ADPCM codec is in the list
    //

    if(!lStatus)
    {
        for(pCodecNode = lstCodecs.GetListHead(); pCodecNode; pCodecNode = pCodecNode->pNext)
        {
            if(strstr(pCodecNode->psz, pszXBName))
            {
                break;
            }
        }

        if(pCodecNode)
        {
            FreeCodecNode(lstCodecs, pCodecNode);
        }

        lStatus = CreateCodecNode(pszXBFull, lstCodecs);
    }

    //
    // Look for the Voxware codec.  If we find it, move it to the end
    // of the list and rewrite the registry list.
    //

    if(!lStatus)
    {
        for(pCodecNode = lstCodecs.GetListHead(); pCodecNode; pCodecNode = pCodecNode->pNext)
        {
            if(strstr(pCodecNode->psz, pszVoxName))
            {
                break;
            }
        }

        if(pCodecNode)
        {
            lstCodecs.RemoveNode(pCodecNode);
            lstCodecs.AddNode(pCodecNode);

            for(pCodecNode = lstCodecs.GetListHead(), dwPriority = 0; pCodecNode && !lStatus; pCodecNode = pCodecNode->pNext, dwPriority++)
            {
                GetPriorityString(dwPriority, szValueName);
    
                lStatus = RegSetValueEx(hkey, szValueName, 0, REG_SZ, (LPBYTE)pCodecNode->psz, strlen(pCodecNode->psz) + 1);
            }
        }
    }

    //
    // Clean up
    //

    while(pCodecNode = lstCodecs.GetListHead())
    {
        FreeCodecNode(lstCodecs, pCodecNode);
    }

    if(hkey)
    {
        RegCloseKey(hkey);
    }

    return lStatus;
}


