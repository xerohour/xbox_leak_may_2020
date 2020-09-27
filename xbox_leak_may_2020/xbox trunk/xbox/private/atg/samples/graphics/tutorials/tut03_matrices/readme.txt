//-----------------------------------------------------------------------------
// Name: Tut03_Matrices Xbox Graphics Tutorial
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The Matrices tutorial shows simply how to transform vertices using matrices.


Required files and media
========================
   This tutorial requires no media to run.


Programming Notes
=================
   In the fixed pipeline, vertices are transformed first with the world
   transform, then the view transform, and finally, the projection
   transform. The resulting vertex position is in 2-D viewport
   coordinates. Each transform is a 4x4 matrix and is set simply as a
   render state.

