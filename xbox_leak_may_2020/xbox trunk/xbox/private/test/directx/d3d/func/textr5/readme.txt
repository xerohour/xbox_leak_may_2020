Direct3D IM Texture Addressing Test -- textr5.exe
--------------------------------------------------------------------

Usage:

    textr5

Description:

    This is a basic test of texture addressing for tiled textures.
    The test displays four quads on the screen, each using a different
    texture addressing mode and containing vertices whose texture
    coordinates range from 0.0 to 2.0.  The top left is set to wrap mode,
    the top right is set to mirror mode, the bottom left is set to clamp
    mode and the bottom right is set to border mode (currently unsupported
    so it should appear as wrap would).

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

