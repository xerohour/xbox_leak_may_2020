//-----------------------------------------------------------------------------
// Name: XBView tool
// 
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========

   The XBView tool demonstrates how to load textures, and other
   resource data such as vertex buffers, from an Xbox Resource (XBR)
   file.  The resource data is read from the file using overlapped I/O
   and w/out buffering, to get the best performance possible.  Also,
   the application can render a loading screen/animation while the
   file is being read in the background.  See the xbrc tool
   for how to create XBR files.
   
   
Required files and media
========================

   Copy the media tree to the target machine before running this sample.


Programming Notes
=================

   The XPR format is designed for fast loading of Direct3D resources
   using the Register() API.  The xbrc tool processes the textures
   (format conversion, swizzling, etc) and then writes out the texture
   data in the format used by the Xbox GPU.  The texture data can then
   be read into a contiguous block of memory, and then Direct3D
   textures can be pointed at the data via the Register() API.  See
   the documentation on Register() for more details.

   The XBR format adds additional resource types for vertex and pixel
   shaders, vertex and index buffers, simple skeleton animation, and
   effects.
