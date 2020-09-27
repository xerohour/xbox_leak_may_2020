//
// CProjComponentMgr, CProjType
//
// Project component management, ie. target types, tools and option tables
//
// [matthewt]
//

#ifndef _INCLUDE_PRJCONFG_H
#define _INCLUDE_PRJCONFG_H

#include "projprop.h"			// MFC usage ids
#include "dlgbase.h"
#include "oletool.h"

#include <bldapi.h>
#include <bldguid.h>

//
// Resource ID ranges for each of the tool packages.
//
// In the absence of a resource loading mechanism that understands the
// context (package) that is to be loaded we need to have unique resource ids
// for those resources belonging to each package. Build add_ons therefore
// have been allocated the following ranges. Additional add_ons should add a new
// range here,
//
// BASE - 0x3000 (12288), UPPER - 0x3fff (16383)
//
// Intelx86	0x3000
// Mac68k	0x3200
// Mips		0x3400
// Alpha	0x3600
// PowerMac	0x3800
// FORTRAN	0x3A00
// PowerPC	0x3B00

// declare our types used up-front
class CPlatform;			// platform (defined in 'prjconfg.h')
class CProjType;			// project type (defined in 'prjconfg.h')
class CProjItem;			// project item (defined in 'projitem.h')
class CBuildTool;			// build tool (defined in 'projtool.h')
class CProjTypeInterface;	// com interface on CProjType.
/*
R.I.P. for v4.0 with VBA?
class CCustomTool;			// custom build tool (defined in 'projtool.h')
*/
class CProject;				// project (defined in 'project.h')
class COptionHandler;		// tool option handler (defined in 'prjoptn.h')
class CBuilderConverter;	// builder (.mak) converter (defoned in 'prjconfg.h')

// Helper function -- probably belongs in some new module PROJUTIL.CPP / PROJUTIL.H.
void GetNewOutputDirName(CProject * pProject, const CString & strNewConfig, CString & strResult);

// declare the global tool option engine (parsing & generation)
class COptionTable;
extern BLD_IFACE COptionTable g_prjoptengine;

// functions to munge/extract the project mode from the configuration
void DefaultConfig(CProject * pProject, const CString & strPlatformName, UINT nIDMode, CString & strConfig);

// build system component registration

// component types
#define BLC_ERROR		0x0000
#define BLC_Platform	0x0001
#define BLC_Tool		0x0002
#define BLC_Scanner		0x0003
#define BLC_OptionHdlr	0x0004
#define BLC_TargetType	0x0005
#define BLC_BldrCnvtr	0x0006
#define BLC_CustomTool	0x0007
#define BLC_OleAddOn	0x0008

#define BLC_Start		BLC_Platform
#define BLC_End			BLC_OleAddOn

// mode of our tool options
#define MOB_Debug		0x1			// debug
#define MOB_Release		0x2			// release

// our CBldSysCmp class ... the base-class for all component manager objects
//
// provides:
// o storing/retrieving of component properties to the system registry 'Visual C++\Build System' key.
// (use REGISTRY_MAPS)

typedef struct
{
	enum {MapBegin, MapEnd, MapData, MapFData} regType;	// type of this registry entry
	UINT  nRegId;	// string resource ID for registry field
	UINT  nRegSubKeyId;	// string resource ID for sub-key
	const TCHAR *szRegField;	// string resource ID for registry field
	const TCHAR *szRegSubKey;	// string resource ID for sub-key
	DWORD dwType;			// type of data, eg. REG_SZ, REG_BINARY etc.
	const void * pvData;	// points to the data
	DWORD cbData;			// size of this data
} CRegistryMap;

// other registry entry type #defines for use by REGISTRY_ENTRY()
#define REG_STRING	(DWORD)(REG_NONE - REG_SZ)	// our own registry type for CStrings
#define REG_BOOL	(DWORD)REG_DWORD

#define NO_SUB_KEY NULL

#define DECLARE_REGISTRY_MAP() \
	static CRegistryMap m_registryMap[]; \
	virtual const CRegistryMap * GetRegistryMap() const {return &m_registryMap[0];}

#define BEGIN_REGISTRY_MAP(theClass, parentClass) \
	CRegistryMap theClass::m_registryMap[] = {\
	{CRegistryMap::MapBegin, 0, 0, NULL, NULL, NULL /* no type */, &parentClass::m_registryMap[0], 0},

#define BEGIN_REGISTRY_MAP_P(theClass) \
	CRegistryMap theClass::m_registryMap[] = {\
	{CRegistryMap::MapBegin, 0, 0, NULL, NULL, NULL /* no type */, NULL /* no base */, 0},

#define REGISTRY_DATA_ENTRY(regid, subkey, regstr, substr, t, var) \
	{CRegistryMap::MapData, regid, subkey, regstr, substr, t, &((theClass*)0)->var, sizeof(((theClass*)0)->var)},

#define REGISTRY_FAKE_DATA_ENTRY(regid, subkey, regstr, substr, t) \
	{CRegistryMap::MapFData, regid, subkey, regstr, substr, t, NULL, 0},

#define END_REGISTRY_MAP() \
	{CRegistryMap::MapEnd, 0, 0, NULL /* no type */, NULL}};

// helpers
LONG RegGetKey(HKEY, LPCTSTR, PHKEY, BOOL fCreate = TRUE);

// declare the global configuration manager
class CProjComponentMgr;
class BLD_IFACE CBldSysCmp : public CSlob
{
	friend class CProjComponentMgr;

	DECLARE_DYNCREATE(CBldSysCmp)
	DECLARE_SLOBPROP_MAP()
	DECLARE_REGISTRY_MAP()

public:
	CBldSysCmp();

	// get/set the component's and related component's id
	__inline DWORD CompId() {return m_dwId;}
	__inline DWORD RelatedCompId() {return m_dwRelatedId;}

	void SetId(DWORD dwId);
	void SetRelatedId(DWORD dwRelatedId);

	// cloning
	virtual CSlob * Clone();

	// storing and retrieving of component properties to system registry
	//
	// this will ensure that the component info. is in the registry,
	// any changes in the information in the registry will be copyed to
	// the component
	virtual BOOL SynchronizeWithRegistry(BOOL fSaveBeforeLoad = TRUE);

	// serialize component to character buffer (zero-term. EOL)
	virtual BOOL Serialize(TCHAR * pch, UINT & cch);

	// deserialize component from character buffer (zero-term. EOL)
	virtual BOOL Deserialize(TCHAR * pch, UINT cch);

	virtual BOOL RegDataExists(const CRegistryMap *, HKEY, CString *, LPDWORD, LPDWORD);
 	virtual BOOL LoadRegData(const CRegistryMap *, HKEY, CString *, LPBYTE, DWORD);
 	virtual BOOL SaveRegData(const CRegistryMap *, HKEY, CString *, LPBYTE, DWORD);

	// returns TRUE if this is a fully supported component,
	// FALSE if it is a fake one for compatibility (ie. unknown tools etc.)
	virtual BOOL IsSupported() const { return TRUE; }

	// helpers
	BOOL RegGetDataKey(const CRegistryMap *, PHKEY, BOOL fCreate = TRUE);

	BOOL LoadRegData(UINT nRegId);
	BOOL SaveRegData(UINT nRegId);

	// get our regular and 'UI' component name
	__inline const CString * GetName() const {return &m_strCompName;}
	__inline const CString * GetUIName() const {return &m_strUICompName;}

	// get component system registry 'parent' key
	__inline const CString * GetRegKeyName() const {return &g_strRegKey;}

	// get component system registry key
	const TCHAR * GetCompRegKeyName(BOOL fInclAppKey = TRUE);

	// map types (n.b. 32 is a hack, just a number greater than REG_* in winnt.h)
	__inline DWORD MapRegType(DWORD dwType) {return dwType < 32 ? dwType : (DWORD)REG_NONE - dwType;}

	// name of this comp.
	CString m_strCompName;

	// UI name of this comp.
	CString m_strUICompName;

	// restore out component's data from the registry
	// this is FALSE by default unless the "Version"
	// value shows a valid component
	BOOL m_fRegRestore;

protected:
	// retrieve the registry map entry for the entry with the id, 'nRegId'
	const CRegistryMap * GetRegMapEntry(UINT nRegId);

	// registry keys
	static const CString g_strRegKey;

	CString m_strCompRegKey;
	CString m_strFullCompRegKey;
	BOOL m_fCompRegKeyDirty;

	// id of this bld system component
	DWORD m_dwId;

	// id of the related bld system component
	DWORD m_dwRelatedId;
};

// our CProjComponentMgr class ... the  global project component manager

// generate a component id based on the package and component id (latter is unique to package only)
#define GenerateComponentId(idPackage, idComponent) ((idPackage << 16) | (idComponent & 0xffff))
#define PackageIdFromBscId(bsc_id) (bsc_id >> 16)
#define PkgCmpIdFromBscId(bsc_id) (bsc_id & 0xffff)

class BLD_IFACE CProjComponentMgr : public CObject
{
public:
	// create project component manager
	CProjComponentMgr();
	~CProjComponentMgr();

	// initialise (ask packages to register their components)
	BOOL FInit();
	void IsInit(BOOL b){m_fInit=b;};
	BOOL IsInit(){return m_fInit;};
	BOOL m_fInit;

	// terminate
	void Term();

	// build system component registration

	// generate package id using the package name
	WORD GenerateBldSysCompPackageId(const TCHAR * szPackageName);

	// package register component
	// pls. note that if no bsc_id passed in it is assumed the component
	// has pre-set it's id.
	BOOL RegisterBldSysComp(CBldSysCmp * pcomp, DWORD bsc_id = (DWORD)-1);

	// lookup a component
	__inline BOOL LookupBldSysComp(DWORD bsc_id, CBldSysCmp *& pcomp)
		{return m_mapBldSysCmps.Lookup((void *)bsc_id, (void *&)pcomp);}

	// get a component type, returns 'BLC_ERROR' if not found
	DWORD BldSysCompType(DWORD bsc_id);
 	DWORD BldSysCompType(CBldSysCmp * pcomp);

	// utility functions
	BOOL GetProjTypefromProjItem(CProjItem * pprojitem, CProjType * & pprojtype);

	//
	// our platform management
	//
	void RegisterPlatform(CPlatform *);
	BOOL DeregisterPlatform(CPlatform *);

	int GetPlatformCount() { return m_lstPlatforms.GetCount(); }
	int GetSupportedPlatformCount();

	const CPtrList *GetPlatforms() { return &m_lstPlatforms; }
	void InitPlatformEnum() { m_posPlatform = m_lstPlatforms.GetHeadPosition(); }
	BOOL NextPlatform(CPlatform * & pPlatform)
	{
		if (m_posPlatform == (POSITION)NULL)
			return FALSE;
		pPlatform = (CPlatform *)m_lstPlatforms.GetNext(m_posPlatform);
		return TRUE;
	}

	// duplicate functions to allow nested enumerations to happen (through interfaces)
	void InitPlatformEnumExport() { m_posPlatExp = m_lstPlatforms.GetHeadPosition(); }
	BOOL NextPlatformExport(CPlatform * & pPlatform)
	{
		if (m_posPlatExp == (POSITION)NULL)
			return FALSE;
		pPlatform = (CPlatform *)m_lstPlatforms.GetNext(m_posPlatExp);
		return TRUE;
	}

	BOOL LookupPlatformByOfficialName(const TCHAR * szOfficialName, CPlatform * & pPlatform);
	BOOL LookupPlatformByUIDescription(const TCHAR * szDescription, CPlatform * & pPlatform);
	BOOL LookupPlatformByUniqueId(UINT nUniqueId, CPlatform * & pPlatform);
	CPlatform * GetPrimaryPlatform(BOOL bAllowNonPrimary = TRUE);
	UINT GetIdFromMachineImage(WORD wMachineImage);

	//
	// our project type management
	//
	void RegisterProjType(CProjType * pprojtype);
	BOOL DeregisterProjType(CProjType * pprojtype);

	__inline BOOL LookupProjTypeByName( const TCHAR * pszName, CProjType * & pprojtype)
		{return m_mapProjTypes.Lookup((const TCHAR *) pszName, (void *&)pprojtype);}
	BOOL LookupProjTypeByUIDescription( const TCHAR * pszUIDesc, CProjType * & pprojtype);

	BOOL LookupProjTypeByUniqueId( UINT nUniqueId, CProjType * & pprojtype);
	__inline void InitProjTypeEnum() {m_posProjType = m_lstProjTypes.GetHeadPosition();}
	__inline BOOL NextProjType(CProjType * & pprojtype)
	{
		if (m_posProjType == (POSITION) NULL)	return FALSE;
		pprojtype = (CProjType *)m_lstProjTypes.GetNext(m_posProjType);
		return TRUE;
	}

	const CPtrList *GetProjTypes() { return &m_lstProjTypes; }

	CObList * GetMatchingProjTypes(const _TCHAR * pszPlatform, const _TCHAR * pszType, BOOL bUiDescription = FALSE);
	const CStringList * GetListOfProjTypes(const _TCHAR * pszPlatform = NULL, const _TCHAR * pszType = NULL, BOOL bUiDescription = FALSE);
	const CStringList * GetListOfPlatforms(const _TCHAR * pszType = NULL, BOOL bUIDescription = FALSE);
	const CStringList * GetListOfTypes(const _TCHAR * pszPlatform = NULL, BOOL bUiDescription = FALSE);

	//
	// our build tool management
	//
	__inline UINT GetBuildToolCount() {return m_lstTools.GetCount();}
	const CPtrList *GetTools() { return &m_lstTools; }
	__inline void InitBuildToolEnum() {m_posTool = m_lstTools.GetHeadPosition();}
	__inline BOOL NextBuildTool(CBuildTool * & ptool)
	{
		if (m_posTool == (POSITION) NULL)	return FALSE;
		ptool = (CBuildTool *)m_lstTools.GetNext(m_posTool);
		return TRUE;
	}

	void RegisterBuildTool(CBuildTool * ptool);
	__inline BOOL DeregisterBuildTool(CBuildTool * ptool)
		{return DeregisterComponent(m_lstTools, (CObject *)ptool);}

/*
R.I.P. for v4.0 with VBA?
	//
	// our global custom component management
	// (note *not* per-platform)
	//

	// custom tools
	static BOOL SerializeProjectCustomTool(CBldrSection * pbldsect, UINT op);
	BOOL RetrieveGlobalCustomTool();
	BOOL StoreGlobalCustomTool();

	__inline CPtrList * GetCustomToolList() {return &m_lstCustTools;}
	__inline void InitCustomToolEnum() {m_posCustTool = m_lstCustTools.GetHeadPosition();}
	__inline BOOL NextCustomTool(CCustomTool * & ptool)
	{
		if (m_posCustTool == (POSITION) NULL)	return FALSE;
		ptool = (CCustomTool *)m_lstCustTools.GetNext(m_posCustTool);
		return TRUE;
	}

	BOOL RegisterCustomTool(CCustomTool * ptool);
	__inline BOOL DeregisterCustomTool(CCustomTool * ptool)
		{return DeregisterComponent(m_lstCustTools, (CObject *)ptool);}
*/

	// Find the tool whose prefix matches the pPrefix. nPrefixLen is the maximum
	// number of characters to match up to.  pprojitem can be used to retrieve
	// only the tools for the current configuration.
	CBuildTool * GetToolFromCodePrefix(CProjItem * pprojitem, const TCHAR *pPrefix, int nPrefixLen);

	//
	// our tool option management
	//
	__inline void InitOptHdlrEnum() {m_posOptHdlr = m_lstOptionHandlers.GetHeadPosition();}
	__inline BOOL NextOptHdlr(COptionHandler * & popthdlr)
	{
		if (m_posOptHdlr == (POSITION) NULL)	return FALSE;
		popthdlr = (COptionHandler *)m_lstOptionHandlers.GetNext(m_posOptHdlr);
		return TRUE;
	}

	void RegisterOptionHandler(COptionHandler * popthdlr);
	BOOL DeregisterOptionHandler(COptionHandler * popthdlr);

	// given a property return the option handler
	// (cached for props belonging to same opt. hdlr)
	UINT m_nPropFirst, m_nPropLast;
	COptionHandler * m_popthdlrCache;
	COptionHandler * FindOptionHandler(UINT idProp);

	// given a property bag we'll set the defaults for the project mode
	// based on the original target type of the property bag
	__inline BOOL SetDefaultDebugToolOptions(CProject * pProject)
		{return SetDefaultToolOptions(MOB_Debug, pProject);}

	__inline BOOL SetDefaultReleaseToolOptions(CProject * pProject)
		{return SetDefaultToolOptions(MOB_Release, pProject);}

	//
	// our builder converter management
	//
	__inline void InitBuilderConverterEnum() { m_posBldrCnvtr = m_lstBldrCnvtrs.GetHeadPosition(); }
	__inline BOOL NextBuilderConverter(CBuilderConverter * & pBldrCnvtr)
	{
		if (m_posBldrCnvtr == (POSITION)NULL)
			return FALSE;
		pBldrCnvtr = (CBuilderConverter *)m_lstBldrCnvtrs.GetNext(m_posBldrCnvtr);
		return TRUE;
	}

	void RegisterBuilderConverter(CBuilderConverter * pBldrCnvtr);
	BOOL DeregisterBuilderConverter(CBuilderConverter * pBldrCnvtr);

	//
	// our unknown project object management
	// ie. unknown option handlers, target types, platforms, tools
	//
	// on destruction of the proj. component manager these will be
	// destroyed as well -> ie. use this to remember objects to clean-up
	// at project facility shutdown time
	__inline void RegisterUnkProjObj(CObject * punkobj)
		{ASSERT_VALID(punkobj); m_lstUnkProjObjs.AddTail(punkobj);}
	__inline BOOL DeregisterUnkProjObj(CObject * punkobj)
		{return DeregisterComponent(m_lstUnkProjObjs, punkobj);}

	// information components might use, such as is MFC or ODBC installed?
	__inline BOOL MFCIsInstalled() {return m_fMFCInstalled;}
	__inline BOOL ODBCIsInstalled() {return m_fODBCInstalled;}

protected:
	BOOL SetDefaultToolOptions(DWORD mode, CProject * pProject);

private:
	// generic component removal
	BOOL DeregisterComponent(CPtrList & plst, CObject * ptr);

	// component registration
	CStringArray	m_rgstrAddOns;
	CMapPtrToPtr	m_mapBldSysCmps;
	DWORD			m_dwRegisterId;		// current related id. when asking add-ons to register comps.
	DWORD			m_dwCustomBase;		// base of our custom components

	// CPlatform registry
	CMapStringToPtr	m_mapPlatforms;		// for quick-lookup -- HASHed on "official name"
	CPtrList		m_lstPlatforms;		// ordered list
	POSITION		m_posPlatform;		// current pos for InitPlatformEnum() / NextPlatform()
	POSITION		m_posPlatExp;		// current pos for InitPlatformEnumExport() / NextPlatformExport()

	// CProjType registry (public)
	CMapStringToPtr	m_mapProjTypes;		// for quick-lookup -- HASH order!
	CPtrList		m_lstProjTypes;		// ordered list
	POSITION		m_posProjType;		// current pos for InitProjTypeEnum() / NextProjType()
	CObList			m_tmplstProjTypes;
	CStringList		m_tmpNameList;
	CStringList		m_tmpUIDescList;

	// CBuildTool registry (public)
	POSITION		m_posTool, m_posCustTool;
	CPtrList		m_lstTools, m_lstCustTools;
	UINT			m_cSchmoozeTools;

	// COptionHandler registry (private)
	CPtrList		m_lstOptionHandlers;
	UINT			m_idOptPropBase;
	POSITION		m_posOptHdlr;

	// CBuilderConverter registry (private)
	CPtrList		m_lstBldrCnvtrs;
	POSITION		m_posBldrCnvtr;

	// unknown object registry
	CPtrList		m_lstUnkProjObjs;

	// information components might use
	BOOL			m_fODBCInstalled;	// ODBC Installed?
	BOOL			m_fMFCInstalled;	// MFC Installed?

	CBldrSection		m_bldsection;		// our custom tool builder file section

	// flags used during component registration
	BOOL			m_fRegPrimaryPlatform;

	CPtrList		m_AddOns;
public:
	void RegisterOlePlatform( IPlatform *plat, DWORD bscid );
	void RegisterOleTool( IBuildTool *tool, DWORD bscid );
	void RegisterOleProjType( IProjectType *proj, DWORD bscid );
	void RegisterOleAddOn( LPBUILDSYSTEMADDON pAddOn, DWORD bsc_id );
};

#ifdef _BLD_INTERNAL
extern CProjComponentMgr g_prjcompmgr;
#endif

#include "proppage.h"	// CEnumerator for property pages

// This class enumerates the configurations for the benefit of the prop page.
class CConfigEnum : public CEnum
{
private:
	CEnumerator *	m_prevListReturned;
	// Keep track of the previous list returned, if any,
	// so we can free it next time we're asked for the list.

public:
	CConfigEnum() { m_prevListReturned = NULL; }
	~CConfigEnum()  { delete [] m_prevListReturned; }

	virtual	CEnumerator *	GetList();
	// Return a pointer to an array of CEnumerators.
};

// This class enumerates the configurations for the benefit of the prop page.
class CProjectEnum : public CEnum
{
private:
	CEnumerator *	m_prevListReturned;
	CString *		m_prevStrListReturned;
	// Keep track of the previous list returned, if any,
	// so we can free it next time we're asked for the list.

public:
	CProjectEnum() { 
		m_prevListReturned = NULL; 
		m_prevStrListReturned = NULL; 
	}
	~CProjectEnum()  { 
		delete [] m_prevListReturned; 
		delete [] m_prevStrListReturned; 
	}

	virtual	CEnumerator *	GetList();
	// Return a pointer to an array of CEnumerators.
};


class CProjConfigEnum : public CEnum
{
private:
	CEnumerator *	m_prevListReturned;
	CString *		m_prevStrListReturned;
	// Keep track of the previous list returned, if any,
	// so we can free it next time we're asked for the list.

public:
	CProjConfigEnum() { 
		m_prevListReturned = NULL; 
		m_prevStrListReturned = NULL; 
	}
	~CProjConfigEnum()  { 
		delete [] m_prevListReturned; 
		delete [] m_prevStrListReturned; 
	}

	virtual	CEnumerator *	GetList();
	// Return a pointer to an array of CEnumerators.
};

// This class enumerates the platforms for the benefit of the prop page.
class CPlatformEnum : public CEnum
{
private:
	CEnumerator *	m_prevListReturned;
	// Keep track of the previous list returned, if any,
	// so we can free it next time we're asked for the list.

public:
	CPlatformEnum() { 
		m_prevListReturned = NULL; 
	}
	~CPlatformEnum() { 
		delete [] m_prevListReturned; 
	}

	virtual	CEnumerator *	GetList();
	// Return a pointer to an array of CEnumerators.
};

// The one and only instance of the CConfigEnum class.
extern CConfigEnum g_theConfigEnum;
extern CProjectEnum g_theProjectEnum;

 // The one and only instance of the CPlatformEnum class.
extern CPlatformEnum g_thePlatformEnum;

// The CPlatform class embodies a target platform -- an operating system and
// a CPU (e.g., Macintosh (680x0), Win32 (80x86), Win32 (MIPS, ALPHA), etc.
//
// All CPlatform instances in the system are static (i.e., exist even in the
// absence of a project) and register themselves with the CProjComponentMgr.
//
// The ToolInfo struct contains specific information needed for some of the tools
// supported by the platform.
// NOTE:
// It would have been nice if we can put this information in the appropriate
// OptionHandler. Unfortunately, for instance, RC name is defined in the
// Common Linker Option Handler but Mac and Intel have different default
// extensions. Until the OptionHandler supports inheritance, we have to
// settle w/ this ToolInfo struct soluntion.
//
// The PlatformInfo class is used to initialize (construct) a CPlatform object.
//
// Exactly one CPlatform object should be the "primary target platform" (set the
// bPrimaryPlatform field in the PlatformInfo struct to TRUE).  If, when checking
// the registry to find out which platforms are installed, we don't find ANY
// platforms, we will ensure that the "primary target platform" is installed
// anyway (and write information to the registry to indicate this).  Note that
// we will do this only if NO installed platforms are found, so the primary target
// platform is "automatically" installed only if no others are found.

// Bit fields for nFlags field of NMInfo struct.
//
//#define NFL_

// Bit fields for the Platform attributes.
// This is to hold information similar to PlatformInfo that would be
// wasteful to represent as a new method of CPlatform, ie. information
// of a similar type BOOL can be represented as a bit field value.

#define PIA_Primary_Platform	0x0001	// Is this the primary platform? ie. default platform
#define PIA_Supports_IncLink	0x0002	// Supports an incremental linker?
#define PIA_Allow_WorkingDir	0x0004	// Allow use of a working directory for 'Debug Options'
#define PIA_Enable_Language		0x0008  // Enable Language option for RC
#define PIA_Allow_MFCinDLL		0x0010	// Allow use of mfc in a dll
#define PIA_Supports_RemoteDbg	0x0020	// Supports remote debugging?
#define PIA_Allow_ODBC			0x0040	// Allow use of mfc ODBC
#define PIA_Allow_ProgArgs		0x0080	// Executable can use program arguments for Debug etc.
#define PIA_Enable_Stub			0x0100	// Allow dos stub
#define PIA_Enable_AllDebugType	0x0200	// Enable all debug options (otherwise enable only MS option
#define PIA_Supports_IncCplr	0x0400	// Supports an incremental compiler?
#define PIA_Supports_RTTI		0x0800	// C++ language implementation supports RTTI?
#define PIA_Supports_MinBuild	0x1000	// Support minimum build compiler
#define PIA_Supports_ENC		0x2000	// Support edit & continue

struct TLInfo						// Transport layer information
{
	TCHAR *		szName;				// Name of DLL (e.g., "TLN0LOC.DLL")
	UINT		nIDDesc;			// String ID of description of DLL
	UINT		nFlags;				// Flags describing transport layer (TFL_... above)
};

struct NMInfo
{
	TCHAR *		szName;				// Name of DLL
	UINT		nFlags;				// Flags describing NM (NFL_... above)
};

struct ToolInfo
{
	CString		strRCExtension;		// RC extension for specific platform (ie: .res or .rsc)
 	CString		strPath;
	CString		strIncludePath;
	CString		strLibPath;
	CString		strSourcePath;
};

struct PlatformInfo
{
	TCHAR *		szName;				// "Official" name of platform
	UINT		nUniqueId;			// Platform identifier (uniq_platform)
	UINT		nIDUIDesc;			// String ID of UI description of platform
	UINT		nIDUIShortDesc;		// String ID of abbreviated UI description
	TCHAR *		szEMName;			// Name of EM DLL
	TCHAR *		szSHName;			// Name of SH DLL
	TCHAR *		szEEName;			// Name of EE DLL
	UINT		nCountTLs;			// Number of TLs
	TLInfo *	rgTLInfo;			// Array of TLInfo structs (nCountTLs in number)
	UINT		nCountNMs;			// Number of NMs
	NMInfo *    rgNMInfo;			// Array of NMInfo structs (nCountNMs in number)
	TCHAR *		szRCExtension;		// RC extension for specific platform (ie: .res or .rsc)

 	TCHAR *		szPath;
	TCHAR *		szIncludePath;
	TCHAR *		szLibPath;
	TCHAR *		szSourcePath;

	DWORD		dwPlatformAttrib;	// The boolean attributes
	UINT		nUseMFCDefault;		// Default use of MFC
	TCHAR *		szOutDirPrefix;		// Default directory prefix
									// (if == NULL then first three chars of UI desc. used)
};


class BLD_IFACE CPlatform : public CBldSysCmp
{
	DECLARE_DYNCREATE(CPlatform)
 	DECLARE_REGISTRY_MAP()

private:
	BOOL			m_bSupported;		// is this platform actually supported?

	UINT			m_nUniqueId;		// Unique identifier from uniq_platform (0-255)
	UINT			m_nIDUIDesc;		// String ID of UI description string
	UINT			m_nIDUIShortDesc;	// String ID of abbreviated UI description
										// string (for loading later)
	UINT *			m_rgIDTLDescs;		// Array of string IDs describing transport layers
	UINT			m_nCurrentTL;		// Current transport layer (index into arrays)

	CString			m_strUIShortDesc;	// Abbreviated description for UI purposes
	CString			m_strEMName;		// Name of EM DLL
	CString			m_strSHName;		// Name of SH DLL
	CString			m_strEEName;		// Name of EE DLL

	UINT			m_nCountTLs;		// Number of TLs
	CString *		m_rgStrTLNames;		// Array of names of TL DLLs
	UINT *			m_rgTLFlags;		// Array of flag values for transport layers
	CString *		m_rgStrTLDescs;		// Array of strings describing transport layers

	UINT			m_nCountNMs;		// Number of NMs
	CString *		m_rgStrNMNames;		// Array of names of NM DLLs
	UINT *			m_rgNMFlags;		// Array of flag values for NMs

	ToolInfo		m_rgToolInfo;		// Tool Info structure

	DWORD			m_dwAttributes;		// Boolean attributes
	UINT			m_nUseMFCDefault;	// Default use of MFC

	CString			m_strDefOutDirPrefix;	// Default output directory prefix
	BOOL			m_bBuildable;

public:
	CPlatform() {};
	CPlatform(PlatformInfo *);
	CPlatform(const _TCHAR * lpszName, UINT nUniqueId = unknown_platform); //
	virtual ~CPlatform();

	// from -> CBldSysCmp
	virtual BOOL RegDataExists(const CRegistryMap *, HKEY, CString *, LPDWORD, LPDWORD);
 	virtual BOOL LoadRegData(const CRegistryMap *, HKEY, CString *, LPBYTE, DWORD);
 	virtual BOOL SaveRegData(const CRegistryMap *, HKEY, CString *, LPBYTE, DWORD);
	virtual BOOL IsSupported() const { return m_bSupported; }
		// Returns TRUE if this is a fully supported platform,
		// FALSE if it is a fake one for compatibility
	// <- CBldSysCmp

	__inline BOOL IsPrimaryPlatform() const { return (m_dwAttributes & PIA_Primary_Platform) != 0; }
		// Returns TRUE if this is the "primary" platform (see notes above),
		// FALSE if not.

	__inline void Disable() { m_bSupported = FALSE; }
		// Hook that allows us to permanently disable (mark as unsupported)
		// a platform, as an alternative to deregistering it completely

	__inline void SetBuildable(BOOL bCan) { m_bBuildable = bCan; }
	BOOL GetBuildable() { return m_bBuildable; }
		// Some "supported" paltforms may not be licensed for use.

	__inline const ToolInfo * GetToolInfo() const { return &m_rgToolInfo; }
		// returns a pointer to Tool Information that is specific to
		// the platform

	__inline DWORD GetAttributes() const { return m_dwAttributes; }

	BOOL SupportsODBC() const;

	UINT GetUseMFCDefault();
		// returns the default use of MFC for this platform

	const CString * GetDefOutDirPrefix() const { return &m_strDefOutDirPrefix; }

		// P_ComponentName
		//
		// Returns the "official" name of the platform.  This name is
		// used when:
		//
		//		- writing to the .MAK file
		//		- writing to the registry
		//
		// It is very important that this "official" name NOT be localized;
		// otherwise, localized versions lose compatibility with other .MAK
		// files.  This "official" name should therefore NOT be placed in
		// the .RC file.
		//
		// The form of this name is:
		//
		// "<operating_system> (<cpu>)"
		//
		// For example, "Macintosh (680x0)", "Win32 (80x86)", etc.

	UINT GetUniqueId() const { return (m_nUniqueId & 255); }
		// Returns the unique identifier (0-255) of this platform (see the
		// uniq_platform enum).

	const CString * GetUIDescription() const { return (m_nIDUIDesc) ? &m_strUICompName : &m_strCompName; }
		// Returns the name of the platform as it is represented in the UI
		// (for example, in dialogs).  This string *SHOULD* be placed in
		// the .RC file so it can be localized.  Needless to say, you should
		// be careful not to use this string instead of the "official" string
		// (GetName()).

	const CString * GetUIShortDescription() const
		{ return &m_strUIShortDesc; }
		// Returns an abbreviated version of the string returned by GetUIDescription().
		// It is recommended (although not guaranteed) that this abbreviated description
		// be 5 characters or fewer, since it is used when generating target names
		// and output directory names.

	const TCHAR * GetEMName() const { ASSERT(m_bSupported); return m_strEMName; }
	const TCHAR * GetSHName() const { ASSERT(m_bSupported); return m_strSHName; }
	const TCHAR * GetEEName() const { ASSERT(m_bSupported); return m_strEEName; }
		// Return the name of the EM DLL, the SH DLL or the EE DLL, respectively.

	UINT GetTLCount() const { return m_nCountTLs; }
		// Return the number of transport layers supported by this platform.

	UINT GetTLFlags(int index) const { ASSERT(m_bSupported); return m_rgTLFlags[index]; }
		// Return the flags value (TFL_... bits) for the transport layer
		// specified by the index (0 <= index <= GetTLCount()-1).

	UINT GetCurrentTLIndex() const { return m_nCurrentTL; }
	void SetCurrentTLIndex(UINT index);
		// Get or set the "current transport layer"
		// (where 0 <= index <= GetTLCount() - 1).

	UINT GetLocalTLIndex() const;
		// Returns the index of the local TL for this platform if
		// one exists, if there is no local TL then it returns -1.

	const TCHAR * GetTLName(int index) const { ASSERT(m_bSupported); return m_rgStrTLNames[index]; }
		// Get the name of the specified transport layer's DLL.

	const UINT GetTLIndex(const TCHAR * pTLName, UINT nDefault);
		// Get the index of a TL given its name, if the name does
		// not match any known TL name then return nDefault

	const TCHAR * GetTLDescription(int index) const { ASSERT(m_bSupported); return m_rgStrTLDescs[index]; }
		// Return the description of the specified transport layer.

	UINT GetNMCount() const { return m_nCountNMs; }
		// Return the number of NMs supported by this platform.

	const TCHAR * GetNMName(int index) const { ASSERT(m_bSupported); return m_rgStrNMNames[index]; }

		// Return the name of the specified NM's DLL.

	UINT GetNMFlags(int index) const { ASSERT(m_bSupported); return m_rgNMFlags[index]; }
		// Return the flag values (NFL_... bits) for the specified NM

	BOOL FInit();
		// Initialize this object -- specifically, load strings (which we
		// can't do at construction time since we don't have an instance
		// handle at that time).
};

class BLD_IFACE COLEPlatform : public CPlatform
{
public:
	COLEPlatform( LPPLATFORM  ):CPlatform(){};
};

// Our CProjType class ... contains information specific to a particular type of project,
// ie. project name, type of 'target generating' tool, tool option defaults etc.
class BLD_IFACE CProjType : public CBldSysCmp
{
	DECLARE_DYNCREATE(CProjType)

public:
	CProjType() { m_pIProjType = NULL; };
	CProjType(const TCHAR * szPkg, WORD id,
			  UINT nIDType, UINT nUniqTypeId,
			  const TCHAR * szPkgPlatform, WORD idPlatform);
	CProjType(const TCHAR * szPkg, WORD id,
			  UINT nIDType, UINT nUniqTypeId,
			  DWORD bsc_id);
	CProjType(const _TCHAR * lpszType, UINT nUniqTypeId, CPlatform * pPlatform);
	virtual ~CProjType();

	// Warning: these enums are defined in order to maintain projtype
	// compatibility between different drops and versions of the product
	// While you can change the names of these enums, the implementations
	// (could be #defines), or even projtype names, you must not change
	// the order or value of these values and their meaning.
	// Also, how they are packed into an int (type in low byte, platform
	// in 2nd byte), cannot be changed without breaking makefile compatibility.
	// new types can be added at the end, or you can use the unknown type.

	enum uniq_type
	{
		unknown_projtype = 0,
		application = 1,
		dynamiclib = 2,
		consoleapp = 3,
		staticlib = 4,
		sharedlib = 5,
		exttarget = 6,
		quickwin = 7,
		standardgraphics = 8,
		javaapplet = 9,
		generic = 10,
	 };

private:
	static const _TCHAR * TypeNameFromId[11];
public:

	// CBldSysComp related methods
	// ***************************

	// initialise
	virtual BOOL 	FInit();
	virtual int 	GetAttributes () { ASSERT(FALSE); return 0; }

	// Platform x ProjType Methods
	// ***************************

	static 	CString 		MakeNameFromPlatformAndType(const CString & platform, const CString & type);
	static 	CString 		MakeNameFromPlatformAndType(const _TCHAR * platform, const _TCHAR * type);

	// note that this is essentially GetPlatformUIDescription() + ' ' + GetTypeUIDescription()
	const CString * GetUIDescription() const { return &m_strUICompName; }
	UINT 			GetUniqueId() const { return GetUniqueTypeId() | (GetUniquePlatformId() << 8); }


	// Platform Related Methods
	// ************************

	virtual CPlatform * 	GetPlatform() const { return m_pPlatform; }
	virtual BOOL 				IsSupported() const { return m_pPlatform->IsSupported(); }

	const CString *		GetPlatformName() const { return m_pPlatform->GetName(); }
	const CString * 	GetPlatformUIDescription() const { return m_pPlatform->GetUIDescription(); }
	UINT 				GetUniquePlatformId() const { return (m_pPlatform->GetUniqueId()); }

	// Unique Type Related Methods
	// ***************************

 	// return the name of the type of target for this target type, eg. 'Application'
	virtual const CString * GetTypeName() { return &m_strType; }

	const CString * GetTypeUIDescription() const { return &m_strUIType; }
	UINT 			GetUniqueTypeId() const { return (m_nUniqueTypeId & 255); }

	// Tools List Related Methods
	// **************************

	// create the list of tools we use
	virtual void 		CreateToolsList() {/* default target type uses no tools*/}
	virtual CPtrList * 	GetToolsList() { return &m_ToolsList; }
	virtual CBuildTool *PickTool(FileRegHandle frh);

	void 		ClearUnknownTools();
	// Find the tool whose prefix matches the pPrefix. nPrefixLen is the maximum number
	// of characters to match up to.
	// pprojitem is used to retrieve the tools for the current configuration.
	CBuildTool * GetToolFromCodePrefix(const TCHAR *pPrefix, int nPrefixLen);
	// add a tool into our list of tool's
	void 		AddTool(const TCHAR * szPckg, WORD idTool);
	void 		AddTool(WORD pckg, WORD idTool);
	void 		AddTool(CBuildTool * pTool);

	// Settings Related Methods
	// ************************

	// default tool options for the target type
	virtual BOOL 	GetDefaultToolOptions(DWORD /*bsc_id*/, const CString & /*strMode*/, CString & /*strOption*/)
						{return FALSE;}	// FUTURE: ask the tool for the default options
	// default project level (i.e. target) options for the target type
	virtual BOOL 	SetDefaultDebugTargetOptions(CProject * pProject, CTargetItem * pTarget, BOOL fUseMFC);
	virtual BOOL 	SetDefaultReleaseTargetOptions(CProject * pProject, CTargetItem * pTarget, BOOL fUseMFC);
	// returns the default use of MFC for this target type (returns platform default use)
	virtual UINT 	GetUseMFCDefault() { return m_pPlatform->GetUseMFCDefault(); }
	//	Ensure that the tool will build an the project item (debug or release config. mode)
	//	Likely to entail doing such things as munging tool options, eg. the libs for linker
	// FUTURE: Use wizards to do this in v4.0?
	virtual void 	PerformSettingsWizard(CProjItem * pItem, BOOL fUsingMFC);
	

protected:
	CPtrList m_ToolsList;	// List of tools that can be used with us. Source tools are at the front.
							// Schmooze tools are in the back in the order in which they should be
							// invoked during build.
	UINT	m_nIDType;		// string resource ID for target type name
	CString	m_strType;		// our target type name
	CString m_strUIType;
	CPlatform * m_pPlatform;

private:
	UINT	m_nUniqueTypeId; // Unique identifier (see uniq_type enum)

public:
	virtual void ReleaseInterface( void );
	virtual LPPROJECTTYPE GetInterface(void);
	void RemoveInterface(void){ pCIProjType=NULL;}

private:
	LPPROJECTTYPE 		m_pIProjType;
	CProjTypeInterface *pCIProjType;
};

// This is the ole interface for use with old style project types
class CProjTypeInterface : public CCmdTarget{
public:
	LPPROJECTTYPE GetInterface(void){
		LPPROJECTTYPE pIProjType;
		m_xProjType.QueryInterface(IID_IProjectType, (void **)&pIProjType);
		return pIProjType;
	}

	void SetThis( CProjType *pProj ){ m_pProjectType = pProj; };
	CProjType *GetProjectType(){ return m_pProjectType; };
	~CProjTypeInterface(){ if( m_pProjectType ) m_pProjectType->RemoveInterface(); };


protected:
	BEGIN_INTERFACE_PART(ProjType, IProjectType)
		INIT_INTERFACE_PART(CProjTypeInterface, ProjectType)

		// IProjectType
		STDMETHOD(GetName)( CString *str );
		STDMETHOD(GetType)( UINT *id );
		STDMETHOD(GetPlatform)( LPPLATFORM *);
		STDMETHOD(IsSupported)(void);

		STDMETHOD(PickTool)( LPBUILDFILE pFile, LPBUILDTOOL *pBuildTool);
		STDMETHOD(AddTool)( LPBUILDTOOL pBuildTool);
		STDMETHOD(EnumBuildTools)( LPENUMBUILDTOOLS *, LPBUILDTOOL * );
	END_INTERFACE_PART(ProjType)

	DECLARE_INTERFACE_MAP()

private:
	CProjType *m_pProjectType;

};



// COLEProjType is the proxy object for add on defined project types
class COLEProjType : public CProjType
{
	DECLARE_DYNAMIC(COLEProjType)
public:
	COLEProjType( LPPROJECTTYPE pPrj, const char *szName, int type ):
		CProjType(szName,1,0,type,szName,1){ m_pProj = pPrj; };

	virtual CPlatform * 		GetPlatform() const ;
	virtual const CString * 	GetTypeName() ;
	virtual BOOL 				IsSupported() const ;
	virtual CBuildTool *		PickTool(FileRegHandle frh);
	virtual CPtrList * 			GetToolsList();

	virtual LPPROJECTTYPE GetInterface(){ m_pProj->AddRef(); return m_pProj;}

private:
	LPPROJECTTYPE m_pProj;		// the add ons project type
};



class CProjTypeUnknown : public CProjType
{
	DECLARE_DYNAMIC(CProjTypeUnknown)
public:
	CProjTypeUnknown(const _TCHAR * lpszType, UINT nUniqTypeId, CPlatform * pPlatform)
		: CProjType(lpszType, nUniqTypeId, pPlatform) {}

	~CProjTypeUnknown();

	// create the list of tools we use
	BOOL FInit();

	// from -> CBldSysCmp
	BOOL IsSupported() const { return FALSE; }
	// <- CBldSysCmp

	// old makefile (Caviar/Cuda) conversion
	int GetOldPROGTYPE(BOOL /*f32Bit*/ = TRUE) {return -1;}

	BOOL GetDefaultToolOptions(DWORD, const CString & , CString & ) { return FALSE; }
	int GetAttributes () {return TargetUnknown;}
};

// helper function used to derive tool options that show common part
void BLD_IFACE VPROJDeriveDefaultOptions(CString & strOption, UINT nIDCommon, UINT nIDMode, UINT nIDOption);

#endif // _INCLUDE_PRJCONFG_H
