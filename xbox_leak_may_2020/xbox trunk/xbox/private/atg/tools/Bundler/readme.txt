//-----------------------------------------------------------------------------
// Name: Bundler Tool
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The bundler tool demonstrates how to create binary Direct3D resource
   files that are optimized for loading on the Xbox.  The Register API on
   the Xbox allows you to simply stream Direct3D resource data into 
   contiguous memory, create the resource header struct, and call Register()
   to fixup the data address to physical memory.
   
   The bundler has three output files.  The packed resource file contains
   all of the resources in binary format.  The header file contains a 
   number of #defines that specify offsets into the file for each 
   resource.  The error file contains errors that occur during 
   processing.
   

Command line options
====================
   The following command line options are supported:
       -o <packed resource filename>
       -h <header filename>
       -p <prefix for header file constants>
       -e <error filename>
       
    The name of the Resource Definition File (.rdf) is required.  The 
    Bundler supports a special mode where a single texture file is 
    converted to the .xpr format without an .rdf file being required.
    This mode is set when the filename specified on the command line 
    has a .bmp, .tga, or .png extension.  This is useful for converting
    single images to a format that can be loaded by the Xbox dashboard.


Resource Definition File Syntax
===============================   
   The Bundler takes as its input an RDF (Resource Definition File), describing
   the resources that should be packed into the XPR (Xbox Packed Resource) file.
   The following can be specified in the RDF file:
   * OUT_PACKEDRESOURCE <filename>  (over-rides -o option)
   * OUT_HEADER <filename>          (over-rides -h option)
   * OUT_PREFIX <filename>          (over-rides -p option)
   * OUT_ERROR <filename>           (over-rides -e option)
   * Texture <texture name>
   * VolumeTexture <texture name>
   * Cubemap <texture name>
   * Vertexbuffer <vb name>

   A Texture must be followed by a series of properties enclosed in curly
   braces {}.  Supported properties are:
   * Source <filename>
   * Format <Direct3D Format, defaults to source format>
   * Filter <Texture filter, defaults to TRIANGLE filter with WRAP adressing>
   * Width <width of image, defaults to source width>
   * Height <height of image, defaults to source height>
   * Levels <# of mipmap levels, defaults to 1, 0 means as many as possible>
   * Alphasource <filename>

   For example:
   Texture MyTex
   {
      Source c:\images\landscape.bmp
      Format D3DFMT_DXT1
      Filter TRIANGLE|CLAMP
      Width 256
      Height 256
      // Levels will default to 1
   }



   A VolumeTexture is similiar to a texture, except that it also takes a depth property
   and a number of sources equal to the depth. Linear formats for volume textures are not
   supported on Xbox hardware, nor are volume textures with any dimension greater than
   512. Supported properties are:
   * Source <filename>
   * Alphasource <filename>
   * Format <Direct3D Format, defaults to source format>
   * Filter <Texture filter, defaults to TRIANGLE filter with WRAP adressing>
   * Width <width of image, defaults to source width>
   * Height <height of image, defaults to source height>
   * Depth <height of image, defaults to source height>
   * Levels <# of mipmap levels, defaults to 1, 0 means as many as possible>

   For example:
   VolumeTexture MyTex
   {
      Depth 4
      Source c:\images\Slice0.bmp
      Source c:\images\Slice1.bmp
      Source c:\images\Slice2.bmp
      Source c:\images\Slice3.bmp
      Format D3DFMT_DXT1
   }



   A CubeMap is similiar to a texture, except that it has 6-sides, each which takes a
   source and optional alpha source. Cubemaps must have power-of-two dimensions. 
   Supported properties are:
   * SourceXP <filename>
   * SourceXN <filename>
   * SourceYP <filename>
   * SourceYN <filename>
   * SourceZP <filename>
   * SourceZN <filename>
   * AlphaSourceXP <filename>
   * AlphaSourceXN <filename>
   * AlphaSourceYP <filename>
   * AlphaSourceYN <filename>
   * AlphaSourceZP <filename>
   * AlphaSourceZN <filename>
   * Format <Direct3D Format, defaults to source format>
   * Filter <Texture filter, defaults to TRIANGLE filter with WRAP adressing>
   * Width <width of image, defaults to source width>
   * Height <height of image, defaults to source height>
   * Levels <# of mipmap levels, defaults to 1, 0 means as many as possible>

   For example:
   Cubemap MyTex
   {
      Width 64
      Height 64
      SourceXP c:\images\EnvironmentXP.bmp
      SourceXN c:\images\EnvironmentXN.bmp
      SourceXP c:\images\EnvironmentXP.bmp
      SourceXN c:\images\EnvironmentXN.bmp
      SourceYP c:\images\EnvironmentYP.bmp
      SourceYN c:\images\EnvironmentYN.bmp
      SourceZP c:\images\EnvironmentZP.bmp
      SourceZN c:\images\EnvironmentZN.bmp
      Format D3DFMT_DXT1
   }

   

   A Vertexbuffer must be followed by a series of properties enclosed in 
   curly braces {}.  Supported properties are:
   * VertexFormat { <list of attribute formats> }
   * VertexData { <list of numbers> }

   The data for each vertex in the vertex buffer is specified as a series
   of floating point, integer, or hex numbers.  A separate number should
   be provided for each component of each attribute.  For example, a normal 
   consists of three components (x,y,z) so it should be specified with 
   three separate numbers.  The formats specify how to convert the numbers
   into packed data in the vertex buffer.  For example, a VertexFormat and
   VertexData defined as follows:
       VertexFormat { D3DVSDT_FLOAT3 D3DVSDT_NORMPACKED3 }
       VertexData {1.0 2.0 3.0 0.5 0.2 0.2}
   would result in a vertex buffer that contained a single vertex defined
   by 16 bytes (3 floats 1.0 2.0 3.0 and 0.5 0.2 and 0.2 packed into a 
   single DWORD.  Note that no formatting information is stored in the .XPR 
   file.  The primitive type and vertex buffer format must be specified by 
   the title that uses the vertex buffer.
   
   Here is an example vertex buffer definition:

   VertexBuffer QuadVB
   {
       VertexFormat {D3DVSDT_FLOAT3 D3DVSDT_FLOAT3 D3DVSDT_D3DCOLOR D3DVSDT_FLOAT1 D3DVSDT_FLOAT1}
       VertexData 
       { 
           -0.5 -0.5 1.0 0.0 0.0 -1.0 1 1 1 1 0.0 1.0      // v0
           -0.5  0.5 1.0 0.0 0.0 -1.0 1 1 1 1 0.0 0.0      // v1
            0.5 -0.5 1.0 0.0 0.0 -1.0 1 1 1 1 1.0 1.0      // v2
            0.5  0.5 1.0 0.0 0.0 -1.0 1 1 1 1 1.0 0.0      // v3    
       }
   }

   

   Both // and /* */ comment constructs are supported.

Required files and media
========================
   None.


Programming Notes
=================
   The XPR format is designed for fast loading of Direct3D resources using the
   Register() API.  The bundler tool processes the resources (format conversion,
   swizzling, etc) and then writes out the texture data in the format used by
   the Xbox GPU.  The texture data can then be read into a contiguous block
   of memory, and then Direct3D textures can be pointed at the data via the
   Register() API.  See the documentation on Register() and the XPRViewer
   sample for more details.
