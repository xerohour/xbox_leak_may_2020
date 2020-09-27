//-----------------------------------------------------------------------------
// Name: VertexBlend Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The VertexBlend sample demonstrates a technique called vertex blending (also
   known as surface skinning). It displays a file-based object which is made to
   bend is various spots.

   Surface skinning is a technique used for things like smooth joints and
   bulging muscles in character animations.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Vertex blending requires each vertex to have an associated blend weight. 
   Multiple world transforms are set up using SetTransformState() and the
   blend weights determine how much contribution each world matrix has when
   positioning each vertex.

   In this sample, a mesh is loaded using the common helper code. What is
   important is how a custom vertex and a custom FVF is declared and used
   to build the mesh (see the SetFVF() call for the mesh object). Without
   using the mesh helper code, the technique is the same: just create a
   vertex buffer full of vertices that have a blend weight, and use the
   appropriate FVF.
