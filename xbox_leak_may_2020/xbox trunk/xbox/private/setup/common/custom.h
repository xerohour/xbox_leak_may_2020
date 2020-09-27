//  CUSTOM.H
//
//      This file defines the abstract interfaces for communication between
//      the custom setup dll, and unpacker.exe or uninstall.exe.
//
//      All communication with the custom setup dll, should go through 
//      a method defined in this file.
//
//  Created 30-Mar-2001 [JonT]
//  Separated the custom dll from the exe's.  23-Aug-2001 [MitchD]

#ifndef _CUSTOM_H_
#define _CUSTOM_H_

//---------------------------------------------------------------------
class CCustomSite
{
  public:
    virtual void   SetLastError(DWORD dwStringID, DWORD dwErrorCode)=0;
    virtual void   SetLastErrorString(LPCTSTR lpString,DWORD dwErrorCode)=0;
    virtual LPTSTR GetDefaultDir()=0;
    virtual DWORD  GetFlags()=0;
    virtual void   SetFlags(DWORD dwNewFlags)=0;
    virtual void   AddRegForUninstall(LPCSTR lpRoot, LPCSTR lpSubkey, LPCSTR lpValueName) = 0;
    virtual LPTSTR GetSetting(DWORD dwKey)=0;
};

#define SETUPSITE_FLAGS_FORCEINSTALL         1
#define SETUPSITE_FLAGS_SKIPPRESETUPCHECKS   2
#define SETUPSITE_FLAGS_SETUPHASRUNBEFORE    4

//---------------------------------------------------------------------
class CCustom
{
public:
    virtual BOOL DoPreSetupChecks()=0;
    virtual BOOL ValidateDirectory(
                    LPCSTR lpDefaultDir,    // Default dir: set on upgrade or by the user
                    LPCSTR lpID,            // Target directory ID string. Unique.
                    LPSTR lpCommand,        // Command that was used to fill in the target path
                    LPSTR lpData,           // Data used to fill in the target path (after : in .INI file)
                    LPSTR lpPath            // Path files will be copied to with this ID
                    )=0;
    virtual VOID AfterCopy()=0;
    virtual VOID PostSetup(int nRet)=0;
};

typedef CCustom *(__stdcall *PFNGETCUSTOM)(CCustomSite *pCustomSite);
#define GETCUSTOMPROC "GetCustom"

// Called by uninstall to
typedef BOOL (__stdcall *PFNPREUNINSTALL)(LPCTSTR pszUninstallPath);
#define PREUNINSTALLPROC "PreUninstall"
typedef void (__stdcall *PFNPOSTUNINSTALL)(LPCTSTR pszUninstallPath);
#define POSTUNINSTALLPROC "PostUninstall"

#endif // #ifndef _CUSTOM_H_
