//#define JAPANESE

// Common settings
#define SHORTFILELEN MAX_PATH    //(8 + 1 + 3 + 1)

// User Messages
#define WM_STATUSTEXT WM_USER+16	// wParam = column num, lParam = (LPCTSTR) text

extern UINT g_suFileGroupDesc;
extern UINT g_suFileContents;
extern UINT g_suXboxObject, g_suXboxFiles, g_suPrefDropEffect;
extern UINT g_suPerfDropEffect;
extern UINT g_suPasteSucceeded;

extern LISTVIEW listview;
extern TOOLVIEW toolview;
extern HWND g_hwnd;
extern int g_iPopupFileItem;
extern XBFU xbfu;
extern HINSTANCE hInst;
extern char g_TargetXbox[256];
extern char g_CurrentDir[MAX_PATH];
extern BOOL splashscreen;

void CenterDialog( HWND );
extern char szAppName[];
extern HISTORY history;
extern char g_szDelFile[MAX_PATH];

// widebyte stuff
extern int MyStrcmp(const char *sz1, const char *sz2);
extern char *MyStrdup(char *sz);
extern int MyStricmp(const char *sz1, const char *sz2);
extern int MyStrlen(const char *sz);
extern char gs_szUtoA[1024];
char *WideToSingleByte(WCHAR *pwIn);


class CXboxConnection;
extern CXboxConnection *g_pxboxconn;

extern char *FormatBigNumber(ULONGLONG n);
extern char *FormatNumber(int n);
extern void FormatSize(ULARGE_INTEGER ulSize, char *szBuf, bool fBytes);
