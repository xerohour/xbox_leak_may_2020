// Microsoft Visual Studio Object Model
// Copyright (C) 1996-1997 Microsoft Corporation
// All rights reserved.

/////////////////////////////////////////////////////////////////////////////
// blddefs.h

// Declaration of constants and error IDs used by objects in the type library
//  VISUAL STUDIO 97 PROJECT SYSTEM (SharedIDE\bin\ide\devbld.pkg)

#ifndef __BLDDEFS_H__
#define __BLDDEFS_H__


///////////////////////////////////////////////////////////////////////
// String constant used by Automation Methods

#define DS_BUILD_PROJECT	"Build"

// consts for AddProject...
const BSTR dsApplication = L"Application";
const BSTR dsDLL = L"DLL";
const BSTR dsConsoleApp = L"Console Application";
const BSTR dsStaticLib = L"Static Library";
const BSTR dsUtility = L"Utility";
const BSTR dsMakefile = L"Makefile";
const BSTR dsFortran1 = L"QuickWin Application";
const BSTR dsFortran2 = L"Standard Graphics Application";


// constant for custom build step in SetTool/GetTool
const BSTR dsCustomBuildStep = L"CustomBuildStep";

// For BuildItems for filtering and for types
enum DsBuildItems
{
	dsBuildFile = 1,
	dsBuildFolder = 2,
	dsBuildDependentProject = 4,
	dsBuildEvent = 8,
};

enum DsBuildItemsFilter   // OR with DsBuildItems
{
	dsSelectedItem = 1024,  // special: allows filtering on selected items
	dsExcluded = 2048,       // put items excluded from building in list
	dsOneLevel = 4096
};

// BuildItem BuildEventTime property
enum DsBuildEventTimes
{
	dsPreBuild = 1,
	dsPreLink = 2,
	dsPostBuild = 4
};

enum DsUseOfMFC
{
	dsNoUseMFC = 					0, // not using MFC
	dsUseMFCInLibrary	 =			1, // using MFC by static link eg. nafxcwd.lib
	dsUseMFCInDll	=				2, // using MFC in shared DLL eg. mfc300d.dll
	dsUseMFCDefault = 2 //				UseMFCInDll // default
};

// Config strings. Use in Variants.
const BSTR dsActiveConfig = L"Active";
const BSTR dsAllConfigs = L"All";


// Source Safe Status (SCC Status) strings
const BSTR  dsSCC_STATUS_NOTCONTROLLED	= L"Not Controlled"; 	// File is not under source control
const BSTR  dsSCC_STATUS_CHECKEDOUT     = L"Checked Out";	// Checked out to current user at local path
const BSTR  dsSCC_STATUS_OUTEXCLUSIVE   = L"\"exclusive\"";	// File is exclusively check out
const BSTR  dsSCC_STATUS_OUTOFDATE      = L"Out-of-date";	// The file is not the most recent
const BSTR  dsSCC_STATUS_DELETED		= L"Deleted";   	// File has been deleted from the project
const BSTR  dsSCC_STATUS_LOCKED			= L"Locked";		// No more versions allowed
const BSTR  dsSCC_STATUS_MERGED         = L"Merged";	// File has been merged but not yet fixed/verified
const BSTR  dsSCC_STATUS_SHARED			= L"Shared";     	// File is shared between projects
const BSTR  dsSCC_STATUS_MODIFIED       = L"Modified";	// File has been modified/broken/violated
const BSTR  dsSCC_STATUS_OUTBYUSER		= L"out \"exclusive\" by another user";	// File is checked out by current user someplace


///////////////////////////////////////////////////////////////////////
// Error constants returned by Automation Methods.

// The configuration represented by this object is no longer valid.
//  The configuration may have been removed by the user, or the
//  workspace closed since the configuration was last accessed.
#define DS_E_CONFIGURATION_NOT_VALID		0x80040301

// The settings can't be added or removed.  Perhaps the tool or options
//  specified do not exist.
#define DS_E_CANT_ADD_SETTINGS				0x80040302
#define DS_E_CANT_REMOVE_SETTINGS			0x80040303

// There was an error in the specification of the custom build step.
#define DS_E_CANT_ADD_BUILD_STEP			0x80040304

// The user attempted to manipulate a configuration of the wrong
//  platform.  For example, trying to manipulate an MIPS configuration
//  while running on an Intel machine will cause this error.
#define DS_E_CONFIGURATION_NOT_SUPPORTED	0x80040305

// The project represented by this object is no longer valid.  The
//  workspace containing that project may have been closed since
//  the project was last accessed, for example.
#define DS_E_PROJECT_NOT_VALID				0x80040306

// A build (or RebuildAll) can not be started when a build is already
//  in progress.  Attempting to do this can cause this error.
#define DS_E_CANT_SPAWN_BUILD				0x80040307

// There was an error in the adding of the file.
#define DS_E_CANT_ADD_FILE					0x80040308

// There was an error in the adding of the configuration.
#define DS_E_CANT_ADD_CONFIGURATION			0x80040309

// "All" configurations not supported in this method
#define DS_E_ALL_NOT_SUPPORTED				0x8004030a

// This project property is not supported in this project
#define DS_E_PROPERTY_NOT_SUPPORTED     		0x8004030b

// The path passsed to InsertProject does not have the form *.dsp
#define DS_E_INSERTPROJECT_NEED_DSP_FILE                0x804b0001

// InsertProject requires an open workspace
#define DS_E_INSERTPROJECT_NEED_OPEN_WORKSPACE          0x804b0002

// InsertProject can't insert the project - duplicate name
#define DS_E_INSERTPROJECT_DUPLICATE_PROJECT_NAME       0x804b0003

// AddProject - Invalid project name or directory path
#define DS_E_ADDPROJECT_BAD_PROJECT_NAME                0x804c0001

// AddProject - Invalid project name or directory path
#define DS_E_ADDPROJECT_BAD_PROJECT_PATH                0x804c0002

// AddProject - Invalid project name or directory path
#define DS_E_ADDPROJECT_NEED_OPEN_WORKSPACE             0x804c0003

// AddProject - Invalid project name or directory path
#define DS_E_ADDPROJECT_DIRECTORY_EXISTS_AS_FILE        0x804c0004

// AddProject - Invalid project name or directory path
#define DS_E_ADDPROJECT_BAD_PROJECT_TYPE                0x804c0005

// AddProject - Invalid project name or directory path
#define DS_E_ADDPROJECT_DUPLICATE_PROJECT_NAME          0x804c0006

// AddProject - Invalid project name or directory path
#define DS_E_ADDPROJECT_COULD_NOT_CREATE_PROJECT        0x804c0007

// AddProject - Invalid project name or directory path
#define DS_E_ADDPROJECT_COULD_NOT_GET_CONFIGURATIONS    0x804c0008

// AddProject - Invalid project name or directory path
#define DS_E_ADDPROJECT_COULD_NOT_GET_IPkgProject       0x804c0009

// AddProject - Invalid project name or directory path
#define DS_E_ADDPROJECT_DIRECTORY_IS_READ_ONLY          0x804c000A

// AddProject - .dsp file exists and is read only
#define DS_E_ADDPROJECT_PROJECT_FILE_EXISTS_AND_IS_READ_ONLY 0x804c000B

// AddProject - .dsp file exists and is read only
#define DS_E_PROPERTY_DOES_NOT_EXIST 0x804c000C

#endif //__BLDDEFS_H__

