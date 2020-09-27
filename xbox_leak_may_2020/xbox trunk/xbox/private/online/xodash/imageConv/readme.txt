//-----------------------------------------------------------------------------
// Name: Xbox Online Dashboard Bundler Tool
// 
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The Xbox Online Dasboard image converter tool is a command line utility 
   designed for converting a directory of .bmp and .tga images into their 
   appropriate bundled format images.  The image converter tool will analyze
   the image and provide the appropriate warnings if the image dimensions are
   not already a power of two.  By default, the tool will scale the image up
   to the next closest power of two and continue with the conversion, but it
   is recommended that the user goes back and manually sets the image size
   for optimal results.
   
   The image converter tool selects between two texture compression formats
   based on the data stored in the source image.  If the source image depth
   is between 8 and 24 bits, the image is compressed with DXT1 compression
   (single bit alpha mask).  If the source image is 32 bits, with an 8 bit
   alpha channel, the image converter tool will compress the image with the
   DXT2 compression format.

   The image converter tool does not require the creation of a resouce
   definition file, and will ignore it if one exists in the source image
   directory.
   

Command line options
====================
   The following command line options are supported:
       -i <input image directory (required)>
       -o <output bundled image directory (required)>
       -f force overwrite of destination files
       -? help
       
    Both the input and the output directories are required.  Without the
	force overwrite flag enabled, the tool will check to see if the 
	destination bundled image file already exists.  If the image exists,
	the tool will continue on to the next source image file.
