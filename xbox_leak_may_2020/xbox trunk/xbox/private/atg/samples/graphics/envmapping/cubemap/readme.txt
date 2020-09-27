//-----------------------------------------------------------------------------
// Name: CubeMap Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The CubeMap sample demonstrates an enviroment-mapping technique called 
   cube-mapping. Environment-mapping is a technique in which the environment
   surrounding a 3D object (such as the lights, etc.) are put into a texture
   map, so that the object can have complex lighting effects without expensive
   lighting calculations.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Cube-mapping is a technique which employs a 6-sided texture. Think of the
   being inside a wall-papered room, and having the wallpaper shrink-wrapped
   around an object. Cube-mapping is superior to sphere-mapping because the
   latter is inherently view-dependant (spheremaps are constructed for one
   particular viewpoint in mind). Cubemaps also have no geometry
   distortions, so they can be generated on the fly using SetRenderTarget()
   for each of the 6 cubemap's faces.

   Cube-mapping works with Direct3D texture coordinate generation. By setting
   D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR, Direct3D will generate cubemap 
   texture coordinates from the reflection vector for a vertex, thereby making
   this technique easy for environment-mapping effects where the environment
   is reflected in the object.
   
