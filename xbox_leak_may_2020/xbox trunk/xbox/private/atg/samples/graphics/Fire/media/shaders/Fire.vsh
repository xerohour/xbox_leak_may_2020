;------------------------------------------------------------------------------
; Vertex shader to do a fire effect 
;------------------------------------------------------------------------------
vs.1.0

;------------------------------------------------------------------------------
; Constants specified by the app
;    c0      = ( 1.00, 0.35, 0, 0 )
;    c1      = ( 0, time, 0, 0 )
;    c4-c7   = matWorld
;    c8-c11  = matView
;    c12-c15 = matProj
;
; Vertex components (as specified in the vertex DECL)
;    v0       = Position
;    v6       = Texcoords
;------------------------------------------------------------------------------


;------------------------------------------------------------------------------
; Vertex transformation
;------------------------------------------------------------------------------

; Transform position
m4x4 oPos, v0, c4


;------------------------------------------------------------------------------
; Texture coordinates
;------------------------------------------------------------------------------

; Copy tex coords for fire texture
mov oT0, v6

; Gen tex coords for flame noise texture
mul r0, c0, v6
add oT1, c1, r0

; Copy tex coords for noise mask texture
mov oT2, v6

