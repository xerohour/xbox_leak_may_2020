//-----------------------------------------------------------------------------
// Name: NoSortAlphaBlend Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The NoSortAlphaBlend sample shows how to use a technique called depth 
   peeling to get correct alpha blending without sorting polygons.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   A number of different variations of the this technique are possible.  The
   one shown here was chosen for performance with a reasonably small number
   (about 5000) of alpha polygons.
   

References
==========
   Paul Diefanbach. Pipeline Rendering: Interaction and Realism Through 
   Hardware-Based Multi-Pass Rendering. University of Pennsylvania, Department 
   of Computer Science, Ph.D. dissertation, 1996.

   Cass Everitt. Interactive Order-Independant Transparency. NVidia.
