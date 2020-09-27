//-----------------------------------------------------------------------------
// Name: Ripple Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The Ripple sample uses a vertex shader to create a ripple effect.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The vertex shader used by this sample actually modulates the positions of
   the vertices that it processes. The sine wave used to position the vertices
   is coded in the vertex shader using the first few elements of the Taylor-
   series expansion for the sine function.
