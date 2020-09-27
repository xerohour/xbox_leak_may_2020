//-----------------------------------------------------------------------------
// Name: EnvelopeGenerator Xbox Sample
// 
// Copyright (c) 2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------


Description
===========
   The EnvelopeGenerator sample demonstrates how to use Envelope Generators on the
Xbox.  The EGs allow you to specify:
* Delay - how long between start of the sound before sound is audible
* Attack - length of the transition from zero volume to max volume
* Hold - length of time from end of attack to start of decay
* Decay - length of time to decay from max volume to zero volume (but volume only
     goes down to the Sustain level )
* Sustain - Main volume level
* Release - length of time to go from max volume to zero volume for release (but 
     starts at the sustain level).
The envelope graph drawn on screen will help to visualize the effects of decay and
release.  Note that when a sound is looping, the envelope only affects the start
and end of playback.
   

Required files and media
========================
   Copy the media tree to the target machine before running this sample.


Programming Notes
=================

