//-----------------------------------------------------------------------------
// Name: Tut07_VertexShaders Xbox Graphics Tutorial
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The VertexShaders tutorial shows how to use vertex shaders in D3D.


Required files and media
========================
   Copy all required media to the target machine before running this tutorial.


Programming Notes
=================
   Vertex shaders are used in place of the fixed-pipeline to transform and light
   vertices. Vertex shaders are written in a special form of assembly language
   and compiled into microcode that is passed to the hardware. During the vertex
   shader creation step, the app must also pass in a vertex declaration, which
   tells the shader how the vertices are created. This replaces the need for
   an FvF code (a flexible vertex format code used by the fixed pipeline).
   Once vertex shaders are in place, an app can send information to the shader
   via the vertices themselves, or through a set of vertex shaders constants. 
