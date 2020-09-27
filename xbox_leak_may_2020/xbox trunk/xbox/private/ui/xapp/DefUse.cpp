#include "std.h"
#include "xapp.h"
#include "DefUse.h"
#include "Node.h"

CNameSpace::CNameSpace()
{
    m_pFirstDefine = NULL;
}

CNameSpace::~CNameSpace()
{
    CDefine* pNextDef;
    for (CDefine* pDef = m_pFirstDefine; pDef != NULL; pDef = pNextDef)
    {
        pDef->m_pNode->Release();
        pNextDef = pDef->m_pNext;
        delete pDef;
    }
}

bool CNameSpace::Define(const TCHAR* pchName, int cchName, CNode* pNode)
{
    if (Lookup(pchName, cchName) != NULL)
        return FALSE;

#pragma push_macro("new")
#undef new
    CDefine* pDefine = new(cchName) CDefine;
#pragma pop_macro("new")

    CopyChars(pDefine->m_szName, pchName, cchName);
    pDefine->m_szName[cchName] = '\0';

    pDefine->m_pNext = m_pFirstDefine;
    m_pFirstDefine = pDefine;

    pDefine->m_pNode = pNode;

    return true;
}

CDefine* CNameSpace::Add(const TCHAR* pchName, int cchName)
{
#pragma push_macro("new")
#undef new
    CDefine* pDefine = new(cchName) CDefine;
#pragma pop_macro("new")

    CopyChars(pDefine->m_szName, pchName, cchName);
    pDefine->m_szName[cchName] = '\0';

    pDefine->m_pNext = m_pFirstDefine;
    m_pFirstDefine = pDefine;

    pDefine->m_pNode = NULL;

    return pDefine;
}

CDefine* CNameSpace::Get(const TCHAR* pchName, int cchName)
{
    for (CDefine* pDefine = m_pFirstDefine; pDefine != NULL; pDefine = pDefine->m_pNext)
    {
        if ((int)_tcslen(pDefine->m_szName) == cchName && _tcsncmp(pchName, pDefine->m_szName, cchName) == 0)
            return pDefine;
    }

    return Add(pchName, cchName);
}

CNode* CNameSpace::Lookup(const TCHAR* pchName, int cchName)
{
    for (CDefine* pDefine = m_pFirstDefine; pDefine != NULL; pDefine = pDefine->m_pNext)
    {
        if ((int)_tcslen(pDefine->m_szName) == cchName && _tcsncmp(pchName, pDefine->m_szName, cchName) == 0)
            return pDefine->m_pNode;
    }

    return NULL;
}

CDefine* CNameSpace::Lookup(CNode* pNode)
{
    for (CDefine* pDefine = m_pFirstDefine; pDefine != NULL; pDefine = pDefine->m_pNext)
    {
        if (pNode == pDefine->m_pNode)
            return pDefine;
    }

    return NULL;
}

#ifdef _DEBUG
void CNameSpace::Dump() const
{
    for (CDefine* pDefine = m_pFirstDefine; pDefine != NULL; pDefine = pDefine->m_pNext)
        TRACE(_T("%s -> 0x%08x\n"), pDefine->m_szName, pDefine->m_pNode);
}
#endif
