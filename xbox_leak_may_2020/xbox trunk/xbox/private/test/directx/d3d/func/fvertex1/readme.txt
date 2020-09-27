Direct3D IM Flexible Vertex Format Test -- fvertex1.exe
--------------------------------------------------------------------

Usage:

    fvertex1

Description:

    This application tests the rendering of geometry composed of
    vertices defined by a number of different flexible vertex 
    formats.  In each case the vertices define a teapot, but they
    vary in terms of the transformation state of their coordinates,
    their lighting information, and the number of their texture 
    coordinates.  In general, the top row of teapots on
    the screen corresponds to untransformed, unlit vertices.  The
    middle row corresponds to untransformed, pre-lit vertices.
    The bottom row is made up of transformed, pre-lit vertices.

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

