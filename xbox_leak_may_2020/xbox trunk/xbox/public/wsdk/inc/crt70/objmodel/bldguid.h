// Microsoft Visual Studio Object Model
// Copyright (C) 1996-1997 Microsoft Corporation
// All rights reserved.

/////////////////////////////////////////////////////////////////////////////
// bldguid.h

// Declaration of GUIDs used for objects found in the type library
//  VISUAL STUDIO 97 PROJECT SYSTEM (SharedIDE\bin\ide\devbld.pkg)

// NOTE!!!  This file uses the DEFINE_GUID macro.  If you #include
//  this file in your project, then you must also #include it in
//  exactly one of your project's other files with a 
//  "#include <initguid.h>" beforehand: i.e.,
//		#include <initguid.h>
//		#include <bldguid.h>
//  If you fail to do this, you will get UNRESOLVED EXTERNAL linker errors.
//  The Developer Studio add-in wizard automatically does this for you.

#ifndef __BLDGUID_H__
#define __BLDGUID_H__

/////////////////////////////////////////////////////////////////////////
// BuildProject Object IID

// {96961264-A819-11cf-AD07-00A0C9034965}
struct __declspec(uuid("{96961264-A819-11cf-AD07-00A0C9034965}")) IBuildProject;
DEFINE_GUID(IID_IBuildProject,
0x96961264L,0xA819,0x11CF,0xAD,0x07,0x00,0xA0,0xC9,0x03,0x49,0x65);


/////////////////////////////////////////////////////////////////////////
// Configuration Object IID

// {96961263-A819-11cf-AD07-00A0C9034965}
struct __declspec(uuid("{96961263-A819-11cf-AD07-00A0C9034965}")) IConfiguration;
DEFINE_GUID(IID_IConfiguration,
0x96961263L,0xA819,0x11CF,0xAD,0x07,0x00,0xA0,0xC9,0x03,0x49,0x65);


/////////////////////////////////////////////////////////////////////////
// Configurations Collection Object IID

// {96961260-A819-11cf-AD07-00A0C9034965}
struct __declspec(uuid("{96961260-A819-11cf-AD07-00A0C9034965}")) IConfigurations;
DEFINE_GUID(IID_IConfigurations,
0x96961260L,0xA819,0x11CF,0xAD,0x07,0x00,0xA0,0xC9,0x03,0x49,0x65);


/////////////////////////////////////////////////////////////////////////
// BuildItems Collection Object IID

// {96961272-A819-11cf-AD07-00A0C9034965}
struct __declspec(uuid("{96961272-A819-11cf-AD07-00A0C9034965}")) IBuildItems;
DEFINE_GUID(IID_IBuildItems,
0x96961272L,0xA819,0x11CF,0xAD,0x07,0x00,0xA0,0xC9,0x03,0x49,0x65);

/////////////////////////////////////////////////////////////////////////
// IBuildItem Object IID

// {96961273-A819-11cf-AD07-00A0C9034965}
struct __declspec(uuid("{96961273-A819-11cf-AD07-00A0C9034965}")) IBuildItem;
DEFINE_GUID(IID_IBuildItem,
0x96961273L,0xA819,0x11CF,0xAD,0x07,0x00,0xA0,0xC9,0x03,0x49,0x65);

/////////////////////////////////////////////////////////////////////////
// IBuildFile Object IID

// {E8D08BDD-023E-11D2-8CDF-00C04F8EEA30}
struct __declspec(uuid("{E8D08BDD-023E-11D2-8CDF-00C04F8EEA30}")) IBuildFile;
DEFINE_GUID(IID_IBuildFile,
0xE8D08BDDL,0x023E,0x11D2L,0x8C,0xDF,0x00,0xC0,0x4F,0x8E,0xEA,0x30);

/////////////////////////////////////////////////////////////////////////
// IBuildFolder Object IID

// {E8D08BDE-023E-11D2-8CDF-00C04F8EEA30}
struct __declspec(uuid("{E8D08BDE-023E-11D2-8CDF-00C04F8EEA30}")) IBuildFolder;
DEFINE_GUID(IID_IBuildFolder,
0xE8D08BDEL,0x023E,0x11D2L,0x8C,0xDF,0x00,0xC0,0x4F,0x8E,0xEA,0x30);

/////////////////////////////////////////////////////////////////////////
// IBuildDependentProject Object IID

// {E8D08BDF-023E-11D2-8CDF-00C04F8EEA30}
struct __declspec(uuid("{E8D08BDF-023E-11D2-8CDF-00C04F8EEA30}")) IBuildDependentProject;
DEFINE_GUID(IID_IBuildDependentProject,
0xE8D08BDFL,0x023E,0x11D2L,0x8C,0xDF,0x00,0xC0,0x4F,0x8E,0xEA,0x30);

/////////////////////////////////////////////////////////////////////////
// IBuildEvent Object IID

// {E8D08BE0-023E-11D2-8CDF-00C04F8EEA30}
struct __declspec(uuid("{E8D08BE0-023E-11D2-8CDF-00C04F8EEA30}")) IBuildEvent;
DEFINE_GUID(IID_IBuildEvent,
0xE8D08BE0L,0x023E,0x11D2L,0x8C,0xDF,0x00,0xC0,0x4F,0x8E,0xEA,0x30);


/////////////////////////////////////////////////////////////////////////
// IPlatform Object IID

// {BE708853-F5EA-11D1-8344-00A0C91BC942}
struct __declspec(uuid("{BE708853-F5EA-11D1-8344-00A0C91BC942}")) IPlatform;
DEFINE_GUID(IID_IPlatform,
0xBE708853L,0xF5EA,0x11D1,0x83,0x44,0x00,0xA0,0xC9,0x1B,0xC9,0x42);


/////////////////////////////////////////////////////////////////////////
// IPlatforms Object IID

// {BE708854-F5EA-11D1-8344-00A0C91BC942}
struct __declspec(uuid("{BE708854-F5EA-11D1-8344-00A0C91BC942}")) IPlatforms;
DEFINE_GUID(IID_IPlatforms,
0xBE708854L,0xF5EA,0x11D1,0x83,0x44,0x00,0xA0,0xC9,0x1B,0xC9,0x42);


#ifndef __IEnvironment_INTERFACE_DEFINED__

/////////////////////////////////////////////////////////////////////////
// IEnvironment Object IID

// {12DB050A-C4EB-11D1-ABAE-0000F8026984}
struct __declspec(uuid("{12DB050A-C4EB-11D1-ABAE-0000F8026984}")) IEnvironment;
DEFINE_GUID(IID_IEnvironment,
0x12DB050AL,0xC4EB,0x11D1,0xAB,0xAE,0x00,0x00,0xF8,0x02,0x69,0x84);
#endif


#ifndef __IEnvironmentVariable_INTERFACE_DEFINED__
/////////////////////////////////////////////////////////////////////////
// IEnvironmentVariable Object IID

// {12DB050C-C4EB-11D1-ABAE-0000F8026984}
struct __declspec(uuid("{12DB050C-C4EB-11D1-ABAE-0000F8026984}")) IEnvironmentVariable;
DEFINE_GUID(IID_IEnvironmentVariable,
0x12DB050CL,0xC4EB,0x11D1,0xAB,0xAE,0x00,0x00,0xF8,0x02,0x69,0x84);



#endif



#endif //__BLDGUID_H__
