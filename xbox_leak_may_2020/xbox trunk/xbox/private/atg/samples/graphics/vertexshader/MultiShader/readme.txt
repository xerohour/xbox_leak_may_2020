//-----------------------------------------------------------------------------
// Name: MultiShader Xbox Sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The MutltiShader sample shows how to keep several small vertex shaders
   resident at one time.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Instead of using the SetVertexShader() API, an app can load several small
   shaders with the LoadVertexShader() API, and then select one at any time
   with the SelectVertexShader() API.

   The biggest gotcha here is that the fixed pipeline, when used with
   transformed vertices, will overwrite the first several vertex shader
   instruction slots. Therefore, it is often advisable to skip the first
   instruction slots entirely. See the code for details.
