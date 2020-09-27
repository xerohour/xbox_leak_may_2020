///////////////////////////////////////////////////////////////////////////////
//	UIMGEDIT.H
//
//	Created by :			Date :
//		EnriqueP				8/13/93
//
//	Description :
//		Declaration of the UIImgEdit class
//

#ifndef __UIMGEDIT_H__
#define __UIMGEDIT_H__

#include "uresedit.h"

#include "export.h"

#ifndef __UIRESEDIT_H__
	#error include 'uresedit.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// UIImgEdit class

// BEGIN_CLASS_HELP
// ClassName: UIImgEdit
// BaseClass: UIResEditor
// Category: Editors
// END_CLASS_HELP
class RES_CLASS UIImgEdit : public UIResEditor
{
	UIWND_COPY_CTOR(UIImgEdit, UIResEditor);

// Attributes
public:
	virtual BOOL IsValid(void) const;

// Utilities
public:
	void ClickInGraphTB(int nButton, int cX, int cY);
	void ClickImgTool(int nImgTool);
	void ClickColorMode( int nColor, int  nButton);
	void SetColor(int nColor, int nButton = VK_LBUTTON);
	HWND GetPane(int nPane);
	void ClickMouse(int nButton, int nPane, int cX, int cY, int nMag = 1);
	void DragMouse(int nButton, int nPane, int cX1, int cY1, int cX2, int cY2, int nMag = 1);
	HWND FontDialog(void);
	void SetFont(LPCSTR szFontName = NULL, int nSize = -1, int nStyle = -1);
	BOOL IsStandalone(void);
	virtual int GetEditorType(void);
};


// Accelerators specific to the image editor

#define ACC_ZOOM     		"m"
#define ACC_ZOOM_OUT        "+{<}"
#define ACC_ZOOM_IN         "+{>}"
#define ACC_BRUSH_BIGGER    "{+}"
#define ACC_BRUSH_SMALLER  	"{-}"
#define ACC_NEXT_FG_COLOR 	"{]}"
#define ACC_NEXT_BK_COLOR	"{}}"

//  Graphics Palette tool identifiers

#define GT_SELECT         0
#define GT_LASSO          1
#define GT_PICKER         2
#define GT_ERASER         3
#define GT_FILL           4
#define GT_ZOOM           5
#define GT_PENCIL         6
#define GT_BRUSH          7
#define GT_SPRAY          8
#define GT_LINE           9
#define GT_CURVE         10
#define GT_TEXT          11
#define GT_RECT          12
#define GT_COMBORECT     13
#define GT_FILLRECT      14
#define GT_ROUND         15
#define GT_COMBOROUND    16
#define GT_FILLROUND     17
#define GT_ELLIPSE       18
#define GT_COMBOELLIPSE   19
#define GT_FILLELLIPSE    20
#define GT_POLY           21
#define GT_COMBOPOLY      22
#define GT_FILLPOLY       23
#define GT_NUMTOOLS       24   //Total number of tools...used to init array


//  Graphics Palette Image Well identifiers

#define IW_ERASER_SIZE4          1
#define IW_ERASER_SIZE6          4
#define IW_ERASER_SIZE8          7
#define IW_ERASER_SIZE10        10
#define IW_SELECT_OPAQUE         4
#define IW_SELECT_TRANS          7
#define IW_SHAPE_ROUND_LG        0
#define IW_SHAPE_ROUND_MED       1
#define IW_SHAPE_ROUND_SM        2
#define IW_SHAPE_SQUARE_LG       3
#define IW_SHAPE_SQUARE_MED      4
#define IW_SHAPE_SQUARE_SM       5
#define IW_SHAPE_RLEAN_LG        6
#define IW_SHAPE_RLEAN_MED       7
#define IW_SHAPE_RLEAN_SM        8
#define IW_SHAPE_LLEAN_LG        9
#define IW_SHAPE_LLEAN_MED      10
#define IW_SHAPE_LLEAN_SM       11
#define IW_SPRAY_SM              0
#define IW_SPRAY_MED             2
#define IW_SPRAY_LG             10
#define IW_ZOOM_1x               1
#define IW_ZOOM_2x               4
#define IW_ZOOM_6x               7
#define IW_ZOOM_8x              10
#define IW_THICKNESS1            -1
#define IW_THICKNESS2            -4
#define IW_THICKNESS3            -7
#define IW_THICKNESS4           -10
#define IW_THICKNESS5           -13

enum { PANE_LEFT, PANE_RIGHT };

#endif //__UIMGEDIT_H__
