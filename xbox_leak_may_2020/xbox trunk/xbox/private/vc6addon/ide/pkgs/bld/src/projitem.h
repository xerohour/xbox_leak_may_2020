/////////////////////////////////////////////////////////////////////////////
//
// PROJITEM.H
//                                                                             
///////////////////////////////////////////////////////////////////////////////
//
//	Rough hierarchy tree:
//
/*
	CProjItem	+
				|
				+-- CFileItem 
				|
				+-- CDependencyFile
				|
				+--	CProjContainer	+
									|
									+-- CProject
									+--	CProjGroup
 									+-- CDependencyContainer

	CProjViewItem is the base class for all items displayed in the shell's
	project view.		


*/
#ifndef __PROJITEM__
#define __PROJITEM__

#ifndef __PATH_H__
#include "path.h"
#endif

#ifndef __UTILBLD__H__
#include "utilbld_.h"
#endif

#include "progdlg.h"
#include "slob.h"
#include "pfilereg.h"
#include "projpage.h"
#include "engine.h"

class COptionHandler;

class CProjItem;
class CFileItem;

class CProjContainer;
class CProject;
class CProjGroup;

class CBuildTool;
class CSchmoozeTool;
class CToolDirectory;

class CFileRegistry;

class CDependencyFile;
class CDependencyContainer;

class CEnvironmentVariableList;
class CIncludeEntry; 			// For scanning.
class CIncludeEntryList;

class CProjectView;

// Makread.h:
class CMakComment;
class CMakMacro;
class CMakDescBlk;
class CMakDirective;
class CMakError;
class CMakEndOfFile;
class CMakFileReader;
class CMakFileWriter;
class CNameMunger;

// Config stuff:
class CProjType;

// retrieve our project properties
#include "projprop.h"

#if 1
#define PROJ_LOG
#endif

#ifdef PROJ_LOG
// For log based debugging.  Implemented in projcomp.cpp:
void BLD_IFACE OpenProjLog(CProject *);
void BLD_IFACE CloseProjLog();
void BLD_IFACE CDECL LogTrace(LPCSTR pszFormat, ...);

#define PBUILD_TRACE     ::LogTrace
#define OPEN_BUILD_LOG(pProject)        LogEnabler lr(pProject);
class BLD_IFACE LogEnabler { public:
	LogEnabler(CProject *pProject){ ::OpenProjLog(pProject); };
	~LogEnabler(){ ::CloseProjLog();};
};

#else

#define PBUILD_TRACE 1 ? (void)0 : ::AfxTrace
#define OPEN_BUILD_LOG 	
#define CLOSE_BUILD_LOG 

#endif
// Debugging support:
#ifdef _DEBUG

#ifdef _VPROJ
#undef TRACE
void BLD_IFACE CDECL PrjTrace(LPCSTR pszFormat, ...);
#define TRACE ::PrjTrace

void BLD_IFACE CDECL PrjTrace(LPCSTR pszFormat, ...);

#else
#define PrjTrace AfxTrace
#endif
//	For serialization (OPT files and make files). Flags are defined in projitem.cpp
extern BLD_IFACE BOOL bDoFileTrace;
#define PFILE_TRACE  if (bDoFileTrace) ::PrjTrace

extern BLD_IFACE BOOL bDoConfigTrace;
#define PCFG_TRACE if (bDoConfigTrace)  ::PrjTrace

extern BLD_IFACE BOOL bDoInformTrace;
#define PINF_TRACE if (bDoInformTrace)  ::PrjTrace

extern BLD_IFACE BOOL bDoScanTrace;
#define PSCAN_TRACE if (bDoScanTrace)  ::PrjTrace

#else
#define PFILE_TRACE 1 ? (void)0 : ::AfxTrace
#define PCFG_TRACE 1 ? (void)0 : ::AfxTrace
#define PINF_TRACE 1 ? (void)0 : ::AfxTrace
#define PSCAN_TRACE 1 ? (void)0 : ::AfxTrace
#endif
///////////////////////////////////////////////////////////////////////////////    
// Structure that holds cached information about an item's state in a 
// particular configuration:
#define CHECK_CACHE \
if (!nCongfigCacheCount || (m_LastUpdate < nSignature)) \
	((ConfigurationRecord *)this)->Refresh();

///////////////////////////////////////////////////////////////////////////////    
// Property bag to extract from Configuration record?
#define BaseBag		0
#define CloneBag	1
#define CurrBag		2

class BLD_IFACE ConfigurationRecord : public CObject
{
	DECLARE_DYNAMIC (ConfigurationRecord);

public:
	//
	// Construction and destruction
	//
	ConfigurationRecord(const ConfigurationRecord * pBaseRecord, CProjItem * pItemOwner);
	~ConfigurationRecord();

	//
	// Record linkage
	//
	const ConfigurationRecord * m_pBaseRecord;

	//
	// Configuration record information retrieval
	//
	CString &GetConfigurationName () const;
	void GetConfigurationDescription (CString &) const;
	void GetProjectName (CString &) const;
	CString &GetOriginalTypeName  () const;

	void FreeOutputFiles();
	
	// Return the associated source tool
	CBuildTool * GetBuildTool();

	// Broadest possible list of files that can be considered
	// output of a tool, including things like .ILK and .MAP
	// files that aren't really "targets". Superset of target files.
	// UNDONE: should rename this to avoid confusion with similarly named function
	__inline CFRFPtrArray * GetOutputFilesList()
	{
		CHECK_CACHE

		return &m_arryOutputFiles;
	}

	// Something caused the target attributes to be dirty (can be used to set 'clean')
	__inline void DirtyTargetAttributes(BOOL fDirty = TRUE) {m_fAttribDirty = fDirty;}

	// Get the target attributes of an internal/external project target
	int GetTargetAttributes();

	// Get the 'main' (CSchmoozeTool) output files for a target
	//
	// fPrimary		fAll		Function
	//
	//	FALSE		FALSE		NOT SUPPORTED
	//	TRUE		FALSE		Returns only primary target file (CSchmooze)
	//	FALSE		TRUE		Returns all target files (CSchmooze)
	//	TRUE		TRUE		NOT SUPPORTED
	//
	// NOTE:
	// GetTargetFiles has been changed to dynamicly create a CFileRegSet instead of 
	// returning a pointer to m_setTargetFiles.  
	// IMPORTANT! - Caller must now delete pointer returned by this function.
	//
	const CFileRegSet * GetTargetFiles(BOOL fPrimary = TRUE, BOOL fAll = FALSE);

	// List of product files, excluding things that aren't really
	// targets as far as dependency scanning, makefile writing, etc.
	// Currently, this differs from Output files only for Schmooze
	// tools, but theoretically this could apply to .COD files, etc.
	CFileRegSet * GetTargetFilesList();

	// Return same list of files as GetTargetFilesList but in an array instead of a CFileRegSet
	// so that memory is conserved.  GetTargetFilesArray can also be called by ConfigurationRecord's
	// that are owned by any CProjItem, not just CProject's.
	CFRFPtrArray * GetTargetFilesArray()
	{
		CHECK_CACHE

		// m_parryTargetFiles is only allocated if different than m_arryOutputFiles.
		if (NULL==m_parryTargetFiles)
			return &m_arryOutputFiles;
		else
			return m_parryTargetFiles;
	}


	// To turn caching on and off for all config record.  Begin cache
	// only if you know that nobody is setting any properties anywhere
	// (e.g. during build):
	static void	BeginConfigCache();
	static void EndConfigCache();

	//
	// Configuration record property management
	//

	// Return a pointer to a particular bag in the config. record
	// FUTURE: On demand creation of property bags?
	__inline CPropBag * GetPropBag(UINT idBag)
		{return idBag == BaseBag ? &m_BasePropBag : (idBag == CloneBag ? &m_ClonePropBag : &m_CurrPropBag);}

	// Empty a particular bag in the config. record
	__inline void BagEmpty(UINT idBag)
	{idBag == BaseBag ? m_BasePropBag.Empty() : (idBag == CloneBag ? m_ClonePropBag.Empty() : m_CurrPropBag.Empty());}

	// Copy the properties from one bag to another.
	// By default it will do a full bag copy.
	//
	// If nPropMin is -1 the lowest prop. id in the source bag will be used.
	// If nPropMax is -1 the highest prop. id in the source bag will be used.
	// ie. if nPropMax is not supplied, and nPropMin is 50 then all props in source
	// with property id <= 50 will be copied to the destination.
	//
	// If nPropMin == nPropMax then a single prop. will be copied.
	//
	// NOT UNDOABLE
	// NOT UNDOABLE
	// NOT UNDOABLE
	void BagCopy(UINT idBagSrc, UINT idBagDest,
				 UINT nPropMin = -1, UINT nPropMax = -1,
				 BOOL fInform = FALSE);

	// Are the bags' properties the same?
	// By default it will do a full bag compare.
	//
	// If nPropMin is -1 the lowest prop. id in the source bag will be used.
	// If nPropMax is -1 the highest prop. id in the source bag will be used.
	// ie. if nPropMax is not supplied, and nPropMin is 50 then all props in source
	// with property id <= 50 will be copied to the destination.
	//
	// If nPropMin == nPropMax then a single prop. will be compared.
	// If fUseDefaults then the bags will be compared using Tool Option Defaults in
	// the absence of an actual property in either/both of the property bags.
	BOOL BagSame(UINT idBag1, UINT idBag2,
				 UINT nPropMin = -1, UINT nPropMax = -1,
				 BOOL fUseDefaults = FALSE, BOOL fCheckPrivate = TRUE);

private:
	// Our property bags for this configuration
	// for each type of property bag,
	// o base
	// o cloned
	// o current
	CPropBag		m_BasePropBag;
	CPropBag		m_ClonePropBag;
	CPropBag		m_CurrPropBag;

public:
	//
	// Access functions to determine if this config rec is valid for the
	// owning item/set the validity of this config rec for the owning item.
	//
	__inline void SetValid(BOOL fValid) { m_fValid = fValid; }
	__inline BOOL IsValid() { return m_fValid; }

	BOOL IsSupported();
	BOOL IsBuildable();

private:
	// Is this config rec valid for the owning item?
	BOOL		m_fValid:1;

public:
	CProjItem *	m_pOwner;	// Owner of this project item

public:
	//
	// Action CSlob management
	//
	CActionSlobList * GetActionList();

protected:
friend class CProjItem;
friend class CProject;
	// File-set access is limited to CProjItem/CProject
	// FUTURE: remove all of this shit altogether...
	CFRFPtrArray m_arryOutputFiles;
	CFRFPtrArray * m_parryTargetFiles;
	CFileRegSet	* m_pTargetFiles;

private:
	CActionSlobList *	m_pActions;	// Our CActionSlobs that attach to this item

	// Corresponding record in project (also is key in config map)
	DWORD		m_LastUpdate;			
	void 		Refresh ();
	static  DWORD nSignature;
	static	int nCongfigCacheCount;

	// A form of attribute caching
	int			m_iAttrib;
	BOOL		m_fAttribDirty:1;
	CPath 		*m_pPathExtTrg;

#if 0
	// Removed 132 byte CFileRegSet to reduce build system memory footprint.
	// GetTargetFiles() now returns a pointer to a dynamicly created CFileRegSet
	// that must be deleted by the calling function.
	//
	// Storage set for a list of target file FileRegHandles
	// (used by GetTargetFiles())
	CFileRegSet	m_setTargetFiles;
#endif

public:
#ifdef _DEBUG
	virtual void AssertValid () const;
	virtual void Dump (CDumpContext &DC) const;
#endif
	CString strAllLine;
};

// Enum returned by calls to get build state:
enum BuildStates 
{
	Current,
	OutOfDate,
	DependencyOutOfDate,
	SourceMissing,
	Unknown,		// For external tools & makefiles.
};

// Helper object for config caching.  Garentees it will be reanbled
// when object goes out of scope:
class ConfigCacheEnabler
{
	BOOL	m_bDidEnable;
public:
	ConfigCacheEnabler ()  
	{ m_bDidEnable = TRUE; ConfigurationRecord::BeginConfigCache (); };
	~ConfigCacheEnabler () 
	{ if (m_bDidEnable) ConfigurationRecord::EndConfigCache (); };
	void EndCacheNow ()
	{if (m_bDidEnable) 
		{ ConfigurationRecord::EndConfigCache (); m_bDidEnable = FALSE; }
	};
};

struct SNode {
	void *data;
	SNode *next;
};

// CStack
class CStack {
public:
	CStack(){ head = NULL; };
	void Push( void * data );
	void *Pop(){
		if( !head ) return NULL;
		SNode *tmp = head; 
		void  *data = head->data;
		head = head->next;
		delete tmp;
		return data;
	}
	BOOL IsEmpty(){
		return( head == NULL );
	}
	~CStack(){		
		SNode *tmp, *tmp2;
		for( tmp = head; tmp != NULL; tmp = tmp2 ) {
			tmp2 = tmp->next;
			delete tmp;
		}
	}
private:
	SNode *head;
};

///////////////////////////////////////////////////////////////////////////////   
 
// definition of option behaviour
typedef WORD OptBehaviour;
#define OBNone				0x0000	// none of below
#define OBSetDefault		0x0001	// set defaults in the property bag (PARSE ONLY, NOT ROOT CProjItems)
#define OBAnti				0x0002	// string represents the 'un-set' options
#define OBClear				0x0004	// clear out the property bag of options not set (PARSE ONLY, NOT with Anti)
#define OBShowDefault 		0x0008	// show option defaults (GENERATE ONLY)
#define OBShowFake			0x0010	// show option fakes (GENERATE ONLY)
#define OBInherit			0x0020	// generate a string for a CSlob using option inheritance (GENERATE ONLY)
#define OBNeedEmptyString	0x0040	// parse/generate a string with empty strings ""
#define OBNoDepCheck		0x0080	// are we performing a dep. check using CProjItem::CheckDepOK()?
#define OBBadContext		0x0100	// are we in a bad context, ie. don't use defs. etc.
#define OBBasePropBag		0x0200  // use the base property bag instead of the current  one for parse/generate
#define OBShowMacro			0x0400	// $(macro) instead of value (GENERATE ONLY)

// Caches for dependency paths during makefile read.  This is used by
// the SuckDependencies method.
VOID InitDependencyCaches( );
VOID FreeDependencyCaches( );

class BLD_IFACE CProjItem : public CProjSlob 
{
//
//	Notes on CProjItem as it relates to CSlob:
//
//		A ProjItems container is simply the item to the left in the 
//		the File pane of the explorer.  This is the only dependency of
//		the ProjItem (in the CSlob sense).  Thus, AddDependent and
//		RemoveDependent should never need to be called.  
//		(Should changes slob.h so AddDep etc. are virtual, and then 
//		replace them.)
//
//		Perhaps the CSlobView should be a dependent as well, so it can
//		selectuvely reapint.
//
//
//		Only CProjContainer and its derived classes have a content list.
//
//
//	Property bag and configuartion tracking:
//		Each project has a CMapPtrToPtr which maps configurations in the item's
//		project to configuration records.  (For the project themselves, the keys
//		are always 0)  
//	  
	DECLARE_SERIAL (CProjItem)

public:
	//	Project item commands
	void CmdCompile(BOOL bMenu);
	void CmdReBuild();
	void CmdClean();
	void CmdBuild();
	void CmdBatchBuild(BOOL bBuildAll, CStringList * pConfigs, BOOL bRecurse = TRUE, BOOL bClean = FALSE);

	CProjItem ();
	virtual ~CProjItem ();

	// Derived classes chould call this when they delete.
	virtual void DeleteContents();		
	//
	//	Get item's moniker.  This must (along with the item's runtime class)
	//	be unique within its container.  Does not have to be user readable:
	//
	virtual void GetMoniker (CString&);
	//
	//	Compare two monikers to see if they're equal.  (Basically case 
	//	insensitive string compare):
	//
    static int CompareMonikers (CString&, CString&);

	//  Get filename associated with this item:
	virtual const CPath *GetFilePath () const; 
	virtual FileRegHandle GetFileRegHandle () const;
	virtual BOOL SetFile (const CPath *pPath, BOOL bCaseApproved = FALSE);

	//  Get list of FileRegHandles that for targets file of this item:
	virtual CFileRegSet * GetTargetPaths()
		{ASSERT(GetActiveConfig()); return GetActiveConfig()->GetTargetFilesList();}

	//  Get list of FileRegHandles that for output file of this item:
	//  Identical to the target list for source items, but proj/schmooze items
	// can have 'extra' outputs that aren't targets (.map, .ilk files, etc.)
	virtual CFileRegSet * GetOutputPaths()
#if 1
	{
		// This call should not be used.
		ASSERT(0);
		return NULL;
	}
#else
		{ASSERT(GetActiveConfig()); return GetActiveConfig()->GetOutputFilesList();}
#endif
	//
	//	Override CSlob's method:
	virtual BOOL SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption = TRUE);
	virtual void InformDependants (UINT idChange, DWORD dwHint = 0);

public:
	//
	// Containment
	//

	// Sets the owner project of this project item
	// and, returns the owner project of this project item
	__inline void SetProject(CProject * pProject) {m_pOwnerProject = pProject;}  
	__inline CProject * GetProject() {return m_pOwnerProject;}

	// Sets the owner target of this project item
	// and, returns the owner target of this project item
	__inline void SetTarget(CTargetItem * pTarget) {m_pOwnerTarget = pTarget;}  
	CTargetItem * GetTarget();
	const CString & GetTargetName();

	// Hook into the containment changes for this item
	// Note, derived classes should hook into the
	// PreMoveItem(), MoveItem() and PostMoveItem() methods
	BOOL SetSlobProp(UINT idProp, CSlob * val);	
	
	// Derived project items can call/override this behaviour
	//
	// 'pContainer' is the new container of the project item
	// 'fToPrj' is TRUE if the move is to a project
	// 'fFromPrj' is TRUE if the move is from a project

	// Removes project item from view
	// Note, can alter the destination 
	virtual BOOL PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);

	// Alters the containment and fixes configs. of item,
	// but not 'officially' in or out-of project
	virtual BOOL MoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);

	// Make item item 'officially' in or out-of project,    
	// adds project item to view
	virtual BOOL PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);

protected:
 	
	// our current 'owner' project, maybe '(CProject *)NULL' if none
	CProject *	m_pOwnerProject;

	// our current 'owner' target, maybe '(CTargetItem *)NULL', if none
	CTargetItem * m_pOwnerTarget;

public:

	//	Return pointer to the item's dependency list, or null if it 
	//	doesn't have one (this is a list of FileRegHandles):
	virtual CFileRegSet * GetDependenciesList() const {return (CFileRegSet *)NULL;}
	
	//  Scans the item and children, refreshing the dependency lists and 
	//	containers along the way.  Returns FALSE if the user bailed or 
	//	there was an error:
	virtual BOOL ScanDependencies (BOOL bUseAnyMethod = FALSE, BOOL bWriteOutput = TRUE)
	{
		return TRUE;
	}

	//  Can we do a build?
	virtual BOOL CanDoTopLevelBuild ();

	//  Can we do a scan?
	virtual BOOL CanDoScan ();

	//  Called for the highest level item to be built:
	virtual BOOL DoTopLevelBuild (UINT buildType,
								  CStringList * pConfigs = NULL, 
								  FlagsChangedAction fca = fcaNeverQueried,
								  BOOL bVerbose = TRUE,
                                  BOOL bClearOutputWindow = TRUE,
                                  BOOL bRecurse = TRUE);

	virtual BOOL UsesSourceTool () {return FALSE;}
	virtual BOOL UsesSchmoozeTool () {return FALSE;}

	__inline CBuildTool * GetSourceTool()
		 { return (CBuildTool *)GetActiveConfig(TRUE)->GetBuildTool();}

	//	List the items child in order, followed by the item.
	enum FlattenOptions 
	{
		flt_Normal			   		= 0,
		flt_ExpandSubprojects  		= 1,
		flt_ExcludeProjects    		= 2,
		flt_OnlyProjects       		= 4,
		flt_RespectItemExclude 		= 8,
		flt_ExcludeDependencies		= 16,
		flt_RespectTargetExclude 	= 32,
		flt_ExcludeGroups			= 64,
		flt_ExcludeProjDeps 		= 128,
		flt_OnlyTargRefs			= 256
	};
	void FlattenSubtree ( CObList& rObList, int fo );
	virtual void FlattenQuery (int fo, BOOL& fAddContent, BOOL& fAddItem);

#define flt_BuildableItems (CProjItem::flt_Normal | CProjItem::flt_ExcludeProjects | CProjItem::flt_ExcludeGroups | CProjItem::flt_RespectItemExclude | CProjItem::flt_ExcludeDependencies | CProjItem::flt_RespectTargetExclude | CProjItem::flt_ExcludeProjDeps)

	// Get the containing registry for this projitem.
	virtual CFileRegistry* GetRegistry();
 
	// Writes out the properties listed in the SERIAL_SLOBPROP_MAP:
	// (For containers this does this contents as well.
	virtual BOOL SerializeProps(CArchive &ar);

	static const CString EndOfBagsRecord;

	// Do we want to search our container's property pages/bags
	// if the prop. cannot be found in our own?
	// return the old inherit flag.
	__inline BOOL EnablePropInherit(BOOL fEnable = TRUE) 
	{
		BOOL fWasInherit = m_optbeh & OBInherit;
		if (fEnable)
			m_optbeh |= OBInherit;
		else	 
			m_optbeh &= ~OBInherit;
		return fWasInherit;
	}

	//	Do we want to use option defaults?
	__inline BOOL UseOptionDefaults(BOOL fEnable = TRUE)
	{
		BOOL fWasDefaults = m_optbeh & OBShowDefault;
		if (fEnable)
			m_optbeh |= OBShowDefault;
		else	 
			m_optbeh &= ~OBShowDefault;
		return fWasDefaults;
	}

	//	Do we want to provide an 'anti-' value for the property?
	__inline BOOL EnableAntiProps(BOOL fEnable = TRUE)
	{
		BOOL fWasAnti = m_optbeh & OBAnti;
		if (fEnable)
			m_optbeh |= OBAnti;
		else	 
			m_optbeh &= ~OBAnti;
		return fWasAnti;
	}

	// Set the option behaviour 'en masse'
	__inline OptBehaviour SetOptBehaviour(OptBehaviour optbeh)
	{
		OptBehaviour optbehOld = m_optbeh; m_optbeh = optbeh; return optbehOld;
	}

	__inline OptBehaviour GetOptBehaviour()
	{
		return m_optbeh;
	}

	// Getting container's to match our config.
	// Use if you are likely to be called when the configs.
	// across related CProjItems might not match
	virtual CSlob * GetContainerInSameConfig();

	// Must reset the container we got from GetContainer...() after using it
#define ResetContainerConfig ResetManualBagSearchConfig

	// Set manual bag config. to be used
	__inline void ResetManualBagSearchConfig()
	{
		ASSERT(!m_stkOldConfigRec.IsEmpty());		
#if 0
		m_pManualConfigRec = (ConfigurationRecord *)m_stkOldConfigRec.Pop();
#else
		m_pManualConfigRec = (ConfigurationRecord *)m_stkOldConfigRec.RemoveTail();
#endif
	}

	__inline void SetManualBagSearchConfig(ConfigurationRecord * pRec)
	{
#if 0
		m_stkOldConfigRec.Push(m_pManualConfigRec);
#else
		m_stkOldConfigRec.AddTail(m_pManualConfigRec);
#endif
		m_pManualConfigRec = pRec;
	}

	// Get a pointer to our configuration array, which is ordered
	// to minimize diffs.  Use for iterating through configs.
	__inline const CPtrArray *GetConfigArray() const
		{return  &m_ConfigArray;}

	// Get a pointer to our configuration map for quick lookup
	// N.B. Do not use for iteration (slow!).  Use GetConfigArray() instead.
	__inline const CMapPtrToPtr *GetConfigMap() const 
		{return &m_ConfigMap;}

	// Force a configuration to be the active one, irrespective of the owner
	// project's current configuration.
	__inline void ForceConfigActive(ConfigurationRecord * pConfig = (ConfigurationRecord *)NULL /* reset */)
	{
		m_pForcedConfig = pConfig;
	}

	// Force a configuration (based on the base config record in the top project)
	// to be the active one, irrespective of the owner project's current configuration.
	__inline void ForceBaseConfigActive(ConfigurationRecord * pConfig)
	{
		ASSERT(pConfig != (ConfigurationRecord *)NULL);
		VERIFY(m_ConfigMap.Lookup((void *)pConfig, (void *&)m_pForcedConfig));
	}

	// Get the active configiguration (create if necessary).
	ConfigurationRecord * GetActiveConfig(BOOL fCreate = FALSE);

	// Make sure we have all of our configs matching the project.
	void CProjItem::CreateAllConfigRecords();

	// Get a matching configuration for a name, or for a base config. record,
	// 'fCreate' if TRUE will create if does not already exist.
	ConfigurationRecord * ConfigRecordFromConfigName(const TCHAR * pszConfig, BOOL fCreate = FALSE, BOOL fMatchExact = TRUE);
	ConfigurationRecord * ConfigRecordFromBaseConfig(ConfigurationRecord * pcrBase, BOOL fCreate = FALSE);

	// Pointer to the active configuration (tracks owner project's config.)
	ConfigurationRecord * m_pActiveConfig;

	// Pointer to a forced configuration    
	ConfigurationRecord * m_pForcedConfig;

// FUTURE
//
// TO REMOVE AND REPLACE with GENERIC property bag handling

	// Reset the current default configuration or the current configuration 
	__inline void SetCurrentConfigAsBase()
		{ASSERT(GetActiveConfig()); GetActiveConfig()->BagCopy(CurrBag, BaseBag);}

	__inline void SetBaseConfigAsCurrent()
		{ASSERT(GetActiveConfig()); GetActiveConfig()->BagCopy(BaseBag, CurrBag);}

	// Copying the current config to the default for a particular tool only
	void SetCurrentConfigAsBase(COptionHandler * popthdlr);
	void SetBaseConfigAsCurrent(COptionHandler * popthdlr);

// FUTURE


	__inline int UsePropertyBag(UINT idBag = (UINT)-1)
	{
		int idOldBag = m_idBagInUse;
		if (idBag != (UINT)-1)
			m_idBagInUse = idBag;
		return idOldBag;
	}

private:
	UINT m_idBagInUse;	// property bag to use

public:
	virtual CProjType * GetProjType();

	//	Override of CSlob's get props.	Do this to allow imp. of CPropBag hierarchy
	//	N.B. The only prop types we require so far are integer, string.
	virtual	GPT GetIntProp(UINT idProp, int& val);
	virtual GPT GetStrProp(UINT idProp, CString& val);

	virtual BOOL SetIntProp(UINT idProp, int val);
	virtual BOOL SetStrProp(UINT idProp, const CString& val);

	//	These are stubbed for now :-) [matthewt]
	virtual GPT GetDoubleProp(UINT idProp, double& val)	{ASSERT(FALSE); return invalid;}
	virtual	GPT GetLongProp(UINT idProp, long& val)	{ASSERT(FALSE); return invalid;}
	virtual GPT GetSlobProp(UINT idProp, CSlob*& val)	{ASSERT(FALSE); return invalid;}
	virtual GPT GetRectProp(UINT idProp, CRect& val)	{ASSERT(FALSE); return invalid;}
	virtual GPT GetPointProp(UINT idProp, CPoint& val)	{ASSERT(FALSE); return invalid;}

	// Property bag handling methods for base, clone and current.
	virtual int GetBagHashSize(){ return( 53 ); }

	//	Override the CPropBag query so that we can handle configuration props
	virtual int GetPropBagCount() const;

	//	FUTURE: implement these using a generic GetConfigRec()
	virtual CPropBag * GetPropBag(int nBag = -1);
	virtual CPropBag * GetPropBag(const CString & strBagName);

	//  Override CSlob's serialize filter fn to save only private props:
	virtual BOOL SerializePropMapFilter(int nProp);

	//	Our container's configuration has changed
	virtual BOOL ConfigurationChanged(const ConfigurationRecord *pBaseRecord);

	// .MAK reading and writing
	virtual BOOL WriteToMakeFile(CMakFileWriter& pw, CMakFileWriter& mw, const CDir &BaseDir, CNameMunger& nm);
	virtual BOOL ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir);

	// Drop/Drag cursor support
	virtual HCURSOR GetCursor(int ddcCursorStyle);

protected:
	// Helpers for makefile reading writing.  These return the three letter item
	// type (e.g. "SRC" from a file item) or get the runtime class given
	// an item type:
	 
	static const TCHAR *GetNamePrefix (CProjItem *pItem);
	static CRuntimeClass *GetRTCFromNamePrefix (const TCHAR *);
	static const TCHAR *EndToken;			// "END"
	static const TCHAR *pcFileMacroName;		// "SOURCE"
	static const TCHAR *pcFileMacroNameEx;		// " $(SOURCE) "

	// Is this the END mak token?  Retruns true if it is. Also NULL the
	// pointer and deletes the object. Throws a file exception if its a MakEndOfFile:
	static BOOL  IsEndToken (CObject *& pToken );
								 // (throw CFileException)

	// Read in anything we need from inference rules / build rules
	BOOL SuckDescBlk(CMakDescBlk *pDescBlk);

	// Dispatch a macro to the appropriate tool and set our options.  If its the
	// DEPS macro, then setup our depencies list:
	BOOL SuckMacro(CMakMacro *pMacro, const CDir &BaseDir, BOOL bCaseApproved = FALSE);

public:
	// Parse a dependency macro string and return the file registry handle.
	FileRegHandle SuckDependency(TCHAR *pchDep, CBuildTool * pTool, const CDir &BaseDir, BOOL & fDepIsPerConfig, BOOL bCaseApproved = FALSE);

	// Helper functions to parse a string, replacing $(OUTDIR) with actual value
	BOOL SubstOutDirMacro(CString & str, const ConfigurationRecord * pcr);

	// Helper functions to generate appropriate $(OUTDIR) macro string
	virtual BOOL GetOutDirString(CString & str, CString * pstrValue = NULL, BOOL bMacro = FALSE, BOOL fRecurse = FALSE);
	BOOL GetOutDirString(CString & str, UINT PropId, CString * pstrValue = NULL, BOOL bMacro = TRUE, BOOL fRecurse = FALSE);

	// Write Output directory macro, desc. block, if necessary
	BOOL WriteOutDirMacro(CMakFileWriter & mw);
	BOOL WriteOutDirDescBlk(CMakFileWriter & mw);

	// Write out the !ifdef / !elseif / !endif block for a configuration block:
	enum ConfigPosition { FirstConfig, MiddleConfig, EndOfConfigs };
	void WriteConfigurationIfDef ( 
							CMakFileWriter& mw,
							const ConfigurationRecord *pcr
							);	 // (throw CFileException)
	// Read in a configuration ifdef and force ourself into that configuration:
	void ReadConfigurationIfDef(CMakDirective *pmd, BOOL bCheckProj = FALSE);
							// (throw CFileException if unknown configuration)

	// A small number of properties are written to the makefile directly, 
	// embedded in comments.  These are per configuration:
	BOOL WriteMakeFileProperties (CMakFileWriter& mw);

	// A small number of non per config properties are written to the
	// makefile directly, embedded in comments. These are NOT per configuration
	BOOL WritePerItemProperties (CMakFileWriter& mw);

    // Items that want to write per config dependency info can do so
    // in this method. Most items don't do anything here, except for
    // CFileItems which will write out their deps list if they are
    // per config
    virtual BOOL WriteMakeFileDependencies (CMakFileWriter& mw, CNameMunger& nm, BOOL bPerConfig = FALSE);

	// See if there are any props in the item's prop bag that need to be
	// written to the makefile:
	BOOL HasMakefileProperties () const;

	// See if this is a comment holding a prop value.  If so, digest it _and_
	// delete it and set pObject= NULL 
	BOOL IsPropComment (CObject *&pObject);
						// (throw CFileException)

	// See if this is a comment hodling an ADD/SUBTRACT line for some tool.
	// If it is, digest it _and_ delete it and set pObject= NULL

	int	CountInterestingChildren ();
	// Count how many projects, groups, andfile item's, this item has as
	// children.
public:
	static const TCHAR *pcADD;
	static const TCHAR *pcSUBTRACT;
	static const TCHAR *pcBASE;
protected:

	BOOL IsAddSubtractComment (CObject *&pObject);
						// (throw CFileException)

	enum PropWritingOptions
	{
		PropRead = 0x01,
		PropWrite = 0x02,
		Project = 0x04,
		Group = 0x08,
		File = 0x10,
		Target = 0x20,
		TargetRef = 0x40,
		All = 0x7c,
		NotPerConfig = 0x80
	};
	struct PropWritingRecord
	{
		UINT idProp;
		const TCHAR *pPropName;
		PROP_TYPE type;
		int nRdWrt;
	};
	static const PropWritingRecord nMakeFileProps[];
	static const TCHAR *pcPropertyPrefix;

	// When we write out the configurations, we want to do it in a repeatable
	
public:
	// Return name of a dependency macro or false if we have none
	// depType == DEP_Scanned or DEP_Missing
	BOOL GetDependenciesMacro(UINT depType, CString &strDeps, CNameMunger& nm);

	// Return the file name macro:
	static const TCHAR *GetFileNameMacro() {return pcFileMacroNameEx;}

	//	Used to keep track of config state for makefile reading/writing
	//  REVIEW(kperry): since we only ever deal with one projitem at a time, this could probably be made static.
	ConfigPosition m_cp;

	// Return a container-less, symbol-less, clone of 'this'
	virtual CSlob* Clone();

protected:

	// See if any items in the oblist produce the same target files are
	// produce an error box if they do,
	static BOOL CheckForDuplicateTargets ( CObList &ol, UINT idMess = -1 );

protected:

	void DeleteConfigFromRecord (ConfigurationRecord *prec, BOOL bChildren = TRUE);

	// Copy the configuration structure of another item:
	virtual void CloneConfigs (const CProjItem *pItem);

	// We over-ride this so we can set up the configs
	// prior to the CSlob::CopyProps() operation
	virtual void CopyProps(CSlob *);

	// Copying the default property bags other CProjItem's
	virtual void CopyDefaultPropBags(CProjItem * pItemClone);

	//	Configuration record map and array
	CMapPtrToPtr m_ConfigMap;
 	CPtrArray m_ConfigArray;

	BOOL m_fGettingListStr:1; // Hack to stop recursing for ::GetStrProp on a list string

	//	Property bag searching heuristics
	BOOL m_fPropsDepFigure:1;	//	Property search is figuring deps?
	OptBehaviour m_optbeh;	//	Property search behaviour


#if 1
	//	Pointer to config. we wish to match for manual bag search:
	ConfigurationRecord * m_pManualConfigRec;

	//	A stack of cached record ptrs. so that we
	//	can nest the calls to SetManualBagSearch...
	CObList m_stkOldConfigRec;
#else
	//	Pointer to config. we wish to match for manual bag search:
	ConfigurationRecord * m_pManualConfigRec;
	CStack m_stkOldConfigRec;
#endif





	//	Our bitmap inset for Project Window
	UINT	m_nIconIdx;

	//	Our internal CanAdd method - tells us what WE can add
	//	rather than what the user can add
	//	So for CProjGroup's we can add both CFileItems and
	//	CDependencyContainer's. However this is different
	//	to what CanAdd says we can add - CanAdd only allows
	//	CFileItems, since a user can never explicitly add
	//	a dependency container (we always do it when it is
	//	necessary!).
	virtual BOOL CanContain(CSlob* pSlob);

public:
    virtual BOOL CanAct(ACTION_TYPE action);
	// called when removing an item from the project
	void IsolateFromProject();
	void SetOwnerProject(CProject * pProject, CTargetItem * pTarget);

    void CopyCommonProps(CProjItem * pItemSrc, const CString & strTarget);
	virtual int CompareSlob(CSlob *pCmpSlob);

private:	
	// Remove any unreferenced configuration records and impose the config
	// ordering of a supplied CProjItem on this item and all its children.
	// (Used only in CloneConfigs)
	void FixConfigs(const CProjItem * pOrderedItem);

	DECLARE_SLOBPROP_MAP()

#ifdef _DEBUG
	virtual void AssertValid () const;
	virtual void Dump (CDumpContext &DC) const;
#endif
};

///////////////////////////////////////////////////////////////////////////////

class BLD_IFACE CFileItem : public CProjItem 
{
	DECLARE_SERIAL (CFileItem)

protected:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);

	//	File item commands
	//{{AFX_MSG(CFileItem)
	afx_msg void OnOpen();
	//}}AFX_MSG(CFileItem)

public:
	CFileItem();
	virtual ~CFileItem()
	{
#ifndef REFCOUNT_WORK
		if (NULL!=m_hndFileReg)
			g_FileRegistry.ReleaseRegRef(m_hndFileReg);
#else
		if (NULL!=m_hndFileReg)
			m_hndFileReg->ReleaseFRHRef();
#endif

		DeleteContents ();
	}

	virtual void GetMoniker (CString&);

	// Property bag handling methods for base, clone and current.
	virtual int GetBagHashSize(){ return( 3 ); }

	// Property storage and retrieval
	virtual BOOL SetStrProp(UINT idProp, const CString& val);
	virtual BOOL SetIntProp(UINT nIDProp, int val);
	virtual GPT GetStrProp(UINT nIDProp, CString & val);
	virtual	GPT GetIntProp(UINT idProp, int& val);

	// Sending out add/delete file notifications
	virtual BOOL PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);
	virtual BOOL MoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);
	virtual BOOL PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);
	
	// Property page UI
	virtual BOOL SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption = TRUE);

	// Setting and retrieving the filename
	virtual BOOL SetFile (const CPath *pPath, BOOL bCaseApproved = FALSE);
	virtual FileRegHandle GetFileRegHandle() const {return m_hndFileReg;}

	virtual CFileRegSet * GetDependenciesList() const;

	// Override this so we can assert, since no one should be doing this on any
	// project items anymore
	virtual CSlobWnd * GetSlobWnd() { ASSERT(FALSE); return CSlob::GetSlobWnd(); }

	virtual BOOL UsesSourceTool() {return TRUE;}

	virtual BOOL ScanDependencies(BOOL bUseAnyMethod = FALSE, BOOL bWriteOutput = TRUE);

	virtual BOOL UpdateDependencies(CPtrList *pdepList);

	// .MAK reading and writing
	virtual BOOL WriteToMakeFile(CMakFileWriter& pw, CMakFileWriter& mw, const CDir &BaseDir, CNameMunger& nm);
    virtual BOOL WriteMakeFileDependencies(CMakFileWriter& mw, CNameMunger& nm, BOOL bPerConfig = FALSE);
	virtual BOOL ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir);

	// Flattening of projitems in a tree.
	virtual void FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem);

	// Return a container-less, symbol-less, clone of 'this'
	virtual CSlob* Clone();

    // Can only delete file items when we are not building
    virtual BOOL CanAct(ACTION_TYPE action);
	virtual BOOL PreAct(ACTION_TYPE action);

public:
	FileRegHandle   m_hndFileReg;	// our filename registry handle

	// by default, file items are relative to project dir
	BOOL m_bIsInProjDir:1;
	BOOL m_bAlwaysWriteDepMacro:1;

private:
	BOOL m_fDisplayName_Dirty:1;		// when to cache?
	BOOL m_bExcludedFromScan:1;		// ignored by ClassView, etc.
	CString	m_strMoniker;			// cached relative path of fileitem
	CString m_strDisplayName;		// cache our display name
	CString m_strOrgPath;			// name as we want to write it in the .dsp
	CString m_strFakeOrgPath;			// name as we want to write it in the .dsp


	DECLARE_SLOBPROP_MAP()
};

///////////////////////////////////////////////////////////////////////////////
class CProjContainer : public CProjItem 
{
//
//	Notes on CProjContainer as it relates to CSlob:
//
//		The add file/remove file functionality comes through the 
//		CSlob::Add and CSlob::Remove functions.
//
//		Only CProjContainer and its derived classes have a content list.
//
//
	DECLARE_SERIAL (CProjContainer)

protected:
//	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
//		AFX_CMDHANDLERINFO* pHandlerInfo);

public:
	//	Project container commands
	//{{AFX_MSG(CProjContainer)
	afx_msg void CmdAddGroup();
	//}}AFX_MSG(CProjContainer)

public:
	CProjContainer ();
	virtual ~CProjContainer ();

private:	

	CObList		m_objects;	// Actual list of contained ProjItems.

public:
	//	Get denpendencies container addociated with this group.
	//	will create one if it doesn't exist:
	virtual CDependencyContainer *GetDependencyContainer (BOOL bMake = TRUE);

    // Causes all dep cntrs associated with this container to be refreshed
    void RefreshAllMirroredDepCntrs();

	//	Causes assocaited dependcy container to refill itself with the 
	//  with the dependencies of the conatiner and its children, where 
	//	appropriate.  Does not cause a rescan:
	virtual void RefreshDependencyContainer ();

	// Create a new group with the standard name and add it.
	CProjGroup *AddNewGroup (const TCHAR * pszName = NULL, const TCHAR * pszFilter = NULL);
	__inline void GetBuildableItems(CObList & ol) { FlattenSubtree( ol, flt_BuildableItems); }

	//	CSlob content access method:
	virtual CObList *GetContentList() { return &m_objects; };
	void SortContentList();

	virtual BOOL CanRemove ( CSlob *pRemoveSlob );

	virtual void Add ( CSlob *pAddSlob );
	virtual void Remove ( CSlob *pRemoveSlob );

	// Addfiles method, called by the add verb (with subgroups = FALSE)
	// or by the main menu (for the entire project)

	virtual void DoAddFiles ( BOOL bSubgroups );

	// Scan the dependencies of the items in the container and refresh its
	// depency container:
	virtual BOOL ScanDependencies (BOOL bUseAnyMethod = FALSE, BOOL bWriteOutput = TRUE);

	virtual BOOL Compile() {ASSERT (FALSE); return FALSE;}	

	// Writes out the properties listed in the SERIAL_SLOBPROP_MAP _and_ the 
	// data for contained itiems:
	virtual BOOL SerializeProps(CArchive &ar);

	// Find a contained item based on moniker and runtime class:
	CProjItem *FindItem (CString strMoniker, CRuntimeClass *pRTC);

	// Finds a group given it's string.
	CProjGroup *FindGroup (const TCHAR * pszName = NULL);

	// convert old-style deps into project folders
	void ConvertDeps();
public:
	virtual BOOL ReadInChildren(CMakFileReader& mr, const CDir &BaseDir);
private:
	static const CString EndOfSerializeRecord;
	CMapStringToPtr *m_pFindItemCache;	// Cache used by FindItem.
	DECLARE_SLOBPROP_MAP()
};
///////////////////////////////////////////////////////////////////////////////
class CProjGroup : public CProjContainer 
{
//
//	Represents one those group items.  Never contains a resource conatiner or
//	group, but can contain CProject's and other groups:
//
//	Basically, the same thing as a CProjContainer.
//
	DECLARE_SERIAL (CProjGroup)

	virtual void GetMoniker (CString&);

protected:
	//	Project container commands
	void CmdAddFiles();

	// Groups must have unique names within a project.  Functions will munge
	// this items name to make it unique if needed:
	BOOL MakeGroupNameUnique (CString & strGroupName);

	// Special Reading code for groups, as we should only see groups in old v2
	// makefiles and so this code is part of the v2 to v3 conversion code
public:
	virtual BOOL ReadInChildren(CMakFileReader& mr, const CDir &BaseDir);

	CProjGroup ();
	CProjGroup (const TCHAR * pszName);
	virtual ~CProjGroup () { DeleteContents ();};

	// The CanAdd method tells us what it makes sense for
	// a user to be able to add to this slob container.
	// So for a CProjGroup a user can add CFileItem's but
	// nothing else. NOTE: This is different to our
	// CanContain method, which tells us what we can
	// add from an internal (within VPROJ code) viewpoint.
	virtual BOOL CanAdd ( CSlob * pSlob );	
	virtual BOOL CanAct(ACTION_TYPE action);

	virtual GPT GetStrProp ( UINT idProp, CString &str );
	virtual BOOL SetStrProp ( UINT idProp,  const CString &str );
	virtual BOOL SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption = TRUE);

	// Ensuring group name uniqueness.
	virtual BOOL PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);
	virtual BOOL MoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);
	virtual BOOL PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);

	// .MAK reading and writing
	virtual BOOL WriteToMakeFile(CMakFileWriter& pw, CMakFileWriter& mw, const CDir &BaseDir, CNameMunger& nm);
	virtual BOOL ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir);

	// Override of default CSlob function to test for duplicate filenames,
	// multiple .rc/.def files, etc. when adding/pasting to a group
	virtual void PrepareAddList(CMultiSlob* pAddList, BOOL bPasting);

	//	Our internal CanAdd method - tells us what WE can add
	//	rather than what the user can add
	//	So for CProjGroup's we can add both CFileItems and
	//	CDependencyContainer's. However this is different
	//	to what CanAdd says we can add - CanAdd only allows
	//	CFileItems, since a user can never explicitly add
	//	a dependency container (we always do it when it is
	//	necessary!).
	virtual BOOL CanContain(CSlob* pSlob);

	// Drop and Drag support
	virtual void PrepareDrop(CDragNDrop *pInfo);
	virtual CSlobDragger* CreateDragger(CSlob *pDragObject, CPoint screenPoint);
	virtual void	DoDrop(CSlob *pSlob, CSlobDragger *pDragger);
	virtual void	DoDrag(CSlob *pSlob, CSlobDragger *pDragger, CPoint screenPoint);

	virtual void	FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem)
		{ fAddContent = TRUE; fAddItem = ((fo & flt_ExcludeGroups)==0); }
private:
	CString	m_strGroupName;
	CPtrList m_DeathRow;	// list of Slobs to be delete during a Drop
	CString m_strDefaultExtensions;  // P_GroupDefaultFilter

	DECLARE_SLOBPROP_MAP()
};
 
///////////////////////////////////////////////////////////////////////////////
class CDependencyContainer : public CProjContainer 
{
//	Class for holding dependency lists. 

	DECLARE_SERIAL (CDependencyContainer)

	CDependencyContainer();
	
	virtual void GetMoniker (CString&);

private:	

	// CObList		m_objects;	// Actual list of contained CDependencyItem.
	ConfigurationRecord * m_pcrBase;	// The base record for the config for
										// which this dep cntr is valid

public:
	virtual ~CDependencyContainer () { DeleteContents ();};

	// The CanAdd method tells us what it makes sense for
	// a user to be able to add to this slob container.
	// So for a CProjGroup a user can add CFileItem's but
	// nothing else. NOTE: This is different to our
	// CanContain method, which tells us what we can
	// add from an internal (within VPROJ code) viewpoint.
	virtual BOOL CanAdd ( CSlob *pAddSlob );

	virtual BOOL CanAct(ACTION_TYPE action);

	// Allow us to fake some of our props, ie. P_ProjItemName
	virtual GPT GetStrProp ( UINT idProp, CString & val );

	// Ensure there is always *only* one depdendency container
	// in any one project.
	virtual BOOL PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);

	virtual BOOL SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption = TRUE);

	// Get our dependencies contained.
	//__inline CObList * GetContents()	{return &m_objects;}
	__inline CObList * GetContents()	{ ASSERT(0); return NULL;}

	//	Our internal CanAdd method - tells us what WE can add
	//	rather than what the user can add
	//	So for CProjGroup's we can add both CFileItems and
	//	CDependencyContainer's. However this is different
	//	to what CanAdd says we can add - CanAdd only allows
	//	CFileItems, since a user can never explicitly add
	//	a dependency container (we always do it when it is
	//	necessary!).
	virtual BOOL CanContain(CSlob* pSlob);
	
	//	This should never be called or we'll get some recursion:
	virtual CDependencyContainer *GetDependencyContainer (BOOL bMake = TRUE)
	{	ASSERT (0); return FALSE;}
	virtual void RefreshDependencyContainer() {}

	// Replace CProjContainer's implementation to do nothing:
	virtual BOOL ScanDependencies (BOOL bUseAnyMethod = FALSE, BOOL bWriteOutput = TRUE)
	{
		return TRUE;
	}

	virtual void FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem);

	BOOL IsValid(ConfigurationRecord * pcrBase = NULL);
    void SetValid(ConfigurationRecord * pcrBase) { m_pcrBase = pcrBase; }

	DECLARE_SLOBPROP_MAP()
};
///////////////////////////////////////////////////////////////////////////////
class CDependencyFile : public CProjItem 
{
	DECLARE_SERIAL (CDependencyFile)
	
protected:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra,
		AFX_CMDHANDLERINFO* pHandlerInfo);

	//	Dependency file item commands
	//{{AFX_MSG(CDependencyFile)
	afx_msg void OnOpen();
	//}}AFX_MSG(CDependencyFile)

public:
	CDependencyFile ();
	virtual ~CDependencyFile ()
	{
		DeleteContents ();
#ifdef REFCOUNT_WORK
		if (NULL!=m_hndFileReg)
			m_hndFileReg->ReleaseFRHRef();
#endif
	}
	
	void SetFileRegHandle (FileRegHandle hndFileReg);
	virtual FileRegHandle GetFileRegHandle () const { return m_hndFileReg; };
	virtual BOOL SetFile (const CPath *pPath, BOOL bCaseApproved = FALSE);

	virtual BOOL SetupPropertyPages(CSlob* pNewSel, BOOL bSetCaption = TRUE);

	virtual GPT GetStrProp(UINT idProp, CString& val);
	
	// Ensure that we are always dropped into a dependency container.
	virtual BOOL PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);
	BOOL SetSlobProp(UINT idProp, CSlob * val);	

	virtual BOOL CanAct(ACTION_TYPE action);

	virtual void FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem);

	// Property bag handling methods for base, clone and current.
	virtual int GetBagHashSize(){ return( 3 ); }

	DECLARE_SLOBPROP_MAP()

private:
	FileRegHandle   m_hndFileReg;

	CString m_strDisplayName;	// cache our display name
	BOOL m_fDisplayName_Dirty;	// when to cache?

};

#include "project.h"

//////////////////////////////////////////////////////////////////////////////

class CProjDragger : public CSlobDragger
{
public:
	virtual void Show(BOOL bShow = TRUE) {};
	virtual CRect GetRect() { return CRect(0, 0, 0, 0); };
	virtual void Move(const CRect& newRect, BOOL bForceShow = FALSE) {};
};

#endif  // __PROJITEM__
