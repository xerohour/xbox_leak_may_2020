;------------------------------------------------------------------------------
; Vertex shader for explosion quad
;------------------------------------------------------------------------------
vs.1.0

;------------------------------------------------------------------------------
;vertex streams
;v0 - verts
;v7 - texture coords
;
;inputs
;c[0,3] - world*view*proj matrix
;c4     - Stage 2 texture coords (0, ZScale, ZBias)
;c5     - Stage 3 texture coords (0, 0, 1)
;c6     - diffuse color
;c7     - Center of explosion in world coords
;c8     - Corrected center of quad in world coords
;c9     - D3DZ_MAX
;c[10,13] - view*proj matrix
;c[14,15] - 2D texture coordinate transform (2x2 matrix)
;c16    - (0.5, 0.5, 0.0, 0.0) translation for texture coord transform
;------------------------------------------------------------------------------

;Step 1: Transform the vertex by the World * View * Projection matrix
dp4		r4.x,v0,c0
dp4		r4.y,v0,c1
dp4		r4.z,v0,c2
dp4		r4.w,v0,c3

; Store a copy of the transformed position in R4
mov oPos,r4

;Step 2: Set up initial texture coords from constants & vertex stream
;For stages 0 and 1, we want the ability to rotate the texture, so 
;we subtract c16 from the texture coords to go from [0,1] to [-0.5,0.5]
;Then, apply our 2x2 texture coord transformation, and scale back to [0,1]
mov r1, c5.z
sub r0, v7, c16
dp4 r1.x, r0, c14
dp4 r1.y, r0, c15
add oT0, r1, c16
add oT1, r1, c16

;For stages 2 and 3, we just copy the constants over
mov oT2, c4
mov oT3, c5

;Propagate the color
mov oD0, c6

;Step 3: Calculate stage 2 texture coords
;We need to calculate the Scale factor for T2, because the lookup from the
;depth texture will get transformed to [0, 1].  The second coordinate is used
;to scale those depth values out to the appropriate values in our depth
;buffer.  We calculate this by taking the world coordinates of the center of
;the explosion (c7) and the world coordinates of the near edge of the
;explosion (c8), and transforming them by our view and projection transforms
;(c10-13).  We then project the resultant vectors back into w=1 before
;calculating the distance between them.  Finally, we take that value and
;multiply by D3DZ_MAX for the appropriate depth buffer format, so that we
;end up with values in the appropriate range 

;Transform and project center of explosion (c7)
mov r5,c7
dp4 r0.x,r5,c10
dp4 r0.y,r5,c11
dp4 r0.z,r5,c12
dp4 r0.w,r5,c13
rcp r2,r0.w
mul r0,r0,r2
mov r7,r0

;Transform and project near edge of explosion (c8)
mov r5,c8
dp4 r1.x,r5,c10
dp4 r1.y,r5,c11
dp4 r1.z,r5,c12
dp4 r1.w,r5,c13
rcp r2,r1.w
mul r1,r1,r2
mov r8,r1

;subtract vectors and calculate length into r1
sub r0,r0,r1
dp3 r2,r0,r0
rsq r1,r2
rcp r1,r1

;multiply length by D3DZ_MAX for scale
mul oT2.y,r1,c9

;Calculate transformed Z - this is the Zbias component of 
;oT2, which represents the depth of the near edge of the
;explosion, in the depth buffer format.  We take the transformed
;vertex from above, project into w=1, and scale by D3DZ_MAX

rcp r1,r4.w
mul r1,r1,r4.z
mul oT2.z,r1,c9

