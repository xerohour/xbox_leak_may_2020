#ifndef __XTEXTBOX_H
#define __XTEXTBOX_H

#include "XBFont.h"

// max textbox string length
#define TBOX_STRINGLEN 80

// textbox flags
#define TBOX_POPUP		0x01		// only active when text in box

typedef struct _XTextBoxItem
{
	DWORD val1, val2;				// user values
	WCHAR string[TBOX_STRINGLEN];
	DWORD expiretime;
	DWORD color;
} XTextBoxItem;

typedef struct _XTextBox
{
	float x, y, w, h;				// screen position & size
	DWORD flags;					// textbox flags
	DWORD textcolor;				// text color
	DWORD topcolor, bottomcolor;	// background color

	DWORD head, tail;
	DWORD maxshow;					// max lines to show
	DWORD maxitems;					// max lines to hold
	DWORD nitems;					// current number of items
	DWORD expiretime;				// time each line lives

	XTextBoxItem *items;			// text items
} XTextBox;

#ifdef __cplusplus
extern "C" {
#endif

void XTextBox_SetFont(CXBFont *font);
XTextBox *XTextBox_Init(float x, float y, float w, DWORD maxitems, DWORD flags);
void XTextBox_Shutdown();
XTextBoxItem *XTextBox_AddItem(XTextBox *xbox, WCHAR *string);
void XTextBox_Display(XTextBox *xbox);
void XTextBox_Expire(XTextBox *xbox);

#ifdef __cplusplus
}
#endif
#endif