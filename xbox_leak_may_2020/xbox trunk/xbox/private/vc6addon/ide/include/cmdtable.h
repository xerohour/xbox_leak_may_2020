// cmdtable.h : Headers for writing command tables
//

#ifndef __CMDTABLE_H__
#define __CMDTABLE_H__ 

#undef AFX_DATA
#define AFX_DATA AFX_EXT_DATA

// Flags used for individual commands in command tables.
#define CT_NONE       	0x0000
#define CT_TOP        	0x0001
#define CT_NOKEY        0x0002
#define CT_NOMENU     	0x0004
#define CT_QUERYMENU  	0x0008	// Hides menuitem.
#define CT_HWND			0x0010
#define CT_HWNDSIZE		0x0020
#define CT_HWNDENABLE	0x0040
#define CT_REPEATABLE	0x0080
// Normally, a command can be added to a toolbar provided it doesn't have the
// CT_NOKEY bit set (which indicates that the command isn't customisable). This
// flag should only be used for commands which can be assigned to custom keystrokes,
// but shouldn't be assigned to buttons. Examples of this are keystrokes used to enter
// or affect toolbars.
#define CT_NOBUTTON		0x0100

// When this is present, the command will be shown textually if placed on a toolbar. Note that
// there is (by design) no combination of flags that puts text plus glyph on a toolbar. It
// would be trivial to add, but text+glyph is not being encouraged.
#define CT_TEXTBUTTON   0x0200

// When this is present, the command represents a pulldown menu. This is *not* the opposite
// of CT_NOMENU; in fact, the two flags can be used together. CT_NOMENU implies that the
// command won't be placed on a menu. CT_MENU implies that the command represents a (SUB)
// menu
#define CT_MENU         0x0400

// When this is present, the command is a placeholder for dynamic menu items. These items
// will be requested just before the menu is shown, and can be filled with, for example,
// a window list, MRU, etc.
#define CT_DYNAMIC      0x0800

// When this is present, the command string may be replaced dynamically at the
// package level. The package GetCommandStrings will be called, and it
// may change the string, or default to the command table entry (by returning NULL).
#define CT_DYNAMIC_CMD_STRING     0x1000

// When this is present, the command should be shown or hidden depending on projects
// currently loaded in the workspace.
#define CT_PROJECT_DEPENDENT	0x2000

// Not in CMDCOMP.EXE
#define CT_NOUI			0x8000

// Command string indices.
#define STRING_COMMAND		0
#define STRING_MENUTEXT		1
#define STRING_PROMPT		2
#define STRING_TIP			3
#define STRING_MAX_INDEX	3

/////////////////////////////////////////////////////////////////////////////
#undef AFX_DATA
#define AFX_DATA NEAR

#endif  // __CMDTABLE_H__
