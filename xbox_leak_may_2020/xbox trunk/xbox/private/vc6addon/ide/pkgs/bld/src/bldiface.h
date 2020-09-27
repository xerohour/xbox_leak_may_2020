//
// CBldSysIFace
//
// Build System Interface
//
// [matthewt]
//

#ifndef _INCLUDE_BLDSYSIFACE_H
#define _INCLUDE_BLDSYSIFACE_H

#if defined(_BLD_INTERNAL)
#define BLD_IFACE _declspec(dllexport)
#elif defined(_BLD_ADD_ON)
#define BLD_IFACE _declspec(dllimport)
#else	// we *don't* export to an other packages (ie. the shell, res etc.)
#define BLD_IFACE
#endif	// !BLD_INTERNAL

#pragma warning(disable:4251) // dll-interface warnings...
#pragma warning(disable:4275)

#include "oletool.h"

// our builder interface (project.h)
class CProject;
class CProjItem;

// our builder component manager (prjconfg.h)
class CProjComponentMgr;

// our file registry (pfilereg.h)
#include "pfilereg.h"

class CBldSysIFace;

// FUTURE: move into the shell and export
#ifndef EXPORTED
#define EXPORTED virtual
#endif

#include <bldapi.h>

#include <utilbld_.h>

#include "prjconfg.h"

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

class CProject;
class CTargetItem;
class CFileItem;
class CProjectDependency;


class CBldSysIFace : CObject
{
public:
	CBldSysIFace();

	// Return the build system's builder component manager.
	// The component manager is used to register tools, platforms etc.
	// through. These components are usually stored in add-on platform 'packages'.
	EXPORTED CProjComponentMgr * GetProjComponentMgr();

	// Return the file system's global file registry.
	EXPORTED CFileRegistry * GetFileRegistry();

	//
	// Builder API
	//

	// Return the active build system if there is one, or NO_BUILDER if there isn't
	EXPORTED HBUILDER GetActiveBuilder();

	// Get the type of a builder. The possible builder types are :-
	// ExeBuilder, this is a builder that can only be used to debug files
	// Internal Builder, a full internal builder, supports building and debugging
	EXPORTED BuilderType GetBuilderType
	(
		HBUILDER		hBld = ACTIVE_BUILDER 	// builder to get type of
	);

	// Get the filename that this builder is stored in
	EXPORTED const CPath * GetBuilderFile
	(
		HBUILDER		hBld 	// builder to get file for
	);

	//
	// Using this API, a builder can be created, targets added, files added
	// to those targets and tool settings & properties set for the files.
	//
	// Create a builder with the name 'pchBldPath'.
	// If fForce == TRUE then if any builder is already open the user will
	// be prompted to close this builder, else the function will fail (HBUILDER == NO_BUILDER).
	EXPORTED HBUILDER CreateBuilder
	(
		const TCHAR *	pchBldPath,		// builder path
		BOOL			fForce = FALSE,	// force the creation
		BOOL			fOpen = FALSE	// create and then read from storage (eg. makefile), default=create new
	);

	// Set the dirty state of the builder
	EXPORTED BOOL SetDirtyState
	(
		HBUILDER	hBld,		// builder to dirty/clean, default='active builder'
		BOOL		fDirty = TRUE				// default='dirty builder'
	);

	// Get the dirty state of the builder
	EXPORTED BOOL GetDirtyState
	(
		HBUILDER	hBld		// builder to dirty/clean, default='active builder'
	);

	// Save a builder.
	EXPORTED BOOL SaveBuilder
	(
		HBUILDER		hBld	// builder to save, default='save active builder'
	);

	// Close a builder.
	EXPORTED BOOL CloseBuilder
	(
		HBUILDER		hBld, 	// builder to close, default='close active builder'
		DeletionCause	dcCause 	// builder to close, default='close active builder'
	);

	//
	// Workspace API
	// NOTE: We don't actually load/create/save the workspace, that is done by the prj package
	// However we do provide the UI for some of these things, and this is done here
	//

	// A workspace is being opened. We must open the makefile, and do various other things too.
	EXPORTED BOOL OnOpenWorkspace
	(
		const TCHAR *	pchWorkspaceName,	// Workspace name
		BOOL            bTemporary,         // Temporary workspace??
		BOOL            bInsertingIntoWks   // inserting into workspace? (don't AddProject main project)
	);

	// A default workspace is being created. We must create the makefile and add in the file
	EXPORTED BOOL OnDefaultWorkspace
	(
		const TCHAR *	pszFileToAdd		// File to base workspace around
	);

    EXPORTED BOOL OpenWorkspace
    (
        const TCHAR * pchWorkspace    // Name of workspace to open, can be
                                      // a .mak, or .exe/.dll/etc.., or .mdp
    );

	//
	// Fileset API
	//

	// Get a fileset with name 'pchFileSet' from the builder, 'hBld'.
	EXPORTED HFILESET GetFileSetFromTargetName
	(
		const TCHAR *	pchFileSet,					// name of this fileset to get
		HBUILDER		hBld 	// handle to the builder containing the fileset, default='active builder'
	);

	// Get the name of the target corresponding to a fileset, uses the default project to get
	// the information about the platform/debugness of the target
	// e.g, if the fileset's name is "foo", and the default project is "bar - win32 debug"
	// then this function returns "foo - win32 debug"
	EXPORTED BOOL GetTargetNameFromFileSet
	(
		HFILESET		hFileset,					// Fileset to get target name for
		CString &		str,						// Target name
		HBUILDER		hBld = ACTIVE_BUILDER, 	// handle to the builder containing the fileset, default='active builder'
		BOOL			bInvalid = FALSE			// If TRUE then we put 'No matching Configuration' after non matching configs
	);

	// Get the name of a fileset, e,g, "foo"
	EXPORTED BOOL GetFileSetName
	(
		HFILESET		hFileset,					// Fileset to get name of
		CString &		str,						// Fileset name
		HBUILDER		hBld 	// handle to the builder containing the fileset, default='active builder'
	);

	// Get the target corresponding to a fileset, given the context of the default project.
	EXPORTED BOOL GetTargetFromFileSet
	(
		HFILESET		hFileSet,					// Fileset to get current target for
		HBLDTARGET &	hTarg,						// Target for the Fileset
		HBUILDER		hBld = ACTIVE_BUILDER, 	// handle to the builder containing the fileset to match, default='active builder'
		BOOL			bInvalid = FALSE			// Do we care about non matching configs?
	);

	//
	// Target API
	//

	// Get the active target if there is on, or NO_TARGET if there isn't.
	EXPORTED HBLDTARGET GetActiveTarget
	(
		HBUILDER		hBld = ACTIVE_BUILDER		// handle to the builder containing the targets, default='active builder'
	);

	// Set the active target.
	EXPORTED BOOL SetActiveTarget
	(
		HBLDTARGET		hTarg,						// target to make the active one
		HBUILDER		hBld = ACTIVE_BUILDER		// handle to the builder containing the targets, default='active builder'
	);

    EXPORTED BOOL SetDefaultTarget
    (
		HBLDTARGET		hTarg,						// target to make the default one
		HBUILDER		hBld = ACTIVE_BUILDER		// handle to the builder containing the targets, default='active builder'
    );

	// Get a target with name 'pchTarg' from the builder, 'hBld'.
	EXPORTED HBLDTARGET GetTarget
	(
		const TCHAR *	pchTarg,					// name of this target to get
		HBUILDER		hBld 	// handle to the builder containing the target, default='active builder'
	);

	EXPORTED BOOL GetTargetName
	(
		HBLDTARGET		hTarg,						// target to get name of
		CString &		str,						// target name
		HBUILDER		hBld 	// handle to the builder containing the target, default='active builder'
	);


	// Create a target with name 'pchTarg' and add it to a builder.
	// Name of the 'official' or 'UI' platform must be specified in 'pchPlat', eg. "Win32 (x86)"
	// Name of the target type must be specified in 'pchType', eg. "Application"
	// Create debug or release default settings when a file is added to this target, 'fDebug'?
	// A target can mirror another target's, 'hMirrorTarg', fileset.
	// Function may fail (HBLDTARGET == NO_TARGET).
	EXPORTED HBLDTARGET AddTarget
	(
		const TCHAR *	pchTarg,				// name of this target (if == NULL then names auto created)

		const TCHAR *	pchPlat,				// UI or 'official' name of this target's platform
		const TCHAR *	pchType,				// UI or 'official' name of this target type
		BOOL			fUIDesc = TRUE,			// using UI or 'official' names? default=UI

		BOOL			fDebug = TRUE,			// debug or release settings? default=debug

		TrgCreateOp		trgop = TrgDefault,		// clone or mirros another target? default=no
		HBLDTARGET		hOtherTarg = NO_TARGET,	// target to clone or mirror

		SettingOp		setop = SettingsDefault,	// copy or default settings?
		HBLDTARGET		hSettingsTarg = NO_TARGET,	// target to copy settings from
		 
		BOOL			fQuiet = FALSE,			// show any warnings/errors during creation? default=yes
		BOOL			fOutDir = TRUE,			// output directories? default=yes
		BOOL			fUseMFC = TRUE,			// use MFC? default=yes
		HBUILDER		hBld = ACTIVE_BUILDER	// builder that contains target? default='active builder' 
	);

	// Create a pair of debug and release targets that mirror each other's file sets.
	EXPORTED BOOL AddDefaultTargets
	(
		const TCHAR *	pchPlat,				// UI or 'official' name of this target's platform
		const TCHAR *	pchType,				// UI or 'official' name of this target type

		HBLDTARGET &	hDebugTarg,				// newly created debug target
		HBLDTARGET &	hReleaseTarg,			// newly created release target

		BOOL			fUIDesc = TRUE,			// using UI or 'official' names? default=UI
		BOOL			fOutDir = TRUE,			// output directories? default=yes
		BOOL			fUseMFC = TRUE,			// use MFC? default=yes

		HBLDTARGET		hMirrorTarg = NO_TARGET,// do we want to 'chain the mirroring'?
		HBUILDER		hBld = ACTIVE_BUILDER,	// builder that contains target? default='active builder'
		const TCHAR *	pchTarg = NULL			// name of this target (if == NULL then names auto created)
	);
	
	// Delete an existing target
	EXPORTED BOOL DeleteTarget
	(
		HBLDTARGET		hTarg,					// target to delete
		HBUILDER		hBld 	// builder that contains target? default='active builder'
	);

	// Add a project as a dependency of another project.
	EXPORTED HPROJDEP AddProjectDependency
	(
		HFILESET		hFileSet,				// fileset in which to *place* dependency
		HFILESET		hDependantFileSet,		// fileset on which to place a dependency
		HBUILDER		hBld, 					// builder that contains hFileSet'
		HBUILDER		hDependantBld = NO_BUILDER 	// builder that contains hDependantFileSet (default is to use hBld)
	);

	EXPORTED BOOL RemoveProjectDependency
	(
		HPROJDEP		hProjDep,				// handle to the project dependency
		HFILESET		hFileSet,				// handle to the fileset
		HBUILDER		hBld 	// builder that contains target? default='active builder'
	);

	EXPORTED HFILESET GetDependantProject
	(
		HPROJDEP		hProjDep,				// handle to the project dependency
		HFILESET		hFileSet,				// handle to the fileset
		HBUILDER		hBld 	// builder that contains target? default='active builder'
	);

	// Add or subtract tool settings, pchSettings, to a target, 'hTarg'.
	// Must specify the 'executable' name of the tool'.
	// Subtract, fAdd==FALSE, will only work for individual options, eg. '/WX' and not '/WX /D "_DEBUG"'
	EXPORTED BOOL SetToolSettings
	(
		HBLDTARGET		hTarg,					// handle to the target
		const TCHAR * 	pchSettings,			// tool settings
		const TCHAR *	pchTool,				// name of the tool that 'owns' these settings
		BOOL			fAdd = TRUE,			// add or subtact? default=add
		BOOL			fClear = FALSE,			// clear settings before add or subtract?
		HBUILDER		hBld = ACTIVE_BUILDER	// builder that contains target? default='active builder'
	);

	//
	// Property API
	//

	// Get a string property for this target
	EXPORTED BOOL GetTargetProp
	(
		HBLDTARGET		hTarg,					// handle to the target
		UINT			idProp,					// identifier of the property
		CString &		str,					// property value
		HBUILDER		hBld = ACTIVE_BUILDER	// builder that contains target? default='active builder'
	);

	// Get an integer property for this target
	EXPORTED BOOL GetTargetProp
	(
		HBLDTARGET		hTarg,					// handle to the target
		UINT			idProp,					// identifier of the property
		int &			i,						// property value
		HBUILDER		hBld = ACTIVE_BUILDER	// builder that contains target? default='active builder'
	);

	// Set an integer or string property for this target
	EXPORTED BOOL SetTargetProp
	(
		HBLDTARGET		hTarg,					// handle to the target
		UINT			idProp,					// identifier of the property
		int				i,						// property value
		HBUILDER		hBld = ACTIVE_BUILDER	// builder that contains target? default='active builder'
	);

	EXPORTED BOOL SetTargetProp
	(
		HBLDTARGET		hTarg,					// handle to the target
		UINT			idProp,					// identifier of the property
		CString &		str,					// property value
		HBUILDER		hBld = ACTIVE_BUILDER	// builder that contains target? default='active builder'
	);

	EXPORTED void MakeTargPropsDefault
	(
		HBLDTARGET		hTarg,					// handle to the target
		HBUILDER		hBld = ACTIVE_BUILDER	// builder that contains target? default='active builder'
	);

	EXPORTED HFOLDER AddFolder
	(
		HFILESET hFileSet,
		const TCHAR * pchFolder,
		HBUILDER hBld = ACTIVE_BUILDER,
		HFOLDER hFolder = NO_FOLDER,
		const TCHAR * pchFilter = NULL
	);

	//
	// File API
	//

	// Create a file and add it to a fileset, 'hFileSet', with or without default settings, 'fSettings'.
	// Function may fail (HBLDFILE == NO_FILE).
	EXPORTED HBLDFILE AddFile
	(
		HFILESET		hFileSet,				// handle to the fileset
		const CPath *	ppathFile,				// file path
		int			fSettings = 0,// use default settings(NYI)
		HBUILDER		hBld = ACTIVE_BUILDER,	// builder that contains target? default='active builder'
		HFOLDER			hFolder = NO_FOLDER		// folder for the file
	);

	// Create and add multiple files to a fileset, 'hFileSet', as above.
	// PLEASE use this in preference to adding them singly as it is more efficient to 
	// batch them up.
	EXPORTED void AddMultiFiles
	(
		HFILESET			hFileSet,				// handle to the fileset
		const CPtrList *	plstpathFiles,			// pointer list of file paths (const CPath *'s)
		CPtrList *			plstHBldFiles,			// returned list of HBLDFILEs (some may be NO_FILE if error)
		int				fSettings = 0,// use default settings for this target type? (NYI)
		HBUILDER			hBld = ACTIVE_BUILDER,	// builder that contains target? default='active builder'
		HFOLDER				hFolder = NO_FOLDER		// group to add files to
	);

	// Add a dependency file to a fileset, 'hFileSet'.
	// ** Note, this doesn't actually add the file as such, it just informs
	// other parties, say SCC, that we have a new file that is a 'part of' the fileset.
	// Use ::AddFile if you want to *actually* be a part of the fileset.
	// Function may fail (return value == FALSE).
	EXPORTED BOOL AddDependencyFile
	(
		HFILESET		hFileSet,				// handle to the fileset
		const CPath *	ppathFile,				// dependency file path
		HBUILDER		hBld = ACTIVE_BUILDER,	// builder that contains target? default='active builder'
		HFOLDER			hFolder = NO_FOLDER		// folder for dependency
	);

	// Checks a filename to see if it is ok to add to a fileset. Returns TRUE if the file is not
	// in the fileset, FALSE otherwise. If the given file is in the fileset then the function "warps"
	// the given filename such that the new filename is not in the fileset, and will be ok to add to
	// it. The "warped" filename is returned in pathFile.
	EXPORTED BOOL CheckFile
	(
		HFILESET		hFileSet,				// handle to the fileset
		CPath &			pathFile,				// file path
		HBUILDER		hBld = ACTIVE_BUILDER	// builder that contains target? default='active builder'
	);

	// Delete a file 'hFile' from a fileset, 'hFileSet'
	EXPORTED BOOL DelFile
	(
		HBLDFILE 		hFile,					// handle to file
		HFILESET		hFileSet,				// handle to the fileset
		HBUILDER		hBld 	// builder that contains target? default='active builder'
	);

	// Gets a file from a given target. If a file corresponding to the file path is found in the fileset,
	// 'hFileset', then 'hFile' is set to be the file. Returns TRUE only if the file was found, otherwise FALSE.
	// If the hFile pointer is NULL, then we don't lookup the corresponding File Item to the ppathfile, but
	// just check if the file is in the target.
 	EXPORTED BOOL GetFile
	(
		const CPath *	ppathFile,				// file path
		HBLDFILE &		hFile,					// pointer to handle to file
		HFILESET		hFileSet,				// handle to the fileset
		HBUILDER		hBld,	// build that contains target? default='active builder'
    	BOOL			bSimple = FALSE         // if TRUE check includes deps and intermediate target files, default = FALSE,
	);

	// Add or subtract tool settings, pchSettings, to a file, 'hFile'.
	// Optional to specify the 'executable' name of the tool'.
	EXPORTED BOOL SetToolSettings
	(
		HBLDTARGET		hTarg,						// handle to the target that contains the file
		HBLDFILE		hFile,						// handle to the file
		const TCHAR * 	pchSettings,				// tool settings
		const TCHAR *	pchTool = (const TCHAR *)NULL,	// executable of the tool that 'owns' these settings, default='primary tool'NULL)
		BOOL			fAdd = TRUE,				// add or subtact? default=add
		BOOL			fClear = FALSE				// clear settings before add or subtract?
	);

	//
	// Property API
	//

	// Set the 'file included in build' property for a file, 'hFile', in a target, 'hTarg'.  
	__inline BOOL SetBuildExclude
	(
		HBLDTARGET		hTarg,						// handle to the target that contains the file
		HBLDFILE		hFile,						// handle to the file
		BOOL			fExclude = TRUE				// include this item in the build for the target? default=no
	)
		{return SetFileProp(hTarg, hFile, Prop_ExcludeFromBuild, (int)fExclude);}
	
	// Set an integer or string property for this file
	EXPORTED BOOL SetFileProp
	(
		HBLDTARGET		hTarg,					// handle to the target that contains the file
		HBLDFILE		hFile,					// handle to the file
		UINT			idProp,					// identifier of the property
		int				i						// property value
	);

	EXPORTED BOOL SetFileProp
	(
		HBLDTARGET		hTarg,					// handle to the target that contains the file
		HBLDFILE		hFile,					// handle to the file
		UINT			idProp,					// identifier of the property
		CString &		str						// property value
	);

	EXPORTED void MakeFilePropsDefault
	(
		HBLDTARGET		hTarg,					// handle to the target that contains the file
		HBLDFILE		hFile					// handle to the file
	);

	//
	// Build state etc. API
	//

	// Returns the state of the primary target.
	// Any files that are considered out of date,
	// eg. an .obj, .exe, or .dll, are returned in the 
	// path list 'plstPath'. Use FileState() to determine
	// the state of these.
	//
	// N.B. build system will alloc./dealloc the CPath *'s in 'plstPath'
	EXPORTED UINT TargetState
	(
		CObList *	plstPath = (CObList *)NULL,	// paths that are not current (== (CObList *)NULL if not req'd)
		const TCHAR * pchTarget = NULL,			// default is the primary target (usually .exe)
		HBLDTARGET	hTarg = NO_TARGET,			// handle to the target 
		HBUILDER	hBld = ACTIVE_BUILDER		// handle to the builder that contains this
	);

	EXPORTED UINT FileState
	(
		const CPath * pathFile				// path of the file to get state for
	);

	//
	// Dependency searching API
	// (eg. source dependencies such as #includes)
	//

	// Given a path to a source file, 'pathSrc', and a list of include directives, 'strlstIncs',
	// this function will return a list of paths, 'lstpath', that represent the 'resolved' (or
	// absolute) location of the file represented by each include directive.
	// The include path used is that for the platform 'strPlat', or thbe platform for the currently
	// selected target if 'strPlat' is empty.
	// Any standard includes, eg. #include <stdio.h>, will be ignored if 'fIgnStdIncs' is TRUE.
	// If per-file include directives, eg. compiler /I, exist for the 'pathSrc' then these will be used.
	//
	// Returns FALSE if the operation cannot be performed.
	EXPORTED BOOL ResolveIncludeDirectives
	(
		const CPath &		pathSrc,
		const CStringList &	strlstIncs, 
		CStringList &			lstPath,
		BOOL				fIgnStdIncs = TRUE,
		const CString &		strPlat = _TEXT("")
	);

 	// As for above but given a directory to initially search, 'dirBase'.
	EXPORTED BOOL ResolveIncludeDirectives
	(
		const CDir &		dirBase,
		const CStringList &	strlstIncs,
		CStringList &			lstPath,
		BOOL				fIgnStdIncs,
		const CString &		strPlat
	);

	// This is used by parser, so we can search a list of parent locations
	EXPORTED BOOL ResolveIncludeDirectives
	(
		const CObList & lstpathSrc,              //list of path as parent dir to search include 
		const CStringList & strlstIncs,
		CStringList & lstPath,
		HBLDTARGET hTarget
	);


	// Appends the full search path (directory list) to 'lstIncDirs', for includes for a file, 'pathSrc',
	// on a particular platform, 'strPlat'.
	// If 'strPlat' is empty, then the platform for the currently selected target is used.
	// FUTURE: Use per-file include paths, eg. as specified by /i for the linker
	//
	// Note: It is up to the caller of this function to deallocate the CDir *'s in 'lstIncDirs'.
	//
	// Returns FALSE if the operation cannot be performed.
	EXPORTED BOOL GetIncludePath
	(
		CObList &			lstIncDirs,
		const CString &		strPlat = _TEXT(""),
		const CPath *		ppathSrc = (const CPath *)NULL,
		const CString & strTool = _TEXT(""),
		HBUILDER hBld = ACTIVE_BUILDER
	);

	//Parser will call this function to give the pathSrc file's include list 
	EXPORTED void UpdateDependencyList
	(
		const CPath &		pathSrc,				// source file
		const CStringList &	strlstIncs,				// list of includes for this file
		HBLDTARGET			hTarg,					// handle to the target
		HBUILDER			hBld = ACTIVE_BUILDER	// handle to the builder that contains this
	);

	//
	// Enumeration and information API
	//
	// WARNING:
	// Currently these enumeration API can only enumerate single builder items at a time.
	// o file set enumertion
	// o file enumeration
	//

	EXPORTED void InitBuilderEnum();
	EXPORTED HBUILDER GetNextBuilder(CString & strBuilder, BOOL bOnlyLoaded = TRUE);
	EXPORTED void InitTargetEnum(HBUILDER hBld = ACTIVE_BUILDER);
	// Returns the name of the target as strTarget.
	EXPORTED HBLDTARGET GetNextTarget(CString & strTarget, HBUILDER hBld = ACTIVE_BUILDER);

	EXPORTED void InitProjectDepEnum(HFILESET hFileSet, HBUILDER hBld = ACTIVE_BUILDER);
	EXPORTED HPROJDEP GetNextProjectDep(HFILESET hFileSet, HBUILDER hBld = ACTIVE_BUILDER);
	EXPORTED DWORD GetDepth(HBLDTARGET);

	EXPORTED void InitProjectDepEnum(HFILESET hFileSet, HBUILDER hBld,CObList &lstDepSet, POSITION &posHDepSet );
	EXPORTED HPROJDEP GetNextProjectDep(HFILESET hFileSet, HBUILDER hBld,CObList &lstDepSet, POSITION &posHDepSet );

	EXPORTED void InitFileSetEnum(HBUILDER hBld = ACTIVE_BUILDER);
	EXPORTED HFILESET GetNextFileSet(HBUILDER hBld = ACTIVE_BUILDER);

	EXPORTED void InitFileEnum(HFILESET, UINT filter = FileEnum_RemoveDeps);
	// Returns the file registry handle as frh.
	EXPORTED HBLDFILE GetNextFile(FileRegHandle & frh, HFILESET);
	EXPORTED BOOL IsScanableFile(HBLDFILE hFile);

	// Get a string list of possible target names for a file set
	EXPORTED BOOL GetFileSetTargets
	(
		HFILESET		hFileSet,			// the file set to retrieve the target list for
		CStringList &	strlstTargs			// list of target names
	);

	// Get a string list of possible platform names for a target type (return *all* platforms when 'pchTarg' is NULL)
	EXPORTED const CStringList * GetAvailablePlatforms
	(
		const TCHAR *	pchTarg = (const TCHAR *)NULL, 	// UI or 'official' target name? default=retrieve all platforms
		BOOL			fUIDesc = TRUE					// return UI or 'official' names? default=UI
	);

	// Get a string list of possible target type names for a platform (across *all* installed platforms when 'pchPlat' is NULL)
	EXPORTED const CStringList * GetTargetTypeNames
	(
		const TCHAR *	pchPlat = (const TCHAR *)NULL,	// UI or 'official' platform name? default=retrieve across *all* platforms
		BOOL			fUIDesc = TRUE					// return UI or 'official' names? default=UI
	);

	EXPORTED HFILESET GetFileSet(HBUILDER hBld, HBLDTARGET hTarg);

	//
	// Get particular files of interest from the dependency graph of
	// a particular target and builder (default is active builder)
	// 'idFile' is the id of the filer we're interested in

	EXPORTED BOOL GetBuildFile
	(
		UINT idFile,
		TCHAR * szFileName, int cchFileBuffer,
		HBLDTARGET hTarg,
		HBUILDER hBld = ACTIVE_BUILDER
	);

	//
	// Builder file section support
	//
	// Provide a callback function that will be called when 
	// a buffer needs to be provided to be written or a 
	// buffer has been read and can be processed.
	//
	// Pls. note that sections are distinguished by name.
	EXPORTED BOOL RegisterSection(CBldrSection * pbldsect);
	EXPORTED BOOL DeregisterSection(const CBldrSection * pbldsect);

	// Enumeration
	EXPORTED void InitSectionEnum();
	EXPORTED CBldrSection * GetNextSection(CString & strSection);

	// Lookup
	EXPORTED CBldrSection * FindSection(const TCHAR * pchName, HBUILDER hBld = ACTIVE_BUILDER);

	// 
	// Builder fileset query support
	//

	// Resource files
	EXPORTED BOOL GetResourceFileList
	(
		CPtrList &		listRcPath,				// returned list of CPath *'s
		BOOL			fOnlyBuildable = FALSE,	// only buildable ones?
		HFILESET		hFileSet = NULL,		// handle to the fileset (FUTURE: only does active)
		HBUILDER		hBld = ACTIVE_BUILDER	// build that contains target? default='active builder'
	);

	EXPORTED BOOL GetResourceIncludePath
	(
		CPath *pResFile, 
		CString &strIncludes
	); 
	
	BOOL CBldSysIFace::OpenResource
	(
		const TCHAR *	strResource,				
		long 			lType,
		HBLDTARGET		hTarg,					// handle to the target 
		HBUILDER		hBld					// build that contains target? default='active builder'
	);

	// 
	// Mixed-language support
	//

#define CPlusPlus	0x0
#define FORTRAN		0x1
#define Java		0x2

	// Is the language currently installed?
	EXPORTED BOOL LanguageSupported(UINT idLang);

	//
	// Custom Build step support
	//

	// Macros supported
	//
	// $IntDir			- intermediate directory
	// $OutDir			- output directory
	// $WkspDir			- workspace directory
	// $ProjDir			- project directory
	// $TargetDir		- target directory
	// $InputDir		- input directory
	// $WkspBase		- workspace file basename
	// $Target			- full path of target
	// $TargetBase		- target file basename
	// $Input			- full path of input
	// $InputBase		- input file basename

	EXPORTED BOOL AssignCustomBuildStep
	(
		const TCHAR *	pchCommand,				// comma-seperated list of commands (can be macro-ized)
		const TCHAR *	pchOutput,				// comma-seperated list of output files (can be macro-ized)
		const TCHAR *	pchDescription,			// description (if NULL then default description)
		HBLDTARGET		hTarg,					// handle to the target 
		HBLDFILE		hFile,					// handle to the file (if NO_FILE then custom build step for target)
		HBUILDER		hBld = ACTIVE_BUILDER	// build that contains target? default='active builder'
	);
	
	EXPORTED BOOL UnassignCustomBuildStep
	(
		HBLDTARGET		hTarg,					// handle to the target 
		HBLDFILE		hFile,					// handle to the file (if NO_FILE then custom build step for target)
		HBUILDER		hBld = ACTIVE_BUILDER	// build that contains target? default='active builder'
	);

	//
	// Build view apis
	//
	
	// Get the path of the selected file in the build view. If we have a multi-selection then we
	// take the dominant. If no file is selected the return value is FALSE, TRUE otherwise.
	EXPORTED BOOL GetSelectedFile
	(
		CPath * path							// Selected file
	);

	// Get project sub directory for this hTarget
	EXPORTED BOOL GetProjectDir(HBLDTARGET hTarget, HBUILDER hBld, CString& str);

	// Get the workspace dir which is where .mak/.vcp resident
	EXPORTED BOOL GetWorkspaceDir(HBUILDER hBld, CString& str);

	//
	// Build system enabled?
	//
	// Dsabled if,
	// o no primary platform is installed

	EXPORTED BOOL BuildSystemEnabled();


	//
	// Builder object conversion
	//

	CProject * CnvHBuilder(HBUILDER hBld);
	CTargetItem * CnvHTarget(HBUILDER hBld, HBLDTARGET hTarg);
	CTargetItem * CnvHFileSet(HBUILDER hBld, HFILESET hFileSet);
	CFileItem * CnvHBldFile(HFILESET hFileSet, HBLDFILE hFile);
	CProjectDependency * CnvHProjDep(HFILESET hFileSet, HPROJDEP hProjDep);

	// Rename an existing target
	BOOL RenameTarget
	(
		const TCHAR *	pchTargFrom,					// name of this target to rename
		const TCHAR *	pcgTargTo						// new name for target
	);

	BOOL GetFlavourFromConfigName
	(
		const TCHAR *	pchConfigname,					// configuration name
		CString &		str							// flavour name (e.g, Debug, Release,..)
	);

	BOOL LoadAppWiz();
	
	// Returns whether a build is currently in progress.
	EXPORTED BOOL IsBuildInProgress();

	EXPORTED HRESULT DoSpawn(CStringList& lstCmd, LPCTSTR szDirInit, BOOL fClear, BOOL fASync, DWORD *pcErr, DWORD *pcWarn);

	// Returns the OLE Automation Project object associated with
	//  the given HBUILDER
	IBuildProject* GetAutoProject(HBUILDER hBld);

private:
	// Set tool settings on an item.
	BOOL SetToolSettingsI
	(
		HBLDTARGET		hTarg,						// handle to the target
		CProjItem *		pItem,						// pointer to the builder item
		const TCHAR * 	pchSettings,				// tool settings
		const TCHAR *	pchTool = (const TCHAR *)NULL,	// name of the tool that 'owns' these settings, default='primary tool'NULL)
		BOOL			fAdd = TRUE,				// add or subtact? default=add
		BOOL			fClear = FALSE				// clear settings before add or subtract?
	);

	// Map an external property number to an internal property number
	BOOL MapPropToPropI
	(
		UINT			idProp,						// external property number
		UINT &			idPropI						// internal (private) property number
	);

	// Get the config. record for a file, 'hFile', in a given target, 'hTarg'.
	ConfigurationRecord * GetConfigRecFromFileI
	(
		HBLDTARGET		hTarg,						// handle to the target
		HBLDFILE		hFile						// handle to the file to get the config. for
	);

	// Get the config. record for a target, 'hTarg'
	ConfigurationRecord * GetConfigRecFromTargetI
	(
		HBLDTARGET		hTarg,						// handle to the target
		HBUILDER		hBld = ACTIVE_BUILDER		// builder that contains target? default=('current'==ACTIVE_BUILDER) 
	);									


	// Core method for CBldSysIFace::ResolveIncludeDirectives() methods.
	BOOL ResolveIncludeDirectivesI
	(
		CString & strlstIncDirs,
		const CDir & dirBase,
		const CStringList & strlstIncs,
		CStringList & lstPath,
		BOOL fIgnStdIncs,
		const CString & strPlat,
		const CPath * pathSrc
	);

	// For mirrored case, we need to make sure dep gets updated
	// For C-syntax files, we just queue them up on the force update list, assuming parser will call us
	// the force update list will do update when we write to proj file if parser didn't do anything for us
	// For non-C-syntax files, we will just do the scan deps
	void PreserveDependencies
	(
		CTargetItem * pNewTarget,
		CProject * pProject
	);

	BOOL GetIncludePathI
	(
		CString &			strIncDirs,               //pass back includes in string form 
		const CString &		strPlat = _TEXT(""),
		const CPath *		ppathSrc = (const CPath *)NULL
	);

public:
	~CBldSysIFace();

	// Determine if the given file is pat of the fileset
	EXPORTED BOOL IsProjectFile
	(
		const CPath *	ppathFile,				// file path
		BOOL			bDepsAreProjFiles = FALSE,	//
		HBUILDER		hBld = ACTIVE_BUILDER	// builder that contains target? default='active builder'
	);

	EXPORTED BOOL GetBuilderToolset
	(
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED CPath *GetTargetFileName
	(
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED int GetTargetAttributes
	(
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void GetRemoteTargetFileName
	(
		CString &		strTarg,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void SetRemoteTargetFileName
	(
		CString &		strTarg,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED CWnd *GetSlobWnd
	(
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED const CPath *GetBrowserDatabaseName
	(
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void UpdateBrowserDatabase
	(
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void GetCallingProgramName
	(
		CString &		strProg,
		HBUILDER		hBld = ACTIVE_BUILDER
	);
	
	EXPORTED void SetCallingProgramName
	(
		CString &		strProg,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void GetProgramRunArguments
	(
		CString &		strArgs,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void SetProgramRunArguments
	(
		CString &		strArgs,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void GetWorkingDirectory
	(
		CString &		strDir,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED BOOL TargetIsCaller
	(
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED BOOL GetPromptForDlls
	(
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void SetPromptForDlls
	(
		BOOL			bPrompt,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void SetProjectState
	(
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED BOOL GetClassWizAddedFiles
	(
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void SetClassWizAddedFiles
	(
		BOOL			bAdded,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void ScanAllDependencies
	(
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void GetVersionInfo
	(
		PROJECT_VERSION_INFO *pInfo,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED BOOL GetInitialExeForDebug
	(
		BOOL			bExecute,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED BOOL GetInitialRemoteTarget
	(
		BOOL			bBuild,
		BOOL			fAlways,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED BOOL UpdateRemoteTarget
	(
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED BOOL DoTopLevelBuild
	(
		BOOL			bBuildAll,
		CStringList *	pConfigs,
		FlagsChangedAction	fca,
		BOOL			bVerbose,
		BOOL			bClearOutputWindow,
		HBUILDER		hBld = ACTIVE_BUILDER,
		BOOL			bRecurse = TRUE
	);

	EXPORTED void GetEnvironmentVariables
	(
		CEnvironmentVariableList *pEnvList,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED BOOL IsProfileEnabled
	(
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	CPlatform *GetCurrentPlatform
	(
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	// Return the builder if there is one, or NO_BUILDER if there isn't
	EXPORTED HBUILDER GetBuilder(HBLDTARGET hTarg);
	EXPORTED HBUILDER GetBuilderFromFileSet(HFILESET hFileSet);

	EXPORTED HBUILDER DeactivateBuilder(HBUILDER hBld);
private:
	// delete the lists contained in pUnknownDepMap
	void DeleteUnknownDepMap(void);

private:
	CMapStringToPtr m_mapSections;					// our map of section names to section info.
	POSITION m_posSection;							// section enumeration 

	CStringList m_strlstTargets;					// used to maintain targets names added so far

	POSITION m_posHDepSet;							// target dep enum.
	CObList m_lstDepSet;							// FUTURE: make per-builder

	POSITION m_posHFileSet;							// file set enumeration
	CObList m_lstFileSet;							// FUTURE: make per-builder

	POSITION m_posHFile;							// file enumeration
	CObList m_lstFile;								// FUTURE: make per-fileset

	static CMapStringToOb *pUnknownDepMap;
	CMapPtrToPtr m_mapPrjTargets;
public:
	EXPORTED HBUILDER GetBuilderFromName(const TCHAR * 	pchBld);
	EXPORTED CString GetNameFromBuilder(HBUILDER hBld);
	EXPORTED BOOL SetActiveBuilder(	HBUILDER hBld );

// Java support
public:
	EXPORTED void GetJavaClassName
	(
		CString &		str,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void GetJavaClassFileName
	(
		CString &		str,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void GetJavaDebugUsing
	(
		ULONG*			pDebugUsing,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void GetJavaBrowser
	(
		CString &		str,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void GetJavaStandalone
	(
		CString &		str,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void GetJavaStandaloneArgs
	(
		CString &		str,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void GetJavaHTMLPage
	(
		CString &		str,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void GetJavaClassPath
	(
		CString &		str,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED void GetJavaStandaloneDebug
	(
		ULONG*			pStandaloneDebug,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED BOOL GetInitialJavaInfoForDebug
	(
		BOOL			bExecute,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	// Delete the target which corresponds to a .exe project 
	EXPORTED void DeleteExeTarget
	(
		HBLDTARGET		hTarg					// target to delete
    );

	EXPORTED BOOL GetPreCompiledHeaderName
	(
		CString 		&strPch,
		HBUILDER		hBld = ACTIVE_BUILDER
	);

	EXPORTED BOOL SetUserDefinedDependencies
	(
		const TCHAR *	pchUserDeps,				// comma-seperated list of commands (can be macro-ized)
		HBLDTARGET		hTarg,					// handle to the target 
		HBLDFILE		hFile,					// handle to the file (if NO_FILE then custom build step for target)
		HBUILDER		hBld = ACTIVE_BUILDER	// build that contains target? default='active builder'
	);

	EXPORTED HRESULT GetPkgProject(
		HBUILDER builder,
		IPkgProject** outPkgProject
	);
	EXPORTED BOOL GetProjectGlyph(HBUILDER hBld, HFILESET hFileSet, CImageWell& imageWell, UINT * pnImage);
};									  

void SetWorkspaceDocDirty();

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

// This is used as a base id for the external target
// type, and so we will use id's starting from this
// running to this id + the number of installed platforms.
// Note, we have a maximum of 100 external target types...
#define BCID_ProjType_ExternalTarget	100

// DO NOT PUT ANY BUILD ID'S AFTER THIS ONE
// This is used as a base id for any custom 
// components such as custom tools
#define BCID_Custom_Component			200
		
extern CBldSysIFace g_BldSysIFace;	// one 'n' only bld system interface

#endif // _INCLUDE_BLDSYSIFACE_H
