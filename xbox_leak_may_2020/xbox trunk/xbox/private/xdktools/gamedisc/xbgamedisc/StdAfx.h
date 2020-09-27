// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:      StdAfx.h
// Contents:  
// Revisions: 30-Oct-2001: Created (jeffsim)
//
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Not defined in our build environment
#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ INCLUDE FILES ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include <shlobj.h>
#include <windows.h>
#include <assert.h>
#include <commctrl.h>
#include <stdio.h>
#include <shlguid.h>  
#include <shellapi.h>
#include <xboxverp.h>
#include <shlobj.h>

#include "CUpdateBox.h"
#include "CInitedObject.h"
#include "CFile.h"
#include "xbGameDisc.h"
#include "NewOpenSave.h"
#include "PersistSettings.h"

#include "resource.h"
#include "Util.h"

#include "CFastToSlow.h"
#include "CMyListView.h"

#include "CFileHierarchy.h"

#include "CStatusBar.h"
#include "CUnplacedListView.h"
#include "CUnplacedWindow.h"
#include "CSplitter.h"
#include "CToolbarMgr.h"
#include "CTreeView.h"
#include "CExpListView.h"
#include "CLayerListView.h"

#include "CViewMode.h"
#include "CViewMode_Explorer.h"
#include "CViewMode_Layer.h"

#include "FileSystemChangeThread.h"
#include "CFileObject.h"

#include "About.h"

#include "Debug.h"

