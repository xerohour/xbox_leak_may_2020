
// Field widths for listbox, INCLUDING terminating NULLs
#define cchExcepNumber	9
#define cchExcepName	42
#define cchExcepAction	26

// Width for a string containing all three of the above, separated by tabs
#define cchExcepAll		(cchExcepNumber + cchExcepName + cchExcepAction)

#define iPlatformGeneric	((UINT)-1)
#define IsForCurrentPlatform(id)	((BOOL)(pDebugCurr->GetPlatform()==id)||id==iPlatformGeneric)

// Structure for storing the exceptions
typedef struct {
	DWORD	dwExceptionCode;		// Exception number
	char	Name[cchExcepName];		// Exception description
	int		iAction;				// Exception action 
									// (0-1 = index of string in combobox)
	UINT	iPlatformId; 			// Platform for exception
									// iPlatformGeneric == all platforms,
									// user added
} EXCEPTION_OBJECT;


// Exception actions (Action field of EXCEPTION_OBJECT)
#define EX_ACTION_1STCHANCE		DLG_Exception_1stChance - DLG_Exception_Action_First
#define EX_ACTION_2NDCHANCE		DLG_Exception_2ndChance - DLG_Exception_Action_First

void CopyListbox(HWND hDlg, UINT id);

BOOL GetExceptionInfo(
	BOOL FirstException,
	BOOL fSystemOnly,
	DWORD *pdwExceptionCode,
	LPSTR Name,
	UINT cchName,
	UINT *piAction,
	UINT *piPlatformId);

void GetExceptionDescription(DWORD dwExceptionCode, CString& strDescription);

int GetExceptionAction(DWORD dwExceptionCode);

void MakeExceptionObject(EXCEPTION_OBJECT *pObject, DWORD dwExceptionCode,
	LPCSTR Name, int iAction, UINT iPlatformId);

void UnmakeExceptionObject(EXCEPTION_OBJECT *pObject, DWORD *pdwExceptionCode,
	LPSTR Name, int *piAction, UINT *piPlatformId);

void MakeExceptionString(EXCEPTION_OBJECT *Object, LPSTR Buffer, int cch);

BOOL FAR PASCAL EXPORT DlgException(HWND, unsigned, UINT, LONG);

VOID InitProcessExceptions(VOID);
