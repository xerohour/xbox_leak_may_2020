Direct3D IM Multitexture Mipmapping Test -- mstage5.exe
--------------------------------------------------------------------

Usage:

    mstage5

Description:

    This is a test of multitexturing in four stages using mipmaps
    whose filter modes differ from stage to stage.  The test renders
    eight quads, each drawn using four texture stages.  The mipmap
    in a given stage is a mostly white surface with a strip 1/4 the
    width of the mipmap containing a brick image with yellow number
    on it indicating the level of the mipmap.  The brick strip runs
    the height of the mipmap and is placed on the far left of the
    mipmap if it is used in stage 0, 1/4 the way in to the mipmap if
    it is used in stage 1, 1/2 the way in to the mipmap if it is
    used in stage 2, and 3/4 to way in to the mipmap (on the far right)
    if it is used in stage 3.  When the colors of all mipmaps are
    modulated together they will produce a solid brick background
    for the quad with four numbers on it indicating the mipmap levels
    used in each of the stages.

    The four quads in the top row are all opaque mipmaps and the four
    quads on the bottom are translucent mipmaps with their translucent
    alpha values coming from a material and modulated with opaque
    texture values in the alpha cascade (different formats are
    used for each of the mipmaps).  The quads in the first column are
    rendered with the filtering set to none for all texture stages.
    The quads in the second column are rendered using a point
    filter in all stages.  The quads in the third column are rendered
    using a linear filter in the first and last stage and a point
    filter in the middle stages.  The quads in the last column are
    rendered with filtering set to none in the first and last
    texture stages (and the fourth texture in the cascade switched
    from a mipmap to a regular texture) and with filtering set to
    point in the middle stages.

User Input:

    Keyboard        Gamepad         Action
    ----------------------------------------------------------------
    <Esc>           X               Quit the program
    Delete          Left keypad     Pan the camera left
    Page down       Right keypad    Pan the camera right
    Home            Up keypad       Pan the camera up
    End             Down keypad     Pan the camera down
    Left arrow      Left joystick   Rotate the camera left
    Right arrow     Right joystick  Rotate the camera right
    Up arrow        Up joystick     Rotate the camera up
    Down arrow      Down joystick   Rotate the camera down
    -               Left trigger    Move the camera back
    +               Right trigger   Move the camera forward
    F7              Y               Toggle specular highlights
    P                               Point fill mode
    W               A               Wireframe fill mode (toggle)
    S               A               Solid fill mode (toggle)
    F2, F3          B               Toggle the shade mode

