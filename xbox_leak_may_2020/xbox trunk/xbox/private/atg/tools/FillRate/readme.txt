//-----------------------------------------------------------------------------
// Name: FillRate Xbox Tool
// 
// Copyright (c) 2002 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   This tool demonstrates the fill rate capabilities of the Xbox and shows how that 
performance varies with multi-sample modes, texture sizes and pixel shader complexities.  
It is user customizable so that the end user can test scenarios they are interested in 
such as complex pixel shaders combined with large textures in a given multi-sample mode.
   

Required files and media
========================
   Copy the INI file and any custom Pixel shader to the Xbox directory containing the XBE


Programming Notes
=================
The INI file must be copied over to the Xbox in the directory where the .XBE lives. The
tool will not do anything without the .INI file to describe the test its supposed to
perform.  Please see the fillrate.ini file for a complete description of each entry in the
file and how it can be changed to test different aspects of the rasterization 
pipeline.  The tool will prompt if it cannot find a .INI file.

The INI file can be changed on the fly without restarting the application.  Simply copy
a new version into the Xbox directory where the .INI is stored and the application will
immediately load the new .INI file and use it as the new test.

You can also specify a pixel shader in the .INI file to test its throughput as well as
testing the fixed function pipeline.

There are four basic tests the tool can perform. The first is the "current" test which is
defined in the .INI file.  It tests the performance throughput of the rasterization pipeline 
based on the specifications in the .INI file.  The second test is the Multisample test.  
This test shows the throughput of the "current" test in each of the multisample anti-alias
modes available on the Xbox.  The third test is the FillRate test.  This test will take the 
parameters for the "current" test and run a series of tests showing the performance results for 
different texture resolutions/bit depths/compression modes and different z-buffer 
read/write/compare states.  The last test is the Texture Test.  This test takes the parameters 
from the "current" test and checks performance against all of the possible texture formats 
supported by the Xbox using three differently sized textures.

There is also a text file included called "TexAddrOps-AllTests.txt".  This is a
list of FillRate performance results for all valid configurations of texture
address operations.  The first column is the fillrate in megapixels per second.
The next four columns represent the texture address operations in stages
0 through 3.  The last two columns represent the PSInputTexture values used by
texture stages 2 and 3, respectively.  (If this value is "X" then the input/source
does not matter for that texture stage.)

The easiest way to extract information from this file is to use grep.  For example,

  grep "2D.*LUM.*LUM.*LUM" TextAddrOps-AllTests.txt

will return the following information

  100.43   PROJECT2D       BUMPENVMAP_LUM  BUMPENVMAP_LUM  BUMPENVMAP_LUM  0 0
   89.43   PROJECT2D       BUMPENVMAP_LUM  BUMPENVMAP_LUM  BUMPENVMAP_LUM  1 1
   89.43   PROJECT2D       BUMPENVMAP_LUM  BUMPENVMAP_LUM  BUMPENVMAP_LUM  1 0
   89.43   PROJECT2D       BUMPENVMAP_LUM  BUMPENVMAP_LUM  BUMPENVMAP_LUM  0 2
   89.43   PROJECT2D       BUMPENVMAP_LUM  BUMPENVMAP_LUM  BUMPENVMAP_LUM  0 1
   74.04   PROJECT2D       BUMPENVMAP_LUM  BUMPENVMAP_LUM  BUMPENVMAP_LUM  1 2

If you cannot find a certain combination it is because that configuration is not valid.
