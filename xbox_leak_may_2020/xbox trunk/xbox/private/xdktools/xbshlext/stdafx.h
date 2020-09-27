/*++

Copyright (c) Microsoft Corporation

Module Name:

    stdafx.h

Abstract:
    
    Precompiled Header for Xbox Namespace Shell Extension

--*/
#ifndef __XBSHLEXT_STDAFX_H__
#define __XBSHLEXT_STDAFX_H__

//-----------------------------------------------------------
//  We have some dependencies on Shell 5.0
//-----------------------------------------------------------
#define STRICT
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define _WIN32_WINNT 0x0500

//-----------------------------------------------------------
//  Standard ATL Stuff
//-----------------------------------------------------------
#define _ATL_APARTMENT_THREADED
//#define _ATL_DEBUG_INTERFACES
#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>
#include <assert.h>

//-----------------------------------------------------------
//  Shell Headers
//-----------------------------------------------------------
#include <shlobj.h>

//-----------------------------------------------------------
//  Some generic globals
//-----------------------------------------------------------//
extern IMalloc *g_pShellMalloc;
extern CLIPFORMAT CF_FILECONTENTS;
extern CLIPFORMAT CF_FILEDESCRIPTORA;
extern CLIPFORMAT CF_FILEDESCRIPTORW;
extern CLIPFORMAT CF_FILENAMEMAPA;
extern CLIPFORMAT CF_FILENAMEMAPW;
extern CLIPFORMAT CF_FILENAME;
extern CLIPFORMAT CF_PREFERREDDROPEFFECT;
extern CLIPFORMAT CF_PERFORMEDDDROPEFFECT;
extern CLIPFORMAT CF_LOGICALPERFORMEDDROPEFFECT;
extern CLIPFORMAT CF_PASTESUCCEEDED;
extern CLIPFORMAT CF_SHELLIDLIST;
extern CLIPFORMAT CF_XBOXFILEDESCRIPTOR;

//-------------------------------------------------------------
//  A useful macro that I saw used in some shell code.  Used
//  when calling QueryInterface and QueryService.  Adds a modicum
//  of type safety.
//-------------------------------------------------------------
#ifndef IID_PPV_ARG
#define IID_PPV_ARG(IType, ppType) IID_##IType, reinterpret_cast<void **>(static_cast<IType**>(ppType))
#endif

//-----------------------------------------------------------
//  Other misc. macros
//-----------------------------------------------------------
#define I2BIT(_index_) (1<<(_index_))  //Converts an index to a bit

//-----------------------------------------------------------
//  XDK Headers
//-----------------------------------------------------------
#include <xboxdbg.h>
#include <ixbconn.h>

//-----------------------------------------------------------
//  Private Shell headers
//-----------------------------------------------------------
#include <shlobj.h>
#include <ntquery.h>
#include <shlobjp.h>
#include <shldispp.h>
#include <shlguidp.h>

//-----------------------------------------------------------
// shell extension headers
//-----------------------------------------------------------
#include "resource.h"
#include "debug.h"
#include "xbshlext.h"       // MIDL Generated Header
#include "utils.h"          // Basic Utilities
#include "objectwithsite.h" // CShellObjectWithSite
#include "pidl.h"           // Pidl Utilities
#include "visit.h"          // Visiting Xbox Items (IXboxVisitor, IXboxVisit, etc.)
#include "xbfolder.h"       // CXboxFolder (and derivatives)
#include "console.h"        // CManageConsoles
#include "delete.h"         // CXboxDelete
#include "attrib.h"         // CGetAttributes, CSetAttributes
#include "icon.h"           // CXboxExtractIcon
#include "menu.h"           // CxboxMenu
#include "prop.h"           // Property Pages (all of them)
#include "wizard.h"         // Wizards (especially the add new console)
#include "dataobj.h"        // CXboxDataObject
#include "drop.h"           // CXboxDropTarget
#include "CXboxStream.h"    // CXboxStream
#include "view.h"           // CXboxViewCB

#endif // __XBSHLEXT_STDAFX_H__
