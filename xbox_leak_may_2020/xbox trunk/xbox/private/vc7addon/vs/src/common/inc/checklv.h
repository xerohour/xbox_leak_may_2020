//=----------------------------------------------------------------------=
// CheckLV.H
//=----------------------------------------------------------------------=
// Checked list view header.  This code primarially supports checked
// list views, but can also be used for a variety of other additional
// styles.
//
//=----------------------------------------------------------------------=
// Copyright 1997 Microsoft Corporation, all rights reserved.
// Information Contained Herein Is Proprietary and Confidential.
// 
//

#ifndef _INC_CHECKLV_H
#define _INC_CHECKLV_H

// ListView checkmarks are handled through the state member of LV_ITEM.
// So, you can use these macros to find out if something is checked or
// not.
//
#define LV_CHECKED   0x2000
#define LV_UNCHECKED 0x1000
#define ListView_IsStateChecked(state) (LV_CHECKED == ((state) & LVIS_STATEIMAGEMASK))

// Flags that can be passed to ListView_ExtendStyle:
//
#define LVES_AUTOSORT  0x0001 // automatically sorts columns.  You must have a unique lParam on
                              // your items.

#define LVES_CHECKED   0x0002 // Creates a checked list view.

#define LVES_FULLROW   0x0004 // Highlites the full row, not just the first column.

//=----------------------------------------------------------------------=
// Extends the list view in ways it never knew were possible
//
HRESULT ListView_ExtendStyle(HWND hwndListView, DWORD dwFlags);

//=----------------------------------------------------------------------=
// Is the given item checked?
//
BOOL ListView_IsItemChecked(HWND hwndListView, int nItem);

//=----------------------------------------------------------------------=
// Check/uncheck an item
//
void ListView_CheckItem(HWND hwndListView, int nItem, BOOL fChecked);

//=----------------------------------------------------------------------=
// Adding a checked item
//
int ListView_InsertCheckItem(HWND hwndListView, LV_ITEMA *pli, BOOL fChecked);
int ListView_InsertCheckItemW(HWND hwndListView, LV_ITEMW *pli, BOOL fChecked);

//=----------------------------------------------------------------------=
// Utility code to draw a check box.  Use this for all of your checkbox
// drawing needs!
//

// Draws a normal unchecked flat check box
#define DRAWCHECK_NORMAL        0

// Draws a normal checked flat check box
#define DRAWCHECK_CHECKED       1

// Draws a greyed out unchecked flat check box.  This is not normally used
#define DRAWCHECK_GREYED        2

// Draws a greyed out checked flat check box.  This is typically used to show
// an "indeterminate" check box.
#define DRAWCHECK_INDETERMINATE DRAWCHECK_CHECKED | DRAWCHECK_GREYED

// The typical recommended check box dimensions
#define DRAWCHECK_WIDTH  11
#define DRAWCHECK_HEIGHT 11

//=----------------------------------------------------------------------=
// Draws a check box.  This method is used to draw the the same check box
// as used by the vs components that provide the "customize toolbox" page.
// 
void DrawCheck
(
  HDC   hdc,        // hdc to draw the checkmark to
  int   nx,         // x position of upper left corner of check
  int   ny,         // y position of upper left corner of check
  int   nWidth,     // width of check, use DRAWCHECK_WIDTH for standard width
  int   nHeight,    // height of check, use DRAWCHECK_HEIGHT for standard height
  DWORD dwStyle     // style.  Must be one of the four styles above.
);

#endif // _INC_CHECKLV_H
