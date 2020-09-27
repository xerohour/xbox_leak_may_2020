//
// CProjComponentMgr, CProjType
//
// Project component management, ie. target types, tools and option tables
//
// [matthewt]
//

#ifndef _INCLUDE_PRJCONFG_H
#define _INCLUDE_PRJCONFG_H

#include "Vcprojcnvt2.h"

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
class CTargetItem;
/*
R.I.P. for v4.0 with VBA?
class CCustomTool;			// custom build tool (defined in 'projtool.h')
*/
class CProject;				// project (defined in 'project.h')
class COptionHandler;		// tool option handler (defined in 'prjoptn.h')

// Helper function -- probably belongs in some new module PROJUTIL.CPP / PROJUTIL.H.
void GetNewOutputDirName(CProject * pProject, const CString & strNewConfig, CString & strResult);

// declare the global tool option engine (parsing & generation)
class COptionTable;
extern  COptionTable * g_pPrjoptengine;

// functions to munge/extract the project mode from the configuration
void DefaultConfig(CProject * pProject, const CString & strPlatformName, UINT nIDMode, CString & strConfig);

#define HPROJECT DWORD_PTR
#define HCONFIGURATION DWORD_PTR
#define HBLDFILE DWORD_PTR
#define HFILESET DWORD_PTR
#define HFOLDER DWORD_PTR

#define ACTIVE_PROJECT	(HPROJECT)NULL
#define ACTIVE_CONFIG	(HCONFIGURATION)NULL
#define ACTIVE_FILESET	(HFILESET)NULL

#define NO_PROJECT		(HPROJECT)NULL
#define NO_CONFIG		(HCONFIGURATION)NULL
#define NO_FILE			(HBLDFILE)NULL
#define NO_FOLDER		(HFOLDER)NULL

// Target attributes
typedef enum
{
	// Image Types:
	ImageStaticLib		= 0x1,
	ImageDLL			= 0x2,
	ImageExe			= 0x4,
	// Subsystems:
	SubsystemConsole	= 0x8,
	SubsystemWindows	= 0x10,
	// Can we debug the resulting target?
	TargetIsDebugable	= 0x20,
	// Is this an unknown target?
	TargetUnknown		= 0x40,
} TargetAttributes;

typedef enum
{
	unknown_projtype = 0,
	application      = 1,
	dynamiclib       = 2,
	consoleapp       = 3,
	staticlib        = 4,
	sharedlib        = 5,
	exttarget        = 6,
	quickwin         = 7,
	standardgraphics = 8,
} uniq_projtype;

// build system component registration

// component types
#define BLC_ERROR		0x0000
#define BLC_Platform	0x0001
#define BLC_Tool		0x0002
#define BLC_Scanner		0x0003
#define BLC_OptionHdlr	0x0004
#define BLC_TargetType	0x0005
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

class CBldrSection;
// callback for builder file 'sections'
// 'pbldsect' is the section data
// 'op' is the callback operation
// callback used for synchronous notification of read/write
typedef BOOL (*PfnSectionCallBack)(CBldrSection * pbldsect, UINT op);

class CBldrSection	// Non-standard COM.
{
public:
	CBldrSection(HPROJECT hBld = ACTIVE_PROJECT) {m_pfn = NULL; m_pch = NULL; m_cch = 0; m_dw = 0; m_hBld = hBld;}

	CString m_strName;			// section name
	
	TCHAR * m_pch;				// char. buffer
	size_t m_cch;				// size of buffer (not used for write)
	// o format of this data is each line zero-terminated with a double terminator
	//   signifying the end of the buffer 

	PfnSectionCallBack m_pfn;	// our callback (if NULL, assume async retrieval)
	DWORD m_dw;					// user-supplied word
	HPROJECT m_hBld;
};

// declare the global configuration manager
class CProjComponentMgr;
class  CBldSysCmp : public CSlob
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

	// retrieving of component properties to system registry
	//
	// this will ensure that the component info. is in the registry,
	// any changes in the information in the registry will be copyed to
	// the component
	virtual BOOL SynchronizeWithRegistry();

	virtual BOOL RegDataExists(const CRegistryMap *, HKEY, CString *, LPDWORD, LPDWORD);
 	virtual BOOL LoadRegData(const CRegistryMap *, HKEY, CString *, LPBYTE, DWORD);

	// returns TRUE if this is a fully supported component,
	// FALSE if it is a fake one for compatibility (ie. unknown tools etc.)
	virtual BOOL IsSupported() const { return TRUE; }

	// helpers
	BOOL RegGetDataKey(const CRegistryMap *, PHKEY, BOOL fCreate = TRUE);

	BOOL LoadRegData(UINT nRegId);

	// get our regular and 'UI' component name
	__inline const CString * GetName() const {return &m_strCompName;}

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

class  CProjComponentMgr : public CObject
{
public:
	// create project component manager
	CProjComponentMgr();
	~CProjComponentMgr();

	// initialise (ask packages to register their components)
	BOOL FInit(IVCProjConvert *pConverter);
	void IsInit(BOOL b){m_fInit=b;};
	BOOL IsInit(){return m_fInit;};
	BOOL m_fInit;
	CObList lstArngdPackages;

	// terminate
	void Term();
	void Clean();

	// build system component registration

	// generate package id using the package name
	WORD GenerateBldSysCompPackageId(const TCHAR * szPackageName);

	// package register component
	// pls. note that if no bsc_id passed in it is assumed the component
	// has pre-set it's id.
	BOOL RegisterBldSysComp(CBldSysCmp * pcomp, DWORD bsc_id = (DWORD)-1);

	// lookup a component
	__inline BOOL LookupBldSysComp(DWORD bsc_id, CBldSysCmp *& pcomp)
		{return m_mapBldSysCmps.Lookup(bsc_id, (void *&)pcomp);}

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

	int GetPlatformCount() { return (int)m_lstPlatforms.GetCount(); }
	int GetSupportedPlatformCount();

	const CVCPtrList *GetPlatforms() { return &m_lstPlatforms; }
	void InitPlatformEnum() { m_posPlatform = m_lstPlatforms.GetHeadPosition(); }
	BOOL NextPlatform(CPlatform * & pPlatform)
	{
		if (m_posPlatform == (VCPOSITION)NULL)
			return FALSE;
		pPlatform = (CPlatform *)m_lstPlatforms.GetNext(m_posPlatform);
		return TRUE;
	}

	BOOL LookupPlatformByUniqueId(UINT nUniqueId, CPlatform * & pPlatform);
	CPlatform * GetPrimaryPlatform(BOOL bAllowNonPrimary = TRUE);

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
		if (m_posProjType == (VCPOSITION) NULL)	return FALSE;
		pprojtype = (CProjType *)m_lstProjTypes.GetNext(m_posProjType);
		return TRUE;
	}

	//
	// our build tool management
	//
	__inline void InitBuildToolEnum() {m_posTool = m_lstTools.GetHeadPosition();}
	__inline BOOL NextBuildTool(CBuildTool * & ptool)
	{
		if (m_posTool == (VCPOSITION) NULL)	return FALSE;
		ptool = (CBuildTool *)m_lstTools.GetNext(m_posTool);
		return TRUE;
	}

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
		if (m_posOptHdlr == (VCPOSITION) NULL)	return FALSE;
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
	BOOL DeregisterComponent(CVCPtrList & plst, CObject * ptr);

	// component registration
	CStringArray		m_rgstrAddOns;
	CVCMapDWordToPtr	m_mapBldSysCmps;
	DWORD				m_dwRegisterId;		// current related id. when asking add-ons to register comps.

	// CPlatform registry
	CMapStringToPtr	m_mapPlatforms;		// for quick-lookup -- HASHed on "official name"
	CVCPtrList		m_lstPlatforms;		// ordered list
	VCPOSITION		m_posPlatform;		// current pos for InitPlatformEnum() / NextPlatform()

	// CProjType registry (public)
	CMapStringToPtr	m_mapProjTypes;		// for quick-lookup -- HASH order!
	CVCPtrList		m_lstProjTypes;		// ordered list
	VCPOSITION		m_posProjType;		// current pos for InitProjTypeEnum() / NextProjType()

	// CBuildTool registry (public)
	VCPOSITION		m_posTool;
	CVCPtrList		m_lstTools;

	// COptionHandler registry (private)
	CVCPtrList		m_lstOptionHandlers;
	UINT			m_idOptPropBase;
	VCPOSITION		m_posOptHdlr;

	// unknown object registry
	CVCPtrList		m_lstUnkProjObjs;

	// information components might use
	BOOL			m_fODBCInstalled;	// ODBC Installed?
	BOOL			m_fMFCInstalled;	// MFC Installed?

	CBldrSection		m_bldsection;		// our custom tool builder file section

	// flags used during component registration
	BOOL			m_fRegPrimaryPlatform;
};

extern CProjComponentMgr *g_pPrjcompmgr;

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

struct ToolInfo
{
	CString		strRCExtension;		// RC extension for specific platform (ie: .res or .rsc)
};

struct PlatformInfo
{
	TCHAR *		szName;				// "Official" name of platform
	UINT		nUniqueId;			// Platform identifier (uniq_platform)
	UINT		nIDUIDesc;			// String ID of UI description of platform
	UINT		nIDUIShortDesc;		// String ID of abbreviated UI description
	TCHAR *		szRCExtension;		// RC extension for specific platform (ie: .res or .rsc)

	DWORD		dwPlatformAttrib;	// The boolean attributes
	UINT		nUseMFCDefault;		// Default use of MFC
	TCHAR *		szOutDirPrefix;		// Default directory prefix
									// (if == NULL then first three chars of UI desc. used)
};


class  CPlatform : public CBldSysCmp
{
	DECLARE_DYNCREATE(CPlatform)
 	DECLARE_REGISTRY_MAP()

private:
	BOOL			m_bSupported;		// is this platform actually supported?

	UINT			m_nUniqueId;		// Unique identifier from uniq_platform (0-255)
	UINT			m_nIDUIDesc;		// String ID of UI description string
	UINT			m_nIDUIShortDesc;	// String ID of abbreviated UI description
										// string (for loading later)

	CString			m_strUIShortDesc;	// Abbreviated description for UI purposes

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
	virtual BOOL IsSupported() const { return m_bSupported; }
		// Returns TRUE if this is a fully supported platform,
		// FALSE if it is a fake one for compatibility
	// <- CBldSysCmp

	__inline BOOL IsPrimaryPlatform() const { return (m_dwAttributes & PIA_Primary_Platform) != 0; }
		// Returns TRUE if this is the "primary" platform (see notes above),
		// FALSE if not.

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

	BOOL FInit();
		// Initialize this object -- specifically, load strings (which we
		// can't do at construction time since we don't have an instance
		// handle at that time).
};

// Our CProjType class ... contains information specific to a particular type of project,
// ie. project name, type of 'target generating' tool, tool option defaults etc.
class  CProjType : public CBldSysCmp
{
	DECLARE_DYNCREATE(CProjType)

public:
	CProjType() {}
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
		generic = 10,
	 };

	static const _TCHAR * TypeNameFromId[11];

	// CBldSysComp related methods
	// ***************************

	// initialise
	virtual BOOL 	FInit();
	virtual int 	GetAttributes () { VSASSERT(FALSE, "GetAttributes must be overridden"); return 0; }

	// Platform x ProjType Methods
	// ***************************

	static 	CString 		MakeNameFromPlatformAndType(const CString & platform, const CString & type);
	static 	CString 		MakeNameFromPlatformAndType(const _TCHAR * platform, const _TCHAR * type);

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

	UINT 			GetUniqueTypeId() const { return (m_nUniqueTypeId & 255); }

	// Tools List Related Methods
	// **************************

	// create the list of tools we use
	virtual CVCPtrList *GetToolsList() { return &m_ToolsList; }

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
	CVCPtrList m_ToolsList;	// List of tools that can be used with us. Source tools are at the front.
							// Schmooze tools are in the back in the order in which they should be
							// invoked during build.
	UINT	m_nIDType;		// string resource ID for target type name
	CString	m_strType;		// our target type name
	CString m_strUIType;
	CPlatform * m_pPlatform;

private:
	UINT	m_nUniqueTypeId; // Unique identifier (see uniq_type enum)
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

	BOOL GetDefaultToolOptions(DWORD, const CString & , CString & ) { return FALSE; }
	int GetAttributes () {return TargetUnknown;}
};

// helper function used to derive tool options that show common part
void  VPROJDeriveDefaultOptions(CString & strOption, UINT nIDCommon, UINT nIDMode, UINT nIDOption);

#endif // _INCLUDE_PRJCONFG_H
