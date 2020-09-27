Direct3D IM Texture Stress Test -- textr1.exe
--------------------------------------------------------------------

Usage:

    textr1

Description:

    This test stresses the creation and rendering of large numbers
    of textures.  A grid of quads is rendered to the screen using
    a distinct texture for each of the quads.  Each texture is 
    composed of a solid color chosen by sampling a texel from a 
    given bitmap.  The texel color chosen for a quad is obtained by 
    taking the position of that quad in the grid and using the 
    corresponding texel position in the bitmap.

User Input:

    Keyboard        Gamepad         Action
    ----------------------------------------------------------------
    <Esc>           X               Quit the program

