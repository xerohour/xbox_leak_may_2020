// Microsoft Visual Studio Object Model
// Copyright (C) 1996-1997 Microsoft Corporation
// All rights reserved.

/////////////////////////////////////////////////////////////////////////////
// addguid.h

// Declaration of interface GUID for IDSAddIn.  IDSAddIn is defined in
//  addauto.h

// NOTE!!!  This file uses the DEFINE_GUID macro.  If you #include
//  this file in your project, then you must also #include it in
//  exactly one of your project's other files with a 
//  "#include <initguid.h>" beforehand: i.e.,
//		#include <initguid.h>
//		#include <addguid.h>
//  If you fail to do this, you will get UNRESOLVED EXTERNAL linker errors.
//  The Developer Studio add-in wizard automatically does this for you.

#ifndef __ADDGUID_H__
#define __ADDGUID_H__

// {94A7C0A0-4D93-11D2-8CF3-00C04F8EEA30}
struct __declspec(uuid("{94A7C0A0-4D93-11D2-8CF3-00C04F8EEA30}")) IDSAddIn;
DEFINE_GUID(IID_IDSAddIn, 
0x94A7C0A0L,0x4D93,0x11D2,0x8C,0xF3,0x00,0xC0,0x4F,0x8E,0xEA,0x30);


#endif //__ADDGUID_H__
