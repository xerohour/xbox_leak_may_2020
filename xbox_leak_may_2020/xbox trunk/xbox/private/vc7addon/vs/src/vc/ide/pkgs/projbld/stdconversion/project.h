///////////////////////////////////////////////////////////////////////////////
//
// PROJECT.H
//                                                                             
///////////////////////////////////////////////////////////////////////////////
#ifndef __PROJECT_H__
#define __PROJECT_H__


#ifndef __PROJITEM__
#include "projitem.h"
#endif

extern  BOOL	g_bInProjClose;

// All CProject 'exported' functions to other packages should be
// virtual so that cross-DLL function calling indirects through
// the object VTABLE ptr
#define EXPORTED virtual

class CBuildPackage;
class CTempMaker;
class CPlatform;

///////////////////////////////////////////////////////////////////////////////

class CToolsMap
{
public:
	CToolsMap() {
	m_pBuildTool = NULL;
	m_strExtensions = "";
	m_bExternalTool = FALSE;
	}
	~CToolsMap() {}

	CString GetExtensions() {return m_strExtensions;}
	BOOL IsExternalTool(){return m_bExternalTool;}

	void * m_pBuildTool;
	CString m_strExtensions;
	BOOL m_bExternalTool;
};


///////////////////////////////////////////////////////////////////////////////
class  CProject : public CProjContainer
{
	friend class CBuildPackage;
	friend class CProjectView;
	friend class CBldSysIFace;
	friend class CProjTempConfigChange;
	friend class CProjDeferDirty;
	friend class CProjOptionsDlg;
	friend class CProjItem;

//
//	Top level project class.  May be more than one if there are subprojects.
//
	DECLARE_SERIAL (CProject)

public:
	// constructor
	CProject ();

	// destroy contents
	void Destroy();

	// destructor
	virtual ~CProject();

// CSlob methods:
	// Special CloneConfigs method.  As a subproject, the project tries
	// to switch to a compatible configuration:
	virtual void CloneConfigs ( const CProjItem *pItem ) {}

	// Hook GetIntProp & GetStrProp to provide some defaults.
	virtual GPT GetIntProp(UINT idProp, int& val);
	virtual GPT GetStrProp(UINT idProp, CString& val);

	// We want to hook into the SetStrProp so we can catch configuration changes.
	virtual BOOL SetStrProp(UINT idProp, const CString& val);

	// This is our internal CanAdd method, and tells us what we can
	// add to this slob container
	// In this case we can add CProjGroup's but nothing else.
	virtual BOOL CanContain(CSlob* pSlob);	

	// File I/O and creation:
	// Fills out a new project:
	BOOL InitNew (const CVCPtrList * pProjTypeList); // Uses a temporary file.
	BOOL InitFromFile (const char *pFileName, BOOL bLoadOPTFile = TRUE);

	virtual BOOL SetFile (const CPath *pPath, BOOL bCaseApproved = FALSE);

	// Describes how we flatten a project node
	virtual void FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem);

public:
	CDir & GetWorkspaceDir() { return m_ProjDir; }
	CDir & GetProjDir(CConfigurationRecord * pcr = NULL);
	const CPath* GetFilePath() const;

// Other API:
	// Get the project's toolset
	EXPORTED INT GetProjectToolset();

	// Returns or'ed together ProjAttribtues (get this from the 
	// CProjType)
	EXPORTED int GetTargetAttributes ();

	// Read makefile.  Assume file name has already been set:
	enum ReadMakeResults
	{
		ReadExternal,
		ReadInternal,
		ReadError,
		ReadExportedMakefile,
	};

	// .MAK reading
	ReadMakeResults ReadMakeFile (BOOL& bProjectDirty);
	virtual BOOL ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir);
	
	// Read comments listing what configurations there are and
	// what project types they reference.  Also read our the current active
	// configuration back in:
	BOOL ReadConfigurationHeader(CMakFileReader& mr);

public:		
	// Set the value of m_bProjWasReadOnly by checking the .MAK file
	// on disk to see if it's read-only or not.
	void		RecheckReadOnly() { m_bProjWasReadOnly = GetFilePath()->IsReadOnlyOnDisk(); }

	void GetName(CString &strName ){ 
		CConfigurationRecord * pcr = GetActiveConfig();
		if(pcr)
			pcr->GetProjectName(strName);
	};

	// delete configuration named strName, and changed the active configuration
	// to strNewConfig.
	// strNewConfig configuration must already exist in the configuration map.
	BOOL	DeleteConfig ( const CString& strName , const CString& strNewConfig = "");

	// Create a new congugration.  Note that creation is a three step process:
	// 1. Create new config.  2. Initizlize new configs type and so on.
	// 2. Make new config active (even if you'll deactivate immediadly)
	BOOL 	CreateConfig ( const CString& strName );

	// Return the platform object corresponding to the current target.
	EXPORTED CPlatform * GetCurrentPlatform();


	// The Configuration manager api
	//
	// Set the currently active target configuration
	// If 'fQuite' is TRUE then no UI changes will be apparent.
	CConfigurationRecord * SetActiveConfig(CConfigurationRecord *, BOOL fQuite = TRUE);
	CConfigurationRecord * SetActiveConfig(const CString& strConfigName, BOOL fQuite = TRUE);

	// The Target manager api
	//
	// Get the target item associated with a given target name
 	CTargetItem* GetTarget(const TCHAR * pchTargetName);

	// Get the target item corresponding to the currently selected config
	CTargetItem* GetActiveTarget();

	// Register a target with the target manager. This is used during
	// project loading only
	BOOL RegisterTarget(const CString& strTargetName, CTargetItem* pTarget);

	// De-register a target with the target manager.
	BOOL DeregisterTarget(const CString& strTargetName, CTargetItem* pTarget);

	// Creates a new target (actually creates a new config and a new
	// CTargetItem)
	BOOL CreateTarget(const CString& strTargetName, CProjType* pProjType, const CString& strMirrorFrom = _T(""));

	// Deletes an existing target
	BOOL DeleteTarget(const CString& strTargetName);

	CConfigurationRecord * FindTargetConfig(CString & strTargetName);

	// Target enumeration
	void InitTargetEnum() { m_posTarget = m_TargetMap.GetStartPosition(); }
	BOOL NextTargetEnum(CString& strTargetName, CTargetItem* & pTarget)
	{
		if (m_posTarget == (POSITION)NULL)
			return FALSE;
		m_TargetMap.GetNextAssoc(m_posTarget, strTargetName, (void* &)pTarget);

		// 2173: need to get name with correct case preserved
		FindTargetConfig(strTargetName);

		return TRUE;
	}

	BOOL NextTargetEnum(CTargetItem* & pTarget)
	{
		if (m_posTarget == (POSITION)NULL)
			return FALSE;

		CString strDummy;
		m_TargetMap.GetNextAssoc(m_posTarget, strDummy, (void* &)pTarget);
		return TRUE;
	}
	int GetTargetCount() { return (int)m_TargetMap.GetCount(); }

protected:
	CConfigurationRecord *CreateConfigRecordForConfig( const CString & strConfig);
	DECLARE_SLOBPROP_MAP()

public:

    // Project dirty flag access functions
	__inline void DirtyProject()
	{
		if ((!m_bProjectDirty) && (m_bOkToDirty) && (!g_bInProjClose))
			DirtyProjectEx();
		m_bDirtyDeferred = !m_bProjectDirty;
	}
	__inline void CleanProject() { m_bProjectDirty = m_bDirtyDeferred = FALSE; }
	__inline BOOL IsOkToDirty() { return (m_bOkToDirty && !g_bInProjClose); }
	__inline BOOL IsDirtyDeferred() { return m_bDirtyDeferred; }
	__inline void SetOkToDirtyProject(BOOL bOk = TRUE) { m_bOkToDirty = bOk; }

	static void InitProjectEnum();
	static const CProject * NextProjectEnum(CString & strBuilder, BOOL bOnlyLoaded = TRUE);
	static void GlobalCleanup();
	__inline BOOL IsLoaded() const { return m_bProjectComplete; /* REVIEW */ }

	const CString & GetTargetName();
	__inline void SetTargetName(const CString & str) { m_strTargetName = str; }

	BOOL GetMatchingConfigName(CProject * pMatchProject, CString &strMatched, BOOL bInvalid);

	void SetBuildToolsMap();

protected:
	BOOL ConvertDirs();

private:
	BOOL IsCustomBuildMacroComment(CObject* pObject);
	BOOL ReadCustomBuildMacros(CMakFileReader& mr);
	virtual void DirtyProjectEx();


    // The project dirty flag. Corresponds to old project doc dirty flag
protected:
    BOOL    m_bProjectDirty:1;    // Is this project object dirty??

public:
	CDir			m_ActiveTargDir;	// Used ???

	CString		m_strProjItemName;			// used  ???
	CString		m_strProjSccProjName;
	CString		m_strProjSccRelLocalPath;

	// our configuration information
	CString		m_strProjActiveConfiguration;	// used  ???
	CString		m_strProjDefaultConfiguration; // CFG= line from makefile
	CString		m_strProjStartupConfiguration; // Primary/Supported cfg from makefile

	// TRUE if the .MAK file was read-only last time we checked.  Of
	// course, its state may have changed; call RecheckReadOnly()
	// to update this variable with a check of the file on disk.
	BOOL		m_bProjWasReadOnly:1;

	// Makefile errored during open due to requirement to use MFC and MFC is not installed
	BOOL		m_bGotUseMFCError:1;
	
	// .MAK reading (conversion of old VC++ 2.0 projects)
	BOOL m_bConvertedVC20:1;
	// .MAK reading (conversion of old DS 4.x projects)
	BOOL m_bConvertedDS4x:1;
	// .DSP reading (conversion of old DS 5.x projects)
	BOOL m_bConvertedDS5x:1;
	// .DSP reading (conversion of old DS 6.0 projects)
	BOOL m_bConvertedDS6:1;
	// .DSP reading (conversion of old DS 6.1 projects)
	BOOL m_bConvertedDS61:1;

	// Makefile has been freshly converted from an external makefile
	BOOL m_bProjExtConverted:1;

	// Makefile has been freshly converted from an 'old' makefile version
	BOOL m_bProjConverted:1;

	// Special properties that are only set when read in from the makefile.
	BOOL m_bProj_PerConfigDeps:1;

	BOOL m_bProjectComplete:1;
	BOOL m_bPrivateDataInitialized:1;

	BOOL m_bHaveODLFiles:1;

private:	
	BOOL    m_bDirtyDeferred:1;   // Mark dirty, but do it "later"
    BOOL    m_bOkToDirty:1;       // Ok to dirty project

	CDir				m_ProjDir;
	CPath*				m_pFilePath;		// path for .mak file.
	CString				m_strTargetName;
	int					m_nDeferred;		// ref counting

	CRITICAL_SECTION	m_critSection;	// Used for CProjTempConfigChange. 

	// This map contains the names of targets and the corresponding
	// CTargetItem ptrs.
	CMapStringToPtr		m_TargetMap;	// REVIEW this is nuts. 
	POSITION			m_posTarget;
	CString				m_strRelPath;	// name as we want to write it in the .dsw

public:
	static POSITION		m_posProjects;
	static CObList		m_lstProjects;
	int m_nProjType;
};	  


class  CProjDeferDirty 
{
public:
	CProjDeferDirty(CProject * pProject)
		{
			m_pProject = pProject;
			if (m_pProject!=NULL)
			{
				
				m_pProject->m_nDeferred++;
				m_bWasOk = m_pProject->IsOkToDirty();
				m_pProject->SetOkToDirtyProject(FALSE);
			}

		}
	~CProjDeferDirty()
		{
			if (m_pProject!=NULL)
			{
				m_pProject->SetOkToDirtyProject(m_bWasOk);
				if ((--(m_pProject->m_nDeferred)==0) && (m_bWasOk) && (m_pProject->IsDirtyDeferred()))
					m_pProject->DirtyProject();
			}
		}
private:
	CProject * m_pProject;
	BOOL m_bWasOk;
};

// This object is to used whenever the project's configuration needs to be changed
// temporarily. Do not call SetActiveConfig directly, unless you want to permanently 
// change the active configuration. Declare a local instance of this object and call
// 'ChangeConfig' on it. The destructor of this object will restore the original config
// back. The primary purpose of this is to make the calls to SetActiveConfig thread safe.

class  CProjTempConfigChange 
{
public:
	CProjTempConfigChange(CProject *pProject);
	// Will Release critical section and reset to original config.
	~CProjTempConfigChange();

	VOID ChangeConfig(CConfigurationRecord *);
	VOID ChangeConfig(const CString& strName);

	// Resets config to the original one, still holds the crit section.
	VOID Reset();	

	// releases critical section, if bReset is TRUE also resets to original config.
	VOID Release(BOOL bReset = TRUE); 

	CProject * m_pProject;

private:
	CConfigurationRecord * m_pcrOriginal;
	BOOL m_bCritSection ; // Do we have the critical section currently.
};


// This object is used when the active project needs to be temporarly changed
// usually during the build of a sub project.

class  CProjTempProjectChange 
{
public:
	CProjTempProjectChange (CProject *pProject);
	~CProjTempProjectChange ();

	VOID Release (); 

	CProject *m_pPrevProject;
};

extern CProject *g_pActiveProject;
extern CProject *g_pConvertedProject;

#endif // __PROJECT_H__

