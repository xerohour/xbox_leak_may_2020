//-----------------------------------------------------------------------------
// Name: xbadpcmencode Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   Demonstrates how to encode PCM data to Xbox ADPCM.  The sample is basically
just a wrapper around the IMA ADPCM codec supplied with the windows DDK.  The
primary difference is that Xbox ADPCM is fixed at 64 samples per block.
   This sample just demonstrates a simple compression program wrapped around
the Xbox ADPCM codec.  By default, it only writes out RIFF, FORMAT, and DATA
chunks, but there is an option that will copy over all other chunks as well.


Required files and media
========================
   


Programming Notes
=================
   The encode/decode routines can be used to create your own customer encoder, 
as well as ported to a different development platform.
   Since this sample only deals with WAVE_FORMAT_PCM source files, it doesn't
check for extra data after the end of the WAVEFORMATEX structure.  However,
some formats, such as WAVE_FORMAT_XBOX_ADPCM, have extra data after the end of
the WAVEFORMATEX structure, and care must be taken to preserve that data.

