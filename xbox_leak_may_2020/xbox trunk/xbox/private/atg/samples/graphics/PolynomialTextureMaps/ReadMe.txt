//-----------------------------------------------------------------------------
// Name: Polynomial Texture Mapping sample
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========

   This sample illustrates how to implement polynomial texture maps as 
   described in [1] using the XBox GPU.

Programming Notes
=================
    
    The coefficients of the biqaudratic polynomial are stored in two texture
    maps. The polynomial is used to compute the luminance of a pixel. The color 
    of the pixel is stored in an additional texture map and is modulated with
    the luminance in the pixel shader.

References
==========

[1] T. Malzbender, D. Gelb, and H. Wolters. Polynomial Texture Maps. In 
    "Computer Graphics Proceedings", "Annual Conference Series", 2001, pages 
    519-528, August 2001.
