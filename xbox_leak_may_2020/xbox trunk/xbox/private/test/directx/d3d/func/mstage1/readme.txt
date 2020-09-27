Direct3D IM Multitexture Test -- mstage1.exe
--------------------------------------------------------------------

Usage:

    mstage1

Description:

    This is a basic test of multitexturing in two stages.  The test
    renders a quad with vertices containing two texture coordinate
    sets.  The colors of two different textures are modulated
    together on the quad.  While rendering, the first texture 
    coordinate set is offset in the v direction and the second
    set is offset in the u direction.  This has the effect of
    scrolling the first texture up and the second texture to
    the left.

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

