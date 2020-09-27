;The first two instructions will pair if all instances of r1 are
;replaced with r2. The renamer would do this, but isn't executed because
;there are no ILU instructions here.

xvs.1.1
#pragma screenspace
mov oPos, c0
mul r1, v0, v0
mul oD0, r1, v1
