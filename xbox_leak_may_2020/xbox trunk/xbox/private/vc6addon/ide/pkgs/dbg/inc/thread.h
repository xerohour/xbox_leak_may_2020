
// Wndproc for the thread dialog box
BOOL FAR PASCAL EXPORT DlgThread(
		  HWND       hDlg,
		  unsigned   message,
		  UINT       wParam,
		  LONG       lParam);

// Fields padding for the listbox
#define THREAD_FIELD1       1                           // fCurrent
#define THREAD_FIELD2       (THREAD_FIELD1 + 1 + 8)     // id
#define THREAD_FIELD3       (THREAD_FIELD2 + 1 + 6)     // cSuspend
#define THREAD_FIELD4       (THREAD_FIELD3 + 1 + 4)     // szPriority
#define THREAD_FIELD5       (THREAD_FIELD4 + 1 + 36)    // szLocation

// Structure for storing the exceptions
typedef struct {
	DWORD id;		// NT Thread ID
	DWORD cSuspend;	// Suspend count
	DWORD cSuspendNew; // New suspend count after user modifications in dialog
	CHAR szPriority[STATESTRINGSIZE]; // Thread priority, as string from OSDebug
	DWORD address;	// Thread address
	CHAR Location[THREAD_FIELD5-THREAD_FIELD4+1]; // Thread function name
	BOOL HasFocus;	// Is this the current thread ?
	}THREAD_OBJECT;


// used to give/kill focus to a thread
#define THREAD_FOCUS				1000
#define THREAD_KILLFOCUS			1001
