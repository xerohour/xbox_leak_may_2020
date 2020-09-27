#ifndef __TNCONTROL_PROCS__
#define __TNCONTROL_PROCS__
//#pragma message("Defining __TNCONTROL_PROCS__")




//==================================================================================
// External Callback Definitions
//==================================================================================
typedef HRESULT	(__stdcall *PTNLOADTESTTABLEPROC)			(PTNTESTTABLEGROUP);

typedef HRESULT	(__stdcall *PTNLOGSTRINGPROC)				(DWORD, char*);

typedef HRESULT	(__stdcall *PTNGETSTARTUPDATAPROC)			(PVOID, DWORD*);

typedef HRESULT	(__stdcall *PTNLOADSTARTUPDATAPROC)			(PLSTRINGLIST);

typedef HRESULT	(__stdcall *PTNWRITESTARTUPDATAPROC)		(PLSTRINGLIST);

typedef HRESULT	(__stdcall *PTNDOSTARTUPPROC)				(PTNMACHINEINFO, PVOID,
															DWORD, BOOL*);

typedef HRESULT	(__stdcall *PTNINITIALIZETESTINGPROC)		(PTNMACHINEINFO);

typedef HRESULT	(__stdcall *PTNCLEANUPTESTINGPROC)			(PTNMACHINEINFO);

typedef HRESULT (__stdcall *PTNGETTESTINGWINDOWPROC)		(HWND*);

typedef HRESULT	(__stdcall *PTNADDIMPORTANTBINARIESPROC)	(PTNTRACKEDBINARIES);

typedef HRESULT	(__stdcall *PTNHANDLEUSERQUERYPROC)			(DWORD, PVOID, DWORD,
															DWORD*, PVOID, DWORD*);

typedef HRESULT	(__stdcall *PTNCANRUNPROC)					(PTNCANRUNDATA);

typedef HRESULT	(__stdcall *PTNGETINPUTDATAPROC)			(PTNGETINPUTDATA);

typedef HRESULT	(__stdcall *PTNEXECCASEPROC)				(PTNEXECCASEDATA);

typedef HRESULT	(__stdcall *PTNCHANGEBINARYSETPROC)			(DWORD, PTNBINARYMANAGER);

typedef HRESULT	(__stdcall *PTNWRITEDATAPROC)				(PTNWRITEDATA);

typedef HRESULT	(__stdcall *PTNFILTERSUCCESSPROC)			(PTNFILTERSUCCESSDATA);

typedef HRESULT	(__stdcall *PTNPOKETESTCOMPLETEDPROC)		(PTNPOKETESTCOMPLETEDDATA);







#else //__TNCONTROL_PROCS__
//#pragma message("__TNCONTROL_PROCS__ already included!")
#endif //__TNCONTROL_PROCS__

