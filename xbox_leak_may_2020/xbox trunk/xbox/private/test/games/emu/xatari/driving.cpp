
#include "stdafx.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Driving::Driving(Jack jack, const Event& event)
    : Controller(jack, event)
{
  myCounter = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Driving::~Driving()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Driving::read(DigitalPin pin)
{
  // Gray codes for clockwise rotation
  static const uInt8 clockwise[] = { 0x03, 0x01, 0x00, 0x02 };

  // Gray codes for counter-clockwise rotation
  static const uInt8 counterclockwise[] = { 0x03, 0x02, 0x00, 0x01 };

  // Delay used for moving through the gray code tables
  const uInt32 delay = 20;

  switch(pin)
  {
    case One:
      ++myCounter;

      if(myJack == Left)
      {
        if(myEvent.get(Event::JoystickZeroLeft) != 0)
        {
          return (counterclockwise[(myCounter / delay) & 0x03] & 0x01) != 0;
        }
        else if(myEvent.get(Event::JoystickZeroRight) != 0)
        {
          return (clockwise[(myCounter / delay) & 0x03] & 0x01) != 0;
        }
      }
      else
      {
        if(myEvent.get(Event::JoystickOneLeft) != 0)
        {
          return (counterclockwise[(myCounter / delay) & 0x03] & 0x01) != 0;
        }
        else if(myEvent.get(Event::JoystickOneRight) != 0)
        {
          return (clockwise[(myCounter / delay) & 0x03] & 0x01) != 0;
        }
      }

    case Two:
      if(myJack == Left)
      {
        if(myEvent.get(Event::JoystickZeroLeft) != 0)
        {
          return (counterclockwise[(myCounter / delay) & 0x03] & 0x02) != 0;
        }
        else if(myEvent.get(Event::JoystickZeroRight) != 0)
        {
          return (clockwise[(myCounter / delay) & 0x03] & 0x02) != 0;
        }
      }
      else
      {
        if(myEvent.get(Event::JoystickOneLeft) != 0)
        {
          return (counterclockwise[(myCounter / delay) & 0x03] & 0x02) != 0;
        }
        else if(myEvent.get(Event::JoystickOneRight) != 0)
        {
          return (clockwise[(myCounter / delay) & 0x03] & 0x02) != 0;
        }
      }

    case Three:
      return true;

    case Four:
      return true;

    case Six:
      return (myJack == Left) ? (myEvent.get(Event::JoystickZeroFire) == 0) : 
          (myEvent.get(Event::JoystickOneFire) == 0);

    default:
      return true;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Int32 Driving::read(AnalogPin)
{
  // Analog pins are not connect in driving controller so we have 
  // infinite resistance 
  return maximumResistance;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Driving::write(DigitalPin, bool)
{
  // Writing doesn't do anything to the driving controller...
}

