#ifndef DBGNEW_H
#define DBGNEW_H

/* New - taken from makeeng4.cpp */
IDE_EXPORT EXEFROM GetExecutableFilename ( PSTR executable, UINT size );

/* Taken from qlimits.h */
#define MAX_LINE_SIZE       2048
#define MAX_EXPRESS_SIZE    256
#define TMP_STRING_SIZE     MAX_LINE_SIZE + 60
#define ENVSTR_SIZE         1024
#define MAX_MSG_TXT         200
#define MAX_VAR_MSG_TXT     400
#define DOS_EXT_SIZE        6
#define CURSOR_WIDTH        2
#define MAX_TOOL_NB         16
#define MAX_STD_TOOL_NB     5
#define MAX_TOTAL_TOOLS     (MAX_TOOL_NB + MAX_STD_TOOL_NB)

/* Taken from util.h */
//ANSI/ASCII compatible isspace() test
// We really should use isspace() since it's faster.
#define whitespace(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')

/* Taken from meclass.h */
#define axMax       (256 + 128) // This should be plenty

/* taken from qcqp.h */
BOOL StopDebugConfirm(BOOL fFailSilently = FALSE);
BOOL EnsureActiveProjectOrJIT(BOOL & fUnknownTarget);

/* taken from menu.cpp */
UINT CommandIdEnabled(UINT CommandId, BOOL& Enabled);

/* from textview.h */
#define DATATIP_TIMER   101

/* new thingies */

#define NUM_ME_WINDOWS  6

typedef VOID (FAR PASCAL *OWNERDRAWCALLBACK)(HWND hDlg, LPDRAWITEMSTRUCT lpDIS);

HGLOBAL     HandleFromString(const CString & str);
HGLOBAL     HandleFromString(LPCTSTR str);
BOOL        StartDialog(UINT rcDlgNb,DLGPROC dlgProc);
BOOL        StartDialogParam (UINT rcDlgNb, DLGPROC dlgProc, LPVOID lpParam, UINT helpID);
BOOL        DefaultDialogHandler(HWND hDlg, unsigned message, UINT wParam, LONG lParam);
BOOL PASCAL fScanAnyLong(LPCTSTR lpszSource, long *plDest, long lMin, long lMax);
void PASCAL freeze_view (void);
BOOL        FileExist(LPCSTR fileName);
BOOL PASCAL ValidFilename( LPCSTR lpszName, BOOL fWildOkay );
char *      NormalizePath (LPSTR lpProjDir, LPSTR lpSearchDir, LPSTR lpFileDir, LPSTR lpPath, int cbPath);
void        EnsureFinalBackslash(LPSTR path);
BOOL        FsSetCurDir(LPCTSTR pathName);
BOOL        IsNonLocalDecimalPoint();
BOOL        InitDebugPackage(HINSTANCE);
void        InitDefaults(void);
void        InitDefaultRunDebugParams(RUNDEBUGPARAMS *);
BOOL        DefaultDialogHandler(HWND, UINT, UINT, LONG);
BOOL PASCAL HandleStandardOwnerDrawListBox(HWND, UINT, WPARAM, LPARAM, UINT, OWNERDRAWCALLBACK);
void PASCAL EnsureDebugWindow(int winType);
void        EnsureShellNotMinimized( void );
HWND        GetShellWindow( void );
long PASCAL MainWndProc(HWND, UINT, UINT, LONG);
HWND        GetDebugWindowHandle(UINT);
CView *     CreateView(int);
CView *     GetCurView(void);
BOOL PASCAL OpenDebugWindow(UINT);
BOOL        FEvalExpr(char *szExpr, char *szOut, int cbOut);
void        RemoveSideEffectsExpr(char *szExpr);

// Internal error handling
VOID PASCAL     SetGlobalError( ERR_TYPE errnum, BOOL fForce ) ;
ERR_TYPE PASCAL CheckGlobalError( BOOL fClear ) ;
VOID PASCAL     ClearGlobalError( VOID ) ;

// This function extracts a suitable top level parent window
HWND PASCAL _SushiGetSafeOwner(CWnd* pParent);

// INI file stuff
void HammerOleRegKey( void );
void HammerLoadExportsRegKey( void );
IDE_EXPORT void MarkIniFileAsDirty(void);

#endif  // DBGNEW_H

