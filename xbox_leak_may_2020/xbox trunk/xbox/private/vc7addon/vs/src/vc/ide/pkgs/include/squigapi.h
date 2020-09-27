#ifndef __SQUIGAPI_H__
#define __SQUIGAPI_H__

// squigapi.h : header file
//

#define assert _ASSERT
#define dassert _ASSERT
#include <array_t.h>
#include <ParseErr.h>

#define WM_PARSER_COPYTOFILE	(WM_USER + 701)
#define WM_PARSER_SHOWSQUIGGLY	(WM_USER + 702)

struct CSItem {
	DWORD dwBeginLine;
	DWORD dwEndLine;
	WORD wBeginColumn;
	WORD wEndColumn;
	SZ   szErrId;
	SZ	 szErrTxt;
	SZ	 szFileName;
	DWORD dwFlags;
};

enum ARRAY_TYPE {ReadArray, WriteArray};
/////////////////////////////////////////////////////////////////////////////
// IVcCodeSense
//   This is an interface to maintain VC specific code sense information

#undef  INTERFACE
#define INTERFACE IVcCodeSense
DECLARE_INTERFACE_(IVcCodeSense, IUnknown)
{
#ifndef NO_BASEINTERFACE_FUNCS

	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)() PURE;
	STDMETHOD_(ULONG,Release)() PURE;
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj) PURE;
#endif

	// IVcCodeSense methods
	STDMETHOD(Add)(DWORD dwErrId, DWORD dwErrType, SZ szFileName,
					DWORD begLine, DWORD endLine, WORD begCol, WORD endCol, SZ szErrTxt) PURE;
	STDMETHOD(Clear) (ARRAY_TYPE) PURE;
	STDMETHOD(Refresh) () PURE;
	STDMETHOD(GetErrorCount) (DWORD *dwErrorCount) PURE;
	STDMETHOD(RetrieveErrorInfo) (Array<CSItem>& rgCSItems) PURE; 
};

/////////////////////////////////////////////////////////////////////////////

#endif //__SQUIGAPI_H__
 
