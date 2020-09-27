
#include "stdafx.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Paddles::Paddles(Jack jack, const Event& event)
    : Controller(jack, event)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Paddles::~Paddles()
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool Paddles::read(DigitalPin pin)
{
  switch(pin)
  {
    case Three:
      return (myJack == Left) ? (myEvent.get(Event::PaddleOneFire) == 0) : 
          (myEvent.get(Event::PaddleThreeFire) == 0);

    case Four:
      return (myJack == Left) ? (myEvent.get(Event::PaddleZeroFire) == 0) : 
          (myEvent.get(Event::PaddleTwoFire) == 0);

    default:
      // Other pins are not connected (floating high)
      return true;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Int32 Paddles::read(AnalogPin pin)
{
  switch(pin)
  {
    case Five:
      return (myJack == Left) ? myEvent.get(Event::PaddleOneResistance) : 
          myEvent.get(Event::PaddleThreeResistance);

    case Nine:
      return (myJack == Left) ? myEvent.get(Event::PaddleZeroResistance) : 
          myEvent.get(Event::PaddleTwoResistance);

    default:
      return maximumResistance;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void Paddles::write(DigitalPin, bool)
{
  // Writing doesn't do anything to the paddles...
}

