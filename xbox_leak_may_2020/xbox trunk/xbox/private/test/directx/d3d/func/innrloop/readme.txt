Direct3D IM Inner Rendering Loop Test -- innrloop.exe
--------------------------------------------------------------------

Usage:

    innrloop [state]

    state    A state identifier specifying the initial state to render

Description:

    This test stresses the run-time generated, dynamic code of
    the inner rendering loop of DDHAL.  It cycles through all
    permutations of the following render states and draws spheres to
    the screen for each of them using different primitive and vertex 
    types: fill mode, front-clipping, texturing (untextured, textured, 
    multitextured, or bump mapped), wrapping (no wrapping or wrap u 
    and v), lighting (no lighting, single directional lighting, or 
    complex lighting with directional, point, spot, and parallel point 
    lights), vertex fog, and specular.

    For each frame rendered using the states above, 36 spheres are
    drawn to the screen using different primitive types, vertex types,
    and index types.  The spheres are arranged on the screen as
    follows:

                 |--------Untransformed-------|    |-Transformed-|

                                   |-Strided--|

                  NI       I        NI       I        NI       I

    Point list    O        O        O        O        O        O

    Line list     O        O        O        O        O        O

    Line strip    O        O        O        O        O        O

    Tri list      O        O        O        O        O        O

    Tri strip     O        O        O        O        O        O

    Tri fan       O        O        O        O        O        O


    NI: Non-indexed primitives rendered using DrawPrimitive
    I:  Indexed primitives rendered using DrawIndexedPrimitive

    In order to obtain a state identifier for a particular frame
    that may be used on the command line to immediately render
    that frame and its associated states without having to cycle
    through all the previous frames, press the Start button on
    the joystick when rendering the frame.  It will cause the 
    state value to be output to the debugger.

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
                    A               Toggle the pause state (when
                                    the test is not paused it will
                                    advance to a new set of loop
                                    states every frame
                    B               Advance to the next set of states
                    Y               Rewind to the previous set of states
                    Start           Output state information
