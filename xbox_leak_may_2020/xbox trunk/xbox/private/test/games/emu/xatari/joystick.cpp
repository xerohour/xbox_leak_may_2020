
#include "stdafx.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Joystick::Joystick(Jack jack, const Event& event)
    : Controller(jack, event)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Joystick::~Joystick()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Joystick::read(DigitalPin pin)
{
  switch(pin)
  {
    case One:
      return (myJack == Left) ? (myEvent.get(Event::JoystickZeroUp) == 0) : 
          (myEvent.get(Event::JoystickOneUp) == 0);

    case Two:
      return (myJack == Left) ? (myEvent.get(Event::JoystickZeroDown) == 0) : 
          (myEvent.get(Event::JoystickOneDown) == 0);

    case Three:
      return (myJack == Left) ? (myEvent.get(Event::JoystickZeroLeft) == 0) : 
          (myEvent.get(Event::JoystickOneLeft) == 0);

    case Four:
      return (myJack == Left) ? (myEvent.get(Event::JoystickZeroRight) == 0) : 
          (myEvent.get(Event::JoystickOneRight) == 0);

    case Six:
      return (myJack == Left) ? (myEvent.get(Event::JoystickZeroFire) == 0) : 
          (myEvent.get(Event::JoystickOneFire) == 0);

    default:
      return true;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Int32 Joystick::read(AnalogPin)
{
  // Analog pins are not connect in joystick so we have infinite resistance 
  return maximumResistance;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Joystick::write(DigitalPin, bool)
{
  // Writing doesn't do anything to the joystick...
}

