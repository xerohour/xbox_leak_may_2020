//
// Variables
//
extern CPtrList DLLGridInfoList;

//
// Functions
//
void ClearDLLInfo(HTARGET hTarget = NULL);
void PurgeDLLInfo(void);
void BuildLocalDLLsStringList(CStringList& slLocalDLLs, BOOL fPreloadOnly);
BOOL FGetLocalDLLFromRemoteDLL(CString& strLocal, CString strRemote);
BOOL FGetRemoteDLLFromLocalDLL(CString strLocal, CString& strRemote);
void AddRecToDLLInfo(CString strLocal, CString strRemote, BOOL fPreload, HTARGET hTarget);
void UpdateDLLInfoRec (CString	strLocal,
					   CString	strRemote,
					   BOOL		fPreload,
					   HTARGET	hTarget,
					   BOOL		fSearchOnLocal);

//
// Structs
//

// DLLGridInfoList contains a list of pointers to GRIDINFOs 
//	(one for each build target)
// Each GRIDINFO contains an HTARGET and a list of pointers to DLLRECs 
//	(one for each grid entry)
typedef struct {
	HTARGET hTarget;
	CPtrList DLLRecList;
} GRIDINFO;

