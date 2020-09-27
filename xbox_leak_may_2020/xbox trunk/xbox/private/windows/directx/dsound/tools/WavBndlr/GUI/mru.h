/***************************************************************************
 *
 *  Copyright (C) 11/26/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       mru.h
 *  Content:    Most-recently-used list.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/26/2001   dereks  Created.
 *
 ****************************************************************************/

#ifndef __MRU_H__
#define __MRU_H__

#define MRU_MAX_COUNT   4
#define MRU_KEY         "MRU"

#ifdef __cplusplus

//
// MRU list
//

class CMRU
{
public:
    LPCSTR                  m_pszParentKey;                         // Parent registry key
    CHAR                    m_aszPaths[MRU_MAX_COUNT][MAX_PATH];    // MRU file array
    HMENU                   m_hmenu;                                // MRU menu handle
    UINT                    m_nCommandId;                           // Base MRU item command identifier

public:
    CMRU(void);
    virtual ~CMRU(void);

public:
    // Initialization
    virtual void Initialize(LPCSTR pszParentKey, HMENU hMenu, UINT nCommandId);

    // MRU data
    virtual void Load(LPCSTR pszParentKey = NULL);
    virtual void Save(LPCSTR pszParentKey = NULL);
    virtual void AddItem(LPCSTR pszPath);
    
    // UI helpers
    virtual void Apply(void);
};

#endif // __cplusplus

#endif // __MRU_H__
