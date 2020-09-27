//-----------------------------------------------------------------------------
// Name: FocusBlur Xbox Sample
// 
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

Description
===========
   The FocusBlur sample approximates focus effects by blurring the
backbuffer with the GPU and then cross-dissolving based on a range of
z-values.  This approximates a camera focus, where only a range of
values near the focal plane are in sharp focus and the rest of the
scene is blurred.


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
   The FilterCopy routine is the workhorse routine that takes as input
a general filter expressed as an array of filter coefficients and
pixel offsets.  The source texture is drawn repeatedly at the
different offset positions to align the input samples with the output
samples and the pixel shader constants are set to multiply the filter
coefficients.
