/***************************************************************************
 *
 *  Copyright (C) 11/2/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       writer.h
 *  Content:    Wave bank writer.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  11/2/2001   dereks  Created.
 *
 ****************************************************************************/

#ifndef __WRITER_H__
#define __WRITER_H__

//
// C header file format
//

#define WBCHEADER_ENUMNAME                      TEXT("WAVEBANK_%s")
#define WBCHEADER_ENUMENTRYNAME                 WBCHEADER_ENUMNAME TEXT("_%s")
#define WBCHEADER_ENTRYCOUNTSHORTNAME           TEXT("ENTRY_COUNT")
#define WBCHEADER_ENTRYCOUNTNAME                WBCHEADER_ENUMNAME TEXT("_") WBCHEADER_ENTRYCOUNTSHORTNAME

#ifdef __cplusplus

//
// Forward declarations
//

class CWaveBank;
class CWaveBankProject;
class CWaveBankCallback;

//
// Wave bank entry
//

class CWaveBankEntry
{
public:
    static BOOL             m_fAutoLoad;            // Automatically load wave file at init?

public:
    CWaveBank *             m_pParent;              // Parent object
    LIST_ENTRY              m_leEntry;              // Parent list entry
    TCHAR                   m_szEntryName[0x100];   // Entry name
    TCHAR                   m_szHeaderName[0x100];  // Entry name (header version)
    TCHAR                   m_szFileName[MAX_PATH]; // Source file path
    WAVEBANKMINIWAVEFORMAT  m_Format;               // Format
    WAVEBANKENTRYREGION     m_PlayRegion;           // Wave data file offset/length
    WAVEBANKENTRYREGION     m_LoopRegion;           // Loop region
    DWORD                   m_dwFlags;              // Flags
    DWORD                   m_dwFileType;           // File type
    BOOL                    m_fLoaded;              // Has the wave file been parsed?
    DWORD                   m_dwDestLength;         // File size when stored in the wave bank

public:
    CWaveBankEntry(CWaveBank *pParent);
    virtual ~CWaveBankEntry(void);

public:
    // Initialization
    virtual HRESULT Initialize(LPCTSTR pszEntryName, LPCTSTR pszFileName, DWORD dwFlags);
    virtual HRESULT Load(BOOL fForce = FALSE);

    // Entry properties
    virtual HRESULT SetName(LPCTSTR pszName);
    virtual DWORD GetValidFlags(void);
    virtual DWORD SetFlags(DWORD dwMask, DWORD dwFlags);

    // Entry data
    virtual HRESULT GetMetaData(LPWAVEBANKENTRY pMetaData, DWORD dwBaseOffset, DWORD dwAlignment, LPDWORD pdwOffset);
    virtual HRESULT CommitWaveData(CWaveBankCallback *pCallback, CStdFileStream *pBankFile, DWORD dwBaseOffset, LPCWAVEBANKENTRY pMetaData, LPVOID pvCopyBuffer, DWORD dwCopyBufferSize);

protected:
    // Validation
    virtual HRESULT FindDuplicateEntry(LPCTSTR pszName, UINT *pnNameCount, LPCTSTR pszPath, UINT *pnPathCount);
};

//
// Wave bank
//

class CWaveBank
{
public:
    static const DWORD      m_dwMinAlignment;                               // Minimum entry alignment

public:
    CWaveBankProject *      m_pParent;                                      // Parent object
    LIST_ENTRY              m_leBank;                                       // Wave bank list entry
    TCHAR                   m_szBankName[WAVEBANKHEADER_BANKNAME_LENGTH];   // Wave bank name
    TCHAR                   m_szHeaderName[WAVEBANKHEADER_BANKNAME_LENGTH]; // Wave bank name (header version)
    TCHAR                   m_szBankFile[MAX_PATH];                         // Wave bank file name
    TCHAR                   m_szHeaderFile[MAX_PATH];                       // Wave bank header file name
    LIST_ENTRY              m_lstEntries;                                   // Bank entry list
    DWORD                   m_dwEntryCount;                                 // Wave bank entry count
    DWORD                   m_dwFlags;                                      // Wave bank flags
    DWORD                   m_dwAlignment;                                  // Entry alignment

public:
    CWaveBank(CWaveBankProject *pParent = NULL);
    virtual ~CWaveBank(void);

public:
    // Initialization
    virtual HRESULT Initialize(LPCTSTR pszBankName, LPCTSTR pszBankFile, LPCTSTR pszHeaderFile);

    // Bank properties
    virtual HRESULT SetName(LPCTSTR pszName);
    virtual HRESULT SetAlignment(DWORD dwAlignment);
    virtual DWORD GetBankDataSize(void);

    // Entries
    virtual HRESULT AddEntry(LPCTSTR pszEntryName, LPCTSTR pszFileName, DWORD dwFlags, CWaveBankEntry **ppEntry = NULL);
    virtual void RemoveEntry(CWaveBankEntry *pEntry);
    virtual HRESULT LoadEntries(BOOL fForce = FALSE);

    // File creation
    virtual HRESULT GenerateBank(CWaveBankCallback *pCallback, BOOL fAllowOverwrite);
    virtual HRESULT GenerateHeader(CWaveBankCallback *pCallback, BOOL fAllowOverwrite);

protected:
    // Entry creation
    virtual CWaveBankEntry *CreateEntry(void);

    // File creation
    virtual DWORD GetBankHeaderSize(void);
    static DWORD GetBankHeaderSize(DWORD dwEntryCount);

    // Validation
    virtual HRESULT FindDuplicateBank(LPCTSTR pszName, UINT *pnNameCount, LPCTSTR pszBankFile, UINT *pnBankFile, LPCTSTR pszHeaderFile, UINT *pnHeaderFile);
};

__inline void CWaveBank::RemoveEntry(CWaveBankEntry *pEntry)
{
    DELETE(pEntry);
}

__inline DWORD CWaveBank::GetBankHeaderSize(DWORD dwEntryCount)
{
    return sizeof(WAVEBANKHEADER) + (sizeof(WAVEBANKENTRY) * dwEntryCount);
}

__inline DWORD CWaveBank::GetBankHeaderSize(void)
{
    return GetBankHeaderSize(m_dwEntryCount);
}

__inline CWaveBankEntry *CWaveBank::CreateEntry(void)
{
    return NEW(CWaveBankEntry(this));
}

//
// Wave bank project
//

class CWaveBankProject
{
public:
    LIST_ENTRY              m_lstBanks;                 // Wave bank list
    DWORD                   m_dwBankCount;              // Bank count
    DWORD                   m_dwEntryCount;             // Total entry count

public:
    CWaveBankProject(void);
    virtual ~CWaveBankProject(void);

public:
    // Banks
    virtual HRESULT AddBank(LPCTSTR pszBankName, LPCTSTR pszBankFile, LPCTSTR pszHeaderFile, CWaveBank **ppBank);
    virtual void RemoveBank(CWaveBank *pBank);
    
    // File creation
    virtual HRESULT Generate(CWaveBankCallback *pCallback, BOOL fAllowOverwrite);

protected:    
    // Bank creation
    virtual CWaveBank *CreateBank(void);
};

__inline void CWaveBankProject::RemoveBank(CWaveBank *pBank)
{
    DELETE(pBank);
}

__inline CWaveBank *CWaveBankProject::CreateBank(void)
{
    return NEW(CWaveBank(this));
}

//
// Wave bank status callback
//

class CWaveBankCallback
{
public:
    virtual BOOL BeginProject(CWaveBankProject *pProject);
    virtual void EndProject(CWaveBankProject *pProject, HRESULT hr);
    virtual BOOL OpenBank(CWaveBank *pBank);
    virtual void CloseBank(CWaveBank *pBank);
    virtual BOOL OpenHeader(CWaveBank *pBank);
    virtual void CloseHeader(CWaveBank *pBank);
    virtual BOOL BeginEntry(CWaveBankEntry *pEntry);
    virtual BOOL ProcessEntry(CWaveBankEntry *pEntry, DWORD dwProcessed);
    virtual void EndEntry(CWaveBankEntry *pEntry);
};

__inline BOOL CWaveBankCallback::BeginProject(CWaveBankProject *pProject)
{
    return TRUE;
}

__inline void CWaveBankCallback::EndProject(CWaveBankProject *pProject, HRESULT hr)
{
}

__inline BOOL CWaveBankCallback::OpenBank(CWaveBank *pBank)
{
    return TRUE;
}

__inline void CWaveBankCallback::CloseBank(CWaveBank *pBank)
{
}

__inline BOOL CWaveBankCallback::OpenHeader(CWaveBank *pBank)
{
    return TRUE;
}

__inline void CWaveBankCallback::CloseHeader(CWaveBank *pBank)
{
}

__inline BOOL CWaveBankCallback::BeginEntry(CWaveBankEntry *pEntry)
{
    return TRUE;
}

__inline BOOL CWaveBankCallback::ProcessEntry(CWaveBankEntry *pEntry, DWORD dwProcessed)
{
    return TRUE;
}

__inline void CWaveBankCallback::EndEntry(CWaveBankEntry *pEntry)
{
}

#endif // __cplusplus

#endif // __WRITER_H__
