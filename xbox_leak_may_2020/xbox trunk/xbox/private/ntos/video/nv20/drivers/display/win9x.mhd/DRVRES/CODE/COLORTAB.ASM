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

        title   Control Panel Color Table
	%out	config
	page	,132


RGB     macro   R, G, B
        db      R,G,B,0
	endm



COLORTABLE  segment public

        dw  48                  ; # colors in table
        RGB 0FFh,080h,080h
        RGB 0FFh,0FFh,0E8h
        RGB 080h,0FFh,080h
        RGB 000h,0FFh,080h
        RGB 080h,0FFh,0FFh
        RGB 000h,080h,0FFh
        RGB 0FFh,080h,0C0h
        RGB 0FFh,080h,0FFh
        RGB 0FFh,000h,000h
        RGB 0FFh,0FFh,080h
        RGB 080h,0FFh,000h
        RGB 000h,0FFh,040h
        RGB 000h,0FFh,0FFh
        RGB 000h,080h,0C0h
        RGB 080h,080h,0C0h
        RGB 0FFh,000h,0FFh
        RGB 080h,040h,040h
        RGB 0FFh,0FFh,000h
;       RGB 0FFh,080h,040h
        RGB 000h,0FFh,000h
        RGB 000h,080h,080h
        RGB 000h,040h,080h
        RGB 080h,080h,0FFh
        RGB 080h,000h,040h
        RGB 0FFh,000h,080h
        RGB 080h,000h,000h
        RGB 0FFh,080h,000h
        RGB 000h,080h,000h
        RGB 000h,080h,040h
        RGB 000h,000h,0FFh
        RGB 000h,000h,0A0h
        RGB 080h,000h,080h
        RGB 080h,000h,0FFh
        RGB 040h,000h,000h
        RGB 080h,040h,000h
        RGB 000h,040h,000h
        RGB 000h,040h,040h
        RGB 000h,000h,080h
        RGB 000h,000h,040h
        RGB 040h,000h,040h
        RGB 040h,000h,080h
        RGB 000h,000h,000h
        RGB 080h,080h,000h
        RGB 080h,080h,040h
        RGB 080h,080h,080h
        RGB 040h,080h,080h
        RGB 0C0h,0C0h,0C0h
        RGB 082h,082h,082h
        RGB 0FFh,0FFh,0FFh
COLORTABLE     ends
end
