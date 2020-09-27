/*++

Copyright (c) 1996 Microsoft Corporation

Module Name:

    winnt32p.h

Abstract:

    Header file for winnt32 plug-in down-level-side DLLs.

Author:

    Ted Miller (tedm) 6 December 1996

Revision History:

--*/
#ifndef WINNT32P_H
#define WINNT32P_H

//
// WMX_ACTIVATEPAGE is sent when a page is being activated or deactivated.
//
// (The plug-in's pages do not receive WM_NOTIFY with PSN_SETACTIVE and
// PSN_KILLACTIVE -- they get a WMX_ACTIVATEPAGE instead.)
//
// wParam non-0: activating
// wParam 0    : deactivating
// lParam      : unused.
//
// Return non-0 to accept (de)activation, 0 to not accept it. The semantics
// of not accepting (de)activation are exactly the same as for the
// PSN_SETACTIVE/PSN_KILLACTIVE case.
//
#define WMX_ACTIVATEPAGE        (WM_APP+0)

//
// First custom window message a plug-in can use.
// Do NOT use any below this value.
//
#define WMX_PLUGIN_FIRST        (WM_APP+1000)

//
// IDs the plug-in must use for its title and subtitle text on each
// wizard page.
//
#define ID_TITLE_TEXT           1000
#define ID_SUBTITLE_TEXT        1029

//
// Define types for routines that the plug-in DLL must export.
//



//
// Maximum source count..
//
#define MAX_SOURCE_COUNT 8


/*
    This structure contains the information that is passed to a Winnt32 plug-in in
    its Init function.

    UnattendedFlag - Supplies the address of the global attended flag within
        winnt32 itself. A plugin should react accordingly to setup being in
        unattended mode.

    CancelledFlag - supplies the address of a global variable within
        winnt32 itself. If the plug-in encounters a fatal error while
        processing later it should inform the user, set the BOOL to which
        this parameter points to TRUE, and do the following:

        PropSheet_PressButton(WizardDialogBox,PSBTN_CANCEL);

        where WizardDialogBox is the window handle of the wizard dialog box
        (typically obtained via GetParent(hdlg) where hdlg is the
        window handle of a page in the wizard).

    AbortedFlag - supplies the address of a global variable within winnt32 itself.
        If the plugin would like to exit setup, but not show the unsuccessfull
        completion page, it should set both CancelledFlag and AbortedFlag to TRUE.

    UpgradeFlag - supplies the address of a global variable that will
        indicate whether the user is upgrading or installing a new fresh
        copy of NT. The plug-in must sample this value when it is asked to
        activate its pages and take appropriate action (ie, not activating
        if the user is not upgrading). The value this pointer points to
        is NOT valid until after the plug-in's pages are first
        activated.

    LocalSourceModeFlag - supplies the address of a global variable that will
        indicate whether the user is installing via local source mode or not.
        This parameter is not valid until after the plug-in's pages are first
        activated.

    CdRomInstallFlag - supplies the address of a global variable that will
        indicate whetherthe user is installing via CdRom or not. This
        parameter is not valid until after the plug-in's pages are first
        activated.

    NotEnoughSpaceBlockFlag - supplies the address of a global variable that will
        indicate wether setup should halt setup and exit if it detects that
        there is not enough space to complete setup (not enough space for the ~ls dir.)

    LocalSourceDrive - supplies the address of a global variable that will indicate
        the drive number of the local source directory. (2 = C, 3 = D, etc...) This is
        not valid until after winnt32 builds the copy list. 0 indicates an invalid drive.

    LocalSourceSpaceRequired - supplies the address of a global variable that indicates the amount
        of space on the LocalSourceDrive required by winnt32. This is not valid until after
        winnt32 builds the copy list.

    UnattendedScriptFile - supplies the address of a global variable that will
        contain the unattend script file (such as passed in on the command line.)
        This parameter is not valid until after the plug-in's pages are first
        activated.

    SourcePath - supplies an array of SourcePaths that indicate where the
        NT source files exist. This parameter is not valid until after the
        plug-in's pages are first activated.

    SourceCount - supplies the count of SourcePaths in the above array.
        This parameter is not valid until after the plug-in's pages are first
        activated.

    UpgradeOptions - supplies a multistring of special Upgrade commandline options
        to the dll. These options are of the form /#U:[Option] so, for example,
        if someone started winnt32 with the commandline winnt32 /#U:FOO /#U:BAR,
        this string would eventually contain "FOO\0BAR\0\0" This parameter is not
        valid untila after the upgrade plug-in's pages are first activated.

    ProductType - Specifies the type of product being installed.  The value this pointer
        points to is NOT valid until after the plug-in's pages are first activated.

    BuildNumber - Specifies the build of NT being installed.

    ProductVersion - Specifies the version of NT being installed.  The major version is
        in the high byte, and the minor version is in the low byte.

    Debug - Specifies if WINNT32 is the checked build (TRUE) or the free build (FALSE).

    PreRelease - Specifies if the current build is a pre-release (TRUE) or final release (FALSE).

*/

typedef enum {
    UNKNOWN,
    NT_WORKSTATION,
    NT_SERVER
} PRODUCTTYPE;


typedef struct tagWINNT32_PLUGIN_INIT_INFORMATION_BLOCK {
    UINT            Size;
    BOOL     *      UnattendedFlag;
    BOOL     *      CancelledFlag;
    BOOL     *      AbortedFlag;
    BOOL     *      UpgradeFlag;
    BOOL     *      LocalSourceModeFlag;
    BOOL     *      CdRomInstallFlag;
    BOOL     *      NotEnoughSpaceBlockFlag;
    DWORD    *      LocalSourceDrive;
    LONGLONG *      LocalSourceSpaceRequired;
    LPCTSTR  *      UnattendedScriptFile;
    LPCTSTR  *      SourceDirectories;
    DWORD    *      SourceDirectoryCount;
    LPCTSTR  *      UpgradeOptions;
    PRODUCTTYPE *   ProductType;
    DWORD           BuildNumber;
    WORD            ProductVersion;         // i.e., MAKEWORD(5,0)
    BOOL            Debug;
    BOOL            PreRelease;
    BOOL     *      ForceNTFSConversion;
    UINT     *      Boot16;                 // Win9x upgrade only
} WINNT32_PLUGIN_INIT_INFORMATION_BLOCK,*PWINNT32_PLUGIN_INIT_INFORMATION_BLOCK;


typedef BOOL (*READ_DISK_SECTORS_PROC) (TCHAR,UINT,UINT,UINT,PBYTE);

typedef struct tagWINNT32_WIN9XUPG_INIT_INFORMATION_BLOCK {



    UINT      Size;
    PWINNT32_PLUGIN_INIT_INFORMATION_BLOCK BaseInfo;
    LPCTSTR   UpgradeSourcePath;
    PLONGLONG WinDirSpace;
    PUINT     RequiredMb;
    PUINT     AvailableMb;
    LPCTSTR * OptionalDirectories;
    DWORD   * OptionalDirectoryCount;
    UINT    * UpgradeFailureReason;
    READ_DISK_SECTORS_PROC ReadDiskSectors;

} WINNT32_WIN9XUPG_INIT_INFORMATION_BLOCK, *PWINNT32_WIN9XUPG_INIT_INFORMATION_BLOCK;


/*++

UPGRADEFAILURES is a list of reasons that an upgrade cannot be performed. This list allows winnt32 to own certain messages
for failures, but for the upgrade dll to do the actual checking for those failures.

If you define a FAILREASON(<x>) you need to add a MSG_<x> to the winnt32 dll message.mc file.

This macro expansion list will create an enumerated type FAILREASON_<x> as well as populate an array of potential
failure messages.


++*/
#define UPGRADEFAILURES                         \
    FAILREASON(UPGRADE_OK)                      \
    FAILREASON(UPGRADE_OTHER_OS_FOUND)          \

#define FAILREASON(x) REASON_##x,

enum {UPGRADEFAILURES /*,*/ REASON_LAST_REASON};

#undef FAILREASON

typedef
DWORD
(CALLBACK WINNT32_PLUGIN_INIT_ROUTINE_PROTOTYPE)(
    PWINNT32_PLUGIN_INIT_INFORMATION_BLOCK Info
    );

typedef WINNT32_PLUGIN_INIT_ROUTINE_PROTOTYPE * PWINNT32_PLUGIN_INIT_ROUTINE;

/*++

Routine Description:

    This routine is called by winnt32 to initialize the plug-in dll.

Arguments:

    Info - A WINNT32_PLUGIN_INIT_INFORMATION_BLOCK. See above for details.

Return Value:

    Win32 error code indicating outcome. If not NO_ERROR then winnt32 will
    put up UI telling the user of the failure.

--*/


typedef
DWORD
(CALLBACK WINNT32_PLUGIN_GETPAGES_ROUTINE_PROTOTYPE)(
    PUINT            PageCount1,
    LPPROPSHEETPAGE *Pages1,
    PUINT            PageCount2,
    LPPROPSHEETPAGE *Pages2,
    PUINT            PageCount3,
    LPPROPSHEETPAGE *Pages3
    );

typedef WINNT32_PLUGIN_GETPAGES_ROUTINE_PROTOTYPE * PWINNT32_PLUGIN_GETPAGES_ROUTINE;

/*++

Routine Description:

    This routine is called by winnt32 to retrieve wizard pages from the
    plug-in dll.

    Note that the plug-in does NOT need to worry about drawing watermarks
    or background bitmaps, or the separator between a header-area watermark
    and the body of its pages. Winnt32 does all this automatically.

    The plugin should, however, have regular static text controls in the
    header area. Static text controls in that area should use the reserved
    IDs (see above) for the title and subtitle, since winnt32 will automatically
    change the font and size of that text when the page is displayed.

Arguments:

    PageCount1 - receives the number of pages in the first set of contiguous
        pages.

    Pages1 - receives a pointer to an array of property sheet page structures.
        The plug-in is responsible for managing this array but must not free
        it at any time since winnt32 may refer to it at any point.

    PageCount2 - receives the number of pages in the second set of contiguous
        pages.

    Pages2 - receives a pointer to an array of property sheet page structures.
        The plug-in is responsible for managing this array but must not free
        it at any time since winnt32 may refer to it at any point.

    PageCount3 - receives the number of pages in the third set of contiguous
        pages.

    Pages3 - receives a pointer to an array of property sheet page structures.
        The plug-in is responsible for managing this array but must not free
        it at any time since winnt32 may refer to it at any point.

Return Value:

    Win32 error code indicating outcome. If not NO_ERROR then winnt32 will
    put up UI telling the user of the failure.

--*/


typedef
DWORD
(CALLBACK WINNT32_PLUGIN_WRITEPARAMS_ROUTINE_PROTOTYPE)(
    LPCTSTR FileName
    );

typedef WINNT32_PLUGIN_WRITEPARAMS_ROUTINE_PROTOTYPE * PWINNT32_PLUGIN_WRITEPARAMS_ROUTINE;

/*++

Routine Description:

    This routine is called by winnt32 to request the plug-in write to the
    parameters file that will be passed to text mode setup (ie, winnt.sif).

Arguments:

    FileName - supplies the filename of the .ini-style file to be written to.
        This file is the parameters file plus any user-specified unattend file.
        The plug-in should make whatever modifications are meaningful to it.

Return Value:

    Win32 error code indicating outcome. If not NO_ERROR then winnt32 will
    put up UI telling the user of the failure.

--*/


typedef
VOID
(CALLBACK WINNT32_PLUGIN_CLEANUP_ROUTINE_PROTOTYPE)(
    VOID
    );

typedef WINNT32_PLUGIN_CLEANUP_ROUTINE_PROTOTYPE * PWINNT32_PLUGIN_CLEANUP_ROUTINE;

/*++

Routine Description:

    This routine is called by winnt32 in the case where installation is
    aborted after the wizard has been started.

    The plug-in should silently perform whatever cleanup is needs to
    to undo any changes it made to the user's system.

Arguments:

    None.

Return Value:

    None.

--*/

typedef
BOOL
(CALLBACK WINNT32_PLUGIN_VIRUSSCANNER_CHECK_PROTOTYPE)(
    VOID
    );

typedef WINNT32_PLUGIN_VIRUSSCANNER_CHECK_PROTOTYPE * PWINNT32_PLUGIN_VIRUSSCANNER_CHECK_ROUTINE;

/*++

Routine Description:

    This routine is called by winnt32 when running on win9x machines.

    The plugin should do a check for any virus scanners on the machine that could cause setup
    to be unable to complete installation (locking the MBR, for instance.) The plugin is also
    responsible for communicating any problems to the user.

Arguments:

    None.

Return Value:

    TRUE if there are no virus scanners to worry about, FALSE otherwise.

--*/



typedef
PTSTR
(CALLBACK WINNT32_PLUGIN_OPTIONAL_DIRS_PROTOTYPE)(
    VOID
    );

typedef WINNT32_PLUGIN_OPTIONAL_DIRS_PROTOTYPE * PWINNT32_PLUGIN_OPTIONAL_DIRS_ROUTINE;

/*++

Routine Description:

    This routine is called by winnt32 when running on win9x machines.

    The plugin should do a check for any virus scanners on the machine that could cause setup
    to be unable to complete installation (locking the MBR, for instance.) The plugin is also
    responsible for communicating any problems to the user.

Arguments:

    None.

Return Value:

    TRUE if there are no virus scanners to worry about, FALSE otherwise.

--*/



//
// Names of routines that must be exported by the plug-in dll.
//
#define WINNT32_PLUGIN_INIT_NAME        "Winnt32PluginInit"
#define WINNT32_PLUGIN_GETPAGES_NAME    "Winnt32PluginGetPages"
#define WINNT32_PLUGIN_WRITEPARAMS_NAME "Winnt32WriteParams"
#define WINNT32_PLUGIN_CLEANUP_NAME     "Winnt32Cleanup"
#define WINNT32_PLUGIN_VIRUSSCANCHECK_NAME "Winnt32VirusScannerCheck"
#define WINNT32_PLUGIN_GETOPTIONALDIRS_NAME "Winnt32GetOptionalDirectories"
#endif










