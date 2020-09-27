//-----------------------------------------------------------------------------
// Name: Tut04_Lights Xbox Graphics Tutorial
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The Lights tutorial shows simply how to perform lighting of vertices.


Required files and media
========================
   This tutorial requires no media to run.


Programming Notes
=================
   In the fixed pipeline, lights come in 3 flavors: directional, point, and
   spot. All lights are enumerated in one giant, internal lighting equation
   that also adds in an ambient component. Lights can have a color, a 
   position and/or direction, and other properties such as falloff and values
   for a spot light's cone. Vertices can also be colored with a material.

   Both materials and lights are setup with structures, and then set into
   place via a few calls to set the render states.
