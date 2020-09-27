;------------------------------------------------------------------------------
; Vertex shader to perform per-pixel lighting
;------------------------------------------------------------------------------
vs.1.0

;------------------------------------------------------------------------------
; Constants specified by the app
;    c4-c7   = matWorld * matView * matProj
;    c10     = vObjectSpaceDirLightDirection
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
m4x4 oPos, v0, c4


;----------------------------------------------------------------
; Output base texture coords
;----------------------------------------------------------------

; Pass thru the base tex coords
mov oT0, v2


;----------------------------------------------------------------
; Output lighting vectors
;----------------------------------------------------------------

; Directional light vector output in the diffuse component
; oD0 = vObjectSpaceDirLightDirection dot [ T B N ]
dp3 oD0.x, c10, v3
dp3 oD0.z, c10, v4
dp3 oD0.y, c10, v5

; vPtLight = vPtLightWorldPos - pVertex->p;
add r0.xyz, c11.xyz, -v0.xyz

; Point light vector output in the texture coord set 1
; oT1 = vPtLight dot [ T B N ]
dp3 oT1.x, r0, v3
dp3 oT1.z, r0, v4
dp3 oT1.y, r0, v5
