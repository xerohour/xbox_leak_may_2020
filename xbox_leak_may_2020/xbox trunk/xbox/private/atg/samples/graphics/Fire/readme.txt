//-----------------------------------------------------------------------------
// Name: Fire Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The Fire sample illustrates a cool, billboarded fire effect. The scene moves
   about the fire effect. Note that since this technique involves billboarding,
   the effect really only looks good with head-on viweing. Therefore, camera
   movement is restricted from top-down viewing.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The fire technique is actually very simple, simply blending together three
   textures to achieve the effect. The first texture is simply a static picture
   of the fire. It has the gradient colors of the fire in the color channel, and
   the outline of the fire in the alpha channel. The second texture is a noise
   texture that modulates the fire texture's alpha channel to achieve the whispy
   flame look. This texture is animated (it's texture coordinates are vertically
   scrolled to give the effect of motion). Finally, a third texture is used to
   mask the noise texture, so that the end result is more opaque towards the
   bottom of the fire.

   In addition to the textures, the geometry is distorted, like a crumpled piece
   of paper, so that when the noise texture is scrolled, the whispy flames look
   turbulent, rather than simply looking as if they are scrolling upward.

   The effect is achieved with a vertex and pixel shader. The vertex shader
   simply transforms the geometry and scrolls the noise texture. The pixel shader
   simply blends the three textures togther.
