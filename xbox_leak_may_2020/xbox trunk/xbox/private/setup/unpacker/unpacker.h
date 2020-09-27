//  UNPACKER.H
//
//  Created 13-Mar-2001 [JonT]

#ifndef _UNPACKER_H
#define _UNPACKER_H

#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <malloc.h>
#include <fdi.h>
#include <shellapi.h>
#include <xboxverp.h>

#define UNPACKER
// stuff from common.lib (private\setup\common)
#include "global.h"
#include "settings.h"
#include "parse.h"
#include "fileinfo.h"
#include "helper.h"
#include "langid.h"

//---------------------------------------------------------------------
//  Custom setup routines
//      To ease maintenance, the engine is designed to put all code that
//      might need to be updated per setup instance in custom.h and custom.cpp.
//      The CCustom class is called in a few occasions where intervention would
//      be useful and where things aren't data-driven. Please make every
//      attempt to make specific changes in this file and in the resources.

#include "custom.h"


//---------------------------------------------------------------------
//  Constants for the registr
//
#define XSETUP_BUILD_REGVALUE        _TEXT("Build")
#define XSETUP_FULL_BUILD_REGVALUE   _TEXT("InstalledVersion")
#define XSETUP_INSTALLTIME_REGVALUE  _TEXT("LastInstallDate")
#define XSETUP_INSTALL_REGVALUE      _TEXT("InstallPath")
#define XSETUP_PRODUCT_NAME_REGVALUE _TEXT("ProductName")
#define XDK_OLD_INSTALL_REGVALUE     _TEXT("SDK Doc Path")  //Legacy hack for old InstallShield XDK

//---------------------------------------------------------------------
//  Debug routines

void _DebugPrint(LPCSTR lpszFormat, va_list vararg);

#if DBG == 1
inline void
DebugPrint(
    LPCSTR lpszFormat,
    ...
    )
{
    va_list vararg;
    va_start(vararg, lpszFormat);
    _DebugPrint(lpszFormat, vararg);
    va_end(vararg);
}
#else
inline void
DebugPrint(LPCSTR lpszFormat, ...) { }
#endif

#if DBG == 1
#define ASSERT(f) _ASSERT((f) == 0, __FILE__, __LINE__)
#else
#define ASSERT(f)
#endif

inline void
_ASSERT(
    BOOL f,
    LPCSTR lpFile,
    int nLine
    )
{
    if (f)
    {
        DebugPrint("ASSERT! %s (%d)\n", lpFile, nLine);
        DebugBreak();
    }
}

#define FLAG_FROM_LETTER(x) (1 << ((x) - 'A'))

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))


//---------------------------------------------------------------------


#include "unpackui.h"

//---------------------------------------------------------------------
//  CManifest class

#define MAX_MANIFEST_LINE       2048

// Standard manifest parsing structures
typedef struct _PARSEFILE
{
    LPSTR lpRoot;
    LPSTR lpDest;
    LPSTR lpFlags;
    LPSTR lpFileInfo;
} PARSEFILE, *LPPARSEFILE;
#define PARSEFILE_DWORDS (sizeof (PARSEFILE) / sizeof (LPSTR))

typedef struct _PARSECOPY
{
    LPSTR lpRoot;
    LPSTR lpSource;
    LPSTR lpDest;
    LPSTR lpFlags;
    LPSTR lpFileInfo;
} PARSECOPY, *LPPARSECOPY;
#define PARSECOPY_DWORDS (sizeof (PARSECOPY) / sizeof (LPSTR))

typedef struct _PARSEDELETE
{
    LPSTR lpRoot;
    LPSTR lpDest;
    LPSTR lpFlags;
} PARSEDELETE, *LPPARSEDELETE;
#define PARSEDELETE_DWORDS (sizeof (PARSEDELETE) / sizeof (LPSTR))

typedef struct _PARSEREG
{
    LPSTR lpRootKey;
    LPSTR lpSubKey;
    LPSTR lpValueName;
    LPSTR lpValue;
    LPSTR lpFlags;
} PARSEREG, *LPPARSEREG;
#define PARSEREG_DWORDS (sizeof (PARSEREG) / sizeof (LPSTR))

typedef struct _PARSESHORTCUT
{
    LPSTR lpRoot;
    LPSTR lpDest;
    LPSTR lpShortcutPath;
    LPSTR lpDescription;
} PARSESHORTCUT, *LPPARSESHORTCUT;
#define PARSESHORTCUT_DWORDS (sizeof (PARSESHORTCUT) / sizeof (LPSTR))

#define MANIFEST_TYPE_FILE          1
#define MANIFEST_TYPE_REMOVE        2
#define MANIFEST_TYPE_REGKEY        3

#define MANIFEST_FLAG_PLACEHOLDER   1

typedef struct _FILEMANIFEST
{
    DWORD dwType;
    LPSTR lpID;
    LPSTR lpFile;
    LPSTR lpFlags;
} FILEMANIFEST, *LPFILEMANIFEST;

class CManifest
{
private:
    LPSTR m_lpData;
    LPSTR m_lpLine;
    DWORD m_cData;
    BOOL m_fEOF;
    char m_szLine[MAX_MANIFEST_LINE + 1];

public:
    CManifest(CSettingsFile* psettings);

    BOOL SetFile(LPCTSTR lpFile);
    BOOL ResetPointer();
    LPSTR GetLine();
    BOOL SkipToNextLine();

    // The rest are obsolete
    // Return the information from a file line in the structure
    void ParseLine(LPFILEMANIFEST lpEntry);

    // Returns TRUE if we're at the end of the manifest
    BOOL IsEOF() { return m_fEOF; }

    // Processes non-file type manifest items like REMOVE and REGKEY
    BOOL ProcessNonFileEntries(LPFILEMANIFEST lpEntry);
};


//---------------------------------------------------------------------
//  CDirectories class

typedef struct _TARGETDIR
{
    LPSTR lpID;
    LPSTR lpCommand;
    LPSTR lpData;
    DWORD dwFlags;
    DWORD dwDiskSpaceUsed;
    DWORD dwDiskSpaceNeeded;
    CHAR szPath[MAX_PATH];
} TARGETDIR, *LPTARGETDIR;

class CDirectories
{
private:
    DWORD m_dwcTargets;
    LPTARGETDIR m_Targets;
    CSettingsFile* m_psettings;

    BOOL Init(DWORD dwcTargets);
    BOOL AddTarget(LPSTR lpID, LPSTR lpCommand);
    BOOL ProcessCommands(LPTARGETDIR lpTarget);
    BOOL FindRegKey(LPTARGETDIR lpTarget);

public:
    CDirectories(CSettingsFile* psettings) { m_psettings = psettings; m_Targets = NULL; }
    ~CDirectories() { if (m_Targets) free(m_Targets); }

    VOID ParseTargetDirs();
    LPTARGETDIR FindTarget(LPCSTR lpID);
    LPTARGETDIR AllTargets(LPDWORD pdwc);
};

inline DWORD
DWORDtoDiskSpace(
    DWORD dw
    )
{
    return (dw >> 15) + 1;
}


//---------------------------------------------------------------------
//  CUninstall

class CUninstall
{
private:
    TCHAR m_szUninstallFile[MAX_PATH];
    LPSTR m_lpFile;
    LPSTR* m_ptrtable;
    DWORD m_dwcOldLines;
    DWORD m_dwcTotalLines;
    DWORD m_dwcMaxLines;
    DWORD ReadOldUninstallFile(LPSTR lpUninstallDir);

public:
    CUninstall();
    BOOL Init(DWORD dwcManifest, LPSTR lpUninstallDir);
    VOID AddFile(LPCSTR lpPath, LPCSTR lpFileInfo, BOOL fRegister);
    VOID AddReg(LPCSTR lpRoot, LPCSTR lpSubkey, LPCSTR lpValueName);
    VOID AddDir(LPCSTR lpPath);
    VOID Flush();
};

inline
CUninstall::CUninstall()
{
    m_lpFile = NULL;
    m_ptrtable = NULL;
}


//---------------------------------------------------------------------
//  CCab class

class CUnpacker;

#define FILEFLAGS_PROMPT        0       // default is to always prompt
#define FILEFLAGS_CLOBBER       FLAG_FROM_LETTER('R') // for Replace Always
#define FILEFLAGS_NEVERCLOBBER  FLAG_FROM_LETTER('K') // for Keep Always
#define FILEFLAGS_NOUNINSTALL   FLAG_FROM_LETTER('U') // for no Uninstall
#define FILEFLAGS_SYSTEMREBOOT  FLAG_FROM_LETTER('S') // for System Reboot required. Needs to be replaced using MoveFileEx with the MOVEFILE_DELAY_UNTIL_REBOOT
                                                      // should be used on codec's and shell extensions.
#define FILEFLAGS_OLEREGISTER   FLAG_FROM_LETTER('O') // for OLE Register (this is REGSVR32)


#define REGLINE_FLAGS_REGSZ     0       // default is REG_SZ
#define REGLINE_FLAGS_DWORD     FLAG_FROM_LETTER('D') // for DWORD

#define REMOVELINE_FLAGS_ONLYONFIRSTRUN FLAG_FROM_LETTER('F') // First run only

typedef struct _EOFSTRUCT
{
    DWORD dwcFiles;
    DWORD dwSignature;
} EOFSTRUCT, *LPEOFSTRUCT;

#define EXE_SIGNATURE 'XOBX'

#define MAX_SETUP_PATH_LENGTH   64

class CCab
{
private:
    HFDI m_hfdi;
    ERF m_erf;
    CDirectories* m_pdirs;
    CManifest* m_pmanifest;
    CUninstall* m_puninstall;
    CSettingsFile* m_psettings;
    CUnpacker* m_punpacker;
    CHAR m_szNoManifestRoot[MAX_PATH];
    CHAR m_szPath[MAX_PATH];
    BOOL m_fEOF;
    DWORD m_dwOverwriteFlags;
    DWORD m_dwcFiles;
    HANDLE m_handle;
    BOOL m_fRegister;
    BOOL m_fDelayUntilReboot;
    CHAR m_szFinalPath[MAX_PATH];

    static BOOL m_fStopCopying;
    static CFileInfo m_fi;

    BOOL ProcessManifestLine(LPSTR lpCommand, LPSTR lpLine);
    BOOL ProcessManifestCopyCommand(LPSTR lpLine);
    BOOL ProcessManifestRemoveCommand(LPSTR lpCommand, LPSTR lpLine);
    BOOL ProcessManifestRegCommand(LPSTR lpCommand, LPSTR lpLine);
    BOOL ProcessManifestShortcut(LPSTR lpLine);
    HRESULT CreateLink(LPCSTR lpszPathObj, LPCSTR lpszPathLink, LPCSTR lpszDesc) ;
    int PrepareFileOverwrite(LPTARGETDIR lpTarget, LPSTR lpRoot, LPSTR lpDest, LPSTR lpFlags, LPSTR lpFileInfo);
    BOOL OnOverwriteMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    VOID NukeDirectory(LPSTR lpdir);
    void SubstitutePathsInString(LPSTR lpszInput, LPSTR lpszOutput);
    LANGID GetUserDefaultUILanguageWithDefault();

public:
    CCab(CSettingsFile* psettings, CDirectories* pdirs, CUninstall* puninstall, CUnpacker* punpacker);
    ~CCab();

    // Prepares for cabfile reading from EXE
    BOOL Init();

    // Closes session so we can reuse object
    void Close();

    // Drive the file copy process. Returns only when entire cabfile has
    // been processed.
    // With manifest means that the callback processes files per the
    // manifest instructions.
    // Without manifest means that the callback just copies files directly as
    // specified in the cabfile
    BOOL ProcessWithManifest(CManifest* pmanifest, DWORD dwFile);
    BOOL ProcessNoManifest(LPCSTR lpRootDirectory, DWORD dwFile);

    // Callback called from FDICopy (called within Process()) for several types
    // of notifications during file copies. This is where the bulk of the
    // file copy work is done.
    // FDINOTIFICATIONTYPE is an int, FDINOTIFICATION is an overloaded structure
    // of items returned on notifications. Contents vary per notification type.
    // definitions of these types is in FDI.H
    INT_PTR Callback(FDINOTIFICATIONTYPE fdint, PFDINOTIFICATION pfdin);

    // Called just before a file is copied, allows us to adjust things
    // and process the manifest per-file
    INT_PTR FileCopyOpen(PFDINOTIFICATION pfdin);

    // Called to close the file. This is the point where we set the dates
    // and match attributes of the original file.
    INT_PTR FileCopyClose(PFDINOTIFICATION pfdin);

    static VOID MakeDirectory(LPCTSTR pszPath);
    static DWORD Attr32FromAttrFAT(WORD attrMSDOS);
    static BOOL AdjustFileTime(INT_PTR hf);

    friend BOOL CALLBACK OverwriteDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    friend VOID StopCopying();
};

inline CCab::CCab(
    CSettingsFile* psettings,
    CDirectories* pdirs,
    CUninstall* puninstall,
    CUnpacker* punpacker
    )
:  m_hfdi(NULL),
   m_pdirs(pdirs),
   m_fEOF(FALSE),
   m_dwOverwriteFlags(0),
   m_puninstall(puninstall),
   m_psettings(psettings),
   m_punpacker(punpacker),
   m_handle(INVALID_HANDLE_VALUE),
   m_fRegister(FALSE),
   m_fDelayUntilReboot(FALSE)

{
//    m_szCabName[0] = 0;
    m_szPath[0] = 0;
}

inline VOID
StopCopying()
{
    CCab::m_fStopCopying = TRUE;
}

BOOL CALLBACK OverwriteDlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//---------------------------------------------------------------------
//  CUnpacker (application) class

#define WMP_ERROR           (WM_USER + 100)
#define WMP_COPYCOMPLETE    (WM_USER + 101)

#define UNPACKER_FLAGS_FORCEINSTALL         1
#define UNPACKER_FLAGS_SKIPPRESETUPCHECKS   2
#define UNPACKER_FLAGS_SETUPHASRUNBEFORE    4
#define UNPACKER_FLAGS_SKIPLICENSEPAGE      8
#define UNPACKER_FLAGS_REBOOT_REQUIRED      0x10

typedef struct _INIREGKEY
{
    LPSTR lpKey;
    LPSTR lpSubkey;
    HKEY  hRootKey;
} INIREGKEY, *LPINIREGKEY;
#define INIREGKEY_DWORDS 2  //The hRootKey is not part of ParseLine

class CUnpacker : public CCustomSite
{
private:
    static CSettingsFile m_settings;
    static CManifest m_manifest;
    static CCustom *m_spCustom;
    static CDirectories m_dirs;
    static CUninstall m_uninstall;
    static BOOL m_sfWizardRunning;
    static CUnpacker* m_spThis;
    static HINSTANCE m_shinst;
    static HINSTANCE m_scustomhinst;

    LPSTR m_lpCommandline;
    TCHAR m_szUninstallDir[MAX_PATH];
    TCHAR m_szDefaultDir[MAX_PATH];
    BOOL m_fTempDir;
    HANDLE m_hthread;
    HWND m_hwndDlg;
    HWND m_hwndProgress;
    HWND m_hwndActionText;
    HWND m_hwndInProgressText;
    DWORD m_dwBytesCopiedSoFar;
    DWORD m_dwcManifestLines;
    DWORD m_dwFlags;
    TCHAR m_szCabSource[MAX_PATH];
    ABC m_abc[256];
    INIREGKEY m_ProductRegKey;
    LPSTR m_lpszProductName;

    BOOL MoveDirectory(LPCTSTR lpSource, LPCTSTR lpDest);
    void DeleteDirectory(LPCTSTR lpDir);
    LPCTSTR AddCommas(__int64 li);
    DWORD CopyFiles();
    BOOL AccumulateFileSizes();
    int CheckAvailableSpace();
    VOID ConvertToEllipses(HWND hwnd, LPSTR lpOut, LPCTSTR lpIn);
    BOOL CheckWindows();
    BOOL CheckUpgradeVersion();
    
public:
    CUnpacker(HINSTANCE hinst, LPSTR lpCommandline);
    ~CUnpacker();

    // Properies that need to be accessed by our 'CAB' file
    CLangID m_SupportedLangs;
    DWORD m_dwDefaultLang;

    BOOL CommandLine(LPSTR lpCmdLine);
    BOOL UnpackSettingsCab();
    BOOL DoPreSetupChecks();
    BOOL CustomDirectory(LPTARGETDIR lpTarget);
    BOOL ProcessDirectories();
    BOOL CheckForOtherProduct(LPSTR lpTarget, LPSTR lpOtherProduct, DWORD cbOtherProduct);
    BOOL ValidateDirectories();
    BOOL MoveSettingsCab();
    int CheckDiskSpace();   // returns -1, 0, 1: see header comment
    VOID StartCopy(HWND hwndDlg, HWND hwndProgress, HWND hwndActionText, HWND hwndInProgressText);
    VOID PostCopyError(DWORD dwMsgID, DWORD dwErrorCode);
    VOID UpdateProgressBar(DWORD dwSize, DWORD dwID, LPCTSTR lpText);
    DWORD AccumulateFlags(LPCSTR lp);
    VOID ProcessLegacySetup();
    VOID WriteInstallInfo();
    VOID PostSetup(int nRet) {if(m_spCustom) m_spCustom->PostSetup(nRet);}

    LPCTSTR GetUninstallDir() { return m_szUninstallDir; }
    HWND GetHWNDDlg() { return m_hwndDlg; }
    CUninstall* GetUninstall() { return &m_uninstall; }
    LPCSTR CUnpacker::GetCabSource() { return CUnpacker::m_szCabSource; }

    friend HINSTANCE GetInstanceHandle();
    friend CSettingsFile* GetSettings();
    friend CUnpacker* GetUnpacker();
    friend DWORD WINAPI ThreadStart(CUnpacker* punpacker);
    friend void SetWizRunningState(BOOL fRunning);

    static LPCSTR GetProductName() { return m_spThis->m_lpszProductName; }

   /* CCustomSite Methods */
    void   SetLastError(DWORD dwStringID, DWORD dwErrorCode);
    void   SetLastErrorString(LPCTSTR lpString,DWORD dwErrorCode);
    LPTSTR GetDefaultDir() { return m_szDefaultDir; }
    DWORD  GetFlags() { return m_dwFlags; }
    VOID   SetFlags(DWORD dwNewFlags) { m_dwFlags |= dwNewFlags; }
    void   AddRegForUninstall(LPCSTR lpRoot, LPCSTR lpSubkey, LPCSTR lpValueName){m_uninstall.AddReg(lpRoot, lpSubkey, lpValueName);}
    LPTSTR GetSetting(DWORD dwKey){return m_settings.GetSetting(dwKey);}
};

inline HINSTANCE
GetInstanceHandle()
{
    return CUnpacker::m_shinst;
}

inline CSettingsFile*
GetSettings()
{
    return &CUnpacker::m_settings;
}

inline CUnpacker*
GetUnpacker()
{
    return CUnpacker::m_spThis;
}

inline void SetWizRunningState(BOOL fRunning)
{
    CUnpacker::m_sfWizardRunning = fRunning;
}


//---------------------------------------------------------------------

#endif // #ifndef _UNPACKER_H
