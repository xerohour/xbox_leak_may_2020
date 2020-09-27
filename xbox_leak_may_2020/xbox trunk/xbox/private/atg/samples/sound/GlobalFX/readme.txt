//-----------------------------------------------------------------------------
// Name: GlobalFX Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The GlobalFX sample demonstrates how to use the DSP on the Global Processor
for audio effects.  The sample loads a DSP image that contains several effects
chains, and then routes the DirectSound Buffer to the effect chains by calling
SetMixBins() on the buffer.
   Each effect may also have parameters that can be controlled via calling
SetEffectData().  The parameters for SetEffectData are specific to each effect.
   Since some effects may operate off a delay line, routing input away from a
given effect may not silence it.  Therefore, in order to silence a given
effect, you must route that effect's output to an unused GP mixbin (see the
NextFilterGraph() function for an example of how to do this).
   This sample also demonstrates how to make use of both a host-based 
oscillator (a looping DirectSound buffer filled with a sine-wave) as well as a
DSP-based oscillator (a DSP effect that generates a sine-wave via lookup table).
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The DSP Image is now built with the rest of the sample by using a custom
build rule to execute the XGPImage tool.  Then, the generated DSP image file,
image.bin, is added to the XBE using the /INSERTFILE switch.  The DSP image
is included as a read-only, non-preloaded section.  To download the image, 
the sample loads the section, calls XAudioDownloadEffectsImage, and then frees
the section.
