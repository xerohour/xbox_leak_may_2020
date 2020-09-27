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
// File extensions
//

#define WBWRITER_FILEEXTENSION_PROJECT          "xwp"
#define WBWRITER_FILEEXTENSION_BANK             "xwb"
#define WBWRITER_FILEEXTENSION_HEADER           "h"
                                                
//                                              
// Project file format                          
//                                              
                                                
#define WBPROJECT_PROJECT_MAIN_SECTION          "PROJECT"
#define WBPROJECT_PROJECT_BANKCOUNT_KEY         "BANKCOUNT"
                                                
#define WBPROJECT_BANK_KEY                      "BANK%lu"
#define WBPROJECT_BANK_NAME_KEY                 "BANKNAME"
#define WBPROJECT_BANK_FILE_KEY                 "BANKFILE"
#define WBPROJECT_BANK_HEADER_KEY               "HEADERFILE"
#define WBPROJECT_BANK_ENTRYCOUNT_KEY           "ENTRYCOUNT"
                                                
#define WBPROJECT_ENTRY_KEY                     "ENTRY%lu"
                                                
#define WBPROJECT_ENTRYEXT_SEPARATOR_STR        ","
#define WBPROJECT_ENTRYEXT_SEPARATOR_LCHAR      ','

#define WBPROJECT_ENTRYEXT_ADPCMFILTER_STR      "c"
#define WBPROJECT_ENTRYEXT_ADPCMFILTER_LCHAR    'c'
                                                
#define WBPROJECT_ENTRYEXT_8BITFILTER_STR       "8"
#define WBPROJECT_ENTRYEXT_8BITFILTER_LCHAR     '8'

//
// C header file format
//

#define WBCHEADER_ENUMNAME                      "WAVEBANK_%s"
#define WBCHEADER_ENUMENTRYNAME                 WBCHEADER_ENUMNAME "_%s"
#define WBCHEADER_ENTRYCOUNTSHORTNAME           "ENTRY_COUNT"
#define WBCHEADER_ENTRYCOUNTNAME                WBCHEADER_ENUMNAME "_" WBCHEADER_ENTRYCOUNTSHORTNAME

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
    static const DWORD      m_dwDataAlignment;      // Entry data file alignment

public:
    CWaveBank *             m_pParent;              // Parent object
    LIST_ENTRY              m_leEntry;              // Parent list entry
    CHAR                    m_szEntryName[0x100];   // Entry name
    CHAR                    m_szHeaderName[0x100];  // Entry name (header version)
    CHAR                    m_szFileName[MAX_PATH]; // Source file path
    WAVEBANKMINIWAVEFORMAT  m_Format;               // Format
    WAVEBANKENTRYREGION     m_PlayRegion;           // Wave data file offset/length
    WAVEBANKENTRYREGION     m_LoopRegion;           // Loop region
    DWORD                   m_dwFlags;              // Flags
    DWORD                   m_dwFileType;           // File type

public:
    CWaveBankEntry(CWaveBank *pParent);
    virtual ~CWaveBankEntry(void);

public:
    // Initialization
    virtual HRESULT Initialize(LPCSTR pszEntryName, LPCSTR pszFileName, DWORD dwFlags);

    // Entry properties
    virtual HRESULT SetName(LPCSTR pszName);
    virtual DWORD GetValidFlags(void);
    virtual DWORD SetFlags(DWORD dwMask, DWORD dwFlags);

    // Entry data
    virtual HRESULT GetMetaData(LPWAVEBANKENTRY pMetaData, LPDWORD pdwOffset);
    virtual HRESULT CommitWaveData(CStdFileStream *pBankFile, DWORD dwBaseOffset, LPCWAVEBANKENTRY pMetaData, LPVOID pvCopyBuffer, DWORD dwCopyBufferSize);

    // Project data
    virtual HRESULT ReadProjectData(LPCSTR pszProjectFile, LPCSTR pszSectionName, DWORD dwEntryIndex);
    virtual HRESULT WriteProjectData(LPCSTR pszProjectFile, LPCSTR pszSectionName, DWORD dwEntryIndex);

protected:
    // Initialization
    virtual HRESULT LoadWaveFile(void);

    // Validation
    virtual HRESULT FindDuplicateEntry(LPCSTR pszName, UINT *pnNameCount, LPCSTR pszPath, UINT *pnPathCount);
};

//
// Wave bank
//

class CWaveBank
{
public:
    CWaveBankProject *      m_pParent;                                  // Parent object
    LIST_ENTRY              m_leBank;                                   // Wave bank list entry
    CHAR                    m_szBankName[16];                           // Wave bank name
    CHAR                    m_szHeaderName[16];                         // Wave bank name (header version)
    CHAR                    m_szBankFile[MAX_PATH];                     // Wave bank file name
    CHAR                    m_szHeaderFile[MAX_PATH];                   // Wave bank header file name
    LIST_ENTRY              m_lstEntries;                               // Bank entry list
    DWORD                   m_dwEntryCount;                             // Wave bank entry count
    DWORD                   m_dwFlags;                                  // Wave bank flags

public:
    CWaveBank(CWaveBankProject *pParent = NULL);
    virtual ~CWaveBank(void);

public:
    // Initialization
    virtual HRESULT Initialize(LPCSTR pszBankName, LPCSTR pszBankFile, LPCSTR pszHeaderFile);

    // Bank properties
    virtual HRESULT SetName(LPCSTR pszName);
    virtual DWORD GetBankDataSize(void);

    // Entries
    virtual HRESULT AddEntry(LPCSTR pszEntryName, LPCSTR pszFileName, DWORD dwFlags, CWaveBankEntry **ppEntry = NULL);
    virtual void RemoveEntry(CWaveBankEntry *pEntry);

    // File creation
    virtual HRESULT GenerateBank(CWaveBankCallback *pCallback, BOOL fAllowOverwrite);
    virtual HRESULT GenerateHeader(CWaveBankCallback *pCallback, BOOL fAllowOverwrite);

    // Project data
    virtual HRESULT ReadProjectData(LPCSTR pszProjectFile, DWORD dwEntryIndex);
    virtual HRESULT WriteProjectData(LPCSTR pszProjectFile, DWORD dwEntryIndex);

protected:
    // Entry creation
    virtual CWaveBankEntry *CreateEntry(void);

    // Validation
    virtual HRESULT FindDuplicateBank(LPCSTR pszName, UINT *pnNameCount, LPCSTR pszBankFile, UINT *pnBankFile, LPCSTR pszHeaderFile, UINT *pnHeaderFile);
};

__inline void CWaveBank::RemoveEntry(CWaveBankEntry *pEntry)
{
    DELETE(pEntry);
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

public:
    CWaveBankProject(void);
    virtual ~CWaveBankProject(void);

public:
    // Banks
    virtual HRESULT AddBank(LPCSTR pszBankName, LPCSTR pszBankFile, LPCSTR pszHeaderFile, CWaveBank **ppBank);
    virtual void RemoveBank(CWaveBank *pBank);
    
    // File creation
    virtual HRESULT Generate(CWaveBankCallback *pCallback, BOOL fAllowOverwrite);

    // Project data
    virtual HRESULT ReadProjectData(LPCSTR pszProjectFile);
    virtual HRESULT WriteProjectData(LPCSTR pszProjectFile);

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
    virtual BOOL BeginProject(UINT nBankCount, UINT nEntryCount);
    virtual void EndProject(HRESULT hr);
    virtual BOOL OpenBank(LPCSTR pszFile);
    virtual void CloseBank(LPCSTR pszFile);
    virtual BOOL OpenHeader(LPCSTR pszFile);
    virtual void CloseHeader(LPCSTR pszFile);
    virtual BOOL BeginEntry(LPCSTR pszFile, DWORD dwFlags);
    virtual void EndEntry(LPCSTR pszFile, DWORD dwFlags);
};

__inline BOOL CWaveBankCallback::BeginProject(UINT nBankCount, UINT nEntryCount)
{
    return TRUE;
}

__inline void CWaveBankCallback::EndProject(HRESULT hr)
{
}

__inline BOOL CWaveBankCallback::OpenBank(LPCSTR pszFile)
{
    return TRUE;
}

__inline void CWaveBankCallback::CloseBank(LPCSTR pszFile)
{
}

__inline BOOL CWaveBankCallback::OpenHeader(LPCSTR pszFile)
{
    return TRUE;
}

__inline void CWaveBankCallback::CloseHeader(LPCSTR pszFile)
{
}

__inline BOOL CWaveBankCallback::BeginEntry(LPCSTR pszFile, DWORD dwFlags)
{
    return TRUE;
}

__inline void CWaveBankCallback::EndEntry(LPCSTR pszFile, DWORD dwFlags)
{
}

#endif // __cplusplus

#endif // __WRITER_H__
