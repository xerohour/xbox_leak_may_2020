//  PACKER.H
//
//  Created 12-Mar-2001 [JonT]

#ifndef _PACKER_H
#define _PACKER_H

#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <fci.h>

#define PACKER
// stuff from common.lib (private\setup\common)
#include "..\common\global.h"
#include "..\common\settings.h"
#include "..\common\parse.h"
#include "..\common\fileinfo.h"
#include "..\common\langid.h"


//---------------------------------------------------------------------
//  Error reporting

    void _ReportError(LPCTSTR lpFile, DWORD dwLine, LPCTSTR lpszFormat, va_list vararg);
    void _ReportSettingsError(DWORD dwSetting, LPCTSTR lpszFormat, va_list vararg);

inline void
ReportError(
    LPCTSTR lpFile,
    DWORD dwLine,
    LPCTSTR lpszFormat,
    ...
    )
{
    va_list vararg;
    va_start(vararg, lpszFormat);
    _ReportError(lpFile, dwLine, lpszFormat, vararg);
    va_end(vararg);
}


inline void
ReportSettingsError(
    DWORD dwSetting,
    LPCTSTR lpszFormat,
    ...
    )
{
    va_list vararg;
    va_start(vararg, lpszFormat);
    _ReportSettingsError(dwSetting, lpszFormat, vararg);
    va_end(vararg);
}

//---------------------------------------------------------------------
//  CFileHash

typedef struct _FILENODE
{
    struct _FILENODE* pnext;
    DWORD m_dwSize;
    LPSTR m_lpName;
    LPSTR m_lpCopyTarget;
    LPSTR m_lpCopyRoot;
} FILENODE, *LPFILENODE;

#define TABLE_SIZE 4001     // prime number

class CFileHash
{
private:
    LPFILENODE m_lhHashTable[TABLE_SIZE];
    DWORD HashSizeToIndex(DWORD dwSize);
    BOOL FileCompare(LPCSTR lpFile1, LPCSTR lpFile2);

public:
    CFileHash();
    LPSTR FindDuplicate(LPCTSTR lpName, LPCTSTR lpCopyRoot, LPCTSTR lpCopyTarget);
};


//---------------------------------------------------------------------
//  CCabFile
//      Manages the FCI (File Compression Interface) and provides an interface
//      to add files to the cabinet.

class CCabFile
{
private:
    HFCI m_hfci;
    ERF m_erf;
    CCAB m_cab;
    BOOL m_fTestOnly;
    DWORD m_dwCompression;

public:
    CCabFile();
    ~CCabFile();

    // Point instance at a particular cab file
    BOOL SetFile(LPCTSTR lpCabFile, BOOL fTestOnly);

    // Sets the type and level of compression to be used
    BOOL SetCompression(LPCSTR lpType, LPCSTR lpLevel);

    // Add a file to the cabinet
    BOOL AddFile(LPCTSTR lpSource, LPCTSTR lpDest);

    // Write the cabinet out to disk
    BOOL Flush();

    // Check error status after failed call
    PERF GetError() { return &m_erf; }

    // Sets the mode to test-only so that nothing is really done but validates
    // all files anyway.
    void SetTestOnly();
};

inline
CCabFile::CCabFile()
{
    m_hfci = NULL;
    memset(&m_erf, sizeof (ERF), 1);
    m_fTestOnly = FALSE;
}

inline void
CCabFile::SetTestOnly()
{
    m_fTestOnly = TRUE;
}

//---------------------------------------------------------------------
//  CManifest
//      Handles the manifest file.
//      The manifest is the output file from the packer that describes
//      all files in the cabinet.

class CManifest
{
private:
    FILE* m_handle;
    DWORD m_error;

public:
    CManifest()
    {
        m_handle = NULL;
    }
    ~CManifest();

    BOOL SetFile(LPTSTR lpManifest);
    BOOL WriteCommand(CLangID* pLangIDs, LPCTSTR lpCommand, ...);
    void Close();
    DWORD GetError() { return m_error; }
};


//---------------------------------------------------------------------
//  CExeResource

#define RES_MAX_FILES 5

class CExeResource
{
private:
    HANDLE m_handle;
    DWORD m_error;
    DWORD m_dwSizes[RES_MAX_FILES];
    DWORD m_dwcFiles;
    DWORD m_dwTotal;
    DWORD m_dwCurrent;

public:
    CExeResource() { m_handle = INVALID_HANDLE_VALUE; m_dwcFiles = 0; }
    ~CExeResource() { Close(); }

    BOOL SetFile(LPCTSTR lpTemplateExe, LPCTSTR lpNewExe);
    void Close();
    VOID SetTotalSize(DWORD dwTotal) { m_dwCurrent = 0; m_dwTotal = dwTotal; }
    BOOL AddResourceFromFile(LPCTSTR lpFile);
    DWORD GetError() { return m_error; }
};


//---------------------------------------------------------------------
//  CDirectories
//      Manages a list of directories encountered in the settings file

#define MAX_ID 64

typedef struct _DIRINFO
{
    char szID[MAX_ID];
    DWORD dwSize;
} DIRINFO, *LPDIRINFO;

class CDirectories
{
private:
    DWORD m_dwcDirsMax;
    DWORD m_dwcDirsNow;
    LPDIRINFO m_lpdirs;

public:
    CDirectories() { m_dwcDirsMax = m_dwcDirsNow = 0; m_lpdirs = NULL; }
    ~CDirectories() { if (m_lpdirs) free(m_lpdirs); }

    BOOL Init(DWORD dwc);
    BOOL Add(LPSTR lpID);
    BOOL AddSize(LPCSTR lpID, LPCSTR lpFile);
    BOOL AddSize(LPCSTR lpID, DWORD dwc);
    LPDIRINFO Find(LPCSTR lpID);
    DWORD GetCount() { return m_dwcDirsNow; }
    LPDIRINFO GetAll() { return m_lpdirs; }
};


//---------------------------------------------------------------------
//  CPackingList
//      Manages the packing list file and allows the program to walk through
//      the list.

typedef struct _SOURCENODE
{
    struct _SOURCENODE* pNext;
    LPSTR lpID;
    int nSize;
    LPSTR lpRoot;
    LPSTR lpSource;
    LPSTR lpName;
    char bFileInfo[FILEINFO_SAVE_SIZE];
} SOURCENODE, *LPSOURCENODE;

class CPackingList;

typedef BOOL (CPackingList::*CMDPFN)();

typedef struct _COMMAND
{
    LPSTR lpText;
    CMDPFN lpHandler;
} COMMAND;

class CPackingList
{
private:
    DWORD m_error;
    LPSTR m_lpEntireFile;
    LPSTR m_lpLine;
    LPCSTR m_lpCommand;
    CLangID m_LangIDs;
    TCHAR m_szPackingList[MAX_PATH];
    DWORD m_dwBuildFlavor;
    LPSOURCENODE m_psources;
    DWORD m_dwcErrors;
    CFileHash m_filehash;
    BOOL m_fTestOnly;
    FILETIME m_ftUpdateTo;
    BOOL m_fDateUpdate;
    CSettingsFile* m_psettings;
    CManifest* m_pmanifest;
    CCabFile* m_pcabfile;
    CDirectories* m_pdirs;

    void ReportError(LPCTSTR lpError, ...);
    void _ReportError(LPCTSTR lpszFormat, va_list vararg);
    LPCSTR GetCommand();
    BOOL DispatchCommand();
    DWORD GetCommandLangID();
    DWORD GetBuildFlavor();
    DWORD AccumulateFlavor(LPCSTR lp);

    // Packing list command handlers
    BOOL ProcessFile();
    BOOL ProcessCopySource();
    BOOL ProcessCopy();
    BOOL ProcessShortcut();
    BOOL ProcessRemove();
    BOOL ProcessRegCommands();
    static const COMMAND m_CommandList[];

public:
    CPackingList()
    {
        m_lpEntireFile = m_lpLine = NULL;
        m_dwBuildFlavor = 0;
        m_psources = NULL;
        m_dwcErrors = 0;
        m_fDateUpdate = FALSE;
    }
    ~CPackingList();

    BOOL SetFile(CSettingsFile* psettings, LPCTSTR lpPackingList, BOOL fTestOnly);
    void SetBuildFlavor(LPCSTR lpFlavor);
    BOOL ProcessLines(CManifest* pmanifest, CCabFile* pcabfiles, CDirectories* pdirs);
    DWORD GetError() { return m_error; }
};


inline void
CPackingList::ReportError(
    LPCTSTR lpszFormat,
    ...
    )
{
    va_list vararg;
    va_start(vararg, lpszFormat);
    _ReportError(lpszFormat, vararg);
    va_end(vararg);
}


//---------------------------------------------------------------------
//  CPacker
//      Application class

class CPacker
{
private:
    LPSTR m_lpLaunchName;
    LPTSTR m_lpCabPath;
    LPTSTR m_lpManifestName;
    TCHAR m_szFileCab[MAX_PATH];
    TCHAR m_szSettingsCab[MAX_PATH];
    BOOL m_fTestPack;
    BOOL m_fNoLogo;
    DWORD m_dwTotalSize;

    CSettingsFile m_settings;
    CCabFile m_cabFiles;
    CCabFile m_cabSettings;
    CPackingList m_plist;
    CManifest m_manifest;
    CDirectories m_dirs;

    BOOL WriteSelfExtractingExe();
    BOOL CreateSettingsCab();
    BOOL ProcessFiles();
    BOOL OpenFiles();
    LPCTSTR CPacker::AddCommas(__int64 li);

    static TCHAR m_szIniName[MAX_PATH];

public:
    CPacker();

    BOOL ParseCommandLine(int argc, char** argv);
    void PrintUsage();
    int Process();
    friend void _ReportSettingsError(DWORD dwSetting, LPCTSTR lpszFormat, va_list vararg);
};


inline
CPacker::CPacker()
{
    m_lpLaunchName = NULL;
    m_fTestPack = FALSE;
    m_fNoLogo = FALSE;
}

#endif // #ifndef _PACKER_H
