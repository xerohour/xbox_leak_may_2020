/////////////////////////////////////////////////////////////////////////////
//	LANGAPI.H
//		Language package interface declarations.

#ifndef __LANGAPI_H__
#define __LANGAPI_H__

class CLex; 	// Non-standard COM.
class CParser; 	// Non-standard COM.

typedef CLex* PLEXER;
typedef CParser* PPARSER;

interface ILangPackage;

typedef ILangPackage* LPLANGPACKAGE;

/////////////////////////////////////////////////////////////////////////////
// ILangPackage

#undef  INTERFACE
#define INTERFACE ILangPackage
DECLARE_INTERFACE_(ILangPackage, IUnknown)
{
	// IUnknown methods
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID FAR* ppvObj) PURE;
	STDMETHOD_(ULONG,AddRef)(THIS) PURE;
	STDMETHOD_(ULONG,Release)(THIS) PURE;

	// ILangPackage methods
	STDMETHOD(RegisterLexers)(PLEXER **pppLexers) PURE;
	STDMETHOD(RegisterParsers)(PPARSER **pppParsers) PURE;
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __LANGAPI_H__
