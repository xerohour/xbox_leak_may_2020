//-----------------------------------------------------------------------------
// Name: StencilMirror Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The StencilMirror sample uses stencil buffers to implement a mirror effect.
   In the sample, a watery terrain scene is rendered with the water reflecting
   a helicopter that flies above.

   Stencil buffers are a depth buffer technique that can be updated as
   geometry is rendered, and used again as a mask for drawing more geometry.
   Common effects include mirrors, shadows (an advanced technique), dissolves,
   etc..


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   In this sample, a stencil buffer is used to create the effect of a 
   reflection coming off the water. The geometry of the water is rendered into
   the stencil buffer. Then, the stencil buffer is used as a mask to render the
   scene again, this time with the geometry translated and rendered upside
   down, to appear as if it was reflected in the mirror.
