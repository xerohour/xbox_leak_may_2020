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
				+-- CFileItem   +-- CTimeCustomBuildItem
				|               +-- CTargetReference
				|
				+--	CProjContainer	+
									|
									+-- CProject
									+--	CProjGroup

	CProjViewItem is the base class for all items displayed in the shell's
	project view.		


*/
#ifndef __PROJITEM__
#define __PROJITEM__

#include "engine.h"

class CTargetItem;

class COptionHandler;

class CProjItem;
class CFileItem;

class CProjContainer;
class CProject;
class CProjGroup;
class CTimeCustomBuildItem;

class CBuildTool;
class CSchmoozeTool;
class CToolDirectory;

class CProjectView;

// Makread.h:
class CMakComment;
class CMakMacro;
class CMakDescBlk;
class CMakDirective;
class CMakError;
class CMakEndOfFile;
class CMakFileReader;
class CNameMunger;

// Config stuff:
class CProjType;


typedef CTimeCustomBuildItem * LPCUSTOMBUILDEVENT;

///////////////////////////////////////////////////////////////////////////////    
// Property bag to extract from Configuration record?
#define BaseBag		0
#define CloneBag	1
#define CurrBag		2

class  CConfigurationRecord : public CObject
{
	DECLARE_DYNAMIC (CConfigurationRecord);

public:
	//
	// Construction and destruction
	//
	CConfigurationRecord(const CConfigurationRecord * pBaseRecord, CProjItem * pItemOwner);
	~CConfigurationRecord();

	//
	// Record linkage
	//
	const CConfigurationRecord * m_pBaseRecord;

	//
	// Configuration record information retrieval
	//
	CString &GetConfigurationName () const;
	void GetConfigurationDescription (CString &) const;
	void GetProjectName (CString &) const;
	CString &GetOriginalTypeName  () const;

	// Get the target attributes of an internal/external project target
	int GetTargetAttributes();

	//
	// Configuration record property management
	//

	// Return a pointer to a particular bag in the config. record
	// FUTURE: On demand creation of property bags?
	__inline CPropBag * GetPropBag(UINT idBag)
		{
			if( idBag == BaseBag )
				return &m_BasePropBag;
			else if( idBag == CloneBag )
				return m_pClonePropBag;
			else 
				return &m_CurrPropBag;
		}

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

	//
	// Access functions to determine if this config rec is valid for the
	// owning item/set the validity of this config rec for the owning item.
	//
	__inline void SetValid(BOOL fValid) { m_fValid = fValid; }
	__inline BOOL IsValid() { return m_fValid; }

	BOOL IsSupported();

	BOOL SetBuildToolsMap();
	CVCPtrList * GetBuildToolsMap() {return m_pToolsMap;}
	BOOL GetExtListFromToolName(CString strTool, CString *strExt);
	BOOL SetExtListFromToolName(CString strTool, CString strExt);
	//
	// Action CSlob management
	//
	CBuildActionList * GetActionList();

	CProjItem *	m_pOwner;	// Owner of this project item

	// list of tools associated with this project and user defined extentions
	// for those tools
	CVCPtrList *	m_pToolsMap;

protected:
friend class CProjItem;
friend class CProject;

private:
	// Is this config rec valid for the owning item?
	BOOL			m_fValid:1;
	CBuildActionList *m_pActions;	// Our CBuildActions that attach to this item

	// A form of attribute caching
	int				m_iAttrib;
	BOOL			m_fAttribDirty:1;

	// Our property bags for this configuration
	// for each type of property bag,
	// o base
	// o cloned
	// o current
	CPropBag		m_BasePropBag;
	CPropBag	*	m_pClonePropBag;	// this guy is only needed when settings dialog is up.
	CPropBag		m_CurrPropBag;
	
public:
#ifdef _DEBUG
	virtual void AssertValid () const;
	virtual void Dump (CDumpContext &DC) const;
#endif
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

class  CProjItem : public CProjSlob 
{
//
//	Notes on CProjItem as it relates to CSlob:
//
//		A ProjItems container is simply the item to the left in the 
//		the File pane of the explorer.  This is the only dependency of
//		the ProjItem (in the CSlob sense).  Thus, AddDependent and
//		RemoveDependent should never need to be called.  
//
//		Perhaps the CSlobView should be a dependent as well, so it can
//		selectuvely reapint.
//
//
//		Only CProjContainer and its derived classes have a content list.
//
//
//	Property bag and configuartion tracking:
//		Each project has a CVCMapPtrToPtr which maps configurations in the item's
//		project to configuration records.  (For the project themselves, the keys
//		are always 0)  
//	  
	DECLARE_SERIAL (CProjItem)

public:
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
	virtual BOOL SetFile (const CPath *pPath, BOOL bCaseApproved = FALSE);

	//
	//	Override CSlob's method:
	virtual void InformDependants (UINT idChange, DWORD_PTR dwHint = 0);

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
		flt_OnlyTargRefs			= 256,
		flt_TargetInclude			= 512,
		flt_OnlyCustomStep			= 1024
	};
	void FlattenSubtree ( CObList& rObList, int fo );
	virtual void FlattenQuery (int fo, BOOL& fAddContent, BOOL& fAddItem);

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
		VSASSERT(!m_stkOldConfigRec.IsEmpty(), "Reset called before set!");		
		m_pManualConfigRec = (CConfigurationRecord *)m_stkOldConfigRec.RemoveTail();
	}

	__inline void SetManualBagSearchConfig(CConfigurationRecord * pRec)
	{
		m_stkOldConfigRec.AddTail(m_pManualConfigRec);
		m_pManualConfigRec = pRec;
	}

	// Get a pointer to our configuration array, which is ordered
	// to minimize diffs.  Use for iterating through configs.
	__inline const CVCPtrArray *GetConfigArray() const
		{return  &m_ConfigArray;}

	// Get a pointer to our configuration map for quick lookup
	// N.B. Do not use for iteration (slow!).  Use GetConfigArray() instead.
	__inline const CVCMapPtrToPtr *GetConfigMap() const 
		{return &m_ConfigMap;}

	// Force a configuration to be the active one, irrespective of the owner
	// project's current configuration.
	__inline void ForceConfigActive(CConfigurationRecord * pConfig = (CConfigurationRecord *)NULL /* reset */)
	{
		m_pForcedConfig = pConfig;
	}

	// Force a configuration (based on the base config record in the top project)
	// to be the active one, irrespective of the owner project's current configuration.
	__inline void ForceBaseConfigActive(CConfigurationRecord * pConfig)
	{
		VSASSERT(pConfig != (CConfigurationRecord *)NULL, "Cannot force NULL config active!");
		m_ConfigMap.Lookup((void *)pConfig, (void *&)m_pForcedConfig);
	}

	// Get the active configiguration (create if necessary).
	CConfigurationRecord * GetActiveConfig(BOOL fCreate = FALSE);

	// Make sure we have all of our configs matching the project.
	void CProjItem::CreateAllConfigRecords();

	// Get a matching configuration for a name, or for a base config. record,
	// 'fCreate' if TRUE will create if does not already exist.
	CConfigurationRecord * ConfigRecordFromConfigName(const TCHAR * pszConfig, BOOL fCreate = FALSE, BOOL fMatchExact = TRUE);
	CConfigurationRecord * ConfigRecordFromBaseConfig(CConfigurationRecord * pcrBase, BOOL fCreate = FALSE);

	// Pointer to the active configuration (tracks owner project's config.)
	CConfigurationRecord * m_pActiveConfig;

	// Pointer to a forced configuration    
	CConfigurationRecord * m_pForcedConfig;

// FUTURE
//
// TO REMOVE AND REPLACE with GENERIC property bag handling

	// Reset the current default configuration or the current configuration 
	__inline void SetCurrentConfigAsBase()
		{
			VSASSERT(GetActiveConfig(), "No active config!  Something not initialized properly!");
			GetActiveConfig()->BagCopy(CurrBag, BaseBag);
		}

	// Copying the current config to the default for a particular tool only
	void SetCurrentConfigAsBase(COptionHandler * popthdlr);

// FUTURE


	__inline int UsePropertyBag(UINT idBag = (UINT)-1)
	{
		int idOldBag = m_idBagInUse;
		if (idBag != (UINT)-1)
			m_idBagInUse = idBag;
		return idOldBag;
	}

	virtual CProjType * GetProjType();

	//	Override of CSlob's get props.	Do this to allow imp. of CPropBag hierarchy
	//	N.B. The only prop types we require so far are integer, string.
	virtual	GPT GetIntProp(UINT idProp, int& val);
	virtual GPT GetStrProp(UINT idProp, CString& val);
	virtual GPT GetConvertedStrProp(UINT idProp, CString& val);

	virtual BOOL SetIntProp(UINT idProp, int val);
	virtual BOOL SetStrProp(UINT idProp, const CString& val);

	//	These are stubbed for now :-) [matthewt]
	virtual GPT GetDoubleProp(UINT idProp, double& val)	{VSASSERT(FALSE, "GetDoubleProp must be overridden"); return invalid;}
	virtual	GPT GetLongProp(UINT idProp, long& val)		{VSASSERT(FALSE, "GetLongProp must be overridden"); return invalid;}
	virtual GPT GetSlobProp(UINT idProp, CSlob*& val)	{VSASSERT(FALSE, "GetSlobProp must be overridden"); return invalid;}
	virtual GPT GetRectProp(UINT idProp, CRect& val)	{VSASSERT(FALSE, "GetRectProp must be overridden"); return invalid;}
	virtual GPT GetPointProp(UINT idProp, CPoint& val)	{VSASSERT(FALSE, "GetPointProp must be overridden"); return invalid;}

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
	virtual BOOL ConfigurationChanged(const CConfigurationRecord *pBaseRecord);

	// .MAK reading and writing
	virtual BOOL ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir);

	// Helper functions to parse a string, replacing $(OUTDIR) with actual value
	BOOL SubstOutDirMacro(CString & str, const CConfigurationRecord * pcr);

	// Helper functions to generate appropriate $(OUTDIR) macro string
	virtual BOOL GetOutDirString(CString & str, CString * pstrValue = NULL, BOOL bMacro = FALSE, BOOL fRecurse = FALSE);
	BOOL GetOutDirString(CString & str, UINT PropId, CString * pstrValue = NULL, BOOL bMacro = TRUE, BOOL fRecurse = FALSE);

	// Write out the !ifdef / !elseif / !endif block for a configuration block:
	enum ConfigPosition { FirstConfig, MiddleConfig, EndOfConfigs };
	// Read in a configuration ifdef and force ourself into that configuration:
	void ReadConfigurationIfDef(CMakDirective *pmd, BOOL bCheckProj = FALSE);
							// (throw CFileException if unknown configuration)

	// See if this is a comment holding a prop value.  If so, digest it _and_
	// delete it and set pObject= NULL 
	BOOL IsPropComment (CObject *&pObject);
						// (throw CFileException)

	// See if this is a comment hodling an ADD/SUBTRACT line for some tool.
	// If it is, digest it _and_ delete it and set pObject= NULL

	static const TCHAR *pcADD;
	static const TCHAR *pcSUBTRACT;
	static const TCHAR *pcBASE;
	
	// Return the file name macro:
	static const TCHAR *GetFileNameMacro() {return pcFileMacroNameEx;}

	// Return a container-less, symbol-less, clone of 'this'
	virtual CSlob* Clone();
	
	// called when removing an item from the project
	void IsolateFromProject();
	void SetOwnerProject(CProject * pProject, CTargetItem * pTarget);

    void CopyCommonProps(CProjItem * pItemSrc, const CString & strTarget);
	virtual int CompareSlob(CSlob *pCmpSlob);
	LPVOID m_pArchyHandle;

public:
	// Helpers for makefile reading writing.  These return the three letter item
	// type (e.g. "SRC" from a file item) or get the runtime class given
	// an item type:
	 
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
	
	virtual BOOL CanContain(CSlob* pSlob);

	void DeleteConfigFromRecord (CConfigurationRecord *prec, BOOL bChildren = TRUE);

	// Copy the configuration structure of another item:
	virtual void CloneConfigs (const CProjItem *pItem);

public:
	//	Used to keep track of config state for makefile reading/writing
	//  REVIEW(kperry): since we only ever deal with one projitem at a time, this could probably be made static.
	ConfigPosition m_cp;


protected:
	//	Configuration record map and array
	CVCMapPtrToPtr m_ConfigMap;
 	CVCPtrArray m_ConfigArray;

	BOOL m_fGettingListStr:1; // Hack to stop recursing for ::GetStrProp on a list string

	//	Property bag searching heuristics
	BOOL m_fPropsDepFigure:1;	//	Property search is figuring deps?
	OptBehaviour m_optbeh;	//	Property search behaviour
	// note OptBehaviour is a WORD ! put beside another word for packing.

	//	Pointer to config. we wish to match for manual bag search:
	CConfigurationRecord * m_pManualConfigRec;

	//	A stack of cached record ptrs. so that we
	//	can nest the calls to SetManualBagSearch...
	CObList m_stkOldConfigRec;
	
	// our current 'owner' project, maybe '(CProject *)NULL' if none
	CProject *	m_pOwnerProject;

	// our current 'owner' target, maybe '(CTargetItem *)NULL', if none
	CTargetItem * m_pOwnerTarget;

private:	
	UINT m_idBagInUse;	// property bag to use

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

class  CFileItem : public CProjItem 
{
	DECLARE_SERIAL (CFileItem)

public:
	CFileItem();
	virtual ~CFileItem()
	{
		if (NULL != m_pFilePath)
			delete m_pFilePath;

		DeleteContents();
	}

	virtual void GetMoniker (CString&);
	const CPath* GetFilePath() const;

	// Property bag handling methods for base, clone and current.
	virtual int GetBagHashSize(){ return( 3 ); }

	// Property storage and retrieval
	virtual BOOL SetStrProp(UINT idProp, const CString& val);
	virtual BOOL SetIntProp(UINT nIDProp, int val);
	virtual GPT GetStrProp(UINT nIDProp, CString & val);
	virtual	GPT GetIntProp(UINT idProp, int& val);

	// Sending out add/delete file notifications
	virtual BOOL MoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);
	virtual BOOL PostMoveItem(CSlob * pContainer, CSlob * pOldContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);
	
	// Setting and retrieving the filename
	virtual BOOL SetFile (const CPath *pPath, BOOL bCaseApproved = FALSE);

	// .MAK reading
	virtual BOOL ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir);

	// Flattening of projitems in a tree.
	virtual void FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem);

	// Return a container-less, symbol-less, clone of 'this'
	virtual CSlob* Clone();

public:
	const CPath*   m_pFilePath;	// our filename path

private:
	BOOL m_fDisplayName_Dirty:1;		// when to cache?
	CString	m_strMoniker;			// cached relative path of fileitem
	CString m_strDisplayName;		// cache our display name
	CString m_strOrgPath;			// name as we want to write it in the .dsp

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

public:
	CProjContainer ();
	virtual ~CProjContainer ();

private:	
	CObList		m_objects;	// Actual list of contained ProjItems.

public:
	// Create a new group with the standard name and add it.
	CProjGroup *AddNewGroup (const TCHAR * pszName = NULL, const TCHAR * pszFilter = NULL);

	//	CSlob content access method:
	virtual CObList *GetContentList() { return &m_objects; };
	void SortContentList();

public:
	virtual BOOL ReadInChildren(CMakFileReader& mr, const CDir &BaseDir);
private:
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

public:
	// Special Reading code for groups, as we should only see groups in old v2
	// makefiles and so this code is part of the v2 to v3 conversion code
	virtual BOOL ReadInChildren(CMakFileReader& mr, const CDir &BaseDir);

	CProjGroup ();
	CProjGroup (const TCHAR * pszName);
	virtual ~CProjGroup () { DeleteContents ();};

	virtual GPT GetStrProp ( UINT idProp, CString &str );
	virtual BOOL SetStrProp ( UINT idProp,  const CString &str );

	// .MAK reading and writing
	virtual BOOL ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir);

	virtual BOOL CanContain(CSlob* pSlob);

	virtual void	FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem)
		{ fAddContent = TRUE; fAddItem = ((fo & flt_ExcludeGroups)==0); }

public:
	CString	m_strGroupName;
	CVCPtrList m_DeathRow;	// list of Slobs to be delete during a Drop
	CString m_strDefaultExtensions;  // P_GroupDefaultFilter

	DECLARE_SLOBPROP_MAP()
};


///////////////////////////////////////////////////////////////////////////////

class CTimeCustomBuildItem : public CFileItem
{
//
//	Represents one those custom build step items. 
//
//
	DECLARE_SERIAL(CTimeCustomBuildItem)

protected:
	// Groups must have unique names within a project.  Functions will munge
	// this items name to make it unique if needed:
	BOOL MakeNameUnique(CString & strName);

	void Init(const TCHAR* pszName);	// c'tor helper

public:
	CTimeCustomBuildItem();
	CTimeCustomBuildItem(const TCHAR * pszName);

	virtual GPT GetStrProp(UINT idProp, CString &str);
	virtual BOOL SetStrProp(UINT idProp,  const CString &str);
	virtual BOOL CanContain(CSlob* pSlob) { return FALSE; }

	virtual void FlattenQuery(int fo, BOOL& bAddContent, BOOL& fAddItem);

private:
	CString	m_strName;
	CString m_strCustomBuildTime;

	DECLARE_SLOBPROP_MAP()
};
 
#endif  // __PROJITEM__
