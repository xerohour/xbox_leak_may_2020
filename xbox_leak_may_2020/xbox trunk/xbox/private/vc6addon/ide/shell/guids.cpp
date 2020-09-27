// guids.cpp : add all header files that contain DEFINE_GUID in this
//				file.  this file is the only file that contains 
//				initguid
//
#include <stdafx.h>

#include <initguid.h>	// this enables definition of GUID's



#define INIT_MSO_GUIDS

#include <ObjModel\appguid.h>	// Shell Automation Guids
#include <utilauto.h>
#include "autoguid.h"			// Internal Shell Automation Guids
#include <aut1guid.h>
#include <aut1gui_.h>
#include <clvwguid.h>
#include <bldguid.h>
#include <cmguids.h>
#include <multinfo.h>
#include <ocdesign.h>
#include <oleipcid.h>
#include <objext.h>
#include <prjguid.h>
#include <srcguid.h>
#include <vbaguids.h>
#include <bldguid_.h>	// For IAutoBld
#include <ocdevo.h>
#include <shlguid_.h>
#include <oaidl.h>

#include "ipcits.h"

#include <iswsguid.h>				// IID_IWebPkgProject

// REVIEW(scotg)  this needs to be removed when we pick up a new
// docobj.h header that will include this guid.
DEFINE_GUID(SID_SContainerDispatch, 0xb722be00, 0x4e68, 0x101b, 0xa2, 0xbc,0x0, 0xaa, 0x0, 0x40, 0x47,0x70);
DEFINE_GUID(SID_SContainer,			0x0000011b, 0x0,    0x0,    0xc0, 0x0, 0x0, 0x0,  0x0, 0x0,  0x0, 0x46);

// REVIEW(billmc) these Guids are defined in ocx96.lib; once we have
// versions of that library for all packages, we can link to it
// and remove these definitions.
//
DEFINE_GUID(IID_IOleUndoManager,	0xd001f200, 0xef97, 0x11ce, 0x9b, 0xc9, 0x00, 0xaa, 0x00, 0x60, 0x8e, 0x01);
DEFINE_GUID(IID_IOleUndoUnit, 	0x894ad3b0, 0xef97, 0x11ce, 0x9b, 0xc9, 0x00, 0xaa, 0x00, 0x60, 0x8e, 0x01);
DEFINE_GUID(IID_IOleParentUndoUnit, 0xa1faf330, 0xef97, 0x11ce, 0x9b, 0xc9, 0x00, 0xaa, 0x00, 0x60, 0x8e, 0x01);
DEFINE_GUID(IID_IEnumOleUndoUnits,	0xb3e7c340, 0xef97, 0x11ce, 0x9b, 0xc9, 0x00, 0xaa, 0x00, 0x60, 0x8e, 0x01);

// This GUID is defined in fm20uuid.lib, which we don't want to use because there
// do not exist RISC builds and the IDE needs to be built for some RISC platforms.
// Also see ipcslob.cpp
DEFINE_GUID(IID_IElement,	0x3050f1ff, 0x98b5, 0x11cf, 0xbb, 0x82, 0x00, 0xaa, 0x00, 0xbd, 0xce, 0x0b);


// IID_IForm needed for ISBug: 7249 - defined in forms3.h which wont compile
// in the devstudio tree
DEFINE_GUID(IID_IForm, 0x04598fc8, 0x866c, 0x11cf, 0xab, 0x7c, 0x0, 0xaa, 0x0, 0xc0, 0x8f, 0xcf);

