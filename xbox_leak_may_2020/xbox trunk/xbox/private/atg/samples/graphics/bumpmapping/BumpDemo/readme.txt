//-----------------------------------------------------------------------------
// Name: BumpDemo Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
  The BumpDemo sample shows a nice bumpmapping effect using the dot product
  reflect specular texture address op.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   To main things are needed for this sample. On the vertex side, each vertex
   has 3 basis vectors, which are generally needed for any per-pixel dot 
   product calculations. They are used to transform vectors into texture
   space. On the pixel side, this sample uses pixel shaders to perform the
   per-pixel calculations.
