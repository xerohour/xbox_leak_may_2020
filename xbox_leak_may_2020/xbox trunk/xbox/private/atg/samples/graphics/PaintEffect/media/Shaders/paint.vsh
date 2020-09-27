;------------------------------------------------------------------------------
; Paint vertex shader
;
; Copyright (C) 2001 Microsoft Corporation
; All rights reserved.
;------------------------------------------------------------------------------
vs.1.0
#pragma screenspace
;------------------------------------------------------------------------------
; VERTEX STREAMS:
;	v0 = screen-space vertex
;	v1 = color
;	v2 = x: point size
;
; REGISTERS:
;	r0 = screen-space vertex
;	
; CONSTANTS:
;	c0 = x: point size scale   y: offset
;------------------------------------------------------------------------------

	; copy vertex->screen
	mov oPos, v0

	; copy color
	mov oD0, v1

	; scale and offset point size
	mad oPts.x, v2.x, c0.x, c0.y
