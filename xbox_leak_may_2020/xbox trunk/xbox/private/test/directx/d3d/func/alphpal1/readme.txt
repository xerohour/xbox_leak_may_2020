Direct3D IM Translucent Palettized Texture Stress Test -- alphpal2.exe
--------------------------------------------------------------------

Usage:

    alphpal2

Description:

    This test stresses the creation and rendering of translucent 
    palettized textures.  It begins by creating six 8 bpp textures
    and ninety-six 4 bpp textures, each of which has a 50% chance
    of being created as a mipmap.  These textures are then rendered
    in various permutations every frame.  The way the textures are
    drawn can be broken into three categories, randomly chosen from
    at the beginning of a frame: homogenous palettes, heterogeneous 
    palettes, and recurring palettes.  When homogeneous palettes are
    selected, only 4 bpp or 8 bpp textures will be rendered in the
    frame.  Either four 8bpp textures are chosen from the pool of six,
    or sixty-four textures are chosen from the pool of ninety-six
    (for a total of 1024 unique palette entries used in the frame, the
    maximum supported by the hardware).  When heterogeneous palettes
    are selected, both 4 bpp and 8 bpp textures are rendered in the 
    frame.  The depth of the textures used alternates in a somewhat
    random order, but still does not exceed the palette entry limit
    in the hardware.  When recurring palettes are selected, both
    4 bpp and 8 bpp textures are rendered, but unlike hetergeneous
    palettes, individual textures are rendered multiple times in the 
    frame.  Whichever category appears in a frame, the textures that
    are drawn have two identifying numbers to verify they have been
    correctly rendered.  The larger number in the lower half of each
    texture is simply a 4 or an 8, corresponding to the bit depth of
    the texture.  The smaller number at the top of the texture is
    a number identifying its location within the texture pool.  When
    looking at the textures from left to right and top to bottom, these
    numbers should increase by one sequentially from a base number
    within a string of textures of the same depth.

    In addition to the variety of rendering combinations, each texture 
    is assigned a decay rate (measured in frames).  When a texture 
    decays, it is released and a new texture or mipmap is created in 
    its stead.

    When rendered on the screen, each texture is drawn on a quad.
    The quads are arranged in a grid of rows and columns.  They should
    appear translucent on the screen in front of a background texture
    that has been rendered opaque.  The alpha information for the
    translucency is stored in the palette entries of the textures.

User Input:

    Keyboard        Gamepad         Action
    ----------------------------------------------------------------
    <Esc>           X               Quit the program
    F7              Y               Toggle specular highlights
    P                               Point fill mode
    W               A               Wireframe fill mode (toggle)
    S               A               Solid fill mode (toggle)
    F2, F3          B               Toggle the shade mode

