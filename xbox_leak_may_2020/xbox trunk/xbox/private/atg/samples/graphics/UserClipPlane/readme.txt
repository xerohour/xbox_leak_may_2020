//-----------------------------------------------------------------------------
// Name: UserClipPlane Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The UserClipPlane sample demonstrates how to use pixel shaders to implement
user clip planes.  Using one stage of the pixel shader, 4 different user clip
planes can be achieved.  The ClipPlane pixel shader stage operaters by 
discarding the fragment if any of the texture coordinate values is negative
(or optionally, non-negative).
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   To get the right values into the texture coordinates, a vertex shader is
used with a transform matrix to generate the texture coordinates.  
A plane can be defined by a normal n = (A, B, C) for the plane and a point 
on the plane p0 = (x0, y0, z0).  A point p1 = (x1, y1, z1) is on the plane 
iff (p1 - p0) dot n == 0.  If the result is negative instead of 0, then p1
is on the negative side of the plane.  If the result is positive, then p1
is on the positive side of the plane.  So by putting any point into this 
equation, we can determine which side of the plane it is on.  
We can rewrite the above equation as p1 dot n - p0 dot n == 0.  
Since we're operating on homogenous vectors, we can defined a vector L as
(A, B, C, -(p0 dot n) ).  Then L dot (x1, y1, z1, 1) gives our test above.
We can fit 4 such vectors into a matrix and transform p1 by this matrix to
perform 4 tests at once.  Unused rows of the matrix can be set to 0.
