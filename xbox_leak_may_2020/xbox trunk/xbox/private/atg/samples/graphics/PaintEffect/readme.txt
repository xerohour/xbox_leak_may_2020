//-----------------------------------------------------------------------------
// Name: PaintEffect Xbox Sample
// 
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The PaintEffect sample renders the scene to a small texture, then uses that
texture as a vertex buffer and draws the scene again as point sprites to
produce a painterly rendering.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   This sample takes advantage of the unified memory architecture of
the Xbox GPU, and "typecasts" a texture as a vertex buffer to pass
color data to the vertex shader.
