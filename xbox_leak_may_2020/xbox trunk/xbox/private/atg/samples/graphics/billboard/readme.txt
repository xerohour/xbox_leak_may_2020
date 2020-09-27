//-----------------------------------------------------------------------------
// Name: Billboard Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The Billboard sample illustrates the billboarding technique. Rather than
   rendering complex 3D models (such as a high-polygon tree model),
   billboarding renders a 2D image of the model and rotates it to always face
   the eyepoint. This technique is commonly used to render trees, clouds,
   smoke, explosions, and more.

   The sample has a camera fly around a 3D scene with a tree-covered hill. The
   trees look like 3D objects, but they are actually 2-D billboarded images
   that are rotated towards the eye point. The hilly terrain and the skybox 
   (6-sided cube containing sky textures) are just objects loaded from .x
   files, used for visual effect, and are unrelated to the billboarding
   technique.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The billboarding technique is the focus of this sample. Each frame, the
   camera is moved, so the viewpoint changes accordingly. As the viewpoint
   changes, a rotation matrix is generated to rotate the billboards about
   the y-axis so that they face the new viewpoint. The computation of the 
   billboard matrix occurs in the FrameMove() function. The trees are also
   sorted in that function, as required for proper alpha blending, since 
   billboards typically have some transparent pixels.
   
   Note that the billboards in this sample are constrained to rotate about the 
   y-axis only, as otherwise the tree trunks would appear to not be fixed to 
   the ground. In a 3D flight sim or space shooter, for effects like 
   explosions, billboards are typically not constrained to one axis.
