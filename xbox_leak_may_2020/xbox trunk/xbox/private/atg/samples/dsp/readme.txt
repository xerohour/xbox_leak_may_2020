//-----------------------------------------------------------------------------
// Name: Xbox DSP samples
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The Xbox DSP samples demonstrate how to create and build your own DSP
effects to run on the Global Processor DSP.  
   The Include directory contains 3 header files - you should include "io.h" in 
your .asm file; io.h will in turn include the other headers.
   The bin directory contains the cldtobin tool.  This tool converts an
assembled and linked .cld file into a binary .bin file, which can then be
passed to the xcodescr tool for scrambling.
   Other directories contain specific DSP sample effects - the BuildDSP.bat
batch file can be used to build the scrambled DSP effect images.  Once you've
built your scrambled image, it can be added to a DSP image through either
XGPImage or DSPBuilder.
   

Programming Notes
=================
   The motorola DSP assembler must be downloaded from the following URL:
http://www.metrowerks.com/embedded/suite56/
   The xcodescr and cldtobin tools are included in the XDK and are specific to
programming the Xbox Global Processor.
