//-----------------------------------------------------------------------------
// Name: Virtual Keyboard Reference UI Sample
// 
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
    Shows an example implementation of a virtual keyboard for obtaining
    text input. Keyboard meets certification requirements for safe screen
    region and controller input. Keyboard conforms to usability recommendations
    for keyboard layout, repeat rates, no overwrite mode, wrapping the 
    cursor from one edge of the keyboard to the next and "sticky" caps key.
    This version includes the English keyboard and a Symbol keyboard.

    This sample is intended to show appropriate functionality only. Please 
    do not lift thegraphics for use in your game. A description of the user 
    research that went into the creation of this sample is located in the 
    XDK documentation at Developing for Xbox - Reference User Interface


Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
    A "key" is identified by a virtual keycode, much like VKEYs in Windows.
    When a key is a character, the keycode is always the Unicode value for
    that character. When the key is not a character (e.g. backspace, shift)
    the keycode is a value greater than 0x10000.

    The string data is represented as a std::string, which gives a simple
    interface for inserting and removing characters from within the string.

    Cursor movement between keys is complicated by the fact that some keys
    are wider than others. The Move() functions handle cursor movement
    from regular to wide keys and vice versa.
