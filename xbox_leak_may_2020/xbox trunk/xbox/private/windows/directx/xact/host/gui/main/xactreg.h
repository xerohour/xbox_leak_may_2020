/***************************************************************************
 *
 *  Copyright (C) 2/12/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       xactreg.h
 *  Content:    XACT GUI global registry definitions.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/12/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __XACTREG_H__
#define __XACTREG_H__

//
// The application root registry key
//

#define HKEY_XACT_APPLICATION_ROOT          HKEY_CURRENT_USER
#define REGKEY_XACT_APPLICATION_ROOT        TEXT("Software\\Microsoft\\XACT\\GUI")

//
// Window placement key/value pairs
//

#define REGKEY_MAINFRAME_PLACEMENT          NULL, TEXT("MainFramePlacement")
#define REGKEY_WORKSPACE_CHILD_PLACEMENT    NULL, TEXT("WorkspaceChildPlacement")
#define REGKEY_SPLITTER_POSITION            NULL, TEXT("MainSplitterPosition")

//
// MRU keys
//

#define REGKEY_PROJECT_MRU                  TEXT("Project MRU")
#define REGKEY_XBOX_MRU                     TEXT("Xbox MRU")

#ifdef __cplusplus

#endif // __cplusplus

#endif // __XACTREG_H__
