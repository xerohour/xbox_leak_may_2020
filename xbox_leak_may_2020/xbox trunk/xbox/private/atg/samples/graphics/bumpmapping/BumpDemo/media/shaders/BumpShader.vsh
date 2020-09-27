;------------------------------------------------------------------------------
; Vertex shader to perform funky bumpmapping effect
;------------------------------------------------------------------------------
xvs.1.1

;------------------------------------------------------------------------------
; Vertex streams expected by this shader
;    struct VERTEX
;    {
;       D3DXVECTOR3 p;        // v0.xyz  = Vertex position
;       D3DXVECTOR3 n;        // v1.xyz  = Vertex normal
;       FLOAT       tu, tv;   // v2.xy   = Vertex tex coords
;    };
;
;    struct VERTEXBASIS
;    {
;       D3DXVECTOR3 S;        // v3.xyz  = Vertex S basis vector
;       D3DXVECTOR3 T;        // v4.xyz  = Vertex S basis vector
;       D3DXVECTOR3 SxT;      // v5.xyz  = Vertex SxT basis vector
;    };
;
; Expected vertex shaders constants
;    c0-c3    = Transpose of world matrix
;    c4-c7    = Transpose of view*projection matrix
;    c8       = Eye position in world space
;------------------------------------------------------------------------------

; Transform position by world matrix (store in r2)
dp4 r2.x, v0, c0
dp4 r2.y, v0, c1
dp4 r2.z, v0, c2
dp4 r2.w, v0, c3

; Copy base texture coords
mov oT0, v2

; Composite the world matrix and the transpose of the basis matrix
dp3 oT1.x, v3, c0    ; Store S basis vector in oT1.xyz
dp3 oT1.y, v4, c0
dp3 oT1.z, v5, c0
dp3 oT2.x, v3, c1    ; Store T basis vector in oT2.xyz
dp3 oT2.y, v4, c1
dp3 oT2.z, v5, c1
dp3 oT3.x, v3, c2    ; Store SxT basis vector in oT3.xyz
dp3 oT3.y, v4, c2
dp3 oT3.z, v5, c2

; Compute the vector to the eye position
add r0, c8, -r2

; Normalize eye vector (not necessary)
;dp3 r0.w, r0.xyz, r0.xyz
;rsq r1.x, r0.w
;mul r0, r0, r1.x

; Transform position by view*projection matrix.
dp4 oPos.x, r2, c4
dp4 oPos.y, r2, c5
dp4 oPos.z, r2, c6
dp4 oPos.w, r2, c7

mov	oT1.w, r0.x
mov	oT2.w, r0.y
mov	oT3.w, r0.z
