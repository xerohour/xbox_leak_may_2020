Direct3D IM Texture Dimensions Test -- textr2.exe
--------------------------------------------------------------------

Usage:

    textr2

Description:

    This is a basic test of the rendering of textures of varying
    dimensions in u and v.  It covers all powers of two up to
    512x512.  A grid of quads is rendered to the screen using
    a distinct texture for each of the quads.  Each texture is 
    composed of a solid color chosen by sampling a texel from a 
    given bitmap.  The texel color chosen for a quad is obtained by 
    taking the position of that quad in the grid and using the 
    corresponding texel position in the bitmap.

User Input:

    Keyboard        Gamepad         Action
    ----------------------------------------------------------------
    <Esc>           X               Quit the program

