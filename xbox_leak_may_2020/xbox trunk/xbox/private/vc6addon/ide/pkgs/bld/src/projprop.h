// Sushi Project Properties

#ifndef __PROJPROP__
#define __PROJPROP__

//
// CSlob notitifications
//
// Disable passing on of OnInform messages
#define SN_HOLD_UPDATES 8
// Re-enable OnInform messages. Assume property based caches must be refreshed.
#define SN_RELEASE_UPDATES 9
// Re-enable OnInform messages. Do not update cached info or set dirty state.
#define SN_RELEASE_UPDATES_NO_CHANGES 10
#define SN_PARENT_PROPERTY 11	// Some property of a parents parent changed.
#define SN_SAVED        12		// Slob was just serialized
// Filename that slob is saved in has changed
// (not a property since its not undoable)
#define SN_FILE_NAME    13
#define SN_LOADED       14		// Slob was just deserialized
#define SN_DIRTY        15		// Slob state force to clean
#define SN_CLEAN        16		// Slob state forced to dirty
#define SN_NEW_TREE     17		// Slob's tree has moved, ie the Slob's container
#define SN_DESTROY_OPTNODE	18	// Destroying an option tree node
#define SN_DESTROY_CONFIG   19  // Destroying a config

//
//	Special properties for build items (should be move to PROPS.H).
//	These are deivded into two sets: private props that get written into the
//	OPT file and public props that go in the .bld file:
//
#define PROJ_PROPS						1500
#define PROJ_PRIVATE_FIRST				PROJ_PROPS

#define P_BuildExcluded       			PROJ_PROPS + 2		// Per-config (currently NYI)
#define P_BuildFrozen       			PROJ_PROPS + 3		// Per-config (currently NYI)
#define P_BuildLocked					PROJ_PROPS + 4		// Per-config (currently NYI)
#define P_ExtOpts_CmdLine				PROJ_PROPS + 5		// Per-config, ext. makefiles only
#define P_ExtOpts_RebuildOpt			PROJ_PROPS + 6		// Per-config, ext. makefiles only
#define P_ExtOpts_Targ					PROJ_PROPS + 7		// Per-config, ext. makefiles only
#define P_ExtOpts_BscName				PROJ_PROPS + 8		// Per-config, ext. makefiles only
#define P_ExtOpts_Platform				PROJ_PROPS + 9		// Per-config, ext. makefiles only	// "Win32 (80x86), "Macintosh (680x0), etc.
//#define P_ExtOpts_ClsWzdName			PROJ_PROPS + 10		// Per-config, ext. makefiles only
#define P_ProjActiveConfiguration		PROJ_PROPS + 11		// Only CProject object has this
#define P_Caller						PROJ_PROPS + 12		// Per-config
#define P_Args							PROJ_PROPS + 13		// Per-config
#define P_PromptForDlls					PROJ_PROPS + 14  	// Per-config
#define P_RemoteTarget					PROJ_PROPS + 15		// Per-config
#define P_WorkingDirectory				PROJ_PROPS + 16		// Per-config
#define P_ItemOptState					PROJ_PROPS + 17  	// Per-config
#define P_ExtOpts_WorkingDir			PROJ_PROPS + 18 		// Per-config, ext. makefiles only
#define P_Java_ClassFileName			PROJ_PROPS + 19		// name of the class file to debug
#define P_Java_DebugUsing				PROJ_PROPS + 20		// specifies Java_DebugUsing_Browser or Java_DebugUsing_Standalone
#define P_Java_Browser					PROJ_PROPS + 21		// the name of the browser
#define P_Java_Standalone				PROJ_PROPS + 22		// the name of the stand-alone interpreter
#define P_Java_ParamSource				PROJ_PROPS + 23		// specifies Java_ParamSource_HTMLPage or Java_ParamSource_User
#define P_Java_HTMLPage					PROJ_PROPS + 24		// name of an HTML page
#define P_Java_TempHTMLPage				PROJ_PROPS + 25		// name of the temp HTML page we generate
#define P_Java_HTMLArgs					PROJ_PROPS + 26		// parameters/arguments in HTML form
#define P_Java_Args						PROJ_PROPS + 27		// parameters/arguments in cmd line form
#define P_Java_StandaloneArgs			PROJ_PROPS + 28		// arguments to the stand-alone interpreter
#define P_Java_ExtraClasses				PROJ_PROPS + 29		// Extra classes for java project
#define P_Java_DebugStandalone			PROJ_PROPS + 30		// specifies Java_DebugStandalone_Applet or Java_DebugStandalone_Application

#define P_HasExternalDeps				PROJ_PROPS + 31		// This mean that we should create an external dep folder.

#define PROJ_PRIVATE_LAST				PROJ_PROPS + 49

//
// Properties in this range are NOT per configuration.
//
#define PROJ_NOT_PER_CONFIG_FIRST		PROJ_PROPS + 50

#define P_GroupName						PROJ_NOT_PER_CONFIG_FIRST + 0
#define P_GroupDefaultFilter			PROJ_NOT_PER_CONFIG_FIRST + 1
#define P_TargetName					PROJ_NOT_PER_CONFIG_FIRST + 2	

#define P_ProjSccProjName				PROJ_NOT_PER_CONFIG_FIRST + 3
#define P_ProjSccRelLocalPath			PROJ_NOT_PER_CONFIG_FIRST + 4
#define P_ItemExcludedFromScan			PROJ_NOT_PER_CONFIG_FIRST + 5

#define P_Proj_PerConfigDeps			PROJ_NOT_PER_CONFIG_FIRST + 6
#ifdef VB_MAKEFILES
#define P_Proj_IsVB						PROJ_NOT_PER_CONFIG_FIRST + 7
#define P_VBProjName					PROJ_NOT_PER_CONFIG_FIRST + 8
#endif
#define PROJ_NOT_PER_CONFIG_LAST		PROJ_PROPS + 75

//
// Properties in this range are PER CONFIGURATION
// (meaning they go into property bags).
//
#define PROJ_PER_CONFIG_FIRST		PROJ_PROPS + 76

#define P_ProjConfiguration			PROJ_PER_CONFIG_FIRST + 0	// Only CProject object has this
#define P_ProjOriginalType			PROJ_PER_CONFIG_FIRST + 1	// Only CProject object has this
#define P_OutDirs_Intermediate		PROJ_PER_CONFIG_FIRST + 2
#define P_OutDirs_Target			PROJ_PER_CONFIG_FIRST + 3
#define P_ItemExcludedFromBuild		PROJ_PER_CONFIG_FIRST + 4 	// All CProjItems have this
#define P_GroupHasPerGroupFlags		PROJ_PER_CONFIG_FIRST + 5	// Only CProjGroup objects have this
#define P_ProjUseMFC				PROJ_PER_CONFIG_FIRST + 6	// Only CProject object has this
#define P_UseDebugLibs				PROJ_PER_CONFIG_FIRST + 7	// Only CProject object has this
#define P_Proj_CmdLine				PROJ_PER_CONFIG_FIRST + 8	// Only CProject object has this (on external targets)
#define P_Proj_RebuildOpt			PROJ_PER_CONFIG_FIRST + 9	// Only CProject object has this (on external targets)
#define P_Proj_Targ					PROJ_PER_CONFIG_FIRST + 10	// Only CProject object has this (on external targets)
#define P_Proj_BscName				PROJ_PER_CONFIG_FIRST + 11	// Only CProject object has this (on external targets)
//#define P_Proj_WorkingDir           PROJ_PER_CONFIG_FIRST + 12  // Only CProject object has this
#define P_ProjClsWzdName			PROJ_PER_CONFIG_FIRST + 12  // Only CProject object has this
#define P_Proj_TargDefExt			PROJ_PER_CONFIG_FIRST + 13	// Only CProject object has this
#define P_Proj_TargDir				PROJ_PER_CONFIG_FIRST + 14	// Only CProject object has this
#define P_Proj_IgnoreExportLib				PROJ_PER_CONFIG_FIRST + 15	// Only CProject object has this (on dll based targets)
#define P_ItemIgnoreDefaultTool		PROJ_PER_CONFIG_FIRST + 16

#define NoUseMFC					0 // not using MFC
#define UseMFCInLibrary				1 // using MFC by static link eg. nafxcwd.lib
#define UseMFCInDll					2 // using MFC in shared DLL eg. mfc300d.dll
#define UseMFCDefault				UseMFCInDll // default

#define PROJ_PER_CONFIG_UNKNOWN			PROJ_PROPS + 150
#define PROJ_PER_CONFIG_LAST			PROJ_PROPS + 399
#define PROJ_OPT_UNKNOWN				PROJ_PROPS + 400

// Build system option handler components are allocated a property range
// where all ids in the range are >= PROJ_BUILD_OPTION_FIRST and < PROJ_BUILD_OPTION_PRIVATE_FIRST
// There are PROJ_BUILD_OPTION_RANGE props that can be allocated
#define PROJ_BUILD_OPTION_FIRST			PROJ_OPT_UNKNOWN + 100
#define PROJ_BUILD_OPTION_PRIVATE_FIRST 0x8000
#define PROJ_BUILD_OPTION_RANGE			PROJ_BUILD_OPTION_PRIVATE_FIRST - PROJ_BUILD_OPTION_FIRST
// Deferred tool option props.
// 0 = Don't defer, 1 = Defer, 2 = Never build
#define P_DeferredMecr					PROJ_BUILD_OPTION_PRIVATE_FIRST + 0
#define P_NoDeferredBscmake				PROJ_BUILD_OPTION_PRIVATE_FIRST + 1
#define PROJ_BUILD_OPTION_PRIVATE_LAST	PROJ_BUILD_OPTION_PRIVATE_FIRST + 99

#define PROJ_BUILD_OPTION_LAST			PROJ_BUILD_OPTION_FIRST + PROJ_BUILD_OPTION_RANGE + 100 

//
// General properties
//
#define PROJ_GENERAL					PROJ_BUILD_OPTION_LAST + 1

//
// Tool Component Props
//
#define P_CompName					PROJ_GENERAL + 0
#define P_ToolExeName				PROJ_GENERAL + 1
#define P_ToolMenuText				PROJ_GENERAL + 2
#define P_ToolPerProject			PROJ_GENERAL + 3
#define P_ToolInternal				PROJ_GENERAL + 4
#define P_ToolInput					PROJ_GENERAL + 5
#define P_ToolOutput				PROJ_GENERAL + 6
#define P_ToolCommand				PROJ_GENERAL + 7
#define P_ToolPrefix				PROJ_GENERAL + 8
#define P_CustomDescription			PROJ_GENERAL + 9
#define P_CustomOutputSpec			PROJ_GENERAL + 10
#define P_CustomCommand				PROJ_GENERAL + 11
#define P_CustomMacros              PROJ_GENERAL + 12

// Support custom build step dependencies
#define P_UserDefinedDeps			PROJ_GENERAL + 13

// Pre-link command step
#define P_PreLink_Description       PROJ_GENERAL + 14
#define P_PreLink_Command           PROJ_GENERAL + 15

// Post-build command step
#define P_PostBuild_Description     PROJ_GENERAL + 16
#define P_PostBuild_Command         PROJ_GENERAL + 17

#define PROJ_BUILD_TOOL_FIRST		P_CompName
#define PROJ_BUILD_TOOL_LAST		P_PostBuild_Command

#define PROJ_GENERAL_LAST			PROJ_GENERAL + 99

#if PROJ_BUILD_TOOL_LAST > PROJ_GENERAL_LAST
#error PROJ_BUILD_TOOL_LAST > PROJ_GENERAL_LAST
#endif

//
// Faked properties
//
#define PROJ_FAKE					PROJ_GENERAL_LAST + 1
#define P_ProjItemName				PROJ_FAKE + 0
#define P_ProjItemIconWell			PROJ_FAKE + 1
#define P_ProjItemIconIdx			PROJ_FAKE + 2
#define P_ProjItemFullPath			PROJ_FAKE + 3
#define P_ProjItemDate				PROJ_FAKE + 4
#define P_ProjMainTarget			PROJ_FAKE + 5
#define P_ProjMainTargetDate		PROJ_FAKE + 6
#define P_ProjAppWizUseMFC			PROJ_FAKE + 7
#define P_ItemOptChanged			PROJ_FAKE + 8
#define P_BldrFileName				PROJ_FAKE + 9

//
// Target reference name
#define P_TargetRefName				PROJ_FAKE + 10

//
// Tools property page
#define P_ItemTools					PROJ_FAKE + 11
#define P_ItemSettings				PROJ_FAKE + 12
#define P_ItemInput					PROJ_FAKE + 13
#define P_ItemOutput				PROJ_FAKE + 14
#define P_ItemDependency			PROJ_FAKE + 15

#define P_ProjItemOrgPath			PROJ_FAKE + 16
#define P_FakeProjItemOrgPath			PROJ_FAKE + 17
#define P_FakeProjItemName			PROJ_FAKE + 18
#define P_FakeGroupFilter			PROJ_FAKE + 19

#ifdef _DEBUG
// These props are used to support the "project info" prop pages (debug only).
#define P_D_Target					PROJ_FAKE + 90
#define P_D_PropList				PROJ_FAKE + 91
#define P_D_PropType				PROJ_FAKE + 92
#define P_D_PropValue				PROJ_FAKE + 93
#define P_D_PropBag					PROJ_FAKE + 94
#endif

// These props are used exclusively by QA to inquire about project item props.
#define P_QA_ProjItemType			PROJ_FAKE + 95
#define QA_TypeIs_Target			0
#define QA_TypeIs_Group				1
#define QA_TypeIs_File				2
#define QA_TypeIs_DepGroup			3
#define QA_TypeIs_DepFile			4

#define PROJ_FAKE_LAST				PROJ_FAKE + 99

//
// Properties for the CBuildSlobs & CBuildViewSlobs
//
// FUTURE (colint): These should be moved elsewhere

#define PROJ_BLDSLOB                PROJ_FAKE_LAST
#define P_ConfigName                PROJ_BLDSLOB + 0

#define PROJ_BLDSLOB_LAST           PROJ_BLDSLOB + 99

//
// Scecial props that get read in, but never written:
//
#define P_ProjMarkedForScan			PROJ_BLDSLOB_LAST + 1
#define P_ProjMarkedForSave			PROJ_BLDSLOB_LAST + 2
#define P_ProjHasNoOPTFile			PROJ_BLDSLOB_LAST + 3
#define P_TargNeedsDepUpdate		PROJ_BLDSLOB_LAST + 4

// Helper functions for determining what kind of prop something is:
__inline BOOL IsPropPrivate(int nProp)
{
	// o in our private range?
	// o one of our tool option private props?
	if ((nProp >= PROJ_PRIVATE_FIRST && nProp <= PROJ_PRIVATE_LAST) ||
		(nProp >= PROJ_BUILD_OPTION_PRIVATE_FIRST && nProp <= PROJ_BUILD_OPTION_PRIVATE_LAST))
		return nProp != P_DeferredMecr;	// private

	return FALSE; // public
}

__inline BOOL IsPropPublic(int nProp)
{
	// is this not in  our private range?
	if (nProp > PROJ_PRIVATE_LAST && nProp <= PROJ_GENERAL_LAST)
	{
		// make sure it's not a tool option private prop!
		if (nProp >= PROJ_BUILD_OPTION_PRIVATE_FIRST && nProp <= PROJ_BUILD_OPTION_PRIVATE_LAST)
			return nProp == P_DeferredMecr;	// private!

		return TRUE;	// public
	}

	return FALSE;	// private
}

#endif // __PROJPROP__
