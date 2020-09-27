//----------------------------------------------------------------------------
// Name: Strip Xbox Sample
//
// Copyright (c) 1999-2001 Microsoft Corporation. All rights reserved.
//----------------------------------------------------------------------------

Description
===========
    The Strip sample demonstrates usage of Xbox's tri-stripper. The sample
    renders a robot with 77 meshes 12 times, gathers statistics, and displays
    them. Press the X button to change options and parameters.


Project Files
=============
    strip.cpp           main code
    modeldata.cpp       robot data file
    tristrip.cpp        tri-stripper code

Dependencies
============
    xbapp.cpp           Xbox framework
    xbfile.cpp          Xbox framework
    xbfont.cpp          Xbox framework
    xbhelp.cpp          Xbox framework
    xbinput.cpp         Xbox framework
    xbutil.cpp          Xbox framework
    xmenu.cpp           Xbox framework

    strip_t.dds         texture file (media\textures)
    strip_c.dds         cube texture file (media\textures)
    gamepad.tga         use to render help display

    strip.abc           font file (media\fonts)
    strip.tga           font file (media\fonts)


User's Guide
============
    Left Thumbstick     Rotates model
    Right Thumbstick    Zoom in/out
    Button X            Invoke/Select menu
    Button Y            Cancel/Dismiss menu
    Back Black          Displays controller button help    
