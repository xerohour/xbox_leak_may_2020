//-----------------------------------------------------------------------------
// Name: Motion Blur Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   This sample illustrates one way of doing motion blur on the Xbox



Programming Notes
=================
   The technique implemented is as follows:

   1. Create a blend texture w/ z buffer and a composite texture.
   2. The blend texture gets (1/numpasses) contribution each time
      the blurred object is rendered into it.
   3. The composite texture has the blend texture added to it for
      each pass. So numpasses additions of (1/numpasses) contributions
      sums up to 1.0 max.
   4. To handle proper zbuffering into the scene, the scene z buffer
      is copied to the blend texture z buffer for each pass.
   5. After the composite is complete, it is rendered to the screen
      using alpha blending.