//-----------------------------------------------------------------------------
// Name: ZSprite Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The ZSprite sample demonstrates how to render a sprite with depth
information.  This is done by having one texture contain the color and alpha
data for the sprite (a normal 2d texture), and having another texture contain
the depth information.  
The z value will be determined by the division of two dot products.  The 
numerator is a set of texture coordinates dotted with the vector 
< Ztex, Wtex, 1.0 >, and the denominator is a second set of texture coordinates
dotted with the same vector < Ztex, Wtex, 1.0 >.  Ztex is the upper 16 bits resulting
from the texture lookup, and Wtex is the lower 16 bits, both scaled from 0.0 to
1.0.  This texture coordinates used for the dot product are < 1.0, 0.0, 0.0 > for
the numerator and < 0.0, 0.0, 1/D3DZ_MAX_D24S8 >.  The division then scales the Ztex 
value to 24 bits for a 24-bit depth buffer.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================

