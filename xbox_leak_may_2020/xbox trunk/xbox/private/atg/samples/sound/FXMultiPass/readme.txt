//-----------------------------------------------------------------------------
// Name: FXMultiPass Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   Demonstrates how to use global multipass audio by routing several mono
DirectSound buffers to a 3d FXIn buffer that is then positioned in 3D.  The 
difference between this and the MultiPass sample is that the MultiPass sample
demonstrates VP multipass, while the FXMultiPass sample demonstrates GP 
multipass.  With VP multipass, the source buffers do not go through GP DSP 
effect processing before being routed to the MixIn buffer.  With GP multipass,
the source buffers get DSP effects processing, and then get routed into the 
FXIn buffer.

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
DownloadEffectsImage() function for how to do this.

