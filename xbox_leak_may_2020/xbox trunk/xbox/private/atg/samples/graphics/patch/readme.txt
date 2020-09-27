//-----------------------------------------------------------------------------
// Name: Patch Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The Patch sample illustrates using N-patches in Direct3D. A low-res teapot
   mesh is created and displayed on the screen. The use can up the resolution
   by using N-patches.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The N-patches technique is the focus of this sample. The Direct3D API is
   used to turn Bezier input data into a higher-order mesh. The mesh is render
   with a series of calls to DrawRectPatch().
