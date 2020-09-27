;------------------------------------------------------------------------------
; Vertex shader to perform per-pixel lighting
;------------------------------------------------------------------------------
vs.1.0

;------------------------------------------------------------------------------
; Constants specified by the app
;    c4-c7   = matWorld * matView * matProj
;    c11     = vObjectSpacePtLightPosition
;
; Vertex components (as specified in the vertex DECL)
;    v0  = pVertex[i].p
;    v1  = pVertex[i].n
;    v2  = pVertex[i].t0
;    v3  = pVertex[i].basis.vTangent;
;    v4  = pVertex[i].basis.vBinormal;
;    v5  = pVertex[i].basis.vNormal;
;------------------------------------------------------------------------------

;------------------------------------------------------------------------------
; Vertex transformation
;------------------------------------------------------------------------------

; Transform position for world, view, and projection matrices
m4x4 oPos, v0, c40


;----------------------------------------------------------------
; Output texture coords
;----------------------------------------------------------------

; Pass thru the base tex coords
mov oT0, v2

; Point eye vector output in the texture coord set 1
; vPtEye = vPtEyeWorldPos - pVertex->p;
; oT1    = vPtEye dot [ T B N ]
add r0.xyz, c11.xyz, -v0.xyz
dp3 oT1.x, r0, v3
dp3 oT1.z, r0, v4
dp3 oT1.y, r0, v5

; Point light vector output in the texture coord set 2
; vPtLight = vPtLightWorldPos - pVertex->p;
; oT2      = vPtLight dot [ T B N ]
add r0.xyz, c12.xyz, -v0.xyz
dp3 oT2.x, r0, v3
dp3 oT2.z, r0, v4
dp3 oT2.y, r0, v5

