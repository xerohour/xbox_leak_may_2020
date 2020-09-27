#ifndef __DESIGNER_H__
#define __DESIGNER_H__

// designer.h : header file
//

#define PROJ_APPLICATION	0x0001
#define PROJ_DLL			0x0002
#define PROJ_UNICODE		0x0004
#define PROJ_CONSOLE_APP	0x0008
#define PROJ_STATIC_LIB		0x0010
#define PROJ_GEN_PROJ		0x0020
#define PROJ_NEW_WORKSPACE	0x1000

#define ITEM_INTERFACE		0x0001
#define ITEM_DISPINTERFACE	0x0002
#define ITEM_CLASS			0x0004
#define ITEM_FUNCTION		0x0008
#define ITEM_VARIABLE		0x0010
#define ITEM_METHOD			0x0020
#define ITEM_PROPERTY		0x0040
#define ITEM_MAP			0x0080
#define ITEM_FILES			0x0100

/////////////////////////////////////////////////////////////////////////////
// ILangDesigner
//   This is an interface to get to language specific stuff.  Current focus: C++

#undef  INTERFACE
#define INTERFACE ILangDesigner
DECLARE_INTERFACE_(ILangDesigner, IDispatch)
{
#ifndef NO_BASEINTERFACE_FUNCS

	// IUnknown methods
	STDMETHOD_(ULONG,AddRef)() PURE;
	STDMETHOD_(ULONG,Release)() PURE;
	STDMETHOD(QueryInterface)(REFIID riid, LPVOID FAR* ppvObj) PURE;

    // IDispatch methods
    STDMETHOD(GetTypeInfoCount)(UINT FAR* pctinfo) PURE;
    STDMETHOD(GetTypeInfo)(UINT itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo) PURE;
    STDMETHOD(GetIDsOfNames)(REFIID riid, OLECHAR FAR* FAR* rgszNames, UINT cNames,
		LCID lcid, DISPID FAR* rgdispid) PURE;
    STDMETHOD(Invoke)(DISPID dispidMember, REFIID riid, LCID lcid, WORD wFlags,
		DISPPARAMS FAR* pdispparams, VARIANT FAR* pvarResult, EXCEPINFO FAR* pexcepinfo,
		UINT FAR* puArgErr) PURE;

#endif

	// ILangDesigner methods
	STDMETHOD(CreateProject)(LPCOLESTR pwstrProjName, long flags, BSTR *pbstrBuilder) PURE;
	STDMETHOD(GetActiveBuilderName)(BSTR *pbstrBuilder) PURE;
	STDMETHOD(AddFile)(LPCOLESTR pwstrBuilder, LPCOLESTR pwstrFileName, long flags) PURE;

	// Project Query methods
	STDMETHOD(GetAllMembers)(LPCOLESTR pwstrBuilder, 
		LPCOLESTR pwstrName, long flags, VARIANT *pvarNames, VARIANT *pvarAttribs) PURE;

	STDMETHOD (GetSourceInformation)
		(LPCOLESTR pwstrBuilder,
		LPCOLESTR pwstrName, 
		long flags, 
		BSTR *pbstrDefn,  
		long *plDefnStart, 
		long *plDefnEnd,
		BSTR *pbstrDecl, 
		long *plDeclStart,
		long *plDeclEnd) PURE;

	STDMETHOD(GetActiveXAssociation)(LPCOLESTR pwstrBuilder, 
		LPCOLESTR pwstrInterfaceName, VARIANT *pvtClassNames) PURE;
	STDMETHOD(GetFileInfo)(LPCOLESTR pwstrBuilder,
		LPCOLESTR pwstrFileName, long flags, long *lastIncludeLine, VARIANT *pvtIncludes) PURE;
	STDMETHOD(GetSourceInformationEx) (
				LPCOLESTR pwstrBuilder, 
				LPCOLESTR pwstrName, 
				long flags, 
				VARIANT *pvarDeclaration, // This gives the complete declaration
				VARIANT *pvarDefnFile,  
				VARIANT *pvarDefnStart, 
				VARIANT *pvarDefnEnd,
				VARIANT *pvarDeclFile, 
				VARIANT *pvarDeclStart,
				VARIANT *pvarDeclEnd) PURE;
};

/////////////////////////////////////////////////////////////////////////////

#endif //__DESIGNER_H__
 
