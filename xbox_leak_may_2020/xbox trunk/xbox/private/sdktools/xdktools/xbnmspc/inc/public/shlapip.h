
#ifndef _SHELAPIP_
#define _SHELAPIP_

#include <objbase.h>

//
// Define API decoration for direct importing of DLL references.
//
#ifndef WINSHELLAPI
#if !defined(_SHELL32_)
#define WINSHELLAPI       DECLSPEC_IMPORT
#else
#define WINSHELLAPI
#endif
#endif // WINSHELLAPI

#ifndef SHSTDAPI
#if !defined(_SHELL32_)
#define SHSTDAPI          EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define SHSTDAPI_(type)   EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#else
#define SHSTDAPI          STDAPI
#define SHSTDAPI_(type)   STDAPI_(type)
#endif
#endif // SHSTDAPI

#ifndef SHDOCAPI
#if !defined(_SHDOCVW_)
#define SHDOCAPI          EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define SHDOCAPI_(type)   EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#else
#define SHDOCAPI          STDAPI
#define SHDOCAPI_(type)   STDAPI_(type)
#endif
#endif // SHDOCAPI


#include <pshpack1.h>

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif  /* __cplusplus */


SHSTDAPI_(HICON) ExtractAssociatedIconExA(HINSTANCE hInst,LPSTR lpIconPath,LPWORD lpiIconIndex, LPWORD lpiIconId);
SHSTDAPI_(HICON) ExtractAssociatedIconExW(HINSTANCE hInst,LPWSTR lpIconPath,LPWORD lpiIconIndex, LPWORD lpiIconId);
#ifdef UNICODE
#define ExtractAssociatedIconEx  ExtractAssociatedIconExW
#else
#define ExtractAssociatedIconEx  ExtractAssociatedIconExA
#endif // !UNICODE
// BUGBUG this API needs to be A/W. Don't make it public until it is.
SHSTDAPI_(BOOL) DragQueryInfo(HDROP hDrop, LPDRAGINFO lpdi);
#define ABE_MAX         4
SHSTDAPI_(HGLOBAL) InternalExtractIconA(HINSTANCE hInst, LPCSTR lpszFile, UINT nIconIndex, UINT nIcons);
SHSTDAPI_(HGLOBAL) InternalExtractIconW(HINSTANCE hInst, LPCWSTR lpszFile, UINT nIconIndex, UINT nIcons);
#ifdef UNICODE
#define InternalExtractIcon  InternalExtractIconW
#else
#define InternalExtractIcon  InternalExtractIconA
#endif // !UNICODE
SHSTDAPI_(HGLOBAL) InternalExtractIconListA(HANDLE hInst, LPSTR lpszExeFileName, LPINT lpnIcons);
SHSTDAPI_(HGLOBAL) InternalExtractIconListW(HANDLE hInst, LPWSTR lpszExeFileName, LPINT lpnIcons);
#ifdef UNICODE
#define InternalExtractIconList  InternalExtractIconListW
#else
#define InternalExtractIconList  InternalExtractIconListA
#endif // !UNICODE
SHSTDAPI_(BOOL)    RegisterShellHook(HWND, BOOL);
#define SHGetNameMappingCount(_hnm) DSA_GetItemCount(_hnm)
#define SHGetNameMappingPtr(_hnm, _iItem) (LPSHNAMEMAPPING)DSA_GetItemPtr(_hnm, _iItem)

typedef struct _RUNDLL_NOTIFYA {
    NMHDR     hdr;
    HICON     hIcon;
    LPSTR     lpszTitle;
} RUNDLL_NOTIFYA;
typedef struct _RUNDLL_NOTIFYW {
    NMHDR     hdr;
    HICON     hIcon;
    LPWSTR    lpszTitle;
} RUNDLL_NOTIFYW;
#ifdef UNICODE
typedef RUNDLL_NOTIFYW RUNDLL_NOTIFY;
#else
typedef RUNDLL_NOTIFYA RUNDLL_NOTIFY;
#endif // UNICODE

typedef void (WINAPI *RUNDLLPROCA)(HWND hwndStub, HINSTANCE hInstance, LPSTR pszCmdLine, int nCmdShow);
typedef void (WINAPI *RUNDLLPROCW)(HWND hwndStub, HINSTANCE hInstance, LPWSTR pszCmdLine, int nCmdShow);
#ifdef UNICODE
#define RUNDLLPROC  RUNDLLPROCW
#else
#define RUNDLLPROC  RUNDLLPROCA
#endif // !UNICODE

#define RDN_FIRST       (0U-500U)
#define RDN_LAST        (0U-509U)
#define RDN_TASKINFO    (RDN_FIRST-0)

#define SEN_DDEEXECUTE (SEN_FIRST-0)

HINSTANCE RealShellExecuteA(
    HWND hwndParent,
    LPCSTR lpOperation,
    LPCSTR lpFile,
    LPCSTR lpParameters,
    LPCSTR lpDirectory,
    LPSTR lpResult,
    LPCSTR lpTitle,
    LPSTR lpReserved,
    WORD nShow,
    LPHANDLE lphProcess);
HINSTANCE RealShellExecuteW(
    HWND hwndParent,
    LPCWSTR lpOperation,
    LPCWSTR lpFile,
    LPCWSTR lpParameters,
    LPCWSTR lpDirectory,
    LPWSTR lpResult,
    LPCWSTR lpTitle,
    LPWSTR lpReserved,
    WORD nShow,
    LPHANDLE lphProcess);
#ifdef UNICODE
#define RealShellExecute  RealShellExecuteW
#else
#define RealShellExecute  RealShellExecuteA
#endif // !UNICODE

HINSTANCE RealShellExecuteExA(
    HWND hwndParent,
    LPCSTR lpOperation,
    LPCSTR lpFile,
    LPCSTR lpParameters,
    LPCSTR lpDirectory,
    LPSTR lpResult,
    LPCSTR lpTitle,
    LPSTR lpReserved,
    WORD nShow,
    LPHANDLE lphProcess,
    DWORD dwFlags);
HINSTANCE RealShellExecuteExW(
    HWND hwndParent,
    LPCWSTR lpOperation,
    LPCWSTR lpFile,
    LPCWSTR lpParameters,
    LPCWSTR lpDirectory,
    LPWSTR lpResult,
    LPCWSTR lpTitle,
    LPWSTR lpReserved,
    WORD nShow,
    LPHANDLE lphProcess,
    DWORD dwFlags);
#ifdef UNICODE
#define RealShellExecuteEx  RealShellExecuteExW
#else
#define RealShellExecuteEx  RealShellExecuteExA
#endif // !UNICODE

//
// RealShellExecuteEx flags
//
#define EXEC_SEPARATE_VDM     0x00000001
#define EXEC_NO_CONSOLE       0x00000002
#define SEE_MASK_FLAG_SHELLEXEC   0x00000800
#define SEE_MASK_FORCENOIDLIST    0x00001000
#define SEE_MASK_NO_HOOKS         0x00002000
#define SEE_MASK_HASLINKNAME      0x00010000
#define SEE_MASK_FLAG_SEPVDM      0x00020000
#define SEE_MASK_RESERVED         0x00040000
#define SEE_MASK_HASTITLE         0x00080000
#define SEE_MASK_FILEANDURL       0x00400000
// we have two CMIC_MASK_ values that don't have corospongind SEE_MASK_ counterparts
//      CMIC_MASK_SHIFT_DOWN      0x10000000
//      CMIC_MASK_CONTROL_DOWN    0x20000000

// All other bits are masked off when we do an InvokeCommand
#define SEE_VALID_CMIC_BITS     0x308FAFF0
#define SEE_VALID_CMIC_FLAGS    0x000FAFC0
// The LPVOID lpIDList parameter is the IDList
//
//  We have to define this structure twice.
//  The public definition uses HWNDs and HICONs.
//  The private definition uses DWORDs for Win32/64 interop.
//  The private version is called "NOTIFYICONDATA32" because it is the
//  explicit 32-bit version.
//
//  Make sure to keep them in sync!
//

typedef struct _NOTIFYICONDATA32A {
        DWORD cbSize;
        DWORD dwWnd;                        // NB!
        UINT uID;
        UINT uFlags;
        UINT uCallbackMessage;
        DWORD dwIcon;                       // NB!
#if (_WIN32_IE < 0x0500)
        CHAR   szTip[64];
#else
        CHAR   szTip[128];
#endif
#if (_WIN32_IE >= 0x0500)
        DWORD dwState;
        DWORD dwStateMask;
        CHAR   szInfo[256];
        union {
            UINT  uTimeout;
            UINT  uVersion;
        } DUMMYUNIONNAME;
        CHAR   szInfoTitle[64];
        DWORD dwInfoFlags;
#endif
} NOTIFYICONDATA32A, *PNOTIFYICONDATA32A;
typedef struct _NOTIFYICONDATA32W {
        DWORD cbSize;
        DWORD dwWnd;                        // NB!
        UINT uID;
        UINT uFlags;
        UINT uCallbackMessage;
        DWORD dwIcon;                       // NB!
#if (_WIN32_IE < 0x0500)
        WCHAR  szTip[64];
#else
        WCHAR  szTip[128];
#endif
#if (_WIN32_IE >= 0x0500)
        DWORD dwState;
        DWORD dwStateMask;
        WCHAR  szInfo[256];
        union {
            UINT  uTimeout;
            UINT  uVersion;
        } DUMMYUNIONNAME;
        WCHAR  szInfoTitle[64];
        DWORD dwInfoFlags;
#endif
} NOTIFYICONDATA32W, *PNOTIFYICONDATA32W;
#ifdef UNICODE
typedef NOTIFYICONDATA32W NOTIFYICONDATA32;
typedef PNOTIFYICONDATA32W PNOTIFYICONDATA32;
#else
typedef NOTIFYICONDATA32A NOTIFYICONDATA32;
typedef PNOTIFYICONDATA32A PNOTIFYICONDATA32;
#endif // UNICODE
typedef struct _TRAYNOTIFYDATAA {
        DWORD dwSignature;
        DWORD dwMessage;
        NOTIFYICONDATA32 nid;
} TRAYNOTIFYDATAA, *PTRAYNOTIFYDATAA;
typedef struct _TRAYNOTIFYDATAW {
        DWORD dwSignature;
        DWORD dwMessage;
        NOTIFYICONDATA32 nid;
} TRAYNOTIFYDATAW, *PTRAYNOTIFYDATAW;
#ifdef UNICODE
typedef TRAYNOTIFYDATAW TRAYNOTIFYDATA;
typedef PTRAYNOTIFYDATAW PTRAYNOTIFYDATA;
#else
typedef TRAYNOTIFYDATAA TRAYNOTIFYDATA;
typedef PTRAYNOTIFYDATAA PTRAYNOTIFYDATA;
#endif // UNICODE

#define NI_SIGNATURE    0x34753423

#define WNDCLASS_TRAYNOTIFY     "Shell_TrayWnd"
//                          (WM_USER + 1) = NIN_KEYSELECT
#define NIF_VALID_V1    0x00000007
#define NIF_VALID       0x0000001F
//
// Old NT Compatibility stuff (remove later)
//
SHSTDAPI_(VOID) CheckEscapesA(LPSTR lpFileA, DWORD cch);
//                          (WM_USER + 1) = NIN_KEYSELECT
#define NIF_VALID_V1    0x00000007
#define NIF_VALID       0x0000001F
//
// Old NT Compatibility stuff (remove later)
//
SHSTDAPI_(VOID) CheckEscapesW(LPWSTR lpFileA, DWORD cch);
#ifdef UNICODE
#define CheckEscapes  CheckEscapesW
#else
#define CheckEscapes  CheckEscapesA
#endif // !UNICODE
SHSTDAPI_(LPSTR) SheRemoveQuotesA(LPSTR sz);
SHSTDAPI_(LPWSTR) SheRemoveQuotesW(LPWSTR sz);
#ifdef UNICODE
#define SheRemoveQuotes  SheRemoveQuotesW
#else
#define SheRemoveQuotes  SheRemoveQuotesA
#endif // !UNICODE
SHSTDAPI_(WORD) ExtractIconResInfoA(HANDLE hInst,LPSTR lpszFileName,WORD wIconIndex,LPWORD lpwSize,LPHANDLE lphIconRes);
SHSTDAPI_(WORD) ExtractIconResInfoW(HANDLE hInst,LPWSTR lpszFileName,WORD wIconIndex,LPWORD lpwSize,LPHANDLE lphIconRes);
#ifdef UNICODE
#define ExtractIconResInfo  ExtractIconResInfoW
#else
#define ExtractIconResInfo  ExtractIconResInfoA
#endif // !UNICODE
SHSTDAPI_(int) SheSetCurDrive(int iDrive);
SHSTDAPI_(int) SheChangeDirA(register CHAR *newdir);
SHSTDAPI_(int) SheChangeDirW(register WCHAR *newdir);
#ifdef UNICODE
#define SheChangeDir  SheChangeDirW
#else
#define SheChangeDir  SheChangeDirA
#endif // !UNICODE
SHSTDAPI_(int) SheGetDirA(int iDrive, CHAR *str);
SHSTDAPI_(int) SheGetDirW(int iDrive, WCHAR *str);
#ifdef UNICODE
#define SheGetDir  SheGetDirW
#else
#define SheGetDir  SheGetDirA
#endif // !UNICODE
SHSTDAPI_(BOOL) SheConvertPathA(LPSTR lpApp, LPSTR lpFile, UINT cchCmdBuf);
SHSTDAPI_(BOOL) SheConvertPathW(LPWSTR lpApp, LPWSTR lpFile, UINT cchCmdBuf);
#ifdef UNICODE
#define SheConvertPath  SheConvertPathW
#else
#define SheConvertPath  SheConvertPathA
#endif // !UNICODE
SHSTDAPI_(BOOL) SheShortenPathA(LPSTR pPath, BOOL bShorten);
SHSTDAPI_(BOOL) SheShortenPathW(LPWSTR pPath, BOOL bShorten);
#ifdef UNICODE
#define SheShortenPath  SheShortenPathW
#else
#define SheShortenPath  SheShortenPathA
#endif // !UNICODE
SHSTDAPI_(BOOL) RegenerateUserEnvironment(PVOID *pPrevEnv,
                                        BOOL bSetCurrentEnv);
SHSTDAPI_(INT) SheGetPathOffsetW(LPWSTR lpszDir);
SHSTDAPI_(BOOL) SheGetDirExW(LPWSTR lpszCurDisk, LPDWORD lpcchCurDir,LPWSTR lpszCurDir);
SHSTDAPI_(DWORD) ExtractVersionResource16W(LPCWSTR  lpwstrFilename, LPHANDLE lphData);
SHSTDAPI_(INT) SheChangeDirExA(register CHAR *newdir);
SHSTDAPI_(INT) SheChangeDirExW(register WCHAR *newdir);
#ifdef UNICODE
#define SheChangeDirEx  SheChangeDirExW
#else
#define SheChangeDirEx  SheChangeDirExA
#endif // !UNICODE

//
// PRINTQ
//
VOID Printer_LoadIconsA(LPCSTR pszPrinterName, HICON* phLargeIcon, HICON* phSmallIcon);
//
// PRINTQ
//
VOID Printer_LoadIconsW(LPCWSTR pszPrinterName, HICON* phLargeIcon, HICON* phSmallIcon);
#ifdef UNICODE
#define Printer_LoadIcons  Printer_LoadIconsW
#else
#define Printer_LoadIcons  Printer_LoadIconsA
#endif // !UNICODE
LPSTR ShortSizeFormatA(DWORD dw, LPSTR szBuf);
LPWSTR ShortSizeFormatW(DWORD dw, LPWSTR szBuf);
#ifdef UNICODE
#define ShortSizeFormat  ShortSizeFormatW
#else
#define ShortSizeFormat  ShortSizeFormatA
#endif // !UNICODE
LPSTR AddCommasA(DWORD dw, LPSTR pszResult);
LPWSTR AddCommasW(DWORD dw, LPWSTR pszResult);
#ifdef UNICODE
#define AddCommas  AddCommasW
#else
#define AddCommas  AddCommasA
#endif // !UNICODE

BOOL Printers_RegisterWindowA(LPCSTR pszPrinter, DWORD dwType, PHANDLE phClassPidl, HWND *phwnd);
BOOL Printers_RegisterWindowW(LPCWSTR pszPrinter, DWORD dwType, PHANDLE phClassPidl, HWND *phwnd);
#ifdef UNICODE
#define Printers_RegisterWindow  Printers_RegisterWindowW
#else
#define Printers_RegisterWindow  Printers_RegisterWindowA
#endif // !UNICODE
VOID Printers_UnregisterWindow(HANDLE hClassPidl, HWND hwnd);

#define PRINTER_PIDL_TYPE_PROPERTIES       0x1
#define PRINTER_PIDL_TYPE_DOCUMENTDEFAULTS 0x2
#define PRINTER_PIDL_TYPE_ALL_USERS_DOCDEF 0x3
#define PRINTER_PIDL_TYPE_JOBID            0x80000000
//
// Internal APIs Follow.  NOT FOR PUBLIC CONSUMPTION.
//

//====== ShellMessageBox ================================================

// If lpcTitle is NULL, the title is taken from hWnd
// If lpcText is NULL, this is assumed to be an Out Of Memory message
// If the selector of lpcTitle or lpcText is NULL, the offset should be a
//     string resource ID
// The variable arguments must all be 32-bit values (even if fewer bits
//     are actually used)
// lpcText (or whatever string resource it causes to be loaded) should
//     be a formatting string similar to wsprintf except that only the
//     following formats are available:
//         %%              formats to a single '%'
//         %nn%s           the nn-th arg is a string which is inserted
//         %nn%ld          the nn-th arg is a DWORD, and formatted decimal
//         %nn%lx          the nn-th arg is a DWORD, and formatted hex
//     note that lengths are allowed on the %s, %ld, and %lx, just
//                         like wsprintf
//

int _cdecl ShellMessageBoxA(
    HINSTANCE hAppInst,
    HWND hWnd,
    LPCSTR lpcText,
    LPCSTR lpcTitle,
    UINT fuStyle, ...);
int _cdecl ShellMessageBoxW(
    HINSTANCE hAppInst,
    HWND hWnd,
    LPCWSTR lpcText,
    LPCWSTR lpcTitle,
    UINT fuStyle, ...);
#ifdef UNICODE
#define ShellMessageBox  ShellMessageBoxW
#else
#define ShellMessageBox  ShellMessageBoxA
#endif // !UNICODE

//====== Random stuff ================================================

SHSTDAPI_(BOOL) IsLFNDriveA(LPCSTR pszPath);
SHSTDAPI_(BOOL) IsLFNDriveW(LPCWSTR pszPath);
#ifdef UNICODE
#define IsLFNDrive  IsLFNDriveW
#else
#define IsLFNDrive  IsLFNDriveA
#endif // !UNICODE


#if (_WIN32_WINNT >= 0x0500) || (_WIN32_WINDOWS >= 0x0500)  

//
// The SHMultiFileProperties API displays a property sheet for a 
// set of files specified in an IDList Array.
//
// Parameters:
//      pdtobj  - Data object containing list of files.  The data
//                object must provide the "Shell IDList Array" 
//                clipboard format.  The parent folder's implementation of
//                IShellFolder::GetDisplayNameOf must return a fully-qualified
//                filesystem path for each item in response to the 
//                SHGDN_FORPARSING flag.
//
//      dwFlags - Reserved for future use.  Should be set to 0.
//
// Returns:
//      S_OK
// 
SHSTDAPI SHMultiFileProperties(IDataObject *pdtobj, DWORD dwFlags);

#endif 


#ifdef __cplusplus
}
#endif  /* __cplusplus */

#include <poppack.h>
#endif  /* _SHELAPIP_ */
