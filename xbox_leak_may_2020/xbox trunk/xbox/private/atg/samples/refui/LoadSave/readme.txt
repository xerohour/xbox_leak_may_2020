//-----------------------------------------------------------------------------
// Name: Save/Load Save Games Reference UI Sample
// 
// Copyright (c) 2001 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
    Shows an example implementation of saving games and loading saved games.
    This implementation meets the certification requires for memory units,
    save game integrity, XMU surprise removal, XMU nicknames, etc.

    The interface for the sample is based on Xbox usability studies.
    Players found this interface easy to understand and navigate. It includes
    visual representations of the Xbox and Xbox MUs, important information
    about storage devices like names and free space, and save game information
    including the game graphic, date and time. Note that the list of saves
    is sorted from most recent to least recent.

    This sample covers many technical issues that you must consider when
    implementing Load/Save in your game:

        Validating the save game file(s) with checksums
        Enumerating MUs on the fly
        Ensuring a device has enough room for a saved game
        Allowing deletion of existing saved games
        Removal of MU and/or controllers at any time (including while 
            reading/writing data)
        Ability to scroll lists of saved games
        Optimal file chunk buffers
        Progress bars when reading/writing large saves to/from MUs
        Displaying essential device information and game information

    The interface graphics for this sample are used to illustrate the 
    techniques, and should not be directly used by your game. Use graphics
    appropriate to the look and feel of your game.

    This sample is intended to show appropriate functionality only. Please do 
    not lift the graphics for use in your game. A description of the user 
    research that went into the creation of this sample is located in the 
    XDK documentation at Developing for Xbox - Reference User Interface.
    For functionality ideas see the Technical Certification game

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================
    Lists used by the sample:
    
        The MU list includes every possible MU that could be inserted
        (8 total). Inserted MUs are mounted (IsMounted() == TRUE).

        The Storage Device list includes the hard drive and all possible MUs.
        The hard drive is at position 0 in the list. Storage Devices
        (CXBStorageDevice) provide functions for enumerating saved games.

        The Saved Game List is the list of all games on the current
        Storage Device. In "Save" mode, the list includes an "empty slot"
        at the top of the list where new games can be saved.

    Indices used by the sample:

        iCurrDev = the current Storage Device
        iCurrGame = the current game in the Saved Game List
        iTopGame = the game displayed at the top of the scrollable list

