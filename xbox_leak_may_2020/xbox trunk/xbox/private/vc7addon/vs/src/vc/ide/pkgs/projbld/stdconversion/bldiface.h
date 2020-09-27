//
// CBldSysIFace
//
// Build System Interface
//
// [matthewt]
//

#ifndef _INCLUDE_BLDSYSIFACE_H
#define _INCLUDE_BLDSYSIFACE_H

#pragma warning(disable:4251) // dll-interface warnings...
#pragma warning(disable:4275)

// our project interface (project.h)
class CProject;
class CProjItem;
class CProjComponentMgr;
class CBldSysIFace;
class CBldrSection;
class CConfigurationRecord;

// our file registry (pfilereg.h)
//#include "pfilereg.h"

// Translation Guide
// An HPROJECT is a CProject *
// An HFILESET is a CTargetItem *
// An HCONFIGURATION is a POSITION in a list of CStrings of configuration names
// An HFOLDER is a CProjGroup *
// An HBLDFILE is a CFileItem *


// properties
//

// item excluded in the build (currently only applicable to files)
#define Prop_ExcludeFromBuild		0x0
// 0 - included
// !0 - excluded

// use of MFC (integer prop)
#define Prop_UseOfMFC				0x1  
// 0 - not using MFC
// 1 - using MFC in a static lib
// 2 - using MFC in a DLL

// mark as an AppWiz target
#define Prop_AppWizTarg				0x2

// class wizard file name
#define Prop_ClsWzdName				0x3

// target directory name
#define Prop_TargetDirectory		0x4

// output directories
#define Prop_IntermediateOutDir		0x5
#define Prop_TargetOutDir			0x6

// default target extension
// if not set then
// '.exe' for apps
// '.dll' for dynamic link libraries
// '.lib' for static libraries
#define Prop_TargetDefExt			0x7

enum DeletionCause {WorkspaceClose, ProjectDelete, ProjectUnload};
typedef enum {TrgDefault, TrgCopy, TrgMirror} TrgCreateOp;
typedef enum {SettingsDefault, SettingsClone} SettingOp;

class CProject;
class CTargetItem;
class CFileItem;


class CBldSysIFace : CObject
{
public:
	CBldSysIFace();
	~CBldSysIFace();

	//
	// Project API
	//

	// Return the active build system if there is one, or NO_PROJECT if there isn't
	HPROJECT GetActiveProject();

	//
	// Using this API, a project can be created, targets added, files added
	// to those targets and tool settings & properties set for the files.
	//
	// Create a project with the name 'pchPrjPath'.
	// If fForce == TRUE then if any project is already open the user will
	// be prompted to close this project, else the function will fail (HPROJECT == NO_PROJECT).
	HPROJECT CreateProject
	(
		const TCHAR *	pchPrjPath		// project path
	);

	// Close a project.
	BOOL CloseProject
	(
		HPROJECT		hPrj, 	// project to close, default='close active project'
		DeletionCause	dcCause 	// project to close, default='close active project'
	);

	//
	// Config API
	//

	// Get the active target if there is on, or NO_CONFIG if there isn't.
	HCONFIGURATION GetActiveConfig
	(
		HPROJECT		hPrj = ACTIVE_PROJECT		// handle to the project containing the targets, default='active project'
	);

	// Set the active target.
	BOOL SetActiveConfig
	(
		HCONFIGURATION		hConfig,						// target to make the active one
		HPROJECT		hPrj = ACTIVE_PROJECT		// handle to the project containing the targets, default='active project'
	);

	// Get a target with name 'pchTarg' from the project, 'hPrj'.
	HCONFIGURATION GetConfig
	(
		const TCHAR *	pchTarg,					// name of this target to get
		HPROJECT		hPrj 	// handle to the project containing the target, default='active project'
	);

	BOOL GetConfigName
	(
		HCONFIGURATION		hConfig,						// target to get name of
		CString &		str,						// target name
		HPROJECT		hPrj 	// handle to the project containing the target, default='active project'
	);

	// Create a target with name 'pchTarg' and add it to a project.
	// Name of the 'official' or 'UI' platform must be specified in 'pchPlat', eg. "Win32 (x86)"
	// Name of the target type must be specified in 'pchType', eg. "Application"
	// Create debug or release default settings when a file is added to this target, 'fDebug'?
	// A target can mirror another target's, 'hMirrorTarg', fileset.
	// Function may fail (HCONFIGURATION == NO_CONFIG).
	HCONFIGURATION AddConfig
	(
		const TCHAR *	pchTarg,				// name of this target (if == NULL then names auto created)

		const TCHAR *	pchPlat,				// UI or 'official' name of this target's platform
		const TCHAR *	pchType,				// UI or 'official' name of this target type
		BOOL			fUIDesc = TRUE,			// using UI or 'official' names? default=UI

		BOOL			fDebug = TRUE,			// debug or release settings? default=debug

		TrgCreateOp		trgop = TrgDefault,		// clone or mirros another target? default=no
		HCONFIGURATION		hOtherTarg = NO_CONFIG,	// target to clone or mirror

		SettingOp		setop = SettingsDefault,	// copy or default settings?
		HCONFIGURATION		hSettingsTarg = NO_CONFIG,	// target to copy settings from
		 
		BOOL			fQuiet = FALSE,			// show any warnings/errors during creation? default=yes
		BOOL			fOutDir = TRUE,			// output directories? default=yes
		BOOL			fUseMFC = TRUE,			// use MFC? default=yes
		HPROJECT		hPrj = ACTIVE_PROJECT	// project that contains target? default='active project' 
	);

	// Create a pair of debug and release targets that mirror each other's file sets.
	BOOL AddDefaultConfigs
	(
		const TCHAR *	pchPlat,				// UI or 'official' name of this target's platform
		const TCHAR *	pchType,				// UI or 'official' name of this target type

		HCONFIGURATION &	hDebugTarg,				// newly created debug target
		HCONFIGURATION &	hReleaseTarg,			// newly created release target

		BOOL			fUIDesc = TRUE,			// using UI or 'official' names? default=UI
		BOOL			fOutDir = TRUE,			// output directories? default=yes
		BOOL			fUseMFC = TRUE,			// use MFC? default=yes

		HCONFIGURATION		hMirrorTarg = NO_CONFIG,// do we want to 'chain the mirroring'?
		HPROJECT		hPrj = ACTIVE_PROJECT,	// project that contains target? default='active project'
		const TCHAR *	pchTarg = NULL			// name of this target (if == NULL then names auto created)
	);
	
	// Delete an existing target
	BOOL DeleteConfig
	(
		HCONFIGURATION	hConfig,				// target to delete
		HPROJECT		hPrj 					// project that contains target? default='active project'
	);

	//
	// File API
	//

	// Create a file and add it to a fileset, 'hFileSet', with or without default settings, 'fSettings'.
	// Function may fail (HBLDFILE == NO_FILE).
	HBLDFILE AddFile
	(
		HFILESET		hFileSet,				// handle to the fileset
		const CPath *	ppathFile,				// file path
		int				fSettings = 0,			// use default settings(NYI)
		HPROJECT		hBld = ACTIVE_PROJECT,	// builder that contains target? default='active builder'
		HFOLDER			hFolder = NO_FOLDER,		// folder for the file
		BOOL			fDeploy = FALSE,
		BOOL			fNoFilter = FALSE
	);

	//
	// Enumeration and information API
	//
	// WARNING:
	// Currently these enumeration API can only enumerate single project items at a time.
	// o file set enumertion
	// o file enumeration
	//

	void 		InitProjectEnum();
	HPROJECT 	GetNextProject(CString & strProject, BOOL bOnlyLoaded = TRUE);
	
	void 		InitConfigEnum(HPROJECT hPrj = ACTIVE_PROJECT);
	HCONFIGURATION 	GetNextConfig(CString & strConfig, HPROJECT hPrj = ACTIVE_PROJECT); // Returns the name of the target as strConfig.

	void 		InitFileEnum(HPROJECT hProj);
	HBLDFILE 	GetNextFile();

	HFILESET GetFileSet(HPROJECT hPrj, HCONFIGURATION hConfig);

	//
	// Project file section support
	//
	// Provide a callback function that will be called when 
	// a buffer needs to be provided to be written or a 
	// buffer has been read and can be processed.
	//
	// Pls. note that sections are distinguished by name.
	BOOL RegisterSection(CBldrSection * pbldsect);
	BOOL DeregisterSection(const CBldrSection * pbldsect);

	// Enumeration
	void InitSectionEnum();
	CBldrSection * GetNextSection(CString & strSection);

	// Lookup
	CBldrSection * FindSection(const TCHAR * pchName, HPROJECT hPrj = ACTIVE_PROJECT);

	//
	// Project object conversion
	//

	CProject * CnvHProject(HPROJECT hPrj);
	CTargetItem * CnvHConfig(HPROJECT hPrj, HCONFIGURATION hConfig);
	CTargetItem * CnvHFileSet(HPROJECT hPrj, HFILESET hFileSet);

	BOOL GetFlavourFromConfigName
	(
		const TCHAR *	pchConfigname,					// configuration name
		CString &		str							// flavour name (e.g, Debug, Release,..)
	);

	BOOL 	GetProjectToolset( HPROJECT hPrj = ACTIVE_PROJECT );

	// Return the project if there is one, or NO_PROJECT if there isn't
	HPROJECT GetProject(HCONFIGURATION hConfig);
	HPROJECT DeactivateProject(HPROJECT hPrj);

public:
	// Get the config. record for a target, 'hConfig'
	CConfigurationRecord * GetConfigRecFromConfigI
	(
		HCONFIGURATION		hConfig,						// handle to the target
		HPROJECT		hPrj = ACTIVE_PROJECT		// project that contains target? default=('current'==ACTIVE_PROJECT) 
	);
	
private:
	CMapStringToPtr m_mapSections;					// our map of section names to section info.
	POSITION m_posSection;							// section enumeration 

	CStringList m_strlstConfigs;					// used to maintain targets names added so far

	POSITION m_posHFile;							// file enumeration
	CObList m_lstFile;								// FUTURE: make per-fileset

	CVCMapPtrToPtr m_mapPrjConfigs;
};									  

// our 'generic' build system components
// pls. note that the package for the 'generic' components is zero
#define idAddOnGeneric			0
#define szAddOnGeneric			(TCHAR *)NULL

#define BCID_Tool_RcCompiler	1
#define BCID_Tool_RcCompilerNT	2
#define BCID_Tool_Compiler		3
#define BCID_Tool_Linker		4
#define BCID_Tool_LinkerNT		5
#define BCID_Tool_BscMake		6
#define BCID_Tool_MkTypLib		7
#define BCID_Tool_Lib			8
#define BCID_Tool_CustomBuild	9
#define BCID_Tool_SpecialBuild	10

#define BCID_OptHdlr_Compiler		20
#define BCID_OptHdlr_Linker			21
#define BCID_OptHdlr_LinkerNT		22
#define BCID_OptHdlr_RcCompiler		23
#define BCID_OptHdlr_RcCompilerNT	24 
#define BCID_OptHdlr_BscMake		25
#define BCID_OptHdlr_Lib			26
#define BCID_OptHdlr_MkTypLib		27

#define BCID_ProjType_Generic			101

		
extern CBldSysIFace g_BldSysIFace;	// one 'n' only bld system interface

#endif // _INCLUDE_BLDSYSIFACE_H
