//-----------------------------------------------------------------------------
// Name: I3DL2 Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   Demonstrates how to use the Interactive 3D Audio Level 2 support on the
Xbox.  


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   Sounds are created using DirectSound and the helper class in the 
XBSound.h/XBSound.cpp files.  The helper class just provides easy access to 
the wav file to get format information and read sample data from the file.
   3D DirectSound Buffers get sent to the crosstalk mixbins.  Therefore, 
in order to get sound from those mixbins to the speakers, you must have a 
DSP image loaded that contains at least the crosstalk code.  See the
DownloadScratch() function for how to do this.

