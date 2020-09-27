//-----------------------------------------------------------------------------
// Name: MultiPass Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   Demonstrates how to use multipass audio by routing several mono DirectSound
buffers to 3D MixIn buffer that is then positioned in 3D.  One mono buffer is 
created for each of several sound effects, as well as one 3D mixin buffer.  The
mono buffers have their output set to be the 3D mixin buffer.  Each mono buffer
then has individual control over its volume and playback, but will be positioned
along with all the other buffers at the position of the 3D buffer.  

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

