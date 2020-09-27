#ifndef __XMENU_H
#define __XMENU_H

#include "XBFont.h"
#include "XBInput.h"

// menu item flags
#define MITEM_SEPARATOR		0x0001			// this item is a separator
#define MITEM_ROUTINE		0x0002			// this item calls a routine
#define MITEM_DISABLED		0x0004			// this item is greyed out

// menu routine commands
#define MROUTINE_INIT		0xffff			// first call to routine
#define MROUTINE_ABORT		0xfffe			// abort routine triggered

// menu routine return values
#define MROUTINE_RETURN		0x0001			// return to menu
#define MROUTINE_DIE		0x0002			// kill routine
#define MROUTINE_SLEEP		0x0003			// routine sleeps

// menu item definition
#define MITEM_STRINGLEN 48
typedef struct _xmenuitem
{
	DWORD flags;
	WCHAR string[MITEM_STRINGLEN];			// text to display
	DWORD val1;								// user defined values
	DWORD val2;
	void *action;							// link to menu or routine
	struct _xmenu *menu;					// menu that owns this item
	DWORD color;							// item color
} XMenuItem;

// menu commands
#define MENU_NEXTITEM		0x0001			// next menu item
#define MENU_PREVITEM		0x0002			// previous menu item
#define MENU_SELECTITEM		0x0003			// select this menu item
#define MENU_BACK			0x0004			// go back one level
#define MENU_ACTIVATE		0x0005			// activate menu

// menu flags
#define MENU_LEFT			0x0001			// left justify menu text
#define MENU_RIGHT			0x0002			// right justify menu text
#define MENU_WRAP			0x0004			// wrap menu selection
#define MENU_NOBACK			0x0008			// cannot use back button

typedef struct _xmenu
{
	float x, y;								// screen position
	float w, h;								// menu size
	DWORD flags;							// menu flags
	DWORD topcolor, bottomcolor;			// menu background gradient color
	DWORD seltopcolor, selbotcolor;			// menu selection bar gradient color
	DWORD itemcolor;						// menu item default color
	struct _xmenu *parent;					// parent menu

	WCHAR title[MITEM_STRINGLEN];			// menu title
	DWORD titlecolor;						// memu title color
	DWORD maxshow;							// max # of items to show at one time
	DWORD maxitems;							// max # of items
	DWORD nitems;							// # of items
	DWORD curitem;							// current selected item
	XMenuItem *items;						// items

	DWORD (*abortroutine)(DWORD, XMenuItem *);	// called when menu aborted
} XMenu;

#ifdef __cplusplus
extern "C" {
#endif

extern XMenu *XMenu_CurMenu;
extern int (*XMenu_CurRoutine)(DWORD cmd, XMenuItem *mi);

void XMenu_SetFont(CXBFont *font);
XMenu *XMenu_Init(float x, float y, DWORD maxitems, DWORD flags, DWORD (*abortroutine)(DWORD, XMenuItem *));
void XMenu_Shutdown();
void XMenu_Delete(XMenu *m);
XMenuItem *XMenu_AddItem(XMenu *m, DWORD flags, WCHAR *string, void *action);
void XMenu_DeleteItem(XMenuItem *mi);
void XMenu_SetMaxShow(XMenu *m, DWORD maxshow);
void XMenu_SetTitle(XMenu *m, WCHAR *string, DWORD color);
void XMenu_SetItemText(XMenuItem *mi, WCHAR *string);
#define XMenu_IsActive() (XMenu_CurMenu||XMenu_CurRoutine)

DWORD XMenu_GetCommand(XBGAMEPAD *gamepad);	// gets command based on joypad
DWORD XMenu_Activate(XMenu *m);				// activates a menu
DWORD XMenu_Routine(DWORD command);			// called each frame to update menu

#ifdef __cplusplus
}
#endif
#endif