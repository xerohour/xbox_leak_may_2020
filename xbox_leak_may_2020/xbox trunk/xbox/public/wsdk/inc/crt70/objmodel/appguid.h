// Microsoft Visual Studio Object Model
// Copyright (C) 1996-1997 Microsoft Corporation
// All rights reserved.

/////////////////////////////////////////////////////////////////////////////
// appguid.h

// Declaration of GUIDs used for objects found in the type library
//  VISUAL STUDIO 97 SHARED OBJECTS (SharedIDE\bin\devshl.dll)

// NOTE!!!  This file uses the DEFINE_GUID macro.  If you #include
//  this file in your project, then you must also #include it in
//  exactly one of your project's other files with a 
//  "#include <initguid.h>" beforehand: i.e.,
//		#include <initguid.h>
//		#include <appguid.h>
//  If you fail to do this, you will get UNRESOLVED EXTERNAL linker errors.
//  The Developer Studio add-in wizard automatically does this for you.

#ifndef __APPGUID_H__
#define __APPGUID_H__

/////////////////////////////////////////////////////////////////////////
// Application Object IDs

// {5330A42A-4B9C-11D2-8CF3-00C04F8EEA30}
struct __declspec(uuid("{5330A42A-4B9C-11D2-8CF3-00C04F8EEA30}")) IApplication;
DEFINE_GUID(IID_IApplication,
0x5330A42AL,0x4B9C,0x11D2,0x8C,0xF3,0x00,0xC0,0x4F,0x8E,0xEA,0x30);

// {8EA3F900-4A9F-11cf-8E4E-00AA004254C4}
struct __declspec(uuid("{8EA3F900-4A9F-11cf-8E4E-00AA004254C4}")) IApplicationEvents;
DEFINE_GUID(IID_IApplicationEvents, 
0x8ea3f900, 0x4a9f, 0x11cf, 0x8e, 0x4e, 0x0, 0xaa, 0x0, 0x42, 0x54, 0xc4);

// {5330A42D-4B9C-11D2-8CF3-00C04F8EEA30}
struct __declspec(uuid("{5330A42D-4B9C-11D2-8CF3-00C04F8EEA30}")) IApplicationEvents2;
DEFINE_GUID(IID_IApplicationEvents2, 
0x5330A42DL,0x4B9C,0x11D2,0x8C,0xF3,0x00,0xC0,0x4F,0x8E,0xEA,0x30);

// {5330A42C-4B9C-11D2-8CF3-00C04F8EEA30}
DEFINE_GUID(CLSID_Application, 
0x5330A42CL,0x4B9C,0x11D2,0x8C,0xF3,0x00,0xC0,0x4F,0x8E,0xEA,0x30);


/////////////////////////////////////////////////////////////////////////
// Document Object IID

// {FB7FDAE1-89B8-11cf-9BE8-00A0C90A632C}
struct __declspec(uuid("{FB7FDAE1-89B8-11cf-9BE8-00A0C90A632C}")) IGenericDocument;
DEFINE_GUID(IID_IGenericDocument, 
0xfb7fdae1, 0x89b8, 0x11cf, 0x9b, 0xe8, 0x0, 0xa0, 0xc9, 0xa, 0x63, 0x2c);


/////////////////////////////////////////////////////////////////////////
// Documents Collection Object IID

// {FB7FDAE3-89B8-11CF-9BE8-00A0C90A632C}
struct __declspec(uuid("{FB7FDAE3-89B8-11CF-9BE8-00A0C90A632C}")) IDocuments;
DEFINE_GUID(IID_IDocuments,
0xFB7FDAE3L,0x89B8,0x11CF,0x9B,0xE8,0x00,0xA0,0xC9,0x0A,0x63,0x2C);


/////////////////////////////////////////////////////////////////////////
// Window Object IID

// {FD20FC80-A9D2-11cf-9C13-00A0C90A632C}
struct __declspec(uuid("{FD20FC80-A9D2-11cf-9C13-00A0C90A632C}")) IGenericWindow;
DEFINE_GUID(IID_IGenericWindow,
0xFD20FC80L,0xA9D2,0x11CF,0x9C,0x13,0x00,0xA0,0xC9,0x0A,0x63,0x2C);


/////////////////////////////////////////////////////////////////////////
// Windows Collection Object IID

// {3928F551-96E6-11cf-9C00-00A0C90A632C}
struct __declspec(uuid("{3928F551-96E6-11cf-9C00-00A0C90A632C}")) IWindows;
DEFINE_GUID(IID_IWindows, 
0x3928f551L, 0x96e6, 0x11cf, 0x9c, 0x00, 0x00, 0xa0, 0xc9, 0xa, 0x63, 0x2c);


/////////////////////////////////////////////////////////////////////////
// Project Object IID

// {8CA5A960-FC7D-11cf-927D-00A0C9138C45}
struct __declspec(uuid("{8CA5A960-FC7D-11cf-927D-00A0C9138C45}")) IGenericProject;
DEFINE_GUID(IID_IGenericProject, 
0x8ca5a960, 0xfc7d, 0x11cf, 0x92, 0x7d, 0x0, 0xa0, 0xc9, 0x13, 0x8c, 0x45);


/////////////////////////////////////////////////////////////////////////
// Projects Collection Object IID

// {13BF7741-A7E8-11cf-AD07-00A0C9034965}
struct __declspec(uuid("{13BF7741-A7E8-11cf-AD07-00A0C9034965}")) IProjects;
DEFINE_GUID(IID_IProjects,
0x13BF7741L,0xA7E8,0x11CF,0xAD,0x07,0x00,0xA0,0xC9,0x03,0x49,0x65);


/////////////////////////////////////////////////////////////////////////
// Workspace Collection Object IID

// {4109DFBE-E9B9-11D1-833B-00A0C91BC942}
struct __declspec(uuid("{4109DFBE-E9B9-11D1-833B-00A0C91BC942}")) IWorkspace;
DEFINE_GUID(IID_IWorkspace,
0x4109DFBEL,0xE9B9,0x11D1,0x83,0x3B,0x00,0xA0,0xC9,0x1B,0xC9,0x42);




#endif //__APPGUID_H__

