;******************************************************************************
;
;   (C) Copyright MICROSOFT Corp., 1989-1990
;
;***************
; Shell_Event definitions for the VDD
;
;   DO NOT CHANGE THESE EVER WITHOUT REDOING THE SHELL AND THE GRABBER
;	AND THE VDD!!!
;
VDA_Type_Chng		EQU	((0400h+20)+2)

VDA_Display_Event	EQU	((0400h+20)+6)
; On VDA_Display_Event event,
;   VMDOSAPP instance which gets the message has display event
;   wParam is a "subfunction code" which identifies the event.
;
VDA_DE_DispChngMin	EQU	0
VDA_DE_DispChngMax	EQU	80
VDA_DE_ControlChngMin	EQU	VDA_DE_DispChngMax
VDA_DE_ControlChngMax	EQU	160
VDA_DE_ScreenGrabMin	EQU	VDA_DE_ControlChngMax
VDA_DE_ScreenGrabMax	EQU	240

VDA_DE_SpecialMin	EQU	0FFAFh
VDA_DE_SpecialMax	EQU	0FFFFh
;
; Subfunctions >= VDA_DE_DispChngMin, < VDA_DE_DispChngMax indicate
;   that the display has changed without a change to the video controller
;   state.
;
; Subfunctions >= VDA_DE_ControlChngMin, < VDA_DE_ControlChngMax indicate
;   that the video controller state has changed.
;
; Subfunctions >= VDA_DE_ScreenGrabMin, < VDA_DE_ScreenGrabMax indicate
;   that a screen grab event has occured.
;
; Subfunctions >= VDA_DE_SpecialMin, < VDA_DE_SpecialMax indicate
;   private GRABBER events which VMDOSAPP just passes through to the
;   GRABBER
;
; This parameter is passed through VMDOSAPP to the display specific GRABBER.
;   "ranges" on the parameter allow specific info to be passed to the GRABBER
;   while allowing VMDOSAPP to know about the general high level aspect of the
;   event.
;
