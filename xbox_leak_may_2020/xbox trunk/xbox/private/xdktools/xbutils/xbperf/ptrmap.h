/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    ptrmap.h

Abstract:

    This module implements class to supports maps of void pointers keyed by
    void pointers.

--*/

#if !defined(_PTRMAP_INCLUDED)
#define _PTRMAP_INCLUDED

struct __POSITION { };
typedef __POSITION* POSITION;

#define BEFORE_START_POSITION ((POSITION)-1L)

struct CPlex {      // warning variable length structure
    CPlex* pNext;
    void* data() { return this+1; }

    static CPlex* PASCAL Create(CPlex*& pHead, UINT nMax, UINT cbElement) {
        _ASSERTE(nMax > 0 && cbElement > 0);
        CPlex* p = (CPlex*)new BYTE[sizeof(CPlex) + nMax * cbElement];
        p->pNext = pHead;
        pHead = p;  // change head (adds in reverse order for simplicity)
        return p;
    }

    void FreeDataChain() {
        CPlex* p = this;
        while (p) {
            BYTE* bytes = (BYTE*)p;
            CPlex* pNext = p->pNext;
            delete[] bytes;
            p = pNext;
        }
    }
};

class CMapPtrToPtr {
protected:
    struct CAssoc {
        CAssoc* pNext;
        void* key;
        void* value;
    };

public:
    CMapPtrToPtr(int nBlockSize = 10);
    virtual ~CMapPtrToPtr();

    int GetCount() const { return m_nCount; }
    bool IsEmpty() const { return m_nCount == 0; }

    bool Lookup(void* key, void*& rValue) const;

    void*& operator[](void* key);

    void SetAt(void* key, void* newValue) { (*this)[key] = newValue; }
    void* GetValueAt(void* key) const;

    bool RemoveKey(void* key);
    void RemoveAll();

    POSITION GetStartPosition() const { return (m_nCount == 0) ? NULL : BEFORE_START_POSITION; }
    void GetNextAssoc(POSITION& rNextPosition, void*& rKey, void*& rValue) const;

    // advanced features for derived classes
    UINT GetHashTableSize() const { return m_nHashTableSize; }
    void InitHashTable(UINT hashSize, BOOL bAllocNow = TRUE);

    // Routine used to user-provided hash keys
    UINT HashKey(void* key) const { return ((UINT)(void*)(DWORD)key) >> 4; }

protected:
    CAssoc** m_pHashTable;
    UINT m_nHashTableSize;
    int m_nCount;
    CAssoc* m_pFreeList;
    struct CPlex* m_pBlocks;
    int m_nBlockSize;

    CAssoc* NewAssoc();
    void FreeAssoc(CAssoc*);
    CAssoc* GetAssocAt(void*, UINT&) const;
};

#endif // !defined(_PTRMAP_INCLUDED)

