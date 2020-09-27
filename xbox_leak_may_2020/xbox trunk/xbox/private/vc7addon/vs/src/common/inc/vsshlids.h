//////////////////////////////////////////////////////////////////////////////
//
//Microsoft Confidential
//Copyright 1996-1997 Microsoft Corporation.  All Rights Reserved.
//
//File: VSShlIds.H
//
//Contents:
//
//////////////////////////////////////////////////////////////////////////////

#ifndef _VSSHLIDS_H_
#define _VSSHLIDS_H_


//////////////////////////////////////////////////////////////////////////////
//
// GUID Identifiers, created by Visual Studio Shell
//
//////////////////////////////////////////////////////////////////////////////
#ifndef NOGUIDS

#ifdef DEFINE_GUID
  // Guid if using Office provided icons
  DEFINE_GUID (guidOfficeIcon,
    0xd309f794, 0x903f, 0x11d0, 0x9e, 0xfc, 0x00, 0xa0, 0xc9, 0x11, 0x00, 0x4f);

  // Guid for the duplicate accelerator keys
  DEFINE_GUID (guidKeyDupe,
    0xf17bdae0, 0xa16d, 0x11d0, 0x9f, 0x4,  0x0,  0xa0, 0xc9, 0x11, 0x0,  0x4f);

  // Guid for Shell's group and menu ids
  DEFINE_GUID (guidSHLMainMenu,
    0xd309f791, 0x903f, 0x11d0, 0x9e, 0xfc, 0x00, 0xa0, 0xc9, 0x11, 0x00, 0x4f);

  // Guid for DocOutline package commands
  DEFINE_GUID (guidDocOutlinePkg,
	0x21af45b0, 0xffa5, 0x11d0, 0xb6, 0x3f, 0x00, 0xa0, 0xc9, 0x22, 0xe8, 0x51);

  // UIContext guid specifying that we're not in View Source mode
  DEFINE_GUID(guidNotViewSourceMode, 
	    0x7174c6a0, 0xb93d, 0x11d1, 0x9f, 0xf4, 0x0, 0xa0, 0xc9, 0x11, 0x0, 0x4f);

  // Guid for shared groups
  // {234A7FC1-CFE9-4335-9E82-061F86E402C1}
  DEFINE_GUID(guidSharedMenuGroup, 
    0x234a7fc1, 0xcfe9, 0x4335, 0x9e, 0x82, 0x6, 0x1f, 0x86, 0xe4, 0x2, 0xc1);

  DEFINE_GUID(guidBuildCmdIcons,
    0x952691c5, 0x34d6, 0x462b, 0xac, 0x56, 0x9a, 0xb0, 0x97, 0x70, 0xa3, 0x0d);

  DEFINE_GUID(CMDSETID_StandardCommandSet2K,
    0x1496A755, 0x94DE, 0x11D0, 0x8C, 0x3F, 0x00, 0xC0, 0x4F, 0xC2, 0xAA, 0xE2);

  // {501822E1-B5AF-11d0-B4DC-00A0C91506EF}
  DEFINE_GUID(guidDataCmdId,
	0x501822e1, 0xB5AF, 0x11D0, 0xB4, 0xDC, 0x00, 0xA0, 0xC9, 0x15, 0x06, 0xEF);

  //{732abe75-cd80-11d0-a2db-00aa00a3efff}
  DEFINE_GUID(CMDSETID_DaVinciDataToolsCommandSet,
	0x732abe75, 0xcd80, 0x11d0, 0xa2, 0xdb, 0x00, 0xaa, 0x00, 0xa3, 0xef, 0xff);

  // WM_APPCOMMAND handling
  // The active ole command targets will receive CMDSETID_WMAppCommand:cmdID, where
  // cmdID is one of APPCOMMAND_****, defined in winuser.h for _WIN32_WINNT >= 0x0500
  // (use common\inc\wmappcmd.h to have this commands defined for all target platforms)
  // If command is not handled, we will look in the registry for the mapped command:
  // HKLM\<appid hive>\WMAppCommand 
  //    val <AppCmdID> = {<guidCmdSet>}:<cmdID>

  // {12F1A339-02B9-46e6-BDAF-1071F76056BF}
  DEFINE_GUID(CMDSETID_WMAppCommand, 
        0x12f1a339, 0x02b9, 0x46e6, 0xbd, 0xaf, 0x10, 0x71, 0xf7, 0x60, 0x56, 0xbf);


#else //!DEFINE_GUID
  // Guid if using Office provided icons
  #define guidOfficeIcon	      { 0xd309f794, 0x903f, 0x11d0, { 0x9e, 0xfc, 0x00, 0xa0, 0xc9, 0x11, 0x00, 0x4f } }
  // Guid for the duplicate accelerator keys
  #define guidKeyDupe		      { 0xf17bdae0, 0xa16d, 0x11d0, { 0x9f, 0x4,  0x0,  0xa0, 0xc9, 0x11, 0x0,  0x4f } }
  // Guid for Shell's group and menu ids
  #define guidSHLMainMenu	      { 0xd309f791, 0x903f, 0x11d0, { 0x9e, 0xfc, 0x00, 0xa0, 0xc9, 0x11, 0x00, 0x4f } }
  // Guid for Standard Shell Commands (97 set)
  #define CMDSETID_StandardCommandSet97  { 0x5efc7975, 0x14bc, 0x11cf, { 0x9b, 0x2b, 0x00, 0xaa, 0x00, 0x57, 0x38, 0x19 } }
  // Guid for Standard Shell Commands (2k set)
  #define CMDSETID_StandardCommandSet2K {0x1496A755, 0x94DE, 0x11D0, {0x8C, 0x3F, 0x00, 0xC0, 0x4F, 0xC2, 0xAA, 0xE2}}
  // Guid for DocOutline package commands
  #define guidDocOutlinePkg { 0x21af45b0, 0xffa5, 0x11d0, { 0xb6, 0x3f, 0x00, 0xa0, 0xc9, 0x22, 0xe8, 0x51 } }
  // Guid for TaskList package commands
  #define CLSID_VsTaskListPackage	  { 0x4A9B7E50, 0xAA16, 0x11d0, { 0xA8, 0xC5, 0x00, 0xA0, 0xC9, 0x21, 0xA4, 0xD2 } }
  // Guid for find/replace bitmaps...
  #define guidFindIcon  { 0x740EEC10, 0x1A5D, 0x11D1, { 0xA0, 0x30, 0x00, 0xA0, 0xC9, 0x11, 0xE8, 0xE9} }
  // Guid for debugger bitmaps
  #define guidDebuggerIcon { 0xb7afe65e, 0x3a96, 0x11d1, { 0xb0, 0x68, 0x0, 0xc0, 0x4f, 0xb6, 0x6f, 0xa0} }
  // Guid for object browser buttons
  #define guidObjectBrowserButtons  { 0x5f810e80, 0x33ad, 0x11d1, { 0xa7, 0x96, 0x0, 0xa0, 0xc9, 0x11, 0x10, 0xc3 } }
  // UIContext guid specifying that we're not in View Source mode
  #define guidNotViewSourceMode  {0x7174c6a0, 0xb93d, 0x11d1, {0x9f, 0xf4, 0x0, 0xa0, 0xc9, 0x11, 0x0, 0x4f} }
  // Guid for text editor bitmaps...
  #define guidTextEditorIcon  { 0xc40a5a10, 0x3eeb, 0x11d3, { 0xaf, 0xe5, 0x0, 0x10, 0x5a, 0x99, 0x91, 0xef } }
  #define guidSharedMenuGroup { 0x234a7fc1, 0xcfe9, 0x4335, { 0x9e, 0x82, 0x6, 0x1f, 0x86, 0xe4, 0x02, 0xc1 } }
  // guid for build cmd icons
  #define guidBuildCmdIcons { 0x952691c5, 0x34d6, 0x462b, {0xac, 0x56, 0x9a, 0xb0, 0x97, 0x70, 0xa3, 0x0d}}
  // {501822E1-B5AF-11d0-B4DC-00A0C91506EF} Guid for Data project commands
  #define guidDataCmdId {0x501822e1, 0xb5af, 0x11d0, {0xb4, 0xdc, 0x00, 0xa0, 0xc9, 0x15, 0x06, 0xef}}
  //{732abe75-cd80-11d0-a2db-00aa00a3efff}
  #define CMDSETID_DaVinciDataToolsCommandSet	{0x732abe75, 0xcd80, 0x11d0, {0xa2, 0xdb, 0x00, 0xaa, 0x00, 0xa3, 0xef, 0xff} }
  // {12F1A339-02B9-46e6-BDAF-1071F76056BF}
  #define CMDSETID_WMAppCommand { 0x12f1a339, 0x02b9, 0x46e6, { 0xbd, 0xaf, 0x10, 0x71, 0xf7, 0x60, 0x56, 0xbf } }
 

#endif //!DEFINE_GUID

#ifdef __CTC__
// *** UIContext Guids for use by CTC parser only...
#define UICONTEXT_SolutionBuilding		{ 0xadfc4e60, 0x397, 0x11d1, { 0x9f, 0x4e, 0x0, 0xa0, 0xc9, 0x11, 0x0, 0x4f } }
#define UICONTEXT_Debugging			{ 0xadfc4e61, 0x397, 0x11d1, { 0x9f, 0x4e, 0x0, 0xa0, 0xc9, 0x11, 0x0, 0x4f } }
#define UICONTEXT_FullScreenMode		{ 0xadfc4e62, 0x397, 0x11d1, { 0x9f, 0x4e, 0x0, 0xa0, 0xc9, 0x11, 0x0, 0x4f } }
#define UICONTEXT_DesignMode			{ 0xadfc4e63, 0x397, 0x11d1, { 0x9f, 0x4e, 0x0, 0xa0, 0xc9, 0x11, 0x0, 0x4f } }
#define UICONTEXT_NoSolution			{ 0xadfc4e64, 0x397, 0x11d1, { 0x9f, 0x4e, 0x0, 0xa0, 0xc9, 0x11, 0x0, 0x4f } }
#define UICONTEXT_SolutionExists                { 0xf1536ef8, 0x92ec, 0x443c, { 0x9e, 0xd7, 0xfd, 0xad, 0xf1, 0x50, 0xda, 0x82 } };
#define UICONTEXT_EmptySolution			{ 0xadfc4e65, 0x397, 0x11d1, { 0x9f, 0x4e, 0x0, 0xa0, 0xc9, 0x11, 0x0, 0x4f } }
#define UICONTEXT_SolutionHasSingleProject	{ 0xadfc4e66, 0x397, 0x11d1, { 0x9f, 0x4e, 0x0, 0xa0, 0xc9, 0x11, 0x0, 0x4f } }
#define UICONTEXT_SolutionHasMultipleProjects 	{ 0x93694fa0, 0x397, 0x11d1, { 0x9f, 0x4e, 0x0, 0xa0, 0xc9, 0x11, 0x0, 0x4f } }
#define UICONTEXT_CodeWindow		{ 0x8fe2df1d, 0xe0da, 0x4ebe, { 0x9d, 0x5c, 0x41, 0x5d, 0x40, 0xe4, 0x87, 0xb5 } }
#endif //__CTC__

#define guidVSStd97					CMDSETID_StandardCommandSet97
#define CLSID_StandardCommandSet97  CMDSETID_StandardCommandSet97

#define guidVSStd2K					CMDSETID_StandardCommandSet2K
#define CLSID_StandardCommandSet2K  CMDSETID_StandardCommandSet2K
#define CLSID_CTextViewCommandGroup CMDSETID_StandardCommandSet2K
#define CLSID_TCG					CMDSETID_StandardCommandSet2K
#define CLSID_ECG					CMDSETID_StandardCommandSet2K
#define guidDavDataCmdId			CMDSETID_DaVinciDataToolsCommandSet

// Standard editor guid.
#define guidStdEditor   {0x9ADF33D0, 0x8AAD, 0x11d0, {0xB6, 0x06, 0x00, 0xA0, 0xC9, 0x22, 0xE8, 0x51} }

// Standard editor shorthand macros for a more compact and manageable table
#define guidStdEd       guidStdEditor
#define guidStdEdCmd    guidStdEditor:IDG_VS_EDITOR_CMDS
#define guidStdEdBmk    guidStdEditor:IDG_VS_EDITOR_BOOKMARK_CMDS
#define guidStdEdAdv    guidStdEditor:IDG_VS_EDITOR_ADVANCED_CMDS
#define guidStdEdOut    guidStdEditor:IDG_VS_EDITOR_OUTLINING_CMDS
#define guidStdLang     guidStdEditor:IDG_VS_EDITOR_LANGUAGE_INFO
#define guidStdEdIntel  guidStdEditor:IDG_VS_EDITOR_INTELLISENSE_CMDS
#endif //!NOGUIDS

//////////////////////////////////////////////////////////////////////////////
//
// Toolbar Identifiers, created by Visual Studio Shell
//
//////////////////////////////////////////////////////////////////////////////
#define IDM_VS_TOOL_MAINMENU	      0x0000
#define IDM_VS_TOOL_STANDARD	      0x0001
#define IDM_VS_TOOL_WINDOWUI	      0x0002
#define IDM_VS_TOOL_PROJWIN	      0x0003
#define IDM_VS_TOOL_FINDREPLACE       0x0004
#define IDM_VS_TOOL_FULLSCREEN	      0x0005
#define IDM_VS_TOOL_DEBUGGER	      0x0006
//UNUSED 0x0007-0x000a
#define IDM_VS_TOOL_OPENWINDOWS	      0x000b
#define IDM_VS_TOOL_VIEWBAR	      0x000c
#define IDM_VS_TOOL_BUILD             0x000d
#define IDM_VS_TOOL_TEXTEDITOR	      0x000e
#define IDM_VS_TOOL_OBJBROWSER        0x000f
#define IDM_VS_TOOL_CLASSVIEW         0x0010
#define IDM_VS_TOOL_PROPERTIES        0x0011
#define IDM_VS_TOOL_DATA              0x0012
#define IDM_VS_TOOL_SCHEMA            0x0013


//////////////////////////////////////////////////////////////////////////////
// Toolbar ID for customize mode only 
//
// **** NOTE ****  DO NOT add any menu or toolbar that has an ID greater than
//                 IDM_VS_TOOL_ADDCOMMAND, otherwise you WILL break customize
//                 mode. IDM_VS_TOOL_UDEFINED is a very special toolbar.
//                 Do not use or place it anywhere - used by shell only.
//////////////////////////////////////////////////////////////////////////////
#define IDM_VS_TOOL_UNDEFINED	      0xEDFF
#define IDM_VS_TOOL_ADDCOMMAND	      0xEE00

//////////////////////////////////////////////////////////////////////////////
//
// Menu Identifiers, created by Visual Studio Shell
//
//////////////////////////////////////////////////////////////////////////////
#define IDM_VS_MENU_FILE	      0x0080
#define IDM_VS_MENU_EDIT	      0x0081
#define IDM_VS_MENU_VIEW	      0x0082
#define IDM_VS_MENU_PROJECT	      0x0083
#define IDM_VS_MENU_TOOLS	      0x0085
#define IDM_VS_MENU_WINDOW	      0x0086
#define IDM_VS_MENU_ADDINS	      0x0087
#define IDM_VS_MENU_HELP	      0x0088
#define IDM_VS_MENU_DEBUG	      0x0089
#define IDM_VS_MENU_FORMAT            0x008A
#define IDM_VS_MENU_ALLMACROS         0x008B
#define IDM_VS_MENU_BUILD             0x008C

//////////////////////////////////////////////////////////////////////////////
//
// Group Identifiers, created by Visual Studio Shell
//
//////////////////////////////////////////////////////////////////////////////

// Main Menu Bar Groups
#define IDG_VS_MM_FILEEDITVIEW	      0x0101    // File/Edit/View menus go here
#define IDG_VS_MM_PROJECT	      0x0102    // Project menu go here
#define IDG_VS_MM_BUILDDEBUGRUN	      0x0103	// Build/Debug/Run menus go here
#define IDG_VS_MM_TOOLSADDINS	      0x0104	// Tools/Addins menu goes here
#define IDG_VS_MM_WINDOWHELP	      0x0105	// Window/Help menus go here

// All Macros Groups
#define IDG_VS_MM_MACROS              0x010A

// File Menu Groups
#define IDG_VS_FILE_NEW_PROJ_CSCD     0x010E
#define IDG_VS_FILE_ITEM              0x010F
#define IDG_VS_FILE_FILE              0x0110
#define IDG_VS_FILE_ADD               0x0111
#define IDG_VS_FILE_SAVE              0x0112
#define IDG_VS_FILE_RENAME            0x0113
#define IDG_VS_FILE_PRINT             0x0114
#define IDG_VS_FILE_MRU               0x0115
#define IDG_VS_FILE_EXIT              0x0116
#define IDG_VS_FILE_DELETE            0x0117
#define IDG_VS_FILE_SOLUTION          0x0118
#define IDG_VS_FILE_NEW_CASCADE       0x0119
#define IDG_VS_FILE_OPENP_CASCADE     0x011A
#define IDG_VS_FILE_OPENF_CASCADE     0x011B
#define IDG_VS_FILE_ADD_PROJECT       0x011C
#define IDG_VS_FILE_ADD_SLNITEM       0x011D
#define IDG_VS_FILE_FMRU_CASCADE      0x011E
#define IDG_VS_FILE_PMRU_CASCADE      0x011F
#define IDG_VS_FILE_BROWSER           0x0120
#define IDG_VS_FILE_MOVE              0x0121
#define IDG_VS_FILE_MOVE_CASCADE      0x0122
#define IDG_VS_FILE_MOVE_PICKER       0x0123
#define IDG_VS_FILE_MISC              0x0124
#define IDG_VS_FILE_MISC_CASCADE      0x0125

// Edit Menu Groups
#define IDG_VS_EDIT_OBJECTS           0x0128
#define IDG_VS_EDIT_UNDOREDO	      0x0129
#define IDG_VS_EDIT_CUTCOPY	      0x012A
#define IDG_VS_EDIT_SELECT	      0x012B
#define IDG_VS_EDIT_FIND	      0x012C
#define IDG_VS_EDIT_GOTO	      0x012D

// View Menu Groups
#define IDG_VS_VIEW_BROWSER	      0x0130
#define IDG_VS_VIEW_PROPPAGES	      0x0131
#define IDG_VS_VIEW_TOOLBARS	      0x0132
#define IDG_VS_VIEW_FORMCODE	      0x0133
#define IDG_VS_VIEW_DEFINEVIEWS	      0x0134
#define IDG_VS_VIEW_WINDOWS	      0x0135
#define IDG_VS_VIEW_REFRESH           0x0136
#define IDG_VS_VIEW_NAVIGATE          0x0137
#define IDG_VS_VIEW_SYMBOLNAVIGATE    0x0138
#define IDG_VS_VIEW_SMALLNAVIGATE     0x0139
#define IDG_VS_VIEW_OBJBRWSR          0x013A
#define IDG_VS_VIEW_LINKS             0x013B

// Project Menu Groups
#define IDG_VS_PROJ_ADD               0x0140
#define IDG_VS_PROJ_OPTIONS           0x0141
#define IDG_VS_PROJ_REFERENCE         0x0142
#define IDG_VS_PROJ_FOLDER            0x0143
#define IDG_VS_PROJ_UNLOADRELOAD      0x0144
#define IDG_VS_PROJ_ADDCODE           0x0145
#define IDG_VS_PROJ_PROJECT           0x0146
#define IDG_VS_PROJ_ADDREMOVE         0x0147
#define IDG_VS_PROJ_WEB1              0x0148
#define IDG_VS_PROJ_WEB2              0x0149
#define IDG_VS_PROJ_TOOLBAR1          0x014A
#define IDG_VS_PROJ_TOOLBAR2          0x014B
#define IDG_VS_PROJ_MISCADD           0x014C
#define IDG_VS_PROJ_SETTINGS          0x014D

// Run Menu Groups
#define IDG_VS_RUN_START	      0x0150
#define IDG_VS_DBG_STEP		      0x0151
#define IDG_VS_DBG_WATCH	      0x0152
#define IDG_VS_DBG_BRKPTS	      0x0153
#define IDG_VS_DBG_STATEMENT	      0x0154
#define IDG_VS_DBG_ATTACH	      0x0155
#define IDG_VS_DBG_TBBRKPTS	      0x0156
#define IDG_VS_DBG_DBGWINDOWS	      0x0157   // this actually resides on the debugger toolbar

//Tools->External Tools Groups
#define IDG_VS_TOOLS_EXT_CUST         0x0158
#define IDG_VS_TOOLS_EXT_TOOLS        0x0159

// Tools Menu Groups
#define IDG_VS_TOOLS_OPTIONS	      0x015A
#define IDG_VS_TOOLS_OTHER2	      0x015B
#define IDG_VS_TOOLS_OBJSUBSET        0x015C

// Addins Menu Groups
#define IDG_VS_ADDIN_BUILTIN	      0x015D
#define IDG_VS_ADDIN_MANAGER	      0x015E

// Window Menu Groups
#define IDG_VS_WINDOW_NEW	      0x0160
#define IDG_VS_WINDOW_ARRANGE	      0x0161
#define IDG_VS_WINDOW_LIST	      0x0162
#define IDG_VS_WINDOW_NAVIGATION      0x0163

// Help Menu Groups
#define IDG_VS_HELP_SUPPORT	      0x016A
#define IDG_VS_HELP_ABOUT 	      0x016B

// Standard Toolbar Groups
#define IDG_VS_TOOLSB_NEWADD	      0x0170
#define IDG_VS_TOOLSB_SAVEOPEN	      0x0171
#define IDG_VS_TOOLSB_CUTCOPY	      0x0172
#define IDG_VS_TOOLSB_UNDOREDO	      0x0173
#define IDG_VS_TOOLSB_RUNBUILD	      0x0174
#define IDG_VS_TOOLSB_WINDOWS	      0x0175  // don't use
#define IDG_VS_TOOLSB_GAUGE	      0x0176
#define IDG_VS_TOOLSB_SEARCH	      0x0177
#define IDG_VS_TOOLSB_NEWWINDOWS      0x0178
#define IDG_VS_TOOLSB_NAVIGATE        0x0179

// Window UI Toolbar Groups
#define IDG_VS_WINDOWUI_LOADSAVE      0x017A

// Open Windows Toolbar Groups
#define IDG_VS_OPENWIN_WINDOWS	      0x017B

// View Bar Toolbar Groups
#define IDG_VS_VIEWBAR_VIEWS	      0x017C

// Watch context menu groups
#define IDG_VS_WATCH_EDITADDDEL	      0x0180
//#define IDG_VS_WATCH_COLLAPSE	      0x0181
#define IDG_VS_WATCH_PROCDEFN	      0x0182
#define IDG_VS_WATCH_STARTEND	      0x0183

// Thread context menu groups
#define IDG_VS_THREAD_SUSPENDRESUME   0x0184

// Hexadecimal group
#define IDG_VS_DEBUG_DISPLAYRADIX     0x0185

// Treegrid context menu
#define IDG_VS_TREEGRID		      0x0186

// Immediate context menu groups
#define IDG_VS_IMMD_OBPROCDEFN	      0x0188

// Docking / Hide Pane Group
#define IDG_VS_DOCKHIDE		      0x0190

// Thread context menu groups
#define IDG_VS_CALLST_RUNTOCURSOR     0x0191

// MenuDesigner Context Menu Groups
#define IDG_VS_MNUDES_CUTCOPY	      0x0195
#define IDG_VS_MNUDES_INSERT	      0x0196
#define IDG_VS_MNUDES_EDITNAMES	      0x0197
#define IDG_VS_MNUDES_VIEWCODE	      0x0198
#define IDG_VS_MNUDES_PROPERTIES      0x0199

#define IDG_VS_MNUDES_UNDOREDO	      0x019A

// Window Menu Cascade groups
#define IDG_VS_WNDO_OTRWNDWS0         0x019E
#define IDG_VS_WNDO_OTRWNDWS1         0x019F
#define IDG_VS_WNDO_OTRWNDWS2         0x01A0
#define IDG_VS_WNDO_OTRWNDWS3         0x01A1
#define IDG_VS_WNDO_OTRWNDWS4         0x01A2
#define IDG_VS_WNDO_OTRWNDWS5         0x01A3
#define IDG_VS_WNDO_OTRWNDWS6         0x01A4
#define IDG_VS_WNDO_WINDOWS1	      0x01A5
#define IDG_VS_WNDO_WINDOWS2	      0x01A6
#define IDG_VS_WNDO_DBGWINDOWS	      IDG_VS_WNDO_WINDOWS1

// OLE Verbs Menu Cascade groups
#define IDG_VS_EDIT_OLEVERBS          0x01A8

// PropBrs Context menu groups
#define IDG_VS_PROPBRS_MISC	      0x01AA

// Output Window Pane Context menu groups
#define IDG_VS_OUTPUTPANECOPY	      0x01AC
#define IDG_VS_OUTPUTPANECLEAR	      0x01AD
#define IDG_VS_OUTPUTPANEGOTO	      0x01AE

// New Toolbox Context Menu groups   
#define IDG_VS_TOOLBOX_TBX	      0x01B0
#define IDG_VS_TOOLBOX_ITEM	      0x01B1
#define IDG_VS_TOOLBOX_TAB	      0x01B2
#define IDG_VS_TOOLBOX_MOVE	      0x01B3
#define IDG_VS_TOOLBOX_RENAME	      0x01B4

// Miscellaneous Files project context menu groups
#define IDG_VS_MISCFILES_PROJ         0x01B8

// Miscellaneous Files project item context menu groups
#define IDG_VS_MISCFILES_PROJITEM     0x01BA

// Solution Items project item context menu groups
#define IDG_VS_SOLNITEMS_PROJ         0x01BC
#define IDG_VS_SOLNITEMS_PROJITEM     0x01BD

// Stub (unloaded/placeholder) project context menu groups
#define IDG_VS_STUB_PROJECT           0x01BE

// Code Window context menu groups
#define IDG_VS_CODEWIN_TEXTEDIT		0x01C0
#define IDG_VS_CODEWIN_DEBUG_BP		0x01C1
#define IDG_VS_CODEWIN_DEBUG_WATCH	0x01C2
#define IDG_VS_CODEWIN_DEBUG_STEP	0x01C3
#define IDG_VS_CODEWIN_MARKER		0x01C4
#define IDG_VS_CODEWIN_OPENURL		0x01C5
#define IDG_VS_CODEWIN_SHORTCUT		0x01C6
#define IDG_VS_CODEWIN_INTELLISENSE     0x02B0
#define IDG_VS_CODEWIN_NAVIGATETOLOCATION         0x02B1
#define IDG_VS_CODEWIN_NAVIGATETOFILE 0x02B2
#define IDG_VS_CODEWIN_OUTLINING    0x02B3
#define IDG_VS_CODEWIN_CTXT_OUTLINING 0x02B4
// Task List context menu groups
#define IDG_VS_TASKLIST		      0x01C7

// cascading Task list menu groups
#define IDG_VS_TASKLIST_SORT		      0x01C8
#define IDG_VS_TASKLIST_NEXTPREV_ERR	  0x01C9
#define IDG_VS_TASKLIST_FILTER		      0x01CA
#define IDG_VS_TASKLIST_FILTER_CATEGORY	  0x01CB
#define IDG_VS_TASKLIST_FILTER_FILE	      0x01CC
#define IDG_VS_TASKLIST_FILTER_CHECKSTATE 0x01CD
#define IDG_VS_VIEWMENU_SHOWTASKS	      0x01CE

// Full screen menu group
#define IDG_VS_FULLSCREEN		0x01D0

// Full Screen toolbar group
#define IDG_VS_FULLSCREENBAR		0x01D1

// Build toolbar group
#define IDG_VS_BUILDBAR                 0x01D2

// Project Window Toolbar group
#define IDG_VS_PROJWIN1			0x01D5
#define IDG_VS_PROJWINEND		0x01D6

// Find and replace toolbar group
#define IDG_VS_FIND_NEW                 0x01DA
#define IDG_VS_FIND_OPTIONS             0x01DB
#define IDG_VS_FIND_OTHER               0x01DC

// Solution Node ctxt menu groups     
#define IDG_VS_SOLNNODE_CTXT_TOP	0x01E0
#define IDG_VS_SOLNNODE_CTXT_BOTTOM	0x01E1

// Project Window Default group
#define IDG_VS_PROJWIN_NODE_CTXT_TOP	0x01E2
#define IDG_VS_PROJWIN_NODE_CTXT_BOTTOM	0x01E3
#define IDG_VS_PROJWIN_ITEM_CTXT_TOP	0x01E4
#define IDG_VS_PROJWIN_ITEM_CTXT_BOTTOM	0x01E5

// Document Window Default groups	
#define IDG_VS_DOCWINDOW_CTXT_TOP	0x01E6
#define IDG_VS_DOCWINDOW_CTXT_BOTTOM	0x01E7

// Tool Window Default groups
#define IDG_VS_TOOLWINDOW_CTXT_TOP	0x01E8
#define IDG_VS_TOOLWINDOW_CTXT_BOTTOM	0x01E9

// EZ MDI groups
#define IDG_VS_EZ_TILE                  0x01EA
#define IDG_VS_EZ_CANCEL                0x01EB
#define IDG_VS_EZ_DOCWINDOWOPS          0x01EC

// Debugger Group
#define IDG_VS_TOOL_DEBUGGER	        0x0200

// Shell defined context menu groups
#define IDG_VS_CTXT_MULTIPROJ_BUILD     0x0201
#define IDG_VS_CTXT_PROJECT_ADD         0x0202
#define IDG_VS_CTXT_PROJECT_ADD_ITEMS   0x0203
#define IDG_VS_CTXT_PROJECT_DEBUG	0x0204
#define IDG_VS_CTXT_PROJECT_START	0x0205
#define IDG_VS_CTXT_PROJECT_BUILD	0x0206
#define IDG_VS_CTXT_PROJECT_TRANSFER	0x0207
#define IDG_VS_CTXT_ITEM_VIEWOBJECT	0x0208
#define IDG_VS_CTXT_ITEM_OPEN		0x0209
#define IDG_VS_CTXT_ITEM_TRANSFER	0x020A
#define IDG_VS_CTXT_ITEM_VIEWBROWSER	0x020B
#define IDG_VS_CTXT_SAVE		0x020C
#define IDG_VS_CTXT_ITEM_PRINT		0x020D
#define IDG_VS_CTXT_ITEM_PROPERTIES	0x020E
#define IDG_VS_CTXT_SCC			0x020F 
#define IDG_VS_CTXT_ITEM_RENAME		0x0210
#define IDG_VS_CTXT_PROJECT_RENAME	0x0211
#define IDG_VS_CTXT_SOLUTION_RENAME	0x0212
#define IDG_VS_CTXT_ITEM_SAVE		IDG_VS_CTXT_SAVE
#define IDG_VS_CTXT_PROJECT_SAVE	0x0213
#define IDG_VS_CTXT_PROJECT_PROPERTIES	0x0214
#define IDG_VS_CTXT_SOLUTION_PROPERTIES	0x0215
#define IDG_VS_CTXT_ITEM_SCC		IDG_VS_CTXT_SCC
#define IDG_VS_CTXT_PROJECT_SCC		0x0216
#define IDG_VS_CTXT_SOLUTION_SCC	0x0217

#define IDG_VS_CTXT_SOLUTION_SAVE	0x0218
#define IDG_VS_CTXT_SOLUTION_BUILD	0x0219
#define IDG_VS_UNUSED			0x021A  // unused group for hidden cmds
#define IDG_VS_CTXT_SOLUTION_START      0x021B
#define IDG_VS_CTXT_SOLUTION_TRANSFER   0x021C
#define IDG_VS_CTXT_SOLUTION_ADD_PROJ   0x021D
#define IDG_VS_CTXT_SOLUTION_ADD_ITEM   0x021E
#define IDG_VS_CTXT_SOLUTION_DEBUG      0x021F

#define IDG_VS_CTXT_DOCOUTLINE		0x0220
#define IDG_VS_CTXT_NOCOMMANDS          0x0221

#define IDG_VS_TOOLS_CMDLINE		0x0222

#define IDG_VS_CTXT_CMDWIN_MARK		0x0223

#define IDG_VS_CTXT_AUTOHIDE		0x0224

//External tools context menu groups
#define IDG_VS_EXTTOOLS_CURARGS         0x0225
#define IDG_VS_EXTTOOLS_PROJARGS        0x0226
#define IDG_VS_EXTTOOLS_SLNARGS         0x0227
#define IDG_VS_EXTTOOLS_CURDIRS         0x0228
#define IDG_VS_EXTTOOLS_PROJDIRS        0x0229
#define IDG_VS_EXTTOOLS_SLNDIRS         0x022A
#define IDG_VS_EXTTOOLS_TARGETARGS      0x022B
#define IDG_VS_EXTTOOLS_EDITORARGS      0x022C
#define IDG_VS_EXTTOOLS_TARGETDIRS      0x022D

#define IDG_VS_CTXT_ITEM_VIEW           0x022E
#define IDG_VS_CTXT_DELETE              0x022F
#define IDG_VS_CTXT_FOLDER_TRANSFER     0x0230
#define IDG_VS_CTXT_MULTISELECT_TRANSFER  0x0231
#define IDG_VS_CTXT_PROJECT_DEPS        0x0232
#define IDG_VS_CTXT_SOLUTION_ADD        0x0233
#define IDG_VS_CTXT_PROJECT_CONFIG      0x0234

// New File/Add New Item Open button drop-down menu
#define IDG_VS_OPENDROPDOWN_MENU        0x0235

// Object search menu groups
//RESERVED: 0x0235 - 0x0237
#define IDG_VS_OBJSEARCH_NAVIGATE     0x0238
#define IDG_VS_OBJSEARCH_EDIT         0x0239

// Context menu group for reloading an unloaded project
#define IDG_VS_CTXT_PROJECT_RELOAD    0x023A

// Classview menu groups
#define IDG_VS_CLASSVIEW_FOLDERS      0x0240  // Used in toolbar
#define IDG_VS_CLASSVIEW_FOLDERS2     0x0241  // Used in context menu
#define IDG_VS_CLASSVIEW_DISPLAY      0x0242
#define IDG_VS_CLASSVIEW_SEARCH       0x0243
#define IDG_VS_CLASSVIEW_EDIT         0x0244
#define IDG_VS_CLASSVIEW_NAVIGATION   0x0245
#define IDG_VS_CLASSVIEW_SHOWINFO     0x0247
#define IDG_VS_CLASSVIEW_PROJADD      0x0248
#define IDG_VS_CLASSVIEW_ITEMADD      0x0249
#define IDG_VS_CLASSVIEW_GROUPING     0x024a
#define IDG_VS_CLASSVIEW_PROJWIZARDS  0x024b
#define IDG_VS_CLASSVIEW_ITEMWIZARDS  0x024c
#define IDG_VS_CLASSVIEW_PROJADDITEMS 0x024d
#define IDG_VS_CLASSVIEW_FOLDERS_EDIT 0x024e

// Regular Expression Context menu groups
#define IDG_VS_FINDREGEXNORM0	      0x024f
#define IDG_VS_FINDREGEXNORM1	      0x0250
#define IDG_VS_FINDREGEXHELP	      0x0251
#define IDG_VS_REPLACEREGEXNORM	      0x0252
#define IDG_VS_REPLACEREGEXHELP	      0x0253
#define IDG_VS_FINDWILDNORM	      0x0254
#define IDG_VS_FINDWILDHELP	      0x0255
#define IDG_VS_REPLACEWILDNORM	      0x0256
#define IDG_VS_REPLACEWILDHELP	      0x0257
#define IDG_VS_FINDREGEXNORM2	      0x0258
#define IDG_VS_FINDREGEXNORM3	      0x0259

// Back/Forward
#define IDG_VS_CSCD_NAVIGATE	      0x0260

// Object Browser menu groups
#define IDG_VS_OBJBROWSER_SUBSETS     0x0270
#define IDG_VS_OBJBROWSER_DISPLAY     0x0271
#define IDG_VS_OBJBROWSER_DISPLAY2    0x0272
#define IDG_VS_OBJBROWSER_SEARCH      0x0273
#define IDG_VS_OBJBROWSER_SEARCH2     0x0274
#define IDG_VS_OBJBROWSER_NAVIGATION  0x0275
#define IDG_VS_OBJBROWSER_EDIT        0x0276
#define IDG_VS_OBJBROWSER_OBJGRP      0x0277
#define IDG_VS_OBJBROWSER_MEMGRP      0x0278
#define IDG_VS_OBJBROWSER_GROUPINGS   0x0279

// Build Menu groups
#define IDG_VS_BUILD_SOLUTION         0x0280
#define IDG_VS_BUILD_SELECTION        0x0281
#define IDG_VS_BUILD_MISC             0x0282
#define IDG_VS_BUILD_CANCEL           0x0283
#define IDG_VS_BUILD_CASCADE          0x0284
#define IDG_VS_REBUILD_CASCADE        0x0285
#define IDG_VS_CLEAN_CASCADE          0x0286
#define IDG_VS_DEPLOY_CASCADE         0x0287
#define IDG_VS_BUILD_PROJPICKER       0x0288
#define IDG_VS_REBUILD_PROJPICKER     0x0289
#define IDG_VS_CLEAN_PROJPICKER       0x0290
#define IDG_VS_DEPLOY_PROJPICKER      0x0291

#define IDG_VS_CTXT_CMDWIN_CUTCOPY    0x0292

// Additional Shell defined context menu groups
#define IDG_VS_CTXT_PROJECT_ADD_FORMS 0x02A0
#define IDG_VS_CTXT_PROJECT_ADD_MISC  0x02A1
#define IDG_VS_CTXT_ITEM_INCLUDEEXCLUDE 0x02A2
#define IDG_VS_CTXT_FOLDER_ADD        0x02A3
#define IDG_VS_CTXT_REFROOT_ADD       0x02A4
#define IDG_VS_CTXT_REFROOT_TRANSFER  0x02A5
#define IDG_VS_CTXT_WEBREFFOLDER_ADD  0x02A6
#define IDG_VS_CTXT_COMPILELINK       0x02A7

//0x02B0 used for Codewindow context menu
//0x02B1 used for Codewindow context menu
//0x02B2 used for Codewindow context menu

// Properties panel groups
#define IDG_VS_PROPERTIES_SORT        0x02BA
#define IDG_VS_PROPERTIES_PAGES       0x02BB

//////////////////////////////////////////////////////////////////////////////
//
// Groups for Menu Controllers
//
//////////////////////////////////////////////////////////////////////////////
#define IDG_VS_MNUCTRL_NEWITM                 0x02C0
#define IDG_VS_MNUCTRL_NEWITM_BOTTOM          0x02C1
#define IDG_VS_MNUCTRL_NEWPRJ                 0x02C2
#define IDG_VS_MNUCTRL_NEWPRJ_BOTTOM          0x02C3
#define IDG_VS_MNUCTRL_NAVBACK                0x02C4
#define IDG_VS_MNUCTRL_OBSEARCHOPTIONS        0x02C5

//////////////////////////////////////////////////////////////////////////////
//
// Cascading Menu Identifiers, created by Visual Studio Shell
//
//////////////////////////////////////////////////////////////////////////////
#define IDM_VS_CSCD_WINDOWS		      0x0300
#define IDM_VS_CSCD_TASKLIST_SORT	      0x0301
#define IDM_VS_CSCD_TASKLIST_FILTER	      0x0302
#define IDM_VS_CSCD_TASKLIST_VIEWMENU_FILTER  0x0303
#define IDM_VS_CSCD_DEBUGWINDOWS	      0x0304
#define IDM_VS_EDITOR_CSCD_OUTLINING_MENU 0x0305
#define IDM_VS_CSCD_COMMANDBARS               0x0306
#define IDM_VS_CSCD_OLEVERBS                  0x0307
#define IDM_VS_CSCD_NEW                       0x0308
#define IDM_VS_CSCD_OPEN                      0x0309
#define IDM_VS_CSCD_ADD                       0x030A
#define IDM_VS_CSCD_MNUDES                    0x030B
#define IDM_VS_CSCD_FILEMRU                   0x030C
#define IDM_VS_CSCD_PROJMRU                   0x030D
#define IDM_VS_CSCD_NEW_PROJ                  0x030E
#define IDM_VS_CSCD_MOVETOPRJ                 0x030F

#define IDM_VS_CSCD_NAVIGATE1		      0x0310
#define IDM_VS_CSCD_NAVIGATE2		      0x0311
#define IDM_VS_CSCD_NAVIGATE3		      0x0312
#define IDM_VS_CSCD_NAVIGATE4		      0x0313
#define IDM_VS_CSCD_NAVIGATE5		      0x0314
#define IDM_VS_CSCD_NAVIGATE6		      0x0315
#define IDM_VS_CSCD_NAVIGATE7		      0x0316
#define IDM_VS_CSCD_NAVIGATE8		      0x0317
#define IDM_VS_CSCD_NAVIGATE9		      0x0318
#define IDM_VS_CSCD_NAVIGATE10		      0x0319
#define IDM_VS_CSCD_NAVIGATE11		      0x031A
#define IDM_VS_CSCD_NAVIGATE12		      0x031B
#define IDM_VS_CSCD_NAVIGATE13		      0x031C
#define IDM_VS_CSCD_NAVIGATE14		      0x031D
#define IDM_VS_CSCD_NAVIGATE15		      0x031E
#define IDM_VS_CSCD_NAVIGATE16		      0x031F
#define IDM_VS_CSCD_NAVIGATE17		      0x0320
#define IDM_VS_CSCD_NAVIGATE18		      0x0321
#define IDM_VS_CSCD_NAVIGATE19		      0x0322
#define IDM_VS_CSCD_NAVIGATE20		      0x0323
#define IDM_VS_CSCD_NAVIGATE21		      0x0324
#define IDM_VS_CSCD_NAVIGATE22		      0x0325
#define IDM_VS_CSCD_NAVIGATE23		      0x0326
#define IDM_VS_CSCD_NAVIGATE24		      0x0327
#define IDM_VS_CSCD_NAVIGATE25		      0x0328
#define IDM_VS_CSCD_NAVIGATE26		      0x0329
#define IDM_VS_CSCD_NAVIGATE27		      0x032A
#define IDM_VS_CSCD_NAVIGATE28		      0x032B
#define IDM_VS_CSCD_NAVIGATE29		      0x032C
#define IDM_VS_CSCD_NAVIGATE30		      0x032D
#define IDM_VS_CSCD_NAVIGATE31		      0x032E
#define IDM_VS_CSCD_NAVIGATE32		      0x032F

#define IDM_VS_CSCD_BUILD                     0x0330
#define IDM_VS_CSCD_REBUILD                   0x0331
#define IDM_VS_CSCD_CLEAN                     0x0332
#define IDM_VS_CSCD_DEPLOY                    0x0333
#define IDM_VS_CSCD_MISCFILES                 0x0334

#define IDM_VS_CSCD_EXTTOOLS                  0x0340

#define IDM_VS_CSCD_SOLUTION_ADD              0x0350
#define IDM_VS_CSCD_SOLUTION_DEBUG            0x0351
#define IDM_VS_CSCD_PROJECT_ADD               0x0352
#define IDM_VS_CSCD_PROJECT_DEBUG             0x0353
// ClassView cascades
#define IDM_VS_CSCD_CV_PROJADD                0x0354
#define IDM_VS_CSCD_CV_ITEMADD                0x0355

//////////////////////////////////////////////////////////////////////////////
//
// Context Menu Identifiers, created by Visual Studio Shell
//
//////////////////////////////////////////////////////////////////////////////
#define IDM_VS_CTXT_PROJNODE	      0x0402
#define IDM_VS_CTXT_PROJWIN	      0x0403
#define IDM_VS_CTXT_PROJWINBREAK      0x0404
// UNUSED 0x0405 
#define IDM_VS_CTXT_DOCKEDWINDOW      0x0406
#define IDM_VS_CTXT_MENUDES	      0x0407
#define IDM_VS_CTXT_PROPBRS	      0x0408
#define IDM_VS_CTXT_TOOLBOXITEM	      0x0409
#define IDM_VS_CTXT_TOOLBOXTAB	      0x040A
#define IDM_VS_CTXT_CODEWIN	      0x040D
#define IDM_VS_CTXT_TASKLIST	      0x040E
#define IDM_VS_CTXT_OUTPUTPANE	      0x0411
#define IDM_VS_CTXT_STUBPROJECT	      0x0412
#define IDM_VS_CTXT_SOLNNODE          0x0413

// Slctn of one or more ProjNodes & SolnNode (doesn't involve ProjItem nodes)
#define IDM_VS_CTXT_XPROJ_SLNPROJ     0x0415
// Slctn of one or more ProjItems & SolnNode (min 1 ProjItem & may involve ProjNodes too)
#define IDM_VS_CTXT_XPROJ_SLNITEM     0x0416
// Selection of one more Project Nodes and one or more Project Items across projects (does not involve Solution Node) 
#define IDM_VS_CTXT_XPROJ_PROJITEM    0x0417
// Selection of two or more Project Nodes (does not involve the Solution Node or Project Item Nodes)
#define IDM_VS_CTXT_XPROJ_MULTIPROJ   0x0418
// Selection of one more Project Items across projects (does not involve Project Nodes or Solution Node)
#define IDM_VS_CTXT_XPROJ_MULTIITEM   0x0419

#define IDM_VS_CTXT_NOCOMMANDS        0x041A

// Miscellaneous Files project and item context menus
#define IDM_VS_CTXT_MISCFILESPROJ     0x041B
#define IDM_VS_CTXT_MISCFILESITEM     0x041C

// Solution Items project and item context menus
#define IDM_VS_CTXT_SOLNITEMSPROJ     0x041D
#define IDM_VS_CTXT_SOLNITEMSITEM     0x041E

// Command Window context menu
#define IDM_VS_CTXT_COMMANDWINDOW     0x041F

// AutoHide context menu on channel
#define IDM_VS_CTXT_AUTOHIDE	      0x0420

//0x0421-0x0423 unused

#define IDM_VS_CTXT_FIND_REGEX	      0x0424
#define IDM_VS_CTXT_REPLACE_REGEX     0x0425
#define IDM_VS_CTXT_FIND_WILD         0x0426
#define IDM_VS_CTXT_REPLACE_WILD      0x0427
#define IDM_VS_CTXT_EXTTOOLSARGS      0x0428
#define IDM_VS_CTXT_EXTTOOLSDIRS      0x0429

// EZMdi context menus
#define IDM_VS_CTXT_EZTOOLWINTAB      0x042A
#define IDM_VS_CTXT_EZDOCWINTAB       0x042B
#define IDM_VS_CTXT_EZDRAGGING        0x042C
#define IDM_VS_CTXT_EZCHANNEL         0x042D

// New File/Add New Item Open button drop-down menu
#define IDM_VS_CTXT_OPENDROPDOWN      0x042E

// Common Item Node context menu
#define IDM_VS_CTXT_ITEMNODE          0x0430

// Folder Node context menu
#define IDM_VS_CTXT_FOLDERNODE        0x0431

//////////////////////////////////////////////////////////////////////////////
// ClassView context menus
#define IDM_VS_CTXT_CV_PROJECT        0x0432
#define IDM_VS_CTXT_CV_ITEM           0x0433
#define IDM_VS_CTXT_CV_FOLDER         0x0434
#define IDM_VS_CTXT_CV_GROUPINGFOLDER 0x0435
#define IDM_VS_CTXT_CV_MULTIPLE       0x0436

// Object Browsing tools context menus
#define IDM_VS_CTXT_OBJBROWSER_OBJECTS 0x0437
#define IDM_VS_CTXT_OBJBROWSER_MEMBERS 0x0438
#define IDM_VS_CTXT_OBJBROWSER_DESC   0x0439
#define IDM_VS_CTXT_OBJSEARCH         0x0440

//////////////////////////////////////////////////////////////////////////////
// Reference context menus
// Reference Root Node context menu
#define IDM_VS_CTXT_REFERENCEROOT     0x0450
// Reference Item context menu
#define IDM_VS_CTXT_REFERENCE         0x0451
// Web Reference Folder context menu
#define IDM_VS_CTXT_WEBREFFOLDER      0x0452

//////////////////////////////////////////////////////////////////////////////
// Right drag menu group
#define IDM_VS_CTXT_RIGHT_DRAG        0x0460
#define IDG_VS_CTXT_RIGHT_DRAG1       0x0461
#define IDG_VS_CTXT_RIGHT_DRAG2       0x0462

//////////////////////////////////////////////////////////////////////////////
//
// Menu Controller dentifiers, created by Visual Studio Shell
//
//////////////////////////////////////////////////////////////////////////////
#define IDM_VS_MNUCTRL_NEWITM	      0x0500
#define IDM_VS_MNUCTRL_NEWPRJ	      0x0501
#define IDM_VS_MNUCTRL_OTRWNDWS	      0x0502
#define IDM_VS_MNUCTRL_NAVBACK        0x0503
#define IDM_VS_MNUCTRL_OBSEARCHOPTS   0x0504
#define IDM_VS_MNUCTRL_CVGROUPING     0x0505
#define IDM_VS_MNUCTRL_OBGRPOBJS      0x0506
#define IDM_VS_MNUCTRL_OBGRPMEMS      0x0507

// Text editor toolbar groups
#define IDG_VS_EDITTOOLBAR_COMPLETION 0x0550
#define IDG_VS_EDITTOOLBAR_INDENT     0x0551
#define IDG_VS_EDITTOOLBAR_COMMENT    0x0552
#define IDG_VS_EDITTOOLBAR_TEMPBOOKMARKS    0x0553

// Format Menu groups
#define IDG_VS_FORMAT_STYLE     0x0569
#define IDG_VS_FORMAT_COLOR     0x056A
#define IDG_VS_FORMAT_PARAGRAPH 0x056B
#define IDG_VS_FORMAT_INDENT    0x056C
#define IDG_VS_FORMAT_GRID      0x0554
#define IDG_VS_FORMAT_SPACE     0x0555
#define IDG_VS_FORMAT_CENTER    0x0556
#define IDG_VS_FORMAT_ORDER     0x0557
#define IDG_VS_FORMAT_ALIGN     0x0567
#define IDG_VS_FORMAT_LOCK      0x0558
#define IDG_VS_FORMAT_ELEMENT   0x056D
#define IDG_VS_FORMAT_STYLEORG  0x056E

// Format Align menu groups
#define IDG_VS_FORMAT_ALIGN_X     0x0559
#define IDG_VS_FORMAT_ALIGN_Y     0x055A
#define IDG_VS_FORMAT_ALIGN_GRID  0x055B

// Format Size menu groups
#define IDG_VS_FORMAT_SIZE        0x055C

// Format Space menu groups
#define IDG_VS_FORMAT_SPACE_X     0x055D
#define IDG_VS_FORMAT_SPACE_Y     0x055E

// Format Center menu groups
#define IDG_VS_FORMAT_CENTER_CMDS 0x055F

// Format Order menu groups
#define IDG_VS_FORMAT_ORDER_CMDS  0x0560

// Format cascaded menus
#define IDM_VS_CSCD_FORMAT_FONT      0x056F
#define IDM_VS_CSCD_FORMAT_JUSTIFY   0x0570
#define IDM_VS_CSCD_FORMAT_ALIGN     0x0561
#define IDM_VS_CSCD_FORMAT_SIZE      0x0562
#define IDM_VS_CSCD_FORMAT_SPACE_X   0x0563
#define IDM_VS_CSCD_FORMAT_SPACE_Y   0x0564
#define IDM_VS_CSCD_FORMAT_CENTER    0x0565
#define IDM_VS_CSCD_FORMAT_ORDER     0x0566

// View menu groups
#define IDG_VS_VIEW_TABORDER         0x0568
#define IDG_VS_VIEW_OPTIONS          0x0571


// 0x0569 used in IDG_VS_FORMAT_STYLE above
// 0x056A used in IDG_VS_FORMAT_COLOR above
// 0x056B used in IDG_VS_FORMAT_PARAGRAPH above
// 0x056C used in IDG_VS_FORMAT_INDENT above
// 0x056D used in IDG_VS_FORMAT_ELEMENT above
// 0x056E used in IDG_VS_FORMAT_STYLEORG above
// 0x056F used in IDM_VS_CSCD_FORMAT_FONT above
// 0x0570 used in IDM_VS_CSCD_FORMAT_JUSTIFY above
// 0x0571 used in IDG_VS_VIEW_OPTIONS above


// Format Paragraph menu groups
#define IDG_VS_FORMAT_FONTFACE       0x0572
#define IDG_VS_FORMAT_FONTSCRIPT     0x0573
#define IDG_VS_FORMAT_JUSTIFY        0x0574

// Table Menu
#define IDM_VS_TABLE_MENU            0x0575

// Table cascaded menus
#define IDM_VS_CSCD_TABLE_INSERT     0x0576
#define IDM_VS_CSCD_TABLE_DELETE     0x0577
#define IDM_VS_CSCD_TABLE_SELECT     0x0578

// Table menu groups
#define IDG_VS_TABLE_MAIN            0x0579
#define IDG_VS_TABLE_INSERT_1        0x057A
#define IDG_VS_TABLE_INSERT_2        0x057B
#define IDG_VS_TABLE_INSERT_3        0x057C
#define IDG_VS_TABLE_INSERT_4        0x057D
#define IDG_VS_TABLE_DELETE          0x057E
#define IDG_VS_TABLE_SELECT          0x057F

// Frame Set Menu
#define IDM_VS_FRAMESET_MENU         0x0580

// Frame Set Menu groups
#define IDG_VS_FRAME_WHOLE           0x0581
#define IDG_VS_FRAME_INDV            0x0582
#define IDG_VS_FRAME_NEW             0x0583

// Tools Menu groups
#define IDG_VS_TOOLS_EDITOPT         0x0584

// Insert Menu
#define IDM_VS_INSERT_MENU           0x0585

// Insert Menu groups
#define IDG_VS_INSERT_TAGS           0x0586
#define IDG_VS_INSERT_TAGS2          0x0587

// Script Block Menu
#define IDM_VS_SCRIPTBLOCK           0x0588

// Script Block Group
#define IDG_VS_SCRIPTBLOCK           0x0589

// Project cascaded menus
#define IDM_VS_CSCD_PROJECT_WEB      0x0600


///////////////////////////////////////////////
//
// Editor menu groups
//
///////////////////////////////////////////////
#define IDG_VS_EDITOR_CMDS           0x3E8A
#define IDG_VS_EDITOR_BOOKMARK_CMDS  0x3E8B
#define IDG_VS_EDITOR_ADVANCED_CMDS  0x3E8F
#define IDG_VS_EDITOR_OUTLINING_CMDS 0x3E90
#define IDG_VS_EDITOR_LANGUAGE_INFO  0x3E93
#define IDG_VS_EDITOR_INTELLISENSE_CMDS 0x3E94

#define IDM_VS_EDITOR_BOOKMARK_MENU  0x3E9E
#define IDM_VS_EDITOR_ADVANCED_MENU  0x3EA0
#define IDM_VS_EDITOR_OUTLINING_MENU 0x3EA1
#define IDM_VS_EDITOR_INTELLISENSE_MENU 0x3EA2
#define IDM_VS_EDITOR_FIND_MENU      0x3EA3
#endif // _VSSHLIDS_H_
