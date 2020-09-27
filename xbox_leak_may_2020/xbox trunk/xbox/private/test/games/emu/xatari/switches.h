#ifndef SWITCHES_HXX
#define SWITCHES_HXX

class Event;
class Properties;
class Switches;

#include "bspf.h"

/**
  This class represents the console switches of the game console.

  @author  Bradford W. Mott
  @version $Id: Switches.hxx,v 1.2 1998/07/15 20:51:29 bwmott Exp $
*/
class Switches
{
  public:
    /**
      Create a new set of switches using the specified events and
      properties

      @param event The event object to use for events
    */
    Switches(const Event& event, const Properties& properties);
 
    /**
      Destructor
    */
    virtual ~Switches();

  public:
    /**
      Get the value of the console switches

      @return The 8 bits which represent the state of the console switches
    */
    uInt8 read();

  private:
    // Reference to the event object to use
    const Event& myEvent;

    // State of the console switches
    uInt8 mySwitches;
};
#endif

