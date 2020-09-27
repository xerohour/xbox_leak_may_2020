//-----------------------------------------------------------------------------
// Name: MatrixPaletteSkinning Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The MatrixPaletteSkinning sample demonstrates how to perform matrix 
palette skinning (also known as boned mesh deformation).  The bone positions
and weights are calculated procedurally by spacing the bones evenly along the
length of the mesh.  The bone transformations are calculated so as to move
the snake in a sine curve.  By increasing and decreasing the total number of 
bones used, you can observe the impact this has on the smoothness of the
animation.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The # of bones is limited to 30, because that is all the space available
in the positive vertex shader constants.  By using the negative vertex shader
constants, this number could be doubled.