#ifndef SOUND_HXX
#define SOUND_HXX

#include "bspf.h"

/**
  Base class that defines the standard API for sound classes.  You
  should derive a new class from this one to create a new sound system 
  for a specific operating system.

  @author  Bradford W. Mott
  @version $Id: Sound.hxx,v 1.1 1998/07/15 20:53:28 bwmott Exp $
*/
class Sound
{
  public:
    /**
      Enumeration of the TIA sound registers
    */
    enum Register 
    { 
      AUDF0=0x17, AUDF1=0x18, AUDC0=0x15, AUDC1=0x16, AUDV0=0x19, AUDV1=0x1A 
    };

  public:
    /**
      Create a new sound object
    */
    Sound();
 
    /**
      Destructor
    */
    virtual ~Sound();

  public: 
    /**
      Set the value of the specified sound register

      @param reg The sound register to set
      @param val The new value for the sound register
    */
    virtual void set(Sound::Register reg, uInt8 val);

    /**
      Set the mute state of the sound object

      @param state Mutes sound iff true
    */
    virtual void mute(bool state);
};
#endif

