#pragma once

class CNode;

struct CDefine
{
    CDefine* m_pNext;
    CNode* m_pNode;
    TCHAR m_szName[ANYSIZE_ARRAY];

#pragma push_macro("new")
#undef new
    void * operator new(size_t sz, int cchName)
    {
        return ::operator new(sz + cchName * sizeof(TCHAR));
    }
#pragma pop_macro("new")
};

struct CNameSpace
{
    CNameSpace();
    ~CNameSpace();

    bool Define(const TCHAR* pchName, int cchName, CNode* pNode);
    CNode* Lookup(const TCHAR* pchName, int cchName);
    CDefine* Lookup(CNode* pNode);
    CDefine* Get(const TCHAR* pchName, int cchName);
    CDefine* Add(const TCHAR* pchName, int cchName);

    CDefine* m_pFirstDefine;

#ifdef _DEBUG
    void Dump() const;
#endif
};
