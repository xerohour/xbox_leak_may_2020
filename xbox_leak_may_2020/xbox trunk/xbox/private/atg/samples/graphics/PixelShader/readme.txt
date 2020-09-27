//-----------------------------------------------------------------------------
// Name: PixelShader Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The PixelShader sample renders some geometry that used pixel shaders.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The code shows how to both hard-code a pixel shader (by directly programming
   the combiners) and also creating a pixel shader from a file. The file-based
   pixel shaders start their life as *.psh text files, and get assembled into
   *.xpu files by the XSASM tool (which is part of the Xbox developer kit).
