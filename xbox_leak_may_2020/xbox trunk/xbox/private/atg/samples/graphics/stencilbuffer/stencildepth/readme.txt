//-----------------------------------------------------------------------------
// Name: StencilDepth Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The StencilDepth sample uses stencil buffers to display the depth complexity
   of a scene. The depth complextity of a scene is defined as the average
   number of times each pixel is rendered to.
   
   Stencil buffers are a depth buffer technique that can be updated as
   geometry is rendered, and used again as a mask for drawing more geometry.
   Common effects include mirrors, shadows (an advanced technique), dissolves,
   etc..


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Displaying depth complexity is a valuable tool to analyze the performance of
   a scene. Scenes with high amounts of overdraw could benefit from some scene
   optimization, such as sorting the geometry in a front-to-back order.
