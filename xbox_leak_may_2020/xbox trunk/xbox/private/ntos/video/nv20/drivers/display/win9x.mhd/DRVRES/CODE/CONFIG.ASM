;****************************************************************************
;                                                                           *
; THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY     *
; KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE       *
; IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR     *
; PURPOSE.                                                                  *
;                                                                           *
; Copyright (C) 1993-95  Microsoft Corporation.  All Rights Reserved.       *
;                                                                           *
;****************************************************************************

	title	Hardware Dependent Parameters
	%out	config
	page	,132


RGB     macro   R, G, B
        db      R,G,B,0
	endm



OEM	segment public

;	Machine dependent parameters

        dw      17                      ;Height of vertical thumb
        dw      17                      ;Width of horizontal thumb
	dw	2			;Icon horiz compression factor
	dw	2			;Icon vert compression factor
; Notes on the cursor compression factors:
;       Cursor size = int(32 / compression factor)
;       i.e. 1 = 32 pixels, 2 = 16, 3 = 10, 4 = 8, 5 = 6, 6 = 5, 7 = 4, 8 = 4, ... 32 = 1
;       DO NOT USE 0 - Windows98 will shut down immediately (divide by 0 probably...)
;       64 will give you a single 32x32 cursor
;       -1 gives NO cursor - always calls SetCursor with NULL
	dw	1			;Cursor horz compression factor
	dw	1			;Cursor vert compression factor
	dw	0			;Kanji window height
	dw	1			;cxBorder (thickness of vertical lines)
	dw	1			;cyBorder (thickness of horizontal lines)

;	Default system color values

        RGB 192,192,192     ;clrScrollbar
        RGB 192,192,192     ;clrDesktop
        RGB 000,000,128     ;clrActiveCaption
        RGB 255,255,255     ;clrInactiveCaption
        RGB 255,255,255     ;clrMenu
        RGB 255,255,255     ;clrWindow
        RGB 000,000,000     ;clrWindowFrame
        RGB 000,000,000     ;clrMenuText
        RGB 000,000,000     ;clrWindowText
        RGB 255,255,255     ;clrCaptionText
        RGB 192,192,192     ;clrActiveBorder
        RGB 192,192,192     ;clrInactiveBorder
        RGB 255,255,255     ;clrAppWorkspace
        RGB 000,000,128     ;clrHiliteBk
        RGB 255,255,255     ;clrHiliteText
        RGB 192,192,192     ;clrBtnFace
        RGB 128,128,128     ;clrBtnShadow
        RGB 192,192,192     ;clrGrayText
        RGB 000,000,000     ;clrBtnText


;	dw	0			;Unused words
;	dw	0
;	dw	0
;	dw	0
;	dw	0
;	dw	0
	dw	0
	dw	0

OEM	ends
end
