//-----------------------------------------------------------------------------
// Name: Tut06_Meshes Xbox Graphics Tutorial
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The Meshes tutorial shows how to load a geometry file and render it.


Required files and media
========================
   Copy all required media to the target machine before running this tutorial.


Programming Notes
=================
   Loading a geometry file can be a complex process. Many apps use custom file
   formats and correspondingly have custom file loading code. This app shows
   how to use the D3DX functions to load .x files from the disk, and use the
   resulting mesh object to render the geometry. Unfortunately, .x files do
   not map very well to the Xbox graphics architecture so loading .x files is 
   slow and internally requires a decent amount of code, but this sample is a
   start.
