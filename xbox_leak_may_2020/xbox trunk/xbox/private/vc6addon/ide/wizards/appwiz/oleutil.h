// oledlgs.h : header file
//

#ifndef _OLEUTIL_H
#define _OLEUTIL_H

#include <objbase.h>
#include <scode.h>


/////////////////////////////////////////////////////////////////////////////
// Helpers for OLE dynamic load

// This struct contains the OLE entry points that we use in ClassWizard
// These are filled in dynamically at runtime if the DLLs happen to exist
//	on the system.
/* struct OleEntryPoints
{
	// from OLE2.DLL
	HRESULT (FAR STDAPICALLTYPE EXPORT* lpOleInitialize)(IMalloc FAR*);
	void (FAR STDAPICALLTYPE EXPORT* lpOleUninitialize)();
	HRESULT (FAR STDAPICALLTYPE EXPORT* lpCoCreateGuid)(GUID FAR*);
	HRESULT (FAR STDAPICALLTYPE EXPORT* lpCLSIDFromString)(LPSTR lpsz, LPCLSID pclsid);	
};

extern OleEntryPoints NEAR oleEntryPoints;
void PASCAL LoadOleEntryPoints(BOOL bNeedTypeLib);	
	// loads above global (call at init time)
	
void PASCAL UnloadOleEntryPoints();	// unloads above global (call at term time)

BOOL PASCAL IsOleLoaded();	// checks for non-typelib entry points
BOOL PASCAL CheckOleVersion();

BOOL PASCAL IsOleInstalled();
*/
BOOL PASCAL GetValidGuid(GUID FAR& guid);
#endif

