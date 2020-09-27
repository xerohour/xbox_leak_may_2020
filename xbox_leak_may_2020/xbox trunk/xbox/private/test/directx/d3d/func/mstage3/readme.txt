Direct3D IM Multitexture Test -- mstage3.exe
--------------------------------------------------------------------

Usage:

    mstage3

Description:

    This is a basic test of multitexturing in eight stages.  The test
    renders a quad with vertices containing four texture coordinate
    sets.  The color and alpha values of eight different textures are 
    modulated together on the quad.  While rendering, one pair of 
    texture coordinates is offset in the +v direction, another in the 
    -v direction, a third in the +u direction, and the last in the -u 
    direction.  This has the effect of scrolling two pairs of each of
    the eight textures in different directions.  On the quad, each of 
    the different colored arrows should be moving in a different 
    direction from the rest.  But the texture of each of the arrows 
    should be moving in the same direction that its arrows are facing.

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

