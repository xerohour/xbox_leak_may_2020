//-----------------------------------------------------------------------------
// Name: ShadowVolume Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The ShadowVolume sample uses stencil buffers to implement real-time shadows.
   In the sample, a complex object is rendered and used as a shadow-caster, to
   cast real-time shadows on itself and on the terrain below.

   Stencil buffers are a depth buffer technique that can be updated as
   geometry is rendered, and used again as a mask for drawing more geometry.
   Common effects include mirrors, shadows (an advanced technique), dissolves,
   etc..


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Real-time shadows is a fairly advanced technique. Each frame, or as the
   geometry or lights in the scene are moves, an object called a shadow volume
   is computed. A shadow volume is an actual 3D object which is the siholuette
   of the shadowcasting object, as pretruded away from the light source.

   In this sample, the 3D object which casts shadows is a bi-plane. Each frame,
   the sihlouette of the plane is computed (using an edge detection algorithm,
   in which sihlouette edges are found because the normals of adjacent polygons
   will have opposing normals with respect to the light vector). The resulting
   edge list (the sihlouette) is pretuded into a 3D object away from the light
   source. This 3D object is known as the shadow volume, as everypoint inside
   the volume is inside a shadow.

   Next, the shadow volume is rendering into the stencil buffer twice. First,
   only forward-facing polygons are rendering, and the stencil buffer values 
   are incremented each time. Then the back-facing polygons of the shadow
   volume are drawm, decrementing values in the stencil buffer. Normally, all
   incremented and decremented values would cancel each other out. However,
   because the scene was already rendered with normal geometry (the plane and
   the terrain, in this case), some pixels will fail the zbuffer test as the 
   shadowvolume is rendered. Any values left in the stencil buffer correspond
   to pixels that are in the shadow.

   Finally, these remaining stencil buffer contents are used as a mask, as a
   large all-encompassing black quad is alpha-blended into the scene. With the
   stencil buffer as a mask, only pixels in shadow are darkened
