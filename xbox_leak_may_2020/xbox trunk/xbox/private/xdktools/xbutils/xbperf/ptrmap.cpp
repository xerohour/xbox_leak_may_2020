/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    ptrmap.cpp

Abstract:

    This module implements class to supports maps of void pointers keyed by
    void pointers (copied from MFC).

--*/

#include "stdafx.h"

CMapPtrToPtr::CMapPtrToPtr(int nBlockSize)
{
    _ASSERTE(nBlockSize > 0);

    m_pHashTable = NULL;
    m_nHashTableSize = 17;  // default size
    m_nCount = 0;
    m_pFreeList = NULL;
    m_pBlocks = NULL;
    m_nBlockSize = nBlockSize;
}

CMapPtrToPtr::~CMapPtrToPtr()
{
    RemoveAll();
    _ASSERTE(m_nCount == 0);
}

void CMapPtrToPtr::RemoveAll()
{
    if (m_pHashTable != NULL) {
        // free hash table
        delete[] m_pHashTable;
        m_pHashTable = NULL;
    }

    m_nCount = 0;
    m_pFreeList = NULL;
    m_pBlocks->FreeDataChain();
    m_pBlocks = NULL;
}

bool CMapPtrToPtr::Lookup(void* key, void*& rValue) const
{
    UINT nHash;
    CAssoc* pAssoc = GetAssocAt(key, nHash);
    if (pAssoc == NULL) {
        return false;  // not in map
    }

    rValue = pAssoc->value;
    return true;
}

void*& CMapPtrToPtr::operator[](void* key)
{
    UINT nHash;
    CAssoc* pAssoc;

    if ((pAssoc = GetAssocAt(key, nHash)) == NULL) {

        if (m_pHashTable == NULL) {
            InitHashTable(m_nHashTableSize);
        }

        // it doesn't exist, add a new Association
        pAssoc = NewAssoc();

        pAssoc->key = key;
        // 'pAssoc->value' is a constructed object, nothing more

        // put into hash table
        pAssoc->pNext = m_pHashTable[nHash];
        m_pHashTable[nHash] = pAssoc;
    }

    return pAssoc->value;  // return new reference
}

void* CMapPtrToPtr::GetValueAt(void* key) const
{
    if (m_pHashTable == NULL) {
        return NULL;
    }

    UINT nHash = HashKey(key) % m_nHashTableSize;

    CAssoc* pAssoc;

    for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext) {
        if (pAssoc->key == key) {
            return pAssoc->value;
        }
    }

    return NULL;
}

bool CMapPtrToPtr::RemoveKey(void* key)
{
    if (m_pHashTable == NULL) {
        return false;  // nothing in the table
    }

    CAssoc** ppAssocPrev;
    ppAssocPrev = &m_pHashTable[HashKey(key) % m_nHashTableSize];

    CAssoc* pAssoc;
    for (pAssoc = *ppAssocPrev; pAssoc != NULL; pAssoc = pAssoc->pNext) {
        if (pAssoc->key == key) {
            // remove it
            *ppAssocPrev = pAssoc->pNext;  // remove from list
            FreeAssoc(pAssoc);
            return true;
        }
        ppAssocPrev = &pAssoc->pNext;
    }

    return false;  // not found
}

void CMapPtrToPtr::GetNextAssoc(POSITION& rNextPosition,
    void*& rKey, void*& rValue) const
{
    _ASSERTE(m_pHashTable != NULL);  // never call on empty map

    CAssoc* pAssocRet = (CAssoc*)rNextPosition;
    _ASSERTE(pAssocRet != NULL);

    if (pAssocRet == (CAssoc*)BEFORE_START_POSITION) {
        // find the first association
        for (UINT nBucket = 0; nBucket < m_nHashTableSize; nBucket++)
            if ((pAssocRet = m_pHashTable[nBucket]) != NULL)
                break;
        _ASSERTE(pAssocRet != NULL);  // must find something
    }

    // find next association
    CAssoc* pAssocNext;
    if ((pAssocNext = pAssocRet->pNext) == NULL) {
        // go to next bucket

        for (UINT nBucket = (HashKey(pAssocRet->key) % m_nHashTableSize) + 1;

          nBucket < m_nHashTableSize; nBucket++)
            if ((pAssocNext = m_pHashTable[nBucket]) != NULL)
                break;
    }

    rNextPosition = (POSITION) pAssocNext;

    // fill in return data
    rKey = pAssocRet->key;
    rValue = pAssocRet->value;
}

void CMapPtrToPtr::InitHashTable(UINT nHashSize, BOOL bAllocNow)
{
    _ASSERTE(m_nCount == 0);
    _ASSERTE(nHashSize > 0);

    if (m_pHashTable != NULL) {
        // free hash table
        delete[] m_pHashTable;
        m_pHashTable = NULL;
    }

    if (bAllocNow) {
        m_pHashTable = new CAssoc* [nHashSize];
        memset(m_pHashTable, 0, sizeof(CAssoc*) * nHashSize);
    }

    m_nHashTableSize = nHashSize;
}

CMapPtrToPtr::CAssoc* CMapPtrToPtr::GetAssocAt(void* key, UINT& nHash) const
{
    nHash = HashKey(key) % m_nHashTableSize;

    if (m_pHashTable == NULL)
        return NULL;

    // see if it exists
    CAssoc* pAssoc;
    for (pAssoc = m_pHashTable[nHash]; pAssoc != NULL; pAssoc = pAssoc->pNext) {
        if (pAssoc->key == key)
            return pAssoc;
    }

    return NULL;
}

CMapPtrToPtr::CAssoc* CMapPtrToPtr::NewAssoc()
{
    if (m_pFreeList == NULL) {
        // add another block
        CPlex* newBlock = CPlex::Create(m_pBlocks, m_nBlockSize, sizeof(CMapPtrToPtr::CAssoc));
        // chain them into free list
        CMapPtrToPtr::CAssoc* pAssoc = (CMapPtrToPtr::CAssoc*) newBlock->data();
        // free in reverse order to make it easier to debug
        pAssoc += m_nBlockSize - 1;
        for (int i = m_nBlockSize-1; i >= 0; i--, pAssoc--)
        {
            pAssoc->pNext = m_pFreeList;
            m_pFreeList = pAssoc;
        }
    }

    _ASSERTE(m_pFreeList != NULL);  // we must have something

    CMapPtrToPtr::CAssoc* pAssoc = m_pFreeList;
    m_pFreeList = m_pFreeList->pNext;
    m_nCount++;
    _ASSERTE(m_nCount > 0);  // make sure we don't overflow

    pAssoc->key = 0;
    pAssoc->value = 0;

    return pAssoc;
}

void CMapPtrToPtr::FreeAssoc(CMapPtrToPtr::CAssoc* pAssoc)
{
    pAssoc->pNext = m_pFreeList;
    m_pFreeList = pAssoc;
    m_nCount--;
    _ASSERTE(m_nCount >= 0);  // make sure we don't underflow

    // if no more elements, cleanup completely
    if (m_nCount == 0) {
        RemoveAll();
    }
}

