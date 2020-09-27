/***************************************************************************
 *
 *  Copyright (C) 02/13/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       loader.h
 *  Content:    Wave bank project loader.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  02/13/2002  dereks  Created.
 *
 ****************************************************************************/

#ifndef __LOADER_H__
#define __LOADER_H__

//
// File extensions
//

#define WBWRITER_FILEEXTENSION_PROJECT          TEXT("xwp")
#define WBWRITER_FILEEXTENSION_BANK             TEXT("xwb")
#define WBWRITER_FILEEXTENSION_HEADER           TEXT("h")
                                                
//                                              
// Project file format                          
//                                              
                                                
#define WBPROJECT_PROJECT_MAIN_SECTION          TEXT("PROJECT")
#define WBPROJECT_PROJECT_BANKCOUNT_KEY         TEXT("BANKCOUNT")
                                                
#define WBPROJECT_BANK_KEY                      TEXT("BANK%lu")
#define WBPROJECT_BANK_NAME_KEY                 TEXT("BANKNAME")
#define WBPROJECT_BANK_FILE_KEY                 TEXT("BANKFILE")
#define WBPROJECT_BANK_HEADER_KEY               TEXT("HEADERFILE")
#define WBPROJECT_BANK_ENTRYCOUNT_KEY           TEXT("ENTRYCOUNT")
                                                
#define WBPROJECT_ENTRY_KEY                     TEXT("ENTRY%lu")
                                                
#define WBPROJECT_ENTRYEXT_SEPARATOR_STR        TEXT(",")
#define WBPROJECT_ENTRYEXT_SEPARATOR_LCHAR      TEXT(',')

#define WBPROJECT_ENTRYEXT_ADPCMFILTER_STR      TEXT("c")
#define WBPROJECT_ENTRYEXT_ADPCMFILTER_LCHAR    TEXT('c')
                                                
#define WBPROJECT_ENTRYEXT_8BITFILTER_STR       TEXT("8")
#define WBPROJECT_ENTRYEXT_8BITFILTER_LCHAR     TEXT('8')

#ifdef __cplusplus

//
// Forward declarations
//

class CWaveBankProject;
class CWaveBank;
class CWaveBankEntry;

//
// Project loader
//

class CWaveBankProjectFile
{
protected:
    CWaveBankProject *      m_pProject;                 // Wave bank project object
    TCHAR                   m_szProjectFile[MAX_PATH];  // Project file path

public:
    // IO
    virtual HRESULT Load(CWaveBankProject *pProject, LPCTSTR pszProjectFile);
    virtual HRESULT Save(CWaveBankProject *pProject, LPCTSTR pszProjectFile);

protected:
    virtual HRESULT LoadBank(DWORD dwBankIndex);
    virtual HRESULT SaveBank(CWaveBank *pBank, DWORD dwBankIndex);

    virtual HRESULT LoadEntry(LPCTSTR pszSectionName, CWaveBank *pBank, DWORD dwEntryIndex);
    virtual HRESULT SaveEntry(LPCTSTR pszSectionName, CWaveBankEntry *pEntry, DWORD dwEntryIndex);
};

#endif // __cplusplus

#endif // __LOADER_H__
