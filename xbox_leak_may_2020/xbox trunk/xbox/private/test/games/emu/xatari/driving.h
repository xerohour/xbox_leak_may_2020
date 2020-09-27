#ifndef DRIVING_HXX
#define DRIVING_HXX

class Driving;
class System;

#include "bspf.h"
#include "Control.h"

/**
  The standard Atari 2600 Indy 500 driving controller.

  @author  Bradford W. Mott
  @version $Id: Driving.hxx,v 1.2 1998/07/15 20:51:09 bwmott Exp $
*/
class Driving : public Controller
{
  public:
    /**
      Create a new Indy 500 driving controller plugged into 
      the specified jack

      @param jack The jack the controller is plugged into
      @param event The event object to use for events
    */
    Driving(Jack jack, const Event& event);

    /**
      Destructor
    */
    virtual ~Driving();

  public:
    /**
      Read the value of the specified digital pin for this controller.

      @param pin The pin of the controller jack to read
      @return The state of the pin
    */
    virtual bool read(DigitalPin pin);

    /**
      Read the resistance at the specified analog pin for this controller.
      The returned value is the resistance measured in ohms.

      @param pin The pin of the controller jack to read
      @return The resistance at the specified pin
    */
    virtual Int32 read(AnalogPin pin);

    /**
      Write the given value to the specified digital pin for this
      controller.  Writing is only allowed to the pins associated
      with the PIA.  Therefore you cannot write to pin six.

      @param pin The pin of the controller jack to write to
      @param value The value to write to the pin
    */
    virtual void write(DigitalPin pin, bool value);

  private:
    // Counter to iterate through the gray codes
    uInt32 myCounter;
};
#endif

