Direct3D IM Multitexture Test -- mstage2.exe
--------------------------------------------------------------------

Usage:

    mstage2

Description:

    This is a test of multitexturing using both color and alpha
    operations in four stages.  The test renders three spheres:
    the one to the left is textured with a 4444 surface containing
    alpha information and modulating the alpha in each stage;
    the center sphere is textured using surfaces of different
    formats with all alpha information set to opaque, but takes 
    translucent alpha values from a material modulated in during
    the first stage; the sphere to the right modulates colors 
    in the stages, but disables alpha operations and appears opaque.

    The vertices defining the spheres contain four texture
    coordinate pairs.  While rendering, one pair is offset in the +v
    direction, another in the -v direction, a third in the +u
    direction, and the last in the -u direction.  This has the 
    effect of scrolling each of the four textures in different
    directions.  On the spheres, each of the different colored
    arrows should be moving in a different direction from the
    rest.  But the texture of each of the arrows should be moving
    in the same direction that its arrows are facing.

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

