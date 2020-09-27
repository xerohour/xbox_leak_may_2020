// dllapi.h
//

#ifndef __DLLAPI_H__
#define __DLLAPI_H__

#include <tchar.h>
#include <stdlib.h>
#include <windows.h>

// prototype struct for name->pfn mapping
struct MPSZPFN
{
	_TCHAR *szRoutine;
	FARPROC pfn;
};

#define DECLARE_DLLAPI(var, map, dll)	CDllApi var(#dll, g_mpszpfn##map)
#define BEGIN_MPSZPFN(var)				MPSZPFN g_mpszpfn##var[] = {
#define MPSZPFN_FCN(name)				{#name, 0},
#define END_MPSZPFN(var)				{0,0}};

class CDllApi
{
	CDllApi() { }				// private default ctor--have to use the real one below
	BOOL _fRoutinesOK;			// all routines loaded successfully
	UINT _cRoutines; 			// count of routines in list
	HINSTANCE _hInstance;		// handle to the dll
	MPSZPFN *_mpszpfn;			// mapping from name to pfn
	_TCHAR _szDll[_MAX_PATH];	// dll name

public:
	CDllApi(const _TCHAR *szDll, MPSZPFN *);
	~CDllApi();
	BOOL FLoaded() const { return _hInstance != 0; }
	BOOL FRoutinesLoaded () const { return _fRoutinesOK; }
	FARPROC operator() (UINT iRoutine) {return _mpszpfn[ iRoutine ].pfn; }
};

#endif	// __DLLAPI_H__
